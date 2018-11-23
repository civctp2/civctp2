# Microsoft Developer Studio Generated NMAKE File, Based on robotcom.dsp
!IF "$(CFG)" == ""
CFG=robotcom - Win32 Test
!MESSAGE No configuration specified. Defaulting to robotcom - Win32 Test.
!ENDIF 

!IF "$(CFG)" != "robotcom - Win32 Release" && "$(CFG)" !=\
 "robotcom - Win32 Debug" && "$(CFG)" != "robotcom - Win32 Test"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "robotcom.mak" CFG="robotcom - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "robotcom - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "robotcom - Win32 Test" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "robotcom - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\civ3\dll\Ai\robotcom.dll"

!ELSE 

ALL : "..\civ3\dll\Ai\robotcom.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Agent.obj"
	-@erase "$(INTDIR)\AI_Match.obj"
	-@erase "$(INTDIR)\AiBldQue.obj"
	-@erase "$(INTDIR)\AiCell.obj"
	-@erase "$(INTDIR)\AIDipReq.obj"
	-@erase "$(INTDIR)\AiMain.obj"
	-@erase "$(INTDIR)\AiMap.obj"
	-@erase "$(INTDIR)\aip.obj"
	-@erase "$(INTDIR)\airndcnt.obj"
	-@erase "$(INTDIR)\AiStats.obj"
	-@erase "$(INTDIR)\AllocWGF.obj"
	-@erase "$(INTDIR)\ArmyAgent.obj"
	-@erase "$(INTDIR)\ArmyGoal.obj"
	-@erase "$(INTDIR)\BldAgent.obj"
	-@erase "$(INTDIR)\BSet.obj"
	-@erase "$(INTDIR)\c3ai.obj"
	-@erase "$(INTDIR)\c3aidebug.obj"
	-@erase "$(INTDIR)\c3mem.obj"
	-@erase "$(INTDIR)\CityAgent.obj"
	-@erase "$(INTDIR)\CityAgentBlg.obj"
	-@erase "$(INTDIR)\CityAgentPop.obj"
	-@erase "$(INTDIR)\CityEdge.obj"
	-@erase "$(INTDIR)\CityFP.obj"
	-@erase "$(INTDIR)\CityGrowth.obj"
	-@erase "$(INTDIR)\CityList.obj"
	-@erase "$(INTDIR)\CityProd.obj"
	-@erase "$(INTDIR)\CityTree.obj"
	-@erase "$(INTDIR)\CityTreeQueue.obj"
	-@erase "$(INTDIR)\CityVertex.obj"
	-@erase "$(INTDIR)\CivArchive.obj"
	-@erase "$(INTDIR)\cont.obj"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\debugassert.obj"
	-@erase "$(INTDIR)\debugcallstack.obj"
	-@erase "$(INTDIR)\debugexception.obj"
	-@erase "$(INTDIR)\debugmemory.obj"
	-@erase "$(INTDIR)\delaunay.obj"
	-@erase "$(INTDIR)\DMapPoint.obj"
	-@erase "$(INTDIR)\Expression.obj"
	-@erase "$(INTDIR)\FliAction.obj"
	-@erase "$(INTDIR)\FliEngine.obj"
	-@erase "$(INTDIR)\FliFuncTab.obj"
	-@erase "$(INTDIR)\fliif.obj"
	-@erase "$(INTDIR)\FliOutputProc.obj"
	-@erase "$(INTDIR)\FliRule.obj"
	-@erase "$(INTDIR)\FliSetFunc.obj"
	-@erase "$(INTDIR)\FliStack.obj"
	-@erase "$(INTDIR)\FliSymbol.obj"
	-@erase "$(INTDIR)\FliSymTab.obj"
	-@erase "$(INTDIR)\ForeignAgent.obj"
	-@erase "$(INTDIR)\ForeignCity.obj"
	-@erase "$(INTDIR)\Foreigner.obj"
	-@erase "$(INTDIR)\FrameClock.obj"
	-@erase "$(INTDIR)\FSAlloc.obj"
	-@erase "$(INTDIR)\FSBeginTurn.obj"
	-@erase "$(INTDIR)\FSCompete.obj"
	-@erase "$(INTDIR)\FSDiplomat.obj"
	-@erase "$(INTDIR)\FSFuzzy.obj"
	-@erase "$(INTDIR)\FSGold.obj"
	-@erase "$(INTDIR)\FSOppAction.obj"
	-@erase "$(INTDIR)\FSWar.obj"
	-@erase "$(INTDIR)\FzOut.obj"
	-@erase "$(INTDIR)\GlAttck.obj"
	-@erase "$(INTDIR)\GlBlg.obj"
	-@erase "$(INTDIR)\GlEnslave.obj"
	-@erase "$(INTDIR)\GlExpl.obj"
	-@erase "$(INTDIR)\GlFreight.obj"
	-@erase "$(INTDIR)\GlOvertime.obj"
	-@erase "$(INTDIR)\GlRoute.obj"
	-@erase "$(INTDIR)\GlSettl.obj"
	-@erase "$(INTDIR)\GlTrans.obj"
	-@erase "$(INTDIR)\Goal.obj"
	-@erase "$(INTDIR)\Goal_Attack_Region.obj"
	-@erase "$(INTDIR)\Goal_Build_List.obj"
	-@erase "$(INTDIR)\Goal_Build_Supplemental.obj"
	-@erase "$(INTDIR)\Goal_Chokepoint.obj"
	-@erase "$(INTDIR)\Goal_Patrol.obj"
	-@erase "$(INTDIR)\Goal_Perimeter.obj"
	-@erase "$(INTDIR)\GoalBombard.obj"
	-@erase "$(INTDIR)\GoalCityTarget.obj"
	-@erase "$(INTDIR)\GoalDefense.obj"
	-@erase "$(INTDIR)\GoalExpel.obj"
	-@erase "$(INTDIR)\GoalInst.obj"
	-@erase "$(INTDIR)\GoalMapTarget.obj"
	-@erase "$(INTDIR)\GoalRetreat.obj"
	-@erase "$(INTDIR)\GoalRoad.obj"
	-@erase "$(INTDIR)\GoalRustle.obj"
	-@erase "$(INTDIR)\GoalWonder.obj"
	-@erase "$(INTDIR)\Gold.obj"
	-@erase "$(INTDIR)\GoodAgent.obj"
	-@erase "$(INTDIR)\InstBidList.obj"
	-@erase "$(INTDIR)\IParser.obj"
	-@erase "$(INTDIR)\lex.yy.obj"
	-@erase "$(INTDIR)\linked_list.obj"
	-@erase "$(INTDIR)\Log.obj"
	-@erase "$(INTDIR)\MST.obj"
	-@erase "$(INTDIR)\OA.obj"
	-@erase "$(INTDIR)\OA_Entrench.obj"
	-@erase "$(INTDIR)\oa_loot.obj"
	-@erase "$(INTDIR)\OA_Refuel.obj"
	-@erase "$(INTDIR)\OTUtilityList.obj"
	-@erase "$(INTDIR)\Plan.obj"
	-@erase "$(INTDIR)\Planner.obj"
	-@erase "$(INTDIR)\RobotCOM.obj"
	-@erase "$(INTDIR)\robotcom.pch"
	-@erase "$(INTDIR)\RouteUtilityList.obj"
	-@erase "$(INTDIR)\scheduler.obj"
	-@erase "$(INTDIR)\ScienceAgent.obj"
	-@erase "$(INTDIR)\Squad.obj"
	-@erase "$(INTDIR)\Squad_Strength.obj"
	-@erase "$(INTDIR)\Strategic_Map.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\WallClock.obj"
	-@erase "$(INTDIR)\WonderBlg.obj"
	-@erase "$(INTDIR)\WonderTbl.obj"
	-@erase "$(INTDIR)\WotP.obj"
	-@erase "$(INTDIR)\y.tab.obj"
	-@erase "$(OUTDIR)\robotcom.exp"
	-@erase "$(OUTDIR)\robotcom.lib"
	-@erase "..\civ3\dll\Ai\robotcom.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\robotcom.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib imagehlp.lib winmm.lib /nologo /base:"0x20000000"\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\robotcom.pdb"\
 /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcd.lib"\
 /def:"..\..\civ3_code\robotcom\InCom\RobotCOM.def"\
 /out:"..\civ3\dll\Ai\robotcom.dll" /implib:"$(OUTDIR)\robotcom.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Agent.obj" \
	"$(INTDIR)\AI_Match.obj" \
	"$(INTDIR)\AiBldQue.obj" \
	"$(INTDIR)\AiCell.obj" \
	"$(INTDIR)\AIDipReq.obj" \
	"$(INTDIR)\AiMain.obj" \
	"$(INTDIR)\AiMap.obj" \
	"$(INTDIR)\aip.obj" \
	"$(INTDIR)\airndcnt.obj" \
	"$(INTDIR)\AiStats.obj" \
	"$(INTDIR)\AllocWGF.obj" \
	"$(INTDIR)\ArmyAgent.obj" \
	"$(INTDIR)\ArmyGoal.obj" \
	"$(INTDIR)\BldAgent.obj" \
	"$(INTDIR)\BSet.obj" \
	"$(INTDIR)\c3ai.obj" \
	"$(INTDIR)\c3aidebug.obj" \
	"$(INTDIR)\c3mem.obj" \
	"$(INTDIR)\CityAgent.obj" \
	"$(INTDIR)\CityAgentBlg.obj" \
	"$(INTDIR)\CityAgentPop.obj" \
	"$(INTDIR)\CityEdge.obj" \
	"$(INTDIR)\CityFP.obj" \
	"$(INTDIR)\CityGrowth.obj" \
	"$(INTDIR)\CityList.obj" \
	"$(INTDIR)\CityProd.obj" \
	"$(INTDIR)\CityTree.obj" \
	"$(INTDIR)\CityTreeQueue.obj" \
	"$(INTDIR)\CityVertex.obj" \
	"$(INTDIR)\CivArchive.obj" \
	"$(INTDIR)\cont.obj" \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\debugassert.obj" \
	"$(INTDIR)\debugcallstack.obj" \
	"$(INTDIR)\debugexception.obj" \
	"$(INTDIR)\debugmemory.obj" \
	"$(INTDIR)\delaunay.obj" \
	"$(INTDIR)\DMapPoint.obj" \
	"$(INTDIR)\Expression.obj" \
	"$(INTDIR)\FliAction.obj" \
	"$(INTDIR)\FliEngine.obj" \
	"$(INTDIR)\FliFuncTab.obj" \
	"$(INTDIR)\fliif.obj" \
	"$(INTDIR)\FliOutputProc.obj" \
	"$(INTDIR)\FliRule.obj" \
	"$(INTDIR)\FliSetFunc.obj" \
	"$(INTDIR)\FliStack.obj" \
	"$(INTDIR)\FliSymbol.obj" \
	"$(INTDIR)\FliSymTab.obj" \
	"$(INTDIR)\ForeignAgent.obj" \
	"$(INTDIR)\ForeignCity.obj" \
	"$(INTDIR)\Foreigner.obj" \
	"$(INTDIR)\FrameClock.obj" \
	"$(INTDIR)\FSAlloc.obj" \
	"$(INTDIR)\FSBeginTurn.obj" \
	"$(INTDIR)\FSCompete.obj" \
	"$(INTDIR)\FSDiplomat.obj" \
	"$(INTDIR)\FSFuzzy.obj" \
	"$(INTDIR)\FSGold.obj" \
	"$(INTDIR)\FSOppAction.obj" \
	"$(INTDIR)\FSWar.obj" \
	"$(INTDIR)\FzOut.obj" \
	"$(INTDIR)\GlAttck.obj" \
	"$(INTDIR)\GlBlg.obj" \
	"$(INTDIR)\GlEnslave.obj" \
	"$(INTDIR)\GlExpl.obj" \
	"$(INTDIR)\GlFreight.obj" \
	"$(INTDIR)\GlOvertime.obj" \
	"$(INTDIR)\GlRoute.obj" \
	"$(INTDIR)\GlSettl.obj" \
	"$(INTDIR)\GlTrans.obj" \
	"$(INTDIR)\Goal.obj" \
	"$(INTDIR)\Goal_Attack_Region.obj" \
	"$(INTDIR)\Goal_Build_List.obj" \
	"$(INTDIR)\Goal_Build_Supplemental.obj" \
	"$(INTDIR)\Goal_Chokepoint.obj" \
	"$(INTDIR)\Goal_Patrol.obj" \
	"$(INTDIR)\Goal_Perimeter.obj" \
	"$(INTDIR)\GoalBombard.obj" \
	"$(INTDIR)\GoalCityTarget.obj" \
	"$(INTDIR)\GoalDefense.obj" \
	"$(INTDIR)\GoalExpel.obj" \
	"$(INTDIR)\GoalInst.obj" \
	"$(INTDIR)\GoalMapTarget.obj" \
	"$(INTDIR)\GoalRetreat.obj" \
	"$(INTDIR)\GoalRoad.obj" \
	"$(INTDIR)\GoalRustle.obj" \
	"$(INTDIR)\GoalWonder.obj" \
	"$(INTDIR)\Gold.obj" \
	"$(INTDIR)\GoodAgent.obj" \
	"$(INTDIR)\InstBidList.obj" \
	"$(INTDIR)\IParser.obj" \
	"$(INTDIR)\lex.yy.obj" \
	"$(INTDIR)\linked_list.obj" \
	"$(INTDIR)\Log.obj" \
	"$(INTDIR)\MST.obj" \
	"$(INTDIR)\OA.obj" \
	"$(INTDIR)\OA_Entrench.obj" \
	"$(INTDIR)\oa_loot.obj" \
	"$(INTDIR)\OA_Refuel.obj" \
	"$(INTDIR)\OTUtilityList.obj" \
	"$(INTDIR)\Plan.obj" \
	"$(INTDIR)\Planner.obj" \
	"$(INTDIR)\RobotCOM.obj" \
	"$(INTDIR)\RouteUtilityList.obj" \
	"$(INTDIR)\scheduler.obj" \
	"$(INTDIR)\ScienceAgent.obj" \
	"$(INTDIR)\Squad.obj" \
	"$(INTDIR)\Squad_Strength.obj" \
	"$(INTDIR)\Strategic_Map.obj" \
	"$(INTDIR)\WallClock.obj" \
	"$(INTDIR)\WonderBlg.obj" \
	"$(INTDIR)\WonderTbl.obj" \
	"$(INTDIR)\WotP.obj" \
	"$(INTDIR)\y.tab.obj"

