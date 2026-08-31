@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0..\.."
pushd "%ROOT%" || exit /b 1

set "REPORT=project_docs\run_all_checks_report.md"
set "MANIFEST=build_logs\latest_firmware_manifest.txt"
set "PY=%ROOT%\.venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"
for /f "usebackq delims=" %%T in (`powershell -NoProfile -Command "Get-Date -Format 'yyyy-MM-dd HH:mm:ss'"`) do set "NOW=%%T"

echo # WT9011DCL-BT50 Run All Checks>"%REPORT%"
echo.>>"%REPORT%"
echo Time: %NOW%>>"%REPORT%"
echo.>>"%REPORT%"
echo ## Safety>>"%REPORT%"
echo.>>"%REPORT%"
echo - This script does not run download.bat.>>"%REPORT%"
echo - This script does not run isd_download.exe.>>"%REPORT%"
echo - This script does not flash hardware.>>"%REPORT%"
echo.>>"%REPORT%"

echo [1/5] Environment check
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0check_environment.ps1" -NoInstall
if errorlevel 1 (
    echo Environment check failed.>>"%REPORT%"
    popd
    exit /b 1
)
echo - Environment check: PASS>>"%REPORT%"

echo [2/5] Build only
call "%~dp0build_only.bat"
if errorlevel 1 (
    echo - Build only: FAIL>>"%REPORT%"
    popd
    exit /b 1
)
echo - Build only: PASS>>"%REPORT%"

echo [3/5] Firmware manifest
if not exist "%MANIFEST%" (
    echo Missing firmware manifest: %MANIFEST%
    echo - Firmware manifest: FAIL>>"%REPORT%"
    popd
    exit /b 1
)
type "%MANIFEST%">>"%REPORT%"
echo.>>"%REPORT%"

for /f "tokens=1,* delims==" %%A in ('findstr /b "APP_BIN=" "%MANIFEST%"') do set "APP_BIN=%%B"
for /f "tokens=1,* delims==" %%A in ('findstr /b "APP_BIN_SHA256=" "%MANIFEST%"') do set "APP_SHA=%%B"

if not exist "!APP_BIN!" (
    echo Firmware missing: !APP_BIN!
    echo - Firmware file: FAIL>>"%REPORT%"
    popd
    exit /b 1
)
echo - Firmware file: PASS `!APP_BIN!`>>"%REPORT%"
echo - Firmware SHA256: `!APP_SHA!`>>"%REPORT%"

echo [4/5] Firmware string check
findstr /m /c:"WT9011DCL-BT50" "!APP_BIN!" >nul
if errorlevel 1 (
    echo - Name string in firmware: FAIL>>"%REPORT%"
    popd
    exit /b 1
)
echo - Name string in firmware: PASS>>"%REPORT%"

findstr /m /c:"WTYI_BOOT" "!APP_BIN!" >nul
if errorlevel 1 (
    echo - WTYI log string in firmware: FAIL>>"%REPORT%"
    popd
    exit /b 1
)
echo - WTYI log string in firmware: PASS>>"%REPORT%"

echo [5/5] Log parser dry run
"%PY%" "tools\wtyi\parse_test_log.py" -o "project_docs\test_log_analysis.md"
if errorlevel 1 (
    echo - Log parser: FAIL>>"%REPORT%"
    popd
    exit /b 1
)
echo - Log parser dry run: PASS>>"%REPORT%"
echo.>>"%REPORT%"
echo ## Next Manual Gate>>"%REPORT%"
echo.>>"%REPORT%"
echo Flashing is still a manual/authorized step. After successful flashing, first verify phone scan name `WT9011DCL-BT50`.>>"%REPORT%"

echo Checks passed. Report: %REPORT%
popd
exit /b 0
