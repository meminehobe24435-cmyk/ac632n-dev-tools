# 阶段2安全基线编译报告

生成时间：2026-07-23 16:12:19

## 结论

原始 `apps/spp_and_le/board/bd19` 基线工程已完成一次安全编译，编译错误数为 0。

本次只执行编译、链接和 `app.bin` 生成，没有执行 `download.bat`，没有执行 `isd_download.exe`，没有进行任何烧录操作。

## 编译命令

执行入口：

```bat
D:\23178\JL\work\WT9011DCL_BT50_FW\tools\wtyi\build_only.bat
```

内部安全构建命令：

```powershell
D:\23178\JL\work\WT9011DCL_BT50_FW\tools\utils\make.exe -C D:\23178\JL\work\WT9011DCL_BT50_FW\apps\spp_and_le\board\bd19 -f Makefile POST_SCRIPT=../../../../build_logs/generated_download_20260723_161003.bat pre_build ../../../../cpu/bd19/tools/sdk.elf
```

后处理生成 `app.bin`：

```powershell
C:\JL\pi32\bin\llvm-objdump.exe -D -address-mask=0x1ffffff -print-dbg sdk.elf > sdk.lst
C:\JL\pi32\bin\llvm-objcopy.exe -O binary -j <section> sdk.elf <section>.bin
D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\lz4_packet.exe -dict text.bin ...
cmd.exe /c copy /b text.bin+data.bin+data_code.bin+aec.bin+aac.bin+bank.bin+aptx.bin app.bin
```

说明：本次后处理没有运行 `isd_download.exe`，没有调用 `download.bat`，也没有进行任何下载烧录动作。

## 生成文件

固件 ELF：

```text
D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\sdk.elf
size: 2529028
sha256: C28E26E9C4E663198FB3903AEBC6175F26B3E325AD092808232EC522362A046B
```

基线 APP：

```text
D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin
size: 207024
sha256: 2290812A498470451D6A084FC90E9FFFA9F48A8A0F4A1337F51790BBFEE037A3
```

日志与清单：

```text
D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\build_only_20260723_161003.log
D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\firmware_manifest_20260723_161003.txt
D:\23178\JL\work\WT9011DCL_BT50_FW\build_logs\latest_firmware_manifest.txt
```

## 编译结果

错误：0

警告：8 条，均为链接阶段 `stack size limit exceeded`：

```text
active_update_task: 552
btstack_task: 376
sdp_send_service_search_attribute_request: 276
hci_send_event: 532
att_handle_request: 324
sdp_packet_handler: 380
bt_rf_init: 368
hci_send_event_cmd_complete: 536
```

这些 warning 来自未修改业务代码的基线工程，后续阶段若新增 warning，需要和本报告对比判断。

## 烧录步骤

本阶段未烧录。

已创建但未执行的烧录入口：

```bat
D:\23178\JL\work\WT9011DCL_BT50_FW\tools\wtyi\flash_confirmed.bat
```

该脚本要求输入明确确认词：

```text
FLASH-WT9011
```

在输入确认词前不会调用 SDK 下载脚本。执行烧录前必须先由人工确认硬件连接和下载模式。

## 预期串口输出

本阶段仍是未修改 SDK 的原始基线固件，尚未加入 `WTYI` 日志前缀。后续阶段 4 才会加入：

```text
[WTYI_BOOT]
[WTYI_BT]
[WTYI_BLE]
[WTYI_SPI]
[WTYI_IIC]
[WTYI_ADC]
[WTYI_ERROR]
```

## 硬件接线

本阶段编译不需要连接开发板，不需要 USB-TTL，不需要 Type-C 进入下载模式。

后续烧录前需要确认：

- Type-C 是否连接到板载 U7 下载口。
- 板子是否能进入官方下载模式。
- 不把 USB-TTL 当作下载器。
- 未确认 PA0 测试点前，不连接 UART 日志线。

## 验收方法

1. `build_only.bat` 返回码为 0。
2. `cpu\bd19\tools\app.bin` 存在。
3. `latest_firmware_manifest.txt` 中记录的 `APP_BIN_SHA256` 与当前文件一致。
4. 编译日志中无 `error:`。
5. 本阶段没有运行 `download.bat` 和没有执行烧录。

## 回退方法

本阶段未修改 SDK 源码。若需回退阶段 2 工具和报告，可删除本阶段新增的：

- `tools\wtyi\build_only.bat`
- `tools\wtyi\build_only.ps1`
- `tools\wtyi\flash_confirmed.bat`
- `project_docs\baseline_build_report.md`
- `build_logs\build_only_20260723_161003.log`
- `build_logs\firmware_manifest_20260723_161003.txt`
- `build_logs\latest_firmware_manifest.txt`

已生成的固件产物 `cpu\bd19\tools\app.bin`、`cpu\bd19\tools\sdk.elf` 可由下一次 `build_only.bat` 重新生成。
