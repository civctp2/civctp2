//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of resource map (city window subscreen)
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
// - Wrapping corrected.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui.h"
#include "aui_Blitter.h"
#include "aui_DirectSurface.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_StringTable.h"

#include "primitives.h"
#include "Globals.h"
#include "Player.h"

#include "DynArr.h"
#include "SelItem.h"

#include "Director.h"

#include "TiledMap.h"
#include "BaseTile.h"
#include "TileInfo.h"
#include "TileSet.h"
#include "ColorSet.h"

#include "ResourceMap.h"
#include "Unit.h"
#include "UnitPool.h"

#include "c3_updateaction.h"

#include "Actor.h"
#include "UnitActor.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "MapPoint.h"
#include "WonderRecord.h"
#include "c3ui.h"
#include "GoodActor.h"

#include "CityData.h"
#include "textutils.h"

#include "maputils.h"
#include "SlicEngine.h"
#include "ProfileDB.h"


#include "CityRadius.h"
#include "StrDB.h"
#include "UnitData.h"

#include "GameEventManager.h"

#include "CityInfluenceIterator.h"

#include "ldl_user.h"

#define k_NUDGE		48					


#define k_CITYNAME_PTSIZE		12
#define k_POP_PTSIZE			10
#define k_POP_BOX_XOFFSET		3
#define k_POP_BOX_YOFFSET		5
#define k_POP_BOX_SIZE			10
#define k_POP_BOX_SIZE_MINIMUM	4
#define k_FONT					0

#define k_RES_PTSIZE			8

#define k_OFFSET_WIDTH			62




#define k_MAX_CITY_RADIUS       3

extern TiledMap			*g_tiledMap;
extern Player			**g_player;
extern SelectedItem		*g_selected_item;
extern Director			*g_director;
extern ColorSet			*g_colorSet;
extern World			*g_theWorld;
extern C3UI				*g_c3ui;
extern UnitPool			*g_theUnitPool;
extern ProfileDB		*g_theProfileDB;
extern StringDB			*g_theStringDB;





ResourceMap::ResourceMap(AUI_ERRCODE *retval,
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


ResourceMap::ResourceMap(AUI_ERRCODE *retval,
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
	InitCommon( k_RESOURCEMAP_DEFAULT_SCALE );	
}


ResourceMap::~ResourceMap()
{
	
	if (m_updateAction) {
		delete m_updateAction;
		m_updateAction = NULL;
	}

	if (m_surface) {
		delete m_surface;
		m_surface = NULL;
	}
}


void ResourceMap::InitCommonLdl(MBCHAR *ldlBlock)
{
	sint32 scale = k_RESOURCEMAP_DEFAULT_SCALE;
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return;

	if (block->GetAttributeType( k_RESOURCEMAP_LDL_SCALE ) == ATTRIBUTE_TYPE_INT) {
		scale = block->GetInt( k_RESOURCEMAP_LDL_SCALE );
	}

	InitCommon( scale );
}


void ResourceMap::InitCommon( sint32 scale)
{
	AUI_ERRCODE			errcode;

	m_scale = scale;
	m_drawHilite = FALSE;

	
	m_unit = NULL;

	
	m_updateAction = NULL;

	m_surface = new aui_DirectSurface( &errcode, 
									   g_tiledMap->GetZoomTilePixelWidth() * ((k_MAX_CITY_RADIUS * 2) + 1),
									   g_tiledMap->GetZoomTilePixelHeight() * ((k_MAX_CITY_RADIUS * 2) + 2),
									   16, (g_c3ui)->DD());
	

	Assert( m_surface != NULL );
	if ( !m_surface ) return;

	m_string = new aui_StringTable( &errcode, "ResourceMapStrings" );
	Assert( m_string );
	if ( !m_string ) return;
}

AUI_ERRCODE ResourceMap::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	
	


	UpdateFromSurface(surface, &rect);

	if (surface == m_window->TheSurface()) {
		m_window->AddDirtyRect(&rect);
	}

	return AUI_ERRCODE_OK;
}


void ResourceMap::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	
	SetWhichSeesMouse(this);

	
	mouseData->position.x -= X();
	mouseData->position.y -= Y();

	Click(mouseData);
	DrawSurface();
}

