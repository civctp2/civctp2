set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\..\demo\utils;..\..\dp 
cl /W3 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /Zi dpusert.c ..\..\..\demo\utils\rawwin.c ..\..\..\win\lib\dp2d.lib /MDd version.lib advapi32.lib user32.lib /Fedpusert.exe
set INCLUDE=%OLDINC%

