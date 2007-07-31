

#include "c3.h"
#include "Globals.h"
#include "CivArchive.h"

#include "IC3DipReq.h"
#include "IC3World.h"
#include "IC3Science.h"
#include "IC3Player.h"
#include "IC3TerrDB.h"              
#include "IC3Rand.h"

#include "FSDiplomat.h"

#include "FliEngine.h"
#include "fliif.h"
#include "FzOut.h"
#include "FzDipVars.h"
#include "FliSymbol.h"

#include "AiMain.h"
#include "Foreigner.h"
#include "ForeignAgent.h"
#include "AIDipReq.h"
#include "BSet.h"
#include "ForeignCity.h"

#include "AIRndCnt.h"
#include "AiMap.h"
#include "Cont.h"

#include "FlatPtr.h"
#include "CityAgent.h"
#include "Gold.h"

#include "IC3Trade.h"
#include "PointerList.h"
#include "AiCell.h"

#include "ArmyAgent.h"
#include "AgreementTypes.h"
#include "Pirate_Marker.h"

#include "WallClock.h"
extern Wall_Clock *g_wall_clock;

#include "IC3Rand.h"
#include "C3Player.h"

#include "TradeRouteData.h"

#define k_TRADE_REGARD_THRESHOLD 35.0


#define k_BEST_VALUE 1000.0
#define k_GOOD_VALUE 750.0
#define k_AVERAGE_VALUE 500.0
#define k_POOR_VALUE 250.0
#define k_WORST_VALUE 50.0

#define k_BELOW_HOT_WAR (k_REGARD_HOTWAR-5.0)
#define k_BELOW_COLD_WAR (k_REGARD_COLDWAR-5.0)
#define k_ABOVE_COLD_WAR (k_REGARD_GOLDWAR+5.0); 

#define REGFV(v) if(ai->m_fliEngine->GetSymbol(v)) { ai->m_fliEngine->GetSymbol(v)->RegisterInit(section); } else {Assert(ai->m_fliEngine->GetSymbol(v)); }

#define REGIM(v) if(ai->m_fliEngine->GetSymbol(v)) { ai->m_fliEngine->GetSymbol(v)->RegisterInit(FLI_SECT_PRE_INCOMING_DIPLOMACY); } else { Assert(ai->m_fliEngine->GetSymbol(v)); }

#define REGACC(v) if(ai->m_fliEngine->GetSymbol(v)) { ai->m_fliEngine->GetSymbol(v)->RegisterInit(FLI_SECT_ACCEPTED_MESSAGE); } else { Assert(ai->m_fliEngine->GetSymbol(v)); }
#define REGREJ(v) if(ai->m_fliEngine->GetSymbol(v)) { ai->m_fliEngine->GetSymbol(v)->RegisterInit(FLI_SECT_REJECTED_MESSAGE); } else { Assert(ai->m_fliEngine->GetSymbol(v)); }


extern double fz_his_message_tone; 
extern double fz_unit_regard_towards_him;
extern double fz_best_friend; 
extern double fz_worst_enemy; 

extern double fz_rand;
extern double fz_has_pact_against_me_decay;
extern double fz_time;

extern double fz_at_war_count;
extern double fz_he_beat_me_decay; 
extern double fz_current_military_readiness;
extern double fz_my_normalized_rank;
extern double fz_normalized_rank_diff_between_first_and_second_place;

FSDiplomat::FSDiplomat()
{
	m_tradeOffers = new PointerList<IC3TradeOffer>;
	sint32 i;
	for(i = 0; i < FLI_SECT_MAX; i++) {
		m_registered_foreigner_vars[i] = FALSE;
	}
	m_registered_incoming_vars = FALSE;
	m_registered_pre_outgoing = FALSE;
	m_registered_received_vars = FALSE;

}

FSDiplomat::~FSDiplomat()
{
	while(m_tradeOffers->GetHead()) {
		m_tradeOffers->RemoveHead()->Release();
	}

	delete m_tradeOffers;
    m_tradeOffers = NULL; 
}


void FSDiplomat::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	return;
}

BOOL FSDiplomat::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
	
	
	

	
	sint32 p;
	
    if (0 == ai->m_my_player_id) 
	{ 
		for(p = 1; p <k_MAX_PLAYERS; p++) 
		{
			
			if (!ai->m_foreigner[p])
				continue;

			
			
			Foreigner *foreigner = ai->m_foreigner[p];
			int player_type = foreigner->GetPlayerType(ai, foreigner);
			
			if (ai->m_round_count->GetRound() < 50 && 
				ai->m_player->IsRobotPlayer(p)) {
				foreigner->SetUnitRegard(100);
				foreigner->SetDiplomaticRegard(ai, 100);
            } else { 
                foreigner->SetUnitRegard(0);
        		foreigner->SetDiplomaticRegard(ai, 0);

            }

		}
		return TRUE; 
    } 
	

	
    
    
	
	for(p = 0; p <k_MAX_PLAYERS; p++) {

		
		if(!ai->m_foreigner[p])
			 continue;

		





		
		if((ai->m_foreigner[p]->GetPlayerIndex()) == 0)
		{
			Foreigner *foreigner = ai->m_foreigner[p];
			foreigner->SetUnitRegard(0);
			foreigner->SetDiplomaticRegard(ai, 0);
			continue ;
		}
				
		if (ai->m_foreigner[p]->HaveContact()) {
			Foreigner *foreigner = ai->m_foreigner[p];

			
			foreigner->SetColdwarAttackedMe(foreigner->GetColdwarAttackedMe() * foreigner->GetHisColdwarAttackedMeDecay());
			foreigner->SetHotwarAttackedMe(foreigner->GetHotwarAttackedMe() * foreigner->GetHisHotwarAttackedMeDecay());
			foreigner->SetAcceptedRequests(foreigner->GetAcceptedRequests() * foreigner->GetAcceptedRequestsDecay());
			foreigner->SetRejectedRequests(foreigner->GetRejectedRequests() * foreigner->GetRejectedRequestsDecay());
			foreigner->SetDecayedPiracy(foreigner->GetDecayedPiracy() * foreigner->GetPiracyDecay());
			foreigner->SetAttackedMyEnemies(foreigner->GetAttackedMyEnemies() *
											foreigner->GetAttackedMyEnemiesDecay());
			foreigner->SetAttackedMyFriends(foreigner->GetAttackedMyFriends() *
											foreigner->GetAttackedMyFriendsDecay());
			foreigner->SetBrokenTreaties(foreigner->GetBrokenTreaties() *
										 foreigner->GetBrokenTreatiesDecay());

            foreigner->BeginTurnHeBeatMe(fz_he_beat_me_decay); 
            foreigner->SetHasPactAgainstMe(
                   foreigner->HasPactAgainstMe() *
                   fz_has_pact_against_me_decay
                );

			
			for(sint32 requests = 0; requests < REQUEST_TYPE_NULL; requests++)
			{
				foreigner->DecayMyResponses((REQUEST_TYPE) requests);
				foreigner->DecayHisResponses((REQUEST_TYPE) requests);
			}
			
			
			
			SetupForeignerVars(ai, foreigner, TRUE, FLI_SECT_BEGIN_DIPLOMACY);
			if(!m_registered_foreigner_vars[FLI_SECT_PRE_OUTGOING_DIPLOMACY]) {
				SetupForeignerVars(ai, foreigner, FALSE, FLI_SECT_PRE_OUTGOING_DIPLOMACY);
			}

			sint32 r;
			static AIDiplomaticRequest request[REQUEST_TYPE_MAX];

			double maxutil = -1;
			sint32 maxreq = -1;

			sint32 leastRecentTime = 0x7fffffff;
			sint32 leastRecent = -1;
			for(r = (sint32)REQUEST_TYPE_GREETING;
				r < (sint32)REQUEST_TYPE_MAX;
				r++) {
				if(foreigner->LastConsideredSendingRequest((REQUEST_TYPE)r) < leastRecentTime) {
					leastRecent = r;
					leastRecentTime = foreigner->LastConsideredSendingRequest((REQUEST_TYPE)r);
				}
			}
			Assert(leastRecent >= 0);
			if(leastRecent < 0 )
				continue;
			
			if(fz_max_messages_considered_per_turn > (double)REQUEST_TYPE_MAX)
				fz_max_messages_considered_per_turn = (double)REQUEST_TYPE_MAX;

			sint32 n;
			for(r = leastRecent, n = 0; n < sint32(fz_max_messages_considered_per_turn); n++, r++) {
				if(r >= (sint32)REQUEST_TYPE_NULL)
					r = REQUEST_TYPE_GREETING;
				request[r].Clear();
				request[r].m_request = (REQUEST_TYPE)r;
				request[r].m_recipient = p;
				if((ai->m_round_count->GetRound() - foreigner->LastSentRequest((REQUEST_TYPE)r) >=
					fz_diplomatic_persistence)) {

					foreigner->RegisterConsideredSendingRequest(ai, (REQUEST_TYPE)r);

					GetSendUtility(ai, foreigner, &request[r]);
					if(request[r].m_isvalid &&
					   request[r].m_utility > request[r].m_threshold) {
						if(((request[r].m_utility - request[r].m_threshold) * request[r].m_dip_send_coef) 
								> maxutil) 
						{
							
							
 							maxutil = (request[r].m_utility - request[r].m_threshold) * request[r].m_dip_send_coef;
							maxreq = r;
						}
					}
				}
			}
			
			if(maxutil >= 0 && request[maxreq].m_isvalid) { 
				
				ai->SendDiplomaticRequest(&request[maxreq]);
			}
			foreigner->SetViolatingStopTrade(0.0);
			foreigner->SetViolatingLeaveOurLands(0.0);
			foreigner->SetViolatingReducePollution(0.0);
			foreigner->SetViolatingEndPollution(0.0);
			foreigner->SetViolatingAttackEnemy(0.0);
		}
	}

    
    BuildFriendTable(ai);

	
	ConsiderTradeOffers(ai);
	ConsiderMakingTradeOffers(ai);
	ConsiderMakingTradeBids(ai);


    return TRUE;
}

