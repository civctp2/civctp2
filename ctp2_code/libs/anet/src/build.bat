REM Trivial batch file to compile Windows version of Activenet libraries.

set TOP=\mw2\commdll
set TOPWIN=%TOP%\src\win

if m%TOPWIN%m==mm goto bad

rem rem ..\3rdparty:
rem 	cd %TOP%\SRC\3rdparty
rem 	wmake unpack
rem 	if errorlevel 1 goto bad

rem rem heatsdk:
rem 	cd %TOPWIN%\3rdparty
rem 	wmake unpack
rem 	if errorlevel 1 goto bad

rem dp.lib:
	cd %TOPWIN%\dp
	if not exist %TOPWIN%\dp\build2.bat goto bad
	call build2.bat
	if errorlevel 1 goto bad

rem wmodem.dll: (normal)
rem wmodemd.dll: (debugging)
	cd %TOPWIN%\wmodem
	if not exist %TOPWIN%\wmodem\build2.bat goto bad
	call build2.bat
	if errorlevel 1 goto bad

rem wserial.dll: (normal)
rem wseriald.dll: (debugging)
	cd %TOPWIN%\wserial
	if not exist %TOPWIN%\wserial\build2.bat goto bad
	call build2.bat
	if errorlevel 1 goto bad

rem winets.dll:   (normal)
rem winetsd.dll:  (debugging)
	cd %TOPWIN%\winets
	if not exist %TOPWIN%\winets\build2.bat goto bad
	call build2.bat
	if errorlevel 1 goto bad

rem wipx.dll: (normal)
rem wipxd.dll: (debugging)
	cd %TOPWIN%\wipx
	if not exist %TOPWIN%\wipx\build2.bat goto bad
	call build2.bat
	if errorlevel 1 goto bad

rem rem wheat.dll: (normal)
rem rem wheatd.dll: (debugging)
rem 	cd %TOPWIN%\wheat
rem 	if not exist %TOPWIN%\wheat\build2.bat goto bad
rem 	call build2.bat
rem 	if errorlevel 1 goto bad

rem rem wmpath.dll: (normal)
rem rem wmpath.dll: (debugging)
rem 	cd %TOPWIN%\wmpath
rem 	if not exist %TOPWIN%\wmpath\build2.bat goto bad
rem 	call build2.bat
rem 	if errorlevel 1 goto bad

rem rem wdwango.dll: (normal)
rem rem wdwango.dll: (debugging)
rem 	cd %TOPWIN%\wdwango
rem 	if not exist %TOPWIN%\wdwango\build2.bat goto bad
rem 	call build2.bat
rem 	if errorlevel 1 goto bad


rem wloop.dll: (normal)
rem wloopd.dll: (debugging)
	cd %TOPWIN%\wloop
	if not exist %TOPWIN%\wloop\build2.bat goto bad
	call build2.bat
	if errorlevel 1 goto bad

rem ----------------------------------------------------------------
rem Install everything we compiled into commdll\win.

mkdir %TOP%\win
mkdir %TOP%\win\bin
mkdir %TOP%\win\bin\chat
mkdir %TOP%\win\lib
mkdir %TOP%\win\dll

cd %TOP%\src\win
if not exist dp\dp2\dp2.lib goto bad
copy dp\dp2\dp2.lib                  %TOP%\win\lib
copy dp\dp2d\dp2d.lib                %TOP%\win\lib
copy dp\anet2\anet2.lib              %TOP%\win\lib
copy dp\anet2\anet2.map              %TOP%\win\dll
copy dp\anet2\anet2.dll              %TOP%\win\dll\anet2.dll
copy dp\anet2d\anet2d.lib            %TOP%\win\lib
copy dp\anet2d\anet2d.map            %TOP%\win\dll
copy dp\anet2d\anet2d.dll            %TOP%\win\dll

rem rem updater:
rem rem For some reason, this copies itself into win\dll.
rem 	cd 3rdparty\updater
rem 	call build.bat
rem 	if errorlevel 1 goto bad

cd %TOP%\src\win
if not exist wmodem\wmodem2\wmodem2.dll goto bad
if not exist wmodem\wmodem2d\wmodem2d.dll goto bad
copy wmodem\wmodem2\wmodem2.dll      %TOP%\win\dll
copy wmodem\wmodem2d\wmodem2d.dll    %TOP%\win\dll

if not exist wserial\wserial2\wserial2.dll goto bad
if not exist wserial\wserial2d\wserial2d.dll goto bad
copy wserial\wserial2\wserial2.dll   %TOP%\win\dll
copy wserial\wserial2d\wserial2d.dll %TOP%\win\dll

if not exist winets\winets2\winets2.dll goto bad
if not exist winets\winets2d\winets2d.dll goto bad
copy winets\winets2\winets2.map      %TOP%\win\dll
copy winets\winets2\winets2.dll      %TOP%\win\dll
copy winets\winets2d\winets2d.map    %TOP%\win\dll
copy winets\winets2d\winets2d.dll    %TOP%\win\dll
copy winets\wudplan\wudplan.map      %TOP%\win\dll
copy winets\wudplan\wudplan.dll      %TOP%\win\dll
copy winets\wudpland\wudpland.map    %TOP%\win\dll
copy winets\wudpland\wudpland.dll    %TOP%\win\dll

if not exist wipx\wipx2\wipx2.dll goto bad
if not exist wipx\wipx2d\wipx2d.dll goto bad
copy wipx\wipx2\wipx2.dll            %TOP%\win\dll
copy wipx\wipx2d\wipx2d.dll          %TOP%\win\dll

