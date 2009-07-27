//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Main Ai File
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
// - force to move the transport units out of city (12 units isn't much, and their is problems when a group want
//   to enter in a transport that is in town(example : 5 - units group cannot enter transport
//   if it is in a city and
//   with 7 other garrison units(based on makeRoomForNewUnits code) - Calvitix
// - force garrison units to ungroup.
// - BombardNearbyEnemies before ending turn (during ExecuteOpportunityActions) PFT
// - Moved the startegic state calculation before everthing else, so that
//   each turn has the right startegy even after a reload. (13-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CTP_AI_H__
#define __CTP_AI_H__

class CtpAi;

#include "player.h" // PLAYER_INDEX
class Army;
class MapPoint;
class Unit;
class OrderRecord;

class CtpAi {
public:

	
	static PLAYER_INDEX s_maxPlayers; /// The maximum numbers of players in the game, including Barbarians

	
	static void Cleanup();
	static void CleanupEvents();

	
	static void InitializeEvents();

	
	static void Initialize();

	
	static void Load(CivArchive & archive);

	
	static void Save(CivArchive & archive);

	
	static void RemovePlayer(const PLAYER_INDEX player);

	
	static void AddPlayer(const PLAYER_INDEX newPlayerId);

	
	static void BeginTurn(const PLAYER_INDEX player);
	static void BeginDiplomacy(const PLAYER_INDEX player, sint32 round);

	static void MakeRoomForNewUnits(const PLAYER_INDEX playerId);

	static void MoveOutofCityTransportUnits(const PLAYER_INDEX playerId);

	static void UnGroupGarrisonUnits(const PLAYER_INDEX playerId);

	
	static void FinishBeginTurn(const PLAYER_INDEX player);

	
	static void NetworkClientBeginTurn(const PLAYER_INDEX player);


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

	static void SetResearch(const PLAYER_INDEX player);

	
	static void ComputeCityGarrisons(const PLAYER_INDEX player);

private:

	static void Resize();

	
	static void AddExploreTargets(const PLAYER_INDEX player);

	
	static void AddSettleTargets(const PLAYER_INDEX player);

	
	static void AddMiscMapTargets(const PLAYER_INDEX player);

#if 0
	static void BombardAdjacentEnemies(const Army & army);
#endif

	static void BombardNearbyEnemies(Army army, sint32 max_rge);

	static bool GetNearestAircraftCarrier(const Army & army, MapPoint & carrier_pos, double & distance);

	
	static void RefuelAirplane(const Army & army);


	
	static void SpendGoldToRushBuy(const PLAYER_INDEX player);

	
	static const OrderRecord * sm_unloadOrderRec;
	static const OrderRecord * sm_spaceLaunchOrderRec;
	static const OrderRecord * sm_disbandArmyOrderRec;
	static const OrderRecord * sm_moveOrderRec;
	static sint32 sm_goalDefendIndex;
	static sint32 sm_goalSeigeIndex;
	static sint32 sm_endgameWorldUnionIndex;
};

#endif // __CTP_AI_H__
