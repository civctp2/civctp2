# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=iotest - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to iotest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "iotest - Win32 Release" && "$(CFG)" != "iotest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "iotest.mak" CFG="iotest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iotest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "iotest - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "iotest - Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iotest - Win32 Release"

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

ALL : "$(OUTDIR)\iotest.exe"

CLEAN : 
	-@erase "$(INTDIR)\iotest.obj"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(OUTDIR)\iotest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "DPRNT" /D "dp_ANET2" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "DPRNT" /D "dp_ANET2"\
 /Fp"$(INTDIR)/iotest.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/iotest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/iotest.pdb" /machine:I386 /out:"$(OUTDIR)/iotest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\iotest.obj" \
	"$(INTDIR)\raw.obj" \
	"..\..\..\win\lib\dpd.lib"

"$(OUTDIR)\iotest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "iotest - Win32 Debug"

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

ALL : "$(OUTDIR)\iotest.exe" "$(OUTDIR)\iotest.bsc"

CLEAN : 
	-@erase "$(INTDIR)\iotest.obj"
	-@erase "$(INTDIR)\iotest.sbr"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\raw.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\iotest.bsc"
	-@erase "$(OUTDIR)\iotest.exe"
	-@erase "$(OUTDIR)\iotest.ilk"
	-@erase "$(OUTDIR)\iotest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "DPRNT" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "DPRNT" /D\
 "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/iotest.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/iotest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\iotest.sbr" \
	"$(INTDIR)\raw.sbr"

"$(OUTDIR)\iotest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:console /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/iotest.pdb" /debug /machine:I386 /out:"$(OUTDIR)/iotest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\iotest.obj" \
	"$(INTDIR)\raw.obj" \
	"..\..\..\win\lib\dpd.lib"

"$(OUTDIR)\iotest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "iotest - Win32 Release"
# Name "iotest - Win32 Debug"

!IF  "$(CFG)" == "iotest - Win32 Release"

!ELSEIF  "$(CFG)" == "iotest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\iotest.c
DEP_CPP_IOTES=\
	"..\..\..\demo\utils\raw.h"\
	"..\..\..\h\anet.h"\
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
	"..\..\..\h\q.h"\
	

!IF  "$(CFG)" == "iotest - Win32 Release"


"$(INTDIR)\iotest.obj" : $(SOURCE) $(DEP_CPP_IOTES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "iotest - Win32 Debug"


"$(INTDIR)\iotest.obj" : $(SOURCE) $(DEP_CPP_IOTES) "$(INTDIR)"

"$(INTDIR)\iotest.sbr" : $(SOURCE) $(DEP_CPP_IOTES) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\win\lib\dpd.lib

!IF  "$(CFG)" == "iotest - Win32 Release"

!ELSEIF  "$(CFG)" == "iotest - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\demo\utils\raw.c
DEP_CPP_RAW_C=\
	"..\..\..\demo\utils\raw.h"\
	"..\..\..\demo\utils\rawlin.c"\
	"..\..\..\demo\utils\rawmsdos.c"\
	"..\..\..\demo\utils\rawsun.c"\
	"..\..\..\demo\utils\rawvms.c"\
	"..\..\..\demo\utils\rawwin.c"\
	"..\..\..\demo\utils\unistd.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

!IF  "$(CFG)" == "iotest - Win32 Release"


"$(INTDIR)\raw.obj" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "iotest - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\raw.obj" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\raw.sbr" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
