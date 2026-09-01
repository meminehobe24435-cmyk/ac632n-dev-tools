# 00 · 项目总览（Project Overview）

> 作者：尤译庆（2027 届嵌入式软件工程师校招候选人，实习于深圳维特智能）
> 仓库：`ac632n-dev-tools`（https://github.com/meminehobe24435-cmyk/ac632n-dev-tools）
> 本地镜像源：`<workspace>`

---

## 1. 一句话定义

这不是一个单一"产品工程"，而是作者在深圳维特智能实习期间维护的**杰理 AC632N（AC6321A）蓝牙开发个人工作区/自研工具集的完整镜像**：它以厂商 SDK 为核心，向外生长出一套"构建 → 烧录 → 串口日志 → BLE 扫描 → 蓝牙改名 → 日报自动化"的自研工具链，外加一份真实产品固件（WT9011DCL-BT50）的驱动与低功耗设计。

**证据**：
- `README.md:1-3` 明确写"杰理 AC6321A / AC632N 蓝牙开发工作区完整备份，覆盖 SPP+LE 固件构建、USB UBOOT 烧录、UART 串口日志、蓝牙改名构建、BLE 扫描、UART 打印 GUI 与钉钉日报 PPT 自动化全流程"。
- `README.md:41-45` 免责声明："本仓库为个人开发工作区备份；杰理 SDK、固件工程、工具链安装包等第三方内容版权归各自权利方所有"。

---

## 2. 背景（为什么做）

实习期间拿到一块杰理 AC6321A 蓝牙板（产品型号 **WT9011DCL-BT50**），需要完成以下闭环（出自 `work/WT9011DCL_BT50_FW/project_docs/requirements.md`）：

1. 在厂商 SDK（`fw-AC63_BT_SDK`）上构建 `apps/spp_and_le`（SPP 串口透传 + BLE 双模）应用，目标平台 `bd19`。
2. 把默认蓝牙名改成产品名 `WT9011DCL-BT50`，并验证电脑/手机能扫到 BLE 广播。
3. 打通 UART 串口日志（PA00 / 1000000 波特率），能收启动打印、心跳、BLE 广播数据。
4. 板载 SPI 六轴 IMU（QMI8658A/QMI8685A）、IIC 磁力计（QMC5883）、电池 ADC（PA1）的驱动与测试。
5. 做一版"低功耗门控"（low-power gate）框架，为后续电流实测打基础。
6. 顺便把每天的日报自动整理成 PPT 汇报。

**证据**：
- `requirements.md:1-8`：硬件 WT9011DCL-BT50 / 主控 AC6321A4 / SDK fw-AC63_BT_SDK / app spp_and_le / 平台 bd19。
- `work/WT9011DCL_BT50_FW/project_docs/hardware_map.md:21`：MCU = **AC6321A4**（schematic U1 / BOM `ac6321a4`）。
- 日报源 `ppt_daily_summary_source.txt`（2026/07/09 ~ 07/13）记录了四天的工作推进轨迹。

---

## 3. 难点（真实技术挑战）

| 难点 | 说明 | 证据 |
| --- | --- | --- |
| 厂商工具链 + Windows 构建闭环 | 杰理编译链在 `<sdk_path>\pi32`、`<sdk_path>\mc`，用 `make ac632n_spp_and_le` 触发，post-build 会自动拉起下载器 | `README_AC6321A_WORKFLOW.txt:20-23`、`build_ac632n_spp_and_le.bat` |
| 高波特率 UART 日志（1 Mbps） | 固件 PA00 以 1000000 波特率打日志，PC 侧需要稳定接收不丢帧 | `README_UART_LOG.md:8`、`serial_log_receiver.py:23` |
| 蓝牙名"改了没生效" | 配置区（syscfg/VM）里存有旧名字，会把代码里的 `.edr_name` 覆盖回来；且设备停在 BD19 UBOOT 模式时根本不会广播 | 日报 07/13、`user_cfg.c:217-218` 的 `bt_set_local_name("WTYI_BT_TEST",...)` 强制覆盖 |
| 运行时动态改名 | 改内存里的本地名后，BLE 广播包需要重建并重新使能广播才会刷新 | `ble_trans.c:799-825` 的 `wtyi_dynamic_ble_name_switch()` |
| 硬件引脚冲突 | 产品 SPI（PA7/PA8）与 SDK demo 的 IIC 选项 C 冲突；Type-C USB 口不是普通串口日志口 | `hardware_map.md:37`、`README_UART_LOG.md:17` |
| 低功耗"不能一睡了之" | 需要在 SDK 的 `REGISTER_LP_TARGET` 机制上叠加"板子当前忙不忙"的判断，避免 SPI/IIC/ADC/OTA 进行中被休眠 | `low_power_design.md:19`、`wtyi_power_manager.c` |

---

## 4. 「我的工作内容」三档（按面试时长选择口径）

