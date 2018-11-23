# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=serial - Win32 V2 Debug
!MESSAGE No configuration specified.  Defaulting to serial - Win32 V2 Debug.
!ENDIF 

!IF "$(CFG)" != "serial - Win32 Release" && "$(CFG)" != "serial - Win32 Debug"\
 && "$(CFG)" != "serial - Win32 V2 Debug" && "$(CFG)" !=\
 "serial - Win32 V2 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "wserial.mak" CFG="serial - Win32 V2 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "serial - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "serial - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "serial - Win32 V2 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "serial - Win32 V2 Release" (based on\
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
# PROP Target_Last_Scanned "serial - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "serial - Win32 Release"

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

ALL : "..\out\dll\wserial.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ser.obj"
	-@erase "$(INTDIR)\serdll.obj"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\wseriali.res"
	-@erase "$(OUTDIR)\wserial.exp"
	-@erase "$(OUTDIR)\wserial.lib"
	-@erase "..\out\dll\wserial.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /I "..\h" /I "..\main" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /YX /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /I "..\h" /I "..\main" /I "..\..\..\h" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK"\
 /Fp"$(INTDIR)/wserial.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wseriali.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wserial.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wserial.dll"
# SUBTRACT LINK32 /incremental:yes
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wserial.pdb" /machine:I386 /out:"..\out\dll\wserial.dll"\
 /implib:"$(OUTDIR)/wserial.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\ser.obj" \
	"$(INTDIR)\serdll.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wseriali.res"

"..\out\dll\wserial.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "serial - Win32 Debug"

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

ALL : "..\out\dll\wseriald.dll" "$(OUTDIR)\wserial.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\ser.obj"
	-@erase "$(INTDIR)\ser.sbr"
	-@erase "$(INTDIR)\serdll.obj"
	-@erase "$(INTDIR)\serdll.sbr"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\serio.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wseriali.res"
	-@erase "$(OUTDIR)\wserial.bsc"
	-@erase "$(OUTDIR)\wseriald.exp"
	-@erase "$(OUTDIR)\wseriald.lib"
	-@erase "$(OUTDIR)\wseriald.pdb"
	-@erase "..\out\dll\wseriald.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I\
 "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/wserial.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wseriali.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wserial.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\ser.sbr" \
	"$(INTDIR)\serdll.sbr" \
	"$(INTDIR)\serio.sbr"

"$(OUTDIR)\wserial.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wseriald.dll"
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wseriald.pdb" /debug /machine:I386\
 /out:"..\out\dll\wseriald.dll" /implib:"$(OUTDIR)/wseriald.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\ser.obj" \
	"$(INTDIR)\serdll.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wseriali.res"

"..\out\dll\wseriald.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "serial__"
# PROP BASE Intermediate_Dir "serial__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "wserial2d"
# PROP Intermediate_Dir "wserial2d"
# PROP Target_Dir ""
OUTDIR=.\wserial2d
INTDIR=.\wserial2d

ALL : "$(OUTDIR)\wserial2d.dll" "$(OUTDIR)\wserial.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ddprint.sbr"
	-@erase "$(INTDIR)\ser.obj"
	-@erase "$(INTDIR)\ser.sbr"
	-@erase "$(INTDIR)\serdll.obj"
	-@erase "$(INTDIR)\serdll.sbr"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\serio.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wseriali.res"
	-@erase "$(OUTDIR)\wserial.bsc"
	-@erase "$(OUTDIR)\wserial2d.dll"
	-@erase "$(OUTDIR)\wserial2d.exp"
	-@erase "$(OUTDIR)\wserial2d.lib"
	-@erase "$(OUTDIR)\wserial2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /FR /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /D "dp_ANET2" /FR /YX /c
CPP_PROJ=/nologo /Zp1 /MDd /W3 /Gm /GX /Zi /Od /I "..\h" /I "..\main" /I\
 "..\..\..\h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /D\
 "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/wserial.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\wserial2d/
CPP_SBRS=.\wserial2d/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wseriali.res" /d "_DEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wserial.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ddprint.sbr" \
	"$(INTDIR)\ser.sbr" \
	"$(INTDIR)\serdll.sbr" \
	"$(INTDIR)\serio.sbr"

"$(OUTDIR)\wserial.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\out\dll\wseriald.dll"
# ADD LINK32 kernel32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"wserial2d\wserial2d.dll"
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wserial2d.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/wserial2d.dll" /implib:"$(OUTDIR)/wserial2d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\ser.obj" \
	"$(INTDIR)\serdll.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wseriali.res"

