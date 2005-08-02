#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#include "ctp2_config.h"

#ifdef _PLAYTEST

#ifndef _C3CMDLINE_H_
#define _C3CMDLINE_H_

class aui_Surface; 

class Command;

struct CommandRecord
{
	char* m_name;
	Command* m_handler;
	char* m_helptext;
};


class Command
{
public:
	virtual void Execute(sint32 argc, char** argv) = 0;
};

#define COMMAND(x) class x : public Command { public: void Execute(sint32 argc, char **argv); }


COMMAND(ZBCommand);
COMMAND(BuildCommand);
COMMAND(UpgradeCity);
COMMAND(CreateCommand);
COMMAND(TaxCommand);
COMMAND(RestoreCommand);
COMMAND(LoadAIPCommand);
COMMAND(LogAICommand);
COMMAND(SaveCommand);
COMMAND(RestartCommand);
COMMAND(ImproveCommand);
COMMAND(DisplayChecksumCommand);
COMMAND(CalcChecksumCommand);
COMMAND(DisplayChecksumOffCommand);
COMMAND(HelpCommand);
COMMAND(StayOnCommand);
COMMAND(TurnOffCommand);
COMMAND(RandTestCommand);
COMMAND(ClearTextCommand); 
COMMAND(TradeRouteCommand);
COMMAND(UntradeRouteCommand);
COMMAND(CityResourcesCommand);
COMMAND(OfferCommand);
COMMAND(ShowOffersCommand);
COMMAND(WithdrawOfferCommand);
COMMAND(InterceptCommand);
COMMAND(FloodCommand);
COMMAND(OzoneCommand);
COMMAND(ToggleFogCommand);
COMMAND(ToggleSmoothScrollCommand);
#ifdef WIN32
COMMAND(DumpCallStackCommand);
#endif
COMMAND(ToggleQuitFastCommand);
COMMAND(ToggleGridCommand);
COMMAND(ToggleHeraldCommand);
COMMAND(ToggleWaterCommand);
COMMAND(ToggleSpaceCommand);
COMMAND(PacCommand);
COMMAND(TerrainImprovementCommand);
COMMAND(TerrainImprovementCompleteCommand);
COMMAND(ReadinessCommand);
COMMAND(GiveCityCommand);
COMMAND(ExchangeCityCommand);
COMMAND(GiveAdvanceCommand);
COMMAND(GiveGoldCommand);
COMMAND(BequeathGoldCommand);
COMMAND(GiveUnitCommand);
COMMAND(StopTradingWithCommand);
COMMAND(FormAllianceCommand);
COMMAND(BreakAllianceCommand);
COMMAND(ExchangeMapCommand);
COMMAND(GiveMapCommand);
COMMAND(IsPollutionReducedCommand);
COMMAND(IsViolatingBordersCommand);
COMMAND(IsViolatingPeaceCommand);
COMMAND(IsViolatingCeaseFireCommand);
COMMAND(ChatCommand);
COMMAND(ChatMaskCommand);
COMMAND(DatacheckCommand);
COMMAND(BuildWonderCommand);
COMMAND(ShowAdvancesCommand);
COMMAND(MakeCeaseFireCommand) ;
COMMAND(BreakCeaseFireCommand) ;
COMMAND(DumpAgreementsCommand) ;
COMMAND(DumpDiplomaticRequestsCommand) ;
COMMAND(DumpMessagesCommand) ;
COMMAND(RequestGreetingCommand) ;
COMMAND(RegardCommand) ;
COMMAND(AttitudeCommand) ;
COMMAND(EndTurnSoundCommand) ;
COMMAND(RequestDemandAdvanceCommand) ;
COMMAND(RequestDemandCityCommand) ;
COMMAND(RequestDemandMapCommand) ;
COMMAND(RequestDemandGoldCommand) ;
COMMAND(RequestDemandStopTradeCommand) ;
COMMAND(RequestDemandAttackEnemyCommand) ;
COMMAND(RequestDemandLeaveOurLandsCommand) ;
COMMAND(RequestDemandReducePollutionCommand) ;
COMMAND(RequestOfferAdvanceCommand) ;
COMMAND(RequestOfferCityCommand) ;
COMMAND(RequestOfferMapCommand) ;
COMMAND(RequestOfferGoldCommand) ;
COMMAND(RequestOfferCeaseFireCommand) ;
COMMAND(RequestOfferPermanentAllianceCommand) ;
COMMAND(RequestOfferPactCaptureCityCommand) ;
COMMAND(RequestOfferPactEndPollutionCommand) ;
COMMAND(RequestOfferRewardCaptureCityForGoldCommand) ;
COMMAND(RequestOfferRewardCaptureCityForCityCommand) ;
COMMAND(RequestOfferRewardCaptureCityForAdvanceCommand) ;
COMMAND(RequestOfferRewardDestroyEnemyForGoldCommand) ;
COMMAND(RequestOfferRewardDestroyEnemyForCityCommand) ;
COMMAND(RequestOfferRewardDestroyEnemyForAdvanceCommand) ;
COMMAND(RequestExchangeAdvanceCommand) ;
COMMAND(RequestExchangeCityCommand) ;
COMMAND(RequestExchangeMapCommand) ;

