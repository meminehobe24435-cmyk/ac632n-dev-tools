# 03 · 代码走读（Code Walkthrough）

> 挑 24 个最值得在面试中讲的文件，每个给「核心函数 / 为什么这样设计 / 改进空间 / GitHub 现场演示点击顺序」。演示顺序 = 面试官在 GitHub 网页上点开的推荐路径。

---

## 一、根目录脚本（构建 / 烧录 / 串口 / 改名）

### 1. `README_AC6321A_WORKFLOW.txt`（45 行）—— 工作流的"总纲"
- **核心内容**：工作目录、SDK 路径、当前 app（`apps/spp_and_le/board/bd19/AC632N_spp_and_le.cbp`）、板级 `CONFIG_BOARD_AC6321A_DEMO`、7 个常见动作、3 条注意事项。
- **为什么这样写**：把"本地依赖路径 + 常见动作"固化成可复现的说明，避免每次都要回忆命令。
- **面试讲法**：这是"把隐性知识显性化"的工程习惯——直接背出 7 步闭环。
- **演示顺序**：`README_AC6321A_WORKFLOW.txt` → `README.md`（第 1 步必读）。

### 2. `build_ac632n_spp_and_le.bat`（7 行）—— 构建最简版
- **核心**：`set PATH=<sdk>\tools\utils;<sdk_path>\pi32\bin;<sdk_path>\mc\bin` → `cd` → `make ac632n_spp_and_le`。
- **为什么这样设计**：`setlocal`/`endlocal` 隔离环境，`cd /d` 跨盘切换。
- **改进空间**：无错误处理（对比 `02_` 版本有 `errorlevel` 判断）；硬编码绝对路径。

### 3. `02_build_ac6321a_spp_and_le.bat`（22 行）—— 构建增强版
- **核心**：比精简版多了 `echo NOTE`（提示 post-build 会自动烧录）、`if errorlevel 1 ( BUILD FAILED + pause + exit /b 1 )`、产物路径提示。
- **为什么这样设计**：把"构建可能顺带烧录"这个副作用显式告知，符合"安全"要求。
- **改进空间**：`errorlevel` 只判断失败不判断产物存在；`pause` 阻塞自动化流水线（CI 下应去掉）。

### 4. `03_burn_ac6321a_usb_uboot.bat`（9 行）—— USB UBOOT 烧录
- **核心**：`cd cpu/bd19/tools/download/data_trans` → `call download.bat`。
- **为什么这样设计**：把"进入烧录目录 + 提醒进 UBOOT 模式"封装成双击动作。
- **改进空间**：烧录成功与否靠人肉看输出里的 "download complete"（脚本第 6 行注释），无自动判定。

### 5. `05_list_serial_ports.bat` / `06_receive_uart_log_COMx.bat` / `07_set_bt_name_build.bat`
- **核心**：分别把 `python serial_log_receiver.py --list`、`-p %PORT% -b 1000000`、`powershell ... -Name "%BTNAME%"` 包成 bat。
- **为什么这样设计**：降低使用门槛，把参数交互（`set /p`）留给 bat。
- **改进空间**：`%PORT%`/`%BTNAME%` 未做引号/特殊字符防护（详见 `13_CODE_REVIEW.md`）。

### 6. `set_bt_name_and_build.ps1`（38 行）—— 改名 + 构建（参数校验版）★重点讲
- **核心函数/逻辑**：
  - `param([ValidateLength(1,30)][string]$Name)` 强类型 + 长度校验（第 2-4 行）。
  - 正则白名单 `$Name -notmatch '^[A-Za-z0-9_\-]+$'` → `throw`（第 10-12 行）。
  - `-replace '\.edr_name\s*=\s*"[^"]*"', ".edr_name        = `"$Name`""`（第 15 行）。
  - 匹配不到则 `throw "Could not find .edr_name"`（第 16-18 行）。
  - 改完调 `make.exe ac632n_spp_and_le`，`$LASTEXITCODE -ne 0` 则 throw（第 26-29 行）。
  - `try/finally { Pop-Location }` 保证切目录后必还原（第 24-32 行）。
