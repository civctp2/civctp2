//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Readded the new slicfunctions of the patch, by Martin Gühmann.
// - Added FreeAllSlaves slicfunction by The Big Mc.
// - Added PlantSpecificGood and RemoveGood slicfunctions by MrBaggins.
// - Data blanked out at screen in between 2 players in hotseat play.
// - Great library history cleared between 2 players in hotseat play.
// - Corrected reported memory leak.
// - Added database access to all databases in the new database format,
//   even if it does not make sense, by Martin Gühmann.
// - New slic functions added by Martin Gühmann:
//   - CargoCapacity     Gets number of additional units a unit can carry.
//   - MaxCargoSize      Gets the maximum number of units a unit can carry.
//   - CargoSize         Gets the current number of units a unit is carrying.
//   - GetUnitFromCargo  Gets the i'th unit a unit is carrying.
//   - GetContinent      Gets the continent ID of an location.
//   - IsWater           Gets whether a location is water.
// - Enable end turn button when unblanking.
// - Removed a syntax error by Klaus Kaan
// - Function by Solver: IsOnSameContinent - Checks if two locations are 
//   on same continent.
// - Added AddSlaves function modelled after the AddPops function.
// - Prevented crash with a missing Slic file.
// - Memory leaks repaired.
// - Redesigned to prevent memory leaks and crashes.
// - Reuse SlicSegment pool between SlicEngine sessions.
// - Added slic civilisation database support.
// - Added slic risk database support. (Sep 15th 2005 Martin Gühmann)
// - Added City Capture options by E 6.09.2006
// - Added database slic access of difficulty, pollution and global warming
//   databases. (July 15th 2006 Martin Gühmann)
// - PopContext refills the builtins when it restores the old context so
//   that slic does not forget the values of the builtins. (Sep 24th 2006 Martin Gühmann)
// - Added GetContinentSize slic function. (Dec 24th 2006 Martin Gühmann)
// - Added slic database access to the new map icon database. (27-Mar-2007 Martin Gühmann)
// - Added slic database access to the new map database. (27-Mar-2007 Martin Gühmann)
// - Added slic database access to the new concept database. (31-Mar-2007 Martin Gühmann)
// - Added slic database access to the new const database. (29-Oct-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicEngine.h"    

#include <iterator>
#include <list>
#include "c3errors.h"
#include "SlicObject.h"
#include "civarchive.h"
#include "SlicSegment.h"
#include "slicif.h"
#include "StringHash.h"
#include "SlicFunc.h"
#include "slicfuncai.h"
#include "SlicSymTab.h"
#include "player.h"					// g_player
#include "Unit.h"
#include "CivPaths.h"				// g_civPaths
#include "SelItem.h"				// g_selected_item
#include "TradeOffer.h"
#include "Agreement.h"
#include "MessagePool.h"			// g_theMessagePool
#include "BldQue.h"
#include "SlicRecord.h"
#include "tutorialwin.h"			// TutorialWin
#include "pointerlist.h"
#include "SimpleDynArr.h"
#include "gamefile.h"
#include "tiledmap.h"
#include "radarmap.h"				// g_radarMap
#include "controlpanelwindow.h"		// g_controlPanel
#include "director.h"
#include "SlicConst.h"
#include "SlicStruct.h"
#include "SlicNamedSymbol.h"
#include "SlicBuiltin.h"
#include "SlicBuiltinEnum.h"
#include "SlicArray.h"
#include "sourcelist.h"
#include "SlicFrame.h"
#include "UnitRecord.h"
#include "AdvanceRecord.h"
#include "TerrainRecord.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "FeatRecord.h"
#include "OrderRecord.h"
#include "TerrainImprovementRecord.h"
#include "GovernmentRecord.h"
#include "StrategyRecord.h"
#include "DiplomacyRecord.h"
#include "PersonalityRecord.h"
#include "AdvanceBranchRecord.h"
#include "AdvanceListRecord.h"
#include "AgeCityStyleRecord.h"
#include "AgeRecord.h"
#include "BuildingBuildListRecord.h"
#include "BuildListSequenceRecord.h"
#include "CitySizeRecord.h"
#include "CityStyleRecord.h"
#include "ConceptRecord.h"
#include "ConstRecord.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"
#include "EndGameObjectRecord.h"
#include "GoalRecord.h"
#include "IconRecord.h"
#include "PopRecord.h"
#include "ImprovementListRecord.h"
#include "SoundRecord.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "SpriteRecord.h"
#include "UnitBuildListRecord.h"
#include "WonderBuildListRecord.h"
#include "WonderMovieRecord.h"
#include "CivilisationRecord.h"
#include "RiskRecord.h"
#include "DifficultyRecord.h"
#include "PollutionRecord.h"
#include "GlobalWarmingRecord.h"
#include "MapRecord.h"
#include "MapIconRecord.h"
#include "SlicDBConduit.h"
#include "SlicModFunction.h"
#include "GameEventManager.h"
#include "Globals.h"
#include "ResourceRecord.h"
#include "CriticalMessagesPrefs.h"
#include "messagewindow.h"
#include "profileDB.h"
#include "greatlibrary.h"
#include "MainControlPanel.h"

extern TutorialWin *g_tutorialWin;

SlicEngine *g_slicEngine = NULL;

char g_slic_filename[_MAX_PATH];
char g_tutorial_filename[_MAX_PATH];

namespace
{
    size_t const            CONST_HASH_SIZE                     = 10;
    size_t const            k_DB_HASH_SIZE                      = 32;
    sint32 const            k_SLIC_DEFAULT_TIMER_GRANULARITY    = 5;
    MBCHAR const            KEY_UNDEFINED                       = 0;
    sint32 const            NOT_IN_USE                          = -1;
    PLAYER_INDEX const      SINGLE_PLAYER_DEFAULT               = 1;
} // namespace


SlicEngine::SlicEngine()
:   m_tutorialActive        (FALSE),
	m_tutorialPlayer        (SINGLE_PLAYER_DEFAULT),
	m_currentMessage        (new Message()),
	m_segmentHash           (new SlicSegmentHash(k_SEGMENT_HASH_SIZE)),
	m_functionHash          (NULL),
	m_uiHash                (new StringHash<SlicUITrigger> (k_SEGMENT_HASH_SIZE)),
	m_dbHash                (NULL),
	m_symTab                (new SlicSymTab(0)),
	m_context               (NULL),
	m_disabledClasses       (new SimpleDynamicArray<sint32>),
	m_uiExecuteObjects      (new PointerList<SlicObject>),
	m_eyepointMessage       (),
	m_timerGranularity      (k_SLIC_DEFAULT_TIMER_GRANULARITY),
	m_doResearchOnUnblank   (FALSE),
	m_researchOwner         (NOT_IN_USE),
	m_constHash             (new StringHash<SlicConst>(CONST_HASH_SIZE)),
	m_builtins              (new SlicSymbolData const * [SLIC_BUILTIN_MAX]),
	m_builtin_desc          (new SlicStructDescription *[SLIC_BUILTIN_MAX]),
	m_loadGameName          (NULL),
	m_currentKeyTrigger     (KEY_UNDEFINED),
	m_blankScreen           (false),
	m_atBreak               (false),
	m_breakContext          (NULL),
	m_contextStack          (new PointerList<SlicObject>),
	m_breakRequested        (false)
{
	for (size_t i = 0; i < TRIGGER_LIST_MAX; ++i) 
	{
		m_triggerLists[i] = new PointerList<SlicSegment>;
	}

	std::fill(m_records, m_records + k_MAX_PLAYERS, (PointerList<SlicRecord> *) NULL);
	std::fill(m_timer, m_timer + k_NUM_TIMERS, NOT_IN_USE);
	std::fill(m_triggerKey, m_triggerKey + k_MAX_TRIGGER_KEYS, KEY_UNDEFINED);
	std::fill(m_builtins, m_builtins + SLIC_BUILTIN_MAX, (SlicSymbolData const *) NULL);
	std::fill(m_builtin_desc, m_builtin_desc + SLIC_BUILTIN_MAX, (SlicStructDescription *) NULL);
	std::fill(m_researchText, m_researchText + 256, 0);
	std::fill(m_modFunc, m_modFunc + mod_MAX, (SlicModFunc *) NULL); 

	AddStructs(true);
	AddBuiltinFunctions();
	AddDatabases();
}

SlicEngine::SlicEngine(CivArchive &archive)
:	m_tutorialActive        (FALSE),
	m_tutorialPlayer        (SINGLE_PLAYER_DEFAULT),
	m_currentMessage        (new Message()),
	m_segmentHash           (new SlicSegmentHash(k_SEGMENT_HASH_SIZE)),
	m_functionHash          (NULL),
	m_uiHash                (new StringHash<SlicUITrigger>(k_SEGMENT_HASH_SIZE)),
	m_dbHash                (NULL),
	m_symTab                (new SlicSymTab(0)),
	m_context               (NULL),
	m_disabledClasses       (new SimpleDynamicArray<sint32>),
	m_uiExecuteObjects      (new PointerList<SlicObject>),
	m_eyepointMessage       (),
	m_timerGranularity      (k_SLIC_DEFAULT_TIMER_GRANULARITY),
	m_doResearchOnUnblank   (FALSE),
	m_researchOwner         (NOT_IN_USE),
	m_constHash             (new StringHash<SlicConst>(CONST_HASH_SIZE)),
	m_builtins              (new SlicSymbolData const * [SLIC_BUILTIN_MAX]),
	m_builtin_desc          (new SlicStructDescription *[SLIC_BUILTIN_MAX]),
	m_loadGameName          (NULL),
	m_currentKeyTrigger     (KEY_UNDEFINED),
	m_blankScreen           (false),
	m_atBreak               (false),
	m_breakContext          (NULL),
	m_contextStack          (new PointerList<SlicObject>),
	m_breakRequested        (false)
{
	for (size_t i = 0; i < TRIGGER_LIST_MAX; ++i) 
	{
		m_triggerLists[i] = new PointerList<SlicSegment>;
	}

	std::fill(m_records, m_records + k_MAX_PLAYERS, (PointerList<SlicRecord> *) NULL);
    std::fill(m_timer, m_timer + k_NUM_TIMERS, NOT_IN_USE);
    std::fill(m_triggerKey, m_triggerKey + k_MAX_TRIGGER_KEYS, KEY_UNDEFINED);
    std::fill(m_builtins, m_builtins + SLIC_BUILTIN_MAX, (SlicSymbolData const *) NULL);
    std::fill(m_builtin_desc, m_builtin_desc + SLIC_BUILTIN_MAX, (SlicStructDescription *) NULL);
    std::fill(m_researchText, m_researchText + 256, 0);
    std::fill(m_modFunc, m_modFunc + mod_MAX, (SlicModFunc *) NULL); 

	AddStructs(true);
	AddBuiltinFunctions();
	AddDatabases();

	Serialize(archive);
}

SlicEngine::~SlicEngine()
{
    if (m_context)
	{
        m_context->Release();
        m_context = NULL;
	}

    if (m_breakContext)
	{
        m_breakContext->Release();
        m_breakContext = NULL;
	}

    while (SlicObject * obj = m_contextStack->RemoveTail())
	{
        obj->Release();
	}
    delete m_contextStack;

	// m_loadGameName: reference only
    KillCurrentMessage();
	delete m_currentMessage;
	
	size_t  i;

    for (i = 0; i < TRIGGER_LIST_MAX; ++i) 
    {
        if (m_triggerLists[i])
        {
	        m_triggerLists[i]->DeleteAll();
            delete m_triggerLists[i];
        }
	}

	for (i = 0; i < k_MAX_PLAYERS; ++i) 
    {
		if (m_records[i]) 
        {
			m_records[i]->DeleteAll();
			delete m_records[i];
		}
	}

    if (m_disabledClasses)
    {
	    m_disabledClasses->Clear();
        delete m_disabledClasses;
    }

    if (m_uiExecuteObjects)
    {
	    m_uiExecuteObjects->DeleteAll();
        delete m_uiExecuteObjects;
    }

	for (i = 0; i < mod_MAX; ++i) 
    {
	    delete m_modFunc[i];
    }


    delete m_segmentHash;
	delete m_functionHash;
	delete m_dbHash;
    delete m_constHash;
    delete m_uiHash;
    delete m_symTab;

	for (i = 0; i < SLIC_BUILTIN_MAX; ++i) 
    {
		delete m_builtin_desc[i];
        // m_builtins[i] not deleted: managed through m_symTab
	}
    delete [] m_builtin_desc;
    delete [] m_builtins;
	
	slicif_cleanup();

    if (g_theMessagePool) 
    {
		g_theMessagePool->NotifySlicReload();
	}
}