COMMAND(RevoltCommand);
COMMAND(SeeWWRCommand); 
COMMAND(SetWorkdayCommand); 
COMMAND(SetWagesCommand); 
COMMAND(SetRationsCommand); 
COMMAND(SetMaterialsPercentCommand);
COMMAND(AddMaterialsCommand);
COMMAND(SetReadinessCommand); 
COMMAND(PactEndPollutionCommand);
COMMAND(PactCaptureCityCommand);
COMMAND(CaptureCityForGoldCommand);
COMMAND(CaptureCityForCityCommand);
COMMAND(CaptureCityForAdvanceCommand);
COMMAND(ShowNetworkStatsCommand);
COMMAND(PopCommand);
COMMAND(FrameCommand);
COMMAND(SetGovernmentCommand);
COMMAND(ShowOwnerCommand);
COMMAND(DebugMaskCommand);
COMMAND(SpewUnitsCommand);
COMMAND(CreateImprovementCommand);
COMMAND(SetCityNameCommand) ;
COMMAND(SetCitySizeCommand);

COMMAND(ToggleAIStr); 

COMMAND(InvestigateCityCommand);
COMMAND(NullifyWallsCommand);
COMMAND(StealTechnologyCommand);
COMMAND(InciteRevolutionCommand);
COMMAND(AssassinateRulerCommand);
COMMAND(InvestigateReadinessCommand);
COMMAND(GrantAdvanceCommand);

COMMAND(BombardCommand);
COMMAND(HearGossipCommand);
COMMAND(FranchiseCommand);
COMMAND(ExpelCommand);
COMMAND(SueCommand);
COMMAND(SueFranchiseCommand);
COMMAND(CauseUnhappinessCommand);
COMMAND(PlantNukeCommand);
COMMAND(SlaveRaidCommand);
COMMAND(EnslaveSettlerCommand);
COMMAND(UndergroundRailwayCommand);
COMMAND(InciteUprisingCommand);
COMMAND(BioInfectCommand);
COMMAND(NanoInfectCommand);
COMMAND(ConvertCityCommand);
COMMAND(LoadBuildQueueCommand) ;
COMMAND(SaveBuildQueueCommand) ;
COMMAND(TileTypeCommand) ;
COMMAND(ReformCityCommand);
COMMAND(IndulgenceCommand);
COMMAND(SoothsayCommand);

COMMAND(CloakCommand);
COMMAND(UncloakCommand);
COMMAND(RustleCommand);
COMMAND(CreateParkCommand);
COMMAND(CreateRiftCommand);

COMMAND(FastRoundCommand); 
COMMAND(DumpAlliesCommand) ;

COMMAND(HelpLinesCommand);
COMMAND(KillTileCommand) ;
COMMAND(DumpChecksumCommand) ;
COMMAND(LearnWhatCommand);
COMMAND(OvertimeCostCommand);
COMMAND(OvertimeCommand);
COMMAND(GivesWhatCommand);
COMMAND(HowLongCommand);
COMMAND(BuildWhatCommand);
COMMAND(SlicCommand);
COMMAND(TestMessageCommand);
COMMAND(YumCommand);
COMMAND(GetAdvanceCommand);
COMMAND(ShowPopCommand);
COMMAND(HidePopCommand);
COMMAND(InstantMessageCommand);
COMMAND(FastMoveCommand);
COMMAND(InjoinCommand);
COMMAND(GrantAllCommand);
COMMAND(GrantManyCommand);
COMMAND(DebugCheckMemCommand); 
COMMAND(UseLadderCommand);
COMMAND(TutorialCommand);
COMMAND(ToggleMapColorCommand);
COMMAND(ToggleHeuristicCommand); 
COMMAND(SimultaneousCommand);
COMMAND(AutoCenterCommand);
COMMAND(AiDumpCommand); 
COMMAND(SetUnitMovesStyleCommand);
COMMAND(SetClassicStyleCommand);
COMMAND(SetSpeedStyleCommand);
COMMAND(SetCarryoverStyleCommand);
COMMAND(SetTimedStyleCommand);
COMMAND(SetupModeCommand);
COMMAND(ReadyCommand);

