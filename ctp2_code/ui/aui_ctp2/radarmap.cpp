






#include "c3.h"


#include "radarmap.h"


#include <algorithm>


#include "aui.h"
#include "aui_directsurface.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_action.h"
#include "c3ui.h"
#include "Player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "UnseenCell.h"
#include "CityData.h"
#include "Unit.h"
#include "UnitData.h"
#include "pixelutils.h"
#include "ColorSet.h"
#include "SelItem.h"
#include "TiledMap.h"
#include "Director.h"
#include "maputils.h"
#include "primitives.h"
#include "ProfileDB.h"
#include "PointerList.h"
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


static const unsigned char k_EAST_BORDER_FLAG		= 0x01;
static const unsigned char k_WEST_BORDER_FLAG		= 0x02;
static const unsigned char k_NORTH_EAST_BORDER_FLAG	= 0x04;
static const unsigned char k_NORTH_WEST_BORDER_FLAG	= 0x08;
static const unsigned char k_SOUTH_EAST_BORDER_FLAG	= 0x10;
static const unsigned char k_SOUTH_WEST_BORDER_FLAG	= 0x20;


RadarMap::RadarMap(AUI_ERRCODE *retval,
							sint32 id,
							MBCHAR *ldlBlock,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:	aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock),
		PatternBase(ldlBlock, NULL)
{
	InitCommonLdl(ldlBlock);
}


RadarMap::RadarMap(AUI_ERRCODE *retval,
							uint32 id,
							sint32 x,
							sint32 y,
							sint32 width,
							sint32 height,
							MBCHAR *pattern,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:	aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
		aui_ImageBase((sint32)0),
		aui_TextBase((MBCHAR *)NULL),
		PatternBase(pattern)
{
	InitCommon();	
}

RadarMap::~RadarMap()
{
	if (m_mapSurface)
		delete m_mapSurface;
	if (m_tempSurface) 
		delete m_tempSurface;
	if (m_tempBuffer) 
		free(m_tempBuffer);
}


void RadarMap::InitCommonLdl(MBCHAR *ldlBlock)
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

	
	m_mapSurface = new aui_DirectSurface(&errcode, m_width, m_height, 16, g_c3ui->DD());
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


void RadarMap::ClearMapOverlay(void)
{
	delete[] m_mapOverlay;
	m_mapOverlay = NULL;
}


void RadarMap::SetMapOverlayCell(MapPoint &pos, COLOR color)
{
	if (m_mapOverlay == NULL) {
		sint32 len = m_mapSize->x * m_mapSize->y;
		m_mapOverlay = new COLOR[len];

		
		for (sint32 i=0; i<len; i++) 
			m_mapOverlay[i] = COLOR_MAX;
	}

	m_mapOverlay[pos.x + (pos.y * m_mapSize->x)] = color;
}



AUI_ERRCODE	RadarMap::Resize( sint32 width, sint32 height )
{
	AUI_ERRCODE		errcode;

	errcode = aui_Region::Resize(width, height);
	Assert(errcode == AUI_ERRCODE_OK);

	if (m_mapSurface)
		delete m_mapSurface;

	
	m_mapSurface = new aui_DirectSurface(&errcode, width, height, 16, g_c3ui->DD());
	Assert( AUI_NEWOK(m_mapSurface, errcode) );

	CalculateMetrics();

	
	RenderMap(m_mapSurface);

	return errcode;
}


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


POINT RadarMap::MapToPixel(sint32 x, sint32 y)
{
	POINT		pt;
	sint32		k;
	double		nudge;

	k = sint32(ceil(y/2) + x) % m_mapSize->x;

	nudge = 0;
	if (y&1) {
		nudge = m_tilePixelWidth / 2.0;
	}
	
	pt.x = (sint32)(k * m_tilePixelWidth);
	pt.y = (sint32)(y * m_tilePixelHeight);

	return pt;
}


POINT RadarMap::MapToPixel(MapPoint *pos)
{
	return MapToPixel(pos->x, pos->y);
}




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




Pixel16 RadarMap::RadarTileColor(const Player *player, const MapPoint &position, uint32 &flags)
{
	
	Unit unit;

	flags = 0;

	
	if(player->m_vision->IsExplored(position)) {
		if(m_displayTrade && g_theWorld->GetCell(position)->GetNumTradeRoutes() > 0) {
			flags = 1;
		}


		
		
		if(m_displayOverlay && m_mapOverlay) {
			COLOR color = m_mapOverlay[position.y * m_mapSize->x + position.x];
			if(color != COLOR_MAX)	
				return(g_colorSet->GetColor(color));
		}

		
		
		if(!g_theWorld->GetTopVisibleUnit(position, unit))
			g_theWorld->GetTopRadarUnit(position, unit);

		
		if(unit.IsValid()) {
			
			if(unit.IsCity()) {
				
				if(m_displayCities) {
					
					
					if(unit.m_id == m_selectedCity.m_id)
						return(g_colorSet->GetColor(COLOR_RED));
					else
						return(g_colorSet->GetPlayerColor(unit.GetOwner()));
				} else {
					
					
					
					
					unit.m_id = 0;
					g_theWorld->GetTopVisibleUnitNotCity(position, unit);
				}
			}

			
			
			
			if(m_displayUnits && unit.m_id)
				return(g_colorSet->GetPlayerColor(unit.GetOwner()));
		}

		
		UnseenCellCarton unseenCellCarton;
		if(player->m_vision->GetLastSeen(position, unseenCellCarton)) {
			if(m_displayTerrain) {
				return(g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
															   unseenCellCarton.m_unseenCell->GetTerrainType())));	
			} else {
				if(g_theWorld->IsLand(position) || g_theWorld->IsMountain(position)) {
					return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
																   TERRAIN_GRASSLAND));
				} else {
					return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
																   TERRAIN_WATER_DEEP));
				}
			}
		}
					

		Cell *cell = g_theWorld->GetCell(position);
		if(m_displayTerrain) {
			
			return(g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
														   cell->GetTerrainType())));
		} else {
			if(g_theWorld->IsLand(position) || g_theWorld->IsMountain(position)) {
				return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
															   TERRAIN_GRASSLAND));
			} else {
				return g_colorSet->GetColor(static_cast<COLOR>(COLOR_TERRAIN_0 +
															   TERRAIN_WATER_DEEP));
			}
		}
	}

	
	
	
	if(g_theWorld->GetTopRadarUnit(position, unit))
		return(g_colorSet->GetPlayerColor(unit.GetOwner()));
	return(g_colorSet->GetColor(COLOR_BLACK));
}



