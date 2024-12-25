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
// - #01 Allow shifting the X and Y axis in the radar map with RMouse clicks
//   (L. Hirth 6/2004)
// - Standardised ceil/min/max usage.
// - Radar tile border color determined by the visual cell owner instead by
//   the actual cell owner. - Nov 1st 2004 - Martin Gühmann
// - Radar tile border is now fully determined by the visible tile owner
//   instead of being determined half by the actual tile owner and half by
//   the visible tile owner this fixes the bug that appears after conquest
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

extern C3UI * g_c3ui;

extern sint32 g_fog_toggle;
extern sint32 g_god;

/*
 * Radar map rendering method
 *
 * Due to the diamond shape of the tiles two elements need to be rendered (see picture):
 * 1. the basic tile (-)
 * 2. the crossing between the tiles (x)
 * where the basic tile always gets the color of the tile, and the crossing will get its colors from the surrounding
 * basic tiles (see method RenderCrossing). As coastal tiles are not only water yet also contain a piece of land, they
 * are processed separately.
 *
 *      X       X       X       X       X       X           --xxxx----xxxx----xxxx----xxxx----xxxx--
 *     / \     / \     / \     / \     / \     /            --xxxx----xxxx----xxxx----xxxx----xxxx--
 *    /   \   /   \   /   \   /   \   /   \   /             xx----xxxx----xxxx----xxxx----xxxx----xx
 *   /     \ /     \ /     \ /     \ /     \ /              xx----xxxx----xxxx----xxxx----xxxx----xx
 *  X       X       X       X       X       X               xx----xxxx----xxxx----xxxx----xxxx----xx
 *   \     / \     / \     / \     / \     / \       --->   xx----xxxx----xxxx----xxxx----xxxx----xx
 *    \   /   \   /   \   /   \   /   \   /   \             --xxxx----xxxx----xxxx----xxxx----xxxx--
 *     \ /     \ /     \ /     \ /     \ /     \            --xxxx----xxxx----xxxx----xxxx----xxxx--
 *      X       X       X       X       X       X           --xxxx----xxxx----xxxx----xxxx----xxxx--
 *     / \     / \     / \     / \     / \     /            --xxxx----xxxx----xxxx----xxxx----xxxx--
 *    /   \   /   \   /   \   /   \   /   \   /             xx----xxxx----xxxx----xxxx----xxxx----xx
 *   /     \ /     \ /     \ /     \ /     \ /              xx----xxxx----xxxx----xxxx----xxxx----xx
 *  X       X       X       X       X       X               xx----xxxx----xxxx----xxxx----xxxx----xx
 *
 *  The size of the rectangles depends both on the radar-map size as the world-map size. This means that the rectangle
 *  size may differ per column/row (depending on the fraction of the rectangle size). The algorithm is implemented
 *  in such a way that there is always a chess-board pattern, meaning that the size of a single row/column is constant.
 *
 *  Units, Cities and Trade are rendered as an overlay and rendered at position independent of the above grid. They
 *  will always be the same size.
 */

class RadarMapCell
{
public:
	typedef uint16 Type;

	static inline Type createValue(const COLOR landColor, const COLOR waterColor, bool isLand) {
		return landColor | (waterColor << 9) | (isLand ? ISLAND_MARKER : 0);
	}

	static inline bool GetIsLand(const Type value) {
		return (value & ISLAND_MARKER);
	}

	static inline Pixel16 GetPixel16(const Type value) {
		return GetIsLand(value) ? GetLandPixel16(value) : GetWaterPixel16(value);
	}

	static inline Pixel16 GetLandPixel16(const Type value) {
		return g_colorSet->GetColor(static_cast<COLOR>(value & 0xff));
	}

	static inline Pixel16 GetWaterPixel16(const Type value) {
		return g_colorSet->GetColor(static_cast<COLOR>(value >> 9));
	}

	static inline Type createValue(bool explored, bool owned, const COLOR ownerColor) {
		return explored ? (owned ? ownerColor : NO_OWNER) : NOT_EXPLORED;
	}

	static inline Pixel16 GetIsExplored(const Type value) {
		return !(value & NOT_EXPLORED);
	}

	static inline Pixel16 GetOwnerPixel16(const Type value) {
		return ((value & NOT_EXPLORED) || (value & NO_OWNER)) ? 0 : g_colorSet->GetColor(static_cast<COLOR>(value));
	}

	static inline Type GetTileCell(const WORLD_DIRECTION direction, const Type *map, sint32 mapPitch)
	{
		// Tile cell deduction is not straight-forward due to diamond shape
		switch (direction) {
			case NORTHWEST:
				return *(map - mapPitch);
			case NORTH:
				return *(map + 1 - 2 * mapPitch);
			case NORTHEAST:
				return *(map + 1 - mapPitch);
			case EAST:
				return *(map + 1);
			case SOUTHEAST:
				return *(map + mapPitch);
			case SOUTH:
				return *(map - 1 + 2 * mapPitch);
			case SOUTHWEST:
				return *(map - 1 + mapPitch);
			case WEST:
				return *(map - 1);
			default:
				return *map;
		}
	}

	static inline Type GetCrossingCell(const WORLD_DIRECTION direction, const Type *map, sint32 mapPitch)
	{
		switch (direction) {
			case WEST:
				return *map;
			case NORTH:
				return *(map + 1 - mapPitch);
			case EAST:
				return *(map + 1);
			case SOUTH:
				return *(map + mapPitch);
			default:
				return *map;
		}
	}

private:
	static const Type ISLAND_MARKER = 0x0100;
	static const Type NOT_EXPLORED = 0x0100;
	static const Type NO_OWNER = 0x0200;
};

/*
 * Radar render classes
 */
class RadarRenderBase
{
public:
	RadarRenderBase(const Player & player, const RadarMap::RadarProperties & radarProperties, const MapPoint & mapSize,
			double tileWidth, double tileHeight)
	: m_player(player)
	, m_radarProperties(radarProperties)
	, m_mapSize(mapSize)
	, m_tileSize { tileWidth, tileHeight }
	{}
	virtual ~RadarRenderBase() {}

protected:
	struct MapPointDouble {
		double x;
		double y;
	};
	inline const MapPointDouble GetScreenPosition(const MapPoint & radarPos) const;

	COLOR RadarTileRelationsColor(const MapPoint & position, sint32 unitOwner = -1) const;
	COLOR DoRadarTileRelationsColor(sint32 owner) const;

	const Player                    & m_player;
	const RadarMap::RadarProperties & m_radarProperties;
	const MapPoint                  & m_mapSize;
	const MapPointDouble              m_tileSize;
};

class RadarRender : public RadarRenderBase
{
public:
	RadarRender(const Player & player, const RadarMap::RadarProperties & radarProperties, const MapPoint & mapSize,
			double tileWidth, double tileHeight)
	: RadarRenderBase(player, radarProperties, mapSize, tileWidth, tileHeight)
	{
	}
	virtual ~RadarRender() {}

	void Render(aui_Surface & surface, RadarMapCell::Type * map, const MapPoint & radarSize,
			const MapPoint & radarPos) const;
	void RenderSingleTile(aui_Surface & surface, const MapPoint & worldPos, const MapPoint & radarPos) const;

protected:
	const MapPoint WrapPoint(const MapPoint & point) const;

	void FillTileMap(const MapPoint & worldPos, RadarMapCell::Type tileMap[15]) const;
	virtual void RenderCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
			sint32 mapPitch) const = 0;
	virtual void RenderTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
			sint32 mapPitch) const = 0;
	void RenderMapTexture(aui_Surface & surface, RadarMapCell::Type * map, const MapPoint & radarSize,
			double oddYStartValueX, sint32 startWorldPosX, sint32 startWorldPosY) const;

	virtual RadarMapCell::Type GetMapCellValue(const MapPoint & worldPos) const = 0;

private:
	void FillMapTexture(RadarMapCell::Type * map) const;
	void FillTextureBorders(RadarMapCell::Type * map) const;
};

class RadarRenderWorld : public RadarRender {
public:
	RadarRenderWorld(const Player & player, const RadarMap::RadarProperties & radarProperties, const MapPoint & mapSize,
			double tileWidth, double tileHeight)
	: RadarRender(player, radarProperties, mapSize, tileWidth, tileHeight) {}
	virtual ~RadarRenderWorld() {}

protected:
	RadarMapCell::Type GetMapCellValue(const MapPoint & worldPos) const override;

	void RenderTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
			sint32 mapPitch) const override;
	void RenderCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
			sint32 mapPitch) const override;

private:
	static const RadarMapCell::Type BLEND_WATER = 128;

	static const uint32 NO_COAST    = 0;
	static const uint32 WEST_COAST  = 1;
	static const uint32 NORTH_COAST = 2;
	static const uint32 EAST_COAST  = 4;
	static const uint32 SOUTH_COAST = 8;
	static const uint32 WATER       = 0;
	static const uint32 WEST_LAND   = 1;
	static const uint32 NORTH_LAND  = 2;
	static const uint32 EAST_LAND   = 4;
	static const uint32 SOUTH_LAND  = 8;
	static const uint32 ALL_LAND    = WEST_LAND | NORTH_LAND | EAST_LAND | SOUTH_LAND;

	uint32 DetermineCoast(RadarMapCell::Type * map, sint32 mapPitch) const;

	inline void RenderStandardTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map) const {
		primitives_ClippedPaintRect16(surface, rect, RadarMapCell::GetPixel16(*map));
	}
	void RenderCoastTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map, sint32 mapPitch,
			uint32 coast) const;
	void RenderTerrainCoastTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map, sint32 mapPitch,
			uint32 coast) const;
	void RenderStandardCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
			sint32 mapPitch) const;
	void RenderCoastCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map, sint32 mapPitch,
			uint32 landPattern) const;
	void RenderBasicCrossing(aui_Surface & surface, const RECT & rect, Pixel16 west, Pixel16 north, Pixel16 east,
			Pixel16 south) const;
};

