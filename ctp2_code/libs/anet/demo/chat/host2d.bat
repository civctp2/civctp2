if exist ..\..\win\dll\%1.dll goto ok
echo dll %1 does not exist 
goto bad

:ok
copy ..\..\win\dll\anet2d.dll
copy ..\..\win\dll\anet2.dll
copy ..\..\win\dll\dkupddll.dll
copy ..\..\win\dll\dkupddlld.dll
dir ..\..\win\bin
copy ..\..\win\bin\anetdrop.exe
copy ..\..\win\bin\anetdropd.exe
mkdir anet
copy ..\..\win\dll\%1.dll anet

if %1==winets2 goto winets
if %1==winets2d goto winetsd
anetdrop -n:anet\%1.dll -h -x
goto end

:winets
anetdrop -n:anet\%1.dll -h -g:California6.activision.com
goto end

:winetsd
anetdropd -n:anet\%1.dll -h -x -g:California6.activision.com
goto end

:bad
:end