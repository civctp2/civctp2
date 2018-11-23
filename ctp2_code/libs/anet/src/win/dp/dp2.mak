# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=DP2 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DP2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DP2 - Win32 Release" && "$(CFG)" != "DP2 - Win32 Debug" &&\
 "$(CFG)" != "DP2 - Win32 Logging"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dp2.mak" CFG="DP2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DP2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DP2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DP2 - Win32 Logging" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "DP2 - Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "DP2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dp2"
# PROP Intermediate_Dir "dp2"
# PROP Target_Dir ""
OUTDIR=.\dp2
INTDIR=.\dp2

ALL : "$(OUTDIR)\dp2.lib"

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
	-@erase "$(INTDIR)\dp2.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\Dpdllwin.obj"
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
	-@erase "$(INTDIR)\Q.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\scorerep.obj"
	-@erase "$(INTDIR)\tca.obj"
	-@erase "$(INTDIR)\tcapw.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tserv.obj"
	-@erase "$(INTDIR)\tserva.obj"
	-@erase "$(OUTDIR)\dp2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /Oy- /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /YX /c
CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /Oy- /I "..\..\..\h" /I "..\..\aeh" /I\
 "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /Fp"$(INTDIR)/dp2.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dp2/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dp2.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dp2.lib" 
LIB32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\assoctab.obj" \
	"$(INTDIR)\crypttab.obj" \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\deltas.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\dp2.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\Dpdllwin.obj" \
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
	"$(INTDIR)\Q.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\scorerep.obj" \
	"$(INTDIR)\tca.obj" \
	"$(INTDIR)\tcapw.obj" \
	"$(INTDIR)\tlstuff.obj" \
	"$(INTDIR)\tserv.obj" \
	"$(INTDIR)\tserva.obj"

"$(OUTDIR)\dp2.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dp2d"
# PROP Intermediate_Dir "dp2d"
# PROP Target_Dir ""
OUTDIR=.\dp2d
INTDIR=.\dp2d

ALL : "$(OUTDIR)\dp2d.lib" "$(OUTDIR)\dp2.bsc"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aeh.sbr"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehlog.sbr"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\aehmap.sbr"
	-@erase "$(INTDIR)\assoctab.obj"
	-@erase "$(INTDIR)\assoctab.sbr"
	-@erase "$(INTDIR)\crypttab.obj"
	-@erase "$(INTDIR)\crypttab.sbr"
	-@erase "$(INTDIR)\d3des.obj"
	-@erase "$(INTDIR)\d3des.sbr"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\delay.sbr"
	-@erase "$(INTDIR)\deltas.obj"
	-@erase "$(INTDIR)\deltas.sbr"
	-@erase "$(INTDIR)\detect.obj"
	-@erase "$(INTDIR)\detect.sbr"
	-@erase "$(INTDIR)\dp2.obj"
	-@erase "$(INTDIR)\dp2.sbr"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpcrc32.sbr"
	-@erase "$(INTDIR)\Dpdllwin.obj"
	-@erase "$(INTDIR)\Dpdllwin.sbr"
	-@erase "$(INTDIR)\dpexcept.obj"
	-@erase "$(INTDIR)\dpexcept.sbr"
	-@erase "$(INTDIR)\dpio.obj"
	-@erase "$(INTDIR)\dpio.sbr"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\dpprivy.sbr"
	-@erase "$(INTDIR)\dprint.obj"
	-@erase "$(INTDIR)\dprint.sbr"
	-@erase "$(INTDIR)\dpscore.obj"
	-@erase "$(INTDIR)\dpscore.sbr"
	-@erase "$(INTDIR)\dpshutdn.obj"
	-@erase "$(INTDIR)\dpshutdn.sbr"
	-@erase "$(INTDIR)\dptab.obj"
	-@erase "$(INTDIR)\dptab.sbr"
	-@erase "$(INTDIR)\dpuser.obj"
	-@erase "$(INTDIR)\dpuser.sbr"
	-@erase "$(INTDIR)\dynatab.obj"
	-@erase "$(INTDIR)\dynatab.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\enumapp.obj"
	-@erase "$(INTDIR)\enumapp.sbr"
	-@erase "$(INTDIR)\enumsrv2.obj"
	-@erase "$(INTDIR)\enumsrv2.sbr"
	-@erase "$(INTDIR)\getdisp.obj"
	-@erase "$(INTDIR)\getdisp.sbr"
	-@erase "$(INTDIR)\hkeytab.obj"
	-@erase "$(INTDIR)\hkeytab.sbr"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\ini.sbr"
	-@erase "$(INTDIR)\launchap.obj"
	-@erase "$(INTDIR)\launchap.sbr"
	-@erase "$(INTDIR)\launchparams.obj"
	-@erase "$(INTDIR)\launchparams.sbr"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\md5c.sbr"
	-@erase "$(INTDIR)\mywcs.obj"
	-@erase "$(INTDIR)\mywcs.sbr"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\ptimer.sbr"
	-@erase "$(INTDIR)\Q.obj"
	-@erase "$(INTDIR)\Q.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\scorerep.obj"
	-@erase "$(INTDIR)\scorerep.sbr"
	-@erase "$(INTDIR)\tca.obj"
	-@erase "$(INTDIR)\tca.sbr"
	-@erase "$(INTDIR)\tcapw.obj"
	-@erase "$(INTDIR)\tcapw.sbr"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tlstuff.sbr"
	-@erase "$(INTDIR)\tserv.obj"
	-@erase "$(INTDIR)\tserv.sbr"
	-@erase "$(INTDIR)\tserva.obj"
	-@erase "$(INTDIR)\tserva.sbr"
	-@erase "$(OUTDIR)\dp2.bsc"
	-@erase "$(OUTDIR)\dp2d.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /Z7 /Od /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "_DEBUG" /D "DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /Fr /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /G5 /MDd /W3 /GX /Z7 /Od /I "..\..\..\h" /I "..\..\aeh" /I\
 "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "_DEBUG" /D "DEBUG" /D\
 "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /Fr"$(INTDIR)/"\
 /Fp"$(INTDIR)/dp2.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dp2d/
