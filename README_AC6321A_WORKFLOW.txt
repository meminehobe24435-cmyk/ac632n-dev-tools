AC6321A JieLi workflow

Working directory:
D:\23178\JL

Main SDK:
D:\23178\JL\sdk\fw-AC63_BT_SDK

Current app:
apps\spp_and_le\board\bd19\AC632N_spp_and_le.cbp

Board config:
apps\spp_and_le\board\bd19\board_config.h
Current selected board: CONFIG_BOARD_AC6321A_DEMO

Common actions:
1. Open Code::Blocks:
   D:\23178\JL\01_open_codeblocks_project.bat

2. Build firmware:
   D:\23178\JL\02_build_ac6321a_spp_and_le.bat
   Note: this SDK target runs post-build download. If the board is in BD19 UBOOT mode,
   the build step may also burn firmware.

3. USB UBOOT burn:
   D:\23178\JL\03_burn_ac6321a_usb_uboot.bat
   Board should appear as BD19 UBOOT before or during burning.

4. Open GUI downloader:
   D:\23178\JL\04_open_isd_download_gui.bat

5. List serial ports:
   D:\23178\JL\05_list_serial_ports.bat

6. Receive UART log:
   D:\23178\JL\06_receive_uart_log_COMx.bat
   Default baudrate: 1000000

7. Change Bluetooth name and build:
   D:\23178\JL\07_set_bt_name_build.bat

Important:
- USB UBOOT mode is for burning. It may not create a COM port.
- UART printing needs USB-TTL connected to the configured TX/GND pins.
- Existing JieLi compiler paths are C:\JL\pi32 and C:\JL\mc.
