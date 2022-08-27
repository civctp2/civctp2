//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source file
// Description  :
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG
// - Generates debug information when set.
//
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
// _PLAYTEST
// DUMP_ASTAR
// _DEBUG_MEMORY
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed non-standard include file <iostream.h>.
// - Standardised min/max usage.
// - Replaced old civilisation database by new one. (Aug 20th 2005 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#ifdef _PLAYTEST
#include "c3cmdline.h"

#include "CivilisationRecord.h"
#include "ConstRecord.h"
#include "c3ui.h"
#include "debugmemory.h"
#include "log.h"

#include "player.h"
#include "Strengths.h"
#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "background.h"
#include "statuswindow.h"
#include "civ3_main.h"
#include "DataCheck.h"
#include "MapPoint.h"
#include "TradePool.h"
#include "UnitData.h"
#include "citydata.h"
#include "World.h"
#include "TerrImprove.h"
#include "Readiness.h"
#include "gamefile.h"
#include "TradeOffer.h"
#include "Cell.h"
#include "MaterialPool.h"
#include "Advances.h"
#include "AdvanceRecord.h"
#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "director.h"
#include "maputils.h"
#include "Regard.h"

#include "profileDB.h"
#include "Order.h"

#include "SlicObject.h"
#include "SlicEngine.h"
#include "statswindow.h"
#include "controlpanelwindow.h"
#include "chatbox.h"

#include "Score.h"
#include "Happy.h"

#include "debugwindow.h"

#include "SlicSymbol.h"
#include "SlicNamedSymbol.h"
#include "sliccmd.h"
/** \todo Unique header names */
#ifndef WIN32
#include "ysc.tab.h"
#else
#include "sc.tab.h"
#endif

#include "watchlist.h"

#include "UnitRecord.h"
#include "DBLexer.h"
#include "TerrainRecord.h"
#include "ResourceRecord.h"

#include "GoalRecord.h"
#include "UnitBuildListRecord.h"
#include "WonderBuildListRecord.h"
#include "BuildingBuildListRecord.h"
#include "ImprovementListRecord.h"
#include "StrategyRecord.h"
#include "BuildListSequenceRecord.h"
#include "DiplomacyRecord.h"
#include "AdvanceListRecord.h"
#include "PersonalityRecord.h"

#include "wonderutil.h"

#include "Diplomat.h"
#include "AgreementMatrix.h"

#include "Globals.h"
#include "cellunitlist.h"
#include "ctpaidebug.h"

#include "gfx_options.h"

extern DebugWindow *g_debugWindow;

extern MBCHAR g_stringdb_filename[_MAX_PATH];
extern MBCHAR g_advancedb_filename[_MAX_PATH];
extern MBCHAR g_terrain_filename[_MAX_PATH];
extern MBCHAR g_unitdb_filename[_MAX_PATH];
extern MBCHAR g_constdb_filename[_MAX_PATH];
extern MBCHAR g_improve_filename[_MAX_PATH];
extern MBCHAR g_wonder_filename[_MAX_PATH];
extern MBCHAR g_installation_filename[_MAX_PATH];
extern MBCHAR g_civilisation_filename[_MAX_PATH];
extern MBCHAR g_pop_filename[_MAX_PATH];

extern MBCHAR g_goal_db_filename[_MAX_PATH];
extern MBCHAR g_personality_db_filename[_MAX_PATH];
extern MBCHAR g_strategy_db_filename[_MAX_PATH];
extern MBCHAR g_buildlist_sequence_db_filename[_MAX_PATH];
extern MBCHAR g_unit_buildlist_db_filename[_MAX_PATH];
extern MBCHAR g_wonder_buildlist_db_filename[_MAX_PATH];
extern MBCHAR g_building_buildlist_db_filename[_MAX_PATH];
extern MBCHAR g_improvement_list_db_filename[_MAX_PATH];
extern MBCHAR g_diplomacy_db_filename[_MAX_PATH];
extern MBCHAR g_advance_list_db_filename[_MAX_PATH];

extern Background           *g_background;
extern ChatBox              *g_chatBox;
extern StatsWindow          *g_statsWindow;
extern ControlPanelWindow   *g_controlPanel;
extern C3UI                 *g_c3ui;

extern sint32               g_check_mem;
extern sint32               g_robotMessages;

#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#include "tiledmap.h"

#include "SelItem.h"
#include "network.h"
#include "net_chat.h"
#include "net_cheat.h"

#include "Readiness.h"
#include "Agreement.h"
#include "DiplomaticRequestData.h"
#include "DiplomaticRequest.h"
#include "Actor.h"
#include "UnitActor.h"
#include "SpriteState.h"
#include "SpriteFile.h"

#include "gameinit.h"
#include "AICause.h"
#include "TurnCnt.h"

#include "civapp.h"

#include "AttractWindow.h"
#include "screenutils.h"

#include "UnitPool.h"

#include "MapFile.h"

#include "GameEventManager.h"

extern TurnCount *g_turn;

static sint32 s_helpLines = 15;

#define k_HELP_LINES s_helpLines

extern Player**       g_player;
extern StatusWindow*  g_statusWindow;
extern sint32         g_debugOwner;
extern SelectedItem   *g_selected_item;
extern World          *g_theWorld;
extern TiledMap       *g_tiledMap;
extern UnitPool       *g_theUnitPool;

extern sint32         g_fog_toggle;

extern BOOL           g_smoothScroll;

extern sint32         g_god;

extern sint32         g_isGridOn;
extern bool           g_showHeralds;

extern BOOL           g_useDDBlit;

extern CivApp         *g_civApp;

extern C3UI           *g_c3ui;

extern BOOL           g_powerPointsMode;

#if defined(_DEBUG) && defined(CELL_COLOR)
sint32                g_is_debug_map_color;
#endif
BOOL                  g_show_ai_dbg;
bool                  g_doingFastRounds = false;

extern BOOL           g_ai_revolt = TRUE;

extern void WhackScreen();

#include "Diplomacy_Log.h"
extern Diplomacy_Log *g_theDiplomacyLog;

#include "director.h"
extern Director *g_director;

extern BOOL	g_drawArmyClumps;

#include "sciencewin.h"
#include "greatlibrary.h"
#include "infowin.h"
#include "optionswindow.h"
#include "km_screen.h"
#include "NationalManagementDialog.h"
#include "ScienceManagementDialog.h"

CommandLine g_commandLine;

                                      ZBCommand g_zbCommand;
                                     PopCommand g_popCommand;
                                     TaxCommand g_taxCommand;
                                     SueCommand g_sueCommand;
                                     PacCommand g_PacCommand;
                                    ChatCommand g_chatCommand;
                                    SaveCommand g_saveCommand;
                                    HelpCommand g_helpCommand;
                                    UpgradeCity g_upgradeCity;
                                   BuildCommand g_buildCommand;
                                   OfferCommand g_offerCommand;
                                   FloodCommand g_floodCommand;
                                   OzoneCommand g_ozoneCommand;
                                   FrameCommand g_frameCommand;
                                   CloakCommand g_cloakCommand;
                                   ExpelCommand g_expelCommand;
                                   LogAICommand g_logAICommand;
                                  CreateCommand g_createCommand;
                                  RevoltCommand g_revoltCommand;
                                  SeeWWRCommand g_SeeWWRCommand;
                                  RustleCommand g_rustleCommand;
                                  StayOnCommand g_stayOnCommand;
                                 UncloakCommand g_uncloakCommand;
                                 BombardCommand g_bombardCommand;
                                 RestoreCommand g_restoreCommand;
                                 RestartCommand g_restartCommand;
                                 ImproveCommand g_improveCommand;
                                 TurnOffCommand g_turnOffCommand;
                                 GiveMapCommand g_giveMapCommand;
                                 LoadAIPCommand g_loadAIPCommand;
                                ChatMaskCommand g_chatMaskCommand;
                                RandTestCommand g_randTestCommand;
                                GiveCityCommand g_giveCityCommand;
                                GiveGoldCommand g_giveGoldCommand;
                                KillTileCommand g_killTileCommand;
                                GiveUnitCommand g_giveUnitCommand;
                                SetWagesCommand g_SetWagesCommand;
                                TileTypeCommand g_tileTypeCommand;
                                SoothsayCommand g_soothsayCommand;
                               BuildWhatCommand g_buildWhatCommand;
                               FastRoundCommand g_fastRoundCommand;
                               ClearTextCommand g_clearTextCommand;
                               DatacheckCommand g_datacheckCommand;
                               InterceptCommand g_interceptCommand;
                               ReadinessCommand g_readinessCommand;
                               DebugMaskCommand g_debugMaskCommand;
                               ShowOwnerCommand g_showOwnerCommand;
                               ToggleFogCommand g_toggleFogCommand;
                               PlantNukeCommand g_plantNukeCommand;
                               FranchiseCommand g_franchiseCommand;
                               SlaveRaidCommand g_slaveRaidCommand;
                               SpewUnitsCommand g_spewUnitsCommand;
                               BioInfectCommand g_bioInfectCommand;
                               HelpLinesCommand g_helpLinesCommand;
                              NanoInfectCommand g_nanoInfectCommand;
                              DumpAlliesCommand g_dumpAlliesCommand;
                              TradeRouteCommand g_tradeRouteCommand;
                              HearGossipCommand g_hearGossipCommand;
                              SetWorkdayCommand g_SetWorkdayCommand;
                              SetRationsCommand g_SetRationsCommand;
                              ShowOffersCommand g_showOffersCommand;
                              ToggleGridCommand g_toggleGridCommand;
                              IndulgenceCommand g_indulgenceCommand;
                              ReformCityCommand g_reformCityCommand;
                              CreateParkCommand g_createParkCommand;
                              CreateRiftCommand g_createRiftCommand;

                             BuildWonderCommand g_buildWonderCommand;
                             GiveAdvanceCommand g_giveAdvanceCommand;
                             ToggleWaterCommand g_toggleWaterCommand;
                             ToggleSpaceCommand g_toggleSpaceCommand;
                             SetCityNameCommand g_setCityNameCommand;
                             SetCitySizeCommand g_setCitySizeCommand;
                                    ToggleAIStr g_toggleAiStr;
                             ExchangeMapCommand g_exchangeMapCommand;
                             ConvertCityCommand g_convertCityCommand;
                             SetAIRevoltCommand g_setAIRevoltCommand;
                            NullifyWallsCommand g_nullifyWallsCommand;
                            SetReadinessCommand g_SetReadinessCommand;
                            ToggleHeraldCommand g_toggleHeraldCommand;
                            CalcChecksumCommand g_calcChecksumCommand;
                            ExchangeCityCommand g_exchangeCityCommand;
                            FormAllianceCommand g_formAllianceCommand;
                            UntradeRouteCommand g_untradeRouteCommand;
                            ShowAdvancesCommand g_showAdvancesCommand;
                            AddMaterialsCommand g_AddMaterialsCommand;
                            SueFranchiseCommand g_sueFranchiseCommand;
                            GrantAdvanceCommand g_grantAdvanceCommand;
                            DumpMessagesCommand g_dumpMessagesCommand;
                            DumpChecksumCommand g_dumpChecksumCommand;
#ifdef WIN32
                           DumpCallStackCommand g_dumpCallStackCommand;
#endif
                           MakeCeaseFireCommand g_makeCeaseFireCommand;
                           SetGovernmentCommand g_setGovernmentCommand;
                           CityResourcesCommand g_cityResourcesCommand;
                           BreakAllianceCommand g_breakAllianceCommand;
                           WithdrawOfferCommand g_withdrawOfferCommand;
                          ToggleQuitFastCommand g_toggleQuitFastCommand;
                          BreakCeaseFireCommand g_breakCeaseFireCommand;
                          DumpAgreementsCommand g_dumpAgreementsCommand;
                          EnslaveSettlerCommand g_enslaveSettlerCommand;
                          LoadBuildQueueCommand g_loadBuildQueueCommand;
                          SaveBuildQueueCommand g_saveBuildQueueCommand;
                          InciteUprisingCommand g_inciteUprisingCommand;
                         RequestGreetingCommand g_requestGreetingCommand;
                         PactCaptureCityCommand g_pactCaptureCityCommand;
                         DisplayChecksumCommand g_displayChecksumCommand;
                         StopTradingWithCommand g_stopTradingWithCommand;
                         InvestigateCityCommand g_investigateCityCommand;
                         StealTechnologyCommand g_stealTechnologyCommand;
                         RequestOfferMapCommand g_requestOfferMapCommand;
                        InciteRevolutionCommand g_inciteRevolutionCommand;
                        CauseUnhappinessCommand g_causeUnhappinessCommand;
                        RequestOfferCityCommand g_requestOfferCityCommand;
                        RequestOfferGoldCommand g_requestOfferGoldCommand;
                        RequestDemandMapCommand g_requestDemandMapCommand;
                        AssassinateRulerCommand g_assassinateRulerCommand;
                        ShowNetworkStatsCommand g_showNetworkStatsCommand;
                        IsViolatingPeaceCommand g_isViolatingPeaceCommand;

                       RequestDemandGoldCommand g_requestDemandGoldCommand;
                       CreateImprovementCommand g_createImprovementCommand;
                       RequestDemandCityCommand g_requestDemandCityCommand;
                      ToggleSmoothScrollCommand g_toggleSmoothScrollCommand;
                      UndergroundRailwayCommand g_undergroundRailwayCommand;
                      DisplayChecksumOffCommand g_displayChecksumOffCommand;
                      TerrainImprovementCommand g_terrainImprovementCommand;
              TerrainImprovementCompleteCommand g_terrainImprovementCompleteCommand;
                      IsPollutionReducedCommand g_isPollutionReducedCommand;
                      IsViolatingBordersCommand g_isViolatingBordersCommand;

                      RequestExchangeMapCommand g_requestExchangeMapCommand;
                     SetMaterialsPercentCommand g_SetMaterialsPercentCommand;
                     RequestExchangeCityCommand g_requestExchangeCityCommand;
                     RequestOfferAdvanceCommand g_requestOfferAdvanceCommand;
                    InvestigateReadinessCommand g_investigateReadinessCommand;
                    IsViolatingCeaseFireCommand g_isViolatingCeaseFireCommand;
                    RequestDemandAdvanceCommand g_requestDemandAdvanceCommand;
                        PactEndPollutionCommand g_pactEndPollutionCommand;

                   RequestOfferCeaseFireCommand g_requestOfferCeaseFireCommand;
                  DumpDiplomaticRequestsCommand g_dumpDiplomaticRequestsCommand;
                  RequestExchangeAdvanceCommand g_requestExchangeAdvanceCommand;
                  RequestDemandStopTradeCommand g_requestDemandStopTradeCommand;
                RequestDemandAttackEnemyCommand g_requestDemandAttackEnemyCommand;
              RequestDemandLeaveOurLandsCommand g_requestDemandLeaveOurLandsCommand;
             RequestOfferPactCaptureCityCommand g_requestOfferPactCaptureCityCommand;
            RequestDemandReducePollutionCommand g_requestDemandReducePollutionCommand;

           RequestOfferPermanentAllianceCommand g_requestOfferPermanentAllianceCommand;
            RequestOfferPactEndPollutionCommand g_requestOfferPactEndPollutionCommand;






                            EndTurnSoundCommand g_endTurnSoundCommand;

                                   DRayTestCode g_DRayTestCode;

































                       ThroneRoomUpgradeCommand g_throneRoomUpgradeCommand;
                             GiveMeProbeCommand g_giveMeProbeCommand;
                          ToggleMapColorCommand g_toggleMapColorCommand;
                         ToggleHeuristicCommand g_toggleHeuristicCommand;
                                     YumCommand g_yumCommand;
                                    SlicCommand g_slicCommand;
                                 ShowPopCommand g_showPopCommand;
                                 HidePopCommand g_hidePopCommand;
                                 HowLongCommand g_howLongCommand;
                           DebugCheckMemCommand g_debugCheckMem;
                                OvertimeCommand g_overtimeCommand;
                               LearnWhatCommand g_learnWhatCommand;
                               GivesWhatCommand g_givesWhatCommand;
                              GetAdvanceCommand g_getAdvanceCommand;
                             TestMessageCommand g_testMessageCommand;
                            OvertimeCostCommand g_overtimeCostCommand;
                            BequeathGoldCommand g_bequeathGoldCommand;
                          InstantMessageCommand g_instantMessageCommand;
                                  InjoinCommand g_injoinCommand;
                                FastMoveCommand g_fastMoveCommand;
                                GrantAllCommand g_grantAllCommand;
                               GrantManyCommand g_grantManyCommand;
                               UseLadderCommand g_useLadderCommand;
                                TutorialCommand g_tutorialCommand;
                            SimultaneousCommand g_simultaneousCommand;
                              AutoCenterCommand g_autoCenterCommand;
                                  AiDumpCommand g_aiDumpCommand;
                                  RegardCommand g_regardCommand;
                                AttitudeCommand g_attitudeCommand;
                            DumpFZRegardCommand g_dumpFZRegardCommand;
                             SetFZRegardCommand g_setFZRegardCommand;
                                TotalWarCommand g_totalWarCommand;
                       SetUnitMovesStyleCommand g_setUnitMovesStyleCommand;
                         SetClassicStyleCommand g_setClassicStyleCommand;
                           SetSpeedStyleCommand g_setSpeedStyleCommand;





                       SetCarryoverStyleCommand g_setCarryoverStyleCommand;
                           SetTimedStyleCommand g_timedGameCommand;
                                MainMenuCommand g_mainMenuCommand;
                               SetupModeCommand g_setupModeCommand;
                                   ReadyCommand g_readyCommand;
                              DisplayMemCommand g_displayMemCommand;
                               UseDDBlitCommand g_useDDBlitCommand;
                                  LoadDBCommand g_loadDBCommand;
                        SellImprovementsCommand g_sellImprovementsCommand;
                               SellUnitsCommand g_sellUnitsCommand;
                           RobotMessagesCommand g_robotMessagesCommand;
                               FZCommentCommand g_fzCommentCommand;
                             AcceptOfferCommand g_acceptOfferCommand;
                                 DescendCommand g_descendCommand;
                                NearFortCommand g_nearestFortCommand;
                                NearCityCommand g_nearestCityCommand;
                             ForceRevoltCommand g_forceRevoltCommand;
                                     ToeCommand g_toeCommand;
                                  WhoAmICommand g_whoAmICommand;
                                AutoSaveCommand g_autoSaveCommand;
                              HeapTotalsCommand g_heapTotalsCommand;
                                 HotSeatCommand g_hotSeatCommand;
                                   EmailCommand g_emailCommand;
                                   ScoreCommand g_scoreCommand;
                                  FliLogCommand g_fliLogCommand;
                               SendSlaveCommand g_sendSlaveCommand;
                                 DisbandCommand g_disbandCommand;
                                  AttachCommand g_attachCommand;
                                  DetachCommand g_detachCommand;
                            ToggleShieldSupport g_toggleShieldSupport;
                      SuperFastDebugModeCommand g_superFastDebugModeCommand;
                                 KillPopCommand g_killPopCommand;
                                   BoardCommand g_boardCommand;
                               AutoGroupCommand g_autoGroupCommand;
                                  AddPopCommand g_addPopCommand;
                              CopyVisionCommand g_copyVisionCommand;

                               CombatLogCommand g_combatLogCommand;
                               RedrawMapCommand g_redrawMapCommand;
                                     GodCommand g_godCommand;
                               ReloadFliCommand g_reloadFliCommand;
                              MultiCycleCommand g_multiCycleCommand;
                              LeaksClearCommand g_leaksClearCommand;
                               LeaksShowCommand g_leaksShowCommand;
                            SlicVariableCommand g_slicVariableCommand;
                                DipLogOnCommand g_dipLogOnCommand;
                               DipLogOffCommand g_dipLogOffCommand;
                            DirectorDumpCommand g_directorDumpCommand;
                                  ResyncCommand g_resyncCommand;
                            CleanSpritesCommand g_cleanSpritesCommand;
                             CleanScreenCommand g_cleanScreenCommand;
                             ResetVisionCommand g_resetVisionCommand;

                               ExportMapCommand g_exportMapCommand;
                               ImportMapCommand g_importMapCommand;

                     InitializeDiplomacyCommand g_initializeDiplomacyCommand;
                          BeginDiplomacyCommand g_beginDiplomacyCommand;
                       ChooseNewProposalCommand g_chooseNewProposalCommand;
                          SetNewProposalCommand g_setNewProposalCommand;
                        SetHasInitiativeCommand g_setHasInitiativeCommand;
                             SetResponseCommand g_setResponseCommand;
                      ExecuteNewProposalCommand g_executeNewProposalCommand;
                         ExecuteResponseCommand g_executeResponseCommand;
                           ShowDiplomacyCommand g_showDiplomacyCommand;
                               NextStateCommand g_nextStateCommand;
                          SetPersonalityCommand g_setPersonalityCommand;
                              DeclareWarCommand g_declareWar;

                      SetGovernorForCityCommand g_setGovernorForCityCommand;
                    SetGovernorPwReserveCommand g_setGovernorPwReserveCommand;

                                 ToggleCellText g_toggleCellText;
                                 ToggleArmyText g_toggleArmyText;
								 ToggleArmyText g_toggleArmyName;

                                     ArmyClumps g_armyClumps;

                                SetGoodsCommand g_setGoodsCommand;

