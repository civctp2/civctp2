# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=WinTCPs - Win32 V2 LAN Debug
!MESSAGE No configuration specified.  Defaulting to WinTCPs - Win32 V2 LAN\
 Debug.
!ENDIF 

!IF "$(CFG)" != "WinTCPs - Win32 V2 Debug" && "$(CFG)" !=\
 "WinTCPs - Win32 V2 Release" && "$(CFG)" != "WinTCPs - Win32 V2 LAN Debug" &&\
 "$(CFG)" != "WinTCPs - Win32 V2 LAN Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "winets.mak" CFG="WinTCPs - Win32 V2 LAN Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinTCPs - Win32 V2 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinTCPs - Win32 V2 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinTCPs - Win32 V2 LAN Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinTCPs - Win32 V2 LAN Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "WinTCPs - Win32 V2 LAN Release"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinTCPs_"
# PROP BASE Intermediate_Dir "WinTCPs_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "winets2d"
# PROP Intermediate_Dir "winets2d"
# PROP Target_Dir ""
OUTDIR=.\winets2d
INTDIR=.\winets2d

ALL : "$(OUTDIR)\winets2d.dll" "$(OUTDIR)\winets.bsc"

CLEAN : 
	-@erase "$(INTDIR)\adialup.obj"
	-@erase "$(INTDIR)\adialup.sbr"
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\dictset.obj"
	-@erase "$(INTDIR)\dictset.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\logpkt.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wineti.res"
	-@erase "$(INTDIR)\WSTCP.obj"
	-@erase "$(INTDIR)\WSTCP.sbr"
	-@erase "$(INTDIR)\WSTCPdll.obj"
	-@erase "$(INTDIR)\WSTCPdll.sbr"
	-@erase "$(OUTDIR)\winets.bsc"
	-@erase "$(OUTDIR)\winets2d.dll"
	-@erase "$(OUTDIR)\winets2d.exp"
	-@erase "$(OUTDIR)\winets2d.lib"
	-@erase "$(OUTDIR)\winets2d.map"
	-@erase "$(OUTDIR)\winets2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /FR /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D\
 "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/winets.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\winets2d/
CPP_SBRS=.\winets2d/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wineti.res" /d "_DEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/winets.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adialup.sbr" \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\dictset.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\logpkt.sbr" \
	"$(INTDIR)\WSTCP.sbr" \
	"$(INTDIR)\WSTCPdll.sbr"

"$(OUTDIR)\winets.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\winetsd.dll"
# ADD LINK32 kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"winets2d\winets2d.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/winets2d.pdb"\
 /map:"$(INTDIR)/winets2d.map" /debug /machine:I386\
 /out:"$(OUTDIR)/winets2d.dll" /implib:"$(OUTDIR)/winets2d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adialup.obj" \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\dictset.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wineti.res" \
	"$(INTDIR)\WSTCP.obj" \
	"$(INTDIR)\WSTCPdll.obj"

"$(OUTDIR)\winets2d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinTCPs0"
# PROP BASE Intermediate_Dir "WinTCPs0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "winets2"
# PROP Intermediate_Dir "winets2"
# PROP Target_Dir ""
OUTDIR=.\winets2
INTDIR=.\winets2

ALL : "$(OUTDIR)\winets2.dll" "$(OUTDIR)\winets.bsc"

CLEAN : 
	-@erase "$(INTDIR)\adialup.obj"
	-@erase "$(INTDIR)\adialup.sbr"
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\dictset.obj"
	-@erase "$(INTDIR)\dictset.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\logpkt.sbr"
	-@erase "$(INTDIR)\wineti.res"
	-@erase "$(INTDIR)\WSTCP.obj"
	-@erase "$(INTDIR)\WSTCP.sbr"
	-@erase "$(INTDIR)\WSTCPdll.obj"
	-@erase "$(INTDIR)\WSTCPdll.sbr"
	-@erase "$(OUTDIR)\winets.bsc"
	-@erase "$(OUTDIR)\winets2.dll"
	-@erase "$(OUTDIR)\winets2.exp"
	-@erase "$(OUTDIR)\winets2.lib"
	-@erase "$(OUTDIR)\winets2.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /Fr /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /Ot /Oi /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FAcs /Fr /YX /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /Ot /Oi /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D\
 "dp_ANET2" /FAcs /Fa"$(INTDIR)/" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/winets.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\winets2/
