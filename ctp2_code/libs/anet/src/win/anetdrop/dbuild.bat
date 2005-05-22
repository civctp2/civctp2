rem Standard version
cl /Fm /MDd /W3 /WX /Zi /D WIN32 /D dp_ANET2 /D DEBUG /I..\..\..\h anetdrop.c cancelbox.c ..\..\dp\launchparams.c ..\..\..\win\lib\anet2d.lib user32.lib gdi32.lib /Feanetdropd.exe /link /subsystem:windows 