void FSDiplomat::SetupForeignerVars(AiMain *ai, Foreigner *foreigner,
									BOOL fireRules, sint32 section)
{
    ForeignAgent *him; 
    BSetID h_id;
    BSet<ForeignAgent> *vu; 

	if(!m_registered_foreigner_vars[section]) {
		m_registered_foreigner_vars[section] = TRUE;

		REGFV("current_military_readiness");
		REGFV("lowest_unit_regard");

		REGFV("enemy_of_my_enemy");
		REGFV("friend_of_my_enemy");

		REGFV("has_pact_against_me");
		REGFV("he_beat_me");
		REGFV("he_beat_me_this_turn");
		
		REGFV("diplomatic_regard_towards_him");
		REGFV("unit_regard_towards_him");
		
		REGFV("loyalty");

		REGFV("rejected_requests");
		REGFV("accepted_requests");
		
		REGFV("length_of_war_with_him");
		REGFV("num_he_is_at_war");
		
		REGFV("closest_capitol");
		REGFV("he_shares_continent");
		
		REGFV("cease_fire_with_him");
		REGFV("alliance_with_him");
		REGFV("war_with_him");
		REGFV("neutral_with_him");
		
		REGFV("last_attacked_me");
		REGFV("he_beat_me");
		REGFV("he_beat_me_this_turn");
		REGFV("his_hotwar_attacked_me");
		REGFV("his_coldwar_attacked_me");
		
		REGFV("his_strength");
		REGFV("my_strength");

		REGFV("his_strength");
		REGFV("relative_strength");
 
		REGFV("military_incursions");
		REGFV("my_military_incursions");

		REGFV("last_got_map");

		REGFV("his_num_trade_routes");
		REGFV("his_relative_num_trade_routes");
		REGFV("his_pirated_me");

		REGFV("num_players_in_game");

		if(section == FLI_SECT_BEGIN_DIPLOMACY) 
		{
			REGFV("violating_stop_trade");
			REGFV("violating_leave_our_lands");
			REGFV("violating_reduce_pollution");
			REGFV("violating_end_pollution");
			REGFV("violating_attack_enemy");
		}
		REGFV("have_stop_trade");
		REGFV("have_leave_our_lands");
		REGFV("have_reduce_pollution");
		REGFV("have_end_pollution");
		REGFV("have_attack_enemy");
		REGFV("have_cease_fire");
		REGFV("have_alliance");
		REGFV("at_war_count");
		REGFV("best_friend");
		REGFV("worst_enemy");
		REGFV("player_type");
		REGFV("attacked_my_enemies");
		REGFV("attacked_my_friends");
		REGFV("his_broken_treaties");
		REGFV("has_pact_against_me");
		REGFV("longest_war");
		REGFV("shortest_peace");
		REGFV("rand");

		REGFV("his_normalized_rank");
		REGFV("my_normalized_rank");
		REGFV("normalized_rank_diff_between_first_and_second_place");

		REGFV("chase_the_rabbit");
		REGFV("distance_to_his_capitol");
		REGFV("my_percent_pollution");
		REGFV("global_pollution_level");
		REGFV("turns_to_disaster");
		REGFV("time");

		REGFV("my_responses_DEMAND_ADVANCE"); 
		REGFV("my_responses_DEMAND_CITY"); 
		REGFV("my_responses_DEMAND_MAP"); 
		REGFV("my_responses_DEMAND_GOLD"); 
		REGFV("my_responses_DEMAND_STOP_TRADE"); 
		REGFV("my_responses_DEMAND_ATTACK_ENEMY"); 
		REGFV("my_responses_DEMAND_LEAVE_OUR_LANDS"); 
		REGFV("my_responses_DEMAND_REDUCE_POLLUTION"); 
		REGFV("my_responses_DEMAND_LOWER_TARIFFS"); 
		REGFV("my_responses_OFFER_ADVANCE"); 
		REGFV("my_responses_OFFER_CITY"); 
		REGFV("my_responses_OFFER_MAP"); 
		REGFV("my_responses_OFFER_GOLD"); 
		REGFV("my_responses_OFFER_CEASE_FIRE"); 
		REGFV("my_responses_OFFER_PERMANENT_ALLIANCE"); 
		REGFV("my_responses_OFFER_PACT_CAPTURE_CITY"); 
		REGFV("my_responses_OFFER_PACT_END_POLLUTION"); 
		REGFV("my_responses_EXCHANGE_ADVANCE"); 
		REGFV("my_responses_EXCHANGE_CITY"); 
		REGFV("my_responses_EXCHANGE_MAP"); 
	
		REGFV("his_responses_GREETING"); 
		REGFV("his_responses_DEMAND_ADVANCE"); 
		REGFV("his_responses_DEMAND_CITY"); 
		REGFV("his_responses_DEMAND_MAP"); 
		REGFV("his_responses_DEMAND_GOLD"); 
		REGFV("his_responses_DEMAND_STOP_TRADE"); 
		REGFV("his_responses_DEMAND_ATTACK_ENEMY"); 
		REGFV("his_responses_DEMAND_LEAVE_OUR_LANDS"); 
		REGFV("his_responses_DEMAND_REDUCE_POLLUTION"); 
		REGFV("his_responses_DEMAND_LOWER_TARIFFS"); 
		REGFV("his_responses_OFFER_ADVANCE"); 
		REGFV("his_responses_OFFER_CITY"); 
		REGFV("his_responses_OFFER_MAP"); 
		REGFV("his_responses_OFFER_GOLD"); 
		REGFV("his_responses_OFFER_CEASE_FIRE"); 
		REGFV("his_responses_OFFER_PERMANENT_ALLIANCE"); 
		REGFV("his_responses_OFFER_PACT_CAPTURE_CITY"); 
		REGFV("his_responses_OFFER_PACT_END_POLLUTION"); 
		REGFV("his_responses_EXCHANGE_ADVANCE"); 
		REGFV("his_responses_EXCHANGE_CITY"); 
		REGFV("his_responses_EXCHANGE_MAP"); 

		REGFV("he_built_undersea");
		REGFV("he_built_space"); 
		REGFV("he_built_wormhole");
		REGFV("someone_built_wormhole");
		REGFV("someone_built_space");
		REGFV("someone_built_undersea");

		REGFV("current_military_readiness");

	}

	
    fz_time = ai->m_round_count->GetRound(); 
	fz_rand = (ai->m_rand->Next((100)));
	
	
	foreigner->UpdateDiplomaticState(ai);

	
	ComputeNormalizedStr(ai);

	foreigner->UpdateMyResponses();
	foreigner->UpdateHisResponses();
	
	foreigner->GetUnitAge(fz_he_built_undersea, fz_he_built_space, fz_he_built_wormhole); 

	fz_his_pollution_level = foreigner->GetPollutionLevel();
	fz_my_pollution_level = ai->m_player->GetPollutionLevel();
	fz_global_pollution_level = ai->m_gs->GetGlobalPollutionLevel();
	
	if(fz_global_pollution_level == 0) { fz_global_pollution_level = 1;}

	fz_my_percent_pollution = fz_my_pollution_level/fz_global_pollution_level;
	fz_his_percent_pollution = fz_his_pollution_level/fz_global_pollution_level;

	fz_turns_to_disaster = ai->m_gs->GetRoundsToNextDisaster();

	fz_current_military_readiness = ai->m_player->GetReadinessLevel();

	fz_distance_to_his_capitol = foreigner->GetDistanceToCapitol();
	
	
	
	fz_diplomatic_regard_towards_him = foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard();
	
	
	
	
	
	fz_unit_regard_towards_him = foreigner->GetUnitRegard() + foreigner->GetBonusRegard();

	
	if (foreigner->GetInsaneHatred())
	{
		fz_unit_regard_towards_him = k_REGARD_INSANE_HATRED -1 ;
		fz_diplomatic_regard_towards_him = k_REGARD_INSANE_HATRED - 1;
	}

	
	fz_best_friend = 0; 
	fz_worst_enemy = 0; 
	double highest_regard = 0;
	double lowest_regard = 100;
	double test_regard = 50;
	sint32 whohigh = -1;
	sint32 wholow = -1;
	sint32 i;

	extern double fz_longest_war;
	extern double fz_shortest_peace;
	extern double fz_someone_built_wormhole;
	extern double fz_someone_built_space;
	extern double fz_someone_built_undersea;

	fz_at_war_count = 0 ;
	fz_longest_war = 0;
	fz_shortest_peace = 10000;
	double test_underwater = FALSE;
	double test_space = FALSE;
	double test_wormhole = FALSE;

	
	for (i=1; i<k_MAX_PLAYERS; i++) 
	{
		if( ai->m_foreigner[i]) 
		{
			test_regard = ai->m_foreigner[i]->GetDiplomaticRegard() + ai->m_foreigner[i]->GetBonusRegard(); 
			if(test_regard >= highest_regard) 
			{
				if(test_regard == highest_regard) 
				{
					whohigh = -1;
					highest_regard = test_regard;
				} 
				else 
				{
					whohigh = i;
					highest_regard = test_regard;
				}
			}
			if(test_regard <= lowest_regard)
			{
				
				if(test_regard == lowest_regard)
				{
					wholow = -1;
					lowest_regard = test_regard;
				} 
				else 
				{
					wholow = i;
					lowest_regard = test_regard;
				}
			}	
			if(fz_longest_war < ai->m_foreigner[i]->GetLengthOfWarWithHim(ai)) 
			{
				fz_longest_war = ai->m_foreigner[i]->GetLengthOfWarWithHim(ai);
			}
			if(fz_shortest_peace > ai->m_foreigner[i]->GetLengthOfPeaceWithHim(ai))
			{
				fz_shortest_peace = ai->m_foreigner[i]->GetLengthOfPeaceWithHim(ai);
			}
		
			
			if (ai->m_foreigner[i]->UpdateWar(ai, ai->my_player_index)) 
			{ 
                fz_at_war_count += 1.0; 
            }
		}
	}

	if(whohigh > 0 && 
		ai->m_foreigner[whohigh] == foreigner && 
		((ai->m_foreigner[whohigh]->GetDiplomaticRegard() + ai->m_foreigner[whohigh]->GetBonusRegard()) > 50)) 
	{
		fz_best_friend = 1;
	}

	if(wholow > 0 && 
		ai->m_foreigner[wholow] == foreigner &&
		((ai->m_foreigner[wholow]->GetDiplomaticRegard() + ai->m_foreigner[wholow]->GetBonusRegard()) < 50)) 
	{
		fz_worst_enemy = 1;
	}
	
	if(fz_best_friend == 1 && fz_worst_enemy == 1)
	{
		fz_worst_enemy = 0;
		fz_best_friend = 0;
	}

	fz_his_coldwar_attacked_me = foreigner->GetColdwarAttackedMe();
	fz_his_hotwar_attacked_me = foreigner->GetHotwarAttackedMe();

	fz_rejected_requests = foreigner->GetRejectedRequests();
	fz_accepted_requests = foreigner->GetAcceptedRequests();

	fz_num_he_is_at_war = foreigner->GetNumPlayersHeIsFighting();
	fz_closest_capitol = foreigner->GetClosestCapitol();
	fz_he_shares_continent = foreigner->GetHeSharesContinent();
	fz_length_of_war_with_him = foreigner->GetLengthOfWarWithHim(ai);

	

	
	fz_his_normalized_rank = foreigner->GetNormalizedRank();
	
	
	fz_player_type = foreigner->GetPlayerType(ai, foreigner);
	
	fz_chase_the_rabbit = FALSE;

	
	
	for (i=1; i<k_MAX_PLAYERS; i++) 	
	{
		if( ai->m_foreigner[i]) 
		{
			double his_type = ai->m_foreigner[i]->GetPlayerType(ai, ai->m_foreigner[i]);
			double his_rank = ai->m_foreigner[i]->GetNormalizedRank();
			if (his_type != 1 && his_rank == 1.0)
			{
				fz_chase_the_rabbit = 1.0;
			}
		
		}
	}

	
	
	
	sint32 my_player_id = ai->my_player_index;

	fz_cease_fire_with_him = foreigner->UpdateCeaseFire(ai, my_player_id); 
	fz_alliance_with_him = foreigner->UpdateAlliance(ai, my_player_id);
	fz_war_with_him = foreigner->UpdateWar(ai, my_player_id);
	fz_neutral_with_him = foreigner->UpdateNeutrality(ai, my_player_id);

	fz_his_num_trade_routes = ai->m_gs->GetTotalTrade(foreigner->GetPlayerIndex());
	fz_his_relative_num_trade_routes = fz_his_num_trade_routes / double(ai->m_gs->GetGlobalTradeRoutes() * 2);
	fz_his_pirated_me = foreigner->GetDecayedPiracy();

	fz_he_beat_me = foreigner->GetHeBeatMe();
	fz_he_beat_me_this_turn = foreigner->GetHeBeatMeThisTurn();

	
	
	if(foreigner->GetLastAttackedMe() < 0)
	{
		
		
		
		

		
		
		
		fz_last_attacked_me = 1000;
	}
	else
	{
		fz_last_attacked_me = ai->m_round_count->GetRound() - foreigner->GetLastAttackedMe();
	}

	fz_his_strength = ai->GetStrength(foreigner->GetPlayerIndex());
	fz_my_strength = ai->GetStrength(ai->my_player_index);
	fz_loyalty = foreigner->GetLoyalty();

	if(fz_his_strength == 0)
		fz_his_strength = 0.001;

	fz_relative_strength = fz_my_strength / fz_his_strength;

	
	
	
	
	
	
	

	fz_military_incursions = 0;

	vu = foreigner->GetVisibleUnits();

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
	double attack = 0;

	for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id)) { 
Assert(finite_loop++ < 1000); 

		MapPointData pos;
		him->GetPos(pos);
		attack = him->GetAttackStrength();
		if(!ai->m_player->IsActuallyVisible(him->GetPlayerIndex(),
											him->GetID().GetVal()))
			continue;

		if(attack > 0.001)
		{
			if(ai->m_world->GetCellOwner(&pos) == ai->my_player_index) 
			{
				fz_military_incursions += him->GetUnitNum();
			}
		}
	} 

    BSetID id;
    ArmyAgent *agent;
	fz_my_military_incursions = 0;

    for (agent = ai->m_army_set->First(id); 
		 ai->m_army_set->Last(); 
		 agent = ai->m_army_set->Next(id)) 
	{ 
        MapPointData pos;
		agent->GetPos(ai, pos);
		attack=agent->GetAttackStrength();
		if(attack > 0)
		{
			if(ai->m_world->GetCellOwner(&pos) == foreigner->GetPlayerIndex())
			fz_my_military_incursions += agent->GetNUnits();
		}
	}

	
	
	

	fz_last_got_map = (sint32)ai->m_round_count->GetRound() - foreigner->GotMapFrom();


	
	
	
	
	

	if(section == FLI_SECT_BEGIN_DIPLOMACY) {
		fz_violating_stop_trade = foreigner->GetViolatingStopTrade();
		fz_violating_leave_our_lands = foreigner->GetViolatingLeaveOurLands();
		fz_violating_reduce_pollution = foreigner->GetViolatingReducePollution();
		fz_violating_end_pollution = foreigner->GetViolatingEndPollution();
		fz_violating_attack_enemy = foreigner->GetViolatingAttackEnemy();

	}


	fz_have_stop_trade = foreigner->HeAgreed(ai, AGREEMENT_TYPE_DEMAND_STOP_TRADE);
	fz_have_leave_our_lands = foreigner->HeAgreed(ai, AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS);
	fz_have_reduce_pollution = foreigner->HeAgreed(ai, AGREEMENT_TYPE_REDUCE_POLLUTION);
	fz_have_end_pollution = foreigner->HeAgreed(ai, AGREEMENT_TYPE_PACT_END_POLLUTION);
	
	
	
	fz_have_attack_enemy = foreigner->HeAgreed(ai, AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY);
	fz_have_cease_fire = foreigner->HeAgreed(ai, AGREEMENT_TYPE_CEASE_FIRE);

	
	
	
	fz_have_alliance = !ai->m_player->IsEnemy(foreigner->GetPlayerIndex());

	
	fz_attacked_my_enemies = foreigner->GetAttackedMyEnemies();
	
	
	
	fz_attacked_my_friends = foreigner->GetAttackedMyFriends();

	
	fz_his_broken_treaties = foreigner->GetBrokenTreaties();

	if(fireRules) {
		fz_his_coldwar_attacked_me_decay = 1.0;
        fz_his_hotwar_attacked_me_decay = 1.0;

		fz_accepted_requests_decay = 1.0;
		fz_rejected_requests_decay = 1.0;
		fz_piracy_decay = 1.0;
		fz_attacked_my_enemies_decay = 1.0;
		fz_attacked_my_friends_decay = 1.0;
		fz_his_broken_treaties_decay = 1.0;

		
		fz_diplomatic_regard_delta = 0;
		fz_dip_regard_pirate_delta = 0;
		fz_dip_regard_attack_delta = 0;
		fz_dip_regard_incursion_delta = 0; 
		fz_dip_regard_pillage_delta = 0; 
		fz_dip_regard_message_delta = 0;
		fz_dip_regard_general_delta =0 ; 
		fz_dip_regard_strength_delta = 0;
		fz_unit_regard_delta = 0.0;
		ai->m_fliEngine->FireRules(section);
				
		
		
		double new_diplomatic_regard = foreigner->GetDiplomaticRegard() 
			+ fz_diplomatic_regard_delta 
			+ fz_dip_regard_pirate_delta 
			+ fz_dip_regard_attack_delta 
			+ fz_dip_regard_incursion_delta 
			+ fz_dip_regard_pillage_delta 
			+ fz_dip_regard_message_delta 
			+ fz_dip_regard_general_delta 
			+ fz_dip_regard_strength_delta 
			; 
		
		fz_diplomatic_regard_delta = 0;
		fz_dip_regard_pirate_delta = 0;
		fz_dip_regard_attack_delta = 0;
		fz_dip_regard_incursion_delta = 0; 
		fz_dip_regard_pillage_delta = 0; 
		fz_dip_regard_message_delta = 0;
		fz_dip_regard_general_delta =0 ; 
		fz_dip_regard_strength_delta = 0;
		
		if(new_diplomatic_regard > 100) {
			
			new_diplomatic_regard = 100;
		}
		
		if(new_diplomatic_regard < 0 )
		
		{
			new_diplomatic_regard = k_REGARD_INSANE_HATRED - 5  ;
			
		}

		foreigner->SetDiplomaticRegard(ai, new_diplomatic_regard);
		fz_diplomatic_regard_towards_him = new_diplomatic_regard;
	
		
		
		double new_unit_regard = foreigner->GetUnitRegard();
		
		
		
		if(fz_unit_regard_delta <= 5) 
		
		{
			new_unit_regard = fz_unit_regard_towards_him; 
		}else if (fz_unit_regard_delta > 5)
		{
			new_unit_regard = fz_unit_regard_delta; 
		}
		fz_unit_regard_delta = 0.0;
		
			

		if(new_unit_regard > 100) {
			
			new_unit_regard = 100;
		}
		
		if(new_unit_regard < 0 )
		
		{
			new_unit_regard = k_REGARD_INSANE_HATRED + 1;
		}

		foreigner->SetUnitRegard(new_unit_regard);
		fz_unit_regard_towards_him = new_unit_regard;



		double newLoyalty = foreigner->GetLoyalty() + fz_loyalty_modifier;
		if(newLoyalty > 100) {
			newLoyalty = 100;
		}

		foreigner->SetLoyalty(newLoyalty);
		fz_loyalty = newLoyalty;

		if((0.0 <= fz_his_coldwar_attacked_me_decay) && (fz_his_coldwar_attacked_me_decay < 0.999999999))  
			foreigner->SetHisColdwarAttackedMeDecay(fz_his_coldwar_attacked_me_decay);

		if((0.0 <= fz_his_hotwar_attacked_me_decay) && (fz_his_hotwar_attacked_me_decay < 0.999999999))  
			foreigner->SetHisHotwarAttackedMeDecay(fz_his_hotwar_attacked_me_decay);

		if ((0.0 <= fz_accepted_requests_decay) && (fz_accepted_requests_decay < 0.99999999))
			foreigner->SetAcceptedRequestsDecay(fz_accepted_requests_decay);

		if ((0.0 <= fz_rejected_requests_decay) && (fz_rejected_requests_decay < 0.99999999))
			foreigner->SetRejectedRequestsDecay(fz_rejected_requests_decay);

		if ((0.0 <= fz_piracy_decay) &&  (fz_piracy_decay < 0.9999999))
			foreigner->SetPiracyDecay(fz_piracy_decay);

		if((0.0 <= fz_attacked_my_enemies_decay) && (fz_attacked_my_enemies_decay < 0.9999999))
			foreigner->SetAttackedMyEnemiesDecay(fz_attacked_my_enemies_decay);
		if((0.0 <= fz_attacked_my_friends_decay) && (fz_attacked_my_friends_decay < 0.9999999))
			foreigner->SetAttackedMyEnemiesDecay(fz_attacked_my_friends_decay);

		if((0.0 <= fz_his_broken_treaties_decay) && 
		   (fz_his_broken_treaties_decay < 0.9999999)) 
		{
			foreigner->SetBrokenTreatiesDecay(fz_his_broken_treaties_decay);
		}
		for(sint32 request = 0 ; request < REQUEST_TYPE_NULL; request++)
		{
			if(fz_his_responses_decay[(REQUEST_TYPE) request] > 0 
				&& fz_his_responses_decay[(REQUEST_TYPE) request] < .99)
			{
				foreigner->SetHisResponsesDecay(
					(REQUEST_TYPE) request,
					fz_his_responses_decay[(REQUEST_TYPE) request]);
			}
			if(fz_my_responses_decay[(REQUEST_TYPE) request] > 0
				&& fz_my_responses_decay[(REQUEST_TYPE) request] < .99)
			{
				foreigner->SetMyResponsesDecay(
					(REQUEST_TYPE) request,
					fz_my_responses_decay[(REQUEST_TYPE) request]);
			}
		}
			
	}
}

