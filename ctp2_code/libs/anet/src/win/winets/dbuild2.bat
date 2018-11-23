nmake -f winets.mak CFG="WinTCPs - Win32 V2 Debug"
if errorlevel 1 goto end
nmake -f winets.mak CFG="WinTCPs - Win32 V2 LAN Debug"
if errorlevel 1 goto end
:end
