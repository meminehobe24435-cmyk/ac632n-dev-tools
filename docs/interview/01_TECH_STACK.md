# 01 · 技术栈（Tech Stack）

> 约定：每个技术点按「是什么 → 为什么用 → 怎么用 → 代码在哪里 → 面试官怎么追问」五段展开。术语保留英文。

---

## 1. Windows 批处理（batch）构建自动化

**是什么**：`.bat` 脚本，用 `setlocal` 临时改 `PATH`、`cd /d` 切目录、调用 `make`/`python`/`start`。
**为什么用**：杰理官方工具链是 Windows 优先（编译链在 `C:\JL\pi32`、`C:\JL\mc`，下载工具 `isd_download.exe`），且团队/同事习惯双击运行；比要求所有人手敲 `make` 命令更防错。
**怎么用**：
```bat
@echo off
setlocal
set SDK_DIR=D:\23178\JL\sdk\fw-AC63_BT_SDK
set PATH=%SDK_DIR%\tools\utils;C:\JL\pi32\bin;C:\JL\mc\bin;%PATH%
cd /d "%SDK_DIR%"
make ac632n_spp_and_le
endlocal
```
**代码在哪里**：`build_ac632n_spp_and_le.bat`（7 行精简版）、`02_build_ac6321a_spp_and_le.bat`（22 行，含 `errorlevel 1` 失败判断 + `pause`）。
**面试官怎么追问**：
- "为什么用 `setlocal`？" → 让 `PATH` 修改只在脚本作用域生效，不污染调用 shell。
- "`errorlevel` 判断了什么？" → `make` 的退出码；非 0 打印 `BUILD FAILED` 并 `exit /b 1`。
- "`%PORT%` 这种变量注入有什么坑？" → 用户输入含空格/特殊字符会截断，见 `06_receive_uart_log_COMx.bat` 用 `set /p` 读入未做引号防护的潜在问题（详见 `13_CODE_REVIEW.md`）。

---

## 2. PowerShell 脚本（带参数校验的改名构建）

**是什么**：`set_bt_name_and_build.ps1`，用 `param(...)` 声明强类型参数 + `ValidateLength` + 正则白名单校验，然后正则替换 `user_cfg.c` 里的 `.edr_name`，再调 `make`。
**为什么用**：改蓝牙名这个动作"参数是用户给的字符串"，必须校验（否则会把非法字符写进 C 源文件，或误替换）。PowerShell 的 `param`/`ValidateLength`/`throw` 比 batch 的 `set /p` 健壮得多。
**怎么用**：
```powershell
powershell -File set_bt_name_and_build.ps1 -Name MY_JL_BT
```
**代码在哪里**：`set_bt_name_and_build.ps1`（38 行，核心是第 15 行 `-replace '\.edr_name\s*=\s*"[^"]*"', ...`）。
**面试官怎么追问**：
- "为什么还要再校验一次 `-notmatch`？" → `ValidateLength` 只查长度，正则白名单 `^[A-Za-z0-9_\-]+$` 才是防注入/防非法字符的关键（第 10-12 行）。
- "如果 `-replace` 没匹配到会怎样？" → `$newText -eq $text` 则 `throw "Could not find .edr_name"`（第 16-18 行），避免"静默失败"。
- "PowerShell 写文件的编码问题？" → 第 20 行 `-Encoding UTF8`；对含中文的源文件有 BOM/无 BOM 风险，属【根据代码推断】的可移植性隐患。

---

## 3. Python + pyserial（串口日志接收）

**是什么**：用 `pyserial`（`serial.Serial` + `serial.tools.list_ports`）打开 COM 口、读字节流、落盘/打印。
**为什么用**：跨平台、几行代码就能枚举 COM 口和读串口；配合 argparse 很容易做成"CLI + 可脚本化"。
**怎么用**：
```python
with serial.Serial(port, baudrate=baud, timeout=0.2) as ser, log_path.open("ab") as log_file:
    while True:
        data = ser.read(4096)
        if not data: continue
        log_file.write(data); log_file.flush()
```
**代码在哪里**：`debug_tools/serial_log_receiver.py`（97 行）、`python_tools/jieli_uart_logger.py`（74 行，更早版本）。
**面试官怎么追问**：
- "`timeout=0.2` 和 `read(4096)` 是什么关系？" → 阻塞读最多等 0.2s，最多返回 4096 字节；`read` 返回空再 `continue`，形成非忙等循环（这是 CPU 友好但非"事件驱动"的简单实现）。
- "`errors="replace"` 干什么？" → 高波特率下字节可能被半切，UTF-8 解码遇到非法序列不崩，用 � 替换（`jieli_uart_logger.py:63`）。
- "1000000 波特率为什么可能丢帧？" → 见 `08_DEBUG_AND_PROBLEMS.md`，核心是 USB-TTL 芯片时钟分频误差 + `read` 不及时导致内核缓冲溢出。

---

## 4. Python + tkinter（GUI）

