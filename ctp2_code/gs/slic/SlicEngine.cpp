
#include "c3.h"
#include "c3errors.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "CivArchive.h"
#include "SlicSegment.h"
#include "slicif.h"
#include "StringHash.h"
#include "SlicFunc.h"
#include "SlicFuncAI.h"
#include "SlicSymTab.h"
#include "Player.h"
extern Player **g_player;
#include "Unit.h"
#include "CivPaths.h"
extern CivPaths *g_civPaths;
#include "SelItem.h"
extern SelectedItem *g_selected_item;
#include "TradeOffer.h"
#include "Agreement.h"
#include "MessagePool.h"
#include "BldQue.h"
#include "SlicRecord.h"
#include "UnitPool.h"
extern UnitPool *g_theUnitPool;

#include "TutorialWin.h"
extern TutorialWin *g_tutorialWin;


#include "PointerList.h"

#include "Pool.h"
#include "SimpleDynArr.h"


#include "GameFile.h"

#include "TiledMap.h"
#include "Radarmap.h"
#include "controlpanelwindow.h"
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

#include "SlicDBConduit.h"
#include "SlicModFunction.h"

#include "GameEventManager.h"

#include "Globals.h"

#include "ResourceRecord.h"
#include "CriticalMessagesPrefs.h"

#include "MessageWindow.h"
#include "ProfileDB.h"

extern RadarMap *g_radarMap;
extern ControlPanelWindow *g_controlPanel;

SlicEngine *g_slicEngine = NULL;

char g_slic_filename[_MAX_PATH];
char g_tutorial_filename[_MAX_PATH];
extern MessagePool *g_theMessagePool;

#define k_SLIC_DEFAULT_TIMER_GRANULARITY 5

#define k_DB_HASH_SIZE 32

SlicEngine::SlicEngine()
{
#ifdef _DEBUG
	
#endif

	m_segmentHash = NULL;
	m_functionHash = NULL;
	m_symTab = NULL;
	m_context = NULL;
	m_uiHash = NULL;
	m_dbHash = NULL;
	m_doResearchOnUnblank = FALSE;
	m_atBreak = false;
	m_breakRequested = false;

	m_tutorialActive = FALSE;
	m_tutorialPlayer = 1;
	m_addedBuiltinFunctions = FALSE;
	m_addedBuiltinVariables = FALSE;

	for(sint32 i = 0; i < TRIGGER_LIST_MAX; i++) {
		m_triggerLists[i] = new PointerList<SlicSegment>;
	}

	m_currentMessage = new Message(0);

	m_objectPond = new Pool<SlicObject>(INITIAL_CHUNK_LIST_SIZE);
	m_segmentPond = new Pool<SlicSegment>(100);
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_records[i] = NULL;
	}

	for(i = 0; i < k_NUM_TIMERS; i++) {
		m_timer[i] = -1;
	}

	for(i = 0; i < k_MAX_TRIGGER_KEYS; i++) {
		m_triggerKey[i] = 0;
	}
	m_currentKeyTrigger = 0;

	m_disabledClasses = new SimpleDynamicArray<sint32>;
	m_loadGameName = NULL;
	m_uiExecuteObjects = new PointerList<SlicObject>;

	m_timerGranularity = k_SLIC_DEFAULT_TIMER_GRANULARITY;

	m_blankScreen = FALSE;
	m_constHash = new StringHash<SlicConst>(10);

	m_builtins = new SlicSymbolData *[SLIC_BUILTIN_MAX];
	memset(m_builtins, 0, SLIC_BUILTIN_MAX * sizeof(SlicSymbolData *));

	m_builtin_desc = new SlicStructDescription *[SLIC_BUILTIN_MAX];
	memset(m_builtin_desc, 0, SLIC_BUILTIN_MAX * sizeof(SlicStructDescription *));

	m_contextStack = new PointerList<SlicObject>;
}