/// Reset the Slic handling and reload from file.
/// \param  a_File  Name of the top level Slic file to load
/// \result File loaded and parsed successfully
bool SlicEngine::Reload(std::basic_string<MBCHAR> const & a_File)
{
    delete g_slicEngine;
    g_slicEngine = new SlicEngine();

    bool isParsedOk = g_slicEngine->Load(a_File, k_NORMAL_FILE);
    if (isParsedOk)
    {
        g_slicEngine->Link();
    }
    return isParsedOk;
}

void SlicEngine::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE
	sint32 i, j, p, num, numRecords;

    if (!archive.IsStoring())
    {
        // Ugly hack to prevent crashes in m_segmentHash->Serialize
        g_slicEngine = this;
    }

	m_segmentHash->Serialize(archive);
	if(archive.IsStoring()) {
		archive << m_tutorialPlayer;
		archive << (sint32)m_tutorialActive;
		m_symTab->Serialize(archive);

		num = 0;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(m_records[i])
				num++;
		}
		archive << num;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(m_records[i]) {
				archive << i;
				num = m_records[i]->GetCount();
				archive << num;
				PointerList<SlicRecord>::Walker walk(m_records[i]);
				while(walk.IsValid()) {
					walk.GetObj()->Serialize(archive);
					walk.Next();
				}
			}
		}
		archive.Store((uint8*)m_timer, sizeof(m_timer));
		archive.Store((uint8*)m_triggerKey, sizeof(m_triggerKey));

		
		archive.PutUINT8(static_cast<uint8>(m_doResearchOnUnblank));
		archive << m_researchOwner;
		archive.Store((uint8*)m_researchText, sizeof(m_researchText));

	} else {
		archive >> m_tutorialPlayer;
		sint32 tmp;
		archive >> tmp;
		m_tutorialActive = tmp;

        delete m_symTab;
		m_symTab = new SlicSymTab(archive);

	    for (i = 0; i < k_MAX_PLAYERS; ++i) 
        {
		    if (m_records[i]) 
            {
			    m_records[i]->DeleteAll();
			    delete m_records[i];
			    m_records[i] = NULL;
		    }
	    }
		archive >> num;
		for(i = 0; i < num; i++) {
			archive >> p;
			m_records[p] = new PointerList<SlicRecord>;
			archive >> numRecords;
			for(j = 0; j < numRecords; j++) {
				m_records[p]->AddTail(new SlicRecord(archive));
			}
		}
		archive.Load((uint8*)m_timer, sizeof(m_timer));
		archive.Load((uint8*)m_triggerKey, sizeof(m_triggerKey));

		m_doResearchOnUnblank = archive.GetUINT8();
		archive >> m_researchOwner;
		archive.Load((uint8*)m_researchText, sizeof(m_researchText));

		m_currentKeyTrigger = KEY_UNDEFINED;
	}

	m_currentMessage->Serialize(archive);
	m_disabledClasses->Serialize(archive);
	m_constHash->Serialize(archive);
}

void SlicEngine::PostSerialize()
{
	m_symTab->PostSerialize();
	m_segmentHash->LinkTriggerSymbols(m_uiHash);

	AddModFuncs();
}

SlicSegment *SlicEngine::GetSegment(const char *id)
{
	return m_segmentHash->Access(id);
}

SlicFunc *SlicEngine::GetFunction(const char *name)
{
    return m_functionHash ? m_functionHash->Access(name) : NULL;
}

SlicNamedSymbol *SlicEngine::GetSymbol(sint32 index)
{
	return m_symTab->Access(index);
}

SlicNamedSymbol *SlicEngine::GetSymbol(const char *name)
{
	return m_symTab->StringHash<SlicNamedSymbol>::Access(name);
}

SlicNamedSymbol *SlicEngine::GetOrMakeSymbol(const char *name)
{
	SlicNamedSymbol *sym = m_symTab->StringHash<SlicNamedSymbol>::Access(name);
	if(!sym) {
		sym = new SlicNamedSymbol(name);
		m_symTab->Add(sym);
	}
	return sym;
}

SlicParameterSymbol *SlicEngine::GetParameterSymbol(const char *name, sint32 parameterIndex)
{
	SlicNamedSymbol *namedSym = m_symTab->StringHash<SlicNamedSymbol>::Access(name);

	SlicParameterSymbol *sym = (SlicParameterSymbol *)namedSym;

	if(!sym) {
		sym = new SlicParameterSymbol(name, parameterIndex);
		m_symTab->Add(sym);
	}
	Assert(sym->GetSerializeType() == SLIC_SYM_SERIAL_PARAMETER);
	if(sym->GetSerializeType() != SLIC_SYM_SERIAL_PARAMETER) {
		return NULL;
	}

	return sym;
}

void SlicEngine::Execute(SlicObject *obj)
{
	Assert(obj && !m_atBreak);
	if (!obj || m_atBreak)
		return;

	PushContext(obj);

	if (obj->IsValid()  &&
        ((obj->GetSegment()->GetFilenum() != k_TUTORIAL_FILE) || g_theProfileDB->IsTutorialAdvice()) &&
		(!g_theCriticalMessagesPrefs || g_theCriticalMessagesPrefs->IsEnabled(obj->GetSegment()->GetName()))
       ) 
    {
		obj->Execute();
	}

    if (m_atBreak)
    {
        // No action: keep context active to Continue later.
    }
    else
    {
	    PopContext();
    }
}

