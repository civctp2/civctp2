rem NDEBUG build
cl /W3 /D NDEBUG /D _WIN32 /D WIN32 /D dp_ANET2 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\..\h /I..\..\dp /I..\..\win\stub dpiot2.c ..\..\..\win\lib\anet2.lib /Fedpiot2.exe
