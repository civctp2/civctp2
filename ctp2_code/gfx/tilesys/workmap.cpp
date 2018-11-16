#include "c3.h"
#include "aui.h"
#include "aui_blitter.h"
#include "aui_directsurface.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_stringtable.h"

#include "primitives.h"
#include "Globals.h"
#include "player.h"

#include "dynarr.h"
#include "SelItem.h"

#include "director.h"

#include "tiledmap.h"
#include "BaseTile.h"
#include "TileInfo.h"
#include "tileset.h"
#include "colorset.h"

#include "workmap.h"
#include "Unit.h"
#include "UnitPool.h"

#include "c3_updateaction.h"

#include "Actor.h"
#include "UnitActor.h"
#include "workeractor.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "MapPoint.h"
#include "WonderRecord.h"
#include "c3ui.h"
#include "GoodActor.h"

#include "citydata.h"
#include "textutils.h"

#include "maputils.h"
#include "SlicEngine.h"
#include "profileDB.h"

#include "CityRadius.h"
#include "StrDB.h"
#include "UnitData.h"

#include "GameEventManager.h"

#include "CityInfluenceIterator.h"
#include "aui_Factory.h"

#define k_NUDGE		48

#define k_WORKER_INDEX		88
#define k_SLAVE_INDEX		89

#define k_CITYNAME_PTSIZE		12
#define k_POP_PTSIZE			10
#define k_POP_BOX_XOFFSET		3
#define k_POP_BOX_YOFFSET		5
#define k_POP_BOX_SIZE			10
#define k_POP_BOX_SIZE_MINIMUM	4
#define k_FONT					0

#define k_RES_PTSIZE			8

#define k_OFFSET_WIDTH			62

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





