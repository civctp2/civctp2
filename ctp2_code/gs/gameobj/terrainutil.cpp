//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Terrain utilities
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected non-standard syntax.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "terrainutil.h"
#include "TerrainRecord.h"
#ifndef __TILETOOL__
#include "MapPoint.h"
#include "World.h"
#include "TerrainImprovementRecord.h"
#include "Cell.h"
#include "Player.h"
#include "UnitDynArr.h"
#include "QuadTree.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "Director.h"
#include "SelItem.h"
#include "MaterialPool.h"
#include "GaiaController.h"
#include "AgreementMatrix.h"
#include "ConstDB.h"
#include "Installation.h"
#include "CityInfluenceIterator.h"
#include "TiledMap.h"
#include "AdvanceRecord.h"
#include "network.h"

extern QuadTree<Unit> *g_theUnitTree;
#endif

sint32 s_TERRAIN_FOREST;
sint32 s_TERRAIN_PLAINS;
sint32 s_TERRAIN_TUNDRA;
sint32 s_TERRAIN_GLACIER;
sint32 s_TERRAIN_GRASSLAND;
sint32 s_TERRAIN_DESERT;
sint32 s_TERRAIN_SWAMP;
sint32 s_TERRAIN_JUNGLE;
sint32 s_TERRAIN_MOUNTAIN;
sint32 s_TERRAIN_HILL;
sint32 s_TERRAIN_WATER_SHALLOW;
sint32 s_TERRAIN_WATER_DEEP;
sint32 s_TERRAIN_WATER_VOLCANO;
sint32 s_TERRAIN_WATER_BEACH;
sint32 s_TERRAIN_WATER_SHELF;
sint32 s_TERRAIN_WATER_TRENCH;
sint32 s_TERRAIN_WATER_RIFT;
sint32 s_TERRAIN_DEAD;
sint32 s_TERRAIN_BROWN_HILL;
sint32 s_TERRAIN_BROWN_MOUNTAIN;
sint32 s_TERRAIN_WHITE_HILL;
sint32 s_TERRAIN_WHITE_MOUNTAIN;
sint32 s_TERRAIN_WATER_KELP;
sint32 s_TERRAIN_WATER_REEF;
sint32 s_TERRAFORM_HILLS_IMPROVEMENT;
sint32 s_TERRAFORM_GRASSLAND_IMPROVEMENT;
sint32 s_TERRAFORM_PLAINS_IMPROVEMENT;
sint32 s_TERRAFORM_FORESTS_IMPROVEMENT;

