# ac632n-dev-tools

杰理 AC6321A / AC632N 蓝牙开发工作区完整备份，覆盖 **SPP+LE 固件构建、USB UBOOT 烧录、UART 串口日志、蓝牙改名构建、BLE 扫描、UART 打印 GUI** 与 **钉钉日报 PPT 自动化** 全流程，按类别整理如下。

## 目录结构

```
├─ 01~07_*.bat / build_ac632n_spp_and_le.bat   # 构建/烧录/串口快捷脚本
├─ set_bt_name_and_build.ps1                   # 修改蓝牙名并重新构建（参数校验版）
├─ README_AC6321A_WORKFLOW.txt                 # 完整工作流说明（本地依赖路径）
├─ sdk/                                        # 杰理官方 SDK（fw-AC63_BT_SDK 完整工程）
├─ work/                                       # 固件工程副本
│  ├─ gitee_fw-AC63_BT_SDK_clean/              # SDK 干净副本
│  ├─ gitee_fw-AC63_BT_SDK_official_burn/      # 官方烧录版副本
│  ├─ WT9011DCL_BT50_FW/                       # WT9011DCL BT50 产品固件工程
│  └─ gitee_fw-AC63_BT_SDK/                    # （本地为空检出，无内容）
├─ tools/                                      # 杰理工具链安装包与辅助工具
├─ CodeBlocks/                                 # Code::Blocks IDE 运行时
├─ python_tools/                               # 自研串口日志工具 jieli_uart_logger.py
├─ debug_tools/                                # 串口调试、BLE 扫描、uart_print_gui.py、原理图
├─ build_uart_gui/ + dist/                     # JieLi_UART_Print_Receiver GUI（spec + exe）
├─ env_check/                                  # 环境检查脚本与截图
├─ ppt_work/                                   # 钉钉日报 PPT 自动化
├─ logs/                                       # 调试期间串口日志
├─ screenshots/                                # 工具截图
└─ python_pkgs/                                # Python 依赖包缓存
```

## 快速开始

环境依赖（本地）：杰理编译链 `C:\JL\pi32`、`C:\JL\mc`，SDK 位于 `D:\23178\JL\sdk\fw-AC63_BT_SDK`。

```bat
:: 构建 SPP+LE 固件
02_build_ac6321a_spp_and_le.bat

:: 修改蓝牙名并重新构建（PowerShell）
powershell -File set_bt_name_and_build.ps1 -Name MY_JL_BT
```

## 说明与免责声明

- 本仓库为个人开发工作区备份；杰理 SDK、固件工程、工具链安装包等第三方内容版权归各自权利方所有，仅供学习与开发备份使用。
- 个别文件（如 `cpu/*/tools/*_config_tool/conf/source/tone/wtgv2_decode.exe`）因本地杀毒软件隔离而缺失，可在需要时从原始环境恢复。
- 若涉及公司产品资料（如 `debug_tools/schematic/`），请自行确认公开授权。
