//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Gaia Controller 
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
// - Fixed neutral tile improvment bug by preventing the game
//   from reading invalid memory, by Martin Gühmann.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "FeatTracker.h"
#include "NewTurnCount.h"
#include "Player.h"
#include "AdvanceRecord.h"
#include "BuildingRecord.h"
#include "TerrainImprovementRecord.h"
#include "WonderRecord.h"
#include "CityData.h"
#include "CityInfluenceIterator.h"
#include "World.h"
#include "Installation.h"
#include "UnitData.h"

#include "Events.h"
#include "GameEventUser.h"
#include "GameEventManager.h"
#include "GaiaController.h"
#include "EndGameObjectRecord.h"
#include "TerrImprove.h"
#include "Cell.h"
#include "terrainutil.h"
#include "MaterialPool.h"
#include "MapAnalysis.h"

using namespace std ;


uint64 GaiaController::sm_endgameImprovements = 0x0;
uint64 GaiaController::sm_endgameBuildings = 0x0;
uint64 GaiaController::sm_endgameWonders = 0x0;
sint32 GaiaController::sm_towerEndgameIndex = 0x0;
sint32 GaiaController::sm_satelliteEndgameIndex = 0x0;
sint32 GaiaController::sm_mainframeEndgameIndex = 0x0;
sint32 GaiaController::sm_towerTileImpIndex = 0x0;
sint32 GaiaController::sm_satelliteBuildingIndex = 0x0;
sint32 GaiaController::sm_mainframeBuildingIndex = 0x0;

GaiaController::GaiaController(const PLAYER_INDEX player)
{
	m_playerId = player;

	Initialize();
}

void GaiaController::Serialize(CivArchive &archive)
{
	double tmp_val;
	if(archive.IsStoring()) {
		archive << m_playerId;
		archive << m_numMainframes;
		archive << m_numSatellites;
		archive << m_numWondersBuilt;
		archive << m_numTowersBuilt;
		tmp_val = m_percentCoverage;
		archive << tmp_val;
		archive << m_completedTurn;
	} else {
		Initialize();
		archive >> m_playerId;
		archive >> m_numMainframes;
		archive >> m_numSatellites;
		archive >> m_numWondersBuilt;
		archive >> m_numTowersBuilt;
		archive >> tmp_val;
		m_percentCoverage = (float) tmp_val;	
		archive >> m_completedTurn;
	}
	m_coveredCells.Serialize(archive);
}


void GaiaController::InitializeStatics()
{
	const TerrainImprovementRecord *terr_rec;
	const BuildingRecord *building_rec;
	const WonderRecord *wonder_rec;

	sm_towerEndgameIndex = 
		g_theEndGameObjectDB->FindRecordNameIndex("ENDGAME_PROCESSING_TOWER");
			
	Assert(sm_towerEndgameIndex >= 0);
	Assert(g_theEndGameObjectDB->Get(sm_towerEndgameIndex) >= 0);
	if (sm_towerEndgameIndex >= 0 && g_theEndGameObjectDB->Get(sm_towerEndgameIndex))
	{
		terr_rec = g_theEndGameObjectDB->Get(sm_towerEndgameIndex)->
			GetTerrainImprovementPtr();
		sm_towerTileImpIndex = terr_rec->GetIndex();
	}


	sm_satelliteEndgameIndex =
		g_theEndGameObjectDB->FindRecordNameIndex("ENDGAME_POWER_SATELLITE");

	Assert(sm_satelliteEndgameIndex >= 0);
	Assert(g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex) >= 0);
	if (sm_satelliteEndgameIndex >= 0 && g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex))
	{
		building_rec = g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex)->
			GetBuildingPtr();
		sm_satelliteBuildingIndex = building_rec->GetIndex();
	}

	sm_mainframeEndgameIndex =
		g_theEndGameObjectDB->FindRecordNameIndex("ENDGAME_GAIA_COMPUTER");

	Assert(sm_mainframeEndgameIndex >= 0);
	Assert(g_theEndGameObjectDB->Get(sm_mainframeEndgameIndex) >= 0);
	if (sm_mainframeEndgameIndex >= 0 && g_theEndGameObjectDB->Get(sm_mainframeEndgameIndex))
	{
		building_rec = g_theEndGameObjectDB->Get(sm_mainframeEndgameIndex)->
			GetBuildingPtr();
		sm_mainframeBuildingIndex = building_rec->GetIndex();
	}

	sm_endgameImprovements = 0x0;
	sm_endgameBuildings = 0x0;
	sm_endgameWonders = 0x0;

	sint32 type;
	for (sint32 i = 0; i < g_theEndGameObjectDB->NumRecords(); i++)
		{
			if (g_theEndGameObjectDB->Get(i)->GetTerrainImprovement())
				{
					terr_rec = g_theEndGameObjectDB->Get(i)->GetTerrainImprovementPtr();
					type = terr_rec->GetIndex();
					sm_endgameImprovements |= uint64((uint64)1 << (uint64)type);
				}

			if (g_theEndGameObjectDB->Get(i)->GetBuilding())
				{
					building_rec = g_theEndGameObjectDB->Get(i)->GetBuildingPtr();
					type = building_rec->GetIndex();
					sm_endgameBuildings |= uint64((uint64)1 << (uint64)type);
				}

			if (g_theEndGameObjectDB->Get(i)->GetWonder())
				{
					wonder_rec = g_theEndGameObjectDB->Get(i)->GetWonderPtr();
					type = wonder_rec->GetIndex();
					sm_endgameWonders |= uint64((uint64)1 << (uint64)type);
				}
		}
}

