rem Debug version
rc sendbox.rc
cl /Fm /MDd /W3 /Zi /D _WIN32 /D dp_ANET2 /D _DEBUG /D aeh_IO_ONLY /D logprint_ENABLED /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\..\h /I..\..\..\demo\utils /I..\..\aeh /I..\..\dp /I..\..\sbd sendcrsh.c sendbox.c sendbox.res ..\..\sbd\sbdclnt.c ..\..\aeh\aeh.c ..\..\aeh\aehlog.c ..\..\dp\dpprivy.c ..\..\..\demo\utils\dpcrc32.c ..\..\..\demo\utils\eclock.c ..\..\..\demo\utils\logprint.c user32.lib gdi32.lib wsock32.lib /Fesendcrshd.exe /link /subsystem:windows 