void terrainutil_Initialize()
{
	sint32 i;
	for(i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		const TerrainRecord *rec = g_theTerrainDB->Get(i);
		if(rec->GetInternalTypeForest()) {
			s_TERRAIN_FOREST = i;
		}
		if(rec->GetInternalTypePlains()) {
			s_TERRAIN_PLAINS = i;
		}
		if(rec->GetInternalTypeTundra()) {
			s_TERRAIN_TUNDRA = i;
		}
		if(rec->GetInternalTypeGlacier()) {
			s_TERRAIN_GLACIER = i;
		}
		if(rec->GetInternalTypeGrassland()) {
			s_TERRAIN_GRASSLAND = i;
		}
		if(rec->GetInternalTypeDesert()) {
			s_TERRAIN_DESERT = i;
		}
		if(rec->GetInternalTypeSwamp()) {
			s_TERRAIN_SWAMP = i;
		}
		if(rec->GetInternalTypeJungle()) {
			s_TERRAIN_JUNGLE = i;
		}
		if(rec->GetInternalTypeMountain()) {
			s_TERRAIN_MOUNTAIN = i;
		}
		if(rec->GetInternalTypeHill()) {
			s_TERRAIN_HILL = i;
		}
		if(rec->GetInternalTypeWaterShallow()) {
			s_TERRAIN_WATER_SHALLOW = i;
		}
		if(rec->GetInternalTypeWaterDeep()) {
			s_TERRAIN_WATER_DEEP = i;
		}
		if(rec->GetInternalTypeWaterVolcano()) {
			s_TERRAIN_WATER_VOLCANO = i;
		}
		if(rec->GetInternalTypeWaterBeach()) {
			s_TERRAIN_WATER_BEACH = i;
		}
		if(rec->GetInternalTypeWaterShelf()) {
			s_TERRAIN_WATER_SHELF = i;
		}
		if(rec->GetInternalTypeWaterTrench()) {
			s_TERRAIN_WATER_TRENCH = i;
		}
		if(rec->GetInternalTypeWaterRift()) {
			s_TERRAIN_WATER_RIFT = i;
		}
		if(rec->GetInternalTypeDead()) {
			s_TERRAIN_DEAD = i;
		}
		if(rec->GetInternalTypeBrownHill()) {
			s_TERRAIN_BROWN_HILL = i;
		}
		if(rec->GetInternalTypeBrownMountain()) {
			s_TERRAIN_BROWN_MOUNTAIN = i;
		}
		if(rec->GetInternalTypeWhiteHill()) {
			s_TERRAIN_WHITE_HILL = i;
		}
		if(rec->GetInternalTypeWhiteMountain()) {
			s_TERRAIN_WHITE_MOUNTAIN = i;
		}		
		if(rec->GetInternalTypeWaterKelp()) {
			s_TERRAIN_WATER_KELP = i;
		}
		if(rec->GetInternalTypeWaterReef()) {
			s_TERRAIN_WATER_REEF = i;
		}
	}

	
	for(i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) {
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(i);
		if (rec->GetTerraformTerrainPtr()->GetIndex() == s_TERRAIN_HILL)
			s_TERRAFORM_HILLS_IMPROVEMENT = i;
		else if (rec->GetTerraformTerrainPtr()->GetIndex() == s_TERRAIN_GRASSLAND)
			s_TERRAFORM_GRASSLAND_IMPROVEMENT = i;
		else if (rec->GetTerraformTerrainPtr()->GetIndex() == s_TERRAIN_PLAINS)
			s_TERRAFORM_PLAINS_IMPROVEMENT = i;
		else if (rec->GetTerraformTerrainPtr()->GetIndex() == s_TERRAIN_FOREST)
			s_TERRAFORM_FORESTS_IMPROVEMENT = i;
	}
}

#ifndef __TILETOOL__

const TerrainImprovementRecord *terrainutil_GetBestRoad(sint32 player, const MapPoint &pos)
{
	sint32 i;
	sint32 lowIndex = -1;
	sint32 lowMoveCost = 0x7fffffff;

	for(i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) {
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(i);
		if(!rec->GetClassRoad() && !rec->GetClassOceanRoad()) continue;

		const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, pos);
		if(!effect) continue;

		if(!g_player[player]->HasAdvance(effect->GetEnableAdvanceIndex())) continue;
		
		sint32 moveCost;
		if(!effect->GetMoveCost(moveCost))
			continue;

		if(lowMoveCost < moveCost)
			continue;

		lowMoveCost = moveCost;
		lowIndex = i;
	}

	if(lowIndex >= 0) {
		return g_theTerrainImprovementDB->Get(lowIndex);
	} else {
		return NULL;
	}
}

sint32 terrainutil_GetTimeToBuild(const MapPoint &pos, sint32 fromType, sint32 toType)
{
	return 10;
}

const TerrainImprovementRecord::Effect *terrainutil_GetTerrainEffect(const TerrainImprovementRecord *rec,
																	 const MapPoint &pos)
{
	return terrainutil_GetTerrainEffect(rec, (sint32)g_theWorld->GetCell(pos)->GetTerrainType());
}

const TerrainImprovementRecord::Effect *terrainutil_GetTerrainEffect(const TerrainImprovementRecord *rec,
																	 sint32 terrainType)
{
	
	sint32 i, j;

	for(i = 0; i < rec->GetNumTerrainEffect(); i++) {
		const TerrainImprovementRecord::Effect *effect = rec->GetTerrainEffect(i);

		for(j = 0; j < effect->GetNumTerrain(); j++) {
			if(effect->GetTerrainIndex(j) == terrainType) {
				return effect;
			}
		}
	}

	
	const TerrainImprovementRecord::Effect *effect;
	if(rec->GetEffect(effect)) {
		return effect;
	} else {
		return NULL;
	}
}

