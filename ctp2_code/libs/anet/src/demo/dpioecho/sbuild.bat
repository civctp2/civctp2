mkdir dpioechos
cd dpioechos
cl /DSIMNET /W3 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /Zi /I..\..\..\..\h /I..\..\..\..\demo\utils ..\dpioecho.c ..\..\..\..\demo\utils\rawwin.c ..\..\..\..\demo\utils\eclock.c /MDd version.lib advapi32.lib user32.lib /Fedpioechos.exe
cd ..
