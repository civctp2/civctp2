# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=anet2 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to anet2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "anet2 - Win32 Release" && "$(CFG)" != "anet2 - Win32 Debug" &&\
 "$(CFG)" != "anet2 - Win32 Logging"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "anet2.mak" CFG="anet2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "anet2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "anet2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "anet2 - Win32 Logging" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "anet2 - Win32 Logging"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "anet2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "anet2"
# PROP Intermediate_Dir "anet2"
# PROP Target_Dir ""
OUTDIR=.\anet2
INTDIR=.\anet2

ALL : "$(OUTDIR)\anet2.dll"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\assoctab.obj"
	-@erase "$(INTDIR)\crypttab.obj"
	-@erase "$(INTDIR)\d3des.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\deltas.obj"
	-@erase "$(INTDIR)\detect.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\dp2.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpdllwin.obj"
	-@erase "$(INTDIR)\dpexcept.obj"
	-@erase "$(INTDIR)\dpio.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\dprint.obj"
	-@erase "$(INTDIR)\dpscore.obj"
	-@erase "$(INTDIR)\dpshutdn.obj"
	-@erase "$(INTDIR)\dptab.obj"
	-@erase "$(INTDIR)\dpuser.obj"
	-@erase "$(INTDIR)\dynatab.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\enumapp.obj"
	-@erase "$(INTDIR)\enumsrv2.obj"
	-@erase "$(INTDIR)\getdisp.obj"
	-@erase "$(INTDIR)\hkeytab.obj"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\launchap.obj"
	-@erase "$(INTDIR)\launchparams.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\mywcs.obj"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\q.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\scorerep.obj"
	-@erase "$(INTDIR)\tca.obj"
	-@erase "$(INTDIR)\tcapw.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tserv.obj"
	-@erase "$(INTDIR)\tserva.obj"
	-@erase "$(OUTDIR)\anet2.dll"
	-@erase "$(OUTDIR)\anet2.exp"
	-@erase "$(OUTDIR)\anet2.lib"
	-@erase "$(OUTDIR)\anet2.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D "dp_ANET2" /FAcs /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I "..\..\aeh"\
 /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "WIN32"\
 /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D "dp_ANET2" /FAcs /Fa"$(INTDIR)/"\
 /Fp"$(INTDIR)/anet2.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\anet2/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/anet2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib version.lib /nologo /subsystem:windows /dll /pdb:none /map /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib version.lib /nologo\
 /subsystem:windows /dll /pdb:none /map:"$(INTDIR)/anet2.map" /machine:I386\
 /out:"$(OUTDIR)/anet2.dll" /implib:"$(OUTDIR)/anet2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\assoctab.obj" \
	"$(INTDIR)\crypttab.obj" \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\deltas.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\dp2.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpdllwin.obj" \
	"$(INTDIR)\dpexcept.obj" \
	"$(INTDIR)\dpio.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\dprint.obj" \
	"$(INTDIR)\dpscore.obj" \
	"$(INTDIR)\dpshutdn.obj" \
	"$(INTDIR)\dptab.obj" \
	"$(INTDIR)\dpuser.obj" \
	"$(INTDIR)\dynatab.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\enumapp.obj" \
	"$(INTDIR)\enumsrv2.obj" \
	"$(INTDIR)\getdisp.obj" \
	"$(INTDIR)\hkeytab.obj" \
	"$(INTDIR)\ini.obj" \
	"$(INTDIR)\launchap.obj" \
	"$(INTDIR)\launchparams.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\mywcs.obj" \
	"$(INTDIR)\ptimer.obj" \
	"$(INTDIR)\q.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\scorerep.obj" \
	"$(INTDIR)\tca.obj" \
	"$(INTDIR)\tcapw.obj" \
	"$(INTDIR)\tlstuff.obj" \
	"$(INTDIR)\tserv.obj" \
	"$(INTDIR)\tserva.obj"

"$(OUTDIR)\anet2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "anet2d"
# PROP Intermediate_Dir "anet2d"
# PROP Target_Dir ""
OUTDIR=.\anet2d
INTDIR=.\anet2d