void ResourceMap::MouseMoveInside(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

    Assert(data); 

	POINT temp = data->position;
	temp.x -= X();
	temp.y -= Y();

    MapPoint tmp;
    if (MousePointToTilePos(temp, tmp)){
        m_current_mouse_tile = tmp; 
    }

	SetHiliteMouseTile(m_current_mouse_tile);

	m_drawHilite = TRUE;
	g_tiledMap->DrawHilite( FALSE );
}

void ResourceMap::MouseMoveAway(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

    Assert(data); 

	m_drawHilite = FALSE;
}





void ResourceMap::NotifyPopChanged()
{
	DrawSurface();
}







sint32 ResourceMap::DrawSurface(void)
{
	sint32 width = m_surface->Width();
	sint32 height = m_surface->Height();

	
	RECT rect = {0,0,width,height};
	primitives_PaintRect16(m_surface,&rect,0x0000);

	MapPoint pos;
	MapPoint newpos;

	
	if ( !g_theUnitPool->IsValid(m_unit) ) return -1;

	
	if (m_unit.m_id)
		m_unit.GetData()->GetPos(pos);
	else
		return 0;

	
	double scale = g_tiledMap->GetScale();
	sint32 zoomLevel = g_tiledMap->GetZoomLevel();

	
	if ( m_scale ) {
		g_tiledMap->SetZoomLevel(k_ZOOM_SMALLEST );
	}
	else {
		g_tiledMap->SetZoomLevel(k_ZOOM_NORMAL );
	}



	
	
	
	MapPoint	myPos1 = pos, 
				myPos2;
	sint32		leftEdge, topEdge; 
	
	g_tiledMap->RecalculateViewRect(m_normalizedViewRect);

	maputils_MapXY2PixelXY(myPos1.x, myPos1.y, &leftEdge, &topEdge, &m_normalizedViewRect);
	leftEdge -= g_tiledMap->GetZoomTilePixelWidth() * k_MAX_CITY_RADIUS;
	topEdge -= g_tiledMap->GetZoomTilePixelHeight() * k_MAX_CITY_RADIUS;

	m_leftEdge = leftEdge;
	m_topEdge = topEdge;

	g_tiledMap->LockThisSurface(m_surface);

	m_usedRect.left = m_usedRect.top = m_usedRect.right = m_usedRect.bottom = -1;

	DrawResourceMapThing(m_surface, DrawATile);

	g_tiledMap->UnlockSurface();

	
	g_tiledMap->SetZoomLevel(zoomLevel);
	g_tiledMap->SetScale( scale );

	g_tiledMap->GetMapBounds( m_mapBounds );

	m_mapViewRect.left = 3;
	m_mapViewRect.right = 7;
	m_mapViewRect.top = 2;
	m_mapViewRect.bottom = 9;

	DrawSprites(m_surface, NULL);

	return 0;
}

