










 
#include "c3.h"
#include "aui.h"
#include "aui_directsurface.h"

#include "pixelutils.h"

#include "MapPoint.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TerrImproveData.h"

#include "tileutils.h"
#include "TileSet.h"
#include "TiledMap.h"

#include "TileDrawRoad.h"

#include "directions.h"

extern World *g_theWorld;

void TiledMap::DrawARoadPiece(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, BOOL fog,
							  sint32 flags)
{
	if (fog) {
		if (m_zoomLevel != k_ZOOM_NORMAL) {
			DrawBlendedOverlayScaled(surface, data, x, y, GetZoomTilePixelWidth(), 
				GetZoomTileGridHeight(), k_FOW_COLOR, k_FOW_BLEND_VALUE, flags);
		} else {
			DrawBlendedOverlay(surface, data, x, y, k_FOW_COLOR, k_FOW_BLEND_VALUE, flags);
		}
	} else {
		if (m_zoomLevel != k_ZOOM_NORMAL) {
			DrawScaledOverlay(surface, data, x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight(), flags);
		} else {
			DrawOverlay(surface, data, x, y, flags);
		}
	}
}







void TiledMap::DrawRoads(
	aui_Surface *surface,
	MapPoint &pos,		
	sint32 x,			
	sint32 y,			
	sint32 roadType,	
	uint16 roadOffset,	
	BOOL fog,			
	sint32 flags)
{
	MapPoint	newPos;
	sint32		isConnectStraight = 0, isConnectDiagonal = 0;
	sint32		neighborFlag = 0;
	Pixel16		*data;

	
	if ( g_theWorld->IsCity(pos) ) {
		DrawCityRoads( surface, pos, x, y, roadType, roadOffset, fog );
		return;
	}

	
	if(pos.GetNeighborPosition(NORTH,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_NORTH;
			isConnectStraight++;
		}
	}
	if(pos.GetNeighborPosition(NORTHEAST,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_NORTHEAST;
			isConnectDiagonal++;
		}
	}
	if(pos.GetNeighborPosition(EAST,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_EAST;
			isConnectStraight++;
		}
	}
	if(pos.GetNeighborPosition(SOUTHEAST,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_SOUTHEAST;
			isConnectDiagonal++;
		}
	}
	if(pos.GetNeighborPosition(SOUTH,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_SOUTH;
			isConnectStraight++;
		}
	}
	if(pos.GetNeighborPosition(SOUTHWEST,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_SOUTHWEST;
			isConnectDiagonal++;
		}
	}
	if(pos.GetNeighborPosition(WEST,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_WEST;
			isConnectStraight++;
		}
	}
	if(pos.GetNeighborPosition(NORTHWEST,newPos)) {
		if (g_theWorld->IsAnyRoad(newPos) ||
			g_theWorld->IsTunnel(newPos) ||
			g_theWorld->IsCity(newPos))
		{
			neighborFlag |= k_BIT_NORTHWEST;
			isConnectDiagonal++;
		}
	}

	
	if (roadType == k_ROAD_TYPE_3 || roadType == k_ROAD_TYPE_4) {
		data = m_tileSet->GetRoadData(ROAD_MAGLEVPOST+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);
















	}

	if (!isConnectStraight && !isConnectDiagonal)
	{
		data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);
















		return;
	}

	
	if (isConnectStraight == k_X_INTERSECT) {
		data = m_tileSet->GetRoadData(ROAD_X_STRAIGHT+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);
















	}
	else if (isConnectStraight == k_T_INTERSECT)	
	{
		if ((neighborFlag & k_MASK_8) == k_MASK_8) {
			data = m_tileSet->GetRoadData(ROAD_T_SOUTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		else if ((neighborFlag & k_MASK_10) == k_MASK_10) {
			data = m_tileSet->GetRoadData(ROAD_T_WEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		else if ((neighborFlag & k_MASK_12) == k_MASK_12) {
			data = m_tileSet->GetRoadData(ROAD_T_NORTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		} else {
			data = m_tileSet->GetRoadData(ROAD_T_EAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
	}
	else if (isConnectStraight)	
	{
		if (neighborFlag & k_BIT_SOUTH) {
			data = m_tileSet->GetRoadData(ROAD_SOUTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		if (neighborFlag & k_BIT_WEST) {
			data = m_tileSet->GetRoadData(ROAD_WEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		if (neighborFlag & k_BIT_NORTH) {
			data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		if (neighborFlag & k_BIT_EAST) {
			data = m_tileSet->GetRoadData(ROAD_EAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
	}

	
	if (isConnectDiagonal == k_X_INTERSECT) {
		data = m_tileSet->GetRoadData(ROAD_X_DIAGONAL+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);
















	}
	else if (isConnectDiagonal == k_T_INTERSECT)	
	{
		if ((neighborFlag & k_MASK_9) == k_MASK_9) {
			data = m_tileSet->GetRoadData(ROAD_T_SOUTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		else if ((neighborFlag & k_MASK_11) == k_MASK_11) {
			data = m_tileSet->GetRoadData(ROAD_T_NORTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		else if ((neighborFlag & k_MASK_13) == k_MASK_13) {
			data = m_tileSet->GetRoadData(ROAD_T_NORTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		else {
			data = m_tileSet->GetRoadData(ROAD_T_SOUTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
	}
	else if (isConnectDiagonal)	
	{
		if (neighborFlag & k_BIT_SOUTHWEST) {
			data = m_tileSet->GetRoadData(ROAD_SOUTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		if (neighborFlag & k_BIT_NORTHWEST) {
			data = m_tileSet->GetRoadData(ROAD_NORTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		if (neighborFlag & k_BIT_NORTHEAST) {
			data = m_tileSet->GetRoadData(ROAD_NORTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
		if (neighborFlag & k_BIT_SOUTHEAST) {
			data = m_tileSet->GetRoadData(ROAD_SOUTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
















		}
	}
}








void TiledMap::DrawCityRoads(
	aui_Surface *surface,
	MapPoint &pos,		
	sint32 x,			
	sint32 y,			
	sint32 roadType,	
	uint16 roadOffset,	
	BOOL fog,			
	sint32 flags
	)
{
	MapPoint newPos;
	sint32 isConnectStraight = 0, isConnectDiagonal = 0;
	sint32 neighborFlag = 0;
	Pixel16	*data = NULL;

	
	if(pos.GetNeighborPosition(NORTH,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(NORTHEAST,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_NORTHEAST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_NORTHEAST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_NORTHEAST+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_NORTHEAST+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(EAST,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_EAST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_EAST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_EAST+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_EAST+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(SOUTHEAST,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_SOUTHEAST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_SOUTHEAST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_SOUTHEAST+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_SOUTHEAST+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);
















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(SOUTH,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_SOUTH+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_SOUTH+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_SOUTH+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_SOUTH+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(SOUTHWEST,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_SOUTHWEST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_SOUTHWEST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_SOUTHWEST+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_SOUTHWEST+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(WEST,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_WEST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_WEST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_WEST+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_WEST+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















			data = NULL;
		}
	}
	
	if(pos.GetNeighborPosition(NORTHWEST,newPos)) {
		if ( g_theWorld->IsRoad(k_ROAD_TYPE_1, newPos) ) {
			roadOffset = k_ROAD_OFFSET_1;
			data = m_tileSet->GetRoadData(ROAD_NORTHWEST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_2, newPos) ) {
			roadOffset = k_ROAD_OFFSET_2;
			data = m_tileSet->GetRoadData(ROAD_NORTHWEST+roadOffset);
		}
		else if ( g_theWorld->IsRoad(k_ROAD_TYPE_3, newPos) ) {
			roadOffset = k_ROAD_OFFSET_3;
			data = m_tileSet->GetRoadData(ROAD_NORTHWEST+roadOffset);
		} else if ( g_theWorld->IsTunnel(newPos) ) {
			roadOffset = k_ROAD_OFFSET_4;
			data = m_tileSet->GetRoadData(ROAD_NORTHWEST+roadOffset);
		}

		if (data) {
			DrawARoadPiece(surface, data, x, y, fog);

















		}
	}
}



