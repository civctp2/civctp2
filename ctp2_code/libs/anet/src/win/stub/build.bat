@echo off
rem  WARNING: The resource file contains strings in Japanese.  This
rem  makes it impossible to open the resource file in VC++ resource
rem  editor on any machine that does not have code page 932 (all
rem  Japanese OS Win32 machines have code page 932).  It also means
rem  the resource compiler will not compile the file except on such
rem  machines.  If you need to modify the localized strings, add
rem  a new language, or otherwise change the resources, you will
rem  need to locate a Japanese language machine in order to
rem  compile the resources.

rem Resource file, (Localizable strings, all languages)
rem WARNING: Compile only on appropriate machines (see above)
rem rc anetstub.rc

echo Compiling standard version
cl /D WIN32 anetstub.c anetstub.res /link /subsystem:windows advapi32.lib user32.lib

echo Compiling debug version with file logging
cl /D WIN32 /D DEBUG anetstub.c anetstub.res /link /subsystem:windows advapi32.lib user32.lib /out:"logstub.exe"

echo Compiling debug version with stdout logging
cl /D WIN32 /D DEBUG /D PRINT_STDOUT anetstub.c anetstub.res /link /subsystem:windows advapi32.lib user32.lib /out:"dbgstub.exe"

echo Compiling special version for Zork - doesn't loop 
cl /D WIN32 /D OLOOP anetstub.c anetstub.res /link /subsystem:windows advapi32.lib user32.lib /out:"zorkstub.exe"