void FSDiplomat::ComputeNormalizedStr(AiMain *ai)
{ 
     double str_table[k_MAX_PLAYERS]; 
     sint32 fcount=1; 
     double str; 
     double min_str = ai->m_my_raw_strength; 
     double max_str = ai->m_my_raw_strength; 
	 double top = 0.001, second = 0.001;

     sint32 i,j;
     

     str_table[0] = 10000000.0; 
	 
     for (i=1; i<k_MAX_PLAYERS; i++) { 
        if (ai->m_foreigner[i]) { 
            str = ai->m_foreigner[i]->GetRawStrength(); 

            if (str<min_str) { 
                min_str =str; 
            } 

            if (max_str < str) { 
                max_str = str; 
            } 

            str_table[i] = str; 
            fcount++; 
        } else { 
            str_table[i] = 10000000.0; 
        }
     }
     str_table[ai->m_my_player_id] = ai->m_my_raw_strength; 

     
     double tmp;

	 
     for (i=1; i<(k_MAX_PLAYERS-1); i++) { 
         for (j=i+1; j<k_MAX_PLAYERS; j++) { 
             if (str_table[i] > str_table[j]) { 
                 tmp = str_table[i]; 
                 str_table[i] = str_table[j];
                 str_table[j] = tmp; 
             } 
         } 
     } 

	
	if(str_table[fcount - 1] > ai->m_my_raw_strength)
	{
		top = str_table[fcount];
		if(str_table[fcount - 1] > ai->m_my_raw_strength)
		{
			second = str_table[fcount - 2];
		}else
		{
			second = ai->m_my_raw_strength;
		}
	}else
	{
		top = ai->m_my_raw_strength;
		second = str_table[fcount - 1];
	}

	
	if(top <0.001) { 
		fz_normalized_rank_diff_between_first_and_second_place = 0.0;
	}else
	{
		fz_normalized_rank_diff_between_first_and_second_place = 
			second/top;
	}

     
     double median = str_table[fcount/2]; 

     if (ai->m_my_raw_strength < median) { 
         fz_my_normalized_rank = 0.5 * (ai->m_my_raw_strength - min_str)/(median-min_str); 
     } else if (median < ai->m_my_raw_strength) { 
         fz_my_normalized_rank = 0.5 + 0.5 * (ai->m_my_raw_strength - median) / (max_str - median); 
     } else { 
         fz_my_normalized_rank = 0.5; 
     } 

     
     if (ai->my_player_index != 0){
        ai->m_foreigner[0]->SetHisRank(1.0); 
     }
     double his_rank; 

	 for (i=1; i<k_MAX_PLAYERS; i++) { 
		 if (ai->m_foreigner[i]) { 
			 str = ai->m_foreigner[i]->GetRawStrength(); 
			 
			 if (str < median) { 
				 his_rank = 0.5 * (str - min_str)/(median-min_str); 
			 } else if (median < str) { 
				 his_rank = 0.5 + 0.5 * (str - median) / (max_str - median); 
			 } else { 
				 his_rank = 0.5; 
			 } 
			 
			 ai->m_foreigner[i]->SetHisRank(his_rank); 		 
		 }
	 }
}


