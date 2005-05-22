@echo off
rem Build file for demo chat app and its installer.

rem Compile the actual chat app.
call build2.bat
if not exist chat.exe goto bad

rem Compile the setup for the chat app.
call sbuild.bat
if not exist setup\setup.exe goto bad
if not exist setup\setup.ins goto bad

echo demo\chat\build succeeded.
goto end

:bad
echo demo\chat\build failed.
goto end

:end
