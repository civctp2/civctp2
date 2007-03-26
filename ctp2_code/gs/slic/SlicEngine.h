//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : The Slic Engine
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
// - Redesigned to prevent memory leaks and crashes.
// - Reuse SlicSegment pool between SlicEngine sessions.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef SLIC_ENGINE_H__
#define SLIC_ENGINE_H__

#include <string>       // std::string

class SlicEngine;

enum SLIC_TAG 
{
	ST_NONE,
	ST_UNIT,
	ST_CITY,
	ST_PLAYER,
	ST_GOOD,
	ST_GOLD,
	ST_LOCATION,
	ST_ACTION,
	ST_ADVANCE,
	ST_POP,
	ST_INT,
	ST_ARMY,
	ST_END
};

#define k_NUM_TIMERS 11
#define k_SEGMENT_HASH_SIZE 512

#define k_NORMAL_FILE 0
#define k_TUTORIAL_FILE 1

#define k_NON_TUTORIAL_MESSAGE_CLASS 666
#define k_NON_TUTORIAL_HELP_CLASS 667

#define k_MAX_TRIGGER_KEYS 10

class SlicObject;
class CivArchive;
class SlicSegment;
class SlicFunc;
template <class T> class StringHash;
template <class T> class PointerList;
class SlicSymTab;
class SlicSymbolData;
class SlicContext;
class SlicSegmentHash;
class Unit;
class MapPoint;
class TradeOffer;
class Agreement;
class Message;
class TradeRoute;
class SlicRecord;
class SlicConst;
class SlicStructDescription;
class SlicNamedSymbol;
class SlicParameterSymbol;
class SlicBuiltinNamedSymbol;
class SlicStack;
class SlicModFunc;
class SlicUITrigger;
class SlicDBInterface;
template <class T> class SimpleDynamicArray;

typedef sint32 AdvanceType;

#include "Advances.h"           // AdvanceType
#include "BldQue.h"             // BuildNode
#include "SlicBuiltinEnum.h"
#include "SlicTriggerLists.h"
#include "message.h"
#include "slicif_sym.h"
#include "SlicModFuncEnum.h"
#include "c3types.h"            // MBCHAR, sint32
#include "player.h"             // PLAYER_INDEX


class SlicEngine 
{
public:
	SlicEngine();
	SlicEngine(CivArchive &archive);
	virtual ~SlicEngine();

	void Serialize(CivArchive &archive);
	void PostSerialize();

	SlicSegment *GetSegment(const char *id);
	SlicSegmentHash *GetSegmentHash() { return m_segmentHash; }
	SlicFunc *GetFunction(const char *name);
	SlicNamedSymbol *GetSymbol(sint32 index);
	SlicNamedSymbol *GetSymbol(const char *name);
	SlicNamedSymbol *GetOrMakeSymbol(const char *name);
	SlicParameterSymbol *GetParameterSymbol(const char *name, sint32 parameterIndex);

	SlicObject *GetContext() { return m_context; }

	void Execute(SlicObject *obj);

	void AddBuiltinFunctions();
    bool Load(std::basic_string<MBCHAR> const & a_File, sint32 filenum);
	void Link();

	sint32 GetTutorialPlayer() const { return m_tutorialPlayer; }
	void SetTutorialPlayer(sint32 tut) { m_tutorialPlayer = tut; }
	void SetTutorialActive(BOOL on);
	bool GetTutorialActive() const { return m_tutorialActive != FALSE; }

	void AddTrigger(SlicSegment *trigger, TRIGGER_LIST which);

	bool SpecialSameGoodEnabled() const;

	void SetCurrentMessage(const Message &message);
	void GetCurrentMessage(Message &message) const;
	void KillCurrentMessage();
	void AddCurrentMessage();

	PointerList<SlicRecord> *GetRecords(sint32 player);
	void AddTutorialRecord(sint32 player, MBCHAR *title, MBCHAR *text,
						   SlicSegment *segment);

	bool IsTimerExpired(sint32 timer) const;
	void StartTimer(sint32 timer, time_t duration);
	void StopTimer(sint32 timer);
	sint32 GetTimerGranularity() { return m_timerGranularity; }
	void SetTimerGranularity(sint32 gran) { m_timerGranularity = gran; }

	void EnableMessageClass(sint32 mclass);
	void DisableMessageClass(sint32 mclass);
	bool IsMessageClassDisabled(sint32 mclass) const;

	void SetLoadGame(char *string) { m_loadGameName = string; }
	bool WaitingForLoad() const { return m_loadGameName != NULL; }
	char *GetLoadName() const { return m_loadGameName; }

	void RecreateTutorialRecord();

