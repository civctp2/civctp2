# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=sertest - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to sertest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "sertest - Win32 Release" && "$(CFG)" !=\
 "sertest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "sertest.mak" CFG="sertest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sertest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sertest - Win32 Debug" (based on "Win32 (x86) Console Application")
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
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sertest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "sertest_"
# PROP BASE Intermediate_Dir "sertest_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "sertest_"
# PROP Intermediate_Dir "sertest_"
# PROP Target_Dir ""
OUTDIR=.\sertest_
INTDIR=.\sertest_

ALL : "$(OUTDIR)\sertest.exe"

CLEAN : 
	-@erase "$(INTDIR)\ser.obj"
	-@erase "$(INTDIR)\Sertest.obj"
	-@erase "$(OUTDIR)\sertest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/sertest.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\sertest_/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/sertest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/sertest.pdb" /machine:I386 /out:"$(OUTDIR)/sertest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ser.obj" \
	"$(INTDIR)\Sertest.obj"

"$(OUTDIR)\sertest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sertest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sertest0"
# PROP BASE Intermediate_Dir "sertest0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "sertest0"
# PROP Intermediate_Dir "sertest0"
# PROP Target_Dir ""
OUTDIR=.\sertest0
INTDIR=.\sertest0

ALL : ".\sertestd\sertest.exe"

CLEAN : 
	-@erase "$(INTDIR)\ser.obj"
	-@erase "$(INTDIR)\Sertest.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\sertest.pdb"
	-@erase ".\sertestd\sertest.exe"
	-@erase ".\sertestd\sertest.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I\
 "..\..\..\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/sertest.pch"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\sertest0/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/sertest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:console /debug /machine:I386 /out:"sertestd/sertest.exe"
LINK32_FLAGS=kernel32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/sertest.pdb" /debug /machine:I386 /out:"sertestd/sertest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ser.obj" \
	"$(INTDIR)\Sertest.obj"

".\sertestd\sertest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "sertest - Win32 Release"
# Name "sertest - Win32 Debug"

!IF  "$(CFG)" == "sertest - Win32 Release"

!ELSEIF  "$(CFG)" == "sertest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Sertest.c

!IF  "$(CFG)" == "sertest - Win32 Release"

NODEP_CPP_SERTE=\
	".\ser.h"\
	

"$(INTDIR)\Sertest.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "sertest - Win32 Debug"

DEP_CPP_SERTE=\
	"..\..\..\h\commerr.h"\
	"..\main\ser.h"\
	

"$(INTDIR)\Sertest.obj" : $(SOURCE) $(DEP_CPP_SERTE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ser.c

!IF  "$(CFG)" == "sertest - Win32 Release"

DEP_CPP_SER_C=\
	"..\main\ser.h"\
	
NODEP_CPP_SER_C=\
	"..\main\commerr.h"\
	"..\main\ddprint.h"\
	"..\main\dpmem.h"\
	

"$(INTDIR)\ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sertest - Win32 Debug"

DEP_CPP_SER_C=\
	"..\..\..\h\dpmem.h"\
	"..\h\ddprint.h"\
	"..\main\ser.h"\
	
NODEP_CPP_SER_C=\
	"..\main\commerr.h"\
	

"$(INTDIR)\ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
