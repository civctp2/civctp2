# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=load - Win32 v2 dll Release
!MESSAGE No configuration specified.  Defaulting to load - Win32 v2 dll\
 Release.
!ENDIF 

!IF "$(CFG)" != "load - Win32 dll debug" && "$(CFG)" !=\
 "load - Win32 dll Release" && "$(CFG)" != "load - Win32 v2 dll debug" &&\
 "$(CFG)" != "load - Win32 v2 dll Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "load.mak" CFG="load - Win32 v2 dll Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "load - Win32 dll debug" (based on "Win32 (x86) Console Application")
!MESSAGE "load - Win32 dll Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "load - Win32 v2 dll debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "load - Win32 v2 dll Release" (based on\
 "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "load - Win32 dll Release"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "load - Win32 dll debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "load__0"
# PROP BASE Intermediate_Dir "load__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "loaddd"
# PROP Intermediate_Dir "loaddd"
# PROP Target_Dir ""
OUTDIR=.\loaddd
INTDIR=.\loaddd

ALL : "$(OUTDIR)\loaddd.exe" "$(OUTDIR)\load.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\load.obj"
	-@erase "$(INTDIR)\load.sbr"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\ptimer.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\load.bsc"
	-@erase "$(OUTDIR)\loaddd.exe"
	-@erase "$(OUTDIR)\loaddd.map"
	-@erase "$(OUTDIR)\loaddd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "DPRNT" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D\
 "DPRNT" /D "DP_DLL" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/load.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\loaddd/
CPP_SBRS=.\loaddd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/load.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\load.sbr" \
	"$(INTDIR)\ptimer.sbr"

"$(OUTDIR)\load.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib ..\..\..\win\lib\anetdll.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"Debug/loadd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"loaddd/loaddd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/loaddd.pdb"\
 /map:"$(INTDIR)/loaddd.map" /debug /machine:I386 /out:"$(OUTDIR)/loaddd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\load.obj" \
	"$(INTDIR)\ptimer.obj"

"$(OUTDIR)\loaddd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "load - Win32 dll Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "load___"
# PROP BASE Intermediate_Dir "load___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "loaddr"
# PROP Intermediate_Dir "loaddr"
# PROP Target_Dir ""
OUTDIR=.\loaddr
INTDIR=.\loaddr

ALL : "$(OUTDIR)\loaddr.exe" "$(OUTDIR)\load.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\load.obj"
	-@erase "$(INTDIR)\load.sbr"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\ptimer.sbr"
	-@erase "$(OUTDIR)\load.bsc"
	-@erase "$(OUTDIR)\loaddr.exe"
	-@erase "$(OUTDIR)\loaddr.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MLd /W3 /GX /O2 /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/load.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\loaddr/
CPP_SBRS=.\loaddr/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/load.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\load.sbr" \
	"$(INTDIR)\ptimer.sbr"

"$(OUTDIR)\load.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib ..\..\..\win\lib\dp.lib /nologo /subsystem:console /map /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anetdll.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"loaddr/loaddr.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anetdll.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/loaddr.pdb"\
 /map:"$(INTDIR)/loaddr.map" /machine:I386 /out:"$(OUTDIR)/loaddr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\load.obj" \
	"$(INTDIR)\ptimer.obj"

"$(OUTDIR)\loaddr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "load___Wi"
# PROP BASE Intermediate_Dir "load___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "load2dd"
# PROP Intermediate_Dir "load2dd"
# PROP Target_Dir ""
OUTDIR=.\load2dd
INTDIR=.\load2dd

ALL : "$(OUTDIR)\load2dd.exe" "$(OUTDIR)\load.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\load.obj"
	-@erase "$(INTDIR)\load.sbr"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\ptimer.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\load.bsc"
	-@erase "$(OUTDIR)\load2dd.exe"
	-@erase "$(OUTDIR)\load2dd.map"
	-@erase "$(OUTDIR)\load2dd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "dp_ANET2" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D\
 "_CONSOLE" /D "DP_DLL" /D "dp_ANET2" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/load.pch"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\load2dd/
CPP_SBRS=.\load2dd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/load.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\load.sbr" \
	"$(INTDIR)\ptimer.sbr"

"$(OUTDIR)\load.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 ..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"loaddd/loaddd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"load2dd/load2dd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/load2dd.pdb"\
 /map:"$(INTDIR)/load2dd.map" /debug /machine:I386 /out:"$(OUTDIR)/load2dd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\load.obj" \
	"$(INTDIR)\ptimer.obj"

"$(OUTDIR)\load2dd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "load___0"
# PROP BASE Intermediate_Dir "load___0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "load2dr"
# PROP Intermediate_Dir "load2dr"
# PROP Target_Dir ""
OUTDIR=.\load2dr
INTDIR=.\load2dr

ALL : "$(OUTDIR)\load2dr.exe" "$(OUTDIR)\load.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\load.obj"
	-@erase "$(INTDIR)\load.sbr"
	-@erase "$(INTDIR)\ptimer.obj"
	-@erase "$(INTDIR)\ptimer.sbr"
	-@erase "$(OUTDIR)\load.bsc"
	-@erase "$(OUTDIR)\load2dr.exe"
	-@erase "$(OUTDIR)\load2dr.map"
	-@erase "$(OUTDIR)\load2dr.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "dp_ANET2" /Fr /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "dp_ANET2"\
 /Fr"$(INTDIR)/" /Fp"$(INTDIR)/load.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\load2dr/
CPP_SBRS=.\load2dr/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/load.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\load.sbr" \
	"$(INTDIR)\ptimer.sbr"

"$(OUTDIR)\load.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 ..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"loaddd/loaddd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anet2.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"load2dr/load2dr.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anet2.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/load2dr.pdb"\
 /map:"$(INTDIR)/load2dr.map" /debug /machine:I386 /out:"$(OUTDIR)/load2dr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\load.obj" \
	"$(INTDIR)\ptimer.obj"

"$(OUTDIR)\load2dr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "load - Win32 dll debug"
# Name "load - Win32 dll Release"
# Name "load - Win32 v2 dll debug"
# Name "load - Win32 v2 dll Release"

!IF  "$(CFG)" == "load - Win32 dll debug"

!ELSEIF  "$(CFG)" == "load - Win32 dll Release"

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll debug"

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\load.c
DEP_CPP_LOAD_=\
	"..\..\..\demo\utils\stricmp.h"\
	"..\..\..\demo\utils\unistd.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\eclock.h"\
	"..\..\dp\ptimer.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

!IF  "$(CFG)" == "load - Win32 dll debug"


"$(INTDIR)\load.obj" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"

"$(INTDIR)\load.sbr" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "load - Win32 dll Release"


"$(INTDIR)\load.obj" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"

"$(INTDIR)\load.sbr" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "load - Win32 v2 dll debug"


"$(INTDIR)\load.obj" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"

"$(INTDIR)\load.sbr" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "load - Win32 v2 dll Release"


"$(INTDIR)\load.obj" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"

"$(INTDIR)\load.sbr" : $(SOURCE) $(DEP_CPP_LOAD_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\ptimer.c
DEP_CPP_PTIME=\
	"..\..\..\h\eclock.h"\
	"..\..\dp\ptimer.h"\
	

!IF  "$(CFG)" == "load - Win32 dll debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ptimer.sbr" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "load - Win32 dll Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ptimer.sbr" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ptimer.sbr" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ptimer.obj" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ptimer.sbr" : $(SOURCE) $(DEP_CPP_PTIME) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "load - Win32 dll debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "load - Win32 dll Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "load - Win32 v2 dll Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
