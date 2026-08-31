# WT9011DCL-BT50 Offline Development Report

Date: 2026-07-23

## Scope

Hardware was kept offline. No flash script, `download.bat`, `isd_download.exe`, or BD19 UBOOT operation was run in this stage.

## Implemented

- Created independent board target `CONFIG_BOARD_WT9011DCL_BT50`.
- Added board files based on the AC6321A reference:
  - `apps/spp_and_le/board/bd19/board_wt9011dcl_bt50.c`
  - `apps/spp_and_le/board/bd19/board_wt9011dcl_bt50_cfg.h`
  - `apps/spp_and_le/board/bd19/board_wt9011dcl_bt50_global_build_cfg.h`
- Added `apps/spp_and_le/wtyi` board-test framework:
  - `wtyi_board_test_init()`
  - `wtyi_board_test_run_once()`
  - `wtyi_board_test_poll()`
- Added centralized configuration/logging:
  - `wtyi_config.h`
  - `wtyi_log.h`
- Added SPI QMI8685A transport-layer framework.
- Added IIC QMC5883P software-IIC transport-layer framework and bus scan.
- Added PA1 battery ADC sampling with 32 samples, trimming, voltage conversion, divider compensation, and calibration helper.
- Added BLE connection parameter module with `COMPATIBLE`, `BALANCED`, and `LOW_LATENCY` profiles. Default is `COMPATIBLE`.
- Added Windows serial logger:
  - `tools/serial_logger/serial_logger.py`
  - `tools/serial_logger/start_logger.bat`
  - `tools/serial_logger/build_exe.bat`
  - `tools/serial_logger/requirements.txt`
  - `tools/serial_logger/README.md`
  - `tools/wtyi/start_debug.bat`
- Added offline acceptance tools:
  - `tools/wtyi/run_all_checks.bat`
  - `tools/wtyi/parse_test_log.py`

## Board Configuration

| Function | Configured value |
| --- | --- |
| Board macro | `CONFIG_BOARD_WT9011DCL_BT50` |
| SPI CS | PB0 |
| SPI SCK | PA7 |
| SPI MOSI | PA8 |
| SPI MISO | PA6 |
| SPI INT | PA3 |
| IIC SCL | USB1DP / `IO_PORT_DP1` |
| IIC SDA | USB1DM / `IO_PORT_DM1` |
| ADC | PA1 / `AD_CH_PA1` |
| USB download | USB0 DP/DM preserved |
| UART log | `WTYI_UART_LOG_ENABLE`, default off |

## Build Result

Command:

```bat
powershell -ExecutionPolicy Bypass -File .\tools\wtyi\build_only.ps1
```

Result:

- Build status: PASS
- Compile errors: 0
- Link warnings: 8 existing stack-size-limit warnings from SDK/library functions
- Firmware: `D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin`
- Firmware size: 207248 bytes
- SHA256: `B9DD70748C5D1DEECFB22CAECE0E6DF7CA2B9EE1A09C221B674D9A80A6769A90`
- Build log: `D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\build_only_20260723_192409.log`
- Manifest: `D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\firmware_manifest_20260723_192409.txt`

Latest one-command offline check:

- Command: `tools\wtyi\run_all_checks.bat`
- Result: PASS
- Report: `D:\23178\JL\work\WT9011DCL_BT50_FW\project_docs\run_all_checks_report.md`
- Log parser dry-run report: `D:\23178\JL\work\WT9011DCL_BT50_FW\project_docs\test_log_analysis.md`

Firmware string check:

- `WT9011DCL-BT50` found.
- `[WTYI_BOOT]`, `[WTYI_SPI]`, `[WTYI_IIC]`, `[WTYI_ADC]` found.

## Current Defaults

- `WTYI_TEST_SPI_ENABLE = 0`
- `WTYI_TEST_IIC_ENABLE = 0`
- `WTYI_TEST_ADC_ENABLE = 0`
- `WTYI_TEST_POLL_ENABLE = 0`
- `WTYI_BT_LOG_ENABLE = 1`
- `WTYI_UART_LOG_ENABLE = 0`

This avoids high-rate UART output and avoids attempting sensor communication before hardware and register maps are confirmed.

## Register Documentation Still Required

- QMI8685A reliable register map or vendor driver.
- Confirmation whether any QMI8658A code is register-compatible with QMI8685A.
- QMC5883P reliable register map and expected IIC address.
- Confirmation that USB1DP/USB1DM can be safely used as GPIO software IIC on this board while USB0 remains reserved for download.
- Confirmation of PA0 test point or alternate safe log output.

## Notes

- BMI160 and IST8308 library references are treated only as reused CAD-library metadata.
- No BMI160 or IST8308 register code was used.
- No QMI8658A-specific register values were added.