WorkMap::WorkMap(AUI_ERRCODE *retval,
							sint32 id,
							MBCHAR *ldlBlock,
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

WorkMap::WorkMap(AUI_ERRCODE *retval,
							uint32 id,
							sint32 x,
							sint32 y,
							sint32 width,
							sint32 height,
							MBCHAR *pattern,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:
		aui_ImageBase((sint32)0),
		aui_TextBase((MBCHAR *)NULL),
		aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
		PatternBase(pattern)
{
	InitCommon( k_WORKMAP_DEFAULT_SCALE );
}

WorkMap::~WorkMap()
{

	for (sint32 i=0; i<k_MAX_WORKERS; i++) {
		if (m_worker[i] != NULL) {
			delete m_worker[i];
			m_worker[i] = NULL;
		}
	}

	if (m_updateAction) {
		delete m_updateAction;
		m_updateAction = NULL;
	}

	if (m_surface) {
		delete m_surface;
		m_surface = NULL;
	}
}

void WorkMap::InitCommonLdl(MBCHAR *ldlBlock)
{
	sint32 scale = k_WORKMAP_DEFAULT_SCALE;
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return;

	if (block->GetAttributeType( k_WORKMAP_LDL_SCALE ) == ATTRIBUTE_TYPE_INT) {
		scale = block->GetInt( k_WORKMAP_LDL_SCALE );
	}

	InitCommon( scale );
}

void WorkMap::InitCommon( sint32 scale)
{
	AUI_ERRCODE			errcode;

	m_scale = scale;
	m_drawHilite = FALSE;

	m_unit = 0;

	m_updateAction = NULL;

	for (sint32 i = 0;i < k_MAX_WORKERS;i++) {
		m_worker[i] = NULL;
	}
	m_numWorkers = 0;

	m_surface = aui_Factory::new_Surface(errcode, m_width, m_height, 16);
	Assert( m_surface != NULL );
	if ( !m_surface ) return;

	m_string = new aui_StringTable( &errcode, "WorkMapStrings" );
	Assert( m_string );
	if ( !m_string ) return;
}

AUI_ERRCODE WorkMap::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{




	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );






	UpdateFromSurface(surface, &rect);
	DrawSprites(surface, &rect);
	DrawLabels(surface);

	if ( m_drawHilite ) {
		DrawHiliteMouseTile(surface, &rect);
	}

	if (surface == m_window->TheSurface()) {
		m_window->AddDirtyRect(&rect);
	}

	return AUI_ERRCODE_OK;
}

void WorkMap::MouseLGrabInside( aui_MouseEvent *mouseData )
{






	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;

	SetWhichSeesMouse(this);

	mouseData->position.x -= X();
	mouseData->position.y -= Y();

	Click(mouseData);
	DrawSurface();
}

void WorkMap::MouseMoveInside(aui_MouseEvent *data)

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

void WorkMap::MouseMoveAway(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

    Assert(data);

	m_drawHilite = FALSE;
}





void WorkMap::NotifyPopChanged()
{
	DrawSurface();
}







sint32 WorkMap::DrawSurface(void)
{
	sint32 width = m_surface->Width();
	sint32 height = m_surface->Height();

	RECT rect = {0,0,width,height};
	primitives_PaintRect16(m_surface,&rect,0x0000);

	MapPoint pos;
	MapPoint newpos;

	Assert( g_theUnitPool->IsValid(m_unit) );
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

	sint32 i;




	MapPoint	myPos1 = pos,
				myPos2;
	sint32		leftEdge, topEdge, temp;

	g_tiledMap->RecalculateViewRect(m_normalizedViewRect);

	if (myPos1.GetNeighborPosition(NORTHWEST, myPos2)) {
		myPos2.GetNeighborPosition(WEST, myPos1);
		maputils_MapXY2PixelXY(myPos1.x, myPos1.y, &leftEdge, &temp, &m_normalizedViewRect);
		m_topLeftPos.x = myPos1.x;
	} else {

		maputils_MapXY2PixelXY(myPos1.x, myPos1.y, &leftEdge, &temp, &m_normalizedViewRect);
		leftEdge -= (g_tiledMap->GetZoomTilePixelWidth() + g_tiledMap->GetZoomTilePixelWidth()/2);
		m_topLeftPos.x = myPos1.x;
	}

	myPos1 = pos;
	if (myPos1.GetNeighborPosition(NORTHWEST, myPos2)) {
		if (myPos2.GetNeighborPosition(NORTH, myPos1)) {
			maputils_MapXY2PixelXY(myPos1.x, myPos1.y, &temp, &topEdge, &m_normalizedViewRect);
			m_topLeftPos.y = myPos1.y;
		} else {

			maputils_MapXY2PixelXY(myPos2.x, myPos2.y, &temp, &topEdge, &m_normalizedViewRect);
			topEdge -= g_tiledMap->GetZoomTilePixelHeight();
			m_topLeftPos.y = myPos2.y;
		}
	} else {

		maputils_MapXY2PixelXY(myPos1.x, myPos1.y, &temp, &topEdge, &m_normalizedViewRect);
		topEdge -= (g_tiledMap->GetZoomTilePixelHeight() + g_tiledMap->GetZoomTilePixelHeight()/2);
		m_topLeftPos.y = myPos1.y;
	}

	m_leftEdge = leftEdge;
	m_topEdge = topEdge;

	for (i=0; i<k_MAX_WORKERS; i++) {
		if (m_worker[i] != NULL) {
			delete m_worker[i];
			m_worker[i] = NULL;
		}
	}
	m_numWorkers = 0;

	g_tiledMap->LockThisSurface(m_surface);

	DrawWorkMapThing(m_surface, DrawATile);

	g_tiledMap->UnlockSurface();

	g_tiledMap->SetZoomLevel(zoomLevel);
	g_tiledMap->SetScale( scale );

	g_tiledMap->GetMapBounds( m_mapBounds );

	m_mapViewRect.left = 3;
	m_mapViewRect.right = 7;
	m_mapViewRect.top = 2;
	m_mapViewRect.bottom = 9;

return 0;

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





	g_tiledMap->LockThisSurface(m_surface);

	for (sint32 y = 0;y < 7;y++) {
		if (y & 0x01) {
			if ( !m_scale ) {
				nudge = k_NUDGE;
			}
			else {
				nudge = k_NUDGE / 2;
			}
			if (!pos.GetNeighborPosition(SOUTHWEST, newpos))
				continue;
		}
		else {
			if (!pos.GetNeighborPosition(SOUTHEAST, newpos))
				continue;

			nudge = 0;
		}
		maputils_MapX2TileX(pos.x,pos.y,&i);

		sint32 x;
		for (x = 0;x < 3;x++) {
			if (x==0 && (y==0 || y==6)) continue;
			if ( !m_scale )
				CalculateWrap(m_surface, pos.y, i+x, x*96+nudge,y*24);
			else
				CalculateWrap(m_surface, pos.y, i+x, x*48+nudge,y*12);


			sint32 mapX = maputils_TileX2MapX(i+x,pos.y);
			MapPoint tempPos( mapX, pos.y);
			Cell *cell = g_theWorld->GetCell(tempPos);

			BOOL drawBorder = FALSE;

			if (cell->GetOwner() != m_unit.GetOwner()) {
				if ( !m_scale ) {
					g_tiledMap->DrawTileBorder(m_surface, x*96+nudge, y*24, g_colorSet->GetPlayerColor(cell->GetOwner()));
				}
				else {
					g_tiledMap->DrawTileBorderScaled(m_surface, pos, x*48+nudge, y*12,
							g_tiledMap->GetZoomTilePixelWidth(),
							g_tiledMap->GetZoomTilePixelHeight(),
							g_colorSet->GetPlayerColor(cell->GetOwner()));
				}

				drawBorder = TRUE;
			}

			if (m_worker[index]) {
				delete m_worker[index];
				m_worker[index] = NULL;
			}
			index++;






















		}
		if (y==2 || y==4) {
			if ( !m_scale )
				CalculateWrap(m_surface, pos.y, i+x, x*96+nudge,y*24);
			else
				CalculateWrap(m_surface, pos.y, i+x, x*48+nudge,y*12);

			sint32 mapX = maputils_TileX2MapX(i+x,pos.y);
			MapPoint tempPos (mapX, pos.y);
			Cell *cell = g_theWorld->GetCell(tempPos);

			BOOL drawBorder = FALSE;

			if (cell->GetOwner() != m_unit.GetOwner()) {
				if ( !m_scale ) {
					g_tiledMap->DrawTileBorder(m_surface, x*96+nudge, y*24, g_colorSet->GetPlayerColor(cell->GetOwner()));
				}
				else {
					g_tiledMap->DrawTileBorderScaled(m_surface, pos, x*48+nudge, y*12,
									g_tiledMap->GetZoomTilePixelWidth(),
									g_tiledMap->GetZoomTilePixelHeight(),
									g_colorSet->GetPlayerColor(cell->GetOwner()));
				}
				drawBorder = TRUE;
			}

			if (m_worker[index]) {
				delete m_worker[index];
				m_worker[index] = NULL;
			}

			index++;





















		}
		pos = newpos;
	}

	g_tiledMap->UnlockSurface();

	g_tiledMap->SetZoomLevel(zoomLevel);
	g_tiledMap->SetScale( scale );

	return TRUE;
}

sint32 WorkMap::DrawSpaceImprovements( aui_Surface *pSurface, sint32 xOff, sint32 yOff )
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

		sint32 x;
		for (x = 0;x < 3;x++) {
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

BOOL WorkMap::DrawACity(aui_Surface *pSurface, const MapPoint &pos, void *context)
{
	WorkMap		*workMap = (WorkMap *)context;
	UnitActor	*actor;
	sint32		x, y;
	Unit		city;
	sint32		mapWidth, mapHeight;

	city = g_theWorld->GetCell(pos)->GetCity();
	if (city.m_id == 0) return FALSE;

	actor = city.GetActor();
	if (!actor) return FALSE;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, workMap->GetNormalizedViewRect());

	x -= workMap->GetLeftEdge();
	y -= workMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);

	POINT p = {workMap->X(), workMap->Y()};
	workMap->ToWindow(&p);
	x += p.x;
	y += p.y;






	if ( !workMap->GetScale() ) {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
	}
	else {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
	}

	return TRUE;
}