SlicEngine::SlicEngine(CivArchive &archive)
{
	m_loadGameName = NULL;
    m_addedBuiltinFunctions = FALSE; 
    m_functionHash = NULL; 
	m_context = NULL;

	m_atBreak = false;
	m_breakRequested = false;

#ifdef _DEBUG
	c3debug_SetDebugMask(k_DBG_SLIC, TRUE);
#endif
	
	g_slicEngine = this;

	m_currentMessage = new Message(0);
	m_objectPond = new Pool<SlicObject>(INITIAL_CHUNK_LIST_SIZE);
	m_segmentPond = new Pool<SlicSegment>(100);

	m_addedBuiltinFunctions = FALSE;
	m_addedBuiltinVariables = FALSE;

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_records[i] = NULL;
	}

	m_segmentHash = new SlicSegmentHash(k_SEGMENT_HASH_SIZE);
	m_uiHash = new StringHash<SlicUITrigger> (k_SEGMENT_HASH_SIZE);
	m_dbHash = NULL;

	m_builtins = new SlicSymbolData *[SLIC_BUILTIN_MAX];
	m_constHash = new StringHash<SlicConst>(10);
	memset(m_builtins, 0, SLIC_BUILTIN_MAX * sizeof(SlicSymbolData *));

	m_builtin_desc = new SlicStructDescription *[SLIC_BUILTIN_MAX];
	memset(m_builtin_desc, 0, SLIC_BUILTIN_MAX * sizeof(SlicStructDescription *));

	
	AddStructs(false);
	AddBuiltinFunctions();
	AddDatabases();

	m_disabledClasses = new SimpleDynamicArray<sint32>;

	Serialize(archive);
	for(i = 0; i < TRIGGER_LIST_MAX; i++) {
		m_triggerLists[i] = new PointerList<SlicSegment>;
	}

	m_uiExecuteObjects = new PointerList<SlicObject>;

	m_blankScreen = FALSE;
	m_contextStack = new PointerList<SlicObject>;
}

SlicEngine::~SlicEngine()
{
	Cleanup();

	if(m_objectPond)
		delete m_objectPond;

	if(m_segmentPond)
		delete m_segmentPond;

}

void SlicEngine::Cleanup()
{
	

	if(g_theMessagePool) {
		g_theMessagePool->NotifySlicReload();
	}

	if(m_segmentHash)
		delete m_segmentHash;

	if(m_functionHash)
		delete m_functionHash;
	
	if(m_symTab)
		delete m_symTab;

	if(m_uiHash) {
		m_uiHash->Clear();
		delete m_uiHash;
	}

	if(m_dbHash) {
		delete m_dbHash;
	}

	sint32 i;
	for(i = 0; i < TRIGGER_LIST_MAX; i++) {
		if(m_triggerLists[i]) {
			delete m_triggerLists[i];
		}
	}

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(m_records[i]) {
			SlicRecord *sr;
			while((sr = m_records[i]->RemoveHead()) != NULL) {
				delete sr;
			}
			delete m_records[i];
			m_records[i] = NULL;
		}
	}

	if(m_currentMessage)
		delete m_currentMessage;

	if(m_disabledClasses)
		delete m_disabledClasses;

	if(m_uiExecuteObjects)
		delete m_uiExecuteObjects;

	if(m_constHash)
		delete m_constHash;

	if(m_builtins) {
		
		
		
		
		delete [] m_builtins;
	}

	if(m_builtin_desc) {
		for(i = 0; i < SLIC_BUILTIN_MAX; i++) {
			if(m_builtin_desc[i])
				delete m_builtin_desc[i];
		}
		delete [] m_builtin_desc;
	}

	if(m_contextStack) {
		delete m_contextStack;
	}

	for(i = 0; i < mod_MAX; i++) {
		if(m_modFunc[i])
			delete m_modFunc[i];
	}

	
	
	slicif_init();

	m_doResearchOnUnblank = FALSE;
}

