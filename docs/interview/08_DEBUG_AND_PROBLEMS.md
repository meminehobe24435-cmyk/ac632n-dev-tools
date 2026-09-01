# 08 · 调试与问题排查（Debug & Problems）

> 从脚本注释 / TODO / logs / commit / 日报反推 3~8 个真实问题。有直接证据的标"证据"，证据不足的标【典型风险分析】。**严禁编造**具体数值或 Bug 故事。

---

## 问题 1：1000000 高波特率下 UART 日志出现乱码/碎片

**现象**：日志里夹着大量 `IPP`、`PIPP`、`P` 等短 ASCII 碎片（`logs/ac63_uart_gui_20260717_102150.log` 全篇可见）。

**证据**：
- 日志头部/尾部均可见 `IPP`/`PIPP` 与正常 `[Info]: [SPP_AND_LE]...` 交错。
- 接收端用 `errors="replace"` 解码（`uart_print_gui.py:183`、`serial_log_receiver.py:71`），说明作者已意识到"会有非法 UTF-8"。

**分析**【根据日志推断】：
1. 高波特率（1 Mbps）下，多路 printf 源（SDK 日志 + `[WTYI]` 心跳）交错输出，某条打印可能被切成非完整片段。
2. `read(4096)` 一次可能读到"半条"日志，decode 时前后拼接产生碎片。
3. USB-TTL 芯片时钟分频误差在 1 Mbps 下可能放大，产生字节错位。

**面试口径**：
> 这不是协议字段，而是高波特率 + 多源 printf 下的接收伪影。我通过"落盘存原始字节、显示才 decode"保证文件不丢真数据，碎片只在显示层出现。根治方向是降低波特率、用硬件流控、或固件侧给 printf 加互斥/整包缓冲。

---

## 问题 2：改了 `.edr_name` 但蓝牙名不生效（配置区旧名覆盖）

**现象**：改代码里的 `.edr_name` 后，扫描仍是旧名。

**证据**：
- `user_cfg.c:217-218` 在 `cfg_file_parse()` 末尾**强制** `bt_set_local_name("WTYI_BT_TEST", ...)`，注释/日志 `WTYI force bt name`。
- 日报 07/13："手机端名称不变化的原因主要与配置区旧名称覆盖...有关"（`ppt_daily_summary_source.txt`）。
- 产品固件演进：`work/.../user_cfg.c:223-227` 用 `syscfg_write(CFG_BT_NAME, WTYI_BT_CLASSIC_NAME, ...)` 把新名**写回配置区**再强制，比 SDK 版更彻底。

**根因**：`syscfg_read(CFG_BT_NAME, ...)` 从配置区读出旧名并 `memcpy` 覆盖 `bt_cfg.edr_name`，把代码默认值顶掉；所以要么在读取后强制覆盖，要么把新名写回配置区持久化。

**面试口径**：
> 这是"代码默认值 vs 配置区持久值"的优先级问题——配置区（flash 里）的旧名会在运行时覆盖代码默认值。我的解法是"读配置后强制 set 一次"，产品固件里进一步"写回配置区持久化"。

---

## 问题 3：板子停在 UBOOT 模式导致扫不到 BLE / 收不到日志

**现象**：烧录完忘了复位，手机/电脑扫不到广播。

**证据**：
- `UART_DEBUG_README.txt:39` "Check board is running app mode, not BD19 UBOOT mode."
- `build_daily_report_deck.mjs:145` "如果板子停留在 BD19 UBOOT 下载模式，手机和电脑都扫不到 BLE 广播；复位到 JLDebug_V0.1 后才能验证运行状态。"
- 日报 07/10 卡点："需要确认板卡 USB-C 是否支持数据通信、是否需要进入下载模式"。

**根因**：UBOOT 是烧录引导态，不跑 app、不广播、不打运行日志；必须复位切到 app 模式。

---

## 问题 4：Type-C 口不能直接收 UART 日志（硬件接线陷阱）

**现象**：以为插 Type-C 就能收串口日志，结果收不到。

