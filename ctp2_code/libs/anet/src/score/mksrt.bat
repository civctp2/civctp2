mkdir scorerept
cd scorerept
set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\dp 
cl /D_DEBUG /DSIMNET /W3 /Zi ..\scorerept.c  ..\scorerep.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c /Fescorerept.exe
set INCLUDE=%OLDINC%
cd ..