void SlicEngine::Reset()
{
	Assert(FALSE);
#if 0
	Cleanup();

	m_segmentHash = NULL;
	m_functionHash = NULL;
	m_symTab = NULL;
	m_context = NULL;
	m_uiHash = NULL;
	m_dbHash = NULL;

	m_tutorialActive = FALSE;
	m_tutorialPlayer = 1;
	m_addedBuiltinFunctions = FALSE;
	m_addedBuiltinVariables = FALSE;

	for(sint32 i = 0; i < TRIGGER_LIST_MAX; i++) {
		m_triggerLists[i] = new PointerList<SlicSegment>;
	}

	m_currentMessage = new Message(0);

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_records[i] = NULL;
	}

	for(i = 0; i < k_NUM_TIMERS; i++) {
		m_timer[i] = -1;
	}

	for(i = 0; i < k_MAX_TRIGGER_KEYS; i++) {
		m_triggerKey[i] = 0;
	}
	m_currentKeyTrigger = 0;

	m_disabledClasses = new SimpleDynamicArray<sint32>;
	m_loadGameName = NULL;
	m_uiExecuteObjects = new PointerList<SlicObject>;
	m_constHash = new StringHash<SlicConst>(10);
	m_contextStack = new PointerList<SlicObject>;

	m_timerGranularity = k_SLIC_DEFAULT_TIMER_GRANULARITY;

	m_doResearchOnUnblank = FALSE;
#endif
}

