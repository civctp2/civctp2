deltree /y wmqt 

mkdir wmqt
cd wmqt
cl /Ddp_ANET2 /D_DEBUG /D_WIN32 /DWIN32 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH"  /I..\..\..\h /I..\..\dp /I..\..\..\demo\utils /I.. ..\wmqt.c ..\wmq.c ..\wmqfile.c ..\..\..\demo\utils\dirent.c ..\..\..\demo\utils\direntry.c /Fewmqt.exe
cd ..
