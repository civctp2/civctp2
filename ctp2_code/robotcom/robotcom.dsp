# Microsoft Developer Studio Project File - Name="robotcom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=robotcom - Win32 Leaks
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "robotcom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "robotcom.mak" CFG="robotcom - Win32 Leaks"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "robotcom - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Test" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 NDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Leaks" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
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
# ADD CPP /nologo /G5 /MD /W3 /GX /Ot /Og /Oi /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AIDLL" /FD /c
# SUBTRACT CPP /Ox /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:none /map:"..\ctp\dll\ai\robotcom.map" /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib" /def:"..\..\ctp2_code\robotcom\InCom\RobotCOM.def" /out:"..\ctp\dll\Ai\robotcom.dll"
# SUBTRACT LINK32 /profile /debug /force

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /I "..\gs\events" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /D "DBGAILOG" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\ctp\dll\ai\DBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\DBGRobotCOM.def" /out:"..\ctp\dll\Ai\DBGrobotcom.dll"
# SUBTRACT LINK32 /profile /pdb:none /map

!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "robotcom"
# PROP BASE Intermediate_Dir "robotcom"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Test"
# PROP Intermediate_Dir "Test"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp2\\" /I "..\ctp2\ctp2_utils\\" /I "..\ctp2\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\ui\freetype\\" /I "..\ctp2\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Ot /Og /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "_AIDLL" /D "_TEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Ox /Ow /Oi
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\ctp2\dll\ai\dbgrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\DBGRobotCOM.def" /out:"..\ctp2\dll\Ai\DBGrobotcom.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib MSVCRT.lib MSVCRTD.lib /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:no /pdb:"..\ctp\dll\ai\Testrobotcom.pdb" /map /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\TestRobotCOM.def" /out:"..\ctp\dll\Ai\TESTrobotcom.dll" /pdbtype:sept
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /D "DBGAILOG" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "_AIDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_NDBG" /U "_DEBUG" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\ctp\dll\ai\DBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\DBGRobotCOM.def" /out:"..\ctp\dll\Ai\DBGrobotcom.dll"
# SUBTRACT BASE LINK32 /profile /pdb:none /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:no /pdb:"..\ctp\dll\ai\NDBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\NDBGRobotCOM.def" /out:"..\ctp\dll\Ai\NDBGrobotcom.dll"
# SUBTRACT LINK32 /profile /pdb:none /map

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
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Ot /Og /Oi /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AIDLL" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Ox
# ADD CPP /nologo /G5 /MD /W3 /GX /Ot /Og /Oi /Ob2 /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AIDLL" /D "_BFR_" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Ox
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:none /map:"..\ctp\dll\ai\robotcom.map" /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib" /def:"..\..\ctp2_code\robotcom\InCom\RobotCOM.def" /out:"..\ctp\dll\Ai\robotcom.dll"
# SUBTRACT BASE LINK32 /profile /debug /force
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:none /map:"..\ctp\dll\ai\robotcom.map" /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib" /def:"..\..\ctp2_code\robotcom\InCom\RobotCOM.def" /out:"..\ctp\dll\Ai\robotcom.dll"
# SUBTRACT LINK32 /profile /debug /force

!ELSEIF  "$(CFG)" == "robotcom - Win32 Leaks"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "robotco0"
# PROP BASE Intermediate_Dir "robotco0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "robotco0"
# PROP Intermediate_Dir "robotco0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /D "DBGAILOG" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\robotcom\Top" /I "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general" /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\ui\freetype\\" /I "..\ctp\debugtools" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\libs\directx\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AIDLL" /D "DBGAILOG" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\ctp\dll\ai\DBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\DBGRobotCOM.def" /out:"..\ctp\dll\Ai\DBGrobotcom.dll"
# SUBTRACT BASE LINK32 /profile /pdb:none /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\ctp\dll\ai\DBGrobotcom.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /def:"..\..\ctp2_code\robotcom\InCom\DBGRobotCOM.def" /out:"..\ctp\dll\Ai\DBGrobotcom.dll"
# SUBTRACT LINK32 /profile /pdb:none /map