BOOL WorkMap::DrawALandCity(aui_Surface *pSurface, const MapPoint &pos, void *context)
{
	WorkMap		*workMap = (WorkMap *)context;
	UnitActor	*actor;
	sint32		x, y;
	Unit		city;
	sint32		mapWidth, mapHeight;

	city = g_theWorld->GetCell(pos)->GetCity();
	if (city.m_id == 0) return FALSE;

	actor = city.GetActor();
	if (!actor) return FALSE;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, workMap->GetNormalizedViewRect());

	x -= workMap->GetLeftEdge();
	y -= workMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);

	POINT p = {workMap->X(), workMap->Y()};
	workMap->ToWindow(&p);
	x += p.x;
	y += p.y;






	if ( !workMap->GetScale() ) {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
	}
	else {
		actor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
	}

	return TRUE;
}

BOOL WorkMap::DrawAGood(aui_Surface *pSurface, const MapPoint &pos, void *context)
{
	WorkMap		*workMap = (WorkMap *)context;
	GoodActor	*goodActor;
	sint32		mapWidth, mapHeight;
	sint32		x, y;

	TileInfo *curTileInfo = g_tiledMap->GetTileInfo(pos);
	Assert(curTileInfo != NULL);
	if(!curTileInfo || !curTileInfo->HasGoodActor()) return FALSE;

	goodActor = curTileInfo->GetGoodActor();

	if (!goodActor) return FALSE;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, workMap->GetNormalizedViewRect());

	x -= workMap->GetLeftEdge();
	y -= workMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);

	POINT p = {workMap->X(), workMap->Y()};
	workMap->ToWindow(&p);
	x += p.x;
	y += p.y;





	if ( !workMap->GetScale() ) {
		goodActor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
	}
	else {
		goodActor->DrawDirect(pSurface, x, y, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
	}

	return TRUE;
}