Pixel16 RadarMap::RadarTileBorderColor(const MapPoint &position)
{
	
	sint32 owner = g_theWorld->GetCell(position)->GetOwner();
	if(owner < 0)
		return(g_colorSet->GetColor(COLOR_BLACK));

	
	return(g_colorSet->GetPlayerColor(owner));
}



uint8 RadarMap::RadarTileBorder(const Player *player, const MapPoint &position)
{
	
	uint8 borderFlags = 0;

	
	if(!m_displayBorders)
		return(borderFlags);

	
	if(!player->m_vision->IsExplored(position))
		return(borderFlags);

	
	sint32 owner = g_theWorld->GetCell(position)->GetOwner();
	if(owner < 0)
		return(borderFlags);

	
	if(owner != player->m_owner && 
	   !player->m_hasGlobalRadar &&
	   !Scheduler::CachedHasContactWithExceptSelf(player->m_owner, owner))
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

	
	tileRectangle.right = std::_MAX(tileRectangle.left, (tileRectangle.right - 1L));
	tileRectangle.bottom = std::_MAX(tileRectangle.top, (tileRectangle.bottom - 1L));

	
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

	
	tileRectangle.right = std::_MAX(tileRectangle.left, (tileRectangle.right - 1L));

	
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

	
	sint32 middle = ceil(xPosition + m_tilePixelWidth/2);

	
	tileRectangle.right = std::_MAX(tileRectangle.left, (tileRectangle.right - 1L));
	tileRectangle.bottom = std::_MAX(tileRectangle.top, (tileRectangle.bottom - 1L));
	middle = std::_MIN(middle, tileRectangle.right);

	
	
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



void RadarMap::RenderMapTile(aui_Surface *surface, const MapPoint &screenPosition, Pixel16 color, uint32 flags)
{
	
	
	if((screenPosition.y & 1) && (screenPosition.x == (m_mapSize->x - 1)))
		RenderSpecialTile(surface, screenPosition, color, flags);
	else	
		RenderNormalTile(surface, screenPosition, color, flags);
}



void RadarMap::RenderMapTileBorder(aui_Surface *surface, const MapPoint &screenPosition,
							 uint8 borderFlags, Pixel16 borderColor)
{
	
	
	if((screenPosition.y & 1) && (screenPosition.x == (m_mapSize->x - 1)))
		RenderSpecialTileBorder(surface, screenPosition, borderFlags, borderColor);
	else	
		RenderNormalTileBorder(surface, screenPosition, borderFlags, borderColor);
}






void RadarMap::RenderTile(aui_Surface *surface, const MapPoint &position,
						  Player *player)
{
	uint32 flags;

	
	Pixel16 color = RadarTileColor(player, MapPoint(position.x, position.y), flags);

	
	sint32 x = static_cast<sint32>(ceil(position.y / 2) + position.x)
		% m_mapSize->x;

	
	RenderMapTile(surface, MapPoint(x, position.y), color, flags);
}

void RadarMap::RenderTileBorder(aui_Surface *surface, const MapPoint &position,
						  Player *player)
{
	
	Pixel16 borderColor = RadarTileBorderColor(MapPoint(position.x, position.y));
	uint8 borderFlags = RadarTileBorder(player, MapPoint(position.x, position.y));

	
	sint32 x = static_cast<sint32>(ceil(position.y / 2) + position.x)
		% m_mapSize->x;

	
	if (borderFlags)
		RenderMapTileBorder(surface, MapPoint(x, position.y), borderFlags, borderColor);
}

void RadarMap::RenderTrade(aui_Surface *surface, const MapPoint &position, Player *player)
{
	if(!m_displayTrade)
		return;

	
	
	MapPoint screenPosition((sint32)(ceil(position.y / 2) + position.x) % (m_mapSize->x), position.y);
		 
	if(!g_theWorld->GetCell(position)->GetNumTradeRoutes() ||
	   !player->m_vision->IsExplored(position)) {
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
			RenderTile(m_tempSurface, MapPoint(x, y), player);	
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
			RenderTileBorder(surface, MapPoint(x, y), player);	
			RenderTrade(surface, MapPoint(x, y), player);
		}
}


void RadarMap::RenderViewRect
(
	aui_Surface *surf, 
	sint32 x,	
	sint32 y	
)
{
	if (g_tiledMap) {
		RECT *temp = g_tiledMap->GetMapViewRect();
		m_mapViewRect = *temp;

		if(!g_tiledMap->ReadyToDraw())
			return;
	}

		
	sint32 x1,x2,x3,x4;
	sint32 y1,y2,y3,y4;

	

	if ( m_mapSize ) 
	{
		sint32 mapWidth = m_mapSize->x;
		sint32 mapHeight = m_mapSize->y;









		
		if (!g_theWorld->IsXwrap() && (m_mapViewRect.left < 0 || m_mapViewRect.right > mapWidth)) 
		{
			x1 = x3 = m_mapViewRect.left;
			x2 = x4 = m_mapViewRect.right;
			if (m_mapViewRect.left < 0) 
			{
				x1 = x3 = 0;
			}
			if (m_mapViewRect.right > mapWidth) 
			{
				x2 = x4 = mapWidth;
			}
		}

		
		else 
		{
			
			if (m_mapViewRect.left < 0) 
			{
				x1 = m_mapViewRect.left + mapWidth;
				x2 = mapWidth;

				
				if (m_mapViewRect.right <= 0) 
				{
					x3 = mapWidth;
					x4 = mapWidth;
				}
				
				else 
				{
					x3 = 0;
					x4 = m_mapViewRect.right;
				}
			}
			
			else 
			{
				
				if (m_mapViewRect.right > mapWidth) 
				{
					x1 = m_mapViewRect.left;
					x2 = mapWidth;
					x3 = 0;
					x4 = m_mapViewRect.right - mapWidth;
				}
				
				
				else 
				{
					x1 = m_mapViewRect.left;
					x2 = m_mapViewRect.right;
					x3 = m_mapViewRect.left;
					x4 = m_mapViewRect.right;
				}
			} 
		} 





		x1 = (sint32) (x1 * m_tilePixelWidth) + x;
		x2 = (sint32) (x2 * m_tilePixelWidth) + x - 1;
		x3 = (sint32) (x3 * m_tilePixelWidth) + x;
		x4 = (sint32) (x4 * m_tilePixelWidth) + x - 1;

		
		if ( !g_theWorld->IsYwrap() && ( m_mapViewRect.top < 0 || m_mapViewRect.bottom >= mapHeight)) 
		{
			
			y1 = y3 = m_mapViewRect.top;
			y2 = y4 = m_mapViewRect.bottom;
			if (m_mapViewRect.top < 0) 
			{
				y1 = y3 = 0;
			}

			
			if (m_mapViewRect.bottom > mapHeight) 
			{
				y2 = y4 = mapHeight;
			}
		}

		
		else 
		{

			if ( m_mapViewRect.top < 0 )
			{
				y1 = m_mapViewRect.top + mapHeight;
				y2 = mapHeight;
				if (m_mapViewRect.bottom <= 0) 
				{
					y3 = mapHeight;
					y4 = mapHeight;
				}
				else 
				{
					y3 = 0;
					y4 = m_mapViewRect.bottom;
				}
			}
			else
			{
				if (m_mapViewRect.bottom > mapHeight)
				{
					y1 = m_mapViewRect.top;
					y2 = mapHeight;
					y3 = 0;
					y4 = m_mapViewRect.bottom - mapHeight;
				}
				else 
				{
					y1 = m_mapViewRect.top;
					y2 = m_mapViewRect.bottom;
					y3 = m_mapViewRect.top;
					y4 = m_mapViewRect.bottom;
				}
			}
		}




		y1 = (sint32) (y1 * m_tilePixelHeight) + y;
		y2 = (sint32) (y2 * m_tilePixelHeight) + y - 1;
		y3 = (sint32) (y3 * m_tilePixelHeight) + y;
		y4 = (sint32) (y4 * m_tilePixelHeight) + y - 1;
	}
	else 
	{
		x1 = x3 = 0;
		x2 = x4 = 0;
		y1 = y3 = 0;
		y2 = y4 = 0;		
	}

	













	
	primitives_DrawLine16(surf,x1,y1,x2,y1,0xffff);
	primitives_DrawLine16(surf,x1,y1,x1,y2,0xffff);
	primitives_DrawLine16(surf,x3,y1,x4,y1,0xffff);
	primitives_DrawLine16(surf,x4,y1,x4,y2,0xffff);
	primitives_DrawLine16(surf,x1,y4,x2,y4,0xffff);
	primitives_DrawLine16(surf,x1,y3,x1,y4,0xffff);
	primitives_DrawLine16(surf,x3,y4,x4,y4,0xffff);
 	primitives_DrawLine16(surf,x4,y3,x4,y4,0xffff);
}


void RadarMap::UpdateMap(aui_Surface *surf, sint32 x, sint32 y)
{
	RECT		destRect = {x, y, x + Width(), y + Height() };
	RECT		srcRect = {0, 0, Width(), Height()};

	
	g_c3ui->TheBlitter()->StretchBlt(surf, &destRect, m_mapSurface, &srcRect, k_AUI_BLITTER_FLAG_COPY);

	
	if(IsInteractive())
		RenderViewRect(surf, x, y);	
}




MapPoint RadarMap::ComputeCenteredMap(MapPoint &pos, RECT *viewRect)
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


MapPoint RadarMap::CenterMap(MapPoint &pos)
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


void RadarMap::Setup(void)
{
	CalculateMetrics();

	
	RenderMap(m_mapSurface);

	if (g_tiledMap) {
		RECT *realMapViewRect = g_tiledMap->GetMapViewRect();
		m_mapViewRect = *realMapViewRect;
	}
}


void RadarMap::Update( void )
{
	
	m_mapSize = g_theWorld->GetSize();

	RenderMap(m_mapSurface);
}


void RadarMap::RedrawTile( const MapPoint *point )
{
	Player *player = GetVisiblePlayerToRender();
	if(!player)
		return;

	RenderTile(m_tempSurface, *point, player);

	fRect sRect, dRect;
	sint32 x0 = (point->y + 2 * point->x) % (2 * m_mapSize->x);
	float adjust = m_filter ? 0.5 : 0.0;
	if (x0 == 2 * m_mapSize->x - 1)
	{
		
		sRect.left = x0 - adjust;
		sRect.right = x0 + 1;
		sRect.top = point->y - adjust;
		sRect.bottom = point->y + 1 + adjust;
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
		sRect.top = point->y - adjust;
		sRect.bottom = point->y + 1 + adjust;
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

	
	RenderTileBorder(m_mapSurface, *point, player);

	
	RenderTrade(m_mapSurface, *point, player);

	if (m_filter)
	{
		MapPoint neighbor;
		if (point->GetNeighborPosition(NORTHEAST, neighbor)) RenderTileBorder(m_mapSurface, neighbor, player);
		if (point->GetNeighborPosition(NORTHWEST, neighbor)) RenderTileBorder(m_mapSurface, neighbor, player);
		if (point->GetNeighborPosition(EAST, neighbor)) RenderTileBorder(m_mapSurface, neighbor, player);
		if (point->GetNeighborPosition(WEST, neighbor)) RenderTileBorder(m_mapSurface, neighbor, player);
		if (point->GetNeighborPosition(SOUTHEAST, neighbor)) RenderTileBorder(m_mapSurface, neighbor, player);
		if (point->GetNeighborPosition(SOUTHWEST, neighbor)) RenderTileBorder(m_mapSurface, neighbor, player);
	}
}


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

	RenderMap(m_mapSurface);

}



AUI_ERRCODE RadarMap::Idle( void )
{
	static uint32 lastDraw = 0;
	if (GetTickCount() - lastDraw > 100) lastDraw = GetTickCount();
	else return AUI_ERRCODE_OK;

	
	DrawThis(NULL, 0, 0);

	return AUI_ERRCODE_OK;
}

