












#define INITGUID


#include "ctp.h"
#include "globals.h"
#include "AICause.h"
#include "Order.h"

#include "RobotCOM.h"
#include "AiMain.h"


#include "IRobot.h"
#include "IC3GameState.h" 
#include "IC3Player.h"
#include "IC3World.h"
#include "IC3GameState.h"
#include "IC3DipReq.h"
#include "IC3Trade.h"
#include "IC3EndGameDB.h"

STDAPI CoCreateInstanceRobotCom(IUnknown *&obj)
{
    obj = new IC3RobotCom();
	obj->AddRef();

	return S_OK;
}

STDAPI InitDLLDebugging(void *memPtr, void *logPtr, void *funcPtr, 
						MBCHAR *civ3logName, sint32 *civ3logNumber, sint32 *civ3logLinesThisFile)
{
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

		if (m_ai_main->Init(gs, goal_time_slice, max_time_slice, max_total_time)) {           
     		return TRUE; 
		} else {

            Assert(0); 
			return FALSE; 
		}
}

STDMETHODIMP IC3RobotCom::Cleanup()
{
	delete m_ai_main; 
        m_ai_main = NULL; 

	return S_OK; 
}

void IC3RobotCom::Save (IC3CivArchive *archive) 
{ 
	m_ai_main->Save(archive);
}

BOOL IC3RobotCom::Load (IC3GameState *gs, IC3CivArchive *archive)
{
	return m_ai_main->Load(gs, archive);
}


STDMETHODIMP IC3RobotCom::BeginTurn(uint32 end_frame_by,  BOOL &my_turn_is_over)
{
	if (m_ai_main->BeginTurn(end_frame_by,  my_turn_is_over)) {
   		return S_OK; 
	} else {
		return E_FAIL; 
	}

	return E_FAIL;
}

STDMETHODIMP IC3RobotCom::ExecuteTimeSlice(uint32 end_frame_by,  BOOL &my_turn_is_over, 
    char debug_str[80] )
{
	if (m_ai_main->ExecuteTimeSlice(end_frame_by,  my_turn_is_over, debug_str)) {
  		return S_OK; 
	} else {
		return E_FAIL; 
	}

	return E_FAIL;
}