class RadarRenderPlayerBorder : public RadarRender {
public:
	RadarRenderPlayerBorder(const Player & player, const RadarMap::RadarProperties & radarProperties,
			const MapPoint & mapSize, double tileWidth, double tileHeight)
	: RadarRender(player, radarProperties, mapSize, tileWidth, tileHeight)
	{
		Assert(m_radarProperties.m_displayBorders);
	}
	virtual ~RadarRenderPlayerBorder() {}

protected:
	void RenderCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
			sint32 mapPitch) const override;
	void RenderTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map, sint32 mapPitch) const override
	{}

	RadarMapCell::Type GetMapCellValue(const MapPoint & worldPos) const override;

private:
	static inline void RenderBorderLine(aui_Surface & surface, sint32 x1, sint32 y1, sint32 x2, sint32 y2,
			Pixel16 color)
	{
		if (color != 0) {
			primitives_ClippedLine16(surface, x1, y1, x2, y2, color);
		}
	}

	void RenderCrossingAllExplored(aui_Surface & surface, const RECT & rect, Pixel16 west, Pixel16 north, Pixel16 east,
			Pixel16 south) const;
};

class RadarRenderOverlay : public RadarRenderBase {
public:
	RadarRenderOverlay(const Player & player, const RadarMap::RadarProperties & radarProperties,
			const MapPoint & mapSize, double tileWidth, double tileHeight)
	: RadarRenderBase(player, radarProperties, mapSize, tileWidth, tileHeight) {}
	virtual ~RadarRenderOverlay() {}

	void Render(aui_Surface & surface, const MapPoint & radarOffset) const;
	void RenderSingleTile(aui_Surface & surface, const MapPoint & worldPos, const MapPoint & radarPos) const;

private:
	void RenderCity(aui_Surface & surface, const MapPoint & worldPos, const MapPointDouble & screenPosition) const;
	void RenderUnit(aui_Surface & surface, const MapPoint & worldPos, const MapPointDouble & screenPosition) const;
	void RenderCapitol(aui_Surface & surface, const MapPoint & worldPos, const MapPointDouble & screenPosition) const;
	void RenderTrade(aui_Surface & surface, const MapPoint & worldPos, const MapPointDouble & screenPosition) const;
	void RenderTradeRoute(aui_Surface & surface, const RECT & rect) const;
};

/*
 * RadarRenderBase - implementation
 */
const RadarRenderBase::MapPointDouble RadarRenderBase::GetScreenPosition(const MapPoint & radarPos) const
{
	MapPointDouble position;
	const MapPoint screenPosition(((radarPos.y / 2) + radarPos.x) % m_mapSize.x, radarPos.y);
	position.x = screenPosition.x * m_tileSize.x;
	position.y = screenPosition.y * m_tileSize.y;
	if (screenPosition.y & 1) {
		position.x += m_tileSize.x / 2.0;
	}
	return position;
}

COLOR RadarRenderBase::RadarTileRelationsColor(const MapPoint & position, sint32 unitOwner) const
{
	sint32 owner = unitOwner < 0 ? g_tiledMap->GetVisibleCellOwner(position) : unitOwner;
	return DoRadarTileRelationsColor(owner);
}

COLOR RadarRenderBase::DoRadarTileRelationsColor(sint32 owner) const
{
	COLOR color = COLOR_WHITE;
	if (owner >= 0)
	{
		if (m_player.m_owner == owner || m_player.HasAllianceWith(owner)) {
			color = COLOR_BLUE;
		} else if (m_player.HasWarWith(owner)) {
			color = COLOR_RED;
		} else if (m_player.HasPeaceTreatyWith(owner) || m_player.HasAnyPactWith(owner)) {
			color = COLOR_GREEN;
		} else if (m_player.HasContactWith(owner)) {
			color = COLOR_YELLOW;
		}
	}
	return color;
}

/*
 * RadarRender - implementation
 */
void RadarRender::Render(aui_Surface & surface, RadarMapCell::Type * map, const MapPoint & radarSize,
		const MapPoint & radarPos) const
{
	FillMapTexture(map);

	sint32 tileWidthWhole = floor(m_tileSize.x);
	double tileWidthFraction = m_tileSize.x - tileWidthWhole;
	double oddYStartValueX = floor(m_tileSize.x / 2.0) + ((tileWidthWhole & 1) ? tileWidthFraction : 0.0);

	sint32 startWorldPosX = m_mapSize.x - radarPos.x;
	if (startWorldPosX >= m_mapSize.x) {
		startWorldPosX = 0;
	}
	sint32 startWorldPosY = m_mapSize.y - radarPos.y;
	if (startWorldPosY >= m_mapSize.y) {
		startWorldPosY = 0;
	}

	RenderMapTexture(surface, map, radarSize, oddYStartValueX, startWorldPosX, startWorldPosY);
}

void RadarRender::RenderMapTexture(aui_Surface & surface, RadarMapCell::Type * map, const MapPoint & radarSize,
		double oddYStartValueX, sint32 startWorldPosX, sint32 startWorldPosY) const
{
	const sint32 mapPitch = m_mapSize.x + 2;
	RadarMapCell::Type * const mapStart = map + 1 + (2 * mapPitch);
	RadarMapCell::Type * const mapEnd = mapStart + m_mapSize.y * mapPitch;
	RadarMapCell::Type * mapRowStart = mapStart + startWorldPosY * mapPitch;

	double y = 0.0;
	bool oddY = startWorldPosY & 1;
	while (y < radarSize.y)
	{
		sint32 y1 = static_cast<sint32>(y);
		y += m_tileSize.y;
		sint32 y2 = static_cast<sint32>(y);

		RadarMapCell::Type * const colorMapRowEnd = mapRowStart + m_mapSize.x;
		RadarMapCell::Type * colorMapCell = mapRowStart + startWorldPosX;

		double x = oddY ? oddYStartValueX : 0.0;
		if (oddY) {
			RenderCrossing(surface, RECT {0, y1, static_cast<sint32>(x) - 1, y2 - 1 },
					colorMapCell - 1, mapPitch);
		}
		while (x < radarSize.x) {
			sint32 x1 = static_cast<sint32>(x);
			x += m_tileSize.x;
			sint32 x2 = static_cast<sint32>(x);

			sint32 width = x2 - x1;
			sint32 split = x1 + (width + (oddY ? 1 : 0)) / 2;
			RECT renderRect = { x1, y1, split - 1, y2 - 1 };
			RenderTile(surface, renderRect, colorMapCell, mapPitch);

			renderRect.left  = split;
			renderRect.right = x2 - 1;
			RenderCrossing(surface, renderRect, colorMapCell, mapPitch);

			colorMapCell++;
			if (colorMapCell >= colorMapRowEnd) {
				colorMapCell = mapRowStart;
			}
		}

		mapRowStart += mapPitch;
		if (mapRowStart >= mapEnd) {
			mapRowStart = mapStart;
		}

		oddY = !oddY;
		if (!oddY) {
			startWorldPosX--;
			if (startWorldPosX < 0) {
				startWorldPosX = m_mapSize.x - 1;
			}
		}
	}
}

void RadarRender::RenderSingleTile(aui_Surface & surface, const MapPoint & worldPos, const MapPoint & radarPos) const
{
	RadarMapCell::Type tileMap[15];
	FillTileMap(worldPos, tileMap);
	const sint32 mapPitch = 3;

	const MapPointDouble position = GetScreenPosition(radarPos);

	sint32 y1 = static_cast<sint32>(position.y);
	sint32 y2 = static_cast<sint32>(position.y + m_tileSize.y);

	sint32 x1 = static_cast<sint32>(position.x);
	sint32 x2 = static_cast<sint32>(position.x + m_tileSize.x);

	bool oddY = radarPos.y & 1;
	sint32 width = x2 - x1;
	sint32 split = x1 + (width + (oddY ? 1 : 0)) / 2;
	RECT renderRect = { x1, y1, split - 1, y2 - 1 };

	RenderTile(surface, renderRect, tileMap + 7, mapPitch);
	if ((x1 <= m_tileSize.x) && oddY) { // first column, render also crossing before
		const RECT firstColumn = {0, y1, x1 - 1, y2 - 1};
		RenderCrossing(surface, firstColumn, tileMap + 6, mapPitch);
	}

	renderRect.left = split;
	renderRect.right = x2 - 1;
	RenderCrossing(surface, renderRect, tileMap + 7, mapPitch);
}

const MapPoint RadarRender::WrapPoint(const MapPoint & point) const
{
	MapPoint result = point;
	if (result.x < 0) {
		result.x = g_theWorld->IsXwrap() ? m_mapSize.x - 1 : 0;
	} else if (result.x >= m_mapSize.x) {
		result.x = g_theWorld->IsXwrap() ? 0 : m_mapSize.x - 1;
	}
	if (result.y < 0) {
		result.y = g_theWorld->IsYwrap() ? m_mapSize.y - 1 : 0;
	} else if (result.y >= m_mapSize.y) {
		result.y = g_theWorld->IsYwrap() ? 0 : m_mapSize.y - 1;
	}
	return result;
}

void RadarRender::FillMapTexture(RadarMapCell::Type * map) const
{
	sint32 mapPitch = m_mapSize.x + 2;
	RadarMapCell::Type * row = map + 1 + (2 * mapPitch); // skip borders

	for (sint32 y = 0; y < m_mapSize.y; y++)
	{
		RadarMapCell::Type * cell = row;
		for (sint32 x = 0; x < m_mapSize.x; x++)
		{
			*cell++ = GetMapCellValue(MapPoint(x, y));
		}
		row += mapPitch;
	}

	FillTextureBorders(map);
}

