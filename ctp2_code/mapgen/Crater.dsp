# Microsoft Developer Studio Project File - Name="Crater" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Crater - Win32 Debug Browse
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Crater.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Crater.mak" CFG="Crater - Win32 Debug Browse"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Crater - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - Win32 Test" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - Win32 NDebug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - Win32 Final" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - Win32 Leaks" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - Win32 Debug Browse" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - SDL Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Crater - SDL Final" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ctp", BAAAAAAA"
# PROP Scc_LocalPath "..\ctp"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Crater - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Crater__"
# PROP BASE Intermediate_Dir "Crater__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Crater__"
# PROP Intermediate_Dir "Crater__"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zd /Og /Oi /Os /Op /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Crater_0"
# PROP BASE Intermediate_Dir "Crater_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Crater_0"
# PROP Intermediate_Dir "Crater_0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Crater_1"
# PROP BASE Intermediate_Dir "Crater_1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Crater_1"
# PROP Intermediate_Dir "Crater_1"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:"..\ctp\dll\map\Crater.pdb" /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:"..\ctp\dll\map\Crater.pdb" /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Crater - Win32 NDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Crater_2"
# PROP BASE Intermediate_Dir "Crater_2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Crater_2"
# PROP Intermediate_Dir "Crater_2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "final"
# PROP BASE Intermediate_Dir "final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "final"
# PROP Intermediate_Dir "final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\gs\gameobj" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - Win32 Leaks"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Crater_3"
# PROP BASE Intermediate_Dir "Crater_3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Crater_3"
# PROP Intermediate_Dir "Crater_3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "_PLAYTEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - Win32 Debug Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Crater___Win32_Debug_Browse"
# PROP BASE Intermediate_Dir "Crater___Win32_Debug_Browse"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Crater_0"
# PROP Intermediate_Dir "Crater_0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "_PLAYTEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - SDL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Crater_-_SDL_Debug"
# PROP BASE Intermediate_Dir "Crater_-_SDL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Crater_-_SDL_Debug"
# PROP Intermediate_Dir "Crater_-_SDL_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "Crater - SDL Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Crater_-_SDL_Final"
# PROP BASE Intermediate_Dir "Crater_-_SDL_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Crater_-_SDL_Final"
# PROP Intermediate_Dir "Crater_-_SDL_Final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /I "..\gs\gameobj" /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils" /I "..\ui\aui_directx" /I "..\ui\freetype" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\ctp\dll\map\Crater.dll"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "Crater - Win32 Release"
# Name "Crater - Win32 Debug"
# Name "Crater - Win32 Test"
# Name "Crater - Win32 NDebug"
# Name "Crater - Win32 Final"
# Name "Crater - Win32 Leaks"
# Name "Crater - Win32 Debug Browse"
# Name "Crater - SDL Debug"
# Name "Crater - SDL Final"
# Begin Source File

SOURCE=.\Crater.cpp
# End Source File
# Begin Source File

SOURCE=.\Crater.def
# End Source File
# Begin Source File

SOURCE=.\Crater.h
# End Source File
# End Target
# End Project
