@echo off & setlocal enabledelayedexpansion
set _Directory=%1
if not exist "%_Directory%" (
	echo Directory '%_Directory%' does not exist.
	goto eof
)
pushd %_Directory%
set _FullPath=%CD%
popd
set _Dummy=...

pushd %_Directory%
for /r . %%i in (%_Dummy%) do (
	:::: 排除掉目录根
	if not "%_FullPath%\%_Dummy%" =="%%i" (
		set _FinalPath=%%i
		:::: 去除末尾的\...
		set _FinalPath=!_FinalPath:~0,-4!		
		:::: 去除完整路径前缀
		set _FinalDirectory=!_FinalPath:%_FullPath%\=!
		echo !_FinalPath!
		echo !_FinalDirectory!		
	)
)
popd

:eof
pause