	void SetEyepointMessage(Message &msg) { m_eyepointMessage = msg; }
	Message &GetEyepointMessage() { return m_eyepointMessage; }
	
	void ProcessUITriggers();

	void BlankScreen(bool blank);
	bool ShouldScreenBeBlank() const { return m_blankScreen; }

	void RunYearlyTriggers();
	void RunPlayerTriggers(PLAYER_INDEX player);
	void RunCityTriggers(const Unit &city);
	void RunCityPopTriggers(const Unit &city);
	void RunClickedUnitTriggers(const Unit &unit);
	void RunSelectedUnitTriggers(const Unit &unit);
	void RunDeselectedUnitTriggers(const Unit &unit);
	void RunDeselectedCityTriggers(const Unit &city);
	void RunIdleTriggers(sint32 seconds);
	void RunUnitMovedTriggers(const Unit &u);
	void RunAllUnitsMovedTriggers();
	void RunCityBuiltTriggers(const Unit &city);
	void RunUnitBuiltTriggers(const Unit &u, const Unit &city);
	void RunDiscoveryTriggers(AdvanceType adv, PLAYER_INDEX p);
	void RunContactTriggers(const Unit &unit1, const Unit &unit2);
	void RunAttackTriggers(const Unit &unit1, const Unit &unit2);
	void RunTradeScreenTriggers();
	void RunSameGoodTriggers(const Unit &city1, const Unit &city2);
	void RunSameGoodAsTradedTriggers(sint32 good, const Unit &city1);
	void RunUnitQueueTriggers();
	void RunProductionQueueTriggers();
	void RunDiplomaticScreenTriggers();
	void RunCreateStackTriggers();
	void RunCreateMixedStackTriggers();
	void RunAutoArrangeOffTriggers();
	void RunBombardmentTriggers(const Unit &attacker, const Unit &defender);
	void RunCounterBombardmentTriggers(const Unit &bombarder, const Unit &counterbombarder);
	
	void RunActiveDefenseTriggers(const Unit &defender, const Unit &aggressor);
	void RunIndulgenceTriggers(const Unit &cleric, const Unit &city);
	void RunTerrorismTriggers(const Unit &terrorist, const Unit &target);
	void RunConversionTriggers(const Unit &cleric, const Unit &city);
	void RunWonderStartedTriggers(const Unit &city, sint32 wondertype);
	void RunWonderFinishedTriggers(const Unit &city, sint32 wondertype);
	
	void RunUnitLaunchedTriggers(const Unit &launchee);
	void RunEnslavementTriggers(const Unit &slaver, const Unit &city);
	void RunSettlerEnslavedTriggers(const Unit &slaver, sint32 settlerOwner);
	void RunVictoryEnslavementTriggers(const Unit &slaver, sint32 slavee, Unit &hc);
	void RunUnitBeginTurnTriggers(const Unit &unit);
	void RunPopMovedTriggers(const Unit &city);

	void RunBuildFarmTriggers(sint32 owner, const MapPoint &point,
							  sint32 type);
	void RunBuildRoadTriggers(sint32 owner, const MapPoint &point,
							  sint32 type);
	void RunBuildMineTriggers(sint32 owner, const MapPoint &point,
							  sint32 type);
	void RunBuildInstallationTriggers(sint32 owner, const MapPoint &point,
									  sint32 type);
	void RunBuildTransformTriggers(sint32 owner, const MapPoint &point,
								   sint32 type);
	void RunScienceRateTriggers(sint32 owner);
	void RunCityCapturedTriggers(sint32 newoner, sint32 oldowner, 
								 const Unit &city);
	void RunTradeOfferTriggers(const TradeOffer &offer);
	void RunTreatyBrokenTriggers(sint32 pl1, sint32 pl2, const Agreement &a);
	void RunUnitDeadTriggers(const Unit &unit, PLAYER_INDEX killedBy);
	void RunOutOfFuelTriggers(const Unit &unit);
	void RunUnitCantBeSupportedTriggers(const Unit &unit);
	void RunMiscUnitDeathTriggers(const Unit &unit);

	void RunDiscoveryTradedTriggers(sint32 pl1, sint32 pl2, AdvanceType adv);

	void RunUITriggers(const MBCHAR *controlName);
	void RunHelpTriggers(const MBCHAR *helpName);
	void RunPopMovedOffGoodTriggers(const Unit &city);
	void RunWastingWorkTriggers(const Unit &city);
	void RunPublicWorksTaxTriggers(sint32 owner);
	void RunWonderAlmostDoneTriggers(const Unit &city, sint32 wonder);
	void RunGovernmentChangedTriggers(sint32 player);
	void RunTradeRouteTriggers(TradeRoute &route, sint32 gold);
	void RunForeignTradeRouteTriggers(TradeRoute &route, sint32 gold);
	void RunUnitDoneMovingTriggers(const Unit &unit);