#ifdef DUMP_ASTAR

                               DumpAstarCommand g_dumpAstarCommand;
#endif

                                 AiDebugCommand g_aiDebugCommand;

                             ShowVictoryCommand g_showVictoryCommand;

                           ReloadSpritesCommand g_reloadSpritesCommand;

COMMAND(ShowVisCommand);
ShowVisCommand g_showVisCommand;

CommandRecord commands[] = {
	{"help", &g_helpCommand,
	"help [page] - this text, default is page 0"},
	{"helplines", &g_helpLinesCommand,
	"helplines <lines> - this many lines for 1 page of help text"},
	{"clear", &g_clearTextCommand,
	"clear - clear debug text from the screen"},
	{"build", &g_buildCommand,
	"build <city_idx> <unit_type> [player] - enter unit in build queue for city"},
	{"cityup", &g_upgradeCity,
	"cityup <city_idx> <upgrade_level 1-9 (0 is original base city)> - Change city to an upgraded city"},
	{"zb", &g_zbCommand,
	"zb - toggle a zoomed batllefield - on/off, using the current screen center point as the focus for the battle"},
	{"create", &g_createCommand,
	"create <home_city_idx> <unit_type> [x] [y] [z] [player] - instantly create unit"},
	{"tax", &g_taxCommand,
	"tax <science> <gold> <luxury> [player] - set taxes"},
	{"save", &g_saveCommand,
	"save <filename> - save current game to file name specified"},
	{"load", &g_restoreCommand,
	"load <filename> - restore previously saved game from file name specified"},
	{"loadaip", &g_loadAIPCommand,
	"loadaip <filename> <team_idx> - force the loading of the aip file from aip directory for specified team"},

	{"whoami", &g_whoAmICommand, "whoami - what aip is loaded up"},
	{"logai", &g_logAICommand,
	"logai <log_level> <team_idx> - set AI's level of logging"},
	{"diplogon", &g_dipLogOnCommand, "diplogon [player] - turn on diplomacy logging for player"},
	{"diplogoff", &g_dipLogOffCommand, "diplogoff [player] - turn off diplomacy logging for player"},
	{"restart", &g_restartCommand,
	"restart - restart game (Rereading profile.txt)"},
	{"improve", &g_improveCommand,
	"improve <city_idx> <improvement> [player] - insert improvement in build queue"},
	{"crc", &g_calcChecksumCommand,
	"crc - show crc's"},
	{"dumpcrc", &g_dumpChecksumCommand,
	"dumpcrc - dump crc's to log"},
	{"crcstart", &g_displayChecksumCommand,
	"crcstart - keep showing crc's"},
	{"crcoff", &g_displayChecksumOffCommand,
	"crcoff - stop showing crc's"},
	{"rtest", &g_randTestCommand,
	"rtest <num commands> <wait factor> - make <num commands> random moves"},
	{"stayon", &g_stayOnCommand,
	"stayon - stay in command mode after enter"},
	{"turnoff", &g_turnOffCommand,
	"turnoff - leave command mode after enter"},
	{"trade", &g_tradeRouteCommand,
	"trade <source_idx> <src_type> <src_resource> <dest_player> <dest_idx> - create a trade route"},
	{"untrade", &g_untradeRouteCommand,
	"untrade <route_idx> - Remove a trade route (index is into global routes array)"},
	{"cityresources", &g_cityResourcesCommand,
	"cityresources - show what cities have what resources"},
	{"offer", &g_offerCommand,
	"offer <city_idx> <type> <resource> <ask_type> <ask_resource> - Offer something in trade"},
	{"showoffers", &g_showOffersCommand,
	"showoffers - Display outstanding trade offers"},
	{"withdraw", &g_withdrawOfferCommand,
	"withdraw <offer_idx> - withdraw a trade offer"},
	{"acceptoffer", &g_acceptOfferCommand,
	 "acceptoffer <player_index> <offer_index> <city1> <city2> - accept a trade offer"},
	{"intercept", &g_interceptCommand,
	"intercept <route_idx> - intercept a trade route (index is into the selected item's cell's list)"},
	{"flood", &g_floodCommand,
	"flood - <phase> flood landscape"},
	{"tfog", &g_toggleFogCommand,
	"tfog - toggle to fog off and on"},
	{"tsmooth", &g_toggleSmoothScrollCommand,
	"tsmooth - toggle smooth scroll on and off"},
#ifdef WIN32
	{"dumpcallstack", &g_dumpCallStackCommand,
	"dumpcallstack - test callstack dumpage to file callstack.txt"},
#endif
	{"tquitfast", &g_toggleQuitFastCommand,
	"tquitfast - turn this on to NOT log memory at quit"},
	{"timprove", &g_terrainImprovementCommand,
	"timprove <type> [transformType] - build a terrain improvement at mouse"},
	{"cimprove", &g_terrainImprovementCompleteCommand,
	"cimprove - complete any terrain improvements under the cursor"},
	{"readiness", &g_readinessCommand,
	"readiness <level> - set player's military readiness"},
	{"givecity", &g_giveCityCommand,
	"givecity <player> - hand over currently selected city to player #"},
	{"exchangecity", &g_exchangeCityCommand,
	"exchangecity <player> <city 1> <city 2>- give city 1 to player # and city 2 to current player"},
	{"giveadvance", &g_giveAdvanceCommand,
	"giveadvance <player> <advance> - hand over advance # to player #"},
	{"givegold", &g_giveGoldCommand,
	"givegold <recipient> <amount> - hand over amount of gold to recipient player #"},
	{"giveunit", &g_giveUnitCommand,
	"giveunit <source player> <target player> <unit index> - hand over unit # player # to player #"},
	{"stoptradingwith", &g_stopTradingWithCommand,
	"stoptradingwith <player> - stop trading with player #"},
	{"dumpallies", &g_dumpAlliesCommand,
	"dumpallies - dump allies of this player"},
	{"formalliance", &g_formAllianceCommand,
	"formalliance <player> - form a permanent alliance with player #"},
	{"breakalliance", &g_breakAllianceCommand,
	"breakalliance <player> - break an alliance with player #"},
	{"exchangemap", &g_exchangeMapCommand,
	"exchangemap <player> - exchange known world map with player #"},
	{"givemap", &g_giveMapCommand,
	"givemap <player> - give my map to player #"},
	{"ispollutionreduced", &g_isPollutionReducedCommand,
	"ispollutionreduced - test to see if pollution is below diplomatic threshold for current player"},
	{"isviolatingborders", &g_isViolatingBordersCommand,
	"isviolatingborders <player> - is the current player violating the borders of player #"},
	{"isviolatingpeace", &g_isViolatingPeaceCommand,
	"isviolatingpeace <player> - is the current player violating the peace of player #"},
	{"isviolatingceasefire", &g_isViolatingCeaseFireCommand,
	"isviolatingceasefire <player> - is the current player violating cease fire agreement between player #"},
	{"chatmask", &g_chatMaskCommand,
	 "chatmask <pl1> [pl2] [pl3] ... [pln] - send chat messages to players listed"},
	{"say", &g_chatCommand,
	 "say <message> - send message to players selected by chatmask"},
	{"'", &g_chatCommand,
	 "' <message> - synonym for say"},
	{"datacheck", &g_datacheckCommand,
	 "datacheck - toggle data check on or off (speeds up end/begin turn!)"},
	{"wonder", &g_buildWonderCommand,
	 "wonder <city> <wonder> - build a wonder"},
	{"tgrid", &g_toggleGridCommand,
	 "tgrid - toggle grid on and off"},
	{"theralds", &g_toggleHeraldCommand,
	"theralds - toggle the hearlds on and off"},
	{"twater", &g_toggleWaterCommand,
	"twater - toggle animated water on and off"},
	{"tspace", &g_toggleSpaceCommand,
	 "tspace - toggle space on and off"},
	{"showadvance", &g_showAdvancesCommand,
	"showadvance - display current player's advances to text buffer"},
	{"makeceasefire", &g_makeCeaseFireCommand,
	"makeceasefire <player> - agree to cease fire between current player and specified player"},
	{"breakceasefire", &g_breakCeaseFireCommand,
	"breakceasefire <player> - break any cease fire agreement between current player and specified player"},
	{"dumpagreements", &g_dumpAgreementsCommand,
	"dumpagreements - display all agreements for current player to log file"},
	{"dumprequests", &g_dumpDiplomaticRequestsCommand,
	"dumprequests - display all diplomatic requests for current player to log file"},
	{"dumpmessages", &g_dumpMessagesCommand,
	"dumpmessages - display all queued messages for current player to log file"},
	{"seestats", &g_SeeWWRCommand,
	"seestats - see workday, wages and rations, material contribution, readiness"},
	{"setworkday", &g_SetWorkdayCommand,
	"setkworday <level> - set workday to new level"},
	{"setwages", &g_SetWagesCommand,
	"setwages <level> - set wages to a new level"},
	{"setrations", &g_SetRationsCommand,
	"setrations <level> - set rations to a new level "},
	{"setmat", &g_SetMaterialsPercentCommand,
	"setmat <percent> - set the percent of production for materials"},
	{"addmat", &g_AddMaterialsCommand,
	"addmat <value> - Give the current player %d material points"},
	{"setready", &g_SetReadinessCommand,
	"setready <level> - set the readiness 0 - peace - 1 alert 2 - war"},
	{"shownet", &g_showNetworkStatsCommand,
	"shownet - toggle display of network stats"},
	{"pop", &g_popCommand,
	"pop - place or remove a pop at the mouse (place from selected city)"},
	{"tframe", &g_frameCommand,
	"tframe - turn frame rate display on or off"},
	{"government", &g_setGovernmentCommand,
	"government <type> - set the type of government"},
	{"revolt", &g_revoltCommand,
	"revolt - the currently selected city goes in to revolt"},
	{"pac", &g_PacCommand,
	 "mystery command"},
	{"pactendpollution", &g_pactEndPollutionCommand,
	"pactendpollution <player> - form a pact with current player and specified player to end pollution"},
	{"pactcapturecity", &g_pactCaptureCityCommand,
	"pactcapturecity <player A> <player B> <city> - Player A to capture Player B's city"},






	{"showowner", &g_showOwnerCommand,
	 "showowner - Show who owns cells"},
	{"requestgreeting", &g_requestGreetingCommand,
	"requestgreeting <player> - send a diplomatic first contact greeting to <player>"},
	{"demandadvance", &g_requestDemandAdvanceCommand,
	"demandadvance <player> <advance> - demand that the specified player give the advance"},
	{"demandcity", &g_requestDemandCityCommand,
	"demandcity <player> <city> - demand a city from the specified player"},
	{"demandmap", &g_requestDemandMapCommand,
	"demandmap <player> - demand that the specified player hand over their map"},
	{"demandgold", &g_requestDemandGoldCommand,
	"demandgold <player> <amount> - demand that the specified player give an amount of gold"},
	{"demandstoptrade", &g_requestDemandStopTradeCommand,
	"demandstoptrade <player A> <player B> - demand that player A stop trading with player B"},
	{"demandattackenemy", &g_requestDemandAttackEnemyCommand,
	"demandattackenemy <player A> <player B>- demand that Player A attack Player B"},
	{"demandleaveourlands", &g_requestDemandLeaveOurLandsCommand,
	"demandleaveourlands <player> - demand that the player leave our lands"},
	{"demandreducepollution", &g_requestDemandReducePollutionCommand,
	"demandreducepollution <player> - demand that the player reduce their pollution"},
	{"offeradvance", &g_requestOfferAdvanceCommand,
	"offeradvance <player> <advance> - offer the advance to the player as good will"},
	{"offercity", &g_requestOfferCityCommand,
	"offercity <player> <city> - offer the city to the player as good will"},
	{"offermap", &g_requestOfferMapCommand,
	"offermap <player> - offer the map to the player as good will"},
	{"offergold", &g_requestOfferGoldCommand,
	"offergold <player> <amount> - offer a gold amount to the player as good will"},
	{"offerceasefire", &g_requestOfferCeaseFireCommand,
	"offerceasefire <player> - offer a cease fire to the player"},
	{"offerpermanentalliance", &g_requestOfferPermanentAllianceCommand,
	"offerpermanentalliance <player> - offer a permanent alliance to the player"},
	{"offerpactcapturecity", &g_requestOfferPactCaptureCityCommand,
	"offerpactcapturecity <player A> <player B> <city> - request a pact with Player A to capture the city of Player B"},
	{"offerpactendpollution", &g_requestOfferPactEndPollutionCommand,
	"offerpactendpollution <player> - request a pact to reduce pollution with the player"},












	{"requestexchangeadvance", &g_requestExchangeAdvanceCommand,
	"requestexchangeadvance <player> <advance A> <advance B> - offer to give Advance A to player if they give Advance B"},
	{"requestexchangecity", &g_requestExchangeCityCommand,
	"requestexchangecity <player> <city 1> <city 2> - offer to exchange cities with the specified player"},
	{"requestexchangemap", &g_requestExchangeMapCommand,
	"requestexchangemap <player> - offer to exchange maps with the specified player"},
	{"debugmask", &g_debugMaskCommand,
	"debugmask <bitpos> - set or clear a bit in the debug mask"},
	{"dm", &g_debugMaskCommand,
	"dm <bitpos> - synonym for debugmask"},
	{"spewunits", &g_spewUnitsCommand,
	"spewunits - spew units!"},
	{"createimprovement", &g_createImprovementCommand,
	"createimprovement <type> - instantly place a building in the selected city"},
	{"setcityname", &g_setCityNameCommand,
	"setcityname <name> - set name of city"},
	{"setcitysize", &g_setCitySizeCommand,
	"setcitysize <size> - set the size of the selected city"},
	{"taistr", &g_toggleAiStr, "taistr - toggle to ai strings on and off"},

	{"investigatecity", &g_investigateCityCommand,
	"investigatecity - use the selected secret agent to investigate the pointed at city"},
	{"stealtechnology", &g_stealTechnologyCommand,
	"stealtechnology - use the selected agent to steal tech from the pointed at city"},
	{"inciterevolution", &g_inciteRevolutionCommand,
	"inciterevolution - use the selected agent to incite a revolt in the pointed at city"},
	{"assassinate", &g_assassinateRulerCommand,
	"assassinate - use the selected agent to assassinate the ruler in the pointed at city"},
	{"investigatereadiness", &g_investigateReadinessCommand,
	"investigatereadiness - use the selected agent to investigate the readiness of the owner of the pointed at city"},
	{"nullifywalls", &g_nullifyWallsCommand,
	"nullifywalls - use the selected agent to temporarily nullify the walls of the pointed at city"},
	{"grantadvance", &g_grantAdvanceCommand,
	"grantadvance <which> - give the current player the specified advance immediately"},
	{"grantall", &g_grantAllCommand,
	 "grantall - give the current player EVERY advance"},
	{"grantmany", &g_grantManyCommand,
	 "grantmanycommand - grant up through advance 63 (computers at the time of this writing)"},
	{"bombard", &g_bombardCommand,
	"bombard - the selected unit bombards the pointed at unit"},
	{"heargossip", &g_hearGossipCommand,
	"heargossip <player> - hear gossip about the given player"},
	{"franchise", &g_franchiseCommand,
	"franchise - the selected unit creates a franchise in the pointed at city"},
	{"expel", &g_expelCommand,
	"expel - expel the pointed at unit with the selected unit"},
	{"sue", &g_sueCommand,
	"sue - sue the pointed at unit with the selected unit"},
	{"suefranchise", &g_sueFranchiseCommand,
	"suefranchise - remove franchise from pointed at city with selected unit (5 turns)"},
	{"causeunhappiness", &g_causeUnhappinessCommand,
	"causeunhappiness - cause unhappiness in the pointed at city with the selected unit"},
	{"plantnuke", &g_plantNukeCommand,
	"plantnuke - plant a nuke in the pointed at city"},
	{"loadbuild", &g_loadBuildQueueCommand,
	"loadbuild <filename> - load a build queue for currently selected city"},
	{"savebuild", &g_saveBuildQueueCommand,
	"savebuild <filename> - save a build queue from the currently selected city"},
	{"slaveraid", &g_slaveRaidCommand,
	"slaveraid - make a slave raid on the pointed at city"},
	{"enslavesettler", &g_enslaveSettlerCommand,
	"enslavesettler - enslave the pointed at settler"},
	{"undergroundrailway", &g_undergroundRailwayCommand,
	"undergroundrailway - deslave a pop in the pointed at city"},
	{"tt", &g_tileTypeCommand,
	"tt [x,y,z] - find tile type & position of tile under mouse (also supports optional specified position)"},
	{"inciteuprising", &g_inciteUprisingCommand,
	"inciteuprising - incite a slave uprising in the pointed at city"},
	{"bioinfect", &g_bioInfectCommand,
	"bioinfect - infect the pointed at city with biologicals"},
	{"nanoinfect", &g_nanoInfectCommand,
	"nanoinfect - infect the pointed at city with nasty itty bitty machines"},
	{"convertcity", &g_convertCityCommand,
	"convertcity - convert the pointed at city to your faith"},
	{"reformcity", &g_reformCityCommand,
	"reformcity - attempt to reform a converted city"},
	{"indulgence", &g_indulgenceCommand,
	"indulgence - sell indulgences to a city"},
	{"soothsay", &g_soothsayCommand,
	"soothsay - Say sooth. \"sooth! I say, SOOTH!\""},

	{"cloak", &g_cloakCommand,
	"cloak - cloak cloakable cloakers"},
	{"uncloak", &g_uncloakCommand,
	"uncloak - uncloak uncloakable uncloakers"},
	{"rustle", &g_rustleCommand,
	"rustle - rustle cattle (or battleships or whatever)"},
	{"createpark", &g_createParkCommand,
	"createpark - use a park ranger"},
	{"createrift", &g_createRiftCommand,
	"createrift - Open a rift gate to the pointed at location"},
	{"rnd", &g_fastRoundCommand,
	"rnd <num> - make num rounds pass"},
	{"killtile", &g_killTileCommand,
	"killtile - kills the tile under the mouse"},
	{"throneupgrade", &g_throneRoomUpgradeCommand,
	"throneupgrade [n] - make new upgrade(s) available for the throne room"},
	{"givemethebleepingprobe", &g_giveMeProbeCommand,
	"givemethebleepingprobe - pretend I have recovered the wormhole probe"},

	{"buildwhat", &g_buildWhatCommand,
	 "buildwhat - show what the selected city can build"},
	{"learnwhat", &g_learnWhatCommand,
	 "learnwhat - Show what can be researched"},
	{"otcost", &g_overtimeCostCommand,
	 "otcost - show how much gold for overtime in the selected city"},
	{"overtime", &g_overtimeCommand,
	 "overtime - buy the item being built in the selected city"},
	{"giveswhat", &g_givesWhatCommand,
	 "giveswhat <num> - show what the advance will give you"},
	{"howlong", &g_howLongCommand,
	 "howlong - how long it will take the selected city to build the current item"},
	{"checkmem", &g_debugCheckMem, "toggle Crt memory validate - the game goes really slow"},

	{"slic", &g_slicCommand,
	 "slic <expression> - add an expression to the watch window"},
	{"assign", &g_slicCommand,
	 "assign <symbol> = <value> - assign a slic symbol a value"},
	{"bequeathgold", &g_bequeathGoldCommand,
	"bequeathgold <amount> - just give a player some gold"},
	{"testmessage", &g_testMessageCommand,
	"testmessage - send a test message -- do not use!!"},
	{"tcolor", &g_toggleMapColorCommand, "toggle map debug color"},
	{"theuristic",  &g_toggleHeuristicCommand, "toggle the old astar heuristic"},
	{"yum", &g_yumCommand,
	 "yum - show food, production, goods for the pointed at square"},
	{"getadvance", &g_getAdvanceCommand,
	 "getadvance <advance> - set what the player is researching"},
	{"showpop", &g_showPopCommand,
	 "showpop - show population on the map"},
	{"hidepop", &g_hidePopCommand,
	 "hidepop - hide population"},
	{"instantmessages", &g_instantMessageCommand,
	 "instantmessages - toggle instant message hack on/off"},
	{"fastmove", &g_fastMoveCommand,
	"fastmove - toggle fast sprite movement on/off"},
	{"injoin", &g_injoinCommand,
	 "injoin - slap an injunction on the pointed at city"},
	{"useladder", &g_useLadderCommand,
	 "useladder - use the space ladder in the current city, if any"},
	{"tutorial", &g_tutorialCommand,
	 "tutorial [player] - set the tutorial player, or turn the tutorial messages on/off with no argument"},
	{"simultaneous", &g_simultaneousCommand,
	 "simultaneous - toggle simultaneous mode"},
	{"autocenter", &g_autoCenterCommand,
	 "Toggle auto center"},
	{"aidump", &g_aiDumpCommand,
	"dump the ai stats to a file"},
	{"regard", &g_regardCommand,
	"regard [civ] [regard] - set regard of current civ to other civ"},
	{"attitude", &g_attitudeCommand,
	"attitude [civ] [attitude] - set attitude of current civ to other civ"},
	{"showregard", &g_dumpFZRegardCommand,
	    "showregard [player] - show the regard of the player to the screen"},
	{"setregard", &g_setFZRegardCommand,
	    "setregard me him r - set the regard of player me to player him to r"},
	{"totalwar", &g_totalWarCommand, "totalwar - everyone hates everyone"},
	{"unitmoves", &g_setUnitMovesStyleCommand,
	 "unitmoves [nummoves] - set unit moves style game for network"},
	{"classic", &g_setClassicStyleCommand,
	 "classic - set classic style network game"},
	{"speedturns", &g_setSpeedStyleCommand,
	 "speedturns <t>- make turns for network games limited to at most t seconds"},











	{"timedgame", &g_timedGameCommand,
	 "timedgame <m> [s] - set time for network game to m minutes, s seconds per player"},
	{"carryover", &g_setCarryoverStyleCommand,
	 "carryover - enable carryover bonus for speed turns"},
	{"mainmenu", &g_mainMenuCommand, "mainmenu - quit the game and return to the main menu"},
	{"setupmode", &g_setupModeCommand,
	 "setupmode - enable network setup mode (host only, turn 0 only!)"},
	{"ready", &g_readyCommand,
	 "ready - signal done setting up"},
	{"ddblit", &g_useDDBlitCommand, "ddblit [on/off] - use Direct Draw for blitting"},
	{"tmem", &g_displayMemCommand, "tmem - toggle memory display"},
	{"loaddb", &g_loadDBCommand,
	 "loaddb [dbname] - reload a database (string, advance, terrain, unit, const, improve, wonder, civ, inst, pop)"},
	{"sellunits", &g_sellUnitsCommand,
	 "sellunits - trade pointed at units for points (when in setup/points mode)"},
	{"sellimprovements", &g_sellImprovementsCommand,
	 "sellimprovements - trade pointed at terrain improvements for points"},
	{"robotmessages", &g_robotMessagesCommand,
	 "robotmessages - toggle messages for robots on/off"},
	{"fzcomment", &g_fzCommentCommand,
	 "fzcomment - add a comment to this turn's fuzzy inputs log"},
	{"descend", &g_descendCommand,
	 "descend - descend from space"},
	{"nearfort", &g_nearestFortCommand,
	 "nearfort - go to the nearest fort"},
	{"nearcity", &g_nearestCityCommand,
	 "nearcity - go to the nearest city"},
	{"forcerevolt", &g_forceRevoltCommand,
	 "forcerevolt - force pointed at city to revolt"},
	{"toe", &g_toeCommand,
	 "toe - Toe of Death"},
	{"autosave", &g_autoSaveCommand,
	"autosave - Toggle the auto-save-at-turn-start functionality"},
	{"heaptotals", &g_heapTotalsCommand,
	"heaptotals - Dump a heap status to the debug log file (CTP_debug.txt)"},
	{"hotseat", &g_hotSeatCommand,
	 "hotseat - make this into a hotseat game"},
	{"email", &g_emailCommand,
	 "email - make this into an email game"},
	{"score", &g_scoreCommand,
	 "score - show the current player's score"},
	{"flilog", &g_fliLogCommand,
	"flilog - toggle flilog.txt logging on and off"},
	{"reloadfli", &g_reloadFliCommand, "reloadfli [player] - reload this players ai"},
	{"sendslave", &g_sendSlaveCommand,
	 "sendslave - send a slave from selected city to pointed at city"},
	{"disband", &g_disbandCommand,
	 "disband - disband a unit"},

	{"attach", &g_attachCommand,
	 "attach [slot] - attach an AI to player [slot] or current player"},
	{"detach", &g_detachCommand,
	 "detach [slot] - detach an AI player"},
	{"tsupport", &g_toggleShieldSupport, "tsupport - turn off and on shield support"},
	{"superfast", &g_superFastDebugModeCommand,
	"superfast [on/off] - turn SuperFastDebugMode on and off (no goal execute)"},
	{"killpop", &g_killPopCommand,
	 "killpop - kill one pop in the selected city"},
	{"board", &g_boardCommand,
	 "board - board selected army into transports in the same tile"},
	{"autogroup", &g_autoGroupCommand,
	 "autogroup - make the game a living hell"},

	{"addpop", &g_addPopCommand,
	    "addpop - add one or more new populations to the currently selected city"},
	{"copyvision", &g_copyVisionCommand,
	 "copyvision - copy gamestate vision to screen"},

	{"combatlog", &g_combatLogCommand,
	 "combatlog - turn on combat logging"},
	{"redrawmap", &g_redrawMapCommand,
	"redrawmap - redraw map"},
	{"god", &g_godCommand,
	"god - view the entire map, but don't affect gameplay vision"},
	{"multicycle", &g_multiCycleCommand,
	 "multicycle - toggle multiple turn auto-cycling"},

	{"leaksclear", &g_leaksClearCommand,
	"leaksclear - reset the leak-watcher and begin watching for leaks again"},
	{"leaksshow", &g_leaksShowCommand,
	"leaksshow - dump all leaks to the debug file"},

	{"svariable", &g_slicVariableCommand,
	 "svariable <name> - show the value of a slic variable"},

	{"directordump", &g_directorDumpCommand,
	 "directordump - dump director stats to the logfile"},

	{"resync", &g_resyncCommand,
	 "resync <x> - as the host in a network game, resync client #x"},

	{"setairevolt", &g_setAIRevoltCommand,
	 "setairevolt <x> - where x is 0 for off, 1 for on."},

	{"cleansprites", &g_cleanSpritesCommand,
	"cleansprites - scrub all game sprites of black pixels"},

	{"cleanscreen", &g_cleanScreenCommand,
	"cleanscreen <screenname> - deallocate memory of a screen"},

	{"resetvision", &g_resetVisionCommand,
	 "Set all the non-visible tiles to unexplored for the current player"},

	{"exportmap", &g_exportMapCommand,
	"Export the terrain, ruins, rivers, and goods for this map"},
	{"importmap", &g_importMapCommand,
	"Import the terrain, ruins, rivers, and goods into this map"},

	{"initdip", &g_initializeDiplomacyCommand,
	"initdip <player1> <player2> - Initialize diplomatic states between two players."},
	{"begindip", &g_beginDiplomacyCommand,
	"begindip [<player>] - begin diplomacy turn for specified player."},
	{"chooseproposal", &g_chooseNewProposalCommand,
	"chooseproposal [<player>] <foreigner> - choose a new proposal from motivation."},
	{"proposal", &g_setNewProposalCommand,
	"proposal <sender> <receiver> <request_type> <offer_type> [<request_arg>] - set the new top proposal from sender to receiver."},
	{"response", &g_setResponseCommand,
	"response <sender> <receiver> <type> [<second_type> <second_arg>] - set the top response from sender or receiver depending on initiative."},
	{"hasinitiative", &g_setHasInitiativeCommand,
	"hasinitiative <sender> <receiver> <has_initiative> - set whether or not receiver has initiative with 3rd parameter."},
	{"execproposal", &g_executeNewProposalCommand,
	"execproposal <sender> <receiver> - execute the top proposal from sender to receiver."},
	{"execresponse", &g_executeResponseCommand,
	"execresponse <sender> <receiver> <player with initiative> - execute the top response from sender or receiver depending on initiative."},
	{"showdip", &g_showDiplomacyCommand,
	"showdip <player> - show diplomatic state of player."},
	{"nextstate", &g_nextStateCommand,
	"nextstate <player> - transition to next strategic and diplomatic states."},
	{"setpersonality", &g_setPersonalityCommand,
	"setpersonality [<player>] <personality_name> - set personality of player."},
	{"declarewar", &g_declareWar,
	"declarewar <foreigner> - unilateral declaration of war against foreigner."},
	{"setgovernor", &g_setGovernorForCityCommand,
	"setgovernor <number> - set governor for selected city (-1 for no governor)."},
	{"govpw", &g_setGovernorPwReserveCommand,
	"govpw <reserve> - set how much pw the tile improvement governor should to reserve for the player  (-1 for no governor)."},


	{"celltext", &g_toggleCellText,
	"celltext - toggle the displaying of AI debug text for Cells on and off"},
	{"armytext", &g_toggleArmyText,
	"armytext - toggle the displaying of AI debug text for Armies on and off"},
	{"armyname", &g_toggleArmyName,
	"armyname - toggle the displaying of name for Armies on and off"},

	{"armyclumps", &g_armyClumps,
	"armyclumps - display armies as clumps of up to 3 small units"},

	{"setgoods", &g_setGoodsCommand,
	"setgoods <good type> - place good of type at current cursor location"},

#ifdef DUMP_ASTAR
	{"dumpastar", &g_dumpAstarCommand,
	"dumpsastar <filename> - dumps the astar findpath call stack to filename."},
#endif

	{"aidebug", &g_aiDebugCommand,
	"aidebug [<goal_type> <player>] - create AI debugging output for player that owns selected cell, specific army if selected or for goal and player if specified."},

	{"victory", &g_showVictoryCommand,
	"victory - show the victory screen"},

	{"endturnsound", &g_endTurnSoundCommand, "toggle the end turn sound on/off lemur poo lemur poo"},

	{"draytest", &g_DRayTestCode,
	"This is just my test function, it will never do anything useful in the game."},

	{"reloadsprites", &g_reloadSpritesCommand,
	 "Tell all UnitActors to reload their sprites."},

	{"showvisibility", &g_showVisCommand, "Show visibility flags for first unit in all armies"},
	 {NULL, NULL, NULL}
};