**是什么**：用标准库 `tkinter`（`tk.Tk` + `ttk`）做一个"UART 打印接收器"窗口，含 COM 口下拉、波特率输入、Start/Stop/Clear、日志文本框 + 关键字高亮。
**为什么用**：`tkinter` 是 Python 自带 GUI，零额外依赖、可直接被 PyInstaller 打包；比引入 PyQt 更轻。
**怎么用**：主线程跑 `mainloop()`，用 `queue.Queue` + `threading.Thread` 把串口读线程的数据转交回 UI 线程（`after(100, self._drain_queue)` 轮询队列）。
**代码在哪里**：`debug_tools/uart_print_gui.py`（222 行），类 `UartPrintGui(tk.Tk)`。
**面试官怎么追问**：
- "为什么不能直接在读线程里 `insert` 文本框？" → tkinter 不是线程安全的，跨线程操作 UI 会崩/未定义行为；正确做法是队列 + 主线程轮询（`_drain_queue`，第 175-191 行）。
- "`pick_default_port()` 的启发式？" → 第 24-30 行按 `CH340/CP210/USB-SERIAL` 关键字优先匹配，匹配不到回退第一个 COM 口。
- "高波特率下 GUI 会卡吗？" → `insert` + `see` 每次 100ms 批量 drain，仍可能因 1 Mbps 流量 + 日志全量渲染导致 UI 掉帧，见 `08/09` 文档。

---

## 5. PyInstaller（打包 exe）

**是什么**：把 `uart_print_gui.py` 打成无控制台窗口（`console=False`）的 Windows exe。
**为什么用**：`requirements.md:93` 明确要求"尽量生成 Windows exe"，让非 Python 环境的同事/产线直接双击运行。
**怎么用**：`pyinstaller JieLi_UART_Print_Receiver.spec`；spec 里 `name='JieLi_UART_Print_Receiver'`、`console=False`、`upx=True`。
**代码在哪里**：`build_uart_gui/JieLi_UART_Print_Receiver.spec`（38 行），产物 `dist/JieLi_UART_Print_Receiver.exe`（8.9 MB）。
**面试官怎么追问**：
- "`console=False` 的坑？" → 无控制台时 Python 的 `print`/未捕获异常会进 `windowed traceback` 弹窗；`disable_windowed_traceback=False` 保留了弹窗。
- "为什么要 `hiddenimports=[]`？" → `pyserial`/`serial.tools.list_ports` 通常被自动探测，但若打包后缺模块要在这里补；当前 spec 未填，属【待本人确认】是否在真实打包时补过。
- "为什么 exe 这么大？" → 打包进了 Python 解释器 + tkinter + pyserial + bleak 等依赖。

---

## 6. BLE 扫描（Python + bleak）

**是什么**：`bleak`（`BleakScanner`）异步扫描周围 BLE 广播，按关键字 `WTYI/JL/AC63/BT_TEST` 匹配并打印 RSSI、地址、时间戳。
**为什么用**：验证"固件改完蓝牙名后，电脑到底能不能扫到"——这是改名验证的最后一环（`ble_scan_wtyi.py`）。
**怎么用**：`asyncio.run(main())`，`BleakScanner(cb)` 注册回调，`scanner.start()` → `sleep(30)` → `scanner.stop()`，最后汇总去重设备。
**代码在哪里**：`debug_tools/ble_scan_wtyi.py`（45 行）；依赖包缓存在 `python_pkgs/`（`bleak-0.22.3`、`bleak_winrt-1.2.0`）。
**面试官怎么追问**：
- "为什么用 `sys.path.insert(0, r"D:\23178\JL\python_pkgs")`？" → 把离线依赖目录插到 import 路径，脱离 pip 环境也能跑（但硬编码绝对路径，见 `13_CODE_REVIEW.md`）。
- "RSSI 是什么？能拿它测距吗？" → RSSI 是接收信号强度（dBm，负值越大越近），仅作粗粒度参考，易受遮挡/多径影响，不能当精确测距。
- "回调线程安全吗？" → bleak 回调在事件循环里串行触发，`seen` 字典单线程访问，安全。

---

## 7. 杰理 AC632N 工具链（pi32 / ISD / UBOOT）

**是什么**：厂商编译链 `C:\JL\pi32`（含 `clang`、`make`）、`C:\JL\mc`；下载工具 `isd_download.exe` + `download.bat`；烧录走 USB UBOOT（BD19 UBOOT 模式）。
**为什么用**：AC632N 是杰理（JieLi）的蓝牙 SoC，只能用它闭源的编译链和下载协议。
**怎么用**：`make ac632n_spp_and_le`（target 名定义在 SDK 根 `Makefile`）→ 产物 `cpu/bd19/tools/download/data_trans/update.ufw`、`jl_isd.fw` → `download.bat`（USB UBOOT）烧录。
**代码在哪里**：`build_ac632n_spp_and_le.bat`、`03_burn_ac6321a_usb_uboot.bat`、`04_open_isd_download_gui.bat`、`env_check/show_env_check.bat`（第 12-20 行验证 `clang`/`make`/`python` 版本）。
**面试官怎么追问**：
- "`pi32` 是什么？" → 杰理 pi32 系列的交叉编译链（llvm/clang 前端 + pi32 后端），目标是 bd19 内核。
- "UBOOT 模式和 app 模式区别？" → UBOOT 是烧录引导模式（不一定枚举出 COM 口），app 是正常运行模式（会广播 BLE、打 UART 日志）——`README_AC6321A_WORKFLOW.txt:43` 与日报都强调"板子停在 UBOOT 模式就扫不到 BLE"。
- "post-build 自动烧录的坑？" → `README_AC6321A_WORKFLOW.txt:21-23` 注明该 target 构建后会自动拉起下载器；这也是 `requirements.md:99-101` 要求"编译和烧录必须分开、编译不能自动调 download.bat"的原因。

