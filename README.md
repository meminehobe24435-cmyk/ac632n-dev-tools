# ac632n-dev-tools

杰理 AC6321A / AC632N 蓝牙开发自研工具集，覆盖 **SPP+LE 固件构建、USB UBOOT 烧录、UART 串口日志、蓝牙改名构建、BLE 扫描、UART 打印 GUI** 与 **钉钉日报 PPT 自动化** 全流程。

> 说明：杰理官方 SDK 与固件工程副本（`sdk/`、`work/`、`tools/` 等）因体积过大且版权归原厂，未纳入本仓库；本地路径与依赖环境见 `README_AC6321A_WORKFLOW.txt`。

## 目录结构

```
├─ 01_open_codeblocks_project.bat      # 打开 Code::Blocks 工程
├─ 02_build_ac6321a_spp_and_le.bat     # 构建 SPP+LE 固件
├─ 03_burn_ac6321a_usb_uboot.bat       # USB UBOOT 烧录
├─ 04_open_isd_download_gui.bat        # 打开 ISD 下载工具
├─ 05_list_serial_ports.bat            # 列出串口
├─ 06_receive_uart_log_COMx.bat        # 接收 UART 日志（默认 1000000 波特率）
├─ 07_set_bt_name_build.bat            # 修改蓝牙名并重新构建
├─ build_ac632n_spp_and_le.bat         # 直接构建 ac632n_spp_and_le 目标
├─ set_bt_name_and_build.ps1           # 修改 .edr_name 并构建（参数校验版）
├─ python_tools/                       # 串口日志工具 jieli_uart_logger.py
├─ debug_tools/                        # 串口调试脚本、uart_print_gui.py、BLE 扫描、原理图
├─ build_uart_gui/                     # JieLi_UART_Print_Receiver PyInstaller spec
├─ dist/                               # JieLi_UART_Print_Receiver.exe（可直接运行）
├─ env_check/                          # 环境检查脚本与截图
├─ ppt_work/                           # 钉钉日报 PPT 自动化生成
├─ logs/                               # 调试期间串口日志
└─ README_AC6321A_WORKFLOW.txt         # 完整工作流说明
```

## 快速开始

环境依赖（本地）：杰理编译链 `C:\JL\pi32`、`C:\JL\mc`，SDK 位于 `D:\23178\JL\sdk\fw-AC63_BT_SDK`。

```bat
:: 构建 SPP+LE 固件
02_build_ac6321a_spp_and_le.bat

:: 修改蓝牙名并重新构建（PowerShell）
powershell -File set_bt_name_and_build.ps1 -Name MY_JL_BT
```

## 免责声明

本仓库仅包含自研脚本与工具；杰理 SDK、固件工程副本、工具链安装包等第三方内容的版权归各自权利方所有，未随仓库分发。
