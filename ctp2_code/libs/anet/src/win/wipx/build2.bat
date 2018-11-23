nmake -f wipx.mak CFG="ipx2dll - Win32 V2 Release"
if errorlevel 1 goto end
nmake -f wipx.mak CFG="ipx2dll - Win32 V2 Debug"
if errorlevel 1 goto end
:end
