# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=DPTab - Win32 v2 loop Debug
!MESSAGE No configuration specified.  Defaulting to DPTab - Win32 v2 loop\
 Debug.
!ENDIF 

!IF "$(CFG)" != "DPTab - Win32 Release" && "$(CFG)" != "DPTab - Win32 Debug" &&\
 "$(CFG)" != "DPTab - Win32 v2 Inet Debug" && "$(CFG)" !=\
 "DPTab - Win32 v2 loop Debug" && "$(CFG)" != "DPTab - Win32 v2 Inet Release" &&\
 "$(CFG)" != "DPTab - Win32 v2 loop Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "test.mak" CFG="DPTab - Win32 v2 loop Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DPTab - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DPTab - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "DPTab - Win32 v2 Inet Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "DPTab - Win32 v2 loop Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "DPTab - Win32 v2 Inet Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "DPTab - Win32 v2 loop Release" (based on\
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
# PROP Target_Last_Scanned "DPTab - Win32 v2 loop Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "DPTab - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dptabtr"
# PROP Intermediate_Dir "dptabtr"
# PROP Target_Dir ""
OUTDIR=.\dptabtr
INTDIR=.\dptabtr

ALL : "$(OUTDIR)\dptabtr.exe" "$(OUTDIR)\test.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dptabt.obj"
	-@erase "$(INTDIR)\dptabt.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(OUTDIR)\dptabtr.exe"
	-@erase "$(OUTDIR)\dptabtr.map"
	-@erase "$(OUTDIR)\test.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /FR /YX /c
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/test.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dptabtr/
CPP_SBRS=.\dptabtr/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dptabt.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\unistd.sbr"

"$(OUTDIR)\test.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ..\..\..\win\lib\dp.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"dptabtr/dptabtr.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\dp.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dptabtr.pdb"\
 /map:"$(INTDIR)/dptabtr.map" /machine:I386 /out:"$(OUTDIR)/dptabtr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dptabt.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\dptabtr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DPTab - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dptabtd"
# PROP Intermediate_Dir "dptabtd"
# PROP Target_Dir ""
OUTDIR=.\dptabtd
INTDIR=.\dptabtd

ALL : "$(OUTDIR)\dptabtd.exe" "$(OUTDIR)\test.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dptabt.obj"
	-@erase "$(INTDIR)\dptabt.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dptabtd.exe"
	-@erase "$(OUTDIR)\dptabtd.map"
	-@erase "$(OUTDIR)\dptabtd.pdb"
	-@erase "$(OUTDIR)\test.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D\
 "DPRNT" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/test.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dptabtd/
CPP_SBRS=.\dptabtd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dptabt.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\unistd.sbr"

"$(OUTDIR)\test.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 ..\..\..\win\lib\dpd.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"dptabtd/dptabtd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\..\win\lib\dpd.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dptabtd.pdb"\
 /map:"$(INTDIR)/dptabtd.map" /debug /machine:I386 /out:"$(OUTDIR)/dptabtd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dptabt.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\dptabtd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DPTab__1"
# PROP BASE Intermediate_Dir "DPTab__1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dptabt2id"
# PROP Intermediate_Dir "dptabt2id"
# PROP Target_Dir ""
OUTDIR=.\dptabt2id
INTDIR=.\dptabt2id

ALL : "$(OUTDIR)\dptabt2id.exe" "$(OUTDIR)\dptabt2id.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dptabt.obj"
	-@erase "$(INTDIR)\dptabt.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dptabt2id.bsc"
	-@erase "$(OUTDIR)\dptabt2id.exe"
	-@erase "$(OUTDIR)\dptabt2id.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "INTERNET_ADDRESS" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D\
 "_CONSOLE" /D "DP_DLL" /D "INTERNET_ADDRESS" /Fr"$(INTDIR)/"\
 /Fp"$(INTDIR)/test.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dptabt2id/
CPP_SBRS=.\dptabt2id/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"dptabt2id/dptabt2id.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dptabt2id.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dptabt.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\unistd.sbr"

"$(OUTDIR)\dptabt2id.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /profile /map /debug /machine:I386 /out:"dptabt2dd/dptabt2dd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /profile /map /debug /machine:I386 /out:"dptabt2id/dptabt2id.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo\
 /subsystem:console /profile /map:"$(INTDIR)/dptabt2id.map" /debug /machine:I386\
 /out:"$(OUTDIR)/dptabt2id.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dptabt.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\dptabt2id.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DPTab__2"
# PROP BASE Intermediate_Dir "DPTab__2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dptabt2ld"
# PROP Intermediate_Dir "dptabt2ld"
# PROP Target_Dir ""
OUTDIR=.\dptabt2ld
INTDIR=.\dptabt2ld

ALL : "$(OUTDIR)\dptabt2ld.exe" "$(OUTDIR)\dptabt2ld.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dptabt.obj"
	-@erase "$(INTDIR)\dptabt.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dptabt2ld.bsc"
	-@erase "$(OUTDIR)\dptabt2ld.exe"
	-@erase "$(OUTDIR)\dptabt2ld.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /Fr /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "LOOPBACK_ADDRESS" /Fr /YX /c
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WIN32" /D\
 "_CONSOLE" /D "DP_DLL" /D "LOOPBACK_ADDRESS" /Fr"$(INTDIR)/"\
 /Fp"$(INTDIR)/test.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dptabt2ld/
