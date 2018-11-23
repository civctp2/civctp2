if exist ..\..\win\dll\%1.dll goto ok
echo dll %1 does not exist 
goto bad

:ok
copy ..\..\win\dll\anet2d.dll
copy ..\..\win\dll\anet2.dll
copy ..\..\win\dll\dkupddll.dll
dir ..\..\win\bin
copy ..\..\win\bin\anetdrop.exe
copy setupsrc\anet.inf
mkdir anet
copy ..\..\win\dll\%1.dll anet

if %1==winets2 anetdrop -n:anet\%1.dll -h -g:California6.activision.com
if not %1==winets2 anetdrop -n:anet\%1.dll -h -x
:bad
