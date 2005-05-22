rem Run unit test for dpsess.
copy ..\..\win\dll\wloopd.dll
cl /MDd -DUNITTEST_DPSESS dpsess.c hkeytab.c dptab.c -I../../h ../../win/lib/dpd.lib > log
if errorlevel 1 goto bad
dpsess
if errorlevel 1 goto bad
echo test passed
goto end

:bad
echo test failed

:end
