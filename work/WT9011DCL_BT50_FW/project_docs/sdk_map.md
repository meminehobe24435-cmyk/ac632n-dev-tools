# WT9011DCL-BT50 SDK Map

Date: 2026-07-23

## SDK Version And Repository

| Item | Value |
| --- | --- |
| Worktree | `D:\23178\JL\work\WT9011DCL_BT50_FW` |
| Original SDK | `D:\23178\JL\sdk\fw-AC63_BT_SDK` |
| Remote | `https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK.git` |
| Local HEAD | `b355973c962e06d2552c5a215fa8bb0279e86bb4` |
| Remote HEAD checked by `git ls-remote` | `b355973c962e06d2552c5a215fa8bb0279e86bb4` |
| Makefile SDK config version | `SDK_VERSION_CFG_DEFINE=0x240001` |
| Makefile SDK date | `SDK_VERSION_DATE_DEFINE=20241231` |

## Correct Application And Board Entry

| Layer | File / target | Finding |
| --- | --- | --- |
| Top Makefile target | `make ac632n_spp_and_le` | Dispatches to `apps/spp_and_le/board/bd19`. |
| Code::Blocks project | `apps/spp_and_le/board/bd19/AC632N_spp_and_le.cbp` | Candidate IDE project for bd19 SPP + BLE. |
| App selection | `apps/spp_and_le/include/app_config.h` | `CONFIG_APP_SPP_LE` is `1`; SPP + BLE data transfer app is selected. |
| Clean HEAD board macro | `apps/spp_and_le/board/bd19/board_config.h` | Default is `CONFIG_BOARD_AC632N_DEMO`, not AC6321A. |
| AC6321A demo files | `board_ac6321a_demo.c/.h` | Exist and are the closest chip-specific reference, but not selected in clean HEAD. |

## Build Flow

| File | Role | Safety note |
| --- | --- | --- |
| Top `Makefile` | target router | `ac632n_spp_and_le` enters bd19 board Makefile. |
| `apps/spp_and_le/board/bd19/Makefile` | compile and link | `all` runs `pre_build`, links `cpu/bd19/tools/sdk.elf`, then runs post-build script. |
| `apps/spp_and_le/board/bd19/Makefile` | version macros | Defines `SDK_VERSION_CFG_DEFINE=0x240001`, `SDK_VERSION_DATE_DEFINE=20241231`. |
| `cpu/bd19/tools/download.c` | generates `download.bat` | Post-build script dispatches to `download/data_trans/download.bat`. |
| `cpu/bd19/tools/download/data_trans/download.bat` | burn/package script | Calls `isd_download.exe`; not safe for build-only. |

## Bluetooth Name Path

| Function | File | Current clean behavior |
| --- | --- | --- |
| Default classic BT name | `apps/spp_and_le/modules/user_cfg.c` | `bt_cfg.edr_name = "JL_app_debug"`. |
| Runtime name getter | `apps/spp_and_le/modules/user_cfg.c` | `bt_get_local_name()` returns `bt_cfg.edr_name`. |
| Runtime name setter | `apps/spp_and_le/modules/user_cfg.c` | `bt_set_local_name()` copies into `bt_cfg.edr_name`. |
| Config override | `apps/spp_and_le/modules/user_cfg.c` | `cfg_file_parse()` reads `CFG_BT_NAME` and can override `bt_cfg.edr_name`. |
| BLE name seed | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `ble_comm_set_config_name(bt_get_local_name(), 1)` uses classic name and appends BLE extension name when enabled. |
| Advertising payload | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `trans_make_set_adv_data()` and `trans_make_set_rsp_data()` build advertising/scan response. |

## BLE Connection Parameter Path

| Item | File | Clean value / behavior |
| --- | --- | --- |
| Advertising interval | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `ADV_INTERVAL_MIN = 160 * 5` units of 0.625 ms. |
| Update enable flag | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `trans_connection_update_enable = 1`. |
| Request table | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `trans_connection_param_table[]`. |
| Request call | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `ble_gatt_server_connetion_update_request(...)`. |
| Connection complete | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `GATT_COMM_EVENT_CONNECTION_COMPLETE`. |
| Disconnect complete | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `GATT_COMM_EVENT_DISCONNECT_COMPLETE`. |
| Final negotiated params | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `GATT_COMM_EVENT_CONNECTION_UPDATE_COMPLETE` prints interval, latency, timeout. |
| MTU | `apps/spp_and_le/examples/trans_data/ble_trans.c` | `GATT_COMM_EVENT_MTU_EXCHANGE_COMPLETE` prints ATT MTU. |

## Log UART Path

| Item | File | Clean setting |
| --- | --- | --- |
| UART0 config | `apps/spp_and_le/board/bd19/board_ac6321a_demo_cfg.h` | TX PA0, RX disabled, 1000000 baud. |
| UART0 platform data | `apps/spp_and_le/board/bd19/board_ac6321a_demo.c` | `UART0_PLATFORM_DATA_BEGIN(uart0_data)`. |
| Debug UART init | `apps/spp_and_le/board/bd19/board_ac6321a_demo.c` | `debug_uart_init()` calls `uart_init(&uart0_data)`. |
| Risk | hardware photo/schematic | PA0 accessible test point is not confirmed. |

## Low-Level Peripheral APIs

| Peripheral | Files | Key APIs |
| --- | --- | --- |
| SPI | `include_lib/driver/cpu/bd19/asm/spi.h`, `cpu/bd19/spi.c` | `spi_open`, `spi_close`, `spi_send_byte`, `spi_recv_byte`, `spi_send_recv_byte`, `spi_set_baud`. |
| Software IIC | `include_lib/driver/cpu/bd19/asm/iic_soft.h`, `cpu/bd19/iic_soft.c` | `soft_iic_init`, `soft_iic_start`, `soft_iic_stop`, `soft_iic_tx_byte`, `soft_iic_rx_byte`, buffer read/write APIs. |
| Hardware IIC | `include_lib/driver/cpu/bd19/asm/iic_hw.h`, `cpu/bd19/iic_hw.c` | `hw_iic_init`, `hw_iic_start`, `hw_iic_stop`, `hw_iic_tx_byte`, `hw_iic_rx_byte`. |
| ADC | `include_lib/driver/cpu/bd19/asm/adc_api.h`, `cpu/bd19/adc_api.c` | `adc_add_sample_ch`, `adc_get_value`, `adc_get_voltage`; PA1 is `AD_CH_PA1`. |
| USB1 GPIO macros | `include_lib/driver/cpu/bd19/asm/gpio.h` | `IO_PORT_DP1`, `IO_PORT_DM1`. |

## Existing WTYI Modifications In Original SDK

The original SDK directory already had pre-existing WTYI changes. They were preserved in:

- `project_docs/original_git_status.txt`
- `project_docs/original_changes.patch`

Notable historical changes:

- Switched `board_config.h` from `CONFIG_BOARD_AC632N_DEMO` to `CONFIG_BOARD_AC6321A_DEMO`.
- Forced classic BT name to `WTYI_BT_TEST`.
- Added dynamic BLE name demo code.
- Added UART heartbeat and hardware test code directly in `app_main.c`.
- Added IIC scan on PA7/PA8, SPI loopback on PB0/PB1/PB2, and ADC test on PB1.

These changes are useful reference material, but they do not match the new product wiring and should not be copied as-is.
