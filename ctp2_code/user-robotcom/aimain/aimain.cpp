

















#include "Aimain.h"



AiMain::AiMain()
{

    
    
    
    
    
    
    

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

	
	m_gs		     = NULL; 
    m_player	     = NULL;
    m_world		     = NULL;
    m_robot_astar    = NULL;
    m_unitDB	     = NULL;
    m_buildingDB     = NULL;
	m_population     = NULL;
	m_science	     = NULL;
	m_governmentDB   = NULL;
	m_wonderDB	     = NULL;
    m_installationDB = NULL;
    m_terrainDB	     = NULL;
	m_rand	         = NULL;
	m_endGameDB      = NULL;

	
    m_playerIndex    = -1;
    m_isDead         = FALSE; 
    m_width          = -1;
    m_height         = -1;
    m_levels		 = -1;
    m_nStartPlayers  = -1;
    m_nPlayers       = -1;

	_RPTF0(_CRT_WARN, "Constructor.\n");
}

AiMain::~AiMain()
{
	_RPTF0(_CRT_WARN, "Destructor.\n");
    Cleanup(); 
}


BOOL AiMain::Init(IC3GameState *gs, uint32 goal_time_slice,
    uint32 max_time_slice, uint32 max_total_time)
{
	_RPTF0(_CRT_WARN, "Initialize.\n");

	
	m_gs             = gs; 
    m_player         = gs->GetPlayer(); 
    m_world          = gs->GetWorld(); 
    m_robot_astar    = gs->GetAstar(); 
    m_unitDB         = gs->GetUnitDB(); 
    m_buildingDB     = gs->GetBuildingDB(); 
	m_population     = gs->GetPopulation() ;
	m_science        = gs->GetScience() ;
	m_governmentDB   = gs->GetGovernmentDB() ;
	m_wonderDB       = gs->GetWonderDB() ;
    m_installationDB = gs->GetInstallationDB(); 
    m_terrainDB      = gs->GetTerrainDB(); 
	m_rand           = gs->GetRand();
	m_endGameDB      = gs->GetEndGameDB(); 

	
    m_playerIndex    = gs->GetPlayerIndex(); 
    m_isDead         = FALSE; 
    m_width          = m_world->GetXWidth();
    m_height         = m_world->GetYHeight(); 
    m_levels         = m_world->GetZHeight(); 
    m_nStartPlayers  = gs->GetNumPlayersAtStart(); 
    m_nPlayers       = gs->GetNumPlayers();

	return TRUE;
}

BOOL AiMain::Cleanup()
{
	_RPTF0(_CRT_WARN, "Cleanup.\n");

	
	return TRUE;
}

void AiMain::Save(IC3CivArchive *archive)
{
	_RPTF0(_CRT_WARN, "Save.\n");

	
    

	
	
    
	
 	
}

BOOL AiMain::Load(IC3GameState *gs, IC3CivArchive *archive)
{
	_RPTF0(_CRT_WARN, "Load.\n");

	
	

	Init(gs,0,0,0);

	
	
    
	
 	
	return TRUE;
}

