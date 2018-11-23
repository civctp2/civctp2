# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=ipx2dll - Win32 V2 Debug
!MESSAGE No configuration specified.  Defaulting to ipx2dll - Win32 V2 Debug.
!ENDIF 

!IF "$(CFG)" != "ipx2dll - Win32 Release" && "$(CFG)" !=\
 "ipx2dll - Win32 Debug" && "$(CFG)" != "ipx2dll - Win32 V2 Debug" && "$(CFG)"\
 != "ipx2dll - Win32 V2 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wipx.mak" CFG="ipx2dll - Win32 V2 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ipx2dll - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipx2dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipx2dll - Win32 V2 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipx2dll - Win32 V2 Release" (based on\
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
# PROP Target_Last_Scanned "ipx2dll - Win32 V2 Release"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ipx2dll - Win32 Release"

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

ALL : "..\out\dll\wipx.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\ipx2dll.obj"
	-@erase "$(INTDIR)\Ipx2win.obj"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\wipxi.res"
	-@erase "$(OUTDIR)\wipx.exp"
	-@erase "$(OUTDIR)\wipx.lib"
	-@erase "..\out\dll\wipx.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK"\
 /Fp"$(INTDIR)/wipx.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wipxi.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wipx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wipx.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wipx.pdb" /machine:I386\
 /out:"..\out\dll\wipx.dll" /implib:"$(OUTDIR)/wipx.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\ipx2dll.obj" \
	"$(INTDIR)\Ipx2win.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wipxi.res"

"..\out\dll\wipx.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"

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

ALL : "..\out\dll\wipxd.dll" "$(OUTDIR)\wipx.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\ipx2dll.obj"
	-@erase "$(INTDIR)\ipx2dll.sbr"
	-@erase "$(INTDIR)\Ipx2win.obj"
	-@erase "$(INTDIR)\Ipx2win.sbr"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\logpkt.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wipxi.res"
	-@erase "$(OUTDIR)\wipx.bsc"
	-@erase "$(OUTDIR)\wipxd.exp"
	-@erase "$(OUTDIR)\wipxd.lib"
	-@erase "$(OUTDIR)\wipxd.pdb"
	-@erase "..\out\dll\wipxd.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "DPRNT" /FR /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D\
 "DPRNT" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wipx.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wipxi.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wipx.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\ipx2dll.sbr" \
	"$(INTDIR)\Ipx2win.sbr" \
	"$(INTDIR)\logpkt.sbr"

"$(OUTDIR)\wipx.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wipxd.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wipxd.pdb" /debug /machine:I386\
 /out:"..\out\dll\wipxd.dll" /implib:"$(OUTDIR)/wipxd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\ipx2dll.obj" \
	"$(INTDIR)\Ipx2win.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wipxi.res"

"..\out\dll\wipxd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ipx2dll_"
# PROP BASE Intermediate_Dir "ipx2dll_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wipx2d"
# PROP Intermediate_Dir "wipx2d"
# PROP Target_Dir ""
OUTDIR=.\wipx2d
INTDIR=.\wipx2d

ALL : "$(OUTDIR)\wipx2d.dll" "$(OUTDIR)\wipx.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\ipx2dll.obj"
	-@erase "$(INTDIR)\ipx2dll.sbr"
	-@erase "$(INTDIR)\Ipx2win.obj"
	-@erase "$(INTDIR)\Ipx2win.sbr"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\logpkt.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wipxi.res"
	-@erase "$(OUTDIR)\wipx.bsc"
	-@erase "$(OUTDIR)\wipx2d.dll"
	-@erase "$(OUTDIR)\wipx2d.exp"
	-@erase "$(OUTDIR)\wipx2d.lib"
	-@erase "$(OUTDIR)\wipx2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "DPRNT" /FR /YX /c
# ADD CPP /nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "_DEBUG" /D "DPRNT" /D "WIN32" /D "_WINDOWS" /D\
 "DYNALINK" /D "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wipx.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wipx2d/
CPP_SBRS=.\wipx2d/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wipxi.res" /d "_DEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wipx.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\ipx2dll.sbr" \
	"$(INTDIR)\Ipx2win.sbr" \
	"$(INTDIR)\logpkt.sbr"

"$(OUTDIR)\wipx.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wipxd.dll"
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"wipx2d\wipx2d.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wipx2d.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/wipx2d.dll" /implib:"$(OUTDIR)/wipx2d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\ipx2dll.obj" \
	"$(INTDIR)\Ipx2win.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wipxi.res"

"$(OUTDIR)\wipx2d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ipx2dll0"
# PROP BASE Intermediate_Dir "ipx2dll0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wipx2"
# PROP Intermediate_Dir "wipx2"
# PROP Target_Dir ""
OUTDIR=.\wipx2
INTDIR=.\wipx2

ALL : "$(OUTDIR)\wipx2.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\ipx2dll.obj"
	-@erase "$(INTDIR)\Ipx2win.obj"
	-@erase "$(INTDIR)\logpkt.obj"
	-@erase "$(INTDIR)\wipxi.res"
	-@erase "$(OUTDIR)\wipx2.dll"
	-@erase "$(OUTDIR)\wipx2.exp"
	-@erase "$(OUTDIR)\wipx2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /YX /c
# ADD CPP /nologo /G5 /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D "dp_ANET2" /FAcs /YX /c
CPP_PROJ=/nologo /G5 /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DYNALINK" /D\
 "dp_ANET2" /FAcs /Fa"$(INTDIR)/" /Fp"$(INTDIR)/wipx.pch" /YX /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\wipx2/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wipxi.res" /d "NDEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wipx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wipx.dll"
# ADD LINK32 kernel32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"wipx2\wipx2.dll"
LINK32_FLAGS=kernel32.lib wsock32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wipx2.pdb" /machine:I386\
 /out:"$(OUTDIR)/wipx2.dll" /implib:"$(OUTDIR)/wipx2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\ipx2dll.obj" \
	"$(INTDIR)\Ipx2win.obj" \
	"$(INTDIR)\logpkt.obj" \
	"$(INTDIR)\wipxi.res"

"$(OUTDIR)\wipx2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ipx2dll - Win32 Release"
# Name "ipx2dll - Win32 Debug"
# Name "ipx2dll - Win32 V2 Debug"
# Name "ipx2dll - Win32 V2 Release"

!IF  "$(CFG)" == "ipx2dll - Win32 Release"

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Ipx2win.c
DEP_CPP_IPX2W=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dpmem.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\h\ddprint.h"\
	".\ipx2win.h"\
	

!IF  "$(CFG)" == "ipx2dll - Win32 Release"


"$(INTDIR)\Ipx2win.obj" : $(SOURCE) $(DEP_CPP_IPX2W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"


"$(INTDIR)\Ipx2win.obj" : $(SOURCE) $(DEP_CPP_IPX2W) "$(INTDIR)"

"$(INTDIR)\Ipx2win.sbr" : $(SOURCE) $(DEP_CPP_IPX2W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"


"$(INTDIR)\Ipx2win.obj" : $(SOURCE) $(DEP_CPP_IPX2W) "$(INTDIR)"

"$(INTDIR)\Ipx2win.sbr" : $(SOURCE) $(DEP_CPP_IPX2W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"


"$(INTDIR)\Ipx2win.obj" : $(SOURCE) $(DEP_CPP_IPX2W) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ipx2dll.c
DEP_CPP_IPX2D=\
	"..\..\..\demo\utils\logpkt.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\h\ddprint.h"\
	".\ipx2win.h"\
	".\nw.h"\
	

!IF  "$(CFG)" == "ipx2dll - Win32 Release"


"$(INTDIR)\ipx2dll.obj" : $(SOURCE) $(DEP_CPP_IPX2D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"


"$(INTDIR)\ipx2dll.obj" : $(SOURCE) $(DEP_CPP_IPX2D) "$(INTDIR)"

"$(INTDIR)\ipx2dll.sbr" : $(SOURCE) $(DEP_CPP_IPX2D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"


"$(INTDIR)\ipx2dll.obj" : $(SOURCE) $(DEP_CPP_IPX2D) "$(INTDIR)"

"$(INTDIR)\ipx2dll.sbr" : $(SOURCE) $(DEP_CPP_IPX2D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"


"$(INTDIR)\ipx2dll.obj" : $(SOURCE) $(DEP_CPP_IPX2D) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wipxi.rc
DEP_RSC_WIPXI=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	

!IF  "$(CFG)" == "ipx2dll - Win32 Release"


"$(INTDIR)\wipxi.res" : $(SOURCE) $(DEP_RSC_WIPXI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"


"$(INTDIR)\wipxi.res" : $(SOURCE) $(DEP_RSC_WIPXI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"


"$(INTDIR)\wipxi.res" : $(SOURCE) $(DEP_RSC_WIPXI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"


"$(INTDIR)\wipxi.res" : $(SOURCE) $(DEP_RSC_WIPXI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ddprint.c

!IF  "$(CFG)" == "ipx2dll - Win32 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\logpkt.c
DEP_CPP_LOGPK=\
	"..\..\..\demo\utils\logpkt.h"\
	"..\..\..\demo\utils\unistd.h"\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "ipx2dll - Win32 Release"


"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logpkt.sbr" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logpkt.sbr" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"


"$(INTDIR)\logpkt.obj" : $(SOURCE) $(DEP_CPP_LOGPK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "ipx2dll - Win32 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ipx2dll - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ipx2dll - Win32 V2 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
