@echo off & setlocal enabledelayedexpansion
set _Directory=%1
if not exist "%_Directory%" (
	echo Directory '%_Directory%' does not exist.
	goto eof
)
pushd %_Directory%
set _FullPath=%CD%
popd

pushd %_Directory%
for /r . %%i in (*) do (	
	:::: ȥ������·��ǰ׺
	set _FinalPath=%%i
	:::: ȥ������·��ǰ׺
	set _FinalFile=!_FinalPath:%_FullPath%\=!
	echo !_FinalPath!
	echo !_FinalFile!
)
popd

:eof
pause