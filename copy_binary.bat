@echo off
REM Helper script to copy binary, converting forward slashes to backslashes
setlocal enabledelayedexpansion
set "source=%~1"
set "dest=%~2"

REM Convert forward slashes to backslashes
set "source=!source:/=\!"
set "dest=!dest:/=\!"

REM Ensure destination directory exists
for %%F in ("!dest!") do set "destdir=%%~dpF"
if not exist "!destdir!" mkdir "!destdir!"

REM Copy the file
copy /Y "!source!" "!dest!"
