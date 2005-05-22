mkdir dpioechos
cd dpioechos
cl /DSIMNET /W3 /Zi /I..\..\..\..\h /I..\..\..\..\demo\utils ..\dpioecho.c ..\..\..\..\demo\utils\rawwin.c ..\..\..\..\demo\utils\eclock.c /MDd version.lib advapi32.lib user32.lib /Fedpioechos.exe
cd ..
