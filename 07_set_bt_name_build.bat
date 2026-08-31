@echo off
set /p BTNAME=Input Bluetooth name, example WTYI_BT_TEST: 
powershell -NoProfile -ExecutionPolicy Bypass -File "D:\23178\JL\set_bt_name_and_build.ps1" -Name "%BTNAME%"
pause
