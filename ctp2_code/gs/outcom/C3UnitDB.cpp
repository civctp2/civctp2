
#include "c3.h"



#include "globals.h"
#include "C3GameState.h"

#include "C3UnitDB.h"


#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "civarchive.h"
#include "MapPoint.h"
#include "UnitRecord.h"

#include "StrDB.h"

extern World *g_theWorld; 
extern StringDB *g_theStringDB;

extern BOOL UnitCanCarry(sint32 dest, sint32 src);

#include "ConstDB.h"
extern ConstDB *g_theConstDB; 

#include "Readiness.h"

STDMETHODIMP C3UnitDB::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3UnitDB)) {
		*obj = (IC3UnitDB*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3UnitDB::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3UnitDB::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}


C3UnitDB::C3UnitDB(Player *p)
{
   	m_refCount = 0;
    m_player = p;
} 

C3UnitDB::C3UnitDB(Player *p, CivArchive &archive)
{
    m_player = p;
    Serialize(archive); 
} 

void C3UnitDB::Serialize(CivArchive &archive) 
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}

sint32 C3UnitDB::GetNumUnitTypes ()
{
    
	return g_theUnitDB->NumRecords() - 1;
}

BOOL C3UnitDB::IsUnitObsolete(sint32 type_unit)
{
	if(type_unit < 0 || type_unit >= g_theUnitDB->NumRecords())
		return FALSE;

	const UnitRecord *rec = g_theUnitDB->Get(type_unit);

	BOOL is_obsolete = FALSE;
	for(sint32 o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
		if(m_player->HasAdvance(rec->GetObsoleteAdvanceIndex(o))) {
			is_obsolete = TRUE;
			break;
		}
	}
	return is_obsolete;
}

BOOL C3UnitDB::CityCanBuildUnit(sint32 type_unit)
{ 
	const UnitRecord *rec = g_theUnitDB->Get(type_unit);
	if(type_unit < 0 || type_unit >= g_theUnitDB->NumRecords())
		return FALSE;

    sint32 e = g_theUnitDB->Get(type_unit)->GetEnableAdvanceIndex(); 
	BOOL isObsolete = FALSE;
	for(sint32 o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
		if(m_player->HasAdvance(rec->GetObsoleteAdvanceIndex(o))) {
			isObsolete = TRUE;
			break;
		}
	}

    if (g_theUnitDB->Get(type_unit)->GetHasPopAndCanBuild()) { 
        return FALSE; 
    } 
   
    if (g_theUnitDB->Get(type_unit)->GetMaxMovePoints() < 0.0001) { 
        return FALSE; 
    } 

    if (o == -1) {    
        if (e == -1) { 
            return TRUE; 
        } else { 
         	return m_player->HasAdvance(e);
        }
    } else if (isObsolete) { 
        return FALSE; 
    } else { 
        if (e == -1) {  
            return TRUE; 
        } else { 
         	return m_player->HasAdvance(e);
        }
    }
}

double C3UnitDB::UnitTypeMovementPoints(sint32 type_unit)

{ 
    return g_theUnitDB->Get(type_unit)->GetMaxMovePoints();
}

uint32 C3UnitDB::GetMovementType(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetMovementType(); 
} 

sint32 C3UnitDB::GetProductionCost(sint32 type_unit) 
{ 
    return int(g_theUnitDB->Get(type_unit)->GetShieldCost());
}

sint32 C3UnitDB::GetAttack(sint32 type_unit)  
{ 
    return int(g_theUnitDB->Get(type_unit)->GetAttack());
}


sint32 C3UnitDB::GetDefense(sint32 type_unit)
{ 
    return int(g_theUnitDB->Get(type_unit)->GetDefense());
}

sint32 C3UnitDB::GetFirepower(sint32 type_unit) 
{ 
    return 1; 
                
                
    return g_theUnitDB->Get(type_unit)->GetFirepower();
}

sint32 C3UnitDB::GetHP(sint32 unit_type) 
{ 
    return g_theUnitDB->Get(unit_type)->GetMaxHP();
}

sint32 C3UnitDB::GetHP_WithReadiness(sint32 unit_type)
{
    return sint32(m_player->m_readiness->GetHPModifier() *
        g_theUnitDB->Get(unit_type)->GetMaxHP());
}


