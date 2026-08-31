# WTYI Serial Logger

Windows UART log receiver for WT9011DCL-BT50 bring-up.

## Usage

```bat
start_logger.bat
start_logger.bat --port COM5 --baudrate 1000000
start_logger.bat --keyword WTYI_SPI --keyword WTYI_ERROR
```

Default baudrate is `1000000`. Logs are saved under `tools/serial_logger/logs`.

When no COM port is present, the tool exits with a clear message and does not require hardware.

## Build exe

```bat
build_exe.bat
```