void RadarRender::FillTextureBorders(RadarMapCell::Type * map) const
{
	sint32 pitch = m_mapSize.x + 2;

	bool xWrap = g_theWorld->IsXwrap();
	bool yWrap = g_theWorld->IsYwrap();

	// Fill double top- and bottom-border
	RadarMapCell::Type * topColor = map + 1 + pitch;
	RadarMapCell::Type * topColor2 = topColor - pitch;
	RadarMapCell::Type * bottomColor = topColor + pitch * (m_mapSize.y + 1);
	RadarMapCell::Type * bottomColor2 = bottomColor + pitch;
	RadarMapCell::Type * endTopColor = topColor + m_mapSize.x;
	for ( ; topColor < endTopColor; topColor++, topColor2++, bottomColor++, bottomColor2++)
	{
		if (yWrap)
		{
			*topColor = *(bottomColor - pitch);
			*topColor2 = *(bottomColor - 2 * pitch);
			*bottomColor = *(topColor + pitch);
			*bottomColor2 = *(topColor + 2 * pitch);
		}
		else
		{
			*topColor = *(topColor + pitch);
			*topColor2 = *topColor;
			*bottomColor = *(bottomColor - pitch);
			*bottomColor2 = *bottomColor;
		}
	}

	// Fill left- and right-border
	RadarMapCell::Type * leftBorder = map + 2 * pitch;
	RadarMapCell::Type * rightBorder = leftBorder + m_mapSize.x + 1;
	RadarMapCell::Type * endLeftBorder = leftBorder + m_mapSize.y * pitch;
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
	RadarMapCell::Type * leftTopCorner     = map + pitch;
	RadarMapCell::Type * leftTop2Corner    = leftTopCorner - pitch;
	RadarMapCell::Type * rightTopCorner    = leftTopCorner + m_mapSize.x + 1;
	RadarMapCell::Type * rightTop2Corner   = rightTopCorner - pitch;
	RadarMapCell::Type * leftBottomCorner  = leftTopCorner + pitch * (m_mapSize.y + 1);
	RadarMapCell::Type * leftBottom2Corner = leftBottomCorner + pitch;
	RadarMapCell::Type * rightBottomCorner = leftBottomCorner + m_mapSize.x + 1;
	RadarMapCell::Type * rightBottom2Corner = rightBottomCorner + pitch;
	if (yWrap && xWrap)
	{
		*leftTopCorner      = *(rightBottomCorner - 1 - pitch);
		*leftTop2Corner     = *(rightBottomCorner - 1 - 2 * pitch);
		*rightTopCorner     = *(leftBottomCorner + 1 - pitch);
		*rightTop2Corner    = *(leftBottomCorner + 1 - 2 * pitch);
		*leftBottomCorner   = *(rightTopCorner - 1 + pitch);
		*leftBottom2Corner  = *(rightTopCorner - 1 + 2 * pitch);
		*rightBottomCorner  = *(leftTopCorner + 1 + pitch);
		*rightBottom2Corner = *(leftTopCorner + 1 + 2 * pitch);
	} else {
		sint32 xOffset = xWrap ? m_mapSize.x : 1;

		*leftTopCorner      = *(leftTopCorner + xOffset);
		*leftTop2Corner     = *(leftTopCorner + xOffset - pitch);
		*rightTopCorner     = *(rightTopCorner - xOffset);
		*rightTop2Corner    = *(rightTopCorner - xOffset - pitch);
		*leftBottomCorner   = *(leftBottomCorner + xOffset);
		*leftBottom2Corner  = *(leftBottomCorner + xOffset + pitch);
		*rightBottomCorner  = *(rightBottomCorner - xOffset);
		*rightBottom2Corner = *(rightBottomCorner - xOffset + pitch);
	}
}

void RadarRender::FillTileMap(const MapPoint & worldPos, RadarMapCell::Type tileMap[15]) const
{
	MapPoint point(worldPos.x - 1, worldPos.y - 2);
	for (sint32 y = 0; y < 5; y++, point.y++)
	{
		for (sint32 x = 0; x < 3; x++, point.x++) {
			tileMap[y * 3 + x] = GetMapCellValue(WrapPoint(point));
		}
		point.x = worldPos.x - 1;
	}
}

/*
 * RadarRenderWorld - implementation
 */
RadarMapCell::Type RadarRenderWorld::GetMapCellValue(const MapPoint & worldPos) const
{
	bool isLand = (g_theWorld->IsLand(worldPos) || g_theWorld->IsMountain(worldPos))
			&& !g_theWorld->IsTunnel(worldPos);

	if (!m_player.IsExplored(worldPos)) {
		return RadarMapCell::createValue( COLOR_BLACK, COLOR_BLACK, isLand);
	}

	COLOR landColor = static_cast<COLOR>(COLOR_TERRAIN_0 + TERRAIN_GRASSLAND);
	if (m_radarProperties.m_displayOverlay && m_radarProperties.m_mapOverlay) {
		landColor = m_radarProperties.m_mapOverlay[worldPos.y * m_mapSize.x + worldPos.x];
	}
	else if (m_radarProperties.m_displayPolitical)
	{
		sint32 owner = g_theWorld->GetOwner(worldPos);
		if (owner >= 0 && (m_player.HasContactWith(owner) || m_player.m_hasGlobalRadar || g_fog_toggle || g_god))
		{
			if (m_radarProperties.m_displayRelations) {
				landColor = RadarTileRelationsColor(worldPos);
			} else {
				landColor = g_colorSet->ComputePlayerColor(g_tiledMap->GetVisibleCellOwner(worldPos));
			}
		}
	}
	else if (m_radarProperties.m_displayTerrain) {
		landColor = static_cast<COLOR>(COLOR_TERRAIN_0 + g_tiledMap->GetVisibleTerrainType(worldPos));
	}

	const COLOR waterColor = isLand ? COLOR_BLACK : static_cast<COLOR>(COLOR_TERRAIN_0 +
			(m_radarProperties.m_displayTerrain ? g_tiledMap->GetVisibleTerrainType(worldPos) : TERRAIN_WATER_DEEP));
	return RadarMapCell::createValue(landColor, waterColor, isLand);
}

void RadarRenderWorld::RenderTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
		sint32 mapPitch) const
{
	const uint32 coast = DetermineCoast(map, mapPitch);
	if (coast == NO_COAST) {
		RenderStandardTile(surface, rect, map);
	} else {
		if (m_radarProperties.m_displayTerrain && !m_radarProperties.m_displayPolitical) {
			RenderTerrainCoastTile(surface, rect, map, mapPitch, coast);
		} else {
			RenderCoastTile(surface, rect, map, mapPitch, coast);
		}
	}
}

void RadarRenderWorld::RenderCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
		sint32 mapPitch) const
{
	uint32 landPattern = WATER;
	landPattern |= RadarMapCell::GetIsLand(RadarMapCell::GetCrossingCell(WEST, map, mapPitch)) ? WEST_LAND : WATER;
	landPattern |= RadarMapCell::GetIsLand(RadarMapCell::GetCrossingCell(NORTH, map, mapPitch)) ? NORTH_LAND : WATER;
	landPattern |= RadarMapCell::GetIsLand(RadarMapCell::GetCrossingCell(EAST, map, mapPitch)) ? EAST_LAND : WATER;
	landPattern |= RadarMapCell::GetIsLand(RadarMapCell::GetCrossingCell(SOUTH, map, mapPitch)) ? SOUTH_LAND : WATER;

	if ((landPattern == ALL_LAND) || (landPattern == WATER))
	{
		RenderStandardCrossing(surface, rect, map, mapPitch);
	} else {
		RenderCoastCrossing(surface, rect, map, mapPitch, landPattern);
	}
}

uint32 RadarRenderWorld::DetermineCoast(RadarMapCell::Type * map, sint32 mapPitch) const
{
	if (RadarMapCell::GetIsLand(*map)) {
		return NO_COAST;
	}

	const bool northWestIsLand = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(NORTHWEST, map, mapPitch));
	const bool northIsLand     = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(NORTH, map, mapPitch));
	const bool northEastIsLand = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(NORTHEAST, map, mapPitch));
	const bool westIsLand      = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(WEST, map, mapPitch));
	const bool eastIsLand      = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(EAST, map, mapPitch));
	const bool southWestIsLand = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(SOUTHWEST, map, mapPitch));
	const bool southIsLand     = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(SOUTH, map, mapPitch));
	const bool southEastIsLand = RadarMapCell::GetIsLand(RadarMapCell::GetTileCell(SOUTHEAST, map, mapPitch));

	uint32 coast = northWestIsLand && westIsLand && southWestIsLand ? WEST_COAST : NO_COAST;
	coast |= (northWestIsLand && northIsLand && northEastIsLand ? NORTH_COAST : NO_COAST);
	coast |= (northEastIsLand && eastIsLand && southEastIsLand ? EAST_COAST : NO_COAST);
	coast |= (southWestIsLand && southIsLand && southEastIsLand ? SOUTH_COAST : NO_COAST);
	return coast;
}

void RadarRenderWorld::RenderCoastTile(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
		sint32 mapPitch, uint32 coast) const
{
	primitives_ClippedPaintRect16(surface, rect, RadarMapCell::GetPixel16(*map));

	const Pixel16 landColor = RadarMapCell::GetLandPixel16(*map);
	if (coast & WEST_COAST) {
		primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.top, rect.left, rect.bottom }, landColor);
	}
	if (coast & NORTH_COAST) {
		primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.top, rect.right, rect.top }, landColor);
	}
	if (coast & EAST_COAST) {
		primitives_ClippedPaintRect16(surface, RECT { rect.right, rect.top, rect.right, rect.bottom }, landColor);
	}
	if (coast & SOUTH_COAST) {
		primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.bottom, rect.right, rect.bottom }, landColor);
	}
}

void RadarRenderWorld::RenderTerrainCoastTile(aui_Surface &surface, const RECT &rect, RadarMapCell::Type *map,
		sint32 mapPitch, uint32 coast) const
{
	const Pixel16 northWest = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(NORTHWEST, map, mapPitch));
	const Pixel16 north     = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(NORTH, map, mapPitch));
	const Pixel16 northEast = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(NORTHEAST, map, mapPitch));
	const Pixel16 west      = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(WEST, map, mapPitch));
	const Pixel16 center    = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(NOWHERE, map, mapPitch));
	const Pixel16 east      = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(EAST, map, mapPitch));
	const Pixel16 southWest = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(SOUTHWEST, map, mapPitch));
	const Pixel16 south     = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(SOUTH, map, mapPitch));
	const Pixel16 southEast = RadarMapCell::GetLandPixel16(RadarMapCell::GetTileCell(SOUTHEAST, map, mapPitch));

	primitives_ClippedPaintRect16(surface, rect, center);

	if (coast & WEST_COAST)
	{
		if ((northWest == west) && (southWest == west)) {
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.top, rect.left, rect.bottom }, west);
		}
	}
	if (coast & NORTH_COAST)
	{
		if ((northWest == north) && (northEast == north)) {
			primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.top, rect.right, rect.top }, north);
		}
	}
	if (coast & EAST_COAST)
	{
		if ((northEast == east) && (southEast == east)) {
			primitives_ClippedPaintRect16(surface, RECT { rect.right, rect.top, rect.right, rect.bottom }, east);
		}
	}
	if (coast & SOUTH_COAST)
	{
		if ((southWest == south) && (southEast == south)) {
			primitives_ClippedPaintRect16(surface, RECT { rect.left, rect.bottom, rect.right, rect.bottom }, south);
		}
	}
}

