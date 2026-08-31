#!/usr/bin/env python3
import argparse
import json
import re
from pathlib import Path


CHECKS = {
    "boot": re.compile(r"\[WTYI_BOOT\]"),
    "classic_name": re.compile(r"\[WTYI_BT\].*classic_name=WT9011DCL-BT50"),
    "ble_name": re.compile(r"\[WTYI_BLE\].*adv_name=WT9011DCL-BT50"),
    "ble_connected": re.compile(r"\[WTYI_BLE\].*connected|\[WTYI_BT\].*connected"),
    "ble_disconnected": re.compile(r"\[WTYI_BLE\].*disconnected|\[WTYI_BT\].*disconnected"),
    "ble_actual_params": re.compile(r"\[WTYI_BLE\].*actual interval="),
    "ble_actual_mtu": re.compile(r"\[WTYI_BLE\].*actual mtu="),
    "adc": re.compile(r"\[WTYI_ADC\].*raw=.*input_mv=.*vbat_mv="),
    "spi_ready": re.compile(r"\[WTYI_SPI\].*transport ready"),
    "spi_unconfirmed": re.compile(r"\[WTYI_SPI\].*register map unconfirmed"),
    "iic_ready": re.compile(r"\[WTYI_IIC\].*transport ready"),
    "iic_scan": re.compile(r"\[WTYI_IIC\].*scan count="),
    "error": re.compile(r"\[WTYI_ERROR\]"),
}


def read_text(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_bytes().decode("utf-8", errors="replace")


def analyze(text: str) -> dict:
    lines = text.splitlines()
    result = {
        "line_count": len(lines),
        "checks": {},
        "errors": [],
    }
    for name, pattern in CHECKS.items():
        matches = [line for line in lines if pattern.search(line)]
        if name == "error":
            result["errors"] = matches[:20]
        else:
            result["checks"][name] = {
                "passed": bool(matches),
                "count": len(matches),
                "sample": matches[:3],
            }
    return result


def write_markdown(report: dict, source: Path, output: Path) -> None:
    lines = [
        "# WTYI Test Log Analysis",
        "",
        f"Source: `{source}`",
        f"Lines: {report['line_count']}",
        "",
        "## Checks",
        "",
        "| Item | Result | Count | Sample |",
        "| --- | --- | --- | --- |",
    ]
    for name, item in report["checks"].items():
        status = "PASS" if item["passed"] else "PENDING"
        sample = "<br>".join(s.replace("|", "/") for s in item["sample"])
        lines.append(f"| `{name}` | {status} | {item['count']} | {sample} |")
    lines.extend(["", "## Errors", ""])
    if report["errors"]:
        for err in report["errors"]:
            lines.append(f"- `{err}`")
    else:
        lines.append("No `[WTYI_ERROR]` lines found.")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description="Parse WTYI UART test log")
    parser.add_argument("log", nargs="?", default="", help="UART log file")
    parser.add_argument("-o", "--output", default="project_docs/test_log_analysis.md")
    parser.add_argument("--json", dest="json_output", default="")
    args = parser.parse_args()

    source = Path(args.log) if args.log else Path("<none>")
    text = read_text(source) if args.log else ""
    report = analyze(text)
    output = Path(args.output)
    write_markdown(report, source, output)

    if args.json_output:
        Path(args.json_output).write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")

    print(f"Analysis report: {output}")
    if not args.log:
        print("No log file provided; hardware checks are marked PENDING.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
