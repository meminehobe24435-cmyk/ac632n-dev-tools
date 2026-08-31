# WT9011DCL-BT50 Bring-up Plan

Date: 2026-07-23

## Current State

- Original SDK changes were backed up to `project_docs/original_changes.patch` and `project_docs/original_git_status.txt`.
- Clean worktree was created at `D:\23178\JL\work\WT9011DCL_BT50_FW`.
- Development branch is `feature/wt9011dcl-bt50-bringup`.
- Stage 0 only generated documentation under `project_docs`; no SDK source code was modified.

## Stage 1: Environment Check

Create:

- `tools/wtyi/check_environment.ps1`
- `tools/wtyi/setup_environment.bat`
- `tools/wtyi/environment_report.md`

Checks:

- Code::Blocks
- JieLi clang toolchain
- `q32s-lto-wrapper`
- `llvm-ar`
- SDK `make`
- Python, pip, pyserial, PyInstaller
- bd19 burn tool files

Rules:

- Use D drive tools where possible.
- Do not modify global PATH permanently.
- Create project venv at `.venv` only if dependencies are missing.

## Stage 2: Safe Baseline Build

Create:

- `tools/wtyi/build_only.bat`
- `tools/wtyi/flash_confirmed.bat`
- `project_docs/baseline_build_report.md`

Build-only requirements:

- Compile and link without running `download.bat`.
- Never run `isd_download.exe`.
- Save logs to `build_logs`.
- Report firmware path, size, timestamp, and checksum.

Flash script requirements:

- Require exact confirmation text `FLASH-WT9011`.
- Show firmware path and checksum.
- Do not run automatically from build.

## Stage 3: Custom Board Configuration

Create independent board config:

- `CONFIG_BOARD_WT9011DCL_BT50`
- `board_wt9011dcl_bt50.c`
- `board_wt9011dcl_bt50_cfg.h`
- `board_wt9011dcl_bt50_global_build_cfg.h`

Board rules:

- Base on AC6321A reference, not AC632N default.
- SPI product pins: CS PB0, SCK PA7, MOSI PA8, MISO PA6, INT PA3.
- IIC product pins: USB1DP/USB1DM.
- ADC product pin: PA1 / `AD_CH_PA1`.
- Preserve USB0 D+/D- for Type-C download.
- Do not assume PA0 UART is externally accessible.
- Treat U2 as QMI8685A for investigation. Do not use BMI160 register code even though BOM Footprint/LibRef reuse BMI160.
- Treat U6 as QMC5883P for investigation. Do not use IST8308 register code even though BOM LibRef reuses IST8308.

## Stage 4: Bluetooth Name And Logs

Target:

- Classic BT name: `WT9011DCL-BT50`
- BLE advertising name: `WT9011DCL-BT50`

Log prefixes:

- `[WTYI_BOOT]`
- `[WTYI_BT]`
- `[WTYI_BLE]`
- `[WTYI_SPI]`
- `[WTYI_IIC]`
- `[WTYI_ADC]`
- `[WTYI_ERROR]`

Implementation rule:

- Track real call chain and centralize constants; do not do a blind text replacement.

## Stage 5: Python Serial Logger

Create:

- `tools/serial_logger/serial_logger.py`
- `tools/serial_logger/requirements.txt`
- `tools/serial_logger/start_logger.bat`
- `tools/serial_logger/build_exe.bat`
- `tools/serial_logger/README.md`
- `tools/wtyi/start_debug.bat`

Main features:

- COM listing and interactive selection.
- Default 1000000 baud.
- Timestamps, log file, reconnect, filtering, Ctrl+C cleanup.

## Stage 6: SPI QMI8685A Validation

Plan:

1. Pause again before writing register initialization code, because U2 exact register compatibility is not final.
2. Confirm chip register map from local datasheet, vendor documentation, or reliable driver.
3. Confirm whether QMI8658A code is compatible with QMI8685A before reuse.
4. Initialize selected SPI transport.
5. Read ID/fixed registers.
6. Initialize accelerometer and gyro only after register map is confirmed.
7. Print raw six-axis data.
8. Ask for board rotation at physical node F.
9. Detect all-zero, all-FF, timeout, and fixed-value failures.

## Stage 7: IIC QMC5883 Validation

Plan:

1. Use USB1DP/USB1DM as IIC GPIO if safe.
2. Disable USB1 conflict while preserving USB0.
3. Scan IIC addresses.
4. Read status/ID/data registers.
5. Ask for board rotation at physical node F.
6. Detect all-zero, all-FF, timeout, and fixed-value failures.

## Stage 8: PA1 Battery ADC Validation

Plan:

1. Use `AD_CH_PA1`.
2. Sample at least 32 times.
3. Drop abnormal values and average.
4. Print raw ADC and input voltage.
5. Compute VBAT as input voltage times 4.
6. Ask for multimeter VBAT at physical node G.
7. Compare error, target <= 5%.

## Stage 9: BLE Connection Parameters

Create:

- `apps/spp_and_le/wtyi/wtyi_bt_config.h`

Profiles:

- `COMPATIBLE`
- `BALANCED`
- `LOW_LATENCY`

Rules:

- Default to `COMPATIBLE`.
- Validate BLE specification constraints.
- Print requested and final negotiated values.
- Fall back to compatible profile on connection failure.

Current offline implementation:

- `COMPATIBLE`, `BALANCED`, and `LOW_LATENCY` profiles are centralized in `wtyi_bt_config.h/.c`.
- The active request table defaults to `COMPATIBLE`.
- The SPP+BLE data example prints requested parameters at boot and actual connection interval/latency/timeout/MTU after BLE events.
- PHY and data length are not printed as final values yet because the current event path has not been confirmed to provide those fields.

## Automation And Report

Create:

- `tools/wtyi/run_all_checks.bat`
- `tools/wtyi/parse_test_log.py`
- `project_docs/final_test_report.md`

Final report must include toolchain, SDK version, git commit, firmware checksum, flash result, BT/BLE name result, BLE params, SPI/IIC/ADC results, unresolved issues, and rollback method.

## Next Physical Pause

Node A result:

- BOM was supplied as screenshot and confirms U1/U2/U6/U7/switch comments.
- The project can continue through environment check and safe baseline build.
- Burning remains blocked until explicit confirmation.
- SPI test driver remains blocked until a later pause confirms U2 register compatibility.
