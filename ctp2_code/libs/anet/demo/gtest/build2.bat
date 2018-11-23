rem Make all four versions of gtest
rem debugging, release, dll debugging, dll release
rem
nmake -f gtest.mak CFG="gtest - Win32 dp2 Release"
if errorlevel 1 goto bad
nmake -f gtest.mak CFG="gtest - Win32 dp2 Debug"
if errorlevel 1 goto bad
nmake -f gtest.mak CFG="gtest - Win32 anet2 Release"
if errorlevel 1 goto bad
nmake -f gtest.mak CFG="gtest - Win32 anet2 Debug"
if errorlevel 1 goto bad

goto end
:bad
echo Build failed.
:end