const TerrainRecord::TransformData *terrainutil_GetTransformData(sint32 terrain, bool add)
{
	const TerrainRecord *rec = g_theTerrainDB->Get(terrain);
	const TerrainRecord::TransformData *td = NULL;
	if(add) {
		rec->GetTransformAdd(td);
	} else {
		rec->GetTransformRemove(td);
	}
	return td;
}

sint32 terrainutil_GetProductionTime(sint32 impType, const MapPoint &pos, sint32 extraData)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(impType);
	Assert(rec);
	if(!rec)
		return -1;

	if(rec->GetClassTerraform()) {
		sint32 toterrain;
		if(!rec->GetTerraformTerrainIndex(toterrain))
			return -1;

		const TerrainRecord::TransformData *tfrom = terrainutil_GetTransformData(g_theWorld->GetCell(pos)->GetTerrain(), false);
		const TerrainRecord::TransformData *tto = terrainutil_GetTransformData(toterrain, true);

		
		if(!tfrom || !tto) 
			return -1;

		return tfrom->GetTime() + tto->GetTime();
	}

	const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, pos);

	if(!effect)
		return -1;

	return effect->GetProductionTime();
}

sint32 terrainutil_GetProductionCost(sint32 impType, const MapPoint &pos, sint32 extraData)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(impType);
	Assert(rec);
	if(!rec)
		return -1;

	if(rec->GetClassTerraform()) {
		sint32 toterrain;
		if(!rec->GetTerraformTerrainIndex(toterrain))
			return -1;

		const TerrainRecord::TransformData *tfrom = terrainutil_GetTransformData(g_theWorld->GetCell(pos)->GetTerrain(), false);
		const TerrainRecord::TransformData *tto = terrainutil_GetTransformData(toterrain, true);

		
		if(!tfrom || !tto) 
			return -1;

		return tfrom->GetMaterials() + tto->GetMaterials();
	}

	const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, pos);

	if(!effect)
		return -1;

	return effect->GetProductionCost();
}



void terrainutil_DoVision(const MapPoint &point)
{
	sint32 type = -1;
	sint32 i;
	Cell *cell = g_theWorld->GetCell(point);
	double myVisionRange = 0;

	
	for(i = 0; i < cell->GetNumDBImprovements(); i++) {
		sint32 t = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *impRec = g_theTerrainImprovementDB->Get(t);
		const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(impRec, cell->GetTerrain());
		sint32 range;
		if(effect && effect->GetVisionRange(range) && range > (sint32)myVisionRange) {
			type = t;
			myVisionRange = (double)range;
		}
	}

	if(type < 0 || myVisionRange < 0.001) {
		
		return;
	}

	sint32 myrsq = sint32((myVisionRange+0.5)*(myVisionRange+0.5));

	
	BOOL revealedUnexplored = FALSE;
	if(cell->GetOwner() >= 0) {
		g_player[cell->GetOwner()]->AddUnitVision(point, myVisionRange, revealedUnexplored);

		if(g_selected_item->GetVisiblePlayer() == cell->GetOwner())
			g_director->AddCopyVision();
	}

	MapPoint topleft = point;
	topleft.x -= sint16(myVisionRange);
	DynamicArray<Unit> unitArray;

	g_theUnitTree->SearchRect(unitArray, topleft,
							  sint32(myVisionRange) * 2 + 1,
							  sint32(myVisionRange) * 2 + 1,
							  ~(1 << cell->GetOwner()));
	sint32 un = unitArray.Num();
	for(i = 0; i < un; i++) {
		UnitData *ud = unitArray[i].AccessData();
		sint32 ls = UnitData::GetDistance(ud, point, sint32(myVisionRange));
		if(ls >= myrsq)
			continue;

		if(ls <= myrsq && !(ud->GetRealVisibility() & (1 << cell->GetOwner()))) {
			if(g_theUnitDB->Get(ud->GetType())->GetVisionClass() &
			   g_theTerrainImprovementDB->Get(type)->GetCanSee()) {
				if(cell->GetOwner() >= 0) {
					ud->SetVisible((PLAYER_INDEX)cell->GetOwner());
				}
			}
		}
	}
}