- **为什么这样设计**：这是"用户输入 → 修改源码 → 触发构建"的完整链路，三层校验（长度 + 白名单 + 替换命中）层层防错。
- **改进空间**：`Set-Content -Encoding UTF8` 可能改写源文件编码（GBK 注释源文件风险）；`-replace` 只匹配第一处（若文件里有多个 `.edr_name` 会漏）。

---

## 二、Python 串口工具

### 7. `python_tools/jieli_uart_logger.py`（74 行）—— 最早的串口日志工具
- **核心函数**：`show_ports()`（枚举 `list_ports.comports()`）、`main()`（argparse + `serial.Serial` + 落盘）。
- **为什么这样设计**：一个文件搞定"列口 + 收数 + 落盘"，带 `--hex` 十六进制模式（第 35 行）。
- **改进空间**：`while True` 无 `SerialException` 兜底（拔掉 USB-TTL 会裸抛）；`parser.error` 只在缺 port 时触发。

### 8. `debug_tools/serial_log_receiver.py`（97 行）—— 落盘版串口接收器 ★重点讲
- **核心函数**：
  - `list_serial_ports()`：打印 device/description/hwid（第 28-39 行）。
  - `open_log_file(log_dir)`：按 `ac63_uart_%Y%m%d_%H%M%S.log` 生成时间戳文件名（第 42-45 行）。
  - `receive(port, baud, log_dir, raw)`：`serial.Serial(port, baudrate=baud, timeout=0.2)` + `log_path.open("ab")`，循环 `ser.read(4096)` → `write + flush`；支持 `--raw` 直接写 `sys.stdout.buffer`（第 48-78 行）。
  - 异常分层：`KeyboardInterrupt`→退出码 0、`serial.SerialException`→退出码 2（第 73-78 行）。
- **为什么这样设计**：默认端口 COM8、默认波特率 1000000、默认日志目录 `<workspace>\logs` 全部可被 argparse 覆盖（第 83-87 行），既"开箱即用"又可脚本化。
- **改进空间**：无自动重连（`requirements.md:90` 要求"串口断开后自动重连"未实现）；无时间戳/关键字过滤（`requirements.md:88`、`:91`）；硬编码默认日志目录。

### 9. `debug_tools/uart_print_gui.py`（222 行）—— tkinter 多线程 GUI ★最重点讲
- **核心类/方法**：
  - `pick_default_port()`：按 `CH340/CP210/USB-SERIAL` 启发式选口（第 24-30 行）。
  - `UartPrintGui.__init__`：初始化 `rx_queue`/`stop_event`/`reader_thread`，`after(100, self._drain_queue)`（第 33-56 行）。
  - `start_reader()`：校验 port/baud、开日志文件、`threading.Thread(target=self._reader_loop, daemon=True)`（第 124-147 行）。
  - `_reader_loop()`：读线程内 `serial.Serial` 循环 `read(4096)` → 写文件 + `rx_queue.put(data)`（第 149-173 行）。
  - `_drain_queue()`：主线程每 100ms 从队列取数、`decode("utf-8", errors="replace")`、按 `WTYI`/`[ERROR]` 打 tag 高亮（第 175-191 行）。
  - `stop_reader()`/`on_close()`：`stop_event.set()` + `serial_handle.close()` + `destroy()`（第 193-207 行）。
- **为什么这样设计**：经典"生产者(读线程)-消费者(UI 线程)"模型，用 `queue.Queue` 解决 tkinter 非线程安全问题；daemon 线程保证窗口关闭能退出。
- **改进空间**：日志全量 `insert` 会随行数增长变卡（缺 `delete` 上限/环形缓冲）；`self.serial_handle` 跨线程读写无锁（第 152/168 行，实际风险低）；`pick_default_port` 匹配不到时 `ports[0]` 可能不是目标口。

### 10. `debug_tools/ble_scan_wtyi.py`（45 行）—— BLE 扫描验证 ★重点讲
- **核心函数**：`cb(device, adv)` 回调收集 `seen[address]=(name, rssi, ts)`，`any(k in name.upper() for k in KEYWORDS)` 命中打印 `[MATCH]`（第 17-23 行）；`main()` 扫描 30 秒后汇总去重列表并返回匹配数（`return 0 if matches else 1`，第 41 行）。
- **为什么这样设计**：把"验证改名是否生效"做成可脚本化、有退出码（0=找到/1=没找到）的命令，方便 CI/自动化。
- **改进空间**：`sys.path.insert(0, r"<workspace>\python_pkgs")` 硬编码；扫描时长 30 秒写死；`seen` 只按地址去重，同名不同地址会重复。

