rem Trivial crash logger demo.  See dp.htm and aeh.htm.
rem Note: crash logger only supports win95; it won't create logs on NT.
rem Meant to be run from (SDKROOT)\demo\crash\

rem Build an activenet program
call build.bat

rem Build the catalog of mapfile/.exe pairs.  (Normally done after
rem copying foo.exe/foo.dll and foo.map to \maps\projectname\buildnumber.)
rem modcrc.exe is supplied with the activenet sdk.
copy ..\..\win\bin\modcrc.exe
modcrc

rem Copy in the needed DLL's 
copy ..\..\win\dll\anet2.dll
copy ..\..\win\dll\winets2.dll

rem Create a dp.ini that causes a crash on purpose (see dp.htm)
echo [Debug] > dp.ini
echo fault=1 >> dp.ini

rem Run the program.  This should crash, and produce a log file
rem c:\windows\temp\atvilog.bin.
crash winets2.dll

rem Analyse the crash log.  
rem analcrsh.exe is supplied with the activenet sdk.
copy ..\..\win\bin\analcrsh.exe
analcrsh c:\windows\temp\atvilog.bin

rem Display the crash analysis (hope it's there).
type atvilog.txt
