











#pragma once
#ifndef __CTP_AI_H__
#define __CTP_AI_H__

class Army;
class MapPoint;
class Unit;
class OrderRecord;

class CtpAi {
public:

	
	static PLAYER_INDEX s_maxPlayers;

	
	
	

	
	
	
	static void Cleanup();

	
	static void InitializeEvents();

	
	static void Initialize();

	
	static void Load(CivArchive & archive);

	
	static void Save(CivArchive & archive);

	
	static void RemovePlayer(const PLAYER_INDEX player);

	
	static void AddPlayer(const PLAYER_INDEX newPlayerId);

	
	
	

	
	static void BeginTurn(const PLAYER_INDEX player);

	
	static void CtpAi::MakeRoomForNewUnits(const PLAYER_INDEX playerId);

	
	static void FinishBeginTurn(const PLAYER_INDEX player);

	
	static void NetworkClientBeginTurn(const PLAYER_INDEX player);

	
	static void HandleMoveFailure(const Army & army, const MapPoint & pos); 

	
	static void AddForeignerGoalsForCity(const Unit & city, const PLAYER_INDEX ownerId);

	
	static void AddOwnerGoalsForCity(const Unit & city, const PLAYER_INDEX foreignerId);

	
	static void GroupWithEscort(const Army & army);

	
	static void AddGoalsForArmy(const Army & army);

	
	static bool GetNearestRefuel(const Army & army, const MapPoint & start_pos, MapPoint & refueling_pos, sint32 & refueling_distance);

	
	static void ExecuteOpportunityActions(const PLAYER_INDEX player);

	
	static const OrderRecord * GetUnloadOrder();
	static const OrderRecord * GetSpaceLaunchOrder();
	static const OrderRecord * GetDisbandArmyOrder();
	static const OrderRecord * GetMoveOrder();

	
	static const sint32 GetGoalDefendIndex();
	static const sint32 GetGoalSeigeIndex();
	static const sint32 GetEndgameWorldUnionIndex();

	
	static void SellRandomBuildings(const Unit & city, const double chance);

private:

	
	static void Resize();

	
	static void AddExploreTargets(const PLAYER_INDEX player);

	
	static void AddSettleTargets(const PLAYER_INDEX player);

	
	static void AddMiscMapTargets(const PLAYER_INDEX player);

	
	static void ComputeCityGarrisons(const PLAYER_INDEX player);

	
	static void BombardAdjacentEnemies(const Army & army);

	
	static bool GetNearestAircraftCarrier(const Army & army, MapPoint & carrier_pos, double & distance);

	
	static void RefuelAirplane(const Army & army);

	
	static void SetResearch(const PLAYER_INDEX player);

	
	static void SpendGoldToRushBuy(const PLAYER_INDEX player);

	
	static const OrderRecord * sm_unloadOrderRec;
	static const OrderRecord * sm_spaceLaunchOrderRec;
	static const OrderRecord * sm_disbandArmyOrderRec;
	static const OrderRecord * sm_moveOrderRec;
	static sint32 sm_goalDefendIndex;
	static sint32 sm_goalSeigeIndex;
	static sint32 sm_endgameWorldUnionIndex;
};

#endif __CTP_AI_H__
