# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=bench - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to bench - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "bench - Win32 Release" && "$(CFG)" != "bench - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "bench.mak" CFG="bench - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bench - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "bench - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "bench - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "bench - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\bench.exe"

CLEAN : 
	-@erase "$(INTDIR)\bench.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\prog.obj"
	-@erase "$(OUTDIR)\bench.exe"
	-@erase "$(OUTDIR)\bench.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\h" /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /Fp"$(INTDIR)/bench.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/bench.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ..\..\..\win\lib\anet2.lib /nologo /subsystem:console /map /machine:I386
LINK32_FLAGS=..\..\..\win\lib\anet2.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/bench.pdb" /map:"$(INTDIR)/bench.map"\
 /machine:I386 /out:"$(OUTDIR)/bench.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bench.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\prog.obj"

"$(OUTDIR)\bench.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "bench - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\bench.exe"

CLEAN : 
	-@erase "$(INTDIR)\bench.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\prog.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\bench.exe"
	-@erase "$(OUTDIR)\bench.ilk"
	-@erase "$(OUTDIR)\bench.map"
	-@erase "$(OUTDIR)\bench.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /D "WIN32" /D\
 "_DEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/bench.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/bench.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 ..\..\..\win\lib\anet2d.lib /nologo /subsystem:console /map /debug /machine:I386
LINK32_FLAGS=..\..\..\win\lib\anet2d.lib /nologo /subsystem:console\
 /incremental:yes /pdb:"$(OUTDIR)/bench.pdb" /map:"$(INTDIR)/bench.map" /debug\
 /machine:I386 /out:"$(OUTDIR)/bench.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bench.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\prog.obj"

"$(OUTDIR)\bench.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "bench - Win32 Release"
# Name "bench - Win32 Debug"

!IF  "$(CFG)" == "bench - Win32 Release"

!ELSEIF  "$(CFG)" == "bench - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\bench.c
DEP_CPP_BENCH=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\eclock.h"\
	".\prog.h"\
	

"$(INTDIR)\bench.obj" : $(SOURCE) $(DEP_CPP_BENCH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\prog.c
DEP_CPP_PROG_=\
	".\prog.h"\
	

"$(INTDIR)\prog.obj" : $(SOURCE) $(DEP_CPP_PROG_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