void ShowVisCommand::Execute(sint32 argc, char **argv)
{
	sint32 p, u;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p])
			continue;

		for(u = 0; u < g_player[p]->m_all_armies->Num(); u++) {
			char buf[40];
			sprintf(buf, "VIS: %lx", g_player[p]->m_all_armies->Access(u).Access(0).GetVisibility());
			g_graphicsOptions->AddTextToArmy(g_player[p]->m_all_armies->Access(u), buf, 255, -1);
		}
	}
}

void ReloadSpritesCommand::Execute(sint32 argc, char **argv)
{
	Director::ReloadAllSprites();
}

void InitializeDiplomacyCommand::Execute(sint32 argc, char **argv) {

	if (argc <= 1)
		return;

	#ifdef _DEBUG
	c3debug_SetDebugMask(k_DBG_DIPLOMACY, TRUE);
	#endif

	sint32 player1 = atoi(argv[1]);
	sint32 player2 = g_selected_item->GetVisiblePlayer();
	if(argc > 2) {
		player2 = atoi(argv[2]);
	}

	if (player1 == player2)
		return;

	Diplomat::GetDiplomat(player1).InitStrategicState();
	Diplomat::GetDiplomat(player1).InitDiplomaticState(player2);
	Diplomat::GetDiplomat(player2).InitDiplomaticState(player1);
}

void BeginDiplomacyCommand::Execute(sint32 argc, char **argv) {
	sint32 player = g_selected_item->GetVisiblePlayer();
	if(argc > 1) {
		player = atoi(argv[1]);
	}

	Diplomat::GetDiplomat(player).BeginTurn();
}

void ChooseNewProposalCommand::Execute(sint32 argc, char **argv) {
	sint32 player = g_selected_item->GetVisiblePlayer();
	sint32 foreigner;
	if (argc < 2)
		return;
	if(argc > 2) {
		player = atoi(argv[1]);
		foreigner = atoi(argv[2]);
	}
	else {
		foreigner = atoi(argv[1]);
	}

	if (player == foreigner)
		return;

	Diplomat::GetDiplomat(player).ChooseNewProposal(foreigner);
}

void SetNewProposalCommand::Execute(sint32 argc, char **argv) {
	if(argc < 5 )
		return;

	sint32 sender = atoi(argv[1]);
	sint32 receiver = atoi(argv[2]);
	sint32 first_type = atoi(argv[3]);
	sint32 second_type = atoi(argv[4]);
	sint32 arg = -1;
	if (argc > 5)
		arg = atoi(argv[5]);

	if (sender == receiver)
		return;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	NewProposal new_proposal;
	new_proposal.detail.first_type = static_cast<PROPOSAL_TYPE>(first_type);
	new_proposal.detail.second_type = static_cast<PROPOSAL_TYPE>(second_type);
	new_proposal.senderId = sender;
	new_proposal.receiverId = receiver;
	new_proposal.detail.first_arg.cityId = arg;
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;
	new_proposal.priority = 9999;

	sender_diplomat.SetMyLastNewProposal(receiver, new_proposal);
}

void SetHasInitiativeCommand::Execute(sint32 argc, char **argv) {
	if(argc < 3 )
		return;

	sint32 sender = atoi(argv[1]);
	sint32 receiver= atoi(argv[2]);
	bool has_initiative = (atoi(argv[3]) != 0);

	if (sender == receiver)
		return;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	bool current_has_initiative = sender_diplomat.GetReceiverHasInitiative(receiver);
	if (current_has_initiative != has_initiative)
	{
		sender_diplomat.SetReceiverHasInitiative(receiver, has_initiative);
	}

}

void SetResponseCommand::Execute(sint32 argc, char **argv) {
	if(argc < 5 )
		return;

	sint32 sender = atoi(argv[1]);
	sint32 receiver = atoi(argv[2]);
	sint32 type = atoi(argv[3]);
	sint32 responder;
	sint32 proposal_type = -1;
	sint32 proposal_arg = -1;
	sint32 target;

	if (sender == receiver)
		return;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	if (sender_diplomat.GetReceiverHasInitiative(receiver)) {
		responder = receiver;
		target = sender;
	}
	else {
		responder = sender;
		target = receiver;
	}

	NewProposal new_proposal = sender_diplomat.GetMyLastNewProposal(receiver);

	Response response;
	response.type = static_cast<RESPONSE_TYPE>(type);
	response.senderId = sender;
	response.receiverId = receiver;
	response.counter.tone = DIPLOMATIC_TONE_EQUAL;
	response.priority = 9999;

	if (argc > 4)
		proposal_type = atoi(argv[4]);

	if (argc > 5)
		proposal_arg = atoi(argv[5]);

	if (type == RESPONSE_COUNTER)
	{

		if (argc < 5 || proposal_type < 0)
			return;
		response.counter.first_type = new_proposal.detail.first_type;
		response.counter.first_arg = new_proposal.detail.first_arg;
		response.counter.second_type = static_cast<PROPOSAL_TYPE>(proposal_type);
		response.counter.second_arg = proposal_arg;
	}
	else if (type == RESPONSE_THREATEN)
	{

		if (argc < 5 || proposal_type == -1)
			return;
		response.threat.type = static_cast<THREAT_TYPE>(proposal_type);

		if (proposal_arg != -1)
			response.threat.arg.cityId = proposal_arg;
	}

	Diplomat &responder_diplomat = Diplomat::GetDiplomat(responder);

	responder_diplomat.SetMyLastResponse(target, response);
}

void ExecuteNewProposalCommand::Execute(sint32 argc, char **argv) {
	if(argc != 3 )
		return;

	sint32 sender = atoi(argv[1]);
	sint32 receiver = atoi(argv[2]);

	if (sender == receiver)
		return;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	sender_diplomat.ExecuteNewProposal(receiver);
}

