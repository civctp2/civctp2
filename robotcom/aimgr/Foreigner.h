
#pragma once

#ifndef __FOREIGNER_H__
#define __FOREIGNER_H__ 1

class CivArchive; 
class AiMain; 
struct MapPointData; 

class ForeignAgent; 
class ForeignCity; 
template <class T> class BSet;
interface C3PlayerInfo; 
enum REQUEST_TYPE;
enum NEW_AGE;


class Foreigner { 

	PLAYER_INDEX m_player_index;

    MapPointData *m_center_of_mass;

    BSet<ForeignAgent> *m_visible_units; 
    BSet<ForeignCity> *m_known_cities; 

	double m_hotwar_attacked_me;
	double m_coldwar_attacked_me;
	sint32 m_attacked_by_me;
	sint32 m_last_attacked_me;
	sint32 m_estimated_gold;
	sint32 m_have_contact;
	sint32 m_military_incursions; 
	sint32 m_got_map_from;

	double m_rejected_requests;
	double m_accepted_requests;

	sint32 *m_sent_requests;
	sint32 *m_considered_sending_requests;

	double *m_my_responses;
	double *m_my_responses_decay;
	double *m_his_responses;
	double *m_his_responses_decay;

    
	double m_diplomatic_regard; 
    double m_unit_regard; 
 	double m_current_bonus_regard; 
    double m_static_bonus_regard; 
    double m_total_production; 
    double m_total_unit_cost;
    double m_his_raw_strength;
    double m_his_relative_strength; 
    double m_his_normalized_rank;   
    double m_he_beat_me; 
    double m_he_beat_me_this_turn; 

    double m_war_began; 
	double m_peace_began; 
	double m_closest_capitol; 

    double m_he_shares_continent; 
    double m_distance_to_his_capitol; 

    double m_num_he_is_at_war; 

    double m_he_nuked_me_count; 
    double m_his_total_nuke_count;
    double m_he_has_space;      
    double m_he_has_undersea; 

	double m_pollution_level;
	double m_trade_threat;
	double m_loyalty;

	double m_hate_you_insanely; 

    double m_pirated_this_turn; 
    double m_piracy_count; 
	double m_decayed_piracy;

    
    double m_he_built_underwater_units; 
    double m_he_built_space_units;
    double m_he_built_wormhole_units; 

    double m_his_coldwar_attacked_me_decay;
	double m_his_hotwar_attacked_me_decay;
	double m_accepted_requests_decay;
	double m_rejected_requests_decay;
	double m_piracy_decay;

	double m_violating_stop_trade;
	double m_violating_leave_our_lands;
	double m_violating_reduce_pollution;
	double m_violating_capture_city;
	double m_violating_end_pollution;
	double m_violating_attack_enemy;

	double m_trade_persistence;
	
	double m_attacked_my_enemies,
	       m_attacked_my_enemies_decay;
	double m_attacked_my_friends,
	       m_attacked_my_friends_decay;
	double m_broken_treaties,
		   m_broken_treaties_decay;

    double m_enemy_of_my_enemy;  
    double m_friend_of_my_enemy; 

    double m_has_pact_against_me; 
	BOOL m_is_robot; 
    BOOL m_leave_our_lands;

public:

	sint32 m_sent_greetings;
	sint32 m_he_accepted_greetings;


    Foreigner(PLAYER_INDEX index);
    Foreigner(CivArchive &archive, AiMain *ai); 
    ~Foreigner();
    void BadValue();

    void Serialize(CivArchive &archive); 
    void BeginTurn(AiMain *ai);
    void UpdateCenterOfMass();
	void UpdateMiscInfo(AiMain *ai);
    void GetCenterOfMass(MapPointData &pos); 
    MapPointData * GetCenterOfMassPtr() { return m_center_of_mass; } 
    void ResetCenterOfMass(MapPointData &pos); 

