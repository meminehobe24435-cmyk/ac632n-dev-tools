# 02 · 系统架构（System Architecture）

> 本工作区不是单体程序，而是"厂商 SDK + 自研工具集"的异构集合。下面按"模块关系"和"数据流"两条线讲清楚，Mermaid 图按真实代码绘制。

---

## 1. 工作区全景（模块关系）

```mermaid
flowchart TB
    subgraph SRC["厂商 SDK（第三方，勿深挖）"]
        SDK["sdk/fw-AC63_BT_SDK<br/>apps/spp_and_le<br/>board/bd19"]
        MAKE["make ac632n_spp_and_le<br/>C:\\JL\\pi32 + C:\\JL\\mc"]
    end

    subgraph FW["作者固件改动（apps/spp_and_le）"]
        APP["app_main.c<br/>[WTYI] 启动打印 + 5s UART 心跳"]
        BLE["ble_trans.c<br/>动态改名 demo（10s 切换）"]
        CFG["user_cfg.c<br/>.edr_name + 强制改名"]
        BOARD["board_config.h / board_ac6321a_demo.c<br/>CONFIG_BOARD_AC6321A_DEMO"]
    end

    subgraph PROD["产品固件 work/WT9011DCL_BT50_FW"]
        WTYI["apps/spp_and_le/wtyi/<br/>spi_imu / iic_mag / adc_battery / power_manager"]
        LP["low_power_design.md<br/>REGISTER_LP_TARGET 门控"]
    end

    subgraph TOOLS["自研工具集（Python/bat）"]
        BATS["根目录 01~07_*.bat<br/>构建/烧录/串口/改名入口"]
        PS1["set_bt_name_and_build.ps1<br/>改名 + 构建"]
        RX["serial_log_receiver.py<br/>串口日志落盘"]
        GUI["uart_print_gui.py<br/>tkinter GUI"]
        SCAN["ble_scan_wtyi.py<br/>bleak BLE 扫描"]
        EXE["dist/JieLi_UART_Print_Receiver.exe<br/>(PyInstaller)"]
    end

    subgraph PPT["日报自动化"]
        MJs["ppt_work/**/*.mjs<br/>@oai/artifact-tool"]
    end

    MAKE -->|产物 update.ufw / jl_isd.fw| BURN{"download.bat<br/>USB UBOOT 烧录"}
    BATS --> MAKE
    BATS --> RX
    BATS --> PS1
    PS1 -->|改 .edr_name 后| MAKE
    RX -.->|1 Mbps 串口| HW["AC6321A4 板<br/>PA00/TX → USB-TTL"]
    GUI -.->|1 Mbps 串口| HW
    SCAN -.->|BLE 广播| HW
    BURN -.->|Type-C USB| HW
    WTYI --> SDKC["复用 SDK 构建链"]
    MJs --> PPTX["研发日报项目总结.pptx"]

    style SRC fill:#f5f5f5,stroke:#999
    style FW fill:#fff3e0,stroke:#fb8c00
    style PROD fill:#e8f5e9,stroke:#43a047
    style TOOLS fill:#e3f2fd,stroke:#1e88e5
```

**要点**：
- `sdk/`、`tools/`、`CodeBlocks/`、`python_pkgs/` 是**第三方/依赖**，`debug_tools/`、`python_tools/`、`build_uart_gui/`、`dist/`、`env_check/`、`ppt_work/`、根脚本是**自研/组装**。
- `work/WT9011DCL_BT50_FW` 是**真实产品固件**（含独立 `.git`），`work/gitee_fw-*` 是 SDK 的干净/烧录版副本，`work/gitee_fw-AC63_BT_SDK` 为空检出（`.gitignore` 明确排除）。

---

## 2. 构建 → 烧录 → 调试 闭环

```mermaid
flowchart LR
    A["双击 02_build_*.bat"] --> B["set PATH=<sdk>\\tools\\utils;C:\\JL\\pi32\\bin;C:\\JL\\mc\\bin"]
    B --> C["cd SDK 根目录<br/>make ac632n_spp_and_le"]
    C --> D{"errorlevel == 0 ?"}
    D -- "否" --> E["BUILD FAILED + pause + exit /b 1"]
    D -- "是" --> F["产物：<br/>cpu/bd19/tools/download/data_trans/<br/>update.ufw + jl_isd.fw"]
    F --> G["（可选）post-build 自动拉起下载器"]
    G --> H["03_burn_*.bat → download.bat<br/>板进 BD19 UBOOT 模式 → USB 烧录"]
    H --> I["复位到 app 模式"]
    I --> J["05/06 收 UART 日志<br/>04 GUI / ble_scan 扫广播"]
```

**关键证据**：
- 构建命令与 PATH：`build_ac632n_spp_and_le.bat:3-6`。
- 失败判断：`02_build_ac6321a_spp_and_le.bat:10-15`。
- post-build 自动烧录风险：`README_AC6321A_WORKFLOW.txt:21-23`。
- 烧录命令：`03_burn_ac6321a_usb_uboot.bat:3-5`（`call download.bat`）。
- 日志/扫描闭环：`05_list_serial_ports.bat`、`06_receive_uart_log_COMx.bat`、`debug_tools/ble_scan_wtyi.py`。