BOOL WorkMap::DrawATile(aui_Surface *pSurface, const MapPoint &pos, void *context)
{
	WorkMap		*workMap = (WorkMap *)context;
	sint32		x, y;
	sint32		mapWidth, mapHeight;

	g_tiledMap->GetMapMetrics(&mapWidth,&mapHeight);

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, workMap->GetNormalizedViewRect());

	x -= workMap->GetLeftEdge();
	y -= workMap->GetTopEdge();

	if (x < 0) x += (mapWidth * g_tiledMap->GetZoomTilePixelWidth());
	if (y < 0) y += (mapHeight * g_tiledMap->GetZoomTilePixelHeight()/2);




























	sint32 tileX;
	maputils_MapX2TileX(pos.x, pos.y, &tileX);

	workMap->CalculateWrap(pSurface, pos.y, tileX, x, y);

	Cell	*cell = g_theWorld->GetCell(pos);

	if (cell) {
		Unit	c;

		workMap->GetOwningCity(c);

		if (cell->GetOwner() != c.GetOwner()) {
			if ( !workMap->GetScale() ) {
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

BOOL WorkMap::DrawWorkMapThing(aui_Surface *pSurface, WorkMapDrawFunc *func)
{
	MapPoint	pos = m_unit.RetPos();
	MapPoint	wpos;




	sint32 zoomLevel = g_tiledMap->GetZoomLevel();

	if ( m_scale ) g_tiledMap->SetZoomLevel(k_ZOOM_SMALLEST );
	else g_tiledMap->SetZoomLevel(k_ZOOM_NORMAL );

	SquareIterator it(pos, 1);
	for(it.Start(); !it.End(); it.Next()) {
		func(pSurface, it.Pos(), (void *)this);
	}


	g_tiledMap->SetZoomLevel(zoomLevel);

	return TRUE;
}







BOOL WorkMap::DrawSprites(aui_Surface *pSurface, RECT *destRect)
{
	Assert(pSurface);
	if (!pSurface) return FALSE;

	if (!m_unit) return FALSE;

	if(!g_theUnitPool->IsValid(m_unit)) return FALSE;

	MapPoint pos;
	sint32 i;

	m_unit.GetData()->GetPos(pos);

	for (i = 0;i < k_MAX_WORKERS;i++) {
		if (m_worker[i]) {
			m_worker[i]->Process();

			if ( !m_scale ) {
				m_worker[i]->DrawDirect(pSurface, m_worker[i]->GetX()+destRect->left, m_worker[i]->GetY()+destRect->top, g_tiledMap->GetZoomScale(k_ZOOM_LARGEST));
			}
			else {
				m_worker[i]->DrawDirect(pSurface, m_worker[i]->GetX()+destRect->left, m_worker[i]->GetY()+destRect->top, g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST));
			}
		}
	}


	DrawWorkMapThing(pSurface, DrawAGood);

	DrawWorkMapThing(pSurface, DrawACity);

	m_totalFood = m_totalProd = m_totalGold = 0;

	Cell *cell = g_theWorld->GetCell( pos );
	m_totalFood += cell->GetFoodProduced();
	m_totalProd += cell->GetShieldsProduced();
	m_totalGold += 0;

	for (i = 0;i < k_MAX_WORKERS;i++) {
		if (m_worker[i]) {

			Assert(FALSE);
		}
	}












	return TRUE;
}







sint32 WorkMap::CalculateWrap(
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

sint32 WorkMap::DrawImprovements(
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

void WorkMap::DrawCityName(aui_Surface *surface, sint32 x, sint32 y, const Unit &unit)
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

		width = textutils_GetWidth(surface,name);
		height = textutils_GetHeight(surface,name);
		rect.left = x;
		rect.top = y;
		rect.right = x+width;
		rect.bottom = y+height;;

		textutils_ColoredDropString(
			surface,
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

	width = textutils_GetWidth(surface,str);
	height = textutils_GetHeight(surface,str);

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
		surface,
		str,
		&popRect,
		k_POP_PTSIZE,
		COLOR_WHITE,
		COLOR_BLACK,
		k_FONT
		);
}

void WorkMap::DrawALabel( aui_Surface *surface, MBCHAR *label, sint32 x, sint32 y, sint32 width, sint32 height )
{
	RECT rect = {0, 0, width, height};

	OffsetRect(&rect, X() + Width() - x, Y() + y );
	ToWindow( &rect );

	if (rect.left < 0)
		rect.left = 0;

	if (rect.top < 0)
		rect.top = 0;

	if (rect.right >= surface->Width()) {
		rect.right = surface->Width() - 1;
	}

	if (rect.bottom >= surface->Height())  {
		rect.bottom = surface->Height() - 1;
	}

	g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, label, 0,
		g_colorSet->GetColorRef(COLOR_WHITE), 0);
}

void WorkMap::DrawLabels( aui_Surface *surface )
{
	sint32 foodWidth, foodHeight;
	sint32 prodWidth, prodHeight;
	sint32 goldWidth, goldHeight;

	MBCHAR foodStr[_MAX_PATH];
	MBCHAR prodStr[_MAX_PATH];
	MBCHAR goldStr[_MAX_PATH];

	if ( m_totalFood < 0 || m_totalProd < 0 || m_totalGold < 0 ) return;

	sprintf(foodStr, "%s %d", m_string->GetString(WM_FOOD), m_totalFood );
	sprintf(prodStr, "%s %d", m_string->GetString(WM_PROD), m_totalProd );
	sprintf(goldStr, "%s %d", m_string->GetString(WM_GOLD), m_totalGold );

	if (g_tiledMap && g_tiledMap->GetFont()) {
		foodWidth = g_tiledMap->GetFont()->GetStringWidth(foodStr);
		foodHeight = g_tiledMap->GetFont()->GetMaxHeight();
		prodWidth = g_tiledMap->GetFont()->GetStringWidth(prodStr);
		prodHeight = g_tiledMap->GetFont()->GetMaxHeight();
		goldWidth = g_tiledMap->GetFont()->GetStringWidth(goldStr);
		goldHeight = g_tiledMap->GetFont()->GetMaxHeight();

		sint32 offsetWidth = foodWidth, offsetHeight;

		if ( prodWidth > offsetWidth )
			offsetWidth = prodWidth;
		if ( goldWidth > offsetWidth )
			offsetWidth = prodWidth;





		offsetHeight = 2;
		DrawALabel( surface, foodStr, k_OFFSET_WIDTH, offsetHeight, foodWidth, foodHeight );

		offsetHeight += foodHeight;
		DrawALabel( surface, prodStr, k_OFFSET_WIDTH, offsetHeight, prodWidth, prodHeight );

		offsetHeight += prodHeight;
		DrawALabel( surface, goldStr, k_OFFSET_WIDTH, offsetHeight, goldWidth, goldHeight );

		MBCHAR tiStr[_MAX_PATH];
		sprintf( tiStr, g_theStringDB->GetNameStr("str_ldl_TILE_RESOURCES_Colon") );
		sint32 tiLabelWidth = g_tiledMap->GetFont()->GetStringWidth(tiStr);
		RECT rect = {0, 0, tiLabelWidth, g_tiledMap->GetFont()->GetMaxHeight()};

		OffsetRect(&rect, X() + 2, Y() + 2 );
		ToWindow( &rect );

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, tiStr, 0,
			g_colorSet->GetColorRef(COLOR_WHITE), 0);

	}
}

void WorkMap::CenterNumber( RECT *rect, sint32 &x, sint32 &y, sint32 width, sint32 height )
{
	sint32 rectWidth = rect->right - rect->left;
	sint32 rectHeight = rect->bottom - rect->top;

	Assert( rectWidth >= width );
	Assert( rectHeight >= height );

	x = rect->left + (( rectWidth - width ) >> 1);
	y = rect->top + (( rectHeight - height ) >> 1);
}

void WorkMap::DrawResourceIcons(aui_Surface *surface, sint32 x, sint32 y, MapPoint &pos)
{
	RECT		iconRect;
	TileSet		*tileSet = g_tiledMap->GetTileSet();
	MBCHAR		str[80];
	sint32		width, height;

	sint32		xcenter, ycenter;

	sint32 prod, food, gold;
	Cell *cell = g_theWorld->GetCell(pos);

	prod = cell->GetShieldsProduced();
	food = cell->GetFoodProduced();




	gold = 0;

	m_totalFood += food;
	m_totalProd += prod;
	m_totalGold += gold;

	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_RESOURCE1);
	Pixel16 color = g_colorSet->GetPlayerColor(m_unit.GetOwner());

	sint32	popEdgeSize = k_POP_BOX_SIZE;
	iconRect.left = x + (popEdgeSize*2);
	iconRect.top = y;
	iconRect.right = iconRect.left + iconDim.x + 1;
	iconRect.bottom = iconRect.top + iconDim.y + 1;

	if (iconRect.left < 0 || iconRect.top < 0 || iconRect.right >= surface->Width() ||
		iconRect.bottom >= surface->Height())
		return;

	Pixel16 *resourceIcon;

	resourceIcon = tileSet->GetMapIconData(MAPICON_RESOURCE1);
	Assert(resourceIcon); if (!resourceIcon) return;
	g_tiledMap->DrawColorizedOverlay(resourceIcon, surface, iconRect.left, iconRect.top, color);

	sprintf(str, "%ld", prod);

	if (g_tiledMap && g_tiledMap->GetFont()) {
		width = g_tiledMap->GetFont()->GetStringWidth(str);
		height = g_tiledMap->GetFont()->GetMaxHeight();

		RECT		rect = {0, 0, width, height};

		CenterNumber( &iconRect, xcenter, ycenter, width, height );
		OffsetRect( &rect, xcenter, ycenter );

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, str, 0,
			g_colorSet->GetColorRef(COLOR_WHITE), 0);

		OffsetRect(&rect, -1, -1);

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, str, 0,
			g_colorSet->GetColorRef(COLOR_BLACK), 0);
	}

	iconRect.left += iconDim.x;
	iconRect.right += iconDim.x;

	iconDim = tileSet->GetMapIconDimensions(MAPICON_RESOURCE2);
	iconRect.bottom = iconRect.top + iconDim.y + 1;

	if (iconRect.left < 0 || iconRect.top < 0 || iconRect.right >= surface->Width() ||
		iconRect.bottom >= surface->Height())
		return;

	resourceIcon = tileSet->GetMapIconData(MAPICON_RESOURCE2);
	Assert(resourceIcon); if (!resourceIcon) return;
	g_tiledMap->DrawColorizedOverlay(resourceIcon, surface, iconRect.left, iconRect.top, color);

	sprintf(str, "%ld", food);

	if (g_tiledMap && g_tiledMap->GetFont()) {
		width = g_tiledMap->GetFont()->GetStringWidth(str);
		height = g_tiledMap->GetFont()->GetMaxHeight();

		RECT		rect = {0, 0, width, height};

		CenterNumber( &iconRect, xcenter, ycenter, width, height );
		OffsetRect( &rect, xcenter, ycenter );

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, str, 0,
			g_colorSet->GetColorRef(COLOR_WHITE), 0);

		OffsetRect(&rect, -1, -1);

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, str, 0,
			g_colorSet->GetColorRef(COLOR_BLACK), 0);
	}

	iconDim = tileSet->GetMapIconDimensions(MAPICON_RESOURCE1);

	iconRect.left = x + (popEdgeSize*2);
	iconRect.top = y + iconDim.y;

	iconDim = tileSet->GetMapIconDimensions(MAPICON_RESOURCE3);

	iconRect.right = iconRect.left + iconDim.x + 1;
	iconRect.bottom = iconRect.top + iconDim.y + 1;

	resourceIcon = tileSet->GetMapIconData( MAPICON_RESOURCE3 );
	Assert( resourceIcon ); if ( !resourceIcon ) return;
	g_tiledMap->DrawColorizedOverlay( resourceIcon, surface, iconRect.left, iconRect.top, color );

	sprintf( str, "%d", gold );

	if (g_tiledMap && g_tiledMap->GetFont()) {
		width = g_tiledMap->GetFont()->GetStringWidth(str);
		height = g_tiledMap->GetFont()->GetMaxHeight();

		RECT		rect = {0, 0, width, height};

		CenterNumber( &iconRect, xcenter, ycenter, width, height );
		OffsetRect( &rect, xcenter, ycenter );

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, str, 0,
			g_colorSet->GetColorRef(COLOR_WHITE), 0);

		OffsetRect(&rect, -1, -1);

		g_tiledMap->GetFont()->DrawString(surface, &rect, &rect, str, 0,
			g_colorSet->GetColorRef(COLOR_BLACK), 0);
	}
}

sint32 WorkMap::UpdateFromSurface(aui_Surface *destSurface, RECT *destRect)
{
	RECT rect = {0,0,m_surface->Width(),m_surface->Height()};
	g_c3ui->TheBlitter()->Blt(destSurface, destRect->left, destRect->top, m_surface, &rect, k_AUI_BLITTER_FLAG_COPY);
	return 0;
}

void WorkMap::Click(aui_MouseEvent *data)
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

BOOL WorkMap::PointInMask(POINT hitPt)
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

BOOL WorkMap::MousePointToTilePos(POINT point, MapPoint &tilePos)
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

void WorkMap::SetHiliteMouseTile(MapPoint &pos)
{
	m_hiliteMouseTile = pos;
}

void WorkMap::DrawHiliteMouseTile(aui_Surface *destSurf, RECT *destRect)
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

void WorkMap::HandlePop( MapPoint point )
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




AUI_ERRCODE WorkMap::Idle( void )
{
	static uint32 lastDraw = 0;
	if (GetTickCount() - lastDraw > 100) lastDraw = GetTickCount();
	else return AUI_ERRCODE_OK;

	DrawThis(NULL, 0, 0);

	return AUI_ERRCODE_OK;
}