void SlicEngine::AddBuiltinFunctions()
{
	if (m_functionHash)
		return; // Already added

	m_functionHash = new StringHash<SlicFunc>(k_SEGMENT_HASH_SIZE);

	m_functionHash->Add(new Slic_PrintInt);
	m_functionHash->Add(new Slic_PrintText);
	m_functionHash->Add(new Slic_Text);
	m_functionHash->Add(new Slic_Message);
	m_functionHash->Add(new Slic_AddMessage);
	m_functionHash->Add(new Slic_MessageAll);
	m_functionHash->Add(new Slic_MessageAllBut);
	m_functionHash->Add(new Slic_EyePoint);
	m_functionHash->Add(new Slic_DisableTrigger);
	m_functionHash->Add(new Slic_EnableTrigger);
	m_functionHash->Add(new Slic_Return1);
	m_functionHash->Add(new Slic_Return0);
	m_functionHash->Add(new Slic_HasAdvance);
	m_functionHash->Add(new Slic_IsContinentBiggerThan);
	m_functionHash->Add(new Slic_IsHostile);
	m_functionHash->Add(new Slic_TradePoints);
	m_functionHash->Add(new Slic_TradeRoutes);
	m_functionHash->Add(new Slic_HasSameGoodAsTraded);
	m_functionHash->Add(new Slic_AddCity);
	m_functionHash->Add(new Slic_IsSecondRowUnit);
	m_functionHash->Add(new Slic_IsFlankingUnit);
	m_functionHash->Add(new Slic_IsBombardingUnit);
	m_functionHash->Add(new Slic_IsWormholeProbe);  
	m_functionHash->Add(new Slic_IsUnderseaCity);   
	m_functionHash->Add(new Slic_IsSpaceCity);      
	m_functionHash->Add(new Slic_IsSpaceUnit);      
	m_functionHash->Add(new Slic_IsWonderType);     
	m_functionHash->Add(new Slic_IsCounterBombardingUnit); 
	m_functionHash->Add(new Slic_IsCleric);         
	m_functionHash->Add(new Slic_IsSlaver);         
	m_functionHash->Add(new Slic_IsActiveDefender); 
	m_functionHash->Add(new Slic_IsDiplomat);
	m_functionHash->Add(new Slic_IsInRegion);
	m_functionHash->Add(new Slic_UnitHasFlag);
	m_functionHash->Add(new Slic_UnitsInCell);
	m_functionHash->Add(new Slic_PlayerCityCount);
	m_functionHash->Add(new Slic_RegardLevel);
	m_functionHash->Add(new Slic_ChangeRegardLevel);
	m_functionHash->Add(new Slic_Kill);
	m_functionHash->Add(new Slic_DeactivateTutorial);
	m_functionHash->Add(new Slic_ControlsRegion);
	m_functionHash->Add(new Slic_DemandWarFromAllies);
	m_functionHash->Add(new Slic_Accept);
	m_functionHash->Add(new Slic_Reject);
	m_functionHash->Add(new Slic_KnowledgeRank);
	m_functionHash->Add(new Slic_MilitaryRank);
	m_functionHash->Add(new Slic_TradeRank);
	m_functionHash->Add(new Slic_GoldRank);
	m_functionHash->Add(new Slic_PopulationRank);
	m_functionHash->Add(new Slic_CitiesRank);
	m_functionHash->Add(new Slic_GeographicRank);
	m_functionHash->Add(new Slic_SpaceRank);
	m_functionHash->Add(new Slic_UnderseaRank);
	m_functionHash->Add(new Slic_EyeDropdown);
	m_functionHash->Add(new Slic_CaptureCity);
	m_functionHash->Add(new Slic_CaptureRegion);
	m_functionHash->Add(new Slic_LeaveRegion);
	m_functionHash->Add(new Slic_Surrender);
	m_functionHash->Add(new Slic_Research);
	m_functionHash->Add(new Slic_MessageType);
	m_functionHash->Add(new Slic_Caption);
	m_functionHash->Add(new Slic_Duration);
	m_functionHash->Add(new Slic_BreakAgreement);
	m_functionHash->Add(new Slic_AcceptTradeOffer);
	m_functionHash->Add(new Slic_DontAcceptTradeOffer);
	m_functionHash->Add(new Slic_SetGovernment);
	m_functionHash->Add(new Slic_StealRandomAdvance);
	m_functionHash->Add(new Slic_StealSpecificAdvance);
	m_functionHash->Add(new Slic_DisableTileImprovementButton);
	m_functionHash->Add(new Slic_DisableScreensButton);
	m_functionHash->Add(new Slic_EnableTileImprovementButton);
	m_functionHash->Add(new Slic_EnableScreensButton);
	m_functionHash->Add(new Slic_OpenCiv);
	m_functionHash->Add(new Slic_OpenCity);
	m_functionHash->Add(new Slic_OpenUnit);
	m_functionHash->Add(new Slic_OpenScience);
	m_functionHash->Add(new Slic_OpenDiplomacy);
	m_functionHash->Add(new Slic_OpenTrade);
	m_functionHash->Add(new Slic_OpenInfo);
	m_functionHash->Add(new Slic_OpenOptions);
	m_functionHash->Add(new Slic_OpenCivTab);
	m_functionHash->Add(new Slic_OpenMaxTab);
	m_functionHash->Add(new Slic_OpenLaborTab);
	m_functionHash->Add(new Slic_OpenProductionTab);
	m_functionHash->Add(new Slic_OpenCityTab);
	m_functionHash->Add(new Slic_ExitToShell);
	m_functionHash->Add(new Slic_SendTradeBid);
	m_functionHash->Add(new Slic_AcceptTradeBid);
	m_functionHash->Add(new Slic_RejectTradeBid);
	m_functionHash->Add(new Slic_BreakAlliance);
	m_functionHash->Add(new Slic_AddOrder);
	m_functionHash->Add(new Slic_EndTurn);
	m_functionHash->Add(new Slic_FinishBuilding);
	m_functionHash->Add(new Slic_Abort);
	m_functionHash->Add(new Slic_Show);
    m_functionHash->Add(new Slic_DoAutoUnload);
    m_functionHash->Add(new Slic_DoLandInOcean);
    m_functionHash->Add(new Slic_DoOutOfFuel);
    m_functionHash->Add(new Slic_DoPillageOwnLand);
    m_functionHash->Add(new Slic_DoSellImprovement);
    m_functionHash->Add(new Slic_DoCertainRevolution);
    m_functionHash->Add(new Slic_DoFreeSlaves);
    m_functionHash->Add(new Slic_DoCannotAffordMaintenance);
    m_functionHash->Add(new Slic_DoCannotAffordSupport);
    m_functionHash->Add(new Slic_DoCityWillStarve);
    m_functionHash->Add(new Slic_DoYouWillBreakRoute);
	m_functionHash->Add(new Slic_TerrainType);
	m_functionHash->Add(new Slic_LibraryUnit);
	m_functionHash->Add(new Slic_LibraryBuilding);
	m_functionHash->Add(new Slic_LibraryWonder);
	m_functionHash->Add(new Slic_LibraryAdvance);
	m_functionHash->Add(new Slic_LibraryTerrain);
	m_functionHash->Add(new Slic_LibraryConcept);
	m_functionHash->Add(new Slic_LibraryGovernment);
	m_functionHash->Add(new Slic_LibraryTileImprovement);
	m_functionHash->Add(new Slic_UnitCount);
	m_functionHash->Add(new Slic_UnitType);
	m_functionHash->Add(new Slic_KillMessages);
	m_functionHash->Add(new Slic_MessageClass);
	m_functionHash->Add(new Slic_KillClass);
	m_functionHash->Add(new Slic_CityHasBuilding);
	m_functionHash->Add(new Slic_Title);
	m_functionHash->Add(new Slic_NetworkAccept);
	m_functionHash->Add(new Slic_NetworkEject);

	m_functionHash->Add(new Slic_Attract);
	m_functionHash->Add(new Slic_StopAttract);

	m_functionHash->Add(new Slic_DontSave);
	m_functionHash->Add(new Slic_IsUnitSelected);
	m_functionHash->Add(new Slic_IsCitySelected);
	m_functionHash->Add(new Slic_BuildingType);
	m_functionHash->Add(new Slic_IsHumanPlayer);
	m_functionHash->Add(new Slic_DisableClose);
	m_functionHash->Add(new Slic_EnableCloseClass);
	m_functionHash->Add(new Slic_EnableCloseMessage);
	m_functionHash->Add(new Slic_AddGoods);
	m_functionHash->Add(new Slic_GoodType);
	m_functionHash->Add(new Slic_GoodCount);
	m_functionHash->Add(new Slic_GoodCountTotal);
	m_functionHash->Add(new Slic_GoodVisibutik);
	m_functionHash->Add(new Slic_StartTimer);
	m_functionHash->Add(new Slic_StopTimer);
	m_functionHash->Add(new Slic_DisableMessageClass);
	m_functionHash->Add(new Slic_EnableMessageClass);
	m_functionHash->Add(new Slic_CreateUnit);
	m_functionHash->Add(new Slic_Random);
	m_functionHash->Add(new Slic_AddCityByIndex);
	m_functionHash->Add(new Slic_DetachRobot);
	m_functionHash->Add(new Slic_AttachRobot);
	m_functionHash->Add(new Slic_Cities);
	m_functionHash->Add(new Slic_ForceRegard);
	m_functionHash->Add(new Slic_AddPops);
	m_functionHash->Add(new Slic_KillUnit);
	m_functionHash->Add(new Slic_PlaySound);
	m_functionHash->Add(new Slic_CreateCity);
	m_functionHash->Add(new Slic_ExtractLocation);
	m_functionHash->Add(new Slic_CreateCoastalCity);
	m_functionHash->Add(new Slic_FindCoastalCity);
	m_functionHash->Add(new Slic_Terraform);
	m_functionHash->Add(new Slic_PlantGood);
	m_functionHash->Add(new Slic_GetRandomNeighbor);
	m_functionHash->Add(new Slic_GrantAdvance);
	m_functionHash->Add(new Slic_AddUnit);
	m_functionHash->Add(new Slic_AllUnitsCanBeExpelled);
	m_functionHash->Add(new Slic_AddExpelOrder);
	m_functionHash->Add(new Slic_GetMessageClass);
	m_functionHash->Add(new Slic_SetPlayer);
	m_functionHash->Add(new Slic_CityCollectingGood);
	m_functionHash->Add(new Slic_GetNearestWater);
	m_functionHash->Add(new Slic_IsPlayerAlive);
	m_functionHash->Add(new Slic_GameOver);
	m_functionHash->Add(new Slic_SaveGame);
	m_functionHash->Add(new Slic_LoadGame);
	m_functionHash->Add(new Slic_HasRiver);
	m_functionHash->Add(new Slic_SetScience);
	m_functionHash->Add(new Slic_SetResearching);
	m_functionHash->Add(new Slic_IsInZOC);
	m_functionHash->Add(new Slic_DisableChooseResearch);
	m_functionHash->Add(new Slic_EnableChooseResearch);
	m_functionHash->Add(new Slic_QuitToLobby);
	m_functionHash->Add(new Slic_KillEyepointMessage);
	m_functionHash->Add(new Slic_ClearBuildQueue);
	m_functionHash->Add(new Slic_BreakLeaveOurLands);
	m_functionHash->Add(new Slic_BreakNoPiracy);
	m_functionHash->Add(new Slic_UseDirector);
	m_functionHash->Add(new Slic_ClearOrders);
	m_functionHash->Add(new Slic_SetTimerGranularity);

	
	m_functionHash->Add(new Slic_SetUnit);
	m_functionHash->Add(new Slic_SetUnitByIndex);
	m_functionHash->Add(new Slic_SetCity);
	m_functionHash->Add(new Slic_SetCityByIndex);
	m_functionHash->Add(new Slic_SetLocation);
	m_functionHash->Add(new Slic_MakeLocation);
	m_functionHash->Add(new Slic_SetOrder);
	m_functionHash->Add(new Slic_Flood);
	m_functionHash->Add(new Slic_Ozone);
	m_functionHash->Add(new Slic_GodMode);
	m_functionHash->Add(new Slic_ExecuteAllOrders);
	m_functionHash->Add(new Slic_CatchUp);
	m_functionHash->Add(new Slic_Deselect);
	m_functionHash->Add(new Slic_Preference);
	m_functionHash->Add(new Slic_SetPreference);
	m_functionHash->Add(new Slic_AddMovement);
	m_functionHash->Add(new Slic_ToggleVeteran);
	m_functionHash->Add(new Slic_IsVeteran);

	
	m_functionHash->Add(new Slic_CantAttackUnit);
	m_functionHash->Add(new Slic_CantAttackCity);
	m_functionHash->Add(new Slic_CityCantRiotOrRevolt);
	m_functionHash->Add(new Slic_SelectUnit);
	m_functionHash->Add(new Slic_SelectCity);
	m_functionHash->Add(new Slic_CantEndTurn);
	m_functionHash->Add(new Slic_Heal);
	m_functionHash->Add(new Slic_AddGold);
	m_functionHash->Add(new Slic_SetActionKey);
	m_functionHash->Add(new Slic_GetCityByLocation);
	m_functionHash->Add(new Slic_GetNeighbor);

	m_functionHash->Add(new Slic_DamageUnit);

	m_functionHash->Add(new Slic_IsUnitInBuildList);
    m_functionHash->Add(new Slic_IsBuildingInBuildList);
    m_functionHash->Add(new Slic_IsWonderInBuildList);
    m_functionHash->Add(new Slic_IsEndgameInBuildList);
    m_functionHash->Add(new Slic_IsBuildingAtHead);
    m_functionHash->Add(new Slic_IsWonderAtHead);

    m_functionHash->Add(new Slic_AddUnitToBuildList);
    m_functionHash->Add(new Slic_AddBuildingToBuildList);
    m_functionHash->Add(new Slic_AddWonderToBuildList);
    m_functionHash->Add(new Slic_AddEndgameToBuildList);
    m_functionHash->Add(new Slic_KillUnitFromBuildList);
    m_functionHash->Add(new Slic_KillBuildingFromBuildList);
    m_functionHash->Add(new Slic_KillWonderFromBuildList);
    m_functionHash->Add(new Slic_KillEndgameFromBuildList);

	m_functionHash->Add(new Slic_SetPW);
	m_functionHash->Add(new Slic_Stacked);

	m_functionHash->Add(new Slic_SetString);
	m_functionHash->Add(new Slic_SetStringByDBIndex);
	m_functionHash->Add(new Slic_GetStringDBIndex);
	m_functionHash->Add(new Slic_UnitHasUserFlag);

	m_functionHash->Add(new Slic_BlankScreen);
	m_functionHash->Add(new Slic_AddCenter);
	m_functionHash->Add(new Slic_AddEffect);
	m_functionHash->Add(new Slic_PlayerCivilization);
	m_functionHash->Add(new Slic_CivilizationIndex);
	m_functionHash->Add(new Slic_ExitToDesktop);

	
	m_functionHash->Add(new Slic_Import);
	m_functionHash->Add(new Slic_Export);

	m_functionHash->Add(new Slic_GetUnitFromArmy);
	m_functionHash->Add(new Slic_GetUnitByIndex);
	m_functionHash->Add(new Slic_GetArmyByIndex);
	m_functionHash->Add(new Slic_GetCityByIndex);

	
	
	

	m_functionHash->Add(new Slic_LogRegardEvent);
	m_functionHash->Add(new Slic_GetPublicRegard);
	m_functionHash->Add(new Slic_GetEffectiveRegard);
	m_functionHash->Add(new Slic_GetTrust);
	m_functionHash->Add(new Slic_SetTrust);
	m_functionHash->Add(new Slic_RecomputeRegard);
	m_functionHash->Add(new Slic_ConsiderResponse);
	m_functionHash->Add(new Slic_SetResponse);;
	m_functionHash->Add(new Slic_ConsiderMotivation);
	m_functionHash->Add(new Slic_ConsiderNewProposal);
	m_functionHash->Add(new Slic_SetNewProposal);
	m_functionHash->Add(new Slic_ConsiderStrategicState);
	m_functionHash->Add(new Slic_ComputeCurrentStrategy);
	m_functionHash->Add(new Slic_ConsiderDiplomaticState);
	m_functionHash->Add(new Slic_ChangeDiplomaticState);
	m_functionHash->Add(new Slic_GetTradeFrom);
	m_functionHash->Add(new Slic_GetTributeFrom);
	m_functionHash->Add(new Slic_GetGoldSurplusPercent);
	m_functionHash->Add(new Slic_CanBuySurplus);
	m_functionHash->Add(new Slic_GetAdvanceLevelPercent);
	m_functionHash->Add(new Slic_AtWarCount);
	m_functionHash->Add(new Slic_EffectiveAtWarCount);
	m_functionHash->Add(new Slic_AtWarWith);
	m_functionHash->Add(new Slic_EffectiveWarWith);


	
	
	

	m_functionHash->Add(new Slic_HasAgreementWithAnyone);
	m_functionHash->Add(new Slic_HasAgreement);
	m_functionHash->Add(new Slic_CancelAgreement);
	m_functionHash->Add(new Slic_TurnsSinceLastWar);
	m_functionHash->Add(new Slic_TurnsAtWar);
	m_functionHash->Add(new Slic_GetLastHotwarAttack);
	m_functionHash->Add(new Slic_GetLastColdwarAttack);

	
	
	
	m_functionHash->Add(new Slic_GetNuclearLaunchTarget);
	m_functionHash->Add(new Slic_TargetNuclearAttack);

	
	
	m_functionHash->Add(new Slic_GetMapHeight);
	m_functionHash->Add(new Slic_GetMapWidth);

	m_functionHash->Add(new Slic_AddFeat);

	
	m_functionHash->Add(new Slic_IsFortress);

	m_functionHash->Add(new Slic_Distance);
	m_functionHash->Add(new Slic_SquaredDistance);
	m_functionHash->Add(new Slic_HasGood);

	
	m_functionHash->Add(new Slic_GetRiotLevel);
	m_functionHash->Add(new Slic_GetRevolutionLevel);

	
	m_functionHash->Add(new Slic_CityFoodDelta);
	m_functionHash->Add(new Slic_PlayerWagesExp);
	m_functionHash->Add(new Slic_PlayerWorkdayExp);
	m_functionHash->Add(new Slic_PlayerRationsExp);
	m_functionHash->Add(new Slic_PlayerWorkdayLevel);
	m_functionHash->Add(new Slic_PlayerRationsLevel);
	m_functionHash->Add(new Slic_PlayerWagesLevel);
	m_functionHash->Add(new Slic_CityStarvationTurns);

	m_functionHash->Add(new Slic_GetUnitsAtLocation);
	m_functionHash->Add(new Slic_GetUnitFromCell);


	
	m_functionHash->Add(new Slic_TradePointsInUse);

	m_functionHash->Add(new Slic_CityIsValid);
	m_functionHash->Add(new Slic_GetCurrentYear);
	m_functionHash->Add(new Slic_GetCurrentRound);

	m_functionHash->Add(new Slic_CellOwner);

	m_functionHash->Add(new Slic_CityIsNamed);

	
	m_functionHash->Add(new Slic_StringCompare);
	m_functionHash->Add(new Slic_CityNameCompare);	
	m_functionHash->Add(new Slic_ChangeGlobalRegard);
	m_functionHash->Add(new Slic_SetCityVisible);

	
	m_functionHash->Add(new Slic_IsCivilian);

	m_functionHash->Add(new Slic_GetArmyFromUnit);

	m_functionHash->Add(new Slic_FinishImprovements);

	m_functionHash->Add(new Slic_RemoveAdvance);
	m_functionHash->Add(new Slic_PlayerGold);
	m_functionHash->Add(new Slic_ClearBattleFlag);

	m_functionHash->Add(new Slic_MinimizeAction);

	
	m_functionHash->Add(new Slic_SetAllCitiesVisible);
	
	m_functionHash->Add(new Slic_IsUnitAtHead);
	m_functionHash->Add(new Slic_OpenScenarioEditor);

	//Readded Slic functions of CTP2.1 by Martin Gühmann
	m_functionHash->Add(new Slic_DestroyBuilding);
	m_functionHash->Add(new Slic_OpenBuildQueue);
	m_functionHash->Add(new Slic_TileHasImprovement);
	m_functionHash->Add(new Slic_PlayerHasWonder);
	m_functionHash->Add(new Slic_WonderOwner);
	m_functionHash->Add(new Slic_CityHasWonder);
	m_functionHash->Add(new Slic_ArmyIsValid);
	m_functionHash->Add(new Slic_GetBorderIncursionBy);
	m_functionHash->Add(new Slic_GetLastNewProposalType);
	m_functionHash->Add(new Slic_GetLastNewProposalArg);
	m_functionHash->Add(new Slic_GetLastNewProposalTone);
	m_functionHash->Add(new Slic_GetLastResponseType);
	m_functionHash->Add(new Slic_GetLastCounterResponseType);
	m_functionHash->Add(new Slic_GetLastCounterResponseArg);
	m_functionHash->Add(new Slic_GetLastThreatResponseType);
	m_functionHash->Add(new Slic_GetLastThreatResponseArg);
	m_functionHash->Add(new Slic_GetAgreementDuration);
	m_functionHash->Add(new Slic_GetNewProposalPriority);
	m_functionHash->Add(new Slic_GetNextAdvance);
	m_functionHash->Add(new Slic_GetDesiredAdvanceFrom);
	m_functionHash->Add(new Slic_GetLastBorderIncursion);
	m_functionHash->Add(new Slic_GetPersonalityType);
	m_functionHash->Add(new Slic_GetAtRiskCitiesValue);
	m_functionHash->Add(new Slic_GetRelativeStrength);
	m_functionHash->Add(new Slic_GetDesireWarWith);
	m_functionHash->Add(new Slic_RoundPercentReduction);
	m_functionHash->Add(new Slic_RoundGold);
	m_functionHash->Add(new Slic_GetPollutionLevelPromisedTo);
	m_functionHash->Add(new Slic_GetPiracyIncomeFrom);
	m_functionHash->Add(new Slic_GetProjectedScience);
	m_functionHash->Add(new Slic_CanFormAlliance);
	m_functionHash->Add(new Slic_GetStopResearchingAdvance);
	m_functionHash->Add(new Slic_GetNanoWeaponsCount);
	m_functionHash->Add(new Slic_GetBioWeaponsCount);
	m_functionHash->Add(new Slic_GetNuclearWeaponsCount);
	m_functionHash->Add(new Slic_FindCityToExtortFrom);
	m_functionHash->Add(new Slic_GetEmbargo);
	m_functionHash->Add(new Slic_SetEmbargo);
	m_functionHash->Add(new Slic_GetTotalValue);
	m_functionHash->Add(new Slic_GetNewProposalResult);
	m_functionHash->Add(new Slic_GetCounterProposalResult);
	m_functionHash->Add(new Slic_GetMostAtRiskCity);
	m_functionHash->Add(new Slic_GetRoundsToNextDisaster);
	m_functionHash->Add(new Slic_GetCurrentPollutionLevel);
	// New slicfunction by The Big Mc
	m_functionHash->Add(new Slic_FreeAllSlaves);
	m_functionHash->Add(new Slic_AddSlaves);
	// New good functions by MrBaggins
	m_functionHash->Add(new Slic_PlantSpecificGood);
	m_functionHash->Add(new Slic_RemoveGood);
	// Added by Peter Triggs
	m_functionHash->Add(new Slic_DeclareWar);
	// Added by Martin Gühmann
	m_functionHash->Add(new Slic_CargoCapacity);
	m_functionHash->Add(new Slic_MaxCargoSize);
	m_functionHash->Add(new Slic_CargoSize);
	m_functionHash->Add(new Slic_GetUnitFromCargo);
	m_functionHash->Add(new Slic_GetContinent);
	m_functionHash->Add(new Slic_GetContinentSize);
	m_functionHash->Add(new Slic_IsWater);
	//Added by Solver
	m_functionHash->Add(new Slic_IsOnSameContinent);
	//Added by E
	m_functionHash->Add(new Slic_KillCity);
	m_functionHash->Add(new Slic_Pillage);
	m_functionHash->Add(new Slic_Plunder);
	m_functionHash->Add(new Slic_Liberate);
	m_functionHash->Add(new Slic_AddPW);
	//m_functionHash->Add(new Slic_PuppetGovt);
	//Added by Maq
	m_functionHash->Add(new Slic_CreateBuilding);
	m_functionHash->Add(new Slic_CreateWonder);
	m_functionHash->Add(new Slic_UnitMovementLeft);

}

