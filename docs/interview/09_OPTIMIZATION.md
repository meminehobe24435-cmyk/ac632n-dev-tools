# 09 · 优化方向（Optimization）

> 两栏对照：现状 → 优化。所有优化项都能在代码里找到"现状"依据，不编造已完成的优化。

---

## 1. UART 日志接收

| 现状（证据） | 优化方向 |
| --- | --- |
| 轮询式 `ser.read(4096)` + `timeout=0.2`（`serial_log_receiver.py:56-60`） | 改用 `serial` 的线程 + `in_waiting`，或 `asyncio` + 串口，减少空转 syscall |
| 落盘与显示分离已做（`"ab"` + `errors=replace`） | 增加**行缓冲**：按 `\n` 切行再写，避免半包碎片 |
| 无自动重连（`requirements.md:90` 要求但未实现） | 外层 `while True` 捕获 `SerialException` 后重试 `serial.Serial`，带退避 |
| 无关键字过滤（`requirements.md:91` 要求，GUI 只有高亮） | 增加正则过滤 + 高亮/导出 |
| 无时间戳（PC 端不打印接收时间） | 每行加 PC 接收时间戳（可选开关） |
| 默认日志目录硬编码 `D:\23178\JL\logs`（`serial_log_receiver.py:25`） | 参数默认值改为相对路径/环境变量 |

## 2. UART GUI（uart_print_gui.py）

| 现状（证据） | 优化方向 |
| --- | --- |
| `tk.Text` 全量累积，无行数上限（`_drain_queue` 每 100ms 全量 insert） | 环形缓冲，只保留最近 N 行（如 5000 行），`delete("1.0", ...)` 裁头 |
| 每次 drain 都 `see(tk.END)` 强制滚到底 | "跟随滚动"开关，用户上翻时暂停自动滚动 |
| 高流量下 UI 可能卡（未实测） | 批量合并文本再 insert；日志渲染节流（如 200ms 一次） |
| `serial_handle` 跨线程读写无锁（第 152/168/195 行） | 句柄只由主线程持有，读线程通过事件通信；或加 `threading.Lock` |
| 端口启发式匹配不到时回退 `ports[0]`（第 30 行） | 匹配不到提示用户手动选，不静默选第一个 |
| 裸 `except Exception`（第 164 行） | 细分 `SerialException` / `ValueError` / 其他 |

## 3. 构建/烧录脚本

| 现状（证据） | 优化方向 |
| --- | --- |
| 硬编码 `D:\23178\JL`（所有 bat/ps1） | 脚本顶部 `%~dp0` 定位自身目录，或环境变量 `%JL_ROOT%` |
| `03_burn_*.bat` 无 `errorlevel` 判断（第 5-6 行仅注释） | 捕获 `download.bat` 返回码 + 扫描输出关键字 `download complete` |
| `%PORT%`/`%BTNAME%` 未加引号（`06/07_*.bat`） | `"%PORT%"` + 输入校验（改名已用 ps1 校验，端口仍裸） |
| `pause` 阻塞自动化（多个 bat） | 加 `--no-pause` 开关或 `if defined CI` 跳过 |
| 构建与烧录未彻底分离（post-build 自动烧录副作用） | 按 `requirements.md:99-101` 用 build-only target（产品固件已有 `tools/wtyi/build_only.bat`） |

## 4. 蓝牙改名

| 现状（证据） | 优化方向 |
| --- | --- |
| SDK 版硬编码 `"WTYI_BT_TEST"` 两次（`user_cfg.c:44` + `:217`） | 收敛为宏（产品固件已做：`app_config.h:21` `WTYI_BT_CLASSIC_NAME`） |
| `-replace` 只匹配第一处 `.edr_name`（`set_bt_name_and_build.ps1:15`） | `-replace` 全量 + 命中次数校验 |
| `Set-Content -Encoding UTF8` 可能改源文件编码（GBK 注释） | 读原始编码、写回同编码；或只替换字节区间 |
| 经典名与 BLE 名耦合（BLE 名 = 经典名 + `(BLE)`） | 独立配置 `WTYI_BT_CLASSIC_NAME` / `WTYI_BT_BLE_NAME`（产品固件已拆） |

## 5. BLE 扫描

| 现状（证据） | 优化方向 |
| --- | --- |
| `sys.path.insert(0, r"D:\23178\JL\python_pkgs")`（第 6 行） | 相对路径/虚拟环境/`--pkgs` 参数 |
| 扫描时长 30s 写死（第 28 行） | `--duration` 参数 |
| `seen` 按地址去重，同名不同地址会重复 | 按 name 聚合 + 地址列表 |
| RSSI 只打印不记录波动 | 记录多次 RSSI 均值/方差，辅助测距粗判 |

## 6. 固件侧（app_main.c / ble_trans.c）

| 现状（证据） | 优化方向 |
| --- | --- |
| `WTYI_HW_TEST_ENABLE` 恒为 1（`app_main.c:129`） | 量产关闭，或宏接 board cfg |
| 心跳 `cnt` 是 `static u32`（`app_main.c:124`） | 无实际影响，仅日志回绕；可 `u64` 或取模 |
| 动态改名未处理"广播发送中"竞态（`ble_trans.c:820-824`） | 关广播后加状态确认/延时，再重建再开 |
| `sys_timer_add` 周期 5s/10s 写死 | 接配置文件，便于调参 |

## 7. 可观测性

| 现状（证据） | 优化方向 |
| --- | --- |
| `[WTYI]`/`[WTYI_HW]` 前缀日志已统一（`app_main.c`、`wtyi_log.h`） | 增加日志级别开关（`WTYI_LOG` 已有空实现宏可关） |
| 日志无结构化字段（纯文本） | 关键事件加机器可解析字段（如 `EV=heartbeat,CNT=123`）供脚本分析 |
| `parse_test_log.py` 已有（产品 `tools/wtyi/`） | 扩展为通用日志解析/统计工具 |

---

## 优先级建议（面试口径）

> 如果继续做，我会按"**影响正确性 > 影响可移植性 > 影响体验**"排序：
> P0：串口自动重连 + 行缓冲（直接影响调试效率与正确性）；
> P1：去掉硬编码路径（`D:\23178\JL` → 环境变量/相对路径）；
> P2：GUI 环形缓冲 + 滚动开关（体验）；
> P3：烧录失败自动判定 + 构建/烧录彻底分离（安全）。