sint32 ResourceMap::DrawSpaceImprovements( aui_Surface *pSurface, sint32 xOff, sint32 yOff )
{
	MapPoint pos;
	MapPoint newpos;

	
	if (m_unit.m_id)
		m_unit.GetData()->GetPos(pos);
	else
		return 0;

	
	double scale = g_tiledMap->GetScale();
	sint32 zoomLevel = g_tiledMap->GetZoomLevel();

	
	if ( m_scale ) {
		g_tiledMap->SetZoomLevel(k_ZOOM_SMALLEST );
	}
	else {
		g_tiledMap->SetZoomLevel(k_ZOOM_NORMAL );
	}

	sint32 i;

	
	for (sint32 j =0;j < 3;j++) {
		pos.GetNeighborPosition(NORTHWEST, newpos);
		pos = newpos;
	}

	
	g_tiledMap->GetMapBounds( m_mapBounds );
	sint32 tileX;
	maputils_MapX2TileX(pos.x, pos.y, &tileX);
	m_mapViewRect.left = 3;
	m_mapViewRect.right = 7;
	m_mapViewRect.top = 2;
	m_mapViewRect.bottom = 9;
	
	sint32 nudge;
	sint32 index = 0;
	sint32 frame = 0;

	g_tiledMap->LockThisSurface(pSurface);

	
	for (sint32 y = 0;y < 7;y++) {
		if (y & 0x01) {
			if ( !m_scale ) {
				nudge = k_NUDGE;
			}
			else {
				nudge = k_NUDGE / 2;
			}
			pos.GetNeighborPosition(SOUTHWEST, newpos);
		}
		else {
			pos.GetNeighborPosition(SOUTHEAST, newpos);
			nudge = 0;
		}
		maputils_MapX2TileX(pos.x,pos.y,&i);

		for (sint32 x = 0;x < 3;x++) {
			if (x==0 && (y==0 || y==6)) continue;
			if ( !m_scale )
				DrawImprovements(pSurface, pos.y, i+x, x*96+nudge+xOff,y*24+yOff);
			else
				DrawImprovements(pSurface, pos.y, i+x, x*48+nudge+xOff,y*12+yOff);


			sint32 mapX = maputils_TileX2MapX(i+x,pos.y);
			MapPoint tempPos( mapX, pos.y);
			Cell *cell = g_theWorld->GetCell(tempPos);
			
			index++;
		}
		if (y==2 || y==4) {
			if ( !m_scale )
				DrawImprovements(pSurface, pos.y, i+x, x*96+nudge+xOff,y*24+yOff);
			else
				DrawImprovements(pSurface, pos.y, i+x, x*48+nudge+xOff,y*12+yOff);

			sint32 mapX = maputils_TileX2MapX(i+x,pos.y);
			MapPoint tempPos (mapX, pos.y);
			Cell *cell = g_theWorld->GetCell(tempPos);
			
			index++;
		}
		pos = newpos;
	}

	g_tiledMap->UnlockSurface();

	
	g_tiledMap->SetZoomLevel(zoomLevel);
	g_tiledMap->SetScale( scale );

	return TRUE;
}

BOOL ResourceMap::DrawACity(aui_Surface *pSurface, MapPoint &pos, void *context)
{
	ResourceMap		*resourceMap = (ResourceMap *)context;
	UnitActor	*actor;
	sint32		x, y;
	Unit		city;
	sint32		mapWidth, mapHeight;


	city = g_theWorld->GetCell(pos)->GetCity();
	if (city.m_id == 0) return FALSE;

	actor = city.GetActor();
	if (!actor) return FALSE;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);
	
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, resourceMap->GetNormalizedViewRect());

	x -= resourceMap->GetLeftEdge();
	y -= resourceMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);


	if ( !resourceMap->GetScale() ) {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
	}
	else {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
	}
	
	return TRUE;
}

BOOL ResourceMap::DrawALandCity(aui_Surface *pSurface, MapPoint &pos, void *context)
{
	ResourceMap		*resourceMap = (ResourceMap *)context;
	UnitActor	*actor;
	sint32		x, y;
	Unit		city;
	sint32		mapWidth, mapHeight;


	city = g_theWorld->GetCell(pos)->GetCity();
	if (city.m_id == 0) return FALSE;

	actor = city.GetActor();
	if (!actor) return FALSE;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);
	
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, resourceMap->GetNormalizedViewRect());

	x -= resourceMap->GetLeftEdge();
	y -= resourceMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);

	if ( !resourceMap->GetScale() ) {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
	}
	else {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
	}
	
	return TRUE;
}


BOOL ResourceMap::DrawAGood(aui_Surface *pSurface, MapPoint &pos, void *context)
{
	ResourceMap		*resourceMap = (ResourceMap *)context;
	GoodActor	*goodActor;
	sint32		mapWidth, mapHeight;
	sint32		x, y;

	TileInfo *curTileInfo = g_tiledMap->GetTileInfo(pos);
	Assert(curTileInfo != NULL);
	if(!curTileInfo || !curTileInfo->HasGoodActor()) return FALSE;
	
	goodActor = curTileInfo->GetGoodActor();

	if (!goodActor) return FALSE;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);
	
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, resourceMap->GetNormalizedViewRect());

	x -= resourceMap->GetLeftEdge();
	y -= resourceMap->GetTopEdge();


	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);

	if ( !resourceMap->GetScale() ) {
		goodActor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
	}
	else {
		goodActor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
	}

	return TRUE;
}

BOOL ResourceMap::DrawATile(aui_Surface *pSurface, MapPoint &pos, void *context)
{
	ResourceMap		*resourceMap = (ResourceMap *)context;
	sint32		x, y;
	sint32		mapWidth, mapHeight;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);
	
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, resourceMap->GetNormalizedViewRect());

	x -= resourceMap->GetLeftEdge();
	y -= resourceMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);

	if(x >= mapWidth * g_tiledMap->GetZoomTilePixelWidth()) {
		
		
		
		x -= mapWidth * g_tiledMap->GetZoomTilePixelWidth();
	}
