rem Run unit test for dynatab.

cl -W3 -Zi /MDd -Ddp_ANET2 -DDPRNT -Ddynatab_UNITTEST /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" dynatab.c -I../../h ../../win/lib/dp2d.lib > log
if errorlevel 1 goto bad
dynatab.exe
if errorlevel 1 goto bad
echo test passed
goto end

:bad
echo test failed

:end
