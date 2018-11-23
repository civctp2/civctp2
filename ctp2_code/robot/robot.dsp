# Microsoft Developer Studio Project File - Name="robot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=robot - Win32 Test
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "robot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "robot.mak" CFG="robot - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "robot - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "robot - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "robot - Win32 Test" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "robot - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\libs\freetype\lib\\" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\mapgen" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "robot - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "..\robotcom\backdoor" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /Fr /YX"c3.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "robot - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "robot___"
# PROP BASE Intermediate_Dir "robot___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Test"
# PROP Intermediate_Dir "Test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /I "..\robotcom\backdoor" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Ob2 /I "..\robotcom\backdoor" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT CPP /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "robot - Win32 Release"
# Name "robot - Win32 Debug"
# Name "robot - Win32 Test"
# Begin Group "utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\utility\RandTest.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\RoboInit.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\RoboInit.h
# End Source File
# End Group
# Begin Group "pathing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pathing\Astar.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\Astar.h
# End Source File
# Begin Source File

SOURCE=.\pathing\AstarPnt.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\AstarPnt.h
# End Source File
# Begin Source File

SOURCE=.\pathing\Avl.h
# End Source File
# Begin Source File

SOURCE=.\pathing\AVLHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\AVLHeap.h
# End Source File
# Begin Source File

SOURCE=.\pathing\CityAstar.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\CityAstar.h
# End Source File
# Begin Source File

SOURCE=.\pathing\Path.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\Path.h
# End Source File
# Begin Source File

SOURCE=.\pathing\RobotAstar.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\RobotAstar.h
# End Source File
# Begin Source File

SOURCE=.\pathing\TradeAstar.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\TradeAstar.h
# End Source File
# Begin Source File

SOURCE=.\pathing\UnitAstar.cpp
# End Source File
# Begin Source File

SOURCE=.\pathing\UnitAstar.h
# End Source File
# End Group
# Begin Group "aibackdoor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\robotcom\backdoor\BSet.cpp
# End Source File
# Begin Source File

SOURCE=..\robotcom\backdoor\BSet.h
# End Source File
# Begin Source File

SOURCE=..\robotcom\backdoor\c3mem.cpp
# End Source File
# Begin Source File

SOURCE=..\robotcom\backdoor\CivArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\robotcom\backdoor\DynArr.h
# End Source File
# End Group
# End Target
# End Project
