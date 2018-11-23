//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface radar map
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #01 Allow shifing the X and Y axis in the radar map with RMouse clicks
//   (L. Hirth 6/2004)
// - Standardised ceil/min/max usage.
// - Radar tile boarder color determined by the visual cell owner instead by
//   the actual cell owner. - Nov 1st 2004 - Martin G�hmann
// - Radar tile boarder is now fully determined by the visible tile onwer
//   instead of being determined half by the actual tile owner and half by the
//   the the visible tile owner this fixes the bug that appears after conquest
//   of a city. - Nov. 1st 2004 - Martin G�hmann
// - The radar map now shows the current terrain and the current units and
//   cities if fog of war is off, otherwise it only displays the kind of
//   information it should display. - Dec. 25th 2004 - Martin G�hmann
// - Borders on the minimap are now shown if fog of war is off or god mode
//   is on, even if the there is no contact to that civilisation.
//   - Mar. 4th 2005 Martin G�hmann
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "radarmap.h"

#include <algorithm>

#include "aui.h"
#include "aui_Factory.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_action.h"
#include "c3ui.h"
#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "UnseenCell.h"
#include "citydata.h"
#include "Unit.h"
#include "UnitData.h"
#include "pixelutils.h"
#include "colorset.h"
#include "SelItem.h"
#include "tiledmap.h"
#include "director.h"
#include "maputils.h"
#include "primitives.h"
#include "profileDB.h"
#include "pointerlist.h"
#include "terrainutil.h"
#include "Scheduler.h"

extern C3UI				*g_c3ui;
extern TiledMap			*g_tiledMap;
extern Player			**g_player;
extern PointerList<Player> *g_deadPlayer;
extern ProfileDB		*g_theProfileDB;
extern SelectedItem		*g_selected_item;
extern ColorSet			*g_colorSet;
extern World			*g_theWorld;

extern sint32 g_fog_toggle;
extern sint32 g_god;

static const unsigned char k_EAST_BORDER_FLAG		= 0x01;
static const unsigned char k_WEST_BORDER_FLAG		= 0x02;
static const unsigned char k_NORTH_EAST_BORDER_FLAG	= 0x04;
static const unsigned char k_NORTH_WEST_BORDER_FLAG	= 0x08;
static const unsigned char k_SOUTH_EAST_BORDER_FLAG	= 0x10;
static const unsigned char k_SOUTH_WEST_BORDER_FLAG	= 0x20;

