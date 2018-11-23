# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=rawtest - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to rawtest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "rawtest - Win32 Release" && "$(CFG)" !=\
 "rawtest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "rawtest.mak" CFG="rawtest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rawtest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "rawtest - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "rawtest - Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rawtest - Win32 Release"

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

ALL : "$(OUTDIR)\rawtest.exe"

CLEAN : 
	-@erase "$(INTDIR)\rawtest.obj"
	-@erase "$(INTDIR)\rawwin.obj"
	-@erase "$(OUTDIR)\rawtest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../h" /I "../../../demo/utils" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_NTSDK" /D "DPRNT" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../../../h" /I "../../../demo/utils" /D\
 "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_NTSDK" /D "DPRNT"\
 /Fp"$(INTDIR)/rawtest.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rawtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib libcd.lib /nologo /subsystem:console /machine:I386 /nodefaultlib
# SUBTRACT LINK32 /map
LINK32_FLAGS=kernel32.lib libcd.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/rawtest.pdb" /machine:I386 /nodefaultlib\
 /out:"$(OUTDIR)/rawtest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\rawtest.obj" \
	"$(INTDIR)\rawwin.obj" \
	"..\..\..\win\lib\dp.lib"

"$(OUTDIR)\rawtest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "rawtest - Win32 Debug"

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

ALL : "$(OUTDIR)\rawtest.exe"

CLEAN : 
	-@erase "$(INTDIR)\rawtest.obj"
	-@erase "$(INTDIR)\rawwin.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\rawtest.exe"
	-@erase "$(OUTDIR)\rawtest.ilk"
	-@erase "$(OUTDIR)\rawtest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../h" /I "../../../demo/utils" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_NTSDK" /D "DPRNT" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "../../../h" /I\
 "../../../demo/utils" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_NTSDK" /D\
 "DPRNT" /Fp"$(INTDIR)/rawtest.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rawtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib libcd.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib
# SUBTRACT LINK32 /map
LINK32_FLAGS=kernel32.lib libcd.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/rawtest.pdb" /debug /machine:I386 /nodefaultlib\
 /out:"$(OUTDIR)/rawtest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\rawtest.obj" \
	"$(INTDIR)\rawwin.obj" \
	"..\..\..\win\lib\dp.lib"

"$(OUTDIR)\rawtest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "rawtest - Win32 Release"
# Name "rawtest - Win32 Debug"

!IF  "$(CFG)" == "rawtest - Win32 Release"

!ELSEIF  "$(CFG)" == "rawtest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\rawtest.c
DEP_CPP_RAWTE=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\q.h"\
	".\../../../demo/utils\raw.h"\
	".\../../../demo/utils\stricmp.h"\
	".\../../../h\dp.h"\
	".\../../../h\dprint.h"\
	

"$(INTDIR)\rawtest.obj" : $(SOURCE) $(DEP_CPP_RAWTE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\win\lib\dp.lib

!IF  "$(CFG)" == "rawtest - Win32 Release"

!ELSEIF  "$(CFG)" == "rawtest - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\demo\utils\rawwin.c

"$(INTDIR)\rawwin.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
