copy ..\..\win\dll\anet2d.dll
copy ..\..\win\dll\dkupddll.dll
mkdir anet
copy ..\..\win\dll\wheatd.dll anet
copy ..\..\win\bin\anetdropd.exe
copy setupsrc\anet.inf

anetdropd -n:anet\wheatd.dll -j -x
:bad
