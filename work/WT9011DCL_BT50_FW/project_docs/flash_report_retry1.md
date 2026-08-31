# 换线后烧录重试报告

生成时间：2026-07-23 16:44

## 结果

换线后重试烧录失败，已停止，未继续重复烧录。

## 换线后识别状态

重试前 Windows 能识别：

```text
BD19 UBOOT1.00 USB Device
PNPDeviceID: USBSTOR\DISK&VEN_BD19&PROD_UBOOT1.00&REV_1.00\7&6A1CA24&0
Status: OK
```

未知/错误设备查询为空。

## 目标固件

```text
D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin
sha256: 78D69E10546AD47FB54DFEB28D58E975DC30981367D29664B9047B238167565A
```

重试前 `flash_confirmed.bat` 已再次通过 SHA256 复核。

## 失败原文

```text
INI PATH: D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\isd_config.ini
ERROR: Failed to download loader(bd19loader.bin) to device(\\?\F:, addr:8192 arg:81)
错误: 发送设备信息失败，错误=1167
```

后续 `jl_isd.ufw` 未生成，属于 loader 下载失败后的连带错误。

## 重试后状态

重试失败后再次查询 `Win32_DiskDrive`，未枚举到 `BD19 UBOOT1.00 USB Device`。

## 判断

换线后仍失败在同一个阶段和同一个错误码：

- 失败阶段：下载 `bd19loader.bin`
- 目标设备：`\\?\F:`
- 地址：`0x2000` / 十进制 `8192`
- 错误码：`1167`

本次不再继续重试。下一步应检查官方烧录器模式、目标板下载模式保持、电源稳定性或官方工具要求，而不是继续改固件或反复烧录。

## 日志文件

```text
D:\23178\JL\work\WT9011DCL_BT50_FW\project_docs\current_programmer_detection_retry1.txt
D:\23178\JL\work\WT9011DCL_BT50_FW\project_docs\flash_output_retry1.txt
```
