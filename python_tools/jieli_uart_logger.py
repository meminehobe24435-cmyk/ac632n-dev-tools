#!/usr/bin/env python3
"""Simple UART log receiver for Jieli AC63 boards."""

from __future__ import annotations

import argparse
import datetime as dt
from pathlib import Path

try:
    import serial
    from serial.tools import list_ports
except ImportError as exc:
    raise SystemExit(
        "Missing dependency: pyserial\n"
        "Install it with: python -m pip install pyserial"
    ) from exc


def show_ports() -> None:
    ports = list(list_ports.comports())
    if not ports:
        print("No serial ports found.")
        return
    for port in ports:
        print(f"{port.device:10} {port.description}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Receive and save Jieli UART logs.")
    parser.add_argument("port", nargs="?", help="Serial port, for example COM8")
    parser.add_argument("-b", "--baud", type=int, default=1_000_000, help="Baud rate")
    parser.add_argument("-o", "--output", default="jieli_uart.log", help="Log file path")
    parser.add_argument("--list", action="store_true", help="List serial ports and exit")
    parser.add_argument("--hex", action="store_true", help="Print bytes as hex")
    args = parser.parse_args()

    if args.list:
        show_ports()
        return 0

    if not args.port:
        parser.error("port is required unless --list is used")

    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    print(f"Opening {args.port} at {args.baud} baud. Writing to {output.resolve()}")
    print("Press Ctrl+C to stop.")

    with serial.Serial(args.port, args.baud, timeout=0.2) as ser, output.open(
        "a", encoding="utf-8", errors="replace"
    ) as log:
        log.write(f"\n===== {dt.datetime.now().isoformat(timespec='seconds')} =====\n")
        while True:
            data = ser.read(4096)
            if not data:
                continue

            if args.hex:
                text = " ".join(f"{byte:02X}" for byte in data) + "\n"
            else:
                text = data.decode("utf-8", errors="replace")

            print(text, end="", flush=True)
            log.write(text)
            log.flush()


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        print("\nStopped.")