void GaiaController::Initialize()
{
	
	

	m_numMainframes = 0;
	m_numSatellites = 0;
	m_numTowersBuilt = 0;
	m_numWondersBuilt = 0;
	m_percentCoverage = 0.0;
	m_completedTurn = -1;

	sint32 x_size = g_theWorld->GetXWidth();
	sint32 y_size = g_theWorld->GetYHeight();

	
	m_coveredCells.Resize( x_size, y_size, 0 );
}



GaiaController::~GaiaController()
{
	m_coveredCells.Resize(0, 0, false);
}


void GaiaController::RecomputeCoverage()
{
	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);
	if (player_ptr == NULL)
		return;

	MapPoint pos;
	sint32 type;
	sint32 radius = GetTowerRadius();

	const DynamicArray<Installation> *tile_imps = 
		player_ptr->m_allInstallations;

	
	m_coveredCells.Reset( 0 );
	m_numTowersBuilt = 0;
	sint32 covered_cells = 0;

	MapPoint cell_pos;

	for(sint32 i = 0; i < tile_imps->Num(); i++) {
		type = tile_imps->Access(i).GetType();
		if (type == sm_towerTileImpIndex)
			{
				m_numTowersBuilt++;

				
				tile_imps->Access(i).GetPos(pos);

				
				RadiusIterator it(pos, radius);

				for(it.Start(); !it.End(); it.Next()) 
					{
						cell_pos = it.Pos();
						
						if (m_coveredCells.Get(cell_pos.x, cell_pos.y) == FALSE)
							{
								
								covered_cells++;
								m_coveredCells.Set(cell_pos.x, cell_pos.y, 1);
							}
					}
			}
	}
	m_percentCoverage = ((float) covered_cells / 
						 (g_theWorld->GetXWidth() * g_theWorld->GetYHeight()));

}


