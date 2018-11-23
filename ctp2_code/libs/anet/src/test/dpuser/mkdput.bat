set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\..\demo\utils;..\..\dp 
cl /W3 /Zi dpusert.c ..\..\..\demo\utils\rawwin.c ..\..\..\win\lib\dp2d.lib /MDd version.lib advapi32.lib user32.lib /Fedpusert.exe
set INCLUDE=%OLDINC%

