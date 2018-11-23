rem Standard version
cl /Fm /MD /W3 /WX /D WIN32 /D dp_ANET2 /I..\..\..\h anetdrop.c cancelbox.c ..\..\dp\launchparams.c ..\..\..\win\lib\anet2.lib user32.lib gdi32.lib /link /subsystem:windows 
call dbuild.bat
