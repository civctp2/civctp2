rem Example of how to spawn two hosts communicating via dpio
rem without a real LAN.  
rem See loopio.bat for more info.
for %%i in (1 2) do start loopio.bat %%i
