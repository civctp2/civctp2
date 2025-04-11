
cl /Fm /MDd /W3 /WX /Zi /D WIN32 /D dp_ANET2 /D DEBUG  /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\h /I..\..\demo\utils /I. scordump.c scorerep.c ..\..\demo\utils\eclock.c ..\dp\assoctab.c ..\dp\dynatab.c ..\..\win\lib\anet2d.lib /Fescordump.exe