bool terrainutil_PlayerHasAdvancesForTerrain(const TerrainImprovementRecord *rec, sint32 pl, sint32 terr)
{
	
	Assert(rec != NULL);
	if(rec == NULL)
		return false;

	Assert(pl >= 0);
	Assert(pl < k_MAX_PLAYERS);
	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return false;

	Assert(g_player[pl]);
	if(!g_player[pl])
		return false;

	if(rec->GetClassTerraform()) {
		sint32 t;
		if(rec->GetTerraformTerrainIndex(t)) {
			const TerrainRecord *terrToRec = g_theTerrainDB->Get(t);
			const TerrainRecord *terrFromRec = g_theTerrainDB->Get(terr);

			if(!g_player[pl]->HasAdvance(terrToRec->GetAddAdvanceIndex()) ||
			   !g_player[pl]->HasAdvance(terrFromRec->GetRemoveAdvanceIndex()))
				return false;

			return true;
		}
		return false;
	} else {
		const TerrainImprovementRecord::Effect *eff;
		eff = terrainutil_GetTerrainEffect(rec, terr);
		if(eff) {
			if(g_player[pl]->HasAdvance(eff->GetEnableAdvanceIndex())) {
				sint32 a;
				bool haveObsolete = false;
				for(a = 0; a < eff->GetNumObsoleteAdvance(); a++) {
					if(g_player[pl]->HasAdvance(eff->GetObsoleteAdvanceIndex(a))) {
						haveObsolete = true;
					}
				}				
				
				if(!haveObsolete) {					
					return true;
				}
			}
		}
	}
	return false;
}
bool terrainutil_PlayerHasAdvancesFor(const TerrainImprovementRecord *rec, sint32 pl)
{
	
	sint32 i;
	for(i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		if(terrainutil_PlayerHasAdvancesForTerrain(rec, pl, i)) {
			return true;
		}
	}
	return false;
}

bool terrainutil_CanPlayerBuild(const TerrainImprovementRecord *rec, sint32 pl, bool checkMaterials)
{
	Assert(rec != NULL);
	if(rec == NULL)
		return false;

	Assert(pl >= 0);
	Assert(pl < k_MAX_PLAYERS);
	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return false;

	Assert(g_player[pl]);
	if(!g_player[pl])
		return false;

	if(terrainutil_PlayerHasAdvancesFor(rec, pl)) {
		
		if(!checkMaterials)
			return true;

		
		
		sint32 i;
		for(i = 0; i < g_theTerrainDB->NumRecords(); i++) {
			if(terrainutil_PlayerHasAdvancesForTerrain(rec, pl, i)) {
				if(rec->GetClassTerraform()) {
					sint32 toterrain;
					if(!rec->GetTerraformTerrainIndex(toterrain))
						return false;
					
					const TerrainRecord::TransformData *tfrom = terrainutil_GetTransformData(toterrain, true);
					const TerrainRecord::TransformData *tto = terrainutil_GetTransformData(toterrain, false);

					if(g_player[pl]->m_materialPool->GetMaterials() >= tfrom->GetMaterials() + tto->GetMaterials()) {
						return true;
					}
				} else {
					const TerrainImprovementRecord::Effect *eff;
					eff = terrainutil_GetTerrainEffect(rec, i);
					if(eff) {
						if(g_player[pl]->m_materialPool->GetMaterials() >= eff->GetProductionCost()) {
							return true;
						}
					}
				}
			}
		}
	}
				

	return false;
}

