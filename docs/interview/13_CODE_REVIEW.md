# 13 · 代码质量审计（Code Review）

> 以"reviewer"视角审计自研代码。分"优点"和"问题"，问题按严重度分级（🔴 高 / 🟡 中 / 🟢 低），每条给证据。这是一份"自己审自己"的诚实清单，面试时主动用它展示工程判断力。

---

## 1. 优点（值得肯定）

| 优点 | 证据 |
| --- | --- |
| 落盘与显示分离，保真 + 容错 | `serial_log_receiver.py:56`（`"ab"`）+ `:71`（`errors=replace`） |
| 参数可脚本化（argparse 全覆盖） | `serial_log_receiver.py:83-87` 全部有默认值且可覆盖 |
| 失败退出码语义清晰（0/1/2） | `serial_log_receiver.py:73-78`、`ble_scan_wtyi.py:41` |
| GUI 用队列解耦线程与 UI | `uart_print_gui.py:39/145-147/175-191` |
| 改名脚本三重校验 + fail-fast | `set_bt_name_and_build.ps1:2-18` |
| try/finally 保证目录还原 | `set_bt_name_and_build.ps1:24-32` |
| 统一日志宏 + 前缀 | `wtyi_log.h`、`app_main.c:126` |
| 诚实工程文档（不编数、标风险） | `low_power_design.md:164`、`hardware_map.md:48-70` |
| 路径穿越防护（Node 侧） | `artifact_tool_utils.mjs:43-46` |
| 构建失败有 `errorlevel` 判断 | `02_build_ac6321a_spp_and_le.bat:10-15` |

---

## 2. 问题清单

### 🔴 高：硬编码绝对路径 `D:\23178\JL`（可移植性）
- **证据**：`build_ac632n_spp_and_le.bat:3`、`set_bt_name_and_build.ps1:7`、`serial_log_receiver.py:25`、`uart_print_gui.py:17`、`ble_scan_wtyi.py:6`、`env_check/show_env_check.bat:3`、`01_open_codeblocks_project.bat:2`。
- **影响**：换机器/盘符/用户即失效。
- **缓解**：README 已声明"本地依赖路径"（`README.md:31`），但作为"可复现工程"仍不合格。
- **建议**：脚本顶部 `%~dp0` 定位，或环境变量 `%JL_ROOT%`，或 `config.ini`。

### 🔴 高：串口无自动重连（需求 gap）
- **证据**：`requirements.md:90` 明确要求"串口断开后自动重连"，但 `serial_log_receiver.py` 和 `uart_print_gui.py` 都是拔线即 Error。
- **影响**：调试中板子复位/拔插频繁，工具易中断。
- **建议**：外层重试循环 + 指数退避 + 状态提示。

### 🟡 中：烧录无自动成功判定
- **证据**：`03_burn_ac6321a_usb_uboot.bat:6` 仅注释 "Check for download complete"，无 `errorlevel`/关键字判定（对比 `02_build_*.bat` 有判定）。
- **建议**：捕获 `download.bat` 返回码 + 扫描输出关键字。

### 🟡 中：读线程异常笼统（`except Exception`）
- **证据**：`uart_print_gui.py:164` 裸 `except Exception`。
- **影响**：把 `SerialException`（可恢复的占用/拔线）和编程错误混为一谈。
- **建议**：区分 `serial.SerialException` / `ValueError` / 其他。

### 🟡 中：`serial_handle` 跨线程无锁
- **证据**：`uart_print_gui.py:152/168`（读线程写）、`:195-197`（主线程读+close）。
- **影响**：理论竞态（`close` 与 `read` 并发），实际靠 `try/except pass` 兜住。
- **建议**：句柄只在主线程持有，读线程通过队列/事件通信。

### 🟡 中：批处理变量未加引号
- **证据**：`06_receive_uart_log_COMx.bat:3` `-p %PORT%`、`07_set_bt_name_build.bat:3` `-Name "%BTNAME%"`（后者已加引号，前者未加）。
- **影响**：输入含空格/特殊字符会截断或注入。
- **建议**：`"%PORT%"` + 输入校验（改名已用 ps1 校验，端口仍裸）。

### 🟢 低：GUI 无行数上限（性能隐患，未实测）
- **证据**：`uart_print_gui.py:188-189` 全量 `insert` + `see`，日志文件可达 20 MB（`logs/ac63_uart_gui_20260716_202656.log`）。
- **建议**：环形缓冲 + 自动滚动开关。

### 🟢 低：`.edr_name` 硬编码重复
- **证据**：`user_cfg.c:44` + `:217` 两处 `"WTYI_BT_TEST"`。
- **建议**：收敛为宏（产品固件 `app_config.h:21` 已示范正确做法）。

### 🟢 低：PowerShell 写文件编码风险
- **证据**：`set_bt_name_and_build.ps1:20` `Set-Content -Encoding UTF8`，而 `user_cfg.c` 含 GBK 中文注释。
- **影响**：可能改写源文件编码/引入 BOM【根据代码推断】。
- **建议**：读原始编码、写回同编码。

### 🟢 低：`pick_default_port` 静默回退第一个口
- **证据**：`uart_print_gui.py:30` `return ports[0].device if ports else ""`。
- **影响**：启发式匹配不到时可能连错口。
- **建议**：匹配不到提示用户手动选。

---

## 3. 审计结论

- **整体**：自研工具集"功能闭环完整、工程意识在线（保真/容错/退出码/路径防护/诚实文档）"，但存在两类系统性问题——**硬编码路径**（可移植性）和**无自动重连**（需求未闭环）。
- **面试一句话**：
> 如果要给这个仓库打 code review，我会先提两个 🔴：硬编码 `D:\23178\JL` 和无自动重连。前者是"个人工作区"定位的代价，后者是我明确写进需求却还没实现的功能，属于"诚实承认的 gap"。

---

## 4. 严重度定义

- 🔴 高：影响正确性/可移植性/需求闭环，必须改。
- 🟡 中：影响健壮性/安全，建议改。
- 🟢 低：体验/可维护性/潜在隐患，可后置。
