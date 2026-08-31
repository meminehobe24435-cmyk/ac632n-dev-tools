# 蓝牙名称修改与编译报告

生成时间：2026-07-23 16:34:59

## 修改内容

- 经典蓝牙名称：`WT9011DCL-BT50`
- BLE 广播名称：`WT9011DCL-BT50`
- BLE 名称不再追加 `(BLE)` 后缀。
- 固件启动后会打印：
  - `[WTYI_BT] classic_name=WT9011DCL-BT50`
  - `[WTYI_BLE] adv_name=WT9011DCL-BT50`
  - `[WTYI_BT] init_ok`
  - `[WTYI_BLE] init_ok`
  - `[WTYI_BT] connected`
  - `[WTYI_BT] disconnected`
  - `[WTYI_BLE] connected`
  - `[WTYI_BLE] disconnected`

## 修改文件

```text
apps/spp_and_le/include/app_config.h
apps/spp_and_le/modules/user_cfg.c
apps/spp_and_le/examples/trans_data/ble_trans.c
apps/spp_and_le/modules/bt/app_comm_ble.c
apps/spp_and_le/modules/bt/app_comm_edr.c
apps/common/third_party_profile/jieli/gatt_common/le_gatt_common.c
```

## 下载链路确认

官方脚本：

```text
cpu/bd19/tools/download/data_trans/download.bat
```

实际下载命令来源于 SDK 现有脚本：

```bat
..\..\isd_download.exe ..\..\isd_config.ini -tonorflash -dev bd19 -boot 0x2000 -div8 -wait 300 -uboot ..\..\uboot.boot -app ..\..\app.bin ..\..\cfg_tool.bin -res ..\..\p11_code.bin -uboot_compress -flash-params flash_params.bin
```

下载输入为：

```text
cpu/bd19/tools/app.bin
cpu/bd19/tools/cfg_tool.bin
cpu/bd19/tools/p11_code.bin
cpu/bd19/tools/uboot.boot
cpu/bd19/tools/flash_params.bin
cpu/bd19/tools/isd_config.ini
```

平台配置：

```text
isd_config.ini: CHIP_NAME = AC632N
isd_config.ini: PID = AC632N
download command: -dev bd19
Windows device: BD19 UBOOT1.00 USB Device
```

说明：SDK 对 bd19/AC632x 使用 `AC632N` 作为下载配置族名，本次未切换板级工程，未修改电源配置。

## 编译命令

```bat
D:\23178\JL\work\WT9011DCL_BT50_FW\tools\wtyi\build_only.bat
```

## 编译结果

- 编译结果：成功
- 编译错误：0
- Warning：8 条，均为基线已存在的链接阶段 `stack size limit exceeded`
- 编译日志：`D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\build_only_20260723_163448.log`
- 固件清单：`D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\firmware_manifest_20260723_163448.txt`

## 新固件

```text
D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin
size: 207312
sha256: 78D69E10546AD47FB54DFEB28D58E975DC30981367D29664B9047B238167565A
last_write_time: 2026-07-23 16:34:58
```

基线 SHA256：

```text
2290812A498470451D6A084FC90E9FFFA9F48A8A0F4A1337F51790BBFEE037A3
```

结论：新固件 SHA256 已变化，不是基线旧固件。

## 固件字符串检查

在 `app.bin` 中已找到：

```text
WT9011DCL-BT50
[WTYI_BT]
[WTYI_BLE]
```

在 `app.bin` 中未找到：

```text
JL_app_debug
JL_BT(BLE)
(BLE)
```

## 烧录前条件

- 烧录器/目标枚举：`BD19 UBOOT1.00 USB Device`
- 接口：USB
- PNPDeviceID：`USBSTOR\DISK&VEN_BD19&PROD_UBOOT1.00&REV_1.00\7&6A1CA24&0`
- 状态：OK
- 未检测到明确未知设备记录。
