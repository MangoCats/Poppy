@echo off

REM Setting up environment so Poppy.exe can run
set QTDIR=C:\Qt\5.15\env
set PATH=%QTDIR%\bin;%PATH%

cd C:\Users\paleo\Dev\Poppy
start Poppy.exe

