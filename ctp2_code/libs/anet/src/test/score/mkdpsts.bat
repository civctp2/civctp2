set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\..\demo\utils;..\..\dp;..\..\tca;..\..\unix\server;..\..\score
cl /W3 /Zi /D_DEBUG /DDPRNT /DSIMNET /DWIN32 /D_WINDOWS /DDYNALINK /Ddp_ANET2 /Ddpscore_CLIENT_ONLY dpscoret.c dpscore.c ..\..\score\scorerep.c ..\..\tca\tserv.c ..\..\tca\tserva.c ..\..\tca\tca.c ..\..\tca\tcapw.c ..\..\dp\dptab.c ..\..\tca\crypttab.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c ..\..\dp\hkeytab.c ..\..\dp\dpcrc32.c ..\..\..\demo\utils\mywcs.c ..\..\3rdparty\d3des\d3des.c ..\..\3rdparty\md5\md5c.c ..\..\..\demo\utils\rawwin.c ..\..\..\demo\utils\eclock.c /MDd /Fedpscorts.exe
set INCLUDE=%OLDINC%
