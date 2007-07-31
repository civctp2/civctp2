













#pragma once
#ifndef __IC3GAMESTATE_H__
#define __IC3GAMESTATE_H__

struct IC3UnitDB; 
struct IC3BuildingDB;
struct C3AIDiplomaticRequest;

DEFINE_GUID(CLSID_IC3GameState, 
    0x92b7bc20,
    0xc94b,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);


#undef INTERFACE
#define INTERFACE IC3GameState

typedef sint32 PLAYER_INDEX; 


interface IC3Player;
interface IC3World;
interface IC3RobotAstar;
interface IC3Government;
interface IC3Wonder; 
interface IC3Population; 
interface IC3Science; 
interface IC3TerrainDB; 
interface IC3ErrorReport; 
interface IC3String; 
interface IC3Rand; 
interface IC3InstDB;
interface IC3EndGameDB;

DECLARE_INTERFACE_(IC3GameState, IUnknown)
{
 	
    STDMETHOD_ (PLAYER_INDEX, GetPlayerIndex) (THIS) PURE;
    STDMETHOD_ (sint32, GetNumPlayersAtStart) (THIS) PURE; 
	STDMETHOD_ (sint32, GetNumPlayers) (THIS) PURE;
    STDMETHOD_ (sint32, GetMyPlayerId) (THIS) PURE; 

 	

    STDMETHOD_ (IC3Player *, GetPlayer) (THIS) PURE; 
    STDMETHOD_ (IC3World *, GetWorld) (THIS) PURE; 
    STDMETHOD_ (IC3RobotAstar *, GetAstar) (THIS) PURE; 
    STDMETHOD_ (IC3UnitDB *, GetUnitDB) (THIS) PURE; 
    STDMETHOD_ (IC3Government *, GetGovernmentDB) (THIS) PURE; 
	STDMETHOD_ (IC3Wonder *, GetWonderDB) (THIS) PURE ;
    STDMETHOD_ (IC3BuildingDB *, GetBuildingDB) (THIS) PURE; 
    STDMETHOD_ (IC3InstDB *, GetInstallationDB) (THIS) PURE; 
	STDMETHOD_ (IC3EndGameDB *, GetEndGameDB) (THIS) PURE;

	STDMETHOD_ (sint32, GetConstRevolutionLevel) (THIS) PURE;
    STDMETHOD_ (sint32,  GetConstRiotLevel) (THIS) PURE ;

	STDMETHOD_ (IC3Population *, GetPopulation) (THIS) PURE;
	STDMETHOD_ (IC3Science *, GetScience) (THIS) PURE ;
    STDMETHOD_ (IC3TerrainDB *, GetTerrainDB) (THIS) PURE; 

	STDMETHOD_(const MBCHAR *, GetDataPath) (THIS) PURE;
	STDMETHOD_(IC3ErrorReport *, GetErrorReport) (THIS) PURE;
	STDMETHOD_(IC3String *, GetStringDB) (THIS) PURE;
	STDMETHOD_(IC3Rand *, GetRand) (THIS) PURE;

	STDMETHOD_(void, SendDiplomaticRequest) (THIS_ C3AIDiplomaticRequest *request) PURE;
	STDMETHOD_(sint32, GetTradeWith) (THIS_ PLAYER_INDEX second, PLAYER_INDEX third) PURE;

    STDMETHOD_(void, GetMinMaxStrength) (THIS_ double *min, double *max) PURE;
	STDMETHOD_(double, GetStrength) (THIS_ PLAYER_INDEX player) PURE;
	STDMETHOD_(sint32, GetGold) (THIS_ PLAYER_INDEX player) PURE;
	STDMETHOD_(double, GetPollutionLevel) (THIS_ PLAYER_INDEX player) PURE;
	STDMETHOD_(sint32, GetGlobalPollutionLevel) (THIS) PURE;
	STDMETHOD_(sint32, GetRoundsToNextDisaster) (THIS) PURE;
	STDMETHOD_(sint32, GetPollutionTriggerLevel) (THIS) PURE;
	STDMETHOD_(sint32, GetHisTradeRoutes) (THIS_ PLAYER_INDEX player) PURE;
	STDMETHOD_(sint32, IsPlayerAlive) (THIS_ PLAYER_INDEX player) PURE;
	STDMETHOD_(sint32, GetGoodCount) (THIS_ PLAYER_INDEX player, uint32 city, sint32 resource) PURE;
	STDMETHOD_(void, MakeTradeBid) (THIS_ PLAYER_INDEX foreigner,
									uint32 foreignCity,
									sint32 resource,
									sint32 goldOffered,
									uint32 myCity) PURE;

	STDMETHOD_(sint32, GetTotalTrade) (THIS_ PLAYER_INDEX foreigner) PURE;
	STDMETHOD_(sint32, GetGlobalTradeRoutes) (THIS_) PURE;
	STDMETHOD_(sint32, GetDifficulty) (THIS_)  PURE;
	STDMETHOD_(sint32, GetOrderGoldCost) (THIS_ sint32 order_type) PURE;
	STDMETHOD_(sint32, GetOrderMaxGoldCost) (THIS_) PURE;
	STDMETHOD_(void, ProcessGraphicsCallback) (THIS_) PURE;
	STDMETHOD_(sint32, GetTurn)(THIS_) PURE;
};


#endif