BOOL AiMain::BeginTurn(uint32 end_frame_by, BOOL &my_turn_is_over )
{
	_RPTF0(_CRT_WARN, "Begin Turn.\n");

	
	
	
	
	my_turn_is_over = TRUE;

	
	
	
	
	
	
	

	MapPointData pos;					
	MapPointData new_pos;				
	BOOL x_wrap;						
	BOOL y_wrap;						
    sint32 max_x = m_world->GetXWidth();  
    sint32 max_y = m_world->GetYHeight(); 
    sint32 max_z = m_world->GetZHeight(); 
	sint16 pos_delta;					
	sint32 owner;						

	uint32 army_id;						
	sint32 unit_type;					
	sint32 num_units;					
	uint32 city_id;						
	BOOL unit_visible;					
	BOOL city_visible;					
	BOOL mine_visible;					
    BOOL can_be_expelled;				
	sint32 player_index;				
	sint32 settler_type = 4;			
	sint32 legion_type = 10;			

    BOOL is_unknown_id;					
    BOOL i_died;						
	BOOL move_violated_zoc;				
	BOOL revealed_foreign_units;		
	BOOL revealed_unexplored;			
	BOOL is_transported;				
	BOOL out_of_fuel;					
	BOOL did_move;						

	

	x_wrap = m_world->IsXwrap();
	y_wrap = m_world->IsYwrap();

	
	player_index = m_gs->GetPlayerIndex();

	
	for (pos.z = 0; pos.z < max_z; pos.z++)
		for (pos.x = 0; pos.x < max_x; pos.x++)
			for (pos.y = 0; pos.y < max_y; pos.y++)
			{
				m_world->GetCellContents( 
					&pos, 
					&owner, 
					&army_id,
					&unit_type,
					&num_units,
					&city_id,
					&unit_visible,
					&city_visible,
					&mine_visible, 
                    &can_be_expelled);

				
				if (city_id != 0 && owner == player_index)
				{
					
					if (m_player->CityBuildQueueLen(city_id, &is_unknown_id) == 0)
					{
						
						if (m_player->CityCanBuildUnit(city_id, settler_type))
						{
							m_player->CityEnqueueBuildItem(city_id, 
								&is_unknown_id,
								k_GAME_OBJ_TYPE_UNIT, 
								settler_type);

						}
						
						else
						{
							m_player->CityEnqueueBuildItem(city_id, 
								&is_unknown_id,
								k_GAME_OBJ_TYPE_UNIT, 
								legion_type);
						}
					}

				}
				
				
				if (army_id != 0 && owner == player_index)
				{

					
					if (unit_type != settler_type || 
						!m_player->ArmySettle(army_id, &is_unknown_id))
					{
						do {
							new_pos = pos;
							pos_delta = m_rand->Next(3) - 1;
							new_pos.x += pos_delta;
							pos_delta = m_rand->Next(3) - 1;
							new_pos.y += pos_delta;
						
							
							m_player->ArmyMoveTo(army_id, &is_unknown_id, 
								&new_pos, 
								&did_move, 
								&i_died, 
								&move_violated_zoc, 
								&revealed_foreign_units, 
								&revealed_unexplored, 
								&is_transported, 
								&out_of_fuel);
						} while (did_move);
					}
				}
			}

	return TRUE;
}

BOOL AiMain::ExecuteTimeSlice(uint32 end_frame_by, 
							  BOOL &my_turn_is_over, 
							  char debug_str[80])
{
	_RPTF0(_CRT_WARN, "Execute Time Slice.\n");

	
    

	

	
	my_turn_is_over = TRUE;

	if (end_frame_by - GetTickCount() <= 0)
	    my_turn_is_over = TRUE;

    return TRUE;
}






void AiMain::RegisterAddArmy (uint32 army_id,
						 CAUSE_NEW_ARMY cause,
						 uint32 city_id,
						 sint32 num_units,
						 sint32 unit_type[k_MAX_ARMY_SIZE],
						 sint32 unit_hp[k_MAX_ARMY_SIZE])
{
	_RPTF0(_CRT_WARN, "Register Add Army.\n");

	Assert(num_units <= k_MAX_ARMY_SIZE);

	

	
	

	

	switch(cause)
		{
			case CAUSE_NEW_ARMY_UNKNOWN:         
				
				break;
			case CAUSE_NEW_ARMY_TRADER:
				
				break;
			case CAUSE_NEW_ARMY_INITIAL:
			case CAUSE_NEW_ARMY_TRANSPORTED:
			case CAUSE_NEW_ARMY_GROUPING: 
			case CAUSE_NEW_ARMY_UNGROUPING:
			case CAUSE_NEW_ARMY_BUILT:
			case CAUSE_NEW_ARMY_PARADROP:
			case CAUSE_NEW_ARMY_SPACE_LAUNCH:
			case CAUSE_NEW_ARMY_UPRISING:
			case CAUSE_NEW_ARMY_GOODY_HUT:
			case CAUSE_NEW_ARMY_RUSTLE:
			case CAUSE_NEW_ARMY_DIPLOMACY:
			case CAUSE_NEW_ARMY_NETWORK:
			case CAUSE_NEW_ARMY_CARGO_POD:
			case CAUSE_NEW_ARMY_EXPELLED:
			case CAUSE_NEW_ARMY_ENTERED_WORMHOLE:
			case CAUSE_NEW_ARMY_CHEAT:
			case CAUSE_NEW_ARMY_AI_TRANSPORT_CHEAT:
			case CAUSE_NEW_ARMY_DISBANDED_CITY:
			case CAUSE_NEW_ARMY_SCENARIO:
				
				break;
		}
}

