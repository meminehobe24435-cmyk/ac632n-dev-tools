@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0unzip-shim.ps1" -Mode "%~1" -ZipPath "%~2" -EntryName "%~3"