### 档位 A —— 30 秒电梯版
> 我实习期间负责杰理 AC632N 蓝牙模块的驱动与工具链建设：写了一套 Python 串口日志工具（含 tkinter GUI 和 PyInstaller 打包成 exe）、蓝牙改名/构建的批处理脚本、BLE 扫描脚本，并在厂商 SDK 上实现了 BLE 动态改名 demo 和硬件自检（ADC/IIC/SPI 回环），最后做了一版低功耗门控框架。

### 档位 B —— 3 分钟标准版
> 一句话：这是"工具集 + 固件"两部分。
> 工具集部分（Python + 批处理）：`serial_log_receiver.py`（1 Mbps 串口日志接收/落盘）、`uart_print_gui.py`（tkinter 多线程 GUI，PyInstaller 打包成 `JieLi_UART_Print_Receiver.exe`）、`ble_scan_wtyi.py`（bleak 扫描 WTYI/JL/AC63 关键字）、`set_bt_name_and_build.ps1`（正则改 `.edr_name` 后调 make 重编，带参数校验）。
> 固件部分（C）：在 `apps/spp_and_le` 里加了 `[WTYI]` 启动打印 + 5 秒 UART 心跳、硬件自检（ADC 电压 / 软 IIC 扫描 / SPI1 回环）、以及 BLE 动态改名 demo（每 10 秒在 `WTYI_BT_TEST` ↔ `WTYI_BT_TEST_B` 间切换并重建广播包）。
> 产品固件（`work/WT9011DCL_BT50_FW`）：新建 `wtyi/` 驱动模块（SPI IMU、IIC 磁力计、ADC 电池、低功耗管理器），并写了一份 `low_power_design.md` 记录低功耗门控设计。

### 档位 C —— 8 分钟深度版
在 B 的基础上，额外讲清三层关系（详见 `02_SYSTEM_ARCHITECTURE.md`）：
1. **构建链**：`make ac632n_spp_and_le` → `<sdk_path>\pi32` 编译 → 产物 `cpu/bd19/tools/download/data_trans/update.ufw` / `jl_isd.fw` → `download.bat`（USB UBOOT）烧录。
2. **日志链**：固件 `printf` → UART PA00 @ 1000000 → USB-TTL（CH340/CP210x）→ `pyserial.read(4096)` → 落盘 + GUI 队列 → 关键字高亮。
3. **名字链路**：`.edr_name`（经典蓝牙名）→ `bt_set_local_name()` → `ble_comm_set_config_name()`（BLE 加 `(BLE)` 后缀）→ 重建 adv/rsp → `ble_gatt_server_adv_enable(1)`。

---

## 5. 最终成果（可量化的产出）

| 成果 | 证据 |
| --- | --- |
| 3 个 Python 串口/扫描工具 + 1 个 GUI（可打包 exe） | `python_tools/jieli_uart_logger.py`、`debug_tools/serial_log_receiver.py`、`debug_tools/uart_print_gui.py`、`debug_tools/ble_scan_wtyi.py` |
| 1 个 Windows 可执行 GUI（8.9 MB） | `dist/JieLi_UART_Print_Receiver.exe` + `build_uart_gui/*.spec` |
| 8 个根目录一键 bat + 5 个 debug_tools bat + 1 个 PowerShell 改名脚本 | 根目录 `01~07_*.bat`、`set_bt_name_and_build.ps1` |
| 固件侧 6 个文件的真实改动（有 git status 为证） | `sdk/.../project_docs/original_git_status.txt` 列出的 `app_main.c`、`board_ac6321a_demo.c`、`ble_trans.c`、`user_cfg.c`、`board_config.h` 等 |
| BLE 动态改名 demo 已实机跑通（日志里可见切换） | `logs/ac63_uart_gui_20260717_102150.log` 中 `WTYI dynamic name switch to: WTYI_BT_TEST_B` |
| 产品固件 `wtyi/` 驱动模块（SPI/IIC/ADC/低功耗） | `work/WT9011DCL_BT50_FW/apps/spp_and_le/wtyi/*.c` |
| 低功耗设计文档（无实测电流，诚实标注） | `work/WT9011DCL_BT50_FW/project_docs/low_power_design.md:164` |
| 钉钉日报 → PPT 自动化（node + `@oai/artifact-tool`） | `ppt_work/daily_report_summary/tmp/build_daily_report_deck.mjs` |

---

## 6. 一句话给面试官

> 这个仓库展示的不是"用了别人的 SDK"，而是**在一个不透明的厂商工具链之上，独立搭建了一套可复用的开发-调试-交付闭环**：从 make 构建、USB UBOOT 烧录、1 Mbps 串口日志 GUI，到蓝牙改名的机制分析、BLE 广播包重建，再到低功耗门控框架，每一层都能讲出"为什么"和"踩过的坑"。
