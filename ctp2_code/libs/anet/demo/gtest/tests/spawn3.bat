rem Example of how to spawn a three-player game named 'spawn3'
rem without a real LAN.  (This is the only way at the moment to
rem run multiple copies of gtest on the same machine.)
rem See spawn.bat for more info.
for %%i in (0 1 2) do start spawn.bat %%i spawn3