ALL : "$(OUTDIR)\anet2d.dll"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\assoctab.obj"
	-@erase "$(INTDIR)\crypttab.obj"
	-@erase "$(INTDIR)\d3des.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\deltas.obj"
	-@erase "$(INTDIR)\detect.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\dp2.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpdllwin.obj"
	-@erase "$(INTDIR)\dpexcept.obj"
	-@erase "$(INTDIR)\dpio.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\dprint.obj"
	-@erase "$(INTDIR)\dpscore.obj"
	-@erase "$(INTDIR)\dpshutdn.obj"
	-@erase "$(INTDIR)\dptab.obj"
	-@erase "$(INTDIR)\dpuser.obj"
	-@erase "$(INTDIR)\dynatab.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\enumapp.obj"
	-@erase "$(INTDIR)\enumsrv2.obj"
	-@erase "$(INTDIR)\getdisp.obj"
	-@erase "$(INTDIR)\hkeytab.obj"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\launchap.obj"
	-@erase "$(INTDIR)\launchparams.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\mywcs.obj"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\q.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\scorerep.obj"
	-@erase "$(INTDIR)\tca.obj"
	-@erase "$(INTDIR)\tcapw.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tserv.obj"
	-@erase "$(INTDIR)\tserva.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\anet2d.dll"
	-@erase "$(OUTDIR)\anet2d.exp"
	-@erase "$(OUTDIR)\anet2d.lib"
	-@erase "$(OUTDIR)\anet2d.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W2 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "_DEBUG" /D "DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D "dp_ANET2" /YX /c
CPP_PROJ=/nologo /MDd /W2 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I\
 "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "_DEBUG"\
 /D "DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D\
 "dp_ANET2" /Fp"$(INTDIR)/anet2.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\anet2d/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /D "DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /D "DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /D "DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/anet2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib version.lib /nologo /subsystem:windows /dll /pdb:none /map /debug /machine:I386 /out:"anet2d/anet2d.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib version.lib /nologo\
 /subsystem:windows /dll /pdb:none /map:"$(INTDIR)/anet2d.map" /debug\
 /machine:I386 /out:"$(OUTDIR)/anet2d.dll" /implib:"$(OUTDIR)/anet2d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\assoctab.obj" \
	"$(INTDIR)\crypttab.obj" \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\deltas.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\dp2.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpdllwin.obj" \
	"$(INTDIR)\dpexcept.obj" \
	"$(INTDIR)\dpio.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\dprint.obj" \
	"$(INTDIR)\dpscore.obj" \
	"$(INTDIR)\dpshutdn.obj" \
	"$(INTDIR)\dptab.obj" \
	"$(INTDIR)\dpuser.obj" \
	"$(INTDIR)\dynatab.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\enumapp.obj" \
	"$(INTDIR)\enumsrv2.obj" \
	"$(INTDIR)\getdisp.obj" \
	"$(INTDIR)\hkeytab.obj" \
	"$(INTDIR)\ini.obj" \
	"$(INTDIR)\launchap.obj" \
	"$(INTDIR)\launchparams.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\mywcs.obj" \
	"$(INTDIR)\ptimer.obj" \
	"$(INTDIR)\q.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\scorerep.obj" \
	"$(INTDIR)\tca.obj" \
	"$(INTDIR)\tcapw.obj" \
	"$(INTDIR)\tlstuff.obj" \
	"$(INTDIR)\tserv.obj" \
	"$(INTDIR)\tserva.obj"

"$(OUTDIR)\anet2d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "anet2___"
# PROP BASE Intermediate_Dir "anet2___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "anet2l"
# PROP Intermediate_Dir "anet2l"
# PROP Target_Dir ""
OUTDIR=.\anet2l
INTDIR=.\anet2l