void ExecuteResponseCommand::Execute(sint32 argc, char **argv) {
	if(argc != 3 )
		return;

	sint32 sender = atoi(argv[1]);
	sint32 receiver = atoi(argv[2]);
	sint32 responder;

	if (sender == receiver)
		return;

	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);






	const Response & receiver_response_pending = receiver_diplomat.GetResponsePending(sender);
	const Response & sender_response_pending = sender_diplomat.GetResponsePending(receiver);

	bool show_response;
	if (sender_diplomat.GetReceiverHasInitiative(receiver))
	{
		show_response =
		 ((receiver == g_selected_item->GetVisiblePlayer()) &&
		 !(sender_response_pending == Diplomat::s_badResponse));
	}
	else
	{
		show_response =
		 ((sender == g_selected_item->GetVisiblePlayer()) &&
		 !(receiver_response_pending == Diplomat::s_badResponse));
	}

	if (show_response)
	{

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResponseReady,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);
	}

	if (sender_diplomat.GetReceiverHasInitiative(receiver)) {
		responder = sender;
	}
	else {
		responder = receiver;
	}
	Diplomat &responder_diplomat = Diplomat::GetDiplomat(responder);
	responder_diplomat.ExecuteResponse(sender, receiver);
}

void ShowDiplomacyCommand::Execute(sint32 argc, char **argv) {

	if(argc == 2 ) {
		sint32 player = atoi(argv[1]);
		Diplomat::GetDiplomat(player).DebugStatus();
	}
	else if (argc == 3)	{
		sint32 player = atoi(argv[1]);
		sint32 foreigner = atoi(argv[2]);
		Diplomat::GetDiplomat(player).DebugStatus(foreigner);
	}
	else
		Diplomat::DebugStatusAll();
}

void NextStateCommand::Execute(sint32 argc, char **argv) {

	sint32 playerId     = PLAYER_INDEX_VANDALS;

	if (argc >= 2) {
		playerId = atoi(argv[1]);
		Diplomat::GetDiplomat(playerId).NextStrategicState();
	}

	if (argc == 3) {
		Diplomat::GetDiplomat(playerId).NextDiplomaticState(atoi(argv[2]));
	}
	else {
		for (sint32 foreignerId = 0; foreignerId < k_MAX_PLAYERS; ++foreignerId)
        {
			if (foreignerId != playerId)
            {
				Diplomat::GetDiplomat(playerId).NextDiplomaticState(foreignerId);
            }
		}
	}
}

void SetPersonalityCommand::Execute(sint32 argc, char **argv) {
	sint32 playerId = g_selected_item->GetVisiblePlayer();
	const char* personality_name;

	if(argc < 2 )
		return;

	if(argc >= 3 ) {
		playerId = atoi(argv[1]);
		personality_name = argv[2];
	}
	else {
		personality_name = argv[1];
	}
	Diplomat::GetDiplomat(playerId).SetPersonalityName(personality_name);
}

void DeclareWarCommand::Execute(sint32 argc, char **argv) {
	if(argc < 2 )
		return;

	Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).
		DeclareWar(atoi(argv[1]));
}

void SetGovernorForCityCommand::Execute(sint32 argc, char **argv) {
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

    g_selected_item->GetTopCurItem(player, item, state);

    if (state == SELECT_TYPE_LOCAL_CITY) {

        sint32 num;
        if (2 == argc) {
            num = atoi(argv[1]);
        } else {
            num = -1;
        }

        Unit city = item;
		if (num == -1 || num >= g_theBuildListSequenceDB->NumRecords())
			city.GetCityData()->SetUseGovernor(FALSE);
		else
		{
			city.GetCityData()->SetUseGovernor(TRUE);
			city.GetCityData()->SetBuildListSequenceIndex(num);
		}
	}
}

void SetGovernorPwReserveCommand::Execute(sint32 argc, char **argv) {

	sint32 player = g_selected_item->GetVisiblePlayer();

	sint32 num;
	if (argc == 2) {
		num = atoi(argv[1]);
	} else {
		num = -1;
	}

	if (num >= 0)
		g_player[player]->SetGovernorPwReserve(num);
	else
		g_player[player]->SetGovernorPwReserve(-1);
}


void ToggleCellText::Execute(sint32 argc, char **argv)
{
	if (g_graphicsOptions->IsCellTextOn()) {
		g_graphicsOptions->CellTextOff();
	} else {
		g_graphicsOptions->CellTextOn(PLAYER_UNASSIGNED);
	}
}

void ToggleArmyText::Execute(sint32 argc, char **argv)
{
	if (g_graphicsOptions->IsArmyTextOn()) {
		g_graphicsOptions->ArmyTextOff();
	} else {
		g_graphicsOptions->ArmyTextOn();
	}
}

void ToggleArmyName::Execute(sint32 argc, char **argv)
{
	if (g_graphicsOptions->IsArmyNameOn()) {
		g_graphicsOptions->ArmyNameOff();
	} else {
		g_graphicsOptions->ArmyNameOn();
	}
}

void ArmyClumps::Execute(sint32 argc, char **argv)
{
	if (g_drawArmyClumps) {
		g_drawArmyClumps = FALSE;
	} else {
		g_drawArmyClumps = TRUE;
	}
}

void ExportMapCommand::Execute(sint32 argc, char **argv)
{
	if (argc != 2) return;

	MapFile mf;
	mf.Save(argv[1]);
}

void ImportMapCommand::Execute(sint32 argc, char **argv)
{
	if (argc != 2) return;

	MapFile mf;
	if(mf.Load(argv[1])) {
		g_tiledMap->PostProcessMap(TRUE);
		g_tiledMap->Refresh();
	}
}

void ResetVisionCommand::Execute(sint32 argc, char **argv)
{
	if(g_player[g_selected_item->GetVisiblePlayer()]) {
		g_player[g_selected_item->GetVisiblePlayer()]->m_vision->SetTheWholeWorldUnexplored();
	}
}

void SetGoodsCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

    sint32 type;
	if (argc < 2)
		return;

	type= atoi(argv[1]);

	g_theWorld->SetGood(point.x, point.y, type);

	g_tiledMap->PostProcessTile(point, g_theWorld->GetTileInfo(point));
	g_tiledMap->TileChanged(point);
}

#ifdef DUMP_ASTAR

#include "debugcallstack.h"
void DumpAstarCommand::Execute(sint32 argc, char **argv)
{
   	extern cDebugCallStackSet g_astarCallStackSet;
	g_astarCallStackSet.Dump(argv[1]);
}
#endif

void AiDebugCommand::Execute(sint32 argc, char **argv)
{
	sint32 goal_type;
	PLAYER_INDEX debug_player;
	CellUnitList unit_list;
	MapPoint pos;
	g_tiledMap->GetMouseTilePos(pos);

	if (argc > 2)
	{
		goal_type = atoi(argv[1]);
		debug_player = atoi(argv[2]);

		CtpAiDebug::SetDebugGoalType(goal_type);
		CtpAiDebug::SetDebugPlayer(debug_player);

		unit_list.Clear();
		CtpAiDebug::SetDebugArmies(unit_list);

		return;
	}
	else
	{
		CtpAiDebug::SetDebugGoalType(-1);
	}

 	Cell *cell = g_theWorld->GetCell(pos);

	debug_player = cell->GetOwner();
	g_theWorld->GetArmy(pos, unit_list);

	if (unit_list.Num() <= 0)
	{

		CtpAiDebug::SetDebugPlayer(debug_player);
	}
	else
	{

		CtpAiDebug::SetDebugPlayer(unit_list.Get(0).GetOwner());
	}

	CtpAiDebug::SetDebugArmies(unit_list);
}

#include "GoodSpriteGroup.h"
#include "UnitSpriteGroup.h"

void CleanSpritesCommand::Execute(sint32 argc, char **argv)
{
	sint32		i;
	MBCHAR		*unitFileNames[] = {
					"GU01.SPR",
					"GU02.SPR",
					"GU03.SPR",
					"GU04.SPR",
					"GU05.SPR",
					"GU06.SPR",
					"GU07.SPR",
					"GU08.SPR",
					"gu09.spr",
					"GU10.SPR",
					"GU11.SPR",
					"GU12.SPR",
					"GU13.SPR",
					"GU14.SPR",
					"GU15.SPR",
					"GU16.SPR",
					"GU17.SPR",
					"GU19.SPR",
					"GU20.SPR",
					"GU21.SPR",
					"GU22.SPR",
					"GU23.SPR",
					"GU24.SPR",
					"GU25.SPR",
					"GU27.SPR",
					"GU28.SPR",
					"GU30.SPR",
					"GU31.SPR",
					"GU33.SPR",
					"GU34.SPR",
					"GU35.SPR",
					"GU36.SPR",
					"GU37.SPR",
					"GU38.SPR",
					"GU39.SPR",
					"GU40.SPR",
					"GU41.SPR",
					"GU42.SPR",
					"GU44.SPR",
					"GU46.SPR",
					"GU47.SPR",
					"GU48.SPR",
					"GU49.SPR",
					"GU50.SPR",
					"GU51.SPR",
					"GU53.SPR",
					"GU54.SPR",
					"GU55.SPR",
					"GU56.SPR",
					"GU57.SPR",
					"GU58.SPR",
					"GU59.SPR",
					"GU60.SPR",
					"GU62.SPR",
					"GU64.SPR",
					"GU65.SPR",
					"GU66.SPR",
					"GU68.SPR",
					"GU69.SPR",
					"GU70.SPR",
					"GU71.SPR",
					"GU73.SPR",
					"GU74.SPR",
					"GU75.SPR",
					"GU76.SPR",
					"GU77.SPR",
					"GU78.SPR",
					"GU79.SPR",
					"GU80.SPR",
					"GU81.SPR",
					"GU87.SPR",
					"GU88.SPR",
					"GU89.SPR",
					"GU90.SPR",
					"GU91.SPR",
					"GU92.SPR",
					"GU93.SPR",
				};
	sint32		numUnits  = 77;

	MBCHAR		*cityFileNames[] = {
						"GC001.SPR",
						"GC002.SPR",
						"GC003.SPR",
						"GC004.SPR",
						"GC005.SPR",
						"GC006.SPR",
						"GC007.SPR",
						"GC008.SPR",
						"GC009.SPR",
						"GC010.SPR",
						"GC011.SPR",
						"GC012.SPR",
						"GC013.SPR",
						"GC014.SPR",
						"GC015.SPR",
						"GC016.SPR",
						"GC017.SPR",
						"GC018.SPR",
						"GC019.SPR",
						"GC020.SPR",
						"GC021.SPR",
						"GC022.SPR",
						"GC023.SPR",
						"GC024.SPR",
						"GC025.SPR",
						"GC026.SPR",
						"GC027.SPR",
						"GC028.SPR",
						"GC029.SPR",
						"GC030.SPR",
						"GC031.SPR",
						"GC032.SPR",
						"GC033.SPR",
						"GC034.SPR",
						"GC035.SPR",
						"GC036.SPR",
						"GC037.SPR",
						"GC038.SPR",
						"GC039.SPR",
						"GC040.SPR",
						"GC041.SPR",
						"GC042.SPR",
						"GC043.SPR",
						"GC044.SPR",
						"GC045.SPR",
						"GC046.SPR",
						"GC047.SPR",
						"GC048.SPR",
						"GC049.SPR",
						"GC050.SPR",
						"GC051.SPR",
						"GC052.SPR",
						"GC053.SPR",
						"GC054.SPR",
						"GC055.SPR",
						"GC056.SPR",
						"GC057.SPR",
						"GC058.SPR",
						"GC059.SPR",
						"GC060.SPR",
						"GC061.SPR",
						"GC062.SPR",
						"GC063.SPR",
						"GC064.SPR",
						"GC065.SPR",
				};
	sint32		numCities = 65;

	MBCHAR		*goodFileNames[] = {
						"GG00.SPR",
						"GG01.SPR",
						"GG02.SPR",
						"GG03.SPR",
						"GG04.SPR",
						"GG05.SPR",
						"GG06.SPR",
						"GG07.SPR",
						"GG08.SPR",
						"GG09.SPR",
						"GG10.SPR",
						"GG11.SPR",
						"GG12.SPR",
						"GG13.SPR",
						"GG14.SPR",
						"GG15.SPR",
						"GG16.SPR",
						"GG17.SPR",
						"GG18.SPR",
				};
	sint32		numGoods = 19;

	UnitSpriteGroup		*usg;
	GoodSpriteGroup		*gsg;

	MBCHAR *name;
	MBCHAR saveName[_MAX_PATH];

	for (i = 0; i < numUnits; i++)
	{
		usg = new UnitSpriteGroup(GROUPTYPE_UNIT);
		name = unitFileNames[i];
		usg->LoadFull(name);
		sprintf(saveName, "Output%s%s", FILE_SEP, name);
		usg->Save(saveName,k_SPRITEFILE_VERSION0,SPRDATA_REGULAR);
		delete usg;
	}

	for (i = 0; i < numGoods; i++)
	{
		gsg = new GoodSpriteGroup(GROUPTYPE_GOOD);
		name = goodFileNames[i];
		gsg->LoadFull(name);
		sprintf(saveName, "Output%s%s", FILE_SEP, name);
		gsg->Save(saveName,k_SPRITEFILE_VERSION0,SPRDATA_REGULAR);
		delete gsg;
	}

	for (i = 0; i < numCities; i++)
	{
		usg = new UnitSpriteGroup(GROUPTYPE_CITY);
		name = cityFileNames[i];
		usg->LoadFull(name);
		sprintf(saveName, "Output" FILE_SEP "%s", name);
		usg->Save(saveName,k_SPRITEFILE_VERSION0,SPRDATA_REGULAR);
		delete usg;
	}
}

void CleanScreenCommand::Execute( sint32 argc, char **argv )
{
	Assert( argc == 2 );
	if ( argc != 2 ) return;

	if ( !strcmp(argv[1], "city") ) {

		NationalManagementDialog::Cleanup();
	}
	else if ( !strcmp(argv[1], "unit") ) {

	}
	else if ( !strcmp(argv[1], "sci") ) {
		ScienceManagementDialog::Cleanup();
	}
	else if ( !strcmp(argv[1], "dip") ) {

	}
	else if ( !strcmp(argv[1], "lib") ) {
		greatlibrary_Cleanup();
	}
	else if ( !strcmp(argv[1], "rank") ) {
		infowin_Cleanup();
	}
	else if ( !strcmp(argv[1], "opt") ) {
		optionsscreen_Cleanup();
	}
	else if ( !strcmp(argv[1], "kb") ) {
		km_screen_Cleanup();
	}
}

void ResyncCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	if(g_network.IsHost()) {
		g_network.Resync(atoi(argv[1]));
	}
}

void DirectorDumpCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG
	if (g_director)
		g_director->DumpInfo();
#endif
}

void SlicVariableCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc >= 2);
	if(argc >= 2) {
		SlicSymbolData *sym = g_slicEngine->GetSymbol(argv[1]);
		if(sym) {
			if(g_debugWindow) {
				Unit u;
				MapPoint pos;
				char buf[1024];
				if(sym->GetUnit(u)) {
					sprintf(buf, "%s=Unit    %lx", sym->GetName(), u.m_id);
					if(g_theUnitPool->IsValid(u)) {
						sprintf(buf + strlen(buf), ", Type=%d", u.GetType());
					} else {
						sprintf(buf + strlen(buf), " [Invalid unit]");
					}
				} else if(sym->GetCity(u)) {
					sprintf(buf, "%s=City    %lx", sym->GetName(), u.m_id);
					if(!g_theUnitPool->IsValid(u)) {
						strcat(buf, " [Invalid city]");
					} else {
						strcat(buf, " ");
						strcat(buf, u.GetData()->GetCityData()->GetName());
					}
				} else if(sym->GetPos(pos)) {
					sprintf(buf, "%s=Point   (%d,%d)", sym->GetName(),
							pos.x, pos.y);
				} else {
					sint32 value;
					sym->GetIntValue(value);
					sprintf(buf, "%s=Integer %d", sym->GetName(), value);
				}
				g_debugWindow->AddText(buf);
			}
		}
	}
}

void MultiCycleCommand::Execute(sint32 argc, char **argv)
{
	g_theProfileDB->SetAutoEndMultiple(!g_theProfileDB->IsAutoEndMulitpleTurns());
}

void CombatLogCommand::Execute(sint32 argc, char **argv)
{
	g_theProfileDB->SetCombatLog(!g_theProfileDB->IsCombatLog());
}

void RedrawMapCommand::Execute(sint32 argc, char **argv)
{
		g_tiledMap->Refresh();
		g_tiledMap->InvalidateMap();
}











void CopyVisionCommand::Execute(sint32 argc, char **argv)
{
	g_tiledMap->CopyVision();
}

void AutoGroupCommand::Execute(sint32 argc, char **argv)
{
	g_theProfileDB->SetAutoGroup(!g_theProfileDB->IsAutoGroup());
}

void BoardCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_ARMY) {
		Army army(item);
		army.AddOrders(UNIT_ORDER_BOARD_TRANSPORT);
	}
}

void KillPopCommand::Execute(sint32 argc, char **argv)
{
}

void AttachCommand::Execute(sint32 argc, char **argv)
{




	sint32 player = g_selected_item->GetVisiblePlayer();
	if(argc > 1) {
		player = atoi(argv[1]);
	}

	if(g_network.IsActive()) {
		Assert(g_network.IsLocalPlayer(player));
		if(!g_network.IsLocalPlayer(player))
			return;

		if(g_network.IsClient() && !g_network.IsMyTurn())
			return;
	}

	if (g_player[player])
		g_player[player]->m_playerType = PLAYER_TYPE_ROBOT;

}

void DetachCommand::Execute(sint32 argc, char **argv)
{




	sint32 player = g_selected_item->GetVisiblePlayer();
	if(argc > 1) {
		player = atoi(argv[1]);
	}

	if(g_network.IsActive()) {
		Assert(g_network.IsLocalPlayer(player));
		if(!g_network.IsLocalPlayer(player))
			return;
		if(!g_network.IsHost())
			g_player[player]->m_playerType = PLAYER_TYPE_NETWORK;
		else
			g_player[player]->m_playerType = PLAYER_TYPE_HUMAN;
	} else {
		if(g_player[player])
			g_player[player]->m_playerType = PLAYER_TYPE_HUMAN;
	}

}


void ToggleShieldSupport::Execute(sint32 argc, char **argv)

{


}

void DisbandCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_ARMY) {
		Army army(item);
		army.AddOrders(UNIT_ORDER_DISBAND);
	} else if(state == SELECT_TYPE_LOCAL_CITY) {
		if(g_selected_item->GetCurPlayer() == player) {
			Unit city(item);
			city.DisbandCity();
		}
	}
}

void SendSlaveCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_CITY) {
		Unit fromCity = Unit(item);
		Assert(fromCity.GetOwner() == g_selected_item->GetVisiblePlayer());
		if(fromCity.GetOwner() != g_selected_item->GetVisiblePlayer())
			return;

		MapPoint pos;
		g_tiledMap->GetMouseTilePos(pos);
		Cell *cell = g_theWorld->GetCell(pos);
		Unit toCity = cell->GetCity();
		Assert(toCity != Unit());

		if(toCity != Unit()) {
			Assert(toCity.GetOwner() == fromCity.GetOwner());
			if(toCity.GetOwner() != fromCity.GetOwner())
				return;

			fromCity.SendSlaveTo(toCity);
		}
	}
}

