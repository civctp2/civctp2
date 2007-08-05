//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Terrain utilities
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
// __TILETOOL__
// - Probably supposed to generate the tool for creating the *.til files.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected non-standard syntax.
// - Added a check in CanPlayerBuild for the CultureOnly flag to see if a   
//   tile improvement is limited to certain CityStyles. - (E 2005/03/12)
// - Added a check in terrainutil_CanPlayerBuild to check if a tile improvement
//   is restricted to certain types of governments and if the player has 
//   that government - (E 2005/03/12)
// - Added a check in terrainutil_CanPlayerBuildAt for the IsRestrictedToGood 
//   flag so a tile improvement can only be built on a tile with a 
//   certain good on it - (E 2005/03/12)
// - Removed .NET warnings - May 7th 2005 Martin Gühmann
// - terrainutil_CanPlayerSpecialBuildAt added by E 4-1-2006
// - Added outcommented terrainutil_HasUpgrader, terrainutil_CanBeCaptured, 
//   terrainutil_HasColony by E (4-25-2006) fo future use
// - implemented above and added HasMinefield by E 5-30-2006
// - Made government modified for units work here. (July 29th 2006 Martin Gühmann)
// - Added CanBuildAlly and CanBuildWasteland checks
// - Added outcommented infrastructure flags
// - Added IsWonder Check to specialbuildat
// - Added HasIrrigation method
// - Added HasWonder method
// - FINALLY got contiguous irrigation to work 4.12.2007
// - City Radius tileimps
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
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
#include "player.h"
#include "UnitDynArr.h"
#include "QuadTree.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "director.h"
#include "SelItem.h"
#include "MaterialPool.h"
#include "gaiacontroller.h"
#include "AgreementMatrix.h"
#include "ConstRecord.h"
#include "installation.h"
#include "CityInfluenceIterator.h"
#include "tiledmap.h"
#include "AdvanceRecord.h"
#include "network.h"
#include "Civilisation.h"
#include "citydata.h"  //for wonder tileimps located in city radius
#include "installation.h"
#include "installationpool.h"
#include "installationtree.h"           // g_theInstallationTree
#include "UnitData.h"
#include "UnitPool.h"
#include "UnitRecord.h"

extern QuadTree<Unit> *g_theUnitTree;
#endif

namespace
{

TERRAIN_TYPE    s_TERRAIN_FOREST;
TERRAIN_TYPE    s_TERRAIN_PLAINS;
TERRAIN_TYPE    s_TERRAIN_TUNDRA;
TERRAIN_TYPE    s_TERRAIN_GLACIER;
TERRAIN_TYPE    s_TERRAIN_GRASSLAND;
TERRAIN_TYPE    s_TERRAIN_DESERT;
TERRAIN_TYPE    s_TERRAIN_SWAMP;
TERRAIN_TYPE    s_TERRAIN_JUNGLE;
TERRAIN_TYPE    s_TERRAIN_MOUNTAIN;
TERRAIN_TYPE    s_TERRAIN_HILL;
TERRAIN_TYPE    s_TERRAIN_WATER_SHALLOW;
TERRAIN_TYPE    s_TERRAIN_WATER_DEEP;
TERRAIN_TYPE    s_TERRAIN_WATER_VOLCANO;
TERRAIN_TYPE    s_TERRAIN_WATER_BEACH;
TERRAIN_TYPE    s_TERRAIN_WATER_SHELF;
TERRAIN_TYPE    s_TERRAIN_WATER_TRENCH;
TERRAIN_TYPE    s_TERRAIN_WATER_RIFT;
TERRAIN_TYPE    s_TERRAIN_DEAD;
TERRAIN_TYPE    s_TERRAIN_BROWN_HILL;
TERRAIN_TYPE    s_TERRAIN_BROWN_MOUNTAIN;
TERRAIN_TYPE    s_TERRAIN_WHITE_HILL;
TERRAIN_TYPE    s_TERRAIN_WHITE_MOUNTAIN;
TERRAIN_TYPE    s_TERRAIN_WATER_KELP;
TERRAIN_TYPE    s_TERRAIN_WATER_REEF;
sint32 const    TERRAIN_TYPE_COUNT_MAX  = 127;  // to fit in sint8

sint32          s_TERRAFORM_HILLS_IMPROVEMENT;
sint32          s_TERRAFORM_GRASSLAND_IMPROVEMENT;
sint32          s_TERRAFORM_PLAINS_IMPROVEMENT;
sint32          s_TERRAFORM_FORESTS_IMPROVEMENT;

} // namespace

