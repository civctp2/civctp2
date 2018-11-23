rem Example of how to spawn two hosts communicating via dp
rem without a real LAN.  
rem See loopdp.bat for more info.
for %%i in (1 2) do start loopdp.bat %%i