---

## 8. BLE / SPP 概念（固件侧背景）

**是什么**：
- **BLE（Bluetooth Low Energy）**：低功耗广播/连接，广播包（ADV_IND）里带 Flags、16-bit UUID、Complete Local Name 等 AD 结构。
- **SPP（Serial Port Profile）**：经典蓝牙（BR/EDR）的串口透传 profile；本项目 `spp_and_le` 应用是"SPP + BLE 双模数据透传"。
**为什么用**：产品既需要低功耗 BLE 广播被发现，又可能用 SPP 做经典蓝牙串口数据通道。
**怎么用/代码在哪里**：
- `apps/spp_and_le/examples/trans_data/ble_trans.c`：BLE 透传 + 广播配置 + 动态改名 demo。
- `apps/spp_and_le/examples/trans_data/app_spp_and_le.c`：SPP/BLE 应用入口。
- 广播包原始字节可见于日志 `trans_adv_data(26): 02 01 06 03 03 30 AF 12 09 ...`（Flags=06、16-bit UUID=0x30AF、Complete Local Name）。
**面试官怎么追问**：
- "广播包最多多少字节？" → 31 字节（ADV_RSP_PACKET_MAX，`ble_trans.c:119-120` 定义 buffer 为 31）。
- "`(BLE)` 后缀哪来的？" → `ble_comm_set_config_name(name, add_ble_ext_name)` 里 `add_ble_ext_name=1` 会在广播名后拼 `(BLE)`（`ble_trans.c:812-816`、`822`）。
- "ADV_IND 是什么？" → 可连接、可扫描的非定向广播类型（`ble_trans.c:787` `adv_type = ADV_IND`）。

---

## 9. 钉钉日报 PPT 自动化（Node.js / mjs）

**是什么**：用 Node.js ESM（`.mjs`）+ `@oai/artifact-tool` 的 `Presentation` API，把 4 份日报（2026/07/09~07/13）程序化生成 10 页 PPT，导出 `.pptx` 和每页 PNG/layout JSON。
**为什么用**：日报是固定结构（姓名/日期/昨日闭环/今日任务/卡点/明日计划），机械重复，适合用脚本一次性"模板化"渲染，且能产出 QA 用预览图。
**怎么用**：`node build_daily_report_deck.mjs` → `Presentation.create({slideSize:{1280x720}})` → 逐页 `addText/addBox/bulletList/metric/card` → `PresentationFile.exportPptx(p)` → `save(OUT)`。
**代码在哪里**：
- `ppt_work/daily_report_summary/tmp/build_daily_report_deck.mjs`（252 行，主渲染脚本）。
- `ppt_work/daily_report_template_simple/tmp/inspect_template_deck.win.mjs`（217 行，模板逆向/检查脚本）。
- `ppt_work/daily_report_template_simple/tmp/container_tools/artifact_tool_utils.mjs`（473 行，工具函数库：路径安全校验、字体收集、Lucide 图标渲染）。
- 日报原文：`ppt_daily_summary_source.txt`。
**面试官怎么追问**：
- "`type: module` 的 package.json 作用？" → 让 `.mjs` 走 ESM（`import`/`export`）。
- "`isWithin()` 在干什么？" → `artifact_tool_utils.mjs:43-46` 用 `path.relative` 防路径穿越，拒绝把输出写到 workspace 之外——这是有安全意识的写法。
- "为什么有 `inspect_template_deck`？" → 把已有 PPT 模板反解成 slide/layout JSON + 字体清单，做"模板理解"。

---

## 10. 技术栈速查表

| 层 | 技术 | 关键文件 |
| --- | --- | --- |
| 构建自动化 | Windows batch / PowerShell | 根目录 `0x_*.bat`、`set_bt_name_and_build.ps1` |
| 串口/工具 | Python 3 + pyserial + tkinter + bleak | `debug_tools/*.py`、`python_tools/*.py` |
| 打包 | PyInstaller | `build_uart_gui/*.spec`、`dist/*.exe` |
| 固件 | C（杰理 SDK，pi32/clang 工具链） | `sdk/fw-AC63_BT_SDK/apps/spp_and_le/*` |
| 烧录 | ISD / USB UBOOT | `03_burn_*.bat`、`download.bat` |
| PPT 自动化 | Node.js ESM + `@oai/artifact-tool` | `ppt_work/**/*.mjs` |
