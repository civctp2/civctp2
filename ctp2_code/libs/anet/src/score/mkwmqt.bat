deltree /y wmqt 

mkdir wmqt
cd wmqt
cl /Ddp_ANET2 /D_DEBUG /D_WIN32 /DWIN32 /I..\..\..\h /I..\..\dp /I..\..\..\demo\utils /I.. ..\wmqt.c ..\wmq.c ..\wmqfile.c ..\..\..\demo\utils\dirent.c ..\..\..\demo\utils\direntry.c /Fewmqt.exe
cd ..
