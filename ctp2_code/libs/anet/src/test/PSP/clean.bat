rem Clean up all versions of psp
rem $Log: clean.bat $
rem Revision 1.1  1997/09/01 19:21:25  dkegel
rem Initial revision
rem
deltree /y psp2dd pspr pspd pspdd pspdr
deltree /y *.err *.obj *.exe *.lnk *.log *.dll *.lib *.ncb *.mdp *.pdb
