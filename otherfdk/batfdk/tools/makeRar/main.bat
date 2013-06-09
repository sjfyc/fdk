@echo off
internal\rar.exe a -r -m5 -sfx internal *
internal\rar.exe c -zinternal\config.txt internal.exe
pause