!ENDIF 

# Begin Target

# Name "robotcom - Win32 Release"
# Name "robotcom - Win32 Debug"
# Name "robotcom - Win32 Test"
# Name "robotcom - Win32 NDebug"
# Name "robotcom - Win32 Final"
# Name "robotcom - Win32 Leaks"
# Begin Group "AiMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aimgr\AiMain.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\AiMain.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\airndcnt.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\airndcnt.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\AiStats.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\AiStats.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\c3ai.cpp
# ADD CPP /Yc"c3.h"
# End Source File
# Begin Source File

SOURCE=.\aimgr\c3aidebug.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\Foreigner.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\Foreigner.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\FrameClock.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\FrameClock.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\Gold.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\Gold.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\WonderBlg.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\WonderBlg.h
# End Source File
# Begin Source File

SOURCE=.\aimgr\WonderTbl.cpp
# End Source File
# Begin Source File

SOURCE=.\aimgr\WonderTbl.h
# End Source File
# End Group
# Begin Group "InCom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\InCom\IRobot.h
# End Source File
# Begin Source File

SOURCE=.\InCom\RobotCOM.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\InCom\RobotCOM.h
# End Source File
# End Group
# Begin Group "Agent"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Agent\Agent.cpp
# End Source File
# Begin Source File

SOURCE=.\agent\Agent.h
# End Source File
# Begin Source File

SOURCE=.\Agent\ArmyAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\ArmyAgent.h
# End Source File
# Begin Source File

SOURCE=.\Agent\BldAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\BldAgent.h
# End Source File
# Begin Source File

SOURCE=.\Agent\CityAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\CityAgent.h
# End Source File
# Begin Source File

SOURCE=.\Agent\CityAgentBlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\CityAgentBlg.h
# End Source File
# Begin Source File

SOURCE=.\Agent\CityAgentPop.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\CityAgentPop.h
# End Source File
# Begin Source File

SOURCE=.\Agent\ForeignAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\ForeignAgent.h
# End Source File
# Begin Source File

SOURCE=.\Agent\ForeignCity.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\ForeignCity.h
# End Source File
# Begin Source File

SOURCE=.\Agent\Pirate_Marker.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\Pirate_Marker.h
# End Source File
# Begin Source File

SOURCE=.\Agent\ScienceAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\Agent\ScienceAgent.h
# End Source File
# End Group
# Begin Group "backdoor"

# PROP Default_Filter ""
# Begin Group "debugtools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ctp\debugtools\breakpoint.h
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\Debug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debug.h
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debugassert.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debugassert.h
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debugcallstack.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debugexception.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debugexception.h
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\debugmemory.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ctp\debugtools\Log.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\backdoor\Avl.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\Bit_Table.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\BSet.cpp
# End Source File
# Begin Source File

SOURCE=.\backdoor\BSet.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\c3mem.cpp
# End Source File
# Begin Source File

SOURCE=.\backdoor\CivArchive.cpp
# End Source File
# Begin Source File

SOURCE=.\backdoor\CivArchive.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Common.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\DMapPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\backdoor\DMapPoint.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\DynArr.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\FlatPtr.h
# End Source File
# Begin Source File

SOURCE=.\planner\list_array.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\Memory_Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\backdoor\Memory_Manager.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\pointer_pool.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\pool.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\popdata_leak_fix_pool.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\PriorityQueue.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\semi_dynamic_array.h
# End Source File
# Begin Source File

SOURCE=.\backdoor\WallClock.cpp
# End Source File
# Begin Source File

SOURCE=.\backdoor\WallClock.h
# End Source File
# End Group
# Begin Group "FrmState"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FrmState\AIDipReq.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\AIDipReq.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FrameState.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSAlloc.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSAlloc.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSBeginTurn.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSBeginTurn.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSCompete.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSCompete.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSDiplomat.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSDiplomat.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSFuzzy.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSFuzzy.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSGold.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSGold.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSOppAction.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSOppAction.h
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSWar.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmState\FSWar.h
# End Source File
# End Group
# Begin Group "Fuzzy"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fuzzy\fli.l

