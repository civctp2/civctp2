echo off
if NOT xx==x%1x goto checkplayer
echo Usage: Load room-name player-count [server-name]
echo ABORTING: Room name not present; no load placed on server.
goto end
:checkplayer
if NOT xx==x%2x goto checkserver
echo Usage: Load room-name player-count [server-name]
echo ABORTING: Player count not present; no load placed on server.
goto end
:checkserver
if NOT xx==x%3x goto serverpresent
set LOADSERVER=iserv.activision.com
goto checkdll
:serverpresent
set LOADSERVER=%3
:checkdll
if not exist anet2d.dll goto dllmissing
if not exist winets2d.dll goto dllmissing
goto run
:dllmissing
echo ABORTING: Required dll not found; no load placed on server.
:run
load2dd\load2dd -b=1 -c=%2 -d=winets2d.dll -r=%1 -s=%LOADSERVER% -l=Load.log
:end