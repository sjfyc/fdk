@echo off

echo --------GO "%~nx0"

:::: ----------------------------------------------------------------
::::  Copy all the files from source directory to destination directory.
::::  If target directory does not exist, then create one.
::::   
::::  @SRCDIR:  The source directory.
::::  @DSTDIR: The destination directory.
::::  ֻҪĳ·��������Main.bat.exclude�м��ɱ��ų�
:::: ----------------------------------------------------------------

:::: --------CONFIG
set SRCDIR="C:\Documents and Settings\Default User\����ʼ���˵�\����"
set DSTDIR="�ҵĳ���"

:::: --------RUN
xcopy %SRCDIR%\* %DSTDIR% /r/i/c/k/h/e/q/y/exclude:%~nx0.exclude

echo --------DONE "%~nx0"

pause