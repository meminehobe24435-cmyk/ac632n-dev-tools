# 07 · 硬件交互（Hardware Interaction）

> 范围：AC6321A 芯片定位（以厂商文档能支持的为准）、USB UBOOT / 串口硬件链路、BD19、波特率、烧录流程。原理图仅列文件不深挖。

---

## 1. 芯片定位：AC6321A / AC632N

**结论（有证据）**：
- 主控是 **AC6321A4**（AC6321A 系列的一个具体封装/型号），SDK 平台名 **bd19**。
- 证据：`work/WT9011DCL_BT50_FW/project_docs/hardware_map.md:21` —— `MCU | AC6321A4 | schematic U1, top-side chip marking, BOM U1 Comment ac6321a4`；`requirements.md:4` —— 主控 AC6321A4；`requirements.md:8` —— 目标平台 bd19。

**AC632N 系列定位**【根据厂商 SDK 目录推断】：
- SDK `apps/spp_and_le/board/bd19/` 下同时有 `board_ac6321a_demo.c`、`board_ac6323a_demo.c`、`board_ac6328a_demo.c`、`board_ac6329b/c/e/f_demo.c` 等，说明 bd19 内核对应 AC6321/6323/6328/6329 一整个 AC632N 系列。
- 厂商 SDK 自带 `AC6321A_Datasheet V1.3.pdf`、`AC6321A_BLE参考原理图V2.0.pdf`、`AC632N用户手册开源版本V1.0.pdf`（见 git log 中 SDK 导入的文件清单）。
- 仓库 README 用"AC6321A / AC632N"泛指这一系列，与 SDK 目录结构一致。

**能支持到什么程度**：
- 它是杰理的低功耗蓝牙 SoC（BLE 广播/连接），本项目跑 `apps/spp_and_le`（SPP 经典蓝牙 + BLE 双模透传）。
- 内置 USB（Type-C 下载）、UART、SPI、IIC、ADC 等外设（SDK `include_lib/driver/cpu/bd19/asm/*.h` 有 uart/spi/iic/adc/usb 等驱动头文件）。

**面试口径**：
> AC6321A 是杰理 AC632N 家族（bd19 内核）里的一款低功耗蓝牙 SoC，我们的板子用 AC6321A4 具体型号；SDK 是闭源的 fw-AC63_BT_SDK，用 pi32 工具链编译。

---

## 2. 硬件链路：USB UBOOT 烧录 vs UART 日志

**两个独立通道，别混**（这是本项目反复踩的坑）：

| 通道 | 接口 | 用途 | 证据 |
| --- | --- | --- | --- |
| USB UBOOT | 板 Type-C（USB0 D+/D-） | 固件下载/烧录 | `README_AC6321A_WORKFLOW.txt:43`、`hardware_map.md:24` |
| UART 日志 | 板 PA00/TX + GND → USB-TTL | 运行日志接收 | `README_UART_LOG.md:5-15` |

**关键事实**：
- **Type-C 不是普通串口日志口**：`README_UART_LOG.md:17` 明确"板子的 USB-C 不是普通串口日志口。要收启动打印，需要 USB-TTL 模块。"
- **UBOOT 模式可能不枚举 COM 口**：`README_AC6321A_WORKFLOW.txt:43` "USB UBOOT mode is for burning. It may not create a COM port."
- **UBOOT 模式下扫不到 BLE**：`build_daily_report_deck.mjs:145` "如果板子停留在 BD19 UBOOT 下载模式，手机和电脑都扫不到 BLE 广播；复位到 JLDebug_V0.1 后才能验证运行状态。"（PPT 里的技术判断，与日报一致）

---

## 3. BD19 平台与烧录流程

**BD19 是什么**：杰理 SoC 的 CPU/内核平台代号（对应 AC632N 家族），SDK 里所有本芯片的编译产物都在 `cpu/bd19/tools/` 下。

**烧录流程（从脚本还原）**：
```mermaid
flowchart LR
    A["构建产物<br/>cpu/bd19/tools/download/data_trans/<br/>update.ufw + jl_isd.fw"] --> B["03_burn_*.bat<br/>cd data_trans + call download.bat"]
    B --> C["板进 BD19 UBOOT 模式<br/>(Type-C 接电脑)"]
    C --> D["download.bat 通过 USB 下载"]
    D --> E["输出出现 download complete"]
    E --> F["复位到 app 模式 → 广播/日志恢复"]
```