void RadarRenderWorld::RenderStandardCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
		sint32 mapPitch) const
{
	Pixel16 west  = RadarMapCell::GetPixel16(RadarMapCell::GetCrossingCell(WEST, map, mapPitch));
	Pixel16 north = RadarMapCell::GetPixel16(RadarMapCell::GetCrossingCell(NORTH, map, mapPitch));
	Pixel16 east  = RadarMapCell::GetPixel16(RadarMapCell::GetCrossingCell(EAST, map, mapPitch));
	Pixel16 south = RadarMapCell::GetPixel16(RadarMapCell::GetCrossingCell(SOUTH, map, mapPitch));

	RenderBasicCrossing(surface, rect, west, north, east, south);
}

void RadarRenderWorld::RenderCoastCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
		sint32 mapPitch, uint32 landPattern) const
{
	const RadarMapCell::Type west = RadarMapCell::GetCrossingCell(WEST, map, mapPitch);
	Pixel16 westLand  = RadarMapCell::GetLandPixel16(west);
	Pixel16 westWater = RadarMapCell::GetWaterPixel16(west);

	RadarMapCell::Type north = RadarMapCell::GetCrossingCell(NORTH, map, mapPitch);
	Pixel16 northLand  = RadarMapCell::GetLandPixel16(north);
	Pixel16 northWater = RadarMapCell::GetWaterPixel16(north);

	RadarMapCell::Type east = RadarMapCell::GetCrossingCell(EAST, map, mapPitch);
	Pixel16 eastLand  = RadarMapCell::GetLandPixel16(east);
	Pixel16 eastWater = RadarMapCell::GetWaterPixel16(east);

	RadarMapCell::Type south = RadarMapCell::GetCrossingCell(SOUTH, map, mapPitch);
	Pixel16 southLand  = RadarMapCell::GetLandPixel16(south);
	Pixel16 southWater = RadarMapCell::GetWaterPixel16(south);

	switch (landPattern) {
		case WEST_LAND:
			primitives_ClippedPaintRect16(surface, rect, eastWater);
			primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.top, rect.left, rect.bottom}, westLand);
			break;
		case NORTH_LAND:
			primitives_ClippedPaintRect16(surface, rect, westWater);
			primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.top, rect.right, rect.top}, northLand);
			break;
		case EAST_LAND:
			primitives_ClippedPaintRect16(surface, rect, westWater);
			primitives_ClippedPaintRect16(surface, RECT{rect.right, rect.top, rect.right, rect.bottom}, eastLand);
			break;
		case SOUTH_LAND:
			primitives_ClippedPaintRect16(surface, rect, westWater);
			primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.bottom, rect.right, rect.bottom}, southLand);
			break;
		case ALL_LAND & ~WEST_LAND:
		case ALL_LAND & ~NORTH_LAND:
		case ALL_LAND & ~EAST_LAND:
		case ALL_LAND & ~SOUTH_LAND:
			RenderBasicCrossing(surface, rect, westLand, northLand, eastLand, southLand);
			break;
		case WEST_LAND | NORTH_LAND:
			if (westLand == northLand) {
				primitives_ClippedPaintRect16(surface, rect, eastWater);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, westLand);
			} else {
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, westLand);
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, northLand);
				if (rect.right > rect.left && rect.bottom > rect.top) {
					primitives_ClippedTriangle16(surface, RECT{rect.left + 1, rect.top + 1, rect.right, rect.bottom},
							TI_RIGHT_BOTTOM, eastWater);
				}
			}
			break;
		case NORTH_LAND | EAST_LAND:
			if (northLand == eastLand) {
				primitives_ClippedPaintRect16(surface, rect, westWater);
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, eastLand);
			} else {
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, northLand);
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, eastLand);
				if (rect.right > rect.left && rect.bottom > rect.top) {
					primitives_ClippedTriangle16(surface, RECT{ rect.left, rect.top + 1, rect.right - 1, rect.bottom },
							TI_LEFT_BOTTOM, westWater);
				}
			}
			break;
		case EAST_LAND | SOUTH_LAND:
			if (eastLand == southLand) {
				primitives_ClippedPaintRect16(surface, rect, westWater);
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, eastLand);
			} else {
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, eastLand);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, southLand);
				if (rect.right > rect.left && rect.bottom > rect.top) {
					primitives_ClippedTriangle16(surface, RECT{ rect.left, rect.top, rect.right - 1, rect.bottom - 1 },
							TI_LEFT_TOP, westWater);
				}
			}
			break;
		case SOUTH_LAND | WEST_LAND:
			if (southLand == westLand) {
				primitives_ClippedPaintRect16(surface, rect, eastWater);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, westLand);
			} else {
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, westLand);
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, southLand);
				if (rect.right > rect.left && rect.bottom > rect.top) {
					primitives_ClippedTriangle16(surface, RECT{ rect.left + 1, rect.top, rect.right, rect.bottom - 1 },
							TI_RIGHT_TOP, eastWater);
				}
			}
			break;
		case WEST_LAND | EAST_LAND:
			if (westLand == eastLand) {
				primitives_ClippedPaintRect16(surface, rect, westLand);
			} else {
				sint32 split = (rect.left + rect.right) / 2;
				primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.top, split, rect.bottom}, westLand);
				if (rect.right > split) {
					primitives_ClippedPaintRect16(surface, RECT{ split + 1, rect.top, rect.right, rect.bottom },
							eastLand);
				}
			}
			if (rect.right - rect.left > 1)
			{
				primitives_ClippedPaintRect16(surface, RECT{rect.left + 1, rect.top, rect.right - 1, rect.top}, northWater,
						BLEND_WATER);
				primitives_ClippedPaintRect16(surface, RECT{rect.left + 1, rect.bottom, rect.right - 1, rect.bottom},
						southWater, BLEND_WATER);
			}
			break;
		case NORTH_LAND | SOUTH_LAND:
			if (northLand == southLand) {
				primitives_ClippedPaintRect16(surface, rect, northLand);
			} else {
				sint32 split = (rect.top + rect.bottom) / 2;
				primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.top, rect.right, split}, northLand);
				if (rect.bottom > split) {
					primitives_ClippedPaintRect16(surface, RECT{ rect.left, split + 1, rect.right, rect.bottom },
							southLand);
				}
			}
			if (rect.bottom - rect.top > 1)
			{
				primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.top + 1, rect.left, rect.bottom - 1}, westWater,
						BLEND_WATER);
				primitives_ClippedPaintRect16(surface, RECT{rect.right, rect.top + 1, rect.right, rect.bottom - 1},
						eastWater, BLEND_WATER);
			}
			break;
	}
}

void RadarRenderWorld::RenderBasicCrossing(aui_Surface & surface, const RECT & rect, Pixel16 west, Pixel16 north,
		Pixel16 east, Pixel16 south) const
{
	const uint32 ALL_EQUAL    = 0;
	const uint32 NORTH_DIFFER = 1;
	const uint32 EAST_DIFFER  = 2;
	const uint32 SOUTH_DIFFER = 4;

	uint32 westPattern = (west == north) ? 0 : NORTH_DIFFER;
	westPattern += (west == east) ? 0 : EAST_DIFFER;
	westPattern += (west == south) ? 0 : SOUTH_DIFFER;
	switch (westPattern) {
		case ALL_EQUAL:
		case NORTH_DIFFER:
		case EAST_DIFFER:
		case SOUTH_DIFFER:
			primitives_ClippedPaintRect16(surface, rect, west);
			break;
		case NORTH_DIFFER | EAST_DIFFER:
			if (east == north) {
				primitives_ClippedPaintRect16(surface, rect, east);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, west);
			} else {
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, north);
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, east);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, west);
			}
			break;
		case NORTH_DIFFER | SOUTH_DIFFER:
			primitives_ClippedPaintRect16(surface, rect, west);
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.top, rect.right, rect.top }, north,
					BLEND_WATER);
			primitives_ClippedPaintRect16(surface, RECT{ rect.left, rect.bottom, rect.right, rect.bottom }, south,
					BLEND_WATER);
			break;
		case EAST_DIFFER | SOUTH_DIFFER:
			if (east == south) {
				primitives_ClippedPaintRect16(surface, rect, east);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, west);
			} else {
				primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, east);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, south);
				primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, west);
			}
			break;
		case NORTH_DIFFER | EAST_DIFFER | SOUTH_DIFFER:
			sint32 eastPattern = (east == north) ? 0 : NORTH_DIFFER;
			eastPattern += (east == south) ? 0 : SOUTH_DIFFER;
			switch (eastPattern) {
				case ALL_EQUAL:
					primitives_ClippedPaintRect16(surface, rect, east);
					break;
				case NORTH_DIFFER:
					primitives_ClippedTriangle16(surface, rect, TI_LEFT_BOTTOM, west);
					primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, north);
					primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, east);
					break;
				case SOUTH_DIFFER:
					primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, west);
					primitives_ClippedTriangle16(surface, rect, TI_RIGHT_BOTTOM, south);
					primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, east);
					break;
				case NORTH_DIFFER | SOUTH_DIFFER:
					if (north == south)
					{
						primitives_ClippedPaintRect16(surface, rect, north);
						primitives_ClippedPaintRect16(surface, RECT{rect.left, rect.top, rect.left, rect.bottom}, west,
								BLEND_WATER);
						primitives_ClippedPaintRect16(surface, RECT{rect.right, rect.top, rect.right, rect.bottom},
								east, BLEND_WATER);
					}
					else
					{
						primitives_ClippedTriangle16(surface, rect, TI_LEFT_TOP, west);
						primitives_ClippedTriangle16(surface, rect, TI_RIGHT_TOP, north);
						if (rect.right > rect.left) {
							primitives_ClippedTriangle16(surface,
									RECT{ rect.left + 1, rect.top, rect.right, rect.bottom },
									TI_RIGHT_BOTTOM, east);
							primitives_ClippedPaintRect16(surface,
									RECT{ rect.left + 1, rect.bottom, rect.right, rect.bottom },
									south);
						}
					}
					break;
			}
			break;
	}
}

