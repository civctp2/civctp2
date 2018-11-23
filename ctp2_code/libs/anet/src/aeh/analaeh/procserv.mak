# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=procserv - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to procserv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "procserv - Win32 Release" && "$(CFG)" !=\
 "procserv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "procserv.mak" CFG="procserv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "procserv - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "procserv - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "procserv - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "procserv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "procserv"
# PROP Intermediate_Dir "procserv"
# PROP Target_Dir ""
OUTDIR=.\procserv
INTDIR=.\procserv

ALL : "$(OUTDIR)\procserv.exe"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\procserv.obj"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(OUTDIR)\procserv.exe"
	-@erase "$(OUTDIR)\procserv.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I ".." /I "..\..\dp" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /D "NO_NETWORK" /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I\
 ".." /I "..\..\dp" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D\
 "dp_ANET2" /D "NO_NETWORK" /Fp"$(INTDIR)/procserv.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\procserv/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/procserv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/procserv.pdb" /map:"$(INTDIR)/procserv.map"\
 /machine:I386 /out:"$(OUTDIR)/procserv.exe" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\procserv.obj" \
	"$(INTDIR)\tlstuff.obj"

"$(OUTDIR)\procserv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "procservd"
# PROP Intermediate_Dir "procservd"
# PROP Target_Dir ""
OUTDIR=.\procservd
INTDIR=.\procservd

ALL : "$(OUTDIR)\procservd.exe" "$(OUTDIR)\procservd.bsc"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aeh.sbr"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\aehlog.sbr"
	-@erase "$(INTDIR)\aehmap.obj"
	-@erase "$(INTDIR)\aehmap.sbr"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\dpcrc32.sbr"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\dpprivy.sbr"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\eclock.sbr"
	-@erase "$(INTDIR)\procserv.obj"
	-@erase "$(INTDIR)\procserv.sbr"
	-@erase "$(INTDIR)\tlstuff.obj"
	-@erase "$(INTDIR)\tlstuff.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\procservd.bsc"
	-@erase "$(OUTDIR)\procservd.exe"
	-@erase "$(OUTDIR)\procservd.map"
	-@erase "$(OUTDIR)\procservd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /I ".." /I "..\..\dp" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /D "NO_NETWORK" /FR /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /I ".." /I "..\..\dp" /D "_DEBUG" /D "WIN32" /D\
 "_CONSOLE" /D "_WINDOWS" /D "dp_ANET2" /D "NO_NETWORK" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/procserv.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\procservd/
CPP_SBRS=.\procservd/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"procservd/procservd.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/procservd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\aeh.sbr" \
	"$(INTDIR)\aehlog.sbr" \
	"$(INTDIR)\aehmap.sbr" \
	"$(INTDIR)\dpcrc32.sbr" \
	"$(INTDIR)\dpprivy.sbr" \
	"$(INTDIR)\eclock.sbr" \
	"$(INTDIR)\procserv.sbr" \
	"$(INTDIR)\tlstuff.sbr"

"$(OUTDIR)\procservd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"procservd/procservd.exe"
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/procservd.pdb" /map:"$(INTDIR)/procservd.map"\
 /debug /machine:I386 /out:"$(OUTDIR)/procservd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\aehmap.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\procserv.obj" \
	"$(INTDIR)\tlstuff.obj"

"$(OUTDIR)\procservd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "procserv - Win32 Release"
# Name "procserv - Win32 Debug"

!IF  "$(CFG)" == "procserv - Win32 Release"

!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\procserv.c
DEP_CPP_PROCS=\
	"..\..\..\demo\utils\dpcrc32.h"\
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\dp\dpprivy.h"\
	"..\aeh.h"\
	"..\aehlog.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\procserv.obj" : $(SOURCE) $(DEP_CPP_PROCS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


"$(INTDIR)\procserv.obj" : $(SOURCE) $(DEP_CPP_PROCS) "$(INTDIR)"

"$(INTDIR)\procserv.sbr" : $(SOURCE) $(DEP_CPP_PROCS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\tlstuff.c
DEP_CPP_TLSTU=\
	"..\aeh.h"\
	"..\tlstuff.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\tlstuff.obj" : $(SOURCE) $(DEP_CPP_TLSTU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\aehmap.obj" : $(SOURCE) $(DEP_CPP_AEHMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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
	"..\..\..\demo\utils\dpcrc32.h"\
	"..\..\..\h\eclock.h"\
	"..\..\dp\dpprivy.h"\
	"..\aeh.h"\
	"..\detect.h"\
	"..\tlstuff.h"\
	"..\win4alt.h"\
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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

SOURCE=..\..\..\demo\utils\dpcrc32.c
DEP_CPP_DPCRC=\
	"..\..\..\demo\utils\dpcrc32.h"\
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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
	"..\..\..\h\anet.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\commerr.h"\
	"..\..\..\h\delay.h"\
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
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	

!IF  "$(CFG)" == "procserv - Win32 Release"


"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "procserv - Win32 Debug"


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
