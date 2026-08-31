@echo off
setlocal
set SDK_DIR=D:\23178\JL\sdk\fw-AC63_BT_SDK
set PATH=%SDK_DIR%\tools\utils;C:\JL\pi32\bin;C:\JL\mc\bin;%PATH%
cd /d "%SDK_DIR%"
echo NOTE: This SDK target runs its post-build downloader after compiling.
echo If the board is in BD19 UBOOT mode, it may burn automatically.
echo.
make ac632n_spp_and_le
if errorlevel 1 (
  echo.
  echo BUILD FAILED
  pause
  exit /b 1
)
echo.
echo BUILD OK
echo Firmware output:
echo %SDK_DIR%\cpu\bd19\tools\download\data_trans\update.ufw
echo %SDK_DIR%\cpu\bd19\tools\download\data_trans\jl_isd.fw
pause
endlocal
