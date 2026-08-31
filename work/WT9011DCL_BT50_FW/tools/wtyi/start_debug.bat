@echo off
setlocal
call "%~dp0setup_environment.bat"
if errorlevel 1 exit /b %ERRORLEVEL%
call "%~dp0..\serial_logger\start_logger.bat" %*
exit /b %ERRORLEVEL%
