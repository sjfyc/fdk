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
	:::: �ų���Ŀ¼��
	if not "%_FullPath%\%_Dummy%" =="%%i" (
		set _FinalPath=%%i
		:::: ȥ��ĩβ��\...
		set _FinalPath=!_FinalPath:~0,-4!		
		:::: ȥ������·��ǰ׺
		set _FinalDirectory=!_FinalPath:%_FullPath%\=!
		echo !_FinalPath!
		echo !_FinalDirectory!		
	)
)
popd

:eof
pause