void FSDiplomat::BuildFriendTable(AiMain *ai)
{
    sint32 player_idx, third_party;
    double who_likes_who[k_MAX_PLAYERS][k_MAX_PLAYERS]; 

    
    for (player_idx=1; player_idx<k_MAX_PLAYERS; player_idx++) { 
        ai->m_player->GetAllRegard(player_idx, who_likes_who[player_idx]); 
    } 

    double is_friend_of_my_enemy; 
    double is_enemy_of_my_enemy;
    sint32 diplomatic;
    sint32 unit;
    sint32 bonus;
    double regard; 

    
    for (player_idx=1; player_idx<k_MAX_PLAYERS; player_idx++) { 

        if (ai->m_foreigner[player_idx] == NULL) continue;

        is_friend_of_my_enemy = 0.0; 
        is_enemy_of_my_enemy = 0.0;

        
        for (third_party=1; third_party<k_MAX_PLAYERS; third_party++) { 

             
            if (ai->m_foreigner[third_party]==NULL) continue; 
            if (player_idx == third_party) continue; 

            ai->m_foreigner[third_party]->GetFZRegard(diplomatic, unit, bonus); 
            if (k_REGARD_HOTWAR < diplomatic) continue; 
            

            
            
            

            regard = min(who_likes_who[player_idx][third_party], 
                    who_likes_who[third_party][player_idx]); 

            if (k_REGARD_FRIENDLY <= regard) { 
                is_friend_of_my_enemy = 1.0; 
            } else if (regard <= k_REGARD_HOTWAR) { 
                is_enemy_of_my_enemy = 1.0; 
            } 
        }

        
        ai->m_foreigner[player_idx]->SetEnemyOfMyEnemy(is_enemy_of_my_enemy);
        ai->m_foreigner[player_idx]->SetFriendOfMyEnemy(is_friend_of_my_enemy);
    } 
}

void FSDiplomat::SetupIncomingMessageVars(AiMain *ai,
										  IC3DiplomaticRequest *request)
{
	fz_i_lose_city = 0; 
	fz_he_gains_city = 0; 
	fz_gold_lost = 0;
	fz_he_gains_advance = 0; 
	fz_contact_gained = 0; 
	fz_third_party_strength = 0; 
	fz_third_party_regard = 0; 
	fz_at_war_with_third_party = 0; 
	fz_stops_trade_with_third_party = 0; 
	fz_i_will_attack_third_party = 0; 
	fz_he_built_undersea = 0;
	fz_he_built_space = 0 ;
	fz_he_built_wormhole = 0;

	fz_reduces_pollution = 0; 
	fz_his_pollution_level = 0; 

	fz_my_pollution_level = 0; 

	fz_global_pollution_level = 0; 

	fz_i_will_stop_piracy = 0; 
	fz_i_gain_advance = 0; 
	fz_advances_skipped = 0; 
	fz_advance_cost = 0; 
	fz_i_gain_city = 0; 
	fz_projected_food = 0; 
	fz_projected_production = 0; 
	fz_gold_gained = 0; 
	fz_gains_cease_fire = 0; 
	fz_i_gain_map = 0;
	fz_i_gain_gold = 0;
	fz_he_gets_map = 0;
	fz_end_pollution_pact = 0;
	fz_gains_alliance = 0;
	fz_i_will_leave_his_lands = 0;
	fz_he_gains_gold = 0;
	fz_he_attacks_enemy = 0;
	fz_his_trade_with_third_party = 0;
	fz_third_party_relative_strength = 0;
	fz_his_third_party_relative_strength = 0;

	if(!m_registered_incoming_vars) {
		m_registered_incoming_vars = TRUE;
		REGIM("gains_cease_fire");
		REGIM("gold_gained");
		REGIM("projected_production");
		REGIM("projected_food");
		REGIM("i_gain_city");
		REGIM("advance_cost");
		REGIM("advances_skipped");
		REGIM("i_gain_advance");
		REGIM("i_will_stop_piracy");
		REGIM("global_pollution_level");
		REGIM("my_pollution_level");
		REGIM("his_pollution_level");
		REGIM("reduces_pollution");
		REGIM("i_will_attack_third_party");
		REGIM("stops_trade_with_third_party");
		REGIM("third_party_regard");
		REGIM("third_party_strength");
		REGIM("contact_gained");
		REGIM("he_gains_advance");
		REGIM("i_lose_city");
		REGIM("he_gains_city");
		REGIM("i_gain_gold");
		REGIM("gold_lost");
		REGIM("i_gain_map");
		REGIM("he_gets_map");
		REGIM("end_pollution_pact");
		REGIM("gains_alliance");
		REGIM("i_will_leave_his_lands");
		REGIM("he_gains_gold");
		REGIM("he_attacks_enemy");
		REGIM("his_trade_with_third_party");
		REGIM("third_party_relative_strength");
		REGIM("his_third_party_relative_strength");
		REGIM("at_war_with_third_party");
	}

	Foreigner *foreigner = ai->m_foreigner[request->GetOwner()];

	switch(request->GetRequest()) {
		case REQUEST_TYPE_GREETING:
			fz_contact_gained = 1;
			break;
		
		case REQUEST_TYPE_DEMAND_ADVANCE:
			fz_advance_cost = ai->m_science->GetCost(request->GetAdvance());
			fz_he_gains_advance = 1;
			break;
		case REQUEST_TYPE_DEMAND_CITY:
		{
			uint32 target = request->GetTarget();
			CityAgent *city  = ai->m_city_set->Find(target);
			fz_i_lose_city = 1;
			fz_he_gains_city = 1;
			break;
		}
		case REQUEST_TYPE_DEMAND_MAP:
			fz_he_gets_map = 1;
			break;
		case REQUEST_TYPE_DEMAND_GOLD:
			if(ai->m_gold->GetCurrentSavings() < 1)
				break;
			if(ai->m_gold->GetCurrentSavings() < request->GetGold())
				break;
			fz_gold_lost = request->GetGold() / ai->m_gold->GetCurrentSavings();
			fz_he_gains_gold = 1;
			break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE:
			if(!ai->m_foreigner[request->GetThirdParty()])
				break;
			fz_third_party_strength = ai->GetStrength(request->GetThirdParty());
			fz_third_party_regard = ai->m_foreigner[request->GetThirdParty()]->GetDiplomaticRegard()
                    + ai->m_foreigner[request->GetThirdParty()]->GetBonusRegard();
			fz_stops_trade_with_third_party = 1;
			fz_his_trade_with_third_party = ai->GetTradeWith(foreigner->GetPlayerIndex(), request->GetThirdParty());
			fz_third_party_relative_strength = ai->GetStrength(ai->my_player_index) / fz_third_party_strength;
			fz_his_third_party_relative_strength = ai->GetStrength(foreigner->GetPlayerIndex()) / fz_third_party_strength;
			break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
			if(!ai->m_foreigner[request->GetThirdParty()])
				break;
			fz_third_party_strength = ai->GetStrength(request->GetThirdParty());
			fz_third_party_regard = ai->m_foreigner[request->GetThirdParty()]->GetDiplomaticRegard()
                + ai->m_foreigner[request->GetThirdParty()]->GetBonusRegard();
			fz_i_will_attack_third_party = 1;
			fz_at_war_with_third_party = 
						ai->m_foreigner[request->GetThirdParty()]->UpdateWar(ai, ai->my_player_index);
			fz_his_trade_with_third_party = ai->GetTradeWith(foreigner->GetPlayerIndex(), request->GetThirdParty());
			fz_third_party_relative_strength = ai->GetStrength(ai->my_player_index) / fz_third_party_strength;
			fz_his_third_party_relative_strength = ai->GetStrength(foreigner->GetPlayerIndex()) / fz_third_party_strength;
			break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS:
			fz_i_will_leave_his_lands = 1;
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION:
			fz_his_pollution_level = foreigner->GetPollutionLevel();
			fz_my_pollution_level = ai->m_player->GetPollutionLevel();
			fz_global_pollution_level = ai->m_gs->GetGlobalPollutionLevel();
			fz_reduces_pollution = 1;
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			fz_i_will_stop_piracy = 1;
			break;

		
		case REQUEST_TYPE_OFFER_ADVANCE:
			fz_advance_cost = ai->m_science->GetCost(request->GetAdvance());
			fz_advances_skipped = ai->m_science->GetMinPrerequisites(request->GetAdvance());
			fz_i_gain_advance = 1;
			break;
		case REQUEST_TYPE_OFFER_CITY:
		{
			BSet<ForeignCity> *hisCities = foreigner->GetKnownCities();
			uint32 target = request->GetTarget();
			ForeignCity *city = hisCities->Find(target);
			if(city) {
				MapPointData pos;
				city->GetPos(pos);
				fz_projected_food = city->GetFood(ai);
				fz_projected_production = city->GetProduction(ai); 
				fz_i_gain_city = 1;
			}
			break;
		}
		case REQUEST_TYPE_OFFER_MAP:
			fz_i_gain_map = 1;
			break;
		case REQUEST_TYPE_OFFER_GOLD:							
			fz_i_gain_gold = 1;
			if(ai->m_gold->GetCurrentSavings() < 1)
				fz_gold_gained = 1;
			else
				fz_gold_gained = (double)request->GetGold() / (double)ai->m_gold->GetCurrentSavings();
			break;

		case REQUEST_TYPE_OFFER_CEASE_FIRE:
			fz_gains_cease_fire = 1;
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE:
			fz_gains_alliance = 1;
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY:
			fz_he_attacks_enemy = 1;
			fz_his_trade_with_third_party = ai->GetTradeWith(foreigner->GetPlayerIndex(), request->GetThirdParty());
			break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION:
			fz_end_pollution_pact = 1;
			break;




		
		case REQUEST_TYPE_EXCHANGE_ADVANCE:
			fz_he_gains_advance = 1;
			fz_advance_cost = ai->m_science->GetCost(request->GetAdvance());
			fz_advances_skipped = ai->m_science->GetMinPrerequisites(request->GetAdvance());
			fz_i_gain_advance = 1;
			break;
		case REQUEST_TYPE_EXCHANGE_CITY:
			break;
		case REQUEST_TYPE_EXCHANGE_MAP:
			fz_i_gain_map = 1;
			fz_he_gets_map = 1;
			break;
		default:
			Assert(FALSE);
		
	}
}

BOOL FSDiplomat::HandleIncomingMessage(AiMain *ai,
									   IC3DiplomaticRequest *request)
{
	request->AddRef();

	if(request->GetOwner() == ai->my_player_index) {
		request->Release();
		return FALSE;
	}

    if (0 == ai->m_my_player_id) { 
        request->Reject(); 
    } else { 
		sint32 section = FLI_SECT_PRE_INCOMING_DIPLOMACY;
		if(!m_registered_foreigner_vars[section]) {
			REGFV("his_message_tone");
		}

	    fz_his_message_tone = request->GetTone();

		SetupIncomingMessageVars(ai, request);
	    SetupForeignerVars(ai, ai->m_foreigner[request->GetOwner()], TRUE,
						   FLI_SECT_PRE_INCOMING_DIPLOMACY);

	    double utility = GetUtility(ai, request);
		utility *= fz_diplomatic_receive_coeff[request->GetRequest()];

	    
		
		
		
 
		
		
		if (	( ai->m_foreigner[request->GetOwner()]->GetLengthOfWarWithHim(ai) < 8) 
				&& (!ai->m_player->IsRobotPlayer(request->GetOwner()))
				&& (request->GetRequest() == REQUEST_TYPE_OFFER_CEASE_FIRE ) ) 
		{ 
			fz_accept_threshold = 10001 ;
		}

		if (fz_accept_threshold <= utility) 
		{
			
			ai->m_foreigner[request->GetOwner()]->SetMyResponses(
				(request->GetRequest()), 
				1); 

			
			if(request->GetRequest() == REQUEST_TYPE_DEMAND_ATTACK_ENEMY &&
				ai->m_foreigner[request->GetThirdParty()])
			{	
				ai->m_foreigner[request->GetThirdParty()]->SetUnitRegard(k_BELOW_HOT_WAR);
				ai->m_foreigner[request->GetThirdParty()]->SetDiplomaticRegard(ai, k_BELOW_HOT_WAR);
			}

			if(request->GetRequest() == REQUEST_TYPE_OFFER_CEASE_FIRE)
			{	
				if((ai->m_foreigner[request->GetOwner()]->GetDiplomaticRegard()) < k_BELOW_COLD_WAR)
					ai->m_foreigner[request->GetOwner()]->SetDiplomaticRegard(ai, k_BELOW_COLD_WAR);
				if((ai->m_foreigner[request->GetOwner()]->GetUnitRegard()) < k_BELOW_COLD_WAR)
					ai->m_foreigner[request->GetOwner()]->SetUnitRegard(k_BELOW_COLD_WAR);
				
				ai->m_foreigner[request->GetOwner()]->SetLengthOfWar(-1) ;
				ai->m_foreigner[request->GetOwner()]->SetLengthOfPeace(ai->m_round_count->GetRound()) ;
			}
			
			
			request->Enact();

		} else {
			
			ai->m_foreigner[request->GetOwner()]->SetMyResponses(
				(request->GetRequest()),
				0);		  

			
			request->Reject();

	    }
    }

	ai->m_fliEngine->FireRules(FLI_SECT_POST_INCOMING_DIPLOMACY);

	request->Release();

	return TRUE;
}

