# Microsoft Developer Studio Project File - Name="gs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gs - Win32 Test
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gs.mak" CFG="gs - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gs - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gs - Win32 Test" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "gs - Win32 Release"

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
# Begin Custom Build
InputPath=.\Release\gs.lib
SOURCE=$(InputPath)

"slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex slic\slic.l

# End Custom Build

!ELSEIF  "$(CFG)" == "gs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gs___Win"
# PROP BASE Intermediate_Dir "gs___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "..\net\general" /I "..\net\io" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\ui\ldl" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\mapgen" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /Fr /YX"c3.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "gs - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gs___Win"
# PROP BASE Intermediate_Dir "gs___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Test"
# PROP Intermediate_Dir "Test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /I "..\net\general" /I "..\net\io" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\ui\ldl" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\mapgen" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
<<<<<<< gs.dsp
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Ob2 /I "..\net\general" /I "..\net\io" /I "..\gs\slic" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\mm\\" /I "..\mapgen" /I "..\robotcom\backdoor" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT CPP /Fr
=======
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Ob2 /I "..\net\general" /I "..\net\io" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\ui\ldl" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I "..\gfx\layers" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /I "..\mapgen" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT CPP /Fr
>>>>>>> 1.52
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "gs - Win32 Release"
# Name "gs - Win32 Debug"
# Name "gs - Win32 Test"
# Begin Group "database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\database\AdvDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\AdvDB.h
# End Source File
# Begin Source File

SOURCE=.\database\AdvRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\AdvRec.h
# End Source File
# Begin Source File

SOURCE=.\database\CivilisationDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\CivilisationDB.h
# End Source File
# Begin Source File

SOURCE=.\database\CivilisationRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\CivilisationRec.h
# End Source File
# Begin Source File

SOURCE=.\database\ConstDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\ConstDB.h
# End Source File
# Begin Source File

SOURCE=.\database\DB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\DB.h
# End Source File
# Begin Source File

SOURCE=.\database\DiffDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\DiffDB.h
# End Source File
# Begin Source File

SOURCE=.\database\GovRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\GovRec.h
# End Source File
# Begin Source File

SOURCE=.\database\GWDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\GWDB.h
# End Source File
# Begin Source File

SOURCE=.\database\GWRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\database\GWRecord.h
# End Source File
# Begin Source File

SOURCE=.\database\IconDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\IconDB.h
# End Source File
# Begin Source File

SOURCE=.\database\ImpDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\ImpDB.h
# End Source File
# Begin Source File

SOURCE=.\database\ImprRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\ImprRec.h
# End Source File
# Begin Source File

SOURCE=.\database\InstDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\InstDB.h
# End Source File
# Begin Source File

SOURCE=.\database\InstRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\InstRec.h
# End Source File
# Begin Source File

SOURCE=.\database\Poprec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\PopRec.h
# End Source File
# Begin Source File

SOURCE=.\database\ProfileDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\ProfileDB.h
# End Source File
# Begin Source File

SOURCE=.\database\Rec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\Rec.h
# End Source File
# Begin Source File

SOURCE=.\database\StrDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\StrDB.h
# End Source File
# Begin Source File

SOURCE=.\database\StrRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\StrRec.h
# End Source File
# Begin Source File

SOURCE=.\database\TerrainDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\TerrainDB.h
# End Source File
# Begin Source File

SOURCE=.\database\TerrRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\TerrRec.h
# End Source File
# Begin Source File

SOURCE=.\database\UnitDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\UnitDB.h
# End Source File
# Begin Source File

SOURCE=.\database\UnitRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\UnitRec.h
# End Source File
# Begin Source File

SOURCE=.\database\UVDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\UVDB.h
# End Source File
# Begin Source File

SOURCE=.\database\UVRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\database\UVRecord.h
# End Source File
# Begin Source File

SOURCE=.\database\WndrRec.cpp
# End Source File
# Begin Source File

SOURCE=.\database\WndrRec.h
# End Source File
# Begin Source File

SOURCE=.\database\WonderDB.cpp
# End Source File
# Begin Source File

