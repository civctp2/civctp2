@echo off
rem Build file for demo chat installer.

rem Must have already compiled chat app.
if exist chat.exe goto prereqok
echo Must have already compiled chat.exe!
goto bad

:prereqok

rem remove old chat dir and recopy files to new one.
deltree /Y setup
mkdir setup
mkdir setup\chat
mkdir setup\chat\anet
copy ..\..\win\dll\wipx2.dll    setup\chat\anet
copy ..\..\win\dll\winets2.dll  setup\chat\anet
copy ..\..\win\dll\wmodem2.dll  setup\chat\anet
copy ..\..\win\dll\wserial2.dll setup\chat\anet
copy ..\..\win\dll\wheat2.dll   setup\chat\anet
copy chat.exe setup\chat
copy ..\..\win\dll\anet2.dll setup\chat
copy ..\..\win\dll\dkupddll.dll setup\chat
copy setupsrc\anet.inf setup\chat
if not exist setup\chat\chat.exe goto bad

rem generate setup.pkg
if not .%ISHIELD3%.==.. goto ishield
echo The ISHIELD3 environment variable must be set to tell us
echo where you have InstallShield3 installed, 
echo Please set ISHIELD3 in your AUTOEXEC.BAT, restart, and rerun.
goto end

:ishield
copy %ISHIELD3%\_INST32I.EX_ setup
copy %ISHIELD3%\_SETUP.DLL setup
copy %ISHIELD3%\_SETUP.LIB setup
copy %ISHIELD3%\setup.exe setup
copy %ISHIELD3%\uninst.exe setup

cd setupsrc
dir /A:-D/B/S ..\setup\chat | perl mklst.pl > setup.lst
if errorlevel 1 goto bad
path > mypath.bat
path %ISHIELD3%;%path%
packlist setup.lst
if errorlevel 1 goto bad
call mypath.bat

rem generate setup.ins
%ISHIELD3%\compile setup.rul
cd ..
if errorlevel 1 goto bad

copy setupsrc\setup.pkg setup
copy setupsrc\setup.ins setup
goto end

:bad
echo Build failed.
goto end

:end