BOOL FSDiplomat::HandleIncomingResponse(AiMain *ai, IC3DiplomaticRequest *request)
{
	switch(request->GetResponse()) {
		case REQUEST_RESPONSE_TYPE_ACCEPTED:
			HandleAcceptedMessage(ai, request);
			
			break;
		case REQUEST_RESPONSE_TYPE_REJECTED:
			HandleRejectedMessage(ai, request); 
			
			break;
		default:
			Assert(FALSE);
			break;
	}
	return TRUE;
}

void FSDiplomat::ClearFuzzyMessageReceived(AiMain *ai)
{
	if(!m_registered_received_vars) {
		m_registered_received_vars = TRUE;
		REGACC("he_accepted_greetings");
		REGACC("he_accepted_gold_demand");
		REGACC("he_accepted_demand_map");
		REGACC("he_accepted_city_demand");
		REGACC("he_accepted_demand_advance");
		REGACC("he_accepted_demand_stop_trade");
		REGACC("he_accepted_demand_attack_enemy");
		REGACC("he_accepted_demand_leave_our_lands");
		REGACC("he_accepted_demand_reduce_pollution");
		REGACC("he_accepted_offer_advance");
		REGACC("he_accepted_offer_city");
		REGACC("he_accepted_offer_map");
		REGACC("he_accepted_offer_gold");
		REGACC("he_accepted_offer_cease_fire");
		REGACC("he_accepted_offer_alliance");
		REGACC("he_accepted_offer_capture_city");
		REGACC("he_accepted_offer_pact_end_pollution");
		REGACC("he_accepted_exchange_advance");
		REGACC("he_accepted_exchange_city");
		REGACC("he_accepted_exchange_map");
		REGACC("he_accepted_demand_no_piracy");

		REGREJ("he_rejected_greetings");
		REGREJ("he_rejected_gold_demand");
		REGREJ("he_rejected_demand_map");
		REGREJ("he_rejected_city_demand");
		REGREJ("he_rejected_demand_advance");
		REGREJ("he_rejected_demand_stop_trade");
		REGREJ("he_rejected_demand_attack_enemy");
		REGREJ("he_rejected_demand_leave_our_lands");
		REGREJ("he_rejected_demand_reduce_pollution");
		REGREJ("he_rejected_offer_advance");
		REGREJ("he_rejected_offer_city");
		REGREJ("he_rejected_offer_map");
		REGREJ("he_rejected_offer_gold");
		REGREJ("he_rejected_offer_cease_fire");
		REGREJ("he_rejected_offer_alliance");
		REGREJ("he_rejected_offer_capture_city");
		REGREJ("he_rejected_offer_pact_end_pollution");
		REGREJ("he_rejected_exchange_advance");
		REGREJ("he_rejected_exchange_city");
		REGREJ("he_rejected_exchange_map");
		REGREJ("he_rejected_demand_no_piracy");
	}

	
	fz_he_accepted_greetings = 0;
	fz_he_accepted_gold_demand = 0;
	fz_he_accepted_demand_map = 0;
	fz_he_accepted_city_demand = 0;
	fz_he_accepted_demand_advance = 0;
	fz_he_accepted_demand_stop_trade = 0;
	fz_he_accepted_demand_attack_enemy = 0;
	fz_he_accepted_demand_leave_our_lands = 0;
	fz_he_accepted_demand_reduce_pollution = 0;
	fz_he_accepted_offer_advance = 0;
	fz_he_accepted_offer_city = 0;
	fz_he_accepted_offer_map = 0;
	fz_he_accepted_offer_gold = 0;
	fz_he_accepted_offer_cease_fire = 0;
	fz_he_accepted_offer_alliance = 0;
	fz_he_accepted_offer_capture_city = 0;
	fz_he_accepted_offer_pact_end_pollution = 0;
	fz_he_accepted_exchange_advance = 0;
	fz_he_accepted_exchange_city = 0;
	fz_he_accepted_exchange_map = 0;
	fz_he_accepted_demand_no_piracy = 0;

	
	fz_he_rejected_greetings = 0;
	fz_he_rejected_gold_demand = 0;
	fz_he_rejected_demand_map = 0;
	fz_he_rejected_city_demand = 0;
	fz_he_rejected_demand_advance = 0;
	fz_he_rejected_demand_stop_trade = 0;
	fz_he_rejected_demand_attack_enemy = 0;
	fz_he_rejected_demand_leave_our_lands = 0;
	fz_he_rejected_demand_reduce_pollution = 0;
	fz_he_rejected_offer_advance = 0;
	fz_he_rejected_offer_city = 0;
	fz_he_rejected_offer_map = 0;
	fz_he_rejected_offer_gold = 0;
	fz_he_rejected_offer_cease_fire = 0;
	fz_he_rejected_offer_alliance = 0;
	fz_he_rejected_offer_capture_city = 0;
	fz_he_rejected_offer_pact_end_pollution = 0;

	fz_he_rejected_exchange_advance = 0;
	fz_he_rejected_exchange_city = 0;
	fz_he_rejected_exchange_map = 0;
	fz_he_rejected_demand_no_piracy = 0;
}

BOOL FSDiplomat::HandleAcceptedMessage(AiMain *ai, IC3DiplomaticRequest *request)
{
	ClearFuzzyMessageReceived(ai);

	Foreigner *foreigner = ai->m_foreigner[request->GetRecipient()];
	if(!foreigner)
		return TRUE;
	foreigner->AcceptedRequest();

	Assert(foreigner);
	fz_diplomatic_regard_towards_him = foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard();

	REQUEST_TYPE r = request->GetRequest();
	foreigner->SetHisResponses(r, TRUE);
	switch(request->GetRequest()) {
		case REQUEST_TYPE_GREETING:
			fz_he_accepted_greetings = 1;
			ai->m_foreigner[request->GetRecipient()]->m_he_accepted_greetings = TRUE;
			break;
		case REQUEST_TYPE_DEMAND_MAP:
			fz_he_accepted_demand_map = 1;
			ai->m_foreigner[request->GetRecipient()]->SetGotMapFrom(ai->m_round_count->GetRound());
			break;
		case REQUEST_TYPE_DEMAND_ADVANCE:
			fz_he_accepted_demand_advance = 1;
			break;
		case REQUEST_TYPE_DEMAND_CITY:
			fz_he_accepted_city_demand = 1;
			break;
		case REQUEST_TYPE_DEMAND_GOLD:
			fz_he_accepted_gold_demand = 1;
			break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE:
			fz_he_accepted_demand_stop_trade = 1;
			break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
			fz_he_accepted_demand_attack_enemy = 1;
			break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS:
			fz_he_accepted_demand_leave_our_lands = 1;
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION:
			fz_he_accepted_demand_reduce_pollution = 1;
			break;
		case REQUEST_TYPE_OFFER_ADVANCE:
			fz_he_accepted_offer_advance = 1;
			break;
		case REQUEST_TYPE_OFFER_CITY:
			fz_he_accepted_offer_city = 1;
			break;
		case REQUEST_TYPE_OFFER_MAP:
			fz_he_accepted_offer_map = 1;
			break;
		case REQUEST_TYPE_OFFER_GOLD:
			fz_he_accepted_offer_gold = 1;
			break;
		case REQUEST_TYPE_OFFER_CEASE_FIRE:
			fz_he_accepted_offer_cease_fire = 1;
			if(foreigner->GetUnitRegard() < k_BELOW_COLD_WAR)
				foreigner->SetUnitRegard(k_BELOW_COLD_WAR);
			if(foreigner->GetDiplomaticRegard() < k_BELOW_COLD_WAR)
				foreigner->SetDiplomaticRegard(ai, k_BELOW_COLD_WAR);
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE:
			fz_he_accepted_offer_alliance = 1;
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY:
			fz_he_accepted_offer_capture_city = 1;
			break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION:
			fz_he_accepted_offer_pact_end_pollution = 1;
			break;



		case REQUEST_TYPE_EXCHANGE_ADVANCE:
			fz_he_accepted_exchange_advance = 1;
			break;
		case REQUEST_TYPE_EXCHANGE_CITY:
			fz_he_accepted_exchange_city = 1;
			break;
		case REQUEST_TYPE_EXCHANGE_MAP:
			fz_he_accepted_exchange_map = 1;
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			fz_he_accepted_demand_no_piracy = 1;
			break;
		default:
			
			break;
	}
	SetupForeignerVars(ai, foreigner, TRUE, FLI_SECT_ACCEPTED_MESSAGE);
	return TRUE;
}

BOOL FSDiplomat::HandleRejectedMessage(AiMain *ai, IC3DiplomaticRequest *request)
{
	ClearFuzzyMessageReceived(ai);

	Foreigner *foreigner = ai->m_foreigner[request->GetRecipient()];
	if(!foreigner)
		return TRUE;

	if (!foreigner) return FALSE;

	foreigner->RejectedRequest();
	
	REQUEST_TYPE r = request->GetRequest();
	foreigner->SetHisResponses(r, FALSE);

	switch(request->GetRequest()) {
		case REQUEST_TYPE_GREETING:
			
			
			ai->m_foreigner[request->GetRecipient()]->m_he_accepted_greetings = TRUE;
			fz_he_rejected_greetings = 1;
			break;
		case REQUEST_TYPE_DEMAND_ADVANCE:
			fz_he_rejected_demand_advance = 1;
			break;
		case REQUEST_TYPE_DEMAND_CITY:
			fz_he_rejected_city_demand = 1;
			break;
		case REQUEST_TYPE_DEMAND_MAP:
			fz_he_rejected_demand_map = 1;
			break;
		case REQUEST_TYPE_DEMAND_GOLD:
			fz_he_rejected_gold_demand = 1;
			break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE:
			fz_he_rejected_demand_stop_trade = 1;
			break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
			fz_he_rejected_demand_attack_enemy = 1;
			break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS:
			fz_he_rejected_demand_leave_our_lands = 1;
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION:
			fz_he_rejected_demand_reduce_pollution = 1;
			break;
		case REQUEST_TYPE_OFFER_ADVANCE:
			fz_he_rejected_offer_advance = 1;
			break;
		case REQUEST_TYPE_OFFER_CITY:
			fz_he_rejected_offer_city = 1;
			break;
		case REQUEST_TYPE_OFFER_MAP:
			fz_he_rejected_offer_map = 1;
			break;
		case REQUEST_TYPE_OFFER_GOLD:
			fz_he_rejected_offer_gold = 1;
			break;
		case REQUEST_TYPE_OFFER_CEASE_FIRE:
			fz_he_rejected_offer_cease_fire = 1;
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE:
			fz_he_rejected_offer_alliance = 1;
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY:
			fz_he_rejected_offer_capture_city = 1;
			break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION:
			fz_he_rejected_offer_pact_end_pollution = 1;
			break;



		case REQUEST_TYPE_EXCHANGE_ADVANCE:
			fz_he_rejected_exchange_advance = 1;
			break;
		case REQUEST_TYPE_EXCHANGE_CITY:
			fz_he_rejected_exchange_city = 1;
			break;
		case REQUEST_TYPE_EXCHANGE_MAP:
			fz_he_rejected_exchange_map = 1;
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			fz_he_rejected_demand_no_piracy = 1;
			break;


		default:
			break;
	}
	SetupForeignerVars(ai, foreigner, TRUE, FLI_SECT_REJECTED_MESSAGE);

	return TRUE;
}