void ScoreCommand::Execute(sint32 argc, char **argv)
{
	sint32 i;
	Score *score = g_player[g_selected_item->GetVisiblePlayer()]->m_score;
	for(i = 0; i < SCORE_CAT_MAX; i++) {
		DPRINTF(k_DBG_GAMESTATE, ("%s: %d\n", score->GetScoreString((SCORE_CATEGORY)i),
							 score->GetPartialScore((SCORE_CATEGORY)i)));
	}
	DPRINTF(k_DBG_GAMESTATE, ("Total: %d\n", score->GetTotalScore()));
}

void EmailCommand::Execute(sint32 argc, char **argv)
{
	g_turn->SetHotSeat(FALSE);
	g_turn->SetEmail(!g_turn->IsEmail());
}

void HotSeatCommand::Execute(sint32 argc, char **argv)
{
	g_turn->SetEmail(FALSE);
	g_turn->SetHotSeat(!g_turn->IsHotSeat());
}

extern BOOL g_toeMode;

void ToeCommand::Execute(sint32 argc, char **argv)
{
	g_toeMode = !g_toeMode;
}

void ForceRevoltCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	Cell *cell = g_theWorld->GetCell(point);
	if(cell->GetCity() != Unit()) {
		cell->GetCity().AccessData()->GetCityData()->Revolt(g_player[cell->GetCity().GetOwner()]->m_civRevoltingCitiesShouldJoin, TRUE);
	}
}

void NearFortCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_ARMY) {
		Army army = Army(item);
		Unit aUnit = army[0];
		aUnit.GiveCommand(UNIT_COMMAND_NEAREST_FORT);
	}
}

void NearCityCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_ARMY) {
		Army army = Army(item);
		Unit aUnit = army[0];
		aUnit.GiveCommand(UNIT_COMMAND_NEAREST_CITY);
	}
}

void DescendCommand::Execute(sint32 argc, char **argv)
{
	g_selected_item->Descend();
}

void FZCommentCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG
	MBCHAR str[_MAX_PATH];
	sint32 pos = 0;

	for(sint32 i = 1; i < argc; i++) {
		if(pos + strlen(argv[i]) >= _MAX_PATH)
			break;
		memcpy(&str[pos], argv[i], strlen(argv[i]));
		pos += strlen(argv[i]);
		str[pos++] = ' ';
	}
	str[pos] = 0;




#endif
}

void FliLogCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;


}

void ReloadFliCommand::Execute (sint32 argc, char **argv)
{
#if 0   /// @todo Find out what this code was supposed to do
    Assert(argc == 2)
    if (argc != 2) return;

    PLAYER_INDEX p = atoi (argv[1]);
#endif
}

void RobotMessagesCommand::Execute(sint32 argc, char **argv)
{
	g_robotMessages = !g_robotMessages;
}

void SellImprovementsCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_player[g_selected_item->GetVisiblePlayer()]->TradeImprovementsForPoints(point);
}

void SellUnitsCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_player[g_selected_item->GetVisiblePlayer()]->TradeUnitsForPoints(point);
}

void ReadyCommand::Execute(sint32 argc, char **argv)
{
	if(g_network.IsActive()) {
		g_network.SignalSetupDone(g_selected_item->GetVisiblePlayer());
	} else {
		g_powerPointsMode = FALSE;
	}
}

void SetupModeCommand::Execute(sint32 argc, char **argv)
{
	if(!g_network.IsActive() || g_network.IsHost()) {
		g_network.EnterSetupMode();
		if(!g_network.IsHost() && !g_network.IsClient()) {
			g_powerPointsMode = TRUE;
			g_player[g_selected_item->GetVisiblePlayer()]->m_doneSettingUp = FALSE;
			g_player[g_selected_item->GetVisiblePlayer()]->SetPoints(g_theProfileDB->PowerPoints());
		}
	}
}

void SetCarryoverStyleCommand::Execute(sint32 argc, char **argv)
{
	if(!g_network.IsActive() || g_network.IsHost()) {
		g_network.SetCarryoverStyle(TRUE);
	}
}

void SetTimedStyleCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc >= 2 && argc <= 3);
	if(argc < 2 || argc > 3)
		return;

	sint32 m = atoi(argv[1]);

	sint32 s = 0;
	if(argc >= 3) {
		s = atoi(argv[2]);
	}

	if(!g_network.IsActive() || g_network.IsHost()) {
		g_network.SetTimedStyle(TRUE, m * 60 + s);
	}
}

void SetSpeedStyleCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc >= 2 && argc <= 3);
	if(argc < 2 || argc > 3)
		return;

	sint32 perCity;
	if(argc == 3) {
		perCity = atoi(argv[2]);
	} else {
		perCity = 0;
	}
	if(!g_network.IsActive() || g_network.IsHost()) {
		g_network.SetSpeedStyle(TRUE, atoi(argv[1]), FALSE, perCity);
	}
}

void SetClassicStyleCommand::Execute(sint32 argc, char **argv)
{
	if(!g_network.IsActive() || g_network.IsHost()) {
		g_network.SetClassicStyle();
	}
}

void SetUnitMovesStyleCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	if(!g_network.IsActive() || g_network.IsHost()) {
		g_network.SetUnitMovesStyle(TRUE, atoi(argv[1]));
	}
}

void AutoCenterCommand::Execute(sint32 argc, char **argv)
{
	if(argc > 1) {
		g_selected_item->SetAutoCenter(atoi(argv[1]) != 0);
	} else {
		g_selected_item->SetAutoCenter(!g_selected_item->IsAutoCenterOn());
	}
}

void SimultaneousCommand::Execute(sint32 argc, char **argv)
{
	g_turn->SetSimultaneousMode(!g_turn->SimultaneousMode());
}

void TutorialCommand::Execute(sint32 argc, char **argv)
{

	if(argc > 1) {
		g_slicEngine->SetTutorialPlayer(atoi(argv[1]));
		g_theProfileDB->SetTutorialAdvice( TRUE );
	} else {
		g_theProfileDB->SetTutorialAdvice( !g_slicEngine->GetTutorialActive() );
	}

}

void UseLadderCommand::Execute(sint32 argc, char **argv)
{
	g_selected_item->UseSpaceLadder();
}

void InjoinCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Injoin(point);
}

void InstantMessageCommand::Execute(sint32 argc, char **argv)
{
	extern sint32 g_hackInstantMessages;
	g_hackInstantMessages = !g_hackInstantMessages;
}

void ZBCommand::Execute(sint32 argc, char **argv)
{
	c3errors_ErrorDialog("Zoomed Battlefield", "Invoke it here.");
}

void ShowPopCommand::Execute(sint32 argc, char **argv)
{
    // Didn't do anything
}

void HidePopCommand::Execute(sint32 argc, char **argv)
{
    // Didn't do anything
}

void AddPopCommand::Execute(sint32 argc, char **argv)

{
    PLAYER_INDEX player;
    ID item;
	SELECT_TYPE state;

    g_selected_item->GetTopCurItem(player, item, state);

    if (SELECT_TYPE_LOCAL_CITY == state) {

        sint32 num;
        if (2 == argc) {
            num = atoi(argv[1]);
        } else {
            num = 1;
        }

        Unit city = item;
        MapPoint pos;
        sint32 i;
        for (i=0; i<num; i++) {
            city.GetPos(pos);
            g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
								   GEA_City, city,
								   GEA_End);

        }

		for (i=0; i > num; i--) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
								   GEA_City, city,
								   GEA_End);
		}

		sint32 total_pop;
		city.CD()->GetPop(total_pop);
		if ( total_pop + num <= 0) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillCity,
								   GEA_City, city,
								   GEA_Int, 0,
								   GEA_Player, -1,
								   GEA_End);
		}
    }
}

void YumCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	Cell *cell = g_theWorld->GetCell(point);
	sint32 good;
	if(!cell->GetGoodsIndex(good))
		good = -1;
	DPRINTF(k_DBG_INFO, ("(%d,%d): Prod: %d, Food: %d, Good: %d\n",
						 point.x, point.y,
						 g_theWorld->GetShieldsProduced(point),
						 g_theWorld->GetFoodProduced(point),
						 good));
}

void ToggleMapColorCommand::Execute(sint32 argc, char **argv)
{
#if defined(_DEBUG) && defined(CELL_COLOR)
    g_is_debug_map_color++;
	if(g_is_debug_map_color > 2 || g_is_debug_map_color < 0)
		g_is_debug_map_color = 0;

	if(g_is_debug_map_color == 2) {
		MapPoint pos;
		MapPoint *size;
		size = g_theWorld->GetSize();
		Cell *c;
		for (pos.x = 0; pos.x < size->x; pos.x++) {
			for (pos.y=0; pos.y < size->y; pos.y++) {
				c = g_theWorld->AccessCell(pos);
				if (c->GetRawZoc()) {
					c->m_color = 1;
				} else {
					c->m_color = 0;
				}
			}
		}
	}

    WhackScreen();
#endif
}

void ToggleHeuristicCommand::Execute(sint32 argc, char **argv)
{
}

void GetAdvanceCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->SetResearching(atoi(argv[1]));
}

void SlicCommand::Execute(sint32 argc, char **argv)
{
	char buf[2048];
	sint32 i;
	buf[0] = 0;
	for(i = 1; i < argc; i++) {
		strcat(buf, argv[i]);
		strcat(buf, " ");
	}

	char outputBuf[1024];
	outputBuf[0] = 0;
	char catString = 0;
	if(strnicmp(argv[0], "slic", strlen(argv[0])) == 0) {
		sint32 res;
		res = sliccmd_parse(SLICCMD_WATCH, buf, outputBuf, 1024, 0, &catString);
		DPRINTF(k_DBG_SLIC, ("%s\n", outputBuf));
	} else {
		sliccmd_parse(SLICCMD_ASSN, buf, outputBuf, 1024, 1, &catString);
		DPRINTF(k_DBG_SLIC, ("%s\n", outputBuf));
	}
}













void TestMessageCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc==1);
	if (argc!=1)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->SendTestMessage();
	}

void HowLongCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	Assert(state == SELECT_TYPE_LOCAL_CITY);
	if(state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit c(item);
	DPRINTF(k_DBG_GAMESTATE, ("Approximate turns remaining: %d\n",
							  c.HowMuchLonger()));
}

void DebugCheckMemCommand::Execute(sint32 argc, char **argv)
{
    g_check_mem = !g_check_mem;

    if (g_check_mem) {
        sint32 * tmp;
        tmp = new sint32 [10];
        delete[] tmp;

    }
}

void PacCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player = g_selected_item->GetVisiblePlayer();

	MapPoint pos;
	g_tiledMap->GetMouseTilePos(pos);

	Unit newu = g_player[player]->CreateUnit(g_theUnitDB->NumRecords() - 1,
											 pos, Unit(),
											 FALSE, CAUSE_NEW_ARMY_INITIAL);
	newu.AccessData()->SetPacMan();
}

void GivesWhatCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;
	AdvanceType adv = atoi(argv[1]);
	const AdvanceRecord *rec = g_theAdvanceDB->Get(adv);

	DPRINTF(k_DBG_GAMESTATE, ("Advance %d(%s) gives:\n", adv,
							  g_theStringDB->GetNameStr(rec->m_name)));

	sint32 i, n = g_theUnitDB->NumRecords();
	for(i = 0; i < n; i++) {
		if(g_theUnitDB->Get(i)->GetEnableAdvanceIndex() == adv) {
			DPRINTF(k_DBG_GAMESTATE, ("Unit: %d(%s)\n", i, g_theStringDB->GetNameStr(g_theUnitDB->Get(i)->m_name)));
		}
	}

	n = g_theBuildingDB->NumRecords();
	for(i = 0; i < n; i++) {
		if(g_theBuildingDB->Get(i)->GetEnableAdvanceIndex() == adv) {
			DPRINTF(k_DBG_GAMESTATE, ("Improvement: %d(%d)\n", i, g_theStringDB->GetNameStr(g_theBuildingDB->Get(i)->m_name)));
		}
	}

	n = g_theWonderDB->NumRecords();
	for(i = 0; i < n; i++) {
		if(g_theWonderDB->Get(i)->GetEnableAdvanceIndex() == adv) {
			DPRINTF(k_DBG_GAMESTATE, ("Wonder: %d(%s)\n", i, g_theStringDB->GetNameStr(g_theWonderDB->Get(i)->m_name)));
		}
	}
}

void OvertimeCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	Assert(state == SELECT_TYPE_LOCAL_CITY);
	if(state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit c(item);
	c.BuyFront();
}

void OvertimeCostCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	Assert(state == SELECT_TYPE_LOCAL_CITY);
	if(state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit c(item);
	DPRINTF(k_DBG_GAMESTATE, ("Overtime cost: %d\n", c.GetOvertimeCost()));
}

void LearnWhatCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player = g_selected_item->GetVisiblePlayer();
	uint8 *advances = g_player[player]->m_advances->CanResearch();
	sint32 i, n = g_theAdvanceDB->NumRecords();
	DPRINTF(k_DBG_GAMESTATE, ("Can research:\n"));
	for(i = 0; i < n; i++) {
		if(advances[i]) {
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)", i,
									  g_theStringDB->GetNameStr(g_theAdvanceDB->Get(i)->GetName())));
		}
	}
}

void BuildWhatCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	Assert(state == SELECT_TYPE_LOCAL_CITY);
	if(state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit c(item);
	c.AccessData()->GetCityData()->BuildWhat();
}

void CreateRiftCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->CreateRift(point);
}

void CreateParkCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->CreatePark(point);
}

void RustleCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Rustle(point);
}

void UncloakCommand::Execute(sint32 argc, char **argv)
{
	g_selected_item->Uncloak();
}

void CloakCommand::Execute(sint32 argc, char **argv)
{
	g_selected_item->Cloak();
}









void SoothsayCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Soothsay(point);
}

void IndulgenceCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->IndulgenceSale(point);
}

void ReformCityCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->ReformCity(point);
}

void ConvertCityCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->ConvertCity(point);
}

void BioInfectCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->BioInfect(point);
}

void NanoInfectCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->NanoInfect(point);
}

void InciteUprisingCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->InciteUprising(point);
}

void UndergroundRailwayCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->UndergroundRailway(point);
}

void EnslaveSettlerCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->EnslaveSettler(point);
}

void SlaveRaidCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->SlaveRaid(point);
}

void PlantNukeCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->PlantNuke(point);
}

void CauseUnhappinessCommand::Execute(sint32 argc, char **argv)
{




}

void ExpelCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Expel(point);
}

void SueCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Sue(point);
}

void SueFranchiseCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->SueFranchise(point);
}

void FranchiseCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Franchise(point);
}

void HearGossipCommand::Execute(sint32 argc, char **argv)
{
	BOOL DontUseThisCommandItSucks = FALSE;
	Assert(DontUseThisCommandItSucks);
	return;
#if 0   // Unreachable
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->m_all_units->Access(0).AccessData()->HearGossip(
		g_player[atoi(argv[1])]->m_all_units->Access(0));
#endif
}

void BombardCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->Bombard(point);
}

void GrantAdvanceCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	if(g_network.IsClient()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_GRANT_ADVANCE,
		                                 atoi(argv[1])));
	}

	g_player[g_selected_item->GetVisiblePlayer()]->m_advances->GiveAdvance(atoi(argv[1]), CAUSE_SCI_UNKNOWN);
}

void GrantAllCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 1);

	if(g_network.IsClient()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_GRANT_ALL));
	}

	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
		g_player[g_selected_item->GetVisiblePlayer()]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
	}
}

void GrantManyCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 1);

	Assert(!g_network.IsActive());
	if(g_network.IsActive())
		return;


	for(sint32 i = 0; i < 64; i++) {
		g_player[g_selected_item->GetVisiblePlayer()]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
	}
}

void InvestigateCityCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->InvestigateCity(point);
}

void NullifyWallsCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->NullifyWalls(point);
}

void StealTechnologyCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->StealTechnology(point);
}

void InciteRevolutionCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->InciteRevolution(point);
}

void AssassinateRulerCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->AssassinateRuler(point);
}

void InvestigateReadinessCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	g_selected_item->InvestigateReadiness(point);
}

void CreateImprovementCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    player;
	ID              item;
	SELECT_TYPE     state;
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_selected_item->GetTopCurItem(player, item, state);
	Assert(state == SELECT_TYPE_LOCAL_CITY);
	if(state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit c(item);
	c.AccessData()->m_city_data->m_built_improvements |= ((uint64)1 << atoi(argv[1]));
	if(g_network.IsClient()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_CREATE_IMPROVEMENT,
		                                 (uint32)c, atoi(argv[1])));
	} else if(g_network.IsHost()) {
		g_network.Enqueue(c.AccessData(), c.AccessData()->m_city_data);
	}

}

void SpewUnitsCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	gameinit_SpewUnits(g_selected_item->GetVisiblePlayer(),
	                   point);
}

void DebugMaskCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG
	extern uint32 g_debug_mask;
	Assert(argc == 2);
	if(argc != 2)
		return;

	uint32 bit = atoi(argv[1]);
	if(g_debug_mask & (1 << bit))
		g_debug_mask &= ~(1 << bit);
	else
		g_debug_mask |= (1 << bit);
#endif
}

void ShowOwnerCommand::Execute(sint32 argc, char **argv)
{

}

void SetGovernmentCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->SetGovernmentType(atoi(argv[1]));
}

void PopCommand::Execute(sint32 argc, char **argv)
{
}

void ShowNetworkStatsCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG
	g_network.TogglePacketLog();
#endif
}

void BuildWonderCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;






	PLAYER_INDEX    player;
	ID              item;
	SELECT_TYPE     state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_CITY) {
		g_player[player]->BuildWonder(atoi(argv[1]),
		                              Unit(item));
	}
}

sint32 g_disableDatacheck = 1;
void DatacheckCommand::Execute(sint32 argc, char **argv)
{
	g_disableDatacheck = !g_disableDatacheck;
}

void ChatMaskCommand::Execute(sint32 argc, char **argv)
{
	uint32 mask = 0;

	for(sint32 i = 1; i < argc; i++) {
		mask |= (1 << atoi(argv[i]));
	}

	g_network.SetChatMask(mask);
}

void ChatCommand::Execute(sint32 argc, char **argv)
{
	MBCHAR str[k_MAX_CHAT_LEN];
	sint32 pos = 0;

	for(sint32 i = 1; i < argc; i++) {
		if(pos + strlen(argv[i]) >= k_MAX_CHAT_LEN)
			break;
		memcpy(&str[pos], argv[i], strlen(argv[i]));
		pos += strlen(argv[i]);
		str[pos++] = ' ';
	}
	str[pos] = 0;
	g_network.SendChatText(str, strlen(str));
}

void ReadinessCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->SetReadinessLevel(
		(READINESS_LEVEL)atoi(argv[1]));
}

void TerrainImprovementCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc >= 2 && argc <= 3);
	if(argc < 2 || argc > 3)
		return;

	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

	TERRAIN_IMPROVEMENT imp = (TERRAIN_IMPROVEMENT)atoi(argv[1]);

	sint32 vplayer = g_selected_item->GetVisiblePlayer();

	if(argc == 3) {
		g_player[vplayer]->CreateImprovement(imp,
		                                     point,
		                                     atoi(argv[2]));
	} else {
		Assert(argc == 2);
		g_gevManager->AddEvent(GEV_INSERT_Tail,
		                       GEV_CreateImprovement,
		                       GEA_Player, vplayer,
		                       GEA_MapPoint, point,
		                       GEA_Int, imp,
		                       GEA_Int, 0,
		                       GEA_End);


	}
}