!IF  "$(CFG)" == "robotcom - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

USERDEP__FLI_L="$(ProjDir)\fuzzy\y.tab.h"	
# Begin Custom Build - drinking coffee...
ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

USERDEP__FLI_L="$(ProjDir)\fuzzy\y.tab.h"	
# Begin Custom Build - drinking coffee...
ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 NDebug"

USERDEP__FLI_L="$(ProjDir)\fuzzy\y.tab.h"	
# Begin Custom Build - drinking coffee...
ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Leaks"

USERDEP__FLI_L="$(ProjDir)\fuzzy\y.tab.h"	
# Begin Custom Build - drinking coffee...
ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fuzzy\fli.y

!IF  "$(CFG)" == "robotcom - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\fuzzy\fli.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

"$(ProjDir)\fuzzy\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\fuzzy\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

# Begin Custom Build - eating donuts...
ProjDir=.
InputPath=.\fuzzy\fli.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

"$(ProjDir)\fuzzy\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\fuzzy\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

# Begin Custom Build - eating donuts...
ProjDir=.
InputPath=.\fuzzy\fli.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

"$(ProjDir)\fuzzy\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\fuzzy\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 NDebug"

# Begin Custom Build - eating donuts...
ProjDir=.
InputPath=.\fuzzy\fli.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

"$(ProjDir)\fuzzy\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\fuzzy\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=.\fuzzy\fli.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

"$(ProjDir)\fuzzy\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\fuzzy\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "robotcom - Win32 Leaks"

# Begin Custom Build - eating donuts...
ProjDir=.
InputPath=.\fuzzy\fli.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

"$(ProjDir)\fuzzy\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\fuzzy\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliAction.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliAction.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliEngine.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliFuncTab.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliFuncTab.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\fliif.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\fliif.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliOutputProc.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliOutputProc.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliRule.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliRule.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliSetFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliSetFunc.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliStack.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliStack.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliSymbol.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliSymTab.cpp
# End Source File
# Begin Source File

SOURCE=.\fuzzy\FliSymTab.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\Fuzzy_Variables.h
# End Source File
# Begin Source File

SOURCE=.\Fuzzy\FzOut.cpp
# End Source File
# Begin Source File

SOURCE=.\Fuzzy\FzOut.h
# End Source File
# Begin Source File

SOURCE=.\fuzzy\lex.yy.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\fuzzy\y.tab.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Goals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Goals\ArmyGoal.cpp
# End Source File
# Begin Source File

SOURCE=.\Goals\ArmyGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goals\GlAttck.cpp
# End Source File
# Begin Source File

SOURCE=.\Goals\GlAttck.h
# End Source File
# Begin Source File

SOURCE=.\goals\GlBlg.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GlBlg.h
# End Source File
# Begin Source File

SOURCE=.\goals\GlEnslave.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GlEnslave.h
# End Source File
# Begin Source File

SOURCE=.\Goals\GlExpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Goals\GlExpl.h
# End Source File
# Begin Source File

SOURCE=.\goals\GlFreight.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GlFreight.h
# End Source File
# Begin Source File

SOURCE=.\goals\GlOvertime.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GlOvertime.h
# End Source File
# Begin Source File

SOURCE=.\goals\GlRoute.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GlRoute.h
# End Source File
# Begin Source File

SOURCE=.\Goals\GlSettl.cpp
# End Source File
# Begin Source File

SOURCE=.\Goals\GlSettl.h
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Attack_Region.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Attack_Region.h
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Build_List.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Build_List.h
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Build_Supplemental.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Build_Supplemental.h
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Chokepoint.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Chokepoint.h
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Patrol.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Patrol.h
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Perimeter.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\Goal_Perimeter.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalBombard.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalBombard.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalCityTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalCityTarget.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalDefense.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalDefense.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalExpel.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalExpel.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalInst.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalInst.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalMapTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalMapTarget.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalRetreat.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalRetreat.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalRoad.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalRoad.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalRustle.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalRustle.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalSally.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalSally.h
# End Source File
# Begin Source File