void SlicEngine::Link()
{
    if (!m_segmentHash)
    {
        m_segmentHash = new SlicSegmentHash(k_SEGMENT_HASH_SIZE);
    }

    if (!m_symTab) 
    {
        m_symTab = new SlicSymTab(0);
    }

    m_segmentHash->SetSize(g_slicNumEntries);

    for (sint32 i = 0; i < g_slicNumEntries; i++) 
    {
        SlicSegment * seg = new SlicSegment(i);
        m_segmentHash->Add(seg->GetName(), seg);
    }

    m_segmentHash->LinkTriggerSymbols(m_uiHash);
	
    slicif_init();
    AddModFuncs();
}


extern "C" char slic_parser_error_text[1024];

bool SlicEngine::Load(std::basic_string<MBCHAR> const & a_File, sint32 filenum)
{
	slicconst_Initialize();

	sint32 symStart = m_symTab->GetNumEntries();

    AddBuiltinFunctions();
	slicif_init();
	slicif_set_file_num(filenum);

    bool isParsedOk = 
        (SLIC_ERROR_OK == 
            slicif_run_parser(const_cast<MBCHAR *>(a_File.c_str()), symStart)
        );    

	if (!isParsedOk)
    {
		c3errors_ErrorDialog(a_File.c_str(), "%s", slic_parser_error_text);
	}

	return isParsedOk;
}

void SlicEngine::AddTrigger(SlicSegment *trigger, TRIGGER_LIST which)
{
	sint32 t;
	for(t = sint32(which); t < sint32(TRIGGER_LIST_MAX); t++) {
		if(m_triggerLists[t]->Find(trigger))
			return;
	}
	
	PointerList<SlicSegment>::PointerListNode *node;

	for(t = sint32(which) - 1; t >= 0; t--) {
		if((node = m_triggerLists[t]->Find(trigger)) != NULL) {
			m_triggerLists[t]->Remove(node);
		}
	}

	m_triggerLists[which]->AddTail(trigger);
}

void SlicEngine::SetCurrentMessage(const Message &message)
{
	*m_currentMessage = message;
}

void SlicEngine::GetCurrentMessage(Message &message) const
{
	message = *m_currentMessage;
}

void SlicEngine::KillCurrentMessage()
{
	if(!g_theMessagePool->IsValid(*m_currentMessage))
		return;
	m_currentMessage->Kill();
	*m_currentMessage = Message();
}

void SlicEngine::AddCurrentMessage()
{
	if(!g_theMessagePool->IsValid(*m_currentMessage))
		return;

	m_currentMessage->Minimize();
	*m_currentMessage = Message();
}

PointerList<SlicRecord> *SlicEngine::GetRecords(sint32 player)
{
	return m_records[player];
}

void SlicEngine::AddTutorialRecord(sint32 player, MBCHAR *title, MBCHAR *text,
								   SlicSegment *segment)
{
    if (!m_records[player]) 
    {
        m_records[player] = new PointerList<SlicRecord>;
    }

    for
    (
        PointerList<SlicRecord>::Walker walk(m_records[player]);
        walk.IsValid();
        walk.Next()
    )
    {
        if (walk.GetObj()->GetSegment() == segment) 
        {
            return;
	  }
    }

    m_records[player]->AddTail(new SlicRecord(player, title, text, segment));

    if (g_tutorialWin && title) 
    {
        g_tutorialWin->AddToList(title, m_records[player]->GetCount() - 1);
    }
}

bool SlicEngine::IsTimerExpired(sint32 timer) const
{
	Assert(timer >= 0);
	Assert(timer < k_NUM_TIMERS);

	if (timer < 0 || timer >= k_NUM_TIMERS)
		return false;

	if (m_timer[timer] < 0)
		return false;

	return time(0) >= static_cast<time_t>(m_timer[timer]);
}
		
void SlicEngine::StartTimer(sint32 timer, time_t duration)
{
	Assert(timer >= 0);
	Assert(timer < k_NUM_TIMERS);

	if (timer < 0 || timer >= k_NUM_TIMERS)
		return;

	m_timer[timer] = static_cast<sint32>(time(0) + duration);
    Assert(static_cast<time_t>(m_timer[timer]) == (time(0) + duration));
}

void SlicEngine::StopTimer(sint32 timer)
{
	Assert(timer >= 0);
	Assert(timer < k_NUM_TIMERS);

	if(timer < 0 || timer >= k_NUM_TIMERS)
		return;

	m_timer[timer] = NOT_IN_USE;
}
	
void SlicEngine::SetTutorialActive(BOOL on)
{ 
	if(!on && m_tutorialActive) {
		RunTrigger(TRIGGER_LIST_TUTORIAL_OFF,
				   ST_END);
	}

	m_tutorialActive = on; 
	if(!m_tutorialActive) {
		
		EnableMessageClass(k_NON_TUTORIAL_MESSAGE_CLASS);
		if(g_player && g_player[m_tutorialPlayer]) {
			sint32 i;
			DynamicArray<Message> *msgs = g_player[m_tutorialPlayer]->m_messages;
			for(i = 0; i < msgs->Num(); i++) {
				if(msgs->Access(i).AccessData()->GetSlicSegment() &&
				   msgs->Access(i).AccessData()->GetSlicSegment()->GetFilenum() == k_TUTORIAL_FILE) {
					g_player[m_tutorialPlayer]->m_messages->Access(i).AccessData()->DisableClose(FALSE);
					g_player[m_tutorialPlayer]->m_messages->Access(i).Kill();
				}
			}
		}
	}
}