void AiMain::RegisterMyRemoveArmy (uint32 army_id, 
							  CAUSE_REMOVE_ARMY cause)
{
	_RPTF0(_CRT_WARN, "Register My Remove Army.\n");

	
	switch (cause)
	{
	case CAUSE_REMOVE_ARMY_UNKNOWN:
		break;
		
	case CAUSE_REMOVE_ARMY_SETTLE: 
		
		break;
		
	case CAUSE_REMOVE_ARMY_FLOOD:
	case CAUSE_REMOVE_ARMY_PARKRANGER:
	case CAUSE_REMOVE_ARMY_NUKE:
	case CAUSE_REMOVE_ARMY_BOMBARD:
	case CAUSE_REMOVE_ARMY_ENSLAVED:
	case CAUSE_REMOVE_ARMY_SUE: 
		
	case CAUSE_REMOVE_ARMY_POLLUTION:
	case CAUSE_REMOVE_ARMY_UPRISING:
		
	case CAUSE_REMOVE_ARMY_SLAVE_UPRISING:
		
	case CAUSE_REMOVE_ARMY_ATTACKED:
		
		break;
		
	case CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT:
		
	case CAUSE_REMOVE_ARMY_DIPLOMACY:
	case CAUSE_REMOVE_ARMY_CHEAT:
	case CAUSE_REMOVE_ARMY_TRANSPORTED:
	case CAUSE_REMOVE_ARMY_GROUPING:
	case CAUSE_REMOVE_ARMY_UNGROUPING:
	case CAUSE_REMOVE_ARMY_RAIL_LAUNCH:
	case CAUSE_REMOVE_ARMY_PARADROP:
	case CAUSE_REMOVE_ARMY_STARVATION: 
		
		break;
		
	case CAUSE_REMOVE_ARMY_DIED_IN_ATTACK:
	case CAUSE_REMOVE_ARMY_DIED_IN_ATTACK_ON_TOP:
	case CAUSE_REMOVE_ARMY_DIED_IN_SPYING:
	case CAUSE_REMOVE_ARMY_DIED_IN_SLAVERAID:
	case CAUSE_REMOVE_ARMY_DIED_IN_UNDERGROUND_RR_RAID:
	case CAUSE_REMOVE_ARMY_DIED_IN_CONVERT_RAID:
	case CAUSE_REMOVE_ARMY_DIED_IN_REFORMATION:
	case CAUSE_REMOVE_ARMY_COUNTERBOMBARD:
	case CAUSE_REMOVE_ARMY_OUTOFFUEL:
	case CAUSE_REMOVE_ARMY_REENTRY:
	case CAUSE_REMOVE_ARMY_GOVERNMENT_CHANGE:
	case CAUSE_REMOVE_ARMY_TOE:
	case CAUSE_REMOVE_ARMY_RUSTLE:
	case CAUSE_REMOVE_ARMY_LAND_MINES:
	case CAUSE_REMOVE_ARMY_TRANSPORT_DIED:
	case CAUSE_REMOVE_ARMY_DIDNT_NEED_IT_AFTER_ALL:
	case CAUSE_REMOVE_ARMY_DISBANDED:
		
		break;
		
	case CAUSE_REMOVE_ARMY_PARADROP_COMPLETE:
	case CAUSE_REMOVE_ARMY_NUKES_ELIMINATED:
	case CAUSE_REMOVE_ARMY_EMANCIPATION:
	case CAUSE_REMOVE_ARMY_ILLEGAL_CELL:
	case CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES:
		
	case CAUSE_REMOVE_ARMY_SCENARIO:
	case CAUSE_REMOVE_ARMY_PROBE_RECOVERED:
		
		break;
	}
}

