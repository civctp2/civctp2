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
//   the actual cell owner. - Nov 1st 2004 - Martin Gühmann
// - Radar tile boarder is now fully determined by the visible tile onwer
//   instead of being determined half by the actual tile owner and half by the
//   the the visible tile owner this fixes the bug that appears after conquest
//   of a city. - Nov. 1st 2004 - Martin Gühmann
// - The radar map now shows the current terrain and the current units and
//   cities if fog of war is off, otherwise it only displays the kind of
//   information it should display. - Dec. 25th 2004 - Martin Gühmann
// - Borders on the minimap are now shown if fog of war is off or god mode
//   is on, even if the there is no contact to that civilisation.
//   - Mar. 4th 2005 Martin Gühmann
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added political map functionality (6-Jul-2009 EPW)
// - Added View capitol on minimap (5-Jan-10 EPW)
//----------------------------------------------------------------------------

#include "c3.h"
#include "colorset.h"               // g_colorSet
#include "radarmap.h"

#include <algorithm>

#include "aui.h"
#include "aui_Factory.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_action.h"
#include "c3ui.h"
#include "player.h"                 // g_player
#include "World.h"                  // g_theWorld
#include "Cell.h"
#include "UnseenCell.h"
#include "citydata.h"
#include "Unit.h"
#include "UnitData.h"
#include "pixelutils.h"
#include "SelItem.h"                // g_selected_item
#include "tiledmap.h"               // g_tiledMap
#include "director.h"
#include "maputils.h"
#include "primitives.h"
#include "profileDB.h"              // g_theProfileDB
#include "pointerlist.h"
#include "terrainutil.h"
#include "Scheduler.h"

extern C3UI				*g_c3ui;
extern PointerList<Player> *g_deadPlayer;

extern sint32 g_fog_toggle;
extern sint32 g_god;

static const WORLD_DIRECTION NEIGHBOR_DIRECTIONS[] = { EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST };

static const unsigned char k_EAST_BORDER_FLAG		= 1 << EAST;
static const unsigned char k_WEST_BORDER_FLAG		= 1 << WEST;
static const unsigned char k_NORTH_EAST_BORDER_FLAG	= 1 << NORTHEAST;
static const unsigned char k_NORTH_WEST_BORDER_FLAG	= 1 << NORTHWEST;
static const unsigned char k_SOUTH_EAST_BORDER_FLAG	= 1 << SOUTHEAST;
static const unsigned char k_SOUTH_WEST_BORDER_FLAG	= 1 << SOUTHWEST;

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
	delete[] m_mapOverlay;
	delete m_mapSurface;
	delete m_tempSurface;
	if (m_tempBuffer)
		free(m_tempBuffer);

	free(m_colorMap);
}

//---------------------------------------------------------------------------
//
//	RadarMap::InitCommonLdl
//
//---------------------------------------------------------------------------
void RadarMap::InitCommonLdl(const MBCHAR *ldlBlock)
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
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
	m_mapSurface = NULL;
	m_mapSize = NULL;
	m_tempSurface = NULL;
	m_tempBuffer = NULL;

	m_colorMap = NULL;

	m_tilePixelWidth = 0.0;
	m_tilePixelHeight = 0.0;

	m_displayUnits = g_theProfileDB->GetDisplayUnits() != FALSE;
	m_displayCities = g_theProfileDB->GetDisplayCities() != FALSE;
	m_displayBorders = g_theProfileDB->GetDisplayBorders() != FALSE;
	m_displayOverlay = true;
	m_filter = g_theProfileDB->GetDisplayFilter() != FALSE;
	m_displayTrade = g_theProfileDB->GetDisplayTrade() != FALSE;
	m_displayTerrain = g_theProfileDB->GetDisplayTerrain() != FALSE;
	m_displayPolitical = g_theProfileDB->GetDisplayPolitical() != FALSE;
	m_displayCapitols = g_theProfileDB->GetDisplayCapitols() != FALSE;
	m_displayRelations = g_theProfileDB->GetDisplayRelations() != FALSE;

	m_mapOverlay = NULL;

	MapPoint resetPos (0,0);
	m_lastCenteredPoint = resetPos;

	m_isInteractive = true;

	m_selectedCity.m_id = 0;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_mapSurface = aui_Factory::new_Surface(errcode, m_width, m_height);
	Assert(AUI_NEWOK(m_mapSurface, errcode));

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
	delete [] m_mapOverlay;
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
void RadarMap::SetMapOverlayCell(MapPoint const & pos, COLOR color)
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
	AUI_ERRCODE		errcode = aui_Region::Resize(width, height);
	Assert(errcode == AUI_ERRCODE_OK);

	delete m_mapSurface;
	m_mapSurface = aui_Factory::new_Surface(errcode, width, height);
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

	delete m_tempSurface;
	if (m_tempBuffer)
		free(m_tempBuffer);

	free(m_colorMap);
	m_colorMap = NULL;

	m_mapSize = g_theWorld->GetSize();

	m_tilePixelWidth = ((double )m_width) / m_mapSize->x;
	m_tilePixelHeight = ((double )m_height) / m_mapSize->y;

	uint32 width = m_mapSize->x * 2;
	uint32 height = m_mapSize->y;

	m_tempBuffer = (uint8 *)calloc((width + 2) * (height + 2), 2);

	m_colorMap  = (Pixel16*) malloc((width + 2) * (height + 2) * sizeof(Pixel16));

	AUI_ERRCODE err;
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
	sint32		k       = ((y / 2) + x) % m_mapSize->x;

    POINT		pt;
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
//	RadarMap::RadarTileBorderColor
//
//---------------------------------------------------------------------------
//	- Checks which color a border must be drawn for the current tile
//
//---------------------------------------------------------------------------
Pixel16 RadarMap::RadarTileBorderColor(const MapPoint &position, const Player & player)
{
	sint32 owner = g_tiledMap->GetVisibleCellOwner(position);
	if(owner < 0)
		return(g_colorSet->GetDarkColor(COLOR_WHITE));

	if(m_displayRelations)
		return g_colorSet->GetColor(RadarTileRelationsColor(position, player));
	else
		return(g_colorSet->GetPlayerColor(owner));
}

