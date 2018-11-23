cd anetdrop
call clean.bat
cd ..
cd sendcrsh
call clean.bat
cd ..
cd dp
call clean.bat
cd ..
cd wmodem
nmake -f wmodem.mak clean
cd ..
cd wserial
nmake -f wserial.mak clean
cd ..
cd wipx
nmake -f wipx.mak clean
cd ..
cd winets
nmake -f winets.mak clean
cd ..
cd wloop
nmake -f wloop.mak clean
cd ..
cd wheat
nmake -f wheat.mak clean
cd ..
cd wmpath
nmake -f wmpath.mak clean
cd ..
cd 3rdparty
call clean.bat
cd ..
cd wdwango
call clean.bat
cd ..

deltree /y dp/debug      dp/release      dp/anetdll       dp/anetdlld
deltree /y dp/dp2d       dp/dp2          dp/anet2         dp/anet2d

deltree /y wmodem/debug  wmodem/release  wmodem/wmodem2   wmodem/wmodem2d
deltree /y wserial/debug wserial/release wserial/wserial2 wserial/wserial2d
deltree /y wipx/debug    wipx/release    wipx/wipx2       wipx/wipx2d
deltree /y wheat/wheat2  wheat/wheat2d
deltree /y wmpath/wmpath2  wmpath/wmpath2d
deltree /y winets/winets winets/winetsd  winets/winets2   winets/winets2d
deltree /y winets/debug  winets/release
deltree /y winets/wudplan winets/wudpland
deltree /y wloop/wloop   wloop/wloopd    wloop/wloopnb    wloop/wloopnbd
deltree /y wloop/wloop2  wloop/wloop2d

deltree /y out