//---------------------------------------------------------------------------
//
//	RadarMap::RadarMap
//
//---------------------------------------------------------------------------
RadarMap::RadarMap(AUI_ERRCODE *retval,
							sint32 id,
							const MBCHAR *ldlBlock,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock),
		aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
		PatternBase(ldlBlock, NULL)
{
	InitCommonLdl(ldlBlock);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RadarMap
//
//---------------------------------------------------------------------------
RadarMap::RadarMap(AUI_ERRCODE *retval,
							uint32 id,
							sint32 x,
							sint32 y,
							sint32 width,
							sint32 height,
							const MBCHAR *pattern,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:
		aui_ImageBase((sint32)0),
		aui_TextBase((MBCHAR *)NULL),
		aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
		PatternBase(pattern)
{
	InitCommon();
}

//---------------------------------------------------------------------------
//
//	RadarMap::~RadarMap
//
//---------------------------------------------------------------------------
RadarMap::~RadarMap()
{
	if (m_mapSurface)
		delete m_mapSurface;
	if (m_tempSurface)
		delete m_tempSurface;
	if (m_tempBuffer)
		free(m_tempBuffer);
}

//---------------------------------------------------------------------------
//
//	RadarMap::InitCommonLdl
//
//---------------------------------------------------------------------------
void RadarMap::InitCommonLdl(const MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return;


	InitCommon();
}

//---------------------------------------------------------------------------
//
//	RadarMap::InitCommon
//
//---------------------------------------------------------------------------
void RadarMap::InitCommon(void)
{
	AUI_ERRCODE			errcode;

	m_mapSurface = NULL;
	m_mapSize = NULL;
	m_tempSurface = NULL;
	m_tempBuffer = NULL;

	m_tilePixelWidth = 0.0;
	m_tilePixelHeight = 0.0;

	m_displayUnits = g_theProfileDB->GetDisplayUnits() != FALSE;
	m_displayCities = g_theProfileDB->GetDisplayCities() != FALSE;
	m_displayBorders = g_theProfileDB->GetDisplayBorders() != FALSE;
	m_displayOverlay = true;
	m_filter = g_theProfileDB->GetDisplayFilter() != FALSE;
	m_displayTrade = g_theProfileDB->GetDisplayTrade() != FALSE;
	m_displayTerrain = g_theProfileDB->GetDisplayTerrain() != FALSE;

	m_mapOverlay = NULL;

	MapPoint resetPos (0,0);
	m_lastCenteredPoint = resetPos;

	m_isInteractive = true;

	m_selectedCity.m_id = 0;

	m_mapSurface = aui_Factory::new_Surface(errcode, m_width, m_height, 16);
	Assert( AUI_NEWOK(m_mapSurface, errcode) );

	RECT rect = { 0, 0, m_width, m_height };

	if ( m_pattern ) {
		m_pattern->Draw( m_mapSurface, &rect );
	}

	if ( g_theWorld ) {

		CalculateMetrics();

		RenderMap(m_mapSurface);
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::ClearMapOverlay
//
//---------------------------------------------------------------------------
void RadarMap::ClearMapOverlay(void)
{
	delete[] m_mapOverlay;
	m_mapOverlay = NULL;
}

//---------------------------------------------------------------------------
//
//	RadarMap::SetMapOverlayCell
//
//---------------------------------------------------------------------------
//	- Sets a position for the Gaia controller overlay and creates the over-
//    lay it does not exists yet
//
//---------------------------------------------------------------------------
void RadarMap::SetMapOverlayCell(const MapPoint &pos, COLOR color)
{
	if (m_mapOverlay == NULL) {
		sint32 len = m_mapSize->x * m_mapSize->y;
		m_mapOverlay = new COLOR[len];

		for (sint32 i=0; i<len; i++)
			m_mapOverlay[i] = COLOR_MAX;
	}

	m_mapOverlay[pos.x + (pos.y * m_mapSize->x)] = color;
}

//---------------------------------------------------------------------------
//
//	RadarMap::Resize
//
//---------------------------------------------------------------------------
//	- resize the radar map
//
//---------------------------------------------------------------------------
AUI_ERRCODE	RadarMap::Resize( sint32 width, sint32 height )
{
	AUI_ERRCODE		errcode;

	errcode = aui_Region::Resize(width, height);
	Assert(errcode == AUI_ERRCODE_OK);

	if (m_mapSurface)
		delete m_mapSurface;

	m_mapSurface = aui_Factory::new_Surface(errcode, width, height, 16);
	Assert( AUI_NEWOK(m_mapSurface, errcode) );

	CalculateMetrics();

	RenderMap(m_mapSurface);

	return errcode;
}

//---------------------------------------------------------------------------
//
//	RadarMap::CalculateMetrics
//
//---------------------------------------------------------------------------
//	- calculate some values depending on the current radar map size
//
//---------------------------------------------------------------------------
void RadarMap::CalculateMetrics(void)
{
	if (!g_theWorld) return;

	if (m_tempSurface)
		delete m_tempSurface;
	if (m_tempBuffer)
		free(m_tempBuffer);

	m_mapSize = g_theWorld->GetSize();

	m_tilePixelWidth = ((double )m_width) / m_mapSize->x;
	m_tilePixelHeight = ((double )m_height) / m_mapSize->y;

	AUI_ERRCODE err;
	uint32 width = m_mapSize->x * 2;
	uint32 height = m_mapSize->y;

	m_tempBuffer = (uint8 *)calloc((width + 2) * (height + 2), 2);

	m_tempSurface = new aui_Surface(&err, width, height, 16, 2*(width + 2), &m_tempBuffer[2*((width + 2) + (1))]);
}

//---------------------------------------------------------------------------
//
//	RadarMap::MapToPixel
//
//---------------------------------------------------------------------------
//	- Seems unused
//
//---------------------------------------------------------------------------
POINT RadarMap::MapToPixel(sint32 x, sint32 y)
{
	POINT		pt;
	sint32		k;
	double		nudge;
	k = ((y / 2) + x) % m_mapSize->x;

	nudge = 0;
	if (y&1) {
		nudge = m_tilePixelWidth / 2.0;
	}

	pt.x = (sint32)(k * m_tilePixelWidth);
	pt.y = (sint32)(y * m_tilePixelHeight);

	return pt;
}

//---------------------------------------------------------------------------
//
//	RadarMap::MapToPixel
//
//---------------------------------------------------------------------------
//	- Seems unused because calling function is unused
//
//---------------------------------------------------------------------------
POINT RadarMap::MapToPixel(MapPoint *pos)
{
	return MapToPixel(pos->x, pos->y);
}




//---------------------------------------------------------------------------
//
//	RadarMap::GetVisiblePlayerToRender
//
//---------------------------------------------------------------------------
//	- Gives back the player that shall be shown currently
//
//---------------------------------------------------------------------------
Player *RadarMap::GetVisiblePlayerToRender()
{


	if(!g_tiledMap || !g_tiledMap->ReadyToDraw() ||
		!g_theWorld || !g_selected_item || !m_mapSize)
		return(NULL);






	Assert(m_mapSize->x < 0 || m_mapSize->y > 0);
	if(m_mapSize->x <= 0 || m_mapSize->y <= 0)
		return(NULL);

	return(g_player[g_selected_item->GetVisiblePlayer()]);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RadarTileColor
//
//---------------------------------------------------------------------------
//	- Determines the color that has to be set for a position of the
//    RadarMap
//
//---------------------------------------------------------------------------
Pixel16 RadarMap::RadarTileColor(const Player *player, const MapPoint &position,
								 const MapPoint &worldpos, uint32 &flags)
{

	Unit unit;

	flags = 0;

	if(player->m_vision->IsExplored(worldpos)) {
		if(m_displayTrade && g_theWorld->GetCell(worldpos)->GetNumTradeRoutes() > 0) {
			flags = 1;
		}

		if(m_displayOverlay && m_mapOverlay) {
			COLOR color = m_mapOverlay[worldpos.y * m_mapSize->x + worldpos.x];
			if(color != COLOR_MAX)
				return(g_colorSet->GetColor(color));
		}

		if(!g_theWorld->GetTopVisibleUnit(worldpos, unit))
			g_theWorld->GetTopRadarUnit(worldpos, unit);

		UnseenCellCarton unseenCellCarton;
		int uCellValid = !g_fog_toggle && player->m_vision->GetLastSeen(worldpos, unseenCellCarton);

		if(unit.IsValid()) {
			if(unit.IsCity()) {
				if(m_displayCities){
					// Added distinction between hidden and non hidden tiles, by Martin G�hmann.
					if(uCellValid){

						if(unseenCellCarton.m_unseenCell->GetActor()){
						// Only if there was a city at the last visit
							if(unit.m_id == m_selectedCity.m_id)
								return(g_colorSet->GetColor(COLOR_RED));
							else
								return(g_colorSet->GetPlayerColor(unseenCellCarton.m_unseenCell->GetCityOwner()));
						}
						else{
							// Check wether something else is visible. At least invalidize the unit.
							unit.m_id = 0;
							g_theWorld->GetTopVisibleUnitNotCity(worldpos, unit);
						}
					}
					else{
						if(unit.m_id == m_selectedCity.m_id)
							return(g_colorSet->GetColor(COLOR_RED));
						else
							return(g_colorSet->GetPlayerColor(unit.GetOwner()));
					}
				}
				else {

					unit.m_id = 0;
					g_theWorld->GetTopVisibleUnitNotCity(worldpos, unit);
				}
			}
			if(m_displayUnits && unit.m_id)
				return(g_colorSet->GetPlayerColor(unit.GetOwner()));
		}

		if(uCellValid) {
			if(m_displayTerrain) {
				return(g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
															   unseenCellCarton.m_unseenCell->GetTerrainType())));
			} else {

				if(g_theWorld->IsLand(worldpos) || g_theWorld->IsMountain(worldpos)) {

					return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
																   TERRAIN_GRASSLAND));
				} else {
					return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
																   TERRAIN_WATER_DEEP));
				}
			}
		}

		Cell *cell = g_theWorld->GetCell(worldpos);

		if(m_displayTerrain) {

			return(g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
														   cell->GetTerrainType())));
		} else {
			if(g_theWorld->IsLand(worldpos) || g_theWorld->IsMountain(worldpos)) {
				return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
															   TERRAIN_GRASSLAND));
			} else {
				return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
															   TERRAIN_WATER_DEEP));
			}
		}
	}

	if(g_theWorld->GetTopRadarUnit(worldpos, unit))
		return(g_colorSet->GetPlayerColor(unit.GetOwner()));

	return(g_colorSet->GetColor(COLOR_BLACK));
}