STDEHANDLER(GaiaController_CaptureCity)
{
	Unit city;
	sint32 newOwner;
	sint32 cause;
	MapPoint pos;
	sint32 originalOwner;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, newOwner))
		return GEV_HD_Continue;

	if(!args->GetInt(0, cause))
		return GEV_HD_Continue;

	originalOwner = city.GetOwner();

	Player *owner_player = g_player[newOwner];
	Player *original_player = g_player[originalOwner];

	CityData *city_data = city->GetCityData();
	Assert(city_data);
	if (city_data == NULL)
		return GEV_HD_Continue;

	
	uint64 city_buildings = city_data->GetImprovements();
	uint64 city_wonders = city_data->GetBuiltWonders();
	if (((city_buildings & GaiaController::sm_endgameBuildings) == 0x0) &&
		((city_wonders & GaiaController::sm_endgameWonders) == 0x0))
		return GEV_HD_Continue;

	
	sint32 type;
	for (type = 0; type < g_theBuildingDB->NumRecords(); type++)
		{
			if ((city_buildings & ((uint64)0x1 << (uint64)type)) &&
				(GaiaController::sm_endgameBuildings & ((uint64)0x1 << (uint64)type)))
				{
					
					if (owner_player &&	owner_player->GetGaiaController())
						{
							owner_player->GetGaiaController()->
								HandleBuildingChange(type, city, 1);
						}

					
					if (original_player && original_player->GetGaiaController())
						{
							original_player->GetGaiaController()->
								HandleBuildingChange(type, city, -1);
						}
				}
		}

	
	for (type = 0; type < g_theWonderDB->NumRecords(); type++)
		{
			if ((city_wonders & ((uint64)0x1 << (uint64)type)) &&
				(GaiaController::sm_endgameWonders & ((uint64)0x1 << (uint64)type)))
				{
					
					if (owner_player &&	owner_player->GetGaiaController())
						{
							owner_player->GetGaiaController()->
								HandleWonderChange(type,1);
						}

					
					if (original_player && original_player->GetGaiaController())
						{
							original_player->GetGaiaController()->
								HandleWonderChange(type,-1);
						}
				}
		}

	return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_CutImprovements)
{
	sint32 owner;
	sint32 type;
	MapPoint pos;

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	TerrainImprovement ti;
	Cell *cell = g_theWorld->GetCell(pos);
	owner = cell->GetOwner();

	//Added by Martin Gühmann to prevent
	//the game from accessing an invalid
	//area of memory, plain arrays don't
	//have out of bounds array, so accessing
	//g_player[-1] may give you everything.
	//It is not very probably that you get 0.
	if (owner == -1)
		return GEV_HD_Continue;

	Player *owner_player = g_player[owner];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	
	sint32 num = cell->GetNumDBImprovements();
	for (sint16 i = 0; i < num; i++)
		{
			type = cell->GetDBImprovement(i);

			if (GaiaController::sm_endgameImprovements & ((uint64)0x1 << (uint64)type))
				{
					owner_player->GetGaiaController()->
						HandleTerrImprovementChange(type,pos, -1);
				}
		}

		return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_ImprovementComplete)
{
	sint32 owner;
	sint32 type;
	MapPoint pos;

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, owner))
		return GEV_HD_Continue;
	
	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	//Added by Martin Gühmann to prevent
	//the game from accessing an invalid
	//area of memory, plain arrays don't
	//have out of bounds array, so accessing
	//g_player[-1] may give you everything.
	//It is not very probably that you get 0.
	if (owner == -1)
		return GEV_HD_Continue;
	
	Player *owner_player = g_player[owner];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameImprovements & ((uint64)0x1 << (uint64)type))
		{
			
			owner_player->GetGaiaController()->
				HandleTerrImprovementChange(type, pos, 1);
		}
	return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_SellBuilding)
{
	sint32 type;
	Unit city;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	
	Player *owner_player = g_player[city.GetOwner()];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameBuildings & ((uint64)0x1 << (uint64)type))
		{
			
			owner_player->GetGaiaController()->
				HandleBuildingChange(type, city, -1);
		}
	return GEV_HD_Continue;
}



STDEHANDLER(GaiaController_CreateBuilding)
{
	sint32 type;
	Unit city;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	
	Player *owner_player = g_player[city.GetOwner()];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameBuildings & ((uint64)0x1 << (uint64)type))
		{
			
			owner_player->GetGaiaController()->
				HandleBuildingChange(type, city, 1);
		}
	return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_DisbandCity)
{
	Unit city;
	sint32 owner;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	CityData *city_data = city->GetCityData();
	Assert(city_data);
	if (city_data == NULL)
		return GEV_HD_Continue;

	owner = city.GetOwner();

	Player *owner_player = g_player[owner];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	
	uint64 city_buildings = city_data->GetImprovements();
	uint64 city_wonders = city_data->GetBuiltWonders();
	if (((city_buildings & GaiaController::sm_endgameBuildings) == 0x0) &&
		((city_wonders & GaiaController::sm_endgameWonders) == 0x0))
		return GEV_HD_Continue;

	
	sint32 type;
	for (type = 0; type < g_theBuildingDB->NumRecords(); type++)
		{
			if ((city_buildings & ((uint64)0x1 << (uint64)type)) &&
				(GaiaController::sm_endgameBuildings & ((uint64)0x1 << (uint64)type)))
				{
					
					owner_player->GetGaiaController()->
						HandleBuildingChange(type, city, -1);
				}
		}

	
	for (type = 0; type < g_theWonderDB->NumRecords(); type++)
		{
			if ((city_wonders & ((uint64)0x1 << (uint64)type)) &&
				(GaiaController::sm_endgameWonders & ((uint64)0x1 << (uint64)type)))
				{
					
					owner_player->GetGaiaController()->
						HandleWonderChange(type, -1);
				}
		}

	return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_CreateWonder)
{
	sint32 type;
	Unit city;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	
	Player *owner_player = g_player[city.GetOwner()];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameWonders & ((uint64)0x1 << (uint64)type))
		{
			
			owner_player->GetGaiaController()->
				HandleWonderChange(type, 1);
		}
	return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_BuildingRemoved)
{
	sint32 type;
	Unit city;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	
	Player *owner_player = g_player[city.GetOwner()];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameBuildings & ((uint64)0x1 << (uint64)type))
		{
			
			owner_player->GetGaiaController()->
				HandleBuildingChange(type, city, -1);
		}
	return GEV_HD_Continue;
}


