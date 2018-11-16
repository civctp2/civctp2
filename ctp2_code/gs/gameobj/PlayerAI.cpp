//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Player AI game object
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
// - Removed GetAllTileValue method
//
//----------------------------------------------------------------------------

#include "c3debug.h"

#include "c3.h"
#include "c3errors.h"

#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "civarchive.h"

#include "SelItem.h"
#include "aui.h"
#include "debugwindow.h"

#include "AdvanceRecord.h"
#include "DataCheck.h"
#include "pollution.h"

#include "CivPaths.h"

#include "TradePool.h"
#include "TradeOfferPool.h"

#include "UnitData.h"
#include "citydata.h"

#include "Vision.h"

#include "TerrImprovePool.h"
#include "TerrainRecord.h"

#include "Readiness.h"

#include "UnseenCell.h"
#include "MaterialPool.h"
#include "ConstDB.h"

#include "installationpool.h"
#include "WonderRecord.h"

#include "Gold.h"
#include "Sci.h"
#include "TaxRate.h"
#include "Diffcly.h"
#include "Advances.h"
#include "PlayHap.h"
#include "DiplomaticRequestData.h"
#include "DiplomaticRequest.h"
#include "DiplomaticRequestPool.h"
#include "AgreementData.h"
#include "AgreementPool.h"
#include "Agreement.h"
#include "UnitPool.h"
#include "BuildingRecord.h"
#include "director.h"
#include "RandGen.h"
#include "Regard.h"
#include "CivilisationPool.h"
#include "MessagePool.h"

#include "profileDB.h"
#include "AICause.h"
#include "CreateUnit.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "StrDB.h"
#include "TradeAstar.h"
#include "cellunitlist.h"
#include "ArmyData.h"
#include "ArmyPool.h"
#include "Order.h"
#include "UnitRecord.h"

#include "Globals.h"


extern TradeAstar g_theTradeAstar;





Army Player::GetArmyList(uint32 army_id, BOOL &is_unknown_id)
{
	Assert(FALSE);
	return Army(0);
}

void Player::GetArmyPos(const uint32 army_id, BOOL &is_unknown_id, MapPoint &p1)const
{
	is_unknown_id = TRUE;
}

void Player::GetArmyCurMinMovementPoints(const uint32 army_id, BOOL &is_unknown_id, double &cur)const
{
	is_unknown_id = TRUE;
}

void Player::GetArmyMinMovementPoints(const uint32 army_id, BOOL &is_unknown_id, double &min_move)const
{
	is_unknown_id = TRUE;
}

BOOL Player::ArmySettle(const uint32 army_id, BOOL &is_unknown_id)
{
	is_unknown_id = TRUE;
	return FALSE;
}

BOOL Player::ArmyCanEnter(const uint32 army_id, BOOL &is_unknown_id, MapPoint &pos,
   BOOL &move_to_many_units_dest, BOOL &move_violated_zoc, BOOL &move_violated_movetype, BOOL &move_out_of_fuel)
{
	is_unknown_id = TRUE;
	return FALSE;
}

BOOL Player::ArmyMoveTo(const uint32 army_id, BOOL &is_unknown_id, MapPoint &pos,
   BOOL &did_move, BOOL &i_died, BOOL &move_violated_zoc,
   BOOL &revealed_foreign_units, BOOL &revealed_unexplored,
   BOOL &is_transported, BOOL &out_of_fuel)
{
	return FALSE;
}

BOOL Player::ArmyCanSettle(uint32 army_id, BOOL &is_unknown_id, const MapPoint &ipos)
{
	 return FALSE;
}

BOOL Player::AiArmyGroup(uint32 add_me, uint32 target, BOOL *is_unknown_id)

{
	return FALSE;
}

BOOL Player::AiArmyUngroup(uint32 split_me, BOOL &is_unknown_id)

{
	return FALSE;
}

BOOL Player::GetCityPos(uint32 city_id, BOOL &is_unknown_id, MapPoint &ipos)
{
    return TRUE;
}

BOOL Player::CityEnqueueBuildItem (uint32 city_id, BOOL *is_unknown_id,
        sint32 category, sint32 unit_type)
{

    return FALSE;
}

sint32 Player::CityGetStoredProduction (uint32 city_id, BOOL *is_unknown_id)

{
	return 0;
}

