@echo off
echo You must run Codewarrior by hand to build dp.lib etc.
echo Only dp.lib can be built here for now; the DLL's have to be built on the Mac.
if exist dp\dp.lib goto built
start dp\dp.cwp
goto end
:built
mkdir ..\..\mac
mkdir ..\..\mac\lib
copy dp\dp.lib ..\..\mac\lib
:end
