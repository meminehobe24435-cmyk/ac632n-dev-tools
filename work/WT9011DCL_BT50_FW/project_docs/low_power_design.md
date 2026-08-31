# WT9011DCL-BT50 low power basic framework

## Goal

This stage only adds a simple, readable and reversible low-power gate for WT9011DCL-BT50.

No poweroff is enabled. Wakeup pins are not changed. BLE connection parameters are not changed. No real current value is claimed before hardware measurement.

## SDK call chain in plain words

1. Board code enables the SDK low-power capability through `TCFG_LOWPOWER_LOWPOWER_SEL`.
2. `board_power_init()` calls `power_init(&power_param)` and `power_set_callback(...)`.
3. The SDK scans modules registered by `REGISTER_LP_TARGET(...)`.
4. Each registered module provides `is_idle()`.
5. If every module returns idle, the SDK may enter sleep or powerdown by itself.
6. If any module returns busy, the SDK stays awake.
7. When the chip enters or leaves sleep, board callbacks `sleep_enter_callback()` and `sleep_exit_callback()` are called.

The WTYI layer only answers one question: "Is WT9011DCL-BT50 board work busy now?" It does not force poweroff or tune aggressive parameters.

## Searched SDK examples

- `include_lib/driver/cpu/bd19/asm/power_interface.h`
  - Defines `struct lp_target`, `REGISTER_LP_TARGET`, `low_power_*` APIs and callback registration.
- `apps/spp_and_le/board/bd19/board_wt9011dcl_bt50.c`
  - Current board `power_param`, `sleep_enter_callback`, `sleep_exit_callback`, `board_power_init`.
- `apps/spp_and_le/board/bd19/board_wt9011dcl_bt50_cfg.h`
  - Current low-power macros.
- `apps/spp_and_le/include/app_config.h`
  - App-level overrides that can disable low power in special modes.
- `apps/common/update/update.c`
  - OTA has `REGISTER_LP_TARGET(ota_lp_target)` to prevent powerdown during update.
- `apps/common/update/uart_update.c`
  - UART update also registers a low-power target.
- `apps/spp_and_le/examples/trans_data/app_spp_and_le.c`
  - SPP/BLE data example registers `spple_state_lp_target`.

## Current project low-power state

Current custom board config is enabled:

- `apps/spp_and_le/board/bd19/board_wt9011dcl_bt50_cfg.h`
  - `TCFG_LOWPOWER_LOWPOWER_SEL = SLEEP_EN`
  - `TCFG_LOWPOWER_POWER_SEL = PWR_DCDC15`
  - `TCFG_LOWPOWER_BTOSC_DISABLE = 0`
  - `TCFG_LOWPOWER_OSC_TYPE = OSC_TYPE_LRC`

Special app modes in `apps/spp_and_le/include/app_config.h` can override `TCFG_LOWPOWER_LOWPOWER_SEL` to `0`, but this stage does not change those modes.

## WTYI design

New module:

- `apps/spp_and_le/wtyi/wtyi_power_manager.c`
- `apps/spp_and_le/wtyi/wtyi_power_manager.h`

Config switch:

- `WTYI_LOW_POWER_ENABLE`
- Defined in `apps/spp_and_le/wtyi/wtyi_config.h`
- `1`: WTYI allows SDK powerdown when all WTYI busy flags are clear.
- `0`: WTYI registered target always reports busy, blocking SDK low power without changing board config.

Busy flags:

- `WTYI_POWER_BUSY_BT`
- `WTYI_POWER_BUSY_SPI`
- `WTYI_POWER_BUSY_IIC`
- `WTYI_POWER_BUSY_ADC`
- `WTYI_POWER_BUSY_OTA`

Rule:

- Any bit set means WTYI is busy and returns `0` from its idle query.
- All bits clear means WTYI returns `1` and the SDK may enter powerdown if other registered targets also agree.

## OTA rule

The SDK already has `ota_lp_target`. This stage also synchronizes WTYI OTA busy:

- `update_init_common_handle()` calls `wtyi_power_ota_begin()`.
- `update_exit_common_handle()` calls `wtyi_power_ota_end()`.
- `ota_idle_query()` mirrors current `ota_status` into WTYI OTA busy.

These calls are guarded by `CONFIG_BOARD_WT9011DCL_BT50`.

## Peripheral integration

WTYI SPI, IIC and ADC test drivers set their busy flag during init and bus/sample operations:

- `wtyi_spi_imu_init/read_reg/write_reg/read_regs`
- `wtyi_iic_mag_init/scan/read_reg/write_reg/read_regs`
- `wtyi_adc_battery_init/read`

BT busy is provided as an API for later data-transfer or connection workflow integration. This stage does not treat normal Bluetooth connection state as always busy, because that would block Bluetooth sleep during connected idle time.

## Expected boot log

When logging is available:

```text
[WTYI_BOOT] low_power=enabled mask=0x0
```

If `WTYI_LOW_POWER_ENABLE` is set to `0`:

```text
[WTYI_BOOT] low_power=disabled by WTYI_LOW_POWER_ENABLE
```

OTA logs, if OTA starts after boot:

```text
[WTYI_BOOT] ota_busy=1 mask=0x10
[WTYI_BOOT] ota_busy=0 mask=0x0
```

## Compile result

Command:

```bat
D:\23178\JL\work\WT9011DCL_BT50_FW\tools\wtyi\build_only.bat
```

Result:

- Build: OK
- Burn/download: not executed
- Firmware: `D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin`
- Size: `207472` bytes
- SHA256: `74DF7FDB96B318E62114782CDD2361161C0AAE3F5AD9D5C9B02BD8208ED90221`
- Latest log: `D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\build_only_20260727_211606.log`
- Manifest: `D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\firmware_manifest_20260727_211606.txt`

Linker warnings are the existing LLVM gold plugin stack-size warnings from SDK/library functions. No compile or link error occurred.

## Rollback

Fast rollback:

1. Set `WTYI_LOW_POWER_ENABLE` to `0` in `apps/spp_and_le/wtyi/wtyi_config.h`.
2. Rebuild with `tools/wtyi/build_only.bat`.

Full code rollback:

1. Remove `apps/spp_and_le/wtyi/wtyi_power_manager.c`.
2. Remove `apps/spp_and_le/wtyi/wtyi_power_manager.h`.
3. Remove the WTYI power manager include and init call from `wtyi_board_test.c`.
4. Remove `wtyi_power_manager.c` from `apps/spp_and_le/board/bd19/Makefile`.
5. Remove the two CodeBlocks units from `apps/spp_and_le/board/bd19/AC632N_spp_and_le.cbp`.
6. Remove the guarded WTYI OTA calls from `apps/common/update/update.c`.

## Hardware validation later

After burn recovers, the first low-power validation should be conservative:

1. Boot normal firmware.
2. Confirm Bluetooth name still appears.
3. Confirm no OTA is running.
4. Observe boot log if UART is available.
5. Measure current only after confirming the firmware boots normally.

This document does not claim measured current.