bool terrainutil_CanPlayerBuildAt(const TerrainImprovementRecord *rec, sint32 pl, const MapPoint &pos)
{
	sint32 i;

	Assert(rec != NULL);
	if(rec == NULL)
		return false;

	Assert(pl >= 0);
	Assert(pl < k_MAX_PLAYERS);
	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return false;

	Assert(g_player[pl]);
	if(!g_player[pl])
		return false;

	Cell *cell = g_theWorld->GetCell(pos);
	Assert(cell);
	if(!cell)
		return false;

	if(cell->GetOwner() == -1) {
		if(rec->GetIntBorderRadius()) {
			if(!g_player[pl]->IsVisible(pos)) {
				
				return false;
			}
		} else {
			return false;
		}
	}

	if(cell->GetOwner() >= 0 && cell->GetOwner() != pl)
	{
		bool const haveAlliance	= 
			AgreementMatrix::s_agreements.HasAgreement(pl, cell->GetOwner(), PROPOSAL_TREATY_ALLIANCE);
		if(cell->GetOwner() > 0 && haveAlliance) {
			if(rec->GetClassRoad() ||
				(g_player[pl]->GetGaiaController() && g_player[pl]->GetGaiaController()->GaiaControllerTileImp(rec->GetIndex()))) {
				
				
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	
	if(g_theWorld->GetCity(pos).IsValid())
		return false;
		
	if(rec->GetClassTerraform()) {
		sint32 terr;
		if(!rec->GetTerraformTerrainIndex(terr))
			return false;

		if(cell->GetTerrain() == terr)
			return false;

		const TerrainRecord *tfrom = g_theTerrainDB->Get(cell->GetTerrain());
		const TerrainRecord *tto = g_theTerrainDB->Get(terr);
		if(tfrom->GetRemoveAdvanceIndex() < 0 || tto->GetAddAdvanceIndex() < 0)
			return false;

		if(!g_player[pl]->HasAdvance(tfrom->GetRemoveAdvanceIndex()) ||
			!g_player[pl]->HasAdvance(tto->GetAddAdvanceIndex())) {
			return false;
		}
	} else {
		
		const TerrainImprovementRecord::Effect *eff;
		eff = terrainutil_GetTerrainEffect(rec, cell->GetTerrain());
		if(!eff)
			return false;
		
		if(!g_player[pl]->HasAdvance(eff->GetEnableAdvanceIndex()))
			return false;
		
		sint32 a;
		for(a = 0; a < eff->GetNumObsoleteAdvance(); a++) {
			if(g_player[pl]->HasAdvance(eff->GetObsoleteAdvanceIndex(a))) {
				return false;
			}
		}
		
		
		for(i = 0; i < rec->GetNumCantBuildOn(); i++) {
			if(rec->GetCantBuildOnIndex(i) == cell->GetTerrain()) {
				return FALSE;
			}
		}
	}

	
	
	return true;
}


void terrainutil_GetDefenseBonus(const MapPoint & pos, double & terrain_bonus, double & fort_bonus)
{
	Cell *cell = g_theWorld->GetCell(pos);
	fort_bonus = 0.0;
	terrain_bonus = cell->GetTerrainDefenseBonus();
	double bonus;

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			if(eff) {
				if(eff->GetDefenseBonus(bonus)) {
					
					fort_bonus += bonus;
				}
			}
		}
	}
}

bool terrainutil_HasAirfield(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetAirport())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasListeningPost(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetListeningPost())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasFort(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);
	
	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {
		
		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
		
		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetDefenseBonus())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasRadar(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);
	
	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {
		
		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
		
		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetRadar())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasEndgame(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);
	
	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {
		
		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
		
		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetEndgame())
				return true;
		}
	}
	return false;
}

bool terrainutil_IsInstallation(const sint32 type)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
	
	Assert(rec);
	if(rec) {
		if(rec->GetIntBorderRadius())
			return true;

		
		for(sint32 i = 0; i < rec->GetNumTerrainEffect(); i++) {
			const TerrainImprovementRecord::Effect *effect = 
				rec->GetTerrainEffect(i);
			Assert(effect);
			if(effect) {
				if (effect->GetAirport() ||
					effect->GetDefenseBonus() ||
					effect->GetRadar() ||
					effect->GetListeningPost() ||
					effect->GetEndgame())
					return true;
			}
		}
	}
	return false;
}

