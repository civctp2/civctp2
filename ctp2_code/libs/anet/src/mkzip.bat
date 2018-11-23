rem ----------------------------------------------------------------
rem Zip everything up for posterity

set TOP=d:\mw2\commdll

cd %TOP%
del anetcod.zip anetsdk.zip

rem Create anetcod.zip, .cod files for use when analyzing crash logs
zip anetcod.zip src\win\dp\anet2\*.cod src\win\wmodem\wmodem2\*.cod src\win\wserial\wserial2\*.cod src\win\wipx\wipx2\*.cod src\win\winet\winet2\*.cod src\win\winets\winets2\*.cod src\win\wloop\wloop2\*.cod src\win\wheat\wheat2\*.cod src\win\wmpath\wmpath2\*.cod

cd %TOP%\src\3rdparty
call clean.bat

cd %TOP%\src\win\3rdparty
call clean.bat

cd %TOP%
if NOT EXIST %TOP%\win\dll\anet2.dll goto bad

rem Create anetsdk.zip
zip anetsdk.zip h\anet.h h\dpcompat.h h\dppack*.h h\dpunpack.h h\eclock.h h\dynatab.h h\assoctab.h h\dpmem.h h\aneterr.h 
zip anetsdk.zip *.htm doc\*.htm doc\*.gif
zip -r anetsdk.zip demo -x@src\eksclude
zip -r anetsdk.zip win\lib win\dll win\bin

rem Create anetsrc.zip
zip -r anetsrc.zip *.htm doc h -x@src\eksclude
zip -r anetsrc.zip demo src -x@src\eksclude

:success
rem Pass success status back to anbuild\manbuildss.bat.
set BUILDSDKRESULT=0
echo Done ok.
goto end

:bad
rem Pass failure status back to anbuild\manbuildss.bat.
set BUILDSDKRESULT=1
echo ERROR during mkzip.
goto end

:end
cd %TOP%\src
