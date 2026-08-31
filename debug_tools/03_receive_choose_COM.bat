@echo off
setlocal
set /p PORT=Input COM port, example COM8: 
python D:\23178\JL\debug_tools\serial_log_receiver.py -p %PORT% -b 1000000
pause
endlocal
