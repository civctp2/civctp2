# Microsoft Developer Studio Project File - Name="net" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=net - Win32 Test
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "net.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "net.mak" CFG="net - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "net - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "net - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "net - Win32 Test" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "net - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\libs\freetype\lib\\" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

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

!ELSEIF  "$(CFG)" == "net - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "net___Wi"
# PROP BASE Intermediate_Dir "net___Wi"
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

# Name "net - Win32 Release"
# Name "net - Win32 Debug"
# Name "net - Win32 Test"
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\io\net_anet.cpp
# End Source File
# Begin Source File

SOURCE=.\io\net_anet.h
# End Source File
# Begin Source File

SOURCE=.\io\net_array.h
# End Source File
# Begin Source File

SOURCE=.\io\net_debug.cpp
# End Source File
# Begin Source File

SOURCE=.\io\net_debug.h
# End Source File
# Begin Source File

SOURCE=.\io\net_error.h
# End Source File
# Begin Source File

SOURCE=.\io\net_io.cpp
# End Source File
# Begin Source File

SOURCE=.\io\net_io.h
# End Source File
# Begin Source File

SOURCE=.\io\net_list.h
# End Source File
# Begin Source File

SOURCE=.\io\net_types.h
# End Source File
# Begin Source File

SOURCE=.\io\net_util.h
# End Source File
# End Group
# Begin Group "general"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\general\net_action.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_action.h
# End Source File
# Begin Source File

SOURCE=.\general\net_agreement.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_agreement.h
# End Source File
# Begin Source File

SOURCE=.\general\net_cell.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_cell.h
# End Source File
# Begin Source File

SOURCE=.\general\net_chat.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_chat.h
# End Source File
# Begin Source File

SOURCE=.\general\net_cheat.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_cheat.h
# End Source File
# Begin Source File

SOURCE=.\general\net_city.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_city.h
# End Source File
# Begin Source File

SOURCE=.\general\net_civ.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_civ.h
# End Source File
# Begin Source File

SOURCE=.\general\net_diff.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_diff.h
# End Source File
# Begin Source File

SOURCE=.\general\net_diplomacy.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_gameobj.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_gameobj.h
# End Source File
# Begin Source File

SOURCE=.\general\net_happy.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_happy.h
# End Source File
# Begin Source File

SOURCE=.\general\net_hash.h
# End Source File
# Begin Source File

SOURCE=.\general\net_info.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_info.h
# End Source File
# Begin Source File

SOURCE=.\general\net_installation.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_installation.h
# End Source File
# Begin Source File

SOURCE=.\general\net_message.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_message.h
# End Source File
# Begin Source File

SOURCE=.\general\net_order.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_order.h
# End Source File
# Begin Source File

SOURCE=.\general\net_packet.h
# End Source File
# Begin Source File

SOURCE=.\general\net_player.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_player.h
# End Source File
# Begin Source File

SOURCE=.\general\net_pollution.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_pollution.h
# End Source File
# Begin Source File

SOURCE=.\general\net_pop.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_pop.h
# End Source File
# Begin Source File

SOURCE=.\general\net_rand.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_rand.h
# End Source File
# Begin Source File

SOURCE=.\general\net_ready.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_ready.h
# End Source File
# Begin Source File

SOURCE=.\general\net_report.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_report.h
# End Source File
# Begin Source File

SOURCE=.\general\net_terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_terrain.h
# End Source File
# Begin Source File

SOURCE=.\general\net_tradeoffer.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_tradeoffer.h
# End Source File
# Begin Source File

SOURCE=.\general\net_traderoute.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_traderoute.h
# End Source File
# Begin Source File

SOURCE=.\general\net_unit.cpp
# End Source File
# Begin Source File

SOURCE=.\general\net_unit.h
# End Source File
# Begin Source File

SOURCE=.\general\network.cpp
# End Source File
# Begin Source File

SOURCE=.\general\network.h
# End Source File
# End Group
# End Target
# End Project