void TerrainImprovementCompleteCommand::Execute(sint32 argc, char **argv)
{
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);

#if 0   // Unused
	sint32 vplayer = g_selected_item->GetVisiblePlayer();
#endif

	Cell *cell = g_theWorld->GetCell(point);

	for (sint32 i = 0; i < cell->GetNumImprovements(); i++)
		cell->AccessImprovement(i).Complete();
}













void KillTileCommand::Execute(sint32 argc, char **argv)
{
	MapPoint pos;

	Cell     *c;

	Assert(argc==1);
	if (argc != 1)
		return;

	g_tiledMap->GetMouseTilePos(pos);
	c = g_theWorld->GetCell(pos.x, pos.y);
	c->Kill();

	g_tiledMap->PostProcessMap();
	g_tiledMap->Refresh();
}














void SaveBuildQueueCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    player;

	ID              item;

	SELECT_TYPE     state;

	Assert(argc==2);
	if (argc != 2)
		return;

	Assert(argv[1] != NULL);
	Assert(argv[1][0] != '\0');

	g_selected_item->GetTopCurItem(player, item, state);
	if (state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit city(item);
	CityData *cityData = city.GetData()->GetCityData();
	cityData->SaveQueue(argv[1]);
}




















































































































































void LoadBuildQueueCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    player;

	ID              item;

	SELECT_TYPE     state;

	Assert(argc==2);
	if (argc != 2)
		return;

	Assert(argv[1] != NULL);
	Assert(argv[1][0] != '\0');

	g_selected_item->GetTopCurItem(player, item, state);
	if (state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit city(item);
	CityData *cityData = city.GetData()->GetCityData();
	cityData->LoadQueue(argv[1]);
}














void SetCityNameCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    player;

	ID              item;

	SELECT_TYPE     state;

	MapPoint pos;

	Assert(argc==2);
	if (argc != 2)
		return;

	g_selected_item->GetTopCurItem(player, item, state);
	if (state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit city(item);
	city.GetPos(pos);
	g_tiledMap->RedrawTile(&pos);
	CityData *cityData = city.GetData()->GetCityData();
	cityData->SetName(argv[1]);
}

void SetCitySizeCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    player;

	ID              item;

	SELECT_TYPE     state;

	MapPoint pos;

	Assert(argc==2);
	if (argc != 2)
		return;

	g_selected_item->GetTopCurItem(player, item, state);
	if (state != SELECT_TYPE_LOCAL_CITY)
		return;

	sint32           size = atoi(argv[1]);

	Unit city(item);
	city.GetPos(pos);
	g_tiledMap->RedrawTile(&pos);
	CityData *cityData = city.GetData()->GetCityData();
	cityData->SetSize(size);
}

void ToggleAIStr::Execute(sint32 argc, char** argv)
{
	g_show_ai_dbg = !g_show_ai_dbg;
}

#if 0

extern bool g_full_propagate_path;
void FullPathToggle::Execute(sint32 argc, char** argv)
{
	g_full_propagate_path = !g_full_propagate_path;
}
#endif














void RegardCommand::Execute(sint32 argc, char **argv)
{
	sint32          regard;

	PLAYER_INDEX    otherParty;

	Assert(argc==3);
	if (argc != 3)
		return;

	otherParty = (PLAYER_INDEX)(atoi(argv[1]));
	regard = atoi(argv[2]);
	g_player[g_selected_item->GetVisiblePlayer()]->GetRegard()->SetForPlayer(otherParty, (REGARD_TYPE)regard);
}















void AttitudeCommand::Execute(sint32 argc, char **argv)
{
	sint32          attitude;

	PLAYER_INDEX    otherParty;

	Assert(argc==3);
	if (argc != 3)
		return;

	otherParty = (PLAYER_INDEX)(atoi(argv[1]));
	attitude = atoi(argv[2]);
	DPRINTF(k_DBG_INFO, ("Current attitude for player %d is %d\n", otherParty, g_player[g_selected_item->GetVisiblePlayer()]->GetAttitude(otherParty)));
	g_player[g_selected_item->GetVisiblePlayer()]->SetAttitude(otherParty, (ATTITUDE_TYPE)attitude);
	DPRINTF(k_DBG_INFO, ("New attitude for player %d is %d\n", otherParty, attitude));
}

void DumpFZRegardCommand::Execute(sint32 argc, char **argv)
{
	sint32 p;
	if (argc == 1) {
		p = g_selected_item->GetCurPlayer();
	} else if (argc == 2) {
		p = atoi(argv[1]);
	} else {
		Assert(argc <= 2);
		return;
	}




	char out_str[80];
	sint32 i;
	sprintf (out_str, "Player %d regards", p);
	g_chatBox->AddLine(g_selected_item->GetCurPlayer(), out_str);

	for (i=0; i<k_MAX_PLAYERS; i++) {
		if (i == p )continue;
		if (!g_player[i]) continue;





	}
}

void SetFZRegardCommand::Execute(sint32 argc, char **argv)
{
#if 0   /// @todo Find out what this code is supposed to do
    Assert(argc == 4)
    if (argc != 4) return;

    sint32 me = atoi(argv[1]);
    sint32 him = atoi(argv[2]);
    sint32 r = atoi(argv[3]);
#endif
}

void TotalWarCommand::Execute(sint32 argc, char **argv)
{
    sint32 i, j;

    for (i=0; i<k_MAX_PLAYERS; i++) {


        for (j=0; j<k_MAX_PLAYERS; j++) {
            if ((i != j) && g_player[i] && g_player[j]) {

            }
        }
    }

}


































































































































void PactCaptureCityCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    owner,
	                recipient,
	                thirdParty;

	sint32          cityIndex;

	Unit            targetCity;

	Assert(argc==4);
	if (argc != 4)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	thirdParty = (PLAYER_INDEX)(atoi(argv[2]));
	cityIndex = atoi(argv[3]);
	targetCity = g_player[thirdParty]->CityIndexToUnit(cityIndex);
	g_player[owner]->MakeCaptureCityPact(recipient, targetCity);
}











void PactEndPollutionCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX    other_party;

	Assert(argc == 2);
	if (argc != 2)
		return;

	other_party = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[g_selected_item->GetVisiblePlayer()]->MakeEndPollutionPact(other_party);
}











void RevoltCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	player;

	ID	item;

	SELECT_TYPE	state;

	sint32	index;

	MapPoint	p;

	Assert(argc==1);
	if (argc != 1)
		return;

	g_selected_item->GetTopCurItem(player, item, state);
	if (state != SELECT_TYPE_LOCAL_CITY)
		return;

	if (!g_player[player]->GetCityIndex(item, index))
		return;

	g_player[player]->Revolt(index);

	}












void IsViolatingBordersCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	player;

	Assert(argc == 2);
	if (argc != 2)
		return;

	player = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[g_selected_item->GetVisiblePlayer()]->IsViolatingBorders(player);
	}












void IsViolatingPeaceCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	player;

	Assert(argc == 2);
	if (argc != 2)
		return;

	player = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[g_selected_item->GetVisiblePlayer()]->IsViolatingPeace(player);
	}












void IsViolatingCeaseFireCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	player;

	Assert(argc == 2);
	if (argc != 2)
		return;

	player = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[g_selected_item->GetVisiblePlayer()]->WillViolateCeaseFire(player);
	}











void IsPollutionReducedCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 1);
	if (argc != 1)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->IsPollutionReduced();
	}













void MakeCeaseFireCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->MakeCeaseFire(recipient);
	}













void BreakCeaseFireCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->BreakCeaseFire(recipient, TRUE);
	}











void RequestGreetingCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestGreeting(recipient);
	}










void RequestDemandAdvanceCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	AdvanceType	advance;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	advance = (AdvanceType)(atoi(argv[2]));
	g_player[owner]->RequestDemandAdvance(recipient, advance);
	}










void RequestDemandCityCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Unit	city;

	sint32	cityIndex;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	cityIndex = atoi(argv[2]);
	city = g_player[recipient]->CityIndexToUnit(cityIndex);
	g_player[owner]->RequestDemandCity(recipient, city);
	}










void RequestDemandMapCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestDemandMap(recipient);
	}










void RequestDemandGoldCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Gold	amount;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	amount.SetLevel(atoi(argv[2]));
	g_player[owner]->RequestDemandGold(recipient, amount);
	}










void RequestDemandStopTradeCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient,
					thirdParty;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	thirdParty = (PLAYER_INDEX)(atoi(argv[2]));
	g_player[owner]->RequestDemandStopTrade(recipient, thirdParty);
	}










void RequestDemandAttackEnemyCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient,
					thirdParty;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	thirdParty = (PLAYER_INDEX)(atoi(argv[2]));
	g_player[owner]->RequestDemandAttackEnemy(recipient, thirdParty);
	}










void RequestDemandLeaveOurLandsCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestDemandLeaveOurLands(recipient);
	}










void RequestDemandReducePollutionCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestDemandReducePollution(recipient);
	}










void RequestOfferAdvanceCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	AdvanceType	advance;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	advance = (AdvanceType)(atoi(argv[2]));
	g_player[owner]->RequestOfferAdvance(recipient, advance);
	}










void RequestOfferCityCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Unit	city;

	sint32	cityIndex;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	cityIndex = atoi(argv[2]);
	city = g_player[owner]->CityIndexToUnit(cityIndex);
	g_player[owner]->RequestOfferCity(recipient, city);
	}










void RequestOfferMapCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestOfferMap(recipient);
	}










void RequestOfferGoldCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	owner,
					recipient;

	Gold	amount;

	Assert(argc==3);
	if (argc != 3)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	amount.SetLevel(atoi(argv[2]));
	g_player[owner]->RequestOfferGold(recipient, amount);
}










void RequestOfferCeaseFireCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestOfferCeaseFire(recipient);
}










void RequestOfferPermanentAllianceCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestOfferPermanentAlliance(recipient);
	}










void RequestOfferPactCaptureCityCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient,
					thirdParty;

	Unit	city;

	sint32	cityIndex;

	Assert(argc==4);
	if (argc != 4)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	thirdParty = (PLAYER_INDEX)(atoi(argv[2]));
	cityIndex = atoi(argv[3]);
	city = g_player[thirdParty]->CityIndexToUnit(cityIndex);
	g_player[owner]->RequestOfferPactCaptureCity(recipient, city);
	}










void RequestOfferPactEndPollutionCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestOfferPactEndPollution(recipient);
	}







































































































































































































void RequestExchangeAdvanceCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	AdvanceType	advance,
				rewardAdvance;

	Assert(argc==4);
	if (argc != 4)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	advance = (AdvanceType)(atoi(argv[2]));
	rewardAdvance = (AdvanceType)(atoi(argv[3]));
	g_player[owner]->RequestExchangeAdvance(recipient, advance, rewardAdvance);
	}










void RequestExchangeCityCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	sint32	cityIndex;

	Unit	cityA,
			cityB;

	Assert(argc==4);
	if (argc != 4)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	cityIndex = atoi(argv[2]);
	cityA = g_player[recipient]->CityIndexToUnit(cityIndex);
	cityIndex = atoi(argv[3]);
	cityB = g_player[owner]->CityIndexToUnit(cityIndex);
	g_player[owner]->RequestExchangeCity(recipient, cityA, cityB);
	}










void RequestExchangeMapCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	owner,
					recipient;

	Assert(argc==2);
	if (argc != 2)
		return;

	owner = g_selected_item->GetVisiblePlayer();
	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	g_player[owner]->RequestExchangeMap(recipient);
	}











void DumpAgreementsCommand::Execute(sint32 argc, char **argv)
	{
	g_player[g_selected_item->GetVisiblePlayer()]->DumpAgreements();
	}










void DumpMessagesCommand::Execute(sint32 argc, char **argv)
	{
	g_player[g_selected_item->GetVisiblePlayer()]->DumpMessages();
	}











void DumpDiplomaticRequestsCommand::Execute(sint32 argc, char **argv)
	{
	g_player[g_selected_item->GetVisiblePlayer()]->DumpRequests();
	}











void GiveGoldCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	recipient;

	Gold	amount;

	Assert(argc == 3);
	if (argc != 3)
		return;

	recipient = (PLAYER_INDEX)(atoi(argv[1]));
	amount.SetLevel(atoi(argv[2]));
	g_player[g_selected_item->GetVisiblePlayer()]->GiveGold(recipient, amount);
	}











void BequeathGoldCommand::Execute(sint32 argc, char **argv)
	{
	Gold	amount;

	Assert(argc == 2);
	if (argc != 2)
		return;

	if(g_network.IsClient() && !g_network.SetupMode()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_ADD_GOLD,
										 g_selected_item->GetVisiblePlayer(),
										 atoi(argv[1])));
	}

	amount.SetLevel(atoi(argv[1]));
	g_player[g_selected_item->GetVisiblePlayer()]->BequeathGold(amount);
	}










void DumpAlliesCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 1);
	if (argc != 1)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->DumpAllies();
	}











void FormAllianceCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 2);
	if (argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->FormAlliance((PLAYER_INDEX)(atoi(argv[1])));
	}











void BreakAllianceCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 2);
	if (argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->BreakAlliance((PLAYER_INDEX)(atoi(argv[1])));
	}











void ExchangeMapCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 2);
	if (argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->ExchangeMap((PLAYER_INDEX)(atoi(argv[1])));
	}











void GiveMapCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 2);
	if (argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->GiveMap((PLAYER_INDEX)(atoi(argv[1])));
	}











void StopTradingWithCommand::Execute(sint32 argc, char **argv)
	{
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->StopTradingWith((PLAYER_INDEX)(atoi(argv[1])));
	}











void GiveUnitCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	target_player,
					other_player;

	sint32	unit_idx;

	Assert(argc == 4);
	if (argc != 4)
		return;

	target_player = (PLAYER_INDEX)(atoi(argv[1]));
	other_player = (PLAYER_INDEX)(atoi(argv[2]));
	unit_idx = atoi(argv[3]);
	g_player[target_player]->GiveUnit(other_player, unit_idx);
	}











void GiveAdvanceCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	player;
	(void)player;

	MapPoint	p;

	Assert(argc == 3);
	if (argc != 3)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->GiveAdvance((PLAYER_INDEX)(atoi(argv[1])), (AdvanceType)(atoi(argv[2])), CAUSE_SCI_UNKNOWN);
}











void GiveCityCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	other_player,
					player;

	ID	item;

	SELECT_TYPE	state;

	sint32	city_idx;

	MapPoint	p;
	Assert(argc == 2);
	if (argc != 2)
		return;

	g_selected_item->GetTopCurItem(player, item, state);
	if (state != SELECT_TYPE_LOCAL_CITY)
		return;

	Unit u(item);

	other_player = (PLAYER_INDEX)(atoi(argv[1]));
	city_idx = g_player[player]->GetAllCitiesList()->Find(u);
	if (city_idx != -1)
		g_player[player]->GiveCity(other_player, city_idx);

	}











void ExchangeCityCommand::Execute(sint32 argc, char **argv)
	{
	PLAYER_INDEX	other_player,
					player;

	ID	item;

	SELECT_TYPE	state;

	sint32	c1, c2;

	MapPoint	p;
	Assert(argc == 4);
	if (argc != 4)
		return;

	g_selected_item->GetTopCurItem(player, item, state);
	other_player = (PLAYER_INDEX)(atoi(argv[1]));
	c1 = atoi(argv[2]);
	c2 = atoi(argv[3]);
	g_player[player]->ExchangeCity(other_player, c1, c2);
	}











void FloodCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if (argc != 2)
		return;

	if(g_network.IsClient()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_GLOBAL_WARMING,
										 atoi(argv[1])));
	} else {
		g_theWorld->GlobalWarming(atoi(argv[1]));
	}
}










void OzoneCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if (argc != 2)
		return;

	if(g_network.IsClient()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_OZONE_DEPLETION,
										 atoi(argv[1])));
	} else {
		g_theWorld->OzoneDepletion();
	}
}












void TileTypeCommand::Execute(sint32 argc, char** argv)
	{
#if 0
	MapPoint pos;

	MBCHAR	terrainName[k_MAX_NAME_LEN];

	Assert(argc ==1 || argc == 4);
	if(argc != 1 && argc != 4)
		return;

	g_tiledMap->GetMouseTilePos(pos);

	if(argc == 4)
		{
		pos.x = atoi(argv[1]);
		pos.y = atoi(argv[2]);
		pos.z = atoi(argv[3]);
		}

	Cell	*c = g_theWorld->GetCell(pos.x, pos.y, pos.z);

	switch (c->GetTerrainType())
		{
		case TERRAIN_NULL :
			strcpy(terrainName, "NULL");
			break;

		case TERRAIN_FOREST :
			strcpy(terrainName, "Forest");
			break;

		case TERRAIN_PLAINS :
			strcpy(terrainName, "Plains");
			break;

		case TERRAIN_TUNDRA :
			strcpy(terrainName, "Tundra");
			break;

		case TERRAIN_GLACIER :
			strcpy(terrainName, "Glacier");
			break;

		case TERRAIN_GRASSLAND :
			strcpy(terrainName, "Grassland");
			break;

		case TERRAIN_DESERT:
			strcpy(terrainName, "Desert");
			break;

		case TERRAIN_SWAMP:
			strcpy(terrainName, "Swamp");
			break;

		case TERRAIN_JUNGLE:
			strcpy(terrainName, "Jungle");
			break;

		case TERRAIN_MOUNTAIN :
			strcpy(terrainName, "Mountain");
			break;

		case TERRAIN_HILL :
			strcpy(terrainName, "Hill");
			break;

		case TERRAIN_WATER_SHALLOW :
			strcpy(terrainName, "Shallow Water");
			break;

		case TERRAIN_WATER_DEEP :
			strcpy(terrainName, "Deep Water");
			break;

		case TERRAIN_WATER_VOLCANO :
			strcpy(terrainName, "Water Volcano");
			break;

		case TERRAIN_SPACE :
			strcpy(terrainName, "Space");
			break;

		case TERRAIN_WATER_BEACH :
			strcpy(terrainName, "Water Beach");
			break;

		case TERRAIN_WATER_SHELF :
			strcpy(terrainName, "Water Shelf");
			break;

		case TERRAIN_WATER_TRENCH :
			strcpy(terrainName, "Water Trench");
			break;

		case TERRAIN_WATER_RIFT :
			strcpy(terrainName, "Water Rift");
			break;

		case TERRAIN_DEAD :
			strcpy(terrainName, "Dead");
			break;

		case TERRAIN_BROWN_HILL :
			strcpy(terrainName, "Brown Hill");
			break;
		case TERRAIN_BROWN_MOUNTAIN :
			strcpy(terrainName, "Brown Mountain");
			break;
		case TERRAIN_WHITE_HILL :
			strcpy(terrainName, "White Hill");
			break;
		case TERRAIN_WHITE_MOUNTAIN :
			strcpy(terrainName, "White Mountain");
			break;

		case TERRAIN_UNEXPLORED :
			strcpy(terrainName, "Unexplored");
			break;

		default :
			strcpy(terrainName, "Unknown");

		}

	DPRINTF(k_DBG_INFO, ("Tile \"%s\" with environment 0x%x @ %d, %d, %d\n", terrainName, c->GetEnv(), pos.x, pos.y, pos.z));
#endif
	}

