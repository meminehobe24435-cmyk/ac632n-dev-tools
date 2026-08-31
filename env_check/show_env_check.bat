@echo off
title JL AC63 Environment Check
set PATH=D:\23178\JL\sdk\fw-AC63_BT_SDK\tools\utils;C:\JL\pi32\bin;C:\JL\mc\bin;D:\23178\JL\tools\CodeBlocks;%PATH%
echo ===== JL AC63 Environment Check =====
echo.
echo SDK:
echo D:\23178\JL\sdk\fw-AC63_BT_SDK
echo.
echo CodeBlocks:
where codeblocks
echo.
echo Clang:
where clang
clang --version
echo.
echo Make:
where make
make --version
echo.
echo Downloader:
echo D:\23178\JL\sdk\fw-AC63_BT_SDK\cpu\bd19\tools\isd_download.exe
echo.
echo Python:
where python
python --version
echo.
echo Serial ports:
python D:\23178\JL\python_tools\jieli_uart_logger.py --list
echo.
echo Press any key to close.
pause >nul
