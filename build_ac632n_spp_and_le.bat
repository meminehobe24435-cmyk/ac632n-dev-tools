@echo off
setlocal
set SDK_DIR=D:\23178\JL\sdk\fw-AC63_BT_SDK
set PATH=%SDK_DIR%\tools\utils;C:\JL\pi32\bin;C:\JL\mc\bin;%PATH%
cd /d "%SDK_DIR%"
make ac632n_spp_and_le
endlocal