ALL : "$(OUTDIR)\anet2l.dll"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\assoctab.obj"
	-@erase "$(INTDIR)\crypttab.obj"
	-@erase "$(INTDIR)\d3des.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\deltas.obj"
	-@erase "$(INTDIR)\detect.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\dp2.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpdllwin.obj"
	-@erase "$(INTDIR)\dpexcept.obj"
	-@erase "$(INTDIR)\dpio.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\dprint.obj"
	-@erase "$(INTDIR)\dpscore.obj"
	-@erase "$(INTDIR)\dpshutdn.obj"
	-@erase "$(INTDIR)\dptab.obj"
	-@erase "$(INTDIR)\dpuser.obj"
	-@erase "$(INTDIR)\dynatab.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\enumapp.obj"
	-@erase "$(INTDIR)\enumsrv2.obj"
	-@erase "$(INTDIR)\getdisp.obj"
	-@erase "$(INTDIR)\hkeytab.obj"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\launchap.obj"
	-@erase "$(INTDIR)\launchparams.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\mywcs.obj"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\q.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\scorerep.obj"
	-@erase "$(INTDIR)\tca.obj"
	-@erase "$(INTDIR)\tcapw.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tserv.obj"
	-@erase "$(INTDIR)\tserva.obj"
	-@erase "$(OUTDIR)\anet2l.dll"
	-@erase "$(OUTDIR)\anet2l.exp"
	-@erase "$(OUTDIR)\anet2l.lib"
	-@erase "$(OUTDIR)\anet2l.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D "dp_ANET2" /FAcs /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D "dp_ANET2" /FAcs /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I "..\..\aeh"\
 /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "DPRNT"\
 /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "DP_DLL" /D "dp_ANET2" /FAcs\
 /Fa"$(INTDIR)/" /Fp"$(INTDIR)/anet2.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\anet2l/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /D "DPRNT" /win32
MTL_PROJ=/nologo /D "NDEBUG" /D "DPRNT" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "DPRNT"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/anet2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib version.lib /nologo /subsystem:windows /dll /pdb:none /map /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib version.lib /nologo /subsystem:windows /dll /pdb:none /map /machine:I386 /out:"anet2l/anet2l.dll"
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib version.lib /nologo\
 /subsystem:windows /dll /pdb:none /map:"$(INTDIR)/anet2l.map" /machine:I386\
 /out:"$(OUTDIR)/anet2l.dll" /implib:"$(OUTDIR)/anet2l.lib" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\assoctab.obj" \
	"$(INTDIR)\crypttab.obj" \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\deltas.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\dp2.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpdllwin.obj" \
	"$(INTDIR)\dpexcept.obj" \
	"$(INTDIR)\dpio.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\dprint.obj" \
	"$(INTDIR)\dpscore.obj" \
	"$(INTDIR)\dpshutdn.obj" \
	"$(INTDIR)\dptab.obj" \
	"$(INTDIR)\dpuser.obj" \
	"$(INTDIR)\dynatab.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\enumapp.obj" \
	"$(INTDIR)\enumsrv2.obj" \
	"$(INTDIR)\getdisp.obj" \
	"$(INTDIR)\hkeytab.obj" \
	"$(INTDIR)\ini.obj" \
	"$(INTDIR)\launchap.obj" \
	"$(INTDIR)\launchparams.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\mywcs.obj" \
	"$(INTDIR)\ptimer.obj" \
	"$(INTDIR)\q.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\scorerep.obj" \
	"$(INTDIR)\tca.obj" \
	"$(INTDIR)\tcapw.obj" \
	"$(INTDIR)\tlstuff.obj" \
	"$(INTDIR)\tserv.obj" \
	"$(INTDIR)\tserva.obj"

"$(OUTDIR)\anet2l.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "anet2 - Win32 Release"
# Name "anet2 - Win32 Debug"
# Name "anet2 - Win32 Logging"

!IF  "$(CFG)" == "anet2 - Win32 Release"

!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dpdllwin.c
DEP_CPP_DPDLL=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpdllwin.obj" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpdllwin.obj" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpdllwin.obj" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dllmain.c

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\q.c
DEP_CPP_Q_C2=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\q.obj" : $(SOURCE) $(DEP_CPP_Q_C2) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\q.obj" : $(SOURCE) $(DEP_CPP_Q_C2) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\q.obj" : $(SOURCE) $(DEP_CPP_Q_C2) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpio.c
DEP_CPP_DPIO_=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\ptimer.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	"..\stub\anetstub.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpio.obj" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpio.obj" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpio.obj" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dprint.c
DEP_CPP_DPRIN=\
	"..\..\..\demo\utils\stricmp.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dprint.obj" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dprint.obj" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dprint.obj" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dynatab.c