void SlicEngine::EnableMessageClass(sint32 mclass)
{
	for (sint32 i = m_disabledClasses->Num() - 1; i >= 0; --i) 
	{
		if (m_disabledClasses->Access(i) == mclass) 
		{
			m_disabledClasses->DelIndex(i);
		}
	}
}

void SlicEngine::DisableMessageClass(sint32 mclass)
{
	m_disabledClasses->Insert(mclass);
}

bool SlicEngine::IsMessageClassDisabled(sint32 mclass) const
{
	for (sint32 i = 0; i < m_disabledClasses->Num(); ++i) 
	{
		if (m_disabledClasses->Access(i) == mclass) 
		{
			return true;
		}
	}
	return false;
}

void SlicEngine::ProcessUITriggers()
{
    for 
    (
        SlicObject *    obj = m_uiExecuteObjects->RemoveHead();
        obj;
        obj = m_uiExecuteObjects->RemoveHead()
    )
    {
        Execute(obj);
    }
}

void SlicEngine::RunYearlyTriggers()
{
    for
    (
        PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_YEARLY]);
        walk.IsValid();
        walk.Next()
    )
    {
        if (walk.GetObj()->IsEnabled()) 
        {
		Execute(new SlicObject(walk.GetObj()));
	  }
    }
}

void SlicEngine::RunPlayerTriggers(PLAYER_INDEX player)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_PLAYER]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddPlayer(player);
			Execute(obj);
		}
		walk.Next();
    }
}

void SlicEngine::RunCityTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCity(city);
			obj->AddPlayer(city.GetOwner());
			Execute(obj);
		}
		walk.Next();
    }
}

void SlicEngine::RunCityPopTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY_POP]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCity(city);
			obj->AddCivilisation(city.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}
	
void SlicEngine::RunClickedUnitTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CLICKED_UNIT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit);
			obj->AddCivilisation(unit.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunSelectedUnitTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_SELECTED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit);
			obj->AddCivilisation(unit.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunDeselectedUnitTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_DESELECTED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit);
			obj->AddPlayer(unit.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunDeselectedCityTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY_DESELECTED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCity(city);
			obj->AddPlayer(city.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunIdleTriggers(sint32 seconds)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_IDLE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->SetIdle(seconds);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunUnitMovedTriggers(const Unit &u)
{
	
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_MOVED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(u);
			obj->AddCivilisation(u.GetOwner());
			Execute(obj);
			if(!u.IsValid())
				return;
		}
		walk.Next();
	}
}

void SlicEngine::RunAllUnitsMovedTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_ALL_UNITS_MOVED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(m_tutorialPlayer);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunCityBuiltTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY_BUILT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCity(city);
			obj->AddPlayer(city.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunUnitBuiltTriggers(const Unit &u, const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_BUILT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if(!u.IsValid()) {
				return;
			}
			if(!city.IsValid()) {
				return;
			}
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(u);
			obj->AddCity(city);
			obj->AddPlayer(u.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunDiscoveryTriggers(AdvanceType adv, PLAYER_INDEX p)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_DISCOVERY]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddAdvance(adv);
			obj->AddPlayer(p);
			Execute(obj);
		}
		walk.Next();
	}
}
	
void SlicEngine::RunContactTriggers(const Unit &unit1, const Unit &unit2)
{
	if (    unit1.IsValid() 
         &&	unit2.IsValid()
         && g_player[unit1.GetOwner()]
         && g_player[unit2.GetOwner()]
       ) 
    {
	    PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CONTACT]);
	    while(walk.IsValid()) {
		    if(walk.GetObj()->IsEnabled()) {
			    SlicObject *obj = new SlicObject(walk.GetObj());
			    obj->AddUnit(unit1);
			    obj->AddUnit(unit2);
			    obj->AddPlayer(unit1.GetOwner());
			    obj->AddPlayer(unit2.GetOwner());
			    Execute(obj);
		    }
		    walk.Next();
	    }
    }
}

void SlicEngine::RunAttackTriggers(const Unit &unit1, const Unit &unit2)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_ATTACK]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit1);
			obj->AddUnit(unit2);
			obj->AddPlayer(unit1.GetOwner());
			obj->AddPlayer(unit2.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}


void SlicEngine::RunTradeScreenTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_TRADE_SCREEN]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddPlayer(g_selected_item->GetVisiblePlayer());
				Execute(obj);
			}
		}
		walk.Next();
	}
}

void SlicEngine::RunSameGoodTriggers(const Unit &city1, const Unit &city2)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_SAME_GOOD]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddPlayer(g_selected_item->GetVisiblePlayer());
				obj->AddCity(city1);
				obj->AddCity(city2);
				Execute(obj);
			}
		}
		walk.Next();
	}
}

bool SlicEngine::SpecialSameGoodEnabled() const
{
	return !m_triggerLists[TRIGGER_LIST_SAME_GOOD]->IsEmpty();
}

void SlicEngine::RunSameGoodAsTradedTriggers(sint32 good, const Unit &city1)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_SAME_GOOD_AS_TRADED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				obj->AddCity(city1);
				Execute(obj);
			}
		}
		walk.Next();
	}
}
	

void SlicEngine::RunUnitQueueTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_QUEUE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				Execute(obj);
			}
		}
		walk.Next();
	}
}
	

void SlicEngine::RunProductionQueueTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_PRODUCTION_QUEUE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				Execute(obj);
			}
		}
		walk.Next();
	}
}
	

void SlicEngine::RunDiplomaticScreenTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_DIPLOMATIC_SCREEN]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				Execute(obj);
			}
		}
		walk.Next();
	}
}

void SlicEngine::RunCreateStackTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CREATE_STACK]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				Execute(obj);
			}
		}
		walk.Next();
	}
}

void SlicEngine::RunCreateMixedStackTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CREATE_MIXED_STACK]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				Execute(obj);
			}
		}
		walk.Next();
	}
}
	

void SlicEngine::RunAutoArrangeOffTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_AUTO_ARRANGE_OFF]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			if (g_player[g_selected_item->GetVisiblePlayer()] != NULL) {
				SlicObject *obj = new SlicObject(walk.GetObj());
				obj->AddCivilisation(*g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation);
				Execute(obj);
			}
		}
		walk.Next();
	}
}

void SlicEngine::RunBombardmentTriggers(const Unit &attacker,
										const Unit &defender)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BOMBARDMENT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[attacker.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[defender.GetOwner()]->m_civilisation);
			obj->AddUnit(attacker);
			obj->AddUnit(defender);

			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunCounterBombardmentTriggers(const Unit &bombarder,
											   const Unit &counterbombarder)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_COUNTER_BOMBARDMENT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[bombarder.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[counterbombarder.GetOwner()]->m_civilisation);
			obj->AddUnit(bombarder);
			obj->AddUnit(counterbombarder);

			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunActiveDefenseTriggers(const Unit &defender, const Unit &aggressor)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_ACTIVE_DEFENSE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[defender.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[aggressor.GetOwner()]->m_civilisation);
			obj->AddUnit(defender);
			obj->AddUnit(aggressor);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunIndulgenceTriggers(const Unit &cleric, const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_INDULGENCES]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[cleric.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddUnit(cleric);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunTerrorismTriggers(const Unit &terrorist, const Unit &target)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_TERRORISM]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[terrorist.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[target.GetOwner()]->m_civilisation);
			obj->AddUnit(terrorist);
			obj->AddCity(target);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunConversionTriggers(const Unit &cleric, const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CONVERSION]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[cleric.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddUnit(cleric);
			obj->AddUnit(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunWonderStartedTriggers(const Unit &city, sint32 wondertype)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_WONDER_STARTED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddCity(city);
			obj->AddWonder(wondertype);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunWonderFinishedTriggers(const Unit &city, sint32 wondertype)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_WONDER_FINISHED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddCity(city);
			obj->AddWonder(wondertype);
			Execute(obj);
		}
		walk.Next();
	}
}

#if 0

void SlicEngine::RunUnitSightedTriggers(const Unit &sighted, const Unit &sightedby)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_SIGHTED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[sighted.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[sightedby.GetOwner()]->m_civilisation);
			obj->AddUnit(sighted);
			obj->AddUnit(sightedby);
			Execute(obj);
		}
		walk.Next();
	}
}
#endif


void SlicEngine::RunEnslavementTriggers(const Unit &slaver, const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_ENSLAVEMENT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[slaver.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddUnit(slaver);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunSettlerEnslavedTriggers(const Unit &slaver, 
											sint32 settlerOwner)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_SETTLERENSLAVED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[slaver.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[settlerOwner]->m_civilisation);
			obj->AddUnit(slaver);
			Execute(obj);
		}
		walk.Next();
	}
}
	
void SlicEngine::RunVictoryEnslavementTriggers(const Unit &slaver, 
											   sint32 slavee,
											   Unit &hc)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_VICTORYENSLAVEMENT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[slaver.GetOwner()]->m_civilisation);
			obj->AddCivilisation(*g_player[slavee]->m_civilisation);
			obj->AddUnit(slaver);
			obj->AddCity(hc);
			Execute(obj);
		}
		walk.Next();
	}
}


void SlicEngine::RunUnitLaunchedTriggers(const Unit &launchee)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_LAUNCHED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[launchee.GetOwner()]->m_civilisation);
			obj->AddUnit(launchee);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunUnitBeginTurnTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_BEGIN_TURN]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			obj->AddUnit(unit);
			Execute(obj);
			if(!unit.IsValid())
				return;
			if(!g_player[unit.GetOwner()])
				return;
		}
		walk.Next();
	}
}
	
void SlicEngine::RunPopMovedTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_POP_MOVED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunBuildFarmTriggers(sint32 owner, const MapPoint &point,
									 sint32 type)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BUILD_FARM]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[owner]->m_civilisation);
			obj->AddLocation(point);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunBuildRoadTriggers(sint32 owner, const MapPoint &point,
									  sint32 type)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BUILD_ROAD]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[owner]->m_civilisation);
			obj->AddLocation(point);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunBuildMineTriggers(sint32 owner, const MapPoint &point,
									  sint32 type)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BUILD_MINE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[owner]->m_civilisation);
			obj->AddLocation(point);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunBuildInstallationTriggers(sint32 owner, const MapPoint &point,
									  sint32 type)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BUILD_INSTALLATION]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[owner]->m_civilisation);
			obj->AddLocation(point);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunBuildTransformTriggers(sint32 owner, const MapPoint &point,
									  sint32 type)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BUILD_TRANSFORM]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			obj->AddCivilisation(*g_player[owner]->m_civilisation);
			obj->AddLocation(point);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunScienceRateTriggers(sint32 owner)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_SCIENCE_RATE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[owner]->m_civilisation);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunCityCapturedTriggers(sint32 newowner, sint32 oldowner, const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY_CAPTURED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[newowner]->m_civilisation);
			obj->AddCivilisation(*g_player[oldowner]->m_civilisation);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}
	
