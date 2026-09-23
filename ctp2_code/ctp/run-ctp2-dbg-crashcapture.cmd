@echo off
setlocal
set "SCRIPT_DIR=%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%run-ctp2-dbg-crashcapture.ps1" %*
set "RC=%ERRORLEVEL%"
endlocal & exit /b %RC%