/*
 * RadarRenderPlayerBorder - implementation
 */
RadarMapCell::Type RadarRenderPlayerBorder::GetMapCellValue(const MapPoint & worldPos) const
{
	if (!m_player.IsExplored(worldPos)) {
		return RadarMapCell::createValue(false, false, COLOR_BLACK);
	}

	sint32 owner = g_tiledMap->GetVisibleCellOwner(worldPos);
	if ((owner == PLAYER_UNASSIGNED) ||
		(   // owner is assigned &&
			owner != m_player.m_owner
			&& !m_player.m_hasGlobalRadar
			&& !Scheduler::CachedHasContactWithExceptSelf(m_player.m_owner, owner)
			&& !g_fog_toggle // Don't forget if fog of war is off
			&& !g_god
		))
	{
		return RadarMapCell::createValue(true, false, COLOR_BLACK);
	}

	bool ownerVisible = m_player.m_hasGlobalRadar || m_player.HasContactWith(owner) || g_fog_toggle || g_god;
	return RadarMapCell::createValue(true, ownerVisible, m_radarProperties.m_displayRelations
		? DoRadarTileRelationsColor(owner) : g_colorSet->ComputePlayerColor(owner));
}

void RadarRenderPlayerBorder::RenderCrossing(aui_Surface & surface, const RECT & rect, RadarMapCell::Type * map,
		sint32 mapPitch) const
{
	const RadarMapCell::Type westCell  = RadarMapCell::GetCrossingCell(WEST, map, mapPitch);
	const RadarMapCell::Type northCell = RadarMapCell::GetCrossingCell(NORTH, map, mapPitch);
	const RadarMapCell::Type eastCell  = RadarMapCell::GetCrossingCell(EAST, map, mapPitch);
	const RadarMapCell::Type southCell = RadarMapCell::GetCrossingCell(SOUTH, map, mapPitch);

	const uint32 NONE_EXPLORED  = 0;
	const uint32 WEST_EXPLORED  = 1;
	const uint32 NORTH_EXPLORED = 2;
	const uint32 EAST_EXPLORED  = 4;
	const uint32 SOUTH_EXPLORED = 8;
	const uint32 ALL_EXPLORED   = WEST_EXPLORED | NORTH_EXPLORED | EAST_EXPLORED | SOUTH_EXPLORED;

	uint32 explored = RadarMapCell::GetIsExplored(westCell) ? WEST_EXPLORED : 0;
	explored |= RadarMapCell::GetIsExplored(northCell) ? NORTH_EXPLORED : 0;
	explored |= RadarMapCell::GetIsExplored(eastCell) ? EAST_EXPLORED : 0;
	explored |= RadarMapCell::GetIsExplored(southCell) ? SOUTH_EXPLORED : 0;

	Pixel16 west  = RadarMapCell::GetOwnerPixel16(westCell);
	Pixel16 north = RadarMapCell::GetOwnerPixel16(northCell);
	Pixel16 east  = RadarMapCell::GetOwnerPixel16(eastCell);
	Pixel16 south = RadarMapCell::GetOwnerPixel16(southCell);

	switch(explored)
	{
		case NONE_EXPLORED:
			break;
		case ALL_EXPLORED:
			RenderCrossingAllExplored(surface, rect, west, north, east, south);
			break;
		case WEST_EXPLORED | NORTH_EXPLORED:
			if (west != north)
			{
				sint32 centerRight = (rect.right + rect.left) / 2;
				sint32 centerBottom = (rect.top + rect.bottom) / 2;
				if (centerRight == rect.left) {
					centerRight++;
				}
				if (centerBottom == rect.top) {
					centerBottom++;
				}
				if (west == 0)
				{
					RenderBorderLine(surface, rect.left, rect.top, centerRight, centerBottom, north);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.top, centerRight, centerBottom, west);
					RenderBorderLine(surface, rect.left, rect.top - 1, centerRight, centerBottom - 1, north);
				}
			}
			break;
		case NORTH_EXPLORED | EAST_EXPLORED:
			if (north != east)
			{
				sint32 centerLeft = (rect.right + rect.left) / 2;
				sint32 centerBottom = (rect.top + rect.bottom) / 2;
				if (centerLeft == rect.right) {
					centerLeft--;
				}
				if (centerBottom == rect.top) {
					centerBottom++;
				}
				if (east == 0) {
					RenderBorderLine(surface, centerLeft, centerBottom, rect.right, rect.top, north);
				}
				else
				{
					RenderBorderLine(surface, centerLeft, centerBottom, rect.right, rect.top, east);
					RenderBorderLine(surface, centerLeft, centerBottom - 1, rect.right, rect.top - 1, north);
				}
			}
			break;
		case EAST_EXPLORED | SOUTH_EXPLORED:
			if (east != south)
			{
				sint32 centerLeft = (rect.right + rect.left) / 2;
				sint32 centerTop = (rect.top + rect.bottom) / 2;
				if (centerLeft == rect.right) {
					centerLeft--;
				}
				if (centerTop == rect.bottom) {
					centerTop--;
				}
				if (east == 0)
				{
					RenderBorderLine(surface, centerLeft, centerTop, rect.right, rect.bottom, south);
				}
				else
				{
					RenderBorderLine(surface, centerLeft, centerTop, rect.right, rect.bottom, east);
					RenderBorderLine(surface, centerLeft, centerTop + 1, rect.right, rect.bottom + 1, south);
				}
			}
			break;
		case SOUTH_EXPLORED | WEST_EXPLORED:
			if (south != west)
			{
				sint32 centerRight = (rect.right + rect.left) / 2;
				sint32 centerTop = (rect.top + rect.bottom) / 2;
				if (centerRight == rect.left) {
					centerRight++;
				}
				if (centerTop == rect.bottom) {
					centerTop--;
				}
				if (west == 0) {
					RenderBorderLine(surface, rect.left, rect.bottom, centerRight, centerTop, south);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.bottom, centerRight, centerTop, west);
					RenderBorderLine(surface, rect.left, rect.bottom + 1, centerRight, centerTop + 1, south);
				}
			}
			break;
		case NORTH_EXPLORED | EAST_EXPLORED | SOUTH_EXPLORED:
			if ((north == south) && (north == east)) { // all equal
				break;
			}
			if (north == south)
			{
				RenderBorderLine(surface, rect.right, rect.top, rect.right, rect.bottom, north);
				RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
			}
			else if (north == east)
			{
				if (north == 0) {
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, south);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, north);
					RenderBorderLine(surface, rect.left, rect.top + 1, rect.right, rect.bottom + 1, south);
				}
			}
			else if (south == east)
			{
				if (south == 0) {
					RenderBorderLine(surface, rect.right, rect.top, rect.left, rect.bottom, north);
				}
				else
				{
					RenderBorderLine(surface, rect.right, rect.top, rect.left, rect.bottom, south);
					RenderBorderLine(surface, rect.right, rect.top - 1, rect.left, rect.bottom - 1, north);
				}
			}
			else // all three different
			{
				RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
				RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
				RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
			}
			break;
		case EAST_EXPLORED | SOUTH_EXPLORED | WEST_EXPLORED:
			if ((east == west) && (east == south)) { // all equal
				break;
			}
			if (east == west)
			{
				RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.bottom, east);
				RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
			}
			else if (east == south)
			{
				if (east == 0) {
					RenderBorderLine(surface, rect.right, rect.top, rect.left, rect.bottom, west);
				}
				else
				{
					RenderBorderLine(surface, rect.right, rect.top, rect.left, rect.bottom, east);
					RenderBorderLine(surface, rect.right - 1, rect.top, rect.left - 1, rect.bottom, west);
				}
			}
			else if (west == south)
			{
				if (west == 0) {
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, east);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, west);
					RenderBorderLine(surface, rect.left + 1, rect.top, rect.right + 1, rect.bottom, east);
				}
			}
			else // all three different
			{
				RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
				RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
				RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
			}
			break;
		case SOUTH_EXPLORED | WEST_EXPLORED | NORTH_EXPLORED:
			if ((south == north) && (south == west)) { // all equal
				break;
			}
			if (south == north)
			{
				RenderBorderLine(surface, rect.left, rect.top, rect.left, rect.bottom, south);
				RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
			}
			else if (south == west)
			{
				if (south == 0) {
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, north);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.top, south);
					RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.bottom - 1, north);
				}
			}
			else if (north == west)
			{
				if (north == 0) {
					RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.top, south);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.top, north);
					RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.top + 1, south);
				}
			}
			else // all three different
			{
				RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
				RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
				RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
			}
			break;
		case WEST_EXPLORED | NORTH_EXPLORED | EAST_EXPLORED:
			if ((west == east) && (west == north)) { // all equal
				break;
			}
			if (west == east)
			{
				RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.top, west);
				RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
			}
			else if (west == north)
			{
				if (west == 0) {
					RenderBorderLine(surface, rect.right, rect.top, rect.left, rect.bottom, east);
				}
				else
				{
					RenderBorderLine(surface, rect.right, rect.top, rect.left, rect.bottom, west);
					RenderBorderLine(surface, rect.right + 1, rect.top, rect.left + 1, rect.bottom, east);
				}
			}
			else if (east == north)
			{
				if (east == 0) {
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, west);
				}
				else
				{
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, east);
					RenderBorderLine(surface, rect.left - 1, rect.top, rect.right - 1, rect.bottom, west);
				}
			}
			else // all three different
			{
				RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
				RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
				RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
			}
			break;
	}
}

