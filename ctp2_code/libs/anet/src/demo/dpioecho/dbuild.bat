mkdir dpioechod
cd dpioechod
cl /W3 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /Zi /I..\..\..\..\h /I..\..\..\..\demo\utils ..\dpioecho.c ..\..\..\..\demo\utils\rawwin.c ..\..\..\..\win\lib\dp2d.lib /MDd version.lib advapi32.lib user32.lib /Fedpioechos.exe
cd ..