"..\civ3\dll\Ai\robotcom.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

OUTDIR=.\debug
INTDIR=.\debug

!IF "$(RECURSE)" == "0" 

ALL : "..\ctp\dll\Ai\DBGrobotcom.dll"

!ELSE 

ALL : "..\ctp\dll\Ai\DBGrobotcom.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Agent.obj"
	-@erase "$(INTDIR)\AI_Match.obj"
	-@erase "$(INTDIR)\AiBldQue.obj"
	-@erase "$(INTDIR)\AiCell.obj"
	-@erase "$(INTDIR)\AIDipReq.obj"
	-@erase "$(INTDIR)\AiMain.obj"
	-@erase "$(INTDIR)\AiMap.obj"
	-@erase "$(INTDIR)\aip.obj"
	-@erase "$(INTDIR)\airndcnt.obj"
	-@erase "$(INTDIR)\AiStats.obj"
	-@erase "$(INTDIR)\AllocWGF.obj"
	-@erase "$(INTDIR)\ArmyAgent.obj"
	-@erase "$(INTDIR)\ArmyGoal.obj"
	-@erase "$(INTDIR)\BldAgent.obj"
	-@erase "$(INTDIR)\BSet.obj"
	-@erase "$(INTDIR)\c3ai.obj"
	-@erase "$(INTDIR)\c3aidebug.obj"
	-@erase "$(INTDIR)\c3mem.obj"
	-@erase "$(INTDIR)\CityAgent.obj"
	-@erase "$(INTDIR)\CityAgentBlg.obj"
	-@erase "$(INTDIR)\CityAgentPop.obj"
	-@erase "$(INTDIR)\CityEdge.obj"
	-@erase "$(INTDIR)\CityFP.obj"
	-@erase "$(INTDIR)\CityGrowth.obj"
	-@erase "$(INTDIR)\CityList.obj"
	-@erase "$(INTDIR)\CityProd.obj"
	-@erase "$(INTDIR)\CityTree.obj"
	-@erase "$(INTDIR)\CityTreeQueue.obj"
	-@erase "$(INTDIR)\CityVertex.obj"
	-@erase "$(INTDIR)\CivArchive.obj"
	-@erase "$(INTDIR)\cont.obj"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\debugassert.obj"
	-@erase "$(INTDIR)\debugcallstack.obj"
	-@erase "$(INTDIR)\debugexception.obj"
	-@erase "$(INTDIR)\debugmemory.obj"
	-@erase "$(INTDIR)\delaunay.obj"
	-@erase "$(INTDIR)\DMapPoint.obj"
	-@erase "$(INTDIR)\Expression.obj"
	-@erase "$(INTDIR)\FliAction.obj"
	-@erase "$(INTDIR)\FliEngine.obj"
	-@erase "$(INTDIR)\FliFuncTab.obj"
	-@erase "$(INTDIR)\fliif.obj"
	-@erase "$(INTDIR)\FliOutputProc.obj"
	-@erase "$(INTDIR)\FliRule.obj"
	-@erase "$(INTDIR)\FliSetFunc.obj"
	-@erase "$(INTDIR)\FliStack.obj"
	-@erase "$(INTDIR)\FliSymbol.obj"
	-@erase "$(INTDIR)\FliSymTab.obj"
	-@erase "$(INTDIR)\ForeignAgent.obj"
	-@erase "$(INTDIR)\ForeignCity.obj"
	-@erase "$(INTDIR)\Foreigner.obj"
	-@erase "$(INTDIR)\FrameClock.obj"
	-@erase "$(INTDIR)\FSAlloc.obj"
	-@erase "$(INTDIR)\FSBeginTurn.obj"
	-@erase "$(INTDIR)\FSCompete.obj"
	-@erase "$(INTDIR)\FSDiplomat.obj"
	-@erase "$(INTDIR)\FSFuzzy.obj"
	-@erase "$(INTDIR)\FSGold.obj"
	-@erase "$(INTDIR)\FSOppAction.obj"
	-@erase "$(INTDIR)\FSWar.obj"
	-@erase "$(INTDIR)\FzOut.obj"
	-@erase "$(INTDIR)\GlAttck.obj"
	-@erase "$(INTDIR)\GlBlg.obj"
	-@erase "$(INTDIR)\GlEnslave.obj"
	-@erase "$(INTDIR)\GlExpl.obj"
	-@erase "$(INTDIR)\GlFreight.obj"
	-@erase "$(INTDIR)\GlOvertime.obj"
	-@erase "$(INTDIR)\GlRoute.obj"
	-@erase "$(INTDIR)\GlSettl.obj"
	-@erase "$(INTDIR)\GlTrans.obj"
	-@erase "$(INTDIR)\Goal.obj"
	-@erase "$(INTDIR)\Goal_Attack_Region.obj"
	-@erase "$(INTDIR)\Goal_Build_List.obj"
	-@erase "$(INTDIR)\Goal_Build_Supplemental.obj"
	-@erase "$(INTDIR)\Goal_Chokepoint.obj"
	-@erase "$(INTDIR)\Goal_Patrol.obj"
	-@erase "$(INTDIR)\Goal_Perimeter.obj"
	-@erase "$(INTDIR)\GoalBombard.obj"
	-@erase "$(INTDIR)\GoalCityTarget.obj"
	-@erase "$(INTDIR)\GoalDefense.obj"
	-@erase "$(INTDIR)\GoalExpel.obj"
	-@erase "$(INTDIR)\GoalInst.obj"
	-@erase "$(INTDIR)\GoalMapTarget.obj"
	-@erase "$(INTDIR)\GoalRetreat.obj"
	-@erase "$(INTDIR)\GoalRoad.obj"
	-@erase "$(INTDIR)\GoalRustle.obj"
	-@erase "$(INTDIR)\GoalWonder.obj"
	-@erase "$(INTDIR)\Gold.obj"
	-@erase "$(INTDIR)\GoodAgent.obj"
	-@erase "$(INTDIR)\InstBidList.obj"
	-@erase "$(INTDIR)\IParser.obj"
	-@erase "$(INTDIR)\lex.yy.obj"
	-@erase "$(INTDIR)\linked_list.obj"
	-@erase "$(INTDIR)\Log.obj"
	-@erase "$(INTDIR)\MST.obj"
	-@erase "$(INTDIR)\OA.obj"
	-@erase "$(INTDIR)\OA_Entrench.obj"
	-@erase "$(INTDIR)\oa_loot.obj"
	-@erase "$(INTDIR)\OA_Refuel.obj"
	-@erase "$(INTDIR)\OTUtilityList.obj"
	-@erase "$(INTDIR)\Plan.obj"
	-@erase "$(INTDIR)\Planner.obj"
	-@erase "$(INTDIR)\RobotCOM.obj"
	-@erase "$(INTDIR)\robotcom.pch"
	-@erase "$(INTDIR)\RouteUtilityList.obj"
	-@erase "$(INTDIR)\scheduler.obj"
	-@erase "$(INTDIR)\ScienceAgent.obj"
	-@erase "$(INTDIR)\Squad.obj"
	-@erase "$(INTDIR)\Squad_Strength.obj"
	-@erase "$(INTDIR)\Strategic_Map.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\WallClock.obj"
	-@erase "$(INTDIR)\WonderBlg.obj"
	-@erase "$(INTDIR)\WonderTbl.obj"
	-@erase "$(INTDIR)\WotP.obj"
	-@erase "$(INTDIR)\y.tab.obj"
	-@erase "$(OUTDIR)\DBGrobotcom.exp"
	-@erase "$(OUTDIR)\DBGrobotcom.lib"
	-@erase "..\ctp\dll\Ai\DBGrobotcom.dll"
	-@erase "..\ctp\dll\Ai\DBGrobotcom.ilk"
	-@erase "..\ctp\dll\Ai\DBGrobotcom.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\robotcom.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib\
 /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:yes\
 /pdb:"..\ctp\dll\Ai\DBGrobotcom.pdb" /debug /machine:I386\
 /nodefaultlib:"libcmtd.lib"\
 /def:"..\..\civ3_code\robotcom\InCom\DBGRobotCOM.def"\
 /out:"..\ctp\dll\Ai\DBGrobotcom.dll" /implib:"$(OUTDIR)\DBGrobotcom.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Agent.obj" \
	"$(INTDIR)\AI_Match.obj" \
	"$(INTDIR)\AiBldQue.obj" \
	"$(INTDIR)\AiCell.obj" \
	"$(INTDIR)\AIDipReq.obj" \
	"$(INTDIR)\AiMain.obj" \
	"$(INTDIR)\AiMap.obj" \
	"$(INTDIR)\aip.obj" \
	"$(INTDIR)\airndcnt.obj" \
	"$(INTDIR)\AiStats.obj" \
	"$(INTDIR)\AllocWGF.obj" \
	"$(INTDIR)\ArmyAgent.obj" \
	"$(INTDIR)\ArmyGoal.obj" \
	"$(INTDIR)\BldAgent.obj" \
	"$(INTDIR)\BSet.obj" \
	"$(INTDIR)\c3ai.obj" \
	"$(INTDIR)\c3aidebug.obj" \
	"$(INTDIR)\c3mem.obj" \
	"$(INTDIR)\CityAgent.obj" \
	"$(INTDIR)\CityAgentBlg.obj" \
	"$(INTDIR)\CityAgentPop.obj" \
	"$(INTDIR)\CityEdge.obj" \
	"$(INTDIR)\CityFP.obj" \
	"$(INTDIR)\CityGrowth.obj" \
	"$(INTDIR)\CityList.obj" \
	"$(INTDIR)\CityProd.obj" \
	"$(INTDIR)\CityTree.obj" \
	"$(INTDIR)\CityTreeQueue.obj" \
	"$(INTDIR)\CityVertex.obj" \
	"$(INTDIR)\CivArchive.obj" \
	"$(INTDIR)\cont.obj" \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\debugassert.obj" \
	"$(INTDIR)\debugcallstack.obj" \
	"$(INTDIR)\debugexception.obj" \
	"$(INTDIR)\debugmemory.obj" \
	"$(INTDIR)\delaunay.obj" \
	"$(INTDIR)\DMapPoint.obj" \
	"$(INTDIR)\Expression.obj" \
	"$(INTDIR)\FliAction.obj" \
	"$(INTDIR)\FliEngine.obj" \
	"$(INTDIR)\FliFuncTab.obj" \
	"$(INTDIR)\fliif.obj" \
	"$(INTDIR)\FliOutputProc.obj" \
	"$(INTDIR)\FliRule.obj" \
	"$(INTDIR)\FliSetFunc.obj" \
	"$(INTDIR)\FliStack.obj" \
	"$(INTDIR)\FliSymbol.obj" \
	"$(INTDIR)\FliSymTab.obj" \
	"$(INTDIR)\ForeignAgent.obj" \
	"$(INTDIR)\ForeignCity.obj" \
	"$(INTDIR)\Foreigner.obj" \
	"$(INTDIR)\FrameClock.obj" \
	"$(INTDIR)\FSAlloc.obj" \
	"$(INTDIR)\FSBeginTurn.obj" \
	"$(INTDIR)\FSCompete.obj" \
	"$(INTDIR)\FSDiplomat.obj" \
	"$(INTDIR)\FSFuzzy.obj" \
	"$(INTDIR)\FSGold.obj" \
	"$(INTDIR)\FSOppAction.obj" \
	"$(INTDIR)\FSWar.obj" \
	"$(INTDIR)\FzOut.obj" \
	"$(INTDIR)\GlAttck.obj" \
	"$(INTDIR)\GlBlg.obj" \
	"$(INTDIR)\GlEnslave.obj" \
	"$(INTDIR)\GlExpl.obj" \
	"$(INTDIR)\GlFreight.obj" \
	"$(INTDIR)\GlOvertime.obj" \
	"$(INTDIR)\GlRoute.obj" \
	"$(INTDIR)\GlSettl.obj" \
	"$(INTDIR)\GlTrans.obj" \
	"$(INTDIR)\Goal.obj" \
	"$(INTDIR)\Goal_Attack_Region.obj" \
	"$(INTDIR)\Goal_Build_List.obj" \
	"$(INTDIR)\Goal_Build_Supplemental.obj" \
	"$(INTDIR)\Goal_Chokepoint.obj" \
	"$(INTDIR)\Goal_Patrol.obj" \
	"$(INTDIR)\Goal_Perimeter.obj" \
	"$(INTDIR)\GoalBombard.obj" \
	"$(INTDIR)\GoalCityTarget.obj" \
	"$(INTDIR)\GoalDefense.obj" \
	"$(INTDIR)\GoalExpel.obj" \
	"$(INTDIR)\GoalInst.obj" \
	"$(INTDIR)\GoalMapTarget.obj" \
	"$(INTDIR)\GoalRetreat.obj" \
	"$(INTDIR)\GoalRoad.obj" \
	"$(INTDIR)\GoalRustle.obj" \
	"$(INTDIR)\GoalWonder.obj" \
	"$(INTDIR)\Gold.obj" \
	"$(INTDIR)\GoodAgent.obj" \
	"$(INTDIR)\InstBidList.obj" \
	"$(INTDIR)\IParser.obj" \
	"$(INTDIR)\lex.yy.obj" \
	"$(INTDIR)\linked_list.obj" \
	"$(INTDIR)\Log.obj" \
	"$(INTDIR)\MST.obj" \
	"$(INTDIR)\OA.obj" \
	"$(INTDIR)\OA_Entrench.obj" \
	"$(INTDIR)\oa_loot.obj" \
	"$(INTDIR)\OA_Refuel.obj" \
	"$(INTDIR)\OTUtilityList.obj" \
	"$(INTDIR)\Plan.obj" \
	"$(INTDIR)\Planner.obj" \
	"$(INTDIR)\RobotCOM.obj" \
	"$(INTDIR)\RouteUtilityList.obj" \
	"$(INTDIR)\scheduler.obj" \
	"$(INTDIR)\ScienceAgent.obj" \
	"$(INTDIR)\Squad.obj" \
	"$(INTDIR)\Squad_Strength.obj" \
	"$(INTDIR)\Strategic_Map.obj" \
	"$(INTDIR)\WallClock.obj" \
	"$(INTDIR)\WonderBlg.obj" \
	"$(INTDIR)\WonderTbl.obj" \
	"$(INTDIR)\WotP.obj" \
	"$(INTDIR)\y.tab.obj"