void SlicEngine::RunTradeOfferTriggers(const TradeOffer &offer)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_TRADE_OFFER]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddGood(offer.GetOfferResource());
			obj->AddGold(offer.GetAskingResource());
			obj->AddCivilisation(*g_player[offer.GetFromCity().GetOwner()]->m_civilisation);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunTreatyBrokenTriggers(sint32 pl1, sint32 pl2, const Agreement &ag)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_TREATY_BROKEN]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[pl1]->m_civilisation);
			obj->AddCivilisation(*g_player[pl2]->m_civilisation);
			
			
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunUnitDeadTriggers(const Unit &unit, PLAYER_INDEX killedBy)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_DEAD]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			if(killedBy >= 0) {
				obj->AddCivilisation(killedBy);
			}
			obj->AddUnit(unit);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunOutOfFuelTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_DEAD_OUTOFFUEL]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			obj->AddUnit(unit);
			Execute(obj);
		}
		walk.Next();
	}
}
void SlicEngine::RunUnitCantBeSupportedTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_DEAD_CANT_SUPPORT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			obj->AddUnit(unit);
			Execute(obj);
		}
		walk.Next();
	}
}
void SlicEngine::RunMiscUnitDeathTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_DEAD_MISC]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			obj->AddUnit(unit);
			Execute(obj);
		}
		walk.Next();
	}
}


void SlicEngine::RunDiscoveryTradedTriggers(sint32 pl1, sint32 pl2, AdvanceType adv)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_DISCOVERY_TRADED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(*g_player[pl1]->m_civilisation);
			obj->AddCivilisation(*g_player[pl2]->m_civilisation);
			obj->AddAdvance(adv);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunUITriggers(const MBCHAR *controlName)
{
	DPRINTF(k_DBG_UI, ("SLIC: control %s used\n", controlName));

	SlicUITrigger * trig = 
		controlName ? m_uiHash->Access(controlName) : NULL;

	if(trig) {
		SlicSegment *seg = trig->GetSegment();
		if(seg && seg->IsEnabled()) {
			m_uiExecuteObjects->AddTail(new SlicObject(seg));
			
		}
	}
}

void SlicEngine::RunHelpTriggers(const MBCHAR *helpName)
{
	DPRINTF(k_DBG_UI, ("SLIC: help for component %s requested\n", helpName));
	if(!helpName)
		return;

	SlicSegment *seg = m_segmentHash->Access(helpName);
	if(seg && seg->IsEnabled()) {
		SlicObject *obj = new SlicObject(seg);
		if (g_selected_item)
			obj->AddRecipient(g_selected_item->GetVisiblePlayer());
		Execute(obj);
	}
}

void SlicEngine::RunPopMovedOffGoodTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_POP_MOVED_OFF_GOOD]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunWastingWorkTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY_BUILDINGNOTHING]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(*g_player[city.GetOwner()]->m_civilisation);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunPublicWorksTaxTriggers(sint32 owner)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_PUBLIC_WORKS_TAX]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			
			obj->AddCivilisation(owner);
			Execute(obj);
		}
		walk.Next();
	}
}
	
void SlicEngine::RunWonderAlmostDoneTriggers(const Unit &city, sint32 wonder)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_WONDER_ALMOST_DONE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCity(city);
			obj->AddCivilisation(city.GetOwner());
			obj->AddWonder(wonder);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunGovernmentChangedTriggers(sint32 player)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_GOVERNMENT_CHANGED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(player);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunTradeRouteTriggers(TradeRoute &route, sint32 gold)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_TRADE_ROUTE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(route.GetOwner());
			obj->AddCity(route.GetSource());
			obj->AddCity(route.GetDestination());
			obj->AddGold(gold);
			sint32 resource;
			ROUTE_TYPE type;
			route.GetSourceResource(type, resource);
			obj->AddGood(resource);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunForeignTradeRouteTriggers(TradeRoute &route, sint32 gold)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_FOREIGN_TRADE_ROUTE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(route.GetSource().GetOwner());
			obj->AddCivilisation(route.GetDestination().GetOwner());
			obj->AddCity(route.GetSource());
			obj->AddCity(route.GetDestination());
			obj->AddGold(gold);
			sint32 resource;
			ROUTE_TYPE type;
			route.GetSourceResource(type, resource);
			obj->AddGood(resource);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunUnitDoneMovingTriggers(const Unit &unit)
{
	
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_UNIT_DONE_MOVING]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit);
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunPiracyTriggers(const TradeRoute &route, const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_PIRACY]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());

			
			obj->AddCivilisation(route.GetSource().GetOwner());
			
			obj->AddCivilisation(route.GetDestination().GetOwner());
			
			obj->AddCivilisation(*g_player[unit.GetOwner()]->m_civilisation);
			obj->AddUnit(unit);

			
			obj->AddCity(route.GetSource());
			
			obj->AddCity(route.GetDestination());

			ROUTE_TYPE type;
			sint32 resource;
			route.GetSourceResource(type, resource);
			Assert(type == ROUTE_TYPE_RESOURCE);
			obj->AddGood(resource);

			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunPillageTriggers(const Unit &unit, PLAYER_INDEX pillagee)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_PIRACY]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit);
			obj->AddCivilisation(unit.GetOwner());
			obj->AddCivilisation(pillagee);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunCitySelectedTriggers(const Unit &city)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CITY_SELECTED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCity(city);
			obj->AddCivilisation(city.GetOwner());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunClickedOnUnexploredTriggers(const MapPoint &pos)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CLICKED_UNEXPLORED]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddLocation(pos);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunZOCTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_ZOC]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunCantSettleMovementTriggers(const Unit &unit)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CANT_SETTLE_MOVEMENT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddUnit(unit);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunBuildingBuiltTriggers(const Unit &city, sint32 building)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_BUILDING_BUILT]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddBuilding(building);
			obj->AddCity(city);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunAgeChangeTriggers(sint32 player)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_AGE_CHANGE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(player);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunTimerTriggers()
{
	for (sint32 i = 0; i < k_NUM_TIMERS; ++i)
	{
		if (IsTimerExpired(i))
		{
			m_timer[i] = NOT_IN_USE;
            g_gevManager->AddEvent(GEV_INSERT_Tail,GEV_TimerExpired,GEA_Int,i, GEA_End);
		}
	}
}

void SlicEngine::RunWorkViewTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_WORK_VIEW]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunSentCeaseFireTriggers(sint32 owner, sint32 recipient)
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_CEASE_FIRE]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			obj->AddCivilisation(owner);
			obj->AddCivilisation(recipient);
			Execute(obj);
		}
		walk.Next();
	}
}

void SlicEngine::RunTrigger(TRIGGER_LIST tlist, ...)
{
	Assert(tlist >= TRIGGER_LIST_YEARLY);
	Assert(tlist < TRIGGER_LIST_MAX);
	if(tlist < TRIGGER_LIST_YEARLY || tlist >= TRIGGER_LIST_MAX)
		return;

	Unit u;
	sint32 player;
	sint32 good, gold, advance;
	MapPoint pos;
	char *str;

	BOOL abort = FALSE;

	PointerList<SlicSegment>::Walker walk(m_triggerLists[tlist]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			BOOL done = FALSE;
			SLIC_TAG tag;
			va_list vl;
			va_start(vl, tlist);
			do {
				tag = va_arg(vl, SLIC_TAG);
				switch(tag) {
					case ST_END:
						done = TRUE;
						break;
					case ST_UNIT:
						u = va_arg(vl, Unit);
						if (u.IsValid()) 
                        {
							obj->AddUnit(u);
						} else {
							abort = TRUE;
						}
						break;
					case ST_CITY:
						u = va_arg(vl, Unit);
						if (u.IsValid()) 
                        {
							obj->AddCity(u);
						} else {
							abort = TRUE;
						}
						break;
					case ST_PLAYER:
						player = va_arg(vl, sint32);
						if(g_player[player]) {
							obj->AddCivilisation(player);
						} else {
							abort = TRUE;
						}
						break;
					case ST_GOOD:
						good = va_arg(vl, sint32);
						obj->AddGood(good);
						break;
					case ST_GOLD:
						gold = va_arg(vl, sint32);
						obj->AddGold(gold);
						break;
					case ST_LOCATION:
						pos = va_arg(vl, MapPoint);
						obj->AddLocation(pos);
						break;
					case ST_ACTION:
						str = va_arg(vl, char*);
						obj->AddAction(str);
						break;
					case ST_ADVANCE:
						advance = va_arg(vl, sint32);
						obj->AddAdvance(advance);
						break;
					default:
					{
						BOOL Unknown_Tag = FALSE;
						Assert(Unknown_Tag);
						break;
					}
				}
			} while(!done && !abort);
			va_end(vl);
			if(!abort) {
				Execute(obj);
			} else {
				delete obj;
			}
		}
		if(abort)
			break;
		walk.Next();
	}
}

void SlicEngine::RecreateTutorialRecord()
{
	if(!g_tutorialWin)
		return;

	sint32 c = 0;
	if(m_tutorialActive) {
		if(m_records[m_tutorialPlayer]) {
			PointerList<SlicRecord>::Walker walk(m_records[m_tutorialPlayer]);
			while(walk.IsValid()) {
				g_tutorialWin->AddToList(walk.GetObj()->AccessTitle(), c++);
				walk.Next();
			}
		}
	}
}

MBCHAR SlicEngine::GetTriggerKey(sint32 index)
{
	Assert(index >= 0);
	Assert(index < k_MAX_TRIGGER_KEYS);
	if(index >= 0 && index < k_MAX_TRIGGER_KEYS)
		return m_triggerKey[index];
	return 0;
}

void SlicEngine::SetTriggerKey(sint32 index, MBCHAR key)
{
	Assert(index >= 0);
	Assert(index < k_MAX_TRIGGER_KEYS);
	if(index >= 0 && index < k_MAX_TRIGGER_KEYS)
		m_triggerKey[index] = key;
}

bool SlicEngine::IsKeyPressed(MBCHAR key) const
{
	return m_currentKeyTrigger && (m_currentKeyTrigger == m_triggerKey[key]);
}

bool SlicEngine::RunKeyboardTrigger(MBCHAR key)
{
	for (size_t i = 0; i < k_MAX_TRIGGER_KEYS; i++) 
	{
		if (m_triggerKey[i] == key) 
		{
			m_currentKeyTrigger = key;
			RunTrigger(TRIGGER_LIST_KEY_PRESSED, ST_END);
			m_currentKeyTrigger = KEY_UNDEFINED;
			return true;
		}
	}

	return false;
}

void SlicEngine::BlankScreen(bool blank)
{
	if (m_blankScreen != blank) 
	{
		m_blankScreen = blank;
		g_tiledMap->InvalidateMap();
		g_tiledMap->Refresh();
		g_radarMap->Update();

		if (m_blankScreen)
		{
			MainControlPanel::Blank();
			if (g_greatLibrary)
			{
				g_greatLibrary->ClearHistory();
			}
		}
		else
		{
			CheckPendingResearch();

			PLAYER_INDEX const		player	= g_selected_item->GetVisiblePlayer();

			MainControlPanel::UpdatePlayer(player);
			MainControlPanel::UpdateCityList();
			MainControlPanel::Update();

			if (g_greatLibrary)
			{
				AdvanceType const	advance	= g_player[player]->m_advances->GetResearching();
				g_greatLibrary->SetLibrary(advance, DATABASE_ADVANCES);
			}
		}
	}
}

void SlicEngine::CheckPendingResearch()
{
	if(m_doResearchOnUnblank) {
		m_doResearchOnUnblank = FALSE;
		if(m_researchOwner == g_selected_item->GetVisiblePlayer()) {
			g_director->AddInvokeResearchAdvance(m_researchText);
		}
	}
}

void SlicEngine::AddResearchOnUnblank(sint32 owner, MBCHAR *text)
{
	strcpy(m_researchText, text);
	m_researchOwner = owner;
	m_doResearchOnUnblank = TRUE;
}

SlicSymbolData *SlicEngine::CheckForBuiltinWithIndex(MBCHAR *name, sint32 &index)
{
	return FALSE;
}

void SlicEngine::AddConst(const MBCHAR *name, sint32 value)
{
	m_constHash->Add(new SlicConst(name, value));
}

bool SlicEngine::FindConst(const MBCHAR *name, sint32 *value) const
{
	SlicConst * sc = m_constHash->Access(name);
	if (sc) 
	{
		*value = sc->GetValue();
		return true;
	}
	return false;
}

