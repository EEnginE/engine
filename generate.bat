@echo off

GOTO MAIN

:: **************************
:: * Sets LINK and CYG_PATH *
:: **************************
:CHECK_ARCH

IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO 32BIT)

:64BIT
echo Detected 64 bit Windows
set CYGWIN=C:\cygwin64\bin\mintty.exe
GOTO:EOF

:32BIT
echo   Detected 32 bit Windows
echo.
SET CYGWIN=C:\cygwin\bin\mintty.exe
GOTO:EOF
:: ***************************
:: * END Function CHECK_ARCH *
:: ***************************


:MAIN

CALL:CHECK_ARCH

echo Cygwin is: %CYGWIN%

IF NOT EXIST "%CYGWIN%" GOTO END

echo Running generate script
"%CYGWIN%" /bin/bash -l -c "T='%CD%'; T=\"${T//\\\\/\\/}\"; T=\"/cygdrive/${T:0:1}${T/#*:/}/generate.sh\"; $T %*"
echo DONE
echo.

:END
