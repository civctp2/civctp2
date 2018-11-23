rem Clean up all versions of dp
rem $Log: clean.bat $
rem Revision 1.1  1997/08/25 03:50:24  dkegel
rem Initial revision
rem
deltree /y dp dpd anetdll anetdlld dp2 dp2d anet2 anet2d Release Debug
deltree /y dp2l anet2l
deltree /y *.err *.obj *.exe *.lnk *.log *.dll *.lib *.ncb *.mdp *.pdb
