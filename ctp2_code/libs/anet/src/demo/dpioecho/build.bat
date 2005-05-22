mkdir dpioecho
cd dpioecho
cl /W3 /I..\..\..\..\h /I..\..\..\..\demo\utils ..\dpioecho.c ..\..\..\..\demo\utils\rawwin.c ..\..\..\..\win\lib\dp2.lib /MD version.lib advapi32.lib user32.lib
cd ..
