@echo off

if not %2#==# goto ok
echo Regression test - make sure variables keep their values across
echo freeze-thaw cycles.
echo Usage: test2 gtest.exe comm.dll
echo -
goto end
:ok

%1 -n=%2 -s < test2.in1
if errorlevel 1 goto fail
if exist test2a.log del test2a.log
ren dp.log test2a.log
%1 -n=%2 -c -s < test2.in2
if errorlevel 1 goto fail
if exist test2b.log del test2b.log
ren dp.log test2b.log
echo Test succeeded.
goto end
:fail
echo Test failed.
:end