CPP_SBRS=.\winets2/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wineti.res" /d "NDEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/winets.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adialup.sbr" \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\dictset.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\logpkt.sbr" \
	"$(INTDIR)\WSTCP.sbr" \
	"$(INTDIR)\WSTCPdll.sbr"

"$(OUTDIR)\winets.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\winets.dll"
# ADD LINK32 kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"winets2\winets2.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/winets2.pdb" /map:"$(INTDIR)/winets2.map"\
 /machine:I386 /out:"$(OUTDIR)/winets2.dll" /implib:"$(OUTDIR)/winets2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adialup.obj" \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\dictset.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wineti.res" \
	"$(INTDIR)\WSTCP.obj" \
	"$(INTDIR)\WSTCPdll.obj"

"$(OUTDIR)\winets2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinTCPs_"
# PROP BASE Intermediate_Dir "WinTCPs_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wudpland"
# PROP Intermediate_Dir "wudpland"
# PROP Target_Dir ""
OUTDIR=.\wudpland
INTDIR=.\wudpland

ALL : "$(OUTDIR)\wudpland.dll" "$(OUTDIR)\winets.bsc"

CLEAN : 
	-@erase "$(INTDIR)\adialup.obj"
	-@erase "$(INTDIR)\adialup.sbr"
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\dictset.obj"
	-@erase "$(INTDIR)\dictset.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\logpkt.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wineti.res"
	-@erase "$(INTDIR)\WSTCP.obj"
	-@erase "$(INTDIR)\WSTCP.sbr"
	-@erase "$(INTDIR)\WSTCPdll.obj"
	-@erase "$(INTDIR)\WSTCPdll.sbr"
	-@erase "$(OUTDIR)\winets.bsc"
	-@erase "$(OUTDIR)\wudpland.dll"
	-@erase "$(OUTDIR)\wudpland.exp"
	-@erase "$(OUTDIR)\wudpland.lib"
	-@erase "$(OUTDIR)\wudpland.map"
	-@erase "$(OUTDIR)\wudpland.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FR /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /D "TCP_LAN" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D\
 "dp_ANET2" /D "TCP_LAN" /FR"$(INTDIR)/" /Fp"$(INTDIR)/winets.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wudpland/
CPP_SBRS=.\wudpland/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
# ADD RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wineti.res" /d "_DEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/winets.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adialup.sbr" \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\dictset.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\logpkt.sbr" \
	"$(INTDIR)\WSTCP.sbr" \
	"$(INTDIR)\WSTCPdll.sbr"

"$(OUTDIR)\winets.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"winets2d\winets2d.dll"
# ADD LINK32 kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"wudpland\wudpland.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/wudpland.pdb"\
 /map:"$(INTDIR)/wudpland.map" /debug /machine:I386\
 /out:"$(OUTDIR)/wudpland.dll" /implib:"$(OUTDIR)/wudpland.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adialup.obj" \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\dictset.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wineti.res" \
	"$(INTDIR)\WSTCP.obj" \
	"$(INTDIR)\WSTCPdll.obj"

"$(OUTDIR)\wudpland.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinTCPs0"
# PROP BASE Intermediate_Dir "WinTCPs0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wudplan"
# PROP Intermediate_Dir "wudplan"
# PROP Target_Dir ""
OUTDIR=.\wudplan
INTDIR=.\wudplan

ALL : "$(OUTDIR)\wudplan.dll" "$(OUTDIR)\winets.bsc"

CLEAN : 
	-@erase "$(INTDIR)\adialup.obj"
	-@erase "$(INTDIR)\adialup.sbr"
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\dictset.obj"
	-@erase "$(INTDIR)\dictset.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\logpkt.sbr"
	-@erase "$(INTDIR)\wineti.res"
	-@erase "$(INTDIR)\WSTCP.obj"
	-@erase "$(INTDIR)\WSTCP.sbr"
	-@erase "$(INTDIR)\WSTCPdll.obj"
	-@erase "$(INTDIR)\WSTCPdll.sbr"
	-@erase "$(OUTDIR)\winets.bsc"
	-@erase "$(OUTDIR)\wudplan.dll"
	-@erase "$(OUTDIR)\wudplan.exp"
	-@erase "$(OUTDIR)\wudplan.lib"
	-@erase "$(OUTDIR)\wudplan.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /Ot /Oi /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FAcs /Fr /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /Ot /Oi /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /D "TCP_LAN" /FAcs /Fr /YX /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /Ot /Oi /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D\
 "dp_ANET2" /D "TCP_LAN" /FAcs /Fa"$(INTDIR)/" /Fr"$(INTDIR)/"\
 /Fp"$(INTDIR)/winets.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wudplan/
