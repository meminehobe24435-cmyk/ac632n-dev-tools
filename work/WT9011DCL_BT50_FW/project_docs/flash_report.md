# WT9011DCL-BT50 Flash Report

Date: 2026-07-23

## Firmware

- Path: `D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools\app.bin`
- SHA256: `B9DD70748C5D1DEECFB22CAECE0E6DF7CA2B9EE1A09C221B674D9A80A6769A90`
- Size: 207248 bytes

## Device Detection

- Programmer/target mode: `BD19 UBOOT1.00 USB Device`
- Drive: `F:\`
- Status: OK

## Command

```bat
cd /d D:\23178\JL\work\WT9011DCL_BT50_FW\cpu\bd19\tools
call download\data_trans\download.bat
```

## Result

- `isd_download.exe` reached target flash.
- Flash ID: `cd7113`
- Flash size: 512K
- Write sector/block completed.
- Download result: PASS
- Full output: `project_docs/flash_output.txt`

## Notes

- The flash script copied the new `app.bin` into `download\data_trans\app.bin`.
- The copied `download\data_trans\app.bin` SHA256 matches the target firmware.
- After flashing, Windows still showed `BD19 UBOOT1.00`; restart the board before phone Bluetooth validation.

## Next Validation

Phone scan must show:

- Classic Bluetooth: `WT9011DCL-BT50`
- BLE advertising: `WT9011DCL-BT50`
