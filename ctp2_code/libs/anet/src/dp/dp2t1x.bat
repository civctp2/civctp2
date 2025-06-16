rem Compile client unit test for dp2.

cl -W3 -Zi /MDd -DDPRNT /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" dp2t1.c dp2.c dptab.c hkeytab.c dprint.c -I../../h ../../win/lib/dpd.lib  > log
if errorlevel 1 goto bad
del dp2*.ilk
del dp2*.pdb
del dp2*.lib
del *.obj
del *.exp
goto end
:bad
more log
echo test failed

:end