---

## 三、打包与环境

### 11. `build_uart_gui/JieLi_UART_Print_Receiver.spec`（38 行）—— PyInstaller 配置
- **核心**：`Analysis(['D:\\23178\\JL\\debug_tools\\uart_print_gui.py'], ...)`，`EXE(name='JieLi_UART_Print_Receiver', console=False, upx=True)`。
- **为什么这样设计**：`console=False` 出 GUI 无黑框，`upx=True` 压缩体积。
- **改进空间**：`hiddenimports=[]`/`datas=[]` 为空，若 pyserial 的 `serial.tools.list_ports` 未被自动收集，打包后 `--list` 会缺模块（属【待本人确认】）。

### 12. `env_check/show_env_check.bat`（31 行）—— 环境自检
- **核心**：`where codeblocks/clang/make/python` + `clang --version` + `python --version` + `python jieli_uart_logger.py --list`。
- **为什么这样设计**：一键排查"构建/烧录/串口"三件套环境是否就绪，输出可直接贴给同事/贴到 bug 报告。
- **改进空间**：`where` 找不到只返回非零不打印友好提示；未校验版本下限（仅打印版本）。

---

## 四、固件改动（C，作者在厂商 SDK 上的真实 diff）

### 13. `sdk/.../apps/spp_and_le/app_main.c`（388 行，作者改动 122-240 行）★重点讲
- **核心函数**：
  - `wtyi_uart_heartbeat(void *priv)`：每 5 秒 `printf("[WTYI] UART heartbeat %lu, ...\r\n", ++cnt)`（第 122-127 行）。
  - `wtyi_hw_test_adc/iic_scan/spi_loopback`：ADC 读 VBAT/VBG/PB1、软 IIC 扫描 0x08~0x77、SPI1 回环（0x55/0xaa/0x5a/0xa5）（第 134-193 行）。
  - `app_main()`：追加 `[WTYI]` 启动打印 + `sys_timer_add(NULL, wtyi_uart_heartbeat, 5000)`（第 234-240 行）。
- **为什么这样设计**：用 `[WTYI]` 前缀让日志可被 GUI 关键字高亮；心跳用于确认"固件活着 + UART 通"。
- **改进空间**：`WTYI_HW_TEST_ENABLE` 恒为 1（第 129 行），量产应关掉；`cnt` 是 `static u32` 无溢出保护（长期运行会回绕）。

### 14. `sdk/.../apps/spp_and_le/examples/trans_data/ble_trans.c`（1100 行，作者改动 83/125-129/798-825/1017-1020）★最重点讲
- **核心函数**：
  - `wtyi_dynamic_ble_name_switch()`：连接中跳过（第 804-807 行），否则 `index ^= 1` 切换名字，`adv_enable(0)` → `bt_set_local_name` → `ble_comm_set_config_name(name, 1)` → `trans_adv_config_set()` → `adv_enable(1)`（第 799-825 行）。
  - 注册：`sys_timer_add(NULL, wtyi_dynamic_ble_name_switch, 10000)`（第 1019 行）。
- **为什么这样设计**：这是对日报里"动态改名需要实机验证"的落地——先关广播、改名、重建广播包、再开广播，是 BLE 改名最稳妥的时序。
- **改进空间**：`add_ble_ext_name` 由 `DOUBLE_BT_SAME_NAME` 控制（第 812-816 行），依赖宏展开；切换逻辑未处理"广播正在发送中"的竞态。

### 15. `sdk/.../apps/spp_and_le/modules/user_cfg.c`（作者改动 44 / 217-218）★重点讲
- **核心**：`.edr_name = "WTYI_BT_TEST"`（第 44 行）；`cfg_file_parse()` 里读配置区旧名后强制 `bt_set_local_name("WTYI_BT_TEST", strlen(...))`（第 217-218 行）。
- **为什么这样设计**：这正是"改了 `.edr_name` 却还被旧名覆盖"问题的根因——配置区（syscfg/VM）的 `CFG_BT_NAME` 会覆盖代码默认值，所以要在 `cfg_file_parse` 末尾再强制一次。
- **改进空间**：硬编码字符串 `"WTYI_BT_TEST"` 出现两次（结构体 + 强制），易不一致；产品固件里已演进为宏 `WTYI_BT_CLASSIC_NAME`（见 `work/.../app_config.h:21`），是更好的做法。