double C3UnitDB::GetEntrenchmentBonus()
{ 
    return g_theConstDB->GetEntrenchmentBonus();
} 

BOOL C3UnitDB::IsSettler(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetSettle();
}

extern BOOL UDUnitTypeCanSettle(sint32 unit_type, const MapPoint &pos);

BOOL C3UnitDB::CanSettleHere(sint32 type_unit, MapPointData *dest_pos)
{
    MapPoint ipos; 

    ipos.Norm2Iso(*dest_pos); 
    return UDUnitTypeCanSettle(type_unit, ipos); 
}











BOOL C3UnitDB::CanInvestigateCity(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetInvestigateCity()) ;
	}












BOOL C3UnitDB::CanStealTechnology(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetStealTechnology()) ;
	}













BOOL C3UnitDB::CanInciteRevolution(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetInciteRevolution()) ;
	}













BOOL C3UnitDB::CanAssasinateRuler(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetAssasinateRuler()) ;
	}















BOOL C3UnitDB::CanInvestigateReadiness(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetInvestigateReadiness()) ;
	}













BOOL C3UnitDB::CanNullifyCityWalls(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetNullifyCityWalls()) ;
	}













BOOL C3UnitDB::CanCreateFranchise(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetCreateFranchise()) ;
	}














BOOL C3UnitDB::CanCauseUnhappiness(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetCauseUnhappiness()) ;
	}













BOOL C3UnitDB::CanConductHits(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetConductHits()) ;
	}










BOOL C3UnitDB::CanSoothsay(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetCanSoothsay()) ;
	}











BOOL C3UnitDB::CanAdvertise(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetAdvertise()) ;
	}













BOOL C3UnitDB::CanConductBioTerror(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetBioTerror()) ;
	}













BOOL C3UnitDB::CanConductNanoTerror(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetNanoTerror()) ;
	}













BOOL C3UnitDB::CanConductSlaveRaids(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetSlaveRaids()) ;
	}













BOOL C3UnitDB::CanConductSettlerSlaveRaids(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetSettlerSlaveRaids()) ;
	}














BOOL C3UnitDB::CanConductSlaveUprising(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetSlaveUprising()) ;
	}















BOOL C3UnitDB::CanConductUndergroundRailway(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetUndergroundRailway()) ;
	}














BOOL C3UnitDB::CanVictoryEnslave(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetVictoryEnslavement()) ;
	}


	











BOOL C3UnitDB::CanSettleSpace(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetSettleSpace()) ;
	}













BOOL C3UnitDB::CanSettleWater(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetSettleWater()) ;
	}













BOOL C3UnitDB::CanMoveSpace(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetMovementTypeSpace()) ;
	}













BOOL C3UnitDB::CanMoveWater(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetMovementTypeSea()) ;
	}


BOOL C3UnitDB::CanMoveShallowWater(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetMovementTypeShallowWater()) ;
	}

BOOL C3UnitDB::CanMoveLand(sint32 type_unit) { 
	return (g_theUnitDB->Get(type_unit)->GetMovementTypeLand()) ;
}

BOOL C3UnitDB::CanMoveMountain(sint32 type_unit) 
{ 
	return (g_theUnitDB->Get(type_unit)->GetMovementTypeMountain()) ;
}












BOOL C3UnitDB::CanMoveAir(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetMovementTypeAir()) ;
	}













sint32 C3UnitDB::EnablingDiscovery(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetEnableAdvanceIndex()) ;
	}













BOOL C3UnitDB::CanPlantNuke(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetPlantNuke()) ;
	}













BOOL C3UnitDB::CanLaunchNuke(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetNuclearAttack()) ;
	}













BOOL C3UnitDB::CanCreateParks(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetCreateParks()) ;
	}













BOOL C3UnitDB::GetIsTrader(sint32 type_unit)
	{
	return (g_theUnitDB->Get(type_unit)->GetIsTrader()) ;
	}


sint32 C3UnitDB::GetZBRange(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetZBRangeAttack();
}

BOOL C3UnitDB::IsFlanker(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetIsFlanker(); 
}

BOOL C3UnitDB::IsTransport(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetCanCarry() != 0;
}

sint32 C3UnitDB::GetMaxCargo(sint32 type_unit)
{
	const UnitRecord::CargoData *data = g_theUnitDB->Get(type_unit)->GetCargoDataPtr();
	if(data) {
		return data->GetMaxCargo(); 
	} else {
		return 0;
	}
}