DEP_CPP_DYNAT=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dynatab.obj" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dynatab.obj" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dynatab.obj" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\ini.c
DEP_CPP_INI_C=\
	"..\..\..\demo\utils\stricmp.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\assoctab.c
DEP_CPP_ASSOC=\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\assoctab.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\assoctab.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\assoctab.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\delay.c
DEP_CPP_DELAY=\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dpmem.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\delay.obj" : $(SOURCE) $(DEP_CPP_DELAY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\delay.obj" : $(SOURCE) $(DEP_CPP_DELAY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\delay.obj" : $(SOURCE) $(DEP_CPP_DELAY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\enumapp.c
DEP_CPP_ENUMA=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\portable.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	"..\3rdparty\updater\update.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\enumapp.obj" : $(SOURCE) $(DEP_CPP_ENUMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\enumapp.obj" : $(SOURCE) $(DEP_CPP_ENUMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\enumapp.obj" : $(SOURCE) $(DEP_CPP_ENUMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\launchap.c
DEP_CPP_LAUNC=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	"..\stub\anetstub.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\launchap.obj" : $(SOURCE) $(DEP_CPP_LAUNC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\launchap.obj" : $(SOURCE) $(DEP_CPP_LAUNC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\launchap.obj" : $(SOURCE) $(DEP_CPP_LAUNC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dp2.c
DEP_CPP_DP2_C=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\..\demo\utils\unistd.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpexcept.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\enumapps.h"\
	"..\..\..\h\enumserv.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\launchparams.h"\
	"..\..\..\h\portable.h"\
	"..\..\..\h\q.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\aehlog.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\dp\ptimer.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dp2.obj" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dp2.obj" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dp2.obj" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dptab.c
DEP_CPP_DPTAB=\
	"..\..\..\demo\utils\dpcrc32.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dptab.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dptab.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dptab.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\hkeytab.c
DEP_CPP_HKEYT=\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\hkeytab.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\hkeytab.obj" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\hkeytab.obj" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\hkeytab.obj" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\enumsrv2.c
DEP_CPP_ENUMS=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\enumsrv2.obj" : $(SOURCE) $(DEP_CPP_ENUMS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\enumsrv2.obj" : $(SOURCE) $(DEP_CPP_ENUMS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\enumsrv2.obj" : $(SOURCE) $(DEP_CPP_ENUMS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpprivy.c
DEP_CPP_DPPRI=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpexcept.c
DEP_CPP_DPEXC=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\..\demo\utils\unistd.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpexcept.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\portable.h"\
	"..\..\..\h\q.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\aehlog.h"\
	"..\..\aeh\detect.h"\
	"..\..\aeh\getdisp.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpexcept.obj" : $(SOURCE) $(DEP_CPP_DPEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpexcept.obj" : $(SOURCE) $(DEP_CPP_DPEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpexcept.obj" : $(SOURCE) $(DEP_CPP_DPEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\aeh.c
DEP_CPP_AEH_C=\
	"..\..\..\demo\utils\dpcrc32.h"\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\..\h\eclock.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\detect.h"\
	"..\..\aeh\tlstuff.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\aehlog.c
DEP_CPP_AEHLO=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\aehlog.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\detect.c
DEP_CPP_DETEC=\
	"..\..\aeh\detect.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\detect.obj" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\detect.obj" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\detect.obj" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\registry.c
DEP_CPP_REGIS=\
	"..\..\aeh\registry.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\getdisp.c
DEP_CPP_GETDI=\
	"..\..\aeh\getdisp.h"\
	"..\..\aeh\registry.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\getdisp.obj" : $(SOURCE) $(DEP_CPP_GETDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\getdisp.obj" : $(SOURCE) $(DEP_CPP_GETDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\getdisp.obj" : $(SOURCE) $(DEP_CPP_GETDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\tlstuff.c
DEP_CPP_TLSTU=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\tlstuff.h"\
	"..\..\aeh\win4alt.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpscore.c
DEP_CPP_DPSCO=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpscore.obj" : $(SOURCE) $(DEP_CPP_DPSCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpscore.obj" : $(SOURCE) $(DEP_CPP_DPSCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpscore.obj" : $(SOURCE) $(DEP_CPP_DPSCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\deltas.c
DEP_CPP_DELTA=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\deltas.obj" : $(SOURCE) $(DEP_CPP_DELTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\deltas.obj" : $(SOURCE) $(DEP_CPP_DELTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\deltas.obj" : $(SOURCE) $(DEP_CPP_DELTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\aehmap.c
DEP_CPP_AEHMA=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\tlstuff.h"\
	"..\..\aeh\win4alt.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\ptimer.c
DEP_CPP_PTIME=\
	"..\..\..\h\eclock.h"\
	"..\..\dp\ptimer.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\tca\tca.c
DEP_CPP_TCA_C=\
	"..\..\..\demo\utils\mywcs.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\3rdparty\md5\global.h"\
	"..\..\3rdparty\md5\md5.h"\
	"..\..\aeh\..\3rdparty\d3des\d3des.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\tca.obj" : $(SOURCE) $(DEP_CPP_TCA_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\tca.obj" : $(SOURCE) $(DEP_CPP_TCA_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\tca.obj" : $(SOURCE) $(DEP_CPP_TCA_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\tca\tcapw.c
DEP_CPP_TCAPW=\
	"..\..\..\demo\utils\mywcs.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\3rdparty\md5\global.h"\
	"..\..\3rdparty\md5\md5.h"\
	"..\..\aeh\..\3rdparty\d3des\d3des.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\crypttab.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\tcapw.obj" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\tcapw.obj" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\tcapw.obj" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\tca\tserv.c
DEP_CPP_TSERV=\
	"..\..\..\demo\utils\mywcs.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\tserv.obj" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\tserv.obj" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\tserv.obj" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\tca\tserva.c
DEP_CPP_TSERVA=\
	"..\..\..\demo\utils\mywcs.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\q.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\tserva.obj" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\tserva.obj" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\tserva.obj" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\mywcs.c
DEP_CPP_MYWCS=\
	"..\..\..\demo\utils\mywcs.h"\
	"..\..\..\h\aneterr.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\mywcs.obj" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\mywcs.obj" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\mywcs.obj" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\3rdparty\d3des\d3des.c
DEP_CPP_D3DES=\
	"..\..\aeh\..\3rdparty\d3des\d3des.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\d3des.obj" : $(SOURCE) $(DEP_CPP_D3DES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\d3des.obj" : $(SOURCE) $(DEP_CPP_D3DES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\d3des.obj" : $(SOURCE) $(DEP_CPP_D3DES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\3rdparty\md5\md5c.c
DEP_CPP_MD5C_=\
	"..\..\3rdparty\md5\global.h"\
	"..\..\3rdparty\md5\md5.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\tca\crypttab.c
DEP_CPP_CRYPT=\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\aeh\..\3rdparty\d3des\d3des.h"\
	"..\..\tca\crypttab.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\crypttab.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\crypttab.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\crypttab.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpuser.c
DEP_CPP_DPUSE=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpuser.obj" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpuser.obj" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpuser.obj" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\score\scorerep.c
DEP_CPP_SCORE=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\scorerep.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\scorerep.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\scorerep.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpshutdn.c
DEP_CPP_DPSHU=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\contract.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpshutdn.obj" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpshutdn.obj" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpshutdn.obj" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\dpcrc32.c
DEP_CPP_DPCRC=\
	"..\..\..\demo\utils\dpcrc32.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\launchparams.c
DEP_CPP_LAUNCH=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\launchparams.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "anet2 - Win32 Release"


"$(INTDIR)\launchparams.obj" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Debug"


"$(INTDIR)\launchparams.obj" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "anet2 - Win32 Logging"


"$(INTDIR)\launchparams.obj" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