SOURCE=.\database\WonderDB.h
# End Source File
# End Group
# Begin Group "gameobj"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gameobj\Advances.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Advances.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Agreement.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Agreement.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\AgreementData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\AgreementData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\AgreementPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\AgreementPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\ArmyList.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\ArmyList.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\BldQue.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\BldQue.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\CityData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\CityData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\CityRadius.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\CityRadius.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Civilisation.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Civilisation.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\CivilisationData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\CivilisationData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\CivilisationPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\CivilisationPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Diffcly.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Diffcly.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\DiplomaticRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\DiplomaticRequest.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\DiplomaticRequestData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\DiplomaticRequestData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\DiplomaticRequestPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\DiplomaticRequestPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\GameObj.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\GameObj.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Gold.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Gold.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\GoodyHuts.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\GoodyHuts.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Happy.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Happy.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\ID.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\ID.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\installation.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\installation.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\installationdata.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\installationdata.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\installationpool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\installationpool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\installationtree.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\MaterialPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\MaterialPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Message.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\MessageData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\MessageData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\MessagePool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\MessagePool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\ObjPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\ObjPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Player.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\PlayerAI.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\PlayHap.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\PlayHap.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Pollution.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Pollution.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\PollutionConst.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Pop.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Pop.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\PopData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\PopData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\PopPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\PopPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Readiness.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Readiness.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Regard.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Regard.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Resources.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Sci.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Sci.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\SlicList.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\SlicList.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Strengths.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Strengths.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TaxRate.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TaxRate.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TerrImprove.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TerrImprove.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TerrImproveData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TerrImproveData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TerrImprovePool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TerrImprovePool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TileVal.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TopTen.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TopTen.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeOffer.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeOffer.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeOfferData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeOfferData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeOfferPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeOfferPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradePool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradePool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeRoute.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeRoute.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeRouteData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\TradeRouteData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Unit.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Unit.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\UnitData.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\UnitData.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\UnitOrdQ.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\UnitOrdQ.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\UnitPool.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\UnitPool.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\Vision.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Vision.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\ZBattle.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\Zbattle.h
# End Source File
# Begin Source File

SOURCE=.\gameobj\ZBS.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobj\ZBS.h
# End Source File
# End Group
# Begin Group "utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\utility\AgreementDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\AgreementDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\checksum.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\Checksum.h
# End Source File
# Begin Source File

SOURCE=.\utility\CivDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\CivDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\clock.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\clock.h
# End Source File
# Begin Source File

SOURCE=.\utility\DataCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\DataCheck.h
# End Source File
# Begin Source File

SOURCE=.\utility\DiplDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\DiplDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\ErrMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\ErrMsg.h
# End Source File
# Begin Source File

SOURCE=.\utility\gameinit.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\gameinit.h
# End Source File
# Begin Source File

SOURCE=.\utility\Globals.h
# End Source File
# Begin Source File

SOURCE=.\utility\MsgDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\MsgDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\PopDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\PopDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\QuadTree.h
# End Source File
# Begin Source File

SOURCE=.\utility\RandGen.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\RandGen.h
# End Source File
# Begin Source File

SOURCE=.\utility\SimpleDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\TradeDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\TradeDynArr.h
# End Source File
# Begin Source File

SOURCE=.\utility\TurnCnt.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\TurnCnt.h
# End Source File
# Begin Source File

SOURCE=.\utility\UnitDynArr.cpp
# End Source File
# Begin Source File

SOURCE=.\utility\UnitDynArr.h
# End Source File
# End Group
# Begin Group "world"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\world\Cell.cpp
# End Source File
# Begin Source File

SOURCE=.\world\Cell.h
# End Source File
# Begin Source File

SOURCE=.\world\IMapGen.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IMapPointData.h
# End Source File
# Begin Source File

SOURCE=.\world\MapPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\world\MapPoint.h
# End Source File
# Begin Source File

SOURCE=.\world\UnseenCell.cpp
# End Source File
# Begin Source File

SOURCE=.\world\UnseenCell.h
# End Source File
# Begin Source File

SOURCE=.\world\UnseenCellQuadTree.h
# End Source File
# Begin Source File

SOURCE=.\world\WldGen.cpp
# End Source File
# Begin Source File