"..\ctp\dll\Ai\DBGrobotcom.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

OUTDIR=.\Test
INTDIR=.\Test

!IF "$(RECURSE)" == "0" 

ALL : "..\ctp\dll\Ai\TESTrobotcom.dll"

!ELSE 

ALL : "..\ctp\dll\Ai\TESTrobotcom.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Agent.obj"
	-@erase "$(INTDIR)\AI_Match.obj"
	-@erase "$(INTDIR)\AiBldQue.obj"
	-@erase "$(INTDIR)\AiCell.obj"
	-@erase "$(INTDIR)\AIDipReq.obj"
	-@erase "$(INTDIR)\AiMain.obj"
	-@erase "$(INTDIR)\AiMap.obj"
	-@erase "$(INTDIR)\aip.obj"
	-@erase "$(INTDIR)\airndcnt.obj"
	-@erase "$(INTDIR)\AiStats.obj"
	-@erase "$(INTDIR)\AllocWGF.obj"
	-@erase "$(INTDIR)\ArmyAgent.obj"
	-@erase "$(INTDIR)\ArmyGoal.obj"
	-@erase "$(INTDIR)\BldAgent.obj"
	-@erase "$(INTDIR)\BSet.obj"
	-@erase "$(INTDIR)\c3ai.obj"
	-@erase "$(INTDIR)\c3aidebug.obj"
	-@erase "$(INTDIR)\c3mem.obj"
	-@erase "$(INTDIR)\CityAgent.obj"
	-@erase "$(INTDIR)\CityAgentBlg.obj"
	-@erase "$(INTDIR)\CityAgentPop.obj"
	-@erase "$(INTDIR)\CityEdge.obj"
	-@erase "$(INTDIR)\CityFP.obj"
	-@erase "$(INTDIR)\CityGrowth.obj"
	-@erase "$(INTDIR)\CityList.obj"
	-@erase "$(INTDIR)\CityProd.obj"
	-@erase "$(INTDIR)\CityTree.obj"
	-@erase "$(INTDIR)\CityTreeQueue.obj"
	-@erase "$(INTDIR)\CityVertex.obj"
	-@erase "$(INTDIR)\CivArchive.obj"
	-@erase "$(INTDIR)\cont.obj"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\debugassert.obj"
	-@erase "$(INTDIR)\debugcallstack.obj"
	-@erase "$(INTDIR)\debugexception.obj"
	-@erase "$(INTDIR)\debugmemory.obj"
	-@erase "$(INTDIR)\delaunay.obj"
	-@erase "$(INTDIR)\DMapPoint.obj"
	-@erase "$(INTDIR)\Expression.obj"
	-@erase "$(INTDIR)\FliAction.obj"
	-@erase "$(INTDIR)\FliEngine.obj"
	-@erase "$(INTDIR)\FliFuncTab.obj"
	-@erase "$(INTDIR)\fliif.obj"
	-@erase "$(INTDIR)\FliOutputProc.obj"
	-@erase "$(INTDIR)\FliRule.obj"
	-@erase "$(INTDIR)\FliSetFunc.obj"
	-@erase "$(INTDIR)\FliStack.obj"
	-@erase "$(INTDIR)\FliSymbol.obj"
	-@erase "$(INTDIR)\FliSymTab.obj"
	-@erase "$(INTDIR)\ForeignAgent.obj"
	-@erase "$(INTDIR)\ForeignCity.obj"
	-@erase "$(INTDIR)\Foreigner.obj"
	-@erase "$(INTDIR)\FrameClock.obj"
	-@erase "$(INTDIR)\FSAlloc.obj"
	-@erase "$(INTDIR)\FSBeginTurn.obj"
	-@erase "$(INTDIR)\FSCompete.obj"
	-@erase "$(INTDIR)\FSDiplomat.obj"
	-@erase "$(INTDIR)\FSFuzzy.obj"
	-@erase "$(INTDIR)\FSGold.obj"
	-@erase "$(INTDIR)\FSOppAction.obj"
	-@erase "$(INTDIR)\FSWar.obj"
	-@erase "$(INTDIR)\FzOut.obj"
	-@erase "$(INTDIR)\GlAttck.obj"
	-@erase "$(INTDIR)\GlBlg.obj"
	-@erase "$(INTDIR)\GlEnslave.obj"
	-@erase "$(INTDIR)\GlExpl.obj"
	-@erase "$(INTDIR)\GlFreight.obj"
	-@erase "$(INTDIR)\GlOvertime.obj"
	-@erase "$(INTDIR)\GlRoute.obj"
	-@erase "$(INTDIR)\GlSettl.obj"
	-@erase "$(INTDIR)\GlTrans.obj"
	-@erase "$(INTDIR)\Goal.obj"
	-@erase "$(INTDIR)\Goal_Attack_Region.obj"
	-@erase "$(INTDIR)\Goal_Build_List.obj"
	-@erase "$(INTDIR)\Goal_Build_Supplemental.obj"
	-@erase "$(INTDIR)\Goal_Chokepoint.obj"
	-@erase "$(INTDIR)\Goal_Patrol.obj"
	-@erase "$(INTDIR)\Goal_Perimeter.obj"
	-@erase "$(INTDIR)\GoalBombard.obj"
	-@erase "$(INTDIR)\GoalCityTarget.obj"
	-@erase "$(INTDIR)\GoalDefense.obj"
	-@erase "$(INTDIR)\GoalExpel.obj"
	-@erase "$(INTDIR)\GoalInst.obj"
	-@erase "$(INTDIR)\GoalMapTarget.obj"
	-@erase "$(INTDIR)\GoalRetreat.obj"
	-@erase "$(INTDIR)\GoalRoad.obj"
	-@erase "$(INTDIR)\GoalRustle.obj"
	-@erase "$(INTDIR)\GoalWonder.obj"
	-@erase "$(INTDIR)\Gold.obj"
	-@erase "$(INTDIR)\GoodAgent.obj"
	-@erase "$(INTDIR)\InstBidList.obj"
	-@erase "$(INTDIR)\IParser.obj"
	-@erase "$(INTDIR)\lex.yy.obj"
	-@erase "$(INTDIR)\linked_list.obj"
	-@erase "$(INTDIR)\Log.obj"
	-@erase "$(INTDIR)\MST.obj"
	-@erase "$(INTDIR)\OA.obj"
	-@erase "$(INTDIR)\OA_Entrench.obj"
	-@erase "$(INTDIR)\oa_loot.obj"
	-@erase "$(INTDIR)\OA_Refuel.obj"
	-@erase "$(INTDIR)\OTUtilityList.obj"
	-@erase "$(INTDIR)\Plan.obj"
	-@erase "$(INTDIR)\Planner.obj"
	-@erase "$(INTDIR)\RobotCOM.obj"
	-@erase "$(INTDIR)\robotcom.pch"
	-@erase "$(INTDIR)\RouteUtilityList.obj"
	-@erase "$(INTDIR)\scheduler.obj"
	-@erase "$(INTDIR)\ScienceAgent.obj"
	-@erase "$(INTDIR)\Squad.obj"
	-@erase "$(INTDIR)\Squad_Strength.obj"
	-@erase "$(INTDIR)\Strategic_Map.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\WallClock.obj"
	-@erase "$(INTDIR)\WonderBlg.obj"
	-@erase "$(INTDIR)\WonderTbl.obj"
	-@erase "$(INTDIR)\WotP.obj"
	-@erase "$(INTDIR)\y.tab.obj"
	-@erase "$(OUTDIR)\TESTrobotcom.exp"
	-@erase "$(OUTDIR)\TESTrobotcom.lib"
	-@erase "$(OUTDIR)\TESTrobotcom.map"
	-@erase "..\ctp\dll\Ai\TESTrobotcom.dll"
	-@erase "..\ctp\dll\ai\Testrobotcom.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Test/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\robotcom.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imagehlp.lib winmm.lib\
 MSVCRT.lib MSVCRTD.lib /nologo /base:"0x20000000" /subsystem:windows /dll\
 /incremental:no /pdb:"..\ctp\dll\ai\Testrobotcom.pdb"\
 /map:"$(INTDIR)\TESTrobotcom.map" /debug /machine:I386\
 /nodefaultlib:"libcmtd.lib"\
 /def:"..\..\civ3_code\robotcom\InCom\TestRobotCOM.def"\
 /out:"..\ctp\dll\Ai\TESTrobotcom.dll" /implib:"$(OUTDIR)\TESTrobotcom.lib"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Agent.obj" \
	"$(INTDIR)\AI_Match.obj" \
	"$(INTDIR)\AiBldQue.obj" \
	"$(INTDIR)\AiCell.obj" \
	"$(INTDIR)\AIDipReq.obj" \
	"$(INTDIR)\AiMain.obj" \
	"$(INTDIR)\AiMap.obj" \
	"$(INTDIR)\aip.obj" \
	"$(INTDIR)\airndcnt.obj" \
	"$(INTDIR)\AiStats.obj" \
	"$(INTDIR)\AllocWGF.obj" \
	"$(INTDIR)\ArmyAgent.obj" \
	"$(INTDIR)\ArmyGoal.obj" \
	"$(INTDIR)\BldAgent.obj" \
	"$(INTDIR)\BSet.obj" \
	"$(INTDIR)\c3ai.obj" \
	"$(INTDIR)\c3aidebug.obj" \
	"$(INTDIR)\c3mem.obj" \
	"$(INTDIR)\CityAgent.obj" \
	"$(INTDIR)\CityAgentBlg.obj" \
	"$(INTDIR)\CityAgentPop.obj" \
	"$(INTDIR)\CityEdge.obj" \
	"$(INTDIR)\CityFP.obj" \
	"$(INTDIR)\CityGrowth.obj" \
	"$(INTDIR)\CityList.obj" \
	"$(INTDIR)\CityProd.obj" \
	"$(INTDIR)\CityTree.obj" \
	"$(INTDIR)\CityTreeQueue.obj" \
	"$(INTDIR)\CityVertex.obj" \
	"$(INTDIR)\CivArchive.obj" \
	"$(INTDIR)\cont.obj" \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\debugassert.obj" \
	"$(INTDIR)\debugcallstack.obj" \
	"$(INTDIR)\debugexception.obj" \
	"$(INTDIR)\debugmemory.obj" \
	"$(INTDIR)\delaunay.obj" \
	"$(INTDIR)\DMapPoint.obj" \
	"$(INTDIR)\Expression.obj" \
	"$(INTDIR)\FliAction.obj" \
	"$(INTDIR)\FliEngine.obj" \
	"$(INTDIR)\FliFuncTab.obj" \
	"$(INTDIR)\fliif.obj" \
	"$(INTDIR)\FliOutputProc.obj" \
	"$(INTDIR)\FliRule.obj" \
	"$(INTDIR)\FliSetFunc.obj" \
	"$(INTDIR)\FliStack.obj" \
	"$(INTDIR)\FliSymbol.obj" \
	"$(INTDIR)\FliSymTab.obj" \
	"$(INTDIR)\ForeignAgent.obj" \
	"$(INTDIR)\ForeignCity.obj" \
	"$(INTDIR)\Foreigner.obj" \
	"$(INTDIR)\FrameClock.obj" \
	"$(INTDIR)\FSAlloc.obj" \
	"$(INTDIR)\FSBeginTurn.obj" \
	"$(INTDIR)\FSCompete.obj" \
	"$(INTDIR)\FSDiplomat.obj" \
	"$(INTDIR)\FSFuzzy.obj" \
	"$(INTDIR)\FSGold.obj" \
	"$(INTDIR)\FSOppAction.obj" \
	"$(INTDIR)\FSWar.obj" \
	"$(INTDIR)\FzOut.obj" \
	"$(INTDIR)\GlAttck.obj" \
	"$(INTDIR)\GlBlg.obj" \
	"$(INTDIR)\GlEnslave.obj" \
	"$(INTDIR)\GlExpl.obj" \
	"$(INTDIR)\GlFreight.obj" \
	"$(INTDIR)\GlOvertime.obj" \
	"$(INTDIR)\GlRoute.obj" \
	"$(INTDIR)\GlSettl.obj" \
	"$(INTDIR)\GlTrans.obj" \
	"$(INTDIR)\Goal.obj" \
	"$(INTDIR)\Goal_Attack_Region.obj" \
	"$(INTDIR)\Goal_Build_List.obj" \
	"$(INTDIR)\Goal_Build_Supplemental.obj" \
	"$(INTDIR)\Goal_Chokepoint.obj" \
	"$(INTDIR)\Goal_Patrol.obj" \
	"$(INTDIR)\Goal_Perimeter.obj" \
	"$(INTDIR)\GoalBombard.obj" \
	"$(INTDIR)\GoalCityTarget.obj" \
	"$(INTDIR)\GoalDefense.obj" \
	"$(INTDIR)\GoalExpel.obj" \
	"$(INTDIR)\GoalInst.obj" \
	"$(INTDIR)\GoalMapTarget.obj" \
	"$(INTDIR)\GoalRetreat.obj" \
	"$(INTDIR)\GoalRoad.obj" \
	"$(INTDIR)\GoalRustle.obj" \
	"$(INTDIR)\GoalWonder.obj" \
	"$(INTDIR)\Gold.obj" \
	"$(INTDIR)\GoodAgent.obj" \
	"$(INTDIR)\InstBidList.obj" \
	"$(INTDIR)\IParser.obj" \
	"$(INTDIR)\lex.yy.obj" \
	"$(INTDIR)\linked_list.obj" \
	"$(INTDIR)\Log.obj" \
	"$(INTDIR)\MST.obj" \
	"$(INTDIR)\OA.obj" \
	"$(INTDIR)\OA_Entrench.obj" \
	"$(INTDIR)\oa_loot.obj" \
	"$(INTDIR)\OA_Refuel.obj" \
	"$(INTDIR)\OTUtilityList.obj" \
	"$(INTDIR)\Plan.obj" \
	"$(INTDIR)\Planner.obj" \
	"$(INTDIR)\RobotCOM.obj" \
	"$(INTDIR)\RouteUtilityList.obj" \
	"$(INTDIR)\scheduler.obj" \
	"$(INTDIR)\ScienceAgent.obj" \
	"$(INTDIR)\Squad.obj" \
	"$(INTDIR)\Squad_Strength.obj" \
	"$(INTDIR)\Strategic_Map.obj" \
	"$(INTDIR)\WallClock.obj" \
	"$(INTDIR)\WonderBlg.obj" \
	"$(INTDIR)\WonderTbl.obj" \
	"$(INTDIR)\WotP.obj" \
	"$(INTDIR)\y.tab.obj"