STDEHANDLER(GaiaController_WonderRemoved)
{
	sint32 type;
	Unit city;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	
	Player *owner_player = g_player[city.GetOwner()];
	if (owner_player == NULL ||
		owner_player->GetGaiaController() == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameWonders & ((uint64)0x1 << (uint64)type))
		{
			
			owner_player->GetGaiaController()->
				HandleWonderChange(type, -1);
		}
	return GEV_HD_Continue;
}

void GaiaController::InitializeEvents()
{
	g_gevManager->AddCallback(GEV_CaptureCity, 
							  GEV_PRI_Pre, 
							  &s_GaiaController_CaptureCity);

	g_gevManager->AddCallback(GEV_CutImprovements,
							  GEV_PRI_Pre,
							  &s_GaiaController_CutImprovements);

	g_gevManager->AddCallback(GEV_ImprovementComplete, 
							  GEV_PRI_Post,
							  &s_GaiaController_ImprovementComplete);

	g_gevManager->AddCallback(GEV_SellBuilding, 
							  GEV_PRI_Pre,
							  &s_GaiaController_SellBuilding);

	g_gevManager->AddCallback(GEV_CreateBuilding, 
							  GEV_PRI_Post,
							  &s_GaiaController_CreateBuilding);

	g_gevManager->AddCallback(GEV_DisbandCity, 
							  GEV_PRI_Pre,
							  &s_GaiaController_DisbandCity);

	g_gevManager->AddCallback(GEV_CreateWonder, 
							  GEV_PRI_Post,
							  &s_GaiaController_CreateWonder);

	g_gevManager->AddCallback(GEV_BuildingRemoved, 
							  GEV_PRI_Pre,
							  &s_GaiaController_BuildingRemoved);

	g_gevManager->AddCallback(GEV_WonderRemoved, 
							  GEV_PRI_Pre,
							  &s_GaiaController_WonderRemoved);
}
	
void GaiaController::CleanupEvents()
{
}

void GaiaController::HandleBuildingChange(const sint32 type, Unit & city, const sint16 delta)
{
	Assert(GaiaController::sm_endgameBuildings & ((uint64)0x1 << (uint64)type));

	if (sm_satelliteBuildingIndex == type)
	{
		m_numSatellites += delta;
		RecomputeCoverage();

		
		if (delta > 0)
		{
			Assert(city->GetCityData());
			CityData *city_data = city->GetCityData();
			city_data->SetImprovements(city_data->GetImprovements() & ~((uint64)0x1 << (uint64)type));
		}
		else
		{
			if (!CanStartCountdown())
				m_completedTurn = -1;
		}
	}
	else if (sm_mainframeBuildingIndex == type)
	{
		m_numMainframes += delta;
		if (delta < 0)
		{
			if (!CanStartCountdown())
				m_completedTurn = -1;
		}
	}
	
}

void GaiaController::HandleWonderChange(const sint32 type, const sint16 delta)
{
	Assert(GaiaController::sm_endgameWonders & ((uint64)0x1 << (uint64)type));
	m_numWondersBuilt += delta;

	if (delta < 0)
	{
		if (!CanStartCountdown())
			m_completedTurn = -1;
	}

	
}

void GaiaController::HandleTerrImprovementChange(const sint32 type, const MapPoint & pos, const sint16 delta)
{
	Assert(GaiaController::sm_endgameImprovements & ((uint64)0x1 << (uint64)type));
	if (sm_towerTileImpIndex == type)
		{
			m_numTowersBuilt += delta;
			RecomputeCoverage();

			if (delta < 0)
			{
				if (!CanStartCountdown())
					m_completedTurn = -1;
			}
		}
	
}

