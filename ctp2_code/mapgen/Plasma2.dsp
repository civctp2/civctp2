# Microsoft Developer Studio Project File - Name="Plasma2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Plasma2 - Win32 Debug Browse
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Plasma2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Plasma2.mak" CFG="Plasma2 - Win32 Debug Browse"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Plasma2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Test" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 NDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Leaks" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Debug Browse" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ctp", BAAAAAAA"
# PROP Scc_LocalPath "..\ctp"
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Plasma2 - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zd /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Plasma2_"
# PROP BASE Intermediate_Dir "Plasma2_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Plasma20"
# PROP BASE Intermediate_Dir "Plasma20"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\ctp2\\" /I "..\ctp2\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_TEST" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp2\dll\map\plasma2.dll" /pdbtype:sept
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 NDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Plasma2_"
# PROP BASE Intermediate_Dir "Plasma2_"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Plasma2_"
# PROP Intermediate_Dir "Plasma2_"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Plasma20"
# PROP BASE Intermediate_Dir "Plasma20"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "final"
# PROP Intermediate_Dir "final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp2\dll\map\plasma2.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Leaks"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Plasma20"
# PROP BASE Intermediate_Dir "Plasma20"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Plasma20"
# PROP Intermediate_Dir "Plasma20"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "_PLAYTEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Debug Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Plasma2___Win32_Debug_Browse"
# PROP BASE Intermediate_Dir "Plasma2___Win32_Debug_Browse"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "_PLAYTEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\ctp\dll\map\plasma2.dll"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "Plasma2 - Win32 Release"
# Name "Plasma2 - Win32 Debug"
# Name "Plasma2 - Win32 Test"
# Name "Plasma2 - Win32 NDebug"
# Name "Plasma2 - Win32 Final"
# Name "Plasma2 - Win32 Leaks"
# Name "Plasma2 - Win32 Debug Browse"
# Begin Source File

SOURCE=.\Plasma2.def
# End Source File
# Begin Source File

SOURCE=.\PlasmaGen2.cpp
# End Source File
# Begin Source File

SOURCE=.\PlasmaGen2.h
# End Source File
# End Target
# End Project
