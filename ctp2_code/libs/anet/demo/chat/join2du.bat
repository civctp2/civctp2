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
copy setupsrc\anet.inf
mkdir anet
copy ..\..\win\dll\%1.dll anet

if %1==winets2 goto winets
if %1==winets2d goto winetsd
anetdrop -n:anet\%1.dll -j -x -p:client
goto end

:winets
anetdrop -n:anet\%1.dll -j -g:207.82.188.83 %2 %3
goto end

:winetsd
anetdropd -n:anet\%1.dll -j -g:207.82.188.83 %2 %3
goto end

:bad
:end