SOURCE=.\world\World.h
# End Source File
# Begin Source File

SOURCE=.\world\WrldCity.cpp
# End Source File
# Begin Source File

SOURCE=.\world\WrldPoll.cpp
# End Source File
# Begin Source File

SOURCE=.\world\WrlEnv.cpp
# End Source File
# Begin Source File

SOURCE=.\world\WrlImprove.cpp
# End Source File
# Begin Source File

SOURCE=.\world\WrlUnit.cpp
# End Source File
# End Group
# Begin Group "fileio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fileio\CivPaths.cpp
# End Source File
# Begin Source File

SOURCE=.\fileio\CivPaths.h
# End Source File
# Begin Source File

SOURCE=.\fileio\GameFile.cpp
# End Source File
# Begin Source File

SOURCE=.\fileio\GameFile.h
# End Source File
# Begin Source File

SOURCE=.\fileio\ParseAss.cpp
# End Source File
# Begin Source File

SOURCE=.\fileio\ParseAss.h
# End Source File
# Begin Source File

SOURCE=.\fileio\Token.cpp
# End Source File
# Begin Source File

SOURCE=.\fileio\Token.h
# End Source File
# End Group
# Begin Group "OutCom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\outcom\AICause.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3BlgDB.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3BlgDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3ErrorReport.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3ErrorReport.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3GameState.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3GameState.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Government.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Government.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3InstDB.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3InstDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Player.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Player.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Population.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Population.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Rand.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Rand.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Robot.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Robot.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Science.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Science.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3String.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3String.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3TerrDB.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3TerrDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3UnitDB.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3UnitDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Wonder.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3Wonder.h
# End Source File
# Begin Source File

SOURCE=.\outcom\C3World.cpp
# End Source File
# Begin Source File

SOURCE=.\outcom\C3World.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3BlgDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3CivArchive.h
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

SOURCE=.\outcom\IC3Player.h
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

SOURCE=.\outcom\IC3UnitDB.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3Wonder.h
# End Source File
# Begin Source File

SOURCE=.\outcom\IC3World.h
# End Source File
# End Group
# Begin Group "slic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\slic\lex.yy.c
# End Source File
# Begin Source File

SOURCE=.\slic\slic.l
USERDEP__SLIC_="$(ProjDir)\slic\y.tab.h"	

!IF  "$(CFG)" == "gs - Win32 Release"

!ELSEIF  "$(CFG)" == "gs - Win32 Debug"

# Begin Custom Build - flexing
ProjDir=.
InputPath=.\slic\slic.l

"$(ProjDir)\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "gs - Win32 Test"

# Begin Custom Build - flexing
ProjDir=.
InputPath=.\slic\slic.l

"$(ProjDir)\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\slic\lex.yy.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\slic\slic.y

!IF  "$(CFG)" == "gs - Win32 Release"

!ELSEIF  "$(CFG)" == "gs - Win32 Debug"

# Begin Custom Build - yaccing
ProjDir=.
InputPath=.\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\slic\\y $(ProjDir)\\slic\\slic.y

"$(ProjDir)\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "gs - Win32 Test"

# Begin Custom Build - yaccing
ProjDir=.
InputPath=.\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b$(ProjDir)\\slic\\y $(ProjDir)\\slic\\slic.y

"$(ProjDir)\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\slic\SlicButton.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicButton.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicContext.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicContext.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicEngine.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicError.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicEyePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicEyePoint.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicFrame.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicFunc.h
# End Source File
# Begin Source File

SOURCE=.\slic\slicif.c
# End Source File
# Begin Source File

SOURCE=.\slic\slicif.h
# End Source File
# Begin Source File

SOURCE=.\slic\sliciffile.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicObject.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicObject.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicSegment.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicStack.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicStack.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicSymbol.h
# End Source File
# Begin Source File

SOURCE=.\slic\SlicSymTab.cpp
# End Source File
# Begin Source File

SOURCE=.\slic\SlicSymTab.h
# End Source File
# Begin Source File

SOURCE=.\slic\StringHash.h
# End Source File
# Begin Source File

SOURCE=.\slic\y.tab.c
# End Source File
# End Group
# End Target
# End Project