"$(OUTDIR)\wserial2d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "serial_0"
# PROP BASE Intermediate_Dir "serial_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "wserial2"
# PROP Intermediate_Dir "wserial2"
# PROP Target_Dir ""
OUTDIR=.\wserial2
INTDIR=.\wserial2

ALL : "$(OUTDIR)\wserial2.dll"

CLEAN : 
	-@erase "$(INTDIR)\ddprint.obj"
	-@erase "$(INTDIR)\ser.obj"
	-@erase "$(INTDIR)\serdll.obj"
	-@erase "$(INTDIR)\serio.obj"
	-@erase "$(INTDIR)\wseriali.res"
	-@erase "$(OUTDIR)\wserial2.dll"
	-@erase "$(OUTDIR)\wserial2.exp"
	-@erase "$(OUTDIR)\wserial2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\main" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\main" /I "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /D "dp_ANET2" /FAcs /YX /c
CPP_PROJ=/nologo /Zp1 /MD /W3 /GX /O2 /Oy- /I "..\h" /I "..\main" /I\
 "..\..\..\h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "DYNALINK" /D\
 "dp_ANET2" /FAcs /Fa"$(INTDIR)/" /Fp"$(INTDIR)/wserial.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\wserial2/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "dp_ANET2"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/wseriali.res" /d "NDEBUG" /d "dp_ANET2" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/wserial.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\out\dll\wserial.dll"
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 kernel32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"wserial2\wserial2.dll"
# SUBTRACT LINK32 /incremental:yes
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/wserial2.pdb" /machine:I386 /out:"$(OUTDIR)/wserial2.dll"\
 /implib:"$(OUTDIR)/wserial2.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ddprint.obj" \
	"$(INTDIR)\ser.obj" \
	"$(INTDIR)\serdll.obj" \
	"$(INTDIR)\serio.obj" \
	"$(INTDIR)\wseriali.res"

"$(OUTDIR)\wserial2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "serial - Win32 Release"
# Name "serial - Win32 Debug"
# Name "serial - Win32 V2 Debug"
# Name "serial - Win32 V2 Release"

!IF  "$(CFG)" == "serial - Win32 Release"

!ELSEIF  "$(CFG)" == "serial - Win32 Debug"

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\serdll.c
DEP_CPP_SERDL=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\h\ddprint.h"\
	"..\main\ser.h"\
	"..\main\serio.h"\
	

!IF  "$(CFG)" == "serial - Win32 Release"


"$(INTDIR)\serdll.obj" : $(SOURCE) $(DEP_CPP_SERDL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "serial - Win32 Debug"


"$(INTDIR)\serdll.obj" : $(SOURCE) $(DEP_CPP_SERDL) "$(INTDIR)"

"$(INTDIR)\serdll.sbr" : $(SOURCE) $(DEP_CPP_SERDL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"


"$(INTDIR)\serdll.obj" : $(SOURCE) $(DEP_CPP_SERDL) "$(INTDIR)"

"$(INTDIR)\serdll.sbr" : $(SOURCE) $(DEP_CPP_SERDL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"


"$(INTDIR)\serdll.obj" : $(SOURCE) $(DEP_CPP_SERDL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ser.c
DEP_CPP_SER_C=\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dpmem.h"\
	"..\h\ddprint.h"\
	"..\main\ser.h"\
	"..\main\serio.h"\
	

!IF  "$(CFG)" == "serial - Win32 Release"


"$(INTDIR)\ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "serial - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ser.sbr" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ser.sbr" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"


"$(INTDIR)\ser.obj" : $(SOURCE) $(DEP_CPP_SER_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wseriali.rc
DEP_RSC_WSERI=\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	

!IF  "$(CFG)" == "serial - Win32 Release"


"$(INTDIR)\wseriali.res" : $(SOURCE) $(DEP_RSC_WSERI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "serial - Win32 Debug"


"$(INTDIR)\wseriali.res" : $(SOURCE) $(DEP_RSC_WSERI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"


"$(INTDIR)\wseriali.res" : $(SOURCE) $(DEP_RSC_WSERI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"


"$(INTDIR)\wseriali.res" : $(SOURCE) $(DEP_RSC_WSERI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\serio.c
DEP_CPP_SERIO=\
	"..\..\..\h\commerr.h"\
	"..\main\serio.h"\
	

!IF  "$(CFG)" == "serial - Win32 Release"


"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "serial - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\serio.sbr" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\serio.sbr" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"


"$(INTDIR)\serio.obj" : $(SOURCE) $(DEP_CPP_SERIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\main\ddprint.c

!IF  "$(CFG)" == "serial - Win32 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "serial - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ddprint.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "serial - Win32 V2 Release"


"$(INTDIR)\ddprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
