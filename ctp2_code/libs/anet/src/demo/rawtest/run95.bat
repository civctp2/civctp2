rem Batch file to set up for running rawtest under ActiveNet Win95
nmake -f rawtest.mak
if errorlevel 1 goto end
mkdir debug\dll
copy ..\..\win\dll\*.dll debug\dll
rem Optional - grab .ini file to enable debug printing
copy mw2.ini debug
rem Set working directory (important: not stored in .mak file!) and go.
cd debug
rawtest %1 %2
:end
