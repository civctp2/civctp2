rem Compile both debugging and nondebugging versions of the chat app.

call dbuild2.bat

rem /D DOSWIN32 = define preprocessor symbol DOSWIN32 (so cdecl = _cdecl, oops!)
rem /D _WIN32 = define preprocessor symbol _WIN32 (some of our code requires it!)
rem /Fm  = generate map file     (for crash logger).
rem /Oy- = generate stack frames (for crash logger).
rem /Ox  = maxiumum optimization (as if this were a real game).
rem /W3  = warning level 3       (for clean code)
rem /WX  = warnings are fatal.   (for clean code)
rem /Gr  = fastcall calling convention (just to show that we can)
rem 
rem /MD  = use MSVCRT.DLL standard C library (because that's what anet2 uses)

del chat.exe
cl /D DOSWIN32 /D _WIN32 /I ..\..\h /I ..\utils chat.c ..\utils\raw.c ..\utils\eclock.c ..\..\win\lib\anet2.lib /Fm /Oy- /Ox /W3 /WX /Gr /MD
if errorlevel 1 goto bad
if not exist chat.exe goto bad

echo demo\chat\build2 succeeded.
goto end

:bad
echo demo\chat\build2 failed.
goto end

:end