rem if not exist wheat\wheat2\wheat2.dll goto bad
rem if not exist wheat\wheat2d\wheat2d.dll goto bad
rem copy wheat\wheat2\wheat2.dll            %TOP%\win\dll
rem copy wheat\wheat2d\wheat2d.dll          %TOP%\win\dll

rem if not exist wmpath\wmpath2\wmpath2.dll goto bad
rem if not exist wmpath\wmpath2d\wmpath2d.dll goto bad
rem copy wmpath\wmpath2\wmpath2.dll            %TOP%\win\dll
rem copy wmpath\wmpath2d\wmpath2d.dll          %TOP%\win\dll

rem if not exist wdwango\wdwango\wdwango.dll goto bad
rem if not exist wdwango\wdwangod\wdwangod.dll goto bad
rem copy wdwango\wdwango\wdwango.dll             %TOP%\win\dll
rem copy wdwango\wdwangod\wdwangod.dll           %TOP%\win\dll

if not exist wloop\wloop2\wloop2.dll goto bad
if not exist wloop\wloop2d\wloop2d.dll goto bad
copy wloop\wloop2\wloop2.dll         %TOP%\win\dll
copy wloop\wloop2d\wloop2d.dll       %TOP%\win\dll

copy 3rdparty\msvcrt.dll             %TOP%\win\dll
copy 3rdparty\imagehlp.dll           %TOP%\win\dll

copy %TOP%\doc\bootserv.txt             %TOP%\win\dll

rem ----------------------------------------------------------------
rem Compile Demo / utility programs

rem gtest.exe:
	cd %TOP%\demo\gtest
	call build2.bat
	if errorlevel 1 goto bad
	cd %TOP%\src\win

rem anetdrop:
	cd anetdrop
	call build.bat
	if errorlevel 1 goto bad
	cd ..

rem sendcrsh:
	cd sendcrsh
	call build.bat
	if errorlevel 1 goto bad
	cd ..

rem analaeh:
	cd ..\aeh\analaeh
	call build.bat
	if errorlevel 1 goto bad
	call dbuild.bat
	if errorlevel 1 goto bad
	cd ..

rem chat:
	cd %TOP%\demo\chat
	call build2.bat
	if errorlevel 1 goto bad
	cd %TOP%\src\win

rem ----------------------------------------------------------------
rem Install Demo / utility programs

if not exist %TOP%\demo\gtest\gtest2dd\gtest2dd.exe goto bad
for %%a in (dd dr d r) do copy %TOP%\demo\gtest\gtest2%%a\gtest2%%a.exe %TOP%\win\bin
for %%a in (dd dr d r) do copy %TOP%\demo\gtest\gtest2%%a\gtest2%%a.map %TOP%\win\bin

if not exist anetdrop\anetdrop.exe goto bad
copy anetdrop\anetdrop.exe   %TOP%\win\bin\anetdrop.exe
copy anetdrop\anetdrop.map                     %TOP%\win\bin
if not exist anetdrop\anetdropd.exe goto bad
copy anetdrop\anetdropd.exe                    %TOP%\win\bin
copy anetdrop\anetdropd.map                    %TOP%\win\bin

if not exist sendcrsh\sendcrsh.exe goto bad
copy sendcrsh\sendcrsh.exe                     %TOP%\win\bin
copy sendcrsh\sendcrsh.map                     %TOP%\win\bin
if not exist sendcrsh\sendcrshd.exe goto bad
copy sendcrsh\sendcrshd.exe                    %TOP%\win\bin
copy sendcrsh\sendcrshd.map                    %TOP%\win\bin

if not exist ..\aeh\analaeh\procserv\procserv.exe goto bad
copy ..\aeh\analaeh\procserv\procserv.exe      %TOP%\win\bin
if not exist ..\aeh\analaeh\modcrc\modcrc.exe goto bad
copy ..\aeh\analaeh\modcrc\modcrc.exe          %TOP%\win\bin
if not exist ..\aeh\analaeh\analcrsh\analcrsh.exe goto bad
copy ..\aeh\analaeh\analcrsh\analcrsh.exe      %TOP%\win\bin
if not exist ..\aeh\analaeh\procservd\procservd.exe goto bad
copy ..\aeh\analaeh\procservd\procservd.exe    %TOP%\win\bin
if not exist ..\aeh\analaeh\modcrcd\modcrcd.exe goto bad
copy ..\aeh\analaeh\modcrcd\modcrcd.exe        %TOP%\win\bin
if not exist ..\aeh\analaeh\analcrshd\analcrshd.exe goto bad
copy ..\aeh\analaeh\analcrshd\analcrshd.exe    %TOP%\win\bin

if not exist %TOP%\demo\chat\chatd.exe goto bad
copy %TOP%\demo\chat\chatd.exe %TOP%\win\bin\chat
copy %TOP%\demo\chat\chatd.map                  %TOP%\win\bin\chat
if not exist %TOP%\demo\chat\chat.exe goto bad
copy %TOP%\demo\chat\chat.exe %TOP%\win\bin\chat\chat.exe
if not exist %TOP%\win\bin\chat\chat.exe goto bad
copy %TOP%\demo\chat\chat.map                   %TOP%\win\bin\chat
copy %TOP%\demo\chat\setupsrc\anet.inf          %TOP%\win\bin\chat

:success
rem Pass success status back to anbuild\manbuildss.bat.
set BUILDLIBRESULT=0
echo Done ok.
goto end

:bad
rem Pass failure status back to anbuild\manbuildss.bat.
set BUILDLIBRESULT=1
echo ERROR during build.
goto end

:end

cd %TOP%\src
