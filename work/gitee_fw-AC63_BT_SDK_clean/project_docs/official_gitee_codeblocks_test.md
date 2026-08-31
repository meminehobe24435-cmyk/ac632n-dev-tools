# 官方 Gitee SDK CodeBlocks 测试记录

时间：2026-07-23 17:19-17:25

## 测试来源

- 官方仓库：`https://gitee.com/Jieli-Tech/fw-AC63_BT_SDK.git`
- 本地干净 worktree：`D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean`
- 提交：`b355973 修正介绍信息`
- 工程：`apps/spp_and_le/board/bd19/AC632N_spp_and_le.cbp`
- CodeBlocks 目标：`Release`

## 执行动作

使用 CodeBlocks 批量构建方式执行：

```bat
D:\23178\JL\CodeBlocks\codeblocks.exe --multiple-instance --build D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean\apps\spp_and_le\board\bd19\AC632N_spp_and_le.cbp --target=Release --no-batch-window-close
```

该目标的 after-build 动作为：

```bat
..\..\..\..\cpu\bd19\tools\download.bat sdk
```

## 编译结果

已生成：

- `D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean\cpu\bd19\tools\sdk.elf`
- `D:\23178\JL\work\gitee_fw-AC63_BT_SDK_clean\cpu\bd19\tools\app.bin`

固件信息：

- `app.bin` 大小：`206780` bytes
- `app.bin` SHA256：`A486C0D0FEB97724AE5A039FE17A0820E74720CA69A1054982C68D2C3761944A`

固件字符串检查：

- `JL_app_debug`：存在
- `JL_BT`：存在
- `WT9011DCL-BT50`：不存在

## 设备状态

构建前曾识别到：

- `BD19 UBOOT1.00 USB Device`
- `USB\VID_4C4A&PID_3442`
- 盘符：`F:\`

构建后当前识别到：

- `USB Composite Device`，`USB\VID_1544&PID_464B`，状态 OK
- `USB Mass Storage Device`，状态 OK
- `Jieli Debug`，状态 Error，错误码 28
- `JLDebug 2.00 USB Device`，状态 Error，错误码 38

## 判断

官方干净 SDK 可以在 CodeBlocks 下生成 bd19 固件。由于 CodeBlocks 批量窗口的下载输出没有被捕获到文件，且当前 Windows 对 `Jieli Debug` 接口仍显示驱动异常，本次不能只凭工具退出判断烧录成功。

如果手机能扫描到官方默认名称 `JL_BT` 或 `JL_app_debug`，则说明官方基线固件已烧录成功；如果仍然是旧名称或无广播，则需要先处理本机 Jieli Debug 驱动/烧录器状态。