#if !defined(ACTIVISION_ORIGINAL)
	if (y >= mapHeight * g_tiledMap->GetZoomTilePixelHeight() / 2)
	{
		y -= mapHeight * g_tiledMap->GetZoomTilePixelHeight() / 2;
	}
#endif

	sint32 tileX;
	maputils_MapX2TileX(pos.x, pos.y, &tileX);

	resourceMap->CalculateWrap(pSurface, pos.y, tileX, x, y);

	if(resourceMap->m_usedRect.left < 0 || x < resourceMap->m_usedRect.left) {
		resourceMap->m_usedRect.left = x;
	}

	if(resourceMap->m_usedRect.right < 0 || x + g_tiledMap->GetZoomTilePixelWidth() > resourceMap->m_usedRect.right) {
		resourceMap->m_usedRect.right = x + g_tiledMap->GetZoomTilePixelWidth();
	}

	if(resourceMap->m_usedRect.top < 0 || y < resourceMap->m_usedRect.top) {
		resourceMap->m_usedRect.top = y;
	}

	if(resourceMap->m_usedRect.bottom < 0 ||  y + g_tiledMap->GetZoomTilePixelHeight() > resourceMap->m_usedRect.bottom) {
		resourceMap->m_usedRect.bottom = y + g_tiledMap->GetZoomTilePixelHeight();
	}


	Cell	*cell = g_theWorld->GetCell(pos);

	if (cell) {
		Unit	c;
		
		resourceMap->GetOwningCity(c);

		if (cell->GetOwner() != c.GetOwner()) {
			if ( !resourceMap->GetScale() ) {
				g_tiledMap->DrawTileBorder(pSurface, x, y, g_colorSet->GetPlayerColor(cell->GetOwner()));
			}
			else {
				g_tiledMap->DrawTileBorderScaled(pSurface, pos, x, y, 
						g_tiledMap->GetZoomTilePixelWidth(), 
						g_tiledMap->GetZoomTilePixelHeight(),
						g_colorSet->GetPlayerColor(cell->GetOwner()));
			}
		}

		
	}

	return TRUE;
}

BOOL ResourceMap::DrawResourceMapThing(aui_Surface *pSurface, ResourceMapDrawFunc *func)
{
	MapPoint	pos = m_unit.RetPos();
	MapPoint	wpos;

	
	
	
	sint32 zoomLevel = g_tiledMap->GetZoomLevel();

	if ( m_scale ) g_tiledMap->SetZoomLevel(k_ZOOM_SMALLEST );
	else g_tiledMap->SetZoomLevel(k_ZOOM_NORMAL );

	CityInfluenceIterator it(pos, m_unit.CD()->GetSizeIndex());
	for(it.Start(); !it.End(); it.Next()) {
		func(pSurface, it.Pos(), (void *)this);
	}

	
	
	g_tiledMap->SetZoomLevel(zoomLevel);

	return TRUE;
}







BOOL ResourceMap::DrawSprites(aui_Surface *pSurface, RECT *destRect)
{
	Assert(pSurface);
	if (!pSurface) return FALSE;

	if (!m_unit) return FALSE;

	if(!g_theUnitPool->IsValid(m_unit)) return FALSE;

	MapPoint pos;


	m_unit.GetData()->GetPos(pos);

	
	
	DrawResourceMapThing(pSurface, DrawAGood);

	
	DrawResourceMapThing(pSurface, DrawACity);

	
	m_totalFood = m_totalProd = m_totalGold = 0;	

	
	Cell *cell = g_theWorld->GetCell( pos );
	m_totalFood += cell->GetFoodProduced();
	m_totalProd += cell->GetShieldsProduced();
	m_totalGold += 0; 











	
	return TRUE;
}







