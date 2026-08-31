JieLi AC6321A UART print debug setup

1. Firmware UART setting
   TX pin: PA00
   Baudrate: 1000000
   Print content:
     [WTYI] AC63 SPP+LE firmware start
     [WTYI] BT name target: WTYI_BT_TEST
     [WTYI] UART heartbeat ...

2. Wiring
   Board PA00/TX  -> USB-TTL RXD
   Board GND      -> USB-TTL GND

   Do not connect USB-TTL 5V to board IO.
   If the board is powered by USB-C, do not connect USB-TTL VCC.
   For receiving prints only, USB-TTL TXD does not need to connect to the board.

3. PC tools
   List COM ports:
     D:\23178\JL\debug_tools\01_list_com_ports.bat

   Receive COM8 at 1000000 baud:
     D:\23178\JL\debug_tools\02_receive_COM8_1000000.bat

   Choose COM manually:
     D:\23178\JL\debug_tools\03_receive_choose_COM.bat

4. Expected result
   After reset, the receiver should show WTYI startup prints.
   Every 5 seconds, it should show:
     [WTYI] UART heartbeat ...

5. If there is no print
   Check whether USB-TTL appears as COM8 or another COM port.
   Check board TX PA00 is connected to USB-TTL RXD.
   Check common GND.
   Check baudrate is 1000000.
   Check board is running app mode, not BD19 UBOOT mode.
