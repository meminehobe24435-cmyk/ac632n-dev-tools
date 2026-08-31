@echo off
setlocal
set "ROOT=%~dp0..\.."
set "PY=%ROOT%\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"
"%PY%" -m pip install -r "%~dp0requirements.txt"
if errorlevel 1 exit /b %ERRORLEVEL%
"%PY%" -m PyInstaller --onefile --name wtyi_serial_logger "%~dp0serial_logger.py"
exit /b %ERRORLEVEL%