sint32 ResourceMap::CalculateWrap(
			aui_Surface *surface,
			sint32 i,	
			sint32 j,	
			sint32 x,
			sint32 y
			)
{

	MapPoint	pos;
	sint16		river = -1;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	sint32 k = maputils_TileX2MapX(j,i);

	MapPoint tempPos (k, i);

	pos = tempPos;
	
	TileInfo *tileInfo = g_tiledMap->GetTileInfo(pos);
	if (tileInfo == NULL) return -1;
	
	river = tileInfo->GetRiverPiece();
	
	BaseTile *baseTile = g_tiledMap->GetTileSet()->GetBaseTile(tileInfo->GetTileNum());
	if (baseTile == NULL) return -1;
	
	
	if ( !m_scale ) {
		
		
		g_tiledMap->DrawTransitionTile(NULL, pos, x, y);
		
		
		g_tiledMap->DrawOverlay(NULL, baseTile->GetHatData(), x, y);
		
		
		if (river != -1)
			g_tiledMap->DrawOverlay(NULL, g_tiledMap->GetTileSet()->GetRiverData(river), x, y);
		
		
		g_tiledMap->DrawImprovementsLayer(NULL, pos, x, y);

	}
	else {	
		
		g_tiledMap->DrawTransitionTileScaled(NULL, pos, x, y, g_tiledMap->GetZoomTilePixelWidth(), g_tiledMap->GetZoomTilePixelHeight() );

		
		g_tiledMap->DrawScaledOverlay(NULL, baseTile->GetHatData(), x, y, 
										g_tiledMap->GetZoomTilePixelWidth(), 
										g_tiledMap->GetZoomTileGridHeight());

		
		if ( river != -1 )
			g_tiledMap->DrawScaledOverlay(NULL, g_tiledMap->GetTileSet()->GetRiverData(river), x, y, 
											g_tiledMap->GetZoomTilePixelWidth(), 
											g_tiledMap->GetZoomTileGridHeight());

		
		g_tiledMap->DrawImprovementsLayer(NULL, pos, x, y);
	}
	
	return 0;
}

sint32 ResourceMap::DrawImprovements(
			aui_Surface *surface,
			sint32 i,	
			sint32 j,	
			sint32 x,
			sint32 y
			)
{
	sint16		river = -1;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	sint32 k = maputils_TileX2MapX(j,i);

	MapPoint pos (k, i);
	
	
	if ( !m_scale ) {
		
		g_tiledMap->DrawImprovementsLayer(NULL, pos, x, y);

	}
	else {	
		
		g_tiledMap->DrawImprovementsLayer(NULL, pos, x, y);
	}
	
	return 0;
}


void ResourceMap::DrawCityName(aui_Surface *surface, sint32 x, sint32 y, const Unit &unit)
{
	sint32 xoffset = (sint32)((k_TILE_PIXEL_WIDTH)/2);
	sint32 yoffset = (sint32)(k_TILE_PIXEL_HEADROOM);

	Assert(unit.IsCity());
	if (!unit.IsCity()) return;

	y-= yoffset;
	
	CityData *cityData = unit.GetData()->GetCityData() ;
	MBCHAR *name = (MBCHAR *)cityData->GetName() ;

	sint32 width, height;
	RECT rect;

	if (x >= 0 && y >= 0 && x < surface->Width() && y < surface->Height()) {

		width = textutils_GetWidth((aui_DirectSurface *)surface,name);
		height = textutils_GetHeight((aui_DirectSurface *)surface,name);
		rect.left = x;
		rect.top = y;
		rect.right = x+width;
		rect.bottom = y+height;;

		textutils_ColoredDropString(
			(aui_DirectSurface *)surface,
			name,
			x,
			y,
			k_CITYNAME_PTSIZE,
			(COLOR)(COLOR_PLAYER1+unit.GetOwner()),
			(COLOR)(COLOR_BLACK),
			k_FONT
			);
	}

	
	sint32 pop = cityData->PopCount();
	MBCHAR str[80];
	sprintf(str,"%i",pop);
	y+=yoffset;

	width = textutils_GetWidth((aui_DirectSurface *)surface,str);
	height = textutils_GetHeight((aui_DirectSurface *)surface,str);

	sint32	popEdgeSize = (sint32)((double)k_POP_BOX_SIZE);
	sint32 nudge = 0;
	if (pop > 9)
		nudge = 4;
	if (pop > 99)
		nudge = 2;
	if (popEdgeSize < k_POP_BOX_SIZE_MINIMUM) popEdgeSize = k_POP_BOX_SIZE_MINIMUM;

	RECT popRect = {x, 
					 y, 
					 x + (popEdgeSize*2), 
					 y + (popEdgeSize*2)};
	primitives_PaintRect16(surface,&popRect,g_colorSet->GetPlayerColor(unit.GetOwner()));
	primitives_FrameRect16(surface,&popRect,0x0000);

	textutils_CenteredColoredDropString(
		(aui_DirectSurface *)surface,
		str,
		&popRect,
		k_POP_PTSIZE,
		COLOR_WHITE,
		COLOR_BLACK,
		k_FONT
		);
}

