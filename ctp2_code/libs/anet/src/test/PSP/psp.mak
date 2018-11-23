# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=PSP - Win32 v2 dll debug
!MESSAGE No configuration specified.  Defaulting to PSP - Win32 v2 dll debug.
!ENDIF 

!IF "$(CFG)" != "PSP - Win32 Release" && "$(CFG)" != "PSP - Win32 Debug" &&\
 "$(CFG)" != "PSP - Win32 dll debug" && "$(CFG)" != "PSP - Win32 dll Release" &&\
 "$(CFG)" != "PSP - Win32 v2 dll debug" && "$(CFG)" !=\
 "PSP - Win32 v2 dll Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "psp.mak" CFG="PSP - Win32 v2 dll debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PSP - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PSP - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "PSP - Win32 dll debug" (based on "Win32 (x86) Console Application")
!MESSAGE "PSP - Win32 dll Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PSP - Win32 v2 dll debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "PSP - Win32 v2 dll Release" (based on\
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
# PROP Target_Last_Scanned "PSP - Win32 dll Release"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "PSP - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PSPr"
# PROP Intermediate_Dir "PSPr"
# PROP Target_Dir ""
OUTDIR=.\PSPr
INTDIR=.\PSPr

ALL : "$(OUTDIR)\PSPr.exe" "$(OUTDIR)\psp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\PSP.obj"
	-@erase "$(INTDIR)\PSP.sbr"
	-@erase "$(OUTDIR)\psp.bsc"
	-@erase "$(OUTDIR)\PSPr.exe"
	-@erase "$(OUTDIR)\PSPr.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /FR /YX /c
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/psp.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PSPr/
CPP_SBRS=.\PSPr/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/psp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\PSP.sbr"

"$(OUTDIR)\psp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ..\..\..\win\lib\dp.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"PSPr/PSPr.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\dp.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/PSPr.pdb"\
 /map:"$(INTDIR)/PSPr.map" /machine:I386 /out:"$(OUTDIR)/PSPr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\PSP.obj"

"$(OUTDIR)\PSPr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PSP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PSPd"
# PROP Intermediate_Dir "PSPd"
# PROP Target_Dir ""
OUTDIR=.\PSPd
INTDIR=.\PSPd

ALL : "$(OUTDIR)\PSPd.exe" "$(OUTDIR)\psp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\PSP.obj"
	-@erase "$(INTDIR)\PSP.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\psp.bsc"
	-@erase "$(OUTDIR)\PSPd.exe"
	-@erase "$(OUTDIR)\PSPd.map"
	-@erase "$(OUTDIR)\PSPd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D\
 "DPRNT" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/psp.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PSPd/
CPP_SBRS=.\PSPd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/psp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\PSP.sbr"

"$(OUTDIR)\psp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 ..\..\..\win\lib\dpd.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"PSPd/PSPd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\dpd.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/PSPd.pdb"\
 /map:"$(INTDIR)/PSPd.map" /debug /machine:I386 /out:"$(OUTDIR)/PSPd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\PSP.obj"

"$(OUTDIR)\PSPd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PSP - Win32 dll debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PSP__0"
# PROP BASE Intermediate_Dir "PSP__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PSPdd"
# PROP Intermediate_Dir "PSPdd"
# PROP Target_Dir ""
OUTDIR=.\PSPdd
INTDIR=.\PSPdd

ALL : "$(OUTDIR)\PSPdd.exe" "$(OUTDIR)\psp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\PSP.obj"
	-@erase "$(INTDIR)\PSP.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\psp.bsc"
	-@erase "$(OUTDIR)\PSPdd.exe"
	-@erase "$(OUTDIR)\PSPdd.map"
	-@erase "$(OUTDIR)\PSPdd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "DPRNT" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D\
 "DPRNT" /D "DP_DLL" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/psp.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PSPdd/
CPP_SBRS=.\PSPdd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/psp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\PSP.sbr"

"$(OUTDIR)\psp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib ..\..\..\win\lib\anetdll.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"Debug/PSPd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"PSPdd/PSPdd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/PSPdd.pdb"\
 /map:"$(INTDIR)/PSPdd.map" /debug /machine:I386 /out:"$(OUTDIR)/PSPdd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\PSP.obj"

"$(OUTDIR)\PSPdd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PSP - Win32 dll Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PSP___"
# PROP BASE Intermediate_Dir "PSP___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PSPdr"
# PROP Intermediate_Dir "PSPdr"
# PROP Target_Dir ""
OUTDIR=.\PSPdr
INTDIR=.\PSPdr

ALL : "$(OUTDIR)\PSPdr.exe" "$(OUTDIR)\psp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\PSP.obj"
	-@erase "$(INTDIR)\PSP.sbr"
	-@erase "$(OUTDIR)\psp.bsc"
	-@erase "$(OUTDIR)\PSPdr.exe"
	-@erase "$(OUTDIR)\PSPdr.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MLd /W3 /GX /O2 /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR /YX /c
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/psp.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PSPdr/
CPP_SBRS=.\PSPdr/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/psp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\PSP.sbr"

