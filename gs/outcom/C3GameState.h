










#pragma once
#ifndef __C3GAMESTATE_H__
#define __C3GAMESTATE_H__


#include "IC3GameState.h"

class RobotAstar;
class C3UnitDB; 
class C3BuildingDB; 
class C3InstDB; 
class CivArchive; 
class C3TerrainDB; 
class C3ErrorReport;
class C3String;
class C3Rand;
class CivArchive; 
class C3Player; 
class C3World; 
class C3Population; 
class C3Science; 
class C3Government; 
class C3Wonder;
class C3EndGameDB;

class C3GameState : public IC3GameState
{
	ULONG m_refCount;
	
	
     
    PLAYER_INDEX m_owner; 

    RobotAstar *m_astar; 
	C3Player *m_player;
	C3World *m_world;
    C3UnitDB *m_unitDB;
    C3BuildingDB *m_buildingDB; 
    C3InstDB *m_installationDB; 
	C3Population *m_population;
	C3Science *m_science;
	C3Government *m_government;
	C3Wonder *m_wonder ;
    C3TerrainDB *m_terrainDB; 
	C3ErrorReport *m_errorReport;
	C3String *m_stringDB;
	C3Rand *m_rand;
	C3EndGameDB *m_endGameDB ;

public:
	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    	
     C3GameState(sint32 idx); 
     C3GameState(CivArchive &archive); 
     void Serialize(CivArchive &archive); 

     ~C3GameState();
     STDMETHODIMP_ (PLAYER_INDEX) GetPlayerIndex()  { return m_owner; }  
    
     STDMETHODIMP_ (sint32) GetNumPlayersAtStart(); 
	 STDMETHODIMP_ (sint32) GetNumPlayers();
     STDMETHODIMP_ (sint32) GetMyPlayerId(); 
	
     STDMETHODIMP_ (IC3Player *) GetPlayer() { return (IC3Player*)(m_player); } 
     STDMETHODIMP_ (IC3World *) GetWorld() { return (IC3World*)(m_world); }
     STDMETHODIMP_ (IC3RobotAstar *) GetAstar() { return (IC3RobotAstar*)(m_astar); } 
     STDMETHODIMP_ (IC3UnitDB *) GetUnitDB() { return (IC3UnitDB*)(m_unitDB); } 
	 STDMETHODIMP_ (IC3Government *) GetGovernmentDB() { return (IC3Government*)(m_government) ; }
	 STDMETHODIMP_ (IC3Wonder *) GetWonderDB() { return (IC3Wonder *)(m_wonder) ; }

     STDMETHODIMP_ (IC3BuildingDB *)  GetBuildingDB() { return (IC3BuildingDB*)(m_buildingDB); }  
     STDMETHODIMP_ (IC3InstDB *) GetInstallationDB() { return (IC3InstDB*)(m_installationDB); } 

	 STDMETHODIMP_ (IC3Population *) GetPopulation() { return (IC3Population*)(m_population); }
	 STDMETHODIMP_ (IC3Science *) GetScience() { return (IC3Science*)(m_science); }

     STDMETHODIMP_ (IC3TerrainDB *) GetTerrainDB() { return (IC3TerrainDB*)(m_terrainDB); } 

	 STDMETHODIMP_ (IC3EndGameDB *) GetEndGameDB() { return (IC3EndGameDB *)(m_endGameDB) ; }

	 STDMETHODIMP_ (sint32) GetConstRevolutionLevel(void) ;
     STDMETHODIMP_ (sint32) GetConstRiotLevel(void) ;

	 STDMETHODIMP_ (const MBCHAR *) GetDataPath(void);
     STDMETHODIMP_ (IC3ErrorReport *)GetErrorReport() { return (IC3ErrorReport *)(m_errorReport); }
	 STDMETHODIMP_ (IC3String *)GetStringDB() { return (IC3String *)(m_stringDB); }
	 STDMETHODIMP_ (IC3Rand *)GetRand() { return (IC3Rand *)(m_rand); }
	 STDMETHODIMP_ (void)SendDiplomaticRequest(C3AIDiplomaticRequest *request);
	STDMETHODIMP_(sint32) GetTradeWith (PLAYER_INDEX second, PLAYER_INDEX third);

    STDMETHODIMP_(void) GetMinMaxStrength(double *min, double *max);
	STDMETHODIMP_(double) GetStrength(PLAYER_INDEX player);
	STDMETHODIMP_(sint32) GetGold (PLAYER_INDEX player);
	STDMETHODIMP_(double) GetPollutionLevel (PLAYER_INDEX player);
	STDMETHODIMP_(sint32) GetGlobalPollutionLevel();
	STDMETHODIMP_(sint32) GetRoundsToNextDisaster();
	STDMETHODIMP_(sint32) GetPollutionTriggerLevel();
	STDMETHODIMP_(sint32) GetHisTradeRoutes(PLAYER_INDEX player);

	STDMETHODIMP_(sint32) IsPlayerAlive(PLAYER_INDEX player);
	STDMETHODIMP_(sint32) GetGoodCount(PLAYER_INDEX player, uint32 city,
									   sint32 resource);
	STDMETHODIMP_(void) MakeTradeBid(PLAYER_INDEX foreigner,
									 uint32 foreignCity,
									 sint32 resource,
									 sint32 goldOffered,
									 uint32 myCity);


	STDMETHODIMP_(sint32) GetTotalTrade(PLAYER_INDEX foreigner);
	STDMETHODIMP_(sint32) GetGlobalTradeRoutes();
	STDMETHODIMP_(sint32) GetDifficulty();
	STDMETHODIMP_(sint32) GetOrderGoldCost(sint32 order_type);
	STDMETHODIMP_(sint32) GetOrderMaxGoldCost();

	STDMETHODIMP_(void) ProcessGraphicsCallback();

	STDMETHODIMP_(sint32) GetTurn();
};


#undef INTERFACE
#define INTERFACE C3GameState

#endif
