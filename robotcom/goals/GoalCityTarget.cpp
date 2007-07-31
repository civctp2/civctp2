






















































































































































































































































































































































#include "c3.h"
#include "DynArr.h"
#include "CivArchive.h"
#include "IMapPointData.h"
#include "IC3World.h"
#include "IC3Player.h"
#include "IC3UnitDB.h"


#include "IC3GameState.h"
#include "AiMain.h"
#include "AiMap.h"
#include "airndcnt.h"
#include "Agreement.h"
#include "gold.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"


#include "FzOut.h"
extern double fz_min_force_matching_percent;
extern double fz_max_force_matching_percent;


#include "BSet.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"
#include "ScienceAgent.h"
#include "Squad_Strength.h"


#include "GoalCityTarget.h"
#include "GoalBombard.h" 
#include "WonderTbl.h"

#include "AILOG.h"


#include "Order.h"







void GoalCityTarget::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
	sint32 v;

	
    if (archive.IsStoring()) 
		{ 
			v = (sint32) what_goal;
			archive << v;
			archive << m_playerIndex;
			archive << m_foreignPlayer;
			v = m_targetId.GetVal();
			archive << v;
			archive << m_pos->x;
			archive << m_pos->y;
			archive << m_pos->z;
		} 
	else 
		{ 
			archive >> v;
			what_goal = (GOAL_TYPE) v;
			archive >> m_playerIndex;
			archive >> m_foreignPlayer;
			archive >> v;
			m_targetId = v;
			
			archive >> m_pos->x;
			archive >> m_pos->y;
			archive >> m_pos->z;





		}
}

BOOL GoalCityTarget::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}














BOOL GoalCityTarget::PretestDipState(AiMain *ai, const MapPointData & pos, const GOAL_TYPE & type)
{
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
	MapPointData toughpos = pos;
	PLAYER_INDEX target_owner = ai->m_world->GetCityOwner (&toughpos);
	sint32 cost;

	
	if (type == GOAL_TYPE_SUE_FRANCHISE)
		{
			cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_SUE_FRANCHISE);
			return ai->m_gold->CanSpendSpecialAction(ai, cost);
		}


	if (type == GOAL_TYPE_REFORM_CITY)
		{
			cost = ai->m_gs->GetOrderGoldCost(UNIT_ORDER_REFORM);
			return ai->m_gold->CanSpendSpecialAction(ai, cost);
		}

	
	if (type == GOAL_TYPE_RECOVER_PROBE)
		return TRUE;

	
	Assert(target_owner != ai->my_player_index);
	Assert(target_owner >= 0 );
	Assert(target_owner < k_MAX_PLAYERS );
	if (target_owner == ai->my_player_index ||
		target_owner < 0 ||
		target_owner >= k_MAX_PLAYERS ||
		!ai->m_foreigner[target_owner])
		return FALSE;

	

	switch (type) 
		{
			case GOAL_TYPE_NUKE_CITY:
			case GOAL_TYPE_MAKE_PARK:
			case GOAL_TYPE_PLANT_NUKE:
                if (!ai->m_foreigner[target_owner]->CanUseMassDestructionOnHim() ||
					!ai->m_foreigner[target_owner]->IsAtHotWar() )
                    return false;

                
			case GOAL_TYPE_BOMBARD:
            case GOAL_TYPE_SEIGE:
			case GOAL_TYPE_HARASS_CITY:
		
				
				if ( !ai->m_foreigner[target_owner]->IsAtHotWar() ) 
					return false;
				return true;
				break;

			case GOAL_TYPE_UNDERGROUND_RAILWAY:
			case GOAL_TYPE_FRANCHISING:
			case GOAL_TYPE_ASSASINATE_RULER:
			case GOAL_TYPE_STEAL_TECHNOLOGY:
			case GOAL_TYPE_INJOIN:
			case GOAL_TYPE_INCITE_REVOLUTION:
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
			case GOAL_TYPE_CONVERT_CITY:
            case GOAL_TYPE_BIOTERROR_CITY:
            case GOAL_TYPE_NANOATTACK_CITY:
				
				if ( !ai->m_foreigner[target_owner]->IsAtColdWar() ) 
					return false;
				return true;
				break;

            case GOAL_TYPE_ESTABLISH_EMBASSY:
				
				if ( ai->m_foreigner[target_owner]->IsAtHotWar())
					return false;

				return true;
				break;
			default:
				
                return false; 
		}
	return false;
}











