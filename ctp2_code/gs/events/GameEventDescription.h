//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Declares all the slic events
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added HotSeat and PBEM human-human diplomacy support. (17-Oct-2007 Martin Gühmann)
// - Seperated the NewProposal event from the Response event so that the 
//   NewProposal event can be called from slic witout any problems. (17-Oct-2007 Martin Gühmann) 
// - Added GobalWarming and OzoneDepletion events. (29-Oct-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_EVENT_DESCRIPTION_H__
#define __GAME_EVENT_DESCRIPTION_H__

struct GameEventDescription {
	char *name;
	char *description;
	char *args;
};

#ifdef EVENT
#undef EVENT
#endif

#ifndef MAKE_EVENT_DESCRIPTIONS
#define EVENT(name, description, args) GEV_##name,
enum GAME_EVENT
#else
#define EVENT(name, description, args) {#name, description, args},
GameEventDescription g_eventDescriptions[] =
#endif
{
	EVENT(MoveOrder,                  "Give a pathed move order to an army", "%a%p%l%i")
	EVENT(MoveToOrder,                "Give a single square move order to an army", "%a%d")
	EVENT(MovePathOrder,              "Give a move order to an army, creates a new path", "%a%l")
	EVENT(SleepOrder,                 "Put an army to sleep", "%a")
	EVENT(UnloadOrder,                "Tell an army to unload its cargo", "%a%l")
	EVENT(MoveUnloadOrder,            "Army moves then unloads", "%a%p%l%i")
	EVENT(EntrenchOrder,              "Entrench an army", "%a")
	EVENT(DetrenchOrder,              "Detrench an army", "%a")
	EVENT(DisbandArmyOrder,           "Disband an army", "%a")
	EVENT(GroupOrder,                 "Group other units in the square into the army", "%a")
	EVENT(GroupUnitOrder,             "Group specified unit into the army", "%a%u")
	EVENT(UngroupOrder,               "Ungroup an army", "%a")
	EVENT(ParadropOrder,              "Drop an army from a plane", "%a%l")
	EVENT(InvestigateCityOrder,       "Army invesigates city", "%a%l")
	EVENT(NullifyWallsOrder,          "Army nullifies walls", "%a%l")
	EVENT(StealTechnologyOrder,       "Army steals technology", "%a%l")
	EVENT(InciteRevolutionOrder,      "Army incites a revolution", "%a%l")
	EVENT(AssassinateRulerOrder,      "Army assasinates a ruler", "%a%l")
	EVENT(InvestigateReadinessOrder,  "Army investigates readiness", "%a%l")
	EVENT(BombardOrder,               "Army bombards", "%a%l")
	EVENT(FranchiseOrder,             "Army franchises", "%a%l")
	EVENT(SueOrder,                   "Army sues", "%a%l")
	EVENT(SueFranchiseOrder,          "Army sues franchise", "%a")
	EVENT(ExpelOrder,                 "Army expels", "%a%l")
	EVENT(EstablishEmbassyOrder,      "Army establishes embassy", "%a%l")
	EVENT(AdvertiseOrder,             "Army advertises", "%a%l")
	EVENT(PlantNukeOrder,             "Army plants a nuke", "%a%l")
	EVENT(SlaveRaidOrder,             "Army slave raids", "%a%l")
	EVENT(EnslaveSettlerOrder,        "Army enslaves a settler", "%a%l")
	EVENT(UndergroundRailwayOrder,    "Army frees slaves", "%a%l")
	EVENT(InciteUprisingOrder,        "Army incites slave uprising", "%a%l")
	EVENT(BioInfectOrder,             "Army bio infects", "%a%l")
	EVENT(PlagueOrder,                "Army inflicts a plague", "%a%l")
	EVENT(NanoInfectOrder,            "Army nano infects", "%a%l")
	EVENT(ConvertCityOrder,           "Army converts a city", "%a%l")
	EVENT(ReformCityOrder,            "Army reforms a city", "%a")
	EVENT(SellIndulgencesOrder,       "Army sells indulgences", "%a%l")
	EVENT(SoothsayOrder,              "Army says sooth.  \"Sooth!\" says Army, \"Sooth!\"", "%a%l")
	EVENT(CloakOrder,                 "Army cloaks", "%a")
	EVENT(UncloakOrder,               "Army uncloaks", "%a")
	EVENT(RustleOrder,                "Army rustles", "%a%l")
	EVENT(CreateParkOrder,            "Army creates a park", "%a%l")
	EVENT(CreateRiftOrder,            "Army creates a rift", "%a%l")
	EVENT(PillageOrder,               "Army pillages", "%a")
	EVENT(InjoinOrder,                "Army injoins", "%a%l")
	EVENT(UseSpaceLadderOrder,        "Army climbs a space ladder", "%a")
	EVENT(AirliftOrder,               "Army airlifts", "%a%l")
	EVENT(DescendOrder,               "Army Descends", "%a")
	EVENT(ThrowPartyOrder,            "Army gets down and boogies", "%a%l")
	EVENT(PirateOrder,                "Army dons an eyepatch and parrot and plunders a trade route", "%a")
	EVENT(GetExpelledOrder,           "This army gets expelled", "%a%l%P")
	EVENT(SettleOrder,                "This army settles", "%a")
	EVENT(BoardTransportOrder,        "Move this army into a transport in the same cell.","%a")
	EVENT(LaunchOrder,                "Give a space launch order to this army", "%a%l")
	EVENT(TargetOrder,                "Give a nuclear missile a target", "%a%l")
	EVENT(ClearTargetOrder,           "Clear a nuclear missile's target", "%a")

	EVENT(CityRiot,                   "A city riots", "%c")
	EVENT(CalcScores,                 "Recalculate a player's scores", "%P")
	EVENT(CaptureCity,                "Make a city change hands", "%c%P%i")
	EVENT(MoveArmy,                   "Move an army one square", "%a%d%i%i%l")
	EVENT(ContactMade,                "Contact between two players established", "%P%P")
	EVENT(ClearOrders,                "Clear an army's orders", "%a")
	EVENT(FinishAttack,               "An army attacks someone", "%a%l")
	EVENT(FinishMove,                 "The last stage of an army moving before the MoveUnits event", "%a%d%l%i")
	EVENT(CantMoveYet,                "Added when an army tries to move but is out of move points", "%a%d%l")
	EVENT(MoveUnits,                  "An army always moves on this event, no more legality checks at this point", "%a%l%l")

	EVENT(MoveIntoTransport,          "Move an army into a transport (or several transports)", "%a%l")
	EVENT(Battle,                     "An army attacks a location", "%a%l")
	EVENT(BattleAftermath,            "Clean up the battlefield", "%a%l%u%u%P%P%i")
	EVENT(KillUnit,                   "Kill a unit", "%u%i%P")
	EVENT(KillCity,                   "Kill a city", "%c%i%P")
	
	EVENT(LaunchUnit,                 "Launch this unit into space", "%u%l")
	EVENT(Reentry,                    "Bring this army back to earth", "%a")

	EVENT(SetTarget,                  "Give a nuclear missile (unit) a target", "%u%c")
	EVENT(ClearTarget,                "Clear a nuclear missile's target", "%u")

	EVENT(BeginTurn,                  "Begin a player's turn", "%P%i")

	EVENT(WormholeTurn,               "Begin wormhole turn", "%P")
	EVENT(PlayerPatience,             "Begin player patience", "%P")
	EVENT(PeaceMovement,              "Begin player peace movement", "%P")
	EVENT(PollutionTurn,              "Begin player pollution", "%P")
	EVENT(BeginTurnAllCities,         "Player event before cities begin turn", "%P")
	EVENT(BeginTurnProduction,        "Begin player production", "%P")
	EVENT(BeginTurnSupport,           "Begin player support phase", "%P")
	EVENT(BeginTurnImprovements,      "Begin player improvements phase", "%P")
	EVENT(BeginTurnAgreements,        "Begin player diplomatic agreements phase", "%P")
	EVENT(ResetAllMovement,           "Reset all unit movement points", "%P")
	EVENT(AttemptRevolt,              "Make cities revolt as needed", "%P")
	EVENT(BeginTurnEndGame,           "Begin endgame processing phase", "%P")
	EVENT(BeginTurnGovernment,        "Handle government changes", "%P")
	EVENT(FinishBeginTurn,            "Last event in player begin turn phase", "%P")
	EVENT(FinishBuildPhase,           "Cities will have added all their build events when this fires", "%P")
	EVENT(StartMovePhase,             "Added at the end of the primary cb for FinishBuildPhase", "%P")
	EVENT(ProcessUnitOrders,          "Added by StartMovePhaseEvent, needed to come later", "%P")
	EVENT(AIFinishBeginTurn,          "Also from StartMovePhaseEvent", "%P")

	EVENT(UnitBeginTurnVision,        "Begin vision phase for enemy units", "%u%P")
	EVENT(BeginTurnUnit,              "Begin turn for own unit", "%u")
	
	EVENT(CityTurnPreProduction,      "Early begin turn phase, before production", "%c")
	EVENT(CityBeginTurn,              "Main city begin turn event", "%c")
	EVENT(CityBeginTurnVision,        "Vision phase for enemy cities", "%c%P")

	EVENT(BeginTurnArmy,              "Begin turn for own army", "%a")

	EVENT(BuildUnit,                  "Add a unit to a city's queue", "%c%i")
	EVENT(BuildBuilding,              "Add a building to a city's queue", "%c%i")
	EVENT(BuildWonder,                "Add a wonder to a city's queue", "%c%i")

	EVENT(BuildFront,                 "Try to build the first thing in a city's queue", "%c")
	EVENT(CreateUnit,                 "Create a unit", "%P%l%c%i%i&u")
	EVENT(ZeroProduction,             "Set a city's stored production to 0", "%c")

	EVENT(Settle,                     "Found a city with a settler", "%a&i")
	EVENT(CreateCity,                 "Create a city (first int is cause, second int is unit type that settled (-1 if not from unit)", "%P%l%i%i&c") 

	EVENT(CreateImprovement,          "Start building a terrain improvement", "%P%l%i%i")
	EVENT(ImprovementAddTurn,         "Add a turn to an improvement", "%I")
	EVENT(ImprovementComplete,        "An improvement is complete.  Location, owner,  and type filled in after completion", "%I&l&P&i")

	EVENT(PopToCity,                  "Move a pop to it's home city", "%C")
	EVENT(PopToField,                 "Move a pop to a new location", "%C%l")
	EVENT(MakePop,                    "Create a pop", "%c&P")
	EVENT(KillPop,                    "Remove a pop", "%c")
	EVENT(CityInfluenceChanged,       "Triggered when city influence radius changes (city,delta).", "%c%i")

	EVENT(GrantAdvance,               "Give a player an advance", "%P%i%i")

	EVENT(SendGood,                   "Send a good from one city to another", "%i%c%c")
	EVENT(TradeBid,                   "Bid on a foreign good", "%P%i%c%c")
	EVENT(KillTradeRoute,             "Kill a trade route", "%t%i")
	EVENT(SetPiratingArmy,            "Set army pirating trade route", "%t%a")

	EVENT(CreatedArmy,                "An empty army was created", "%a")
	EVENT(AddUnitToArmy,              "Insert a unit into an army", "%u%a%i")

	EVENT(FinishUnload,               "Finish the unload process for an army", "%a%a%l")
	EVENT(SetUnloadMovement,          "Take away the army's move points after an unload (Except CanBeachAssault units)", "%a")
	EVENT(SetUnloadMovementUnit,      "Take a unit's movement points away after an unload.", "%u")
	EVENT(FinishUprising,             "Finish a slave uprising", "%c%a%i")
	EVENT(CleanupUprising,            "Really finish a slave uprising", "%a%c")

	EVENT(GiveMap,                    "Give map from first player to second player", "%P%P")
	EVENT(GiveCity,                   "Give a city to a foreign player",             "%c%P")

	
	EVENT(SleepUnit,                  "Put a unit to sleep", "%u")
	EVENT(WakeUnit,                   "Wake a unit", "%u")
	EVENT(EntrenchUnit,               "Entrench a unit", "%u")
	EVENT(DetrenchUnit,               "Detrench a unit", "%u")
	EVENT(WakeArmy,                   "Wake all units in an army", "%a")
	EVENT(DisplayInvestigationWindow, "Display the investigate city window", "%u%c")
	EVENT(InciteRevolutionUnit,       "Unit incites a revolution", "%u%c")
	EVENT(SubGold,                    "Subtract gold from player", "%P%i")
	EVENT(AddGold,                    "Add gold to player", "%P%i")
	EVENT(AssassinateRulerUnit,       "Unit bombs cabinet", "%u%c")
	EVENT(MakeFranchise,              "Add a franchise to a city", "%u%c%P")
	EVENT(Teleport,                   "Army teleports", "%a%l")
	EVENT(PlantNukeUnit,              "Unit planted a nuke", "%u%c")
	EVENT(SlaveRaidCity,              "Unit raids a city for slaves", "%u%c")
	EVENT(Lawsuit,                    "Sue an army", "%a%u%l")
	EVENT(RemoveFranchise,            "Sue a franchise", "%a%u%c")
	EVENT(ExpelUnits,                 "Army expelling units", "%a%l")
	EVENT(NukeCityUnit,               "A unit nukes a city", "%u%c")
	EVENT(NukeLocationUnit,           "A unit nukes something else", "%u%l")
	EVENT(NukeCity,                   "The actual event that nukes a city", "%c%P")
	EVENT(EnslaveSettler,             "Enslave a settler", "%a%u%u")
	EVENT(UndergroundRailwayUnit,     "Free slaves from city", "%u%c")
	EVENT(InciteUprisingUnit,         "Incite a slave uprising", "%u%c")
	EVENT(EstablishEmbassyUnit,       "Attempt to establish an embassy", "%u%c")
	EVENT(EstablishEmbassy,           "Actually establish an embassy", "%P%P")
	EVENT(ThrowPartyUnit,             "Attempt to throw a party", "%u%c")
	EVENT(ThrowParty,                 "Actually throw a party", "%P%P")
	EVENT(BioInfectCityUnit,          "Unit infects city", "%u%c")
	EVENT(BioInfectCity,              "City gets infected", "%c%P")
	EVENT(PlagueCityUnit,             "Unit plagues a city", "%u%c")
	EVENT(PlagueCity,                 "City gets plagues", "%c%P")
	EVENT(NanoInfectCityUnit,         "Unit nanoinfects city", "%u%c")
	EVENT(NanoInfectCity,             "City gets nanoinfected", "%c%P")
	EVENT(ConvertCityUnit,            "Unit converts city", "%u%c")
	EVENT(ConvertCity,                "City actually converted", "%c%P%i")
	EVENT(ReformCityUnit,             "Unit reforms city", "%u%c")
	EVENT(UnconvertCity,              "City is reformed", "%c")
	EVENT(IndulgenceSaleMade,         "Indulgence sale made", "%u%c")
	EVENT(AddHappyTimer,              "Cause a timed amount of happiness or unhappiness", "%c%i%i%i")
	EVENT(CreateParkUnit,             "Unit creates a park", "%u%c")
	EVENT(CreatePark,                 "City becomes a park", "%c%P")
	EVENT(CutImprovements,            "Pillage terrain improvements", "%l")
	EVENT(PillageUnit,                "Unit pillages", "%u")
	EVENT(InjoinUnit,                 "Unit injoins city", "%u%c")
	EVENT(InjoinCity,                 "City is injoined", "%c%P")

	EVENT(CreateBuilding,             "Create a building", "%c%i")
	EVENT(CreateWonder,               "Create a wonder", "%c%i")

	EVENT(RunCombat,                  "Run one round of combat", "%a%l%P%P")
	EVENT(StartCombat,                "Start a battle", "%a%l")

	EVENT(BuyFront,                   "Rush buy an item in a city", "%c")
	EVENT(SellBuilding,               "Sell a building from a city", "%c%i")

	
	EVENT(BorderIncursion,            "Occurs when army first crosses foreign border.", "%P%P")
	EVENT(BorderPullout,              "Occurs when army leaves foreign border.", "%P%P")

	
	EVENT(ComputeMotivations,         "Determine fears and desires", "%P")
	EVENT(ReactionMotivation,         "Find new proposals motivated as reactions to game events", "%P%P")
	EVENT(FearMotivation,             "Find new proposal for fear motivation", "%P%P%i")
	EVENT(DesireMotivation,           "Find new proposal for desire motivation", "%P%P%i")
	EVENT(NewProposal,                "A new proposal has been made.", "%P%P")
	EVENT(Threaten,                   "A player has been threatened.", "%P%P")
	EVENT(Counter,                    "A proposal has been countered.", "%P%P")
	EVENT(Reject,                     "A proposal has been rejected.", "%P%P")
	EVENT(Accept,                     "A proposal has been accepted.", "%P%P")
	EVENT(ResponseReady,              "Notify world that a diplomatic response is ready", "%P%P")
	EVENT(NewProposalReady,           "Notify world that a new proposal is ready", "%P%P")
	EVENT(ContinueDiplomacy,          "Decide if sender should make a new proposal.", "%P%P")
	EVENT(ToggleInitiative,           "Initiative passes to party currently without initiative.", "%P%P")

	EVENT(InitStrategicState,         "Initialize strategic state", "%P")
	EVENT(NextStrategicState,         "Determine next strategic state", "%P")
	EVENT(InitDiplomaticState,        "Initialize Diplomatic state", "%P%P")
	EVENT(NextDiplomaticState,        "Determine next diplomatic state", "%P%P")

	
	EVENT(StartNegotiations,          "After computing motivations, select new proposals", "%P")
	EVENT(BeginScheduler,             "Match all armies with goals.","%P")
	EVENT(ProcessMatches,             "Perform one pass through match list, executing matches.","%P%i")
	
	EVENT(NewNegotiationEvent,        "A new negotiation history event has been added", "%P%P")

	EVENT(DisbandUnit,                "Disband one unit", "%u")
	EVENT(DisbandCity,                "Disband one city", "%c")

	
	EVENT(TimerExpired,               "Triggered when a timer expires.", "%i")
	
	
	EVENT(ArmyClicked,                "Triggered when the user clicks an army with the mouse.", "%a")
	EVENT(ArmySelected,               "Triggered when the user selects an army.", "%a")
	EVENT(ArmyDeselected,             "Triggered when the user deselects an army.", "%a")
	EVENT(CityClicked,                "Triggered when the user clicks a city with the mouse.", "%c")
	EVENT(CitySelected,               "Triggered when the user selects a city.", "%c")
	EVENT(CityDeselected,             "Triggered when the user deselects a city.", "%c")
	
	EVENT(AccomplishFeat,             "Someone has accomplished a feat (int = feat DB index)", "%i%P")

	EVENT(MADLaunch,                  "A targetted launches towards its target.", "%u")

	EVENT(BuildingRemoved,            "Triggered when a building is removed.","%c%i")
	EVENT(WonderRemoved,              "Triggered when a wonder is removed.","%c%i")

	EVENT(NetworkTurnSync,            "Process the start of a turn for this player", "%P")
	EVENT(EndAIClientTurn,            "Used in automated testing only", "%P")

	EVENT(EnterAge,                   "Player enters a new age.", "%P%i")

	EVENT(KillTile,                   "A map tile dies (turns to polluted)", "%l")

	EVENT(ActivateAllUnits,           "Detrench/wake all units at location", "%l")

	EVENT(BeginTurnExecute,           "Call ExecuteOrders on an Army for a begin turn (normally AI only)", "%a")

	EVENT(KillPlayer,                 "A player has died (all cities gone)", "%P%i%P")
	
	EVENT(EndTurn,                    "End a player's turn.", "%P")
	EVENT(VictoryMoveOrder,           "The move into an attacked square after winning a battle", "%a%l")

	EVENT(OpenInitialCityInterface,   "Generated when a city is initialized, after all population is added.", "%c")

	EVENT(ResumeEmailAndHotSeatDiplomacy, "Continues human to human diplomacy in Email and HotSeat mode.", "%P")
	EVENT(ProposalResponse,           "A proposal response is been considered.", "%P%P")
	EVENT(GlobalWarming,              "Global warming causes the sea level to rise so that vast land is flooded.", "%i")
	EVENT(OzoneDepletion,             "The ozone layer depletes so that the sun burns the world.", "")

	EVENT(MAX,                        "This is not a real event, it marks the end of the list", "")
};

extern GameEventDescription g_eventDescriptions[];

#endif
