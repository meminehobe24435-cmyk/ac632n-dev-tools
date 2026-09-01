# 04 · 核心模块（Core Modules）

> 逐模块讲"是什么 / 做了什么 / 关键实现 / 面试怎么讲"。模块与 `03_CODE_WALKTHROUGH.md` 互补：这里按"功能域"纵向拆，那里按"文件"横向拆。

---

## 1. UART 日志接收模块

**是什么**：从固件 `printf` 到 PC 落盘/显示的完整接收链路。

**做了什么**：
- 枚举 COM 口（device/description/hwid）。
- 以 1 Mbps 打开串口，循环读 4096 字节。
- 追加写日志文件（时间戳文件名 `ac63_uart_YYYYMMDD_HHMMSS.log`）。
- 支持 `--raw`（原始字节）与文本（UTF-8 `errors=replace`）两种输出。

**关键实现**：
```python
# serial_log_receiver.py:56-66
with serial.Serial(port, baudrate=baud, timeout=0.2) as ser, log_path.open("ab") as log_file:
    while True:
        data = ser.read(4096)
        if not data: continue
        log_file.write(data); log_file.flush()
```

**面试怎么讲**：
- 强调"落盘用 `ab` 二进制 + `flush()`"，保证异常退出也不丢已收数据。
- 强调 `timeout=0.2` 是"读超时"，不是"波特率"——面试官常混淆。
- 诚实说明：这是**轮询式**读（简单可靠），不是 pyserial 的 `in_waiting`/线程事件驱动；高流量下靠 `read(4096)` 批量拉取降低 syscall 频率。

**证据文件**：`debug_tools/serial_log_receiver.py`、`python_tools/jieli_uart_logger.py`。

---

## 2. UART 打印 GUI 模块

**是什么**：`tkinter` 写的图形化串口接收器，打包成 exe 给非 Python 环境用。

**做了什么**：
- COM 口下拉（含 `Refresh`）、波特率输入、Start/Stop/Clear。
- 状态栏实时显示"Connected COM8 @ 1000000"和日志路径。
- 日志文本框 + `WTYI` 绿色高亮 / `[ERROR]` 红色。
- 读线程与 UI 线程用 `queue.Queue` 解耦。

**关键实现**：
```python
# uart_print_gui.py:145-147 / 175-191
self.reader_thread = threading.Thread(target=self._reader_loop, args=(port, baud), daemon=True)
...
def _drain_queue(self):
    while True:
        try: item = self.rx_queue.get_nowait()
        except queue.Empty: break
        tag = "match" if "WTYI" in text else ("error" if "[ERROR]" in text else None)
        self.output.insert(tk.END, text, tag); self.output.see(tk.END)
    self.after(100, self._drain_queue)
```

**面试怎么讲**：
- 核心考点：**为什么不在读线程里操作 UI** → tkinter 非线程安全，跨线程 `insert` 未定义行为。
- 核心考点：**daemon 线程 + `stop_event`** → 关窗口时 `on_close()` 先 `stop_reader()`（`stop_event.set()` + `serial.close()`）再 `destroy()`，避免串口句柄泄漏/线程悬挂。

**证据文件**：`debug_tools/uart_print_gui.py`、`build_uart_gui/*.spec`、`dist/JieLi_UART_Print_Receiver.exe`。

---

## 3. BLE 扫描模块

**是什么**：`bleak` 异步扫描，验证固件改名后广播是否可见。

**做了什么**：
- 注册 `BleakScanner(cb)` 回调，收集所有"有名字"的广播设备。
- 按 `KEYWORDS = ("WTYI", "JL", "AC63", "BT_TEST")` 匹配，命中打印 `[MATCH] RSSI=... 地址 名字`。
- 30 秒后汇总去重、按名字排序、返回匹配数。

**关键实现**：
```python
# ble_scan_wtyi.py:17-23
def cb(device, adv):
    name = adv.local_name or device.name or ""
    if not name: return
    seen[device.address] = (name, adv.rssi, time.strftime("%H:%M:%S"))
    if any(k in name.upper() for k in KEYWORDS):
        print(f"[MATCH] ... RSSI={adv.rssi} {device.address} {name}", flush=True)
```

**面试怎么讲**：
- `adv.local_name or device.name` 优先级：广播包里的 Local Name 比系统缓存的 `device.name` 更新鲜。
- `return 0 if matches else 1`：让扫描结果可直接进 CI/脚本判断。
- 依赖离线缓存 `python_pkgs/`（bleak + bleak_winrt），说明"离线开发环境"意识。

**证据文件**：`debug_tools/ble_scan_wtyi.py`、`python_pkgs/bleak*`。

---

## 4. 构建 / 烧录脚本模块

**是什么**：把 `make` + 杰理工具链 + USB UBOOT 烧录封装成一键 bat。

**做了什么**：
- `build_ac632n_spp_and_le.bat` / `02_build_*.bat`：设 PATH → `make ac632n_spp_and_le`。
- `03_burn_*.bat`：进入 data_trans 目录 `call download.bat`。
- `04_open_isd_download_gui.bat` / `open_ac632n_downloader.bat`：`start isd_download.exe` 打开下载 GUI。
- `01_open_codeblocks_project.bat`：打开 Code::Blocks 工程。