void AiMain::RegisterHisRemoveArmy (PLAYER_INDEX owner, 
							   uint32 army_id)
{
	_RPTF0(_CRT_WARN, "Register His Remove Army.\n");

	
}

void AiMain::RegisterMyAddCity (uint32 city_id, 
						   CAUSE_NEW_CITY cause, 
						   MapPointData *pos)
{
	_RPTF0(_CRT_WARN, "Register My Add City.\n");

	
	
	switch (cause)
		{
			case CAUSE_NEW_CITY_UNKNOWN: 
				
				break;
			case CAUSE_NEW_CITY_SETTLE:
			case CAUSE_NEW_CITY_GOODY_HUT:
			case CAUSE_NEW_CITY_REVOLT: 
			case CAUSE_NEW_CITY_CONQUEST: 
			case CAUSE_NEW_CITY_SPACE_LADDER:
			case CAUSE_NEW_CITY_CHEAT:
			case CAUSE_NEW_CITY_DIPLOMACY:
			case CAUSE_NEW_CITY_SLAVE_UPRISING:
			case CAUSE_NEW_CITY_UPRISING:
			case CAUSE_NEW_CITY_INITIAL:
			case CAUSE_NEW_CITY_SCENARIO:
				break;
		}
}

void AiMain::RegisterHisAddCity (uint32 id, 
							CAUSE_NEW_CITY cause, 
							MapPointData *pos)
{
	_RPTF0(_CRT_WARN, "Register His Add City.\n");

	
	
	
	

	
}

void AiMain::RegisterMyRemoveCity (uint32 city_id, 
							  CAUSE_REMOVE_CITY cause)
{
	_RPTF0(_CRT_WARN, "Register My Remove City.\n");

	

	switch (cause)
		{
			case CAUSE_REMOVE_CITY_UNKNOWN:
			case CAUSE_REMOVE_CITY_FLOOD:
			case CAUSE_REMOVE_CITY_HAPPINESS_REVOLT:
			case CAUSE_REMOVE_CITY_SLAVE_UPRISING:
			case CAUSE_REMOVE_CITY_ATTACK:
			case CAUSE_REMOVE_CITY_DIPLOMACY:
			case CAUSE_REMOVE_CITY_CHEAT:
			case CAUSE_REMOVE_CITY_AI_ENTITY_REVOLT:
				break;
		}
}

void AiMain::RegisterHisRemoveCity(PLAYER_INDEX owner, 
							  uint32 city_id)
{
	_RPTF0(_CRT_WARN, "Register His Remove City.\n");
	
}

void AiMain::RegisterCityDiedAt(MapPointData *pos)
{
	_RPTF0(_CRT_WARN, "Register City Died At.\n");
	
	
}

void AiMain::RegisterAdjustDeathDifferential (double adjustment, 
										 MapPointData &pos) 
{
	_RPTF0(_CRT_WARN, "Register Adjust Death Differential.\n");
	
}

void AiMain::RegisterClearCargo(uint32 army_id)
{
	_RPTF0(_CRT_WARN, "Register Clear Cargo.\n");
	
	
}

void AiMain::RegisterInsertCargo (uint32 army_id, 
							 sint32 unit_type, 
							 sint32 hp)
{
	_RPTF0(_CRT_WARN, "Register Insert Cargo.\n");
	
}

