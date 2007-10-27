//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map fog of war visibility handling
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
// BATTLE_FLAGS
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Finished tile improvements are now stored as well in the improvements
//   list to be able to draw or access them later. - Dec. 21st 2004 Martin Gühmann
// - Added new functions to calculate the food, shields and gold values produced 
//   at the storing time of this UnseenCell. - Dec. 22nd 2004 Martin Gühmann
// - Modified constructors and serialize method to support the new 
//   m_visibleCityOwner member. - Dec. 26th 2004 - Martin Gühmann 
// - When an UnseenCell object is created and the according cell contains a
//   city the owner is now taken from the CityData instead of the Unit 
//   itsself this allows to get the right owner info when a city changes 
//   hands. - Mar. 4th 2005 Martin Gühmann
// - Moved Peter's good's fix to the according Get*FromTerrain functions.
//   - April 13th 2005 Martin Gühmann
// - Fix retrieval of good boni. - May 18th 2005 Martin Gühmann
// - Added isCapitol
//
//----------------------------------------------------------------------------
//
/// \file   UnseenCell.cpp
/// \brief  Handling of tiles that are not currently visible (definitions)

#include "c3.h"
#include "UnseenCell.h"

#include "Cell.h"
#include "citydata.h"
#include "civarchive.h"
#include "director.h"           // g_director
#include "dynarr.h"
#include "installationtree.h"
#include "pointerlist.h"
#include "QuadTree.h"
#include "ResourceRecord.h"
#include "SelItem.h"            // g_selected_item
#include "SpriteState.h"
#include "TerrainImprovementRecord.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "TerrImprove.h"
#include "tiledmap.h"           // g_tiledMap
#include "TileInfo.h"
#include "UnitActor.h"
#include "UnitData.h"
#include "World.h"              // g_theWorld

namespace
{
    sint16 const    TERRAIN_UNKNOWN     = -1;
    sint16 const    MOVECOST_UNKNOWN    = 0x7fff;

