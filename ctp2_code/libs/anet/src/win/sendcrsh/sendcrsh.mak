# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=sendcrsh - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to sendcrsh - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "sendcrsh - Win32 Release" && "$(CFG)" !=\
 "sendcrsh - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "sendcrsh.mak" CFG="sendcrsh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sendcrsh - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sendcrsh - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "sendcrsh - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "sendcrsh - Win32 Release"

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

ALL : ".\sendcrsh.exe"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\logprint.obj"
	-@erase "$(INTDIR)\sbdclnt.obj"
	-@erase "$(INTDIR)\sendbox.obj"
	-@erase "$(INTDIR)\sendbox.res"
	-@erase "$(INTDIR)\sendcrsh.obj"
	-@erase ".\sendcrsh.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I "..\..\dp" /I "..\..\aeh" /I "..\..\sbd" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "dp_ANET2" /D "aeh_IO_ONLY" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\h" /I "..\..\..\demo\utils" /I\
 "..\..\dp" /I "..\..\aeh" /I "..\..\sbd" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WIN32" /D "dp_ANET2" /D "aeh_IO_ONLY" /Fp"$(INTDIR)/sendcrsh.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/sendbox.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/sendcrsh.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 user32.lib gdi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /out:"sendcrsh.exe"
LINK32_FLAGS=user32.lib gdi32.lib wsock32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/sendcrsh.pdb" /machine:I386 /out:"sendcrsh.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\logprint.obj" \
	"$(INTDIR)\sbdclnt.obj" \
	"$(INTDIR)\sendbox.obj" \
	"$(INTDIR)\sendbox.res" \
	"$(INTDIR)\sendcrsh.obj"

".\sendcrsh.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sendcrsh - Win32 Debug"

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

ALL : ".\sendcrshd.exe"

CLEAN : 
	-@erase "$(INTDIR)\aeh.obj"
	-@erase "$(INTDIR)\aehlog.obj"
	-@erase "$(INTDIR)\dpprivy.obj"
	-@erase "$(INTDIR)\eclock.obj"
	-@erase "$(INTDIR)\logprint.obj"
	-@erase "$(INTDIR)\sbdclnt.obj"
	-@erase "$(INTDIR)\sendbox.obj"
	-@erase "$(INTDIR)\sendbox.res"
	-@erase "$(INTDIR)\sendcrsh.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\sendcrshd.pdb"
	-@erase ".\sendcrshd.exe"
	-@erase ".\sendcrshd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I "..\..\..\demo\utils" /I "..\..\dp" /I "..\..\aeh" /I "..\..\sbd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "dp_ANET2" /D "aeh_IO_ONLY" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\..\h" /I\
 "..\..\..\demo\utils" /I "..\..\dp" /I "..\..\aeh" /I "..\..\sbd" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "dp_ANET2" /D "aeh_IO_ONLY"\
 /Fp"$(INTDIR)/sendcrsh.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/sendbox.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/sendcrsh.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 user32.lib gdi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"sendcrshd.exe"
LINK32_FLAGS=user32.lib gdi32.lib wsock32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/sendcrshd.pdb" /debug /machine:I386\
 /out:"sendcrshd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\aeh.obj" \
	"$(INTDIR)\aehlog.obj" \
	"$(INTDIR)\dpprivy.obj" \
	"$(INTDIR)\eclock.obj" \
	"$(INTDIR)\logprint.obj" \
	"$(INTDIR)\sbdclnt.obj" \
	"$(INTDIR)\sendbox.obj" \
	"$(INTDIR)\sendbox.res" \
	"$(INTDIR)\sendcrsh.obj"

".\sendcrshd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "sendcrsh - Win32 Release"
# Name "sendcrsh - Win32 Debug"

!IF  "$(CFG)" == "sendcrsh - Win32 Release"

!ELSEIF  "$(CFG)" == "sendcrsh - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\sendcrsh.c
DEP_CPP_SENDC=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\eclock.h"\
	"..\..\..\h\portable.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\aehlog.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\sbd\sbd.h"\
	"..\..\sbd\sbdclnt.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	".\sendbox.h"\
	{$(INCLUDE)}"\anet.h"\
	{$(INCLUDE)}"\commerr.h"\
	{$(INCLUDE)}"\delay.h"\
	{$(INCLUDE)}"\dpdll.h"\
	{$(INCLUDE)}"\dppackn.h"\
	{$(INCLUDE)}"\dppkt.h"\
	{$(INCLUDE)}"\dptab.h"\
	{$(INCLUDE)}"\dynatab.h"\
	{$(INCLUDE)}"\hkeytab.h"\
	{$(INCLUDE)}"\q.h"\
	