void AiMain::RegisterUnloadCargo (uint32 army_id, 
							 sint32 unit_type, 
							 sint32 hp, 
							 CAUSE_REMOVE_ARMY cause)
{
	_RPTF0(_CRT_WARN, "Register Unload Cargo.\n");
	
}

void AiMain::RegisterDeadUnit (uint32 army_id, 
						  sint32 unit_type)
{
	_RPTF0(_CRT_WARN, "Register Dead Unit.\n");
	
	
}

void AiMain::RegisterArmyWasGrouped (uint32 army_id, 
								sint32 num_units, 
								sint32 unit_type[k_MAX_ARMY_SIZE], 
								sint32 unit_hp[k_MAX_ARMY_SIZE])
{
	_RPTF0(_CRT_WARN, "Register Army Was Grouped.\n");
	
}	

void AiMain::RegisterYourArmyWasMoved(uint32 army_id, 
								 MapPointData *new_pos)
{
	_RPTF0(_CRT_WARN, "Register Your Army Was Moved.\n");
	
}

void AiMain::RegisterHisArmyTeleported (PLAYER_INDEX his_owner, 
								   uint32 army_id, 
								   MapPointData *new_pos)
{
	_RPTF0(_CRT_WARN, "Register His Army Teleported.\n");
	
}

void AiMain::RegisterCreateBuilding (uint32 city_id, 
								sint32 building_type)
{
	_RPTF0(_CRT_WARN, "Register Create Building.\n");
	
}

void AiMain::RegisterLostBuilding (uint32 city_id, 
							  sint32 building_type)
{
	_RPTF0(_CRT_WARN, "Register Lost Building.\n");
	
}

void AiMain::RegisterNewGovernment (uint32 city_id, 
							   sint32 government_type)
{
	_RPTF0(_CRT_WARN, "Register New Government.\n");
	
	
}

void AiMain::RegisterNewWGF (sint32 wages, 
						sint32 gold, 
						sint32 food)
{
	_RPTF0(_CRT_WARN, "RegisterNewWGF.\n");
	
	
}

void AiMain::RegisterNewCapitolBuilding (uint32 city_id)
{
	_RPTF0(_CRT_WARN, "Register New Capitol Building.\n");
	
}

void AiMain::RegisterCreateWonder (uint32 city_id, 
							  sint32 wonder_type, 
							  PLAYER_INDEX owner)
{
	_RPTF0(_CRT_WARN, "Register Create Wonder.\n");
	
	
}

void AiMain::RegisterLearnedScience(sint32 idx_sci, CAUSE_SCI cause)
{
	_RPTF0(_CRT_WARN, "Register Learned Science.\n");
	
}

void AiMain::RegisterNewPlayer (PLAYER_INDEX player_id, 
						   CAUSE_NEW_PLAYER cause, 
						   PLAYER_INDEX old_owner)
{
	_RPTF0(_CRT_WARN, "Register New Player.\n");
	
	

	
	
	
	
	

	
	
}

void AiMain::RegisterDeadPlayer (PLAYER_INDEX player_id)
{
	_RPTF0(_CRT_WARN, "Register Dead Player.\n");
	
}

void AiMain::RegisterAddInstallation(MapPointData &pos, 
								sint32 inst_type)
{
	_RPTF0(_CRT_WARN, "Register Add Installation.\n");
	
	
}

void AiMain::RegisterRemoveInstallation(MapPointData &pos, 
								   sint32 inst_type)
{
	_RPTF0(_CRT_WARN, "Register Remove Installation.\n");
	
	
}
	
