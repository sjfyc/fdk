@echo off
call "%VS90COMNTOOLS%"vsvars32.bat
cl  /c /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /I lua\include main.c
link /libpath:lua\lib\ /out:main.exe *.obj lua.lib
del *.obj
pause