BOOL C3UnitDB::UsesFuel(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetNoFuelThenCrash(); 
}

BOOL C3UnitDB::GetMaxFuel(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetMaxFuel(); 
}

BOOL C3UnitDB::CanSpaceLaunch(sint32 type_unit)
{ 
    return g_theUnitDB->Get(type_unit)->GetSpaceLaunch(); 
} 

BOOL C3UnitDB::CanSpaceLand(sint32 type_unit)
{ 
    return g_theUnitDB->Get(type_unit)->GetSpaceLand(); 
} 


BOOL C3UnitDB::CanTransport (sint32 transport_type, sint32 cargo_type)
{
    return UnitCanCarry(transport_type, cargo_type);
}

sint32 C3UnitDB::Unittype_String_To_Int(char * unittype_string)
{
	StringId str_id;
	sint32 index;
	
	
	if (g_theStringDB->GetStringID(unittype_string, str_id))
		{
		
		if (g_theUnitDB->GetNamedItem(str_id, index))
			{
			
			return index;
			
		} 
		
	} 
	
	Assert(false);
	
	return -1;
}

char * C3UnitDB::Unittype_Int_To_String(sint32 index)
{
	StringId str_id;
	char * unittype_string;
	
	
	if (g_theUnitDB->GetNamedItemID(index, str_id))
		{
		
		unittype_string = (char *) g_theStringDB->GetNameStr(str_id);
		
	} 
	
	return unittype_string;
}


BOOL C3UnitDB::CanConvertCity( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetConvertCities(); 
}

BOOL C3UnitDB::IsNoZoc( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetNoZoc();
} 

BOOL C3UnitDB::IsIgnoresZoc( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetIgnoreZOC(); 
} 

BOOL C3UnitDB::IsBombard( sint32 type_unit )
{
	return g_theUnitDB->Get(type_unit)->GetCanBombard();
}

BOOL C3UnitDB::IsLandBombard( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBombardLand();
} 

BOOL C3UnitDB::IsAirBombard( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBombardAir();
} 

BOOL C3UnitDB::IsWaterBombard( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBombardWater();
} 

BOOL C3UnitDB::IsSpaceBombard( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBombardSpace();
} 

BOOL C3UnitDB::IsMountainBombard( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBombardMountain();
} 

BOOL C3UnitDB::CanCounterBombard( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanCounterBombard();
} 

BOOL C3UnitDB::IsCantCaptureCity( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCantCaptureCity();
} 

BOOL C3UnitDB::CanEntrench( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanEntrench();
} 

BOOL C3UnitDB::CanPatrol( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanPatrol();
} 

BOOL C3UnitDB::CanExpelPop( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanExpelPop();
} 

BOOL C3UnitDB::IsDeathEffectsHappy( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetDeathEffectsHappy();
} 

BOOL C3UnitDB::IsCanBeExpelled( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBeExpelled();
} 

BOOL C3UnitDB::CanSue( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanSue();
} 


BOOL C3UnitDB::IsCanBeSued( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBeSued();
} 

BOOL C3UnitDB::CanTradeAttack( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanAttackTrade();
} 

BOOL C3UnitDB::IsSingleUse( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetSingleUse();
} 

BOOL C3UnitDB::IsVisibleAttacking( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetVisibileAttacking();
} 

BOOL C3UnitDB::IsNuclearAttack( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetNuclearAttack();
} 

BOOL C3UnitDB::IsStealthy( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetIsStealthy();
} 

BOOL C3UnitDB::CanEstablishEmbassy( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetEstablishEmbassy();
} 

BOOL C3UnitDB::CanDefuseLandMines( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetDefuseLandMines();
} 

BOOL C3UnitDB::CanDefuseSeaMines( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetDefuseSeaMines();
} 

BOOL C3UnitDB::CanDefuseXrayMines( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetDefuseXrayMines();
} 

BOOL C3UnitDB::IsNoSlaves( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetNoSlaves();
} 

BOOL C3UnitDB::CanCreateRift( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCreateRift();
} 

BOOL C3UnitDB::IsTelevangelist( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetIsTelevangelist();
} 

BOOL C3UnitDB::CanBeRustled( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanBeRustled();
} 

