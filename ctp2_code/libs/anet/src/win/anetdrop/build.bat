rem Standard version
rem Removed /WX (treat warnings as errors)
cl /Fm /MD /W3 /D WIN32 /D dp_ANET2 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\..\h anetdrop.c cancelbox.c ..\..\dp\launchparams.c ..\..\..\win\lib\anet2.lib user32.lib gdi32.lib /link /subsystem:windows 
call dbuild.bat
