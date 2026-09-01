# 10 · 面试问答（Interview Q&A）

> 20~40 题，分星级（★ 基础 / ★★ 进阶 / ★★★ 深度）。答案全部结合真实代码，面向嵌入式软件岗：UART 底层、波特率误差、缓冲、蓝牙名修改原理、批处理自动化、Python 与嵌入式协作。

---

## 一、UART / 串口底层

### Q1（★）说说你的 UART 日志链路，从芯片到文件。
**答**：固件 `printf` → UART0 TX（PA00）→ 1000000 波特率 → USB-TTL（CH340/CP210x）→ Windows 虚拟 COM 口 → `pyserial` 打开 → `read(4096)` 循环 → 追加写日志文件（`"ab"`）+ 显示（`utf-8 errors=replace`）。
**代码**：`app_main.c:126`、`README_UART_LOG.md:5-15`、`serial_log_receiver.py:56-66`。

### Q2（★★）波特率 1000000 是什么意思？为什么 1 Mbps 容易出问题？
**答**：每秒 1,000,000 个码元（这里 8N1，每字节 10 bit，理论 100 KB/s 上限）。高波特率下：① USB-TTL 芯片要用主时钟精确分频出 1 Mbps，分频误差会累积到采样点漂移，误差超过约 ±2~3% 就开始错位乱码；② PC 侧 `read` 不及时，内核 FIFO/驱动缓冲溢出就丢帧。所以"收不到/乱码先查波特率和线材"。
**代码**：`DEFAULT_BAUD = 1_000_000`（`serial_log_receiver.py:23`）。

### Q3（★）`serial.Serial(port, baudrate=baud, timeout=0.2)` 里 timeout 是波特率吗？
**答**：不是。`timeout` 是**读超时**（秒），`read()` 最多等 0.2s 就返回（即使没读满）；波特率由 `baudrate` 决定。用 `timeout` 是为了让 `read` 不永久阻塞，配合 `if not data: continue` 形成轮询。
**代码**：`serial_log_receiver.py:56-62`。

### Q4（★★）为什么落盘用 `"ab"` 二进制，显示才 decode？
**答**：落盘要**保真**——高波特率下可能读到半个多字节序列，落盘存原始字节不丢任何信息；显示时用 `errors="replace"` 把非法 UTF-8 替换成 �，避免解码崩溃。两阶段分离，文件和屏幕各司其职。
**代码**：`serial_log_receiver.py:56`（`"ab"`）、`:71`（`errors="replace"`）。

### Q5（★★）`read(4096)` 和一次 `read()` 一字节哪个好？
**答**：批量好。每次 `read` 都是系统调用 + 串口缓冲区拷贝，`read(4096)` 一次搬 4 KB，大幅降低 syscall 频率，高波特率下吞吐更好。代价是可能读到"半条"日志，需要行缓冲或容忍碎片。

### Q6（★★）串口缓冲区满了会怎样？怎么防？
**答**：驱动/内核接收 FIFO 满了会丢字节（或拉流控），表现就是日志缺字/乱码。防法：读线程及时、批量读、加大超时下的读取频率；固件侧降低打印频率/波特率；必要时硬件流控（RTS/CTS）。本仓库日志里的 `IPP`/`PIPP` 碎片就疑似高波特率下丢字/半包的产物【根据日志推断】。

### Q7（★★★）8N1 的 N 是什么？如果固件是偶校验会怎样？
**答**：8 数据位、No parity、1 停止位，每字节 10 bit（1 起始 + 8 数据 + 1 停止）。若固件偶校验而接收端 8N1，校验位被当成数据位，奇偶不匹配时数据错乱或看不到明显报错。本仓库脚本**未显式**指定 bytesize/parity/stopbits，用的是 pyserial 默认 8N1【待本人确认】。

---

## 二、蓝牙 / BLE

### Q8（★）怎么改的蓝牙名？改哪一行？
**答**：改 `user_cfg.c` 里 `BT_CONFIG bt_cfg` 的 `.edr_name` 字段（经典蓝牙名）。脚本 `set_bt_name_and_build.ps1` 用正则 `-replace '\.edr_name\s*=\s*"[^"]*"'` 替换后重新 `make`。
**代码**：`user_cfg.c:44`、`set_bt_name_and_build.ps1:15`。

### Q9（★★）改了 `.edr_name` 为什么还会被旧名覆盖？
**答**：因为配置区（flash 的 syscfg/VM）里存了旧名字，`cfg_file_parse()` 里 `syscfg_read(CFG_BT_NAME, ...)` 会把旧名 `memcpy` 回 `bt_cfg.edr_name`，覆盖代码默认值。所以要在读取之后**强制** `bt_set_local_name("WTYI_BT_TEST", ...)` 再覆盖一次；产品固件里进一步 `syscfg_write` 把新名写回配置区持久化。
**代码**：`user_cfg.c:217-218`（强制）、`work/.../user_cfg.c:223-227`（写回）。

