# Microsoft Developer Studio Project File - Name="dbgen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dbgen - Win32 Debug Browse
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbgen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbgen.mak" CFG="dbgen - Win32 Debug Browse"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbgen - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbgen - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dbgen - Win32 Debug Browse" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ctp", BAAAAAAA"
# PROP Scc_LocalPath "..\..\ctp"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbgen - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\ctp\ctp2_utils" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"ctpdb.exe"

!ELSEIF  "$(CFG)" == "dbgen - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\ctp\ctp2_utils" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"ctpdb.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "dbgen - Win32 Debug Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dbgen___Win32_Debug_Browse"
# PROP BASE Intermediate_Dir "dbgen___Win32_Debug_Browse"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\ctp\ctp2_utils" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\ctp\ctp2_utils" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"ctpdb.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"ctpdb.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "dbgen - Win32 Release"
# Name "dbgen - Win32 Debug"
# Name "dbgen - Win32 Debug Browse"
# Begin Source File

SOURCE=.\ctpdb.cpp
# End Source File
# Begin Source File

SOURCE=.\ctpdb.h
# End Source File
# Begin Source File

SOURCE=.\ctpdb.l

!IF  "$(CFG)" == "dbgen - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\ctpdb.l

"$(ProjDir)\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "dbgen - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\ctpdb.l

"$(ProjDir)\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "dbgen - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=.\ctpdb.l

"$(ProjDir)\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\lex.yy.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctpdb.y

!IF  "$(CFG)" == "dbgen - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\ctpdb.y

BuildCmds= \
	$(CDKDIR)\byacc -d ctpdb.y

"$(ProjDir)\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "dbgen - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\ctpdb.y

BuildCmds= \
	$(CDKDIR)\byacc -d ctpdb.y

"$(ProjDir)\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "dbgen - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=.\ctpdb.y

BuildCmds= \
	$(CDKDIR)\byacc -d ctpdb.y

"$(ProjDir)\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Datum.cpp
# End Source File
# Begin Source File

SOURCE=.\Datum.h
# End Source File
# Begin Source File

SOURCE=.\lex.yy.c
# End Source File
# Begin Source File

SOURCE=.\MemberClass.cpp
# End Source File
# Begin Source File

SOURCE=.\MemberClass.h
# End Source File
# Begin Source File

SOURCE=.\RecordDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordDescription.h
# End Source File
# Begin Source File

SOURCE=.\y.tab.c
# End Source File
# Begin Source File

SOURCE=.\y.tab.h
# End Source File
# End Target
# End Project