sint32 ResourceMap::UpdateFromSurface(aui_Surface *destSurface, RECT *destRect)
{
	
	if(m_usedRect.right - m_usedRect.left < m_usedRect.bottom - m_usedRect.top) {
		
		sint32 diff = (m_usedRect.bottom - m_usedRect.top) - (m_usedRect.right - m_usedRect.left);
		if(diff > 1) {
			m_usedRect.left -= diff / 2;
			m_usedRect.right += diff / 2;
		} else {
			m_usedRect.left -= 1;
		}
	} else if(m_usedRect.right - m_usedRect.left > m_usedRect.bottom - m_usedRect.top) {
		
		sint32 diff = (m_usedRect.right - m_usedRect.left) - (m_usedRect.bottom - m_usedRect.top);
		if(diff > 1) {
			m_usedRect.top -= diff / 2;
			m_usedRect.bottom += diff / 2;
		} else {
			m_usedRect.top -= 1;
		}
	}
		
	RECT rect = {m_usedRect.left,m_usedRect.top,m_usedRect.right,m_usedRect.bottom};
	g_c3ui->TheBlitter()->StretchBlt(destSurface, destRect, m_surface, &rect, k_AUI_BLITTER_FLAG_COPY);

	return 0;
}

void ResourceMap::Click(aui_MouseEvent *data)
{	
	UnitActor		*actor=NULL;
	MapPoint		pos;
	Unit			top;
	POINT			point;

	point = data->position;

	if (MousePointToTilePos(point, pos)) {
		if (data->lbutton && !data->rbutton) {
			
			HandlePop(pos);
		} else {
			if (data->rbutton && !data->lbutton) {
				
			} else {
				
			}
		}
	}
}

BOOL ResourceMap::PointInMask(POINT hitPt)
{
	sint32		x, y;

	TILEHITMASK *thm = g_tiledMap->GetTileHitMask();

	double scale = 1.0;
	if ( m_scale ) {
		scale = 0.5;
	}

	x = (sint32)((double)hitPt.x / scale);
	y = (sint32)(((double)hitPt.y / scale) + k_TILE_PIXEL_HEADROOM);

	if (x >= thm[y].start &&
		x <= thm[y].end) 
		return TRUE;

	return FALSE;
}