### 16. `sdk/.../apps/spp_and_le/board/bd19/board_config.h`（39 行）
- **核心**：`#define CONFIG_BOARD_AC6321A_DEMO`（第 17 行），其余 board 宏注释掉。
- **为什么这样设计**：`board_config.h` 是板级选择入口，通过宏切换 demo 板。
- **演示讲法**：说明"改一个宏 = 换一块板级配置"的 SDK 惯例。

### 17. `sdk/.../apps/spp_and_le/board/bd19/board_ac6321a_demo.c`（作者改动 202-246）
- **核心**：`soft_iic_cfg[]`/`hw_iic_cfg[]`/`spi1_p_data` 等硬件总线配置，标了 `/* WTYI hardware bus config */`。
- **为什么这样设计**：把板级引脚（IIC 用 PA7/PA8，SPI1 用 PB0/PB1/PB2）集中声明。

---

## 五、产品固件（work/WT9011DCL_BT50_FW）

### 18. `apps/spp_and_le/wtyi/wtyi_config.h`（2.2 KB）—— 产品配置总开关
- **核心**：`WTYI_BOARD_NAME "WT9011DCL_BT50"`、`WTYI_LOW_POWER_ENABLE 1`、SPI/IIC/ADC 引脚宏、BLE 连接参数档（compatible/balanced/low_latency）。
- **为什么这样设计**：把产品特有的引脚、开关、参数集中到单一头文件，替代散落的 `#ifdef`。

### 19. `apps/spp_and_le/wtyi/wtyi_log.h`（868 字节）—— 统一日志宏
- **核心**：`WTYI_LOG(tag, fmt, ...)` → `printf(tag " " fmt, ...)`，派生 `WTYI_LOG_BT/BLE/SPI/IIC/ADC/ERROR`。
- **为什么这样设计**：统一前缀让日志可过滤；`#define WTYI_LOG(...)` 空实现可一键关闭（第 10 行）。

### 20. `apps/spp_and_le/wtyi/wtyi_power_manager.c`（2 KB）—— 低功耗门控 ★重点讲
- **核心**：busy flags（`WTYI_POWER_BUSY_BT/SPI/IIC/ADC/OTA`）+ `is_idle` 语义（任一 busy 返回非空闲）。
- **为什么这样设计**：见 `low_power_design.md:19` —— WTYI 层只回答"板子现在忙不忙"，不强制 poweroff，把休眠决策交给 SDK。
- **改进空间**：BT busy 仅作为"预留 API"，本阶段未接入真实连接流程（`low_power_design.md:95` 明确说明）。

### 21. `apps/spp_and_le/wtyi/wtyi_spi_imu.c` / `wtyi_iic_mag.c` / `wtyi_adc_battery.c`
- **核心**：SPI 读 QMI8685A 六轴 IMU、IIC 读 QMC5883 磁力计、ADC 采样电池电压（VBAT = 采样值 × 4）。
- **为什么这样设计**：按 `requirements.md` 的"测试目标"逐条实现（init/读 ID/读原始数据/转动变化）。
- **改进空间**：`hardware_map.md:48-49` 提示 BOM 器件是 QMI8685A/QMC5883P，但 `requirements.md` 写的是 QMI8658A，寄存器映射需以 BOM 为准（作者已在文档里诚实标注风险）。

### 22. `apps/spp_and_le/include/app_config.h`（作者加 21-22 行）
- **核心**：`#define WTYI_BT_CLASSIC_NAME "WT9011DCL-BT50"`、`#define WTYI_BT_BLE_NAME "WT9011DCL-BT50"`。
- **为什么这样设计**：比 SDK 里硬编码 `"WTYI_BT_TEST"` 更工程化——产品名集中定义、可复用。

