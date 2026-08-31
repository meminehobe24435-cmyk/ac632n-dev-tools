@echo off
setlocal EnableExtensions EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
for %%I in ("%SCRIPT_DIR%..\..") do set REPO_ROOT=%%~fI
set MANIFEST=%REPO_ROOT%\build_logs\latest_firmware_manifest.txt

if not exist "%MANIFEST%" (
    echo Missing manifest: %MANIFEST%
    echo Run tools\wtyi\build_only.bat first.
    exit /b 1
)

for /f "usebackq tokens=1,* delims==" %%A in ("%MANIFEST%") do (
    if "%%A"=="APP_BIN" set APP_BIN=%%B
    if "%%A"=="APP_BIN_SHA256" set APP_SHA256=%%B
)

if not defined APP_BIN (
    echo APP_BIN not found in manifest.
    exit /b 1
)
if not exist "%APP_BIN%" (
    echo Firmware not found: %APP_BIN%
    exit /b 1
)

set FW_TO_HASH=%APP_BIN%
for /f %%H in ('powershell -NoProfile -Command "$p=$env:FW_TO_HASH; (Get-FileHash -Algorithm SHA256 -LiteralPath $p).Hash"') do set CURRENT_SHA256=%%H
if /I not "%CURRENT_SHA256%"=="%APP_SHA256%" (
    echo Firmware hash mismatch.
    echo Manifest: %APP_SHA256%
    echo Current : %CURRENT_SHA256%
    echo Refusing to flash.
    exit /b 1
)

echo Firmware ready for flashing:
echo   %APP_BIN%
echo   SHA256=%CURRENT_SHA256%
echo.
echo This script will call the official bd19 data_trans download script after confirmation.
echo It may run isd_download.exe and burn the connected board.
echo.
set /p CONFIRM=Type FLASH-WT9011 to continue: 
if not "%CONFIRM%"=="FLASH-WT9011" (
    echo Flash cancelled.
    exit /b 2
)

cd /d "%REPO_ROOT%\cpu\bd19\tools\download\data_trans"
call download.bat
exit /b %ERRORLEVEL%