"..\ctp\dll\Ai\TESTrobotcom.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "robotcom - Win32 Release" || "$(CFG)" ==\
 "robotcom - Win32 Debug" || "$(CFG)" == "robotcom - Win32 Test"
SOURCE=.\aimgr\AiMain.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AIMAI=\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\FrameClock.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DMapPoint.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\AIDipReq.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSAlloc.h"\
	".\FrmState\FSBeginTurn.h"\
	".\FrmState\FSCompete.h"\
	".\FrmState\FSDiplomat.h"\
	".\FrmState\FSFuzzy.h"\
	".\FrmState\FSGold.h"\
	".\FrmState\FSOppAction.h"\
	".\FrmState\FSWar.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlOvertime.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\OA.h"\
	".\goals\OA_Entrench.h"\
	".\goals\oa_loot.h"\
	".\goals\OA_Refuel.h"\
	".\InCom\IRobot.h"\
	".\InCom\RobotCOM.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\MST.h"\
	
NODEP_CPP_AIMAI=\
	".\aimgr\c3.h"\
	".\aimgr\DebugMemory.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\AiMain.obj" : $(SOURCE) $(DEP_CPP_AIMAI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AIMAI=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\FrameClock.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DMapPoint.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\list_array.h"\
	".\backdoor\pool.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\AIDipReq.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSAlloc.h"\
	".\FrmState\FSBeginTurn.h"\
	".\FrmState\FSCompete.h"\
	".\FrmState\FSDiplomat.h"\
	".\FrmState\FSFuzzy.h"\
	".\FrmState\FSGold.h"\
	".\FrmState\FSOppAction.h"\
	".\FrmState\FSWar.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlOvertime.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\OA.h"\
	".\goals\OA_Entrench.h"\
	".\goals\oa_loot.h"\
	".\goals\OA_Refuel.h"\
	".\InCom\IRobot.h"\
	".\InCom\RobotCOM.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\MST.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AiMain.obj" : $(SOURCE) $(DEP_CPP_AIMAI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AIMAI=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\FrameClock.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DMapPoint.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\AIDipReq.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSAlloc.h"\
	".\FrmState\FSBeginTurn.h"\
	".\FrmState\FSCompete.h"\
	".\FrmState\FSDiplomat.h"\
	".\FrmState\FSFuzzy.h"\
	".\FrmState\FSGold.h"\
	".\FrmState\FSOppAction.h"\
	".\FrmState\FSWar.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlOvertime.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\OA.h"\
	".\goals\OA_Entrench.h"\
	".\goals\oa_loot.h"\
	".\goals\OA_Refuel.h"\
	".\InCom\IRobot.h"\
	".\InCom\RobotCOM.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\MST.h"\
	

"$(INTDIR)\AiMain.obj" : $(SOURCE) $(DEP_CPP_AIMAI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\airndcnt.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AIRND=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\airndcnt.h"\
	".\backdoor\CivArchive.h"\
	
NODEP_CPP_AIRND=\
	".\aimgr\c3.h"\
	

"$(INTDIR)\airndcnt.obj" : $(SOURCE) $(DEP_CPP_AIRND) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AIRND=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\airndcnt.obj" : $(SOURCE) $(DEP_CPP_AIRND) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AIRND=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\airndcnt.h"\
	".\backdoor\CivArchive.h"\
	

"$(INTDIR)\airndcnt.obj" : $(SOURCE) $(DEP_CPP_AIRND) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\AiStats.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AISTA=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_AISTA=\
	".\aimgr\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\AiStats.obj" : $(SOURCE) $(DEP_CPP_AISTA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AISTA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AiStats.obj" : $(SOURCE) $(DEP_CPP_AISTA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AISTA=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_AISTA=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\AiStats.obj" : $(SOURCE) $(DEP_CPP_AISTA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\c3ai.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

NODEP_CPP_C3AI_=\
	".\aimgr\c3.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yc"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\c3ai.obj"	"$(INTDIR)\robotcom.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_C3AI_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	"c:\mssdk\include\dinput.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fp"$(INTDIR)\robotcom.pch" /Yc"c3.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\c3ai.obj"	"$(INTDIR)\robotcom.pch" : $(SOURCE) $(DEP_CPP_C3AI_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_C3AI_=\
	"..\ctp\c3.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fp"$(INTDIR)\robotcom.pch" /Yc"c3.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\c3ai.obj"	"$(INTDIR)\robotcom.pch" : $(SOURCE) $(DEP_CPP_C3AI_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\aimgr\c3aidebug.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_C3AID=\
	".\aimgr\c3ai.h"\
	
NODEP_CPP_C3AID=\
	".\aimgr\c3.h"\
	

"$(INTDIR)\c3aidebug.obj" : $(SOURCE) $(DEP_CPP_C3AID) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_C3AID=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\c3aidebug.obj" : $(SOURCE) $(DEP_CPP_C3AID) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_C3AID=\
	"..\ctp\c3.h"\
	".\aimgr\c3ai.h"\
	

"$(INTDIR)\c3aidebug.obj" : $(SOURCE) $(DEP_CPP_C3AID) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\Foreigner.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FOREI=\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_FOREI=\
	".\aimgr\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Foreigner.obj" : $(SOURCE) $(DEP_CPP_FOREI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FOREI=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Foreigner.obj" : $(SOURCE) $(DEP_CPP_FOREI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FOREI=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\Foreigner.obj" : $(SOURCE) $(DEP_CPP_FOREI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\FrameClock.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FRAME=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\FrameClock.h"\
	".\backdoor\CivArchive.h"\
	
NODEP_CPP_FRAME=\
	".\aimgr\c3.h"\
	

"$(INTDIR)\FrameClock.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FRAME=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\FrameClock.h"\
	".\backdoor\CivArchive.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FrameClock.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FRAME=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\FrameClock.h"\
	".\backdoor\CivArchive.h"\
	

"$(INTDIR)\FrameClock.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\Gold.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOLD_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOLD_=\
	".\aimgr\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Gold.obj" : $(SOURCE) $(DEP_CPP_GOLD_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOLD_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Gold.obj" : $(SOURCE) $(DEP_CPP_GOLD_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOLD_=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOLD_=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\Gold.obj" : $(SOURCE) $(DEP_CPP_GOLD_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\WonderBlg.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_WONDE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\WonderBlg.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	
NODEP_CPP_WONDE=\
	".\aimgr\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\WonderBlg.obj" : $(SOURCE) $(DEP_CPP_WONDE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_WONDE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\WonderBlg.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\WonderBlg.obj" : $(SOURCE) $(DEP_CPP_WONDE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_WONDE=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\WonderBlg.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	
NODEP_CPP_WONDE=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\WonderBlg.obj" : $(SOURCE) $(DEP_CPP_WONDE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\aimgr\WonderTbl.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_WONDER=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\WonderBlg.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_WONDER=\
	".\aimgr\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\WonderTbl.obj" : $(SOURCE) $(DEP_CPP_WONDER) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_WONDER=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\WonderBlg.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\WonderTbl.obj" : $(SOURCE) $(DEP_CPP_WONDER) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_WONDER=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\WonderBlg.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_WONDER=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\WonderTbl.obj" : $(SOURCE) $(DEP_CPP_WONDER) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\InCom\RobotCOM.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_ROBOT=\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\fliif.h"\
	".\InCom\IRobot.h"\
	".\InCom\RobotCOM.h"\
	
NODEP_CPP_ROBOT=\
	".\InCom\c3.h"\
	".\InCom\DebugCallstack.h"\
	".\InCom\DebugMemory.h"\
	".\InCom\Log.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\RobotCOM.obj" : $(SOURCE) $(DEP_CPP_ROBOT) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_ROBOT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\fliif.h"\
	".\InCom\IRobot.h"\
	".\InCom\RobotCOM.h"\
	"c:\mssdk\include\dinput.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\RobotCOM.obj" : $(SOURCE) $(DEP_CPP_ROBOT) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_ROBOT=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\fliif.h"\
	".\InCom\IRobot.h"\
	".\InCom\RobotCOM.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\RobotCOM.obj" : $(SOURCE) $(DEP_CPP_ROBOT) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Agent\Agent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AGENT=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_AGENT=\
	".\Agent\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Agent.obj" : $(SOURCE) $(DEP_CPP_AGENT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AGENT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Agent.obj" : $(SOURCE) $(DEP_CPP_AGENT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AGENT=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\Agent.obj" : $(SOURCE) $(DEP_CPP_AGENT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\AiBldQue.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AIBLD=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_AIBLD=\
	".\Agent\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\AiBldQue.obj" : $(SOURCE) $(DEP_CPP_AIBLD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AIBLD=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AiBldQue.obj" : $(SOURCE) $(DEP_CPP_AIBLD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AIBLD=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\AiBldQue.obj" : $(SOURCE) $(DEP_CPP_AIBLD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\ArmyAgent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_ARMYA=\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\FrmState\FrameState.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\GoalDefense.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_ARMYA=\
	".\Agent\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\ArmyAgent.obj" : $(SOURCE) $(DEP_CPP_ARMYA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_ARMYA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\FrmState\FrameState.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\GoalDefense.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\ArmyAgent.obj" : $(SOURCE) $(DEP_CPP_ARMYA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_ARMYA=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\FrmState\FrameState.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\GoalDefense.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\ArmyAgent.obj" : $(SOURCE) $(DEP_CPP_ARMYA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\BldAgent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_BLDAG=\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_BLDAG=\
	".\Agent\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\BldAgent.obj" : $(SOURCE) $(DEP_CPP_BLDAG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_BLDAG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\BldAgent.obj" : $(SOURCE) $(DEP_CPP_BLDAG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_BLDAG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\BldAgent.obj" : $(SOURCE) $(DEP_CPP_BLDAG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\CityAgent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYA=\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_CITYA=\
	".\Agent\c3.h"\
	".\Agent\c3errors.h"\
	".\Agent\c3types.h"\
	".\Agent\PointerList.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityAgent.obj" : $(SOURCE) $(DEP_CPP_CITYA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\list_array.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityAgent.obj" : $(SOURCE) $(DEP_CPP_CITYA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_CITYA=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityAgent.obj" : $(SOURCE) $(DEP_CPP_CITYA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\CityAgentBlg.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYAG=\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_CITYAG=\
	".\Agent\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityAgentBlg.obj" : $(SOURCE) $(DEP_CPP_CITYAG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYAG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityAgentBlg.obj" : $(SOURCE) $(DEP_CPP_CITYAG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYAG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_CITYAG=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityAgentBlg.obj" : $(SOURCE) $(DEP_CPP_CITYAG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\CityAgentPop.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYAGE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_CITYAGE=\
	".\Agent\c3.h"\
	".\Agent\c3errors.h"\
	".\Agent\c3types.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityAgentPop.obj" : $(SOURCE) $(DEP_CPP_CITYAGE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYAGE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityAgentPop.obj" : $(SOURCE) $(DEP_CPP_CITYAGE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYAGE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\AiBldQue.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\goals\GlFreight.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_CITYAGE=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityAgentPop.obj" : $(SOURCE) $(DEP_CPP_CITYAGE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\ForeignAgent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FOREIG=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ForeignAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\goals\GoalBombard.h"\
	".\goals\GoalExpel.h"\
	".\goals\GoalRustle.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_FOREIG=\
	".\Agent\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\ForeignAgent.obj" : $(SOURCE) $(DEP_CPP_FOREIG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FOREIG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ForeignAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\goals\GoalBombard.h"\
	".\goals\GoalExpel.h"\
	".\goals\GoalRustle.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\ForeignAgent.obj" : $(SOURCE) $(DEP_CPP_FOREIG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FOREIG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ForeignAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\goals\GoalBombard.h"\
	".\goals\GoalExpel.h"\
	".\goals\GoalRustle.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\ForeignAgent.obj" : $(SOURCE) $(DEP_CPP_FOREIG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\ForeignCity.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FOREIGN=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_FOREIGN=\
	".\Agent\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\ForeignCity.obj" : $(SOURCE) $(DEP_CPP_FOREIGN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FOREIGN=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\ForeignCity.obj" : $(SOURCE) $(DEP_CPP_FOREIGN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FOREIGN=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\ForeignCity.obj" : $(SOURCE) $(DEP_CPP_FOREIGN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\GoodAgent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOODA=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	
NODEP_CPP_GOODA=\
	".\Agent\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GoodAgent.obj" : $(SOURCE) $(DEP_CPP_GOODA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOODA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoodAgent.obj" : $(SOURCE) $(DEP_CPP_GOODA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOODA=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	
NODEP_CPP_GOODA=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GoodAgent.obj" : $(SOURCE) $(DEP_CPP_GOODA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Agent\ScienceAgent.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_SCIEN=\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderBlg.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_SCIEN=\
	".\Agent\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\ScienceAgent.obj" : $(SOURCE) $(DEP_CPP_SCIEN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_SCIEN=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderBlg.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\ScienceAgent.obj" : $(SOURCE) $(DEP_CPP_SCIEN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_SCIEN=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\BldAgent.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Gold.h"\
	".\aimgr\WonderBlg.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_SCIEN=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\ScienceAgent.obj" : $(SOURCE) $(DEP_CPP_SCIEN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ctp\debugtools\Debug.c

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DEBUG=\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\debugexception.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DEBUG=\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\debugexception.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DEBUG=\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\debugexception.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ctp\debugtools\debugassert.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DEBUGA=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugassert.obj" : $(SOURCE) $(DEP_CPP_DEBUGA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DEBUGA=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugassert.obj" : $(SOURCE) $(DEP_CPP_DEBUGA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DEBUGA=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugassert.obj" : $(SOURCE) $(DEP_CPP_DEBUGA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ctp\debugtools\debugcallstack.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DEBUGC=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugcallstack.obj" : $(SOURCE) $(DEP_CPP_DEBUGC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DEBUGC=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugcallstack.obj" : $(SOURCE) $(DEP_CPP_DEBUGC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DEBUGC=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugcallstack.obj" : $(SOURCE) $(DEP_CPP_DEBUGC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ctp\debugtools\debugexception.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DEBUGE=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\debugexception.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugexception.obj" : $(SOURCE) $(DEP_CPP_DEBUGE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DEBUGE=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\debugexception.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugexception.obj" : $(SOURCE) $(DEP_CPP_DEBUGE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DEBUGE=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\debugexception.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugexception.obj" : $(SOURCE) $(DEP_CPP_DEBUGE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ctp\debugtools\debugmemory.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DEBUGM=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugmemory.obj" : $(SOURCE) $(DEP_CPP_DEBUGM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DEBUGM=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugmemory.obj" : $(SOURCE) $(DEP_CPP_DEBUGM) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DEBUGM=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\debugassert.h"\
	"..\ctp\debugtools\DebugCallStack.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debugmemory.obj" : $(SOURCE) $(DEP_CPP_DEBUGM) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ctp\debugtools\Log.cpp
DEP_CPP_LOG_C=\
	"..\ctp\debugtools\breakpoint.h"\
	"..\ctp\debugtools\log.h"\
	"..\ctp\debugtools\log_on.h"\
	

!IF  "$(CFG)" == "robotcom - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fp"$(INTDIR)\robotcom.pch" /Yu"c3.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\backdoor\BSet.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_BSET_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	
NODEP_CPP_BSET_=\
	".\backdoor\c3.h"\
	

"$(INTDIR)\BSet.obj" : $(SOURCE) $(DEP_CPP_BSET_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_BSET_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\BSet.obj" : $(SOURCE) $(DEP_CPP_BSET_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_BSET_=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	

"$(INTDIR)\BSet.obj" : $(SOURCE) $(DEP_CPP_BSET_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\backdoor\c3mem.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

NODEP_CPP_C3MEM=\
	".\backdoor\c3.h"\
	".\backdoor\DebugMemory.h"\
	

"$(INTDIR)\c3mem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_C3MEM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\c3mem.obj" : $(SOURCE) $(DEP_CPP_C3MEM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_C3MEM=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	

"$(INTDIR)\c3mem.obj" : $(SOURCE) $(DEP_CPP_C3MEM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\backdoor\CivArchive.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CIVAR=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	
NODEP_CPP_CIVAR=\
	".\backdoor\c3.h"\
	".\backdoor\c3errors.h"\
	".\backdoor\c3types.h"\
	

"$(INTDIR)\CivArchive.obj" : $(SOURCE) $(DEP_CPP_CIVAR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CIVAR=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CivArchive.obj" : $(SOURCE) $(DEP_CPP_CIVAR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CIVAR=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	

"$(INTDIR)\CivArchive.obj" : $(SOURCE) $(DEP_CPP_CIVAR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\backdoor\DMapPoint.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DMAPP=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DMapPoint.h"\
	
NODEP_CPP_DMAPP=\
	".\backdoor\c3.h"\
	

"$(INTDIR)\DMapPoint.obj" : $(SOURCE) $(DEP_CPP_DMAPP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DMAPP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DMapPoint.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\DMapPoint.obj" : $(SOURCE) $(DEP_CPP_DMAPP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DMAPP=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DMapPoint.h"\
	

"$(INTDIR)\DMapPoint.obj" : $(SOURCE) $(DEP_CPP_DMAPP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\backdoor\WallClock.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_WALLC=\
	".\backdoor\WallClock.h"\
	
NODEP_CPP_WALLC=\
	".\backdoor\c3.h"\
	

"$(INTDIR)\WallClock.obj" : $(SOURCE) $(DEP_CPP_WALLC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_WALLC=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\WallClock.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\WallClock.obj" : $(SOURCE) $(DEP_CPP_WALLC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_WALLC=\
	"..\ctp\c3.h"\
	".\backdoor\WallClock.h"\
	

"$(INTDIR)\WallClock.obj" : $(SOURCE) $(DEP_CPP_WALLC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\AIDipReq.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AIDIP=\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\FrmState\AIDipReq.h"\
	".\Fuzzy\FzDipVars.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_AIDIP=\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\AIDipReq.obj" : $(SOURCE) $(DEP_CPP_AIDIP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AIDIP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\FrmState\AIDipReq.h"\
	".\Fuzzy\FzDipVars.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AIDipReq.obj" : $(SOURCE) $(DEP_CPP_AIDIP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AIDIP=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\FrmState\AIDipReq.h"\
	".\Fuzzy\FzDipVars.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\AIDipReq.obj" : $(SOURCE) $(DEP_CPP_AIDIP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSAlloc.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSALL=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSAlloc.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\goals\GlRoute.h"\
	".\Goals\GlSettl.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalRoad.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_FSALL=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSAlloc.obj" : $(SOURCE) $(DEP_CPP_FSALL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSALL=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSAlloc.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\goals\GlRoute.h"\
	".\Goals\GlSettl.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalRoad.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSAlloc.obj" : $(SOURCE) $(DEP_CPP_FSALL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSALL=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSAlloc.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\goals\GlRoute.h"\
	".\Goals\GlSettl.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalRoad.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_FSALL=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\FSAlloc.obj" : $(SOURCE) $(DEP_CPP_FSALL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSBeginTurn.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSBEG=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSBeginTurn.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	
NODEP_CPP_FSBEG=\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSBeginTurn.obj" : $(SOURCE) $(DEP_CPP_FSBEG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSBEG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSBeginTurn.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSBeginTurn.obj" : $(SOURCE) $(DEP_CPP_FSBEG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSBEG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSBeginTurn.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	

"$(INTDIR)\FSBeginTurn.obj" : $(SOURCE) $(DEP_CPP_FSBEG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSCompete.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSCOM=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSCompete.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\goals\GlRoute.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\Goal_Attack_Region.h"\
	".\goals\Goal_Patrol.h"\
	".\goals\Goal_Perimeter.h"\
	".\goals\GoalCityTarget.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\GoalRetreat.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_FSCOM=\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSCompete.obj" : $(SOURCE) $(DEP_CPP_FSCOM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSCOM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSCompete.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\goals\GlRoute.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\Goal_Attack_Region.h"\
	".\goals\Goal_Patrol.h"\
	".\goals\Goal_Perimeter.h"\
	".\goals\GoalCityTarget.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\GoalRetreat.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSCompete.obj" : $(SOURCE) $(DEP_CPP_FSCOM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSCOM=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSCompete.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\goals\GlRoute.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\Goal_Attack_Region.h"\
	".\goals\Goal_Patrol.h"\
	".\goals\Goal_Perimeter.h"\
	".\goals\GoalCityTarget.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\GoalRetreat.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\FSCompete.obj" : $(SOURCE) $(DEP_CPP_FSCOM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSDiplomat.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSDIP=\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\AIDipReq.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSDiplomat.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\Fuzzy\FzDipVars.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	
NODEP_CPP_FSDIP=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\FrmState\c3.h"\
	".\FrmState\PointerList.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSDiplomat.obj" : $(SOURCE) $(DEP_CPP_FSDIP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSDIP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\AIDipReq.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSDiplomat.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\Fuzzy\FzDipVars.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSDiplomat.obj" : $(SOURCE) $(DEP_CPP_FSDIP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSDIP=\
	"..\ctp\c3.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3DipReq.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IC3Trade.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\AIDipReq.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSDiplomat.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\Fuzzy\FzDipVars.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	
NODEP_CPP_FSDIP=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\FSDiplomat.obj" : $(SOURCE) $(DEP_CPP_FSDIP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSFuzzy.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSFUZ=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSFuzzy.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\cont.h"\
	
NODEP_CPP_FSFUZ=\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSFuzzy.obj" : $(SOURCE) $(DEP_CPP_FSFUZ) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSFUZ=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSFuzzy.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\cont.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSFuzzy.obj" : $(SOURCE) $(DEP_CPP_FSFUZ) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSFUZ=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\AiStats.h"\
	".\aimgr\Foreigner.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSFuzzy.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\cont.h"\
	

"$(INTDIR)\FSFuzzy.obj" : $(SOURCE) $(DEP_CPP_FSFUZ) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSGold.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSGOL=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSGold.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_FSGOL=\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSGold.obj" : $(SOURCE) $(DEP_CPP_FSGOL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSGOL=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSGold.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSGold.obj" : $(SOURCE) $(DEP_CPP_FSGOL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSGOL=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSGold.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\FSGold.obj" : $(SOURCE) $(DEP_CPP_FSGOL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSOppAction.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSOPP=\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSOppAction.h"\
	".\goals\OA.h"\
	
NODEP_CPP_FSOPP=\
	".\FrmState\c3.h"\
	

"$(INTDIR)\FSOppAction.obj" : $(SOURCE) $(DEP_CPP_FSOPP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSOPP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSOppAction.h"\
	".\goals\OA.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSOppAction.obj" : $(SOURCE) $(DEP_CPP_FSOPP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSOPP=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSOppAction.h"\
	".\goals\OA.h"\
	

"$(INTDIR)\FSOppAction.obj" : $(SOURCE) $(DEP_CPP_FSOPP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FrmState\FSWar.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FSWAR=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSWar.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_FSWAR=\
	".\FrmState\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FSWar.obj" : $(SOURCE) $(DEP_CPP_FSWAR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FSWAR=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSWar.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FSWar.obj" : $(SOURCE) $(DEP_CPP_FSWAR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FSWAR=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\WallClock.h"\
	".\FrmState\FrameState.h"\
	".\FrmState\FSWar.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\FSWar.obj" : $(SOURCE) $(DEP_CPP_FSWAR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\fli.l
USERDEP__FLI_L="$(ProjDir)\fuzzy\y.tab.h"	

!IF  "$(CFG)" == "robotcom - Win32 Release"

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"\
 $(USERDEP__FLI_L)
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

ProjDir=.
InputPath=.\fuzzy\fli.l

"$(ProjDir)\fuzzy\lex.yy.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"\
 $(USERDEP__FLI_L)
	$(CDKDIR)\flex -i -o$(ProjDir)\fuzzy\lex.yy.c $(InputPath)

!ENDIF 

SOURCE=.\fuzzy\fli.y

!IF  "$(CFG)" == "robotcom - Win32 Release"

!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

ProjDir=.
InputPath=.\fuzzy\fli.y

"$(ProjDir)\fuzzy\y.tab.h"	"$(ProjDir)\fuzzy\y.tab.c"	 : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

ProjDir=.
InputPath=.\fuzzy\fli.y

"$(ProjDir)\fuzzy\y.tab.h"	"$(ProjDir)\fuzzy\y.tab.c"	 : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
	$(CDKDIR)\byacc -d -b$(ProjDir)\\fuzzy\\y $(ProjDir)\\fuzzy\\fli.y

!ENDIF 

SOURCE=.\fuzzy\FliAction.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIAC=\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\fuzzy\FliAction.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_FLIAC=\
	".\fuzzy\c3.h"\
	".\fuzzy\PointerList.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\FliAction.obj" : $(SOURCE) $(DEP_CPP_FLIAC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIAC=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\fuzzy\FliAction.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliAction.obj" : $(SOURCE) $(DEP_CPP_FLIAC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIAC=\
	"..\ctp\c3.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\fuzzy\FliAction.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\FliAction.obj" : $(SOURCE) $(DEP_CPP_FLIAC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliEngine.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIEN=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3ErrorReport.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3String.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliAction.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\FliFuncTab.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliRule.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\FliSymTab.h"\
	
NODEP_CPP_FLIEN=\
	".\fuzzy\c3.h"\
	".\fuzzy\PointerList.h"\
	

"$(INTDIR)\FliEngine.obj" : $(SOURCE) $(DEP_CPP_FLIEN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIEN=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3ErrorReport.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3String.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliAction.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\FliFuncTab.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliRule.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\FliSymTab.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliEngine.obj" : $(SOURCE) $(DEP_CPP_FLIEN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIEN=\
	"..\ctp\c3.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3ErrorReport.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3String.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliAction.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\FliFuncTab.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliRule.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\FliSymTab.h"\
	

"$(INTDIR)\FliEngine.obj" : $(SOURCE) $(DEP_CPP_FLIEN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliFuncTab.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIFU=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliFuncTab.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	
NODEP_CPP_FLIFU=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FliFuncTab.obj" : $(SOURCE) $(DEP_CPP_FLIFU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIFU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliFuncTab.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliFuncTab.obj" : $(SOURCE) $(DEP_CPP_FLIFU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIFU=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliFuncTab.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	

"$(INTDIR)\FliFuncTab.obj" : $(SOURCE) $(DEP_CPP_FLIFU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\fliif.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIIF=\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	
NODEP_CPP_FLIIF=\
	".\fuzzy\c3.h"\
	".\fuzzy\PointerList.h"\
	

"$(INTDIR)\fliif.obj" : $(SOURCE) $(DEP_CPP_FLIIF) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIIF=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\fliif.obj" : $(SOURCE) $(DEP_CPP_FLIIF) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIIF=\
	"..\ctp\c3.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	

"$(INTDIR)\fliif.obj" : $(SOURCE) $(DEP_CPP_FLIIF) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliOutputProc.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIOU=\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliSetFunc.h"\
	
NODEP_CPP_FLIOU=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FliOutputProc.obj" : $(SOURCE) $(DEP_CPP_FLIOU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIOU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliSetFunc.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliOutputProc.obj" : $(SOURCE) $(DEP_CPP_FLIOU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIOU=\
	"..\ctp\c3.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliSetFunc.h"\
	

"$(INTDIR)\FliOutputProc.obj" : $(SOURCE) $(DEP_CPP_FLIOU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliRule.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIRU=\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliRule.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliStack.h"\
	".\fuzzy\FliSymbol.h"\
	
NODEP_CPP_FLIRU=\
	".\fuzzy\c3.h"\
	".\fuzzy\PointerList.h"\
	

"$(INTDIR)\FliRule.obj" : $(SOURCE) $(DEP_CPP_FLIRU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIRU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliRule.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliStack.h"\
	".\fuzzy\FliSymbol.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliRule.obj" : $(SOURCE) $(DEP_CPP_FLIRU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIRU=\
	"..\ctp\c3.h"\
	"..\ctp\civ3_utils\pointerlist.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliRule.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliStack.h"\
	".\fuzzy\FliSymbol.h"\
	

"$(INTDIR)\FliRule.obj" : $(SOURCE) $(DEP_CPP_FLIRU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliSetFunc.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLISE=\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	
NODEP_CPP_FLISE=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FliSetFunc.obj" : $(SOURCE) $(DEP_CPP_FLISE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLISE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliSetFunc.obj" : $(SOURCE) $(DEP_CPP_FLISE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLISE=\
	"..\ctp\c3.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	

"$(INTDIR)\FliSetFunc.obj" : $(SOURCE) $(DEP_CPP_FLISE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliStack.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLIST=\
	".\fuzzy\FliStack.h"\
	
NODEP_CPP_FLIST=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FliStack.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLIST=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\FliStack.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliStack.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLIST=\
	"..\ctp\c3.h"\
	".\fuzzy\FliStack.h"\
	

"$(INTDIR)\FliStack.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliSymbol.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLISY=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	
NODEP_CPP_FLISY=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FliSymbol.obj" : $(SOURCE) $(DEP_CPP_FLISY) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLISY=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliSymbol.obj" : $(SOURCE) $(DEP_CPP_FLISY) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLISY=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliOutputProc.h"\
	".\fuzzy\FliSetFunc.h"\
	".\fuzzy\FliSymbol.h"\
	

"$(INTDIR)\FliSymbol.obj" : $(SOURCE) $(DEP_CPP_FLISY) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\FliSymTab.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FLISYM=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\FliSymTab.h"\
	
NODEP_CPP_FLISYM=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FliSymTab.obj" : $(SOURCE) $(DEP_CPP_FLISYM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FLISYM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\FliSymTab.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FliSymTab.obj" : $(SOURCE) $(DEP_CPP_FLISYM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FLISYM=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\slic\StringHash.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\FliSymTab.h"\
	

"$(INTDIR)\FliSymTab.obj" : $(SOURCE) $(DEP_CPP_FLISYM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Fuzzy\FzOut.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_FZOUT=\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Fuzzy\FzOut.h"\
	
NODEP_CPP_FZOUT=\
	".\fuzzy\c3.h"\
	

"$(INTDIR)\FzOut.obj" : $(SOURCE) $(DEP_CPP_FZOUT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_FZOUT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Fuzzy\FzOut.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\FzOut.obj" : $(SOURCE) $(DEP_CPP_FZOUT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_FZOUT=\
	"..\ctp\c3.h"\
	"..\gs\gameobj\DiplomaticTypes.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\fuzzy\FliEngine.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\FliSymbol.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Fuzzy\FzOut.h"\
	

"$(INTDIR)\FzOut.obj" : $(SOURCE) $(DEP_CPP_FZOUT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fuzzy\lex.yy.c

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_LEX_Y=\
	".\fuzzy\fliif.h"\
	".\fuzzy\y.tab.h"\
	
NODEP_CPP_LEX_Y=\
	".\fuzzy\c3debug.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lex.yy.obj" : $(SOURCE) $(DEP_CPP_LEX_Y) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_LEX_Y=\
	"..\ctp\civ3_utils\c3debug.h"\
	".\aimgr\c3ai.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\y.tab.h"\
	
CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lex.yy.obj" : $(SOURCE) $(DEP_CPP_LEX_Y) "$(INTDIR)"\
 ".\fuzzy\y.tab.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_LEX_Y=\
	"..\ctp\civ3_utils\c3debug.h"\
	".\fuzzy\fliif.h"\
	".\fuzzy\y.tab.h"\
	
CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lex.yy.obj" : $(SOURCE) $(DEP_CPP_LEX_Y) "$(INTDIR)"\
 ".\fuzzy\y.tab.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\fuzzy\y.tab.c
DEP_CPP_Y_TAB=\
	".\fuzzy\fliif.h"\
	

!IF  "$(CFG)" == "robotcom - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I\
 "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\civ3\debugtools" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\y.tab.obj" : $(SOURCE) $(DEP_CPP_Y_TAB) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AIDLL" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\y.tab.obj" : $(SOURCE) $(DEP_CPP_Y_TAB) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /Zi /O2 /Ob2 /I "..\robotcom\Top" /I\
 "..\robotcom\Planner" /I "..\gs\slic\\" /I "..\robotcom\Alloc" /I\
 "..\robotcom\Goals" /I "..\robotcom\Fuzzy" /I "..\robotcom\FrmState" /I\
 "..\robotcom\backdoor" /I "..\robotcom\Agent" /I "..\robotcom\AiMgr" /I\
 "..\robotcom\InCom" /I "c:\d5sdk\sdk\inc" /I "..\gs\OutCom" /I "..\net\general"\
 /I "..\net\io" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32\\" /I\
 "..\gfx\layers" /I "..\mm\\" /I "..\ui\interface" /I "..\gs\outcom" /I\
 "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I\
 "..\libs\freetype\\" /I "..\ctp\\" /I "..\ctp\civ3_utils\\" /I\
 "..\ctp\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I\
 "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I\
 "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I\
 "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I\
 "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I\
 "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I\
 "..\ui\freetype\\" /I "..\ctp\debugtools" /D "_AIDLL" /D "_TEST" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /D "DBGAILOG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\y.tab.obj" : $(SOURCE) $(DEP_CPP_Y_TAB) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Goals\ArmyGoal.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_ARMYG=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Goals\ArmyGoal.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_ARMYG=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\ArmyGoal.obj" : $(SOURCE) $(DEP_CPP_ARMYG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_ARMYG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\ArmyGoal.obj" : $(SOURCE) $(DEP_CPP_ARMYG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_ARMYG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Goals\ArmyGoal.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\ArmyGoal.obj" : $(SOURCE) $(DEP_CPP_ARMYG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Goals\GlAttck.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLATT=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GLATT=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlAttck.obj" : $(SOURCE) $(DEP_CPP_GLATT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLATT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlAttck.obj" : $(SOURCE) $(DEP_CPP_GLATT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLATT=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GlAttck.obj" : $(SOURCE) $(DEP_CPP_GLATT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GlBlg.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLBLG=\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLBLG=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlBlg.obj" : $(SOURCE) $(DEP_CPP_GLBLG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLBLG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlBlg.obj" : $(SOURCE) $(DEP_CPP_GLBLG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLBLG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlBlg.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\GlBlg.obj" : $(SOURCE) $(DEP_CPP_GLBLG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GlEnslave.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLENS=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GLENS=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlEnslave.obj" : $(SOURCE) $(DEP_CPP_GLENS) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLENS=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlEnslave.obj" : $(SOURCE) $(DEP_CPP_GLENS) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLENS=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlEnslave.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GLENS=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GlEnslave.obj" : $(SOURCE) $(DEP_CPP_GLENS) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Goals\GlExpl.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLEXP=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlExpl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GLEXP=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlExpl.obj" : $(SOURCE) $(DEP_CPP_GLEXP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLEXP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlExpl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlExpl.obj" : $(SOURCE) $(DEP_CPP_GLEXP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLEXP=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlExpl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GlExpl.obj" : $(SOURCE) $(DEP_CPP_GLEXP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GlFreight.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLFRE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlFreight.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLFRE=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlFreight.obj" : $(SOURCE) $(DEP_CPP_GLFRE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLFRE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlFreight.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlFreight.obj" : $(SOURCE) $(DEP_CPP_GLFRE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLFRE=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GlFreight.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLFRE=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GlFreight.obj" : $(SOURCE) $(DEP_CPP_GLFRE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GlOvertime.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLOVE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLOVE=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlOvertime.obj" : $(SOURCE) $(DEP_CPP_GLOVE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLOVE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlOvertime.obj" : $(SOURCE) $(DEP_CPP_GLOVE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLOVE=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLOVE=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GlOvertime.obj" : $(SOURCE) $(DEP_CPP_GLOVE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GlRoute.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLROU=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlRoute.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLROU=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlRoute.obj" : $(SOURCE) $(DEP_CPP_GLROU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLROU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlRoute.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlRoute.obj" : $(SOURCE) $(DEP_CPP_GLROU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLROU=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3TerrDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\goals\GlRoute.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLROU=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GlRoute.obj" : $(SOURCE) $(DEP_CPP_GLROU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Goals\GlSettl.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLSET=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GLSET=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlSettl.obj" : $(SOURCE) $(DEP_CPP_GLSET) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLSET=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlSettl.obj" : $(SOURCE) $(DEP_CPP_GLSET) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLSET=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GlSettl.obj" : $(SOURCE) $(DEP_CPP_GLSET) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GlTrans.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GLTRA=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLTRA=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GlTrans.obj" : $(SOURCE) $(DEP_CPP_GLTRA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GLTRA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GlTrans.obj" : $(SOURCE) $(DEP_CPP_GLTRA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GLTRA=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GLTRA=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GlTrans.obj" : $(SOURCE) $(DEP_CPP_GLTRA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\Goal_Attack_Region.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Attack_Region.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOAL_=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal_Attack_Region.obj" : $(SOURCE) $(DEP_CPP_GOAL_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Attack_Region.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal_Attack_Region.obj" : $(SOURCE) $(DEP_CPP_GOAL_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Attack_Region.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\Goal_Attack_Region.obj" : $(SOURCE) $(DEP_CPP_GOAL_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\Goal_Build_List.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_B=\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_List.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOAL_B=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal_Build_List.obj" : $(SOURCE) $(DEP_CPP_GOAL_B) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_B=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_List.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal_Build_List.obj" : $(SOURCE) $(DEP_CPP_GOAL_B) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_B=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_List.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\Goal_Build_List.obj" : $(SOURCE) $(DEP_CPP_GOAL_B) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\Goal_Build_Supplemental.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_BU=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOAL_BU=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal_Build_Supplemental.obj" : $(SOURCE) $(DEP_CPP_GOAL_BU)\
 "$(INTDIR)" "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_BU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal_Build_Supplemental.obj" : $(SOURCE) $(DEP_CPP_GOAL_BU)\
 "$(INTDIR)" "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_BU=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\Goal_Build_Supplemental.obj" : $(SOURCE) $(DEP_CPP_GOAL_BU)\
 "$(INTDIR)" "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\Goal_Chokepoint.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_C=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Chokepoint.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOAL_C=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal_Chokepoint.obj" : $(SOURCE) $(DEP_CPP_GOAL_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_C=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Chokepoint.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal_Chokepoint.obj" : $(SOURCE) $(DEP_CPP_GOAL_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_C=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Chokepoint.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\Goal_Chokepoint.obj" : $(SOURCE) $(DEP_CPP_GOAL_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\Goal_Patrol.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_P=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Patrol.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOAL_P=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal_Patrol.obj" : $(SOURCE) $(DEP_CPP_GOAL_P) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_P=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Patrol.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal_Patrol.obj" : $(SOURCE) $(DEP_CPP_GOAL_P) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_P=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Patrol.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\Goal_Patrol.obj" : $(SOURCE) $(DEP_CPP_GOAL_P) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\Goal_Perimeter.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_PE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Perimeter.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOAL_PE=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal_Perimeter.obj" : $(SOURCE) $(DEP_CPP_GOAL_PE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_PE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Perimeter.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal_Perimeter.obj" : $(SOURCE) $(DEP_CPP_GOAL_PE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_PE=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Perimeter.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\Goal_Perimeter.obj" : $(SOURCE) $(DEP_CPP_GOAL_PE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalBombard.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALB=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalBombard.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALB=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalBombard.obj" : $(SOURCE) $(DEP_CPP_GOALB) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALB=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalBombard.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalBombard.obj" : $(SOURCE) $(DEP_CPP_GOALB) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALB=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalBombard.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalBombard.obj" : $(SOURCE) $(DEP_CPP_GOALB) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalCityTarget.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALC=\
	"..\gs\gameobj\Agreement.h"\
	"..\gs\gameobj\AgreementData.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\Gold.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\Unit.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalCityTarget.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALC=\
	"..\gs\gameobj\c3types.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalCityTarget.obj" : $(SOURCE) $(DEP_CPP_GOALC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALC=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Agreement.h"\
	"..\gs\gameobj\AgreementData.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\Gold.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\Unit.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalCityTarget.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalCityTarget.obj" : $(SOURCE) $(DEP_CPP_GOALC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALC=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Agreement.h"\
	"..\gs\gameobj\AgreementData.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\Gold.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\Unit.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalCityTarget.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalCityTarget.obj" : $(SOURCE) $(DEP_CPP_GOALC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalDefense.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALD=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalDefense.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALD=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalDefense.obj" : $(SOURCE) $(DEP_CPP_GOALD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALD=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalDefense.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalDefense.obj" : $(SOURCE) $(DEP_CPP_GOALD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALD=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalDefense.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalDefense.obj" : $(SOURCE) $(DEP_CPP_GOALD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalExpel.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalExpel.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALE=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalExpel.obj" : $(SOURCE) $(DEP_CPP_GOALE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalExpel.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalExpel.obj" : $(SOURCE) $(DEP_CPP_GOALE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALE=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalExpel.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalExpel.obj" : $(SOURCE) $(DEP_CPP_GOALE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalInst.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALI=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_GOALI=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalInst.obj" : $(SOURCE) $(DEP_CPP_GOALI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALI=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalInst.obj" : $(SOURCE) $(DEP_CPP_GOALI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALI=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_GOALI=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GoalInst.obj" : $(SOURCE) $(DEP_CPP_GOALI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalMapTarget.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALM=\
	"..\gs\gameobj\Agreement.h"\
	"..\gs\gameobj\AgreementData.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\Gold.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\Unit.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalMapTarget.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALM=\
	"..\gs\gameobj\c3types.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalMapTarget.obj" : $(SOURCE) $(DEP_CPP_GOALM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Agreement.h"\
	"..\gs\gameobj\AgreementData.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\Gold.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\Unit.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalMapTarget.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalMapTarget.obj" : $(SOURCE) $(DEP_CPP_GOALM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Agreement.h"\
	"..\gs\gameobj\AgreementData.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\Gold.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\Unit.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalMapTarget.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalMapTarget.obj" : $(SOURCE) $(DEP_CPP_GOALM) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalRetreat.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALR=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalRetreat.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALR=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalRetreat.obj" : $(SOURCE) $(DEP_CPP_GOALR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALR=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalRetreat.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalRetreat.obj" : $(SOURCE) $(DEP_CPP_GOALR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALR=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalRetreat.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalRetreat.obj" : $(SOURCE) $(DEP_CPP_GOALR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalRoad.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALRO=\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\TerrImproveData.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalRoad.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOALRO=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalRoad.obj" : $(SOURCE) $(DEP_CPP_GOALRO) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALRO=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\TerrImproveData.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalRoad.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalRoad.obj" : $(SOURCE) $(DEP_CPP_GOALRO) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALRO=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Gameobj.h"\
	"..\gs\gameobj\ID.h"\
	"..\gs\gameobj\TerrImproveData.h"\
	"..\gs\OutCom\AiCause.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\ErrMsg.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GoalInst.h"\
	".\goals\GoalRoad.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOALRO=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\GoalRoad.obj" : $(SOURCE) $(DEP_CPP_GOALRO) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalRustle.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALRU=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalRustle.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_GOALRU=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalRustle.obj" : $(SOURCE) $(DEP_CPP_GOALRU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALRU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalRustle.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalRustle.obj" : $(SOURCE) $(DEP_CPP_GOALRU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALRU=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\airndcnt.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalRustle.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	

"$(INTDIR)\GoalRustle.obj" : $(SOURCE) $(DEP_CPP_GOALRU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\GoalWonder.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOALW=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOALW=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\GoalWonder.obj" : $(SOURCE) $(DEP_CPP_GOALW) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOALW=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\GoalWonder.obj" : $(SOURCE) $(DEP_CPP_GOALW) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOALW=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\GoalWonder.obj" : $(SOURCE) $(DEP_CPP_GOALW) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\OA.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_OA_CP=\
	".\goals\OA.h"\
	
NODEP_CPP_OA_CP=\
	".\Goals\c3.h"\
	

"$(INTDIR)\OA.obj" : $(SOURCE) $(DEP_CPP_OA_CP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_OA_CP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\goals\OA.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\OA.obj" : $(SOURCE) $(DEP_CPP_OA_CP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_OA_CP=\
	"..\ctp\c3.h"\
	".\goals\OA.h"\
	

"$(INTDIR)\OA.obj" : $(SOURCE) $(DEP_CPP_OA_CP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\OA_Entrench.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_OA_EN=\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\goals\OA.h"\
	".\goals\OA_Entrench.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_OA_EN=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\OA_Entrench.obj" : $(SOURCE) $(DEP_CPP_OA_EN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_OA_EN=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\OA.h"\
	".\goals\OA_Entrench.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\OA_Entrench.obj" : $(SOURCE) $(DEP_CPP_OA_EN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_OA_EN=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\goals\OA.h"\
	".\goals\OA_Entrench.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\OA_Entrench.obj" : $(SOURCE) $(DEP_CPP_OA_EN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\oa_loot.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_OA_LO=\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\goals\OA.h"\
	".\goals\oa_loot.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_OA_LO=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\oa_loot.obj" : $(SOURCE) $(DEP_CPP_OA_LO) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_OA_LO=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\OA.h"\
	".\goals\oa_loot.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\oa_loot.obj" : $(SOURCE) $(DEP_CPP_OA_LO) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_OA_LO=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\gameobj\Order.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\goals\OA.h"\
	".\goals\oa_loot.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\oa_loot.obj" : $(SOURCE) $(DEP_CPP_OA_LO) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\goals\OA_Refuel.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_OA_RE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\goals\OA.h"\
	".\goals\OA_Refuel.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	
NODEP_CPP_OA_RE=\
	".\Goals\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\OA_Refuel.obj" : $(SOURCE) $(DEP_CPP_OA_RE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_OA_RE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\OA.h"\
	".\goals\OA_Refuel.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\OA_Refuel.obj" : $(SOURCE) $(DEP_CPP_OA_RE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_OA_RE=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\goals\OA.h"\
	".\goals\OA_Refuel.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	

"$(INTDIR)\OA_Refuel.obj" : $(SOURCE) $(DEP_CPP_OA_RE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\AllocWGF.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_ALLOC=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_ALLOC=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\AllocWGF.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_ALLOC=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AllocWGF.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_ALLOC=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Population.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_ALLOC=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\AllocWGF.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\CityFP.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYF=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYF=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityFP.obj" : $(SOURCE) $(DEP_CPP_CITYF) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYF=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityFP.obj" : $(SOURCE) $(DEP_CPP_CITYF) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYF=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityFP.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYF=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityFP.obj" : $(SOURCE) $(DEP_CPP_CITYF) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\CityGrowth.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYG=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYG=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityGrowth.obj" : $(SOURCE) $(DEP_CPP_CITYG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYG=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityGrowth.obj" : $(SOURCE) $(DEP_CPP_CITYG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYG=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityGrowth.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYG=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityGrowth.obj" : $(SOURCE) $(DEP_CPP_CITYG) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\CityList.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYL=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYL=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityList.obj" : $(SOURCE) $(DEP_CPP_CITYL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYL=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityList.obj" : $(SOURCE) $(DEP_CPP_CITYL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYL=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYL=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityList.obj" : $(SOURCE) $(DEP_CPP_CITYL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\CityProd.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYP=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYP=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\CityProd.obj" : $(SOURCE) $(DEP_CPP_CITYP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityProd.obj" : $(SOURCE) $(DEP_CPP_CITYP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYP=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\CityList.h"\
	".\Alloc\CityProd.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_CITYP=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityProd.obj" : $(SOURCE) $(DEP_CPP_CITYP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\InstBidList.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_INSTB=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_INSTB=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\InstBidList.obj" : $(SOURCE) $(DEP_CPP_INSTB) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_INSTB=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\InstBidList.obj" : $(SOURCE) $(DEP_CPP_INSTB) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_INSTB=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\goals\GoalInst.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_INSTB=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\InstBidList.obj" : $(SOURCE) $(DEP_CPP_INSTB) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\OTUtilityList.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_OTUTI=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_OTUTI=\
	".\Alloc\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\OTUtilityList.obj" : $(SOURCE) $(DEP_CPP_OTUTI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_OTUTI=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\OTUtilityList.obj" : $(SOURCE) $(DEP_CPP_OTUTI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_OTUTI=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\OTUtilityList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\goals\GlOvertime.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\OTUtilityList.obj" : $(SOURCE) $(DEP_CPP_OTUTI) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\Plan.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_PLAN_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_PLAN_=\
	".\Alloc\c3.h"\
	".\Alloc\c3errors.h"\
	".\Alloc\c3types.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Plan.obj" : $(SOURCE) $(DEP_CPP_PLAN_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_PLAN_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\list_array.h"\
	".\backdoor\pool.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Plan.obj" : $(SOURCE) $(DEP_CPP_PLAN_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_PLAN_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_PLAN_=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\Plan.obj" : $(SOURCE) $(DEP_CPP_PLAN_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\RouteUtilityList.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_ROUTE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\goals\GlRoute.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_ROUTE=\
	".\Alloc\c3.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\RouteUtilityList.obj" : $(SOURCE) $(DEP_CPP_ROUTE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_ROUTE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\goals\GlRoute.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\RouteUtilityList.obj" : $(SOURCE) $(DEP_CPP_ROUTE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_ROUTE=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\Alloc\RouteUtilityList.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\goals\GlRoute.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\RouteUtilityList.obj" : $(SOURCE) $(DEP_CPP_ROUTE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Alloc\WotP.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_WOTP_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_WOTP_=\
	".\Alloc\c3.h"\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\WotP.obj" : $(SOURCE) $(DEP_CPP_WOTP_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_WOTP_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\WotP.obj" : $(SOURCE) $(DEP_CPP_WOTP_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_WOTP_=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Government.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Gold.h"\
	".\Alloc\AllocWGF.h"\
	".\Alloc\Plan.h"\
	".\Alloc\WotP.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Fuzzy\FzOut.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_WOTP_=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\WotP.obj" : $(SOURCE) $(DEP_CPP_WOTP_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\AI_Match.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AI_MA=\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\Planner\AI_Match.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_AI_MA=\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\AI_Match.obj" : $(SOURCE) $(DEP_CPP_AI_MA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AI_MA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\common.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AI_Match.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AI_Match.obj" : $(SOURCE) $(DEP_CPP_AI_MA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AI_MA=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\Planner\AI_Match.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	

"$(INTDIR)\AI_Match.obj" : $(SOURCE) $(DEP_CPP_AI_MA) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\aip.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AIP_C=\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_List.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_AIP_C=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\aip.obj" : $(SOURCE) $(DEP_CPP_AIP_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AIP_C=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_List.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\aip.obj" : $(SOURCE) $(DEP_CPP_AIP_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AIP_C=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3BlgDB.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Science.h"\
	"..\gs\OutCom\IC3UnitDB.h"\
	"..\gs\OutCom\IC3Wonder.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\WonderTbl.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\goals\Goal_Build_List.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_AIP_C=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\aip.obj" : $(SOURCE) $(DEP_CPP_AIP_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\Expression.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_EXPRE=\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Planner\ExpressionP.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	
NODEP_CPP_EXPRE=\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	

"$(INTDIR)\Expression.obj" : $(SOURCE) $(DEP_CPP_EXPRE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_EXPRE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\common.h"\
	".\Planner\ExpressionP.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Expression.obj" : $(SOURCE) $(DEP_CPP_EXPRE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_EXPRE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Planner\ExpressionP.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	

"$(INTDIR)\Expression.obj" : $(SOURCE) $(DEP_CPP_EXPRE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\Goal.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_GOAL_CP=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOAL_CP=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Goal.obj" : $(SOURCE) $(DEP_CPP_GOAL_CP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_GOAL_CP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Goal.obj" : $(SOURCE) $(DEP_CPP_GOAL_CP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_GOAL_CP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_GOAL_CP=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\Goal.obj" : $(SOURCE) $(DEP_CPP_GOAL_CP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\IParser.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_IPARS=\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Planner\AILog.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\ExpressionP.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	
NODEP_CPP_IPARS=\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\IParser.obj" : $(SOURCE) $(DEP_CPP_IPARS) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_IPARS=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\common.h"\
	".\Planner\AILog.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\ExpressionP.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\IParser.obj" : $(SOURCE) $(DEP_CPP_IPARS) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_IPARS=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\Planner\AILog.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\ExpressionP.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	

"$(INTDIR)\IParser.obj" : $(SOURCE) $(DEP_CPP_IPARS) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\linked_list.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_LINKE=\
	".\Planner\linked_list.h"\
	
NODEP_CPP_LINKE=\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	

"$(INTDIR)\linked_list.obj" : $(SOURCE) $(DEP_CPP_LINKE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_LINKE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\common.h"\
	".\Planner\linked_list.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\linked_list.obj" : $(SOURCE) $(DEP_CPP_LINKE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_LINKE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	".\Planner\linked_list.h"\
	

"$(INTDIR)\linked_list.obj" : $(SOURCE) $(DEP_CPP_LINKE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\Planner.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_PLANN=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlBlg.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\Goal_Attack_Region.h"\
	".\goals\Goal_Build_List.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\goals\Goal_Patrol.h"\
	".\goals\Goal_Perimeter.h"\
	".\goals\GoalBombard.h"\
	".\goals\GoalCityTarget.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalExpel.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\GoalRetreat.h"\
	".\goals\GoalRustle.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_PLANN=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Planner.obj" : $(SOURCE) $(DEP_CPP_PLANN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_PLANN=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\list_array.h"\
	".\backdoor\pool.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlBlg.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\Goal_Attack_Region.h"\
	".\goals\Goal_Build_List.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\goals\Goal_Patrol.h"\
	".\goals\Goal_Perimeter.h"\
	".\goals\GoalBombard.h"\
	".\goals\GoalCityTarget.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalExpel.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\GoalRetreat.h"\
	".\goals\GoalRustle.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Planner.obj" : $(SOURCE) $(DEP_CPP_PLANN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_PLANN=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\goals\GlBlg.h"\
	".\goals\GlEnslave.h"\
	".\Goals\GlExpl.h"\
	".\goals\GlFreight.h"\
	".\Goals\GlSettl.h"\
	".\goals\GlTrans.h"\
	".\goals\Goal_Attack_Region.h"\
	".\goals\Goal_Build_List.h"\
	".\goals\Goal_Build_Supplemental.h"\
	".\goals\Goal_Patrol.h"\
	".\goals\Goal_Perimeter.h"\
	".\goals\GoalBombard.h"\
	".\goals\GoalCityTarget.h"\
	".\goals\GoalDefense.h"\
	".\goals\GoalExpel.h"\
	".\goals\GoalMapTarget.h"\
	".\goals\GoalRetreat.h"\
	".\goals\GoalRustle.h"\
	".\goals\GoalWonder.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_PLANN=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\Planner.obj" : $(SOURCE) $(DEP_CPP_PLANN) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\scheduler.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_SCHED=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\WallClock.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_SCHED=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\scheduler.obj" : $(SOURCE) $(DEP_CPP_SCHED) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_SCHED=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\list_array.h"\
	".\backdoor\pool.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\backdoor\WallClock.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\scheduler.obj" : $(SOURCE) $(DEP_CPP_SCHED) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_SCHED=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\WallClock.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_SCHED=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\scheduler.obj" : $(SOURCE) $(DEP_CPP_SCHED) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Planner\Squad.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_SQUAD=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Planner\AI_Match.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_SQUAD=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\c3.h"\
	".\Planner\c3errors.h"\
	".\Planner\c3types.h"\
	".\Planner\DebugMemory.h"\
	

"$(INTDIR)\Squad.obj" : $(SOURCE) $(DEP_CPP_SQUAD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_SQUAD=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Planner\AI_Match.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Squad.obj" : $(SOURCE) $(DEP_CPP_SQUAD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_SQUAD=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\GoodAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\InstBidList.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlAttck.h"\
	".\Planner\AI_Match.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\planner\Squad_Strength.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	
NODEP_CPP_SQUAD=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\Squad.obj" : $(SOURCE) $(DEP_CPP_SQUAD) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\planner\Squad_Strength.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_SQUAD_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\planner\Squad_Strength.h"\
	
NODEP_CPP_SQUAD_=\
	".\Planner\c3.h"\
	

"$(INTDIR)\Squad_Strength.obj" : $(SOURCE) $(DEP_CPP_SQUAD_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_SQUAD_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\planner\Squad_Strength.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Squad_Strength.obj" : $(SOURCE) $(DEP_CPP_SQUAD_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_SQUAD_=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\planner\Squad_Strength.h"\
	

"$(INTDIR)\Squad_Strength.obj" : $(SOURCE) $(DEP_CPP_SQUAD_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\AiCell.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AICEL=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\Avl.h"\
	".\backdoor\CivArchive.h"\
	".\Fuzzy\FzOut.h"\
	".\Top\AiCell.h"\
	
NODEP_CPP_AICEL=\
	".\Top\c3.h"\
	

"$(INTDIR)\AiCell.obj" : $(SOURCE) $(DEP_CPP_AICEL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AICEL=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\Avl.h"\
	".\backdoor\CivArchive.h"\
	".\Fuzzy\FzOut.h"\
	".\Top\AiCell.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AiCell.obj" : $(SOURCE) $(DEP_CPP_AICEL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AICEL=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\Avl.h"\
	".\backdoor\CivArchive.h"\
	".\Fuzzy\FzOut.h"\
	".\Top\AiCell.h"\
	

"$(INTDIR)\AiCell.obj" : $(SOURCE) $(DEP_CPP_AICEL) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\AiMap.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_AIMAP=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	
NODEP_CPP_AIMAP=\
	".\Planner\DebugMemory.h"\
	".\Top\c3.h"\
	

"$(INTDIR)\AiMap.obj" : $(SOURCE) $(DEP_CPP_AIMAP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_AIMAP=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\AiMap.obj" : $(SOURCE) $(DEP_CPP_AIMAP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_AIMAP=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\Fuzzy\FzOut.h"\
	".\Goals\ArmyGoal.h"\
	".\Goals\GlSettl.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\cont.h"\
	

"$(INTDIR)\AiMap.obj" : $(SOURCE) $(DEP_CPP_AIMAP) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\CityEdge.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYE=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\CityEdge.h"\
	".\Top\CityVertex.h"\
	
NODEP_CPP_CITYE=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	".\Top\c3.h"\
	".\Top\c3errors.h"\
	".\Top\c3types.h"\
	

"$(INTDIR)\CityEdge.obj" : $(SOURCE) $(DEP_CPP_CITYE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\CityEdge.h"\
	".\Top\CityVertex.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityEdge.obj" : $(SOURCE) $(DEP_CPP_CITYE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYE=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\CityEdge.h"\
	".\Top\CityVertex.h"\
	
NODEP_CPP_CITYE=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityEdge.obj" : $(SOURCE) $(DEP_CPP_CITYE) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\CityTree.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYT=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityVertex.h"\
	
NODEP_CPP_CITYT=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Top\c3.h"\
	

"$(INTDIR)\CityTree.obj" : $(SOURCE) $(DEP_CPP_CITYT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityVertex.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityTree.obj" : $(SOURCE) $(DEP_CPP_CITYT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYT=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\utility\globals.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityVertex.h"\
	
NODEP_CPP_CITYT=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityTree.obj" : $(SOURCE) $(DEP_CPP_CITYT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\CityTreeQueue.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYTR=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Top\CityTree.h"\
	".\Top\CityTreeQueue.h"\
	
NODEP_CPP_CITYTR=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Top\c3.h"\
	

"$(INTDIR)\CityTreeQueue.obj" : $(SOURCE) $(DEP_CPP_CITYTR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYTR=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Top\CityTree.h"\
	".\Top\CityTreeQueue.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityTreeQueue.obj" : $(SOURCE) $(DEP_CPP_CITYTR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYTR=\
	"..\ctp\c3.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\utility\globals.h"\
	".\aimgr\AiMain.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Top\CityTree.h"\
	".\Top\CityTreeQueue.h"\
	
NODEP_CPP_CITYTR=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityTreeQueue.obj" : $(SOURCE) $(DEP_CPP_CITYTR) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\CityVertex.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CITYV=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityVertex.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	
NODEP_CPP_CITYV=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	".\Top\c3.h"\
	

"$(INTDIR)\CityVertex.obj" : $(SOURCE) $(DEP_CPP_CITYV) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CITYV=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityVertex.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\CityVertex.obj" : $(SOURCE) $(DEP_CPP_CITYV) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CITYV=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Alloc\Plan.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityVertex.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	
NODEP_CPP_CITYV=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\CityVertex.obj" : $(SOURCE) $(DEP_CPP_CITYV) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\cont.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_CONT_=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\CityTree.h"\
	".\Top\cont.h"\
	
NODEP_CPP_CONT_=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	".\Top\c3.h"\
	

"$(INTDIR)\cont.obj" : $(SOURCE) $(DEP_CPP_CONT_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_CONT_=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\CityTree.h"\
	".\Top\cont.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\cont.obj" : $(SOURCE) $(DEP_CPP_CONT_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_CONT_=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiCell.h"\
	".\Top\AiMap.h"\
	".\Top\CityTree.h"\
	".\Top\cont.h"\
	
NODEP_CPP_CONT_=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\cont.obj" : $(SOURCE) $(DEP_CPP_CONT_) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\delaunay.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_DELAU=\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	
NODEP_CPP_DELAU=\
	".\Top\c3.h"\
	

"$(INTDIR)\delaunay.obj" : $(SOURCE) $(DEP_CPP_DELAU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_DELAU=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\aimgr\c3ai.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\delaunay.obj" : $(SOURCE) $(DEP_CPP_DELAU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_DELAU=\
	"..\ctp\c3.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	

"$(INTDIR)\delaunay.obj" : $(SOURCE) $(DEP_CPP_DELAU) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\MST.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_MST_C=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\FrmState\FrameState.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityTreeQueue.h"\
	".\Top\CityVertex.h"\
	".\Top\cont.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	".\Top\MST.h"\
	
NODEP_CPP_MST_C=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	".\Top\c3.h"\
	

"$(INTDIR)\MST.obj" : $(SOURCE) $(DEP_CPP_MST_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_MST_C=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\FrmState\FrameState.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityTreeQueue.h"\
	".\Top\CityVertex.h"\
	".\Top\cont.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	".\Top\MST.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\MST.obj" : $(SOURCE) $(DEP_CPP_MST_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_MST_C=\
	"..\ctp\c3.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3InstDB.h"\
	"..\gs\OutCom\IC3RobotAstar.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ScienceAgent.h"\
	".\aimgr\AiMain.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\FrmState\FrameState.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\CityEdge.h"\
	".\Top\CityTree.h"\
	".\Top\CityTreeQueue.h"\
	".\Top\CityVertex.h"\
	".\Top\cont.h"\
	".\Top\delaunay.h"\
	".\Top\geom2d.h"\
	".\Top\MST.h"\
	
NODEP_CPP_MST_C=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\MST.obj" : $(SOURCE) $(DEP_CPP_MST_C) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Top\Strategic_Map.cpp

!IF  "$(CFG)" == "robotcom - Win32 Release"

DEP_CPP_STRAT=\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_STRAT=\
	".\backdoor\c3.h"\
	".\backdoor\Dummy.h"\
	".\Planner\DebugMemory.h"\
	".\Top\c3.h"\
	".\Top\c3errors.h"\
	".\Top\c3types.h"\
	

"$(INTDIR)\Strategic_Map.obj" : $(SOURCE) $(DEP_CPP_STRAT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Debug"

DEP_CPP_STRAT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\c3ai.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\common.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\backdoor\semi_dynamic_array.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\Strategic_Map.obj" : $(SOURCE) $(DEP_CPP_STRAT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "robotcom - Win32 Test"

DEP_CPP_STRAT=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\ctp\debugtools\DebugMemory.h"\
	"..\gs\OutCom\IC3CivArchive.h"\
	"..\gs\OutCom\IC3GameState.h"\
	"..\gs\OutCom\IC3Player.h"\
	"..\gs\OutCom\IC3Rand.h"\
	"..\gs\OutCom\IC3World.h"\
	"..\gs\OutCom\IMapPointData.h"\
	"..\gs\utility\globals.h"\
	"..\gs\world\MapPoint.h"\
	".\agent\Agent.h"\
	".\Agent\ArmyAgent.h"\
	".\Agent\CityAgent.h"\
	".\Agent\CityAgentBlg.h"\
	".\Agent\CityAgentPop.h"\
	".\Agent\ForeignAgent.h"\
	".\Agent\ForeignCity.h"\
	".\aimgr\AiMain.h"\
	".\aimgr\Foreigner.h"\
	".\Alloc\Plan.h"\
	".\backdoor\Avl.h"\
	".\backdoor\BSet.h"\
	".\backdoor\CivArchive.h"\
	".\backdoor\DynArr.h"\
	".\backdoor\FlatPtr.h"\
	".\fuzzy\Fuzzy_Variables.h"\
	".\Planner\AILog.h"\
	".\Planner\aip.h"\
	".\Planner\DR_Debug.h"\
	".\Planner\Goal.h"\
	".\Planner\Grassfire.h"\
	".\Planner\IParser.h"\
	".\Planner\linked_list.h"\
	".\Planner\matrix.h"\
	".\Planner\Planner.h"\
	".\Planner\Scheduler.h"\
	".\Planner\sorted_array.h"\
	".\Planner\Squad.h"\
	".\Planner\Strategic_AI_Forward.h"\
	".\Planner\Unitflow.h"\
	".\Top\AiMap.h"\
	".\Top\Strategic_Map.h"\
	
NODEP_CPP_STRAT=\
	".\backdoor\Dummy.h"\
	

"$(INTDIR)\Strategic_Map.obj" : $(SOURCE) $(DEP_CPP_STRAT) "$(INTDIR)"\
 "$(INTDIR)\robotcom.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