double terrainutil_GetMaxVisionRange()
{
	static double max_vision_range = -1.0;
	if (max_vision_range < 0.0)
		{
			
			sint32 range;
			for(sint32 i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) {

				const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(i);

				Assert(rec);
				if(rec) {
					
					for(sint32 j = 0; j < rec->GetNumTerrainEffect(); j++) {
						const TerrainImprovementRecord::Effect *effect = 
							rec->GetTerrainEffect(j);
						
						Assert(effect);
						if(effect && effect->GetVisionRange())
						{
							effect->GetVisionRange(range);
							if (range > max_vision_range)
								max_vision_range = range;
						}
					}
				}
			}
	}
	return max_vision_range;
}

double terrainutil_GetVisionRange(const sint32 terrainType, const MapPoint &pos)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(terrainType);

	sint32 range = 0;
	Assert(rec);
	if(rec) {
		
		const TerrainImprovementRecord::Effect *eff = 
			terrainutil_GetTerrainEffect(rec, pos);
		Assert(eff);
		if(eff && eff->GetVisionRange())
			{
				eff->GetVisionRange(range);
			}
	}
	return range;
}

bool terrainutil_AdvanceEnablesImprovementForPlayer(sint32 player, sint32 advance, sint32 imp)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
	if(!rec) return false;
	if(player < 0 || player >= k_MAX_PLAYERS || !g_player[player]) return false;
	
	if(advance < 0 || advance >= g_theAdvanceDB->NumRecords()) return false;

	bool enables = false;
	sint32 i;
	for(i = 0; i < rec->GetNumTerrainEffect(); i++) {
		const TerrainImprovementRecord::Effect *effect = rec->GetTerrainEffect(i);
		Assert(effect);
		if(effect) {
			if(g_player[player]->HasAdvance(effect->GetEnableAdvanceIndex())) {
				
				
				return false;
			} else if(advance == effect->GetEnableAdvanceIndex()) {
				enables = true;
			}			
		}
	}

	return enables;
}

bool terrainutil_AdvanceEnablesImprovement(sint32 advance, sint32 imp)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
	if(!rec) return false;
	
	if(advance < 0 || advance >= g_theAdvanceDB->NumRecords()) return false;

	bool enables = false;
	sint32 i;
	for(i = 0; i < rec->GetNumTerrainEffect(); i++) {
		const TerrainImprovementRecord::Effect *effect = rec->GetTerrainEffect(i);
		Assert(effect);
		if(effect) {
			if(advance == effect->GetEnableAdvanceIndex()) {
				enables = true;
			}			
		}
	}

	return enables;
}

bool terrainutil_GetSomethingOwnsCell(MapPoint &pos, sint32 owner, Unit &ignoreCity)
{
	
	sint32 i;
	for(i = 0; i < g_player[owner]->m_all_cities->Num(); i++) {
		Unit city = g_player[owner]->m_all_cities->Access(i);
		if(ignoreCity.m_id != 0 && city.m_id == ignoreCity.m_id) continue;
		
		if(MapPoint::GetSquaredDistance(pos, city.RetPos()) <= g_theConstDB->GetBorderSquaredRadius()) {
			return true;
		}
	}
	
	
	for(i = 0; i < g_player[owner]->m_allInstallations->Num(); i++) {
		Installation inst = g_player[owner]->m_allInstallations->Access(i);
		const TerrainImprovementRecord *rec = inst.GetDBRec();
		sint32 sqRad;
		if(!rec->GetSquaredBorderRadius(sqRad))
			continue;
		
		if(MapPoint::GetSquaredDistance(pos, inst.RetPos()) <= sqRad) {
			return true;
		}
	}

	return false;
}

