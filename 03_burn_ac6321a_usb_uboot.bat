@echo off
setlocal
cd /d D:\23178\JL\sdk\fw-AC63_BT_SDK\cpu\bd19\tools\download\data_trans
echo Put the board into BD19 UBOOT mode if the tool waits.
call download.bat
echo.
echo Burn command finished. Check for "download complete" in the output.
pause
endlocal
