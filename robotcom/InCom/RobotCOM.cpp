











#define INITGUID
#include "c3.h"
#include "IRobot.h"
#include "IC3GameState.h" 
#include "IC3Player.h"
#include "IC3World.h"
#include "RobotCOM.h"
#include "IC3GameState.h"
#include "AiMain.h"
#include "CivArchive.h"
#include "IC3DipReq.h"
#include "IC3Trade.h"
#include "common.h"
#include "linked_list.h"
#include "semi_dynamic_array.h"
#include "Sorted_array.h"
#include "AIP.h"
#include "Planner.h"
#include "IC3EndGameDB.h"


#include "DebugMemory.h"
#include "DebugCallstack.h"
#include "Log.h"

#include "fliif.h"







extern LONG _cdecl c3ai_CivExceptionHandler (LPEXCEPTION_POINTERS exception_pointers);

#ifdef _DEBUG
	#define C3TRY		__try
	#define C3EXCEPT	__except (c3ai_CivExceptionHandler(GetExceptionInformation())) {}
#else
	#define C3TRY		;
	#define C3EXCEPT	;
#endif

#include "Memory_Manager.h"


#include "geom2d.h"
Memory_Manager* g_memory_Point2d; 

#include "CityEdge.h"
Memory_Manager* g_memory_CityEdge; 

#include "CityTree.h"
Memory_Manager* g_memory_CityTree;

#include "CityVertex.h"
Memory_Manager* g_memory_CityVertex; 

#include "Delaunay.h"
Memory_Manager* g_memory_QuadEdge; 

#include "AiCell.h"
Memory_Manager* g_memory_AiCellPtr; 

STDAPI CoCreateInstanceRobotCom(IUnknown *&obj)
{
	
    if (NULL == g_memory_CityEdge) { 
        g_memory_CityEdge = new Memory_Manager(sizeof(CityEdge)); 
    } else { 
        g_memory_CityEdge->AddRef();
    }

    if (NULL == g_memory_CityTree) { 
        g_memory_CityTree = new Memory_Manager (sizeof(CityTree)); 
    } else { 
        g_memory_CityTree->AddRef(); 
    }

    if (NULL == g_memory_CityVertex) { 
        g_memory_CityVertex = new Memory_Manager (sizeof(CityVertex)); 
    } else { 
        g_memory_CityVertex->AddRef(); 
    }

    if (NULL == g_memory_QuadEdge) { 
        g_memory_QuadEdge = new Memory_Manager (sizeof(QuadEdge));
    } else {
        g_memory_QuadEdge->AddRef(); 
    }

    if (NULL == g_memory_AiCellPtr) { 
        g_memory_AiCellPtr = new Memory_Manager (sizeof(AiCellPtr)); 
    } else { 
        g_memory_AiCellPtr->AddRef();
    }

    if (NULL == g_memory_Point2d) { 
        g_memory_Point2d = new Memory_Manager (sizeof(Point2d)); 
    } else { 
        g_memory_Point2d->AddRef(); 
    }

    obj = new IC3RobotCom();
	obj->AddRef();

	return S_OK;
}






extern void c3aidebug_SetCiv3LogName(MBCHAR *civ3logName);
extern void c3aidebug_SetCiv3LogNumber(sint32 *civ3logNumber);
extern void c3aidebug_SetCiv3LogLinesThisFile(sint32 *civ3logLinesThisFile);

STDAPI InitDLLDebugging(void *memPtr, void *logPtr, void *funcPtr, 
						MBCHAR *civ3logName, sint32 *civ3logNumber, sint32 *civ3logLinesThisFile)
{
#ifdef _DEBUG
	DebugMemory_SetDebugMemoryPtr(memPtr);

	Log_SetLoggingPtr(logPtr);
	
	Debug_SetFAFirst(funcPtr);

	
	Hash_Init();
	Log_AddLoggingClasses();

	c3aidebug_SetCiv3LogName(civ3logName);
	c3aidebug_SetCiv3LogNumber(civ3logNumber);
	c3aidebug_SetCiv3LogLinesThisFile(civ3logLinesThisFile);
#endif

	return S_OK;
}


