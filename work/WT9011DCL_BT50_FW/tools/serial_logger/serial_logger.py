#!/usr/bin/env python3
import argparse
import datetime as dt
import os
import sys
import time

import serial
from serial.tools import list_ports


def list_com_ports():
    return list(list_ports.comports())


def choose_port():
    ports = list_com_ports()
    if not ports:
        print("No COM ports found. Connect USB-TTL when UART logging is available.")
        return None

    for index, port in enumerate(ports, start=1):
        print(f"{index}. {port.device} - {port.description} [{port.hwid}]")

    while True:
        choice = input("Select COM port number: ").strip()
        if choice.isdigit() and 1 <= int(choice) <= len(ports):
            return ports[int(choice) - 1].device
        print("Invalid selection.")


def open_log_file(log_dir):
    os.makedirs(log_dir, exist_ok=True)
    name = dt.datetime.now().strftime("serial_%Y%m%d_%H%M%S.log")
    return open(os.path.join(log_dir, name), "a", encoding="utf-8", errors="replace")


def should_print(line, keywords):
    if not keywords:
        return True
    return any(key in line for key in keywords)


def run_logger(port, baudrate, log_dir, keywords, reconnect_delay):
    with open_log_file(log_dir) as log_file:
        print(f"Logging to {log_file.name}")
        while True:
            try:
                with serial.Serial(port, baudrate, timeout=0.5) as ser:
                    print(f"Opened {port} @ {baudrate}")
                    while True:
                        raw = ser.readline()
                        if not raw:
                            continue
                        text = raw.decode("utf-8", errors="replace").rstrip("\r\n")
                        stamp = dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                        line = f"{stamp} {text}"
                        log_file.write(line + "\n")
                        log_file.flush()
                        if should_print(line, keywords):
                            print(line)
            except KeyboardInterrupt:
                print("\nExit.")
                return 0
            except serial.SerialException as exc:
                print(f"Serial error: {exc}. Reconnecting in {reconnect_delay}s...")
                time.sleep(reconnect_delay)


def main(argv=None):
    parser = argparse.ArgumentParser(description="WTYI Windows serial log receiver")
    parser.add_argument("-p", "--port", help="COM port, for example COM5")
    parser.add_argument("-b", "--baudrate", type=int, default=1000000)
    parser.add_argument("--log-dir", default=os.path.join(os.path.dirname(__file__), "logs"))
    parser.add_argument("-k", "--keyword", action="append", default=[], help="filter keyword; can repeat")
    parser.add_argument("--reconnect-delay", type=float, default=1.5)
    args = parser.parse_args(argv)

    port = args.port or choose_port()
    if not port:
        return 2

    return run_logger(port, args.baudrate, args.log_dir, args.keyword, args.reconnect_delay)


if __name__ == "__main__":
    sys.exit(main())