void SlicEngine::AddStructArray(bool createSymbols, SlicStructDescription *desc, SLIC_BUILTIN which)
{
	m_builtin_desc[which] = desc;

	if (createSymbols) 
    {
        SlicBuiltinNamedSymbol *    newSymbol = 
		    new SlicBuiltinNamedSymbol(which, desc->GetName(), new SlicArray(desc));
		m_builtins[which] = newSymbol;  // not deleted, managed through m_symTab
        m_symTab->Add(newSymbol);
	}
}

void SlicEngine::AddStruct(bool createSymbols, SlicStructDescription *desc, SLIC_BUILTIN which)
{
    m_builtin_desc[which] = desc;

    if (createSymbols) 
    {
        SlicBuiltinNamedSymbol *    newSymbol = 
		    new SlicBuiltinNamedSymbol(which, desc->GetName(), desc);
        m_builtins[which] = newSymbol;  // not deleted, managed through m_symTab
		m_symTab->Add(newSymbol);
	}
}

void SlicEngine::AddStructs(bool createSymbols)
{
	AddStruct(createSymbols, new SlicStruct_Global, SLIC_BUILTIN_GLOBAL);

	AddStructArray(createSymbols, new SlicStruct_Unit, SLIC_BUILTIN_UNIT);
	AddStructArray(createSymbols, new SlicStruct_City, SLIC_BUILTIN_CITY);
	AddStructArray(createSymbols, new SlicStruct_Player, SLIC_BUILTIN_PLAYER);
	AddStructArray(createSymbols, new SlicStruct_Army, SLIC_BUILTIN_ARMY);
	AddStructArray(createSymbols, new SlicStruct_Location, SLIC_BUILTIN_LOCATION);
	AddStructArray(createSymbols, new SlicStruct_Government, SLIC_BUILTIN_GOVERNMENT);
	AddStructArray(createSymbols, new SlicStruct_Advance, SLIC_BUILTIN_ADVANCE);
	AddStructArray(createSymbols, new SlicStruct_Action, SLIC_BUILTIN_ACTION);
	AddStructArray(createSymbols, new SlicStruct_Value, SLIC_BUILTIN_VALUE);
	AddStructArray(createSymbols, new SlicStruct_Improvement, SLIC_BUILTIN_IMPROVEMENT);
	AddStructArray(createSymbols, new SlicStruct_Building, SLIC_BUILTIN_BUILDING);
	AddStructArray(createSymbols, new SlicStruct_Wonder, SLIC_BUILTIN_WONDER);
	AddStructArray(createSymbols, new SlicStruct_UnitRecord, SLIC_BUILTIN_UNITRECORD);
	AddStructArray(createSymbols, new SlicStruct_Gold, SLIC_BUILTIN_GOLD);
	AddStructArray(createSymbols, new SlicStruct_Good, SLIC_BUILTIN_GOOD);

	if(createSymbols && 0) {
		m_symTab->Add(new SlicNamedSymbol("special", new SlicArray(SS_TYPE_SYM, SLIC_SYM_STRUCT)));
		m_symTab->Add(new SlicNamedSymbol("discovery", new SlicArray(SS_TYPE_SYM, SLIC_SYM_STRUCT)));
		m_symTab->Add(new SlicNamedSymbol("gold", new SlicArray(SS_TYPE_SYM, SLIC_SYM_STRUCT)));
		m_symTab->Add(new SlicNamedSymbol("pop", new SlicArray(SS_TYPE_SYM, SLIC_SYM_STRUCT)));
	}

}

void SlicEngine::AddBuiltinSymbol(SlicBuiltinNamedSymbol *sym)
{
    size_t const index = sym->GetBuiltin();

    Assert(index < SLIC_BUILTIN_MAX);
	m_builtins[index] = sym;
}

SlicSymbolData const * SlicEngine::GetBuiltinSymbol(SLIC_BUILTIN which) const
{
	Assert(which >= 0);
	Assert(which < SLIC_BUILTIN_MAX);
	if(which < 0 || which >= SLIC_BUILTIN_MAX)
		return NULL;

	return m_builtins[which];
}


void SlicEngine::AddSymbol(SlicNamedSymbol *sym)
{
	m_symTab->Add(sym);
}


void SlicEngine::SetContext(SlicObject *obj)
{
	if (m_context)
	{
		m_context->Release();
	}

	m_context = obj;

	if (obj)
	{
		obj->AddRef();
		obj->FillBuiltins();
	}
}

SlicStructDescription *SlicEngine::GetStructDescription(SLIC_SYM which)
{
	switch(which) {
		case SLIC_SYM_UNIT: return m_builtin_desc[SLIC_BUILTIN_UNIT];
		case SLIC_SYM_CITY: return m_builtin_desc[SLIC_BUILTIN_CITY];
		case SLIC_SYM_ARMY: return m_builtin_desc[SLIC_BUILTIN_ARMY];
		case SLIC_SYM_LOCATION: return m_builtin_desc[SLIC_BUILTIN_LOCATION];
		case SLIC_SYM_PLAYER: return m_builtin_desc[SLIC_BUILTIN_PLAYER];
		case SLIC_SYM_POP: return m_builtin_desc[SLIC_BUILTIN_POP];

		default:
			return NULL;
	}
}

SlicStructDescription *SlicEngine::GetStructDescription(SLIC_BUILTIN which)
{
	Assert(which >= 0);
	Assert(which < SLIC_BUILTIN_MAX);
	if(which < 0 || which >= SLIC_BUILTIN_MAX)
		return NULL;
	return m_builtin_desc[which];
}

void SlicEngine::Break(SlicSegment *segment, sint32 offset, SlicObject *context, SlicStack *stack,
					   MessageData *message)
{
	
	Assert(!m_atBreak);
	if(m_atBreak)
		return;

	m_breakRequested = false;
	m_breakContext = context;
	m_breakContext->CopyFromBuiltins();
	m_breakContext->AddRef();
	
	
	m_atBreak = true;

#ifdef CTP2_ENABLE_SLICDEBUG
	sourcelist_RegisterBreak(segment, offset);
#endif
}

void SlicEngine::Continue()
{
	
	Assert(m_atBreak);
	if(!m_atBreak)
		return;

	m_atBreak = false;

	SetContext(m_breakContext);
	m_breakContext->Continue();
	m_breakContext->Release();
    m_breakContext = NULL;

	if (!m_atBreak) 
    {
	    for 
        (
            SlicObject * oldContext = m_contextStack->RemoveTail();
            oldContext;
            oldContext = m_contextStack->RemoveTail()
        ) 
        {
			SetContext(oldContext);
			oldContext->Continue();
		}
	}

	SetContext(NULL);
}

void SlicEngine::RequestBreak()
{
	m_breakRequested = true;
}

bool SlicEngine::BreakRequested()
{
	return m_breakRequested;
}

void SlicEngine::PushContext(SlicObject * obj)
{
	if (m_context)
	{
		m_contextStack->AddTail(m_context);
	}

	m_context = obj;

	if (obj)
	{
		obj->AddRef();
		obj->FillBuiltins();
	}
}

void SlicEngine::PopContext()
{
	if (m_context)
	{
		m_context->Release();
	}

	m_context = m_contextStack->RemoveTail();

	if(m_context){
		// TODO check whether builtins filling is superflous.
		m_context->FillBuiltins(); // Builtins were overwritten in PushContext
	}
}