void SlicEngine::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE
	sint32 i, j, p, num, numRecords;
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

		
		archive.PutUINT8(m_doResearchOnUnblank);
		archive << m_researchOwner;
		archive.Store((uint8*)m_researchText, sizeof(m_researchText));

	} else {
		archive >> m_tutorialPlayer;
		sint32 tmp;
		archive >> tmp;
		m_tutorialActive = tmp;
		m_symTab = new SlicSymTab(archive);
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

		m_currentKeyTrigger = 0;

        AddBuiltinFunctions();
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

SlicObject *SlicEngine::GetNewObject()
{
	int index;
	SlicObject *obj = m_objectPond->Get_Next_Pointer(index);
	obj->SetIndex(index);
	return obj;
}

void SlicEngine::ReleaseObject(SlicObject *object)
{
	m_objectPond->Release_Pointer(object->GetIndex());
}

SlicSegment *SlicEngine::GetNewSegment()
{
	int index;
	SlicSegment *seg = m_segmentPond->Get_Next_Pointer(index);
	seg->SetPoolIndex(index);
	return seg;
}

void SlicEngine::ReleaseSegment(SlicSegment *seg)
{
	m_segmentPond->Release_Pointer(seg->GetPoolIndex());
}

SlicSegment *SlicEngine::GetSegment(const char *id)
{
	return m_segmentHash->Access(id);
}

SlicFunc *SlicEngine::GetFunction(const char *name)
{
	return m_functionHash->Access(name);
}

SlicNamedSymbol *SlicEngine::GetSymbol(sint32 index)
{
	return m_symTab->Access(index);
}

SlicNamedSymbol *SlicEngine::GetSymbol(const char *name)
{
	if(!m_symTab)
		return NULL;

	return m_symTab->StringHash<SlicNamedSymbol>::Access(name);
}

SlicNamedSymbol *SlicEngine::GetOrMakeSymbol(const char *name)
{
	if(!m_symTab)
		return NULL;

	SlicNamedSymbol *sym = m_symTab->StringHash<SlicNamedSymbol>::Access(name);
	if(!sym) {
		sym = new SlicNamedSymbol(name);
		m_symTab->Add(sym);
	}
	return sym;
}

SlicParameterSymbol *SlicEngine::GetParameterSymbol(const char *name, sint32 parameterIndex)
{
	if(!m_symTab)
		return NULL;

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
	Assert(!m_atBreak);
	if(m_atBreak)
		return;

	if(m_context)
		PushContext();

	
	SetContext(obj);

	obj->AddRef();
	Assert(obj->IsValid());
	if(obj->IsValid()) {
		if((obj->GetSegment()->GetFilenum() != k_TUTORIAL_FILE ||
			(g_theProfileDB->IsTutorialAdvice())) &&
			(!g_theCriticalMessagesPrefs || g_theCriticalMessagesPrefs->IsEnabled(obj->GetSegment()->GetName()) )) {
			
			
			
			

			
			
			






			obj->Execute();
		}
	}

	if(m_atBreak)
		return;

	PopContext();
	
	obj->Release();
}

void SlicEngine::AddBuiltinFunctions()
{
	if(m_addedBuiltinFunctions)
		return;
	m_addedBuiltinFunctions = TRUE;

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
}

void SlicEngine::AddBuiltinVariables()
{
	if(m_addedBuiltinVariables)
		return;

	m_addedBuiltinVariables = TRUE;
}
void SlicEngine::Link()
{
	if(!m_segmentHash)
		m_segmentHash = new SlicSegmentHash(k_SEGMENT_HASH_SIZE);

	sint32 symStart;
	if(!m_symTab) {
		symStart = 0;+
		m_symTab = new SlicSymTab(0);
	} else {
		symStart = m_symTab->GetNumEntries();
	}

	if(!m_uiHash)
		m_uiHash = new StringHash<SlicUITrigger>(k_SEGMENT_HASH_SIZE);

	m_segmentHash->SetSize(g_slicNumEntries);

	for(sint32 i = 0; i < g_slicNumEntries; i++) {
		SlicSegment *seg = new SlicSegment(i);
		m_segmentHash->Add(seg->GetName(), seg);
	}

	m_segmentHash->LinkTriggerSymbols(m_uiHash);

	
	slicif_init();

	AddModFuncs();
}

extern "C" char slic_parser_error_text[1024];

BOOL SlicEngine::Load(MBCHAR *filename, sint32 filenum)
{
	slicconst_Initialize();

	sint32 symStart;
	if(m_symTab) {
		symStart = m_symTab->GetNumEntries();
	} else {
		m_symTab = new SlicSymTab(0);
		AddStructs(true);
		AddBuiltinVariables();
		AddDatabases();
		symStart = m_symTab->GetNumEntries();
	}

	AddBuiltinFunctions();

	slicif_init();
	slicif_set_file_num(filenum);

	if(slicif_run_parser(filename, symStart) != SLIC_ERROR_OK) {
		c3errors_ErrorDialog(filename, "%s", slic_parser_error_text);
		return FALSE;
	}

	return TRUE;
}

void SlicEngine::AddTrigger(SlicSegment *trigger, TRIGGER_LIST which)
{
	for(sint32 t = sint32(which); t < sint32(TRIGGER_LIST_MAX); t++) {
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
	*m_currentMessage = Message(0);
}

void SlicEngine::AddCurrentMessage()
{
	
	
	if(!g_theMessagePool->IsValid(*m_currentMessage))
		return;

	m_currentMessage->Minimize();
	*m_currentMessage = Message(0);
}

PointerList<SlicRecord> *SlicEngine::GetRecords(sint32 player)
{
	return m_records[player];
}

void SlicEngine::AddTutorialRecord(sint32 player, MBCHAR *title, MBCHAR *text,
								   SlicSegment *segment)
{
	if(!m_records[player]) {
		m_records[player] = new PointerList<SlicRecord>;
	}

	
	PointerList<SlicRecord>::Walker walk(m_records[player]);
	while(walk.IsValid()) {
		if(walk.GetObj()->GetSegment() == segment) {
			return;
		}
		walk.Next();
	}

	m_records[player]->AddTail(new SlicRecord(player, title, text, segment));

	
	if ( g_tutorialWin && title) {
		g_tutorialWin->AddToList( title, m_records[player]->GetCount() - 1 );
	}
}

BOOL SlicEngine::IsTimerExpired(sint32 timer)
{
	Assert(timer >= 0);
	Assert(timer < k_NUM_TIMERS);

	if(timer < 0 || timer >= k_NUM_TIMERS)
		return FALSE;

	if(m_timer[timer] < 0)
		return FALSE;

	if(time(0) >= m_timer[timer]) {
		return TRUE;
	}
	return FALSE;
}
		
void SlicEngine::StartTimer(sint32 timer, sint32 duration)
{
	Assert(timer >= 0);
	Assert(timer < k_NUM_TIMERS);

	if(timer < 0 || timer >= k_NUM_TIMERS)
		return;

	m_timer[timer] = time(0) + duration;
}

void SlicEngine::StopTimer(sint32 timer)
{
	Assert(timer >= 0);
	Assert(timer < k_NUM_TIMERS);

	if(timer < 0 || timer >= k_NUM_TIMERS)
		return;

	m_timer[timer] = -1;
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
	sint32 i;
	for(i = 0; i < m_disabledClasses->Num(); i++) {
		if(m_disabledClasses->Access(i) == mclass) {
			m_disabledClasses->DelIndex(i);
			i--;
		}
	}
}

void SlicEngine::DisableMessageClass(sint32 mclass)
{
	m_disabledClasses->Insert(mclass);
}

BOOL SlicEngine::IsMessageClassDisabled(sint32 mclass)
{
	sint32 i;
	for(i = 0; i < m_disabledClasses->Num(); i++) {
		if(m_disabledClasses->Access(i) == mclass) {
			return TRUE;
		}
	}
	return FALSE;
}

void SlicEngine::ProcessUITriggers()
{
	SlicObject *obj;
	while((obj = m_uiExecuteObjects->RemoveHead()) != NULL) {
		Execute(obj);
	}
}

void SlicEngine::RunYearlyTriggers()
{
	PointerList<SlicSegment>::Walker walk(m_triggerLists[TRIGGER_LIST_YEARLY]);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsEnabled()) {
			SlicObject *obj = new SlicObject(walk.GetObj());
			Execute(obj);
		}
		walk.Next();
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
			if(!g_theUnitPool->IsValid(u))
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
			if(!g_theUnitPool->IsValid(u)) {
				return;
			}
			if(!g_theUnitPool->IsValid(city)) {
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
	if(!g_theUnitPool->IsValid(unit1) ||
	   !g_theUnitPool->IsValid(unit2) ||
	   !g_player[unit1.GetOwner()] ||
	   !g_player[unit2.GetOwner()]) {
		return;
	}

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

BOOL SlicEngine::SpecialSameGoodEnabled() const
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
			if(!g_theUnitPool->IsValid(unit))
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
	if(!this)
		return;

	DPRINTF(k_DBG_UI, ("SLIC: control %s used\n", controlName));
	if(!controlName)
		return;
	SlicUITrigger *trig = m_uiHash->Access(controlName);
	if(trig) {
		SlicSegment *seg = trig->GetSegment();
		if(seg && seg->IsEnabled()) {
			SlicObject *obj = new SlicObject(seg);
			m_uiExecuteObjects->AddTail(obj);
			
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
	
	
	






















	sint32 i;
	for(i = 0; i < k_NUM_TIMERS; i++)
		if(IsTimerExpired(i))
			{
			m_timer[i]=-1;
            g_gevManager->AddEvent(GEV_INSERT_Tail,GEV_TimerExpired,GEA_Int,i, GEA_End);
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
						if(g_theUnitPool->IsValid(u)) {
							obj->AddUnit(u);
						} else {
							abort = TRUE;
						}
						break;
					case ST_CITY:
						u = va_arg(vl, Unit);
						if(g_theUnitPool->IsValid(u)) {
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
	return;
}

BOOL SlicEngine::IsKeyPressed(MBCHAR key)
{
	if(m_currentKeyTrigger && m_currentKeyTrigger == m_triggerKey[key])
		return TRUE;
	else
		return FALSE;
}

BOOL SlicEngine::RunKeyboardTrigger(MBCHAR key)
{
	sint32 i;
	for(i = 0; i < k_MAX_TRIGGER_KEYS; i++) {
		if(m_triggerKey[i] == key) {
			m_currentKeyTrigger = key;
			RunTrigger(TRIGGER_LIST_KEY_PRESSED, ST_END);
			m_currentKeyTrigger = 0;
			return TRUE;
		}
	}
	return FALSE;
}

void SlicEngine::BlankScreen(BOOL blank)
{
	if(m_blankScreen != blank) {
		m_blankScreen = blank;
		g_tiledMap->InvalidateMap();
		g_tiledMap->Refresh();
		g_radarMap->Update();

		if(!m_blankScreen)
			CheckPendingResearch();

		if(g_controlPanel) {
			if(m_blankScreen) {

			} else {

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

BOOL SlicEngine::FindConst(const MBCHAR *name, sint32 *value)
{
	SlicConst *sc = m_constHash->Access(name);
	if(sc) {
		*value = sc->GetValue();
		return TRUE;
	}
	return FALSE;
}

void SlicEngine::AddStructArray(bool createSymbols, SlicStructDescription *desc, SLIC_BUILTIN which)
{
	m_builtin_desc[which] = desc;
	if(createSymbols) {
		m_builtins[which] = new SlicBuiltinNamedSymbol(which, desc->GetName(), new SlicArray(desc));
		m_symTab->Add((SlicNamedSymbol *)m_builtins[which]);
	}
}

void SlicEngine::AddStruct(bool createSymbols, SlicStructDescription *desc, SLIC_BUILTIN which)
{
	m_builtin_desc[which] = desc;
	if(createSymbols) {
		m_builtins[which] = new SlicBuiltinNamedSymbol(which, desc->GetName(), desc);
		m_symTab->Add((SlicNamedSymbol *)m_builtins[which]);
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
	m_builtins[sym->GetBuiltin()] = sym;
}

SlicSymbolData *SlicEngine::GetBuiltinSymbol(SLIC_BUILTIN which)
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
	m_context = obj;
	if(obj)
		obj->FillBuiltins();
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

	m_breakSegment = segment;
	
	m_breakContext = context;
	m_breakContext->CopyFromBuiltins();
	m_breakContext->AddRef();
	
	
	m_atBreak = true;

	sourcelist_RegisterBreak(segment, offset);
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

	SlicObject *oldContext = NULL;
	if(!m_atBreak) {
		while((oldContext = m_contextStack->RemoveTail()) != NULL) {
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

void SlicEngine::PushContext()
{
	if(m_context) {
		m_context->AddRef();
		m_contextStack->AddTail(m_context);
		m_context = NULL;
	}
}

void SlicEngine::PopContext()
{
	SlicObject *obj = m_contextStack->RemoveTail();
	if(obj)
		obj->Release();
	
	SetContext(obj);
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
}

SlicDBInterface *SlicEngine::GetDBConduit(const char *name)
{
	return m_dbHash->Access(name);
}

#define SMF_2A(name, a1, a2) m_modFunc[name] = new SlicModFunc(#name, a1, a2, ST_END);
#define SMF_3A(name, a1, a2, a3) m_modFunc[name] = new SlicModFunc(#name, a1, a2, a3, ST_END);

void SlicEngine::AddModFuncs()
{
	sint32 i;
	for(i = 0; i < mod_MAX; i++) {
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

	for(arg = 0; arg < slicArgs->m_numArgs; arg++) {
		delete slicArgs->m_argValue[arg].m_symbol;
	}
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

	sint32 arg;
	for(arg = 0; arg < slicArgs->m_numArgs; arg++) {
		delete slicArgs->m_argValue[arg].m_symbol;
	}
	delete slicArgs;

	sint32 result = obj->GetResult();
	obj->Release();

	return result;
}
