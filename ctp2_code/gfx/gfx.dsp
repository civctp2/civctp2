# Microsoft Developer Studio Project File - Name="gfx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gfx - Win32 Test
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gfx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gfx.mak" CFG="gfx - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gfx - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gfx - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gfx - Win32 Test" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "gfx - Win32 Release"

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

!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gfx___Wi"
# PROP BASE Intermediate_Dir "gfx___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "..\libs\freetype\lib\\" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT CPP /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "gfx - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gfx___Wi"
# PROP BASE Intermediate_Dir "gfx___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Test"
# PROP Intermediate_Dir "Test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /I "..\libs\freetype\lib\\" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\robotcom\backdoor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Ob2 /I "..\libs\freetype\lib\\" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\gs\slic" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\robotcom\backdoor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT CPP /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "gfx - Win32 Release"
# Name "gfx - Win32 Debug"
# Name "gfx - Win32 Test"
# Begin Group "spritesys"

# PROP Default_Filter ""
# Begin Group "Actors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\spritesys\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\Action.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\Actor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\Actor.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\ActorPath.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\ActorPath.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\Anim.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\Anim.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\EffectActor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\EffectActor.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\GoodActor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\GoodActor.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\ProjectileActor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\ProjectileActor.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\TradeActor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\TradeActor.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\UnitActor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\UnitActor.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\WorkerActor.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\WorkerActor.h
# End Source File
# End Group
# Begin Group "Sprites"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\spritesys\CitySpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\CitySpriteGroup.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\EffectSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\EffectSpriteGroup.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\FacedSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\FacedSprite.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\FacedSpriteWshadow.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\FacedSpriteWshadow.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\GoodSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\GoodSpriteGroup.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\ProjectileSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\ProjectileSpriteGroup.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\Sprite.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteFile.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteFile.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteGroup.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteGroupList.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteGroupList.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteList.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteList.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteState.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteState.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteStateDB.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\SpriteStateDB.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\spriteutils.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\spriteutils.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\UnitSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\UnitSpriteGroup.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\spritesys\Director.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\Director.h
# End Source File
# Begin Source File

SOURCE=.\spritesys\ScreenManager.cpp
# End Source File
# Begin Source File

SOURCE=.\spritesys\ScreenManager.h
# End Source File
# End Group
# Begin Group "tilesys"

# PROP Default_Filter ""
# Begin Group "maputils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tilesys\maputils.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\maputils.h
# End Source File
# End Group
# Begin Group "radar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tilesys\RadarMap.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\RadarMap.h
# End Source File
# End Group
# Begin Group "workview"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tilesys\WorkMap.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\WorkMap.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\tilesys\BaseTile.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\BaseTile.h
# End Source File
# Begin Source File

SOURCE=.\tilesys\TiledMap.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\TiledMap.h
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileDrawRoad.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileDrawRoad.h
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileHighlight.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileInfo.h
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileSet.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\TileSet.h
# End Source File
# Begin Source File

SOURCE=.\tilesys\tileutils.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesys\tileutils.h
# End Source File
# End Group
# Begin Group "gfx_utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gfx_utils\Arproces.cpp
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\Arproces.h
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\ColorSet.cpp
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\ColorSet.h
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\pixelutils.cpp
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\pixelutils.h
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\tiffutils.cpp
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\tiffutils.h
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\videoutils.cpp
# End Source File
# Begin Source File

SOURCE=.\gfx_utils\videoutils.h
# End Source File
# End Group
# Begin Group "freetype"

# PROP Default_Filter ""
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libs\freetype\lib\freetype.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ft_conf.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ftxgasp.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ftxkern.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ftxkern.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\gdriver.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\header.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttapi.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcache.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcache.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcalc.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcalc.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcmap.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcmap.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttcommon.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttconfig.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttdebug.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttdebug.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttengine.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\tterror.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\tterror.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttextend.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttextend.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttfile.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttfile.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttfile2.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttgasp.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttgload.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttgload.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttinterp.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttinterp.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttlists.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttlists.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttload.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttload.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttmemory.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttmemory.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttmutex.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttmutex.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttobjs.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttobjs.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttraster.c
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\ttraster.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\tttables.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\tttags.h
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\lib\tttypes.h
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libs\freetype\utils\fontutils.cpp
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\utils\fontutils.h
# End Source File
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libs\freetype\win32\view.cpp
# End Source File
# Begin Source File

SOURCE=..\libs\freetype\win32\view.h
# End Source File
# End Group
# End Group
# Begin Group "layers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\layers\citylayer.cpp
# End Source File
# Begin Source File

SOURCE=.\layers\citylayer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\gfx_utils\Queue.h
# End Source File
# End Target
# End Project