	void RunPiracyTriggers(const TradeRoute &route, const Unit &unit);
	void RunPillageTriggers(const Unit &unit, PLAYER_INDEX pillagee);

	void RunCitySelectedTriggers(const Unit &city);
	void RunClickedOnUnexploredTriggers(const MapPoint &pos);
	void RunZOCTriggers();
	void RunCantSettleMovementTriggers(const Unit &unit);
	void RunBuildingBuiltTriggers(const Unit &city, sint32 building);
	void RunAgeChangeTriggers(sint32 player);
	void RunTimerTriggers();
	void RunWorkViewTriggers();
	void RunSentCeaseFireTriggers(sint32 owner, sint32 recipient);

	void RunTrigger(TRIGGER_LIST tlist, ...);
	MBCHAR GetTriggerKey(sint32 index);
	void SetTriggerKey(sint32 index, MBCHAR key);
	bool IsKeyPressed(MBCHAR key) const;
	bool RunKeyboardTrigger(MBCHAR key);

	void CheckPendingResearch();
	void AddResearchOnUnblank(sint32 owner, MBCHAR *text);	

	SlicSymbolData *CheckForBuiltinWithIndex(MBCHAR *name, sint32 &index);

	void AddConst(const MBCHAR *name, sint32 value);
	bool FindConst(const MBCHAR *name, sint32 *value) const;

	void AddSymbol(SlicNamedSymbol *sym);
	void AddStructArray(bool createSymbols, SlicStructDescription *desc, SLIC_BUILTIN which);
	void AddStruct(bool createSymbols, SlicStructDescription *desc, SLIC_BUILTIN which);
	SlicSymbolData const * GetBuiltinSymbol(SLIC_BUILTIN which) const;

	void AddStructs(bool createSymbols);
	void AddBuiltinSymbol(SlicBuiltinNamedSymbol *sym);
	void SetContext(SlicObject *obj);

	SlicStructDescription *GetStructDescription(SLIC_SYM which);
	SlicStructDescription *GetStructDescription(SLIC_BUILTIN which);

	void Break(SlicSegment *segment, sint32 codeOffset, SlicObject *context, SlicStack *stack,
			   MessageData *message);
	void Continue();
	bool AtBreak() const { return m_atBreak; }

	void RequestBreak();
	bool BreakRequested();

	void PushContext(SlicObject * obj);
	void PopContext();

	void AddDatabases();
	SlicDBInterface *GetDBConduit(const char *name);

	void AddModFuncs();
	sint32 CallMod(MOD_FUNC modFunc, sint32 def, ...);

	sint32 CallExcludeFunc(const MBCHAR *name, sint32 type, sint32 player);

    static bool Reload(std::basic_string<MBCHAR> const & a_File);

private:
	BOOL m_tutorialActive;
	PLAYER_INDEX m_tutorialPlayer;
	Message *m_currentMessage;

	SlicSegmentHash *m_segmentHash;
	StringHash<SlicFunc> *m_functionHash;
	StringHash<SlicUITrigger> *m_uiHash;
	StringHash<SlicDBInterface> *m_dbHash;

	SlicSymTab *m_symTab;
	SlicObject *m_context;

	SlicModFunc *m_modFunc[mod_MAX];

	PointerList<SlicSegment> *		m_triggerLists[TRIGGER_LIST_MAX];
	PointerList<SlicRecord> *		m_records[k_MAX_PLAYERS];
	sint32                          m_timer[k_NUM_TIMERS];
	SimpleDynamicArray<sint32> *	m_disabledClasses;
	PointerList<SlicObject> *		m_uiExecuteObjects;
	Message							m_eyepointMessage;

	MBCHAR m_triggerKey[k_MAX_TRIGGER_KEYS];
	sint32 m_timerGranularity;

	BOOL m_doResearchOnUnblank;
	sint32 m_researchOwner;
	MBCHAR m_researchText[256];

	StringHash<SlicConst> *m_constHash;

	
	SlicSymbolData const **     m_builtins;
	SlicStructDescription **m_builtin_desc;

	char *m_loadGameName;
	MBCHAR m_currentKeyTrigger;
	bool m_blankScreen;

	bool m_atBreak;
	SlicObject *m_breakContext;

	PointerList<SlicObject> *   m_contextStack;
	bool                        m_breakRequested;
};

extern SlicEngine * g_slicEngine;

#endif