**证据**：
- `README_UART_LOG.md:17` "板子的 USB-C 不是普通串口日志口。要收启动打印，需要 USB-TTL 模块。"
- `hardware_map.md:42` "UART log | not confirmed on PCB | SDK demo uses PA0 | Do not assume PA0 has an accessible test point."
- 日报 07/13 卡点："PA00 未引出到 P1，直接焊芯片脚风险较高；建议评估 USB CDC 虚拟串口"。

**根因**：产品板 UART 日志脚 PA0 未引出，需外接 USB-TTL 焊测试点，或评估 USB CDC 虚拟串口方案。

---

## 问题 5：串口占用 / 拔线后程序行为

**现象/风险**：COM 口被别的软件占用时 `serial.Serial` 抛异常；拔线后读线程报错。

**证据**：
- `serial_log_receiver.py:76-78` 捕获 `serial.SerialException` 返回码 2（说明作者遇到过占用/打不开）。
- `uart_print_gui.py:164` 裸 `except Exception` 把错误塞进队列显示 `[ERROR]`。

**分析**【典型风险分析】：
- `requirements.md:90` 明确要求"串口断开后自动重连"，但 `serial_log_receiver.py` 和 `uart_print_gui.py` **都未实现自动重连**——拔线即进入 Error 态。这是"需求 vs 实现"的明确 gap。

---

## 问题 6：GUI 在高日志量下可能卡顿

**现象/风险**：1 Mbps 持续输出时，`tk.Text` 全量累积渲染可能掉帧。

**证据**【典型风险分析，未实测性能】：
- `_drain_queue` 每次 `insert` + `see` 全量追加（`uart_print_gui.py:188-189`），无行数上限/环形缓冲。
- 日志文件实测可达 20 MB（`logs/ac63_uart_gui_20260716_202656.log` = 20,247,415 字节），说明日志量巨大。

**面试口径**：
> 我没有做性能实测，但从实现看，GUI 把全部历史日志都堆在 `tk.Text` 里，长时间高流量会内存/渲染压力增大。改进方向是"环形缓冲 + 只渲染最近 N 行 + 可暂停刷新"。

---

## 问题 7：烧录成功与否靠人肉确认

**现象/风险**：`03_burn_ac6321a_usb_uboot.bat` 调 `download.bat` 后无自动成功判定。

**证据**：
- `03_burn_ac6321a_usb_uboot.bat:6` 注释 "Check for download complete in the output." + `pause`。
- 对比 `02_build_*.bat` 有 `if errorlevel 1` 判断——烧录脚本明显更弱。

**分析**【根据代码推断】：烧录结果依赖人肉看 `download complete`，易漏判。

---

## 问题 8：硬编码绝对路径 `<workspace>` 的可移植性

**现象/风险**：几乎所有脚本都硬编码 `<workspace>`。

**证据**：
- `build_ac632n_spp_and_le.bat:3`、`serial_log_receiver.py:25` `DEFAULT_LOG_DIR`、`uart_print_gui.py:17` `LOG_DIR`、`ble_scan_wtyi.py:6` `sys.path.insert(0, r"<workspace>\python_pkgs")`、`set_bt_name_and_build.ps1:7`、`env_check/show_env_check.bat:3`。

**分析**：换机器/换盘符即失效，是"个人工作区"定位的必然代价（README 已声明"本地依赖路径"），但面试时应主动承认并给出改进（环境变量/相对路径/配置文件）。详见 `13_CODE_REVIEW.md`。

---

## 问题清单汇总

| # | 问题 | 证据强度 | 状态 |
| --- | --- | --- | --- |
| 1 | 高波特率日志碎片/乱码 | 日志实测 + 代码 | 已识别，未根治 |
| 2 | 配置区旧名覆盖 | 源码 + 日报 | 已解决（强制覆盖/写回） |
| 3 | UBOOT 模式扫不到 BLE | 文档 + 日报 | 已解决（复位切 app） |
| 4 | Type-C 非串口日志口 | 文档 + 日报 | 已识别，USB CDC 待评估 |
| 5 | 无自动重连 | 需求 vs 实现 gap | 未实现 |
| 6 | GUI 高流量卡顿 | 典型风险分析 | 未实测 |
| 7 | 烧录无人肉判断 | 代码对比 | 未实现 |
| 8 | 硬编码路径 | 全代码 | 已知，README 声明 |
