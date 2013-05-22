@echo off
call "%VS90COMNTOOLS%"vsvars32.bat
cl /EHsc /c main.cpp
link /subsystem:console /out:main.exe *.obj
del *.obj
pause