### Q10（★★）经典蓝牙名和 BLE 广播名的区别？
**答**：经典蓝牙（BR/EDR）用 `.edr_name`（`bt_get_local_name()` 返回）；BLE 广播名由 `ble_comm_set_config_name(name, add_ble_ext_name)` 生成，`add_ble_ext_name=1` 时会追加 `(BLE)` 后缀。所以日志里看到 `WTYI_BT_TEST(BLE)`。
**代码**：`ble_trans.c:812-822`、日志 `ble name(17): WTYI_BT_TEST(BLE)`。

### Q11（★★★）运行时动态改 BLE 广播名，要做什么？
**答**：不能只改内存字符串。正确时序（`ble_trans.c:820-824`）：① `ble_gatt_server_adv_enable(0)` 先关广播；② `bt_set_local_name()` 改本地名；③ `ble_comm_set_config_name()` 重新生成含 `(BLE)` 后缀的名字；④ `trans_adv_config_set()` 重建 adv/rsp 广播包；⑤ `ble_gatt_server_adv_enable(1)` 重新广播。否则扫描端缓存/广播包不刷新。
**代码**：`wtyi_dynamic_ble_name_switch()`（`ble_trans.c:799-825`）。

### Q12（★★）BLE 广播包结构？日志里那串 hex 是什么？
**答**：BLE 广播包是 TL（Type-Length）AD 结构序列。日志 `02 01 06 | 03 03 30 AF | 12 09 <name>` 三段：Flags(0x01)=06、Complete List of 16-bit UUID(0x03)=0x30AF、Complete Local Name(0x09)=`WTYI_BT_TEST(BLE)`。广播包上限 31 字节（`ADV_RSP_PACKET_MAX`）。
**代码**：`ble_trans.c:119-120`、日志 `trans_adv_data(26)`。

### Q13（★）UBOOT 模式和 app 模式有什么区别？为什么烧录完扫不到 BLE？
**答**：UBOOT 是烧录引导态（用于 USB 下载，可能不枚举 COM 口、不跑 app、不广播）；app 是正常运行态（广播 BLE、打 UART 日志）。烧录完必须复位切到 app 模式才能被扫到。
**代码**：`README_AC6321A_WORKFLOW.txt:43`、`UART_DEBUG_README.txt:39`。

### Q14（★★）RSSI 能当测距用吗？
**答**：RSSI（接收信号强度，dBm）只能做**粗粒度**参考。它受距离、遮挡、天线方向、多径、发射功率影响，不能当精确测距。`ble_scan_wtyi.py` 只把它打出来做"在不在附近/信号强弱"判断。
**代码**：`ble_scan_wtyi.py:21-23`。

---

## 三、Python 与嵌入式协作 / 并发

### Q15（★★）GUI 为什么用多线程 + 队列，而不是直接在读线程刷新 UI？
**答**：tkinter 不是线程安全的，跨线程操作 UI 控件是未定义行为/易崩。所以读线程只 `rx_queue.put(data)`，主线程用 `after(100, self._drain_queue)` 轮询队列消费并 `insert`。这是标准的"生产者-消费者"解耦。
**代码**：`uart_print_gui.py:145-147`、`:175-191`。

### Q16（★★）`queue.Queue` 是线程安全的吗？为什么用它？
**答**：是，`queue.Queue` 内部有锁，支持多生产者/多消费者安全存取。用它让读线程（生产者）和 UI 线程（消费者）不共享可变状态，避免加锁竞态。

### Q17（★★）`daemon=True` 的线程有什么用？关窗口时怎么优雅退出？
**答**：daemon 线程在主线程退出时不会阻止进程结束。关窗口走 `on_close()`：先 `stop_reader()`（`stop_event.set()` + `serial_handle.close()` 踢醒阻塞的 `read()`），再 `destroy()`。用关闭串口来唤醒阻塞读，是常见的协作式停止技巧。
**代码**：`uart_print_gui.py:193-207`。

### Q18（★★）`ble_scan_wtyi.py` 用 asyncio 而不是线程，为什么？
**答**：bleak 本身是异步 API（基于事件循环 + WinRT/BlueZ 后端），`BleakScanner` 的回调在事件循环里调度，用 `asyncio.run` 最自然，避免为扫描单独开线程。扫描是 I/O 密集（等广播），asyncio 单线程事件循环足够。

