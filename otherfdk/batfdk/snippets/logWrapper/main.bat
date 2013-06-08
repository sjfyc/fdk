@echo off
set LOGFILE="%~nx0-%DATE:~0,4%-%DATE:~5,2%-%DATE:~8,2%-%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%.log"
cd. > %LOGFILE%

echo --------GO "%~nx0"

:::: ----------------------------------------------------------------
::::  A log wrapper to the orignal batch file.
:::: ----------------------------------------------------------------

:::: --------CONFIG

:::: --------RUN
call "%~n0_.bat" >> %LOGFILE%

echo --------DONE "%~nx0"

pause