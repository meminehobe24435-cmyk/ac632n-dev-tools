# WT9011DCL-BT50 Conflict Report

Date: 2026-07-23

## Summary

The clean SDK builds the bd19 SPP + BLE project but defaults to `CONFIG_BOARD_AC632N_DEMO`. The product hardware is AC6321A4 with a custom sensor pinout. Long-term development should create `CONFIG_BOARD_WT9011DCL_BT50` instead of modifying `AC6321A_DEMO` in place.

## Pin Conflicts

| Conflict | SDK/demo setting | Product hardware | Impact | Resolution |
| --- | --- | --- | --- | --- |
| Active board macro | Clean HEAD selects `CONFIG_BOARD_AC632N_DEMO` | Product MCU is AC6321A4 | Wrong board file can carry wrong USB, power, pin, and peripheral defaults. | Create custom board config based on AC6321A reference. |
| SPI pins | Demo SPI1 A group uses PB0/PB1/PB2 as CLK/DO/DI in comments | Product SPI uses PB0 CS, PA7 SCK, PA8 MOSI, PA6 MISO | Hardware SPI fixed group may not match product wiring. | Verify if bd19 SPI can map to product pins; otherwise use bit-banged SPI or a supported alternate controller. |
| IIC pins | Demo software IIC uses PA9/PA10; historical WTYI patch changed to PA7/PA8 | Product IIC is USB1DP/USB1DM | PA7/PA8 are product SPI pins; PA9/PA10 are not product IIC. | Use `IO_PORT_DP1`/`IO_PORT_DM1`; disable USB1 conflicts and preserve USB0. |
| USB0 download | Some SDK configs can repurpose USB DP/DM for IIC or UART | Type-C download uses USB0 D+/D- | Repurposing USB0 can break download. | Do not use `IO_PORT_DP`/`IO_PORT_DM` for product IIC/log. |
| ADC | Demo ADKEY uses PB1 / `AD_CH_PB1` | Product battery ADC is PA1 / `AD_CH_PA1` | Reading PB1 will not validate battery divider. | Disable unrelated ADKEY ADC path for board test; add PA1 sampling. |
| UART log | Demo uses PA0 TX | PA0 test point not confirmed from photos | USB-TTL logging may be impossible without soldering. | Do not assume PA0 is accessible; document minimum solder point if needed later. |
| QMI part identity | BOM U2 Comment is `QMI8685A`; Footprint/LibRef show `bmi160/BMI160` | Photo marking is not fully readable | Wrong register map can make SPI test invalid. | Treat BMI160 as reused CAD library only. Verify QMI8685A or compatible register map before coding; do not continue writing QMI8658A code unless proven compatible. |
| QMC part identity | BOM U6 Comment is `QMC5883P`; LibRef shows `IST8308` | Schematic says QMC5883 magnetometer family | Wrong register map can make IIC test invalid. | Treat IST8308 as reused CAD library only. Use QMC5883P documentation or reliable driver. |

## Source Conflicts From Existing Historical Changes

The original SDK directory is dirty. The worktree created for this project is clean at HEAD, and the original changes were saved before worktree creation.

| Historical change | Risk |
| --- | --- |
| Hardware test code placed directly in `app_main.c` | Violates the project rule to keep board tests in independent modules. |
| IIC scan on PA7/PA8 | Conflicts with product SPI SCK/MOSI. |
| SPI loopback test PB0/PB1/PB2 | Does not validate board-mounted QMI8685A. |
| ADC PB1 test | Does not validate PA1 PWR_ADC. |
| Forced name `WTYI_BT_TEST` | Does not match target `WT9011DCL-BT50`. |

## Build Safety Conflict

`apps/spp_and_le/board/bd19/Makefile` target `all` runs a post-build script:

1. preprocesses `cpu/bd19/tools/download.c` into `cpu/bd19/tools/download.bat`;
2. links `cpu/bd19/tools/sdk.elf`;
3. calls `cpu/bd19/tools/download.bat`;
4. `download/data_trans/download.bat` calls `isd_download.exe`.

This means plain `make ac632n_spp_and_le` is not safe as a build-only command. Stage 2 must create `tools/wtyi/build_only.bat` that links and packages firmware without running `download.bat` or `isd_download.exe`.

## Open Checks Before Hardware Test

- Confirm whether product SPI can be handled by a flexible hardware SPI mapping on bd19 or must be bit-banged.
- Confirm exact QMI8685A register map and whether QMI8658A register code is compatible.
- Confirm QMC5883P register map and expected IIC address.
- Confirm PA0 or another safe log output path.
- Confirm whether USB1 must be fully disabled before using `IO_PORT_DP1`/`IO_PORT_DM1` as IIC GPIO.

## Offline Development Status

- Active board macro is now `CONFIG_BOARD_WT9011DCL_BT50`.
- Product IIC config is now `IO_PORT_DP1`/`IO_PORT_DM1`, not PA7/PA8.
- Product ADC config is now PA1 / `AD_CH_PA1`; PB1 ADKEY is disabled for this board.
- UART0 logging is controlled by `WTYI_UART_LOG_ENABLE` and defaults to off until PA0 access is confirmed.
- SPI and IIC transport layers compile, but QMI8685A/QMC5883P register initialization remains blocked pending reliable register documentation.

## Added Node A BOM Constraints

- U1 Comment: `ac6321a4`.
- U2 Comment: `QMI8685A`.
- U2 Footprint/LibRef: `bmi160/BMI160`; this must not be used as a register-level identity.
- U6 Comment: `QMC5883P`.
- U6 LibRef: `IST8308`; this must not be used as a register-level identity.
- U7 Comment: `TYPE-C-31-M-12`.
- Board key Comment: `TSA053G20-250T`.
- PA0 test point remains unconfirmed.
- SPI test-driver stage must pause again before implementing sensor register initialization.
