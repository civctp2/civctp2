# Microsoft Developer Studio Project File - Name="geometric" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=geometric - Win32 Debug Browse
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "geometric.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "geometric.mak" CFG="geometric - Win32 Debug Browse"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "geometric - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - Win32 Test" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - Win32 NDebug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - Win32 Final" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - Win32 Leaks" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - Win32 Debug Browse" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - SDL Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "geometric - SDL Final" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ctp", BAAAAAAA"
# PROP Scc_LocalPath "..\ctp"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "geometric - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geometri"
# PROP BASE Intermediate_Dir "geometri"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "geometri"
# PROP Intermediate_Dir "geometri"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zd /Og /Oi /Os /Op /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /FD /c
# SUBTRACT CPP /Ox /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometr0"
# PROP BASE Intermediate_Dir "geometr0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "geometr0"
# PROP Intermediate_Dir "geometr0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometr1"
# PROP BASE Intermediate_Dir "geometr1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "geometr1"
# PROP Intermediate_Dir "geometr1"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:"..\ctp\dll\map\geometric.pdb" /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:"..\ctp\dll\map\geometric.pdb" /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "geometric - Win32 NDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometr2"
# PROP BASE Intermediate_Dir "geometr2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "geometr2"
# PROP Intermediate_Dir "geometr2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geometr3"
# PROP BASE Intermediate_Dir "geometr3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "final"
# PROP Intermediate_Dir "final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\gs\gameobj" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\os\include" /D "GEOMETRIC" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - Win32 Leaks"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometr3"
# PROP BASE Intermediate_Dir "geometr3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "geometr3"
# PROP Intermediate_Dir "geometr3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "GEOMETRIC" /D "_PLAYTEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - Win32 Debug Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometric___Win32_Debug_Browse"
# PROP BASE Intermediate_Dir "geometric___Win32_Debug_Browse"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "geometr0"
# PROP Intermediate_Dir "geometr0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "GEOMETRIC" /D "_PLAYTEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - SDL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometric_-_SDL_Debug"
# PROP BASE Intermediate_Dir "geometric_-_SDL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "geometric_-_SDL_Debug"
# PROP Intermediate_Dir "geometric_-_SDL_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /D "USE_SDL" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "geometric - SDL Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geometric_-_SDL_Final"
# PROP BASE Intermediate_Dir "geometric_-_SDL_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "geometric_-_SDL_Final"
# PROP Intermediate_Dir "geometric_-_SDL_Final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "GEOMETRIC" /D "USE_SDL" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /D "GEOMETRIC" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp\dll\map\geometric.dll"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "geometric - Win32 Release"
# Name "geometric - Win32 Debug"
# Name "geometric - Win32 Test"
# Name "geometric - Win32 NDebug"
# Name "geometric - Win32 Final"
# Name "geometric - Win32 Leaks"
# Name "geometric - Win32 Debug Browse"
# Name "geometric - SDL Debug"
# Name "geometric - SDL Final"
# Begin Source File

SOURCE=.\FaultGen.cpp
# End Source File
# Begin Source File

SOURCE=.\FaultGen.h
# End Source File
# Begin Source File

SOURCE=.\Geometric.cpp
# End Source File
# Begin Source File

SOURCE=.\Geometric.def
# End Source File
# Begin Source File

SOURCE=.\Geometric.h
# End Source File
# End Target
# End Project
