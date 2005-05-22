rem Release version
rc sendbox.rc
cl /Fm /MD /W3 /D _WIN32 /D dp_ANET2 /D aeh_IO_ONLY /I..\..\..\h /I..\..\..\demo\utils /I..\..\aeh /I..\..\dp /I..\..\sbd sendcrsh.c sendbox.c sendbox.res ..\..\sbd\sbdclnt.c ..\..\aeh\aeh.c ..\..\aeh\aehlog.c ..\..\dp\dpprivy.c ..\..\..\demo\utils\dpcrc32.c ..\..\..\demo\utils\eclock.c ..\..\..\demo\utils\logprint.c user32.lib gdi32.lib wsock32.lib /Fesendcrsh.exe /link /subsystem:windows 
if errorlevel 1 goto exit
call dbuild.bat
:exit