### 23. `project_docs/low_power_design.md`（164 行）★文档范例
- **核心**：目标（"只加简单可回退的低功耗门控"）、SDK 调用链白话、busy flag 规则、OTA 规则、编译产物（`app.bin 207472 字节 + SHA256`）、回滚步骤、硬件验证清单。
- **为什么这样写**：诚实声明"不声称实测电流"（第 164 行），把"能做什么/没做什么"讲清楚——这是嵌入式工程师该有的严谨。

### 24. `project_docs/hardware_map.md`（70 行）★文档范例
- **核心**：板卡身份表（MCU=AC6321A4）、引脚映射表、器件表、照片 vs 原理图核对、硬件事实清单（"PA7/PA8 属产品 SPI，不得复用为 IIC"等）。
- **为什么这样写**：把 BOM/原理图/实物照片三方交叉核对，提前暴露 QMI8658A vs QMI8685A、IST8308 vs QMC5883P 等"CAD 库复用陷阱"。

---

## 六、日报自动化（Node.js）

### 25. `ppt_work/daily_report_summary/tmp/build_daily_report_deck.mjs`（252 行）
- **核心**：`Presentation.create({slideSize:{1280x720}})`，辅助函数 `addText/addBox/addRule/titleSlide/addSlideTitle/bulletList/metric/card`，逐页构建 10 页汇报，`PresentationFile.exportPptx(p)` 导出。
- **为什么这样设计**：把日报这种"固定结构文本"用"布局原语函数"参数化渲染，改内容不改代码结构。
- **改进空间**：输出路径 `OUT` 硬编码到 `D:/zhuomian/...`（第 4 行）；配色/字体写死在常量 `C`/`font`。

### 26. `ppt_work/daily_report_template_simple/tmp/container_tools/artifact_tool_utils.mjs`（473 行）
- **核心**：`parseArgs/requireArg`、`isWithin`（路径穿越防护）、`collectFonts`、`ensureArtifactToolWorkspace`、`createSlideContext`（含 `addText/addShape/addImage/addLucideIcon`）。
- **为什么这样设计**：把"artifact-tool 运行时装载 + 路径安全 + 图标渲染"沉淀为可复用工具库。
- **改进空间**：`isWithin` 依赖 `path.relative` 但未处理 symlink 逃逸（`artifact_tool_utils.mjs:43-46`，属【根据代码推断】）。

---

## 七、日志样本（面试现场可读）

### 27. `logs/ac63_uart_gui_20260717_102150.log`（4.3 MB）★可读样本
- **头部**：`[00:00:19.497][Info]: [SPP_AND_LE]app_key_evnet: 1,9` + `WTYI dynamic name switch to: WTYI_BT_TEST` + `trans_adv_data(26): 02 01 06 03 03 30 AF 12 09 ...`。
- **尾部**：`WTYI dynamic name switch to: WTYI_BT_TEST_B` + `ble name(19): WTYI_BT_TEST_B(BLE)`。
- **讲法**：直接展示"动态改名 demo 真实跑通"的日志证据，并指出 `IPP`/`PIPP` 这类交错的短 ASCII 碎片是高波特率/多 printf 源下的接收伪影（详见 `05/08`）。

---

## 八、GitHub 现场演示推荐点击顺序（5 分钟）

1. `README.md` → `README_AC6321A_WORKFLOW.txt`（建立"工作区 + 闭环"认知）。
2. `debug_tools/uart_print_gui.py`（最重头，讲多线程 + 队列）。
3. `debug_tools/serial_log_receiver.py`（讲落盘 + 默认参数）。
4. `debug_tools/ble_scan_wtyi.py`（讲异步扫描 + 退出码）。
5. `set_bt_name_and_build.ps1`（讲参数校验 + 改名原理）。
6. `sdk/.../examples/trans_data/ble_trans.c`（搜 `WTYI`，讲动态改名时序）。
7. `sdk/.../modules/user_cfg.c`（搜 `edr_name`，讲"旧名覆盖"根因）。
8. `work/WT9011DCL_BT50_FW/project_docs/low_power_design.md`（讲低功耗门控）。
9. `logs/ac63_uart_gui_20260717_102150.log`（看真实日志）。
10. `build_uart_gui/*.spec` + `dist/*.exe`（讲打包交付）。
