#pragma once

#ifndef __C3UnitDB_H__
#define __C3UnitDB_H__ 1

#include "ic3UnitDB.h"

class Player;
class CivArchive;

class C3UnitDB : public IC3UnitDB
{
	ULONG m_refCount;

    Player *m_player;

public:

	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3UnitDB(Player *p);
    C3UnitDB(Player *p, CivArchive &archive);
    void Serialize(CivArchive &archive);

    STDMETHODIMP_(sint32) GetNumUnitTypes ();
	STDMETHODIMP_ (BOOL) IsUnitObsolete(sint32 type_unit);
    STDMETHODIMP_ (BOOL) CityCanBuildUnit(sint32 type_unit);

    STDMETHODIMP_ (double) UnitTypeMovementPoints(sint32 type_unit);
    STDMETHODIMP_ (uint32) GetMovementType(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetProductionCost(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetAttack(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetDefense(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetFirepower(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetHP(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetHP_WithReadiness(sint32 unit_type);

    STDMETHODIMP_ (double) GetEntrenchmentBonus();

    STDMETHODIMP_ (BOOL) IsSettler(sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanSettleHere(sint32 type_unit, MapPointData *dest_pos);

	STDMETHODIMP_ (BOOL) CanSettleSpace(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanSettleWater(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanMoveSpace(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanMoveWater(sint32 type_unit) ;
  	STDMETHODIMP_ (BOOL) CanMoveShallowWater(sint32 type_unit) ;
    STDMETHODIMP_ (BOOL) CanMoveLand(sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanMoveMountain(sint32 type_unit);
	STDMETHODIMP_ (BOOL) CanMoveAir(sint32 type_unit) ;

   	STDMETHODIMP_ (sint32) EnablingDiscovery(sint32 type_unit) ;

	STDMETHODIMP_ (BOOL) CanPlantNuke(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanLaunchNuke(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanCreateParks(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanInvestigateCity(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanStealTechnology(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanInciteRevolution(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanAssasinateRuler(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanInvestigateReadiness(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanNullifyCityWalls(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanCreateFranchise(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanCauseUnhappiness(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanConductHits(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanSoothsay(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanAdvertise(sint32 type_unit) ;

	STDMETHODIMP_ (BOOL) CanConductBioTerror(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanConductNanoTerror(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanConductSlaveRaids(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanConductSettlerSlaveRaids(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanConductSlaveUprising(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanConductUndergroundRailway(sint32 type_unit) ;
	STDMETHODIMP_ (BOOL) CanVictoryEnslave(sint32 type_unit) ;

    STDMETHODIMP_ (sint32) GetZBRange(sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsFlanker(sint32 type_unit);
    STDMETHODIMP_ (BOOL) GetIsTrader(sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsTransport(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetMaxCargo(sint32 type_unit);

    STDMETHODIMP_ (BOOL) UsesFuel(sint32 type_unit);
    STDMETHODIMP_ (BOOL) GetMaxFuel(sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanSpaceLaunch(sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanSpaceLand(sint32 type_unit);

    STDMETHODIMP_ (BOOL) CanTransport (sint32 transport_type, sint32 cargo_type);

    STDMETHODIMP_ (sint32) Unittype_String_To_Int(char * unittype_string);
    STDMETHODIMP_ (char *) Unittype_Int_To_String(sint32 index);

	STDMETHODIMP_ (BOOL) CanConvertCity( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsNoZoc( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsIgnoresZoc( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsBombard( sint32 type_unit );
    STDMETHODIMP_ (BOOL) IsLandBombard( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsAirBombard( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsWaterBombard( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsSpaceBombard( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsMountainBombard( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanCounterBombard( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsCantCaptureCity( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanEntrench( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanPatrol( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanExpelPop( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsDeathEffectsHappy( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsCanBeExpelled( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanSue( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsCanBeSued( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanTradeAttack( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsSingleUse( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsVisibleAttacking( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsNuclearAttack( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsStealthy( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanEstablishEmbassy( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanDefuseLandMines( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanDefuseSeaMines( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanDefuseXrayMines( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsNoSlaves( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanCreateRift( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsTelevangelist( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanBeRustled( sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanInjoin( sint32 type_unit);
    STDMETHODIMP_ (BOOL) IsSpecialForces( sint32 type_unit);

	STDMETHODIMP_ (BOOL) GetMovementTypeAir( sint32 type_unit);
	STDMETHODIMP_ (BOOL) GetMovementTypeLand( sint32 type_unit);
	STDMETHODIMP_ (BOOL) GetMovementTypeSea( sint32 type_unit);
	STDMETHODIMP_ (BOOL) GetMovementTypeSpace( sint32 type_unit);
	STDMETHODIMP_ (BOOL) GetMovementTypeMountain( sint32 type_unit);
	STDMETHODIMP_ (BOOL) GetMovementTypeShallowWater( sint32 type_unit);

	STDMETHODIMP_ (BOOL) IsAirDefense( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsLandDefense( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsWaterDefense( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsSpaceDefense( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsMountainDefense( sint32 type_unit);

	STDMETHODIMP_ (BOOL) IsAirAttack( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsLandAttack( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsWaterAttack( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsSpaceAttack( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsMountainAttack( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsShallowWaterAttack( sint32 type_unit);
	STDMETHODIMP_ (BOOL) IsUnderwaterAttack( sint32 type_unit);

	STDMETHODIMP_ (double) GetConvertCityChance(sint32 type_unit);
	STDMETHODIMP_ (double) GetConvertCityDeathChance(sint32 type_unit);
    STDMETHODIMP_ (sint32) GetZBRangeAttack( sint32 type_unit);
    STDMETHODIMP_ (double) GetActiveDefenseRange( sint32 type_unit);
    STDMETHODIMP_ (sint32) GetProbOfBombHit( sint32 type_unit);
    STDMETHODIMP_ (sint32) GetBombRounds( sint32 type_unit);
    STDMETHODIMP_ (double) GetDefendAgainstSpies( sint32 type_unit);
    STDMETHODIMP_ (sint32) GetBombardRange( sint32 type_unit);
	STDMETHODIMP_ (sint32) GetBonusFood( sint32 type_unit);


	STDMETHODIMP_ (BOOL) CanBeachAssault( sint32 type_unit, MapPointData *point);

    STDMETHODIMP_ (BOOL) CanAttackFromSpaceShip(sint32 type_unit);
    STDMETHODIMP_ (BOOL) CanBeachAssaultAtAll(sint32 type_unit);

    STDMETHODIMP_ (BOOL) CanNanoattackCity (sint32 unit_type);
    STDMETHODIMP_ (BOOL) CanBioterrorCity (sint32 unit_type);
    STDMETHODIMP_ (BOOL) IsParatrooper(sint32 unit_type);
	STDMETHODIMP_ (BOOL) IsWormholeProbe (sint32 unit_type);
};

#endif __C3UnitDB_H__
