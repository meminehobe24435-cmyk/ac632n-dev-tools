# 12 · GitHub 演示路线（5 分钟 Code Navigation）

> 面试现场 5 分钟演示路线，按"认知递进"安排点击顺序。每一步给出：看什么 + 讲一句话 + 停留时间。

---

## 路线总览

| 分钟 | 目标 | 关键文件 |
| --- | --- | --- |
| 0:00-0:40 | 建立定位 | `README.md` → `README_AC6321A_WORKFLOW.txt` |
| 0:40-1:40 | 最重头的工具 | `debug_tools/uart_print_gui.py` |
| 1:40-2:20 | 工具链+落盘 | `debug_tools/serial_log_receiver.py` + `ble_scan_wtyi.py` |
| 2:20-3:00 | 改名脚本 | `set_bt_name_and_build.ps1` |
| 3:00-4:00 | 固件改动 | `ble_trans.c`（搜 WTYI）+ `user_cfg.c`（搜 edr_name） |
| 4:00-4:40 | 产品/低功耗 | `project_docs/low_power_design.md` |
| 4:40-5:00 | 自证 + 交付 | `logs/*.log` + `dist/*.exe` |

---

## 逐步详解

### ① README.md + README_AC6321A_WORKFLOW.txt（40 秒）
- **看**：目录结构树 + 免责声明 + 工作流 7 步。
- **讲**："这是工作区镜像，不是单一产品工程；工作流文件把本地依赖路径和常见动作都固化了。"
- **技巧**：主动说明"里面 SDK 是第三方闭源的，我只讲自己改的部分"，避免被追问厂商代码。

### ② uart_print_gui.py（60 秒，全场的核心）
- **看**：滚动到 `_reader_loop`（149-173 行）和 `_drain_queue`（175-191 行）。
- **讲**："读线程往 queue 塞字节，主线程每 100ms 消费渲染，因为 tkinter 非线程安全；`timeout=0.2` 是读超时，`errors=replace` 容错。"
- **技巧**：这是最能体现"并发 + 嵌入式协作"的地方，务必讲透。

### ③ serial_log_receiver.py + ble_scan_wtyi.py（40 秒）
- **看**：`DEFAULT_BAUD = 1_000_000`（23 行）、`log_path.open("ab")`（56 行）；ble_scan 的 `KEYWORDS` + `return 0 if matches else 1`。
- **讲**："默认 1 Mbps、落盘保真、BLE 扫描带退出码可进 CI。"

### ④ set_bt_name_and_build.ps1（40 秒）
- **看**：`ValidateLength`（3 行）、正则白名单（10-12 行）、`-replace`（15 行）。
- **讲**："用户输入→改源码→构建，三层校验防注入防静默失败。"

### ⑤ ble_trans.c（搜 `WTYI`）+ user_cfg.c（搜 `edr_name`）（60 秒）
- **看**：`wtyi_dynamic_ble_name_switch()`（799-825 行）、`user_cfg.c` 的 `.edr_name`（44 行）和强制改名（217-218 行）。
- **讲**："改名的坑：配置区旧名会覆盖代码默认值，所以要强制 set；动态改名要关广播→改名→重建包→开广播。"
- **技巧**：这是全场最有"深度"的一段，能引到 BLE 广播包 31 字节、`(BLE)` 后缀等追问。

### ⑥ project_docs/low_power_design.md（40 秒）
- **看**：`REGISTER_LP_TARGET` 调用链（9-19 行）、busy flags（64-75 行）、诚实声明（164 行）。
- **讲**："低功耗是门控不是强制睡；文档明确不声称实测电流。"

### ⑦ logs/*.log + dist/*.exe（20 秒收尾）
- **看**：`ac63_uart_gui_20260717_102150.log` 里的 `WTYI dynamic name switch to: WTYI_BT_TEST_B` + hex 广播包；`dist/JieLi_UART_Print_Receiver.exe`（8.9 MB）。
- **讲**："这是实机跑通的日志证据，这是交付的 exe。"

---

## 演示前自查清单

- [ ] 能在 10 秒内定位 `uart_print_gui.py` 的 `_drain_queue`。
- [ ] 能背出 `DEFAULT_BAUD = 1_000_000`、`timeout=0.2`、`read(4096)` 三个参数。
- [ ] 能一句话讲清 `wtyi_dynamic_ble_name_switch` 的五步时序。
- [ ] 能说出配置区旧名覆盖的根因和两种解法。
- [ ] 能主动承认三个短板（硬编码路径 / 无自动重连 / GUI 未实测性能）。

## 容易被追问的"当场翻车点"（提前准备）

1. "`timeout=0.2` 是什么单位？" → 秒，读超时。
2. "为什么 `ab` 不是 `a`？" → 二进制追加，保真，避免文本模式换行转换/编码破坏。
3. "`make ac632n_spp_and_le` 在哪里定义？" → SDK 根 `Makefile`（第三方，未深挖，坦承）。
4. "RSSI 能测距吗？" → 不能精确测距，只粗判远近。
5. "低功耗实测电流多少？" → 没实测，文档已声明，不编数。