void
InterceptCommand::Execute(sint32 argc, char **argv)
{
	PLAYER_INDEX s_player;
	ID s_item;
	SELECT_TYPE s_state;

	Assert(argc == 2);
	if(argc != 2)
		return;

	g_selected_item->GetTopCurItem(s_player, s_item, s_state);
	if(s_state != SELECT_TYPE_LOCAL_ARMY)
		return;

	Army army = Army(s_item);
	army.AddOrders(UNIT_ORDER_INTERCEPT_TRADE);
}

void
WithdrawOfferCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	Assert(g_player[g_selected_item->GetVisiblePlayer()]->GetTradeOffersList()->Num() > atoi(argv[1]));
	if(g_player[g_selected_item->GetVisiblePlayer()]->GetTradeOffersList()->Num() <= atoi(argv[1]))
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->WithdrawTradeOffer(
		g_player[g_selected_item->GetVisiblePlayer()]->GetTradeOffersList()->Get(atoi(argv[1])));
}

void
OfferCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 7);
	if(argc != 7)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->CreateTradeOffer(
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList()->Get(atoi(argv[1])),
		(ROUTE_TYPE)atoi(argv[2]), atoi(argv[3]),
		(ROUTE_TYPE)atoi(argv[4]), atoi(argv[5]),
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList()->Get(atoi(argv[6])));
}

void
AcceptOfferCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 5);
	if(argc != 5)
		return;

	sint32 index = atoi(argv[2]);
	DynamicArray<TradeOffer> *offers = g_player[atoi(argv[1])]->m_tradeOffers;
	Assert(index >= 0);
	Assert(index < offers->Num());
	if(index < 0 || index >= offers->Num())
		return;

	TradeOffer offer = offers->Access(index);
	UnitDynamicArray *cities = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities;
	sint32 city1index = atoi(argv[3]);
	sint32 city2index = atoi(argv[4]);
	Assert(city1index >= 0);
	Assert(city1index < cities->Num());
	Assert(city2index >= 0);
	Assert(city2index < cities->Num());
	if(city1index < 0 || city2index < 0 ||
	   city2index >= cities->Num() ||
	   city1index >= cities->Num()) {
		return;
	}

	Unit city1 = cities->Access(city1index);
	Unit city2 = cities->Access(city2index);
	g_player[g_selected_item->GetVisiblePlayer()]->AcceptTradeOffer(offer, city1, city2);
}

void ShowVictoryCommand::Execute(sint32 argc, char **argv)
{
	open_VictoryWindow();
}

void
ShowOffersCommand::Execute(sint32 argc, char **argv)
{
	g_commandLine.DisplayOffers(TRUE);
	g_debugOwner = k_DEBUG_OWNER_COMMANDLINE;
}

void DisplayMemCommand::Execute(sint32 argc, char **argv)
{
    g_commandLine.DisplayMem();
	g_debugOwner = k_DEBUG_OWNER_COMMANDLINE;
}

void
CityResourcesCommand::Execute(sint32 argc, char **argv)
{
	g_commandLine.DisplayCityResources(TRUE);
	g_debugOwner = k_DEBUG_OWNER_COMMANDLINE;
}

void
UntradeRouteCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	g_player[g_selected_item->GetVisiblePlayer()]->CancelTradeRoute(
		g_theTradePool->GetRouteIndex(atoi(argv[1])));
}

void
TradeRouteCommand::Execute(sint32 argc, char** argv)
{
	Assert(argc == 6);
	if(argc != 6)
		return;

	sint32 s_idx, d_idx;
	sint32 s_plr, d_plr;
    s_plr = g_selected_item->GetVisiblePlayer();
	ROUTE_TYPE sourceType;
	sint32 sourceResource;

	s_idx = atoi(argv[1]);
	sourceType = (ROUTE_TYPE)atoi(argv[2]);
	sourceResource = atoi(argv[3]);
	d_plr = atoi(argv[4]);
	d_idx = atoi(argv[5]);

	UnitDynamicArray *cities = g_player[d_plr]->GetAllCitiesList();
	Unit destCity = cities->Get(d_idx);
	cities = g_player[s_plr]->GetAllCitiesList();
	Unit srcCity = cities->Get(s_idx);

	g_player[s_plr]->CreateTradeRoute(srcCity, sourceType, sourceResource,
									  destCity, s_plr, 0);
}

void ClearTextCommand::Execute(sint32 argc, char **)

{
    g_debugOwner = k_DEBUG_OWNER_NONE;
}

void ToggleFogCommand::Execute(sint32 argc, char **)

{
    g_fog_toggle = !g_fog_toggle;
    WhackScreen();
}

void ToggleSmoothScrollCommand::Execute(sint32 argc, char **)

{
    g_smoothScroll = !g_smoothScroll;
}

#ifdef WIN32
void DumpCallStackCommand::Execute(sint32 argc, char **)

{

    FILE *callstack_file = fopen("CallStack.txt", "w");

	Assert(callstack_file);

	fprintf(callstack_file, "The Stack\n\n%s\n\n___________\n",
		c3debug_StackTrace());

	fclose(callstack_file);
}
#endif

void ToggleQuitFastCommand::Execute(sint32 argc, char **)
{
#if defined(_DEBUG) && defined(WIN32)
	extern bool g_quitfast;

    g_quitfast = !g_quitfast;
#endif
}

void SetAIRevoltCommand::Execute(sint32 argc, char **argv)
{
    BOOL val = TRUE;
    if (argc == 2) {
        val = (BOOL) atoi(argv[1]);
    }

    g_ai_revolt = val;
}

void GodCommand::Execute(sint32 argc, char **)
{
	g_god = !g_god;
	WhackScreen();
}

void ToggleGridCommand::Execute(sint32 argc, char **)
{
	g_isGridOn = !g_isGridOn;
	WhackScreen();
}

void ToggleSpaceCommand::Execute(sint32 argc, char **)
{
}

void ToggleHeraldCommand::Execute(sint32 argc, char **argv)
{
	g_showHeralds = !g_showHeralds;
	WhackScreen();
}

void ShowAdvancesCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG
	g_player[g_selected_item->GetVisiblePlayer()]->DisplayAdvances();
#endif





}

void ToggleWaterCommand::Execute(sint32 argc, char **argv)
{

}

void EndTurnSoundCommand::Execute(sint32 argc, char **argv)
{
	g_theProfileDB->SetEndTurnSound(!g_theProfileDB->GetEndTurnSound());
}

extern sint32 g_is_rand_test;
extern sint32 g_ai_rand_test_wait;
extern sint32 g_ai_rand_test_max;
extern sint32 g_ai_rand_test_total;
void RandTestCommand::Execute(sint32 argc, char** argv)

{
    g_is_rand_test=TRUE;
    g_ai_rand_test_wait = atoi(argv[2]);
    g_ai_rand_test_max = atoi(argv[1]);
    g_ai_rand_test_total = 0;
}

void StayOnCommand::Execute(sint32 argc, char** argv)
{
	g_commandLine.SetPersistence(TRUE);
}

void TurnOffCommand::Execute(sint32 argc, char** argv)
{
	g_commandLine.SetPersistence(FALSE);
    g_clearTextCommand.Execute(argc, argv);

}

void HelpCommand::Execute(sint32 argc, char **argv)
{
	g_commandLine.DisplayHelp(TRUE);
	g_debugOwner = k_DEBUG_OWNER_COMMANDLINE;
	if(argc <= 1) {
		g_commandLine.SetHelpStart(0);
	} else {
		g_commandLine.SetHelpStart(atoi(argv[1]) * k_HELP_LINES);
	}
}

void HelpLinesCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;
	s_helpLines = atoi(argv[1]);
}

extern DataCheck *g_DataCheck;

void DisplayChecksumOffCommand::Execute(sint32 argc, char **argv)
{
	if(g_dataCheck) {
		g_dataCheck->SetDisplay(FALSE);
		g_clearTextCommand.Execute(argc, argv);
	}

}

void DumpChecksumCommand::Execute(sint32 argc, char **argv)
{
	if(g_dataCheck) {
		g_debugOwner = k_DEBUG_OWNER_CRC;
		g_dataCheck->DumpChecksum();
	}
}

void DisplayChecksumCommand::Execute(sint32 argc, char **argv)

{
	g_debugOwner = k_DEBUG_OWNER_CRC;
	if(g_dataCheck) {
		if(argc < 2)
			g_dataCheck->SetDisplay(2);
		else
			g_dataCheck->SetDisplay(atoi(argv[1]));
	}
}

void CalcChecksumCommand::Execute(sint32 argc, char **argv)

{
	if(g_dataCheck) {
		g_dataCheck->BeginTurn();
		g_displayChecksumCommand.Execute(argc, argv);
	}
}

void BuildCommand::Execute(sint32 argc, char** argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_CITY) {
		sint32 type = atoi(argv[1]);
		g_player[player]->BuildUnit(type, Unit(item));
	}
}

void UpgradeCity::Execute(sint32 argc, char** argv)
{
	Assert(argc == 3);
	if(argc != 3)
		return;
	sint32 city_idx = atoi(argv[1]);
	if(city_idx < 0 || city_idx > 9)
		return;
	sint32 upgLevel = atoi(argv[2]);
	sint32 player = g_selected_item->GetVisiblePlayer();
	Unit u = g_player[player]->GetCityFromIndex(city_idx);

	SpriteState *newSS = new SpriteState(90+upgLevel);

	u.GetActor()->ChangeType(newSS, u.GetType(), u);
}

void CreateCommand::Execute(sint32 argc, char** argv)
{
	Assert(argc >= 3 && argc <= 4);
	if(argc < 3 || argc > 4)
		return;

	MapPoint pos;
	g_tiledMap->GetMouseTilePos(pos);

	sint32 city_idx = 0;
	sint32 num_new_units = atoi(argv[1]);
    if (num_new_units < 1)  {
        num_new_units = 1;
    }
	sint32 type = atoi(argv[2]);
	if(type >= g_theUnitDB->NumRecords())
		return;

	PLAYER_INDEX player = g_selected_item->GetVisiblePlayer();

	if(g_theUnitDB->Get(type)->GetHasPopAndCanBuild()) {
		BOOL BiteMe = FALSE;
		Assert(BiteMe);
		return;
	}

	if(argc >= 4) {
		player = atoi(argv[3]);
		Assert((player>=0) && (player<k_MAX_PLAYERS));
		if ((player<0) || (player>=k_MAX_PLAYERS))
			return;
	}

	if(!g_player[player])
		return;

	if (type < 0 || type >= g_theUnitDB->NumRecords())
		return;

    sint32 i;

    for (i=0; i<num_new_units; i++) {
        if(g_network.IsClient()) {
	        g_network.SendCheat(new NetCheat(NET_CHEAT_CREATE_UNIT,
									         city_idx, type, player,
									         pos.x, pos.y));
	        return;
        }

        UnitDynamicArray* unitList = g_player[player]->GetAllCitiesList();
        Unit city;
        if(unitList->Num() > 0) {
	        city = unitList->Get(city_idx);
        } else {
	        city = Unit();
        }

        Unit newu = g_player[player]->CreateUnit(type, pos, city,
										         FALSE, CAUSE_NEW_ARMY_INITIAL);
    }
}

void TaxCommand::Execute(sint32 argc, char** argv)
{

	double s, g, l;
	PLAYER_INDEX player = g_selected_item->GetVisiblePlayer();

	if(1 != sscanf(argv[1], "%lf", &s))
		return;
	if(1 != sscanf(argv[2], "%lf", &g))
		return;
	if(1 != sscanf(argv[3], "%lf", &l))
		return;
	if(argc > 4)
		player = (PLAYER_INDEX)atoi(argv[4]);
	g_player[player]->SetTaxes(s);
}

void SaveCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc==2);
	if (argc!=2)
		return;




	g_isScenario = FALSE;

	GameFile::SaveGame(argv[1], NULL);
}
void RestoreCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc==2);
	if (argc!=2)
		return;

	main_RestoreGame(argv[1]);
}

void LoadAIPCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc==3);
	if (argc!=3)
		return;

	sint32 team_idx;

	team_idx = atoi(argv[2]);


}

void WhoAmICommand::Execute(sint32 argc, char **argv)
{
#if 0   /// @todo Find out what this was supposed to do
    Assert(argc==1)

	char *aipName = NULL;
#endif
}

void LogAICommand::Execute(sint32 argc, char **argv)
{
	Assert(argc==3);
	if (argc!=3)
		return;

	sint32 team_idx;
	sint32 log_level;

	log_level = atoi(argv[1]);
	team_idx = atoi(argv[2]);


}

void DipLogOnCommand::Execute (sint32 argc, char** argv)
{
#ifdef _DEBUG
    if (NULL == g_theDiplomacyLog) {
        g_theDiplomacyLog = new Diplomacy_Log();
    }

    if (argc ==1) {
        g_theDiplomacyLog->LogAllPlayers();
    } else {
        sint32 p = atoi(argv[1]);
        g_theDiplomacyLog->LogPlayer(p);
    }
#endif _DEBUG
}

void DipLogOffCommand::Execute (sint32 argc, char** argv)
{
#ifdef _DEBUG

    if (NULL == g_theDiplomacyLog)
        return;

    if (argc ==1) {
        g_theDiplomacyLog->UnlogAllPlayers();
    } else {
        sint32 p = atoi(argv[1]);
        g_theDiplomacyLog->UnlogPlayer(p);
    }
#endif _DEBUG
}

void RestartCommand::Execute(sint32 argc, char** argv)
{
	Assert(argc == 1);
	if(argc != 1)
		return;

	main_Restart();
}

void ImproveCommand::Execute(sint32 argc, char** argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	PLAYER_INDEX	player;
	ID	item;
	SELECT_TYPE	state;

	g_selected_item->GetTopCurItem(player, item, state);
	if(state == SELECT_TYPE_LOCAL_CITY) {
		g_player[player]->BuildImprovement(atoi(argv[1]), Unit(item));
	}
}

void SeeWWRCommand::Execute(sint32 argc, char**argv)
{

	g_player[g_selected_item->GetVisiblePlayer()]->DisplayWWR();
}

void SetWorkdayCommand::Execute(sint32 argc, char**argv)
{
    if(argc > 2)
        return;

	sint32 val = (PLAYER_INDEX)atoi(argv[1]);
    g_player[g_selected_item->GetVisiblePlayer()]->SetWorkdayLevel(val);
}

void SetWagesCommand::Execute(sint32 argc, char** argv)
{
   	if(argc > 2)
        return;

	sint32 val = (PLAYER_INDEX)atoi(argv[1]);
    g_player[g_selected_item->GetVisiblePlayer()]->SetWagesLevel(val);
}

void SetRationsCommand::Execute(sint32 argc, char** argv)
{
  	if(argc > 2)
        return;

	sint32 val = (PLAYER_INDEX)atoi(argv[1]);
    g_player[g_selected_item->GetVisiblePlayer()]->SetRationsLevel(val);
}

void AddMaterialsCommand::Execute(sint32 argc, char **argv)
{
	Assert(argc == 2);
	if(argc != 2)
		return;

	if(g_network.IsClient() && !g_network.SetupMode()) {
		g_network.SendCheat(new NetCheat(NET_CHEAT_ADD_MATERIALS,
										 g_selected_item->GetVisiblePlayer(),
										 atoi(argv[1])));
	}

	g_player[g_selected_item->GetVisiblePlayer()]->m_materialPool->
		AddMaterials(atoi(argv[1]));
}

void SetMaterialsPercentCommand::Execute(sint32 argc, char** argv)
{
  	if(argc > 2)
        return;

	sint32 val = (PLAYER_INDEX)atoi(argv[1]);
    double d = double (val) * 0.01;
    g_player[g_selected_item->GetVisiblePlayer()]->SetMaterialsTax(d);
}

void ThroneRoomUpgradeCommand::Execute(sint32 argc, char** argv)
{
}

void GiveMeProbeCommand::Execute(sint32 argc, char** argv)
{





}

void SetReadinessCommand::Execute(sint32 argc, char** argv)
{
  	if(argc > 2)
        return;

	sint32 val = (PLAYER_INDEX)atoi(argv[1]);
    READINESS_LEVEL r;
    switch (val) {
    case 0:
        r = READINESS_LEVEL_PEACE;
        break;
    case 1:
        r = READINESS_LEVEL_ALERT;
        break;
    case 2:
        r = READINESS_LEVEL_WAR;
        break;
    default:
        return;
    }
    g_player[g_selected_item->GetVisiblePlayer()]->SetReadinessLevel(r);
}

void FrameCommand::Execute(sint32 argc, char **argv)

{
    if (g_debugOwner != k_DEBUG_OWNER_FRAME_RATE) {
        g_debugOwner = k_DEBUG_OWNER_FRAME_RATE;
    } else {
        g_debugOwner = k_DEBUG_OWNER_NONE;
    }
}
extern BOOL gDone;
extern HWND	gHwnd;
extern BOOL g_letUIProcess;

void FastRoundCommand::Execute(sint32 argc, char **argv)
{

	if(g_doingFastRounds)
		return;

	if (g_statusWindow)
		g_statusWindow->Hide();

    sint32 i, n;

    n = atoi(argv[1]);

	sint32 t = GetTickCount();
#ifdef __AUI_USE_SDL__
	SDL_Event event;
#else
	MSG     msg;
#endif

	g_doingFastRounds = TRUE;

    for (i=0; i<(n) && !gDone; i++) {

		NewTurnCount::StartNextPlayer(false);

		g_director->NextPlayer();
		do {
			g_controlPanel->Idle();
            if (g_civApp)
    			g_civApp->Process();

#if __AUI_USE_SDL__
			while (1) {
				int n = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_MOUSEMOTION - 1);
				if (0 > n) {
					fprintf(stderr, "[FastRoundCommand::Execute] PeepEvents failed:\n%s\n", SDL_GetError());
					break;
				}
				if (0 == n) {
					n = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEWHEEL + 1, SDL_LASTEVENT);
					if (0 > n) {
						fprintf(stderr, "[FastRoundCommand::Execute] PeepEvents failed:\n%s\n", SDL_GetError());
						break;
					}
					if (0 == n) {
						// other events are handled in other threads
						// or no more events
						break;
					}
				}
				if (SDL_QUIT == event.type) {
					gDone = TRUE;
				}
				if (SDL_KEYDOWN == event.type)
				{
					SDL_KeyboardEvent key = event.key;
					if (SDLK_ESCAPE == key.keysym.sym)
						i = n;
				}
			}
#else
          	while (PeekMessage(&msg, gHwnd, 0, 0, PM_REMOVE) && !g_letUIProcess) {

			    if (msg.message == WM_QUIT) {
				    gDone = TRUE;
			    }
			    TranslateMessage(&msg);

				if (msg.message == WM_CHAR) {
					if ((MBCHAR)msg.wParam == 0x1B)
						i = n;
				}
			    DispatchMessage(&msg);
    		}
#endif
       		g_letUIProcess = FALSE;

		} while ((g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer()) &&
			    !gDone);

    }

	t = GetTickCount() - t;

    if (0 < n){
	    DPRINTF(k_DBG_UI, ("%d turns in %d seconds, %d ms/turn\n",
				    n, t/100, t/n));
    }
	g_tiledMap->InvalidateMix();
	g_doingFastRounds = FALSE;
	if (g_statusWindow)
		g_statusWindow->Show();

}

