
#include "c3.h"
#include "Globals.h"
#include "IC3Science.h"
#include "IC3Player.h"
#include "IC3Rand.h"

#include "AIDipReq.h"
#include "AiMain.h"
#include "Foreigner.h"
#include "FzDipVars.h"
#include "Gold.h"
#include "BSet.h"
#include "ForeignCity.h"
#include "AiMap.h"
#include "AiCell.h"
#include "AIRndCnt.h"
#include "CityAgent.h"

#include "FliEngine.h"
#include "FliSymbol.h"

#define REG(v) if(ai->m_fliEngine->GetSymbol(v)) { ai->m_fliEngine->GetSymbol(v)->RegisterInit(FLI_SECT_PRE_OUTGOING_DIPLOMACY); } else { Assert(ai->m_fliEngine->GetSymbol(v)); }







void AIDiplomaticRequest::WeighOptions(AiMain *ai, Foreigner *foreigner, BOOL firstCall)
{
	if(firstCall) {
		REG("contact_gained");
		REG("gold_gained");
		REG("reduces_incursions");
		REG("advances_skipped");
		REG("advance_cost");
		REG("projected_food");
		REG("projected_production");
		REG("map_area_gained");
		REG("last_got_map");
		REG("third_party_strength");
		REG("third_party_regard");
		REG("his_trade_with_third_party");
		REG("he_gains_advance");
		REG("his_route_threat");
		REG("his_pollution_level");
		REG("my_pollution_level");
		REG("global_pollution_level");
		REG("he_stops_polluting");
		REG("projected_city_happiness");
		REG("projected_food");
		REG("projected_production");
		REG("he_gets_map");
		REG("he_gains_gold");
		REG("gains_cease_fire");
		REG("gains_alliance");
		REG("he_gains_city");
		REG("i_lose_city");
		REG("end_pollution_pact");
		REG("he_attacks_enemy");
		REG("i_gain_advance");
		REG("i_gain_gold");
		REG("i_gain_map");	
		REG("i_gain_advance");
		REG("he_stops_trade_with_third_party");
		REG("third_party_relative_strength");
		REG("he_will_stop_piracy");
		REG("his_third_party_relative_strength");
	}

	
	fz_contact_gained = 0;
	fz_gold_gained = 0;
	fz_reduces_incursions = 0;
	fz_advances_skipped = 0;
	fz_advance_cost = 0;
	fz_projected_food = 0;
	fz_projected_production = 0;
	fz_map_area_gained = 0;
	fz_last_got_map = 0;
	fz_third_party_strength = 0;
	fz_third_party_regard = 0;
	fz_at_war_with_third_party = 0; 
	fz_his_trade_with_third_party = 0;
	fz_he_gains_advance = 0;
	fz_his_route_threat = 0;
	fz_his_pollution_level = 0;
	fz_my_pollution_level = 0;
	fz_global_pollution_level = 0;
	fz_he_stops_polluting = 0;
	fz_projected_city_happiness = 0;
	fz_projected_food = 0;
	fz_projected_production = 0;
	fz_he_gets_map = 0;
	fz_he_gains_gold = 0;
	fz_gains_cease_fire = 0;
	fz_gains_alliance = 0;
	fz_he_gains_city = 0;
	fz_i_lose_city = 0;
	fz_end_pollution_pact = 0;
	fz_reduces_pollution = 0;
	fz_he_attacks_enemy = 0;
	fz_i_gain_advance = 0;
	fz_i_gain_map = 0;
	fz_i_gain_gold = 0;
	fz_he_stops_trade_with_third_party = 0;
	fz_third_party_relative_strength = 0;
	fz_he_will_stop_piracy = 0;
	fz_his_third_party_relative_strength = 0;

	int rand;
	
	
	
	m_isvalid = FALSE;

	double regard =	foreigner->GetDiplomaticRegard();

	switch(m_request) {
		case REQUEST_TYPE_GREETING:
			if(!foreigner->m_he_accepted_greetings) {
				fz_contact_gained = 100;
				m_isvalid = TRUE;
			}
			break;
			
		
		case REQUEST_TYPE_DEMAND_ADVANCE:
		{
			if(ai->m_player->HasEmbassyWith(foreigner->GetPlayerIndex())) {
				sint32 numAdvances = ai->m_science->GetNumAdvances();
				uint8 *canaskfor = new uint8[numAdvances];
				sint32 num = ai->m_science->GetCanAskFor(foreigner->GetPlayerIndex(), canaskfor);
				sint32 checked = 0;
				sint32 i;
				sint32 best = -1;
				sint32 bestskipped = 0;
				sint32 skipped;

				for(i = 0; i < numAdvances; i++) {
					if(!canaskfor[i])
						continue;

					skipped = ai->m_science->GetMinPrerequisites(i);
					if(skipped > bestskipped) {
						best = i;
						bestskipped = skipped;
					}
					checked++;
					if(checked >= num)
						break;
				}

				if(best >= 0) {
					fz_advances_skipped = bestskipped;
					fz_advance_cost = ai->m_science->GetCost(best);
					fz_i_gain_advance = 1;
					m_advance = best;
					m_isvalid = TRUE;
				}

				delete [] canaskfor;
                canaskfor = NULL; 
			}
			break;

		}
			
		case REQUEST_TYPE_DEMAND_CITY:
		{
			break; 
			BSet<ForeignCity> *vc = NULL;
			BSetID c_id;
			ForeignCity *his_city = NULL;
			sint16 max_value = 0;
			BSetID max_city;


			
			vc = foreigner->GetKnownCities();
			for(his_city = vc->First(c_id); vc->Last(); his_city = vc->Next(c_id)) {
				
				MapPointData pos;
				his_city->GetPos(pos);
			
				sint16 target_value = sint16(his_city->GetPopulation(ai)); 
				if(target_value >= max_value) {
					max_value = target_value;
					max_city = c_id;
					fz_projected_food = his_city->GetFood(ai);
					fz_projected_production = his_city->GetProduction(ai);
					m_target_city_ptr = his_city;
				}
			}

			if(max_value > 0) {
				m_isvalid = TRUE;
				m_targetCity = max_city.GetVal();
			}
			break;
		}
		case REQUEST_TYPE_DEMAND_MAP:
			m_isvalid = TRUE;
			fz_map_area_gained = 1;
			fz_i_gain_map = 1;
			fz_last_got_map = (sint32)ai->m_round_count->GetRound() - foreigner->GotMapFrom();
			break;
		case REQUEST_TYPE_DEMAND_GOLD:
		{
			double gold;
			rand = (ai->m_rand->Next(100))+50; 
			gold = double(foreigner->GetEstimatedGold() / 4); 
			if(gold < 100)
			{
				gold = 100;
			}
			gold = double(gold / double(rand)); 
			gold = gold * 100; 
			
			if(gold > 10000) gold = 10000;
			if(ai->m_gold->GetCurrentSavings() == 0) {
				fz_gold_gained = 100;
			} else {
				
				
				fz_gold_gained = (double(m_gold) / double(ai->m_gold->GetCurrentSavings())) * 100.0;
			}
			if(gold > 25) {
				m_gold = sint32(gold);
				m_isvalid = TRUE;
			}

			break;
		}
		case REQUEST_TYPE_DEMAND_STOP_TRADE:
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
		{
			
			
			PLAYER_INDEX third;
			double minRegard = 100;
			sint32 minThird = -1;

			for(third = 1; third < k_MAX_PLAYERS; third++) {
				if (!ai->m_foreigner[third])
					continue;
				if(third == ai->my_player_index ||
				   third == foreigner->GetPlayerIndex() ||
				   !ai->m_foreigner[third]->HaveContact()) {
					continue;
				}
				
				
				if((ai->m_foreigner[third]->GetDiplomaticRegard() < minRegard )
						&& 
					(ai->m_player->GetWar(
						ai->my_player_index, 
						foreigner->GetPlayerIndex(), 
						ai->m_foreigner[third]->GetPlayerIndex())
					))
				{
					minThird = third;
					minRegard = ai->m_foreigner[third]->GetDiplomaticRegard();
				}
			}
			if(minThird >= 0) {
				m_thirdParty = minThird;
				m_isvalid = TRUE;
				fz_third_party_strength = ai->GetStrength(m_thirdParty);
				fz_third_party_regard = minRegard;
				fz_at_war_with_third_party = 
						ai->m_foreigner[m_thirdParty]->UpdateWar(ai, ai->my_player_index);

				double hs;
				if(fz_third_party_strength < 1) {
					hs = 1;
				} else {
					hs = fz_third_party_strength;
				}
				fz_third_party_relative_strength = ai->GetStrength(ai->my_player_index) / hs;
				fz_his_third_party_relative_strength = ai->GetStrength(foreigner->GetPlayerIndex()) / hs;

				if(m_request == REQUEST_TYPE_DEMAND_STOP_TRADE) {
					fz_his_trade_with_third_party = ai->GetTradeWith(m_recipient, m_thirdParty);
					if(fz_his_trade_with_third_party < 1)
						m_isvalid = FALSE;

					fz_he_stops_trade_with_third_party = 1;
				} else {
					fz_he_attacks_enemy = 1;
				}
			}
			break;
		}
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS:
			fz_reduces_incursions = fz_military_incursions;
			m_isvalid = TRUE;
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION:
			fz_his_pollution_level = foreigner->GetPollutionLevel();
			fz_my_pollution_level = ai->m_player->GetPollutionLevel();
			fz_global_pollution_level = ai->m_gs->GetGlobalPollutionLevel();
			fz_he_stops_polluting = 1;
			if(fz_his_pollution_level > 1)
				m_isvalid = TRUE;
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			
			fz_his_route_threat = foreigner->GetTradeThreat();
			fz_he_will_stop_piracy = 1;
			fz_my_num_trade_routes = ai->m_player->GetNumTradeRoutes();
			m_isvalid = TRUE;
			break;

		
		case REQUEST_TYPE_OFFER_ADVANCE:
		{
			if(ai->m_player->HasEmbassyWith(foreigner->GetPlayerIndex())) {
				sint32 numAdvances = ai->m_science->GetNumAdvances();
				uint8 *canoffer = new uint8[numAdvances];
				sint32 *advancesSkipped = new sint32[numAdvances];
				sint32 num = ai->m_science->GetCanOffer(foreigner->GetPlayerIndex(), canoffer, advancesSkipped);
				sint32 checked = 0;
				sint32 i;
				sint32 best = -1;
				sint32 bestskipped = 0;

				for(i = 0; i < numAdvances; i++) {
					if(!canoffer[i]) {
						continue;
					}
					
					
					if(advancesSkipped[i] > bestskipped) {
						best = i;
						bestskipped = advancesSkipped[i];
					}
					checked++;
					if(checked >= num)
						break;
				}
				if(best >= 0) {
					fz_he_gains_advance = 1;
					fz_advances_skipped = bestskipped;
					m_advance = best;
					m_isvalid = TRUE;
				}
				delete [] canoffer;
                canoffer = NULL; 
				delete advancesSkipped;
                advancesSkipped = NULL; 
			}
			break;
		}
		case REQUEST_TYPE_OFFER_CITY:
		{
			break; 
			BSetID id;
			CityAgent *the_city;
			CityAgent *the_nearest_city = NULL;
			double nearestToHim = 1000000;
			MapPointData center;
			foreigner->GetCenterOfMass(center);
			MapPointData pos;
			double dist;

			for(the_city = ai->m_city_set->First(id); ai->m_city_set->Last(); the_city = ai->m_city_set->Next(id)) {
				the_city->GetPos(pos);
				dist = ai->m_map->WrappedDistance(pos, center);
				if(dist < nearestToHim) {
					nearestToHim = dist;
					the_nearest_city = the_city;
					BOOL is_unknown;
					uint32 cid = the_city->m_id.GetVal();
					sint32 oldhap;
					double crime;
					ai->m_player->GetCityHappiness(cid, &is_unknown, &oldhap, &crime);
					fz_projected_city_happiness = oldhap;
					fz_projected_food = ai->m_player->CityGetNetFood(cid, &is_unknown);
					fz_projected_production = ai->m_player->CityGetNetProduction(cid, &is_unknown);
				}
			}
			if(the_nearest_city) {
				m_targetCity = the_nearest_city->m_id.GetVal();
				fz_he_gains_city = 1;
				fz_i_lose_city = 1;
				m_isvalid = TRUE;
			}
			break;
		}

		case REQUEST_TYPE_OFFER_MAP:
			m_isvalid = TRUE;
			fz_he_gets_map = 1;
			break;
		case REQUEST_TYPE_OFFER_GOLD:
		{
			
			
			double gold = double(m_gold);
			gold = ai->m_gold->GetCurrentSavings() / 10 ;
			if(gold < 100)
			{
				gold = 100;
			}
			gold = int(gold/100);

			if(regard >= 80) gold *= 500;
			if(80 > regard && regard >= 60) gold *=200;
			if(60 > regard && regard >= 40) gold *=100;
			if(40 > regard && regard >= 20) gold =100;
			if(20 > regard && regard > 10) gold *=400;
			if(regard <= 10) gold = 0;

			if(gold > 10000) gold = 10000;
			while (gold > ( ai->m_gold->GetCurrentSavings() / 2)) 
				gold = gold - 100;

			
			if(gold >= 25) {
				fz_he_gains_gold = gold;
				m_isvalid = TRUE;
			} else {
				gold = 0;
			}
			m_gold = sint32(gold);
			break;
		}
		case REQUEST_TYPE_OFFER_CEASE_FIRE:
			m_isvalid = TRUE;
			fz_gains_cease_fire = 1;
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE:
			m_isvalid = TRUE;
			fz_gains_alliance = 1;
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY:
			break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION:
			
			m_isvalid = TRUE;
			fz_end_pollution_pact = 1;
			break;

		
		case REQUEST_TYPE_EXCHANGE_ADVANCE:
		{
			if(ai->m_player->HasEmbassyWith(foreigner->GetPlayerIndex())) {
				sint32 numAdvances = ai->m_science->GetNumAdvances();
				uint8 *canoffer = new uint8[numAdvances];
				sint32 *advancesSkipped = new sint32[numAdvances];
				sint32 num = ai->m_science->GetCanOffer(foreigner->GetPlayerIndex(), canoffer, advancesSkipped);
				sint32 checked = 0;
				sint32 i;
				sint32 best = -1;
				sint32 bestskipped = 0;
				
				for(i = 0; i < numAdvances; i++) {
					if(!canoffer[i]) {
						continue;
					}
					if(advancesSkipped[i] > bestskipped) {
						best = i;
						bestskipped = advancesSkipped[i];
					}
					checked++;
					if(checked >= num)
						break;
				}
				if(best >= 0) {
					fz_he_gains_advance = 1;
					fz_advances_skipped = bestskipped; 
					m_advance = best;
					m_isvalid = TRUE;
				}
				delete [] canoffer;
                canoffer = NULL; 
				delete advancesSkipped;
                advancesSkipped = NULL; 

				if(!m_isvalid)
					break;
				m_isvalid = FALSE;
				
				numAdvances = ai->m_science->GetNumAdvances();
				uint8 *canaskfor = new uint8[numAdvances];
				num = ai->m_science->GetCanAskFor(foreigner->GetPlayerIndex(), canaskfor);
				checked = 0;
				i;
				best = -1;
				bestskipped = 0;
				sint32 skipped;
				
				for(i = 0; i < numAdvances; i++) {
					if(!canaskfor[i])
						continue;
					
					skipped = ai->m_science->GetMinPrerequisites(i);
					if(skipped > bestskipped) {
						best = i;
						bestskipped = skipped;
					}
					checked++;
					if(checked >= num)
						break;
				}
				
				if(best >= 0) {
					fz_advances_skipped = bestskipped;
					fz_advance_cost = ai->m_science->GetCost(best);
					fz_i_gain_advance = 1;
					m_reciprocalAdvance = best;
					m_isvalid = TRUE;
				}

				delete [] canaskfor;
                canaskfor = NULL; 
			}
			break;
		}
		case REQUEST_TYPE_EXCHANGE_CITY:
			break; 
		case REQUEST_TYPE_EXCHANGE_MAP:
			fz_map_area_gained = 1;
			fz_i_gain_map = 1;
			fz_he_gets_map = 1;
			fz_last_got_map = (sint32)ai->m_round_count->GetRound() - foreigner->GotMapFrom();
			m_isvalid = TRUE;
			break;
	}
}
