nmake -f wmodem.mak CFG="modem - Win32 V2 Release"
if errorlevel 1 goto end
nmake -f wmodem.mak CFG="modem - Win32 V2 Debug"
if errorlevel 1 goto end
:end