BOOL C3UnitDB::CanInjoin( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetCanInjoin();
} 

BOOL C3UnitDB::GetMovementTypeAir( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetMovementTypeAir();
}

BOOL C3UnitDB::GetMovementTypeLand( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetMovementTypeLand();
}

BOOL C3UnitDB::GetMovementTypeSea( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetMovementTypeSea();
}

BOOL C3UnitDB::GetMovementTypeSpace( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetMovementTypeSpace();
}

BOOL C3UnitDB::GetMovementTypeMountain( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetMovementTypeMountain();
}

BOOL C3UnitDB::GetMovementTypeShallowWater( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetMovementTypeShallowWater();
}

BOOL C3UnitDB::IsAirDefense( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetDefendAir();
}

BOOL C3UnitDB::IsLandDefense( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetDefendLand();
}

BOOL C3UnitDB::IsWaterDefense( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetDefendWater();
}

BOOL C3UnitDB::IsSpaceDefense( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetDefendSpace();
}

BOOL C3UnitDB::IsMountainDefense( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetDefendMountain();
}

BOOL C3UnitDB::IsAirAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackAir();
}

BOOL C3UnitDB::IsLandAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackLand();
}

BOOL C3UnitDB::IsWaterAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackSea();
}

BOOL C3UnitDB::IsSpaceAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackSpace();
}

BOOL C3UnitDB::IsMountainAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackMountain();
}

BOOL C3UnitDB::IsShallowWaterAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackShallowWater();
}

BOOL C3UnitDB::IsUnderwaterAttack( sint32 type_unit)
{
	return g_theUnitDB->Get(type_unit)->GetCanAttackUnderwater();
}



double C3UnitDB::GetConvertCityChance(sint32 type_unit)
{
	UnitRecord::SuccessDeathEffect *data;
	if(g_theUnitDB->Get(type_unit)->GetConvertCities(data))
		return data->GetChance();
	else
		return 0;
}

double C3UnitDB::GetConvertCityDeathChance(sint32 type_unit)
{
	UnitRecord::SuccessDeathEffect *data;
	if(g_theUnitDB->Get(type_unit)->GetConvertCities(data))
		return data->GetDeathChance();
	else
		return 0;
}

sint32 C3UnitDB::GetZBRangeAttack( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetZBRangeAttack();
} 

double C3UnitDB::GetActiveDefenseRange( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetActiveDefenseRange();
} 

sint32 C3UnitDB::GetProbOfBombHit( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetProbOfBombHit();
} 

sint32 C3UnitDB::GetBombRounds( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetBombRounds();
} 

double C3UnitDB::GetDefendAgainstSpies( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetDefendAgainstSpies();
} 

sint32 C3UnitDB::GetBombardRange( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetBombardRange();
} 

BOOL C3UnitDB::IsSpecialForces( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetIsSpecialForces();
} 

sint32 C3UnitDB::GetBonusFood( sint32 type_unit)
{ 
	return g_theUnitDB->Get(type_unit)->GetBonusFood();
} 












BOOL C3UnitDB::CanBeachAssault( sint32 type_unit, MapPointData *point)
{ 
    MapPoint ipos; 

    ipos.Norm2Iso(*point);

	return g_theUnitDB->Get(type_unit)->GetCanBeachAssault();
} 

BOOL C3UnitDB::CanAttackFromSpaceShip(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetAttackFromSpaceship();
}

BOOL C3UnitDB::CanBeachAssaultAtAll(sint32 type_unit)
{
    return g_theUnitDB->Get(type_unit)->GetCanBeachAssault();
}

BOOL C3UnitDB::CanNanoattackCity (sint32 unit_type)
{ 
	return g_theUnitDB->Get(unit_type)->GetNanoTerror();
} 

BOOL C3UnitDB::CanBioterrorCity (sint32 unit_type)
{ 
	return g_theUnitDB->Get(unit_type)->GetBioTerror();
} 

BOOL C3UnitDB::IsParatrooper(sint32 unit_type)
{ 
    return g_theUnitDB->Get(unit_type)->GetParatrooper(); 
}

BOOL C3UnitDB::IsWormholeProbe (sint32 unit_type)
{ 
	return g_theUnitDB->Get(unit_type)->GetWormholeProbe();
} 

