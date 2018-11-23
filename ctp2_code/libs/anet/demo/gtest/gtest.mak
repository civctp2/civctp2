# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=gtest - Win32 anet2 Release
!MESSAGE No configuration specified.  Defaulting to gtest - Win32 anet2\
 Release.
!ENDIF 

!IF "$(CFG)" != "gtest - Win32 anet2 Debug" && "$(CFG)" !=\
 "gtest - Win32 dp2 Release" && "$(CFG)" != "gtest - Win32 dp2 Debug" &&\
 "$(CFG)" != "gtest - Win32 anet2 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "gtest.mak" CFG="gtest - Win32 anet2 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gtest - Win32 anet2 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "gtest - Win32 dp2 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "gtest - Win32 dp2 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "gtest - Win32 anet2 Release" (based on\
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
# PROP Target_Last_Scanned "gtest - Win32 anet2 Release"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gtest___"
# PROP BASE Intermediate_Dir "gtest___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "gtest2dd"
# PROP Intermediate_Dir "gtest2dd"
# PROP Target_Dir ""
OUTDIR=.\gtest2dd
INTDIR=.\gtest2dd

ALL : "$(OUTDIR)\gtest2dd.exe"

CLEAN : 
	-@erase "$(INTDIR)\avgstat.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\gtest.obj"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\gtest2dd.exe"
	-@erase "$(OUTDIR)\gtest2dd.map"
	-@erase "$(OUTDIR)\gtest2dd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\h" /I "..\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\h" /I "..\utils" /D "_DEBUG" /D "DPRNT" /D "DP_DLL" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "dp_ANET2" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\h" /I "..\utils" /D\
 "_DEBUG" /D "DPRNT" /D "DP_DLL" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D\
 "dp_ANET2" /Fp"$(INTDIR)/gtest.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\gtest2dd/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 ..\..\win\lib\anetdlld.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"gtestdd/gtestdd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"gtest2dd/gtest2dd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/gtest2dd.pdb"\
 /map:"$(INTDIR)/gtest2dd.map" /debug /machine:I386\
 /out:"$(OUTDIR)/gtest2dd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\avgstat.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\gtest.obj" \
	"$(INTDIR)\raw.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\gtest2dd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gtest___"
# PROP BASE Intermediate_Dir "gtest___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "gtest2r"
# PROP Intermediate_Dir "gtest2r"
# PROP Target_Dir ""
OUTDIR=.\gtest2r
INTDIR=.\gtest2r

ALL : "$(OUTDIR)\gtest2r.exe"

CLEAN : 
	-@erase "$(INTDIR)\avgstat.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\gtest.obj"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(OUTDIR)\gtest2r.exe"
	-@erase "$(OUTDIR)\gtest2r.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Oy- /I "..\..\h" /I "..\utils" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Oy- /I "..\..\h" /I "..\utils" /D "NDEBUG" /D "dp_ANET2" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Oy- /I "..\..\h" /I "..\utils" /D "NDEBUG" /D\
 "dp_ANET2" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /Fp"$(INTDIR)/gtest.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\gtest2r/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 ..\..\win\lib\dp.lib kernel32.lib advapi32.lib /nologo /subsystem:console /map /machine:I386 /out:"gtestr/gtestr.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\win\lib\dp2.lib kernel32.lib advapi32.lib user32.lib version.lib ..\..\win\lib\dkupddll.lib /nologo /subsystem:console /map /machine:I386 /out:"gtest2r/gtest2r.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\win\lib\dp2.lib kernel32.lib advapi32.lib user32.lib\
 version.lib ..\..\win\lib\dkupddll.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/gtest2r.pdb" /map:"$(INTDIR)/gtest2r.map"\
 /machine:I386 /out:"$(OUTDIR)/gtest2r.exe" 
LINK32_OBJS= \
	"$(INTDIR)\avgstat.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\gtest.obj" \
	"$(INTDIR)\raw.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\gtest2r.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gtest___"
# PROP BASE Intermediate_Dir "gtest___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "gtest2d"
# PROP Intermediate_Dir "gtest2d"
# PROP Target_Dir ""
OUTDIR=.\gtest2d
INTDIR=.\gtest2d

ALL : "$(OUTDIR)\gtest2d.exe"

CLEAN : 
	-@erase "$(INTDIR)\avgstat.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\gtest.obj"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\gtest2d.exe"
	-@erase "$(OUTDIR)\gtest2d.map"
	-@erase "$(OUTDIR)\gtest2d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\h" /I "..\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\h" /I "..\utils" /D "_DEBUG" /D "DPRNT" /D "dp_ANET2" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\h" /I "..\utils" /D\
 "_DEBUG" /D "DPRNT" /D "dp_ANET2" /D "WIN32" /D "_WIN32" /D "_CONSOLE"\
 /Fp"$(INTDIR)/gtest.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\gtest2d/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 ..\..\win\lib\dpd.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"gtestd/gtestd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\win\lib\dp2d.lib kernel32.lib advapi32.lib user32.lib version.lib ..\..\win\lib\dkupddll.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"gtest2d/gtest2d.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\win\lib\dp2d.lib kernel32.lib advapi32.lib user32.lib\
 version.lib ..\..\win\lib\dkupddll.lib /nologo /subsystem:console\
 /incremental:no /pdb:"$(OUTDIR)/gtest2d.pdb" /map:"$(INTDIR)/gtest2d.map"\
 /debug /machine:I386 /out:"$(OUTDIR)/gtest2d.exe" 
LINK32_OBJS= \
	"$(INTDIR)\avgstat.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\gtest.obj" \
	"$(INTDIR)\raw.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\gtest2d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gtest___"
# PROP BASE Intermediate_Dir "gtest___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "gtest2dr"
# PROP Intermediate_Dir "gtest2dr"
# PROP Target_Dir ""
OUTDIR=.\gtest2dr
INTDIR=.\gtest2dr

ALL : "$(OUTDIR)\gtest2dr.exe"

CLEAN : 
	-@erase "$(INTDIR)\avgstat.obj"
	-@erase "$(INTDIR)\dpcrc32.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\gtest.obj"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\gtest2dr.exe"
	-@erase "$(OUTDIR)\gtest2dr.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /O2 /Oy- /I "..\..\h" /I "..\utils" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /D "DPRNT" /D "DP_DLL" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /O2 /Oy- /I "..\..\h" /I "..\utils" /D "_DEBUG" /D "DPRNT" /D "DP_DLL" /D "dp_ANET2" /D "WIN32" /D "_WIN32" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /O2 /Oy- /I "..\..\h" /I "..\utils" /D\
 "_DEBUG" /D "DPRNT" /D "DP_DLL" /D "dp_ANET2" /D "WIN32" /D "_WIN32" /D\
 "_CONSOLE" /Fp"$(INTDIR)/gtest.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\gtest2dr/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/gtest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 ..\..\win\lib\anet2d.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /debug /machine:I386 /out:"gtest2dd/gtest2dd.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 ..\..\win\lib\anet2.lib kernel32.lib advapi32.lib /nologo /subsystem:console /incremental:no /map /machine:I386 /out:"gtest2dr/gtest2dr.exe"
# SUBTRACT LINK32 /debug /nodefaultlib
LINK32_FLAGS=..\..\win\lib\anet2.lib kernel32.lib advapi32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/gtest2dr.pdb"\
 /map:"$(INTDIR)/gtest2dr.map" /machine:I386 /out:"$(OUTDIR)/gtest2dr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\avgstat.obj" \
	"$(INTDIR)\dpcrc32.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\gtest.obj" \
	"$(INTDIR)\raw.obj" \
	"$(INTDIR)\unistd.obj"

"$(OUTDIR)\gtest2dr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "gtest - Win32 anet2 Debug"
# Name "gtest - Win32 dp2 Release"
# Name "gtest - Win32 dp2 Debug"
# Name "gtest - Win32 anet2 Release"

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"

!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"

!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"

!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\gtest.c
DEP_CPP_GTEST=\
	"..\..\h\anet.h"\
	"..\..\h\aneterr.h"\
	"..\..\h\assoctab.h"\
	"..\..\h\commapi.h"\
	"..\..\h\commerr.h"\
	"..\..\h\delay.h"\
	"..\..\h\dp.h"\
	"..\..\h\dp2.h"\
	"..\..\h\dpdll.h"\
	"..\..\h\dpio.h"\
	"..\..\h\dppack1.h"\
	"..\..\h\dppackn.h"\
	"..\..\h\dppkt.h"\
	"..\..\h\dppv.h"\
	"..\..\h\dprint.h"\
	"..\..\h\dptab.h"\
	"..\..\h\dpturn.h"\
	"..\..\h\dpunpack.h"\
	"..\..\h\dynatab.h"\
	"..\..\h\eclock.h"\
	"..\..\h\hkeytab.h"\
	"..\..\h\q.h"\
	"..\..\src\score\scorerep.h"\
	"..\..\src\tca\tca.h"\
	"..\..\src\tca\tcapw.h"\
	"..\..\src\tca\tserv.h"\
	"..\utils\avgstat.h"\
	"..\utils\dpcrc32.h"\
	"..\utils\raw.h"\
	"..\utils\strdup.h"\
	"..\utils\stricmp.h"\
	"..\utils\unistd.h"\
	

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"


"$(INTDIR)\gtest.obj" : $(SOURCE) $(DEP_CPP_GTEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"


"$(INTDIR)\gtest.obj" : $(SOURCE) $(DEP_CPP_GTEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"


"$(INTDIR)\gtest.obj" : $(SOURCE) $(DEP_CPP_GTEST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"


"$(INTDIR)\gtest.obj" : $(SOURCE) $(DEP_CPP_GTEST) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\utils\raw.c
DEP_CPP_RAW_C=\
	"..\utils\raw.h"\
	"..\utils\rawlin.c"\
	"..\utils\rawmsdos.c"\
	"..\utils\rawsun.c"\
	"..\utils\rawvms.c"\
	"..\utils\rawwin.c"\
	"..\utils\unistd.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"


"$(INTDIR)\raw.obj" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"


"$(INTDIR)\raw.obj" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"


"$(INTDIR)\raw.obj" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"


"$(INTDIR)\raw.obj" : $(SOURCE) $(DEP_CPP_RAW_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\utils\avgstat.c
DEP_CPP_AVGST=\
	"..\..\h\anet.h"\
	"..\..\h\aneterr.h"\
	"..\utils\avgstat.h"\
	

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"


"$(INTDIR)\avgstat.obj" : $(SOURCE) $(DEP_CPP_AVGST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"


"$(INTDIR)\avgstat.obj" : $(SOURCE) $(DEP_CPP_AVGST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"


"$(INTDIR)\avgstat.obj" : $(SOURCE) $(DEP_CPP_AVGST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"


"$(INTDIR)\avgstat.obj" : $(SOURCE) $(DEP_CPP_AVGST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\utils\unistd.c
DEP_CPP_UNIST=\
	"..\utils\unistd.h"\
	

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"


"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"


"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"


"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"


"$(INTDIR)\unistd.obj" : $(SOURCE) $(DEP_CPP_UNIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\h\eclock.h"\
	

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"


"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\demo\utils\dpcrc32.c
DEP_CPP_DPCRC=\
	"..\utils\dpcrc32.h"\
	

!IF  "$(CFG)" == "gtest - Win32 anet2 Debug"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 dp2 Debug"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gtest - Win32 anet2 Release"


"$(INTDIR)\dpcrc32.obj" : $(SOURCE) $(DEP_CPP_DPCRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