---

## 3. UART 日志链路（核心数据流）

```mermaid
flowchart LR
    FW["固件 printf()<br/>app_main.c / ble_trans.c"] -->|"UART0 TX = PA00<br/>baud = 1000000"| TTL["USB-TTL 模块<br/>(CH340 / CP210x / FT232)"]
    TTL -->|"USB CDC 虚拟 COM 口<br/>COM8 等"| PY["pyserial<br/>serial.Serial(port, 1000000, timeout=0.2)"]
    PY -->|"ser.read(4096) 字节流"| SPLIT{"分两路"}
    SPLIT -->|"log_file.write + flush<br/>（'ab' 追加二进制）"| LOG["logs/ac63_uart_*.log"]
    SPLIT -->|"decode utf-8 errors=replace"| GUIQ["queue.Queue[bytes|str]"]
    GUIQ -->|"after(100ms) _drain_queue"| TEXT["tkinter Text<br/>WTYI 高亮 / ERROR 红"]
```

**证据**：
- 固件侧：`app_main.c:126` `printf("[WTYI] UART heartbeat %lu, ...\r\n", ++cnt)`，`sys_timer_add(NULL, wtyi_uart_heartbeat, 5000)`（第 237 行）。
- 接线/波特率：`README_UART_LOG.md:5-17`、`serial_log_receiver.py:23` `DEFAULT_BAUD = 1_000_000`。
- 接收实现：`serial_log_receiver.py:56-72`（`serial.Serial` + `log_path.open("ab")`）。
- GUI 队列：`uart_print_gui.py:39` `self.rx_queue`、`145-147` 读线程、`175-191` `_drain_queue`。

---

## 4. 蓝牙名修改链路（名字是怎么生效的）

```mermaid
flowchart TB
    CODE[".edr_name = \"WTYI_BT_TEST\"<br/>user_cfg.c BT_CONFIG bt_cfg"] --> PARSE["cfg_file_parse()"]
    PARSE --> READ["syscfg_read(CFG_BT_NAME, ...) 读配置区旧名"]
    READ --> FORCE["bt_set_local_name(\"WTYI_BT_TEST\", ...)<br/>强制覆盖（解决旧名覆盖）"]
    FORCE --> GET["bt_get_local_name() 返回 edr_name"]
    GET --> BLE["ble_comm_set_config_name(name, add_ble_ext_name)<br/>经典名 + '(BLE)' 后缀"]
    BLE --> ADV["trans_adv_config_set() 重建 adv/rsp 广播包"]
    ADV --> ON["ble_gatt_server_adv_enable(1) 重新广播"]
    ON --> SCAN["电脑/手机 BLE 扫描可见"]

    style FORCE fill:#fff3e0,stroke:#fb8c00
```

**证据**：
- `.edr_name`：`user_cfg.c:44`。
- 强制改名：`user_cfg.c:217-218`（`bt_set_local_name("WTYI_BT_TEST", strlen(...))` + `log_info("WTYI force bt name:%s\n", ...)`）。
- 动态改名完整流程：`ble_trans.c:799-825` `wtyi_dynamic_ble_name_switch()`（`ble_gatt_server_adv_enable(0)` → `bt_set_local_name` → `ble_comm_set_config_name` → `trans_adv_config_set` → `adv_enable(1)`）。
- 实机日志验证：`logs/ac63_uart_gui_20260717_102150.log` 中 `WTYI dynamic name switch to: WTYI_BT_TEST_B` + `trans_adv_data(28)`。

---

## 5. 产品固件低功耗门控（PROD 层）

```mermaid
flowchart TB
    SDKLP["SDK：TCFG_LOWPOWER_LOWPOWER_SEL = SLEEP_EN<br/>PWR_DCDC15 / OSC_TYPE_LRC"] --> REG["REGISTER_LP_TARGET(...)<br/>各模块注册 is_idle()"]
    WTYIPM["wtyi_power_manager.c<br/>WTYI_LOW_POWER_ENABLE + busy flags"] --> REG
    FLAGS["busy flags:<br/>BT / SPI / IIC / ADC / OTA"] --> JUDGE{"所有模块 idle ?"}
    JUDGE -- "任一 busy" --> AWAKE["SDK 保持唤醒"]
    JUDGE -- "全 idle" --> SLEEP["SDK 进入 sleep/powerdown<br/>sleep_enter/exit_callback"]
```

**证据**：`low_power_design.md:9-19`、`:38-75`（busy flags 规则）、`wtyi_config.h:33` `WTYI_LOW_POWER_ENABLE 1`、`wtyi_power_manager.c`（busy 位 + `is_idle` 语义）。

---

## 6. 架构层面的一句话总结

- **分层**：第三方 SDK（只读）→ 作者固件改动（C）→ 自研工具集（Python/bat）→ 产品固件（C + 文档）。
- **解耦点**：固件通过"UART 文本日志 + BLE 广播"对外暴露可观测性，工具集只依赖这两个标准接口，不依赖厂商私有协议。
- **数据边界**：串口读线程只负责"搬字节 + 写文件"，UI 线程只负责"渲染"，通过 `queue.Queue` 解耦（生产者-消费者）。