void RadarRenderPlayerBorder::RenderCrossingAllExplored(aui_Surface & surface, const RECT & rect,
		Pixel16 west, Pixel16 north, Pixel16 east, Pixel16 south) const
{
	const uint32 ALL_EQUAL    = 0;
	const uint32 NORTH_DIFFER = 1;
	const uint32 EAST_DIFFER  = 2;
	const uint32 SOUTH_DIFFER = 4;

	uint32 westPattern = (west == north) ? 0 : NORTH_DIFFER;
	westPattern += (west == east) ? 0 : EAST_DIFFER;
	westPattern += (west == south) ? 0 : SOUTH_DIFFER;
	switch (westPattern) {
		case ALL_EQUAL: // to west
			// nothing to do
			break;
		case NORTH_DIFFER: // from west; east + south equal to west
			RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.top, west);
			RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
			break;
		case EAST_DIFFER: // from west; south + north equal to west
			RenderBorderLine(surface, rect.right, rect.top, rect.right, rect.bottom, west);
			RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
			break;
		case SOUTH_DIFFER: // from west; east + north equal to west
			RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.bottom, west);
			RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
			break;
		case NORTH_DIFFER | EAST_DIFFER: // from west; south equal to west
			if (east == north) {
				if (east == 0) {
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, west);
				} else {
					RenderBorderLine(surface, rect.left - 1, rect.top, rect.right, rect.bottom + 1, west);
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, east);
				}
			} else {
				RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, west);
				if (east == 0) {
					RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
				} else {
					RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
					RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.bottom - 1, north);
					RenderBorderLine(surface, rect.right, rect.top, rect.right, rect.bottom - 1, north);
				}
			}
			break;
		case NORTH_DIFFER | SOUTH_DIFFER: // from west; east equal to west
			RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.top, west);
			RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.bottom, west);
			RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
			RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
			break;
		case EAST_DIFFER | SOUTH_DIFFER: // from west; north equal to west
			if (east == south) {
				if (west == 0) {
					RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.top, east);
				} else {
					RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.top, west);
					RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right + 1, rect.top, east);
				}
			} else {
				RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.top, west);
				if (east == 0) {
					RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
				} else {
					RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
					RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.top + 1, south);
					RenderBorderLine(surface, rect.right, rect.top + 1, rect.right, rect.bottom, south);
				}
			}
			break;
		case NORTH_DIFFER | EAST_DIFFER | SOUTH_DIFFER: // all differ from west
			sint32 eastPattern = (east == north) ? 0 : NORTH_DIFFER;
			eastPattern += (east == south) ? 0 : SOUTH_DIFFER;
			switch (eastPattern) {
				case ALL_EQUAL: // to east; and differ from west
					RenderBorderLine(surface, rect.left, rect.top, rect.left, rect.bottom, east);
					RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
					break;
				case NORTH_DIFFER: // to east; south equal to east
					RenderBorderLine(surface, rect.left, rect.bottom, rect.right, rect.top, east);
					if (west == 0) {
						RenderBorderLine(surface, rect.left, rect.top - 1, rect.right, rect.top - 1, north);
					} else {
						RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
						RenderBorderLine(surface, rect.left, rect.bottom - 1, rect.right, rect.top - 1, north);
						RenderBorderLine(surface, rect.left, rect.bottom - 1, rect.left, rect.top, north);
					}
					break;
				case SOUTH_DIFFER: // to east; north equal to east
					RenderBorderLine(surface, rect.left, rect.top, rect.right, rect.bottom, east);
					if (west == 0) {
						RenderBorderLine(surface, rect.left, rect.bottom + 1, rect.right, rect.bottom + 1, south);
					} else {
						RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
						RenderBorderLine(surface, rect.left, rect.top + 1, rect.right, rect.bottom + 1, south);
						RenderBorderLine(surface, rect.left, rect.top + 1, rect.left, rect.bottom, south);
					}
					break;
				case NORTH_DIFFER | SOUTH_DIFFER: // to east; all differ to west and east
					if (north == south) {
						RenderBorderLine(surface, rect.left, rect.top, rect.left, rect.bottom, north);
						RenderBorderLine(surface, rect.right, rect.top, rect.right, rect.bottom, north);
						RenderBorderLine(surface, rect.left - 1, rect.top, rect.left - 1, rect.bottom, west);
						RenderBorderLine(surface, rect.right + 1, rect.top, rect.right + 1, rect.bottom, east);
					} else {
						RenderBorderLine(surface, rect.left, rect.top + 1, rect.left, rect.bottom - 1, west);
						RenderBorderLine(surface, rect.left + 1, rect.top, rect.right - 1 , rect.top, north);
						RenderBorderLine(surface, rect.right, rect.top + 1, rect.right, rect.bottom - 1, east);
						RenderBorderLine(surface, rect.left + 1, rect.bottom, rect.right - 1, rect.bottom, south);
					}
					break;
			}
			break;
	}
}

/*
 * RadarRenderOverlay
 */
void RadarRenderOverlay::Render(aui_Surface & surface, const MapPoint & radarOffset) const
{
	MapPoint radarPos = radarOffset;
	for (sint32 y = 0; y < m_mapSize.y; y++)
	{
		for (sint32 x = 0; x < m_mapSize.x; x++)
		{
			const MapPoint worldPos(x,y);
			if (m_player.IsExplored(worldPos))
			{
				const MapPointDouble screenPosition = GetScreenPosition(radarPos);
				RenderCity(surface, worldPos, screenPosition);
				RenderUnit(surface, worldPos, screenPosition);
				RenderCapitol(surface, worldPos, screenPosition);
				RenderTrade(surface, worldPos, screenPosition);
			}
			radarPos.x++;
			if (radarPos.x >= m_mapSize.x) {
				radarPos.x = 0;
			}
		}

		radarPos.y++;
		if (radarPos.y >= m_mapSize.y) {
			radarPos.y = 0;
		}
	}
}

void RadarRenderOverlay::RenderSingleTile(aui_Surface & surface, const MapPoint & worldPos,
		const MapPoint & radarPos) const
{
	if (m_player.IsExplored(worldPos)) {
		const MapPointDouble screenPosition = GetScreenPosition(radarPos);
		RenderUnit(surface, worldPos, screenPosition);
		RenderCapitol(surface, worldPos, screenPosition);
		RenderTrade(surface, worldPos, screenPosition);
	}
}

void RadarRenderOverlay::RenderCity(aui_Surface & surface, const MapPoint & worldPos,
		const MapPointDouble & screenPosition) const
{
	if (m_radarProperties.m_displayCities && g_tiledMap->HasVisibleCity(worldPos)) {
		const Pixel16 color = g_colorSet->GetColor(COLOR_WHITE);
		const uint8 alpha = 128;

		sint32 x1 = static_cast<sint32>(screenPosition.x);
		sint32 x2 = x1 + static_cast<sint32>(m_tileSize.x / 2.0);
		sint32 y1 = static_cast<sint32>(screenPosition.y);
		sint32 y2 = y1 + static_cast<sint32>(m_tileSize.y);

		primitives_ClippedPaintRect16(surface, RECT { x1, y1, x2, y2}, color);
		primitives_ClippedPaintRect16(surface, RECT { x1 - 1, y1, x1 - 1, y2 }, color, alpha);
		primitives_ClippedPaintRect16(surface, RECT { x1, y1 - 1, x2, y1 - 1 }, color, alpha);
		primitives_ClippedPaintRect16(surface, RECT { x2 + 1, y1, x2 + 1, y2 }, color, alpha);
		primitives_ClippedPaintRect16(surface, RECT { x1, y2 + 1, x2, y2 + 1 }, color, alpha);
	}
}

void RadarRenderOverlay::RenderUnit(aui_Surface & surface, const MapPoint & worldPos,
		const MapPointDouble & screenPosition) const
{
	Unit unit;
	if (m_radarProperties.m_displayUnits
		&& (g_theWorld->GetTopVisibleUnit(worldPos, unit) || g_theWorld->GetTopRadarUnit(worldPos, unit)))
	{
		COLOR unitColor =  m_radarProperties.m_displayRelations ? RadarTileRelationsColor(worldPos, unit.GetOwner()) :
				g_colorSet->ComputePlayerColor(unit.GetOwner());

		sint32 x1 = static_cast<sint32>(screenPosition.x);
		sint32 x2 = x1 + static_cast<sint32>(m_tileSize.x / 2.0);
		sint32 y1 = static_cast<sint32>(screenPosition.y);
		sint32 y2 = y1 + static_cast<sint32>(m_tileSize.y);

		Pixel16 unitColorPixel = m_radarProperties.m_displayPolitical ? g_colorSet->GetDarkColor(unitColor) : g_colorSet->GetColor(unitColor);
		primitives_ClippedPaintRect16(surface, RECT { x1, y1, x2, y2}, unitColorPixel);
	}
}

void RadarRenderOverlay::RenderCapitol(aui_Surface & surface, const MapPoint & worldPos,
		const MapPointDouble & screenPosition) const
{
	if (!m_radarProperties.m_displayCapitols) {
		return;
	}

	Unit unit;
	if (!g_theWorld->GetTopVisibleUnit(worldPos, unit) || !g_theWorld->GetTopRadarUnit(worldPos, unit)) {
		return;
	}

	if (!unit.IsValid() || !unit.IsCity() || !unit.IsCapitol()) {
		return;
	}

	const Pixel16 color = g_colorSet->GetColor(COLOR_ORANGE);

	sint32 x1 = static_cast<sint32>(screenPosition.x);
	sint32 x2 = x1 + static_cast<sint32>(m_tileSize.x / 2.0);
	sint32 y1 = static_cast<sint32>(screenPosition.y);
	sint32 y2 = y1 + static_cast<sint32>(m_tileSize.y);

	primitives_ClippedPaintRect16(surface, RECT { x1 - 2, y1, x2 + 2, y2}, color);
	primitives_ClippedPaintRect16(surface, RECT { x1, y1 - 2, x2, y2 + 2}, color);
}

void RadarRenderOverlay::RenderTrade(aui_Surface & surface, const MapPoint & worldPos,
		const MapPointDouble & screenPosition) const
{
	if (!m_radarProperties.m_displayTrade) {
		return;
	}

	bool seenTrade= false;
	for (sint32 i = 0; i < g_theWorld->GetCell(worldPos)->GetNumTradeRoutes(); i++)
	{
		TradeRoute route = g_theWorld->GetCell(worldPos)->GetTradeRoute(i);
		if (route.SeenBy(m_player.GetOwner()))
		{
			seenTrade= true;
			break;
		}
	}
	if (!seenTrade) {
		return;
	}

	RECT rect = {
			screenPosition.x, screenPosition.y,
			screenPosition.x + (m_tileSize.x / 2.0),screenPosition.y + m_tileSize.y
	};
	RenderTradeRoute(surface, rect);
}

