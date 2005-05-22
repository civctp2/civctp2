nmake -f winets.mak CFG="WinTCPs - Win32 V2 Release"
if errorlevel 1 goto end
nmake -f winets.mak CFG="WinTCPs - Win32 V2 LAN Release"
if errorlevel 1 goto end
call dbuild2.bat
:end
