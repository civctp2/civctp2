rem Build script for debug dynamic and static dp22 libraries.

rem nmake -f dp2.mak CFG="DP2 - Win32 Release"
rem if errorlevel 1 goto end
nmake -f dp2.mak CFG="DP2 - Win32 Debug"
if errorlevel 1 goto end

rem nmake -f anet2.mak CFG="anet2 - Win32 Release" 
rem if errorlevel 1 goto end
nmake -f anet2.mak CFG="anet2 - Win32 Debug"
if errorlevel 1 goto end

echo Done.
:end
