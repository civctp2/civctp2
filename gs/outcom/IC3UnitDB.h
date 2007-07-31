
#pragma once

#ifndef __IC3UnitDB_H__
#define __IC3UnitDB_H__ 1


#include <OBJBASE.H>  


DEFINE_GUID(CLSID_IC3UnitDB, 
    0x72498240,
    0xd2c7,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);



#undef INTERFACE
#define INTERFACE IC3UnitDB

class MapPoint;
struct MapPointData; 
DECLARE_INTERFACE_(IC3UnitDB, IUnknown)
{
 	

    STDMETHOD_(sint32, GetNumUnitTypes) (THIS_ ) PURE;
	STDMETHOD_ (BOOL, IsUnitObsolete) (THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CityCanBuildUnit) (THIS_ sint32 type_unit) PURE;
    
    STDMETHOD_ (double, UnitTypeMovementPoints(THIS_ sint32 type_unit)) PURE;
    STDMETHOD_ (uint32, GetMovementType) (THIS_ sint32 type_unit) PURE; 
    STDMETHOD_ (BOOL,  CanSettleHere) (THIS_ sint32 type_unit, MapPointData *dest_pos) PURE;    
    STDMETHOD_ (sint32, GetProductionCost) (THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetAttack)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetDefense)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetFirepower)(THIS_ sint32 type_unit) PURE; 
    STDMETHOD_ (sint32, GetHP)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetHP_WithReadiness) (THIS_ sint32 unit_type)PURE;
    STDMETHOD_ (double, GetEntrenchmentBonus) (THIS_) PURE; 

    STDMETHOD_ (BOOL, IsSettler)(THIS_ sint32 type_unit) PURE; 
    STDMETHOD_ (BOOL, CanSettleSpace)(THIS_ sint32 type_unit) PURE ;
    STDMETHOD_ (BOOL, CanSettleWater)(THIS_ sint32 type_unit) PURE ;
    STDMETHOD_ (BOOL, CanMoveSpace)(THIS_ sint32 type_unit) PURE ;
    STDMETHOD_ (BOOL, CanMoveWater)(THIS_ sint32 type_unit) PURE ;
    STDMETHOD_ (BOOL, CanMoveShallowWater)(THIS_ sint32 type_unit) PURE ;
    STDMETHOD_ (BOOL, CanMoveAir)(THIS_ sint32 type_unit) PURE ;
    STDMETHOD_ (BOOL, CanMoveLand)(sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanMoveMountain)(sint32 type_unit) PURE;

    STDMETHOD_ (sint32, EnablingDiscovery)(THIS_ sint32 type_unit) PURE ;

	STDMETHOD_ (BOOL, CanPlantNuke)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanLaunchNuke)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanCreateParks)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanInvestigateCity)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanStealTechnology)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanInciteRevolution)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanAssasinateRuler)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanInvestigateReadiness)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanNullifyCityWalls)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanCreateFranchise)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanCauseUnhappiness)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanConductHits)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanSoothsay)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, CanAdvertise)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, CanConductBioTerror)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanConductNanoTerror)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanConductSlaveRaids)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanConductSettlerSlaveRaids)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanConductSlaveUprising)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanConductUndergroundRailway)(THIS_ sint32 type_unit) PURE ;
	STDMETHOD_ (BOOL, CanVictoryEnslave)(THIS_ sint32 type_unit) PURE ;

    STDMETHOD_ (sint32, GetZBRange)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsFlanker)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, GetIsTrader)(THIS_ sint32 type_unit) PURE; 
    STDMETHOD_ (BOOL, IsTransport)(THIS_ sint32 type_unit) PURE; 
    STDMETHOD_ (sint32, GetMaxCargo)(THIS_ sint32 type_unit) PURE; 

    STDMETHOD_ (BOOL, UsesFuel) (THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, GetMaxFuel) (THIS_ sint32 type_unit) PURE;

    STDMETHOD_ (BOOL, CanSpaceLaunch)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanSpaceLand)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanTransport) (THIS_ sint32 transport_type, sint32 cargo_type) PURE;
    STDMETHOD_ (sint32, Unittype_String_To_Int) (THIS_ char * unittype_string) PURE;
    STDMETHOD_ (char *, Unittype_Int_To_String) (THIS_ sint32 index) PURE;

	
	STDMETHOD_ (BOOL, CanConvertCity)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsNoZoc)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsIgnoresZoc)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsLandBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsAirBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsWaterBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsSpaceBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsMountainBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanCounterBombard)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsCantCaptureCity)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanEntrench)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanPatrol)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanExpelPop)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsDeathEffectsHappy)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsCanBeExpelled)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanSue)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsCanBeSued)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanTradeAttack)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsSingleUse)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsVisibleAttacking)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsNuclearAttack)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsStealthy)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanEstablishEmbassy)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanDefuseLandMines)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanDefuseSeaMines)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanDefuseXrayMines)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsNoSlaves)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanCreateRift)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsTelevangelist)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanBeRustled)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, CanInjoin)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (BOOL, IsSpecialForces)(THIS_ sint32 type_unit) PURE;

	STDMETHOD_ (BOOL, GetMovementTypeAir)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, GetMovementTypeLand)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, GetMovementTypeSea)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, GetMovementTypeSpace)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, GetMovementTypeMountain)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, GetMovementTypeShallowWater)(THIS_ sint32 type_unit) PURE;

	STDMETHOD_ (BOOL, IsAirDefense)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsLandDefense)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsWaterDefense)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsSpaceDefense)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsMountainDefense)(THIS_ sint32 type_unit) PURE;

	STDMETHOD_ (BOOL, IsAirAttack)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsLandAttack)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsWaterAttack)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsSpaceAttack)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsMountainAttack)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsShallowWaterAttack)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (BOOL, IsUnderwaterAttack)(THIS_ sint32 type_unit) PURE;

	
	STDMETHOD_ (double, GetConvertCityChance)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (double, GetConvertCityDeathChance)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetZBRangeAttack)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (double, GetActiveDefenseRange)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetProbOfBombHit)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetBombRounds)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (double, GetDefendAgainstSpies)(THIS_ sint32 type_unit) PURE;
    STDMETHOD_ (sint32, GetBombardRange)(THIS_ sint32 type_unit) PURE;
	STDMETHOD_ (sint32, GetBonusFood)(THIS_ sint32 type_unit) PURE;

	

	STDMETHOD_ (BOOL, CanBeachAssault)(THIS_ sint32 type_unit, MapPointData *point) PURE;
    STDMETHOD_ (BOOL, CanAttackFromSpaceShip)(THIS_ sint32 type_unit)PURE;
    STDMETHOD_ (BOOL, CanBeachAssaultAtAll)(THIS_ sint32 type_unit)PURE;


    STDMETHOD_ (BOOL, CanNanoattackCity) (THIS_ sint32 unit_type) PURE; 
    STDMETHOD_ (BOOL, CanBioterrorCity) (THIS_ sint32 unit_type) PURE; 
    STDMETHOD_ (BOOL, IsParatrooper) (THIS_ sint32 unit_type) PURE;
	STDMETHOD_ (BOOL, IsWormholeProbe) (THIS_ sint32 unit_type) PURE;
};

#endif __IC3UnitDB_H__