double FSDiplomat::GetUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	switch(request->GetRequest()) {
		case REQUEST_TYPE_GREETING: return GreetingUtility(ai, request);

		
		case REQUEST_TYPE_DEMAND_ADVANCE: return DemandAdvanceUtility(ai, request);
		case REQUEST_TYPE_DEMAND_CITY: return DemandCityUtility(ai, request);
		case REQUEST_TYPE_DEMAND_MAP: return DemandMapUtility(ai, request);
		case REQUEST_TYPE_DEMAND_GOLD: return DemandGoldUtility(ai, request);
		case REQUEST_TYPE_DEMAND_STOP_TRADE: return DemandStopTradeUtility(ai, request);
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY: return DemandAttackEnemyUtility(ai, request);
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS: return DemandLeaveOurLandsUtility(ai, request);
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION: return DemandReducePollutionUtility(ai, request);
		case REQUEST_TYPE_DEMAND_NO_PIRACY: return DemandLowerTariffsUtility(ai, request);
		
		
		case REQUEST_TYPE_OFFER_ADVANCE: return OfferAdvanceUtility(ai, request);
		case REQUEST_TYPE_OFFER_CITY: return OfferCityUtility(ai, request);
		case REQUEST_TYPE_OFFER_MAP: return OfferMapUtility(ai, request);
		case REQUEST_TYPE_OFFER_GOLD: return OfferGoldUtility(ai, request);
		case REQUEST_TYPE_OFFER_CEASE_FIRE: return OfferCeaseFireUtility(ai, request);
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE: return OfferPermanentAllianceUtility(ai, request);
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY: return OfferPactCaptureCityUtility(ai, request);
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION: return OfferPactEndPollutionUtility(ai, request);


		
		case REQUEST_TYPE_EXCHANGE_ADVANCE: return ExchangeAdvanceUtility(ai, request);
		case REQUEST_TYPE_EXCHANGE_CITY: return ExchangeCityUtility(ai, request);
		case REQUEST_TYPE_EXCHANGE_MAP: return ExchangeMapUtility(ai, request);
		default:
			Assert(FALSE);
			return 0.0;
	}
}



double FSDiplomat::GreetingUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
    return k_AVERAGE_VALUE;
}



double FSDiplomat::DemandAdvanceUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	























	return k_AVERAGE_VALUE;
}



double FSDiplomat::DemandCityUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	if(ai->m_city_set->GetNum() < 2)
		
		return 0.0;

	uint32 wanted_city_id = request->GetTarget();
	CityAgent *wanted_city = ai->m_city_set->Find(wanted_city_id);
	Assert(wanted_city);
	if(!wanted_city)
		return k_WORST_VALUE;
	return 0.0;
	
}

double FSDiplomat::DemandMapUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	
	
	MapPointData *size = ai->m_map->GetSize();

	double explored = ai->m_map->ExploredArea(ai);
	
	
	return k_POOR_VALUE - k_AVERAGE_VALUE * (explored / (size->x * size->y));
}




double FSDiplomat::DemandGoldUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	
	double gold = request->GetGold();
	double savings = ai->m_gold->GetCurrentSavings();
	if(gold > savings )
		return 0.0;

	
	if (!ai->m_gold->CanSpendSpecialAction(ai, (sint32) gold))
		return 0.0;
		
	double goldpart = gold / savings;
	
	
	return k_AVERAGE_VALUE * goldpart;
}

double FSDiplomat::DemandStopTradeUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	
	double tradewiththird = ai->m_player->GetTradeWith(sint32(request->GetThirdParty()));
	
	
	return k_GOOD_VALUE - (tradewiththird * fz_trade_utility);
}

double FSDiplomat::DemandAttackEnemyUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	Foreigner *foreigner = ai->m_foreigner[request->GetThirdParty()];
	
	
	double regardthird = foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard(); 
	
	
	if (regardthird > 60)
	{
		return 0.0;
	}
	
	return k_BEST_VALUE - ((foreigner->GetDiplomaticRegard()  + foreigner->GetBonusRegard())* 10);
}

double FSDiplomat::DemandLeaveOurLandsUtility(AiMain *ai, IC3DiplomaticRequest *request)
	
{
	double mystrength = ai->m_gs->GetStrength(ai->m_my_player_id);
	if(mystrength == 0)
		mystrength = 0.001;
	double hisstrength = ai->m_gs->GetStrength(request->GetOwner());
	if(hisstrength == 0)
		hisstrength = 0.001;

	
	return k_AVERAGE_VALUE;
		
}


double FSDiplomat::DemandReducePollutionUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	double pollutiontrigger = ai->m_gs->GetPollutionTriggerLevel();
	double global_pollution = ai->m_gs->GetGlobalPollutionLevel();
		
	if(!pollutiontrigger)
		pollutiontrigger = 1;
    
	
	
	
	
	return k_AVERAGE_VALUE;

}



double FSDiplomat::DemandLowerTariffsUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	double mystrength = ai->m_gs->GetStrength(ai->m_my_player_id);
	if(mystrength == 0)
		mystrength = 0.001;
	double hisstrength = ai->m_gs->GetStrength(request->GetOwner());
	if(hisstrength == 0)
		hisstrength = 0.001;

	
	double globalTradeRoutes = ai->m_gs->GetGlobalTradeRoutes() * 2;
	double hisRelativeTradeRoutes = 
		double(ai->m_gs->GetHisTradeRoutes(request->GetOwner())) / 
		globalTradeRoutes;
	if(hisRelativeTradeRoutes < 1)
		hisRelativeTradeRoutes = 1; 

	
	
	return k_AVERAGE_VALUE;
}
		

double FSDiplomat::OfferAdvanceUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	
    return k_GOOD_VALUE;
}


double FSDiplomat::OfferCityUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
    
    return k_AVERAGE_VALUE;
}

double FSDiplomat::OfferMapUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
    
    return k_AVERAGE_VALUE;
}

double FSDiplomat::OfferGoldUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
    
	return k_AVERAGE_VALUE;
}

double FSDiplomat::OfferCeaseFireUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	Foreigner *foreigner = ai->m_foreigner[request->GetOwner()];

	

	double mystrength = ai->m_gs->GetStrength(ai->m_my_player_id);
	if(mystrength == 0)
		mystrength = 0.001;
	
	double hisstrength = ai->GetStrength(request->GetOwner());
	
	double hisregard = foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard();

	
	
	
	
	

	
	
	
	
	
	return k_GOOD_VALUE ;
}

double FSDiplomat::OfferPermanentAllianceUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	












	return k_AVERAGE_VALUE;
}

double FSDiplomat::OfferPactCaptureCityUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
    return k_AVERAGE_VALUE;
}

double FSDiplomat::OfferPactEndPollutionUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	sint32 pollutiontrigger = ai->m_gs->GetPollutionTriggerLevel();
	if(!pollutiontrigger)
		pollutiontrigger = 1;

	return k_AVERAGE_VALUE;
	
	
	
		
}

double FSDiplomat::ExchangeAdvanceUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	Foreigner *foreigner = ai->m_foreigner[request->GetOwner()];
	
	
	double hisgain = (ai->m_science->GetHisCost(foreigner->GetPlayerIndex(), request->GetWantedAdvance()));
	
	double hisstepsgained = (ai->m_science->GetHisMinPrerequisites(foreigner->GetPlayerIndex(), request->GetWantedAdvance()));
	
	double mygain = ai->m_science->GetCost(request->GetAdvance());
	
	double mystepsgained = ai->m_science->GetMinPrerequisites(request->GetAdvance());

	
	
	if(mygain > hisgain)
	{
		if(mystepsgained > hisstepsgained )

		{
			return k_BEST_VALUE;
		}else
		{
			return k_AVERAGE_VALUE;
		}
	}else
	{
		if(mystepsgained > hisstepsgained )
		{
			return k_POOR_VALUE;
		}else
		{
			return k_WORST_VALUE;
		}
	}
			
	return 0;
			
}

double FSDiplomat::ExchangeCityUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	if(ai->m_city_set->GetNum() < 2)
		return 0;
	else 
	
		return 0.0;
}

double FSDiplomat::ExchangeMapUtility(AiMain *ai, IC3DiplomaticRequest *request)
{
	Foreigner *foreigner = ai->m_foreigner[request->GetOwner()];
	MapPointData *size = ai->m_map->GetSize();
	sint32 worldArea = size->x * size->y;
	
	double regard = foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard();
	
	
	if(regard < 60)
		return 0.0;

	double known_percent = (ai->m_map->ExploredArea(ai) / worldArea);
	
	if(known_percent < 0.5)	return k_GOOD_VALUE;
	
	
	if(known_percent > 0.8) 
	{
		return k_POOR_VALUE;
	}

	
	return k_AVERAGE_VALUE;
}

void FSDiplomat::GetSendUtility(AiMain *ai,
								Foreigner *foreigner,
								AIDiplomaticRequest *request)
{
	
	
	
	
	request->WeighOptions(ai, foreigner, !m_registered_pre_outgoing);
	m_registered_pre_outgoing = TRUE;

	if(!request->m_isvalid)
		return;

	
	
	ai->m_fliEngine->FireRules(FLI_SECT_PRE_OUTGOING_DIPLOMACY);
	
	request->m_threshold = fz_send_threshold;
	request->m_dip_send_coef = fz_dip_send_coef;
	request->m_tone = (sint32)fz_send_tone;
	request->m_strength = (sint32)fz_send_strength;

	
	switch(request->m_request) {
		case REQUEST_TYPE_GREETING: request->m_utility = GreetingSendUtility(ai, foreigner, request); break;

		
		case REQUEST_TYPE_DEMAND_ADVANCE: request->m_utility = DemandAdvanceSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_CITY: request->m_utility = DemandCitySendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_MAP: request->m_utility = DemandMapSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_GOLD: request->m_utility = DemandGoldSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE: request->m_utility = DemandStopTradeSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY: request->m_utility = DemandAttackEnemySendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS: request->m_utility = DemandLeaveOurLandsSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION: request->m_utility = DemandReducePollutionSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY: request->m_utility = DemandLowerTariffsSendUtility(ai, foreigner, request); break;
		
		
		case REQUEST_TYPE_OFFER_ADVANCE: request->m_utility = OfferAdvanceSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_CITY: request->m_utility = OfferCitySendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_MAP: request->m_utility = OfferMapSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_GOLD: request->m_utility = OfferGoldSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_CEASE_FIRE: request->m_utility = OfferCeaseFireSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE: request->m_utility = OfferPermanentAllianceSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY: request->m_utility = OfferPactCaptureCitySendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION: request->m_utility = OfferPactEndPollutionSendUtility(ai, foreigner, request); break;


		
		case REQUEST_TYPE_EXCHANGE_ADVANCE: request->m_utility = ExchangeAdvanceSendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_EXCHANGE_CITY: request->m_utility = ExchangeCitySendUtility(ai, foreigner, request); break;
		case REQUEST_TYPE_EXCHANGE_MAP: request->m_utility = ExchangeMapSendUtility(ai, foreigner, request); break;
	}		
#ifdef _DEBUG
	double pre_utility = request->m_utility;
#endif
	request->m_utility *= fz_diplomatic_coeff[request->m_request]; 

	
	sint32 randRange = sint32(double(fz_diplomacy_rand_max - fz_diplomacy_rand_min) * 1000.0);
	if(randRange > 0) {
		double mult = (double(ai->m_rand->Next(randRange)) / 1000.0) + fz_diplomacy_rand_min;
		Assert(mult <= fz_diplomacy_rand_max); 
		request->m_utility *= mult;
	}

		
	WPRINT((wstr, "Request type %d to player %d: Raw utility: %#.3le.  Utility coef: %#.31e. Cooked utility: %#.3le.  Threshold: %#.3le.\n",
			request->m_request,
			request->m_recipient,
			pre_utility,
			request->m_dip_send_coef,
			request->m_utility,
			request->m_threshold));
}


