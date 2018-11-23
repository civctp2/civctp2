# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=dictset - Win32 Debug Selftest
!MESSAGE No configuration specified.  Defaulting to dictset - Win32 Debug\
 Selftest.
!ENDIF 

!IF "$(CFG)" != "dictset - Win32 Debug Selftest" && "$(CFG)" !=\
 "dictset - Win32 Release Selftest"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dictset.mak" CFG="dictset - Win32 Debug Selftest"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dictset - Win32 Debug Selftest" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "dictset - Win32 Release Selftest" (based on\
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
# PROP Target_Last_Scanned "dictset - Win32 Debug Selftest"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "dictset - Win32 Debug Selftest"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Selftest"
# PROP BASE Intermediate_Dir "Debug Selftest"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug Selftest"
# PROP Intermediate_Dir "Debug Selftest"
# PROP Target_Dir ""
OUTDIR=.\Debug Selftest
INTDIR=.\Debug Selftest

ALL : "$(OUTDIR)\dictset.exe" "$(OUTDIR)\dictset.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dictset.obj"
	-@erase "$(INTDIR)\dictset.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dictset.bsc"
	-@erase "$(OUTDIR)\dictset.exe"
	-@erase "$(OUTDIR)\dictset.ilk"
	-@erase "$(OUTDIR)\dictset.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "DCST_SELFTEST" /FR /YX /c
# ADD CPP /nologo /Zp1 /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "DCST_SELFTEST" /D "DCST_ASSERTALL" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MLd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_CONSOLE" /D "DCST_SELFTEST" /D "DCST_ASSERTALL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/dictset.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=".\Debug Selftest/"
CPP_SBRS=".\Debug Selftest/"
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dictset.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dictset.sbr"

"$(OUTDIR)\dictset.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/dictset.pdb" /debug /machine:I386 /out:"$(OUTDIR)/dictset.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dictset.obj"

"$(OUTDIR)\dictset.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dictset - Win32 Release Selftest"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release Selftest"
# PROP BASE Intermediate_Dir "Release Selftest"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release Selftest"
# PROP Intermediate_Dir "Release Selftest"
# PROP Target_Dir ""
OUTDIR=.\Release Selftest
INTDIR=.\Release Selftest

ALL : "$(OUTDIR)\dictset.exe" "$(OUTDIR)\dictset.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dictset.obj"
	-@erase "$(INTDIR)\dictset.sbr"
	-@erase "$(OUTDIR)\dictset.bsc"
	-@erase "$(OUTDIR)\dictset.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /FR /YX /c
# ADD CPP /nologo /Zp1 /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "DCST_SELFTEST" /D "DCST_ASSERTALL" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /ML /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "DCST_SELFTEST" /D "DCST_ASSERTALL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/dictset.pch"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=".\Release Selftest/"
CPP_SBRS=".\Release Selftest/"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dictset.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dictset.sbr"

"$(OUTDIR)\dictset.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/dictset.pdb" /machine:I386 /out:"$(OUTDIR)/dictset.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dictset.obj"

"$(OUTDIR)\dictset.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "dictset - Win32 Debug Selftest"
# Name "dictset - Win32 Release Selftest"

!IF  "$(CFG)" == "dictset - Win32 Debug Selftest"

!ELSEIF  "$(CFG)" == "dictset - Win32 Release Selftest"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dictset.c

"$(INTDIR)\dictset.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\dictset.sbr" : $(SOURCE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
