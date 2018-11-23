rem Compile the debugging version of the band app.

rem /D DOSWIN32 = define preprocessor symbol DOSWIN32 (so cdecl = _cdecl, oops!)
rem /D WIN32 = define preprocessor symbol WIN32 (some of our code requires it!)
rem /Fm  = generate map file     (for crash logger).
rem /Oy- = generate stack frames (for crash logger).
rem /Ox  = maxiumum optimization (as if this were a real game).
rem /W3  = warning level 3       (for clean code)
rem /WX  = warnings are fatal.   (for clean code)
rem /Gr  = fastcall calling convention (just to show that we can)
rem 
rem /MDd = use MSVCRTD.DLL standard C library (because that's what anet2d uses)
rem /Zi  = insert symbolic debugging info

del bandd.exe
cl /D DOSWIN32 /D WIN32 /I ..\..\h /I ..\utils band.c ..\utils\raw.c ..\utils\eclock.c ..\utils\assoctab.c ..\utils\dynatab.c ..\..\win\lib\anet2d.lib /Fm /Oy- /Ox /W3 /WX /Gr /MDd /Zi /Febandd.exe
if errorlevel 1 goto bad
if not exist bandd.exe goto bad

echo demo\band\dbuild2 succeeded.
goto end

:bad
echo demo\band\dbuild2 failed.
goto end

:end