STDMETHODIMP IC3RobotCom::RegisterAddArmy (uint32 id, 
   CAUSE_NEW_ARMY cause, uint32 hc_id, sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE], 
   sint32 unit_hp[k_MAX_ARMY_SIZE])
{
	m_ai_main->RegisterAddArmy (id, cause, hc_id, nUnits, unit_type, unit_hp);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterMyAddCity (uint32 id, 
    CAUSE_NEW_CITY cause, MapPointData *pos)
{
	m_ai_main->RegisterMyAddCity (id, cause, pos);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterHisAddCity (uint32 id, 
    CAUSE_NEW_CITY cause, MapPointData *pos)
{
	m_ai_main->RegisterHisAddCity (id, cause, pos);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterAdjustDeathDifferential (double adjustment, MapPointData &pos) 
{
	m_ai_main->RegisterAdjustDeathDifferential (adjustment, pos);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::Load_AIP (char *aip_file_name) 
{
	return S_OK; 
}


STDMETHODIMP IC3RobotCom::Set_AILog_Verbosity (sint32 log_level) 
{
	return S_OK; 
}


STDMETHODIMP IC3RobotCom::RegisterMyRemoveArmy (uint32 id, CAUSE_REMOVE_ARMY cause)
{
	m_ai_main->RegisterMyRemoveArmy (id, cause);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterHisRemoveArmy (PLAYER_INDEX owner, uint32 id)
{
	m_ai_main->RegisterHisRemoveArmy (owner, id);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterMyRemoveCity (uint32 id, 
    CAUSE_REMOVE_CITY cause)
{
	m_ai_main->RegisterMyRemoveCity (id, cause);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterHisRemoveCity(PLAYER_INDEX owner, uint32 id)
{
	m_ai_main->RegisterHisRemoveCity(owner, id);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterCityDiedAt(MapPointData *pos)
{
	m_ai_main->RegisterCityDiedAt(pos);
	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterClearCargo(uint32 army_id)
{
	m_ai_main->RegisterClearCargo(army_id);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterInsertCargo (uint32 army_id, sint32 unit_type, sint32 hp)
{
	m_ai_main->RegisterInsertCargo(army_id, unit_type, hp);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterUnloadCargo (uint32 army_id, sint32 unit_type, sint32 hp, 
    CAUSE_REMOVE_ARMY cause)
{
	m_ai_main->RegisterUnloadCargo(army_id, unit_type, hp, cause);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterDeadUnit (uint32 army_id, 
     sint32 unit_type)
{
	m_ai_main->RegisterDeadUnit(army_id, unit_type);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterArmyWasGrouped (uint32 target, sint32 nUnits, 
        sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE])

{
	m_ai_main->RegisterArmyWasGrouped(target, nUnits, unit_type, unit_hp);
	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterYourArmyWasMoved(uint32 army_id, MapPointData *new_pos) 
{
	m_ai_main->RegisterYourArmyWasMoved(army_id, new_pos);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved, 
        MapPointData *new_pos)
{
	m_ai_main->RegisterHisArmyTeleported(his_owner, i_moved, new_pos);
	return S_OK;
}


STDMETHODIMP IC3RobotCom::RegisterCreateBuilding (uint32 city_id, 
        sint32 building_type)
{
	m_ai_main->RegisterCreateBuilding (city_id, building_type); 
	return S_OK;
} 

STDMETHODIMP IC3RobotCom::RegisterLostBuilding (uint32 city_id, 
        sint32 building_type)
{
	m_ai_main->RegisterLostBuilding (city_id, building_type); 
	return S_OK;
} 

STDMETHODIMP IC3RobotCom::RegisterNewGovernment (uint32 city_id, 
        sint32 government_type)
{
	m_ai_main->RegisterNewGovernment (city_id, government_type); 
	return S_OK;
}
 

STDMETHODIMP IC3RobotCom::RegisterNewWGF (sint32 w, sint32 g, sint32 f)
{
        m_ai_main->RegisterNewWGF(w, g, f);
        return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterNewCapitolBuilding (uint32 city_id)
{
	m_ai_main->RegisterNewCapitolBuilding (city_id); 
	return S_OK;
} 


STDMETHODIMP IC3RobotCom::RegisterCreateWonder (uint32 city_id, 
        sint32 wonder_type, PLAYER_INDEX owner)
{
	m_ai_main->RegisterCreateWonder (city_id, wonder_type, owner); 
	return S_OK;
} 

STDMETHODIMP IC3RobotCom::RegisterLearnedScience(sint32 idx_sci, CAUSE_SCI cause)
{
	m_ai_main->RegisterLearnedScience(idx_sci, cause); 
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterNewPlayer (PLAYER_INDEX player_id, 
    CAUSE_NEW_PLAYER cause, PLAYER_INDEX old_owner)
{
	m_ai_main->RegisterNewPlayer(player_id, cause, old_owner);
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterDeadPlayer (PLAYER_INDEX player_id)
{
	m_ai_main->RegisterDeadPlayer(player_id);
	return S_OK;
}

STDMETHODIMP IC3RobotCom::RegisterAddInstallation(MapPointData &pos, sint32 inst_type)
{
	m_ai_main->RegisterAddInstallation(pos, inst_type); 
	return S_OK; 
}

STDMETHODIMP IC3RobotCom::RegisterRemoveInstallation(MapPointData &pos, sint32 inst_type)
{
	m_ai_main->RegisterRemoveInstallation(pos, inst_type); 
        return S_OK; 
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterCityAttack(uint32 my_city_id, PLAYER_INDEX his_owner, uint32 his_unit_id, UNIT_ORDER_TYPE attack_type)
{
	m_ai_main->RegisterCityAttack(my_city_id, his_owner, his_unit_id, attack_type);
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterHostileAction(UNIT_ORDER_TYPE attack_type, 
    uint32 my_army_id, PLAYER_INDEX his_owner, uint32 his_unit_id, MapPointData *target_pos)
{
	m_ai_main->RegisterHostileAction(attack_type, my_army_id, his_owner, his_unit_id, 
					 target_pos);
}


STDMETHODIMP_ (void) IC3RobotCom::RegisterPollution(MapPointData *target_pos)
{
	m_ai_main->RegisterPollution(target_pos);
}


STDMETHODIMP_ (void) IC3RobotCom::RegisterPiracy(PLAYER_INDEX he_did_it,                                              
    PLAYER_INDEX source_owner, uint32 src_id, PLAYER_INDEX dest_owner, 
    uint32 dest_id, sint32 resource)

{
	m_ai_main->RegisterPiracy(he_did_it, source_owner, src_id, dest_owner, dest_id, 
				  resource);
}

sint32 IC3RobotCom::GetNumFuzzySections()
{
	return 0;
} 

sint32 IC3RobotCom::GetNumFuzzyVariables(sint32 idx_section)
{
  
  return 0;
}

void IC3RobotCom::GetFuzzyGraph(THIS_ sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val)
{
  
}

BOOL IC3RobotCom::ReloadFuzzyLogic(BOOL run_rules)

{
  
  return FALSE;
}

void IC3RobotCom::ResetFuzzyInput(sint32 idx_section, sint32 idx_variable, 
        double new_defuzz_val)
{
  
}


void IC3RobotCom::DumpStats()

{
  
} 

void IC3RobotCom::RegisterDiplomaticRequest(IC3DiplomaticRequest *request)
{
	m_ai_main->RegisterDiplomaticRequest(request);
}

void IC3RobotCom::RegisterDiplomaticResponse(IC3DiplomaticRequest *request)
{
	m_ai_main->RegisterDiplomaticResponse(request);
}

void IC3RobotCom::RegisterYouAttacked(PLAYER_INDEX defender)
{
  
}

void IC3RobotCom::RegisterAttackedBy(PLAYER_INDEX agressor)
{
  
}

void IC3RobotCom::RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
        sint32 defense_owner, sint32 defense_start_num, sint32 winner)
{
	m_ai_main->RegisterFightWinner(attack_owner, 
				       attack_start_num,
				       defense_owner, 
				       defense_start_num, 
				       winner);
}


void IC3RobotCom::RegisterContactMade(PLAYER_INDEX contacted)
{
	m_ai_main->RegisterContactMade(contacted);
}

void  IC3RobotCom::RegisterLeaveOurLands(PLAYER_INDEX get_off_me)
{
	m_ai_main->RegisterLeaveOurLands(get_off_me);
}

void IC3RobotCom::RegisterLeaveOurLandsExpired (PLAYER_INDEX get_off_me)
{
	m_ai_main->RegisterLeaveOurLandsExpired(get_off_me);
}


STDMETHODIMP_(void) IC3RobotCom::RegisterPlayerInfo(PLAYER_INDEX index, C3PlayerInfo *pi)
{
	m_ai_main->RegisterPlayerInfo(index, pi);
}

void IC3RobotCom::SetPersonality(const char *filename)
{
	char fullname[1024];
	sprintf(fullname, "personality%s.fli", filename);
	m_ai_main->SetPersonalityFilename(fullname);
}

void IC3RobotCom::SetInputLogComment(const char *comment)
{
  
}

BOOL IC3RobotCom::GetFliLoggingEnabled(void)
{
  
  return FALSE;
}

void IC3RobotCom::SetFliLoggingEnabled(BOOL enabled)
{
  
}

STDMETHODIMP_(void) IC3RobotCom::SendTradeBid(uint32 fromCity_id,
					      sint32 resource,
					      sint32 toCityOwner,
					      uint32 toCity_id,
					      sint32 price,
					      uint32 bidId)
{
	m_ai_main->ReceiveTradeBid(fromCity_id,
				   resource,
				   toCityOwner,
				   toCity_id,
				   price,
				   bidId);
}

STDMETHODIMP_(void) IC3RobotCom::RegisterTradeOffer(IC3TradeOffer *offer)
{
	m_ai_main->RegisterTradeOffer(offer);
}

STDMETHODIMP_(void) IC3RobotCom::RemoveTradeOffer(uint32 id)
{
	m_ai_main->RemoveTradeOffer(id);
}

STDMETHODIMP_(BOOL) IC3RobotCom::ConfirmAcceptTradeOffer(uint32 offer,
							 uint32 destCityID,
							 PLAYER_INDEX player)
{
	return m_ai_main->ConfirmAcceptTradeOffer(offer,
						  destCityID,
						  player);
}



STDMETHODIMP_(void) IC3RobotCom::RegisterNewContinents()
{
	m_ai_main->RegisterNewContinents();
}

STDMETHODIMP_(void) IC3RobotCom::RegisterYouBrokeCeaseFire(PLAYER_INDEX with)
{
	m_ai_main->RegisterYouBrokeCeaseFire(with);
}

STDMETHODIMP_(void) IC3RobotCom::RegisterCeaseFireBroken(PLAYER_INDEX by)
{
	m_ai_main->RegisterCeaseFireBroken(by);
}

STDMETHODIMP_(void) IC3RobotCom::RegisterYouBrokeAlliance(PLAYER_INDEX with)
{
	m_ai_main->RegisterYouBrokeAlliance(with);
}

STDMETHODIMP_(void) IC3RobotCom::RegisterAllianceBroken(PLAYER_INDEX by)
{
	m_ai_main->RegisterAllianceBroken(by);
}

STDMETHODIMP_(void) IC3RobotCom::ToggleSuperFastDebugMode(BOOL on)
{
  
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterHeNuked(PLAYER_INDEX he_did_it, 
    PLAYER_INDEX his_target) 
{
	m_ai_main->RegisterHeNuked(he_did_it, his_target); 
}

STDMETHODIMP_ (void) IC3RobotCom::RegisterHeBuiltNewAgeUnit(
    PLAYER_INDEX he_did_it, NEW_AGE unit_type)
{
  
}

STDMETHODIMP_(void) IC3RobotCom::RegisterRegardIncrease(
	PLAYER_INDEX who, sint32 amount)
{
	m_ai_main->RegisterRegardIncrease(who, amount);
}

STDMETHODIMP_(void) IC3RobotCom::GetFZRegard (sint32 him, sint32 &diplomatic, sint32 &unit, 
        sint32 &bonus)
{
	m_ai_main->GetFZRegard (him, diplomatic, unit, bonus);
}

STDMETHODIMP_(void)  IC3RobotCom::SetFZRegard (sint32 him, sint32 diplomatic, sint32 unit, 
        sint32 bonus)
{
	m_ai_main->SetFZRegard(him, diplomatic, unit, bonus);
}

STDMETHODIMP_(void)  IC3RobotCom::GetAllRegard(double i_like[k_MAX_PLAYERS]) 
{
  
}

STDMETHODIMP_ (void)  IC3RobotCom::RegisterAgreementAgainstMe(sint32 first_player, 
							      sint32 second_player, 
							      double agreement_str)
{
	m_ai_main->RegisterAgreementAgainstMe(first_player, 
					      second_player, 
					      agreement_str);
}


STDMETHODIMP_(void) IC3RobotCom::RegisterYouAreViolatingAgreement(AGREEMENT_TYPE agreement,
								  PLAYER_INDEX against,
								  sint32 rounds)
{
  
}

STDMETHODIMP_(void) IC3RobotCom::RegisterOtherCivIsViolatingAgreement (AGREEMENT_TYPE agreement,
								       PLAYER_INDEX violator,
								       sint32 rounds)
{
  
}

STDMETHODIMP_(char *) IC3RobotCom::GetAIPName(void)
{
  
  return 0x0;
}

STDMETHODIMP_(void) IC3RobotCom::RegisterAttack(PLAYER_INDEX attacker,
												PLAYER_INDEX defender)
{
	m_ai_main->RegisterAttack(attacker, defender);
}

STDMETHODIMP_(void) IC3RobotCom::RegisterBrokeTreaty(PLAYER_INDEX breaker,
													 PLAYER_INDEX breakee)
{
	m_ai_main->RegisterBrokeTreaty(breaker, breakee);
}

STDMETHODIMP_ (BOOL) IC3RobotCom::IWantToAttackRevealedArmy (uint32 revealed_army_id, MapPointData *pos, PLAYER_INDEX reveled_player, 
       uint32 attacking_army_id)
{
	
	return FALSE;
}

STDMETHODIMP_(void) IC3RobotCom::RegisterRemoveBuildNode(PLAYER_INDEX owner, 
											uint32 city,
											sint32 category, 
											sint32 type)
{
	m_ai_main->RegisterRemoveBuildNode(owner, 
					   city,
					   category, 
					   type);
}


STDMETHODIMP_(void) IC3RobotCom::RegisterSetBuildFront(	uint32 city,
							sint32 category, 
							sint32 type)
{
	m_ai_main->RegisterSetBuildFront(city,
					 category, 
					 type);
}

STDMETHODIMP_ (BOOL) IC3RobotCom::ValidateArmyID(uint32 army_id, 
    sint32 unit_num)
{
	
	return TRUE;
}

STDMETHODIMP_ (void) IC3RobotCom::ForceRegard(sint32 forPlayer, double toRegard)
{
	m_ai_main->ForceRegard(forPlayer, toRegard);
}


STDMETHODIMP_ (void) IC3RobotCom::ForceHate(sint32 forPlayer)
{
	m_ai_main->ForceHate(forPlayer);
}

STDMETHODIMP_ (void) IC3RobotCom::SetBeingAttached(BOOL on)
{
	m_ai_main->SetBeingAttached(on);
}
