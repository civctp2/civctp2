//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Road drawing.
// Id           : $Id:$
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
// - Roads now use the TileSetIndex from the TerrainImprovement database. (28-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui.h"
#include "aui_directsurface.h"

#include "pixelutils.h"

#include "MapPoint.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TerrImproveData.h"

#include "tileutils.h"
#include "tileset.h"
#include "tiledmap.h"

#include "TileDrawRoad.h"

#include "directions.h"

extern World *g_theWorld;

void TiledMap::DrawARoadPiece
(
    aui_Surface *surface,
    Pixel16     *data,
    sint32       x,
    sint32       y,
    bool         fog,
    sint32       flags
)
{
	if (fog)
	{
		if (m_zoomLevel != k_ZOOM_NORMAL)
		{
			DrawBlendedOverlayScaled(surface, data, x, y, GetZoomTilePixelWidth(), 
				GetZoomTileGridHeight(), k_FOW_COLOR, k_FOW_BLEND_VALUE, flags);
		}
		else
		{
			DrawBlendedOverlay(surface, data, x, y, k_FOW_COLOR, k_FOW_BLEND_VALUE, flags);
		}
	}
	else
	{
		if (m_zoomLevel != k_ZOOM_NORMAL)
		{
			DrawScaledOverlay(surface, data, x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight(), flags);
		}
		else
		{
			DrawOverlay(surface, data, x, y, flags);
		}
	}
}

void TiledMap::DrawRoads
(
    aui_Surface    *surface,
    const MapPoint &pos,
    sint32          x,
    sint32          y,
    uint16          roadOffset,
    bool            fog,
    sint32          flags
)
{
	MapPoint    newPos;
	sint32      isConnectStraight = 0;
	sint32      isConnectDiagonal = 0;
	sint32      neighborFlag = 0;
	Pixel16    *data;

	for(WORLD_DIRECTION d = NORTH; d < NOWHERE; d = (WORLD_DIRECTION)((sint32)d + 1))
	{
		if(pos.GetNeighborPosition(d, newPos))
		{
			if(g_theWorld->IsAnyRoad(newPos)
			|| g_theWorld->IsTunnel(newPos)
			|| g_theWorld->IsCity(newPos)
			){
				neighborFlag |= (1 << d);

				if(d == NORTH
				|| d == EAST
				|| d == WEST
				|| d == SOUTH
				){
					isConnectStraight++;
				}
				else
				{
					isConnectDiagonal++;
				}
			}
		}
	}

	data = m_tileSet->GetRoadData(ROAD_MAGLEVPOST+roadOffset);
	if(data)
	{
		DrawARoadPiece(surface, data, x, y, fog);
	}

	if (!isConnectStraight && !isConnectDiagonal)
	{
		data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);

		return;
	}

	if (isConnectStraight == k_X_INTERSECT)
	{
		data = m_tileSet->GetRoadData(ROAD_X_STRAIGHT+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);
	}
	else if (isConnectStraight == k_T_INTERSECT)
	{
		if ((neighborFlag & k_T_SOUTH) == k_T_SOUTH)
		{
			data = m_tileSet->GetRoadData(ROAD_T_SOUTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
		else if ((neighborFlag & k_T_WEST) == k_T_WEST)
		{
			data = m_tileSet->GetRoadData(ROAD_T_WEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
		else if ((neighborFlag & k_T_NORTH) == k_T_NORTH)
		{
			data = m_tileSet->GetRoadData(ROAD_T_NORTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
		else /*if((neighborFlag & k_T_EAST) == k_T_EAST) */
		{
			data = m_tileSet->GetRoadData(ROAD_T_EAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
	}
	else if (isConnectStraight)
	{
		if (neighborFlag & k_BIT_SOUTH)
		{
			data = m_tileSet->GetRoadData(ROAD_SOUTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}

		if (neighborFlag & k_BIT_WEST)
		{
			data = m_tileSet->GetRoadData(ROAD_WEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}

		if (neighborFlag & k_BIT_NORTH)
		{
			data = m_tileSet->GetRoadData(ROAD_NORTH+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}

		if (neighborFlag & k_BIT_EAST)
		{
			data = m_tileSet->GetRoadData(ROAD_EAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
	}

	if (isConnectDiagonal == k_X_INTERSECT)
	{
		data = m_tileSet->GetRoadData(ROAD_X_DIAGONAL+roadOffset);
		DrawARoadPiece(surface, data, x, y, fog);
	}
	else if (isConnectDiagonal == k_T_INTERSECT)
	{
		if ((neighborFlag & k_T_SOUTHWEST) == k_T_SOUTHWEST)
		{
			data = m_tileSet->GetRoadData(ROAD_T_SOUTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
		else if ((neighborFlag & k_T_NORTHWEST) == k_T_NORTHWEST)
		{
			data = m_tileSet->GetRoadData(ROAD_T_NORTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
		else if ((neighborFlag & k_T_NORTHEAST) == k_T_NORTHEAST)
		{
			data = m_tileSet->GetRoadData(ROAD_T_NORTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
		else /*if((neighborFlag & k_T_SOUTHEAST) == SOUTHEAST) */
		{
			data = m_tileSet->GetRoadData(ROAD_T_SOUTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
	}
	else if (isConnectDiagonal)
	{
		if (neighborFlag & k_BIT_SOUTHWEST)
		{
			data = m_tileSet->GetRoadData(ROAD_SOUTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}

		if (neighborFlag & k_BIT_NORTHWEST)
		{
			data = m_tileSet->GetRoadData(ROAD_NORTHWEST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}

		if (neighborFlag & k_BIT_NORTHEAST)
		{
			data = m_tileSet->GetRoadData(ROAD_NORTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}

		if (neighborFlag & k_BIT_SOUTHEAST)
		{
			data = m_tileSet->GetRoadData(ROAD_SOUTHEAST+roadOffset);
			DrawARoadPiece(surface, data, x, y, fog);
		}
	}
}