**证据**：
- 产物路径：`02_build_ac6321a_spp_and_le.bat:19-20`、`set_bt_name_and_build.ps1:37-38`（`update.ufw`、`jl_isd.fw`）。
- 烧录命令：`03_burn_ac6321a_usb_uboot.bat:3-5`。
- 下载工具：`04_open_isd_download_gui.bat`（`isd_download.exe`）。

**ISD 下载**：`isd_download.exe` 是杰理的 GUI 下载器；`download.bat` 是命令行封装（厂商提供，作者未改，只在脚本里调用）。

---

## 4. 波特率：1000000（1 Mbps）

**证据**：
- 固件侧 `app_main.c:126/236` 打印 `baud=1000000`。
- 文档 `README_UART_LOG.md:8`、`UART_DEBUG_README.txt:4`。
- 脚本默认 `serial_log_receiver.py:23`、`02_receive_COM8_1000000.bat`。

**面试怎么讲（底层）**：
- 1 Mbps 是"高波特率"，对 USB-TTL 芯片的时钟分频精度要求高（CH340/CP2102/FT232 常见支持范围内，但不同芯片误差不同）。
- 波特率误差若超过约 ±2~3%，采样点会漂移导致字节错位；这就是为什么"接收乱码先查波特率"。
- PC 侧只要 `pyserial` 传对 1000000，配合驱动支持的 USB-TTL，通常能通；乱码/丢帧更可能来自 USB-TTL 质量或线长/共地问题（详见 `08`）。

---

## 5. 板级硬件映射（产品板 WT9011DCL-BT50）

来自 `hardware_map.md`（作者从原理图 + BOM + 实物照片交叉核对）：

| 功能 | 引脚 | 器件 | 说明 |
| --- | --- | --- | --- |
| Type-C 下载 | USB0DM / USB0DP | TYPE-C-31-M-12 | 保留下载，不得用于 IIC |
| SPI IMU | CS=PB0 / SCK=PA7 / MOSI=PA8 / MISO=PA6 / INT=PA3 | QMI8685A | 6 轴 IMU；PA7/PA8 与 demo IIC 选项 C 冲突 |
| IIC 磁力计 | SCL=USB1DP / SDA=USB1DM（10K 上拉） | QMC5883P | 复用 USB1 差分对当 GPIO-IIC |
| 电池 ADC | PA1（PWR_ADC，3M/1M 分压） | — | VBAT = 采样电压 × 4 |
| UART 日志 | PA0（demo 用，产品板未确认测试点） | — | 不得假设 PA0 已引出 |
| 按键 | TSA053G20-250T | — | 需审查与 SPI CS 的 PB0 是否冲突 |

**面试口径**：
> 我做板级 bring-up 时，先自己从原理图/BOM/实物照片交叉核对出这份引脚映射表（`hardware_map.md`），再据此写 `wtyi_config.h` 的引脚宏和驱动。过程中发现 CAD 库复用陷阱（BOM 器件 QMI8685A 但库封装是 BMI160、QMC5883P 但库是 IST8308），都在文档里标注了风险。

---

## 6. 原理图（仅列文件，不深挖）

- `debug_tools/schematic/WT9011DCL_BT50_schematic.pdf`（296 KB）
- `debug_tools/schematic/page-1.png`（364 KB）
- `sdk/.../project_docs/WIT2026053WT9011DCL-BT50 V1.0.pdf`（同产品文档，296 KB）
- 厂商参考原理图在 SDK 的 doc 目录（如 `AC6321A_BLE参考原理图V2.0.pdf`、`AC6321A_Datasheet V1.3.pdf`），不深挖。

---

## 7. 环境依赖（<sdk_path> 工具链）

- 编译链：`<sdk_path>\pi32`（含 `clang`/`make`）、`<sdk_path>\mc`。
- 已确认本机存在（`<sdk_path>` 下含 `pi32`、`mc`、`commtools`、`isd_download` 相关）。
- `env_check/show_env_check.bat:3` 把 `<sdk_path>\pi32\bin;<sdk_path>\mc\bin` 加入 PATH 后 `where clang/make` 自检。

**面试口径**：
> 杰理工具链装在 `<sdk_path>\pi32`（clang 前端 + pi32 后端）和 `<sdk_path>\mc`，我写了个 `env_check/show_env_check.bat` 一键自检 codeblocks/clang/make/python/串口环境。