sint16 GaiaController::NumMainframesBuilt() const
{
	return m_numMainframes;
}

sint16 GaiaController::NumSatellitesLaunched() const
{
	return m_numSatellites;
}

sint16 GaiaController::NumTowersBuilt() const
{
	return m_numTowersBuilt;
}

sint16 GaiaController::NumWondersBuilt() const
{
	return m_numWondersBuilt;
}

sint16 GaiaController::NumSatellitesRequired() const
{
	sint32 value = 0;
	if (g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex)->GetMinNeeded())
		g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex)->GetMinNeeded(value);
	
	return (sint16) value;
}

sint16 GaiaController::MaxSatellitesAllowed() const
{
	sint32 value = 0;
	if(g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex)->GetMaxNeeded())
		g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex)->GetMaxNeeded(value);

	return (sint16) value;
}

sint16 GaiaController::NumMainframesRequired() const
{
	sint32 value = 0;
	if (g_theEndGameObjectDB->Get(sm_mainframeEndgameIndex)->GetMinNeeded())
		g_theEndGameObjectDB->Get(sm_mainframeEndgameIndex)->GetMinNeeded(value);
	return (sint16) value;
}

sint16 GaiaController::NumTowersRequired() const
{
	sint32 value = 0;
	if (g_theEndGameObjectDB->Get(sm_towerEndgameIndex)->GetMinNeeded())
		g_theEndGameObjectDB->Get(sm_towerEndgameIndex)->GetMinNeeded(value);
	
	return (sint16) value;
}

double GaiaController::TowerCoverageRequired() const
{
	double value = 0;
	if (g_theEndGameObjectDB->Get(sm_towerEndgameIndex)->GetMinCoverage())
		g_theEndGameObjectDB->Get(sm_towerEndgameIndex)->GetMinCoverage(value);
	
	return value;
}

sint16 GaiaController::GetTowerRadius() const
{
	Assert(sm_towerEndgameIndex >= 0);
	const EndGameObjectRecord *tower_rec = 
		g_theEndGameObjectDB->Get(sm_towerEndgameIndex);
	Assert(sm_satelliteEndgameIndex >= 0);
	const EndGameObjectRecord *satellite_rec = 
		g_theEndGameObjectDB->Get(sm_satelliteEndgameIndex);

	sint32 min_radius = 0;
	if (tower_rec->GetMinRadius())
		tower_rec->GetMinRadius(min_radius);

	sint32 max_radius = 0;
	if (tower_rec->GetMaxRadius())
		tower_rec->GetMaxRadius(max_radius);
	Assert(max_radius);

	sint32 min_satellites = 0;
	if (satellite_rec->GetMinNeeded())
		satellite_rec->GetMinNeeded(min_satellites);

	sint32 max_satellites = 0;
	if (satellite_rec->GetMaxNeeded())
		satellite_rec->GetMaxNeeded(max_satellites);
	Assert(max_satellites);

	sint16 add_radius = 0;
	if ( (m_numSatellites > min_satellites) &&
		 (max_radius > min_radius) )
		{
			double ratio = ((double)(m_numSatellites - min_satellites) / 
							(max_satellites - min_satellites));
			if (ratio > 1.0)
				add_radius = (sint16) (max_radius - min_radius);
			else
				add_radius = (sint16) ceil(ratio * (double) (max_radius - min_radius));
		}

	return ((sint16) min_radius + add_radius);
}


float GaiaController::GetTowerCoverage() const
{
	return m_percentCoverage;	
}


float GaiaController::NewCoverageFrom(const MapPoint & pos, const sint16 radius) const
{
	RadiusIterator it(pos, radius);
	MapPoint cell_pos;
	sint32 covered_cells = 0;

	
	for(it.Start(); !it.End(); it.Next()) 
		{
			cell_pos = it.Pos();
			
			if (m_coveredCells.Get(cell_pos.x, cell_pos.y) != FALSE)
				covered_cells++;
		}
	return (float) (covered_cells / 
			(g_theWorld->GetXWidth() * g_theWorld->GetYHeight()));
}


const Bit_Table & GaiaController::GetCoverage() const
{
	return m_coveredCells;
}


bool GaiaController::CanStartCountdown() const
{
	if (GetTowerCoverage() < TowerCoverageRequired()) 
		return false;

	if (NumTowersBuilt() < NumTowersRequired()) 
		return false;

	if (NumMainframesBuilt() < NumMainframesRequired())
		return false;

	if (NumSatellitesLaunched() < NumSatellitesRequired())
		return false;

	
	

	return true;
}

