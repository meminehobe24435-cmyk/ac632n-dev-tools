@echo off
setlocal
set "ROOT=%~dp0..\.."
set "PY=%ROOT%\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"
"%PY%" "%~dp0serial_logger.py" %*
exit /b %ERRORLEVEL%