void RadarRenderOverlay::RenderTradeRoute(aui_Surface & surface, const RECT & rect) const
{
	sint32 midX = (rect.left + rect.right) / 2;
	sint32 midY = (rect.top + rect.bottom) / 2;
	RECT tradeRect = { midX, midY, midX, midY };

	primitives_ClippedPaintRect16(surface, tradeRect, g_colorSet->GetColor(COLOR_YELLOW));
}

//---------------------------------------------------------------------------
//
//	RadarMap::RadarMap
//
//---------------------------------------------------------------------------
RadarMap::RadarMap(AUI_ERRCODE * errCode, sint32 id, const MBCHAR * ldlBlock, ControlActionCallback * ActionFunc,
		void * cookie)
	:
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock),
		aui_Control(errCode, id, ldlBlock, ActionFunc, cookie),
		PatternBase(ldlBlock, NULL)
{
	InitCommonLdl(ldlBlock);
}

//---------------------------------------------------------------------------
//
//	RadarMap::RadarMap
//
//---------------------------------------------------------------------------
RadarMap::RadarMap(AUI_ERRCODE * errCode, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
		const MBCHAR * pattern, ControlActionCallback * ActionFunc, void * cookie)
	:
		aui_ImageBase(0),
		aui_TextBase(NULL),
		aui_Control(errCode, id, x, y, width, height, ActionFunc, cookie),
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
	delete[] m_radarProperties.m_mapOverlay;
	delete m_mapSurface;

	free(m_mapTexture);
}

//---------------------------------------------------------------------------
//
//	RadarMap::InitCommonLdl
//
//---------------------------------------------------------------------------
void RadarMap::InitCommonLdl(const MBCHAR * ldlBlock)
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) {
		return;
	}

	InitCommon();
}

