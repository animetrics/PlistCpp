@echo off
if "%1" == "" goto USAGE 

cd testing
..\Windows\%1\runTests
cd ..

goto END

:USAGE
echo.
echo runTests.bat [config type Release/Debug]
echo.

:END

