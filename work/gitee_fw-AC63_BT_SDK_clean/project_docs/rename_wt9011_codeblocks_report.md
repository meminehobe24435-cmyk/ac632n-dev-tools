# WT9011DCL-BT50 改名固件 CodeBlocks 记录

时间：2026-07-23 17:31-17:33

## 来源

- 仓库：`https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK.git`
- 本地 worktree：`D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean`
- 基线提交：`b355973 修正介绍信息`
- 工程：`apps/spp_and_le/board/bd19/AC632N_spp_and_le.cbp`
- 目标：`Release`

## 修改文件

- `apps/spp_and_le/include/app_config.h`
- `apps/spp_and_le/modules/user_cfg.c`
- `apps/common/third_party_profile/jieli/gatt_common/le_gatt_common.c`
- `apps/spp_and_le/examples/trans_data/ble_trans.c`
- `apps/spp_and_le/modules/bt/app_comm_edr.c`
- `apps/spp_and_le/modules/bt/app_comm_ble.c`

## 修改内容

- 增加统一名称宏：`WTYI_BT_NAME = WT9011DCL-BT50`
- 经典蓝牙名称固定为：`WT9011DCL-BT50`
- BLE 广播名称固定为：`WT9011DCL-BT50`
- BLE 不再追加 `(BLE)`
- 增加简短日志：
  - `[WTYI_BT] classic_name=...`
  - `[WTYI_BT] connected`
  - `[WTYI_BT] disconnected`
  - `[WTYI_BLE] init_ok`
  - `[WTYI_BLE] adv_name=...`

## CodeBlocks 执行

```bat
D:\23178\JL\CodeBlocks\codeblocks.exe --multiple-instance --build D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean\apps\spp_and_le\board\bd19\AC632N_spp_and_le.cbp --target=Release --no-batch-window-close
```

CodeBlocks `Release` 目标会执行 SDK 原有 after-build：

```bat
..\..\..\..\cpu\bd19\tools\download.bat sdk
```

## 固件结果

- 固件：`D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean\cpu\bd19\tools\app.bin`
- 大小：`206940` bytes
- 生成时间：`2026-07-23 17:32:37`
- SHA256：`33A1EC82A86B30E5106A4D1B8580FF0EC0B01416D20F5B88424A52DA3410AF57`

固件字符串检查：

- `WT9011DCL-BT50`：存在
- `[WTYI_BT]`：存在
- `[WTYI_BLE]`：存在
- `AC632N_1`：不存在
- `AC632N_1(BLE)`：不存在
- `JL_app_debug`：不存在
- `JL_BT(BLE)`：不存在

## 当前 USB 状态

CodeBlocks 执行后设备从 `BD19 UBOOT1.00 USB Device` 切换为：

- `USB\VID_1544&PID_464B`
- `USB Composite Device`：OK
- `USB Mass Storage Device`：OK
- `Jieli Debug`：Error 28
- `JLDebug 2.00 USB Device`：Error 38

## 验收方法

用手机重新扫描蓝牙：

- BLE 扫描预期：`WT9011DCL-BT50`
- 经典蓝牙扫描预期：`WT9011DCL-BT50`

如果仍显示 `AC632N_1(BLE)`，说明本次改名固件未真正烧录生效；如果显示目标名，则改名和烧录通过。