//---------------------------------------------------------------------------
//
//	RadarMap::InitCommon
//
//---------------------------------------------------------------------------
void RadarMap::InitCommon()
{
	m_mapSurface = NULL;
	m_mapSize = NULL;

	m_mapTexture = NULL;

	m_tilePixelWidth = 0.0;
	m_tilePixelHeight = 0.0;

	m_radarProperties.m_displayUnits = g_theProfileDB->GetDisplayUnits() != FALSE;
	m_radarProperties.m_displayCities = g_theProfileDB->GetDisplayCities() != FALSE;
	m_radarProperties.m_displayBorders = g_theProfileDB->GetDisplayBorders() != FALSE;
	m_radarProperties.m_displayOverlay = true;
	m_radarProperties.m_displayTrade = g_theProfileDB->GetDisplayTrade() != FALSE;
	m_radarProperties.m_displayTerrain = g_theProfileDB->GetDisplayTerrain() != FALSE;
	m_radarProperties.m_displayPolitical = g_theProfileDB->GetDisplayPolitical() != FALSE;
	m_radarProperties.m_displayCapitols = g_theProfileDB->GetDisplayCapitols() != FALSE;
	m_radarProperties.m_displayRelations = g_theProfileDB->GetDisplayRelations() != FALSE;

	m_radarProperties.m_mapOverlay = NULL;

	MapPoint resetPos (0,0);
	m_lastCenteredPoint = resetPos;

	m_isInteractive = true;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_mapSurface = aui_Factory::new_Surface(errcode, m_width, m_height);
	Assert(AUI_NEWOK(m_mapSurface, errcode));

	RECT rect = { 0, 0, m_width, m_height };

	if ( m_pattern ) {
		m_pattern->Draw( m_mapSurface, &rect );
	}

	if ( g_theWorld )
	{
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
	delete [] m_radarProperties.m_mapOverlay;
	m_radarProperties.m_mapOverlay = NULL;
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
void RadarMap::SetMapOverlayCell(const MapPoint & pos, COLOR color)
{
	if (!m_radarProperties.m_mapOverlay)
	{
		sint32 len = m_mapSize->x * m_mapSize->y;
		m_radarProperties.m_mapOverlay = new COLOR[len];

		for (sint32 i=0; i<len; i++) {
			m_radarProperties.m_mapOverlay[i] = COLOR_MAX;
		}
	}

	m_radarProperties.m_mapOverlay[pos.x + (pos.y * m_mapSize->x)] = color;
}

//---------------------------------------------------------------------------
//
//	RadarMap::Resize
//
//---------------------------------------------------------------------------
//	- resize the radar map
//
//---------------------------------------------------------------------------
AUI_ERRCODE	RadarMap::Resize(sint32 width, sint32 height)
{
	AUI_ERRCODE errCode = aui_Region::Resize(width, height);
	Assert(errCode == AUI_ERRCODE_OK);

	delete m_mapSurface;
	m_mapSurface = aui_Factory::new_Surface(errCode, width, height);
	Assert(AUI_NEWOK(m_mapSurface, errCode));

	CalculateMetrics();
	RenderMap(m_mapSurface);

	return errCode;
}

//---------------------------------------------------------------------------
//
//	RadarMap::CalculateMetrics
//
//---------------------------------------------------------------------------
//	- calculate some values depending on the current radar map size
//
//---------------------------------------------------------------------------
void RadarMap::CalculateMetrics()
{
	if (!g_theWorld) {
		return;
	}

	free(m_mapTexture);
	m_mapTexture = NULL;

	m_mapSize = g_theWorld->GetSize();

	m_tilePixelWidth = ((double )m_width) / m_mapSize->x;
	m_tilePixelHeight = ((double )m_height) / m_mapSize->y;

	uint32 width = m_mapSize->x * 2;
	uint32 height = m_mapSize->y;

	m_mapTexture = malloc((width + 2) * (height + 4) * sizeof(RadarMapCell::Type));
}

//---------------------------------------------------------------------------
//
//	RadarMap::GetVisiblePlayerToRender
//
//---------------------------------------------------------------------------
//	- Gives back the player that shall be shown currently
//
//---------------------------------------------------------------------------
Player * RadarMap::GetVisiblePlayerToRender()
{
	if (!g_tiledMap || !g_tiledMap->ReadyToDraw() || !g_theWorld || !g_selected_item || !m_mapSize) {
		return NULL;
	}

	Assert(m_mapSize->x < 0 || m_mapSize->y > 0);
	if (m_mapSize->x <= 0 || m_mapSize->y <= 0) {
		return NULL;
	}

	return g_player[g_selected_item->GetVisiblePlayer()];
}

//---------------------------------------------------------------------------
//
//	RadarMap::RenderMap
//
//---------------------------------------------------------------------------
//	- Redraws the complete radar-map
//
//---------------------------------------------------------------------------
void RadarMap::RenderMap(aui_Surface * surface)
{
	Player *player = GetVisiblePlayerToRender();

	if (!player)
	{
		RECT destRect = { 0, 0, surface->Width(), surface->Height() };
		primitives_PaintRect16(surface, &destRect, g_colorSet->GetColor(COLOR_BLACK));
		return;
	}

	uint8 *pSurfBase;
	surface->Lock(NULL, (LPVOID *)&pSurfBase, 0);

	const MapPoint & radarPos = PosWorldToPosRadar(MapPoint(0, 0));

	const RadarRenderWorld radarRenderTile(*player, m_radarProperties, *m_mapSize,
			m_tilePixelWidth, m_tilePixelHeight);
	radarRenderTile.Render(*surface, static_cast<RadarMapCell::Type*>(m_mapTexture),
			MapPoint(m_width, m_height), radarPos);

	if (m_radarProperties.m_displayBorders)
	{
		const RadarRenderPlayerBorder radarRenderPlayerBorder(*player, m_radarProperties, *m_mapSize,
				m_tilePixelWidth, m_tilePixelHeight);
		radarRenderPlayerBorder.Render(*surface, static_cast<RadarMapCell::Type *>(m_mapTexture),
				MapPoint(m_width, m_height), radarPos);
	}

	const RadarRenderOverlay radarRenderOverlay(*player, m_radarProperties, *m_mapSize,
			m_tilePixelWidth,m_tilePixelHeight);
	radarRenderOverlay.Render(*surface, radarPos);

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
void RadarMap::RenderViewRect(aui_Surface & surface, sint32 x, sint32 y)
{
	RECT offsetRect = {0, 0, 0, 0 };

	if (g_tiledMap)
	{
		m_mapViewRect = g_tiledMap->GetMapViewRect();

		if (!g_tiledMap->ReadyToDraw()) {
			return;
		}

		sint32 nrPlayer = g_selected_item->GetVisiblePlayer();

		offsetRect.bottom = m_mapViewRect.bottom;
		offsetRect.top    = m_mapViewRect.top;
		offsetRect.left   = m_mapViewRect.left;
		offsetRect.right  = m_mapViewRect.right;

		offsetRect.top    += m_displayOffset[nrPlayer].y;
		offsetRect.bottom += m_displayOffset[nrPlayer].y;
		if (offsetRect.top + ((offsetRect.bottom - offsetRect.top)/2) > m_mapSize->y)
		{
			// if view is now too far to the bottom subtract the y-map-size
			offsetRect.top    -= m_mapSize->y;
			offsetRect.bottom -= m_mapSize->y;
		}

		offsetRect.left  += m_displayOffset[nrPlayer].x;
		offsetRect.right += m_displayOffset[nrPlayer].x;
		if (offsetRect.left + ((offsetRect.right - offsetRect.left)/2) > m_mapSize->x)
		{
			// if view is now too far to the right subtract the x-map-size
			offsetRect.left  -= m_mapSize->x;
			offsetRect.right -= m_mapSize->x;
		}
	}

	sint32 x1, x2, x3, x4;
	sint32 y1, y2, y3, y4;

	if (m_mapSize)
	{
		sint32 mapWidth  = m_mapSize->x;
		sint32 mapHeight = m_mapSize->y;

		// Set the X coordinate points
		if (!g_theWorld->IsXwrap() && (offsetRect.left < 0 || offsetRect.right > mapWidth))
		{
			x1 = x3 = offsetRect.left;
			x2 = x4 = offsetRect.right;

			if (offsetRect.left < 0) {
				x1 = x3 = 0;
			}
			if (offsetRect.right > mapWidth) {
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
		if (!g_theWorld->IsYwrap() && ( offsetRect.top < 0 || offsetRect.bottom >= mapHeight))
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
	primitives_ClippedLine16(surface, x1, y1, x2, y1, lineColor);
	primitives_ClippedLine16(surface, x1, y1, x1, y2, lineColor);
	primitives_ClippedLine16(surface, x3, y1, x4, y1, lineColor);
	primitives_ClippedLine16(surface, x4, y1, x4, y2, lineColor);
	primitives_ClippedLine16(surface, x1, y4, x2, y4, lineColor);
	primitives_ClippedLine16(surface, x1, y3, x1, y4, lineColor);
	primitives_ClippedLine16(surface, x3, y4, x4, y4, lineColor);
	primitives_ClippedLine16(surface, x4, y3, x4, y4, lineColor);
}

//---------------------------------------------------------------------------
//
//	RadarMap::UpdateMap
//
//---------------------------------------------------------------------------
void RadarMap::UpdateMap(aui_Surface & surface, sint32 x, sint32 y)
{
	RECT destRect = {x, y, x + Width(), y + Height() };
	RECT srcRect  = {0, 0, Width(), Height()};

	LPVOID surfaceBuffer = NULL;
	if (!surface.Buffer())
	{
		surface.Lock(NULL, &surfaceBuffer, 0);
		Assert(surface.Buffer());
	}

	g_c3ui->TheBlitter()->StretchBlt(&surface, &destRect, m_mapSurface, &srcRect, k_AUI_BLITTER_FLAG_COPY);

	if (IsInteractive()) {
		RenderViewRect(surface, x, y);
	}

	if (surfaceBuffer) {
		surface.Unlock(surfaceBuffer);
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::ComputeCenteredMap
//
//---------------------------------------------------------------------------
MapPoint RadarMap::ComputeCenteredMap(const MapPoint & pos, RECT * viewRect)
{
	sint32 width  = viewRect->right - viewRect->left;
	sint32 height = viewRect->bottom - viewRect->top;

	sint32 tileX;
	maputils_MapX2TileX(pos.x, pos.y, &tileX);

	viewRect->left   = tileX - (width >> 1);
	viewRect->top    = (pos.y - (height >> 1)) & (~1);
	viewRect->right  = viewRect->left + width;
	viewRect->bottom = viewRect->top + height;

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
MapPoint RadarMap::CenterMap(const MapPoint & pos)
{
	MapPoint lastPoint = m_lastCenteredPoint;
	if (!lastPoint.IsValid()) {
		lastPoint = pos;
	}

	m_lastCenteredPoint = pos;

	RECT mapViewRect = g_tiledMap->GetMapViewRect();
	ComputeCenteredMap(pos, &mapViewRect);
	g_tiledMap->UpdateAndClipMapViewRect(mapViewRect);
	m_mapViewRect = g_tiledMap->GetMapViewRect();

	RenderMap(m_mapSurface);

	return lastPoint;
}

//---------------------------------------------------------------------------
//
//	RadarMap::Setup
//
//---------------------------------------------------------------------------
void RadarMap::Setup()
{
	CalculateMetrics();

	RenderMap(m_mapSurface);

	if (g_tiledMap) {
		m_mapViewRect = g_tiledMap->GetMapViewRect();
	}
}

//---------------------------------------------------------------------------
//
//	RadarMap::Update
//
//---------------------------------------------------------------------------
void RadarMap::Update()
{
	m_mapSize = g_theWorld->GetSize();

	RenderMap(m_mapSurface);
}

void RadarMap::DoRedrawTile(aui_Surface & surface, const Player & player, const MapPoint & pos)
{
	const MapPoint radarPos = PosWorldToPosRadar(pos);

	const RadarRenderWorld radarRenderTile(player, m_radarProperties, *m_mapSize, m_tilePixelWidth, m_tilePixelHeight);
	radarRenderTile.RenderSingleTile(surface, pos, radarPos);

	if (m_radarProperties.m_displayBorders)
	{
		const RadarRenderPlayerBorder radarRenderPlayerBorder(player, m_radarProperties, *m_mapSize,
				m_tilePixelWidth, m_tilePixelHeight);
		radarRenderPlayerBorder.Render(surface, static_cast<RadarMapCell::Type *>(m_mapTexture),
				MapPoint(m_width, m_height), PosWorldToPosRadar(MapPoint(0, 0)));
	}

	const RadarRenderOverlay radarRenderOverlay(player, m_radarProperties, *m_mapSize,m_tilePixelWidth, m_tilePixelHeight);
	radarRenderOverlay.RenderSingleTile(surface, pos, radarPos);
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

	DoRedrawTile(*m_mapSurface, *player, pos);

	// Draw neighbor crossings
	MapPoint neighborPosition;
	if (pos.GetNeighborPosition(NORTHWEST, neighborPosition)) {
		DoRedrawTile(*m_mapSurface, *player, neighborPosition);
	}
	if (pos.GetNeighborPosition(WEST, neighborPosition)) {
		DoRedrawTile(*m_mapSurface, *player, neighborPosition);
	}
	if (pos.GetNeighborPosition(SOUTHWEST, neighborPosition)) {
		DoRedrawTile(*m_mapSurface, *player, neighborPosition);
	}

	m_mapSurface->Unlock(pSurfBase);
}

//---------------------------------------------------------------------------
//
//	RadarMap::DrawThis
//
//---------------------------------------------------------------------------
AUI_ERRCODE RadarMap::DrawThis(aui_Surface * surface, sint32 x,	sint32 y)
{
	if (IsHidden()) {
		return AUI_ERRCODE_OK;
	}

	if (!surface) {
		surface = m_window->TheSurface();
	}

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect(&rect, m_x + x, m_y + y);
	ToWindow(&rect);

	UpdateMap(*surface, rect.left, rect.top);

	if (surface == m_window->TheSurface()) {
		m_window->AddDirtyRect(&rect);
	}

	return AUI_ERRCODE_OK;
}

//---------------------------------------------------------------------------
//
//	RadarMap::MouseLGrabInside
//
//---------------------------------------------------------------------------
//  - Processes a left mouse click in the radar-map region. Sets therefore
//	  a rectangle which region is seen centered by the point the mouse click
//    was located and call tiled-map methods to refresh the current view.
//---------------------------------------------------------------------------
void RadarMap::MouseLGrabInside(aui_MouseEvent * data)
{
	if (IsDisabled() || !IsInteractive()) {
		return;
	}

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) {
		return;
	}
	SetWhichSeesMouse(this);

	Assert(g_tiledMap);
	if (!g_tiledMap) {
		return;
	}

	data->position.x -= X();
	data->position.y -= Y();

	RECT mapRect = { 0, 0, Width(), Height() };
	if (!PtInRect(&mapRect, data->position)) {
		return;
	}

	g_tiledMap->SetSmoothScrollOffsets(0,0);

	sint32 mapWidth, mapHeight;
	g_tiledMap->GetMapMetrics(&mapWidth, &mapHeight);

	sint32 tileY = (sint32) (data->position.y / m_tilePixelHeight);
	double nudge = (tileY & 1) ? m_tilePixelWidth / 2.0 : 0.0;
	sint32 tileX = (sint32) (ceil(((double)(data->position.x - nudge) / m_tilePixelWidth)));

	tileX = (sint32) ((tileX - m_displayOffset[g_selected_item->GetVisiblePlayer()].x + m_mapSize->x) % m_mapSize->x);
	tileY = (sint32) ((tileY - m_displayOffset[g_selected_item->GetVisiblePlayer()].y + m_mapSize->y) % m_mapSize->y);

	sint32 width  = m_mapViewRect.right - m_mapViewRect.left;
	sint32 height = m_mapViewRect.bottom - m_mapViewRect.top;

	m_mapViewRect.left   = tileX - (width / 2);
	m_mapViewRect.right  = m_mapViewRect.left + width;
	m_mapViewRect.top    = (tileY - (height / 2)) & ~0x01;
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
void RadarMap::MouseRGrabInside(aui_MouseEvent * data)
{
	if (IsDisabled() || !IsInteractive()) {
		return;
	}

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) {
		return;
	}
	SetWhichSeesMouse(this);

	Assert(g_tiledMap);
	if (!g_tiledMap) {
		return;
	}

	data->position.x -= X();
	data->position.y -= Y();

	sint32 nrPlayer = g_selected_item->GetVisiblePlayer();

	// compute the offsets after the MouseRClick to center the map with the desired point
	if (g_theWorld->IsXwrap())
	{
		m_displayOffset[nrPlayer].x =
				(m_mapSize->x - ( m_mapSize->x * data->position.x / m_width) + (m_mapSize->x / 2)
				+ m_mapSize->x + m_displayOffset[nrPlayer].x) % m_mapSize->x;
	} else {
		m_displayOffset[nrPlayer].x = 0;
	}

	if (g_theWorld->IsYwrap())
	{
		m_displayOffset[nrPlayer].y =
				(m_mapSize->y - ((( m_mapSize->y * data->position.y / m_height) >>1)<<1)
				+ (m_mapSize->y / 2) + m_mapSize->y + m_displayOffset[nrPlayer].y) % m_mapSize->y;
	} else {
		m_displayOffset[nrPlayer].y = 0;
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
AUI_ERRCODE RadarMap::Idle()
{
	static uint32 lastDraw = 0;
	if (GetTickCount() - lastDraw > 100) {
		lastDraw = GetTickCount();
	}
	else {
		return AUI_ERRCODE_OK;
	}

	DrawThis(NULL, 0, 0);

	return AUI_ERRCODE_OK;
}

//---------------------------------------------------------------------------
//
// RadarMap::PosWorldToPosRadarCorrectedX
//
//---------------------------------------------------------------------------
// - Calculates from a given world point the corresponding point of the
//   radar map for handling the offset of the X and/or the Y axis
//
//---------------------------------------------------------------------------
MapPoint RadarMap::PosWorldToPosRadar(const MapPoint & worldPos)
{
	sint32 nrPlayer = g_selected_item->GetVisiblePlayer();

	MapPoint posRadar;
	posRadar.x = (worldPos.x - m_displayOffset[nrPlayer].y / 2 + m_mapSize->x + m_displayOffset[nrPlayer].x)
			% m_mapSize->x;
	posRadar.y = (worldPos.y + m_displayOffset[nrPlayer].y) % m_mapSize->y;

	return posRadar;
}