    void ClearVision(AiMain *ai); 
    void CanSeeArmy(AiMain *ai, MapPointData &pos, uint32 a_id, 
        const sint32 top_unit_type, const sint32 unit_num); 
    void CanSeeCity(AiMain *ai, MapPointData &pos, uint32 c_id); 

    void RemoveArmy(AiMain *ai, uint32 a_id);
	void RemoveCity(AiMain *ai, uint32 c_id);

    BSet<ForeignAgent> * GetVisibleUnits() const { return m_visible_units; }
    BSet<ForeignCity> * GetKnownCities() const { return m_known_cities; }

    
    void SetAffection(double val); 
    BOOL IsAtHotWar(); 
    BOOL IsAtColdWar();

	void ColdwarAttackedMe(AiMain *ai, const double delta);
	void HotwarAttackedMe(AiMain *ai, const double delta);
	void AttackedByMe();
	sint32 GetLastAttackedMe() const { return m_last_attacked_me; }

	double GetHotwarAttackedMe() const { return m_hotwar_attacked_me; }
	double GetColdwarAttackedMe() const { return m_coldwar_attacked_me; }

	sint32 GetAttackedByMe() { return m_attacked_by_me; }
	void SetColdwarAttackedMe(const double a) { m_coldwar_attacked_me = a; }
	void SetHotwarAttackedMe(const double a) { m_hotwar_attacked_me = a; }

	void SetLengthOfPeace( const double a ) { m_peace_began = a; }
	void SetLengthOfWar( const double a ) { m_war_began = a; }


	sint32 GetEstimatedGold() const { return m_estimated_gold; }
	void SetEstimatedGold(const sint32 g) { m_estimated_gold = g; }

	BOOL HaveContact() const { return m_have_contact; }
	void ContactMade() { m_have_contact = TRUE; }

	void RegisterPlayerInfo(C3PlayerInfo *pi);

	PLAYER_INDEX GetPlayerIndex() const { return m_player_index; }

	PLAYER_INDEX GetPlayerType(AiMain *ai, Foreigner *foreigner);

	double GetLengthOfWarWithHim(AiMain *ai);
	double GetLengthOfPeaceWithHim(AiMain *ai);

	void SetGotMapFrom(sint32 when) { m_got_map_from = when; }
	sint32 GotMapFrom() { return m_got_map_from; }

	sint32 LastSentRequest(REQUEST_TYPE r);
	void RegisterSentRequestTo(AiMain *ai, REQUEST_TYPE r);
	sint32 LastConsideredSendingRequest(REQUEST_TYPE r);
	void RegisterConsideredSendingRequest(AiMain *ai, REQUEST_TYPE r);

	
	void Foreigner::SetMyResponses(REQUEST_TYPE r, int answer);
	double Foreigner::GetMyResponses(REQUEST_TYPE r);
	void Foreigner::UpdateMyResponses();
	void Foreigner::DecayMyResponses(REQUEST_TYPE r);
	void Foreigner::SetMyResponsesDecay(REQUEST_TYPE r,double decay);

	void Foreigner::SetHisResponses(REQUEST_TYPE r, int answer);
	double Foreigner::GetHisResponses(REQUEST_TYPE r);
	void Foreigner::UpdateHisResponses();
	void Foreigner::DecayHisResponses(REQUEST_TYPE r);
	void Foreigner::SetHisResponsesDecay(REQUEST_TYPE r,double decay);


	double GetDiplomaticRegard() const { return m_diplomatic_regard; }
	void SetDiplomaticRegard(AiMain *ai, double reg);

    double GetBonusRegard() const { return m_current_bonus_regard; } 

    double GetRawStrength() const { return m_his_raw_strength; } 
    double GetNormalizedRank() const { return m_his_normalized_rank; } 
    void SetHisRank(const double his_rank) { m_his_normalized_rank = his_rank; }  
	
	
	double GetUnitRegard() const { return m_unit_regard; }
	void SetUnitRegard(double relations) { m_unit_regard = relations; }
	
	
	double GetNumPlayersHeIsFighting() const {return m_num_he_is_at_war;}
	double GetHeSharesContinent() const { return m_he_shares_continent;}
	
