nmake -f psp.mak CFG="PSP - Win32 load V2 dll Release"
copy ..\..\..\win\dll\anet2.dll
copy ..\..\..\win\dll\winets2.dll
load2dr\load2dr -c=2 -d=winets2.dll -r=HamRoom -s=iserv.activision.com -l=Load.log
:end
