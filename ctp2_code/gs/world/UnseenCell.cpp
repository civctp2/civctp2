









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

extern Director				*g_director;
extern World				*g_theWorld;
extern	TiledMap			*g_tiledMap ;
extern SelectedItem			*g_selected_item;
extern Pool<UnseenCell>     *g_theUnseenPond;

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

	if(cell->GetCity().m_id != (0)) {
		Unit		city = cell->GetCity();

		m_citySize = (sint16)city.PopCount();
		m_citySpriteIndex = (sint16)city.CD()->GetDesiredSpriteIndex();
		const MBCHAR *name = city.GetName();
		m_cityName = new MBCHAR[strlen(name) + 1];
		strcpy(m_cityName, name);
		
		m_cityOwner = (sint16)city.GetOwner();

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

UnseenCell::UnseenCell(CivArchive &archive)
{
	m_installations = NULL;
	m_improvements = NULL;
	m_cityName = NULL;
	m_tileInfo = NULL;
	m_actor = NULL;

	Serialize(archive);
}

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

sint32 UnseenCell::IsAirfield(void)
{
	return terrainutil_HasAirfield(m_point);
}

sint32 UnseenCell::IsListeningPost(void)
{
	return terrainutil_HasListeningPost(m_point);
}























sint32 UnseenCell::IsFort(void)
{
	return terrainutil_HasFort(m_point);
}

sint32 UnseenCell::IsRadar(void)
{
	return terrainutil_HasRadar(m_point);
}

sint32 UnseenCell::IsHealUnits(void)
{
	return terrainutil_HasFort(m_point);
}


void UnseenCell::Serialize(CivArchive &archive)
{
	sint32 l,i;
	if(archive.IsStoring()) {
		m_point.Serialize(archive);
		archive.StoreChunk((uint8 *)&m_env, ((uint8 *)&m_slaveBits)+sizeof(m_slaveBits));

		{
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
		for(i = 0; i < l; i++) {
			m_installations->AddTail(new UnseenInstallationInfo(archive));
		}

		

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

UnseenInstallationInfo::UnseenInstallationInfo(CivArchive &archive)
{
	archive.Load((uint8*)&m_type, sizeof(UnseenInstallationInfo));
}

UnseenImprovementInfo::UnseenImprovementInfo(CivArchive &archive)
{
	archive.Load((uint8*)&m_type, sizeof(UnseenImprovementInfo));
}
