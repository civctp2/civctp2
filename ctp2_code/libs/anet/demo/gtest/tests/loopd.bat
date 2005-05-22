@echo off
rem Run a copy of gtestd with the loopback driver.
rem If a script is specified, run it; else run interractively.
if %1#==#     ..\..\..\win\bin\gtestd -n=..\..\..\win\dll\wloopd.dll 
if not %1#==# ..\..\..\win\bin\gtestd -n=..\..\..\win\dll\wloopd.dll -s < %1
