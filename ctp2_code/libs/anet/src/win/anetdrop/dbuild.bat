rem Standard version
rem Removed /WX (treat warnings as errors)
cl /Fm /MDd /W3 /Zi /D WIN32 /D dp_ANET2 /D DEBUG /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\..\h anetdrop.c cancelbox.c ..\..\dp\launchparams.c ..\..\..\win\lib\anet2d.lib user32.lib gdi32.lib /Feanetdropd.exe /link /subsystem:windows 

