# AC63 串口打印接收说明

## 1. 接线

板子串口打印配置：

- UART TX: PA00
- 波特率: 1000000
- RX: 未使用

USB-TTL 接线：

- 板子 PA00/TX -> USB-TTL RX
- 板子 GND -> USB-TTL GND
- 板子 USB-C 继续接电脑供电/下载

注意：板子的 USB-C 不是普通串口日志口。要收启动打印，需要 USB-TTL 模块。

## 2. 查看电脑串口

```bat
D:\23178\JL\debug_tools\receive_print.bat --list
```

插入 USB-TTL 后，列表里一般会出现 CH340、CP210x、USB Serial、USB-SERIAL 等 COM 口。

## 3. 接收打印

把 `COMx` 换成实际串口号：

```bat
D:\23178\JL\debug_tools\receive_print.bat -p COMx
```

默认波特率是 1000000。如果需要手动指定：

```bat
D:\23178\JL\debug_tools\receive_print.bat -p COMx -b 1000000
```

日志会自动保存到：

```text
D:\23178\JL\logs
```

## 4. 本次固件新增打印

启动后应看到类似：

```text
[WTYI] AC63 SPP+LE firmware start
[WTYI] BT name target: WTYI_BT_TEST
[WTYI] UART log: TX=PA00, baud=1000000
```
