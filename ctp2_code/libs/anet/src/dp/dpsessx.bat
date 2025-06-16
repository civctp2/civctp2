rem Run unit test for dpsess.
copy ..\..\win\dll\wloopd.dll
cl /MDd -DUNITTEST_DPSESS /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" dpsess.c hkeytab.c dptab.c -I../../h ../../win/lib/dpd.lib > log
if errorlevel 1 goto bad
dpsess
if errorlevel 1 goto bad
echo test passed
goto end

:bad
echo test failed

:end
