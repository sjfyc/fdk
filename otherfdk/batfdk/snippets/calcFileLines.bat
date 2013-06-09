:: 计算文件中的非空行数
@echo off 
setlocal enabledelayedexpansion
set _Count=0
for /f %%i in ('type %1') do (
  set /a _Count=!_Count!+1
)
echo %_Count%
setlocal disabledelayedexpansion
pause