double FSDiplomat::GreetingSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{

	if(!foreigner->m_sent_greetings)
		
		return 2000; 
	else
		return 0.0;
}



double FSDiplomat::DemandAdvanceSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
		return k_AVERAGE_VALUE;
}

double FSDiplomat::DemandCitySendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
 	double happy = request->m_target_city_ptr->ProjectedHappiness(ai);

	return -10; 

}

double FSDiplomat::DemandMapSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	MapPointData *size = ai->m_map->GetSize();
	
	double mapsize = (size->x * size->y);
	double explored = ai->m_map->ExploredArea(ai);
	
	

	return (1 - (ai->m_map->ExploredArea(ai) / (size->x * size->y)))
		* (1 
		- (ai->m_round_count->GetRound() / 
		(ai->m_round_count->GetRound() + ai->m_round_count->EstimateRoundsToEnd()))) 
		* k_AVERAGE_VALUE; 

}

double FSDiplomat::DemandGoldSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	if (fz_gold_gained * fz_gold_utility > 500)
		return k_AVERAGE_VALUE;
	if (fz_gold_gained * fz_gold_utility > 5000)
		return k_GOOD_VALUE;
	return 0.0;
}

double FSDiplomat::DemandStopTradeSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	return k_AVERAGE_VALUE;










}

double FSDiplomat::DemandAttackEnemySendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	

	double hisstrength = ai->GetStrength(request->m_recipient);
	double thirdstrength = ai->GetStrength(request->m_thirdParty);
	double hisregard = (foreigner->GetDiplomaticRegard() - 75);
	double thirdregard = ai->m_foreigner[request->m_thirdParty]->GetDiplomaticRegard();
	
	
	
	if ((ai->m_foreigner[request->m_thirdParty]->GetUnitRegard() + 
        ai->m_foreigner[request->m_thirdParty]->GetBonusRegard()) < 20 )
	{
			return k_GOOD_VALUE;
	}
	return 0;	
}

double FSDiplomat::DemandLeaveOurLandsSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	
	double mystrength = ai->GetStrength(ai->my_player_index);
	double hisstrength = ai->GetStrength(foreigner->GetPlayerIndex());




	return k_AVERAGE_VALUE;




}

double FSDiplomat::DemandReducePollutionSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{

	double pollutiontrigger = ai->m_gs->GetPollutionTriggerLevel();	

	if(!pollutiontrigger)
		pollutiontrigger = 1;
	

	double globalpollution = ai->m_gs->GetGlobalPollutionLevel();
	if(globalpollution < 1)
		globalpollution = 0.1;

	if(!fz_his_pollution_level)
		fz_his_pollution_level = 0.1; 	

	
	if((globalpollution / pollutiontrigger ) > 0.8) 
	{
		if(fz_his_pollution_level / globalpollution > 0.8) return k_BEST_VALUE;
		if(fz_his_pollution_level / globalpollution > 0.6) return k_GOOD_VALUE;
		if(fz_his_pollution_level / globalpollution > 0.4) return k_AVERAGE_VALUE;
		if(fz_his_pollution_level / globalpollution > 0.2) return k_POOR_VALUE;
		return k_WORST_VALUE;
	}
	return -10;
}


double FSDiplomat::DemandLowerTariffsSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	
	double traderoutes = ai->m_player->GetNumTradeRoutes();
	double hisPiratedMe = foreigner->GetDecayedPiracy();


	if(hisPiratedMe > 4)
	{
		return k_BEST_VALUE;
	}
	if(hisPiratedMe > 2)
	{
		return k_AVERAGE_VALUE;
	}
	if(hisPiratedMe > 0.75)
	{
		return k_POOR_VALUE;
	}
	return -10;
}
		

double FSDiplomat::OfferAdvanceSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)

{






	if(fz_advances_skipped < 2 )	return k_BEST_VALUE;
	if(fz_advances_skipped < 5 )	return k_GOOD_VALUE;
	if(fz_advances_skipped < 8 )	return k_AVERAGE_VALUE;
	if(fz_advances_skipped < 10 )	return k_POOR_VALUE;
	if(fz_advances_skipped >= 10 )	return k_WORST_VALUE;
	return -10;
}


double FSDiplomat::OfferCitySendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	if(ai->m_city_set->GetNum() < 2)
		return 0;

	
	return -1000;
}

double FSDiplomat::OfferMapSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	
	return k_POOR_VALUE;

}

double FSDiplomat::OfferGoldSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	
	
	if ((foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard()) > 70)
	{
		return (k_AVERAGE_VALUE * fz_gold_need);
	}
	return 0;
}


double FSDiplomat::OfferCeaseFireSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
	{
	double mydefense = ai->GetMyDefense();
	if(mydefense == 0)
		mydefense = 0.001;
	double hisdefense = foreigner->GetDefense();
	if(hisdefense == 0)
		hisdefense = 0.001;
	double defeatedhim = foreigner->IDefeatedHimRecently();
	if(defeatedhim == 0)
		defeatedhim = 0.001;

	
	double mystrength = ai->GetStrength(ai->m_my_player_id);
	double hisstrength = ai->GetStrength(foreigner->GetPlayerIndex());

	
	fz_length_of_war_with_him = foreigner->GetLengthOfWarWithHim(ai);
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	
	
	return k_GOOD_VALUE;
}

double FSDiplomat::OfferPermanentAllianceSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	double mystrength = ai->GetStrength(ai->m_my_player_id);
	if(mystrength == 0)
		mystrength = 0.001;

	
	double hisstrength = ai->GetStrength(foreigner->GetPlayerIndex());
	
	
	
	if (((foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard()) > 50) && (fz_loyalty > 30))
	{
		return (ai->GetStrength(foreigner->GetPlayerIndex()) / mystrength) * 250 * fz_alliance;
	}
	
	
	return 0.0;
}


double FSDiplomat::OfferPactCaptureCitySendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
    return 0.0;
}

double FSDiplomat::OfferPactEndPollutionSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{

	double global_pollution = ai->m_gs->GetGlobalPollutionLevel();
	double pollutiontrigger = ai->m_gs->GetPollutionTriggerLevel();
	if(!pollutiontrigger)
		pollutiontrigger = 1;

	
	if(fz_pact_end_pollution > 500)  
	{
	
	

	return (global_pollution / pollutiontrigger) * 	fz_pact_end_pollution;
	}
	return 0;
}












double FSDiplomat::ExchangeAdvanceSendUtility(AiMain *ai, Foreigner *foreigner, AIDiplomaticRequest *request)
{
	
	double hiscost = (ai->m_science->GetHisCost(foreigner->GetPlayerIndex(), request->m_advance));
	double mycost =(ai->m_science->GetCost(request->m_reciprocalAdvance)); 
	double hisskipped = ai->m_science->GetHisMinPrerequisites(foreigner->GetPlayerIndex(), request->m_advance);
	double myskipped = ai->m_science->GetMinPrerequisites(request->m_reciprocalAdvance);
	double regard_him = foreigner->GetDiplomaticRegard();

	
	if(hiscost > mycost)
	{
		if(hisskipped > myskipped)
		{
			return k_WORST_VALUE;
		}else
		{
			return k_POOR_VALUE;
		}
	}else
	{
		if(hisskipped > myskipped)
		{
			return k_AVERAGE_VALUE;
		}else
		{
			return k_GOOD_VALUE;
		}
	}		
}

double FSDiplomat::ExchangeCitySendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	if(ai->m_city_set->GetNum() < 2)
		return 0;
	else 
		return -1;
}

double FSDiplomat::ExchangeMapSendUtility(AiMain *ai,  Foreigner *foreigner, AIDiplomaticRequest *request)
{
	MapPointData *size = ai->m_map->GetSize();
	double worldArea = size->x * size->y;
	double exploredArea = ai->m_map->ExploredArea(ai);
	double regard_to_him = foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard();

	if(regard_to_him < 70) return 0;

	double percent_explored = exploredArea / worldArea;
	if(percent_explored < .3) return k_GOOD_VALUE; 
	if(percent_explored > .8) return k_POOR_VALUE;

	return k_AVERAGE_VALUE;
}

void FSDiplomat::ReceiveTradeBid(AiMain *ai,
								 uint32 fromCity,
								 sint32 resource,
								 sint32 toCityOwner,
								 uint32 toCity,
								 sint32 price,
								 uint32 bidId)
{
    BOOL is_unknown_id; 
    sint32 local_good_count; 
    sint32 total_good_count; 

    ai->m_player->GetGoodCount(&is_unknown_id, fromCity,  
        resource, &local_good_count, &total_good_count);

	BOOL accept = FALSE;
    sint32 in_num; 
    sint32 in_max; 
    sint32 out_num; 
    sint32 out_max;
    BOOL is_pirated;
	if ((local_good_count == 1) && (total_good_count == 1)) {

        ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, fromCity, &is_unknown_id, 
            &in_num, &in_max, &out_num, &out_max);

        CityAgent *the_city = ai->m_city_set->Find(fromCity); 
        Assert(the_city); 
        if (the_city) {

            Pirate_Marker *pirate_ptr = the_city->FindPiracyMarker(ai->my_player_index, fromCity);
            if (pirate_ptr) { 
                if (pirate_ptr->IsSafe(ai->m_round_count->GetRound())) is_pirated=TRUE; 
            }
			else
				is_pirated = FALSE;

            if (!is_pirated) { 
		        if((ai->m_foreigner[toCityOwner]->GetDiplomaticRegard() +
                    ai->m_foreigner[toCityOwner]->GetBonusRegard()) >= k_TRADE_REGARD_THRESHOLD) {
			        sint32 valueToMe = ai->m_terrainDB->GetGoodGoldValue(resource);
			        if(price >= valueToMe + 10) {
				        accept = TRUE;
			        }
		        }
            }
        }
	}

	if(accept) {


        ai->AcceptTradeBid(bidId);












		
	} else {
		ai->RejectTradeBid(bidId);
	}
}

void FSDiplomat::RegisterTradeOffer(AiMain *ai, IC3TradeOffer *offer)
{
	offer->AddRef();
	m_tradeOffers->AddTail(offer);
	if(ai->my_player_index == offer->GetOwner()) {
		uint32 fromCity = offer->GetFromCity();
		CityAgent *city = ai->m_city_set->Find(fromCity);
		Assert(city);
		if(city) {
			city->AddTradeOffer(offer);
		}
	}
}

void FSDiplomat::RemoveTradeOffer(AiMain *ai, uint32 id)
{
	PointerList<IC3TradeOffer>::Walker walk(m_tradeOffers);
	for(walk.SetList(m_tradeOffers); walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->GetID() == id) {
			if(walk.GetObj()->GetOwner() == ai->my_player_index) {
				uint32 fromCity = walk.GetObj()->GetFromCity();
				if(fromCity) {
					CityAgent *city = ai->m_city_set->Find(fromCity);
					
					
					if(city) {
						city->RemoveTradeOffer(id);
					}
				}
			}
					
			walk.GetObj()->Release();
			walk.Remove();
			break;
		}
	}
}

