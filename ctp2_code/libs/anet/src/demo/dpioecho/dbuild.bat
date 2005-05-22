mkdir dpioechod
cd dpioechod
cl /W3 /Zi /I..\..\..\..\h /I..\..\..\..\demo\utils ..\dpioecho.c ..\..\..\..\demo\utils\rawwin.c ..\..\..\..\win\lib\dp2d.lib /MDd version.lib advapi32.lib user32.lib /Fedpioechos.exe
cd ..