//---------------------------------------------------------------------------
//
//	RadarMap::RadarTileBorderColor
//
//---------------------------------------------------------------------------
//	- Checks which color a border must be drawn for the current tile
//
//---------------------------------------------------------------------------
Pixel16 RadarMap::RadarTileBorderColor(const MapPoint &position)
{

// Added by Martin G�hmann
	sint32 owner = g_tiledMap->GetVisibleCellOwner(const_cast<MapPoint&>(position));
	if(owner < 0)
		return(g_colorSet->GetColor(COLOR_BLACK));

	return(g_colorSet->GetPlayerColor(owner));
}

//---------------------------------------------------------------------------
//
//	RadarMap::RadarTileBorder
//
//---------------------------------------------------------------------------
//	- Checks which borders must be drawn for the current tile
//
//---------------------------------------------------------------------------
uint8 RadarMap::RadarTileBorder(const Player *player, const MapPoint &position)
{

	uint8 borderFlags = 0;

	if(!m_displayBorders)
		return(borderFlags);

	if(!player->m_vision->IsExplored(position))
		return(borderFlags);

// Added by Martin G�hmann
	sint32 owner = g_tiledMap->GetVisibleCellOwner(const_cast<MapPoint&>(position));

	if(owner < 0)
		return(borderFlags);

	if(owner != player->m_owner
	&& !player->m_hasGlobalRadar
	&& !Scheduler::CachedHasContactWithExceptSelf(player->m_owner, owner)
	&& !g_fog_toggle // Don't forget if fog of war is off
	&& !g_god
	)
		return(borderFlags);

	MapPoint neighborPosition;

	if(position.GetNeighborPosition(EAST, neighborPosition) &&
		(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		borderFlags |= k_EAST_BORDER_FLAG;
	if(position.GetNeighborPosition(WEST, neighborPosition) &&
		(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		borderFlags |= k_WEST_BORDER_FLAG;
	if(position.GetNeighborPosition(NORTHEAST, neighborPosition) &&
		(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		borderFlags |= k_NORTH_EAST_BORDER_FLAG;
	if(position.GetNeighborPosition(NORTHWEST, neighborPosition) &&
		(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		borderFlags |= k_NORTH_WEST_BORDER_FLAG;
	if(position.GetNeighborPosition(SOUTHEAST, neighborPosition) &&
		(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		borderFlags |= k_SOUTH_EAST_BORDER_FLAG;
	if(position.GetNeighborPosition(SOUTHWEST, neighborPosition) &&
		(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		borderFlags |= k_SOUTH_WEST_BORDER_FLAG;

	return(borderFlags);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderTradeRoute
//
//---------------------------------------------------------------------------
//	- Draws a trade route sign for the current tile
//
//---------------------------------------------------------------------------
void RadarMap::RenderTradeRoute(aui_Surface *surface,
								const RECT &tileRectangle)
{
	sint32 tileWidth = tileRectangle.right - tileRectangle.left;
	sint32 tileHeight = tileRectangle.bottom - tileRectangle.top;

	RECT tradeRect = {
		tileRectangle.left + (tileWidth / 2),
		tileRectangle.top + (tileHeight / 2),
		tileRectangle.left + (tileWidth / 2) + 1,
		tileRectangle.top + (tileHeight / 2) + 1
	};

	primitives_PaintRect16(surface, &tradeRect, g_colorSet->GetColor(COLOR_YELLOW));
}


//---------------------------------------------------------------------------
//
//	RadarMap::RenderSpecialTile
//
//---------------------------------------------------------------------------
//	- Renders the current special radar map area, that means tiles that are
//    on the max or min x value.
//
//---------------------------------------------------------------------------
void RadarMap::RenderSpecialTile(aui_Surface *surface,
								 const MapPoint &screenPosition,
								 Pixel16 color, uint32 flags)
{
	RECT tileRectangle = {
		2*screenPosition.x + 1,
		screenPosition.y,
		2*screenPosition.x + 2,
		screenPosition.y + 1,
	};

	primitives_PaintRect16(surface, &tileRectangle, color);


	tileRectangle.left = 0;
	tileRectangle.right = 1;
	primitives_PaintRect16(surface, &tileRectangle, color);

}


//---------------------------------------------------------------------------
//
//	RadarMap::RenderSpecialTileBorder
//
//---------------------------------------------------------------------------
//	- Renders the current special radar map borders, that means tiles that are
//    on the max or min x value.
//
//---------------------------------------------------------------------------
void RadarMap::RenderSpecialTileBorder(aui_Surface *surface,
								 const MapPoint &screenPosition,
								 uint8 borderFlags,
								 Pixel16 borderColor)
{

	double xPosition = screenPosition.x * m_tilePixelWidth;
	double yPosition = screenPosition.y * m_tilePixelHeight;

	xPosition += m_tilePixelWidth / 2.0;

	RECT tileRectangle = {
		static_cast<sint32>(ceil(xPosition)),
		static_cast<sint32>(ceil(yPosition)),
		static_cast<sint32>(ceil(xPosition + (m_tilePixelWidth / 2.0))),
		static_cast<sint32>(ceil(yPosition + m_tilePixelHeight))
	};

	tileRectangle.right		= std::max(tileRectangle.left, (tileRectangle.right - 1));
	tileRectangle.bottom	= std::max(tileRectangle.top, (tileRectangle.bottom - 1));

	if(borderFlags & k_WEST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.left, tileRectangle.top,
			tileRectangle.left, tileRectangle.bottom, borderColor);
	if(borderFlags & k_NORTH_WEST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.right, tileRectangle.top,
			tileRectangle.left, tileRectangle.top, borderColor);
	if(borderFlags & k_SOUTH_WEST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.right, tileRectangle.bottom,
			tileRectangle.left, tileRectangle.bottom, borderColor);


	tileRectangle.left = 0;
	tileRectangle.right = static_cast<sint32>(ceil(m_tilePixelWidth / 2.0));

	tileRectangle.right	= std::max(tileRectangle.left, (tileRectangle.right - 1));

	if(borderFlags & k_EAST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.right, tileRectangle.top,
			tileRectangle.right, tileRectangle.bottom, borderColor);
	if(borderFlags & k_NORTH_EAST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.left, tileRectangle.top,
			tileRectangle.right, tileRectangle.top, borderColor);
	if(borderFlags & k_SOUTH_EAST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.left, tileRectangle.bottom,
			tileRectangle.right, tileRectangle.bottom, borderColor);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderNormalTile
//
//---------------------------------------------------------------------------
//	- Renders the current normal radar map area
//
//---------------------------------------------------------------------------
void RadarMap::RenderNormalTile(aui_Surface *surface,
								const MapPoint &screenPosition,
								Pixel16 color, uint32 flags)
{
	RECT tileRectangle;
	tileRectangle.left = 2 * screenPosition.x + (screenPosition.y&1);
	tileRectangle.right = tileRectangle.left + 2;
	tileRectangle.top = screenPosition.y;
	tileRectangle.bottom = screenPosition.y + 1;
	primitives_PaintRect16(surface, &tileRectangle, color);

}


//---------------------------------------------------------------------------
//
//	RadarMap::RenderNormalTileBorder
//
//---------------------------------------------------------------------------
//	- Renders the current normal radar map borders
//
//---------------------------------------------------------------------------
void RadarMap::RenderNormalTileBorder(aui_Surface *surface,
		const MapPoint &screenPosition,
		uint8 borderFlags, Pixel16 borderColor)
{

	double xPosition = screenPosition.x * m_tilePixelWidth;
	double yPosition = screenPosition.y * m_tilePixelHeight;

	if(screenPosition.y & 1)
		xPosition += m_tilePixelWidth / 2.0;

	RECT tileRectangle = {
		static_cast<sint32>(ceil(xPosition)),
		static_cast<sint32>(ceil(yPosition)),
		static_cast<sint32>(ceil(xPosition + m_tilePixelWidth)),
		static_cast<sint32>(ceil(yPosition + m_tilePixelHeight))
	};

	sint32 middle = sint32(ceil(xPosition + m_tilePixelWidth/2));

	tileRectangle.right		= std::max(tileRectangle.left, (tileRectangle.right - 1));
	tileRectangle.bottom	= std::max(tileRectangle.top, (tileRectangle.bottom - 1));
	middle					= std::min(middle, tileRectangle.right);

	if(tileRectangle.right >= surface->Width())
		tileRectangle.right = surface->Width() - 1;
	if(tileRectangle.bottom >= surface->Height())
		tileRectangle.bottom = surface->Height() - 1;

	if(borderFlags & k_EAST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.right, tileRectangle.top,
			tileRectangle.right, tileRectangle.bottom, borderColor);
	if(borderFlags & k_NORTH_EAST_BORDER_FLAG)
		primitives_DrawLine16(surface, middle, tileRectangle.top,
			tileRectangle.right, tileRectangle.top, borderColor);
	if(borderFlags & k_SOUTH_EAST_BORDER_FLAG)
		primitives_DrawLine16(surface, middle, tileRectangle.bottom,
			tileRectangle.right, tileRectangle.bottom, borderColor);

	if(borderFlags & k_WEST_BORDER_FLAG)
		primitives_DrawLine16(surface, tileRectangle.left, tileRectangle.top,
			tileRectangle.left, tileRectangle.bottom, borderColor);
	if(borderFlags & k_NORTH_WEST_BORDER_FLAG)
		primitives_DrawLine16(surface, middle, tileRectangle.top,
			tileRectangle.left, tileRectangle.top, borderColor);
	if(borderFlags & k_SOUTH_WEST_BORDER_FLAG)
		primitives_DrawLine16(surface, middle, tileRectangle.bottom,
			tileRectangle.left, tileRectangle.bottom, borderColor);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderMapTile
//
//---------------------------------------------------------------------------
//	- Controls which method renders the current radar map
//    tile
//
//---------------------------------------------------------------------------
void RadarMap::RenderMapTile(aui_Surface *surface, const MapPoint &screenPosition, Pixel16 color, uint32 flags)
{

	if((screenPosition.y & 1) && (screenPosition.x == (m_mapSize->x - 1)))
		RenderSpecialTile(surface, screenPosition, color, flags);
	else
		RenderNormalTile(surface, screenPosition, color, flags);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderMapTileBorder
//
//---------------------------------------------------------------------------
//	- Controls which method renders the current radar map
//    borders
//
//---------------------------------------------------------------------------
void RadarMap::RenderMapTileBorder(aui_Surface *surface, const MapPoint &screenPosition,
							 uint8 borderFlags, Pixel16 borderColor)
{

	if((screenPosition.y & 1) && (screenPosition.x == (m_mapSize->x - 1)))
		RenderSpecialTileBorder(surface, screenPosition, borderFlags, borderColor);
	else
		RenderNormalTileBorder(surface, screenPosition, borderFlags, borderColor);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderTile
//
//---------------------------------------------------------------------------
//	- Controls the rendering of the area for the current radar map
//    tile
//
//---------------------------------------------------------------------------
void RadarMap::RenderTile(aui_Surface *surface, const MapPoint &position,
						  const MapPoint &worldpos, Player *player)

{
	uint32 flags;

	Pixel16 const	color	=
		RadarTileColor(player, MapPoint(position.x, position.y), worldpos, flags);
	sint32 const	x		= ((position.y / 2) + position.x) % m_mapSize->x;




	RenderMapTile(surface, MapPoint(x, position.y), color, flags);

}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderTileBorder
//
//---------------------------------------------------------------------------
//	- Controls the rendering of the borders for the current radar map
//    position
//---------------------------------------------------------------------------
void RadarMap::RenderTileBorder(aui_Surface *surface, const MapPoint &position,
						  const MapPoint &worldpos, Player *player)
{
	uint8 const	borderFlags =
		RadarTileBorder(player, MapPoint(worldpos.x, worldpos.y));

	if (borderFlags)
	{
		Pixel16 const	borderColor =
			RadarTileBorderColor(MapPoint(worldpos.x, worldpos.y));
		sint32 const	x	= ((position.y / 2) + position.x) % m_mapSize->x;
		RenderMapTileBorder(surface, MapPoint(x, position.y), borderFlags, borderColor);
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderTrade
//
//---------------------------------------------------------------------------
//  - Prepares and call the rendering of the trade routes
//
//---------------------------------------------------------------------------
void RadarMap::RenderTrade(aui_Surface *surface, const MapPoint &position, const MapPoint &worldpos, Player *player)
{
	if(!m_displayTrade)
		return;




	MapPoint screenPosition(((worldpos.y / 2) + position.x) % (m_mapSize->x), position.y);

	if(!g_theWorld->GetCell(worldpos)->GetNumTradeRoutes() ||
	   !player->m_vision->IsExplored(worldpos)) {
		return;
	}

	double xPosition = screenPosition.x * m_tilePixelWidth;
	double yPosition = screenPosition.y * m_tilePixelHeight;

	if(screenPosition.y & 1)
		xPosition += m_tilePixelWidth / 2.0;

	RECT tileRectangle = {
		static_cast<sint32>(ceil(xPosition)),
		static_cast<sint32>(ceil(yPosition)),
		static_cast<sint32>(ceil(xPosition + m_tilePixelWidth)),
		static_cast<sint32>(ceil(yPosition + m_tilePixelHeight))
	};

	RenderTradeRoute(surface, tileRectangle);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderMap
//
//---------------------------------------------------------------------------
//	- Redraws the complete radarmap
//
//---------------------------------------------------------------------------
void RadarMap::RenderMap(aui_Surface *surface)
{

	Player *player = GetVisiblePlayerToRender();

	if(!player) {

		RECT destRect = { 0, 0, surface->Width(), surface->Height() };
		primitives_PaintRect16(surface, &destRect, g_colorSet->GetColor(COLOR_BLACK));
		return;
	}

	sint32 x, y;

	for(y = 0; y < m_mapSize->y; y++)
		for(x = 0; x < m_mapSize->x; x++)
		{
			RenderTile(m_tempSurface, PosWorldToPosRadar(MapPoint(x, y)), MapPoint(x, y), player);
		}

	fRect sRect = {0, 0, m_tempSurface->Width(), m_tempSurface->Height()};
	fRect dRect;
	dRect.left = 0;
	dRect.top = 0;
	dRect.right = (m_tilePixelWidth/2) * sRect.right;
	dRect.bottom = m_tilePixelHeight * sRect.bottom;

	primitives_Scale16(m_tempSurface, surface,
		sRect,
		dRect,
		m_filter);

	for(y = 0; y < m_mapSize->y; y++)
		for(x = 0; x < m_mapSize->x; x++)
		{
			RenderTileBorder(surface, PosWorldToPosRadar(MapPoint(x, y)), MapPoint(x, y), player);
			RenderTrade(surface, PosWorldToPosRadar(MapPoint(x, y)), MapPoint(x, y), player);
		}
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderViewRect
//
//---------------------------------------------------------------------------
//  - Draws the rectangle on the radar map that corresponds to the view of
//    the main tile map.
//
//---------------------------------------------------------------------------
void RadarMap::RenderViewRect
(
	aui_Surface *surf,
	sint32 x,
	sint32 y
)
{
	RECT offsetRect;
	sint32 nrplayer;

	if (g_tiledMap) {

		nrplayer = g_selected_item->GetVisiblePlayer();

		RECT *temp = g_tiledMap->GetMapViewRect();
		m_mapViewRect = *temp;

		if(!g_tiledMap->ReadyToDraw())
			return;

		offsetRect.bottom = m_mapViewRect.bottom;
		offsetRect.top = m_mapViewRect.top;
		offsetRect.left = m_mapViewRect.left;
		offsetRect.right = m_mapViewRect.right;

		offsetRect.top += m_displayOffset[nrplayer].y;
		offsetRect.bottom += m_displayOffset[nrplayer].y;
		if (offsetRect.top + ((offsetRect.bottom - offsetRect.top)/2) > m_mapSize->y) {
			// if view is now too far to the bottom subtract the y-mapsize
			offsetRect.top -= m_mapSize->y;
			offsetRect.bottom -= m_mapSize->y;
		}

		offsetRect.left += m_displayOffset[nrplayer].x;
		offsetRect.right += m_displayOffset[nrplayer].x;
		if (offsetRect.left + ((offsetRect.right - offsetRect.left)/2) > m_mapSize->x) {
			// if view is now too far to the right subtract the x-mapsize
			offsetRect.left -= m_mapSize->x;
			offsetRect.right -= m_mapSize->x;
		}
	}


	sint32 x1,x2,x3,x4;
	sint32 y1,y2,y3,y4;

	if ( m_mapSize )
	{
		sint32 mapWidth = m_mapSize->x;
		sint32 mapHeight = m_mapSize->y;

		// Set the X coordinate points
		if (!g_theWorld->IsXwrap() && (offsetRect.left < 0 || offsetRect.right > mapWidth))
		{
			x1 = x3 = offsetRect.left;
			x2 = x4 = offsetRect.right;

			if (offsetRect.left < 0)
			{
				x1 = x3 = 0;
			}
			if (offsetRect.right > mapWidth)
			{
				x2 = x4 = mapWidth;
			}
		}

		else
		{

			if (offsetRect.left < 0)
			{
				x1 = offsetRect.left + mapWidth;
				x2 = mapWidth;

				if (offsetRect.right <= 0)
				{
					x3 = mapWidth;
					x4 = mapWidth;
				}

				else
				{
					x3 = 0;
					x4 = offsetRect.right;
				}
			}

			else
			{

				if (offsetRect.right > mapWidth)
				{
					x1 = offsetRect.left;
					x2 = mapWidth;
					x3 = 0;
					x4 = offsetRect.right - mapWidth;
				}

				else
				{
					x1 = offsetRect.left;
					x2 = offsetRect.right;
					x3 = offsetRect.left;
					x4 = offsetRect.right;
				}
			}
		}

		x1 = (sint32) (x1 * m_tilePixelWidth) + x;
		x2 = (sint32) (x2 * m_tilePixelWidth) + x - 1;
		x3 = (sint32) (x3 * m_tilePixelWidth) + x;
		x4 = (sint32) (x4 * m_tilePixelWidth) + x - 1;

		// Set the Y points
		if ( !g_theWorld->IsYwrap() && ( offsetRect.top < 0 || offsetRect.bottom >= mapHeight))
		{

			y1 = y3 = offsetRect.top;
			y2 = y4 = offsetRect.bottom;
			if (offsetRect.top < 0)
			{
				y1 = y3 = 0;
			}

			if (offsetRect.bottom > mapHeight)
			{
				y2 = y4 = mapHeight;
			}
		}

		else
		{

			if ( offsetRect.top < 0 )
			{
				y1 = offsetRect.top + mapHeight;
				y2 = mapHeight;
				if (offsetRect.bottom <= 0)
				{
					y3 = mapHeight;
					y4 = mapHeight;
				}
				else
				{
					y3 = 0;
					y4 = offsetRect.bottom;
				}
			}
			else
			{
				if (offsetRect.bottom > mapHeight)
				{
					y1 = offsetRect.top;
					y2 = mapHeight;
					y3 = 0;
					y4 = offsetRect.bottom - mapHeight;
				}
				else
				{
					y1 = offsetRect.top;
					y2 = offsetRect.bottom;
					y3 = offsetRect.top;
					y4 = offsetRect.bottom;
				}
			}
		}

		y1 = (sint32) (y1 * m_tilePixelHeight) + y;
		y2 = (sint32) (y2 * m_tilePixelHeight) + y - 1;
		y3 = (sint32) (y3 * m_tilePixelHeight) + y;
		y4 = (sint32) (y4 * m_tilePixelHeight) + y - 1;

	}
	else
	{ // no map size
		x1 = x3 = 0;
		x2 = x4 = 0;
		y1 = y3 = 0;
		y2 = y4 = 0;
	}

	// Draw the rectangle
	primitives_DrawLine16(surf,x1,y1,x2,y1,0xffff);
	primitives_DrawLine16(surf,x1,y1,x1,y2,0xffff);
	primitives_DrawLine16(surf,x3,y1,x4,y1,0xffff);
	primitives_DrawLine16(surf,x4,y1,x4,y2,0xffff);
	primitives_DrawLine16(surf,x1,y4,x2,y4,0xffff);
	primitives_DrawLine16(surf,x1,y3,x1,y4,0xffff);
	primitives_DrawLine16(surf,x3,y4,x4,y4,0xffff);
 	primitives_DrawLine16(surf,x4,y3,x4,y4,0xffff);
}

//---------------------------------------------------------------------------
//
//	RadarMap::UpdateMap
//
//---------------------------------------------------------------------------
void RadarMap::UpdateMap(aui_Surface *surf, sint32 x, sint32 y)
{
	RECT		destRect = {x, y, x + Width(), y + Height() };
	RECT		srcRect = {0, 0, Width(), Height()};

	g_c3ui->TheBlitter()->StretchBlt(surf, &destRect, m_mapSurface, &srcRect, k_AUI_BLITTER_FLAG_COPY);

	if(IsInteractive())
		RenderViewRect(surf, x, y);
}


//---------------------------------------------------------------------------
//
//	RadarMap::ComputeCenteredMap
//
//---------------------------------------------------------------------------
MapPoint RadarMap::ComputeCenteredMap(const MapPoint &pos, RECT *viewRect)
{
	RECT *mapViewRect = viewRect;

	sint32 w = mapViewRect->right - mapViewRect->left;
	sint32 h = mapViewRect->bottom - mapViewRect->top;

	sint32 tileX;
	maputils_MapX2TileX(pos.x,pos.y,&tileX);

	mapViewRect->left = tileX - (w>>1);
	mapViewRect->top = (pos.y - (h>>1)) & (~1);
	mapViewRect->right = mapViewRect->left + w;
	mapViewRect->bottom = mapViewRect->top + h;

	return pos;
}

//---------------------------------------------------------------------------
//
//	RadarMap::CenterMap
//
//---------------------------------------------------------------------------
//  - Used to focus the RadarMap to a specific point
//
//---------------------------------------------------------------------------
MapPoint RadarMap::CenterMap(const MapPoint &pos)
{
	MapPoint LastPT = m_lastCenteredPoint;
	if(LastPT.x == 0 && LastPT.y == 0)
		LastPT = pos;

	m_lastCenteredPoint = pos;

	RECT *mapViewRect = g_tiledMap->GetMapViewRect();

	ComputeCenteredMap(pos, mapViewRect);
	m_mapViewRect = *mapViewRect;
	RenderMap(m_mapSurface);

	return LastPT;
}


//---------------------------------------------------------------------------
//
//	RadarMap::IncludePointInView
//
//---------------------------------------------------------------------------
//	- seems unused
//
//---------------------------------------------------------------------------
BOOL RadarMap::IncludePointInView(MapPoint &pos, sint32 radius)
{
	RECT		*mapViewRect = g_tiledMap->GetMapViewRect();
	RECT		adjustedRect = *mapViewRect;
	sint32		wrappedLeft, wrappedTop;
	sint32		tileX, tileY;

	maputils_MapX2TileX(pos.x, pos.y, &tileX);
	tileY = pos.y;

	maputils_WrapPoint(mapViewRect->left, mapViewRect->top, &wrappedLeft, &wrappedTop);

	InflateRect(&adjustedRect, -radius, -radius);


	adjustedRect.top += 1;
	adjustedRect.bottom -= 3;

	if (tileY >= adjustedRect.top && tileY < adjustedRect.bottom &&
			tileX >=adjustedRect.left && tileX < adjustedRect.right)
		return FALSE;

	sint32 w = mapViewRect->right - mapViewRect->left;
	sint32 h = mapViewRect->bottom - mapViewRect->top;

	sint32	newLeft=wrappedLeft,
			newTop=wrappedTop;

	if (tileX < adjustedRect.left)
		newLeft = wrappedLeft - (adjustedRect.left - tileX);
	else
		if (tileX > adjustedRect.right)
			newLeft = wrappedLeft + (tileX - adjustedRect.right);

	if (tileY < adjustedRect.top)
		newTop = wrappedTop - (adjustedRect.top - tileY);
	else
		if (tileY >= adjustedRect.bottom)
			newTop = wrappedTop + (tileY - adjustedRect.bottom);

	sint32 newX, newY;

	maputils_WrapPoint(newLeft, newTop, &newX, &newY);

	mapViewRect->left = newX;
	mapViewRect->top = newY & ~0x1;
	mapViewRect->right = mapViewRect->left + w;
	mapViewRect->bottom = mapViewRect->top + h;

	return TRUE;
}

//---------------------------------------------------------------------------
//
//	RadarMap::Setup
//
//---------------------------------------------------------------------------
void RadarMap::Setup(void)
{
	CalculateMetrics();

	RenderMap(m_mapSurface);

	if (g_tiledMap) {
		RECT *realMapViewRect = g_tiledMap->GetMapViewRect();
		m_mapViewRect = *realMapViewRect;
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::Update
//
//---------------------------------------------------------------------------
void RadarMap::Update( void )
{

	m_mapSize = g_theWorld->GetSize();

	RenderMap(m_mapSurface);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RedrawTile
//
//---------------------------------------------------------------------------
//  - Refreshes the part of the radar map from and around the given point
//
//---------------------------------------------------------------------------
void RadarMap::RedrawTile( const MapPoint *point )
{
	Player *player = GetVisiblePlayerToRender();
	if(!player)
		return;

	// modify with the offset values
    MapPoint offsetpos = PosWorldToPosRadar( *point);
    RenderTile(m_tempSurface, offsetpos, *point, player);

	fRect sRect, dRect;
	sint32 x0 = (offsetpos.y + 2 * offsetpos.x) % (2 * m_mapSize->x);




	float adjust = m_filter ? 0.5 : 0.0;
	if (x0 == 2 * m_mapSize->x - 1)
	{

		sRect.left = x0 - adjust;
		sRect.right = x0 + 1;
		sRect.top = offsetpos.y - adjust;
		sRect.bottom = offsetpos.y + 1 + adjust;

		if (sRect.top < 0) sRect.top = 0;
		if (sRect.bottom > m_tempSurface->Height()) sRect.bottom = m_tempSurface->Height();
		dRect.left = sRect.left * (m_tilePixelWidth/2);
		dRect.right = sRect.right * (m_tilePixelWidth/2);
		dRect.top = sRect.top * m_tilePixelHeight;
		dRect.bottom = sRect.bottom * m_tilePixelHeight;

		primitives_Scale16(m_tempSurface, m_mapSurface, sRect, dRect, m_filter);

		sRect.left = 0;
		sRect.right = 1.0 + adjust;
		dRect.left = 0;
		dRect.right = sRect.right * (m_tilePixelWidth/2);

		primitives_Scale16(m_tempSurface, m_mapSurface, sRect, dRect, m_filter);
	}
	else
	{

		sRect.left = x0 - adjust;
		sRect.right = x0 + 2 + adjust;
		sRect.top = offsetpos.y - adjust;
		sRect.bottom = offsetpos.y + 1 + adjust;
		if (sRect.bottom > m_tempSurface->Height()) sRect.bottom = m_tempSurface->Height();
		if (sRect.right > m_tempSurface->Width()) sRect.right = m_tempSurface->Width();
		if (sRect.left < 0) sRect.left = 0;
		if (sRect.top < 0) sRect.top = 0;

		dRect.left = sRect.left * (m_tilePixelWidth/2);
		dRect.right = sRect.right * (m_tilePixelWidth/2);
		dRect.top = sRect.top * m_tilePixelHeight;
		dRect.bottom = sRect.bottom * m_tilePixelHeight;

		primitives_Scale16(m_tempSurface, m_mapSurface, sRect, dRect, m_filter);
	}

	RenderTileBorder(m_mapSurface, offsetpos, *point, player);
	RenderTrade(m_mapSurface, offsetpos, *point, player);

	if (m_filter)
	{
		MapPoint neighbor;
		if (point->GetNeighborPosition(NORTHEAST, neighbor))
				RenderTileBorder(m_mapSurface, PosWorldToPosRadar(neighbor),neighbor, player);
		if (point->GetNeighborPosition(NORTHWEST, neighbor))
				RenderTileBorder(m_mapSurface, PosWorldToPosRadar(neighbor),neighbor, player);
		if (point->GetNeighborPosition(EAST, neighbor))
				RenderTileBorder(m_mapSurface, PosWorldToPosRadar(neighbor),neighbor, player);
		if (point->GetNeighborPosition(WEST, neighbor))
				RenderTileBorder(m_mapSurface, PosWorldToPosRadar(neighbor),neighbor, player);
		if (point->GetNeighborPosition(SOUTHEAST, neighbor))
				RenderTileBorder(m_mapSurface, PosWorldToPosRadar(neighbor),neighbor, player);
		if (point->GetNeighborPosition(SOUTHWEST, neighbor))
				RenderTileBorder(m_mapSurface, PosWorldToPosRadar(neighbor),neighbor, player);

	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::DrawThis
//
//---------------------------------------------------------------------------
AUI_ERRCODE RadarMap::DrawThis(aui_Surface *surface, sint32 x,	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface )
		surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	UpdateMap(surface, rect.left, rect.top);

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

//---------------------------------------------------------------------------
//
//	RadarMap::MouseLGrabInside
//
//---------------------------------------------------------------------------
//  - Processes a left mouse click in the radarmap region. Sets therefore
//	  a rectangle which region is seen centered by the point the mouse click
//    was located and call tiledmap methods to refresh the current view.
//---------------------------------------------------------------------------
void RadarMap::MouseLGrabInside(aui_MouseEvent *data)
{

	if(IsDisabled() || !IsInteractive())
		return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	data->position.x -= X();
	data->position.y -= Y();

	RECT mapRect = {0, 0, Width(), Height()};
	if ( !PtInRect(&mapRect, data->position) ) return;

	g_tiledMap->SetSmoothScrollOffsets(0,0);

	sint32		mapWidth, mapHeight;
	g_tiledMap->GetMapMetrics(&mapWidth, &mapHeight);

	sint32 tileY = (sint32) (data->position.y / m_tilePixelHeight);
	sint32 tileX;

	double nudge = m_tilePixelWidth / 2.0;

	if (tileY & 1) {
		tileX = (sint32) ( ceil(((double)(data->position.x - nudge) / m_tilePixelWidth)) );
	}
	else {
		tileX = (sint32) ( ceil((double)data->position.x / m_tilePixelWidth) );
	}

	tileX = (sint32) ((tileX - m_displayOffset[g_selected_item->GetVisiblePlayer()].x
									+ m_mapSize->x) % m_mapSize->x);
	tileY = (sint32) ((tileY - m_displayOffset[g_selected_item->GetVisiblePlayer()].y
									+ m_mapSize->y) % m_mapSize->y);

	sint32 width = m_mapViewRect.right - m_mapViewRect.left;
	sint32 height = m_mapViewRect.bottom - m_mapViewRect.top;

	sint32 offsetX = width / 2;
	sint32 offsetY = height / 2;

	m_mapViewRect.left = tileX - (sint32)offsetX;
	m_mapViewRect.right = m_mapViewRect.left + width;
	m_mapViewRect.top = (tileY - (sint32)offsetY) & ~0x01;
	m_mapViewRect.bottom = m_mapViewRect.top + height;

	RECT *mapViewRect = &m_mapViewRect;
	RECT *realMapViewRect = g_tiledMap->GetMapViewRect();
	*realMapViewRect = *mapViewRect;

	g_tiledMap->Refresh();
	g_tiledMap->InvalidateMap();
}

//---------------------------------------------------------------------------
//
//	RadarMap::MouseRGrabInside
//
//---------------------------------------------------------------------------
//	- Handling of a right mouseclick over the radar map
//
//---------------------------------------------------------------------------
void RadarMap::MouseRGrabInside(aui_MouseEvent *data)
{

	if(IsDisabled() || !IsInteractive())
		return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	data->position.x -= X();
	data->position.y -= Y();

	sint32 nrplayer = g_selected_item->GetVisiblePlayer();

	// compute the offsets after the MouseRClick to center the map with the
	// desired point

	if (g_theWorld->IsXwrap()) {
		m_displayOffset[nrplayer].x  =
			(m_mapSize->x - ( m_mapSize->x * data->position.x / m_width) + (m_mapSize->x / 2)
			  + m_mapSize->x + m_displayOffset[nrplayer].x) % m_mapSize->x;
	} else {
		m_displayOffset[nrplayer].x = 0;
	}
	if (g_theWorld->IsYwrap()) {
		m_displayOffset[nrplayer].y  =
			(m_mapSize->y - ((( m_mapSize->y * data->position.y / m_height) >>1)<<1)
			  + (m_mapSize->y / 2) + m_mapSize->y + m_displayOffset[nrplayer].y) % m_mapSize->y;
	} else {
		m_displayOffset[nrplayer].y = 0;
	}

	RenderMap(m_mapSurface);

}


//---------------------------------------------------------------------------
//
// RadarMap::Idle
//
//---------------------------------------------------------------------------
//  - Constantly called in the idle time
//
//---------------------------------------------------------------------------
AUI_ERRCODE RadarMap::Idle( void )
{
	static uint32 lastDraw = 0;
	if (GetTickCount() - lastDraw > 100) lastDraw = GetTickCount();
	else return AUI_ERRCODE_OK;

	DrawThis(NULL, 0, 0);

	return AUI_ERRCODE_OK;
}

//---------------------------------------------------------------------------
//
// RadarMap::MapOffset
//
//---------------------------------------------------------------------------
// - Gives Back a MapPoint that is modified  with the current X and Y
//   offsets
//
//---------------------------------------------------------------------------
MapPoint RadarMap::MapOffset(MapPoint oldPoint)
{
	MapPoint newPoint;

	newPoint.x =
		(oldPoint.x + m_displayOffset[g_selected_item->GetVisiblePlayer()].x) % m_mapSize->x;

	newPoint.y =
		(oldPoint.y + m_displayOffset[g_selected_item->GetVisiblePlayer()].y) % m_mapSize->y;

	return newPoint;
}

//---------------------------------------------------------------------------
//
// RadarMap::PosWorldToPosRadarCorrectedX
//
//---------------------------------------------------------------------------
// - Calculates from a given world point the coresponding point of the
//   radar map for handling the offset of the X and/or the Y axis
//
//---------------------------------------------------------------------------
MapPoint RadarMap::PosWorldToPosRadar(MapPoint worldpos)
{
	MapPoint posRadar;
	sint32 nrplayer = g_selected_item->GetVisiblePlayer();

	posRadar.x = (worldpos.x - m_displayOffset[nrplayer].y/2 + m_mapSize->x
					+ m_displayOffset[nrplayer].x) % m_mapSize->x;

	posRadar.y = (worldpos.y + m_displayOffset[nrplayer].y) % m_mapSize->y;

	return posRadar;
}