    /// Release an actor
    /// \param      a_Actor  actor to release
    /// \remarks    The actor is reference counted
    /// \todo       Move to UnitActor
    void ReleaseActor(UnitActor * & a_Actor)
    {
        if (a_Actor)
        {
		    if (--a_Actor->m_refCount <= 0) 
            {
			    g_director->ActiveUnitRemove(a_Actor);
			    delete a_Actor;
		    }

            a_Actor = NULL;
	    }
    }
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::UnseenCell
//
// Description: Constructor
//
// Parameters : point			: The point (RC coordinate)
//
// Globals    : g_theWorld		: World information
//
// Returns    : -
//
// Remark(s)  : The constructor to use normally.
//
//----------------------------------------------------------------------------
UnseenCell::UnseenCell(const MapPoint & point)
:
	m_env                           (0),
	m_terrain_type                  (TERRAIN_UNKNOWN),
	m_move_cost                     (MOVECOST_UNKNOWN),
	m_flags                         (0x0000),
	m_bioInfectedOwner              (0x00), /// @todo Check PLAYER_UNASSIGNED?
	m_nanoInfectedOwner             (0x00),
	m_convertedOwner                (0x00),
	m_franchiseOwner                (0x00),
	m_injoinedOwner                 (0x00),
	m_happinessAttackOwner          (0x00),
	m_citySize                      (0),
	m_cityOwner                     (0),
	m_citySpriteIndex               (0),
	m_cell_owner                    (PLAYER_UNASSIGNED),
	m_slaveBits                     (0x0000),
#ifdef BATTLE_FLAGS
	m_battleFlags                   (0),
#endif
	m_tileInfo                      (NULL),
	m_point                         (point),
	m_installations                 (new PointerList<UnseenInstallationInfo>),
	m_improvements                  (new PointerList<UnseenImprovementInfo>),
	m_cityName                      (NULL),
	m_actor                         (NULL),
	m_poolIndex                     (-1),
	m_visibleCityOwner              (0)
{
	if (g_theWorld->GetTileInfo(point))
	{
		m_tileInfo = new TileInfo(g_theWorld->GetTileInfo(point));
	}

	Cell * cell = g_theWorld->GetCell(point);
	if (cell)
	{
		m_env = cell->GetEnv();
		m_move_cost = sint16(cell->GetMoveCost());
		m_terrain_type = (sint8)TERRAIN_TYPES(cell->GetTerrain());
#ifdef BATTLE_FLAGS
		m_battleFlags = cell->GetBattleFlags();
#endif

		sint32 i;

		// Same as well information about existing
		// tile improvments, except roadlike ones,
		// so that this information is available
		// later as well.
		// And in order not to break anythink use the existing
		// list for unfinished tile improvements.
		for(i = 0; i < cell->GetNumDBImprovements(); i++) {
			sint32 imp = cell->GetDBImprovement(i);
			m_improvements->AddTail(new UnseenImprovementInfo(imp, 100));
		}
		for(i = 0; i < cell->GetNumImprovements(); i++) {
			TerrainImprovement imp = cell->AccessImprovement(i);
			if (imp.IsValid()) 
			{
				m_improvements->AddTail(new UnseenImprovementInfo(imp.GetType(),
															      imp.PercentComplete()));
			}
		}

		DynamicArray<Installation> instArray;
		g_theInstallationTree->GetAt(point, instArray);
		for(i = 0; i < instArray.Num(); i++) {
			m_installations->AddTail(new UnseenInstallationInfo(instArray[i].GetType(),
														       instArray[i].GetVisibility()));
		}

		m_cell_owner = (sint8) cell->GetOwner();

		// Store the city that controlls this tile.
		m_visibleCityOwner = cell->GetCityOwner().m_id;

		Unit    city = cell->GetCity();

		if (city.IsValid()) 
		{
			m_citySize = (sint16)city.PopCount();
			m_citySpriteIndex = (sint16)city.CD()->GetDesiredSpriteIndex();
			const MBCHAR *name = city.GetName();
			m_cityName = new MBCHAR[strlen(name) + 1];
			strcpy(m_cityName, name);

			m_cityOwner = static_cast<sint16>(city.GetCityData()->GetOwner());

			CityData *cityData = city.GetData()->GetCityData();

			UnitActor *actor = city.GetActor();

			if (actor) {

				SpriteState *newSS = new SpriteState(city.GetSpriteState()->GetIndex());

				UnitActor *newActor = new UnitActor(newSS, 
												    city, 
												    city.GetType(), 
												    point, 
												    city.GetOwner(),
												    TRUE,
												    city.GetVisionRange(),
												    city.CD()->GetDesiredSpriteIndex());

				newActor->SetUnitVisibility((1 << g_selected_item->GetVisiblePlayer())
										    | actor->GetUnitVisibility());
				newActor->SetPos(point);

				newActor->SetIsFortified(actor->IsFortified());
				newActor->SetIsFortifying(actor->IsFortifying());
				newActor->SetHasCityWalls(actor->HasCityWalls());
				newActor->SetHasForceField(actor->HasForceField());

				newActor->SetSize(m_citySize);

				newActor->ChangeImage(newSS, city.GetType(), city);

				newActor->AddIdle();
				newActor->GetNextAction();

				m_actor = newActor;
			} // actor

			SetIsBioInfected(cityData->IsBioInfected());
			SetIsNanoInfected(cityData->IsNanoInfected());
			SetIsConverted(cityData->IsConverted());
			SetIsFranchised(cityData->IsFranchised());
			SetIsInjoined(cityData->IsInjoined());
			SetWasHappinessAttacked(cityData->WasHappinessAttacked());
			SetIsRioting(cityData->GetIsRioting());
			SetHasAirport(cityData->HasAirport());
			SetHasSleepingUnits(cityData->HasSleepingUnits());
			SetIsWatchful(cityData->IsWatchful());
			SetIsCapitol(cityData->IsCapitol()); //emod
			SetIsReligionIcon(cityData->HasReligionIcon()); //emod
			m_bioInfectedOwner = (sint8)cityData->GetOwner();
			m_nanoInfectedOwner = (sint8)cityData->GetOwner();
			m_convertedOwner = (sint8)cityData->IsConvertedTo();
			m_franchiseOwner = (sint8)cityData->GetFranchiseOwner();
			m_injoinedOwner = (sint8)cityData->GetOwner();
			m_happinessAttackOwner = (sint8)cityData->GetOwner();
			m_slaveBits = cityData->GetSlaveBits();
			SetIsSpecialIcon(cityData->HasSpecialIcon()); //emod

		} // city.IsValid
	} // cell
	
	SetHasHut(NULL != g_theWorld->GetGoodyHut(point));
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::UnseenCell
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The constructor to construct an empty object.
//
//----------------------------------------------------------------------------
UnseenCell::UnseenCell()
:
	m_env                           (0),
	m_terrain_type                  (TERRAIN_UNKNOWN),
	m_move_cost                     (MOVECOST_UNKNOWN),
	m_flags                         (0x0000),
	m_bioInfectedOwner              (0x00), /// @todo Check PLAYER_UNASSIGNED?
	m_nanoInfectedOwner             (0x00),
	m_convertedOwner                (0x00),
	m_franchiseOwner                (0x00),
	m_injoinedOwner                 (0x00),
	m_happinessAttackOwner          (0x00),
	m_citySize                      (0),
	m_cityOwner                     (0),
	m_citySpriteIndex               (0),
	m_cell_owner                    (PLAYER_UNASSIGNED),
	m_slaveBits                     (0x0000),
#ifdef BATTLE_FLAGS
	m_battleFlags                   (0),
#endif
	m_tileInfo                      (NULL),
	m_point                         (),
	m_installations                 (new PointerList<UnseenInstallationInfo>),
	m_improvements                  (new PointerList<UnseenImprovementInfo>),
	m_cityName                      (NULL),
	m_actor                         (NULL),
	m_poolIndex                     (-1),
	m_visibleCityOwner              (0)
{
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::UnseenCell
//
// Description: Copy-Constructor
//
// Parameters : old			: The UnseenCell object to copy.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The constructor for copying.
//
//----------------------------------------------------------------------------
UnseenCell::UnseenCell(UnseenCell *old)
{
	int realPoolIndex = m_poolIndex;
	*this = *old;
	m_poolIndex = realPoolIndex;

	if(m_actor) {
		m_actor->m_refCount++;
	}

	if(old->m_tileInfo) {
		m_tileInfo = new TileInfo(old->m_tileInfo);
	}

	m_installations = new PointerList<UnseenInstallationInfo>;
	m_improvements = new PointerList<UnseenImprovementInfo>;

	{
		PointerList<UnseenInstallationInfo>::Walker walk(old->m_installations);
		while(walk.IsValid()) {
			m_installations->AddTail(new UnseenInstallationInfo(walk.GetObj()));
			walk.Next();
		}
	}

	{
		PointerList<UnseenImprovementInfo>::Walker walk(old->m_improvements);
		while(walk.IsValid()) {
			m_improvements->AddTail(new UnseenImprovementInfo(walk.GetObj()));
			walk.Next();
		}
	}

	if(old->m_cityName) {
		m_cityName = new MBCHAR[strlen(old->m_cityName) + 1];
		strcpy(m_cityName, old->m_cityName);
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::UnseenCell
//
// Description: Constructor
//
// Parameters : archive			: The source archive
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The constructor for loading.
//
//----------------------------------------------------------------------------
UnseenCell::UnseenCell(CivArchive &archive)
:
	m_env                           (0),
	m_terrain_type                  (TERRAIN_UNKNOWN),
	m_move_cost                     (MOVECOST_UNKNOWN),
	m_flags                         (0x0000),
	m_bioInfectedOwner              (0x00), /// @todo Check PLAYER_UNASSIGNED?
	m_nanoInfectedOwner             (0x00),
	m_convertedOwner                (0x00),
	m_franchiseOwner                (0x00),
	m_injoinedOwner                 (0x00),
	m_happinessAttackOwner          (0x00),
	m_citySize                      (0),
	m_cityOwner                     (0),
	m_citySpriteIndex               (0),
	m_cell_owner                    (PLAYER_UNASSIGNED),
	m_slaveBits                     (0x0000),
#ifdef BATTLE_FLAGS
	m_battleFlags                   (0),
#endif
	m_tileInfo                      (NULL),
	m_point                         (),
	m_installations                 (NULL),
	m_improvements                  (NULL),
	m_cityName                      (NULL),
	m_actor                         (NULL),
	m_poolIndex                     (-1),
	m_visibleCityOwner              (0)
{
	Serialize(archive);
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::~UnseenCell
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
UnseenCell::~UnseenCell()
{
    ReleaseActor(m_actor);

    delete m_tileInfo;

	if (m_installations) 
    {
		m_installations->DeleteAll();
		delete m_installations;
	}

	if (m_improvements) 
    {
		m_improvements->DeleteAll();
		delete m_improvements;
	}

	delete [] m_cityName;
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::IsAirfield
//
// Description: Gets whether this location has an airfield post.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32: 1 if location has an airfield, 0 otherwise.
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be.
//
//----------------------------------------------------------------------------
bool UnseenCell::IsAirfield(void) const
{
	return terrainutil_HasAirfield(m_point);
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::IsListeningPost
//
// Description: Gets whether this location has a listening post.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool: location has a listening post
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be. 
//
//----------------------------------------------------------------------------
bool UnseenCell::IsListeningPost(void) const
{
	return terrainutil_HasListeningPost(m_point);
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::IsFort
//
// Description: Gets whether this location has a fort.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool: location has a fort
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be.
//
//----------------------------------------------------------------------------
bool UnseenCell::IsFort(void) const
{
	return terrainutil_HasFort(m_point);
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::IsRadar
//
// Description: Gets whether this location has a radar.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool: location has a radar
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be.
//
//----------------------------------------------------------------------------
bool UnseenCell::IsRadar(void) const
{
	return terrainutil_HasRadar(m_point);
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::IsHealUnits
//
// Description: Gets whether this location can heal units.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : bool: location can heal units
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be. 
//
//----------------------------------------------------------------------------
bool UnseenCell::IsHealUnits(void) const
{
	return terrainutil_HasFort(m_point);
}


//----------------------------------------------------------------------------
//
// Name       : UnseenCell::GetFoodFromTerrain
//
// Description: Gets the food from the given cell with terrain presumed the 
//              terrain type did not change since the last visit.
//
// Parameters : -
//
// Globals    : g_theTerrainDB: The terrain database
//
// Returns    : sint32 the ammount of food that can be produced in that cell
//              without tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnseenCell::GetFoodFromTerrain() const
{
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);

	sint32 food = rec->GetEnvBase()->GetFood();

	if(m_cityName != NULL && rec->HasEnvCity()) {
		food += rec->GetEnvCityPtr()->GetFood();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
		food += rec->GetEnvRiverPtr()->GetFood();
	}

	sint32 good;
	if(g_theWorld->GetGood(m_point, good)){
		food += g_theResourceDB->Get(good)->GetFood();
	}

	return food;
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::GetFoodProduced
//
// Description: Gets the food from the given cell with terrain presumed the 
//              terrain type did not change since the last visit including
//              tile improvements.
//
// Parameters : -
//
// Globals    : g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : sint32 the ammount of food that can be produced in that cells
//              including tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnseenCell::GetFoodProduced() const 
{
	sint32 food = GetFoodFromTerrain();

	PointerList<UnseenImprovementInfo>::Walker walker = 
			PointerList<UnseenImprovementInfo>::Walker(m_improvements);

	for ( ; walker.IsValid(); walker.Next())
	{
		sint32 percent	= walker.GetObj()->m_percentComplete;
		if(percent < 100){
			continue;
		}

		sint32 type		= walker.GetObj()->m_type;
		const TerrainImprovementRecord *impRec = 
		    g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect * effect = 
		    terrainutil_GetTerrainEffect(impRec, m_terrain_type);

		sint32 bonus;
		if (effect && effect->GetBonusFood(bonus)) 
		{
			food += bonus;
		}
	}

	return food; 
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::GetShieldsFromTerrain
//
// Description: Gets the shields from the given cell with terrain presumed the 
//              terrain type did not change since the last visit.
//
// Parameters : -
//
// Globals    : g_theTerrainDB: The terrain database
//
// Returns    : sint32 the ammount of shields that can be produced in that cells
//              without tile improvements and the given terrain.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnseenCell::GetShieldsFromTerrain() const
{
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);

	sint32 shield = rec->GetEnvBase()->GetShield();

	if(m_cityName != NULL && rec->HasEnvCity()) {
		shield += rec->GetEnvCityPtr()->GetShield();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
		shield += rec->GetEnvRiverPtr()->GetShield();
	}

	sint32 good;
	if(g_theWorld->GetGood(m_point, good)){
		shield += g_theResourceDB->Get(good)->GetProduction();
	}

	return shield;
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::GetShieldsProduced
//
// Description: Gets the shields from the given cell with terrain presumed the 
//              terrain type did not change since the last visit including
//              tile improvements.
//
// Parameters : -
//
// Globals    : g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : sint32 the ammount of shields that can be produced in that cells
//              including tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnseenCell::GetShieldsProduced() const 
{
	sint32 shield = GetShieldsFromTerrain();

	PointerList<UnseenImprovementInfo>::Walker walker = 
			PointerList<UnseenImprovementInfo>::Walker(m_improvements);

	for ( ; walker.IsValid(); walker.Next())
	{
		sint32 percent	= walker.GetObj()->m_percentComplete;
		if(percent < 100){
			continue;
		}

		sint32 type		= walker.GetObj()->m_type;
		const TerrainImprovementRecord *impRec = 
			g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect * effect = 
		    terrainutil_GetTerrainEffect(impRec, m_terrain_type);
		sint32 bonus;
		if(effect && effect->GetBonusProduction(bonus)) 
		{
			shield += bonus;
		}
	}

	return shield; 
}


//----------------------------------------------------------------------------
//
// Name       : UnseenCell::GetGoldFromTerrain
//
// Description: Gets the gold from the given cell with terrain presumed the 
//              terrain type did not change since the last visit.
//
// Parameters : -
//
// Globals    : g_theTerrainDB: The terrain database
//
// Returns    : sint32 the ammount of gold that can be produced in that cells
//              without tile improvements and the given terrain.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnseenCell::GetGoldFromTerrain() const
{
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);

	sint32 gold = rec->GetEnvBase()->GetGold();

	if(m_cityName != NULL && rec->HasEnvCity()) {
		gold += rec->GetEnvCityPtr()->GetGold();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
		gold += rec->GetEnvRiverPtr()->GetGold();
	}

	sint32 good;
	if(g_theWorld->GetGood(m_point, good)){
		gold += g_theResourceDB->Get(good)->GetGold();
	}

	return gold;
}

//----------------------------------------------------------------------------
//
// Name       : UnseenCell::GetGoldProduced
//
// Description: Gets the gold from the given cell with terrain presumed the 
//              terrain type did not change since the last visit including
//              tile improvements.
//
// Parameters : -
//
// Globals    : g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : sint32 the ammount of gold that can be produced in that cells
//              including tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 UnseenCell::GetGoldProduced() const 
{
	sint32 gold = GetGoldFromTerrain();

	PointerList<UnseenImprovementInfo>::Walker walker = 
			PointerList<UnseenImprovementInfo>::Walker(m_improvements);

	for ( ; walker.IsValid(); walker.Next())
	{
		sint32 percent	= walker.GetObj()->m_percentComplete;
		if (percent < 100){
			continue;
		}

		sint32 type		= walker.GetObj()->m_type;
		const TerrainImprovementRecord *impRec = 
		    g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect * effect = 
		    terrainutil_GetTerrainEffect(impRec, m_terrain_type);
		sint32 bonus;
		if(effect && effect->GetBonusGold(bonus)) {
			gold += bonus;
		}
	}

	return gold;
}


//----------------------------------------------------------------------------
//
// Name       : UnseenCell::Serialize
//
// Description: Serialization method
//
// Parameters : archive			: The source/target archive
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void UnseenCell::Serialize(CivArchive &archive)
{
	sint32 l;
	if(archive.IsStoring()) {
		m_point.Serialize(archive);
		archive.StoreChunk((uint8 *)&m_env, ((uint8 *)&m_slaveBits)+sizeof(m_slaveBits));

		{
			// A dirty workaround in order not to change the save game format.
			// UnseenInstallationInfo is now abused to store m_visibleCityOwner
			// as visibility member. As both are of type uint32 it is not a 
			// problem. Its type is marked as -1. At least this doesn't interfere
			// with anything else, as the installation data is just stored in
			// this class but it isn't aceessed. Maybe a mistake or a left
			// over from CTP1.
			m_installations->AddTail(new UnseenInstallationInfo(-1, m_visibleCityOwner));
			archive << m_installations->GetCount();
			PointerList<UnseenInstallationInfo>::Walker walk(m_installations);
			while(walk.IsValid()) {
				archive.Store((uint8*)walk.GetObj(), sizeof(UnseenInstallationInfo));
				walk.Next();
			}
		}

		{
			archive << m_improvements->GetCount();
			PointerList<UnseenImprovementInfo>::Walker walk(m_improvements);
			while(walk.IsValid()) {
				archive.Store((uint8*)walk.GetObj(), sizeof(UnseenImprovementInfo));
				walk.Next();
			}
		}



		if (m_cityName) 
		{
			l = strlen(m_cityName) + 1;
			archive << l;
			archive.Store((uint8*)m_cityName, (strlen(m_cityName) + 1) * sizeof(MBCHAR));
		}
		else
		{
			l = 0;
			archive << l;
		} 

		archive << (sint32)(m_actor != NULL);
		if (m_actor) {
			m_actor->Serialize(archive);
		}
		m_tileInfo->Serialize(archive);
	}
	else
	{
		m_point.Serialize(archive);
		archive.LoadChunk((uint8 *)&m_env, ((uint8 *)&m_slaveBits)+sizeof(m_slaveBits));

		if (m_installations) 
		{
			m_installations->DeleteAll();
		}
		else
		{
			m_installations = new PointerList<UnseenInstallationInfo>;
		}

		archive >> l;

		UnseenInstallationInfo* tmpUII;
		bool    vCityOwnerNotSet = true;
		sint32  i;
		for(i = 0; i < l; i++) {
			tmpUII = new UnseenInstallationInfo(archive);

			// Only store the additional UnseenInstallationInfo in the
			// save file but not in the UnseenCell object itsself.
			if(tmpUII->m_type >= 0){
				m_installations->AddTail(tmpUII);
			}
			else{
				m_visibleCityOwner = tmpUII->m_visibility;
				vCityOwnerNotSet = false;
				delete tmpUII;
			}
		}
		// Backwards compartibility: If this UnseenCell didn't have an m_visibleCityOwner
		if(vCityOwnerNotSet) m_visibleCityOwner = g_theWorld->GetCell(m_point)->GetCityOwner().m_id;

		if (m_improvements) 
		{
			m_improvements->DeleteAll();
		}
		else
		{
			m_improvements = new PointerList<UnseenImprovementInfo>;
		}

		archive >> l;
		for(i = 0; i < l; i++) {
			m_improvements->AddTail(new UnseenImprovementInfo(archive));
		}
		
		delete [] m_cityName;
		archive >> l;
		if (l > 0)
		{
			m_cityName = new MBCHAR[l];
			archive.Load((uint8*)m_cityName, l * sizeof(MBCHAR));
		}
		else
		{
			m_cityName = NULL;
		}

		sint32 hasActor;
		archive >> hasActor;

		ReleaseActor(m_actor);
		if (hasActor) 
		{
			m_actor = new UnitActor(archive);
		}

		delete m_tileInfo;
		m_tileInfo = new TileInfo(g_theWorld->GetTileInfo(m_point));
		Assert(m_tileInfo); 
		m_tileInfo->Serialize(archive);
	}
}

//----------------------------------------------------------------------------
//
// Name       : UnseenInstallationInfo::UnseenInstallationInfo
//
// Description: Constructor
//
// Parameters : archive         : The source archive
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The constructor for loading
//
//----------------------------------------------------------------------------
UnseenInstallationInfo::UnseenInstallationInfo(CivArchive &archive)
{
	archive.Load((uint8*)&m_type, sizeof(UnseenInstallationInfo));
}

//----------------------------------------------------------------------------
//
// Name       : UnseenInstallationInfo::UnseenInstallationInfo
//
// Description: Constructor
//
// Parameters : archive         : The source archive
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The constructor for loading
//
//----------------------------------------------------------------------------
UnseenImprovementInfo::UnseenImprovementInfo(CivArchive &archive)
{
	archive.Load((uint8*)&m_type, sizeof(UnseenImprovementInfo));
}