BOOL ResourceMap::MousePointToTilePos(POINT point, MapPoint &tilePos)
{
	sint32			width, height;
	MapPoint		pos;
	POINT			hitPt;
	sint32			x, y;
	sint32			maxX;

	double scale = 1.0;
	if ( m_scale ) {
		scale = 0.5;
	}

	sint32			headroom = (sint32)((double)k_TILE_PIXEL_HEADROOM * scale);

	width = (sint32)((double)k_TILE_GRID_WIDTH * scale);
	height = (sint32)((double)(k_TILE_GRID_HEIGHT-k_TILE_PIXEL_HEADROOM) * scale);

	x = point.x;
	y = point.y;

	
	if (!(m_mapViewRect.top & 1)) 
		y -= headroom;

	pos.x = (sint16) (x / width + m_mapViewRect.left);
	pos.y = (sint16) ((y / height) + m_mapViewRect.top/2);
 
	hitPt.x = x % width;
	hitPt.y = y % height;

	maxX = m_mapBounds.right;

	if (!PointInMask(hitPt)) {
		
		pos.x = (sint16)((x + (width/2)) / width - 1 + m_mapViewRect.left);
		pos.y = (sint16)(((y + (height)/2) / height - 1) + m_mapViewRect.top/2);

		hitPt.x = (x + (width/2)) % width;
		hitPt.y = (y + (height)/2) % height;

		if (!PointInMask(hitPt)) 
			return FALSE;
		else {
			if (pos.x >= pos.y) {
				tilePos.x = pos.x - pos.y;
				tilePos.y = pos.y * 2 + 1;
			} else {
				tilePos.x = maxX + pos.x - pos.y;
				tilePos.y = pos.y * 2 + 1;
			}
		}
	} else {
		if (pos.x >= pos.y) {
			tilePos.x = pos.x - pos.y;
			tilePos.y = pos.y * 2;
		} else {
			tilePos.x = maxX + pos.x - pos.y;
			tilePos.y = pos.y * 2;
		}
	}

	if (g_theWorld->IsYwrap()) {
		if (tilePos.x <0) tilePos.x = g_theWorld->GetWidth() + tilePos.x; 
		else if (g_theWorld->GetWidth() <= tilePos.x) tilePos.x = tilePos.x - (sint16)g_theWorld->GetWidth(); 
		
		sint16 sx, sy;
		if (tilePos.y < 0) {
			sx = (sint16)g_theWorld->GetWidth();
			sy = (sint16)g_theWorld->GetHeight();
			tilePos.y = sy + tilePos.y;
			tilePos.x = (tilePos.x + (sx - (sy/2))) % sx;
		} else if (g_theWorld->GetHeight() <= tilePos.y) {
			sx = (sint16)g_theWorld->GetWidth();
			sy = (sint16)g_theWorld->GetHeight();
			tilePos.y = tilePos.y - sy;
			tilePos.x = (tilePos.x - (sx - (sy/2))) % sx;
		}
	} else { 
		
		
		if (tilePos.y <0) { 
			tilePos.y = 0; 
			return FALSE; 
		} else if (g_theWorld->GetHeight() <= tilePos.y) { 
			tilePos.y = g_theWorld->GetHeight() -1;
			return FALSE; 
		} 
	}

	if (tilePos.x <0) tilePos.x = g_theWorld->GetWidth() + tilePos.x; 
	else if (g_theWorld->GetWidth() <= tilePos.x) tilePos.x = tilePos.x - (sint16)g_theWorld->GetWidth(); 
		
	MapPoint tempPos;

	
	if (m_unit.m_id) {
		m_unit.GetData()->GetPos(tempPos);
	}

	return TRUE;
}

void ResourceMap::SetHiliteMouseTile(MapPoint &pos)
{
	m_hiliteMouseTile = pos;
}

void ResourceMap::DrawHiliteMouseTile(aui_Surface *destSurf, RECT *destRect)
{
	sint32 zoomLevel = g_tiledMap->GetZoomLevel();
	double scale = g_tiledMap->GetScale();

	
	if ( m_scale ) {
		g_tiledMap->SetZoomLevel(k_ZOOM_SMALLEST);
	}
	else {
		g_tiledMap->SetZoomLevel(k_ZOOM_NORMAL );
	}
	
	g_tiledMap->DrawHitMask(destSurf, m_hiliteMouseTile, &m_mapViewRect, destRect);

	
	g_tiledMap->SetZoomLevel(zoomLevel);
	g_tiledMap->SetScale( scale );
}

void ResourceMap::HandlePop( MapPoint point )
{
	
	MapPoint mp;

	
	m_unit.GetData()->GetPos(mp);

	
	sint32 diffX, diffY;
	diffY = 5 - point.y;
	diffX = 2 - point.x;

	
	sint32 x = mp.x - diffX;
	sint32 y = mp.y - diffY;

	
	
	sint32 xx, yy;
	maputils_WrapPoint( x, y, &xx, &yy);
	point.x = (sint16)xx;
	point.y = (sint16)yy;



	Cell *cell;
	cell = g_theWorld->GetCell(point);

	

	return;

	PLAYER_INDEX	player ;
	ID	item ;
	SELECT_TYPE	state ;
	
	g_selected_item->GetTopCurItem(player, item, state);
	Assert(player == g_selected_item->GetVisiblePlayer());
	if(player != g_selected_item->GetVisiblePlayer())
		return;
	
	Assert( m_unit != Unit(0) );

}




AUI_ERRCODE ResourceMap::Idle( void )
{
	static uint32 lastDraw = 0;
	if (GetTickCount() - lastDraw > 100) lastDraw = GetTickCount();
	else return AUI_ERRCODE_OK;

	
	DrawThis(NULL, 0, 0);

	return AUI_ERRCODE_OK;
}