### Q19（★★★）Python 读串口是"实时"的吗？延迟来自哪里？
**答**：不是硬实时。延迟来自：① 串口 `timeout` 轮询间隔；② 操作系统调度（Python 线程切换）；③ GUI `after(100ms)` 的消费间隔；④ decode/渲染开销。对日志采集足够，但不适合需要微秒级响应的场景。面试要能区分"软实时日志"和"硬实时控制"。

---

## 四、批处理 / 自动化 / 构建链

### Q20（★）`setlocal` / `endlocal` 干什么？
**答**：让 `PATH` 等环境变量的修改只在脚本作用域生效，脚本结束自动还原，不污染调用它的 shell/后续命令。
**代码**：`build_ac632n_spp_and_le.bat:2,7`。

### Q21（★★）`make ac632n_spp_and_le` 这个 target 是什么？
**答**：杰理 SDK 根 `Makefile` 里定义的构建目标，对应 `apps/spp_and_le`（SPP+BLE 双模透传）应用 + `board/bd19` 平台。产物在 `cpu/bd19/tools/download/data_trans/update.ufw` 和 `jl_isd.fw`。
**代码**：`02_build_ac6321a_spp_and_le.bat:19-20`。

### Q22（★★）为什么 `requirements.md` 要求"编译和烧录必须分开"？
**答**：因为 SDK 的 target 有 post-build 自动拉起下载器（`README_AC6321A_WORKFLOW.txt:21-23`），若板子处于 UBOOT 模式，编译完可能**顺带烧录**，存在误烧风险。所以安全要求是编译归编译、烧录归烧录、未经确认不烧。
**代码**：`requirements.md:99-101`、产品固件 `tools/wtyi/build_only.bat`。

### Q23（★★）PowerShell 改蓝牙名脚本的三重校验是什么？为什么这么设计？
**答**：① `ValidateLength(1,30)` 长度；② 正则白名单 `^[A-Za-z0-9_\-]+$` 防注入/非法字符；③ 替换后 `$newText -eq $text` 判断是否真的匹配到 `.edr_name`，防止"静默没改成"。因为"用户输入 → 修改 C 源码"是高危动作，必须 fail-fast。
**代码**：`set_bt_name_and_build.ps1:2-18`。

### Q24（★★）`errorlevel 1` 和 `$LASTEXITCODE -ne 0` 分别是什么？
**答**：`errorlevel` 是 batch 里判断上一个命令退出码的方式（`if errorlevel 1` = 退出码 ≥1）；`$LASTEXITCODE` 是 PowerShell 里外部程序（`make.exe`）的退出码。两者都是判断"命令是否成功"。
**代码**：`02_build_ac6321a_spp_and_le.bat:10`、`set_bt_name_and_build.ps1:27`。

### Q25（★）为什么用 PyInstaller 打包 exe？
**答**：`requirements.md:93` 要求"尽量生成 Windows exe"，让没有 Python 环境的同事/产线直接双击运行 GUI。`spec` 里 `console=False` 出无黑框窗口，`upx=True` 压缩。
**代码**：`build_uart_gui/JieLi_UART_Print_Receiver.spec`、`dist/*.exe`。

---

## 五、低功耗 / 固件工程

### Q26（★★）你的低功耗方案是什么？不是简单 sleep 吗？
**答**：不是"想睡就睡"。我在 SDK 的 `REGISTER_LP_TARGET` 机制上加了一层**门控**：`wtyi_power_manager.c` 维护 busy flags（BT/SPI/IIC/ADC/OTA），任一 busy 就返回"非空闲"阻止休眠；全 idle 才允许 SDK 进入 sleep/powerdown。核心是回答"板子现在忙不忙"，把休眠决策交给 SDK。
**代码**：`low_power_design.md:19`、`wtyi_power_manager.c`、`wtyi_config.h:33`。

### Q27（★★★）`REGISTER_LP_TARGET` 是什么机制？
**答**：杰理 SDK 的低功耗目标注册机制：每个模块注册一个 `is_idle()` 回调，SDK 在考虑进休眠前逐个询问；所有目标都 idle 才休眠，任一 busy 就保持唤醒。我的 WTYI 门控就是其中一个 `lp_target`。
**代码**：`low_power_design.md:12-16`、`include_lib/driver/cpu/bd19/asm/power_interface.h`（SDK 定义）。

### Q28（★★）为什么说"低功耗不能一上来就实测电流"？
**答**：因为硬件验证前不知道固件能不能正常跑、蓝牙名是否还在、OTA 是否运行。我的文档明确"先确认正常 boot，再测电流"，且**诚实声明未声称实测电流**——避免拿没测过的数误导。
**代码**：`low_power_design.md:154-164`。