sint32 Player::CityGetGrossProduction (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::CityGetNetProduction (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::CityGetStoredFood (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::CityGetGrossFood (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::CityGetNetFood (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::CityGetGrossGold (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::CityGetNetGold (uint32 city_id, BOOL *is_unknown_id)
{
	return 0;
}

sint32 Player::GetTotalBuildingUpkeep()
{
    return 0;
}

BOOL Player::CityChangeCurrentlyBuildingItem(uint32 city_id, BOOL *is_unknown_id,
   sint32 category, sint32 item_type)

{
	return FALSE;
}

uint32 Player::GetArmyId (const Unit &u)
{
    Assert(0);
    return 0;
}

uint32 Player::GetCityId (const Unit &u)
{
    Assert(0);
    return 0;
}

void Player::RegisterCreateBuilding(Unit &u, sint32 blg_type)
{
}

void Player::RegisterCreateEndgameObject(Unit &u, sint32 object_type)
{
}

void Player::RegisterLostBuilding(Unit &u, sint32 blg_type)
{
}

void Player::RegisterNewGovernment(Unit &u, sint32 government_type)
{
}

void Player::RegisterNewCapitolBuilding(Unit &u)
{
	g_theWorld->SetCapitolDistanceDirtyFlags(1<<m_owner);
}

void Player::RegisterCreateWonder(Unit &u, sint32 wonder_type)
{
}














BOOL Player::HasCityPopChanged(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}















BOOL Player::HasCityPopGrown(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}















BOOL Player::HasCityPopStarved(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}
















BOOL Player::WasCityImprovementBuilt(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}















BOOL Player::WasTerrainImprovementBuilt(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}















BOOL Player::WasCityHappinessAttacked(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}

BOOL Player::AiCreateTradeRoute(BOOL *is_unknown_id, uint32 u_src_city,
    sint32 type_route, sint32 scr_good,   uint32 u_dest_city)
{
	return FALSE;
}

sint32 Player::GetResourceCount(BOOL *is_unknown_id , uint32 u_city_id,
                                const sint32 type_good) const

{
	return 0;
}

double Player::GetRouteCost(BOOL *is_unknown_id , uint32 u_src_city, uint32 u_dest_city)
{
	return 0;
}


BOOL Player::CityBuyFront(BOOL *is_unknown_id , uint32 u_city_id)
{
	return FALSE;
}

sint32 Player::CityGetOvertimeCost(BOOL *is_unknown_id , uint32 u_city_id)
{
	return FALSE;
}













sint32 Player::GetCityPopCount(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}














sint32 Player::GetCitySlaveCount(uint32 city_id, BOOL &is_unknown_id)
{
	return FALSE;
}










void Player::GetCityProjectedHappinessAndCrime(uint32 city_id, double &happiness, double &crime, BOOL &is_unknown_id,
     sint32 *delta_martial_law)
{
}











void Player::GetCityProjectedFood(uint32 city_id, sint32 &food, BOOL &is_unknown_id)
{
}









void Player::GetCityRequiredFood(uint32 city_id, sint32 &food, BOOL &is_unknown_id)
{
}










void Player::GetCityProjectedProduction(uint32 city_id, sint32 &production, BOOL &is_unknown_id)
{
}










void Player::GetCityProjectedTrade(uint32 city_id, BOOL &is_unknown_id,
   sint32 &projected_gross_gold, sint32 &projected_net_gold)
{
}









sint32 Player::GetTileFood(uint32 city_id, MapPoint &pos, BOOL &is_unknown_id)
{
	return 0;
}










sint32 Player::GetTileProduction(uint32 city_id, MapPoint &pos, BOOL &is_unknown_id)
{
	return 0;
}










sint32 Player::GetTileResource(uint32 city_id, MapPoint &pos, BOOL &is_unknown_id)
{
	return 0;
}









void Player::GetProjectedTradeFromCell(uint32 city_id, MapPoint &pos, sint32 &trade, BOOL &is_unknown_id)
{
}









BOOL Player::IsPopAllowed(uint32 city_id, uint32 popType, BOOL &is_unknown_id)
{
	return TRUE;
}











void Player::GetCityScience(uint32 city_id, sint32 &science, BOOL &is_unknown_id)
{
}

BOOL Player::ArmyGetNumCargo (BOOL *is_unknown_id, uint32 u_id,
         sint32 *full_slots, sint32 *empty_slots)
{
	return FALSE;
}

sint32 Player::AiNumUnitsCanMoveIntoThisTranportEver(BOOL *is_unknown_id,  uint32 test_me,
   uint32 transport)
{
	return 0;
}

BOOL Player::AiCanMoveArmyIntoThisTranportEver(BOOL *is_unknown_id,  uint32 test_me,
   uint32 transport)
{

	return FALSE;
}

BOOL Player::AiCanMoveArmyIntoThisTranportRightNow(BOOL *is_unknown_id,
   uint32 move_me, uint32 transport, BOOL *enough_move, BOOL *adjacent,
   BOOL *ever)
{
	return FALSE;
}

BOOL Player::AiArmyMoveIntoTranport(BOOL *is_unknown_id,  uint32 move_me,
   uint32 transport, BOOL *is_transported)
{
	return FALSE;
}

BOOL Player::AiUnloadAllTransportsInArmy(BOOL *is_unknown_id,
        uint32 unload_me, const MapPoint &dest_pos,
        BOOL *did_move, BOOL *i_died,
		BOOL *revealed_foreign_units, BOOL *revealed_unexplored,
        BOOL *zocViolation, BOOL *is_transported)

{
	return FALSE;
}

BOOL Player::AiGetCargoMovementPoints (BOOL *is_unknown_id,
         uint32 unload_me,  double *min_move_points, BOOL *first_move)

{
	return FALSE;
}


BOOL Player::AiGetArmy(BOOL &is_unknown_id, uint32 u_army_id, Army &a)
{
    return TRUE;
}

BOOL Player::AiGetCity(BOOL &is_unknown_id, uint32 u_city_id, Unit &city)
{
    return TRUE;
}

uint32 Player::AiGetCityID(Unit &city)

{









    return 0;
}





void Player::AiRegisterAllObjects()
{
}

void Player::AiMakeTradeOffer(uint32 cityId, sint32 resource, sint32 gold)
{
}

void Player::SetGovernorPwReserve(const sint32 &reserve)
{
	m_governorPwReserve = reserve;
}

sint32 Player::GetGovernorPwReserve() const
{
	return m_governorPwReserve;
}