CPP_SBRS=.\dp2d/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dp2.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\aeh.sbr" \
	"$(INTDIR)\aehlog.sbr" \
	"$(INTDIR)\aehmap.sbr" \
	"$(INTDIR)\assoctab.sbr" \
	"$(INTDIR)\crypttab.sbr" \
	"$(INTDIR)\d3des.sbr" \
	"$(INTDIR)\delay.sbr" \
	"$(INTDIR)\deltas.sbr" \
	"$(INTDIR)\detect.sbr" \
	"$(INTDIR)\dp2.sbr" \
	"$(INTDIR)\dpcrc32.sbr" \
	"$(INTDIR)\Dpdllwin.sbr" \
	"$(INTDIR)\dpexcept.sbr" \
	"$(INTDIR)\dpio.sbr" \
	"$(INTDIR)\dpprivy.sbr" \
	"$(INTDIR)\dprint.sbr" \
	"$(INTDIR)\dpscore.sbr" \
	"$(INTDIR)\dpshutdn.sbr" \
	"$(INTDIR)\dptab.sbr" \
	"$(INTDIR)\dpuser.sbr" \
	"$(INTDIR)\dynatab.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\enumapp.sbr" \
	"$(INTDIR)\enumsrv2.sbr" \
	"$(INTDIR)\getdisp.sbr" \
	"$(INTDIR)\hkeytab.sbr" \
	"$(INTDIR)\ini.sbr" \
	"$(INTDIR)\launchap.sbr" \
	"$(INTDIR)\launchparams.sbr" \
	"$(INTDIR)\md5c.sbr" \
	"$(INTDIR)\mywcs.sbr" \
	"$(INTDIR)\ptimer.sbr" \
	"$(INTDIR)\Q.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\scorerep.sbr" \
	"$(INTDIR)\tca.sbr" \
	"$(INTDIR)\tcapw.sbr" \
	"$(INTDIR)\tlstuff.sbr" \
	"$(INTDIR)\tserv.sbr" \
	"$(INTDIR)\tserva.sbr"

"$(OUTDIR)\dp2.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"dp2d\dp2d.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dp2d.lib" 
LIB32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\assoctab.obj" \
	"$(INTDIR)\crypttab.obj" \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\deltas.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\dp2.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\Dpdllwin.obj" \
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
	"$(INTDIR)\Q.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\scorerep.obj" \
	"$(INTDIR)\tca.obj" \
	"$(INTDIR)\tcapw.obj" \
	"$(INTDIR)\tlstuff.obj" \
	"$(INTDIR)\tserv.obj" \
	"$(INTDIR)\tserva.obj"

