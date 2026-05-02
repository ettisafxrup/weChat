@echo off
setlocal enabledelayedexpansion

set ROOT=..
set BUILD_FILE=%ROOT%\build_log.txt
set VERSION=1

REM Read sub version
if exist %BUILD_FILE% (
    set /p SUB_VERSION=<%BUILD_FILE%
) else (
    set SUB_VERSION=0
)

REM Create folders
if not exist %ROOT%\object mkdir %ROOT%\object
if not exist %ROOT%\release mkdir %ROOT%\release

REM Compile
g++ -c %ROOT%\main.cpp -I%ROOT%\include -o %ROOT%\object\main.o

REM Link
set OUTPUT=%ROOT%\release\weChat_v%VERSION%.%SUB_VERSION%.exe
g++ %ROOT%\object\main.o -o %OUTPUT%

echo Build created: %OUTPUT%

REM Increment build number
set /a NEXT_VERSION=%SUB_VERSION%+1
echo %NEXT_VERSION% > %BUILD_FILE%

endlocal