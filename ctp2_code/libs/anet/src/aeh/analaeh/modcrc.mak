# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=modcrc - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to modcrc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "modcrc - Win32 Release" && "$(CFG)" != "modcrc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "modcrc.mak" CFG="modcrc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "modcrc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "modcrc - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "modcrc - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "modcrc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "modcrc"
# PROP Intermediate_Dir "modcrc"
# PROP Target_Dir ""
OUTDIR=.\modcrc
INTDIR=.\modcrc

ALL : "$(OUTDIR)\modcrc.exe"

CLEAN : 
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\modcrc.obj"
	-@erase "$(OUTDIR)\modcrc.exe"
	-@erase "$(OUTDIR)\modcrc.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I\
 ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2"\
 /Fp"$(INTDIR)/modcrc.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\modcrc/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modcrc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/modcrc.pdb" /map:"$(INTDIR)/modcrc.map"\
 /machine:I386 /out:"$(OUTDIR)/modcrc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\modcrc.obj"

"$(OUTDIR)\modcrc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modcrc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "modcrcd"
# PROP Intermediate_Dir "modcrcd"
# PROP Target_Dir ""
OUTDIR=.\modcrcd
INTDIR=.\modcrcd

ALL : "$(OUTDIR)\modcrcd.exe" "$(OUTDIR)\modcrcd.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpcrc32.sbr"
	-@erase "$(INTDIR)\modcrc.obj"
	-@erase "$(INTDIR)\modcrc.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\modcrcd.bsc"
	-@erase "$(OUTDIR)\modcrcd.exe"
	-@erase "$(OUTDIR)\modcrcd.map"
	-@erase "$(OUTDIR)\modcrcd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /FR /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "_WINDOWS" /D "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/modcrc.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\modcrcd/
CPP_SBRS=.\modcrcd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"modcrcd/modcrcd.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/modcrcd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dpcrc32.sbr" \
	"$(INTDIR)\modcrc.sbr"

"$(OUTDIR)\modcrcd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"modcrcd/modcrcd.exe"
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/modcrcd.pdb" /map:"$(INTDIR)/modcrcd.map"\
 /debug /machine:I386 /out:"$(OUTDIR)/modcrcd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\modcrc.obj"

"$(OUTDIR)\modcrcd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "modcrc - Win32 Release"
# Name "modcrc - Win32 Debug"

!IF  "$(CFG)" == "modcrc - Win32 Release"

!ELSEIF  "$(CFG)" == "modcrc - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\dpcrc32.c
DEP_CPP_DPCRC=\
	"..\..\..\demo\utils\dpcrc32.h"\
	

!IF  "$(CFG)" == "modcrc - Win32 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modcrc - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpcrc32.sbr" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\modcrc.c
DEP_CPP_MODCR=\
	"..\..\..\demo\utils\dpcrc32.h"\
	"..\aeh.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "modcrc - Win32 Release"


"$(INTDIR)\modcrc.obj" : $(SOURCE) $(DEP_CPP_MODCR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modcrc - Win32 Debug"


"$(INTDIR)\modcrc.obj" : $(SOURCE) $(DEP_CPP_MODCR) "$(INTDIR)"

"$(INTDIR)\modcrc.sbr" : $(SOURCE) $(DEP_CPP_MODCR) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