### Q29（★★）`sys_timer_add` 是什么？跟裸机定时器什么关系？
**答**：`sys_timer_add` 是杰理 SDK OS 之上的软件定时器接口，用于周期回调（如 5s 心跳、10s 动态改名）。它运行在 SDK 的 OS（FreeRTOS/ucos 抽象）上，不是裸机硬件定时器。
**代码**：`app_main.c:218/237`、`ble_trans.c:1019`。

### Q30（★★★）板级 bring-up 里最容易被坑的是什么？
**答**：CAD 库复用陷阱——原理图/PCB 库里某器件的 LibRef 可能是另一个型号（如 BMI160 vs BOM 实际 QMI8685A、IST8308 vs QMC5883P）。我靠 BOM + 原理图 + 实物照片三方交叉核对，并明确"不得用 BMI160/IST8308 的寄存器代码"。另一个坑是引脚冲突（PA7/PA8 既是产品 SPI 又是 demo IIC 选项 C）。
**代码**：`hardware_map.md:48-70`。

---

## 六、代码质量 / 工程习惯

### Q31（★★）硬编码 `D:\23178\JL` 有什么问题？怎么改？
**答**：换机器/盘符即失效，不可移植。这是"个人工作区备份"的定位决定的（README 已声明本地依赖路径），但工程上应该用环境变量（如 `%JL_ROOT%`）、脚本 `%~dp0` 相对定位、或配置文件。
**代码**：`build_ac632n_spp_and_le.bat:3`、`serial_log_receiver.py:25` 等。

### Q32（★★）你的日志有哪些"可观测性"设计？
**答**：① 统一 `[WTYI]`/`[WTYI_HW]` 前缀，便于关键字过滤/高亮；② 固件侧直接 hex dump 广播包（`trans_adv_data(26)`），把"发出去了什么"打出来；③ 心跳日志确认"固件活着 + UART 通"；④ 落盘保真 + 显示容错分离。
**代码**：`app_main.c:126/234-236`、`ble_trans.c`、`wtyi_log.h`。

### Q33（★★★）如果日志量巨大导致 GUI 卡，你会怎么优化？
**答**：① 环形缓冲，只保留最近 N 行，`delete` 裁头；② 批量合并文本减少 `insert` 次数；③ "自动滚动"开关，用户上翻时暂停；④ 渲染节流（降频 drain）；⑤ 考虑把日志渲染换成 `tk.Text` 之外更高效控件，或写文件为主、界面只做 tail。诚实说明：我当前没做性能实测，这些是分析出来的方向。

### Q34（★★）`--raw` 模式和文本模式的区别？
**答**：`--raw` 直接把原始字节写 `sys.stdout.buffer`（保真、不 decode），文本模式 decode 后 `print`（可读、容错）。raw 适合十六进制分析/抓完整帧，文本适合人眼看。
**代码**：`serial_log_receiver.py:67-72`。

### Q35（★）你为什么在 PPT 自动化里做路径穿越校验？
**答**：`isWithin()` 用 `path.relative` 判断输出目录是否在 workspace 内，拒绝越界写，防止 `--out-dir` 传入 `..` 逃逸。这是处理用户输入路径时的安全意识。
**代码**：`artifact_tool_utils.mjs:43-46`、`inspect_template_deck.win.mjs:109-119`。

---

## 七、开放题

### Q36（★★★）这个项目最能体现你什么能力？
**答**：在**不透明的闭源厂商工具链**之上，独立搭出可复用的开发闭环：能读 SDK 源码定位"改名被覆盖"根因、能写 Python 工具把调试可观测化、能按需求写硬件自检和低功耗门控、能诚实记录"没实测/不确定"的部分。核心是"**从现象反推机制，再把机制固化成工具/文档**"。

### Q37（★★★）如果重新做，你会先改什么？
**答**：① 先把硬编码路径抽象掉；② 串口工具补上自动重连 + 行缓冲（当前最大 gap）；③ 构建/烧录彻底分离并加失败自动判定；④ 固件日志加统一结构化宏（`wtyi_log.h` 已有雏形）。

### Q38（★★）你怎么证明"动态改名 demo 真的跑通了"？
**答**：看日志 `logs/ac63_uart_gui_20260717_102150.log`——有 `WTYI dynamic name switch to: WTYI_BT_TEST` / `..._B` 交替出现，且 `ble name(17)` vs `ble name(19)`、`trans_adv_data(26)` vs `(28)` 随名字长度变化，证明广播包真的被重建了。这是"用日志自证"的好例子。