void terrainutil_Initialize()
{
    Assert(g_theTerrainDB->NumRecords() <= TERRAIN_TYPE_COUNT_MAX);

	sint32 i;
	for(i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		const TerrainRecord *rec = g_theTerrainDB->Get(i);

		if(rec->GetInternalTypeForest()) {
			s_TERRAIN_FOREST = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypePlains()) {
			s_TERRAIN_PLAINS = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeTundra()) {
			s_TERRAIN_TUNDRA = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeGlacier()) {
			s_TERRAIN_GLACIER = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeGrassland()) {
			s_TERRAIN_GRASSLAND = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeDesert()) {
			s_TERRAIN_DESERT = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeSwamp()) {
			s_TERRAIN_SWAMP = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeJungle()) {
			s_TERRAIN_JUNGLE = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeMountain()) {
			s_TERRAIN_MOUNTAIN = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeHill()) {
			s_TERRAIN_HILL = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterShallow()) {
			s_TERRAIN_WATER_SHALLOW = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterDeep()) {
			s_TERRAIN_WATER_DEEP = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterVolcano()) {
			s_TERRAIN_WATER_VOLCANO = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterBeach()) {
			s_TERRAIN_WATER_BEACH = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterShelf()) {
			s_TERRAIN_WATER_SHELF = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterTrench()) {
			s_TERRAIN_WATER_TRENCH = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterRift()) {
			s_TERRAIN_WATER_RIFT = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeDead()) {
			s_TERRAIN_DEAD = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeBrownHill()) {
			s_TERRAIN_BROWN_HILL = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeBrownMountain()) {
			s_TERRAIN_BROWN_MOUNTAIN = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWhiteHill()) {
			s_TERRAIN_WHITE_HILL = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWhiteMountain()) {
			s_TERRAIN_WHITE_MOUNTAIN = static_cast<TERRAIN_TYPE>(i);
		}		
		if(rec->GetInternalTypeWaterKelp()) {
			s_TERRAIN_WATER_KELP = static_cast<TERRAIN_TYPE>(i);
		}
		if(rec->GetInternalTypeWaterReef()) {
			s_TERRAIN_WATER_REEF = static_cast<TERRAIN_TYPE>(i);
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
	for (sint32 i = 0; i < rec->GetNumTerrainEffect(); ++i) 
    {
		const TerrainImprovementRecord::Effect * effect = rec->GetTerrainEffect(i);

        if (effect)
        {
		    for (sint32 j = 0; j < effect->GetNumTerrain(); ++j) 
            {
			    if (effect->GetTerrainIndex(j) == terrainType) 
                {
				    return effect;
                }
			}
		}
	}

	return rec->GetEffectPtr();
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

sint32 terrainutil_GetBonusProductionExport(sint32 impType, const MapPoint &pos, sint32 extraData) //EMOD
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(impType);
	Assert(rec);
	if(!rec)
		return -1;

	const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, pos);

	if(!effect)
		return -1;

	sint32 value;
	effect->GetBonusProductionExport(value);
	return value;
}

void terrainutil_DoVision(const MapPoint &point)
{
	Cell *  cell            = g_theWorld->GetCell(point);
	sint32  maxVisionRange  = 0;
	sint32  type            = -1;

	for (sint32 impr = 0; impr < cell->GetNumDBImprovements(); ++impr)
    {
		sint32      t       = cell->GetDBImprovement(impr);
		TerrainImprovementRecord::Effect const *    
                    effect  = terrainutil_GetTerrainEffect
                                (g_theTerrainImprovementDB->Get(t), 
                                 cell->GetTerrain()
                                );
		sint32 range;
		if (effect && effect->GetVisionRange(range) && (range > maxVisionRange)) 
        {
			type            = t;
			maxVisionRange  = range;
		}
	}

	if (type < 0 || maxVisionRange <= 0) 
    {
		return;
	}

	bool    revealedUnexplored  = false;
    sint32  cellOwner           = cell->GetOwner();
	if (cellOwner >= 0) 
    {
		g_player[cellOwner]->AddUnitVision(point, maxVisionRange, revealedUnexplored);

		if (g_selected_item->GetVisiblePlayer() == cellOwner)
        {
			g_director->AddCopyVision();
        }
	}

	MapPoint topleft = MapPoint(point.x - maxVisionRange, point.y);
	DynamicArray<Unit> unitArray;

	g_theUnitTree->SearchRect(unitArray, topleft,
	                          static_cast<sint16>(maxVisionRange) * 2 + 1,
	                          static_cast<sint16>(maxVisionRange) * 2 + 1,
	                          ~(1 << cellOwner)
                             );

	sint32 myrsq    = sint32((maxVisionRange+0.5) * (maxVisionRange+0.5));
	sint32 un       = unitArray.Num();

	for (sint32 i = 0; i < un; i++) 
    {
		UnitData * ud = unitArray[i].AccessData();
		sint32 ls = UnitData::GetDistance(ud, point, maxVisionRange);
		if(ls >= myrsq)
			continue;

		if (!(ud->GetRealVisibility() & (1 << cellOwner))) 
        {
			if (ud->GetDBRec()->GetVisionClass() &
			    g_theTerrainImprovementDB->Get(type)->GetCanSee()
               ) 
            {
				if (cellOwner >= 0) 
                {
					ud->SetVisible((PLAYER_INDEX)cellOwner);
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
						haveObsolete = true;  //emod is this source of defect
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

//----------------------------------------------------------------------------
//
// Name       : terrainutil_CanPlayerBuild
//
// Description: Checks whether the city can build the improvement 
//             
// Parameters : checkMaterials   : Checks to see if player has materials  
//                                 to build it.
//              sint32 pl        : index of the player that is checked 
//                                 for whether the terrain improvement 
//                                 in question can be built
//
// Globals    : g_player:      The list of players
//
// Returns    : Whether the imp is available for a player to build.
//
// Remark(s)  : GovernmentType flag for improvements limits imps to govt type.
//              CultureOnly flag added by E. It allows only civilizations with 
//              the same CityStyle as CultureOnly's style to build that imp.
//
//----------------------------------------------------------------------------
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

// Added by E - Compares Improvement's GovernmentType to the Player's Government
	if(rec->GetNumGovernmentType() > 0) {
		sint32 i;
		bool found = false;
		for(i = 0; i < rec->GetNumGovernmentType(); i++) {
			if(rec->GetGovernmentTypeIndex(i) == g_player[pl]->GetGovernmentType()) {
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

// Added by E - Compares Improvement's CultureOnly to the Player's CityStyle
	if(rec->GetNumCultureOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCultureOnly(); s++) {
			if(rec->GetCultureOnlyIndex(s) == g_player[pl]->GetCivilisation()->GetCityStyle()) {
				found = true;
				break;
			}
		}
		if(!found)
			return false;
//	} else {
//  See Use of ELSE below for how effect is implemented 
//	Added by E - Compares Improvement's CultureOnly to the Player's CityStyle for terrain effects
//		for(sint32 b = 0; b < g_theTerrainDB->NumRecords(); b++) {
//		const TerrainImprovementRecord::Effect *eff;
//		sint32 t;
//		bool found = false;
//		eff = terrainutil_GetTerrainEffect(rec, b);
//			if(eff) {
			//	if(eff->GetNumCultureOnly() > 0) {
//					for(t = 0; t < eff->GetNumCultureOnly(); t++) {
//						if(eff->GetCultureOnlyIndex(t) == g_player[pl]->GetCivilisation()->GetCityStyle()) {
//							found = true;
//							break;
//						}
//					}
//			//	}
//			}
//		if(!found)
//			return false;
//		}		
	}






	
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

//----------------------------------------------------------------------------
//
// Name       :  terrainutil_CanPlayerBuildAt
//
// Description:  Checks terrain improvement properties to see if the player 
//               can build it on a tile 
//
// Parameters :  sint32 pl              : index of the player that is checked 
//                                        for whether the terrain improvement 
//                                        in question can be built
//               const MapPoint &pos    : Variable for tile on map
//               const TerrainImprovementRecord *rec    
//                                      : terrain improvement record in the 
//                                        terrain improvement database.
// Globals    :   g_theWorld            : The game world properties
//                g_player              : The list of players 
//
// Returns    :   bool                  : Returns true if an improvement 
//                                        can be built on a tile
//                                        false if the improvement cannot 
//
// Remark(s)  :   A new improvement attribute IsRestrictedToGood was 
//                added by E. Modders will define this in tileimp.txt as 
//                IsRestrictedToGood: X. The flag adds an additional option 
//                in order to restrict ceratin improvements to goods, adding 
//                new options and bonuses.
//
//----------------------------------------------------------------------------
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
		if(rec->HasIntBorderRadius()) {  //Add has unit IsWorker?
			if(!g_player[pl]->IsVisible(pos)) {
				
				return false;
			}
		} else if(rec->GetCanBuildWasteland()) {  //Allows palyer to connect cities
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
			if(rec->GetClassRoad() ||               //Why only build roads in allied territory?
				(g_player[pl]->GetGaiaController() && g_player[pl]->GetGaiaController()->GaiaControllerTileImp(rec->GetIndex()))
				|| rec->GetCanBuildAlly()	//added for other ally improving
				){
				
				
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

		//EMOD Improvement can only be built on a tile with a certain good on it
		if(rec->GetNumIsRestrictedToGood() <= 0) {
			for(i = 0; i < rec->GetNumCantBuildOn(); i++) {
				if(rec->GetCantBuildOnIndex(i) == cell->GetTerrain()) {
					return false;
				}
			}
		}
		else {
			sint32 good;
			if (g_theWorld->GetGood(pos, good)) {
				bool hasCorrectGood = false;
				for(i = 0; i < rec->GetNumIsRestrictedToGood(); i++) {
					if(rec->GetIsRestrictedToGoodIndex(i) == good) {
						hasCorrectGood = true;
						break;
					}
				}
				if(!hasCorrectGood)
				return false;
			}
		}

		// EMOD for river only like dams and mills 4.30.2007
		if(eff->GetRiverOnly()) {
			bool canbuild = false;
			if(g_theWorld->IsRiver(pos)){

					canbuild = true;
					//	break;
			}

			if(!canbuild)
					return false;
		}

		// EMOD for contiguous city tiles 4.30.2007
		if(eff->GetNextToUrban()) {
			CityInfluenceIterator it(pos, 1);
			bool canbuild = false;
			for(it.Start(); !it.End(); it.Next()) {
				if( (g_theWorld->HasCity(it.Pos())) || (terrainutil_HasUrban(it.Pos())) ){
					canbuild = true;
						break;
				}

			}
				if(!canbuild)
					return false;
		}

		// EMOD for contiguous city tiles 4.30.2007
		if(eff->GetNextToCity()) {
			CityInfluenceIterator it(pos, 1);
			bool canbuild = false;
			for(it.Start(); !it.End(); it.Next()) {
				if( g_theWorld->HasCity(it.Pos()) ){
					canbuild = true;
						break;
				}

			}
				if(!canbuild)
					return false;
		}

		// EMOD for contiguous irrigation //finally works 4.12.2007
		if(eff->GetNeedsIrrigation()) {
			CityInfluenceIterator it(pos, 1);
			bool canbuild = false;
			for(it.Start(); !it.End(); it.Next()) {
				if( (g_theWorld->IsRiver(it.Pos())) || (terrainutil_HasIrrigation(it.Pos())) ){
					canbuild = true;
						break;
				}

			}
			if(!canbuild)
				return false;
		}

		// Emod - the tileimp can only be built in the radius of the city with the wonder
		// need to add a start building check to prevent multiple instances
		if(eff->GetNumIsWonder() > 0)
		{
			for(sint32 won = 0; won < eff->GetNumIsWonder(); won++)
			{
				if(eff->GetIsWonderIndex(won))
				{
					Unit cellcity = cell->GetCityOwner();
					CityData *wondercity = cellcity.GetData()->GetCityData();
					if (cellcity.GetOwner() != pl)
						return false;

					for (sint32 w =0; w < wondercity->GetBuiltWonders(); w++)
					{
						if (w != won)
							return false;
					}
				}
			} //end isbuilding loop

		//end wonder check
		}

		// for PrerequisiteTileImp
		if(rec->GetNumPrerequisiteTileImp() > 0) {
			bool hasCorrectImp = false;
			for(i = 0; i < rec->GetNumPrerequisiteTileImp(); i++) {
				if(rec->GetPrerequisiteTileImpIndex(i) == cell->GetDBImprovement(i)) {
						hasCorrectImp = true;
						break;
				}
			}
			if(!hasCorrectImp)
				return false;
		}

	// End EMOD
	}
	return true;
}

bool terrainutil_CanPlayerSpecialBuildAt(const TerrainImprovementRecord *rec, sint32 pl, const MapPoint &pos)
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
	
	const TerrainImprovementRecord::Effect *eff;
	eff = terrainutil_GetTerrainEffect(rec, cell->GetTerrain());
		if(!eff)
			return false;

//	if(cell->GetOwner() == -1) {
//		if(rec->GetIntBorderRadius()) {
//			if(!g_player[pl]->IsVisible(pos)) {
				
//				return false;
//			}
//		} else {
//			return false;
//		}
//	}

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


		if(rec->GetNumIsRestrictedToGood () == 0) {
			for(i = 0; i < rec->GetNumCantBuildOn(); i++) {
				if(rec->GetCantBuildOnIndex(i) == cell->GetTerrain()) {
					return false;
				}
			}
		}
		else {
			sint32 good;
			if (g_theWorld->GetGood(pos, good)) {
				for(i = 0; i < rec->GetNumIsRestrictedToGood(); i++) {
					if(rec->GetIsRestrictedToGoodIndex(i) == good) {
						return true; 
					}
				}
				return false;
			}
		}
	if(eff->GetNumIsWonder() > 0) {  //added for show on map code
		if(terrainutil_HasWonder(pos)) {
			return false;
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

bool terrainutil_HasUpgrader(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetCanUpgrade())
				return true;
		}
	}
	return false;
}

bool terrainutil_CanBeCaptured(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetCanBeCaptured())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasColony(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetColony())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasMinefield(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->HasMinefield())
				return true;
		}
	}
	return false;
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
			
			if((eff && eff->HasDefenseBonus()) || (eff && eff->GetFort()))  //EMOD to have just a fort flag
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
		if(rec->HasIntBorderRadius())
			return true;

		
		for(sint32 i = 0; i < rec->GetNumTerrainEffect(); i++) {
			const TerrainImprovementRecord::Effect *effect = 
				rec->GetTerrainEffect(i);
			Assert(effect);
			if(effect) {
				if (effect->GetAirport() ||
					effect->HasDefenseBonus() ||
					effect->GetRadar() ||
					effect->GetColony() ||  //EMOD
					//effect->GetFort()
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
						if (effect && effect->GetVisionRange(range))
						{
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
	if (rec) 
	{
		const TerrainImprovementRecord::Effect *eff = 
			terrainutil_GetTerrainEffect(rec, pos);
		Assert(eff);
		if (eff)
		{
			(void) eff->GetVisionRange(range);
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
	for (sint32 i = 0; i < rec->GetNumTerrainEffect(); i++) {
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

	for (sint32 i = 0; i < rec->GetNumTerrainEffect(); i++) 
    {
		const TerrainImprovementRecord::Effect *effect = rec->GetTerrainEffect(i);
		Assert(effect);
		if (effect && (advance == effect->GetEnableAdvanceIndex())) 
        {
			return true;
		}
	}

	return false;
}

bool terrainutil_GetSomethingOwnsCell(MapPoint const & pos, sint32 owner, Unit ignoreCity)
{
	
	sint32 i;
	for(i = 0; i < g_player[owner]->m_all_cities->Num(); i++) {
		Unit city = g_player[owner]->m_all_cities->Access(i);
		if(ignoreCity.m_id != 0 && city.m_id == ignoreCity.m_id) continue;
		
		if(MapPoint::GetSquaredDistance(pos, city.RetPos()) <= g_theConstDB->Get(0)->GetBorderSquaredRadius()) {
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

void terrainutil_RemoveBorders(const MapPoint &center, sint32 owner, sint32 intRad, sint32 sqRad, Unit ignoreCity)
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
		if (!stillOwned) {
			
			cell->SetOwner(PLAYER_UNASSIGNED);
		}

		g_network.Block(owner);
		g_network.Enqueue(cell, it.Pos().x, it.Pos().y);
		g_network.Unblock(owner);

	}
	g_tiledMap->Refresh();
}


#endif 

TERRAIN_TYPE terrainutil_GetForest()
{
	return s_TERRAIN_FOREST;
}

TERRAIN_TYPE terrainutil_GetPlains()
{
	return s_TERRAIN_PLAINS;
}

TERRAIN_TYPE terrainutil_GetTundra()
{
	return s_TERRAIN_TUNDRA;
}

TERRAIN_TYPE terrainutil_GetGlacier()
{
	return s_TERRAIN_GLACIER;
}

TERRAIN_TYPE terrainutil_GetGrassland()
{
	return s_TERRAIN_GRASSLAND;
}

TERRAIN_TYPE terrainutil_GetDesert()
{
	return s_TERRAIN_DESERT;
}

TERRAIN_TYPE terrainutil_GetSwamp()
{
	return s_TERRAIN_SWAMP;
}

TERRAIN_TYPE terrainutil_GetJungle()
{
	return s_TERRAIN_JUNGLE;
}

TERRAIN_TYPE terrainutil_GetMountain()
{
	return s_TERRAIN_MOUNTAIN;
}

TERRAIN_TYPE terrainutil_GetHill()
{
	return s_TERRAIN_HILL;
}

TERRAIN_TYPE terrainutil_GetWaterShallow()
{
	return s_TERRAIN_WATER_SHALLOW;
}

TERRAIN_TYPE terrainutil_GetWaterDeep()
{
	return s_TERRAIN_WATER_DEEP;
}

TERRAIN_TYPE terrainutil_GetWaterVolcano()
{
	return s_TERRAIN_WATER_VOLCANO;
}

TERRAIN_TYPE terrainutil_GetWaterBeach()
{
	return s_TERRAIN_WATER_BEACH;
}

TERRAIN_TYPE terrainutil_GetWaterShelf()
{
	return s_TERRAIN_WATER_SHELF;
}

TERRAIN_TYPE terrainutil_GetWaterTrench()
{
	return s_TERRAIN_WATER_TRENCH;
}

TERRAIN_TYPE terrainutil_GetWaterRift()
{
	return s_TERRAIN_WATER_RIFT;
}

TERRAIN_TYPE terrainutil_GetDead()
{
	return s_TERRAIN_DEAD;
}

TERRAIN_TYPE terrainutil_GetBrownHill()
{
	return s_TERRAIN_BROWN_HILL;
}

TERRAIN_TYPE terrainutil_GetBrownMountain()
{
	return s_TERRAIN_BROWN_MOUNTAIN;
}

TERRAIN_TYPE terrainutil_GetWhiteHill()
{
	return s_TERRAIN_WHITE_HILL;
}

TERRAIN_TYPE terrainutil_GetWhiteMountain()
{
	return s_TERRAIN_WHITE_MOUNTAIN;
}

TERRAIN_TYPE terrainutil_GetWaterKelp()
{
	return s_TERRAIN_WATER_KELP;
}

TERRAIN_TYPE terrainutil_GetWaterReef()
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
	for (int i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) 
    {
		if (g_theTerrainImprovementDB->Get(i)->GetEffectPtr()->GetEndgame())
			return i;
	}

    return CTPRecord::INDEX_INVALID;
}
			
bool terrainutil_HasIrrigation(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetIsIrrigation())
				return true;
		}
	}
	return false;
}

bool terrainutil_HasUrban(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetIsUrban())
				return true;
		}
	}
	return false;
}


bool terrainutil_HasWonder(const MapPoint & pos)
{
	Cell *cell = g_theWorld->GetCell(pos);

	
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			
			if(eff && eff->GetNumIsWonder() > 0)
				return true;
		}
	}
	return false;
}