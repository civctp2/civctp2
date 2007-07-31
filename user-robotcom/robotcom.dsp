# Microsoft Developer Studio Project File - Name="robotcom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=robotcom - Win32 NDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "robotcom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "robotcom.mak" CFG="robotcom - Win32 NDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "robotcom - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 NDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "robotcom - Win32 Release"

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
# ADD CPP /nologo /G5 /MD /W3 /GX /Ot /Og /Oi /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AIDLL" /FD /c
# SUBTRACT CPP /Ox /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:none /map:"..\ctp\dll\ai\robotcom.map" /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib" /def:"InCom\RobotCOM.def" /out:"..\ctp\dll\Ai\robotcom.dll"

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "robotco0"
# PROP BASE Intermediate_Dir "robotco0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /I ".\shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /D "DBGAILOG" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"e:\ctp-user\civ3_code\ctp\dll\Ai\DBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"InCom\DBGRobotCOM.def" /out:"c:\ctp2\ctp2_code\ctp\dll\Ai\DBGrobotcom.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "robotcom - Win32 NDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "robotcom"
# PROP BASE Intermediate_Dir "robotcom"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "robotcom"
# PROP Intermediate_Dir "robotcom"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /D "DBGAILOG" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_AIDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_NDBG" /U "_DEBUG" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\ctp\dll\ai\DBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\civ3_code\robotcom\InCom\DBGRobotCOM.def" /out:"..\ctp\dll\Ai\DBGrobotcom.dll"
# SUBTRACT BASE LINK32 /profile /pdb:none /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:no /pdb:"..\ctp\dll\ai\NDBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"InCom\NDBGRobotCOM.def" /out:"..\ctp\dll\Ai\NDBGrobotcom.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "robotcom - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "robotco0"
# PROP BASE Intermediate_Dir "robotco0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "final"
# PROP Intermediate_Dir "final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Ot /Og /Oi /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AIDLL" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Ox
# ADD CPP /nologo /G5 /MD /W3 /GX /Ot /Og /Oi /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AIDLL" /D "_BFR_" /FD /c
# SUBTRACT CPP /Ox /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:none /map:"..\ctp\dll\ai\robotcom.map" /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib" /def:"..\..\civ3_code\robotcom\InCom\RobotCOM.def" /out:"..\ctp\dll\Ai\robotcom.dll"
# SUBTRACT BASE LINK32 /profile /debug /force
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:none /map:"..\ctp\dll\ai\robotcom.map" /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib" /def:"InCom\RobotCOM.def" /out:"..\ctp\dll\Ai\robotcom.dll"

!ENDIF 

# Begin Target

# Name "robotcom - Win32 Release"
# Name "robotcom - Win32 Debug"
# Name "robotcom - Win32 NDebug"
# Name "robotcom - Win32 Final"
# Begin Group "outcom"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=.\outcom\IC3BlgDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\Ic3CivArchive.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3DipReq.h
# End Source File
# Begin Source File

SOURCE=.\outcom\ic3endgamedb.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3ErrorReport.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3GameState.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Government.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3InstDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\ic3player.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Population.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Rand.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3RobotAstar.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Science.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3String.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3TerrDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Trade.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3UnitDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Wonder.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3World.h
# End Source File
# End Group
# Begin Group "aimain"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=.\aimain\AiMain.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

# ADD CPP /I "shared" /I "outcom" /I "aimain"
# SUBTRACT CPP /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include"

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

# ADD CPP /I "..\user-robotcom\outcom" /I "..\user-robotcom\incom" /I "..\user-robotcom\aimain" /I "..\user-robotcom\InCom" /I "..\user-robotcom\shared"
# SUBTRACT CPP /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /I ".\shared"

!ELSEIF  "$(CFG)" == "robotcom - Win32 NDebug"

# ADD CPP /I "shared" /I "outcom" /I "aimain"

!ELSEIF  "$(CFG)" == "robotcom - Win32 Final"

# ADD CPP /I "shared" /I "outcom" /I "aimain"
# SUBTRACT CPP /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aimain\AiMain.h
# End Source File
# End Group
# Begin Group "shared"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=.\shared\AICause.h
# End Source File
# Begin Source File

SOURCE=".\shared\ctp-types.h"
# End Source File
# Begin Source File

SOURCE=.\shared\ctp.h
# End Source File
# Begin Source File

SOURCE=.\shared\DiplomaticTypes.h
# End Source File
# Begin Source File

SOURCE=.\shared\Globals.h
# End Source File
# Begin Source File

SOURCE=.\shared\ImprovementTypes.h
# End Source File
# Begin Source File

SOURCE=.\shared\MapPointData.h
# End Source File
# Begin Source File

SOURCE=.\shared\Order.h
# End Source File
# End Group
# Begin Group "incom"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=.\incom\IRobot.h
# End Source File
# Begin Source File

SOURCE=.\incom\robotcom.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

# ADD CPP /I "shared" /I "outcom" /I "aimain"
# SUBTRACT CPP /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include"

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

# ADD CPP /I "..\user-robotcom\outcom" /I "..\user-robotcom\incom" /I "..\user-robotcom\aimain" /I "..\user-robotcom\InCom" /I "..\user-robotcom\shared"
# SUBTRACT CPP /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /I ".\shared"

!ELSEIF  "$(CFG)" == "robotcom - Win32 NDebug"

# ADD CPP /I "shared" /I "outcom" /I "aimain"
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "robotcom - Win32 Final"

# ADD CPP /I "shared" /I "outcom" /I "aimain"
# SUBTRACT CPP /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\civ3_utils\\" /I "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\incom\RobotCOM.h
# End Source File
# End Group
# End Target
# End Project
