@echo off

set DLOAD_SCRIPT=download.vbs
set SETUP=cygwinSetup.exe
set SETUP_DIR=windowsSetup
set CYGWIN_PACKAGES=wget,curl,ncurses
set CYGWIN_CATEGORIES=Base
set MIRROR=http://cygwin.mirrors.pair.com

set START_PATH=%CD%


GOTO MAIN


:: *****************************
:: * BEGIN Function CHECK_ARCH *
:: * Sets LINK and CYG_PATH    *
:: *****************************
:CHECK_ARCH

IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO 32BIT)

:64BIT
echo   Detected 64 bit Windows
echo.
set LINK=https://cygwin.com/setup-x86_64.exe
set CYG_PATH=C:\cygwin64
GOTO:EOF

:32BIT
echo   Detected 32 bit Windows
echo.
SET LINK=https://cygwin.com/setup-x86.exe
SET CYG_PATH=C:\cygwin
GOTO:EOF
:: ***************************
:: * END Function CHECK_ARCH *
:: ***************************

:: ********************************
:: * BEGIN Function CREATE_SCRIPT *
:: ********************************
:CREATE_SCRIPT
echo   Creating Download script %DLOAD_SCRIPT%

echo Option Explicit                                                      >  %DLOAD_SCRIPT%
echo Dim args, http, fileSystem, adoStream, url, target, status           >> %DLOAD_SCRIPT%
echo.                                                                     >> %DLOAD_SCRIPT%
echo Set args = Wscript.Arguments                                         >> %DLOAD_SCRIPT%
echo Set http = CreateObject("WinHttp.WinHttpRequest.5.1")                >> %DLOAD_SCRIPT%
echo url = args(0)                                                        >> %DLOAD_SCRIPT%
echo target = args(1)                                                     >> %DLOAD_SCRIPT%
echo WScript.Echo "  Getting '" ^& target ^& "' from '" ^& url ^& "'..."  >> %DLOAD_SCRIPT%
echo.                                                                     >> %DLOAD_SCRIPT%
echo http.Open "GET", url, False                                          >> %DLOAD_SCRIPT%
echo http.Send                                                            >> %DLOAD_SCRIPT%
echo status = http.Status                                                 >> %DLOAD_SCRIPT%
echo.                                                                     >> %DLOAD_SCRIPT%
echo If status ^<^> 200 Then                                              >> %DLOAD_SCRIPT%
echo 	WScript.Echo "FAILED to download: HTTP Status " ^& status           >> %DLOAD_SCRIPT%
echo 	WScript.Quit 1                                                      >> %DLOAD_SCRIPT%
echo End If                                                               >> %DLOAD_SCRIPT%
echo.                                                                     >> %DLOAD_SCRIPT%
echo Set adoStream = CreateObject("ADODB.Stream")                         >> %DLOAD_SCRIPT%
echo adoStream.Open                                                       >> %DLOAD_SCRIPT%
echo adoStream.Type = 1                                                   >> %DLOAD_SCRIPT%
echo adoStream.Write http.ResponseBody                                    >> %DLOAD_SCRIPT%
echo adoStream.Position = 0                                               >> %DLOAD_SCRIPT%
echo.                                                                     >> %DLOAD_SCRIPT%
echo Set fileSystem = CreateObject("Scripting.FileSystemObject")          >> %DLOAD_SCRIPT%
echo If fileSystem.FileExists(target) Then fileSystem.DeleteFile target   >> %DLOAD_SCRIPT%
echo adoStream.SaveToFile target                                          >> %DLOAD_SCRIPT%
echo adoStream.Close                                                      >> %DLOAD_SCRIPT%
echo.                                                                     >> %DLOAD_SCRIPT%

GOTO:EOF
:: ******************************
:: * END Function CREATE_SCRIPT *
:: ******************************


:: ***************************
:: * BEGIN Function DOWNLOAD *
:: ***************************
:DOWNLOAD

cscript //Nologo %DLOAD_SCRIPT% "%LINK%" %SETUP%

GOTO:EOF
:: *************************
:: * END Function DOWNLOAD *
:: *************************


:: **************************
:: * BEGIN Function INSTALL *
:: **************************
:INSTALL
IF EXIST "%CD%\%SETUP%" (
  echo   Installing Cygwin. Please wait...
  %SETUP% -P %CYGWIN_PACKAGES% -C %CYGWIN_CATEGORIES% -s %MIRROR% -l %CD% -q
) ELSE (
  echo   ERROR Downloading %LINK% FAILED!!
  GOTO:EOF
)
GOTO:EOF
:: ************************
:: * END Function INSTALL *
:: ************************


:: **********************************
:: * BEGIN Function WAIT_FOR_CYGWIN *
:: **********************************
:WAIT_FOR_CYGWIN

IF EXIST "%CYG_PATH%\Cygwin.bat" (
  echo   Found %CYG_PATH%\Cygwin.bat
  GOTO:EOF
) ELSE (
  set /p someVar="Installer (still) working - Press enter when Cygwin is done":
  goto WAIT_FOR_CYGWIN
)

GOTO:EOF
:: ********************************
:: * END Function WAIT_FOR_CYGWIN *
:: ********************************

:MAIN

echo.
echo                   Installing Cygwin
echo                   =================
echo.
echo   Using default and recommended install path C:\cygwin64


IF NOT EXIST %SETUP_DIR% (MKDIR %SETUP_DIR%)
CD %SETUP_DIR%

IF NOT EXIST %DLOAD_SCRIPT% CALL:CREATE_SCRIPT

CALL:CHECK_ARCH
CALL:DOWNLOAD
CALL:INSTALL
CALL:WAIT_FOR_CYGWIN

cd %START_PATH%
generate -uG