**面试怎么讲**：
- 强调**构建与烧录要分离**（`requirements.md:99-101` 的安全要求），以及 SDK target 的 post-build 自动烧录副作用（`README_AC6321A_WORKFLOW.txt:21-23`）。
- 强调产物路径 `cpu/bd19/tools/download/data_trans/update.ufw` + `jl_isd.fw`。

**证据文件**：根目录 `0x_*.bat`、`README_AC6321A_WORKFLOW.txt`。

---

## 5. 蓝牙改名模块

**是什么**：改 `.edr_name`（经典蓝牙名）并重新构建的一键工具。

**做了什么**：
- PowerShell 正则替换 `user_cfg.c` 的 `.edr_name`。
- 参数三重校验（长度 + 白名单 + 替换命中）。
- 改完调 `make ac632n_spp_and_le`，失败即 throw。

**关键实现**（见 `set_bt_name_and_build.ps1:15`）：
```powershell
$newText = $text -replace '\.edr_name\s*=\s*"[^"]*"', ".edr_name        = `"$Name`""
```

**面试怎么讲**：
- 这是"**改代码里的名字 ≠ 改生效的名字**"的典型场景：配置区旧名会覆盖，所以固件里还要在 `cfg_file_parse()` 末尾强制 `bt_set_local_name(...)`（`user_cfg.c:217-218`）。
- 经典蓝牙名（`.edr_name`）与 BLE 广播名（`+ "(BLE)"`）是两个概念，后者由 `ble_comm_set_config_name(name, add_ble_ext_name)` 生成。

**证据文件**：`set_bt_name_and_build.ps1`、`07_set_bt_name_build.bat`、`user_cfg.c`、`ble_trans.c`。

---

## 6. 日报自动化模块

**是什么**：把 4 份日报（2026/07/09~07/13）程序化渲染成 10 页 PPT。

**做了什么**：
- `build_daily_report_deck.mjs`：用 `@oai/artifact-tool` 的 `Presentation` API 逐页排版。
- 导出 `.pptx` + 每页 PNG/layout JSON 做 QA。
- `inspect_template_deck.win.mjs`：逆向已有 PPT 模板（渲染 PNG、收集字体、提取媒体）。

**面试怎么讲**：
- 强调"固定结构文本 → 参数化布局函数 → 程序化渲染"的思路，把重复劳动自动化。
- 强调路径安全（`isWithin` 防越界写）等工程细节（`artifact_tool_utils.mjs:43-46`）。

**证据文件**：`ppt_work/**/*.mjs`、`ppt_daily_summary_source.txt`。

---

## 7. 产品固件驱动模块（work/WT9011DCL_BT50_FW）

**是什么**：真实产品 WT9011DCL-BT50 的板级驱动与低功耗框架。

**做了什么**：
- `wtyi_spi_imu.c`：SPI 驱动六轴 IMU（QMI8685A，CS=PB0/SCK=PA7/MOSI=PA8/MISO=PA6/INT=PA3）。
- `wtyi_iic_mag.c`：IIC 驱动磁力计（QMC5883P，SCL=USB1DP/SDA=USB1DM）。
- `wtyi_adc_battery.c`：ADC 采样电池（PA1，VBAT = 采样 × 4，3M/1M 分压）。
- `wtyi_power_manager.c`：低功耗门控（busy flags + `is_idle`）。
- 新建板级 `board_wt9011dcl_bt50.c/_cfg.h`。
- 工具脚本 `tools/wtyi/*`（build_only / check_environment / flash_confirmed / parse_test_log / run_all_checks）。

**面试怎么讲**：
- 这是从"demo 板"到"产品板"的**板级 bring-up**：自己从原理图/BOM 反推引脚、写驱动、写硬件自检、写低功耗门控。
- 强调"诚实工程"：`low_power_design.md:164` 明确"不声称实测电流"，`hardware_map.md` 明确标注 CAD 库复用陷阱（BMI160/IST8308 vs 实际 BOM 器件）。

**证据文件**：`work/WT9011DCL_BT50_FW/apps/spp_and_le/wtyi/*`、`project_docs/*.md`。

---

## 模块关系一览

| 模块 | 输入 | 输出 | 依赖 |
| --- | --- | --- | --- |
| UART 接收 | COM 口 + 波特率 | 日志文件 / stdout | pyserial |
| UART GUI | COM 口 + 波特率 + 按钮 | 窗口 + 日志文件 | tkinter + threading + queue |
| BLE 扫描 | 空气广播 | [MATCH] 列表 + 退出码 | bleak |
| 构建/烧录 | bat 双击 | update.ufw / jl_isd.fw + 烧录 | make + pi32 + isd_download |
| 改名 | 新名字字符串 | 改后 user_cfg.c + 重新构建 | PowerShell + make |
| 日报自动化 | 日报文本 | .pptx + PNG | node + @oai/artifact-tool |
| 产品驱动 | 硬件总线 | 驱动 + 自检 + 低功耗门控 | 杰理 SDK API |