void FSDiplomat::ConsiderTradeOffers(AiMain *ai)
{
	PointerList<IC3TradeOffer>::Walker walk;
	IC3TradeOffer *bestOffer = NULL;
	CityAgent *bestCity = NULL;
	sint32 bestProfit = 0;

	if(ai->m_player->UnusedTradePoints() < 1)
		return;

	
    BOOL is_unknown_id;
    sint32 in_num; 
    sint32 in_max;
    sint32 out_num; 
    sint32 out_max; 

	for(walk.SetList(m_tradeOffers); walk.IsValid(); walk.Next()) {		
		IC3TradeOffer *offer = walk.GetObj();

		sint32 resource = offer->GetOfferResource();

		
		
		if(offer->GetOwner() == ai->my_player_index)
			continue;

		Foreigner *foreigner = ai->m_foreigner[offer->GetOwner()];

		
		if((foreigner->GetDiplomaticRegard() +  foreigner->GetBonusRegard()) < k_TRADE_REGARD_THRESHOLD)
			continue;

		switch(offer->GetOfferType()) {
			case ROUTE_TYPE_RESOURCE:
			{
				

				
				CityAgent *city = ai->GetBestCityForResource(resource);
               
				if(!city)
					continue;

				if(!city->SeenByPlayer(ai, offer->GetOwner()))
					continue;

                 ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, city->GetID().GetVal(), &is_unknown_id, 
                    &in_num, &in_max, &out_num, &out_max);

                 if (in_num >= in_max) continue; 

                Pirate_Marker *pirate_ptr = city->FindPiracyMarker(offer->GetOwner(), offer->GetFromCity());
                if (pirate_ptr) { 
                    if (!pirate_ptr->IsSafe(ai->m_round_count->GetRound())) continue; 
                }


				
                BOOL is_unknown_id; 
                sint32 local_good_count;
                sint32 total_good_count; 
				ai->m_player->GetGoodCount(&is_unknown_id, city->m_id.GetVal(), 
                    resource, &local_good_count, &total_good_count); 

				sint32 currentValue = sint32((total_good_count * (total_good_count + 1)) * 0.5 ) *
					ai->m_terrainDB->GetGoodGoldValue(resource);

				total_good_count++;
				sint32 newValue = sint32(((total_good_count * (total_good_count + 1)) * 0.5) *
					ai->m_terrainDB->GetGoodGoldValue(resource));

				sint32 delta = newValue - currentValue;

				
				
				if(delta > offer->GetAskingGold()) {
					if(delta > bestProfit) {
						bestProfit = delta;
						bestCity = city;
						bestOffer = offer;
					}
				}
				break;
			}
			default:
				break;
		}
	}

	if(bestOffer && bestProfit >= 10) {
		sint32 owner = bestOffer->GetOwner();
		bestOffer->Accept(bestCity->m_id.GetVal());
		
		
		
		
		if (!ai->m_player->IsRobotPlayer(owner))
			ai->SetCaravansReserved(TRUE);
	}
}

IC3TradeOffer *FSDiplomat::AlreadyOffering(AiMain *ai, 
										   CityAgent *city, 
										   sint32 resource)
{
	if(!city->GetTradeOffers())
		return NULL;

	PointerList<IC3TradeOffer>::Walker walk;
	for(walk.SetList(city->GetTradeOffers()); walk.IsValid(); walk.Next()) {
		if(resource == walk.GetObj()->GetOfferResource()) {
			return walk.GetObj();
		}
	}
	return NULL;
}

void FSDiplomat::ConsiderMakingTradeOffers(AiMain *ai)
{
	BSetID id;
	CityAgent *city;
	sint32 g;
	sint32 numTypes = ai->m_terrainDB->GetNumTypeGoods();
	BOOL madeAnOffer = FALSE;
    BOOL is_unknown_id; 
    sint32 local_good_count, total_good_count;

    sint32 in_num;
    sint32 in_max;
    sint32 out_num;
    sint32 out_max;

	for(city = ai->m_city_set->First(id); ai->m_city_set->Last();
		city = ai->m_city_set->Next(id)) {
		for(g = 0; g < numTypes; g++) {

			IC3TradeOffer *offer = AlreadyOffering(ai, city, g);

            ai->m_player->GetGoodCount(&is_unknown_id, city->m_id.GetVal(), g, 
                &local_good_count, &total_good_count);

            ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, city->m_id.GetVal(), &is_unknown_id, 
                &in_num, &in_max, &out_num, &out_max);

            if (out_num >= out_max) continue; 

            if (city->PiracyRecently(ai)) continue; 

			if ((local_good_count  == 1) && (total_good_count == 1)){
				if(!madeAnOffer) {
					
					
					if(!offer) {
						ai->m_player->MakeTradeOffer(city->m_id.GetVal(),
							g,
							ai->m_terrainDB->GetGoodGoldValue(g) + 10);
						
						madeAnOffer = TRUE;
					}
				}
			} else if(offer) {
				
				
				ai->m_player->CancelTradeOffer(offer);
			}
		}
	}
}

BOOL FSDiplomat::ConfirmAcceptTradeOffer(AiMain *ai,
	uint32 offerId, uint32 destCityId, PLAYER_INDEX player)
{
	PointerList<IC3TradeOffer>::Walker walk;
	IC3TradeOffer *offer = NULL;
	for(walk.SetList(m_tradeOffers); walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->GetID() == offerId) {
			offer = walk.GetObj();
			break;
		}
	}
	if(!offer)
		return FALSE;

	Assert(offer->GetOwner() == ai->my_player_index);
	if(offer->GetOwner() != ai->my_player_index)
		return FALSE;

	Foreigner *foreigner = ai->m_foreigner[player];
	if((foreigner->GetDiplomaticRegard() + foreigner->GetBonusRegard()) < k_TRADE_REGARD_THRESHOLD)
		return FALSE;

	uint32 fromCityId = offer->GetFromCity();
	CityAgent *fromCityAgent = ai->m_city_set->Find(fromCityId);













	
	
	return TRUE;
}

void FSDiplomat::ConsiderMakingTradeBids(AiMain *ai)
{
	if(ai->m_player->UnusedTradePoints() < 1)
		
		return;


	
	BSetID id;
	CityAgent *myCity;
	sint32 g;
	sint32 numTypes = ai->m_terrainDB->GetNumTypeGoods();
	sint32 *maxCount = new sint32[numTypes];
	CityAgent **maxCity = new CityAgent *[numTypes];
    BOOL is_unknown_id;
    sint32 local_good_count; 
    sint32 total_good_count;
    sint32 in_num, in_max, out_num, out_max;

	memset(maxCount, 0, sizeof(sint32) * numTypes);

	for(myCity = ai->m_city_set->First(id); ai->m_city_set->Last(); myCity = ai->m_city_set->Next(id)) {

        if (myCity->GetNumCitizens() < 3) continue; 

        if (myCity->PiracyRecently(ai)) continue;  

		for(g = 0; g < numTypes; g++) {

            ai->m_player->GetGoodCount(&is_unknown_id, myCity->m_id.GetVal(), g, 
                &local_good_count, &total_good_count);

            if ((local_good_count == 1) && (total_good_count == 1)) { 
                ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, myCity->GetID().GetVal(), &is_unknown_id, 
                        &in_num, &in_max, &out_num, &out_max);

                if ((in_num < in_max) && (total_good_count > maxCount[g])) {
				        maxCount[g] = total_good_count;
				        maxCity[g] = myCity;
                }
            } 
		}
	}

	sint32 bestBidDelta = 0;
	CityAgent *bestBidCity = NULL;
	sint32 bestBidGood;
	ForeignCity *bestBidForeignCity = NULL;
	sint32 bestoffer;
  
	sint32 p;
	
	for(p = 1; p < k_MAX_PLAYERS; p++) {
		if(p == ai->my_player_index)
			continue;		
		if(ai->m_foreigner[p]) {
			Foreigner *foreigner = ai->m_foreigner[p];
			if(foreigner->GetTradePersistence() > 0) {
				foreigner->SetTradePersistence(foreigner->GetTradePersistence() - 1);
				continue;
			}
			
			if(foreigner->GetDiplomaticRegard() < k_TRADE_REGARD_THRESHOLD)
				continue;
			
			BSet<ForeignCity> *knownCities = foreigner->GetKnownCities();
			ForeignCity *city;
			BSetID cityId;
			for(city = knownCities->First(cityId);
				knownCities->Last();
				city = knownCities->Next(cityId)) {

				if(!ai->m_player->CanSeeHisCity(city->m_id.GetVal(), p))
					continue;

                city->GetCityNumTradeRoutes(ai, in_num, in_max, out_num, out_max);

                if (in_num >= in_max) continue; 
                if (out_num >= out_max) continue;  

				for(g = 0; g < numTypes; g++) {
					if(!maxCount[g])
						continue;
					
					sint32 goodCount = city->GetGoodCount(ai, g);
					if(goodCount) {
						
						
						
						sint32 num = maxCount[g];
						sint32 oldValue = ((num * (num + 1)) / 2) * 
							ai->m_terrainDB->GetGoodGoldValue(g);
						num++;
						sint32 newValue = ((num * (num + 1)) / 2) *
							ai->m_terrainDB->GetGoodGoldValue(g);
						
						
						sint32 delta = newValue - oldValue;

						
						num = goodCount;
						sint32 hisnowValue = ((num * (num + 1)) / 2) * 
							ai->m_terrainDB->GetGoodGoldValue(g);
						num--;
						sint32 hisnewValue = ((num * (num + 1)) / 2) *
							ai->m_terrainDB->GetGoodGoldValue(g);
						sint32 hisgoodvalue = hisnowValue - hisnewValue;
						if(delta < hisgoodvalue + 10) 
						{
							delta = 0;
						}
						if(delta > bestBidDelta) {
							bestBidDelta = delta;
							bestBidGood = g;
							bestBidCity = maxCity[g];
							bestBidForeignCity = city;
							bestoffer = hisgoodvalue + 10; 
						}
					}
				}
			}
		}
	}
	
	if(bestBidDelta > 0 && bestBidCity && bestBidForeignCity) {
		ai->m_gs->MakeTradeBid(bestBidForeignCity->GetPlayerIndex(),
							   bestBidForeignCity->m_id.GetVal(),
							   bestBidGood,
							   bestoffer,
							   bestBidCity->m_id.GetVal());
		ai->m_foreigner[bestBidForeignCity->GetPlayerIndex()]->
			SetTradePersistence(fz_diplomatic_persistence);
	}
	delete [] maxCount;
    maxCount = NULL; 
	delete [] maxCity;
    maxCity = NULL; 
}

void FSDiplomat::RegisterYouAreViolatingAgreement(AiMain *ai,
												  AGREEMENT_TYPE agreement,
												  PLAYER_INDEX against,
												  sint32 rounds)
{
	
}

void FSDiplomat::RegisterOtherCivIsViolatingAgreement(AiMain *ai,
													  AGREEMENT_TYPE agreement,
													  PLAYER_INDEX violator,
													  sint32 rounds)
{
	Foreigner *foreigner = ai->m_foreigner[violator];
	if(!foreigner) {
		
		return;
	}

	switch(agreement) {
		case AGREEMENT_TYPE_DEMAND_STOP_TRADE:
			foreigner->SetViolatingStopTrade(rounds);
			break;
		case AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS:
			foreigner->SetViolatingLeaveOurLands(rounds);
			break;
		case AGREEMENT_TYPE_REDUCE_POLLUTION:
			foreigner->SetViolatingReducePollution(rounds);
			break;
		case AGREEMENT_TYPE_PACT_END_POLLUTION:
			foreigner->SetViolatingEndPollution(rounds);
			break;
		case AGREEMENT_TYPE_DEMAND_ATTACK_ENEMY:
			foreigner->SetViolatingAttackEnemy(rounds);
			break;
	}
}



void FSDiplomat::RegisterAttack(AiMain *ai,
								PLAYER_INDEX attacker,
								PLAYER_INDEX defender)
{
	if(attacker == ai->my_player_index ||
	   defender == ai->my_player_index) {
		
		return;
	}

	Foreigner *a = ai->m_foreigner[attacker];
	Foreigner *d = ai->m_foreigner[defender];
	Assert(a);
	Assert(d);
	if(!a || !d) {
		return;
	}


	if(d->IsAtHotWar() || d->IsAtColdWar()) {
		a->RegisterAttackedMyEnemy();
	} else {
		a->RegisterAttackedMyFriend();
	}
}

void FSDiplomat::RegisterBrokeTreaty(AiMain *ai,
									 PLAYER_INDEX breaker,
									 PLAYER_INDEX breakee)
{
	if(breaker == ai->my_player_index) {
		
		return;
	}
	
	
	Foreigner *foreigner = ai->m_foreigner[breaker];
	Assert(foreigner);
	if(foreigner) {
		foreigner->RegisterHeBrokeATreaty(breakee);
	}
}