	void SetClosestCapitol(double test) { m_closest_capitol = test;}
	double GetClosestCapitol() const { return m_closest_capitol;}

	void AcceptedRequest() { m_accepted_requests += 1; }
	void RejectedRequest() { m_rejected_requests += 1; }
	double GetAcceptedRequests() const { return m_accepted_requests; }
	double GetRejectedRequests() const { return m_rejected_requests; }
	void SetAcceptedRequests(double r) { m_accepted_requests = r; }
	void SetRejectedRequests(double r) { m_rejected_requests = r; }

	double GetInsaneHatred() const { return m_hate_you_insanely; }
	void SetInsaneHatred(BOOL r) { m_hate_you_insanely = r; }

	sint32 GetTradeWith(AiMain *ai, PLAYER_INDEX third_party);

	double GetPollutionLevel() const { return m_pollution_level; }


	double GetTradeThreat() const { return m_trade_threat; }

	double GetMilitaryThreat() const;
	double GetDefense() const;

	double HeDefeatedMeRecently() const;
	double IDefeatedHimRecently() const;

	double GetLoyalty() const;
	void SetLoyalty(double loyalty) { m_loyalty = loyalty; }

	void HeBrokeCeaseFire();
	void HeBrokeAlliance();
	
	
	double UpdateCeaseFire(AiMain *ai, sint32 player_idx);
   	double UpdateAlliance(AiMain *ai, sint32 player_idx);
	double UpdateWar(AiMain *ai, sint32 player_idx);
	double UpdateNeutrality(AiMain *ai, sint32 player_idx);
    
	void UpdateDiplomaticState(AiMain *ai);

    double GetDistanceToCapitol() const { return m_distance_to_his_capitol; } 
    void RegisterHeNuked(const PLAYER_INDEX my_player_id, const PLAYER_INDEX his_target); 
    void RegisterHeBuiltNewAgeUnit(NEW_AGE unit_type);
    void GetUnitAge(double &underwater, double &space, 
                           double &wormhole);

    void RegisterPiracy(); 
	double GetDecayedPiracy() const { return m_decayed_piracy; }
	void SetDecayedPiracy(double p)         { m_decayed_piracy = p; }

    void ComputeLandContinentShared(AiMain *ai, double &continents_shared);

    void SetHisColdwarAttackedMeDecay(const double d)    { m_his_coldwar_attacked_me_decay = d; }
	void SetHisHotwarAttackedMeDecay(const double d)    { m_his_hotwar_attacked_me_decay = d; }
	void SetAcceptedRequestsDecay(const double d) { m_accepted_requests_decay = d; }
	void SetRejectedRequestsDecay(const double d) { m_rejected_requests_decay = d; }
	void SetPiracyDecay(const double d)           { m_piracy_decay = d; }
    void RegisterFightWinner(sint32 lost_units, sint32 winner);
    double GetHeBeatMe() const { return m_he_beat_me; }
    double GetHeBeatMeThisTurn() const { return m_he_beat_me_this_turn; }
    void BeginTurnHeBeatMe(const double m_he_beat_me_decay);
	void EndTurnClear_HeBeatMeThisTurn();


	double GetHisColdwarAttackedMeDecay() const    { return m_his_coldwar_attacked_me_decay; }
	double GetHisHotwarAttackedMeDecay() const    { return m_his_hotwar_attacked_me_decay; }
	double GetAcceptedRequestsDecay() const { return m_accepted_requests_decay; }
	double GetRejectedRequestsDecay() const { return m_rejected_requests_decay; }
	double GetPiracyDecay() const { return m_piracy_decay; }