void AiMain::RegisterCityAttack(uint32 my_city_id, 
						   PLAYER_INDEX his_owner, 
						   uint32 his_unit_id, 
						   UNIT_ORDER_TYPE attack_type)
{
	_RPTF0(_CRT_WARN, "Register City Attack.\n");

	switch (attack_type)
		{
			
			
			case UNIT_ORDER_BOMBARD:
            case UNIT_ORDER_PLANT_NUKE:
			case UNIT_ORDER_CREATE_PARK:
			case UNIT_ORDER_SUE:
				break;
			case UNIT_ORDER_NULLIFY_WALLS:
			case UNIT_ORDER_INVESTIGATE_CITY:
			case UNIT_ORDER_STEAL_TECHNOLOGY:
			case UNIT_ORDER_INCITE_REVOLUTION:
			case UNIT_ORDER_ASSASSINATE:
			case UNIT_ORDER_INVESTIGATE_READINESS:
			case UNIT_ORDER_FRANCHISE:
			case UNIT_ORDER_CAUSE_UNHAPPINESS:
			case UNIT_ORDER_SLAVE_RAID:
			case UNIT_ORDER_UNDERGROUND_RAILWAY:
			case UNIT_ORDER_INCITE_UPRISING:
			case UNIT_ORDER_BIO_INFECT:
			case UNIT_ORDER_NANO_INFECT:
			case UNIT_ORDER_CONVERT:
			case UNIT_ORDER_REFORM:
			case UNIT_ORDER_INJOIN:
			default:
				
				break;
		}
}

void AiMain::RegisterHostileAction(UNIT_ORDER_TYPE attack_type, 
							  uint32 my_army_id, 
							  PLAYER_INDEX his_owner, 
							  uint32 his_unit_id, 
							  MapPointData *target_pos)
{
	_RPTF0(_CRT_WARN, "Register Hostile Action.\n");

	
	BOOL is_coldwar_attack = FALSE; 
    switch(attack_type) {
		case UNIT_ORDER_INVESTIGATE_CITY:
		case UNIT_ORDER_NULLIFY_WALLS:
		case UNIT_ORDER_STEAL_TECHNOLOGY:
		case UNIT_ORDER_ASSASSINATE:
		case UNIT_ORDER_INVESTIGATE_READINESS:
		case UNIT_ORDER_SUE:
		case UNIT_ORDER_FRANCHISE:
		case UNIT_ORDER_SUE_FRANCHISE:
		
		case UNIT_ORDER_CAUSE_UNHAPPINESS:
		case UNIT_ORDER_SLAVE_RAID:
		case UNIT_ORDER_UNDERGROUND_RAILWAY:
		case UNIT_ORDER_INCITE_UPRISING:
		case UNIT_ORDER_BIO_INFECT:
		case UNIT_ORDER_NANO_INFECT:
		case UNIT_ORDER_CONVERT:
		case UNIT_ORDER_INDULGENCE:
		case UNIT_ORDER_SOOTHSAY:
		case UNIT_ORDER_INJOIN:
		case UNIT_ORDER_THROW_PARTY:
		case UNIT_ORDER_INCITE_REVOLUTION:
		case UNIT_ORDER_EXPEL:
		case UNIT_ORDER_INTERCEPT_TRADE:
			is_coldwar_attack = TRUE;
			break;

		case UNIT_ORDER_PLANT_NUKE:
		case UNIT_ORDER_BOMBARD:
		case UNIT_ORDER_ENSLAVE_SETTLER:
		case UNIT_ORDER_RUSTLE:
		case UNIT_ORDER_FINISH_ATTACK:
		case UNIT_ORDER_CREATE_PARK:
		case UNIT_ORDER_INFORM_AI_CAPTURE_CITY:
		case UNIT_ORDER_PILLAGE:
		default:
			is_coldwar_attack = FALSE;
			break; 
	}
}
	
void AiMain::RegisterPollution(MapPointData *target_pos)
{
	_RPTF0(_CRT_WARN, "Register Pollution.\n");
	
}

void AiMain::RegisterPiracy(PLAYER_INDEX he_did_it,
					   PLAYER_INDEX source_owner, 
					   uint32 src_city_id, 
					   PLAYER_INDEX dest_owner, 
					   uint32 dest_city_id, 
					   sint32 resource)
{
	_RPTF0(_CRT_WARN, "Register Piracy.\n");
	
	
}

