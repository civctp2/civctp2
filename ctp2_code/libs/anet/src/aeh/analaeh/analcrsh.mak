# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=analcrsh - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to analcrsh - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "analcrsh - Win32 Release" && "$(CFG)" !=\
 "analcrsh - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "analcrsh.mak" CFG="analcrsh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "analcrsh - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "analcrsh - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "analcrsh - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "analcrsh - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "analcrsh"
# PROP Intermediate_Dir "analcrsh"
# PROP Target_Dir ""
OUTDIR=.\analcrsh
INTDIR=.\analcrsh

ALL : "$(OUTDIR)\analcrsh.exe"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\analcrsh.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(OUTDIR)\analcrsh.exe"
	-@erase "$(OUTDIR)\analcrsh.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I\
 ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2"\
 /Fp"$(INTDIR)/analcrsh.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\analcrsh/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/analcrsh.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/analcrsh.pdb" /map:"$(INTDIR)/analcrsh.map"\
 /machine:I386 /out:"$(OUTDIR)/analcrsh.exe" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\analcrsh.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\tlstuff.obj"

"$(OUTDIR)\analcrsh.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "analcrshd"
# PROP Intermediate_Dir "analcrshd"
# PROP Target_Dir ""
OUTDIR=.\analcrshd
INTDIR=.\analcrshd

ALL : "$(OUTDIR)\analcrshd.exe" "$(OUTDIR)\analcrshd.bsc"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aeh.sbr"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehlog.sbr"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\aehmap.sbr"
	-@erase "$(INTDIR)\analcrsh.obj"
	-@erase "$(INTDIR)\analcrsh.sbr"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpcrc32.sbr"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\dpprivy.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tlstuff.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\analcrshd.bsc"
	-@erase "$(OUTDIR)\analcrshd.exe"
	-@erase "$(OUTDIR)\analcrshd.map"
	-@erase "$(OUTDIR)\analcrshd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /Zp8 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /FR /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /Zp8 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "_WINDOWS" /D "dp_ANET2" /FR"$(INTDIR)/" /Fp"$(INTDIR)/analcrsh.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\analcrshd/
CPP_SBRS=.\analcrshd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"analcrshd/analcrshd.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/analcrshd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\aeh.sbr" \
	"$(INTDIR)\aehlog.sbr" \
	"$(INTDIR)\aehmap.sbr" \
	"$(INTDIR)\analcrsh.sbr" \
	"$(INTDIR)\dpcrc32.sbr" \
	"$(INTDIR)\dpprivy.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\tlstuff.sbr"

"$(OUTDIR)\analcrshd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"analcrshd/analcrshd.exe"
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/analcrshd.pdb" /map:"$(INTDIR)/analcrshd.map"\
 /debug /machine:I386 /out:"$(OUTDIR)/analcrshd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\analcrsh.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\tlstuff.obj"

"$(OUTDIR)\analcrshd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "analcrsh - Win32 Release"
# Name "analcrsh - Win32 Debug"

!IF  "$(CFG)" == "analcrsh - Win32 Release"

!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\analcrsh.c
NODEP_CPP_ANALC=\
	".\aehlog.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\analcrsh.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


"$(INTDIR)\analcrsh.obj" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\analcrsh.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\tlstuff.c
DEP_CPP_TLSTU=\
	"..\aeh.h"\
	"..\tlstuff.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tlstuff.sbr" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\aehlog.c
DEP_CPP_AEHLO=\
	"..\..\dp\dpprivy.h"\
	"..\aeh.h"\
	"..\aehlog.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\aehlog.sbr" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\aehmap.c
DEP_CPP_AEHMA=\
	"..\aeh.h"\
	"..\tlstuff.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\aehmap.sbr" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\aeh.c
DEP_CPP_AEH_C=\
	"..\..\..\h\eclock.h"\
	"..\..\dp\dpprivy.h"\
	"..\aeh.h"\
	"..\detect.h"\
	"..\tlstuff.h"\
	"..\win4alt.h"\
	
NODEP_CPP_AEH_C=\
	"..\dpcrc32.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\aeh.sbr" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\eclock.c
NODEP_CPP_ECLOC=\
	"..\..\..\demo\utils\eclock.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\eclock.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\eclock.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\..\demo\utils\dpcrc32.c
DEP_CPP_DPCRC=\
	"..\..\..\demo\utils\dpcrc32.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpcrc32.sbr" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\..\dp\dpprivy.c
DEP_CPP_DPPRI=\
	"..\..\dp\dpprivy.h"\
	
NODEP_CPP_DPPRI=\
	"..\..\dp\dp.h"\
	

!IF  "$(CFG)" == "analcrsh - Win32 Release"


"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "analcrsh - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dpprivy.sbr" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
