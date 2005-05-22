rem Compile the debugging version of the chat app.

rem /D DOSWIN32 = define preprocessor symbol DOSWIN32 (so cdecl = _cdecl, oops!)
rem /D _WIN32 = define preprocessor symbol _WIN32 (some of our code requires it!)
rem /Fm  = generate map file     (for crash logger).
rem /Oy- = generate stack frames (for crash logger).
rem /Ox  = maxiumum optimization (as if this were a real game).
rem /W3  = warning level 3       (for clean code)
rem /WX  = warnings are fatal.   (for clean code)
rem /Gr  = fastcall calling convention (just to show that we can)
rem 
rem /MDd = use MSVCRTD.DLL standard C library (because that's what anet2d uses)
rem /Zi  = insert symbolic debugging info

del chatd.exe
cl /D DOSWIN32 /D _WIN32 /I ..\..\h /I ..\utils chat.c ..\utils\raw.c ..\utils\eclock.c ..\..\win\lib\anet2d.lib /Fm /Oy- /Ox /W3 /WX /Gr /MDd /Zi /Fechatd.exe
if errorlevel 1 goto bad
if not exist chatd.exe goto bad

echo demo\chat\dbuild2 succeeded.
goto end

:bad
echo demo\chat\dbuild2 failed.
goto end

:end