COLOR RadarMap::RadarTileRelationsColor(const MapPoint & position, const Player & player, sint32 unitOwner)
{
	Assert(m_displayRelations);

	COLOR color = COLOR_WHITE;

	sint32 owner = unitOwner < 0 ? g_tiledMap->GetVisibleCellOwner(position) : unitOwner;
	if (owner >= 0) {
		if (player.m_owner == owner || player.HasAllianceWith(owner)) {
			color = COLOR_BLUE;
		} else if (player.HasWarWith(owner)) {
			color = COLOR_RED;
		} else if (player.HasPeaceTreatyWith(owner) || player.HasAnyPactWith(owner)) {
			color = COLOR_GREEN;
		} else if (player.HasContactWith(owner)) {
			color = COLOR_YELLOW;
		}
	}
	return color;
}

//---------------------------------------------------------------------------
//
//	RadarMap::RadarTileBorder
//
//---------------------------------------------------------------------------
//	- Checks which borders must be drawn for the current tile
//
//---------------------------------------------------------------------------
uint8 RadarMap::RadarTileBorder(const Player & player, const MapPoint &position)
{
	uint8 borderFlags = 0;
	if (!m_displayBorders) {
		return borderFlags;
	}

	if (!player.IsExplored(position)) {
		return borderFlags;
	}

	// Added by Martin Gühmann
	sint32 owner = g_tiledMap->GetVisibleCellOwner(const_cast<MapPoint&>(position));
	if (owner < 0) {
		return borderFlags;
	}

	if (owner != player.m_owner
			&& !player.m_hasGlobalRadar
			&& !Scheduler::CachedHasContactWithExceptSelf(player.m_owner, owner)
			&& !g_fog_toggle // Don't forget if fog of war is off
			&& !g_god)
	{
		return borderFlags;
	}

	for (auto neighborDirection : NEIGHBOR_DIRECTIONS)
	{
		MapPoint neighborPosition;
		if (position.GetNeighborPosition(neighborDirection, neighborPosition) &&
			(g_tiledMap->GetVisibleCellOwner(neighborPosition) != owner))
		{
			borderFlags |= (1 << neighborDirection);
		}
	}

	return borderFlags;
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderTradeRoute
//
//---------------------------------------------------------------------------
//	- Draws a trade route sign for the current tile
//
//---------------------------------------------------------------------------
void RadarMap::RenderTradeRoute(aui_Surface & surface, const RECT & tileRectangle)
{
	sint32 midX = (tileRectangle.left + tileRectangle.right) / 2;
	sint32 midY = (tileRectangle.top + tileRectangle.bottom) / 2;
	RECT tradeRect = { midX, midY, midX, midY };

	primitives_ClippedPaintRect16(surface, tradeRect, g_colorSet->GetColor(COLOR_YELLOW));
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderCapitol
//
//---------------------------------------------------------------------------
//	- Draws a Capitol marker for the current tile
//
//---------------------------------------------------------------------------
void RadarMap::RenderCapitol(aui_Surface & surface, const MapPoint & position, const MapPoint & worldPos,
		const Player & player)
{
	if (!m_displayCapitols || !player.IsExplored(worldPos)) {
		return;
	}

	Unit unit;
	if (!g_theWorld->GetTopVisibleUnit(worldPos, unit) || !g_theWorld->GetTopRadarUnit(worldPos, unit)) {
		return;
	}

	if (!unit.IsValid() || !unit.IsCity() || !unit.IsCapitol()) {
		return;
	}

	MapPoint screenPosition(((worldPos.y / 2) + position.x) % (m_mapSize->x), position.y);
	double xPosition = screenPosition.x * m_tilePixelWidth;
	double yPosition = screenPosition.y * m_tilePixelHeight;

	if (screenPosition.y & 1) {
		xPosition += m_tilePixelWidth / 2.0;
	}

	//Now to build the "star"
	RECT vertical = {
		xPosition - m_tilePixelWidth, yPosition,
		xPosition + 2 * m_tilePixelWidth - 1, yPosition + m_tilePixelHeight - 1
	};

	RECT horizontal = {
		xPosition, yPosition - m_tilePixelHeight,
		xPosition + m_tilePixelWidth - 1, yPosition + 2 * m_tilePixelHeight - 1
	};

	primitives_ClippedPaintRect16(surface, vertical, g_colorSet->GetColor(COLOR_ORANGE));
	primitives_ClippedPaintRect16(surface, horizontal, g_colorSet->GetColor(COLOR_ORANGE));
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
void RadarMap::RenderMapTileBorder(aui_Surface & surface, const MapPoint & position, uint8 borderFlags,
                                   Pixel16 borderColor)
{
	double xPosition = position.x * m_tilePixelWidth;
	double yPosition = position.y * m_tilePixelHeight;
	if (position.y & 1) {
		xPosition += m_tilePixelWidth / 2.0;
	}

	RECT tileRectangle = { xPosition, yPosition, xPosition + m_tilePixelWidth, yPosition + m_tilePixelHeight };
	sint32 middleX = xPosition + m_tilePixelWidth / 2.0;

	sint32 eastFlags = borderFlags & (k_EAST_BORDER_FLAG | k_NORTH_EAST_BORDER_FLAG | k_SOUTH_EAST_BORDER_FLAG);
	if (eastFlags == 0) {
		// skip
	} else if (eastFlags == (k_EAST_BORDER_FLAG | k_NORTH_EAST_BORDER_FLAG | k_SOUTH_EAST_BORDER_FLAG)) {
		primitives_ClippedLine16(surface, middleX, tileRectangle.top, middleX, tileRectangle.bottom, borderColor);
	} else if (eastFlags == k_EAST_BORDER_FLAG) {
		primitives_ClippedLine16(surface, tileRectangle.right, tileRectangle.top, tileRectangle.right,
				tileRectangle.bottom, borderColor);
	} else if (eastFlags == (k_EAST_BORDER_FLAG | k_NORTH_EAST_BORDER_FLAG)) {
		primitives_ClippedLine16(surface, middleX, tileRectangle.top, tileRectangle.right, tileRectangle.bottom,
				borderColor);
	} else if (eastFlags == (k_EAST_BORDER_FLAG | k_SOUTH_EAST_BORDER_FLAG)) {
		primitives_ClippedLine16(surface, tileRectangle.right, tileRectangle.top, middleX, tileRectangle.bottom,
				borderColor);
	} else if (eastFlags == k_NORTH_EAST_BORDER_FLAG) {
		primitives_ClippedLine16(surface, middleX, tileRectangle.top, tileRectangle.right, tileRectangle.top,
				borderColor);
	} else if (eastFlags == k_SOUTH_EAST_BORDER_FLAG) {
		primitives_ClippedLine16(surface, middleX, tileRectangle.bottom, tileRectangle.right, tileRectangle.bottom,
				borderColor);
	}

	sint32 westFlags = borderFlags & (k_WEST_BORDER_FLAG | k_NORTH_WEST_BORDER_FLAG | k_SOUTH_WEST_BORDER_FLAG);
	if (westFlags == 0) {
		// skip
	} else if (westFlags == (k_WEST_BORDER_FLAG | k_NORTH_WEST_BORDER_FLAG | k_SOUTH_WEST_BORDER_FLAG)) {
		primitives_ClippedLine16(surface, middleX, tileRectangle.top, middleX, tileRectangle.bottom, borderColor);
	} else if (westFlags == k_WEST_BORDER_FLAG) {
		primitives_ClippedLine16(surface, tileRectangle.left, tileRectangle.top, tileRectangle.left,
				tileRectangle.bottom, borderColor);
	} else if (westFlags == (k_WEST_BORDER_FLAG | k_NORTH_WEST_BORDER_FLAG)) {
		primitives_ClippedLine16(surface, tileRectangle.left, tileRectangle.bottom, middleX, tileRectangle.top,
				borderColor);
	} else if (westFlags == (k_WEST_BORDER_FLAG | k_SOUTH_WEST_BORDER_FLAG)) {
		primitives_ClippedLine16(surface, tileRectangle.left, tileRectangle.top, middleX, tileRectangle.bottom,
				borderColor);
	} else if (westFlags == k_SOUTH_WEST_BORDER_FLAG) {
		primitives_ClippedLine16(surface, tileRectangle.left, tileRectangle.bottom, middleX, tileRectangle.bottom,
				borderColor);
	} else if (westFlags == k_NORTH_WEST_BORDER_FLAG) {
		primitives_ClippedLine16(surface, tileRectangle.left, tileRectangle.top, middleX, tileRectangle.top,
				borderColor);
	}
}

void RadarMap::RenderTiles(aui_Surface & surface)
{
	double tileWidth = m_width / static_cast<double>(m_mapSize->x);
	double tileHeight = m_height / static_cast<double>(m_mapSize->y);

	MapPoint worldPos(0, 0);
	MapPoint radarOffset = PosWorldToPosRadar(worldPos);
	bool     oddY        = radarOffset.y & 1;
	double   startX      = (radarOffset.x + (radarOffset.y / 2)) * tileWidth + (oddY ? (tileWidth / 2.0) : 0.0);
	double   y           = radarOffset.y * tileHeight;

	sint32    colorMapPitch = m_mapSize->x + 2;
	Pixel16 * colorMap      = m_colorMap + 1 + colorMapPitch;
	Pixel16 * endColorMap   = colorMap + m_mapSize->y * colorMapPitch;
	for (; colorMap < endColorMap; colorMap += colorMapPitch, worldPos.y++)
	{
		if (y > (m_height - 1)) {
			y -= m_height;
		}

		sint32 y1 = static_cast<sint32>(y);
		y += tileHeight;
		sint32 y2 = static_cast<sint32>(y);

		double x = startX;
		worldPos.x = 0;

		Pixel16 * endColorMapRow = colorMap + m_mapSize->x;
		for (Pixel16 * colorMapRow = colorMap; colorMapRow < endColorMapRow; colorMapRow++, worldPos.x++)
		{
			bool isLand = (g_theWorld->IsLand(worldPos) || g_theWorld->IsMountain(worldPos))
					&& !g_theWorld->IsTunnel(worldPos);

			if (x > (m_width - 1)) { // - 1 to prevent rounding errors
				x -= m_width;
			}

			sint32 x1 = static_cast<sint32>(x);
			x += tileWidth;
			sint32 x2 = static_cast<sint32>(x);
			if ((x1 <= tileWidth) && oddY) { // first row, render also crossing before
				RenderTileCrossing(surface, RECT { 0, y1, x1 - 1, y2 - 1 }, colorMapRow - 1, isLand);
			}
			RenderTile(surface, RECT { x1, y1, x2, y2 }, colorMapRow, isLand);
		}

		oddY = !oddY;
		startX += (tileWidth / 2.0);
		if (startX > (m_width - 1)) { // - 1 to prevent rounding errors
			startX -= m_width;
		}
	}
}

void RadarMap::RenderTile(aui_Surface & surface, const RECT & tileRectangle, Pixel16 * colorMap, bool isLand)
{
	sint32 split = (tileRectangle.left + tileRectangle.right - 1) / 2;
	RECT renderRect = { tileRectangle.left, tileRectangle.top, split, tileRectangle.bottom - 1 };
	primitives_ClippedPaintRect16(surface, renderRect, *colorMap);

	renderRect.left  = renderRect.right + 1;
	renderRect.right = tileRectangle.right - 1;
	RenderTileCrossing(surface, renderRect, colorMap, isLand);
}

void RadarMap::RenderTileCrossing(aui_Surface & surface, const RECT & rect, Pixel16 * colorMap, bool isLand)
{
	const uint8 alpha = 128;
	Pixel16 current = *colorMap;
	Pixel16 above   = *(colorMap + 1 - (m_mapSize->x + 2));
	Pixel16 right   = *(colorMap + 1);
	Pixel16 below   = *(colorMap + (m_mapSize->x + 2));

	if ((current == above) && (above == right) && (right == below)) { // all same color
		primitives_ClippedPaintRect16(surface, rect, current);
	}
	else if ((current == above && current == right) || (current == right && current == below)
			|| (current == above && current == below)) // two others same as current
	{
		primitives_ClippedPaintRect16(surface, rect, current);
		if (current != above) {
			primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.top, rect.right, rect.top },
					above, alpha);
		} else if (current != right) {
			primitives_ClippedPaintRect16(surface, RECT {rect.right, rect.top, rect.right, rect.bottom },
					right, alpha);
		} else { // below differs
			primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.bottom, rect.right, rect.bottom },
					below, alpha);
		}
	}
	else if ((above == right) && (right == below)) // three same colors, different from current
	{
		primitives_ClippedPaintRect16(surface, rect, above);
		primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.top, rect.left, rect.bottom },
				current, alpha);
	}
	else if (current == above) // two same colors
	{
		primitives_ClippedPaintRect16(surface, rect, current);
		if (right == below) {
			primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, right);
		}
		else
		{
			primitives_ClippedPaintRect16(surface, RECT {rect.right, rect.top, rect.right, rect.bottom},
					right, alpha);
			primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.bottom, rect.left, rect.bottom},
					below, alpha);
		}
	}
	else if (current == below) // two same colors
	{
		primitives_ClippedPaintRect16(surface, rect, current);
		if (above == right) {
			primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, above);
		}
		else
		{
			primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.top, rect.right, rect.top },
					above, alpha);
			primitives_ClippedPaintRect16(surface, RECT {rect.right, rect.top, rect.right, rect.bottom },
					right, alpha);
		}
	}
	else if (above == right) // two same colors, current != below as is already tested
	{
		primitives_ClippedPaintRect16(surface, rect, above);
		primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.top, rect.left, rect.bottom },
				current, alpha);
		primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.bottom, rect.right, rect.bottom },
				below, alpha);
	}
	else if (right == below) // two same colors, current != above as is already tested
	{
		primitives_ClippedPaintRect16(surface, rect, right);
		primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.top, rect.left, rect.bottom },
				current, alpha);
		primitives_ClippedPaintRect16(surface, RECT {rect.left, rect.top, rect.right, rect.top },
				above, alpha);
	}
	else if (current == right) // two same colors, opposite
	{
		if (isLand || (above != below))
		{
			primitives_ClippedPaintRect16(surface, rect, current);
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.top, rect.right, rect.top },
					above, alpha);
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.bottom, rect.right, rect.bottom },
					below, alpha);
		}
		else { // not isLand and above == below
			primitives_ClippedPaintRect16(surface, rect, above);
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.top, rect.left, rect.bottom },
					current, alpha);
			primitives_ClippedPaintRect16(surface, RECT{ rect.right, rect.top, rect.right, rect.bottom },
					right, alpha);
		}
	}
	else if (above == below) // two same colors, opposite
	{
		primitives_ClippedPaintRect16(surface, rect, above);
		primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.top, rect.left, rect.bottom },
				current, alpha);
		primitives_ClippedPaintRect16(surface, RECT { rect.right, rect.top, rect.right, rect.bottom },
				right, alpha);
	}
	else // four different colors
	{
		if (isLand)
		{
			primitives_ClippedPaintRect16(surface, rect, current);
			primitives_ClippedPaintRect16(surface, RECT{ rect.right, rect.top, rect.right, rect.bottom },
					right, alpha);
		}
		else // arbitrary choice to give preference to right over above and bottom
		{
			primitives_ClippedPaintRect16(surface, rect, right);
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.top, rect.left, rect.bottom },
					current, alpha);
		}
		primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.top, rect.right, rect.top },
				above, alpha);
		primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.bottom, rect.right, rect.bottom },
				below, alpha);
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderTileBorder
//
//---------------------------------------------------------------------------
//	- Controls the rendering of the borders for the current radar map
//    position
//---------------------------------------------------------------------------
void RadarMap::RenderTileBorder(aui_Surface & surface, const MapPoint & position, const MapPoint & worldPos,
		const Player & player)
{
	uint8 borderFlags = RadarTileBorder(player, worldPos);

	if (borderFlags)
	{
		Pixel16 borderColor = RadarTileBorderColor(worldPos, player);
		sint32 x = ((position.y / 2) + position.x) % m_mapSize->x;
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
void RadarMap::RenderTrade(aui_Surface & surface, const MapPoint & position, const MapPoint & worldPos,
		const Player & player)
{
	if (!m_displayTrade || !player.IsExplored(worldPos)) {
		return;
	}

	bool seenTrade= false;
	for (sint32 i = 0; i < g_theWorld->GetCell(worldPos)->GetNumTradeRoutes(); i++) {
		TradeRoute route = g_theWorld->GetCell(worldPos)->GetTradeRoute(i);
		if (route.SeenBy(g_selected_item->GetVisiblePlayer())) {
			seenTrade= true;
			break;
		}
	}

	if (!seenTrade) {
		return;
	}

	MapPoint screenPosition(((worldPos.y / 2) + position.x) % (m_mapSize->x), position.y);
	double xPosition = screenPosition.x * m_tilePixelWidth;
	double yPosition = screenPosition.y * m_tilePixelHeight;

	if (screenPosition.y & 1) {
		xPosition += m_tilePixelWidth / 2.0;
	}

	RECT tileRectangle = { xPosition, yPosition, xPosition + m_tilePixelWidth, yPosition + m_tilePixelHeight };
	RenderTradeRoute(surface, tileRectangle);
}

Pixel16 RadarMap::MapTileColor(const Player & player, const MapPoint & position, const MapPoint & worldPos)
{
	Unit unit;

	bool  darken = false;
	COLOR color  = COLOR_BLACK;

	if (player.IsExplored(worldPos))
	{
		sint32 owner = g_theWorld->GetOwner(worldPos);

		if (m_displayOverlay && m_mapOverlay) {
			color = m_mapOverlay[worldPos.y * m_mapSize->x + worldPos.x];
		}
		else if (m_displayCities && g_tiledMap->HasVisibleCity(worldPos)) {
			color = COLOR_WHITE;
		}
		else if (m_displayUnits
				&& (g_theWorld->GetTopVisibleUnit(worldPos, unit) || g_theWorld->GetTopRadarUnit(worldPos, unit)))
		{
			if (m_displayRelations) {
				color = RadarTileRelationsColor(worldPos, player, unit.GetOwner());
			}
			else {
				color = g_colorSet->ComputePlayerColor(unit.GetOwner());
			}
		}
		else if (m_displayPolitical && !g_theWorld->IsWater(worldPos) )
		{
			if (owner < 0)
			{
				darken = true;
				color  = COLOR_WHITE;
			}
			else if (m_displayRelations) {
				color = RadarTileRelationsColor(worldPos, player);
			}
			else {
				color = g_colorSet->ComputePlayerColor(g_tiledMap->GetVisibleCellOwner(worldPos));
			}
		}
		else if (m_displayTerrain) {
			color = static_cast<COLOR>(COLOR_TERRAIN_0 + g_tiledMap->GetVisibleTerrainType(worldPos));
		}
		else
		{
			if ((g_theWorld->IsLand(worldPos) || g_theWorld->IsMountain(worldPos)) && !g_theWorld->IsTunnel(worldPos))
			{
				color = static_cast<COLOR>(COLOR_TERRAIN_0 + TERRAIN_GRASSLAND);
			}
			else
			{
				color = static_cast<COLOR>(COLOR_TERRAIN_0 + TERRAIN_WATER_DEEP);
			}
		}
	} else if (g_theWorld->GetTopRadarUnit(worldPos, unit)) {
		color = g_colorSet->ComputePlayerColor(unit.GetOwner());
	}

	return darken ? g_colorSet->GetDarkColor(color) : g_colorSet->GetColor(color);
}

void RadarMap::FillColorMapBorders()
{
	sint32 pitch = m_mapSize->x + 2;

	bool xWrap = g_theWorld->IsXwrap();
	bool yWrap = g_theWorld->IsYwrap();

	// Fill top- and bottom-border
	Pixel16 * topColor = m_colorMap + 1;
	Pixel16 * bottomColor = topColor + pitch * (m_mapSize->y + 1);
	Pixel16 * endTopColor = topColor + m_mapSize->x;
	for ( ; topColor < endTopColor; topColor++, bottomColor++)
	{
		if (yWrap)
		{
			*topColor = *(bottomColor - pitch);
			*bottomColor = *(topColor + pitch);
		}
		else
		{
			*topColor = *(topColor + pitch);
			*bottomColor = *(bottomColor - pitch);
		}
	}

	// Fill left- and right-border
	Pixel16 * leftBorder = m_colorMap + pitch;
	Pixel16 * rightBorder = leftBorder + m_mapSize->x + 1;
	Pixel16 * endLeftBorder = leftBorder + m_mapSize->y * pitch;
	for ( ; leftBorder < endLeftBorder; leftBorder += pitch, rightBorder += pitch)
	{
		if (xWrap)
		{
			*leftBorder = *(rightBorder - 1);
			*rightBorder = *(leftBorder + 1);
		}
		else
		{
			*leftBorder = *(leftBorder + 1);
			*rightBorder = *(rightBorder - 1);
		}
	}

	// Corners
	Pixel16 * leftTopCorner     = m_colorMap;
	Pixel16 * rightTopCorner    = leftTopCorner + m_mapSize->x + 1;
	Pixel16 * leftBottomCorner  = leftTopCorner + pitch * (m_mapSize->y + 1);
	Pixel16 * rightBottomCorner = leftBottomCorner + m_mapSize->x + 1;
	if (yWrap && xWrap)
	{
		*leftTopCorner     = *(rightBottomCorner - 1 - pitch);
		*rightTopCorner    = *(leftBottomCorner + 1 - pitch);
		*leftBottomCorner  = *(rightTopCorner - 1 + pitch);
		*rightBottomCorner = *(leftTopCorner + 1 + pitch);
	} else {
		sint32 xOffset = xWrap ? 0 : 1;
		sint32 yOffset = yWrap ? 0 : pitch;

		*leftTopCorner     = *(leftTopCorner + xOffset + yOffset);
		*rightTopCorner    = *(rightTopCorner - xOffset + yOffset);
		*leftBottomCorner  = *(leftBottomCorner + xOffset - yOffset);
		*rightBottomCorner = *(rightBottomCorner - xOffset - yOffset);
	}
}

void RadarMap::RenderMapTileColor(const Player & player)
{
	sint32 pitch = m_mapSize->x + 2;
	Pixel16 * color = m_colorMap + 1 + pitch; // skip border

	MapPoint radarOffset = PosWorldToPosRadar(MapPoint(0, 0));
	for (sint32 y = 0; y < m_mapSize->y; y++)
	{
		Pixel16 * tileColor = color;
		for (sint32 x = 0; x < m_mapSize->x; x++)
		{
			*tileColor++ = MapTileColor(player, radarOffset, MapPoint(x, y));

			radarOffset.x++;
			if (radarOffset.x >= m_mapSize->x) {
				radarOffset.x -= m_mapSize->x;
			}
		}
		color += pitch;

		radarOffset.y++;
		if (radarOffset.y >= m_mapSize->y) {
			radarOffset.y -= m_mapSize->y;
		}
	}

	FillColorMapBorders();
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

	if (!player)
	{
		RECT destRect = { 0, 0, surface->Width(), surface->Height() };
		primitives_PaintRect16(surface, &destRect, g_colorSet->GetColor(COLOR_BLACK));
		return;
	}

	RenderMapTileColor(*player);

	uint8 *pSurfBase;
	surface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
	RenderTiles(*surface);

	MapPoint radarOffset = PosWorldToPosRadar(MapPoint(0, 0));
	for (sint32 y = 0; y < m_mapSize->y; y++)
	{
		for (sint32 x = 0; x < m_mapSize->x; x++)
		{
			const MapPoint worldPos(x,y);
			RenderTileBorder(*surface, radarOffset, worldPos, *player);
			RenderCapitol(*surface, radarOffset, worldPos, *player);
			RenderTrade(*surface, radarOffset, worldPos, *player);

			radarOffset.x++;
			if (radarOffset.x >= m_mapSize->x) {
				radarOffset.x -= 0;
			}
		}

		radarOffset.y++;
		if (radarOffset.y >= m_mapSize->y) {
			radarOffset.y -= 0;
		}
	}

	surface->Unlock(pSurfBase);
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
void RadarMap::RenderViewRect(aui_Surface & surf, sint32 x, sint32 y)
{
	RECT offsetRect = {0, 0, 0, 0};

	if (g_tiledMap)
	{
		m_mapViewRect = g_tiledMap->GetMapViewRect();

		if(!g_tiledMap->ReadyToDraw()) {
			return;
		}

		sint32 nrplayer = g_selected_item->GetVisiblePlayer();

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
            y1 = y3 = std::max<sint32>(0, offsetRect.top);
            y2 = y4 = std::min<sint32>(mapHeight, offsetRect.bottom);
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

	// Draw the rectangles
	Pixel16 lineColor = g_colorSet->GetColor(COLOR_WHITE);
	primitives_ClippedLine16(surf, x1, y1, x2, y1, lineColor);
	primitives_ClippedLine16(surf, x1, y1, x1, y2, lineColor);
	primitives_ClippedLine16(surf, x3, y1, x4, y1, lineColor);
	primitives_ClippedLine16(surf, x4, y1, x4, y2, lineColor);
	primitives_ClippedLine16(surf, x1, y4, x2, y4, lineColor);
	primitives_ClippedLine16(surf, x1, y3, x1, y4, lineColor);
	primitives_ClippedLine16(surf, x3, y4, x4, y4, lineColor);
	primitives_ClippedLine16(surf, x4, y3, x4, y4, lineColor);
}

//---------------------------------------------------------------------------
//
//	RadarMap::UpdateMap
//
//---------------------------------------------------------------------------
void RadarMap::UpdateMap(aui_Surface * surf, sint32 x, sint32 y)
{
	RECT destRect = {x, y, x + Width(), y + Height() };
	RECT srcRect  = {0, 0, Width(), Height()};

	LPVOID surfaceBuffer = NULL;
	if (!surf->Buffer())
	{
		surf->Lock(NULL, &surfaceBuffer, 0);
		Assert(surf->Buffer());
	}

	g_c3ui->TheBlitter()->StretchBlt(surf, &destRect, m_mapSurface, &srcRect, k_AUI_BLITTER_FLAG_COPY);

	if (IsInteractive()) {
		RenderViewRect(*surf, x, y);
	}

	if (surfaceBuffer) {
		surf->Unlock(surfaceBuffer);
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::ComputeCenteredMap
//
//---------------------------------------------------------------------------
MapPoint RadarMap::ComputeCenteredMap(MapPoint const & pos, RECT *viewRect)
{
	LONG const  w   = viewRect->right - viewRect->left;
	LONG const  h   = viewRect->bottom - viewRect->top;

	sint32 tileX;
	maputils_MapX2TileX(pos.x, pos.y, &tileX);

	viewRect->left      = tileX - (w>>1);
	viewRect->top       = (pos.y - (h>>1)) & (~1);
	viewRect->right     = viewRect->left + w;
	viewRect->bottom    = viewRect->top + h;

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
MapPoint RadarMap::CenterMap(MapPoint const & pos)
{
	MapPoint LastPT = m_lastCenteredPoint;
	if(!LastPT.IsValid())
		LastPT = pos;

	m_lastCenteredPoint = pos;

	RECT mapViewRect = g_tiledMap->GetMapViewRect();
	ComputeCenteredMap(pos, &mapViewRect);
	g_tiledMap->UpdateAndClipMapViewRect(mapViewRect);
	m_mapViewRect = g_tiledMap->GetMapViewRect();

	RenderMap(m_mapSurface);

	return LastPT;
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

	if (g_tiledMap)
    {
		m_mapViewRect = g_tiledMap->GetMapViewRect();
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

void RadarMap::DoRedrawTile(const Player & player, const MapPoint & pos)
{
	bool isLand = (g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos)) && !g_theWorld->IsTunnel(pos);

	double tileWidth = m_width / static_cast<double>(m_mapSize->x);
	double tileHeight = m_height / static_cast<double>(m_mapSize->y);

	MapPoint radarOffset = PosWorldToPosRadar(pos);

	Pixel16 * tileColor = m_colorMap + pos.x + 1 + (pos.y + 1) * (m_mapSize->x + 2);
	*tileColor = MapTileColor(player, radarOffset, pos);
	if (pos.x == 0 || pos.x == (m_mapSize->x - 1) || pos.y == 0 || pos.y == (m_mapSize->y - 1)) {
		FillColorMapBorders();
	}

	double x = (radarOffset.x + (radarOffset.y / 2)) * tileWidth + ((radarOffset.y & 1) ? (tileWidth / 2.0) : 0.0);
	if (x > (m_width - 1)) {
		x -= m_width;
	}

	double y = radarOffset.y * tileHeight;
	if (y > (m_height - 1)) {
		y -= m_height;
	}

	sint32 y1 = static_cast<sint32>(y);
	sint32 y2 = static_cast<sint32>(y + tileHeight);

	sint32 x1 = static_cast<sint32>(x);
	sint32 x2 = static_cast<sint32>(x + tileWidth);

	if ((x1 <= tileWidth) && (radarOffset.y & 1)) { // first row, render also crossing before
		RenderTileCrossing(*m_mapSurface, RECT { 0, y1, x1 - 1, y2 - 1 }, tileColor - 1, isLand);
	}
	RECT tileRect = { x1, y1, x2, y2 };
	RenderTile(*m_mapSurface, tileRect, tileColor, isLand);

	RenderTileBorder(*m_mapSurface, radarOffset, pos, player);
	RenderCapitol(*m_mapSurface, radarOffset, pos, player);
	RenderTrade(*m_mapSurface, radarOffset, pos, player);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RedrawTile
//
//---------------------------------------------------------------------------
//  - Refreshes the part of the radar map from and around the given point
//
//---------------------------------------------------------------------------
void RadarMap::RedrawTile(const MapPoint & pos)
{
	Player * player = GetVisiblePlayerToRender();
	if (!player) {
		return;
	}

	uint8 *pSurfBase;
	m_mapSurface->Lock(NULL, (LPVOID *)&pSurfBase, 0);

	DoRedrawTile(*player, pos);

	// Draw neighbor crossings
	MapPoint neighborPosition;
	if (pos.GetNeighborPosition(NORTHWEST, neighborPosition)) {
		DoRedrawTile(*player, neighborPosition);
	}
	if (pos.GetNeighborPosition(WEST, neighborPosition)) {
		DoRedrawTile(*player, neighborPosition);
	}
	if (pos.GetNeighborPosition(SOUTHWEST, neighborPosition)) {
		DoRedrawTile(*player, neighborPosition);
	}
	m_mapSurface->Unlock(pSurfBase);
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

	sint32  tileY   = (sint32) (data->position.y / m_tilePixelHeight);
    double  nudge   = (tileY & 1) ? m_tilePixelWidth / 2.0 : 0.0;
    sint32  tileX   = (sint32) ( ceil(((double)(data->position.x - nudge) / m_tilePixelWidth)) );

	tileX = (sint32) ((tileX - m_displayOffset[g_selected_item->GetVisiblePlayer()].x
									+ m_mapSize->x) % m_mapSize->x);
	tileY = (sint32) ((tileY - m_displayOffset[g_selected_item->GetVisiblePlayer()].y
									+ m_mapSize->y) % m_mapSize->y);

	sint32 width = m_mapViewRect.right - m_mapViewRect.left;
	sint32 height = m_mapViewRect.bottom - m_mapViewRect.top;

	m_mapViewRect.left = tileX - (width / 2);
	m_mapViewRect.right = m_mapViewRect.left + width;
	m_mapViewRect.top = (tileY - (height / 2)) & ~0x01;
	m_mapViewRect.bottom = m_mapViewRect.top + height;

	g_tiledMap->UpdateAndClipMapViewRect(m_mapViewRect);
	m_mapViewRect = g_tiledMap->GetMapViewRect();

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
// RadarMap::PosWorldToPosRadarCorrectedX
//
//---------------------------------------------------------------------------
// - Calculates from a given world point the coresponding point of the
//   radar map for handling the offset of the X and/or the Y axis
//
//---------------------------------------------------------------------------
MapPoint RadarMap::PosWorldToPosRadar(MapPoint worldPos)
{
	sint32 nrplayer = g_selected_item->GetVisiblePlayer();

	MapPoint posRadar;
	posRadar.x = (worldPos.x - m_displayOffset[nrplayer].y / 2 + m_mapSize->x
	              + m_displayOffset[nrplayer].x) % m_mapSize->x;

	posRadar.y = (worldPos.y + m_displayOffset[nrplayer].y) % m_mapSize->y;

	return posRadar;
}