COMMAND(MaxFoodCommand) ;
COMMAND(MaxProductionCommand) ;
COMMAND(MaxGoldCommand) ;
COMMAND(MaxHappyCommand) ;
COMMAND(MaxScienceCommand) ;

COMMAND(MainMenuCommand);

COMMAND(UseDDBlitCommand);
COMMAND(DisplayMemCommand);
COMMAND(LoadDBCommand);

COMMAND(SellUnitsCommand);
COMMAND(SellImprovementsCommand);
COMMAND(RobotMessagesCommand);

COMMAND(FZCommentCommand);
COMMAND(AcceptOfferCommand);
COMMAND(DescendCommand);
COMMAND(NearFortCommand);
COMMAND(NearCityCommand);
COMMAND(ForceRevoltCommand);

COMMAND(ToeCommand);

COMMAND(WhoAmICommand); 
COMMAND(AutoSaveCommand);
COMMAND(HeapTotalsCommand);
COMMAND(HotSeatCommand);
COMMAND(EmailCommand);
COMMAND(ScoreCommand);

COMMAND(FliLogCommand);
COMMAND(SendSlaveCommand);
COMMAND(DisbandCommand);
COMMAND(AttachCommand);
COMMAND(DetachCommand);
COMMAND(ToggleShieldSupport); 


COMMAND(SuperFastDebugModeCommand);
COMMAND(KillPopCommand);
COMMAND(BoardCommand);
COMMAND(AutoGroupCommand);
COMMAND(AddPopCommand);
COMMAND(CopyVisionCommand);


COMMAND(CombatLogCommand);


COMMAND(ThroneRoomUpgradeCommand);



COMMAND(GiveMeProbeCommand);

COMMAND(RedrawMapCommand);
COMMAND(GodCommand);
COMMAND(ReloadFliCommand); 

COMMAND(MultiCycleCommand);
COMMAND(DumpFZRegardCommand); 
COMMAND(SetFZRegardCommand); 

COMMAND(LeaksClearCommand);
COMMAND(LeaksShowCommand);
COMMAND(TotalWarCommand); 

COMMAND(SlicVariableCommand);
COMMAND(DipLogOnCommand); 
COMMAND(DipLogOffCommand); 

COMMAND(DirectorDumpCommand);
COMMAND(ResyncCommand);
COMMAND(SetAIRevoltCommand);

COMMAND(CleanSpritesCommand);

COMMAND(CleanScreenCommand);
COMMAND(ResetVisionCommand);


COMMAND(ExportMapCommand);
COMMAND(ImportMapCommand);


COMMAND(InitializeDiplomacyCommand);
COMMAND(BeginDiplomacyCommand);
COMMAND(ChooseNewProposalCommand);
COMMAND(SetNewProposalCommand);
COMMAND(SetResponseCommand);
COMMAND(SetHasInitiativeCommand);
COMMAND(ExecuteNewProposalCommand);
COMMAND(ExecuteResponseCommand);
COMMAND(ShowDiplomacyCommand);
COMMAND(NextStateCommand);
COMMAND(SetPersonalityCommand);
COMMAND(DeclareWarCommand);


COMMAND(SetGovernorForCityCommand);
COMMAND(SetGovernorPwReserveCommand);


COMMAND(ToggleCellText);
COMMAND(ToggleArmyText);


COMMAND(ArmyClumps);


COMMAND(SetGoodsCommand);


COMMAND(AiDebugCommand);


COMMAND(DRayTestCode);


COMMAND(ShowVictoryCommand);

COMMAND(ReloadSpritesCommand);


#ifdef DUMP_ASTAR

COMMAND(DumpAstarCommand);
#endif







class CommandLine
{
public:
	CommandLine();
	
	BOOL AddKey(char c);
	void Clear();
	void Execute();
	void Draw();
	void DisplayOutput(aui_Surface* surf);
	void DisplayHelp(BOOL on);
	void DisplayCityResources(BOOL on);
	void DisplayOffers(BOOL on);
    void DisplayMem();

	void SetPersistence(BOOL on) { m_persistent = on;}
	void SetHelpStart(sint32 start) { m_helpStart = start; }

private:
	sint32 Parse();
	BOOL PartialMatch(const char* x, const char* y);

	char* m_argv[256]; 
	sint32 m_argc;

	char m_buf[1024];
	sint32 m_len;
	BOOL m_displayHelp;
	BOOL m_persistent;
	BOOL m_displayCityResources;
	BOOL m_displayOffers;
	sint32 m_helpStart;

    bool m_addingKey;

    BOOL m_display_mem; 
    sint32 m_old_mem_count;
    double m_flux; 
    double m_flux_decay; 
};

extern CommandLine g_commandLine;
#else
class CommandLine;
#endif
#endif
