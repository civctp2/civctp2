//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map fog of war visibility handling
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
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "UnseenCell.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TileInfo.h"
#include "Cell.h"
#include "CivArchive.h"
#include "TiledMap.h"
#include "PointerList.h"
#include "TerrImprove.h"
#include "DynArr.h"
#include "QuadTree.h"
#include "installationtree.h"
#include "UnitActor.h"
#include "SelItem.h"
#include "Pool.h"
#include "Director.h"
#include "SpriteState.h"
#include "UnitData.h"
#include "CityData.h"
#include "terrainutil.h"
#include "TerrImprovePool.h"

// Added by Martin Gühmann to support unseen terrain stat calculation
#include "TerrainRecord.h"
#include "TerrainImprovementRecord.h"

extern Director				*g_director;
extern World				*g_theWorld;
extern	TiledMap			*g_tiledMap ;
extern SelectedItem			*g_selected_item;
extern Pool<UnseenCell>     *g_theUnseenPond;

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
UnseenCell::UnseenCell(const MapPoint &point)
{
	Cell* cell = g_theWorld->GetCell(point);

	m_point = point;
	m_env = cell->GetEnv();
	m_move_cost = sint16(cell->GetMoveCost());
	m_terrain_type = (sint8)TERRAIN_TYPES(cell->GetTerrain());
#ifdef BATTLE_FLAGS
	m_battleFlags = cell->GetBattleFlags();
#endif

	if(g_theWorld->GetTileInfo(point))
		m_tileInfo = new TileInfo(g_theWorld->GetTileInfo(point));
	else
		m_tileInfo = NULL;

	m_installations = new PointerList<UnseenInstallationInfo>;
	m_improvements = new PointerList<UnseenImprovementInfo>;

	sint32 i;
// Added by Martin Gühmann

	// Same as well information about existing
	// tile improvments, except roadlike ones,
	// so that this information is available
	// later as well.
	// And in order not to break anythink use the existing
	// list for unfinished tile improvements.
	const TerrainImprovementRecord *rec;
	for(i = 0; i < cell->GetNumDBImprovements(); i++) {
		sint32 imp = cell->GetDBImprovement(i);
		rec = g_theTerrainImprovementDB->Get(imp);
		m_improvements->AddTail(new UnseenImprovementInfo(imp, 100));
	}
	for(i = 0; i < cell->GetNumImprovements(); i++) {
		TerrainImprovement imp = cell->AccessImprovement(i);
		if(g_theTerrainImprovementPool->IsValid(imp)) {
			m_improvements->AddTail(new UnseenImprovementInfo(imp.GetType(),
															  imp.PercentComplete()));
		}
	}

	static DynamicArray<Installation> instArray;
	instArray.Clear();
	g_theInstallationTree->GetAt(point, instArray);
	for(i = 0; i < instArray.Num(); i++) {
		m_installations->AddTail(new UnseenInstallationInfo(instArray[i].GetType(),
														   instArray[i].GetVisibility()));
	}
	
	m_cell_owner = (sint8) cell->GetOwner();
// Added by Martin Gühmann
	// Store the city that controlls this tile.
	m_visibleCityOwner = g_theWorld->GetCell(point)->GetCityOwner().m_id;

	if(cell->GetCity().m_id != (0)) {
		Unit		city = cell->GetCity();

		m_citySize = (sint16)city.PopCount();
		m_citySpriteIndex = (sint16)city.CD()->GetDesiredSpriteIndex();
		const MBCHAR *name = city.GetName();
		m_cityName = new MBCHAR[strlen(name) + 1];
		strcpy(m_cityName, name);
		
// Added by Martin Gühmann
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


		}

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









		m_bioInfectedOwner = (sint8)cityData->GetOwner();
		m_nanoInfectedOwner = (sint8)cityData->GetOwner();
		m_convertedOwner = (sint8)cityData->IsConvertedTo();
		m_franchiseOwner = (sint8)cityData->GetFranchiseOwner();
		m_injoinedOwner = (sint8)cityData->GetOwner();
		m_happinessAttackOwner = (sint8)cityData->GetOwner();

		
		m_slaveBits = cityData->GetSlaveBits();







	} else {
		m_citySize = 0;
		m_cityName = NULL;
		m_cityOwner = 0;
		m_actor = NULL;
		m_citySpriteIndex = 0;

		m_flags = 0;












		m_bioInfectedOwner = 0;
		m_nanoInfectedOwner = 0;
		m_convertedOwner = 0;
		m_franchiseOwner = 0;
		m_injoinedOwner = 0;
		m_happinessAttackOwner = 0;

		m_slaveBits = 0;

	}

	
	GoodyHut *hut = g_theWorld->GetGoodyHut((MapPoint)point);

	SetHasHut(hut != NULL);
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
{
	m_tileInfo = NULL;

	m_flags = 0;












	m_bioInfectedOwner = 0;
	m_nanoInfectedOwner = 0;
	m_convertedOwner = 0;
	m_franchiseOwner = 0;
	m_injoinedOwner = 0;
	m_happinessAttackOwner = 0;

	m_slaveBits = 0;

	m_cell_owner = -1;
	// Added by Martin Gühmann
	m_visibleCityOwner = 0;

	m_actor = NULL;
#ifdef BATTLE_FLAGS
	m_battleFlags = 0;
#endif
	m_citySize = 0;
	m_cityOwner = 0;
	m_cityName = NULL;
	m_citySpriteIndex = 0;

	m_installations = new PointerList<UnseenInstallationInfo>;
	m_improvements = new PointerList<UnseenImprovementInfo>;
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
{
	m_installations = NULL;
	m_improvements = NULL;
	m_cityName = NULL;
	m_tileInfo = NULL;
	m_actor = NULL;

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
	if (m_actor) {
		m_actor->m_refCount--;
		if(m_actor->m_refCount <= 0) {
			
			
			
			g_director->ActiveUnitRemove(m_actor);

			delete m_actor;
			m_actor = NULL;
		}
		m_actor = NULL;
	}

    if(m_tileInfo) {
		delete m_tileInfo;
		m_tileInfo = NULL;
	}

	if(m_installations) {
		m_installations->DeleteAll();
		delete m_installations;
		m_installations = NULL;
	}

	if(m_improvements) {
		m_improvements->DeleteAll();
		delete m_improvements;
		m_improvements = NULL;
	}

	if(m_cityName) {
		delete [] m_cityName;
		m_cityName = NULL;
	}
}

#if 0
void *UnseenCell::operator new(size_t size)
{
	Assert(g_theUnseenPond);
	if(g_theUnseenPond) {
		int index;
		UnseenCell *ucell = g_theUnseenPond->Get_Next_Pointer(index);
		ucell->m_poolIndex = index;
		return ucell;
	} else {
		UnseenCell *ucell = ::new UnseenCell;
		ucell->m_poolIndex = -1;
		return ucell;
	}
}

void UnseenCell::operator delete(void *ptr)
{
	UnseenCell *ucell = (UnseenCell *)ptr;
	if(ucell->m_poolIndex >= 0) {
		g_theUnseenPond->Release_Pointer(ucell->m_poolIndex);
	} else {
		::delete(ptr);
	}
}
#endif

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
//              should be. Return type should be bool. But does not seem
//              to be used.
//
//----------------------------------------------------------------------------
sint32 UnseenCell::IsAirfield(void)
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
// Returns    : sint32: 1 if location has a listening post, 0 otherwise.
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be. Return type should be bool. But does not seem
//              to be used.
//
//----------------------------------------------------------------------------
sint32 UnseenCell::IsListeningPost(void)
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
// Returns    : sint32: 1 if location has a fort, 0 otherwise.
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be. Return type should be bool. But does not seem
//              to be used.
//
//----------------------------------------------------------------------------
sint32 UnseenCell::IsFort(void)
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
// Returns    : sint32: 1 if location has a radar, 0 otherwise.
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be. Return type should be bool. But does not seem
//              to be used.
//
//----------------------------------------------------------------------------
sint32 UnseenCell::IsRadar(void)
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
// Returns    : sint32: 1 if location can heal units, 0 otherwise.
//
// Remark(s)  : Does not seem to depend on hidden tile information as it 
//              should be. Return type should be bool. But does not seem
//              to be used.
//
//----------------------------------------------------------------------------
sint32 UnseenCell::IsHealUnits(void)
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

	if(m_cityName != NULL && rec->GetEnvCity()) {
		food += rec->GetEnvCityPtr()->GetFood();
	}

	if(g_theWorld->GetCell(m_point)->HasRiver() && rec->GetEnvRiver()) {
		food += rec->GetEnvRiverPtr()->GetFood();
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

	PointerList<UnseenImprovementInfo>::Walker *walker = 
			new PointerList<UnseenImprovementInfo>::Walker(m_improvements);

	while(walker->IsValid()){
		sint32 type		= walker->GetObj()->m_type;
		sint32 percent	= walker->GetObj()->m_percentComplete;
		if(percent < 100){
			break;
		}

		const TerrainImprovementRecord *impRec = 
			g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect *effect;
		effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
		sint32 bonus;
		if(effect && effect->GetBonusFood(bonus)) {
			food += bonus;
		}
		walker->Next();
	}
	
	delete walker;

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

	if(m_cityName != NULL && rec->GetEnvCity()) {
		shield += rec->GetEnvCityPtr()->GetShield();
	}

	if(g_theWorld->GetCell(m_point)->HasRiver() && rec->GetEnvRiver()) {
		shield += rec->GetEnvRiverPtr()->GetShield();
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

	PointerList<UnseenImprovementInfo>::Walker *walker = 
			new PointerList<UnseenImprovementInfo>::Walker(m_improvements);

	while(walker->IsValid()){
		sint32 type		= walker->GetObj()->m_type;
		sint32 percent	= walker->GetObj()->m_percentComplete;
		if(percent < 100){
			break;
		}

		const TerrainImprovementRecord *impRec = 
			g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect *effect;
		effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
		sint32 bonus;
		if(effect && effect->GetBonusProduction(bonus)) {
			shield += bonus;
		}
		walker->Next();
	}

	delete walker;
																					
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

	if(m_cityName != NULL && rec->GetEnvCity()) {
		gold += rec->GetEnvCityPtr()->GetGold();
	}

	if(g_theWorld->GetCell(m_point)->HasRiver() && rec->GetEnvRiver()) {
		gold += rec->GetEnvRiverPtr()->GetGold();
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

	PointerList<UnseenImprovementInfo>::Walker *walker = 
			new PointerList<UnseenImprovementInfo>::Walker(m_improvements);

	while(walker->IsValid()){
		sint32 type		= walker->GetObj()->m_type;
		sint32 percent	= walker->GetObj()->m_percentComplete;
		if(percent < 100){
			break;
		}

		const TerrainImprovementRecord *impRec = 
			g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect *effect;
		effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
		sint32 bonus;
		if(effect && effect->GetBonusGold(bonus)) {
			gold += bonus;
		}
		walker->Next();
	}

	delete walker;

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
	sint32 l,i;
	if(archive.IsStoring()) {
		m_point.Serialize(archive);
		archive.StoreChunk((uint8 *)&m_env, ((uint8 *)&m_slaveBits)+sizeof(m_slaveBits));

		{
// Added by Martin Gühmann
			// A dirty workaround in order not to change the save game format.
			// UnseenInstallationInfo is now abused to store m_visibleCityOwner
			// as visibility memeber. As both are of type uint32 it is not a 
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



		if(!m_cityName) {
			l = 0;
			archive << l;
		} else {
			l = strlen(m_cityName) + 1;
			archive << l;
			archive.Store((uint8*)m_cityName, (strlen(m_cityName) + 1) * sizeof(MBCHAR));
		}

		archive << (sint32)(m_actor != NULL);
		if (m_actor) {
			m_actor->Serialize(archive);
		}
		m_tileInfo->Serialize(archive);
	} else {
		m_point.Serialize(archive) ;
		archive.LoadChunk((uint8 *)&m_env, ((uint8 *)&m_slaveBits)+sizeof(m_slaveBits));

		
		
		if(m_installations) {
			m_installations->DeleteAll();
			delete m_installations;
			m_installations = NULL;
		}

		if(m_improvements) {
			m_improvements->DeleteAll();
			delete m_improvements;
			m_improvements = NULL;
		}
		if(m_cityName) {
			delete [] m_cityName;
			m_cityName = NULL;
		}
		if(m_tileInfo) {
			delete m_tileInfo;
			m_tileInfo = NULL;
		}



        m_installations = new PointerList<UnseenInstallationInfo>;
		archive >> l;

// Added by Martin Gühmann
		UnseenInstallationInfo* tmpUII;
		bool vCityOwnerNotSet = true;
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

		

		m_improvements = new PointerList<UnseenImprovementInfo>;
		archive >> l;
		for(i = 0; i < l; i++) {
			m_improvements->AddTail(new UnseenImprovementInfo(archive));
		}
		


		archive >> l;
		if(l > 0) {
			m_cityName = new MBCHAR[l];
			archive.Load((uint8*)m_cityName, l * sizeof(MBCHAR));
		} else {
			m_cityName = NULL;
		}

		
		Cell* cell = g_theWorld->GetCell(m_point) ;
		m_tileInfo = new TileInfo(g_theWorld->GetTileInfo(m_point));

		

















		sint32 hasActor;
		archive >> hasActor;
		if (hasActor) {
			m_actor = new UnitActor(archive);
        } else { 
            m_actor = NULL; 
        } 
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
// Parameters : archive			: The source archive
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
// Parameters : archive			: The source archive
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
