# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=loop - Win32 V2 Debug
!MESSAGE No configuration specified.  Defaulting to loop - Win32 V2 Debug.
!ENDIF 

!IF "$(CFG)" != "loop - Win32 Release" && "$(CFG)" != "loop - Win32 Debug" &&\
 "$(CFG)" != "loop - Win32 Debug no broadcast" && "$(CFG)" !=\
 "loop - Win32 Release no broadcast" && "$(CFG)" != "loop - Win32 V2 Release" &&\
 "$(CFG)" != "loop - Win32 V2 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wloop.mak" CFG="loop - Win32 V2 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "loop - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "loop - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "loop - Win32 Debug no broadcast" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "loop - Win32 Release no broadcast" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "loop - Win32 V2 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "loop - Win32 V2 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "loop - Win32 Debug no broadcast"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "loop - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wloop"
# PROP Intermediate_Dir "wloop"
# PROP Target_Dir ""
OUTDIR=.\wloop
INTDIR=.\wloop

ALL : "..\out\dll\wloop.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loopwin.obj"
	-@erase "$(INTDIR)\wloopi.res"
	-@erase "$(OUTDIR)\wloop.exp"
	-@erase "$(OUTDIR)\wloop.lib"
	-@erase "..\out\dll\wloop.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /Fp"$(INTDIR)/wloop.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wloop/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wloopi.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wloop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wloop.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wloop.pdb" /machine:I386\
 /out:"..\out\dll\wloop.dll" /implib:"$(OUTDIR)/wloop.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\loopwin.obj" \
	"$(INTDIR)\wloopi.res"

"..\out\dll\wloop.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "loop - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wloopd"
# PROP Intermediate_Dir "wloopd"
# PROP Target_Dir ""
OUTDIR=.\wloopd
INTDIR=.\wloopd

ALL : "..\out\dll\wloopd.dll" "$(OUTDIR)\wloop.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loop.sbr"
	-@erase "$(INTDIR)\loopwin.obj"
	-@erase "$(INTDIR)\loopwin.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wloopi.res"
	-@erase "$(OUTDIR)\wloop.bsc"
	-@erase "$(OUTDIR)\wloopd.exp"
	-@erase "$(OUTDIR)\wloopd.lib"
	-@erase "$(OUTDIR)\wloopd.pdb"
	-@erase "..\out\dll\wloopd.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "DPRNT" /FR /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "DPRNT" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/wloop.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wloopd/
CPP_SBRS=.\wloopd/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wloopi.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wloop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\loop.sbr" \
	"$(INTDIR)\loopwin.sbr"

"$(OUTDIR)\wloop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wloopd.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wloopd.pdb" /debug /machine:I386\
 /out:"..\out\dll\wloopd.dll" /implib:"$(OUTDIR)/wloopd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\loopwin.obj" \
	"$(INTDIR)\wloopi.res"

"..\out\dll\wloopd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "loop - Win32 Debug no broadcast"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "loop___W"
# PROP BASE Intermediate_Dir "loop___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wloopnbd"
# PROP Intermediate_Dir "wloopnbd"
# PROP Target_Dir ""
OUTDIR=.\wloopnbd
INTDIR=.\wloopnbd

ALL : "..\out\dll\wloopnbd.dll" "$(OUTDIR)\wloop.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loop.sbr"
	-@erase "$(INTDIR)\loopwin.obj"
	-@erase "$(INTDIR)\loopwin.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wloopi.res"
	-@erase "$(OUTDIR)\wloop.bsc"
	-@erase "$(OUTDIR)\wloopnbd.exp"
	-@erase "$(OUTDIR)\wloopnbd.lib"
	-@erase "$(OUTDIR)\wloopnbd.pdb"
	-@erase "..\out\dll\wloopnbd.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "DPRNT" /FR /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "NO_BROADCAST" /FR /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D\
 "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "NO_BROADCAST"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/wloop.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 
CPP_OBJS=.\wloopnbd/
CPP_SBRS=.\wloopnbd/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "NO_BROADCAST"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wloopi.res" /d "_DEBUG" /d "NO_BROADCAST" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wloop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\loop.sbr" \
	"$(INTDIR)\loopwin.sbr"

"$(OUTDIR)\wloop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wloopd.dll"
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wloopnbd.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wloopnbd.pdb" /debug /machine:I386\
 /out:"..\out\dll\wloopnbd.dll" /implib:"$(OUTDIR)/wloopnbd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\loopwin.obj" \
	"$(INTDIR)\wloopi.res"

"..\out\dll\wloopnbd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "loop - Win32 Release no broadcast"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "loop___0"
# PROP BASE Intermediate_Dir "loop___0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wloopnb"
# PROP Intermediate_Dir "wloopnb"
# PROP Target_Dir ""
OUTDIR=.\wloopnb
INTDIR=.\wloopnb

ALL : "..\out\dll\wloopnb.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loopwin.obj"
	-@erase "$(INTDIR)\wloopi.res"
	-@erase "$(OUTDIR)\wloopnb.exp"
	-@erase "$(OUTDIR)\wloopnb.lib"
	-@erase "..\out\dll\wloopnb.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /Zp1 /MTd /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "NO_BROADCAST" /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "NO_BROADCAST"\
 /Fp"$(INTDIR)/wloop.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wloopnb/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "NO_BROADCAST"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wloopi.res" /d "NDEBUG" /d "NO_BROADCAST" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wloop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wloop.dll"
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wloopnb.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wloopnb.pdb" /machine:I386\
 /out:"..\out\dll\wloopnb.dll" /implib:"$(OUTDIR)/wloopnb.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\loopwin.obj" \
	"$(INTDIR)\wloopi.res"

