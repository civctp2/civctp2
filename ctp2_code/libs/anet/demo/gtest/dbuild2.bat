rem Make dll debugging version of gtest
rem
nmake -f gtest.mak CFG="gtest - Win32 anet2 Debug"
if errorlevel 1 goto bad

goto end

:bad
echo Build failed.

:end