void AiMain::RegisterDiplomaticRequest(IC3DiplomaticRequest *request)
{
	_RPTF0(_CRT_WARN, "Register Diplomatic Request.\n");
	
	
}

void AiMain::RegisterDiplomaticResponse(IC3DiplomaticRequest *request)
{
	_RPTF0(_CRT_WARN, "Register Diplomatic Response.\n");

	
	
	

	switch(request->GetRequest()) 
	{
		case REQUEST_TYPE_GREETING:
			break;
		case REQUEST_TYPE_DEMAND_MAP:
			break;
		case REQUEST_TYPE_DEMAND_ADVANCE:
			break;
		case REQUEST_TYPE_DEMAND_CITY:
			break;
		case REQUEST_TYPE_DEMAND_GOLD:
			break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE:
			break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
			break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS:
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION:
			break;
		case REQUEST_TYPE_OFFER_ADVANCE:
			break;
		case REQUEST_TYPE_OFFER_CITY:
			break;
		case REQUEST_TYPE_OFFER_MAP:
			break;
		case REQUEST_TYPE_OFFER_GOLD:
			break;
		case REQUEST_TYPE_OFFER_CEASE_FIRE:
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE:
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY:
			break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION:
			break;
		case REQUEST_TYPE_EXCHANGE_ADVANCE:
			break;
		case REQUEST_TYPE_EXCHANGE_CITY:
			break;
		case REQUEST_TYPE_EXCHANGE_MAP:
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			break;
		default:
			
			break;
	}

	switch(request->GetResponse()) 
	{
		case REQUEST_RESPONSE_TYPE_ACCEPTED:
			break;
		case REQUEST_RESPONSE_TYPE_REJECTED:
			
			break;
		default:
			Assert(FALSE);
			break;
	}
}

void AiMain::RegisterFightWinner(PLAYER_INDEX attack_owner, 
								 sint32 attack_start_num,
								 PLAYER_INDEX defense_owner, 
								 sint32 defense_start_num, 
								 PLAYER_INDEX winner)
{
	_RPTF0(_CRT_WARN, "Register Fight Winner.\n");

	
}

void AiMain::RegisterContactMade(PLAYER_INDEX contacted)
{
	_RPTF0(_CRT_WARN, "Register Contact Made.\n");
	
}

void AiMain::RegisterLeaveOurLands(PLAYER_INDEX foreign_owner)
{
	_RPTF0(_CRT_WARN, "Register Leave Our Lands.\n");
	
}

void AiMain::RegisterLeaveOurLandsExpired (PLAYER_INDEX foreign_owner)
{
	_RPTF0(_CRT_WARN, "Register Leave Our Lands Expired .\n");
	
}

void AiMain::RegisterPlayerInfo(PLAYER_INDEX index, 
								C3PlayerInfo *pi)
{
	_RPTF0(_CRT_WARN, "Register Player Info.\n");
	
}

void AiMain::SetPersonalityFilename(char *filename)
{
	_RPTF0(_CRT_WARN, "Set Personality Filename.\n");
	
}

void AiMain::ReceiveTradeBid(uint32 from_city_id,
							 sint32 resource,
							 PLAYER_INDEX to_city_owner,
							 uint32 to_city_id,
							 sint32 price,
							 uint32 bid_id)
{
	_RPTF0(_CRT_WARN, "Receive Trade Bid.\n");
	
	

	
	
    
	
}

void AiMain::RegisterTradeOffer(IC3TradeOffer *offer)
{
	_RPTF0(_CRT_WARN, "Register Trade Offer.\n");
	

	
	
}

void AiMain::RemoveTradeOffer(uint32 id)
{
	_RPTF0(_CRT_WARN, "Remove Trade Offer.\n");
	
	
	
	
}

BOOL AiMain::ConfirmAcceptTradeOffer(uint32 offer,
									 uint32 destCityID,
									 PLAYER_INDEX player)
{
	_RPTF0(_CRT_WARN, "Confirm Accept Trade Offer.\n");
	
	

	return TRUE;
}