STDMETHODIMP IC3RobotCom::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IRobot)) {
		*obj = (IRobot*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IC3RobotCom::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) IC3RobotCom::Release()
{
    --m_refCount;
    if(m_refCount) { 
		return m_refCount;
    }
	delete this;
	return 0;
}




IC3RobotCom::IC3RobotCom()
{
    m_ai_main = new AiMain(); 
    return ;
}

IC3RobotCom::~IC3RobotCom()
{
    return ;
}


STDMETHODIMP IC3RobotCom::Init(IC3GameState *gs, uint32 goal_time_slice,
    uint32 max_time_slice, uint32 max_total_time)
{

	
	C3TRY {
		if (m_ai_main->Init(gs, goal_time_slice, max_time_slice, max_total_time)) {           
     		return TRUE; 
		} else {

            Assert(0); 
			return FALSE; 
		}
	}

	C3EXCEPT
}

STDMETHODIMP IC3RobotCom::Cleanup()
{
	
	C3TRY {
		delete m_ai_main; 
        m_ai_main = NULL; 
        if (g_memory_CityEdge->Release())
            g_memory_CityEdge=NULL; 
        if (g_memory_CityTree->Release()) 
            g_memory_CityTree=NULL;  
        if (g_memory_CityVertex->Release()) 
            g_memory_CityVertex=NULL; 
        if (g_memory_QuadEdge->Release()) 
            g_memory_QuadEdge=NULL;  
        if (g_memory_AiCellPtr->Release()) 
            g_memory_AiCellPtr=NULL; 
        if (g_memory_Point2d->Release()) 
            g_memory_Point2d=NULL;  

		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}

void IC3RobotCom::Save (IC3CivArchive *archive) 
{ 
	
	C3TRY {
		m_ai_main->Save(*((CivArchive *)archive));
	}

	C3EXCEPT
}

BOOL IC3RobotCom::Load (IC3GameState *gs, IC3CivArchive *archive)
{
	
	

	return m_ai_main->Load(gs, *((CivArchive*)archive)); 
}


STDMETHODIMP IC3RobotCom::BeginTurn(uint32 end_frame_by,  BOOL &my_turn_is_over)
{
	
	C3TRY {
		if (m_ai_main->BeginTurn(end_frame_by,  my_turn_is_over)) {
    		return S_OK; 
		} else {
			return E_FAIL; 
		}
	} 

	C3EXCEPT

	return E_FAIL;
}



STDMETHODIMP IC3RobotCom::ExecuteTimeSlice(uint32 end_frame_by,  BOOL &my_turn_is_over, 
    char debug_str[80] )
{
	
	C3TRY {
		if (m_ai_main->ExecuteTimeSlice(end_frame_by,  my_turn_is_over, debug_str)) {
    		return S_OK; 
		} else {
			return E_FAIL; 
		}
	}

	C3EXCEPT

	return E_FAIL;
}

STDMETHODIMP IC3RobotCom::RegisterAddArmy (uint32 id, 
   CAUSE_NEW_ARMY cause, uint32 hc_id, sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE], 
   sint32 unit_hp[k_MAX_ARMY_SIZE])
{
	
	C3TRY {
		m_ai_main->RegisterAddArmy (id, cause, hc_id, nUnits, unit_type, unit_hp);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterMyAddCity (uint32 id, 
    CAUSE_NEW_CITY cause, MapPointData *pos)
{
	
	C3TRY {
		m_ai_main->RegisterMyAddCity (id, cause, pos);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterHisAddCity (uint32 id, 
    CAUSE_NEW_CITY cause, MapPointData *pos)
{
	
	C3TRY {
		m_ai_main->RegisterHisAddCity (id, cause, pos);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterAdjustDeathDifferential (double adjustment, MapPointData &pos) 
{
	
	C3TRY {
		m_ai_main->RegisterAdjustDeathDifferential (adjustment, pos);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::Load_AIP (char *aip_file_name) 
{
	
	C3TRY {
		m_ai_main->Load_AIP (aip_file_name);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::Set_AILog_Verbosity (sint32 log_level) 
{
	
	C3TRY {
		m_ai_main->Set_AILog_Verbosity (log_level);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterMyRemoveArmy (uint32 id, CAUSE_REMOVE_ARMY cause)
{
	
	C3TRY {
		m_ai_main->RegisterMyRemoveArmy (id, cause);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterHisRemoveArmy (PLAYER_INDEX owner, uint32 id)
{
	
	C3TRY {
		m_ai_main->RegisterHisRemoveArmy (owner, id);
	    return S_OK;
	}

	C3EXCEPT
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterMyRemoveCity (uint32 id, 
    CAUSE_REMOVE_CITY cause)
{
	
	C3TRY {
		m_ai_main->RegisterMyRemoveCity (id, cause);
  		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterHisRemoveCity(PLAYER_INDEX owner, uint32 id)
{
	
	C3TRY {
		m_ai_main->RegisterHisRemoveCity(owner, id);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterCityDiedAt(MapPointData *pos)
{
	

	C3TRY {
		m_ai_main->RegisterCityDiedAt(pos);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterClearCargo(uint32 army_id)
{
	C3TRY {
		m_ai_main->RegisterClearCargo(army_id);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterInsertCargo (uint32 army_id, sint32 unit_type, sint32 hp)
{
	C3TRY {
		m_ai_main->RegisterInsertCargo(army_id, unit_type, hp);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterUnloadCargo (uint32 army_id, sint32 unit_type, sint32 hp, 
    CAUSE_REMOVE_ARMY cause)
{
	C3TRY {
		m_ai_main->RegisterUnloadCargo(army_id, unit_type, hp, cause);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterDeadUnit (uint32 army_id, 
     sint32 unit_type)
{
	C3TRY {
		m_ai_main->RegisterDeadUnit(army_id, unit_type);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterArmyWasGrouped (uint32 target, sint32 nUnits, 
        sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE])

{
	
	C3TRY {
		m_ai_main->RegisterArmyWasGrouped(target, nUnits, unit_type, unit_hp);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterYourArmyWasMoved(uint32 army_id, MapPointData *new_pos) 
{
	
	C3TRY {
		m_ai_main->RegisterYourArmyWasMoved(army_id, new_pos);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved, 
        MapPointData *new_pos)
{
    C3TRY {
		m_ai_main->RegisterHisArmyTeleported(his_owner, i_moved, new_pos);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;

}


STDMETHODIMP IC3RobotCom::RegisterCreateBuilding (uint32 city_id, 
        sint32 building_type)
{
	
	C3TRY {
		m_ai_main->RegisterCreateBuilding (city_id, building_type); 
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
} 

STDMETHODIMP IC3RobotCom::RegisterLostBuilding (uint32 city_id, 
        sint32 building_type)
{
	
	C3TRY {
		m_ai_main->RegisterLostBuilding (city_id, building_type); 
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
} 

STDMETHODIMP IC3RobotCom::RegisterNewGovernment (uint32 city_id, 
        sint32 government_type)
{
	
	C3TRY {
		m_ai_main->RegisterNewGovernment (city_id, government_type); 
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}
 

STDMETHODIMP IC3RobotCom::RegisterNewWGF (sint32 w, sint32 g, sint32 f)
{
    C3TRY { 
        m_ai_main->RegisterNewWGF(w, g, f);
        return S_OK; 
    } 

  	C3EXCEPT

	return S_OK;

}

STDMETHODIMP IC3RobotCom::RegisterNewCapitolBuilding (uint32 city_id)
{
	
	C3TRY {
		m_ai_main->RegisterNewCapitolBuilding (city_id); 
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
} 


STDMETHODIMP IC3RobotCom::RegisterCreateWonder (uint32 city_id, 
        sint32 wonder_type, PLAYER_INDEX owner)
{
	
	C3TRY {
		m_ai_main->RegisterCreateWonder (city_id, wonder_type, owner); 
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
} 

STDMETHODIMP IC3RobotCom::RegisterLearnedScience(sint32 idx_sci, CAUSE_SCI cause)
{
	
	C3TRY {
		m_ai_main->RegisterLearnedScience(idx_sci, cause); 
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterNewPlayer (PLAYER_INDEX player_id, 
    CAUSE_NEW_PLAYER cause, PLAYER_INDEX old_owner)
{
	
	C3TRY {
		m_ai_main->RegisterNewPlayer(player_id, cause, old_owner);
		return S_OK; 
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterDeadPlayer (PLAYER_INDEX player_id)
{
	
	C3TRY {
		m_ai_main->RegisterDeadPlayer(player_id);
		return S_OK;
	}

	C3EXCEPT

	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterAddInstallation(MapPointData &pos, sint32 inst_type)
{
    C3TRY { 
        m_ai_main->RegisterAddInstallation(pos, inst_type); 
        return S_OK; 
    } 

  	C3EXCEPT

	return S_OK;

}

STDMETHODIMP IC3RobotCom::RegisterRemoveInstallation(MapPointData &pos, sint32 inst_type)
{
    C3TRY { 
        m_ai_main->RegisterRemoveInstallation(pos, inst_type); 
        return S_OK; 
    } 

  	C3EXCEPT
	return S_OK;
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterCityAttack(uint32 my_city_id, PLAYER_INDEX his_owner, uint32 his_unit_id, UNIT_ORDER_TYPE attack_type)
{
	
	C3TRY {
		m_ai_main->RegisterCityAttack(my_city_id, his_owner, his_unit_id, attack_type);
	}

	C3EXCEPT
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterHostileAction(UNIT_ORDER_TYPE attack_type, 
    uint32 my_army_id, PLAYER_INDEX his_owner, uint32 his_unit_id, MapPointData *target_pos)
{
	C3TRY {
		m_ai_main->RegisterHostileAction(attack_type, my_army_id, his_owner, his_unit_id, 
            target_pos);
	}

	C3EXCEPT
}


STDMETHODIMP_ (void) IC3RobotCom::RegisterPollution(MapPointData *target_pos)
{
	C3TRY {
		m_ai_main->RegisterPollution(target_pos);
	}

	C3EXCEPT
}


STDMETHODIMP_ (void) IC3RobotCom::RegisterPiracy(PLAYER_INDEX he_did_it,                                              
    PLAYER_INDEX source_owner, uint32 src_id, PLAYER_INDEX dest_owner, 
    uint32 dest_id, sint32 resource)

{
	C3TRY {
		m_ai_main->RegisterPiracy(he_did_it, source_owner, src_id, dest_owner, dest_id, 
            resource);
	}

	C3EXCEPT
}
























sint32 IC3RobotCom::GetNumFuzzySections()
{
	
	C3TRY {
	    return m_ai_main->GetNumFuzzySections(); 
	}

	C3EXCEPT

	return 0;
} 

sint32 IC3RobotCom::GetNumFuzzyVariables(sint32 idx_section)
{
	
	C3TRY {
		return m_ai_main->GetNumFuzzyVariables(idx_section);
	}

	C3EXCEPT

	return 0;
}

void IC3RobotCom::GetFuzzyGraph(THIS_ sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val)
{
	
	C3TRY {
		m_ai_main->GetFuzzyGraph(idx_section, idx_variable, 
			label, minx, maxx, miny, maxy, num_graphs, num_x, height, defuzz_val);
	}

	C3EXCEPT
}

BOOL IC3RobotCom::ReloadFuzzyLogic(BOOL run_rules)

{
	
    C3TRY {
		return m_ai_main->ReloadFuzzyLogic(run_rules); 
	}

	C3EXCEPT
	return FALSE;
}

void IC3RobotCom::ResetFuzzyInput(sint32 idx_section, sint32 idx_variable, 
        double new_defuzz_val)
{
	
	C3TRY {
		m_ai_main->ResetFuzzyInput(idx_section, idx_variable, 
									new_defuzz_val);
	}

	C3EXCEPT
}


void IC3RobotCom::DumpStats()

{
	
	C3TRY {
		m_ai_main->DumpStats(); 
	}

	C3EXCEPT
} 

void IC3RobotCom::RegisterDiplomaticRequest(IC3DiplomaticRequest *request)
{
	
	C3TRY {
		m_ai_main->RegisterDiplomaticRequest(request);
	}

	C3EXCEPT
}

void IC3RobotCom::RegisterDiplomaticResponse(IC3DiplomaticRequest *request)
{
	
	C3TRY {
		m_ai_main->RegisterDiplomaticResponse(request);
	}

	C3EXCEPT
}

void IC3RobotCom::RegisterYouAttacked(PLAYER_INDEX defender)
{
	
	C3TRY {
		m_ai_main->RegisterYouAttacked(defender);
	}

	C3EXCEPT
}

void IC3RobotCom::RegisterAttackedBy(PLAYER_INDEX agressor)
{
	
	C3TRY {
		m_ai_main->RegisterAttackedBy(agressor);
	}

	C3EXCEPT
}

void IC3RobotCom::RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner)
{
	C3TRY {
		m_ai_main->RegisterFightWinner(attack_owner, attack_start_num,
            defense_owner, defense_start_num, winner);
	}

	C3EXCEPT
}


void IC3RobotCom::RegisterContactMade(PLAYER_INDEX contacted)
{
	
	C3TRY {
		m_ai_main->RegisterContactMade(contacted);
	}

	C3EXCEPT
}

void  IC3RobotCom::RegisterLeaveOurLands(PLAYER_INDEX get_off_me)
{
	
	C3TRY {
		m_ai_main->RegisterLeaveOurLands(get_off_me);
	}

	C3EXCEPT
}

void IC3RobotCom::RegisterLeaveOurLandsExpired (PLAYER_INDEX get_off_me)
{
	
	C3TRY {
		m_ai_main->RegisterLeaveOurLandsExpired(get_off_me);
	}

	C3EXCEPT
}


STDMETHODIMP_(void) IC3RobotCom::RegisterPlayerInfo(PLAYER_INDEX index, C3PlayerInfo *pi)
{
	
	C3TRY {
		m_ai_main->RegisterPlayerInfo(index, pi);
	}

	C3EXCEPT
}

void IC3RobotCom::SetPersonality(const char *filename)
{
	
	C3TRY {
		char fullname[1024];
		sprintf(fullname, "personality%s.fli", filename);
		m_ai_main->SetPersonalityFilename(fullname);
	}

	C3EXCEPT
}

void IC3RobotCom::SetInputLogComment(const char *comment)
{
#ifdef _DEBUG
	
	C3TRY {
		extern MBCHAR s_inputComment[_MAX_PATH];
		strcpy(s_inputComment, comment);
	}
	C3EXCEPT
#endif
}


BOOL IC3RobotCom::GetFliLoggingEnabled(void)
{
	BOOL res;
	C3TRY {
		res = fli_GetFliLoggingEnabled(m_ai_main->my_player_index);
	}
	C3EXCEPT
	return res;
}

void IC3RobotCom::SetFliLoggingEnabled(BOOL enabled)
{
	C3TRY {
		fli_SetFliLoggingEnabled(m_ai_main->my_player_index, enabled);
	}
	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::SendTradeBid(uint32 fromCity_id,
								 sint32 resource,
								 sint32 toCityOwner,
								 uint32 toCity_id,
								 sint32 price,
								 uint32 bidId)
{
	C3TRY {
		m_ai_main->ReceiveTradeBid(fromCity_id,
								   resource,
								   toCityOwner,
								   toCity_id,
								   price,
								   bidId);
	}

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterTradeOffer(IC3TradeOffer *offer)
{
	C3TRY {
		m_ai_main->RegisterTradeOffer(offer);
	}

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RemoveTradeOffer(uint32 id)
{
	C3TRY {
		m_ai_main->RemoveTradeOffer(id);
	}

	C3EXCEPT
}

STDMETHODIMP_(BOOL) IC3RobotCom::ConfirmAcceptTradeOffer(uint32 offer,
														 uint32 destCityID,
														 PLAYER_INDEX player)
{
	C3TRY {
		return m_ai_main->ConfirmAcceptTradeOffer(offer,
												  destCityID,
												  player);
	}

	C3EXCEPT
}



STDMETHODIMP_(void) IC3RobotCom::RegisterNewContinents()
{
	C3TRY {
		m_ai_main->RegisterNewContinents();
	}

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterYouBrokeCeaseFire(PLAYER_INDEX with)
{
    C3TRY {
		m_ai_main->RegisterYouBrokeCeaseFire(with);
    }

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterCeaseFireBroken(PLAYER_INDEX by)
{
    C3TRY {
		m_ai_main->RegisterCeaseFireBroken(by);
    }

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterYouBrokeAlliance(PLAYER_INDEX with)
{
    C3TRY {
		m_ai_main->RegisterYouBrokeAlliance(with);
    }

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterAllianceBroken(PLAYER_INDEX by)
{
    C3TRY {
		m_ai_main->RegisterAllianceBroken(by);
    }

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::ToggleSuperFastDebugMode(BOOL on)
{
#ifdef SUPER_DEBUG_SPEED
    C3TRY {
		if (!m_ai_main) return;
		if (!m_ai_main->m_planner) return;
		if (!m_ai_main->m_planner->the_scheduler) return;

		m_ai_main->m_planner->the_scheduler->SetSuperDebugSpeed(on);
    }

	C3EXCEPT
#endif 
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterHeNuked(PLAYER_INDEX he_did_it, 
    PLAYER_INDEX his_target) 
{
    C3TRY { 
        m_ai_main->RegisterHeNuked(he_did_it, his_target); 
    } 

	C3EXCEPT
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterHeBuiltNewAgeUnit(
    PLAYER_INDEX he_did_it, NEW_AGE unit_type)
{
    C3TRY { 
        m_ai_main->RegisterHeBuiltNewAgeUnit(he_did_it, unit_type); 
    } 

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterRegardIncrease(
	PLAYER_INDEX who, sint32 amount)
{
	C3TRY {
		m_ai_main->RegisterRegardIncrease(who, amount);
	}

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::GetFZRegard (sint32 him, sint32 &diplomatic, sint32 &unit, 
        sint32 &bonus)
{
   	C3TRY {
		m_ai_main->GetFZRegard (him, diplomatic, unit, bonus);
	}

	C3EXCEPT
}

STDMETHODIMP_(void)  IC3RobotCom::SetFZRegard (sint32 him, sint32 diplomatic, sint32 unit, 
        sint32 bonus)
{ 	C3TRY {
		m_ai_main->SetFZRegard(him, diplomatic, unit, bonus);
	}

	C3EXCEPT
}

STDMETHODIMP_(void)  IC3RobotCom::GetAllRegard(double i_like[k_MAX_PLAYERS]) 
{
    C3TRY {
		m_ai_main->GetAllRegard(i_like);
	}

	C3EXCEPT
}

STDMETHODIMP_ (void)  IC3RobotCom::RegisterAgreementAgainstMe(sint32 first_player, 
             sint32 second_player, double agreement_str)
{
    C3TRY {
		m_ai_main->RegisterAgreementAgainstMe(first_player, second_player, agreement_str);
	}

	C3EXCEPT
}


STDMETHODIMP_(void) IC3RobotCom::RegisterYouAreViolatingAgreement(AGREEMENT_TYPE agreement,
																  PLAYER_INDEX against,
																  sint32 rounds)
{
	C3TRY {
		m_ai_main->RegisterYouAreViolatingAgreement(agreement,
													against,
													rounds);
	}

	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterOtherCivIsViolatingAgreement (AGREEMENT_TYPE agreement,
																	   PLAYER_INDEX violator,
																	   sint32 rounds)
{
	C3TRY {
		m_ai_main->RegisterOtherCivIsViolatingAgreement(agreement, violator, rounds);
	}

	C3EXCEPT

}

STDMETHODIMP_(char *) IC3RobotCom::GetAIPName(void)
{
	C3TRY {
		if (!m_ai_main) return NULL;
		if (!m_ai_main->m_planner) return NULL;

		return (char *)m_ai_main->m_planner->the_AIP.name;
	}
	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterAttack(PLAYER_INDEX attacker,
												PLAYER_INDEX defender)
{
	C3TRY {
		m_ai_main->RegisterAttack(attacker, defender);
	}
	
	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterBrokeTreaty(PLAYER_INDEX breaker,
													 PLAYER_INDEX breakee)
{
	C3TRY {
		m_ai_main->RegisterBrokeTreaty(breaker, breakee);
	}

	C3EXCEPT
}

STDMETHODIMP_ (BOOL) IC3RobotCom::IWantToAttackRevealedArmy (uint32 revealed_army_id, MapPointData *pos, PLAYER_INDEX reveled_player, 
       uint32 attacking_army_id)
{
	C3TRY {
		return m_ai_main->IWantToAttackRevealedArmy(revealed_army_id, pos, 
            reveled_player, attacking_army_id);
	}
	
	C3EXCEPT
}

STDMETHODIMP_(void) IC3RobotCom::RegisterRemoveBuildNode(PLAYER_INDEX owner, 
											uint32 city,
											sint32 category, 
											sint32 type)
{
	C3TRY {
		m_ai_main->RegisterRemoveBuildNode(owner, 
			city,
			category, 
			type);
	}
	
	C3EXCEPT
}


STDMETHODIMP_(void) IC3RobotCom::RegisterSetBuildFront(	uint32 city,
														sint32 category, 
														sint32 type)
{
	C3TRY {
		m_ai_main->RegisterSetBuildFront(city,
										 category, 
										 type);
	}
	
	C3EXCEPT
}

STDMETHODIMP_ (BOOL) IC3RobotCom::ValidateArmyID(uint32 army_id, 
    sint32 unit_num)
{
    C3TRY {
		return m_ai_main->ValidateArmyID(army_id, unit_num); 
	}
	
	C3EXCEPT
}

STDMETHODIMP_ (void) IC3RobotCom::ForceRegard(sint32 forPlayer, double toRegard)
{
	C3TRY {
		m_ai_main->ForceRegard(forPlayer, toRegard);
	}

	C3EXCEPT
}


STDMETHODIMP_ (void) IC3RobotCom::ForceHate(sint32 forPlayer)
{
	C3TRY {
		m_ai_main->ForceHate(forPlayer);
	}

	C3EXCEPT
}

STDMETHODIMP_ (void) IC3RobotCom::SetBeingAttached(BOOL on)
{
	C3TRY {
		m_ai_main->SetBeingAttached(on);
	}

	C3EXCEPT
}