	void RegisterRegardIncrease(sint32 amount); 
    void RegisterLeaveOurLands () { m_leave_our_lands = TRUE; }  
    void RegisterLeaveOurLandsExpired () { m_leave_our_lands = FALSE; } 
    BOOL  IsLeaveOurLands() const { return m_leave_our_lands; }  

	double GetViolatingStopTrade() { return m_violating_stop_trade; }
	double GetViolatingLeaveOurLands() { return m_violating_leave_our_lands; }
	double GetViolatingReducePollution() { return m_violating_reduce_pollution; }
	double GetViolatingEndPollution() { return m_violating_end_pollution; }
	double GetViolatingCaptureCity() { return m_violating_capture_city; }
	double GetViolatingAttackEnemy() { return m_violating_attack_enemy; }

	void SetViolatingStopTrade(double v) { m_violating_stop_trade = v; }
	void SetViolatingLeaveOurLands(double v) { m_violating_leave_our_lands = v; }
	void SetViolatingReducePollution(double v) { m_violating_reduce_pollution = v; }
	void SetViolatingEndPollution(double v) { m_violating_end_pollution = v; }
	void SetViolatingCaptureCity(double v) { m_violating_capture_city = v; }
	void SetViolatingAttackEnemy(double v) { m_violating_attack_enemy = v; }

	void SetTradePersistence(double p) { m_trade_persistence = p; }
	double GetTradePersistence() const { return m_trade_persistence; }

	BOOL HeAgreed(AiMain *ai, AGREEMENT_TYPE agreement);
	
	void RegisterAttackedMyEnemy() { m_attacked_my_enemies += 1; }
	void SetAttackedMyEnemies(double val) { m_attacked_my_enemies = val; }
	double GetAttackedMyEnemies() { return m_attacked_my_enemies; }
	void SetAttackedMyEnemiesDecay(double val) { m_attacked_my_enemies_decay = val; }
	double GetAttackedMyEnemiesDecay() { return m_attacked_my_enemies_decay; }

	void RegisterAttackedMyFriend() { m_attacked_my_friends += 1; }
	void SetAttackedMyFriends(double val) { m_attacked_my_friends = val; }
	double GetAttackedMyFriends() { return m_attacked_my_friends; }
	void SetAttackedMyFriendsDecay(double val) { m_attacked_my_friends_decay = val; }
	double GetAttackedMyFriendsDecay() { return m_attacked_my_friends_decay; }

	void RegisterHeBrokeATreaty(PLAYER_INDEX with) { m_broken_treaties += 1; }
	void SetBrokenTreaties(double val) { m_broken_treaties = val; }
	double GetBrokenTreaties() const { return m_broken_treaties; }
	void SetBrokenTreatiesDecay(double val) { m_broken_treaties_decay = val; }
	double GetBrokenTreatiesDecay() { return m_broken_treaties_decay; }

    double IsEnemyOfMyEnemy () const { return m_enemy_of_my_enemy;  } 
    void SetEnemyOfMyEnemy(double f) { m_enemy_of_my_enemy = f; }

    double IsFriendOfMyEnemy() const { return m_friend_of_my_enemy; } 
    void SetFriendOfMyEnemy(double f) { m_friend_of_my_enemy = f; }

    double HasPactAgainstMe() const { return m_has_pact_against_me; }  
    void SetHasPactAgainstMe(const double p) { m_has_pact_against_me = p; }
    void RegisterAgreementAgainstMe(double str);

    void RegisterHisArmyTeleported (AiMain *ai, uint32 i_moved, 
        MapPointData *new_pos);

    BOOL CanUseMassDestructionOnHim() const;

    void GetFZRegard(sint32 &diplomatic, sint32 &unit, sint32 &bonus);

    void SetFZRegard (sint32 diplomatic, sint32 unit, sint32 bonus);

	BOOL IsRobot() { return m_is_robot; };
}; 

#endif __FOREIGNER_H__

