
cl /Fm /MDd /W3 /WX /Zi /D WIN32 /D dp_ANET2 /D DEBUG /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\h /I..\..\demo\utils /I. scorsend.c ..\..\demo\utils\eclock.c ..\..\win\lib\anet2d.lib /Fescorsend.exe