BOOL GoalCityTarget::IsTargetType(
	AiMain *ai,
	MapPointData & pos, 
	const GOAL_TYPE & type
)
{
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
	MapPointData toughpos = pos;
	PLAYER_INDEX converted_to_player;
	PLAYER_INDEX franchised_by_player;
	BOOL is_nuke_safe;
	BOOL is_unknown_id;
	uint32 target_id_val;
	sint32 slave_count;
	PLAYER_INDEX target_owner;
	BOOL id_found = false;
	sint32 population_size;

	Assert(ai->m_world->IsCityHere(&toughpos));
	id_found = ai->m_world->GetCityId(&toughpos, &target_owner, &target_id_val);
	
	
	if (!id_found)
		return false;

	
	if (! PretestDipState(ai, pos, type))
		return false;

	switch (type) 
		{
			case GOAL_TYPE_NUKE_CITY:
                if (!ai->m_science_agent->CanMissileNuke()) 
					return FALSE; 

				
				is_nuke_safe = ai->m_map->IsNukeSafeCity(pos);
				return (!is_nuke_safe);
				break;

			case GOAL_TYPE_PLANT_NUKE:
                if (!ai->m_science_agent->CanPlantNuke()) 
					return FALSE; 

				
				break;

			case GOAL_TYPE_BOMBARD:
				
				return ai->m_world->IsArmyHere(&toughpos);
				break;

			case GOAL_TYPE_SEIGE:
			case GOAL_TYPE_HARASS_CITY:
				
				return true;
				break;

			case GOAL_TYPE_MAKE_PARK:
                if (!ai->m_science_agent->CanMakePark()) 
					return FALSE; 

				
				return true;
				break;

			case GOAL_TYPE_UNDERGROUND_RAILWAY:


                if (!ai->m_science_agent->CanUndergroundRailway()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				slave_count = ai->m_world->GetCitySlaveCount(&toughpos);

				
				return (slave_count > 0);
				break;

			case GOAL_TYPE_CONVERT_CITY:
                if (!ai->m_science_agent->CanConvertCity()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				converted_to_player = ai->m_world->
					GetConvertedTo(target_id_val, &is_unknown_id, &toughpos);

				
				
				return (converted_to_player != ai->my_player_index);
				break;
			case GOAL_TYPE_REFORM_CITY:

				
				if (ai->my_player_index != target_owner)
					return FALSE;

				converted_to_player = ai->m_world->
					GetConvertedTo(target_id_val, &is_unknown_id, &toughpos);

				
				
				
				population_size = ai->m_player->
					GetCityPopulation(target_owner, target_id_val, 
									  &is_unknown_id);

				
				
				return (converted_to_player != ai->my_player_index &&
						converted_to_player >= 0 &&
						population_size > 2);
				break;
            case GOAL_TYPE_BIOTERROR_CITY:

				
				if (!ai->m_foreigner[target_owner]) 
					return FALSE;

                if (!ai->m_science_agent->CanBioterrorCity()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

                Assert(ai->m_foreigner[target_owner]);
                if (0 < ai->m_player->CountMyTradeToForeignCity(target_owner, target_id_val, &is_unknown_id)) 
                    return FALSE; 

                return TRUE; 
            case GOAL_TYPE_NANOATTACK_CITY:

				
				if (!ai->m_foreigner[target_owner]) 
					return FALSE;

                if (!ai->m_science_agent->CanNanoattackCity()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

                Assert(ai->m_foreigner[target_owner]); 
                if (0 < ai->m_player->CountMyTradeToForeignCity(target_owner, target_id_val, &is_unknown_id)) 
                    return FALSE; 

                return TRUE; 

			case GOAL_TYPE_ESTABLISH_EMBASSY:
                if (!ai->m_science_agent->CanEstablishEmbassy()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				
				
				if (ai->m_foreigner[target_owner])
					if (ai->m_foreigner[target_owner]->GetBonusRegard() > 0.0)
					return FALSE;

				
				return (!ai->m_player->HasEmbassyWith(target_owner));
				break;

			case GOAL_TYPE_FRANCHISING:
                if (!ai->m_science_agent->CanFranchise()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				franchised_by_player = ai->m_world->
					GetFranchiseOwner(target_id_val, &is_unknown_id, &toughpos);
				
				
				return (franchised_by_player != ai->my_player_index);

			case GOAL_TYPE_SUE_FRANCHISE:

				
				if (!ai->m_science_agent->CanSue())
					return FALSE;

				
				if (ai->my_player_index != target_owner)
					return FALSE;

				franchised_by_player = ai->m_world->
					GetFranchiseOwner(target_id_val, &is_unknown_id, &toughpos);
				
				
				return (franchised_by_player != ai->my_player_index &&
						franchised_by_player >= 0);
				break;

			

			case GOAL_TYPE_ASSASINATE_RULER:
                if (!ai->m_science_agent->CanAssasinateRuler()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				return TRUE;
				break;

			case GOAL_TYPE_STEAL_TECHNOLOGY:
                if (!ai->m_science_agent->CanStealTechnology()) return FALSE; 
    				return TRUE;

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				break;

			case GOAL_TYPE_INJOIN:
                if (!ai->m_science_agent->CanInjoin()) 
					return FALSE; 

                if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				franchised_by_player = ai->m_world->
					GetFranchiseOwner(target_id_val, &is_unknown_id, &toughpos);

                Assert(!is_unknown_id); 
				
				if (franchised_by_player == ai->my_player_index)
					return FALSE;
                
				return TRUE;
				break;

			case GOAL_TYPE_INCITE_REVOLUTION:
                if (!ai->m_science_agent->CanInciteRevolution()) 
					return FALSE; 

               if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				return TRUE;
				break;

			case GOAL_TYPE_CAUSE_UNHAPPINESS:
                if (!ai->m_science_agent->CanCauseUnhappiness()) 
 					return FALSE; 

                 if ((pos.z == SPACE_Z) || ai->m_world->IsMoveTypeWater(&pos) || ai->m_world->IsMoveTypeShallowWater(&pos))
                    return FALSE; 

				return TRUE;
				break;

			case GOAL_TYPE_RECOVER_PROBE:
				
				
				
				if (target_owner == ai->my_player_index)
					return TRUE;

			default:
				return FALSE;  
		}

	return FALSE;
}













void GoalCityTarget::SetupCityTargetGoals(
	AiMain *ai
)
{
	
	

	
    sint32 i; 
    BSet<ForeignCity> *vc=NULL; 
    BSetID c_id; 
    ForeignCity *his_city=NULL; 
    CityAgent *my_city=NULL; 
	MapPointData target_pos;			
	GoalCityTarget *tmp_goal = NULL;
	uint32 tindex;
	

	
	
    for (my_city = ai->m_city_set->First(c_id); 
		 ai->m_city_set->Last(); 
		 my_city = ai->m_city_set->Next(c_id)) 
		{ 
			my_city->GetPos(target_pos);
			if ( IsTargetType(ai, target_pos, GOAL_TYPE_REFORM_CITY))
				{
					
					tmp_goal = new GoalCityTarget(ai, 
												  target_pos, 
												  GOAL_TYPE_REFORM_CITY,
												  my_city->GetID(),
												  ai->my_player_index);
				}

			if ( IsTargetType(ai, target_pos, GOAL_TYPE_SUE_FRANCHISE))
				{
					
					tmp_goal = new GoalCityTarget(ai, 
												  target_pos, 
												  GOAL_TYPE_SUE_FRANCHISE,
												  my_city->GetID(),
												  ai->my_player_index);
				}

			
			if (ai->m_world->IsWormholeVisible(ai->my_player_index))
			{
				
				tmp_goal = new GoalCityTarget(ai, 
											  target_pos, 
											  GOAL_TYPE_RECOVER_PROBE,
											  my_city->GetID(),
											  ai->my_player_index);
			}
		}

	
	
    for (i=0; i<k_MAX_PLAYERS; i++) 
		{ 
			
			if (!ai->m_foreigner[i]) 
				continue;

			
            

			
			vc = ai->m_foreigner[i]->GetKnownCities();

           
			
			for (his_city = vc->First(c_id); vc->Last(); his_city =  vc->Next(c_id)) 
				{
					his_city->GetPos(target_pos);

					
					for (tindex = 0; tindex < (uint32) GOAL_TYPE_MAX; tindex++) {

							
							if ((GOAL_TYPE)tindex == GOAL_TYPE_NANOATTACK_CITY ||
								(GOAL_TYPE)tindex == GOAL_TYPE_BIOTERROR_CITY)
								if (his_city->HasBeenNanoAttacked())
									continue;

							if ( IsTargetType(ai, target_pos, (GOAL_TYPE)tindex))
								 {
									 if ((GOAL_TYPE)tindex == GOAL_TYPE_BOMBARD)
										 {
											 
										 GoalBombard *tmp_bombard;
										 tmp_bombard = new GoalBombard(ai,
											 his_city,
											 target_pos);
										 
										 ai->m_planner->Add_Goal(tmp_bombard);
										 }
									 else
										 {
											 
											 tmp_goal = new GoalCityTarget(ai, 
																		   target_pos, 
																		   (GOAL_TYPE) tindex, 
																		   his_city->GetID(),
																		   i);
										 }
								 }
						}
				} 
		} 
}













GoalCityTarget::GoalCityTarget()
{
	
	BOOL GOAL_CITY_TARGET_DEFAULT_CTOR = 0;
	Assert(GOAL_CITY_TARGET_DEFAULT_CTOR);
    Init(); 
}













GoalCityTarget::GoalCityTarget
(
	AiMain *ai,							
	const MapPointData &target,			
	const GOAL_TYPE &type,			
	const BSetID &cityid,				
	const PLAYER_INDEX foreign_player   
) 
: ArmyGoal(ai, 1000000, target)
{
	BOOL ASSERT_CITY_TARGET_INVALID = 0;

    Init(); 

	
	what_goal = type;

	switch( what_goal )
		{
			case GOAL_TYPE_REFORM_CITY:
				the_class = MILITARY_GOAL_CLASS;
				break;
			case GOAL_TYPE_SEIGE:
			case GOAL_TYPE_HARASS_CITY:
				the_class = SEIGE_GOAL_CLASS;
				break;
			case GOAL_TYPE_CONVERT_CITY:
				the_class = GOD_GOAL_CLASS;				
				break;
			case GOAL_TYPE_ESTABLISH_EMBASSY:
				the_class = DIPLOMATIC_GOAL_CLASS;
				break;
            case GOAL_TYPE_BIOTERROR_CITY:
            case GOAL_TYPE_NANOATTACK_CITY:
			case GOAL_TYPE_STEAL_TECHNOLOGY:
			case GOAL_TYPE_ASSASINATE_RULER:
			case GOAL_TYPE_INCITE_REVOLUTION:
			case GOAL_TYPE_PLANT_NUKE:
				the_class = ESPIONAGE_GOAL_CLASS;
				break;
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
				the_class = ABOLITIONIST_GOAL_CLASS;
				break;
			case GOAL_TYPE_FRANCHISING:
				the_class = BUSINESS_GOAL_CLASS;
				break;
			case GOAL_TYPE_INJOIN:
			case GOAL_TYPE_SUE_FRANCHISE:
				the_class = LAW_GOAL_CLASS;
				break;
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
				the_class = MEDIA_GOAL_CLASS;				
				break;
			case GOAL_TYPE_NUKE_CITY:
			case GOAL_TYPE_MAKE_PARK:
				the_class = DESTROY_CITY_GOAL_CLASS;
				break;
			case GOAL_TYPE_RECOVER_PROBE:
				the_class = WORMHOLE_PROBE_GOAL_CLASS;
				break;
			default:
				Assert(ASSERT_CITY_TARGET_INVALID);
		}

	
	
	
	

	
	m_playerIndex = ai->my_player_index;
	m_foreignPlayer = foreign_player;

	
	*m_pos = target;

	
	removal_time = REMOVE_WHEN_SORTED;

	
	ai->m_planner->Add_Goal(this);

	
	sint32 id = this->GetID();
	Assert(id != UNASSIGNED_ID);
	
	
	m_targetId = cityid;

	m_ai = ai; 
} 	












GoalCityTarget::GoalCityTarget(AiMain *ai, CivArchive &archive)
{
    Init(); 
	m_ai = ai; 
	Serialize(ai, archive); 
}













void GoalCityTarget::Init()
{
	what_goal = GOAL_TYPE_MAX;
    removal_time = REMOVE_WHEN_SORTED;
	raw_priority = BAD_UTILITY;
	m_rallying = TRUE; 
}













GoalCityTarget::~GoalCityTarget()
{ 
	
} 













BOOL GoalCityTarget::operator==(const GoalCityTarget & ref) const
{
	return ref.GetID() == GetID() &&
		ref.GetPlayerIndex() == GetPlayerIndex();
}

















GOAL_TYPE GoalCityTarget::GetType () const
{
	return what_goal;
}













const PLAYER_INDEX & GoalCityTarget::GetPlayerIndex() const
{
	return m_playerIndex;
}












ForeignCity *GoalCityTarget::GetTarget(AiMain *ai) 
{ 
	if (m_targetId.GetVal()) {
		
		if (!ai->m_foreigner) return NULL;
		if (!ai->m_foreigner[m_foreignPlayer]) return NULL;

        BSet<ForeignCity> *vua = ai->m_foreigner[m_foreignPlayer]->GetKnownCities();
        BSetID h_id; 
        ForeignCity *his_city=NULL; 
        for (his_city = vua->First(h_id); vua->Last(); his_city =  vua->Next(h_id)) { 
            if (his_city->GetID().GetVal() == m_targetId.GetVal()) {  
                return his_city;
            }
		}
	}
	return NULL;
} 














void GoalCityTarget::Display_Goal_Type(AiMain *ai)
{
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		{
			switch(GetType())
				{
					case GOAL_TYPE_NUKE_CITY:
						AILOG(( wstr,  "GOAL_TYPE_NUKE_CITY"));
						break;
					case GOAL_TYPE_SEIGE:
						AILOG(( wstr,  "GOAL_TYPE_SEIGE"));
						break;
					case GOAL_TYPE_HARASS_CITY:
						AILOG(( wstr,  "GOAL_TYPE_HARASS_CITY"));
						break;
					case GOAL_TYPE_PLANT_NUKE:
						AILOG(( wstr,  "GOAL_TYPE_PLANT_NUKE"));
						break;
					case GOAL_TYPE_MAKE_PARK:
						AILOG(( wstr,  "GOAL_TYPE_MAKE_PARK"));
						break;
					case GOAL_TYPE_UNDERGROUND_RAILWAY:
						AILOG(( wstr,  "GOAL_TYPE_UNDERGROUND_RAILWAY"));
						break;
					case GOAL_TYPE_REFORM_CITY:
						AILOG(( wstr,  "GOAL_TYPE_REFORM_CITY"));
						break;
					case GOAL_TYPE_SUE_FRANCHISE:
						AILOG(( wstr,  "GOAL_TYPE_SUE_FRANCHISE"));
						break;
					case GOAL_TYPE_CONVERT_CITY:
						AILOG(( wstr,  "GOAL_TYPE_CONVERT_CITY"));
						break;
                    case GOAL_TYPE_BIOTERROR_CITY:
						AILOG(( wstr,  "GOAL_TYPE_BIOTERROR_CITY"));
						break;
                    case GOAL_TYPE_NANOATTACK_CITY:
						AILOG(( wstr,  "GOAL_TYPE_NANOATTACK_CITY"));
						break;
                    case GOAL_TYPE_ESTABLISH_EMBASSY:
						AILOG(( wstr,  "GOAL_TYPE_ESTABLISH_EMBASSY"));
						break;
					case GOAL_TYPE_FRANCHISING:
						AILOG(( wstr,  "GOAL_TYPE_FRANCHISING"));
						break;
					case GOAL_TYPE_ASSASINATE_RULER:
						AILOG(( wstr,  "GOAL_TYPE_ASSASINATE_RULER"));
						break;
					case GOAL_TYPE_STEAL_TECHNOLOGY:
						AILOG(( wstr,  "GOAL_TYPE_STEAL_TECHNOLOGY"));
						break;
					case GOAL_TYPE_INJOIN:
						AILOG(( wstr,  "GOAL_TYPE_INJOIN"));
						break;
					case GOAL_TYPE_INCITE_REVOLUTION:
						AILOG(( wstr,  "GOAL_TYPE_REVOLUTION"));
						break;
					case GOAL_TYPE_CAUSE_UNHAPPINESS:
						AILOG(( wstr,  "GOAL_TYPE_CAUSE_UNHAPPINESS"));
						break;
					case GOAL_TYPE_RECOVER_PROBE:
						AILOG(( wstr,  "GOAL_TYPE_RECOVER_PROBE"));
						break;
					default:
						AILOG(( wstr,  "GOAL_CITY_TARGET??"));
						break;
				}
		}
	
}












void GoalCityTarget::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " Target: (%d, %d, %d)", m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













void GoalCityTarget::GetDebugName(char *str)
{
	switch (what_goal)
		{
			case GOAL_TYPE_NUKE_CITY:
				sprintf(str, "nukecity");
				break;
			case GOAL_TYPE_SEIGE:
				sprintf(str, "seige");
				break;
			case GOAL_TYPE_HARASS_CITY:
				sprintf(str, "harass");
				break;
			case GOAL_TYPE_PLANT_NUKE:
				sprintf(str, "plantnuke");
				break;
			case GOAL_TYPE_MAKE_PARK:
				sprintf(str, "makepark");
				break;
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
				sprintf(str, "ugrailway");
				break;
			case GOAL_TYPE_SUE_FRANCHISE:
				sprintf(str, "sue");
				break;
			case GOAL_TYPE_REFORM_CITY:
				sprintf(str, "reform");
				break	;
			case GOAL_TYPE_CONVERT_CITY:
				sprintf(str, "convert");
				break;
            case GOAL_TYPE_BIOTERROR_CITY:
				sprintf(str, "bioterror");
				break;
            case GOAL_TYPE_NANOATTACK_CITY:
				sprintf(str, "nanoattack");
				break;
			case GOAL_TYPE_ESTABLISH_EMBASSY:
				sprintf(str, "embassy");
				break;
			case GOAL_TYPE_FRANCHISING:
				sprintf(str, "raykroc");
				break;
			case GOAL_TYPE_ASSASINATE_RULER:
				sprintf(str, "oswald");
				break;
			case GOAL_TYPE_STEAL_TECHNOLOGY:
				sprintf(str, "steal");
				break;
			case GOAL_TYPE_INJOIN:
				sprintf(str, "injoin");
				break;
			case GOAL_TYPE_INCITE_REVOLUTION:
				sprintf(str, "incite");
				break;
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
				sprintf(str, "unhappiness");
				break;
			case GOAL_TYPE_RECOVER_PROBE:
				sprintf(str, "recover");
				break;				
			default:
				sprintf(str, "target?");
				break;
		}
}













Goal_Result GoalCityTarget::Execute_Task(AiMain *ai, Plan *the_plan)
{
#ifdef DBGAILOG
	#ifdef LOG_GOAL_ATTACK
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "GoalTargetCity::Execute_Task: Entering\n"));
	#endif
#endif DBGAILOG

	
	if (what_goal == GOAL_TYPE_SEIGE)
		{
			
			if (ai->m_i_am_dead) { 
				return GOAL_FAILED; 
			} else if (Get_Totally_Complete(ai)) {
				return GOAL_COMPLETE; 
			} else if (NULL == attacking_squad){
				return GOAL_FAILED; 
			} 

			
			
			sint32 agent_count = attacking_squad->my_agents.count;

			
			if (agent_count == 1) m_rallying = false;

			
			
			Goal_Result result;

			BOOL ok_to_rally = attacking_squad->Ok_To_Rally(ai); 
			if (m_rallying && ok_to_rally)
				{
					
					MapPointData dest_pos; 
					ForeignCity *the_bastard = GetTarget(ai);

					
					
					if (the_bastard == NULL)
						{
							m_rallying = FALSE;
							return GOAL_COMPLETE;
						}

					the_bastard->GetPos(dest_pos); 

					
					result = attacking_squad->Rally_Troops(ai, 
														   dest_pos, 
														   this, 
														   m_rallying);

					if (GOAL_IN_PROGRESS != result &&
						GOAL_COMPLETE != result) { 
						return result;
					}

					if (GOAL_COMPLETE == result)
						m_rallying = FALSE;
				} 

			
			if (m_rallying && ok_to_rally)
				{
#ifdef DBGAILOG
#ifdef LOG_GOAL_ATTACK
		if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			AILOG((wstr, "GoalTargetCity::still rallying-Exiting\n"));
		#endif
#endif DBGAILOG
					return GOAL_IN_PROGRESS;
				} 
			else if (ok_to_rally)
				{
#ifdef DBGAILOG
		        #ifdef LOG_GOAL_ATTACK
					if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
						AILOG((wstr, "GoalTargetCity::rallying complete-Exiting\n"));
				#endif
					; 
#endif DBGAILOG
				}
		} 
	
	
	return ArmyGoal::Execute_Task(ai,the_plan); 
}














void GoalCityTarget::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	float attack;						
	float defend;						
    sint32 defend_unit_count; 
    sint32 ranged_unit_count;
    float ranged; 
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
	
	
	ForeignCity *target_city = GetTarget(ai);
	float min_required_attack;
	float min_required_defense;
    float min_required_strength;
	double match_ratio;

	
	if (target_city == NULL)
		{
			min_required_attack = 1.0;
			min_required_defense = 0.0;
			return;
		}

	switch (what_goal)
		{
			case GOAL_TYPE_SEIGE:
        		
                ComputeEnemyStrengthAt(ai, *m_pos, attack, defend_unit_count, defend, 
                    ranged_unit_count, ranged);

                

				
				match_ratio = 
					AddMinMaxRandomRange(ai, 
										 ai->m_planner->the_AIP.min_defense_matching_force_ratio,
										 fz_min_force_matching_percent,
										 fz_max_force_matching_percent);

                
                min_required_strength = float(attack * match_ratio);
                m_min_needed_strength->SetDefense(min_required_strength);
                m_current_needed_strength->SetDefense(min_required_strength);

                

				
				match_ratio = 
					AddMinMaxRandomRange(ai, 
										 ai->m_planner->the_AIP.min_attack_matching_force_ratio,
										 fz_min_force_matching_percent,
										 fz_max_force_matching_percent);

                
                min_required_strength = float(defend * match_ratio);
                m_min_needed_strength->SetAttack(min_required_strength);
                m_current_needed_strength->SetAttack(min_required_strength);

                m_min_needed_strength->SetUnitCount(1); 
                m_current_needed_strength->SetUnitCount(1);

                m_min_needed_strength->SetRangeUnitCount(ranged_unit_count); 
                m_current_needed_strength->SetRangeUnitCount(ranged_unit_count); 

                m_min_needed_strength->SetRangeStrength(ranged); 
                m_current_needed_strength->SetRangeStrength(ranged);

				break;
			case GOAL_TYPE_NUKE_CITY:
				
				
			case GOAL_TYPE_PLANT_NUKE:
			case GOAL_TYPE_HARASS_CITY:
			case GOAL_TYPE_MAKE_PARK:
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
			case GOAL_TYPE_SUE_FRANCHISE:
			case GOAL_TYPE_REFORM_CITY:
			case GOAL_TYPE_CONVERT_CITY:
            case GOAL_TYPE_BIOTERROR_CITY:
            case GOAL_TYPE_NANOATTACK_CITY:
			case GOAL_TYPE_ESTABLISH_EMBASSY:
			case GOAL_TYPE_FRANCHISING:
			case GOAL_TYPE_ASSASINATE_RULER:
			case GOAL_TYPE_STEAL_TECHNOLOGY:
			case GOAL_TYPE_INJOIN:
			case GOAL_TYPE_INCITE_REVOLUTION:
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
			case GOAL_TYPE_RECOVER_PROBE:
				


				break;
			default:
				Assert(ASSERT_CITY_TARGET_INVALID);
		}

}















double GoalCityTarget::Compute_Matching_Value
(
	AiMain *ai,							
	squad_ptr the_squad				
)
{
	
	double utility;					
	double summed_utility = 0.0;	
	double avg_utility;				
	int unit_count;					
	ArmyAgent *tmpAgent;			
	BOOL same_squad_goal_combo;
	

	
	unit_count = the_squad->GetUnitCount();
	if (unit_count <= 0) 
		return BAD_UTILITY;

	
	sint32 found_agents = 0;
	for (int i = 0; i< unit_count; i++)
		{
			tmpAgent = (ArmyAgent *) the_squad->GetUnitByNumber(i);
			utility = Action_Bid(ai, tmpAgent);
			if (utility > BAD_UTILITY + 0.5)
			{
				summed_utility += utility;
				found_agents++;
			}

	} 

	
	if (found_agents > 0)
	{
		
		avg_utility = summed_utility / found_agents;
	}
	else
		
		return BAD_UTILITY;

	
	same_squad_goal_combo = (the_squad->last_goal_type is what_goal);

	
	if (same_squad_goal_combo)
	{
		
		avg_utility += ai->m_planner->the_AIP.persistence_modifier;

	} 

	
	return (avg_utility);
}

















int GoalCityTarget::Is_Unit_Appropriate
(
	AiMain *ai,							
	Agent * unit_in_question			
)
{

	
	AGENT_STATE agent_state;			
	AGENT_TYPE  agent_type;				
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	

	
	agent_type = unit_in_question->GetType();
	switch (agent_type)
	{
	case AGENT_TYPE_ARMY:
	{
		
		the_army = (ArmyAgent *) unit_in_question;

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_MARKED_SETTLE:
		case AGENT_STATE_MARKED_DEAD:            
		case AGENT_STATE_MARKED_GARRISON:
		case AGENT_STATE_AT_GOAL:
			return false;
			break;
		}

		
		if (!PretestBid(ai, the_army))
			return false;

        if (the_army->IsWounded()) return false; 

		
		
		
		
		if (the_army->IsInCity(ai, the_city) &&
			ArmyGoal::PretestUnitTypes(ai, the_army, GOAL_TYPE_DEFENSE))
			{
				sint32 units_in_city = (sint32) the_city->GetGarrisonCount();
				Assert(the_city);
				MapPointData pos;
				the_city->GetPos(pos);
				
				
				if (!(pos == *m_pos))
				{
					if (units_in_city - the_army->GetNUnits() < 
						ai->m_planner->the_AIP.num_city_defenders ||
						units_in_city - the_army->GetNUnits() <
						the_city->GetMinSlaveGarrison())
					return FALSE;
				}
			}

		return TRUE;
		break;
	} 
	break;
	case AGENT_TYPE_CITY:
	{
		
		the_city = (CityAgent *) unit_in_question;
        if (the_city->GetQueueLen() > 0)  
			return true;
		else
			return false;
		break;
	}
	default:
		return false;
	}
}























BOOL GoalCityTarget::WithinRange
(
    AiMain *ai, 
	Agent *agent,
	const SUB_TASK_TYPE & sub_task
)
{
	BOOL close_enough = false;
	ArmyAgent *army_agent = (ArmyAgent *)agent;

	if (agent->GetType() != AGENT_TYPE_ARMY)
		return false;

	
	if (sub_task == SUB_TASK_CARGO_TO_BOARD ||
		sub_task == SUB_TASK_TRANSPORT_TO_BOARD)
		return army_agent->AtEndOfPath(ai);

	
	if (GetType() == GOAL_TYPE_SEIGE ||
		GetType() == GOAL_TYPE_HARASS_CITY ||
		GetType() == GOAL_TYPE_NUKE_CITY)
		{
			
			close_enough = (army_agent->AtEndOfPath(ai));
		}
	else
		{
			
			
			close_enough = (army_agent->GetRemainingPathPoints(ai) <= 1);
		}

	return close_enough;
}























void GoalCityTarget::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army,
								   ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
	BOOL succeeded = true;

	

	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
		return;
	}

	
	if (sub_task != SUB_TASK_GOAL)
		return;

	switch (what_goal)
		{
			case GOAL_TYPE_NUKE_CITY:
				
			case GOAL_TYPE_SEIGE:
			case GOAL_TYPE_HARASS_CITY:
				
				break;
			case GOAL_TYPE_PLANT_NUKE:
				succeeded= the_army->ActionTryToPlantNuke(ai, *m_pos);
				
				break;
			case GOAL_TYPE_MAKE_PARK:
				succeeded= the_army->ActionTryToMakePark(ai, *m_pos);
				break;
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
				succeeded= the_army->ActionTryToUndergroundRailway(ai, *m_pos);
				
				break;
			case GOAL_TYPE_SUE_FRANCHISE:
				succeeded= the_army->ActionTryToSueFranchise(ai, *m_pos);
				break;
			case GOAL_TYPE_REFORM_CITY:
				succeeded= the_army->ActionTryToReformCity(ai, *m_pos);
				break;
			case GOAL_TYPE_CONVERT_CITY:
				succeeded= the_army->ActionTryToConvert(ai, *m_pos);
				break;
            case GOAL_TYPE_BIOTERROR_CITY:
                succeeded = the_army->ActionTryToBioterrorCity(ai, *m_pos); 
                break; 
            case GOAL_TYPE_NANOATTACK_CITY:
                succeeded = the_army->ActionTryToNanoattackCity(ai, *m_pos); 
                break; 
			case GOAL_TYPE_ESTABLISH_EMBASSY:
				succeeded= the_army->ActionTryToEstablishEmbassy(ai, *m_pos);
				break;
			case GOAL_TYPE_FRANCHISING:
				succeeded= the_army->ActionTryToCreateFranchise(ai, *m_pos);
				break;
			case GOAL_TYPE_ASSASINATE_RULER:
				succeeded= the_army->ActionTryToAssasinateRuler(ai, *m_pos);
				
				break;
			case GOAL_TYPE_STEAL_TECHNOLOGY:
				succeeded= the_army->ActionTryToStealTechnology(ai, *m_pos);
				
				break;
			case GOAL_TYPE_INJOIN:
				succeeded= the_army->ActionTryToInjoin(ai, *m_pos);
				
				break;
			case GOAL_TYPE_INCITE_REVOLUTION:
				succeeded= the_army->ActionTryToInciteRevolution(ai, *m_pos);
				
				break;
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
				succeeded= the_army->ActionTryToCauseUnhappiness(ai, *m_pos);
				
				break;
			case GOAL_TYPE_RECOVER_PROBE:
				
				succeeded = TRUE;
				break;
			default:
				Assert(ASSERT_CITY_TARGET_INVALID);
		}

	
	if (the_army->GetState() != AGENT_STATE_MARKED_DEAD)
		{
			the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
		}
	Set_Totally_Complete(succeeded); 
}













BOOL GoalCityTarget::UnitTypeCapable(AiMain *ai, sint32 unit_type)
{
	BOOL ASSERT_CITY_TARGET_INVALID = 0;

	
	switch (what_goal)
	{
	case GOAL_TYPE_NUKE_CITY:
		
		
		return ai->m_unitDB->IsNuclearAttack(unit_type);
		break;
	case GOAL_TYPE_SEIGE:
	case GOAL_TYPE_HARASS_CITY:
		return (ai->m_unitDB->GetAttack(unit_type) > 0) &&
				(!ai->m_unitDB->IsCantCaptureCity(unit_type));
		break;
		
	case GOAL_TYPE_REFORM_CITY:
		return (ai->m_unitDB->GetAttack(unit_type) > 0);
	 
	case GOAL_TYPE_PLANT_NUKE:
		
		return ai->m_unitDB->CanPlantNuke(unit_type);
		break;
	case GOAL_TYPE_MAKE_PARK:
		
		return ai->m_unitDB->CanCreateParks(unit_type);
		break;
	case GOAL_TYPE_UNDERGROUND_RAILWAY:
		
		return ai->m_unitDB->CanConductUndergroundRailway(unit_type);
		break;
	case GOAL_TYPE_CONVERT_CITY:
		
		return ai->m_unitDB->CanConvertCity(unit_type);
		break;

    case GOAL_TYPE_BIOTERROR_CITY:
        return ai->m_unitDB->CanBioterrorCity(unit_type); 

    case GOAL_TYPE_NANOATTACK_CITY:
        return ai->m_unitDB->CanNanoattackCity(unit_type); 

	case GOAL_TYPE_ESTABLISH_EMBASSY:
		
		return ai->m_unitDB->CanEstablishEmbassy(unit_type);
		break;
	case GOAL_TYPE_FRANCHISING:
		
		return ai->m_unitDB->CanCreateFranchise(unit_type);
		break;
	case GOAL_TYPE_ASSASINATE_RULER:
		
		return ai->m_unitDB->CanAssasinateRuler(unit_type);
		break;
	case GOAL_TYPE_STEAL_TECHNOLOGY:
		
		return ai->m_unitDB->CanStealTechnology(unit_type);
		break;
	case GOAL_TYPE_INJOIN:
		
		return ai->m_unitDB->CanInjoin(unit_type);
		break;
	case GOAL_TYPE_INCITE_REVOLUTION:
		
		
		
		return (ai->m_unitDB->CanInciteRevolution(unit_type) &&
				!ai->m_unitDB->CanConductUndergroundRailway(unit_type));
		break;
	case GOAL_TYPE_CAUSE_UNHAPPINESS:
		
		return ai->m_unitDB->CanCauseUnhappiness(unit_type);
		break;
	case GOAL_TYPE_SUE_FRANCHISE:
		
		return ai->m_unitDB->CanSue(unit_type);
		break;
	case GOAL_TYPE_RECOVER_PROBE:
		return ai->m_unitDB->IsWormholeProbe(unit_type);
		break;
	default:
		Assert(ASSERT_CITY_TARGET_INVALID);
	}
	return FALSE;
}








BOOL GoalCityTarget::PretestBid(AiMain *ai, ArmyAgent *agent)
{
	
	BOOL can_doit = FALSE;
	BOOL cargo_can_enter = FALSE;
	BOOL cargo_can_nuke = FALSE;
	sint32 empty_slots = 0;
	sint32 full_slots = 0;
	int i;
	sint32 unit_type;

	if (agent->PretestBid(ai, GetType(), *m_pos) <= BAD_UTILITY)
			return FALSE;

	
	if (GetType() == GOAL_TYPE_RECOVER_PROBE)
	{
		
		
		
		
		return agent->HasProbedWormhole();
	}
	
    if (!agent->PretestFuelToTarget(ai, *m_pos)) { 
        return false; 
    }

    BOOL is_same_cont;
    BOOL is_space_dest;
    if (!agent->PretestContinentReachable(ai, *m_pos, 
										  is_same_cont, is_space_dest)) { 
        return false; 
    } 

    if (agent->GetArmyContainsSettler()) {
        return false; 
    }

	
	if (agent->GetNumCargo(ai, full_slots, empty_slots) == TRUE)
		{ 
			for (i=0; i < full_slots; i++)
				{
					unit_type = agent->GetCargoUnitTypeByNumber(ai, i);
					
					
					
					
					
		
					
					cargo_can_nuke = cargo_can_nuke || 
						ai->m_unitDB->IsNuclearAttack(unit_type);

					can_doit = can_doit ||
						UnitTypeCapable(ai, unit_type);
				} 
		}

	
    for (i=0; i < agent->GetUnitCount(); i++)
		{ 
			unit_type = agent->GetUnitTypeByNumber(i); 

			
			
			
			
			
			

			
			if ((what_goal != GOAL_TYPE_NUKE_CITY) &&
				ai->m_unitDB->IsNuclearAttack(unit_type) &&
				!cargo_can_nuke)
				return false;

			
			if (ai->m_unitDB->IsSettler(unit_type))
				return false;

			can_doit = can_doit ||
			   UnitTypeCapable(ai, unit_type);
		} 

	return can_doit;
}













double GoalCityTarget::Action_Bid(AiMain *ai, Agent *agent)
{
	ArmyAgent *army_agent = (ArmyAgent *)agent;
	CityAgent *city_agent = (CityAgent *)agent;
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
	
	
	if (agent->GetType() == AGENT_TYPE_CITY)
		
		return city_agent->FindBestBuildUtility(ai,this,army_agent);
	
   	
	
	
	if (!PretestBid(ai, army_agent))
		return BAD_UTILITY;
	
	
	if (army_agent->GetState() == AGENT_STATE_UNBUILT) 
	{ 
        if (army_agent->GetHomeCityAgent()->GetQueueLen() > 0)  
            return BAD_UTILITY; 
    }
	
	
	double move_point_cost;
    sint32 rounds; 
    double build_time;
    if (army_agent->BidDistance(ai, FALSE, *m_pos, move_point_cost, rounds, build_time) < 0.0) { 
        return BAD_UTILITY; 
    }

	
	if (ai->m_my_player_id == PLAYER_INDEX_VANDALS &&
		rounds > ai->m_planner->the_AIP.wander_rounds_from_target)
		return BAD_UTILITY;

	
	double high_movement_weight;
	double high_defense_weight;
	double high_attack_weight;
	double bonuses = 0.0;
	
	
	switch (what_goal)
		{
			case GOAL_TYPE_SEIGE:
			case GOAL_TYPE_REFORM_CITY:
				
				high_movement_weight = 0.0;
				high_defense_weight = 0.5;
				high_attack_weight = 0.5;
				break;
			case GOAL_TYPE_NUKE_CITY:
			case GOAL_TYPE_HARASS_CITY:
				
				high_movement_weight = 0.5;
				high_defense_weight = 0.0;
				high_attack_weight = 0.5;
				break;
			case GOAL_TYPE_PLANT_NUKE:
			case GOAL_TYPE_MAKE_PARK:
				
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
				
			case GOAL_TYPE_ESTABLISH_EMBASSY:
				
			case GOAL_TYPE_CONVERT_CITY:
            case GOAL_TYPE_BIOTERROR_CITY:
            case GOAL_TYPE_NANOATTACK_CITY:

			case GOAL_TYPE_FRANCHISING:
				
			case GOAL_TYPE_INCITE_REVOLUTION:
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
			case GOAL_TYPE_ASSASINATE_RULER:
				
			case GOAL_TYPE_STEAL_TECHNOLOGY:
				
			case GOAL_TYPE_INJOIN:
			case GOAL_TYPE_SUE_FRANCHISE:
			case GOAL_TYPE_RECOVER_PROBE:
				
				
				high_movement_weight = 0.25;
				high_defense_weight = 0.75;
				high_attack_weight = 0.0;
				break;
			default:
				Assert(ASSERT_CITY_TARGET_INVALID);
		}
	
	
	sint32 best_defense;
	sint32 my_defense  = army_agent->GetBestDefense(ai);
	if (ai->m_science_agent->GetBestLandDefense(best_defense))
		{
			bonuses += (my_defense / best_defense)  
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* high_defense_weight;
		}
	else
		; 

	
	sint32 best_attack;
	sint32 my_attack  = army_agent->GetBestAttack(ai);
	if (ai->m_science_agent->GetBestLandAttack(best_attack))
		{
			bonuses += (my_attack / best_attack)  
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* high_attack_weight;
		}
	else
		; 
	
	
	sint32 best_move;
	double my_move = army_agent->GetBestMove(ai);
	if (ai->m_science_agent->GetBestLandMove(best_move))
		{
			bonuses += (my_move / best_move) 
				* ai->m_planner->the_AIP.max_misc_bid_bonus
				* high_movement_weight;
		}
	else
		; 

	
	if (what_goal == GOAL_TYPE_RECOVER_PROBE)
		{
			BOOL is_unknown_id;
			sint32 net_production = 
				ai->m_player->CityGetNetProduction(
					m_targetId.GetVal(), 
					&is_unknown_id);
			Assert(is_unknown_id == FALSE);
			
			
			bonuses += net_production *
				ai->m_planner->the_AIP.max_misc_bid_bonus;
		}

	
	double time_term;
    MapPointData dest_pos; 
    army_agent->GetPos(ai, dest_pos); 
	double tile_count = ai->m_map->WrappedDistance(*m_pos, dest_pos);
	time_term = ( rounds * rounds 
		* ai->m_planner->the_AIP.distance_from_unit_priority_modifier )
		- tile_count;

    
	return bonuses + time_term + Compute_Raw_Priority(ai); 
}	




	











double GoalCityTarget::Compute_Raw_Priority(AiMain *ai)
{
#ifdef _DEBUG
    MapPointData *size; 
    size = ai->m_map->GetSize(); 
    Assert(m_pos); 
    Assert(m_XYpos); 
    Assert(0 <= m_XYpos->x); 
    Assert(m_XYpos->x < size->x*2); 
    Assert(0 <= m_XYpos->y); 
    Assert(m_XYpos->y < size->y); 
    Assert(0 <= m_XYpos->z); 
    Assert(m_XYpos->z < size->z); 

    Assert(0 <= m_pos->x); 
    Assert(m_pos->x < size->x); 
    Assert(0 <= m_pos->y); 
    Assert(m_pos->y < size->y); 
    Assert(0 <= m_pos->z); 
    Assert(m_pos->z < size->z); 
#endif
	BOOL ASSERT_CITY_TARGET_INVALID = 0;
    double watchful_city_bonus = 2000.0; 
	

	
	
	switch (what_goal)
		{
			case GOAL_TYPE_SEIGE:
				raw_priority = ai->m_planner->the_AIP.seige_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 

				
				if (m_min_needed_strength->GetDefense() <= 0.0)
					
					raw_priority += 
						ai->m_planner->the_AIP.defenseless_city_raw_bonus;
				break;
			case GOAL_TYPE_HARASS_CITY:
				raw_priority = ai->m_planner->the_AIP.harass_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
				break;
			case GOAL_TYPE_NUKE_CITY:
				raw_priority = ai->m_planner->the_AIP.nuke_city_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					 1, 
					-1, 
					 1); 
				break;
			case GOAL_TYPE_PLANT_NUKE:
				raw_priority = ai->m_planner->the_AIP.plant_nuke_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					 1, 
					-1, 
					 1); 
				break;
			case GOAL_TYPE_MAKE_PARK:
				raw_priority = ai->m_planner->the_AIP.create_park_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
				break;
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
				raw_priority = ai->m_planner->the_AIP.underground_railway_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_REFORM_CITY:
				raw_priority = ai->m_planner->the_AIP.reform_city_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
				break;
			case GOAL_TYPE_SUE_FRANCHISE:
				raw_priority = ai->m_planner->the_AIP.sue_franchise_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus;     
				break;
			case GOAL_TYPE_CONVERT_CITY:
				raw_priority = ai->m_planner->the_AIP.convert_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
            case GOAL_TYPE_BIOTERROR_CITY:
				raw_priority = ai->m_planner->the_AIP.bioterror_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
            case GOAL_TYPE_NANOATTACK_CITY:
				raw_priority = ai->m_planner->the_AIP.nanoattack_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_ESTABLISH_EMBASSY:
				raw_priority = ai->m_planner->the_AIP.establish_embassy_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
				break;
			case GOAL_TYPE_FRANCHISING:
				raw_priority = ai->m_planner->the_AIP.franchising_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_ASSASINATE_RULER:
				raw_priority = ai->m_planner->the_AIP.assasinate_ruler_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_STEAL_TECHNOLOGY:
				raw_priority = ai->m_planner->the_AIP.steal_technology_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_INJOIN:
				raw_priority = ai->m_planner->the_AIP.injoin_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_INCITE_REVOLUTION:
				raw_priority = ai->m_planner->the_AIP.incite_revolution_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
				raw_priority = ai->m_planner->the_AIP.cause_unhappiness_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					 1, 
					-1, 
					 1, 
					 1, 
					-1, 
					-1, 
					 1); 
                if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= watchful_city_bonus; 
				break;
			case GOAL_TYPE_RECOVER_PROBE:
				raw_priority = ai->m_planner->the_AIP.probe_wormhole_priority;
				raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
					-1, 
					-1, 
					 1, 
					 1, 
					-1, 
					 1, 
					 1); 
				break;
			default:
				Assert(ASSERT_CITY_TARGET_INVALID);
		}

	
	return raw_priority;
}












Goal_Result GoalCityTarget::BuildTaskSolution(AiMain *ai, CityAgent *the_city,
    Plan *the_plan)
{ 

	
	
	
	
    ArmyAgent *unbuilt_army; 
	the_city->FindBestBuildUtility(ai, this, unbuilt_army);

    double utility = the_plan->GetMatchingValue(); 

	if (unbuilt_army) {
        sint32 unit_idx = unbuilt_army->GetUnbuiltType(); 
		
		if (the_city->PretestContructUnit(ai, unit_idx))
		{
			the_city->EnqueueArmy(ai, unit_idx, utility);

			
			the_city->current_goal_type = what_goal;
	        return GOAL_IN_PROGRESS;
		}
		return GOAL_INAPPROPRIATE;
    } else { 
    	
		return GOAL_INAPPROPRIATE;
	}
}





