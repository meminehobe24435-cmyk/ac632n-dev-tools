# 06 · 并发与异常处理（Concurrency & Exceptions）

> 本仓库的"并发"主要在 Python GUI（线程 + 队列）和异步 BLE 扫描（asyncio）；异常处理分散在 Python（try/except 分层）与批处理（errorlevel）。固件侧用的是 SDK 的定时器/低功耗机制，**不是裸机无 RTOS**（这点要讲准确）。

---

## 1. Python 串口线程模型（uart_print_gui.py）

### 1.1 结构：单读线程 + 主线程轮询

```
主线程 (tkinter mainloop)
   └── after(100ms) → _drain_queue()  消费 rx_queue
读线程 (daemon Thread)
   └── _reader_loop() → serial.read() → 写文件 + rx_queue.put()
```

**代码证据**：
- 队列与事件：`uart_print_gui.py:39-41`（`rx_queue`、`stop_event`、`reader_thread`）。
- 启动读线程：`145-147`。
- 读循环：`149-163`。
- 消费队列：`175-191`。

### 1.2 为什么这样设计（面试考点）

1. **tkinter 非线程安全**：跨线程操作 UI 控件是未定义行为，所以读线程只 `put` 队列，UI 操作全部回到主线程的 `_drain_queue`。
2. **`queue.Queue` 是线程安全的生产者-消费者容器**：读线程塞字节，主线程取字节，天然解耦速率差异。
3. **daemon=True**：窗口关闭时若读线程还阻塞在 `read()`，daemon 线程不阻止进程退出。
4. **`stop_event` 协作式停止**：读线程每圈检查 `while not self.stop_event.is_set()`；主线程 `stop_reader()` 先 `set()` 再 `serial.close()`（`193-199`），用关闭串口把阻塞的 `read()` 踢醒。

### 1.3 潜在竞态（诚实指出）

- `self.serial_handle` 在 `_reader_loop`（第 152、168 行）和 `stop_reader`（第 195-197 行）跨线程读写，**未加锁**；实际风险低（Python 对象赋值原子），但严格说是竞态【根据代码推断】。
- `stop_reader()` 里 `serial_handle.close()` 与读线程 `with serial.Serial(...)` 上下文退出可能**双关闭**，靠 `try/except pass` 兜住（第 197-199 行）。
- `self.log_file` 同样跨线程写（读线程写、`stop` 后主线程 `finally` 里可能关闭），靠 `if self.log_file` 判空规避，非原子【根据代码推断】。

---

## 2. 异常处理分层

### 2.1 Python：按异常类型给不同退出码

`serial_log_receiver.py`：
```python
except KeyboardInterrupt:
    print("\nStopped."); return 0
except serial.SerialException as exc:
    print(f"Serial error: {exc}"); return 2
```
- `KeyboardInterrupt`（Ctrl+C）→ 正常退出 0。
- `serial.SerialException`（拔线/占用）→ 明确错误码 2。
- **缺失**：`_reader_loop` 里用的是**裸 `except Exception`**（`uart_print_gui.py:164`），捕获一切并把错误塞进队列显示，比 `serial_log_receiver.py` 更宽容但更不区分错误类型。

`jieli_uart_logger.py` 顶层：
```python
try:
    raise SystemExit(main())
except KeyboardInterrupt:
    print("\nStopped.")
```
- 把 Ctrl+C 与正常 `main()` 返回统一收口。

### 2.2 批处理：errorlevel + pause

`02_build_ac6321a_spp_and_le.bat`：
```bat
make ac632n_spp_and_le
if errorlevel 1 (
  echo BUILD FAILED
  pause
  exit /b 1
)
```
- 用 `errorlevel 1` 判断 `make` 是否失败。
- 失败 `pause`（交互式停住让人看）+ `exit /b 1`（返回码供上层判断）。
- **缺失**：烧录脚本 `03_burn_*.bat` 调 `download.bat` 后**没有** `errorlevel` 判断，只靠注释"Check for download complete"人工确认。

### 2.3 PowerShell：fail-fast + try/finally

`set_bt_name_and_build.ps1`：
```powershell
if ($Name -notmatch '^[A-Za-z0-9_\-]+$') { throw "..." }
...
$newText = $text -replace ...
if ($newText -eq $text) { throw "Could not find .edr_name in $cfg" }
...
try {
    & make.exe ac632n_spp_and_le
    if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE" }
} finally {
    Pop-Location
}
```
- **fail-fast**：任何一步不满足立即 `throw` 停止。
- **`try/finally { Pop-Location }`**：保证 `Push-Location` 切目录后必然还原（防止污染后续命令的工作目录）。

### 2.4 Node.js：try/catch + 非零退出

`inspect_template_deck.win.mjs`：
```js
main().catch((error) => {
  console.error(error.stack || error.message || String(error));
  console.error(usage());
  process.exit(1);
});
```
- 顶层 `catch` 兜底，打印堆栈 + 用法，`process.exit(1)`。

---

## 3. 无 RTOS 的说明（讲准确）

**结论：Python 工具和批处理是无 RTOS 的（普通进程/线程），但固件运行在杰理 SDK 的 OS 上，不是裸机。**

- **PC 侧**：`uart_print_gui.py` 的并发靠 Python 的 `threading` + `queue`，`ble_scan_wtyi.py` 靠 `asyncio` 事件循环——都是用户态并发，无抢占式 RTOS 概念。
- **固件侧**：SDK `include_lib/system/os/` 下同时存在 `FreeRTOS`（`pi32v2/portmacro.h`、`task.h`、`queue.h` 等）和 `ucos_ii.h`，即 SDK 自带 OS 抽象（`os_api.h`），固件代码里用的 `sys_timer_add(...)`（`app_main.c:237`）就是运行在 SDK OS 之上的定时器接口，而非裸机定时器。
- **低功耗侧**：`REGISTER_LP_TARGET` / `is_idle` 是 SDK 的休眠判定机制，与"有没有 RTOS"无关，是电源管理子系统。

**面试口径**：
> 我写的 Python 工具是普通多线程/异步；固件跑在杰理 SDK 自带的 OS（FreeRTOS/ucos 抽象层）上，我用的是 SDK 的 `sys_timer_add` 和低功耗 `REGISTER_LP_TARGET` 机制，没有自己写 RTOS 调度。

**证据**：`sdk/.../include_lib/system/os/FreeRTOS/task.h`、`os_api.h`、`app_main.c:218/237` 的 `sys_timer_add`、`ble_trans.c:1019`。

---

## 4. 并发/异常处理的改进清单（详见 09/13）

| 问题 | 现状 | 改进 |
| --- | --- | --- |
| 读线程异常太笼统 | `except Exception` | 区分 `SerialException` / `UnicodeDecodeError` |
| 无自动重连 | 拔线即 `Serial error` 退出 | 检测断开后重试 `serial.Serial` 循环（`requirements.md:90` 要求） |
| `serial_handle` 跨线程无锁 | 直接赋值 | 用 `threading.Lock` 或只在主线程持有句柄 |
| 烧录无失败判断 | 人肉看输出 | `download.bat` 返回码 + 关键字扫描 |
| 批处理变量未引号 | `%PORT%` | `"%PORT%"` 或延迟扩展 + 转义 |