void AiMain::RegisterNewContinents()
{
	_RPTF0(_CRT_WARN, "Register New Continents.\n");
	
}

void AiMain::RegisterYouBrokeCeaseFire(PLAYER_INDEX with)
{
	_RPTF0(_CRT_WARN, "Register You Broke Cease Fire.\n");
	
}

void AiMain::RegisterCeaseFireBroken(PLAYER_INDEX by)
{
	_RPTF0(_CRT_WARN, "Register Cease Fire Broken.\n");
	
}

void AiMain::RegisterYouBrokeAlliance(PLAYER_INDEX with)
{
	_RPTF0(_CRT_WARN, "Register Cease Fire Broken.\n");
	
}

void AiMain::RegisterAllianceBroken(PLAYER_INDEX by)
{
	_RPTF0(_CRT_WARN, "Register Alliance Broken.\n");
	
}

void AiMain::RegisterHeNuked(PLAYER_INDEX he_did_it, 
							 PLAYER_INDEX his_target) 
{
	_RPTF0(_CRT_WARN, "Register He Nuked.\n");
	
}

void AiMain::RegisterRegardIncrease(PLAYER_INDEX who, 
									sint32 amount)
{
	_RPTF0(_CRT_WARN, "Register Regard Increase.\n");
	
}

void AiMain::GetFZRegard (PLAYER_INDEX him, 
						  sint32 &diplomatic, 
						  sint32 &unit, 
						  sint32 &bonus)
{
	_RPTF0(_CRT_WARN, "Get Fuzzy Regard.\n");
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}

void AiMain::SetFZRegard (PLAYER_INDEX him, 
						  sint32 diplomatic, 
						  sint32 unit, 
						  sint32 bonus)
{
	_RPTF0(_CRT_WARN, "Set Fuzzy Regard.\n");
	
}

void AiMain::RegisterAgreementAgainstMe(PLAYER_INDEX first_player, 
										PLAYER_INDEX second_player, 
										double agreement_str)
{
	_RPTF0(_CRT_WARN, "Register Agreement Against Me.\n");
	
	
	
}

void AiMain::RegisterAttack(PLAYER_INDEX attacker,
							PLAYER_INDEX defender)
{
	_RPTF0(_CRT_WARN, "Register Attack.\n");
	
}

void AiMain::RegisterBrokeTreaty(PLAYER_INDEX breaker,
								 PLAYER_INDEX breakee)
{
	_RPTF0(_CRT_WARN, "Register Broke Treaty.\n");

	
}

void AiMain::RegisterRemoveBuildNode(PLAYER_INDEX owner, 
									 uint32 city_id,
									 sint32 category, 
									 sint32 type)
{
	_RPTF0(_CRT_WARN, "Register Remove Build Node.\n");
	
	
	
	switch (category)
		{
			case k_GAME_OBJ_TYPE_IMPROVEMENT:
				break;
			case k_GAME_OBJ_TYPE_WONDER:
				break;
			case k_GAME_OBJ_TYPE_ENDGAME_OBJECT:
				break;
			case k_GAME_OBJ_TYPE_UNIT:
				break;
		}				
}

void AiMain::RegisterSetBuildFront(	uint32 city_id,
									sint32 category, 
									sint32 type)
{
	_RPTF0(_CRT_WARN, "Register Set Build Front.\n");
	
	
	
}

void AiMain::ForceRegard(PLAYER_INDEX foreign_player, 
						 double to_regard)
{
	_RPTF0(_CRT_WARN, "Force Regard.\n");
	
}

void AiMain::ForceHate(PLAYER_INDEX foreign_player)
{
	_RPTF0(_CRT_WARN, "Force Hate.\n");
	
}

void AiMain::SetBeingAttached(BOOL is_attaching)
{
	_RPTF0(_CRT_WARN, "Set Being Attached.\n");
	
	
}
