# CodeBlocks 编译与烧录尝试报告

时间：2026-07-23 16:56-16:58

## 结论

- CodeBlocks 工程：`apps/spp_and_le/board/bd19/AC632N_spp_and_le.cbp`
- CodeBlocks 目标：`Release`
- 编译结果：通过，`0 error(s), 8 warning(s)`
- after-build 下载脚本：已执行
- 烧录结果：失败，卡在向 BD19 UBOOT 设备下载 `bd19loader.bin`

## 设备识别

当前 Windows 可识别到目标 UBOOT 设备：

- `BD19 UBOOT1.00 USB Device`
- `USB Mass Storage Device`
- `USB\VID_4C4A&PID_3442`
- 盘符：`F:\`

未发现与本次筛选条件匹配的 Unknown/未知设备。

## 固件信息

- 待烧录文件：`D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin`
- 文件大小：`207004` bytes
- 生成时间：`2026-07-23 16:58:20`
- SHA256：`A89AE32A57AF5850193A960C235D7CE0D247659DE455710499F61FD5B663C6C3`

固件字符串检查：

- `WT9011DCL-BT50`：存在
- `[WTYI_BT]`：存在
- `[WTYI_BLE]`：存在
- `JL_app_debug`：不存在
- `JL_BT(BLE)`：不存在

## 下载链路

CodeBlocks 的 `Release` 目标在 after-build 阶段调用：

```bat
..\..\..\..\cpu\bd19\tools\download.bat sdk
```

随后 SDK 官方脚本调用：

```bat
cpu\bd19\tools\download\data_trans\download.bat
```

实际下载工具：

```bat
cpu\bd19\tools\isd_download.exe
```

关键参数来自 SDK 现有脚本和 `isd_config.ini`，平台为 `bd19`。

## 关键失败原文

```text
INI PATH: D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\isd_config.ini
ERROR: Failed to download loader(bd19loader.bin) to device(\\?\F:, addr:8192 arg:81)
=1167
Process terminated with status 0
0 error(s), 8 warning(s)
```

## 判断

CodeBlocks 能完成工程编译，也确实执行了 SDK 官方 after-build 下载脚本。失败点与前一次命令行烧录一致，说明当前阻塞不在 VSCode、`build_only` 脚本或蓝牙名称修改代码，而在官方下载器与 BD19 UBOOT 设备之间的 loader 传输阶段。

由于同一阶段已重复失败，不应继续反复烧录碰运气。下一步应核查官方烧录器模式、驱动、目标板下载模式和官方工具版本。