"$(INTDIR)\sendcrsh.obj" : $(SOURCE) $(DEP_CPP_SENDC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sendbox.h

!IF  "$(CFG)" == "sendcrsh - Win32 Release"

!ELSEIF  "$(CFG)" == "sendcrsh - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sendbox.rc
DEP_RSC_SENDB=\
	".\sendboxrc.h"\
	

"$(INTDIR)\sendbox.res" : $(SOURCE) $(DEP_RSC_SENDB) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sendboxrc.h

!IF  "$(CFG)" == "sendcrsh - Win32 Release"

!ELSEIF  "$(CFG)" == "sendcrsh - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sendbox.c
DEP_CPP_SENDBO=\
	".\sendbox.h"\
	".\sendboxrc.h"\
	

"$(INTDIR)\sendbox.obj" : $(SOURCE) $(DEP_CPP_SENDBO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\src\aeh\aehlog.c
DEP_CPP_AEHLO=\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\aehlog.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	

"$(INTDIR)\aehlog.obj" : $(SOURCE) $(DEP_CPP_AEHLO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\src\aeh\aeh.c
DEP_CPP_AEH_C=\
	"..\..\..\demo\utils\dpcrc32.h"\
	"..\..\..\h\eclock.h"\
	"..\..\aeh\aeh.h"\
	"..\..\aeh\detect.h"\
	"..\..\aeh\tlstuff.h"\
	"..\..\aeh\win4alt.h"\
	"..\..\dp\dpprivy.h"\
	

"$(INTDIR)\aeh.obj" : $(SOURCE) $(DEP_CPP_AEH_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\src\sbd\sbdclnt.c
DEP_CPP_SBDCL=\
	"..\..\..\demo\utils\logprint.h"\
	"..\..\..\demo\utils\unistd.h"\
	"..\..\..\h\portable.h"\
	"..\..\sbd\sbd.h"\
	"..\..\sbd\sbdclnt.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\sbdclnt.obj" : $(SOURCE) $(DEP_CPP_SBDCL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\demo\utils\logprint.c

"$(INTDIR)\logprint.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\demo\utils\eclock.c
DEP_CPP_ECLOC=\
	"..\..\..\h\eclock.h"\
	

"$(INTDIR)\eclock.obj" : $(SOURCE) $(DEP_CPP_ECLOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\mw2\commdll\src\dp\dpprivy.c
DEP_CPP_DPPRI=\
	"..\..\..\h\aneterr.h"\
	"..\..\..\h\assoctab.h"\
	"..\..\..\h\commapi.h"\
	"..\..\..\h\dp2.h"\
	"..\..\..\h\dpio.h"\
	"..\..\..\h\dppack1.h"\
	"..\..\..\h\dppv.h"\
	"..\..\..\h\dpturn.h"\
	"..\..\..\h\dpunpack.h"\
	"..\..\..\h\eclock.h"\
	"..\..\dp\dpprivy.h"\
	"..\..\score\scorerep.h"\
	"..\..\tca\tca.h"\
	"..\..\tca\tcapw.h"\
	"..\..\tca\tserv.h"\
	{$(INCLUDE)}"\anet.h"\
	{$(INCLUDE)}"\commerr.h"\
	{$(INCLUDE)}"\delay.h"\
	{$(INCLUDE)}"\dp.h"\
	{$(INCLUDE)}"\dpdll.h"\
	{$(INCLUDE)}"\dppackn.h"\
	{$(INCLUDE)}"\dppkt.h"\
	{$(INCLUDE)}"\dptab.h"\
	{$(INCLUDE)}"\dynatab.h"\
	{$(INCLUDE)}"\hkeytab.h"\
	{$(INCLUDE)}"\q.h"\
	

"$(INTDIR)\dpprivy.obj" : $(SOURCE) $(DEP_CPP_DPPRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
