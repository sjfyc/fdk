@echo off

echo --------GO "%~nx0"

:::: ----------------------------------------------------------------
::::  Copy all the files from source directory to destination directory.
::::  If target directory does not exist, then create one.
::::   
::::  @SRCDIR:  The source directory.
::::  @DSTDIR: The destination directory.
::::  只要某路径出现在Main.bat.exclude中即可被排除
:::: ----------------------------------------------------------------

:::: --------CONFIG
set SRCDIR="C:\Documents and Settings\Default User\「开始」菜单\程序"
set DSTDIR="我的程序"

:::: --------RUN
xcopy %SRCDIR%\* %DSTDIR% /r/i/c/k/h/e/q/y/exclude:%~nx0.exclude

echo --------DONE "%~nx0"

pause