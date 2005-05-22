rem Build script for release and debug, dynamic and static dp libraries.
rem Also compresses final release DLL.
rem $Log: build2.bat $
rem Revision 1.2  1997/09/12 16:44:26  dkegel
rem Got rid of last vestige of 'win/out' directory.
rem Properly compress production dll.
rem Revision 1.1  1997/08/22 16:57:15  dkegel
rem Initial revision

nmake -f dp2.mak CFG="DP2 - Win32 Release"
if errorlevel 1 goto end
nmake -f dp2.mak CFG="DP2 - Win32 Debug"
if errorlevel 1 goto end
mkdir anet2
nmake -f anet2.mak CFG="anet2 - Win32 Release" 
if errorlevel 1 goto end
mkdir anet2d
nmake -f anet2.mak CFG="anet2 - Win32 Debug"
if errorlevel 1 goto end
:end