CPP_SBRS=.\wudplan/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
# ADD RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wineti.res" /d "NDEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/winets.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adialup.sbr" \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\dictset.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\logpkt.sbr" \
	"$(INTDIR)\WSTCP.sbr" \
	"$(INTDIR)\WSTCPdll.sbr"

"$(OUTDIR)\winets.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"winets2\winets2.dll"
# ADD LINK32 kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"wudplan\wudplan.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib advapi32.lib /nologo /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/wudplan.pdb" /map:"$(INTDIR)/wudplan.map"\
 /machine:I386 /out:"$(OUTDIR)/wudplan.dll" /implib:"$(OUTDIR)/wudplan.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adialup.obj" \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\dictset.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wineti.res" \
	"$(INTDIR)\WSTCP.obj" \
	"$(INTDIR)\WSTCPdll.obj"

"$(OUTDIR)\wudplan.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "WinTCPs - Win32 V2 Debug"
# Name "WinTCPs - Win32 V2 Release"
# Name "WinTCPs - Win32 V2 LAN Debug"
# Name "WinTCPs - Win32 V2 LAN Release"

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\WSTCPdll.c
DEP_CPP_WSTCP=\
	"..\..\..\demo\utils\logpkt.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dictset.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	"..\h\ddprint.h"\
	".\adialup.h"\
	".\wstcp.h"\
	

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


"$(INTDIR)\WSTCPdll.obj" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"

"$(INTDIR)\WSTCPdll.sbr" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


"$(INTDIR)\WSTCPdll.obj" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"

"$(INTDIR)\WSTCPdll.sbr" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


"$(INTDIR)\WSTCPdll.obj" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"

"$(INTDIR)\WSTCPdll.sbr" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


"$(INTDIR)\WSTCPdll.obj" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"

"$(INTDIR)\WSTCPdll.sbr" : $(SOURCE) $(DEP_CPP_WSTCP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WSTCP.c
DEP_CPP_WSTCP_=\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
	"..\..\..\h\dictset.h"\
	"..\..\..\h\dp.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpdll.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppackn.h"\
	"..\..\..\h\dppkt.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dptab.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\dynatab.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\hkeytab.h"\
	"..\..\..\h\q.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	"..\h\ddprint.h"\
	".\wstcp.h"\
	

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


"$(INTDIR)\WSTCP.obj" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"

"$(INTDIR)\WSTCP.sbr" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


"$(INTDIR)\WSTCP.obj" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"

"$(INTDIR)\WSTCP.sbr" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


"$(INTDIR)\WSTCP.obj" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"

"$(INTDIR)\WSTCP.sbr" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


"$(INTDIR)\WSTCP.obj" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"

"$(INTDIR)\WSTCP.sbr" : $(SOURCE) $(DEP_CPP_WSTCP_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wineti.rc
DEP_RSC_WINET=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


"$(INTDIR)\wineti.res" : $(SOURCE) $(DEP_RSC_WINET) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


"$(INTDIR)\wineti.res" : $(SOURCE) $(DEP_RSC_WINET) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


"$(INTDIR)\wineti.res" : $(SOURCE) $(DEP_RSC_WINET) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


"$(INTDIR)\wineti.res" : $(SOURCE) $(DEP_RSC_WINET) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dictset.c
DEP_CPP_DICTS=\
	"..\..\..\h\dictset.h"\
	"..\..\..\h\dpmem.h"\
	

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dictset.obj" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dictset.sbr" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dictset.obj" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dictset.sbr" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dictset.obj" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dictset.sbr" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dictset.obj" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dictset.sbr" : $(SOURCE) $(DEP_CPP_DICTS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ddprint.c

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\logpkt.c
DEP_CPP_LOGPK=\
	"..\..\..\demo\utils\logpkt.h"\
	"..\..\..\demo\utils\unistd.h"\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logpkt.sbr" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logpkt.sbr" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logpkt.sbr" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logpkt.sbr" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\adialup.c

!IF  "$(CFG)" == "WinTCPs - Win32 V2 Debug"


"$(INTDIR)\adialup.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\adialup.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 Release"


"$(INTDIR)\adialup.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\adialup.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Debug"


"$(INTDIR)\adialup.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\adialup.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WinTCPs - Win32 V2 LAN Release"


"$(INTDIR)\adialup.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\adialup.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