CPP_SBRS=.\dptabt2ld/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"dptabt2ld/dptabt2ld.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dptabt2ld.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dptabt.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\unistd.sbr"

"$(OUTDIR)\dptabt2ld.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /profile /map /debug /machine:I386 /out:"dptabt2dd/dptabt2dd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /profile /map /debug /machine:I386 /out:"dptabt2ld/dptabt2ld.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo\
 /subsystem:console /profile /map:"$(INTDIR)/dptabt2ld.map" /debug /machine:I386\
 /out:"$(OUTDIR)/dptabt2ld.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dptabt.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\dptabt2ld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DPTab___"
# PROP BASE Intermediate_Dir "DPTab___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dptabt2ir"
# PROP Intermediate_Dir "dptabt2ir"
# PROP Target_Dir ""
OUTDIR=.\dptabt2ir
INTDIR=.\dptabt2ir

ALL : "$(OUTDIR)\dptabt2ir.exe" "$(OUTDIR)\dptabt2ir.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dptabt.obj"
	-@erase "$(INTDIR)\dptabt.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(OUTDIR)\dptabt2ir.bsc"
	-@erase "$(OUTDIR)\dptabt2ir.exe"
	-@erase "$(OUTDIR)\dptabt2ir.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR /YX /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "INTERNET_ADDRESS" /FR /YX /c
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "INTERNET_ADDRESS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/test.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dptabt2ir/
CPP_SBRS=.\dptabt2ir/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"dptabt2ir/dptabt2ir.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dptabt2ir.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dptabt.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\unistd.sbr"

"$(OUTDIR)\dptabt2ir.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /map /machine:I386 /out:"dptabt2dr/dptabt2dr.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /map /machine:I386 /out:"dptabt2ir/dptabt2ir.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dptabt2ir.pdb"\
 /map:"$(INTDIR)/dptabt2ir.map" /machine:I386 /out:"$(OUTDIR)/dptabt2ir.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dptabt.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\dptabt2ir.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DPTab__0"
# PROP BASE Intermediate_Dir "DPTab__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dptabt2lr"
# PROP Intermediate_Dir "dptabt2lr"
# PROP Target_Dir ""
OUTDIR=.\dptabt2lr
INTDIR=.\dptabt2lr

ALL : "$(OUTDIR)\dptabt2lr.exe" "$(OUTDIR)\dptabt2lr.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dptabt.obj"
	-@erase "$(INTDIR)\dptabt.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(OUTDIR)\dptabt2lr.bsc"
	-@erase "$(OUTDIR)\dptabt2lr.exe"
	-@erase "$(OUTDIR)\dptabt2lr.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /FR /YX /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "LOOPBACK_ADDRESS" /FR /YX /c
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /D\
 "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DP_DLL" /D "LOOPBACK_ADDRESS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/test.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dptabt2lr/
CPP_SBRS=.\dptabt2lr/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"dptabt2lr/dptabt2lr.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dptabt2lr.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dptabt.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\unistd.sbr"

"$(OUTDIR)\dptabt2lr.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /map /machine:I386 /out:"dptabt2dr/dptabt2dr.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo /subsystem:console /map /machine:I386 /out:"dptabt2lr/dptabt2lr.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib advapi32.lib ..\..\..\win\lib\dp2d.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dptabt2lr.pdb"\
 /map:"$(INTDIR)/dptabt2lr.map" /machine:I386 /out:"$(OUTDIR)/dptabt2lr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dptabt.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\dptabt2lr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "DPTab - Win32 Release"
# Name "DPTab - Win32 Debug"
# Name "DPTab - Win32 v2 Inet Debug"
# Name "DPTab - Win32 v2 loop Debug"
# Name "DPTab - Win32 v2 Inet Release"
# Name "DPTab - Win32 v2 loop Release"

!IF  "$(CFG)" == "DPTab - Win32 Release"

!ELSEIF  "$(CFG)" == "DPTab - Win32 Debug"

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Debug"

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Debug"

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Release"

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dptabt.c
DEP_CPP_DPTAB=\
	"..\..\..\demo\utils\unistd.h"\
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
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "DPTab - Win32 Release"


"$(INTDIR)\dptabt.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"

"$(INTDIR)\dptabt.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DPTab - Win32 Debug"


"$(INTDIR)\dptabt.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"

"$(INTDIR)\dptabt.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Debug"


"$(INTDIR)\dptabt.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"

"$(INTDIR)\dptabt.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Debug"


"$(INTDIR)\dptabt.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"

"$(INTDIR)\dptabt.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Release"


"$(INTDIR)\dptabt.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"

"$(INTDIR)\dptabt.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Release"


"$(INTDIR)\dptabt.obj" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"

"$(INTDIR)\dptabt.sbr" : $(SOURCE) $(DEP_CPP_DPTAB) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\unistd.c
DEP_CPP_UNIST=\
	"..\..\..\demo\utils\unistd.h"\
	

!IF  "$(CFG)" == "DPTab - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\unistd.sbr" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\unistd.sbr" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\unistd.sbr" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\unistd.sbr" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\unistd.sbr" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\unistd.sbr" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "DPTab - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 Inet Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DPTab - Win32 v2 loop Release"


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
