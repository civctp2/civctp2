rem Compile an activenet program, and generate a map file
cl /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" crash.c ..\..\win\lib\anet2.lib  /Fm