"$(OUTDIR)\psp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib ..\..\..\win\lib\dp.lib /nologo /subsystem:console /map /machine:I386
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anetdll.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"PSPdr/PSPdr.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anetdll.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/PSPdr.pdb"\
 /map:"$(INTDIR)/PSPdr.map" /machine:I386 /out:"$(OUTDIR)/PSPdr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\PSP.obj"

"$(OUTDIR)\PSPdr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PSP___Wi"
# PROP BASE Intermediate_Dir "PSP___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PSP2dd"
# PROP Intermediate_Dir "PSP2dd"
# PROP Target_Dir ""
OUTDIR=.\PSP2dd
INTDIR=.\PSP2dd

ALL : "$(OUTDIR)\PSP2dd.exe" "$(OUTDIR)\psp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\PSP.obj"
	-@erase "$(INTDIR)\PSP.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\psp.bsc"
	-@erase "$(OUTDIR)\PSP2dd.exe"
	-@erase "$(OUTDIR)\PSP2dd.map"
	-@erase "$(OUTDIR)\PSP2dd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "dp_ANET2" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D\
 "_CONSOLE" /D "DP_DLL" /D "dp_ANET2" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/psp.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PSP2dd/
CPP_SBRS=.\PSP2dd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/psp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\PSP.sbr"

"$(OUTDIR)\psp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 ..\..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"PSPdd/PSPdd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"PSP2dd/PSP2dd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/PSP2dd.pdb"\
 /map:"$(INTDIR)/PSP2dd.map" /debug /machine:I386 /out:"$(OUTDIR)/PSP2dd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\PSP.obj"

"$(OUTDIR)\PSP2dd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PSP___Wi"
# PROP BASE Intermediate_Dir "PSP___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PSP2dr"
# PROP Intermediate_Dir "PSP2dr"
# PROP Target_Dir ""
OUTDIR=.\PSP2dr
INTDIR=.\PSP2dr

ALL : "$(OUTDIR)\PSP2dr.exe" "$(OUTDIR)\psp.bsc"

CLEAN : 
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\PSP.obj"
	-@erase "$(INTDIR)\PSP.sbr"
	-@erase "$(OUTDIR)\psp.bsc"
	-@erase "$(OUTDIR)\PSP2dr.exe"
	-@erase "$(OUTDIR)\PSP2dr.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR /YX /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "dp_ANET2"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/psp.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PSP2dr/
CPP_SBRS=.\PSP2dr/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/psp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\PSP.sbr"

"$(OUTDIR)\psp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 ..\..\..\win\lib\anetdll.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"PSPdr/PSPdr.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\..\win\lib\anet2.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"PSP2dr/PSP2dr.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=..\..\..\win\lib\anet2.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/PSP2dr.pdb"\
 /map:"$(INTDIR)/PSP2dr.map" /machine:I386 /out:"$(OUTDIR)/PSP2dr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\PSP.obj"

"$(OUTDIR)\PSP2dr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "PSP - Win32 Release"
# Name "PSP - Win32 Debug"
# Name "PSP - Win32 dll debug"
# Name "PSP - Win32 dll Release"
# Name "PSP - Win32 v2 dll debug"
# Name "PSP - Win32 v2 dll Release"

!IF  "$(CFG)" == "PSP - Win32 Release"

!ELSEIF  "$(CFG)" == "PSP - Win32 Debug"

!ELSEIF  "$(CFG)" == "PSP - Win32 dll debug"

!ELSEIF  "$(CFG)" == "PSP - Win32 dll Release"

!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll debug"

!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\PSP.c
DEP_CPP_PSP_C=\
	"..\..\..\demo\utils\stricmp.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "PSP - Win32 Release"


"$(INTDIR)\PSP.obj" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"

"$(INTDIR)\PSP.sbr" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PSP - Win32 Debug"


"$(INTDIR)\PSP.obj" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"

"$(INTDIR)\PSP.sbr" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PSP - Win32 dll debug"


"$(INTDIR)\PSP.obj" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"

"$(INTDIR)\PSP.sbr" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PSP - Win32 dll Release"


"$(INTDIR)\PSP.obj" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"

"$(INTDIR)\PSP.sbr" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll debug"


"$(INTDIR)\PSP.obj" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"

"$(INTDIR)\PSP.sbr" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll Release"


"$(INTDIR)\PSP.obj" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"

"$(INTDIR)\PSP.sbr" : $(SOURCE) $(DEP_CPP_PSP_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "PSP - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PSP - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PSP - Win32 dll debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PSP - Win32 dll Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "PSP - Win32 v2 dll Release"


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
