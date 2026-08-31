#!/usr/bin/env python3
from __future__ import annotations

import argparse
import datetime as dt
import pathlib
import queue
import threading
import tkinter as tk
from tkinter import ttk, messagebox

import serial
from serial.tools import list_ports


DEFAULT_BAUD = 1_000_000
LOG_DIR = pathlib.Path(r"D:\23178\JL\logs")


def list_com_ports():
    return list(list_ports.comports())


def pick_default_port() -> str:
    ports = list_com_ports()
    for port in ports:
        text = f"{port.device} {port.description} {port.hwid}".upper()
        if "CH340" in text or "CP210" in text or "USB-SERIAL" in text or "USB SERIAL" in text:
            return port.device
    return ports[0].device if ports else ""


class UartPrintGui(tk.Tk):
    def __init__(self, auto_start: bool = False):
        super().__init__()
        self.title("JieLi AC63 UART Print Receiver")
        self.geometry("980x640")

        self.rx_queue: queue.Queue[bytes | str] = queue.Queue()
        self.stop_event = threading.Event()
        self.reader_thread: threading.Thread | None = None
        self.serial_handle: serial.Serial | None = None
        self.log_file = None
        self.log_path: pathlib.Path | None = None

        self.port_var = tk.StringVar(value=pick_default_port())
        self.baud_var = tk.StringVar(value=str(DEFAULT_BAUD))
        self.status_var = tk.StringVar(value="Disconnected")
        self.log_path_var = tk.StringVar(value="")

        self._build_ui()
        self.refresh_ports()
        self.after(100, self._drain_queue)

        if auto_start and self.port_var.get():
            self.after(500, self.start_reader)

    def _build_ui(self):
        root = ttk.Frame(self, padding=10)
        root.pack(fill=tk.BOTH, expand=True)

        top = ttk.Frame(root)
        top.pack(fill=tk.X)

        ttk.Label(top, text="Port").pack(side=tk.LEFT)
        self.port_combo = ttk.Combobox(top, textvariable=self.port_var, width=16, state="readonly")
        self.port_combo.pack(side=tk.LEFT, padx=(6, 12))

        ttk.Label(top, text="Baud").pack(side=tk.LEFT)
        ttk.Entry(top, textvariable=self.baud_var, width=12).pack(side=tk.LEFT, padx=(6, 12))

        ttk.Button(top, text="Refresh", command=self.refresh_ports).pack(side=tk.LEFT, padx=4)
        ttk.Button(top, text="Start", command=self.start_reader).pack(side=tk.LEFT, padx=4)
        ttk.Button(top, text="Stop", command=self.stop_reader).pack(side=tk.LEFT, padx=4)
        ttk.Button(top, text="Clear", command=self.clear_output).pack(side=tk.LEFT, padx=4)

        info = ttk.Label(
            root,
            text="Wiring: Board PA00/TX -> USB-TTL RXD, Board GND -> USB-TTL GND. Do not connect USB-TTL 5V/VCC.",
        )
        info.pack(fill=tk.X, pady=(10, 4))

        status = ttk.Frame(root)
        status.pack(fill=tk.X, pady=(0, 8))
        ttk.Label(status, text="Status:").pack(side=tk.LEFT)
        ttk.Label(status, textvariable=self.status_var).pack(side=tk.LEFT, padx=(6, 20))
        ttk.Label(status, text="Log:").pack(side=tk.LEFT)
        ttk.Label(status, textvariable=self.log_path_var).pack(side=tk.LEFT, padx=6)

        text_frame = ttk.Frame(root)
        text_frame.pack(fill=tk.BOTH, expand=True)

        self.output = tk.Text(text_frame, wrap=tk.WORD, font=("Consolas", 11))
        self.output.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        scroll = ttk.Scrollbar(text_frame, command=self.output.yview)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.output.configure(yscrollcommand=scroll.set)

        self.output.tag_configure("match", foreground="#008000")
        self.output.tag_configure("error", foreground="#b00020")
        self.output.insert(tk.END, "Ready.\n")
        self.output.insert(tk.END, "Expected prints: [WTYI] AC63 SPP+LE firmware start / [WTYI] UART heartbeat ...\n\n")

        self.protocol("WM_DELETE_WINDOW", self.on_close)

    def refresh_ports(self):
        ports = list_com_ports()
        values = [f"{p.device}  {p.description}" for p in ports]
        self.port_combo["values"] = values

        current = self.port_var.get()
        if not current and ports:
            self.port_var.set(pick_default_port())
        elif current:
            for p in ports:
                if p.device == current:
                    self.port_var.set(f"{p.device}  {p.description}")
                    break

    def _port_name(self) -> str:
        return self.port_var.get().split()[0].strip()

    def start_reader(self):
        if self.reader_thread and self.reader_thread.is_alive():
            return

        port = self._port_name()
        if not port:
            messagebox.showerror("No COM port", "No serial port found.")
            return

        try:
            baud = int(self.baud_var.get())
        except ValueError:
            messagebox.showerror("Bad baud", "Baud must be a number.")
            return

        LOG_DIR.mkdir(parents=True, exist_ok=True)
        self.log_path = LOG_DIR / f"ac63_uart_gui_{dt.datetime.now():%Y%m%d_%H%M%S}.log"
        self.log_file = self.log_path.open("ab")
        self.log_path_var.set(str(self.log_path))
        self.stop_event.clear()

        self.reader_thread = threading.Thread(target=self._reader_loop, args=(port, baud), daemon=True)
        self.reader_thread.start()
        self.status_var.set(f"Opening {port} @ {baud}")

    def _reader_loop(self, port: str, baud: int):
        try:
            with serial.Serial(port, baudrate=baud, timeout=0.2) as ser:
                self.serial_handle = ser
                self.rx_queue.put(f"[GUI] Opened {port} @ {baud}\n")
                self.rx_queue.put("[GUI] Press board RST if no data appears.\n")
                self.status_var.set(f"Connected {port} @ {baud}")
                while not self.stop_event.is_set():
                    data = ser.read(4096)
                    if not data:
                        continue
                    if self.log_file:
                        self.log_file.write(data)
                        self.log_file.flush()
                    self.rx_queue.put(data)
        except Exception as exc:
            self.rx_queue.put(f"[ERROR] {exc}\n")
            self.status_var.set("Error")
        finally:
            self.serial_handle = None
            if self.log_file:
                self.log_file.close()
                self.log_file = None
            if self.status_var.get() != "Error":
                self.status_var.set("Disconnected")

    def _drain_queue(self):
        while True:
            try:
                item = self.rx_queue.get_nowait()
            except queue.Empty:
                break

            if isinstance(item, bytes):
                text = item.decode("utf-8", errors="replace")
            else:
                text = item

            tag = "match" if "WTYI" in text else ("error" if "[ERROR]" in text else None)
            self.output.insert(tk.END, text, tag)
            self.output.see(tk.END)

        self.after(100, self._drain_queue)

    def stop_reader(self):
        self.stop_event.set()
        if self.serial_handle:
            try:
                self.serial_handle.close()
            except Exception:
                pass
        self.status_var.set("Stopping")

    def clear_output(self):
        self.output.delete("1.0", tk.END)

    def on_close(self):
        self.stop_reader()
        self.destroy()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--auto", action="store_true", help="start receiving automatically")
    parser.add_argument("--no-auto", action="store_true", help="open GUI without starting receiver")
    args = parser.parse_args()

    app = UartPrintGui(auto_start=(args.auto or not args.no_auto))
    app.mainloop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