"..\out\dll\wloopnb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "loop - Win32 V2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "loop___W"
# PROP BASE Intermediate_Dir "loop___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wloop2"
# PROP Intermediate_Dir "wloop2"
# PROP Target_Dir ""
OUTDIR=.\wloop2
INTDIR=.\wloop2

ALL : "$(OUTDIR)\wloop2.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loopwin.obj"
	-@erase "$(INTDIR)\wloopi.res"
	-@erase "$(OUTDIR)\wloop2.dll"
	-@erase "$(OUTDIR)\wloop2.exp"
	-@erase "$(OUTDIR)\wloop2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /Zp1 /MDd /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FAcs /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FAcs\
 /Fa"$(INTDIR)/" /Fp"$(INTDIR)/wloop.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wloop2/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wloopi.res" /d "NDEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wloop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wloop.dll"
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"wloop2\wloop2.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wloop2.pdb" /machine:I386\
 /out:"$(OUTDIR)/wloop2.dll" /implib:"$(OUTDIR)/wloop2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\loopwin.obj" \
	"$(INTDIR)\wloopi.res"

"$(OUTDIR)\wloop2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "loop - Win32 V2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "loop___0"
# PROP BASE Intermediate_Dir "loop___0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wloop2d"
# PROP Intermediate_Dir "wloop2d"
# PROP Target_Dir ""
OUTDIR=.\wloop2d
INTDIR=.\wloop2d

ALL : "$(OUTDIR)\wloop2d.dll" "$(OUTDIR)\wloop.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loop.sbr"
	-@erase "$(INTDIR)\loopwin.obj"
	-@erase "$(INTDIR)\loopwin.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wloopi.res"
	-@erase "$(OUTDIR)\wloop.bsc"
	-@erase "$(OUTDIR)\wloop2d.dll"
	-@erase "$(OUTDIR)\wloop2d.exp"
	-@erase "$(OUTDIR)\wloop2d.lib"
	-@erase "$(OUTDIR)\wloop2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "DPRNT" /FR /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D\
 "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/wloop.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 
CPP_OBJS=.\wloop2d/
CPP_SBRS=.\wloop2d/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wloopi.res" /d "_DEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wloop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\loop.sbr" \
	"$(INTDIR)\loopwin.sbr"

"$(OUTDIR)\wloop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wloopd.dll"
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"wloop2d\wloop2d.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wloop2d.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/wloop2d.dll" /implib:"$(OUTDIR)/wloop2d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\loopwin.obj" \
	"$(INTDIR)\wloopi.res"

"$(OUTDIR)\wloop2d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "loop - Win32 Release"
# Name "loop - Win32 Debug"
# Name "loop - Win32 Debug no broadcast"
# Name "loop - Win32 Release no broadcast"
# Name "loop - Win32 V2 Release"
# Name "loop - Win32 V2 Debug"

!IF  "$(CFG)" == "loop - Win32 Release"

!ELSEIF  "$(CFG)" == "loop - Win32 Debug"

!ELSEIF  "$(CFG)" == "loop - Win32 Debug no broadcast"

!ELSEIF  "$(CFG)" == "loop - Win32 Release no broadcast"

!ELSEIF  "$(CFG)" == "loop - Win32 V2 Release"

!ELSEIF  "$(CFG)" == "loop - Win32 V2 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\loop.c
DEP_CPP_LOOP_=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\h\ddprint.h"\
	".\loopwin.h"\
	

!IF  "$(CFG)" == "loop - Win32 Release"


"$(INTDIR)\loop.obj" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 Debug"


"$(INTDIR)\loop.obj" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"

"$(INTDIR)\loop.sbr" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 Debug no broadcast"


"$(INTDIR)\loop.obj" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"

"$(INTDIR)\loop.sbr" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 Release no broadcast"


"$(INTDIR)\loop.obj" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Release"


"$(INTDIR)\loop.obj" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Debug"


"$(INTDIR)\loop.obj" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"

"$(INTDIR)\loop.sbr" : $(SOURCE) $(DEP_CPP_LOOP_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\loopwin.c
DEP_CPP_LOOPW=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\h\ddprint.h"\
	".\loopwin.h"\
	

!IF  "$(CFG)" == "loop - Win32 Release"


"$(INTDIR)\loopwin.obj" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 Debug"


"$(INTDIR)\loopwin.obj" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"

"$(INTDIR)\loopwin.sbr" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 Debug no broadcast"


"$(INTDIR)\loopwin.obj" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"

"$(INTDIR)\loopwin.sbr" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 Release no broadcast"


"$(INTDIR)\loopwin.obj" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Release"


"$(INTDIR)\loopwin.obj" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Debug"


"$(INTDIR)\loopwin.obj" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"

"$(INTDIR)\loopwin.sbr" : $(SOURCE) $(DEP_CPP_LOOPW) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wloopi.rc
DEP_RSC_WLOOP=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	

!IF  "$(CFG)" == "loop - Win32 Release"


"$(INTDIR)\wloopi.res" : $(SOURCE) $(DEP_RSC_WLOOP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 Debug"


"$(INTDIR)\wloopi.res" : $(SOURCE) $(DEP_RSC_WLOOP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 Debug no broadcast"


"$(INTDIR)\wloopi.res" : $(SOURCE) $(DEP_RSC_WLOOP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 Release no broadcast"


"$(INTDIR)\wloopi.res" : $(SOURCE) $(DEP_RSC_WLOOP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Release"


"$(INTDIR)\wloopi.res" : $(SOURCE) $(DEP_RSC_WLOOP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Debug"


"$(INTDIR)\wloopi.res" : $(SOURCE) $(DEP_RSC_WLOOP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ddprint.c

!IF  "$(CFG)" == "loop - Win32 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "loop - Win32 Debug no broadcast"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "loop - Win32 Release no broadcast"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "loop - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