bool GaiaController::HasReqTowerCoverage() const
{
	if(GetTowerCoverage() >= TowerCoverageRequired()) 
		return true;
	else 
		return false;
}

bool GaiaController::HasMinTowersBuilt() const
{
	if(NumTowersBuilt() >= NumTowersRequired()) 
		return true;
	else 
		return false;
}

bool GaiaController::HasMinCoresBuilt() const
{
	if(NumMainframesBuilt() >= NumMainframesRequired())
		return true;
	else 
		return false;
}

bool GaiaController::HasMinSatsBuilt() const
{
	if(NumSatellitesLaunched() >= NumSatellitesRequired())
		return true;
	else 
		return false;
}

bool GaiaController::HasMaxSatsBuilt() const
{
	if(NumSatellitesLaunched() >= MaxSatellitesAllowed())
		return true;
	else 
		return false;
}



bool GaiaController::StartCountdown()
{
	
	sint32 max_turns_to_activate = TotalCountdownTurns();

	
	if (CanStartCountdown())
	{
		if (m_completedTurn < 0)
		{
			m_completedTurn = 
				(sint16) (NewTurnCount::GetCurrentRound() + max_turns_to_activate);
		}
		
		return true;
	}
	else
	{
		m_completedTurn = -1;
		return false;
	}
}


sint32 GaiaController::TotalCountdownTurns() const
{
	
	static sint32 max_turns_to_activate = -1;
	sint32 turns;
	if (max_turns_to_activate == -1)
		{
			for (sint32 index = 0; index < g_theEndGameObjectDB->NumRecords(); index++)
				{
					if (g_theEndGameObjectDB->Get(index)->GetTurnsToActivate())
						{
							g_theEndGameObjectDB->Get(index)->GetTurnsToActivate(turns);
							if (turns > max_turns_to_activate)
							{
								max_turns_to_activate = turns;
							}
						}
				}
		}

	
	return(max_turns_to_activate);
}


sint16 GaiaController::TurnsToComplete() const
{
	if (m_completedTurn >= 0)
		if (m_completedTurn < NewTurnCount::GetCurrentRound())
			return 0;
		else
			return (sint16) (m_completedTurn - NewTurnCount::GetCurrentRound());
	else
		return -1;
}


bool GaiaController::GaiaControllerTileImp(const sint32 type) const
{
	if (type == sm_towerTileImpIndex)
		return true;
	return false;
}


bool GaiaController::CanBuildTowers(const bool & check_pw) const
{
	const TerrainImprovementRecord *rec =
		g_theTerrainImprovementDB->Get(sm_towerTileImpIndex);

	return terrainutil_CanPlayerBuild(rec, m_playerId, check_pw);
}



sint32 GaiaController::ScoreTowerPosition(MapPoint & pos, const MapPoint empire_center, MapPoint_List & towers) const
{
	

	
	static sint32 optimal_distance = -1;
	if (optimal_distance < 0)
		{
			const EndGameObjectRecord *tower_rec = 
				g_theEndGameObjectDB->Get(sm_towerEndgameIndex);
			Assert(tower_rec);

			sint32 min_radius = 0;
			if (tower_rec->GetMinRadius())
				tower_rec->GetMinRadius(min_radius);

			sint32 max_radius = 0;
			if (tower_rec->GetMaxRadius())
				tower_rec->GetMaxRadius(max_radius);


			if (max_radius != 0)
				{
					
					optimal_distance = 
						(sint32) floor(1.75 * (float) min_radius + ((float)(max_radius - min_radius) / 2.0));

					
					optimal_distance *= optimal_distance;
				}
			else
				optimal_distance = 1;
		}

	
	sint32 empire_distance =
		MapPoint::GetSquaredDistance(pos, empire_center);
	
	
	MapPoint_List::const_iterator tower_iter;
	sint32 tmp_distance;
	sint32 tmp_score;
	sint32 min_score = optimal_distance;
	for (tower_iter = towers.begin(); tower_iter != towers.end(); tower_iter++)
		{
			
			tmp_distance = MapPoint::GetSquaredDistance(pos, *tower_iter);
			tmp_score = optimal_distance;
			if (tmp_distance <= optimal_distance)
				tmp_score -= (optimal_distance - tmp_distance);
			else
				tmp_score -= ((tmp_distance - optimal_distance)/2 > optimal_distance ? 0 : (tmp_distance - optimal_distance)/2);
			if (tmp_score < min_score)
				{
					min_score = tmp_score;
				}
		}

	
	sint32 max_distance = (g_theWorld->GetHeight() * g_theWorld->GetWidth()) * 
		(g_theWorld->GetHeight() * g_theWorld->GetWidth()); 
	min_score += 
		(optimal_distance/10) * (1.0 - ((float) empire_distance / (float) max_distance));
	
	return min_score;
}


