@echo off
call "%VS90COMNTOOLS%"vsvars32.bat

cl /EHsc /MD /c WinMain.c
link /subsystem:windows /out:WinMain.exe *.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
del *.obj
pause