void AiDumpCommand::Execute(sint32 argc, char **argv)
{

}

void FastMoveCommand::Execute(sint32 argc, char **argv)
{
	g_theProfileDB->SetUnitAnim(!g_theProfileDB->IsUnitAnim());

}

void MainMenuCommand::Execute(sint32 argc, char **argv)
{
	g_civApp->PostEndGameAction();
}

void UseDDBlitCommand::Execute(sint32 argc, char **argv)
{























	c3errors_ErrorDialog("Temp", "Temp");








}

void AutoSaveCommand::Execute(sint32 argc, char **argv)
{
	g_theProfileDB->SetAutoSave(!g_theProfileDB->IsAutoSave());
}

void HeapTotalsCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG_MEMORY
	DebugMemory_LeaksShow(g_turn->GetRound());
	Log_Close();
	exit(1);
#endif
}

void LeaksClearCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG_MEMORY
	DebugMemory_LeaksClear();
#endif
}

void LeaksShowCommand::Execute(sint32 argc, char **argv)
{
#ifdef _DEBUG_MEMORY
	DebugMemory_LeaksShow(g_turn->GetRound());
#endif
}

void SuperFastDebugModeCommand::Execute(sint32 argc, char **argv)
{
	BOOL		on;

	if(argc == 2) {
		if (!strcmp(argv[1],"on")) {
			on = TRUE;
		} else {
			if (!strcmp(argv[1],"off")) {
				on = FALSE;
			}
		}
	}




}

void LoadDBCommand::Execute(sint32 argc, char **argv)
{
	DBLexer *lex;
	if (!strcmp(argv[1], "string")) {

		delete g_theStringDB;
		g_theStringDB = new StringDB();
		Assert(g_theStringDB);
		if (g_theStringDB) {
			if(!g_theStringDB->Parse(g_stringdb_filename)) {
				Assert(FALSE);
				return;
			}
		} else {
			return;
		}
	} else
	if (!strcmp(argv[1], "advance")) {

		delete g_theAdvanceDB;
        g_theAdvanceDB = new CTPDatabase<AdvanceRecord>;
		Assert(g_theAdvanceDB);
		if (g_theAdvanceDB) {
			if(!g_theAdvanceDB->Parse(C3DIR_GAMEDATA, g_advancedb_filename)) {
				Assert(FALSE);
				return;
			}
		} else {
			return;
		}
	} else
	if (!strcmp(argv[1], "terrain")) {

		delete g_theTerrainDB;
        g_theTerrainDB = new CTPDatabase<TerrainRecord>;
		if (g_theTerrainDB) {
			if(!g_theTerrainDB->Parse(C3DIR_GAMEDATA, g_terrain_filename)) {
				Assert(FALSE);
				return;
			}
		} else {
			return;
		}


	} else
	if (!strcmp(argv[1], "unit")) {

		delete g_theUnitDB;
        g_theUnitDB = new CTPDatabase<UnitRecord>;
		Assert(g_theUnitDB);
		if (g_theUnitDB) {
			lex = new DBLexer(C3DIR_GAMEDATA, g_unitdb_filename);
			if (!g_theUnitDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;

		} else {
			return;
		}

	} else
	if (!strcmp(argv[1], "const")) {

		delete g_theConstDB;
        g_theConstDB = new CTPDatabase<ConstRecord>;

		if (g_theConstDB) {
			if(!g_theConstDB->Parse(C3DIR_GAMEDATA, g_constdb_filename)) {
				Assert(FALSE);
				return;
			}

		} else {
			return;
		}
	} else
	if (!strcmp(argv[1], "wonder")) {

		delete g_theWonderDB;
		g_theWonderDB = new CTPDatabase<WonderRecord>;

		if (g_theWonderDB) {
			if(!g_theWonderDB->Parse(C3DIR_GAMEDATA, g_wonder_filename)) {
				Assert(FALSE);
				return;
			}
		} else {
			return;
		}
	} else
	if (!strcmp(argv[1], "civ")) {

		delete g_theCivilisationDB;
		g_theCivilisationDB = new CTPDatabase<CivilisationRecord>;
		if (g_theCivilisationDB) {
			if(!g_theCivilisationDB->Parse(C3DIR_GAMEDATA, g_civilisation_filename)) {
				Assert(FALSE);
				return;
			}
		} else {
			return;
		}
	} else










	if (!strcmp(argv[1], "pop")) {

	} else

	if (!strcmp(argv[1], "ai")) {
		delete g_theGoalDB;
		g_theGoalDB = new CTPDatabase<GoalRecord>();
		Assert(g_theGoalDB );
		if (g_theGoalDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_goal_db_filename);
			if (!g_theGoalDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theUnitBuildListDB;
		g_theUnitBuildListDB= new CTPDatabase<UnitBuildListRecord>();
		Assert(g_theUnitBuildListDB);
		if (g_theUnitBuildListDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_unit_buildlist_db_filename);
			if (!g_theUnitBuildListDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theWonderBuildListDB;
		g_theWonderBuildListDB= new CTPDatabase<WonderBuildListRecord>();
		Assert(g_theWonderBuildListDB);
		if (g_theWonderBuildListDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_wonder_buildlist_db_filename);
			if (!g_theWonderBuildListDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theBuildingBuildListDB;
		g_theBuildingBuildListDB= new CTPDatabase<BuildingBuildListRecord>();
		Assert(g_theBuildingBuildListDB);
		if (g_theBuildingBuildListDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_building_buildlist_db_filename);
			if (!g_theBuildingBuildListDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theImprovementListDB;
		g_theImprovementListDB= new CTPDatabase<ImprovementListRecord>();
		Assert(g_theImprovementListDB);
		if (g_theImprovementListDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_improvement_list_db_filename);
			if (!g_theImprovementListDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theStrategyDB;
		g_theStrategyDB = new CTPDatabase<StrategyRecord>();
		Assert(g_theStrategyDB);
		if (g_theStrategyDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_strategy_db_filename);
			if (!g_theStrategyDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theBuildListSequenceDB;
		g_theBuildListSequenceDB = new CTPDatabase<BuildListSequenceRecord>();
		Assert(g_theBuildListSequenceDB);
		if (g_theBuildListSequenceDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_buildlist_sequence_db_filename);
			if (!g_theBuildListSequenceDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theDiplomacyDB;
		g_theDiplomacyDB= new CTPDatabase<DiplomacyRecord>();
		Assert(g_theDiplomacyDB);
		if (g_theDiplomacyDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_diplomacy_db_filename);
			if (!g_theDiplomacyDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}
		delete g_theAdvanceListDB;
		g_theAdvanceListDB= new CTPDatabase<AdvanceListRecord>();
		Assert(g_theAdvanceListDB);
		if (g_theAdvanceListDB) {
			lex = new DBLexer(C3DIR_AIDATA, g_advance_list_db_filename);
			if (!g_theAdvanceListDB->Parse(lex)) {
				Assert(FALSE);
				return;
			}
			delete lex;
		} else {
			return;
		}

	}

	DPRINTF(k_DBG_INFO, ("Database '%s' successfully loaded.\n", argv[1]));

	MBCHAR filename[_MAX_PATH];
	g_civPaths->FindFile(C3DIR_SOUNDS, "pct.wav", filename);
#ifdef WIN32
    PlaySound (filename, NULL, SND_ASYNC | SND_FILENAME);
#endif
	return;
}

void DRayTestCode::Execute(sint32 argc, char **argv)
{
	int const curRound	= std::min<sint32>(NewTurnCount::GetCurrentRound(), 200);
	int turnStrength[200];
	int i;
	for(i=0; i<curRound; i++)
		turnStrength[i] = g_player[g_selected_item->GetVisiblePlayer()]->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS,i);

	return;
}

CommandLine::CommandLine()
{
	m_len = 0;
	m_displayHelp = FALSE;
	m_persistent = FALSE;
	m_displayCityResources = FALSE;

    m_display_mem = FALSE;
    m_flux=0;
    m_flux_decay = 0.85;

}

void CommandLine::Draw()
{
	m_buf[m_len] = 0;
	if (g_statusWindow)
		g_statusWindow->DrawCommand(m_buf);
}

#define k_LEFT_EDGE 100
#define k_TOP_EDGE 120
#define k_TEXT_SPACING 16

void CommandLine::DisplayOutput(aui_Surface* surf)
{
	sint32 l = 0;
    MBCHAR buf[256];
    sint32 arsize =  sizeof(commands)/sizeof(CommandRecord);

	if(m_displayHelp) {
		primitives_DrawText(surf, k_LEFT_EDGE, k_TOP_EDGE,
							(MBCHAR*)"command <required_param> [optional_param]",
							0,0);
		sint32 i;
		for
		(
			i = m_helpStart;
            (i < arsize) && commands[i].m_name && (i < m_helpStart + k_HELP_LINES);
			++i
		)
		{
			primitives_DrawText(surf, k_LEFT_EDGE, (k_TOP_EDGE + k_TEXT_SPACING) + (i-m_helpStart) * k_TEXT_SPACING,
								(MBCHAR*)commands[i].m_helptext, 0, 0);





		}
		if (commands[i].m_name) {

			sprintf(buf, "[~help %d] for next page", (m_helpStart / k_HELP_LINES) + 1);
			primitives_DrawText(surf, k_LEFT_EDGE, (k_TOP_EDGE + k_TEXT_SPACING) + (i - m_helpStart) * k_TEXT_SPACING,
								(MBCHAR*)buf, 0, 0);
		}
	} else if(m_displayCityResources) {
		primitives_DrawText(surf, k_LEFT_EDGE, k_TOP_EDGE,
						   (MBCHAR*)"player city resources",
						   0, 0);
		l = 0;
		for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
			if(!g_player[i]) continue;
			UnitDynamicArray* cityList = g_player[i]->GetAllCitiesList();
			sint32 n = cityList->Num();
			for(sint32 j = 0; j < n; j++) {
				char buf[1024];
				sprintf(buf, "    %2d %4d ", i, j);
				CityData* cityData = cityList->Get(j).GetData()->GetCityData();
				for(sint32 r = 0; r < g_theResourceDB->NumRecords(); r++)
                {

#ifdef CTP1_TRADE
				    sint32 const rc = cityData->GetResourceCount(r);
#else
					sint32 const rc = (*cityData->GetCollectingResources())[r];
#endif
                    if (rc > 0)
					{
						char rbuf[80];
						sprintf(rbuf, "%d:%d ", r, rc);
						strcat(buf, rbuf);
					}
				}
				primitives_DrawText(surf, k_LEFT_EDGE, (k_TOP_EDGE + k_TEXT_SPACING) + l * k_TEXT_SPACING,
									(MBCHAR*)buf, 0, 0);
				l++;
			}
		}
	} else if(m_displayOffers) {
		for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
			if(!g_player[i]) continue;
			DynamicArray<TradeOffer>* tradeOffers = g_player[i]->GetTradeOffersList();
			for(sint32 j = 0; j < tradeOffers->Num(); j++) {
				char buf[1024];
				char buf2[1024];
				switch(tradeOffers->Get(j).GetOfferType()) {
					case ROUTE_TYPE_RESOURCE:
						sprintf(buf, "Player %d offers resource %d from %d in exchange for ",
								i, tradeOffers->Get(j).GetOfferResource(),
								(uint32)tradeOffers->Get(j).GetFromCity());
						break;
					case ROUTE_TYPE_GOLD:
						sprintf(buf, "Player %d offers %d gold from %d in exchange for ",
								i, tradeOffers->Get(j).GetOfferResource(),
								(uint32)tradeOffers->Get(j).GetFromCity());
						break;
					case ROUTE_TYPE_FOOD:
						sprintf(buf, "Player %d offers %d food from %d in exchange for ",
								i, tradeOffers->Get(j).GetOfferResource(),
								(uint32)tradeOffers->Get(j).GetFromCity());
						break;
				}
				switch(tradeOffers->Get(j).GetAskingType()) {
					case ROUTE_TYPE_RESOURCE:
						sprintf(buf2, "resource %d",
								tradeOffers->Get(j).GetAskingResource());
						break;
					case ROUTE_TYPE_GOLD:
						sprintf(buf2, "%d gold",
								tradeOffers->Get(j).GetAskingResource());
						break;
					case ROUTE_TYPE_FOOD:
						sprintf(buf2, "%d food",
								tradeOffers->Get(j).GetAskingResource());
						break;
				}
				strcat(buf, buf2);
				sprintf(buf2, " to city %d", (int)tradeOffers->Get(j).GetToCity());

				strcat(buf, buf2);
				primitives_DrawText(surf, k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
									(MBCHAR *)buf, 0, 0);
				l++;
			}
		}
    } else if (m_display_mem) {
#ifdef _DEBUG_MEMORY

		l=0;
		sprintf (buf, "EXE total bytes: %d", DebugMemory_GetTotalFromEXE());
		primitives_DrawText(surf, k_LEFT_EDGE,
			k_TOP_EDGE + l * k_TEXT_SPACING, (MBCHAR *)buf, 0, 0);

		l++;
		sprintf (buf, "DLL total bytes: %d", DebugMemory_GetTotalFromDLL());
		primitives_DrawText(surf, k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
			(MBCHAR *)buf, 0, 0);

		l++;
		sprintf (buf, "Combined total : %d", DebugMemory_GetTotalFromEXE()+DebugMemory_GetTotalFromDLL());
		primitives_DrawText(surf, k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
			(MBCHAR *)buf, 0, 0);
#elif defined(_DEBUG) && defined(WIN32)

		_CrtMemState new_state;
       _CrtMemCheckpoint(&new_state);

       l = 0;
       sprintf (buf, "call count %d", new_state.lCounts[1]);
	   primitives_DrawText(surf,
                k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
									(MBCHAR *)buf, 0, 0);
        l++;
       sprintf (buf, "currently allocated %d", new_state.lSizes[1]);
	   primitives_DrawText(surf,
                k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
									(MBCHAR *)buf, 0, 0);

       l++;
       l++;
       sprintf (buf, "cumlative total %d", new_state.lTotalCount);
	   primitives_DrawText(surf,
                k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
									(MBCHAR *)buf, 0, 0);

       double raw_flux = new_state.lTotalCount - m_old_mem_count;
       m_old_mem_count = new_state.lTotalCount;
       m_flux = m_flux_decay * m_flux + (1.0 - m_flux_decay) * raw_flux;

       l++;
       sprintf (buf, "           flux %d", sint32(m_flux));
       sint32 color;
       if (1000 < m_flux) {
          color = RGB(255,0, 0);
       } else {
           color = 0;
       }
	   primitives_DrawText(surf,
                k_LEFT_EDGE, k_TOP_EDGE + l * k_TEXT_SPACING,
									(MBCHAR *)buf, color , 0);
#endif
    }

	g_tiledMap->InvalidateMix();
}

void CommandLine::DisplayHelp(BOOL on)
{
	m_displayHelp = on;
}

void CommandLine::DisplayCityResources(BOOL on)
{
	m_displayCityResources = on;
	m_displayOffers = FALSE;
}

void CommandLine::DisplayOffers(BOOL on)
{
	m_displayOffers = on;
	m_displayCityResources = FALSE;
}

void CommandLine::DisplayMem()
{
    m_displayHelp = FALSE;
	m_persistent = FALSE;
	m_displayCityResources = FALSE;
	m_displayOffers = FALSE;
    m_display_mem = !m_display_mem;

#if defined(_DEBUG) && defined(WIN32)
    if (m_display_mem) {
        _CrtMemState new_state;
        _CrtMemCheckpoint(&new_state);

        m_old_mem_count = new_state.lTotalCount;
    }
#endif
}

BOOL
CommandLine::AddKey(char c)
{

	if(m_addingKey)
		return FALSE;

	m_addingKey = true;

	switch(c) {
	case '\r':
	case '\n':
		m_buf[m_len] = 0;
		m_displayHelp = FALSE;
		Execute();
		Clear();
		Draw();
		if (!m_persistent)
			if (g_statusWindow)
				g_statusWindow->Hide();
		m_addingKey = false;
		return m_persistent;
	case 8:
		if(m_len > 0) {
			m_len--;
		}
		if (g_statusWindow)
			g_statusWindow->Draw();
		m_addingKey = false;
		return TRUE;
	case 27:
		m_len = 0;
		if (g_statusWindow)
			g_statusWindow->Draw();
		m_addingKey = false;
		return FALSE;
    default:
		if(c == '~') {
			m_addingKey = false;
			return FALSE;
		}

		m_buf[m_len++] = c;
		Draw();
		m_addingKey = false;
		return TRUE;
	}
}

void
CommandLine::Clear()
{
	m_len = 0;
	m_buf[m_len] = 0;
	while(m_argc > 0) {
		delete [] m_argv[m_argc - 1];
		m_argc--;
	}
	if (g_statusWindow)
		g_statusWindow->Draw();
}

sint32
CommandLine::Parse()
{
	sint32 p = 0;
	sint32 state = 0;
	char curarg[1024];
	int argpos = 0;

	while(isspace(m_buf[p])) p++;

	for(; p < m_len; p++) {
		switch(state) {
		case 0:
			if(isspace(m_buf[p])) {
				m_argv[m_argc] = new char[argpos + 1];
				strncpy(m_argv[m_argc], curarg, argpos);
				m_argv[m_argc][argpos] = 0;
				m_argc++;
				state = 1;
			} else if(m_buf[p] == '"') {
				state = 2;
			} else {
				curarg[argpos++] = m_buf[p];
			}
			break;
		case 1:
			if(!isspace(m_buf[p])) {
				argpos = 0;
				curarg[argpos++] = m_buf[p];
				state = 0;
			}
			break;
		case 2:
			if(m_buf[p] == '"') {
				state = 0;
			} else {
				curarg[argpos] = m_buf[p];
			}
			break;

		}
	}
	if((state == 0 || state == 2) && argpos > 0) {
		m_argv[m_argc] = new char[argpos + 1];
		strncpy(m_argv[m_argc], curarg, argpos);
		m_argv[m_argc][argpos] = 0;
		m_argc++;
	}
	return m_argc;
}

BOOL
CommandLine::PartialMatch(const char* x, const char* y)
{
	while(*y && *x) {
		if(*x != *y)

			return FALSE;
		x++;
		y++;
	}
	if(*y == 0)

		return TRUE;

	return FALSE;
}

void
CommandLine::Execute()
{
	if(Parse() > 0) {
		sint32 cmd = 0;
		while(commands[cmd].m_name != NULL) {
			if(PartialMatch(commands[cmd].m_name, m_argv[0])) {
				DPRINTF(k_DBG_INFO, ("Executing command: %s\n",
									 commands[cmd].m_name));
				commands[cmd].m_handler->Execute(m_argc, m_argv);
				break;
			}
			cmd++;
		}
	}
}


#endif
