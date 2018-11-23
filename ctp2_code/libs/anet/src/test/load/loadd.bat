echo off
rem Load the server.
rem
rem This uses the load app to add players to the specified
rem rooms on the server.  The players are present in each
rem room in turn for as many seconds as requested.

rem *******************************************************************
rem Check for the count
rem *******************************************************************
if NOT xx == x%1x goto checkroom
echo Usage: Load count room-list time sessiontype [server-name]
echo ABORTING: Count not present; no load placed on server.
goto end

rem *******************************************************************
rem Check for the room
rem *******************************************************************
:checkroom
if NOT xx == x%2x goto checktime
echo Usage: Load count room-list time sessiontype [server-name]
echo ABORTING: Room list not present; no load placed on server.
goto end

rem *******************************************************************
rem Check for the time
rem *******************************************************************
:checktime
if NOT xx == x%3x goto checksessiontype
echo Usage: Load count room-list time sessiontype [server-name]
echo ABORTING: Time not present; no load placed on server.
goto end

rem *******************************************************************
rem Check for the sessiontype
rem *******************************************************************
:checksessiontype
if NOT xx == x%4x goto checkserver
echo Usage: Load count room-list time sessiontype [server-name]
echo ABORTING: sessiontype not present; no load placed on server.
goto end

rem *******************************************************************
rem Check for the server
rem *******************************************************************
:checkserver
if NOT xx == x%5x goto serverpresent
set LOADSERVER=iserv.activision.com
goto checkdll
:serverpresent
set LOADSERVER=%5

rem *******************************************************************
rem Get the dlls
rem *******************************************************************
:checkdll
copy ..\..\..\win\dll\anet*.dll
copy ..\..\..\win\dll\winets*.dll

rem *******************************************************************
rem Run the test
rem *******************************************************************
:run
load2dd\load2dd -c=%1 -t=%3 -d=winets2.dll -r=%2 -g=%4 -s=%LOADSERVER%

:end