"$(OUTDIR)\dp2d.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DP2___Wi"
# PROP BASE Intermediate_Dir "DP2___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dp2l"
# PROP Intermediate_Dir "dp2l"
# PROP Target_Dir ""
OUTDIR=.\dp2l
INTDIR=.\dp2l

ALL : "$(OUTDIR)\dp2l.lib"

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
	-@erase "$(INTDIR)\dp2.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\Dpdllwin.obj"
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
	-@erase "$(INTDIR)\Q.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\scorerep.obj"
	-@erase "$(INTDIR)\tca.obj"
	-@erase "$(INTDIR)\tcapw.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tserv.obj"
	-@erase "$(INTDIR)\tserva.obj"
	-@erase "$(OUTDIR)\dp2l.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /MD /W3 /GX /O2 /Oy- /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /Oy- /I "..\..\..\h" /I "..\..\aeh" /I "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /YX /c
CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /Oy- /I "..\..\..\h" /I "..\..\aeh" /I\
 "..\..\score" /I "..\..\..\demo\utils" /I "..\stub" /D "NDEBUG" /D "DPRNT" /D\
 "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /Fp"$(INTDIR)/dp2.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dp2l/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"dp2l/dp2l.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dp2l.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"dp2l\dp2l.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dp2l.lib" 
LIB32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\assoctab.obj" \
	"$(INTDIR)\crypttab.obj" \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\deltas.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\dp2.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\Dpdllwin.obj" \
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
	"$(INTDIR)\Q.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\scorerep.obj" \
	"$(INTDIR)\tca.obj" \
	"$(INTDIR)\tcapw.obj" \
	"$(INTDIR)\tlstuff.obj" \
	"$(INTDIR)\tserv.obj" \
	"$(INTDIR)\tserva.obj"

"$(OUTDIR)\dp2l.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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

# Name "DP2 - Win32 Release"
# Name "DP2 - Win32 Debug"
# Name "DP2 - Win32 Logging"

!IF  "$(CFG)" == "DP2 - Win32 Release"

!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Dpdllwin.c
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\Dpdllwin.obj" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


"$(INTDIR)\Dpdllwin.obj" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"

