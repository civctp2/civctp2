mkdir dpioecho
cd dpioecho
cl /W3 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\..\..\h /I..\..\..\..\demo\utils ..\dpioecho.c ..\..\..\..\demo\utils\rawwin.c ..\..\..\..\win\lib\dp2.lib /MD version.lib advapi32.lib user32.lib
cd ..