void GaiaController::ComputeTowerCandidates(Scored_MapPoint_List & candidates) const
{
	MapPoint pos;
	const TerrainImprovementRecord *rec =
		g_theTerrainImprovementDB->Get(sm_towerTileImpIndex);

	for (pos.x = 0; pos.x < g_theWorld->GetWidth(); pos.x++)
		for (pos.y = 0; pos.y < g_theWorld->GetHeight(); pos.y++)
			{
				if (terrainutil_CanPlayerBuildAt(rec, m_playerId, pos))
					{
						candidates.push_back(pair<sint32,MapPoint>(-1,pos));
					}
			}
}



void GaiaController::ComputeTowerPositions()
{
	MapPoint_List towers;
	Scored_MapPoint_List candidates;
	MapPoint pos;

	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);
	if (player_ptr == NULL)
		return;

	
	if (!CanBuildTowers(true))
		return;

	
	
	ComputeTowerCandidates(candidates);

	
	m_maxPercentCoverage = candidates.size();
	m_maxPercentCoverage /= (g_theWorld->GetWidth() * g_theWorld->GetHeight());
	m_maxPercentCoverage *= (float) 1.2;

	
	const DynamicArray<Installation> *tile_imps = 
		player_ptr->m_allInstallations;

	sint32 type;
	for(sint32 i = 0; i < tile_imps->Num(); i++) {
		type = tile_imps->Access(i).GetType();
		if (type == sm_towerTileImpIndex)
			{
				tile_imps->Access(i).GetPos(pos);
				towers.push_back(pos);
			}
	}

	
	m_newTowerPositions.clear();

	
	Assert(candidates.size());
	if (candidates.size() <= 0)
	{
		return;
	}

	
	sint32 tower_pw_cost;
	sint32 total_pw = player_ptr->m_materialPool->GetMaterials();
	do {

		
		MapPoint empire_pos = MapAnalysis::GetMapAnalysis().GetEmpireCenter(m_playerId);
		Scored_MapPoint_List::iterator candidate_iter;
		for (candidate_iter = candidates.begin(); 
			 candidate_iter != candidates.end(); 
			 candidate_iter++)
			{
				candidate_iter->first = ScoreTowerPosition(candidate_iter->second, empire_pos, towers); 
			}

		
		candidates.sort(std::greater<pair<sint32, MapPoint> >());

		
	    pos =  candidates.front().second;
		candidates.pop_front();
		m_newTowerPositions.push_back(pos);

		
		towers.push_back(pos);

		
		tower_pw_cost = terrainutil_GetProductionCost(sm_towerTileImpIndex, pos, -1);
		total_pw -= tower_pw_cost;
	} while (total_pw > 0);
}


float GaiaController::GetMaxTowerCoverage() const
{
	return m_maxPercentCoverage;
}


bool GaiaController::PopNextTowerPosition(MapPoint & pos)
{
	const TerrainImprovementRecord *rec =
		g_theTerrainImprovementDB->Get(sm_towerTileImpIndex);

	bool found = false;
	while (m_newTowerPositions.size() > 0 && !found)
	{
		pos = m_newTowerPositions.front();
		m_newTowerPositions.pop_front();
		found = terrainutil_CanPlayerBuildAt(rec, m_playerId, pos);
	}
	return found;
}


void GaiaController::BuildProcessingTowers()
{
	
	MapPoint pos;
	bool found = PopNextTowerPosition(pos);
	if (!found)
	{
		
		ComputeTowerPositions();
		found = PopNextTowerPosition(pos);
	}
	
	while (found)
	{
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateImprovement,
			GEA_Player, m_playerId,
			GEA_MapPoint, pos,
			GEA_Int, GetTowerTileImpIndex(),
			GEA_Int, 0,  
			GEA_End);
		found = PopNextTowerPosition(pos);
	}
}

