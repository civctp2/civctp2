rem Run unit test for dptab.

copy ..\..\win\dll\wloop2d.dll
cl -W3 -Zi /MDd -D_WIN32 -DWIN32 -Ddp_ANET2 -DDPRNT -Ddptab_UNITTEST ..\..\demo\utils\dpcrc32.c ..\..\demo\utils\eclock.c ..\aeh\aeh.c ..\aeh\aehlog.c ..\aeh\aehmap.c ..\aeh\tlstuff.c ..\aeh\detect.c ..\win\dp\dpdllwin.c assoctab.c delay.c dpexcept.c dpio.c dpprivy.c dprint.c dptab.c dynatab.c hkeytab.c ini.c q.c -I../../h -I../win/stub -I../aeh -I../../demo/utils kernel32.lib user32.lib advapi32.lib version.lib imagehlp.lib /Fedptab.exe > log
if errorlevel 1 goto bad
mkdir x
copy wloop2d.dll x
copy dptab.exe x
copy dp.ini x
cd x
start dptab.exe 1
cd ..
dptab.exe 2
if errorlevel 1 goto bad
echo test passed
goto end

:bad
echo test failed

:end