SOURCE=.\goals\GoalWonder.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\GoalWonder.h
# End Source File
# Begin Source File

SOURCE=.\goals\OA.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\OA.h
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Entrench.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Entrench.h
# End Source File
# Begin Source File

SOURCE=.\goals\oa_loot.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\oa_loot.h
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Paradrop.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Paradrop.h
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Pillage.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Pillage.h
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Refuel.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Refuel.h
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Repair_Installation.cpp
# End Source File
# Begin Source File

SOURCE=.\goals\OA_Repair_Installation.h
# End Source File
# End Group
# Begin Group "Alloc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Alloc\AllocWGF.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\AllocWGF.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityFP.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityFP.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityGrowth.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityGrowth.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityList.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityList.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityProd.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\CityProd.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\InstBidList.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\InstBidList.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\OTUtilityList.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\OTUtilityList.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\Plan.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\Plan.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\RouteUtilityList.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\RouteUtilityList.h
# End Source File
# Begin Source File

SOURCE=.\Alloc\WotP.cpp
# End Source File
# Begin Source File

SOURCE=.\Alloc\WotP.h
# End Source File
# End Group
# Begin Group "Planner"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Planner\AI_Match.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\AI_Match.h
# End Source File
# Begin Source File

SOURCE=.\Planner\AILog.h
# End Source File
# Begin Source File

SOURCE=.\Planner\aip.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\aip.h
# End Source File
# Begin Source File

SOURCE=.\planner\AIP_Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\planner\AIP_Manager.h
# End Source File
# Begin Source File

SOURCE=.\Planner\DR_Debug.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Expression.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\ExpressionP.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Goal.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\Goal.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Grassfire.h
# End Source File
# Begin Source File

SOURCE=.\Planner\IParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\IParser.h
# End Source File
# Begin Source File

SOURCE=.\Planner\linked_list.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\linked_list.h
# End Source File
# Begin Source File

SOURCE=.\Planner\matrix.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Planner.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\Planner.h
# End Source File
# Begin Source File

SOURCE=.\Planner\scheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\Scheduler.h
# End Source File
# Begin Source File

SOURCE=.\Planner\sorted_array.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Squad.cpp
# End Source File
# Begin Source File

SOURCE=.\Planner\Squad.h
# End Source File
# Begin Source File

SOURCE=.\planner\Squad_And_Goal_Types.h
# End Source File
# Begin Source File

SOURCE=.\planner\Squad_Strength.cpp
# End Source File
# Begin Source File

SOURCE=.\planner\Squad_Strength.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Strategic_AI_Forward.h
# End Source File
# Begin Source File

SOURCE=.\Planner\Unitflow.h
# End Source File
# End Group
# Begin Group "Top"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Top\AiCell.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\AiCell.h
# End Source File
# Begin Source File

SOURCE=.\Top\AiMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\AiMap.h
# End Source File
# Begin Source File

SOURCE=.\Top\CityEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\CityEdge.h
# End Source File
# Begin Source File

SOURCE=.\Top\CityTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\CityTree.h
# End Source File
# Begin Source File

SOURCE=.\Top\CityTreeQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\CityTreeQueue.h
# End Source File
# Begin Source File

SOURCE=.\Top\CityVertex.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\CityVertex.h
# End Source File
# Begin Source File

SOURCE=.\Top\cont.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\cont.h
# End Source File
# Begin Source File

SOURCE=.\Top\delaunay.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\delaunay.h
# End Source File
# Begin Source File

SOURCE=.\Top\geom2d.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\geom2d.h
# End Source File
# Begin Source File

SOURCE=.\Top\MST.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\MST.h
# End Source File
# Begin Source File

SOURCE=.\Top\Strategic_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\Top\Strategic_Map.h
# End Source File
# End Group
# End Target
# End Project