void terrainutil_RemoveBorders(MapPoint &center, sint32 owner, sint32 intRad, sint32 sqRad, Unit &ignoreCity)
{
	
	

	if(!g_player[owner]) return;

	RadiusIterator it(center, intRad, sqRad);

	for(it.Start(); !it.End(); it.Next()) {
		Cell *cell = g_theWorld->GetCell(it.Pos());
		if(cell->GetOwner() != owner)
			continue;

		if(cell->GetCityOwner().IsValid() &&
		   cell->GetCityOwner().m_id != ignoreCity.m_id) {
			
			continue;
		}

		
		bool stillOwned = terrainutil_GetSomethingOwnsCell(it.Pos(), owner, ignoreCity);
		if(!stillOwned) {
			
			cell->SetOwner(-1);
		}

		g_network.Block(owner);
		g_network.Enqueue(cell, it.Pos().x, it.Pos().y);
		g_network.Unblock(owner);

	}
	g_tiledMap->Refresh();
}


#endif 

sint32 terrainutil_GetForest()
{
	return s_TERRAIN_FOREST;
}

sint32 terrainutil_GetPlains()
{
	return s_TERRAIN_PLAINS;
}

sint32 terrainutil_GetTundra()
{
	return s_TERRAIN_TUNDRA;
}

sint32 terrainutil_GetGlacier()
{
	return s_TERRAIN_GLACIER;
}

sint32 terrainutil_GetGrassland()
{
	return s_TERRAIN_GRASSLAND;
}

sint32 terrainutil_GetDesert()
{
	return s_TERRAIN_DESERT;
}

sint32 terrainutil_GetSwamp()
{
	return s_TERRAIN_SWAMP;
}

sint32 terrainutil_GetJungle()
{
	return s_TERRAIN_JUNGLE;
}

sint32 terrainutil_GetMountain()
{
	return s_TERRAIN_MOUNTAIN;
}

sint32 terrainutil_GetHill()
{
	return s_TERRAIN_HILL;
}

sint32 terrainutil_GetWaterShallow()
{
	return s_TERRAIN_WATER_SHALLOW;
}

sint32 terrainutil_GetWaterDeep()
{
	return s_TERRAIN_WATER_DEEP;
}

sint32 terrainutil_GetWaterVolcano()
{
	return s_TERRAIN_WATER_VOLCANO;
}

sint32 terrainutil_GetWaterBeach()
{
	return s_TERRAIN_WATER_BEACH;
}

sint32 terrainutil_GetWaterShelf()
{
	return s_TERRAIN_WATER_SHELF;
}

sint32 terrainutil_GetWaterTrench()
{
	return s_TERRAIN_WATER_TRENCH;
}

sint32 terrainutil_GetWaterRift()
{
	return s_TERRAIN_WATER_RIFT;
}

sint32 terrainutil_GetDead()
{
	return s_TERRAIN_DEAD;
}

sint32 terrainutil_GetBrownHill()
{
	return s_TERRAIN_BROWN_HILL;
}

sint32 terrainutil_GetBrownMountain()
{
	return s_TERRAIN_BROWN_MOUNTAIN;
}

sint32 terrainutil_GetWhiteHill()
{
	return s_TERRAIN_WHITE_HILL;
}

sint32 terrainutil_GetWhiteMountain()
{
	return s_TERRAIN_WHITE_MOUNTAIN;
}

sint32 terrainutil_GetWaterKelp()
{
	return s_TERRAIN_WATER_KELP;
}

sint32 terrainutil_GetWaterReef()
{
	return s_TERRAIN_WATER_REEF;
}


sint32 terrainutil_GetTerraformHillsImprovement()
{
	return s_TERRAFORM_HILLS_IMPROVEMENT;
}

sint32 terrainutil_GetTerraformPlainsImprovement()
{
	return s_TERRAFORM_PLAINS_IMPROVEMENT;
}

sint32 terrainutil_GetTerraformForestImprovement()
{
	return s_TERRAFORM_FORESTS_IMPROVEMENT;
}

sint32 terrainutil_GetTerraformGrasslandImprovement()
{
	return s_TERRAFORM_GRASSLAND_IMPROVEMENT;
}

sint32 terrainutil_GetEndgameTileImpIndex()
{
	const TerrainImprovementRecord *me; 
	for(sint32 i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) {
		me = g_theTerrainImprovementDB->Get(i);
		if(me->GetEffectPtr()->GetEndgame())
			return i;
	}
	return i;
}
			
