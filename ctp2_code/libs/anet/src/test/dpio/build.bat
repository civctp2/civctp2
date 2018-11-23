rem NDEBUG build
cl /W3 /D NDEBUG /D _WIN32 /D WIN32 /D dp_ANET2 /I..\..\..\h /I..\..\dp /I..\..\win\stub dpiot2.c ..\..\..\win\lib\anet2.lib /Fedpiot2.exe
