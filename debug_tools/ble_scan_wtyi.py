#!/usr/bin/env python3
import asyncio
import sys
import time

sys.path.insert(0, r"D:\23178\JL\python_pkgs")

from bleak import BleakScanner


KEYWORDS = ("WTYI", "JL", "AC63", "BT_TEST")


async def main():
    seen = {}

    def cb(device, adv):
        name = adv.local_name or device.name or ""
        if not name:
            return
        seen[device.address] = (name, adv.rssi, time.strftime("%H:%M:%S"))
        if any(k in name.upper() for k in KEYWORDS):
            print(f"[MATCH] {time.strftime('%H:%M:%S')} RSSI={adv.rssi} {device.address} {name}", flush=True)

    scanner = BleakScanner(cb)
    print("BLE scan start, 30 seconds. Looking for WTYI/JL/AC63/BT_TEST ...")
    await scanner.start()
    await asyncio.sleep(30)
    await scanner.stop()

    print("\nAll named BLE devices:")
    for address, (name, rssi, ts) in sorted(seen.items(), key=lambda item: item[1][0]):
        print(f"{ts} RSSI={rssi:>4} {address} {name}")

    matches = [
        (address, name, rssi, ts)
        for address, (name, rssi, ts) in seen.items()
        if any(k in name.upper() for k in KEYWORDS)
    ]
    print(f"\nMatch count: {len(matches)}")
    return 0 if matches else 1


if __name__ == "__main__":
    raise SystemExit(asyncio.run(main()))