"$(INTDIR)\Dpdllwin.sbr" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\Dpdllwin.obj" : $(SOURCE) $(DEP_CPP_DPDLL) "$(INTDIR)"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\assoctab.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\assoctab.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\assoctab.sbr" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\assoctab.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dynatab.obj" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dynatab.obj" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dynatab.sbr" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dynatab.obj" : $(SOURCE) $(DEP_CPP_DYNAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\Q.c
DEP_CPP_Q_C6=\
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\Q.obj" : $(SOURCE) $(DEP_CPP_Q_C6) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Q.obj" : $(SOURCE) $(DEP_CPP_Q_C6) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Q.sbr" : $(SOURCE) $(DEP_CPP_Q_C6) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\Q.obj" : $(SOURCE) $(DEP_CPP_Q_C6) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dprint.obj" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dprint.obj" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dprint.sbr" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dprint.obj" : $(SOURCE) $(DEP_CPP_DPRIN) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ini.sbr" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\ini.obj" : $(SOURCE) $(DEP_CPP_INI_C) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpio.obj" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpio.obj" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpio.sbr" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dpio.obj" : $(SOURCE) $(DEP_CPP_DPIO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\delay.c
DEP_CPP_DELAY=\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dpmem.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\delay.obj" : $(SOURCE) $(DEP_CPP_DELAY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\delay.obj" : $(SOURCE) $(DEP_CPP_DELAY) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\delay.sbr" : $(SOURCE) $(DEP_CPP_DELAY) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\enumapp.obj" : $(SOURCE) $(DEP_CPP_ENUMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


"$(INTDIR)\enumapp.obj" : $(SOURCE) $(DEP_CPP_ENUMA) "$(INTDIR)"

"$(INTDIR)\enumapp.sbr" : $(SOURCE) $(DEP_CPP_ENUMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\launchap.obj" : $(SOURCE) $(DEP_CPP_LAUNC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


"$(INTDIR)\launchap.obj" : $(SOURCE) $(DEP_CPP_LAUNC) "$(INTDIR)"

"$(INTDIR)\launchap.sbr" : $(SOURCE) $(DEP_CPP_LAUNC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dp2.obj" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dp2.obj" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dp2.sbr" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dp2.obj" : $(SOURCE) $(DEP_CPP_DP2_C) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\hkeytab.obj" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\hkeytab.obj" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hkeytab.sbr" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\hkeytab.obj" : $(SOURCE) $(DEP_CPP_HKEYT) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dptab.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dptab.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dptab.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dptab.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\enumsrv2.obj" : $(SOURCE) $(DEP_CPP_ENUMS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\enumsrv2.obj" : $(SOURCE) $(DEP_CPP_ENUMS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\enumsrv2.sbr" : $(SOURCE) $(DEP_CPP_ENUMS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpprivy.sbr" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpexcept.obj" : $(SOURCE) $(DEP_CPP_DPEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpexcept.obj" : $(SOURCE) $(DEP_CPP_DPEXC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpexcept.sbr" : $(SOURCE) $(DEP_CPP_DPEXC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\aeh.sbr" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\aehlog.sbr" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\detect.c
DEP_CPP_DETEC=\
	"..\..\aeh\detect.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\detect.obj" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\detect.obj" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\detect.sbr" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\detect.obj" : $(SOURCE) $(DEP_CPP_DETEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\aeh\registry.c
DEP_CPP_REGIS=\
	"..\..\aeh\registry.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\registry.sbr" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\getdisp.obj" : $(SOURCE) $(DEP_CPP_GETDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\getdisp.obj" : $(SOURCE) $(DEP_CPP_GETDI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\getdisp.sbr" : $(SOURCE) $(DEP_CPP_GETDI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tlstuff.sbr" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpscore.obj" : $(SOURCE) $(DEP_CPP_DPSCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpscore.obj" : $(SOURCE) $(DEP_CPP_DPSCO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpscore.sbr" : $(SOURCE) $(DEP_CPP_DPSCO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\deltas.obj" : $(SOURCE) $(DEP_CPP_DELTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\deltas.obj" : $(SOURCE) $(DEP_CPP_DELTA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\deltas.sbr" : $(SOURCE) $(DEP_CPP_DELTA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\aehmap.sbr" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ptimer.sbr" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\tserv.obj" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tserv.obj" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tserv.sbr" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\tserv.obj" : $(SOURCE) $(DEP_CPP_TSERV) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\tca.obj" : $(SOURCE) $(DEP_CPP_TCA_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tca.obj" : $(SOURCE) $(DEP_CPP_TCA_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tca.sbr" : $(SOURCE) $(DEP_CPP_TCA_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\tcapw.obj" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tcapw.obj" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tcapw.sbr" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\tcapw.obj" : $(SOURCE) $(DEP_CPP_TCAPW) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\crypttab.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\crypttab.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\crypttab.sbr" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\crypttab.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\3rdparty\d3des\d3des.c
DEP_CPP_D3DES=\
	"..\..\aeh\..\3rdparty\d3des\d3des.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\d3des.obj" : $(SOURCE) $(DEP_CPP_D3DES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\d3des.obj" : $(SOURCE) $(DEP_CPP_D3DES) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\d3des.sbr" : $(SOURCE) $(DEP_CPP_D3DES) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\md5c.sbr" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_MD5C_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\mywcs.c
DEP_CPP_MYWCS=\
	"..\..\..\demo\utils\mywcs.h"\
	"..\..\..\h\aneterr.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\mywcs.obj" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\mywcs.obj" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mywcs.sbr" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\mywcs.obj" : $(SOURCE) $(DEP_CPP_MYWCS) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpuser.obj" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpuser.obj" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpuser.sbr" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dpuser.obj" : $(SOURCE) $(DEP_CPP_DPUSE) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\tserva.obj" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tserva.obj" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tserva.sbr" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\tserva.obj" : $(SOURCE) $(DEP_CPP_TSERVA) "$(INTDIR)"
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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\scorerep.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\scorerep.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\scorerep.sbr" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpshutdn.obj" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpshutdn.obj" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpshutdn.sbr" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\dpshutdn.obj" : $(SOURCE) $(DEP_CPP_DPSHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\dpcrc32.c
DEP_CPP_DPCRC=\
	"..\..\..\demo\utils\dpcrc32.h"\
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpcrc32.sbr" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


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
	

!IF  "$(CFG)" == "DP2 - Win32 Release"


"$(INTDIR)\launchparams.obj" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DP2 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\launchparams.obj" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\launchparams.sbr" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DP2 - Win32 Logging"


"$(INTDIR)\launchparams.obj" : $(SOURCE) $(DEP_CPP_LAUNCH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
