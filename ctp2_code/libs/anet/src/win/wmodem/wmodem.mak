# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=modem - Win32 V2 Debug
!MESSAGE No configuration specified.  Defaulting to modem - Win32 V2 Debug.
!ENDIF 

!IF "$(CFG)" != "modem - Win32 Release" && "$(CFG)" != "modem - Win32 Debug" &&\
 "$(CFG)" != "modem - Win32 V2 Debug" && "$(CFG)" != "modem - Win32 V2 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wmodem.mak" CFG="modem - Win32 V2 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "modem - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "modem - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "modem - Win32 V2 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "modem - Win32 V2 Release" (based on\
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
# PROP Target_Last_Scanned "modem - Win32 V2 Release"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "modem - Win32 Release"

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

ALL : "..\out\dll\wmodem.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\modmdll.obj"
	-@erase "$(INTDIR)\regmo.obj"
	-@erase "$(INTDIR)\Ser.obj"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\wmodemi.res"
	-@erase "$(OUTDIR)\wmodem.exp"
	-@erase "$(OUTDIR)\wmodem.lib"
	-@erase "..\out\dll\wmodem.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /I "..\h" /I "..\main" /I "..\..\..\h" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /YX /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /I "..\h" /I "..\main" /I "..\..\..\h" /I\
 "..\stub" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK"\
 /Fp"$(INTDIR)/wmodem.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wmodemi.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wmodem.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc" /out:"..\out\dll\wmodem.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib advapi32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wmodem.pdb" /machine:I386 /nodefaultlib:"libc"\
 /out:"..\out\dll\wmodem.dll" /implib:"$(OUTDIR)/wmodem.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\modmdll.obj" \
	"$(INTDIR)\regmo.obj" \
	"$(INTDIR)\Ser.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wmodemi.res"

"..\out\dll\wmodem.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modem - Win32 Debug"

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

ALL : "..\out\dll\wmodemd.dll" "$(OUTDIR)\wmodem.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\modmdll.obj"
	-@erase "$(INTDIR)\modmdll.sbr"
	-@erase "$(INTDIR)\regmo.obj"
	-@erase "$(INTDIR)\regmo.sbr"
	-@erase "$(INTDIR)\Ser.obj"
	-@erase "$(INTDIR)\Ser.sbr"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\serio.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wmodemi.res"
	-@erase "$(OUTDIR)\wmodem.bsc"
	-@erase "$(OUTDIR)\wmodemd.exp"
	-@erase "$(OUTDIR)\wmodemd.lib"
	-@erase "$(OUTDIR)\wmodemd.pdb"
	-@erase "..\out\dll\wmodemd.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /I "..\stub" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I\
 "..\..\..\h" /I "..\stub" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D\
 "DYNALINK" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wmodem.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wmodemi.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wmodem.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\modmdll.sbr" \
	"$(INTDIR)\regmo.sbr" \
	"$(INTDIR)\Ser.sbr" \
	"$(INTDIR)\serio.sbr"

"$(OUTDIR)\wmodem.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wmodemd.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib advapi32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wmodemd.pdb" /debug /machine:I386\
 /out:"..\out\dll\wmodemd.dll" /implib:"$(OUTDIR)/wmodemd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\modmdll.obj" \
	"$(INTDIR)\regmo.obj" \
	"$(INTDIR)\Ser.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wmodemi.res"

"..\out\dll\wmodemd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "modem___"
# PROP BASE Intermediate_Dir "modem___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wmodem2d"
# PROP Intermediate_Dir "wmodem2d"
# PROP Target_Dir ""
OUTDIR=.\wmodem2d
INTDIR=.\wmodem2d

ALL : "$(OUTDIR)\wmodem2d.dll" "$(OUTDIR)\wmodem.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\modmdll.obj"
	-@erase "$(INTDIR)\modmdll.sbr"
	-@erase "$(INTDIR)\regmo.obj"
	-@erase "$(INTDIR)\regmo.sbr"
	-@erase "$(INTDIR)\Ser.obj"
	-@erase "$(INTDIR)\Ser.sbr"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\serio.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wmodemi.res"
	-@erase "$(OUTDIR)\wmodem.bsc"
	-@erase "$(OUTDIR)\wmodem2d.dll"
	-@erase "$(OUTDIR)\wmodem2d.exp"
	-@erase "$(OUTDIR)\wmodem2d.lib"
	-@erase "$(OUTDIR)\wmodem2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /FR /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /I "..\stub" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I\
 "..\..\..\h" /I "..\stub" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D\
 "DYNALINK" /D "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wmodem.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wmodem2d/
CPP_SBRS=.\wmodem2d/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wmodemi.res" /d "_DEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wmodem.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\modmdll.sbr" \
	"$(INTDIR)\regmo.sbr" \
	"$(INTDIR)\Ser.sbr" \
	"$(INTDIR)\serio.sbr"

"$(OUTDIR)\wmodem.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wmodemd.dll"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"wmodem2d\wmodem2d.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib advapi32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wmodem2d.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/wmodem2d.dll" /implib:"$(OUTDIR)/wmodem2d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\modmdll.obj" \
	"$(INTDIR)\regmo.obj" \
	"$(INTDIR)\Ser.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wmodemi.res"

"$(OUTDIR)\wmodem2d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "modem__0"
# PROP BASE Intermediate_Dir "modem__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wmodem2"
# PROP Intermediate_Dir "wmodem2"
# PROP Target_Dir ""
OUTDIR=.\wmodem2
INTDIR=.\wmodem2

ALL : "$(OUTDIR)\wmodem2.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\modmdll.obj"
	-@erase "$(INTDIR)\regmo.obj"
	-@erase "$(INTDIR)\Ser.obj"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\wmodemi.res"
	-@erase "$(OUTDIR)\wmodem2.dll"
	-@erase "$(OUTDIR)\wmodem2.exp"
	-@erase "$(OUTDIR)\wmodem2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\main" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\main" /I "..\..\..\h" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /D "dp_ANET2" /FAcs /YX /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\main" /I\
 "..\..\..\h" /I "..\stub" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D\
 "DYNALINK" /D "dp_ANET2" /FAcs /Fa"$(INTDIR)/" /Fp"$(INTDIR)/wmodem.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\wmodem2/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wmodemi.res" /d "NDEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wmodem.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc" /out:"..\out\dll\wmodem.dll"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc" /out:"wmodem2\wmodem2.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib advapi32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/wmodem2.pdb" /machine:I386 /nodefaultlib:"libc"\
 /out:"$(OUTDIR)/wmodem2.dll" /implib:"$(OUTDIR)/wmodem2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\modmdll.obj" \
	"$(INTDIR)\regmo.obj" \
	"$(INTDIR)\Ser.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wmodemi.res"

"$(OUTDIR)\wmodem2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "modem - Win32 Release"
# Name "modem - Win32 Debug"
# Name "modem - Win32 V2 Debug"
# Name "modem - Win32 V2 Release"

!IF  "$(CFG)" == "modem - Win32 Release"

!ELSEIF  "$(CFG)" == "modem - Win32 Debug"

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\modmdll.c
DEP_CPP_MODMD=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\h\ddprint.h"\
	"..\main\ser.h"\
	"..\main\serio.h"\
	"..\stub\anetstub.h"\
	".\regmo.h"\
	

!IF  "$(CFG)" == "modem - Win32 Release"


"$(INTDIR)\modmdll.obj" : $(SOURCE) $(DEP_CPP_MODMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modem - Win32 Debug"


"$(INTDIR)\modmdll.obj" : $(SOURCE) $(DEP_CPP_MODMD) "$(INTDIR)"

"$(INTDIR)\modmdll.sbr" : $(SOURCE) $(DEP_CPP_MODMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"


"$(INTDIR)\modmdll.obj" : $(SOURCE) $(DEP_CPP_MODMD) "$(INTDIR)"

"$(INTDIR)\modmdll.sbr" : $(SOURCE) $(DEP_CPP_MODMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"


"$(INTDIR)\modmdll.obj" : $(SOURCE) $(DEP_CPP_MODMD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\Main\Ser.c
DEP_CPP_SER_C=\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dpmem.h"\
	"..\h\ddprint.h"\
	"..\main\ser.h"\
	"..\main\serio.h"\
	

!IF  "$(CFG)" == "modem - Win32 Release"


"$(INTDIR)\Ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modem - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Ser.sbr" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Ser.sbr" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"


"$(INTDIR)\Ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wmodemi.rc

!IF  "$(CFG)" == "modem - Win32 Release"

DEP_RSC_WMODE=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	
NODEP_RSC_WMODE=\
	".\Release\(comm_INIT_NEEDS_SESSIONID"\
	

"$(INTDIR)\wmodemi.res" : $(SOURCE) $(DEP_RSC_WMODE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modem - Win32 Debug"

DEP_RSC_WMODE=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	
NODEP_RSC_WMODE=\
	".\Debug\(comm_INIT_NEEDS_SESSIONID"\
	

"$(INTDIR)\wmodemi.res" : $(SOURCE) $(DEP_RSC_WMODE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"

DEP_RSC_WMODE=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	
NODEP_RSC_WMODE=\
	".\wmodem2d\(comm_INIT_NEEDS_SESSIONID"\
	

"$(INTDIR)\wmodemi.res" : $(SOURCE) $(DEP_RSC_WMODE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"

DEP_RSC_WMODE=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	
NODEP_RSC_WMODE=\
	".\wmodem2\(comm_INIT_NEEDS_SESSIONID"\
	

"$(INTDIR)\wmodemi.res" : $(SOURCE) $(DEP_RSC_WMODE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\serio.c
DEP_CPP_SERIO=\
	"..\..\..\h\commerr.h"\
	"..\h\ddprint.h"\
	"..\main\serio.h"\
	

!IF  "$(CFG)" == "modem - Win32 Release"


"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modem - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\serio.sbr" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\serio.sbr" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"


"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ddprint.c

!IF  "$(CFG)" == "modem - Win32 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modem - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regmo.c
DEP_CPP_REGMO=\
	".\regmo.h"\
	

!IF  "$(CFG)" == "modem - Win32 Release"


"$(INTDIR)\regmo.obj" : $(SOURCE) $(DEP_CPP_REGMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modem - Win32 Debug"


"$(INTDIR)\regmo.obj" : $(SOURCE) $(DEP_CPP_REGMO) "$(INTDIR)"

"$(INTDIR)\regmo.sbr" : $(SOURCE) $(DEP_CPP_REGMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modem - Win32 V2 Debug"


"$(INTDIR)\regmo.obj" : $(SOURCE) $(DEP_CPP_REGMO) "$(INTDIR)"

"$(INTDIR)\regmo.sbr" : $(SOURCE) $(DEP_CPP_REGMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "modem - Win32 V2 Release"


"$(INTDIR)\regmo.obj" : $(SOURCE) $(DEP_CPP_REGMO) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