void SlicEngine::AddDatabases()
{
	if(m_dbHash)
		return;

	m_dbHash = new StringHash<SlicDBInterface>(k_DB_HASH_SIZE);

	m_dbHash->Add(new SlicDBConduit<UnitRecord, UnitRecordAccessorInfo>("UnitDB", g_theUnitDB,
																		g_UnitRecord_Accessors,
																		g_Unit_Tokens,
																		k_Num_UnitRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<AdvanceRecord, AdvanceRecordAccessorInfo>("AdvanceDB", g_theAdvanceDB,
																			  g_AdvanceRecord_Accessors,
																			  g_Advance_Tokens,
																			  k_Num_AdvanceRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<TerrainRecord, TerrainRecordAccessorInfo>("TerrainDB", g_theTerrainDB,
																			  g_TerrainRecord_Accessors,
																			  g_Terrain_Tokens,
																			  k_Num_TerrainRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<BuildingRecord, BuildingRecordAccessorInfo>("BuildingDB", g_theBuildingDB,
																				g_BuildingRecord_Accessors,
																				g_Building_Tokens,
																				k_Num_BuildingRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<WonderRecord, WonderRecordAccessorInfo>("WonderDB", g_theWonderDB,
																			g_WonderRecord_Accessors,
																			g_Wonder_Tokens,
																			k_Num_WonderRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<FeatRecord, FeatRecordAccessorInfo>("FeatDB", g_theFeatDB,
																		g_FeatRecord_Accessors,
																		g_Feat_Tokens,
																		k_Num_FeatRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<ResourceRecord, ResourceRecordAccessorInfo>("ResourceDB", g_theResourceDB,
																				g_ResourceRecord_Accessors,
																				g_Resource_Tokens,
																				k_Num_ResourceRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<OrderRecord, OrderRecordAccessorInfo>("OrderDB", g_theOrderDB,
																		  g_OrderRecord_Accessors,
																		  g_Order_Tokens,
																		  k_Num_OrderRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<TerrainImprovementRecord, 
									TerrainImprovementRecordAccessorInfo>("TerrainImprovementDB", 
																		  g_theTerrainImprovementDB,
																		  g_TerrainImprovementRecord_Accessors,
																		  g_TerrainImprovement_Tokens,
																		  k_Num_TerrainImprovementRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<GovernmentRecord, 
									GovernmentRecordAccessorInfo>("GovernmentDB", g_theGovernmentDB,
																  g_GovernmentRecord_Accessors,
																  g_Government_Tokens,
																  k_Num_GovernmentRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<StrategyRecord, 
									StrategyRecordAccessorInfo>("StrategyDB", g_theStrategyDB,
																  g_StrategyRecord_Accessors,
																  g_Strategy_Tokens,
																  k_Num_StrategyRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<DiplomacyRecord, 
									DiplomacyRecordAccessorInfo>("DiplomacyDB", g_theDiplomacyDB,
																  g_DiplomacyRecord_Accessors,
																  g_Diplomacy_Tokens,
																  k_Num_DiplomacyRecord_Tokens));
	//The rest of the new databases available through slic added by Martin Gühmann
	m_dbHash->Add(new SlicDBConduit<PersonalityRecord, 
									PersonalityRecordAccessorInfo>("PersonalityDB", g_thePersonalityDB,
																  g_PersonalityRecord_Accessors,
																  g_Personality_Tokens,
																  k_Num_PersonalityRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<AdvanceBranchRecord, 
									AdvanceBranchRecordAccessorInfo>("AdvanceBranchDB", g_theAdvanceBranchDB,
																  g_AdvanceBranchRecord_Accessors,
																  g_AdvanceBranch_Tokens,
																  k_Num_AdvanceBranchRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<AdvanceListRecord, 
									AdvanceListRecordAccessorInfo>("AdvanceListDB", g_theAdvanceListDB,
																  g_AdvanceListRecord_Accessors,
																  g_AdvanceList_Tokens,
																  k_Num_AdvanceListRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<AgeCityStyleRecord, 
									AgeCityStyleRecordAccessorInfo>("AgeCityStyleDB", g_theAgeCityStyleDB,
																  g_AgeCityStyleRecord_Accessors,
																  g_AgeCityStyle_Tokens,
																  k_Num_AgeCityStyleRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<AgeRecord, AgeRecordAccessorInfo>("AgeDB", g_theAgeDB,
																  g_AgeRecord_Accessors,
																  g_Age_Tokens,
																  k_Num_AgeRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<BuildingBuildListRecord, 
									BuildingBuildListRecordAccessorInfo>("BuildingBuildListDB", g_theBuildingBuildListDB,
																  g_BuildingBuildListRecord_Accessors,
																  g_BuildingBuildList_Tokens,
																  k_Num_BuildingBuildListRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<BuildListSequenceRecord, 
									BuildListSequenceRecordAccessorInfo>("BuildListSequenceDB", g_theBuildListSequenceDB,
																  g_BuildListSequenceRecord_Accessors,
																  g_BuildListSequence_Tokens,
																  k_Num_BuildListSequenceRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<CitySizeRecord, 
									CitySizeRecordAccessorInfo>("CitySizeDB", g_theCitySizeDB,
																  g_CitySizeRecord_Accessors,
																  g_CitySize_Tokens,
																  k_Num_CitySizeRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<CityStyleRecord, 
									CityStyleRecordAccessorInfo>("CityStyleDB", g_theCityStyleDB,
																  g_CityStyleRecord_Accessors,
																  g_CityStyle_Tokens,
																  k_Num_CityStyleRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<DiplomacyProposalRecord, 
									DiplomacyProposalRecordAccessorInfo>("DiplomacyProposalDB", g_theDiplomacyProposalDB,
																  g_DiplomacyProposalRecord_Accessors,
																  g_DiplomacyProposal_Tokens,
																  k_Num_DiplomacyProposalRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<DiplomacyThreatRecord, 
									DiplomacyThreatRecordAccessorInfo>("DiplomacyThreatDB", g_theDiplomacyThreatDB,
																  g_DiplomacyThreatRecord_Accessors,
																  g_DiplomacyThreat_Tokens,
																  k_Num_DiplomacyThreatRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<EndGameObjectRecord, 
									EndGameObjectRecordAccessorInfo>("EndGameObjectDB", g_theEndGameObjectDB,
																  g_EndGameObjectRecord_Accessors,
																  g_EndGameObject_Tokens,
																  k_Num_EndGameObjectRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<GoalRecord, GoalRecordAccessorInfo>("GoalDB", g_theGoalDB,
																  g_GoalRecord_Accessors,
																  g_Goal_Tokens,
																  k_Num_GoalRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<IconRecord, IconRecordAccessorInfo>("IconDB", g_theIconDB,
																  g_IconRecord_Accessors,
																  g_Icon_Tokens,
																  k_Num_IconRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<ImprovementListRecord, 
									ImprovementListRecordAccessorInfo>("ImprovementListDB", g_theImprovementListDB,
																  g_ImprovementListRecord_Accessors,
																  g_ImprovementList_Tokens,
																  k_Num_ImprovementListRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<PopRecord, PopRecordAccessorInfo>("PopDB", g_thePopDB,
																  g_PopRecord_Accessors,
																  g_Pop_Tokens,
																  k_Num_PopRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<SoundRecord, SoundRecordAccessorInfo>("SoundDB", g_theSoundDB,
																  g_SoundRecord_Accessors,
																  g_Sound_Tokens,
																  k_Num_SoundRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<SpecialAttackInfoRecord, 
									SpecialAttackInfoRecordAccessorInfo>("SpecialAttackInfoDB", g_theSpecialAttackInfoDB,
																  g_SpecialAttackInfoRecord_Accessors,
																  g_SpecialAttackInfo_Tokens,
																  k_Num_SpecialAttackInfoRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<SpecialEffectRecord, 
									SpecialEffectRecordAccessorInfo>("SpecialEffectDB", g_theSpecialEffectDB,
																  g_SpecialEffectRecord_Accessors,
																  g_SpecialEffect_Tokens,
																  k_Num_SpecialEffectRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<SpriteRecord, SpriteRecordAccessorInfo>("SpriteDB", g_theSpriteDB,
																  g_SpriteRecord_Accessors,
																  g_Sprite_Tokens,
																  k_Num_SpriteRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<UnitBuildListRecord, 
									UnitBuildListRecordAccessorInfo>("UnitBuildListDB", g_theUnitBuildListDB,
																  g_UnitBuildListRecord_Accessors,
																  g_UnitBuildList_Tokens,
																  k_Num_UnitBuildListRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<WonderBuildListRecord, 
									WonderBuildListRecordAccessorInfo>("WonderBuildListDB", g_theWonderBuildListDB,
																  g_WonderBuildListRecord_Accessors,
																  g_WonderBuildList_Tokens,
																  k_Num_WonderBuildListRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<WonderMovieRecord, 
									WonderMovieRecordAccessorInfo>("WonderMovieDB", g_theWonderMovieDB,
																  g_WonderMovieRecord_Accessors,
																  g_WonderMovie_Tokens,
																  k_Num_WonderMovieRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<CivilisationRecord, 
									CivilisationRecordAccessorInfo>("CivilisationDB", g_theCivilisationDB,
																  g_CivilisationRecord_Accessors,
																  g_Civilisation_Tokens,
																  k_Num_CivilisationRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<RiskRecord, 
									RiskRecordAccessorInfo>("RiskDB", g_theRiskDB,
																  g_RiskRecord_Accessors,
																  g_Risk_Tokens,
																  k_Num_RiskRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<DifficultyRecord, 
									DifficultyRecordAccessorInfo>("DifficultyDB", g_theDifficultyDB,
																  g_DifficultyRecord_Accessors,
																  g_Difficulty_Tokens,
																  k_Num_DifficultyRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<PollutionRecord, 
									PollutionRecordAccessorInfo>("PollutionDB", g_thePollutionDB,
																  g_PollutionRecord_Accessors,
																  g_Pollution_Tokens,
																  k_Num_PollutionRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<GlobalWarmingRecord, 
									GlobalWarmingRecordAccessorInfo>("GlobalWarmingDB", g_theGlobalWarmingDB,
																	  g_GlobalWarmingRecord_Accessors,
																	  g_GlobalWarming_Tokens,
																	  k_Num_GlobalWarmingRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<MapIconRecord, 
									MapIconRecordAccessorInfo>("MapIconDB", g_theMapIconDB,
																g_MapIconRecord_Accessors,
																g_MapIcon_Tokens,
																k_Num_MapIconRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<MapRecord, 
									MapRecordAccessorInfo>("MapDB", g_theMapDB,
															g_MapRecord_Accessors,
															g_Map_Tokens,
															k_Num_MapRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<ConceptRecord, 
									ConceptRecordAccessorInfo>("ConceptDB", g_theConceptDB,
															   g_ConceptRecord_Accessors,
															   g_Concept_Tokens,
															   k_Num_ConceptRecord_Tokens));
	m_dbHash->Add(new SlicDBConduit<ConstRecord, 
									ConstRecordAccessorInfo>("ConstDB", g_theConstDB,
															   g_ConstRecord_Accessors,
															   g_Const_Tokens,
															   k_Num_ConstRecord_Tokens));
}

SlicDBInterface *SlicEngine::GetDBConduit(const char *name)
{
	return m_dbHash ? m_dbHash->Access(name) : NULL;
}

#define SMF_2A(name, a1, a2) m_modFunc[name] = new SlicModFunc(#name, a1, a2, ST_END);
#define SMF_3A(name, a1, a2, a3) m_modFunc[name] = new SlicModFunc(#name, a1, a2, a3, ST_END);

void SlicEngine::AddModFuncs()
{
	for (size_t i = 0; i < mod_MAX; ++i) 
	{
		delete m_modFunc[i];
		m_modFunc[i] = NULL;
	}

	SMF_2A(mod_CanPlayerHaveAdvance, ST_PLAYER, ST_ADVANCE);
	SMF_2A(mod_CanCityBuildUnit, ST_CITY, ST_INT);
	SMF_2A(mod_CanCityBuildBuilding, ST_CITY, ST_INT);
	SMF_2A(mod_CanCityBuildWonder, ST_CITY, ST_INT);
	
	SMF_2A(mod_CityHappiness, ST_CITY, ST_INT);
	SMF_3A(mod_UnitAttack, ST_UNIT, ST_UNIT, ST_INT);
	SMF_3A(mod_UnitRangedAttack, ST_UNIT, ST_UNIT, ST_INT);
	SMF_3A(mod_UnitDefense, ST_UNIT, ST_UNIT, ST_INT);
}

sint32 SlicEngine::CallMod(MOD_FUNC modFunc, sint32 def, ...)
{
	Assert(modFunc > mod_INVALID);
	Assert(modFunc < mod_MAX);
	
	SlicModFunc *mf = m_modFunc[modFunc];
	if(!mf) return def;

	if(!mf->GetSegment())
		return def;

	va_list vl;
	va_start(vl, def);

	sint32 arg;
	SlicArgList *slicArgs = new SlicArgList;

	Unit u;
	Army a;
	sint32 val;
	MapPoint pos;

	for(arg = 0; arg < mf->GetNumArgs(); arg++) {
		SlicSymbolData *sym;
		switch(mf->GetArg(arg)) {			
			case ST_UNIT:
				u.m_id = va_arg(vl, uint32);
				sym = new SlicSymbolData(SLIC_SYM_UNIT);
				sym->SetUnit(u);
				slicArgs->AddArg(SA_TYPE_INT_VAR, sym);
				break;
			case ST_CITY:
				u.m_id = va_arg(vl, uint32);
				sym = new SlicSymbolData(SLIC_SYM_CITY);
				sym->SetCity(u);
				slicArgs->AddArg(SA_TYPE_INT_VAR, sym);
				break;
			case ST_PLAYER:
				val = va_arg(vl, sint32);
				sym = new SlicSymbolData(SLIC_SYM_PLAYER);
				sym->SetIntValue(val);
				slicArgs->AddArg(SA_TYPE_INT_VAR, sym);
				break;
			case ST_GOOD:
			case ST_GOLD:
			case ST_ADVANCE:
			case ST_INT:
				val = va_arg(vl, sint32);
				sym = new SlicSymbolData(SLIC_SYM_IVAR);
				sym->SetIntValue(val);
				slicArgs->AddArg(SA_TYPE_INT_VAR, sym);
				break;
			case ST_LOCATION:
				pos = va_arg(vl, MapPoint);
				sym = new SlicSymbolData(SLIC_SYM_LOCATION);
				sym->SetPos(pos);
				slicArgs->AddArg(SA_TYPE_INT_VAR, sym);
				break;
			case ST_ARMY:
				a.m_id = va_arg(vl, uint32);
				sym = new SlicSymbolData(SLIC_SYM_ARMY);
				sym->SetArmy(a);
				slicArgs->AddArg(SA_TYPE_INT_VAR, sym);
				break;
		}
	}

	va_end(vl);

	SlicObject *obj;
	mf->GetSegment()->Call(slicArgs, obj);

	slicArgs->ReleaseSymbols();
	delete slicArgs;

	sint32 result = obj->GetResult();
	obj->Release();

	return result;
}

sint32 SlicEngine::CallExcludeFunc(const MBCHAR *name, sint32 type, sint32 player)
{
	SlicSegment *seg = GetSegment(name);
	if(!seg) return false;
	
	SlicArgList *slicArgs =  new SlicArgList;
	SlicSymbolData *sym;
	sym = new SlicSymbolData(SLIC_SYM_IVAR);
	sym->SetIntValue(type);
	slicArgs->AddArg(SA_TYPE_INT_VAR, sym);

	sym = new SlicSymbolData(SLIC_SYM_PLAYER);
	sym->SetIntValue(player);
	slicArgs->AddArg(SA_TYPE_INT_VAR, sym);

	SlicObject *obj;
	seg->Call(slicArgs, obj);

    slicArgs->ReleaseSymbols();
	delete slicArgs;

	sint32 result = obj->GetResult();
	obj->Release();

	return result;
}

sint32 SlicEngine::GetCurrentLine() const
{
	if(m_context)
	{
		return m_context->GetFrame()->GetCurrentLine();
	}
	else
	{
		return -1;
	}
}

const char* SlicEngine::GetSegmentName() const
{
	if(m_context)
	{
		return m_context->GetFrame()->GetSlicSegment()->GetName();
	}
	else
	{
		return "";
	}
}

const char* SlicEngine::GetFileName() const
{
	if(m_context)
	{
		return m_context->GetFrame()->GetSlicSegment()->GetFilename();
	}
	else
	{
		return "";
	}
}
