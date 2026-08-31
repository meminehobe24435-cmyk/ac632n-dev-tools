#!/usr/bin/env python3
"""
JieLi AC63 UART log receiver.

Default AC632N demo UART log settings:
  Board TX: PA00
  Baudrate: 1000000
  Wiring: board PA00/TX -> USB-TTL RX, board GND -> USB-TTL GND
"""

from __future__ import annotations

import argparse
import datetime as _dt
import pathlib
import sys
import time

import serial
from serial.tools import list_ports


DEFAULT_BAUD = 1_000_000
DEFAULT_PORT = "COM8"
DEFAULT_LOG_DIR = pathlib.Path(r"D:\23178\JL\logs")


def list_serial_ports() -> int:
    ports = list(list_ports.comports())
    if not ports:
        print("No serial ports found.")
        return 1

    print("Available serial ports:")
    for port in ports:
        desc = port.description or ""
        hwid = port.hwid or ""
        print(f"  {port.device:<8} {desc}  {hwid}")
    return 0


def open_log_file(log_dir: pathlib.Path) -> pathlib.Path:
    log_dir.mkdir(parents=True, exist_ok=True)
    stamp = _dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    return log_dir / f"ac63_uart_{stamp}.log"


def receive(port: str, baud: int, log_dir: pathlib.Path, raw: bool) -> int:
    log_path = open_log_file(log_dir)
    print(f"Opening {port} @ {baud} baud")
    print(f"Saving log to {log_path}")
    print("Wiring: board PA00/TX -> USB-TTL RXD, board GND -> USB-TTL GND")
    print("Press Ctrl+C to stop.\n")

    try:
        with serial.Serial(port, baudrate=baud, timeout=0.2) as ser, log_path.open(
            "ab"
        ) as log_file:
            while True:
                data = ser.read(4096)
                if not data:
                    continue

                log_file.write(data)
                log_file.flush()

                if raw:
                    sys.stdout.buffer.write(data)
                    sys.stdout.buffer.flush()
                else:
                    text = data.decode("utf-8", errors="replace")
                    print(text, end="", flush=True)
    except KeyboardInterrupt:
        print("\nStopped.")
        return 0
    except serial.SerialException as exc:
        print(f"Serial error: {exc}")
        return 2


def main() -> int:
    parser = argparse.ArgumentParser(description="Receive JieLi AC63 UART logs.")
    parser.add_argument("-l", "--list", action="store_true", help="list COM ports")
    parser.add_argument("-p", "--port", default=DEFAULT_PORT, help="serial port, example: COM8")
    parser.add_argument("-b", "--baud", type=int, default=DEFAULT_BAUD)
    parser.add_argument("--log-dir", type=pathlib.Path, default=DEFAULT_LOG_DIR)
    parser.add_argument("--raw", action="store_true", help="print raw bytes")
    args = parser.parse_args()

    if args.list:
        return list_serial_ports()

    return receive(args.port, args.baud, args.log_dir, args.raw)


if __name__ == "__main__":
    raise SystemExit(main())
