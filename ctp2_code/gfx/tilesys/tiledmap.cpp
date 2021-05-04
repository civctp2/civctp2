//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile map handling
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
// _DEBUG
// - Set when generating the debug version
//
// _SMALL_MAPPOINTS
// - Use 2D world when set, add 3rd dimension (CTP1 space layer) when not set.
//
// __BIG_DIRTY_BLITS__
// __USING_SPANS__
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Make sure that cities created by the scenario editor keep their style and
//   their size. The last created city by the scenario editor is now selected.
//	 By Martin Gühmann.
// - Map wrapping corrected.
// - Possible leaks/invalid accesses corrected.
// - Current terrain improvements are displayed instead of those from the
//   last visit if the fog of war is toggled off. - Dec 24th 2004 - Martin Gühmann
// - With fog of war off the current city sprites and unit sprites at the
//   right position are displayed. - Dec. 25th 2004 - Martin Gühmann
// - Improved destructor (useless code removed, corrected delete [])
// - Removed .NET compiler warnings. - April 23rd 2005 Martin Gühmann
// - Prevented crashes on game startup and exit.
// - The good sprite index is now retrieved from the resource database
//   instaed of good sprite state database. (Aug 29th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Made government modified for units work here. (July 29th 2006 Martin Gühmann)
// - added debugai profile switch - E 4-3-2007
// - When yes the debugai switch causes a crash
// - Full city radius is now drawn around settlers. (30-Jan-2008 Martin Gühmann)
// - Changed colour of maximum zoom grid from white to black. (12-Mar-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tiledmap.h"

#include "AICause.h"
#include <algorithm>                    // std::fill
#include "ArmyData.h"
#include "aui.h"
#include "aui_blitter.h"
#include "aui_surface.h"
#include "aui_Factory.h"
#include "aui_dirtylist.h"
#include "aui_stringtable.h"
#include "background.h"
#include "BaseTile.h"
#include "c3errors.h"
#include "c3_popupwindow.h"
#include "c3ui.h"
#include "c3window.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "citydata.h"
#include "CityInfluenceIterator.h"
#include "CityRadius.h"
#include "civarchive.h"
#include "CivPaths.h"                   // g_civPaths
#include "colorset.h"                   // g_colorSet
#include "ctp2_button.h"
#include "ctp2_Window.h"
#include "director.h"                   // g_director
#include "EffectActor.h"
#include "gamefile.h"
#include "gfx_options.h"
#include "GoodActor.h"
#include "GoodyHuts.h"
#include "grabitem.h"
#include "MapPoint.h"
#include "maputils.h"
#include "MoveFlags.h"
#include "network.h"
#include "pixelutils.h"
#include "player.h"                     // g_player
#include "pointerlist.h"
#include "primitives.h"
#include "profileDB.h"                  // g_theProfileDB
#include "radarmap.h"                   // g_radarMap
#include "radarwindow.h"
#include "ResourceRecord.h"
#include "scenarioeditor.h"
#include "screenmanager.h"
#include "SelItem.h"                    // g_selected_item
#include "SlicEngine.h"
#include "Sprite.h"
#include "spriteeditor.h"
#include "StrDB.h"                      // g_theStringDB
#include "TerrainImprovementRecord.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "TerrImprove.h"
#include "TerrImproveData.h"
#include "TerrImprovePool.h"
#include "tiffutils.h"
#include "TileDrawRoad.h"
#include "TileInfo.h"
#include "tileset.h"
#include "tileutils.h"
#include "TradePool.h"
#include "TradeRoute.h"
#include "TradeRouteData.h"
#include "TurnCnt.h"                    // g_turn
#include "UnitActor.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "UnitSpriteGroup.h"
#include "UnseenCell.h"
#include "World.h"                      // g_theWorld

extern C3UI             *g_c3ui;
extern sint32           g_is565Format;
extern Background       *g_background;
extern RECT             g_backgroundViewport;
extern ScreenManager    *g_screenManager;
extern SpriteEditWindow *g_spriteEditWindow;
extern GrabItem         *g_grabbedItem;
extern sint32           g_tradeSelectedState;
extern sint32           g_fog_toggle;
extern sint32           g_god;
extern sint32           g_isCheatModeOn;
extern TiledMap         *g_tiledMap;
extern BOOL             g_show_ai_dbg;
extern sint32           g_modalWindow;

sint32      g_unitNum               = -1;
sint32      g_cityNum               = -1;
BOOL        g_killMode              = FALSE;
sint32      g_tileImprovementMode   = 0;
BOOL        g_isTransportOn         = FALSE;
sint32      g_isFastCpu             = 1;
sint32      g_isGridOn              = 0;
sint32      g_placeGoodsMode        = FALSE;
BOOL	    g_drawArmyClumps;

sint32 s_zoomTilePixelWidth[k_MAX_ZOOM_LEVELS]  = {48, 56, 64, 72, 80, 88, 96};
sint32 s_zoomTilePixelHeight[k_MAX_ZOOM_LEVELS] = {24, 28, 32, 36, 40, 44, 48};
sint32 s_zoomTileGridHeight[k_MAX_ZOOM_LEVELS]  = {36, 42, 48, 54, 60, 66, 72};
sint32 s_zoomTileHeadroom[k_MAX_ZOOM_LEVELS]    = {12, 14, 16, 18, 20, 22, 24};
double s_zoomTileScale[k_MAX_ZOOM_LEVELS]       = {0.5, 0.58333, 0.66667, 0.75, 0.83333, 0.91667, 1.0};

namespace
{
    RECT const          RECT_INVISIBLE      = {0, 0, 0, 0};
}

TiledMap::TiledMap(MapPoint &size)
:
#if defined(_DEBUG)
	num_loops               (1.0f),
	num_rects               (0.0f),
#endif
    m_zoomLevel             (k_ZOOM_NORMAL),
	m_zoomCallback          (NULL),
	m_isScrolling           (false),
	m_surface               (NULL),
	m_mapSurface            (NULL),
	m_lockedSurface         (NULL),
	m_surfBase              (NULL),
	m_surfWidth             (0),
	m_surfHeight            (0),
	m_surfPitch             (0),
	m_surfIsLocked          (false),
    m_displayRect           (RECT_INVISIBLE),
    m_surfaceRect           (RECT_INVISIBLE),
	m_mapBounds             (RECT_INVISIBLE),
	m_mapViewRect           (RECT_INVISIBLE),
    m_scale                 (1.0),
	m_smoothOffsetX         (0),
    m_smoothOffsetY         (0),
	m_smoothLastX           (0),
    m_smoothLastY           (0),
	m_overlayActive         (false),
	m_overlayRec            (NULL),
	m_overlayPos            (),
	m_overlayColor          (0),
	m_tileSet               (NULL),
    m_hiliteMouseTile       (),
	m_drawHilite            (false),
#ifdef __USING_SPANS__
    m_mapDirtyList          (NULL),
	m_mixDirtyList          (NULL),
	m_oldMixDirtyList       (NULL),
#else
    m_mapDirtyList          (new aui_DirtyList),
	m_mixDirtyList          (new aui_DirtyList),
	m_oldMixDirtyList       (new aui_DirtyList),
#endif
	m_localVision           (NULL),
	m_nextPlayer            (false),
	m_oldPlayer             (PLAYER_INDEX_INVALID),
	m_font                  (NULL),
	m_gridWidth             (0),
	m_gridHeight            (0),
	m_one_over_gridWidth    (1.0f),
	m_one_over_gridHeight   (1.0f),
	m_gridRects             (NULL),
	m_chatRect              (RECT_INVISIBLE)
{
    std::fill(m_fortifyString, m_fortifyString + 4, 0);
    std::copy(s_zoomTilePixelWidth, s_zoomTilePixelWidth + k_MAX_ZOOM_LEVELS,
              m_zoomTilePixelWidth
             );
    std::copy(s_zoomTilePixelHeight, s_zoomTilePixelHeight + k_MAX_ZOOM_LEVELS,
              m_zoomTilePixelHeight
             );
    std::copy(s_zoomTilePixelWidth, s_zoomTilePixelWidth + k_MAX_ZOOM_LEVELS,
              m_zoomTilePixelWidth
             );
    std::copy(s_zoomTileGridHeight, s_zoomTileGridHeight + k_MAX_ZOOM_LEVELS,
              m_zoomTileGridHeight
             );
    std::copy(s_zoomTileHeadroom, s_zoomTileHeadroom + k_MAX_ZOOM_LEVELS,
              m_zoomTileHeadroom
             );
    std::copy(s_zoomTileScale, s_zoomTileScale + k_MAX_ZOOM_LEVELS,
              m_zoomTileScale
             );

	SetRect(&m_mapBounds, 0, 0, size.x, size.y);
	SetZoomLevel(k_ZOOM_NORMAL);
	GenerateHitMask();  // fills m_tileHitMask[]

	AUI_ERRCODE         errcode     = AUI_ERRCODE_OK;
	aui_StringTable	*   stringTable =
	    new aui_StringTable(&errcode, "TiledMapFontStringTable");

	if (AUI_NEWOK(stringTable, errcode))
	{
		const MBCHAR *    fontNameString  = stringTable->GetString(0);
		const MBCHAR *    fontSizeString  = stringTable->GetString(1);

		m_font = g_c3ui->LoadBitmapFont(fontNameString);
		Assert(m_font);
		m_font->SetPointSize(atoi(fontSizeString));

		const MBCHAR *    fString         = stringTable->GetString(2);
		strncpy(m_fortifyString, fString, 3);
	}

	delete stringTable;
}

TiledMap::~TiledMap()
{
	DeleteGrid();

	if (g_c3ui && m_font)
	{
		g_c3ui->UnloadBitmapFont(m_font);
	}
	delete m_mapSurface;
	delete m_mixDirtyList;
	delete m_oldMixDirtyList;
	delete m_mapDirtyList;
	delete m_tileSet;
	// m_localVision    not deleted: reference only
	// m_surface        not deleted: reference only
	// m_surfBase       not deleted: reference only
	// m_overlayRec     not deleted: reference only
	// m_lockedSurface  not deleted: reference only
}

sint32 TiledMap::Initialize(RECT *viewRect)
{
	sint32			w = viewRect->right - viewRect->left;
	sint32			h = viewRect->bottom - viewRect->top;
	AUI_ERRCODE		errcode;

	m_mapSurface = aui_Factory::new_Surface(errcode, w, h);
	Assert(m_mapSurface);
	if (!m_mapSurface) return AUI_ERRCODE_MEMALLOCFAILED;




	m_surface = m_mapSurface;

#ifdef __USING_SPANS__

	m_mixDirtyList = new aui_DirtyList( TRUE, w, h );
	m_oldMixDirtyList = new aui_DirtyList( TRUE, w, h );
	m_mapDirtyList = new aui_DirtyList( TRUE, w, h );
#endif

	m_displayRect = m_surfaceRect = *viewRect;
	OffsetRect(&m_surfaceRect, -m_surfaceRect.left, -m_surfaceRect.top);

	CalculateMetrics();

	m_localVision = g_player[g_selected_item->GetVisiblePlayer()]->m_vision;

	Assert(m_localVision);

	m_localVision->SetAmOnScreen(true);

	InitGrid( 128, 128);

	return AUI_ERRCODE_OK;
}

void TiledMap::InitGrid(sint32 maxPixelsPerGridRectX, sint32 maxPixelsPerGridRectY)
{
	RECT		tempRect;

	maxPixelsPerGridRectX &= ~0x01;

	m_gridWidth  = (m_surfaceRect.right /maxPixelsPerGridRectX)+1;
	m_gridHeight = (m_surfaceRect.bottom/maxPixelsPerGridRectY)+1;





	m_one_over_gridWidth  = 1.0f/(float)maxPixelsPerGridRectX;
	m_one_over_gridHeight = 1.0f/(float)maxPixelsPerGridRectY;

	m_gridRects = new GridRect*[m_gridHeight];

	for (sint32 i=0; i<m_gridHeight; i++)
	{
		m_gridRects[i] = new GridRect[m_gridWidth];

		for(sint32 j=0; j<m_gridWidth; j++)
		{

			tempRect.left  = j * maxPixelsPerGridRectX;
		    tempRect.right = tempRect.left + maxPixelsPerGridRectX;

			tempRect.top    = i * maxPixelsPerGridRectY;
			tempRect.bottom = tempRect.top + maxPixelsPerGridRectY;

			if(tempRect.right > m_surfaceRect.right)
			   tempRect.right = m_surfaceRect.right;


			if(tempRect.bottom > m_surfaceRect.bottom)
			   tempRect.bottom = m_surfaceRect.bottom;

			m_gridRects[i][j].rect = tempRect;
			m_gridRects[i][j].dirty = FALSE;
		}
	}
}

void TiledMap::DeleteGrid(void)
{
	for (sint32 i=0; i<m_gridHeight; i++) {
		delete m_gridRects[i];
	}

	delete [] m_gridRects;
	m_gridRects = NULL;
}

void TiledMap::UpdateAndClipMapViewRect(const RECT & rect)
{
	RECT * mapViewRect = const_cast<RECT *> (&m_mapViewRect);
	*mapViewRect = rect;

	sint32 width  = rect.right - rect.left;
	sint32 height = rect.bottom - rect.top;

	mapViewRect->left = rect.left;
	if (!g_theWorld->IsXwrap() && mapViewRect->left < GetMinMapLeft()) {
		mapViewRect->left = GetMinMapLeft();
	}

	mapViewRect->right = mapViewRect->left + width;
	if (!g_theWorld->IsXwrap() && mapViewRect->right > GetMaxMapRight()) {
		mapViewRect->right = GetMaxMapRight();
		mapViewRect->left = mapViewRect->right - width;
	}

	mapViewRect->top = rect.top;
	if (!g_theWorld->IsYwrap() && mapViewRect->top < GetMinMapTop()) {
		mapViewRect->top = GetMinMapTop();
	}

	mapViewRect->bottom = mapViewRect->top + height;
	if (!g_theWorld->IsYwrap() && mapViewRect->bottom > GetMaxMapBottom()) {
		mapViewRect->bottom = GetMaxMapBottom();
		mapViewRect->top = mapViewRect->bottom - height;
	}

	primitives_SetPatternOffset(m_mapViewRect.left * m_zoomTilePixelWidth[m_zoomLevel],
			m_mapViewRect.top * (m_zoomTilePixelHeight[m_zoomLevel] / 2));
}

void TiledMap::CheckRectAgainstGrid
(
    RECT &          rect,
    aui_DirtyList * a_List
)
{
	sint32 x_start = (sint32)((float)rect.left   * m_one_over_gridWidth )-1;
	sint32 x_end   = (sint32)((float)rect.right  * m_one_over_gridWidth )+1;
	sint32 y_start = (sint32)((float)rect.top    * m_one_over_gridHeight)-1;
	sint32 y_end   = (sint32)((float)rect.bottom * m_one_over_gridHeight)+1;

	if(x_start<0)
	   x_start = 0;
	else
	   if(x_start>=m_gridWidth)
		  return;

	if(x_end>=m_gridWidth)
	   x_end =m_gridWidth-1;
	else
	   if(x_end<0)
		  return;

	if(y_start<0)
	   y_start = 0;
	else
	   if(y_start>=m_gridHeight)
		  return;

	if(y_end>=m_gridHeight)
	   y_end =m_gridHeight-1;
	else
	   if(y_end<0)
		  return;

	for (sint32 i=y_start; i<y_end; i++)
	{
		for (sint32 j=x_start; j<x_end; j++)
		{
			GridRect * gr = &m_gridRects[i][j];

			if(gr->dirty)
		       continue;

			if(rect.left>=gr->rect.right)
			   continue;

			if(rect.right<=gr->rect.left)
			   continue;

		    if(rect.top>=gr->rect.bottom)
			   continue;

		    if(rect.bottom<=gr->rect.top)
			   continue;

			gr->dirty = TRUE;
			a_List->AddRect(&gr->rect);

#ifdef _DEBUG
		    IncRectMetric();
#endif

		}
	}
}

void TiledMap::ClearGrid(void)
{
	for (sint32 i=0; i<m_gridHeight; i++) {
		for (sint32 j=0; j<m_gridWidth; j++) {
			m_gridRects[i][j].dirty = FALSE;
		}
	}
}

void TiledMap::LockSurface(void)
{
	LockThisSurface(m_surface);
}

void TiledMap::LockThisSurface(aui_Surface *surface)
{

	m_lockedSurface = surface;

	AUI_ERRCODE	errcode = surface->Lock(NULL, (LPVOID *)&m_surfBase, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return;

	m_surfWidth = surface->Width();
	m_surfHeight = surface->Height();
	m_surfPitch = surface->Pitch();
	m_surfIsLocked = TRUE;
}

void TiledMap::UnlockSurface(void)
{
	AUI_ERRCODE	errcode = m_lockedSurface->Unlock((LPVOID)m_surfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return;

	m_surfBase = NULL;
	m_surfWidth = 0;
	m_surfHeight = 0;
	m_surfPitch = 0;
	m_surfIsLocked = FALSE;
}

void TiledMap::AddDirty(sint32 left, sint32 top, sint32 width, sint32 height, aui_DirtyList * a_List)
{
	RECT		rect;

	rect.left = left;
	rect.top = top;
	rect.right = left + width;
	rect.bottom = top + height;

	AddDirtyRect(rect, a_List);
}

#define __GRIDDED_BLITS__

void TiledMap::AddDirtyRect(const RECT & rect, aui_DirtyList * a_List)
{
	if (a_List) {
		RECT tempRect = rect;

#ifdef __GRIDDED_BLITS__
	if (a_List == m_mixDirtyList) {
		CheckRectAgainstGrid(tempRect, a_List);
	} else {

		tempRect.left = tempRect.left & 0xFFFFFFFC;
		tempRect.right = (tempRect.right & 0xFFFFFFFC)+4;

		tempRect.top = tempRect.top & 0xFFFFFFFE;
		tempRect.bottom = (tempRect.bottom & 0xFFFFFFFE) + 2;

		a_List->AddRect(&tempRect);
	}
#else
	#ifdef __BIG_DIRTY_BLITS__

		tempRect.left = tempRect.left & 0xFFFFFF00;
		tempRect.right = (tempRect.right & 0xFFFFFF00)+256;

		tempRect.top = tempRect.top & 0xFFFFFFF0;
		tempRect.bottom = (tempRect.bottom & 0xFFFFFFF0) + 16;

		a_List->AddRect(&tempRect);
	#else

		tempRect.left = tempRect.left & 0xFFFFFFFC;
		tempRect.right = (tempRect.right & 0xFFFFFFFC)+4;

		tempRect.top = tempRect.top & 0xFFFFFFFE;
		tempRect.bottom = (tempRect.bottom & 0xFFFFFFFE) + 2;

		a_List->AddRect(&tempRect);
	#endif
#endif

	}
}

void TiledMap::AddDirtyTile(MapPoint &pos, aui_DirtyList * a_List)
{
	sint32		x, y;

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	RECT		rect;

	rect.left = x;
	rect.top = y;
	rect.right = x + k_TILE_GRID_WIDTH;
	rect.bottom = y + k_TILE_GRID_HEIGHT;

	AddDirtyRect(rect, a_List);
}

void TiledMap::AddDirtyToMap(sint32 left, sint32 top, sint32 width, sint32 height)
{
	AddDirty(left, top, width, height, m_mapDirtyList);
}

void TiledMap::AddDirtyRectToMap(RECT &rect)
{
	AddDirtyRect(rect, m_mapDirtyList);
}

void TiledMap::AddDirtyTileToMap(MapPoint &pos)
{
	AddDirtyTile(pos, m_mapDirtyList);
}

void TiledMap::AddDirtyToMix(sint32 left, sint32 top, sint32 width, sint32 height)
{
	AddDirty(left, top, width, height, m_mixDirtyList);
}

void TiledMap::AddDirtyRectToMix(const RECT & rect)
{
	AddDirtyRect(rect, m_mixDirtyList);
}

void TiledMap::AddDirtyTileToMix(MapPoint &pos)
{
	AddDirtyTile(pos, m_mixDirtyList);
}


void TiledMap::ClearMixDirtyRects(void)
{
	Assert (m_mixDirtyList);

	if (m_mixDirtyList)
		m_mixDirtyList->Flush();

	ClearGrid();
}

void TiledMap::CopyMixDirtyRects(aui_DirtyList *dest)
{
	dest->Flush();

	ListPos	position = m_oldMixDirtyList->GetHeadPosition();;
	sint32	i;
	for (i = m_oldMixDirtyList->L(); i > 0; --i)
	{
		dest->AddRect(m_oldMixDirtyList->GetNext(position));
	}

	m_oldMixDirtyList->Flush();

#ifdef __USING_SPANS__
	m_oldMixDirtyList->SetSpans(m_mixDirtyList);
#endif

	position = m_mixDirtyList->GetHeadPosition();
	for (i = m_mixDirtyList->L(); i > 0; --i)
	{
		RECT *rect = m_mixDirtyList->GetNext(position);
		dest->AddRect(rect);
		m_oldMixDirtyList->AddRect(rect);
	}
}






void TiledMap::RestoreMixFromMap(aui_Surface *destSurf)
{
#ifdef __USING_SPANS__
	g_c3ui->TheBlitter()->SpanBlt(
		destSurf,
		0,
		0,
		m_surface,
		m_oldMixDirtyList,
		k_AUI_BLITTER_FLAG_COPY );
#else
	m_oldMixDirtyList->Minimize();

	ListPos position = m_oldMixDirtyList->GetHeadPosition();

	for (sint32 i = m_oldMixDirtyList->L(); i > 0; --i)
	{
		RECT * rect = m_oldMixDirtyList->GetNext(position);
		g_c3ui->TheBlitter()->Blt(destSurf, rect->left, rect->top, m_surface, rect, k_AUI_BLITTER_FLAG_COPY);
	}
#endif
}

void TiledMap::OffsetMixDirtyRects(sint32 deltaX, sint32 deltaY)
{
#ifdef __USING_SPANS__

	deltaX = -deltaX, deltaY = -deltaY;

	aui_SpanList *curSpanList = m_oldMixDirtyList->GetSpans();

	sint32 height = m_oldMixDirtyList->GetHeight();
	if ( deltaY >= height || deltaY <= -height )
		memset( curSpanList, 0, height * sizeof( aui_SpanList ) );
	else
	{
		if ( deltaY > 0 )
		{
			memmove(
				curSpanList + deltaY,
				curSpanList,
				height - deltaY );
			memset(
				curSpanList,
				0,
				deltaY * sizeof( aui_SpanList ) );
		}
		else if ( deltaY < 0 )
		{
			memmove(
				curSpanList,
				curSpanList - deltaY,
				height + deltaY );
			memset(
				curSpanList + height + deltaY,
				0,
				-deltaY * sizeof( aui_SpanList ) );
		}
	}

	sint32 width = m_oldMixDirtyList->GetWidth();
	if ( deltaX >= width || deltaX <= -width )
		memset( curSpanList, 0, height * sizeof( aui_SpanList ) );
	else
	{
		if ( deltaX > 0 )
		{
			for ( sint32 i = height; i; i-- )
			{
				if ( curSpanList->num > 0 )
				{
					aui_Span *curSpan = curSpanList->spans;
					curSpan->run += (sint16)deltaX;

					sint32 right = 0;
					for ( sint32 num = curSpanList->num; num; num-- )
					{
						right += curSpan->run + curSpan->length;
						curSpan++;
					}

					curSpan = curSpanList->spans;

					if ( right > width )
					{

						curSpanList->num = 1;

						curSpan->run = 0;
						curSpan->length = (sint16)width;
					}
				}

				curSpanList++;
			}
		}
		else if ( deltaX < 0 )
		{
			for ( sint32 i = height; i; i-- )
			{
				if ( curSpanList->num > 0 )
				{
					aui_Span *curSpan = curSpanList->spans;
					if ( ( curSpan->run += (sint16)deltaX ) < 0 )
					{

						curSpanList->num = 1;

						curSpan->run = 0;
						curSpan->length = (sint16)width;
					}
				}

				curSpanList++;
			}
		}
	}
#else
	ListPos position = m_oldMixDirtyList->GetHeadPosition();

	for (sint32 i = m_oldMixDirtyList->L(); i > 0; --i)
	{
		RECT * rect = m_oldMixDirtyList->GetNext(position);

		OffsetRect(rect, -deltaX, -deltaY);
		if (rect->left < m_displayRect.left) rect->left = m_displayRect.left;
		if (rect->top < m_displayRect.top) rect->top = m_displayRect.top;
		if (rect->right > m_displayRect.right) rect->right = m_displayRect.right;
		if (rect->bottom > m_displayRect.bottom) rect->bottom = m_displayRect.bottom;
	}
#endif
}

void TiledMap::InvalidateMap(void)
{

	RECT tempRect = m_surfaceRect;

	m_mapDirtyList->Flush();

	AddDirtyRect(tempRect, m_mapDirtyList);
}

void TiledMap::ValidateMap(void)
{
	m_mapDirtyList->Flush();
}

void TiledMap::InvalidateMix(void)
{
	RECT tempRect = g_backgroundViewport;

	m_mixDirtyList->Flush();

	AddDirtyRect(tempRect, m_mixDirtyList);

	for (sint32 i=0; i<m_gridHeight; i++) {
		for (sint32 j=0; j<m_gridWidth; j++) {
			m_gridRects[i][j].dirty = TRUE;
		}
	}


m_oldMixDirtyList->Flush();
AddDirtyRect(g_backgroundViewport, m_oldMixDirtyList);
}

void TiledMap::ValidateMix(void)
{
	m_mixDirtyList->Flush();

	for (sint32 i=0; i<m_gridHeight; i++) {
		for (sint32 j=0; j<m_gridWidth; j++) {
			m_gridRects[i][j].dirty = FALSE;
		}
	}
	m_oldMixDirtyList->Flush();
}





void TiledMap::UpdateMixFromMap(aui_Surface *mixSurf)
{
#ifdef __USING_SPANS__

	ListPos position = m_mapDirtyList->GetHeadPosition();

	for (sint32 i = m_mapDirtyList->L(); i > 0; --i)
	{
		RECT * rect = m_mapDirtyList->GetNext(position);
		m_mixDirtyList->AddRect(rect);
	}

	g_c3ui->TheBlitter()->SpanBlt(
		mixSurf,
		0,
		0,
		m_surface,
		m_mapDirtyList,
		k_AUI_BLITTER_FLAG_COPY );

#else

	if (m_mapDirtyList->L() > 500) {

		InvalidateMap();
	} else {
		m_mapDirtyList->Minimize();
	}

	ListPos position = m_mapDirtyList->GetHeadPosition();

	for (sint32 i = m_mapDirtyList->L(); i > 0; --i)
	{
		RECT * rect = m_mapDirtyList->GetNext(position);
		g_c3ui->TheBlitter()->Blt(mixSurf, rect->left, rect->top, m_surface, rect, k_AUI_BLITTER_FLAG_COPY);
		m_mixDirtyList->AddRect(rect);
	}

#endif

	m_mapDirtyList->Flush();
}





void TiledMap::LoadTileset(void)
{
	TileSet *tileSet = new TileSet;
	tileSet->QuickLoadMapped();
	delete m_tileSet;
	m_tileSet = tileSet;
}

sint16 TiledMap::TryRiver(BOOL bc, BOOL bn, BOOL bne, BOOL be, BOOL bse, BOOL bs, BOOL bsw, BOOL bw, BOOL bnw, BOOL cwater)
{
	if (!bc) return -1;

	sint16		tc, tn, tne, te, tse, ts, tsw, tw, tnw;

	for (uint16 i = 0; i < m_tileSet->GetNumRiverTransforms(); i++)
    {
		tn =	m_tileSet->GetRiverTransform(i, 2);
		tne =	m_tileSet->GetRiverTransform(i, 5);
		te =	m_tileSet->GetRiverTransform(i, 6);
		tse =	m_tileSet->GetRiverTransform(i, 7);
		tc =	m_tileSet->GetRiverTransform(i, 4);
		ts =	m_tileSet->GetRiverTransform(i, 6);
		tsw =	m_tileSet->GetRiverTransform(i, 3);
		tw =	m_tileSet->GetRiverTransform(i, 0);
		tnw =	m_tileSet->GetRiverTransform(i, 1);

		if (	((cwater && (tc == 2)) || (bc && (tc==1)))
			&&	((bne && tne)	|| (!bne && !tne))
			&&	((bse && tse)	|| (!bse && !tse))
			&&	((bsw && tsw)	|| (!bsw && !tsw))
			&&	((bnw && tnw)	|| (!bnw && !tnw))
			) {

			return (sint16)i;
		}
	}

	return -1;
}

#define WILDCARD_ALL			-1
#define WILDCARD_LAND			-2
#define WILDCARD_WATER			-3
#define WILDCARD_DEEP			-4
#define WILDCARD_SHALLOW		-5
#define WILDCARD_TRENCHSHELF	-6
#define WILDCARD_ALL_BUT_SELF   -7
#define WILDCARD_LAND_NOT_SELF  -8
#define WILDCARD_WATER_NOT_SELF -9

#define IS_WATER(x) ((g_theTerrainDB->Get(x)->GetMovementTypeSea()) || (g_theTerrainDB->Get(x)->GetMovementTypeShallowWater()))

#define IS_LAND(x)          ((g_theTerrainDB->Get(x)->GetMovementTypeLand()) || (g_theTerrainDB->Get(x)->GetMovementTypeMountain()))

#define IS_DEEP(x)		((x == TERRAIN_WATER_DEEP) || (x == TERRAIN_WATER_VOLCANO) || (x == TERRAIN_WATER_SHELF) || (x == TERRAIN_WATER_TRENCH))
#define IS_SHALLOW(x)	((g_theTerrainDB->Get(x)->GetMovementTypeShallowWater()))
#define IS_TRENCHSHELF(x) ((x == TERRAIN_WATER_SHELF) || (x == TERRAIN_WATER_TRENCH))

#define MATCH_ALL(transform)				(transform == WILDCARD_ALL)
#define MATCH_WATER(transform, map)			((transform == WILDCARD_WATER) && IS_WATER(map))
#define MATCH_LAND(transform, map)			((transform == WILDCARD_LAND) && IS_LAND(map))
#define MATCH_DEEP(transform, map)			((transform == WILDCARD_DEEP) && IS_DEEP(map))
#define MATCH_SHALLOW(transform, map)		((transform == WILDCARD_SHALLOW) && IS_SHALLOW(map))
#define MATCH_TRENCHSHELF(transform, map)	((transform == WILDCARD_TRENCHSHELF) && IS_TRENCHSHELF(map))
#define MATCH_ALL_BUT_SELF(transform, map)  ((transform == WILDCARD_ALL_BUT_SELF) && ((c) != (map)))
#define MATCH_LAND_NOT_SELF(transform, map) ((transform == WILDCARD_LAND_NOT_SELF) && ((c) != (map)) && IS_LAND(map))
#define MATCH_WATER_NOT_SELF(transform, map) ((transform == WILDCARD_WATER_NOT_SELF) && ((c) != (map)) && IS_WATER(map))

#define MATCH_GLOB(t, m) (MATCH_ALL(t) || (t == g_theTerrainDB->Get(m)->GetTilesetIndex()) || MATCH_LAND(t, m) || MATCH_WATER(t, m) || MATCH_DEEP(t,m) || MATCH_SHALLOW(t,m) || MATCH_TRENCHSHELF(t,m) || MATCH_ALL_BUT_SELF(t,m) || MATCH_LAND_NOT_SELF(t,m) || MATCH_WATER_NOT_SELF(t,m))

bool TiledMap::TryTransforms(MapPoint &pos, uint16 c, uint16 n, uint16 ne, uint16 e, uint16 se,
							 uint16 s, uint16 sw, uint16 w, uint16 nw, uint16 *newIndex)
{
	for (uint16 i = 0; i < m_tileSet->GetNumTransforms(); i++)
    {
		sint16  tn      = m_tileSet->GetTransform(i,2);
		sint16  tne     = m_tileSet->GetTransform(i,5);
		sint16  te      = m_tileSet->GetTransform(i,8);
		sint16  tse     = m_tileSet->GetTransform(i,7);
		sint16  tc      = m_tileSet->GetTransform(i,4);
		sint16  ts      = m_tileSet->GetTransform(i,6);
		sint16  tsw     = m_tileSet->GetTransform(i,3);
		sint16  tw      = m_tileSet->GetTransform(i,0);
		sint16  tnw     = m_tileSet->GetTransform(i,1);
		sint16  tNew    = m_tileSet->GetTransform(i,9);

		if (MATCH_GLOB(tc,c) &&
			MATCH_GLOB(tn, n) &&
			MATCH_GLOB(tne, ne) &&
			MATCH_GLOB(te, e) &&
			MATCH_GLOB(tse, se) &&
			MATCH_GLOB(ts, s) &&
			MATCH_GLOB(tsw, sw) &&
			MATCH_GLOB(tw, w) &&
			MATCH_GLOB(tnw, nw)
           )
		{
			if (tNew == k_TRANSFORM_TO_LIST_ID) {

				sint16		xform;
				bool		legal = false;

                for (uint16 j = 0; j < k_MAX_TRANSFORM_TO_LIST; j++)
                {
					xform = m_tileSet->GetTransform(i, k_TRANSFORM_TO_LIST_FIRST + j);
					if (xform != k_TRANSFORM_TO_LIST_ID) legal = true;
				}

				if (!legal) return false;

				TileInfo *  ti      = GetTileInfo(pos);
				sint32      which   = ti->GetTransform() % k_MAX_TRANSFORM_TO_LIST;

				do {
					xform = (sint16)m_tileSet->GetTransform((uint16)i, (uint16)(k_TRANSFORM_TO_LIST_FIRST + which));
					which--;
					if(which < 0) {
						which = k_MAX_TRANSFORM_TO_LIST;
					}
					if(which == ti->GetTransform() % k_MAX_TRANSFORM_TO_LIST) {
						return false;
					}
				} while (xform == k_TRANSFORM_TO_LIST_ID || m_tileSet->GetBaseTile(xform) == NULL);

				*newIndex = xform;
			} else {

				*newIndex = tNew;
			}
			return true;
		}

	}

	return false;
}

void TiledMap::TryMegaTiles(MapPoint &pos, BOOL regenTilenum)
{
	if (m_tileSet->GetNumMegaTiles() < 1)
		return;

	MapPoint	newPos = pos;

    MapPoint	goodPath[k_MAX_MEGATILE_STEPS];
	uint16		goodPathTiles[k_MAX_MEGATILE_STEPS];
	uint8		goodPathLastDirs[k_MAX_MEGATILE_STEPS];
	uint8		goodPathNextDirs[k_MAX_MEGATILE_STEPS];

	MegaTileStep	step;

	sint32 i = rand() % m_tileSet->GetNumMegaTiles();

	for
    (
        sint32 tilesTried = 0;
        tilesTried < m_tileSet->GetNumMegaTiles();
        ++tilesTried
    )
    {
        std::fill(goodPath, goodPath + k_MAX_MEGATILE_STEPS, MapPoint());
        std::fill(goodPathTiles, goodPathTiles + k_MAX_MEGATILE_STEPS, 0);
        std::fill(goodPathLastDirs, goodPathLastDirs + k_MAX_MEGATILE_STEPS, 0);
        std::fill(goodPathNextDirs, goodPathNextDirs + k_MAX_MEGATILE_STEPS, 0);

		uint16  pathLen = m_tileSet->GetMegaTileLength(i);
		uint16  pathPos = 0;
		step = m_tileSet->GetMegaTileStep(i, pathPos);

		while (pathPos < pathLen)
        {
			TileInfo *  tileInfo    = GetTileInfo(newPos);
			if (tileInfo->GetTileNum() >= k_FIRST_VARIATION) break;

			sint32      terrainType = tileInfo->GetTerrainType();
			if (terrainType != (sint32)step.terrainType || tileInfo->IsMega())
				break;

			goodPath[pathPos] = newPos;
			goodPathTiles[pathPos] = (uint16)step.tileNum;

			pathPos++;

			if (pathPos < pathLen) {
				step = m_tileSet->GetMegaTileStep(i, pathPos);

				goodPathNextDirs[pathPos-1] = (uint8)step.direction;

				switch(step.direction) {
				case k_MEGATILE_DIRECTION_N :
					newPos.GetNeighborPosition(NORTHWEST, newPos);
					break;
				case k_MEGATILE_DIRECTION_E :
					newPos.GetNeighborPosition(NORTHEAST, newPos);
					break;
				case k_MEGATILE_DIRECTION_S :
					newPos.GetNeighborPosition(SOUTHEAST, newPos);
					break;
				case k_MEGATILE_DIRECTION_W :
					newPos.GetNeighborPosition(SOUTHWEST, newPos);
					break;
				}

				goodPathLastDirs[pathPos] = m_tileSet->ReverseDirection((sint32)step.direction);
			}
		}

		if ((pathPos == pathLen) && regenTilenum)
        {
			for (uint16 j = 0; j < pathLen; j++)
            {
				TileInfo *	theTileInfo = g_theWorld->GetTileInfo(goodPath[j]);

				theTileInfo->SetTileNum(goodPathTiles[j]);
				theTileInfo->SetLastMega(goodPathLastDirs[j]);
				theTileInfo->SetNextMega(goodPathNextDirs[j]);
			}
		}

		if (++i >= m_tileSet->GetNumMegaTiles()) i = 0;
	}
}

void TiledMap::PostProcessTile(MapPoint &pos, TileInfo *theTileInfo,
							   BOOL regenTilenum)
{
	if (theTileInfo->HasGoodActor())
		theTileInfo->DeleteGoodActor();

	sint32			goodIndex;
	if(g_theWorld->GetGood(pos, goodIndex)) {
		theTileInfo->SetGoodActor(g_theResourceDB->Get(goodIndex)->GetSpriteID(), pos);
	}

	uint8 index = static_cast<uint8>(g_theWorld->GetTerrain(pos.x, pos.y));
	if(regenTilenum)
		theTileInfo->SetTileNum(static_cast<TILEINDEX>(g_theTerrainDB->Get(index)->GetTilesetIndex()));

	theTileInfo->SetTerrainType(index);




	uint16	n, ne, e, se, s, sw, w, nw;
	BOOL		rn, rne, re, rse, rs, rsw, rw, rnw;

	uint16 c = static_cast<uint16>(g_theTerrainDB->Get(index)->GetTilesetIndex());
	BOOL	rc = g_theWorld->IsRiver(pos.x, pos.y);

	MapPoint	newPos;
	if(pos.GetNeighborPosition(NORTH, newPos)) {

		n = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rn = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		n = index;
		rn = FALSE;
	}

	if(pos.GetNeighborPosition(SOUTH, newPos)) {

		s = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rs = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		s = index;
		rs = FALSE;
	}

	if(pos.GetNeighborPosition(EAST, newPos)) {

		e = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		re = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		e = index;
		re = FALSE;
	}

	if(pos.GetNeighborPosition(WEST, newPos)) {

		w = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rw = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		w = index;
		rw = FALSE;
	}

	if(pos.GetNeighborPosition(SOUTHWEST, newPos)) {

		sw = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rsw = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		sw = index;
		rsw = FALSE;
	}

	if(pos.GetNeighborPosition(NORTHWEST, newPos)) {

		nw = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rnw = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		nw = index;
		rnw = FALSE;
	}

	if(pos.GetNeighborPosition(NORTHEAST, newPos)) {

		ne = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rne = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		ne = index;
		rne = FALSE;
	}

	if(pos.GetNeighborPosition(SOUTHEAST, newPos)) {

		se = static_cast<uint16>(g_theWorld->GetTerrain(newPos.x, newPos.y));
		rse = g_theWorld->IsRiver(newPos.x, newPos.y);
	} else {
		se = index;
		rse = FALSE;
	}

	theTileInfo->SetTransition(0, static_cast<uint16>(g_theTerrainDB->Get(sw)->GetTilesetIndex()));
	theTileInfo->SetTransition(1, static_cast<uint16>(g_theTerrainDB->Get(nw)->GetTilesetIndex()));
	theTileInfo->SetTransition(2, static_cast<uint16>(g_theTerrainDB->Get(ne)->GetTilesetIndex()));
	theTileInfo->SetTransition(3, static_cast<uint16>(g_theTerrainDB->Get(se)->GetTilesetIndex()));






	uint16 newIndex;

	if (TryTransforms(pos, index, n, ne, e, se, s, sw, w, nw, &newIndex)) {
		if(regenTilenum)
			theTileInfo->SetTileNum(newIndex);
	}

	if (rc) {
		BOOL	cwater = FALSE;
		if (c == TERRAIN_WATER_BEACH) {
			if (n == TERRAIN_WATER_BEACH) rn = FALSE;
			if (ne == TERRAIN_WATER_BEACH) rne = FALSE;
			if (e == TERRAIN_WATER_BEACH) re = FALSE;
			if (se == TERRAIN_WATER_BEACH) rse = FALSE;
			if (s == TERRAIN_WATER_BEACH) rs = FALSE;
			if (sw == TERRAIN_WATER_BEACH) rsw = FALSE;
			if (w == TERRAIN_WATER_BEACH) rw = FALSE;
			if (nw == TERRAIN_WATER_BEACH) rnw = FALSE;

			cwater = TRUE;
		}

		sint16	river = TryRiver(rc, rn, rne, re, rse, rs, rsw, rw, rnw, cwater);

		theTileInfo->SetRiverPiece(river);
	} else {

		theTileInfo->SetRiverPiece(-1);
	}

}




void TiledMap::PostProcessMap(BOOL regenTilenums)
{
	MapPoint		pos;
	sint16			i,j;
	TILEINDEX       origTilenum = 0;
	uint8           origMega = 0;

	Assert (g_theWorld != NULL);
	if (g_theWorld == NULL) return;

//	MapPoint * size = g_theWorld->GetSize();

	TileInfo		*theTileInfo = NULL;

	for (i=0; i<m_mapBounds.bottom; i++) {
		for (j=0; j<m_mapBounds.right; j++) {

			pos.x = j;
			pos.y = i;

			theTileInfo = g_theWorld->GetTileInfoStoragePtr(pos);

			if(!regenTilenums) {
				origTilenum = theTileInfo->GetTileNum();
				origMega = theTileInfo->GetMega();
			}

			PostProcessTile(pos, theTileInfo, regenTilenums);

			if(!regenTilenums) {
				theTileInfo->SetTileNum(origTilenum);
				theTileInfo->SetMega(origMega);
			}










		}
	}

	for (i=0; i<m_mapBounds.bottom; i++) {
		for (j=0; j<m_mapBounds.right; j++) {

			pos.x = j;
			pos.y = i;

			if(!regenTilenums) {
				theTileInfo = g_theWorld->GetTileInfoStoragePtr(pos);
				origTilenum = theTileInfo->GetTileNum();
				origMega = theTileInfo->GetMega();
			}
			TryMegaTiles(pos, regenTilenums);
			if(!regenTilenums) {
				theTileInfo->SetTileNum(origTilenum);
				theTileInfo->SetMega(origMega);
			}
		}
	}
}

void TiledMap::BreakMegaTile(MapPoint &pos)
{
	TileInfo * tileInfo = GetTileInfo(pos);
	Assert(tileInfo);
	if (tileInfo == NULL) return;

	tileInfo->SetTileNum((TILEINDEX)g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex());
	PostProcessTile(pos, tileInfo);
	RedrawTile(&pos);

	MapPoint	curPos = pos;
	bool isAtMap = true;
	while (sint32 next = tileInfo->GetNextMega())
	{
		switch (next)
		{
		case k_MEGATILE_DIRECTION_N :
			isAtMap = curPos.GetNeighborPosition(NORTHWEST, curPos);
			break;
		case k_MEGATILE_DIRECTION_E :
			isAtMap = curPos.GetNeighborPosition(NORTHEAST, curPos);
			break;
		case k_MEGATILE_DIRECTION_S :
			isAtMap = curPos.GetNeighborPosition(SOUTHEAST, curPos);
			break;
		case k_MEGATILE_DIRECTION_W :
			isAtMap = curPos.GetNeighborPosition(SOUTHWEST, curPos);
			break;
		default:
			Assert(FALSE);
		}

		if(!isAtMap)
		{
			break;
		}

		tileInfo = GetTileInfo(curPos);
		tileInfo->SetTileNum((TILEINDEX)g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex());
		PostProcessTile(curPos, tileInfo);
		RedrawTile(&curPos);
	}

	curPos = pos;
	while (sint32 last = tileInfo->GetLastMega())
	{
		switch (last)
		{
		case k_MEGATILE_DIRECTION_N :
			isAtMap = curPos.GetNeighborPosition(NORTHWEST, curPos);
			break;
		case k_MEGATILE_DIRECTION_E :
			isAtMap = curPos.GetNeighborPosition(NORTHEAST, curPos);
			break;
		case k_MEGATILE_DIRECTION_S :
			isAtMap = curPos.GetNeighborPosition(SOUTHEAST, curPos);
			break;
		case k_MEGATILE_DIRECTION_W :
			isAtMap = curPos.GetNeighborPosition(SOUTHWEST, curPos);
			break;
		default:
			Assert(FALSE);
		}

		if(!isAtMap)
		{
			break;
		}

		tileInfo = GetTileInfo(curPos);
		tileInfo->SetTileNum((TILEINDEX)g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex());
		PostProcessTile(curPos, tileInfo);
		RedrawTile(&curPos);
	}
}

void TiledMap::TileChanged(MapPoint &pos)
{
	TileInfo * tileInfo = GetTileInfo(pos);

	if (tileInfo && tileInfo->IsMega())
	{
		BreakMegaTile(pos);
	}
}


void TiledMap::ReloadGoodActors(void)
{
	Assert (g_theWorld);
	if (g_theWorld == NULL) return;

	LOADTYPE const	loadType = (g_theProfileDB->IsGoodAnim()) ? LOADTYPE_FULL : LOADTYPE_BASIC;

	for (sint16 i = 0; i < m_mapBounds.bottom; ++i)
	{
		for (sint16 j = 0; j < m_mapBounds.right; ++j)
		{
			TileInfo * theTileInfo = g_theWorld->GetTileInfoStoragePtr(MapPoint(j, i));

			if (theTileInfo)
			{
				GoodActor * goodActor = theTileInfo->GetGoodActor();
				if (goodActor) {
					goodActor->Reload(loadType);
				}
			}
		}
	}
}

void TiledMap::GenerateHitMask(void)
{
 	sint32 const    startLine = k_TILE_PIXEL_HEADROOM;
	sint32 const    midLine = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM) / 2;
	sint32 const    endLine = k_TILE_GRID_HEIGHT;

	sint32		i;
	for (i = 0; i < k_TILE_PIXEL_HEADROOM; ++i)
	{
		m_tileHitMask[i].start = 1;
		m_tileHitMask[i].end = 0;
		m_tileHitMask[i].d_start = 1.0;
		m_tileHitMask[i].d_end   = 0.0;
	}

	uint16	startPos = k_TILE_GRID_WIDTH/2 - 1;
	uint16 	endPos = k_TILE_GRID_WIDTH/2;

	for (i = startLine; i < midLine; ++i)
	{
		m_tileHitMask[i].start = startPos;
		m_tileHitMask[i].end = endPos;
		m_tileHitMask[i].d_start = (double)startPos;
		m_tileHitMask[i].d_end   = (double)endPos;

		startPos -= 2;
		endPos += 2;
	}

	startPos = 0;
	endPos = k_TILE_GRID_WIDTH - 1;

	for (i = midLine; i < endLine; ++i)
	{
		m_tileHitMask[i].start = startPos;
		m_tileHitMask[i].end = endPos;
		m_tileHitMask[i].d_start = (double)startPos;
		m_tileHitMask[i].d_end   = (double)endPos;

		startPos += 2;
		endPos -= 2;
	}
}

void TiledMap::SetHiliteMouseTile(MapPoint &pos)
{
	m_hiliteMouseTile = pos;
}

void TiledMap::DrawHiliteMouseTile(aui_Surface *destSurf)
{
	if ( !m_drawHilite ) return;

	if (ScenarioEditor::DrawRegion())
	{
		MapPoint ul = ScenarioEditor::GetRegionUpperLeft();
		sint32 w = ScenarioEditor::GetRegionWidth();
		sint32 h = ScenarioEditor::GetRegionHeight();

		// x and y are orthogonal coordinates now
		for (sint16 y = 0; y < h; ++y)
		{
			for (sint16 x = (y & 1); x < (2 * w); x += 2)
			{
				OrthogonalPoint	cur(ul);
				cur.Move(MapPointData(x, y));
				if (cur.IsValid())
				{
					DrawHitMask(destSurf, cur.GetRC());
				}
			}
		}
	}

	DrawHitMask(destSurf, m_hiliteMouseTile);
}

sint32 TiledMap::RecalculateViewRect(RECT &myRect)
{

	CalculateZoomViewRectangle(GetZoomLevel(), myRect);

	return(0);
}

sint32 TiledMap::CalculateMetrics(void)
{
	sint32 displayWidth = GetZoomDisplayWidth();
	sint32 displayHeight = GetZoomDisplayHeight();
	RECT mapViewRect = { 0, 0, (sint32)((displayWidth-(GetZoomTilePixelWidth()/2))/(GetZoomTilePixelWidth())),
					(sint32)((displayHeight-GetZoomTileHeadroom())/(GetZoomTilePixelHeight()/2)-1) };
	UpdateAndClipMapViewRect(mapViewRect);

	return 0;
}

#if defined(_PLAYTEST)
extern sint32 g_is_debug_map_color;
#endif
extern uint16 myRGB(sint32 r,  sint32 g, sint32 b);

sint32 TiledMap::CalculateWrap
(
    aui_Surface *   surface,
    sint32          i,
    sint32          j
)
{
	Assert(m_localVision != NULL);

	maputils_WrapPoint(j, i, &j, &i);
	MapPoint tempPos    = MapPoint(maputils_TileX2MapX(j, i), i);

	if (!ReadyToDraw() || !m_localVision->IsExplored(tempPos))
	{
		BlackTile(surface, &tempPos);
		return 0;
	}

	MapPoint	pos = tempPos;
	sint32		x;
	sint32      y;
	maputils_MapXY2PixelXY(pos.x,pos.y,&x,&y);

	if (    (x < m_surfaceRect.left)
	     || (x > (m_surfaceRect.right - GetZoomTilePixelWidth()))
	     || (y < m_surfaceRect.top)
	     || (y > (m_surfaceRect.bottom - (GetZoomTilePixelHeight() + GetZoomTileHeadroom())))
	   )
	{
		// Outside displayed surface
		return 0;
	}

	TileInfo * tileInfo = GetTileInfo(pos);
	if (tileInfo == NULL) return -1;

	BaseTile * baseTile = m_tileSet->GetBaseTile(tileInfo->GetTileNum());
	if (baseTile == NULL) return -1;

	sint32  terrainType;
	bool    fog = !m_localVision->IsVisible(tempPos);
	if (fog)
	{
		UnseenCellCarton ucell;
		if (m_localVision->GetLastSeen(tempPos, ucell))
		{
			terrainType = ucell.m_unseenCell->GetTerrainType();
		}
		else
		{
			terrainType = g_theWorld->GetTerrain(tempPos.x,tempPos.y);
		}
	}
	else
	{
		terrainType = g_theWorld->GetTerrain(tempPos.x, tempPos.y);
	}

	sint16		river = tileInfo->GetRiverPiece();

	if (m_zoomLevel == k_ZOOM_LARGEST)
	{
		if (!fog)
		{
			DrawTransitionTile(surface, pos, x, y);

			if (river != -1)
				DrawOverlay(surface, m_tileSet->GetRiverData(river), x, y);
		}
		else
		{
			if (g_isFastCpu) {

				DrawBlendedTile(surface, pos,x,y,k_FOW_COLOR,k_FOW_BLEND_VALUE);
				if (river != -1)
					DrawBlendedOverlay(surface, m_tileSet->GetRiverData(river),x,y,k_FOW_COLOR,k_FOW_BLEND_VALUE);
			} else {

				DrawDitheredTile(surface, x,y,k_FOW_COLOR);

				if (river != -1)
					DrawDitheredOverlay(surface, m_tileSet->GetRiverData(river),x,y,k_FOW_COLOR);
			}
		}

		if (g_isGridOn)
			DrawTileBorder(surface, x,y,(g_colorSet->GetColor(COLOR_BLACK)));


		AddDirtyToMap(x, y, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);

	} else {

		if (!fog) {

			DrawTransitionTileScaled(surface, pos, x, y, GetZoomTilePixelWidth(),
														GetZoomTilePixelHeight());

			if (river != -1)
				DrawScaledOverlay(surface, m_tileSet->GetRiverData(river),
									x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
		} else {

			if (g_isFastCpu) {

				DrawBlendedTileScaled(surface, pos, x, y, GetZoomTilePixelWidth(), GetZoomTilePixelHeight(),
										k_FOW_COLOR,k_FOW_BLEND_VALUE);

				if (river != -1)
					DrawBlendedOverlayScaled(surface, m_tileSet->GetRiverData(river),
								x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight(),
								k_FOW_COLOR, k_FOW_BLEND_VALUE);
			} else {

				DrawDitheredTileScaled(surface, pos, x, y, GetZoomTilePixelWidth(),
										GetZoomTilePixelHeight(),k_FOW_COLOR);

				if (river != -1)
					DrawDitheredOverlayScaled(surface, m_tileSet->GetRiverData(river),
								x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight(),
								k_FOW_COLOR);
			}
		}

		if (g_isGridOn)
			DrawTileBorderScaled(surface, pos, x, y, GetZoomTilePixelWidth(), GetZoomTilePixelHeight(), g_colorSet->GetColor(COLOR_BLACK));




		AddDirtyToMap(x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
	}

	if (g_graphicsOptions)
	{
		if (g_graphicsOptions->IsCellTextOn())
		{
			CellText *cellText = g_graphicsOptions->GetCellText(pos);
			if (cellText != NULL)
			{
				sint32 r,g,b;
				ColorMagnitudeToRGB(cellText->m_color, &r, &g, &b);

				COLORREF fgColor = RGB(r, g, b);
				COLORREF bgColor = RGB(0, 0, 0);

				DrawSomeText(false,
							 cellText->m_text,
							 x + GetZoomTilePixelWidth()/2,
							 y + GetZoomTilePixelHeight(),
							 bgColor,
							 fgColor
							);
			}
		}
	}

#if defined(_DEBUG) && defined(CELL_COLOR)

	if (g_is_debug_map_color)
	{
		sint32 color = g_theWorld->GetColor(pos);
		if (0 < color)
		{
			sint32 r=0,g=0, b;
			b = color * 2;
			if (127 < b) {
				g = b - 127;
				b = 0;
			}
			if (127 < g) {
				r = g - 127;
				g = 0;
			}

			uint16 c = myRGB(r, g, b);
			DrawNumber(surface,
			           color,
			           c,
			           (sint32)(x+(k_TILE_PIXEL_WIDTH*m_scale)/2),
			           (sint32)(y+(k_TILE_PIXEL_HEIGHT*m_scale))
			          );
		}
	}
#endif

	return 0;
}

sint32 TiledMap::CalculateWrapClipped(
			aui_Surface *surface,
			sint32 i,
			sint32 j
			)
{
	Assert(m_localVision != NULL);

	sint32 drawx = j;
	maputils_WrapPoint(j,i,&j,&i);

	sint32 drawy = i;
	maputils_TileX2MapXAbs(drawx,drawy,&drawx);

	MapPoint tempPos = MapPoint(maputils_TileX2MapX(j, i), i);

	if (!m_localVision->IsExplored(tempPos) || !ReadyToDraw())
	{
		return 0;
	}

	sint32	terrainType;

	bool    fog = !m_localVision->IsVisible(tempPos);

	if (fog)
	{
		UnseenCellCarton ucell;

		if (m_localVision->GetLastSeen(tempPos, ucell))
        {
			terrainType = ucell.m_unseenCell->GetTerrainType();
        }
		else
        {
			terrainType = g_theWorld->GetTerrain(tempPos.x,tempPos.y);
        }
	}
	else
	{
		terrainType = g_theWorld->GetTerrain(tempPos.x, tempPos.y);
	}

	MapPoint    pos = tempPos;

	maputils_MapXY2PixelXY(drawx,drawy,&drawx,&drawy);

	sint32      baseX,baseY;
	maputils_TileX2MapXAbs(m_mapViewRect.left,m_mapViewRect.top,&baseX);

	maputils_MapXY2PixelXY(baseX,m_mapViewRect.top,&baseX,&baseY);

	TileInfo *tileInfo = GetTileInfo(pos);

	if (tileInfo == NULL)
		return -1;

	BaseTile *baseTile = m_tileSet->GetBaseTile(tileInfo->GetTileNum());

	if (baseTile == NULL)
		return -1;

	if (m_zoomLevel == k_ZOOM_LARGEST)
	{
		if (!fog)
		{
		   	DrawTransitionTileClipped(surface, pos, drawx, drawy);
		}

		AddDirtyToMap(drawx, drawy, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);
	}
	else
	{
		sint16		river = tileInfo->GetRiverPiece();

		if (!fog)
		{
			if (river != -1)
				DrawScaledOverlay(surface, m_tileSet->GetRiverData(river),
									drawx, drawy, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
		}
		else
		{
			if (g_isFastCpu)
			{

				DrawBlendedTileScaled(surface, pos, drawx, drawy, GetZoomTilePixelWidth(), GetZoomTilePixelHeight(),
										k_FOW_COLOR,k_FOW_BLEND_VALUE);

				if (river != -1)
					DrawBlendedOverlayScaled(surface, m_tileSet->GetRiverData(river),
								drawx, drawy, GetZoomTilePixelWidth(), GetZoomTileGridHeight(),
								k_FOW_COLOR, k_FOW_BLEND_VALUE);
			}
			else
			{

				DrawDitheredTileScaled(surface, pos, drawx, drawy, GetZoomTilePixelWidth(),
										GetZoomTilePixelHeight(),k_FOW_COLOR);

				if (river != -1)
					DrawDitheredOverlayScaled(surface, m_tileSet->GetRiverData(river),
								drawx, drawy, GetZoomTilePixelWidth(), GetZoomTileGridHeight(),
								k_FOW_COLOR);
			}
		}

		if (g_isGridOn)
			DrawTileBorderScaled(surface, pos, drawx, drawy, GetZoomTilePixelWidth(), GetZoomTilePixelHeight(), g_colorSet->GetColor(COLOR_BLACK));

		AddDirtyToMap(drawx, drawy, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
	}

	return 0;
}

sint32 TiledMap::DrawImprovements(aui_Surface *surface,
			sint32 i,
			sint32 j,
			bool clip
			)
{
	maputils_WrapPoint(j,i,&j,&i);

	MapPoint            tempPos = MapPoint(maputils_TileX2MapX(j, i), i);
	MapPoint            pos     = tempPos;
	UnseenCellCarton	ucell;
	Cell *              cell;
	uint32				env;
	sint32				numImprovements;
    sint32              numDBImprovements;
	bool				hasGoody = false;

	bool visiblePlayerOwnsThis = g_selected_item->GetVisiblePlayer() == g_theWorld->GetOwner(pos);

// Added by Martin Gühmann
	if(!g_fog_toggle // The sense of toogling off the fog is to see something
	&& !visiblePlayerOwnsThis
	&& m_localVision->GetLastSeen(pos, ucell)
//	&& ucell.m_unseenCell->GetImprovements()->GetCount() > 0
	){
		env = ucell.m_unseenCell->GetEnv();
		numDBImprovements = 0; // Maybe has to be reconsidered
		numImprovements = ucell.m_unseenCell->GetImprovements()->GetCount();
		hasGoody = ucell.m_unseenCell->HasHut();
	}
	else
	{
		cell = g_theWorld->GetCell(pos);

		if (cell==NULL)
		   return 0;

		env = cell->GetEnv();

		numDBImprovements	= cell->GetNumDBImprovements();
		numImprovements		= cell->GetNumImprovements();

		hasGoody = (g_theWorld->GetGoodyHut(pos) != NULL);
	}

	uint32 mask = (k_MASK_ENV_INSTALLATION |
					k_MASK_ENV_MINE |
					k_MASK_ENV_IRRIGATION |
					k_MASK_ENV_ROAD |
					k_MASK_ENV_CANAL_TUNNEL);

	if (!(env & mask) &&
		(numImprovements==0) &&
		(numDBImprovements==0) &&
		!hasGoody)
		return 0;

	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x,pos.y,&x,&y);

	if(x < m_surfaceRect.left || x > (m_surfaceRect.right - GetZoomTilePixelWidth()) ||
		y < m_surfaceRect.top || y > (m_surfaceRect.bottom - GetZoomTileGridHeight())) {
		return 0;
	}

	if (DrawImprovementsLayer(surface, pos, x, y,clip))
	{
		if (m_zoomLevel == k_ZOOM_LARGEST)
		{
			AddDirtyToMap(x, y, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);
		}
		else
		{
			AddDirtyToMap(x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
		}
	}

	return 0;
}

void TiledMap::RetargetTileSurface(aui_Surface *surf)
{
	m_surface = (surf) ? surf : m_mapSurface;
}

void TiledMap::RepaintTiles(const RECT & repaintRect)
{
	sint32		mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);

	for (sint32 i = repaintRect.top; i < repaintRect.bottom; i++){
		if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
			for (sint32 j = repaintRect.left; j < repaintRect.right; j++) {
				if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
					CalculateWrap(NULL,i,j);
				}
			}
		}
	}
}

void TiledMap::RepaintTilesClipped(const RECT & repaintRect)
{
	for (sint32 i = repaintRect.top; i <= repaintRect.bottom; i++)
	{
		for (sint32 j = repaintRect.left; j <= repaintRect.right; j++)
			CalculateWrapClipped(NULL,i,j);
	}
}

void TiledMap::RepaintImprovements(const RECT & repaintRect, bool clip)
{
	sint32		mapWidth, mapHeight;
	sint32		i;

	GetMapMetrics(&mapWidth,&mapHeight);

	for (i = repaintRect.top; i < repaintRect.bottom; i++){
		if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
			for (sint32 j = repaintRect.left; j < repaintRect.right; j++) {
				if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
					DrawImprovements(NULL,i,j,0);
				}
			}
		}
	}
}

sint32 TiledMap::CalculateHatWrap(
			aui_Surface *surface,
			sint32 i,
			sint32 j
			)
{
	maputils_WrapPoint(i,j,&i,&j);

	sint32 mapX = maputils_TileX2MapX(i,j);

	MapPoint pos(mapX, j);

	TileInfo *tileInfo;

	bool    fog = !m_localVision->IsVisible(pos);
	if (fog)
	{
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell)) {
			tileInfo = ucell.m_unseenCell->GetTileInfo();
		} else {
			tileInfo = GetTileInfo(pos);
		}
	} else {
		tileInfo = GetTileInfo(pos);
	}

	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x,pos.y,&x,&y);

	if (tileInfo == NULL) return -1;

	BaseTile *baseTile = m_tileSet->GetBaseTile(tileInfo->GetTileNum());
	if (baseTile == NULL) return -1;

	if (m_zoomLevel == k_ZOOM_LARGEST) {

		if (!fog) {
			DrawOverlay(surface, baseTile->GetHatData(), x, y);
		} else {
			if (g_isFastCpu)
				DrawBlendedOverlay(surface, baseTile->GetHatData(),x,y,k_FOW_COLOR,k_FOW_BLEND_VALUE);
			else
				DrawDitheredOverlay(surface, baseTile->GetHatData(),x,y,k_FOW_COLOR);
		}

		AddDirtyToMap(x, y, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);
	} else {

		if (!fog) {
			DrawScaledOverlay(surface, baseTile->GetHatData(), x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
		} else {
			if (g_isFastCpu) {
				DrawBlendedOverlayScaled(surface, baseTile->GetHatData(), x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight(),k_FOW_COLOR,k_FOW_BLEND_VALUE);
			} else {
				DrawDitheredOverlayScaled(surface, baseTile->GetHatData(), x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight(),k_FOW_COLOR);
			}
		}

		AddDirtyToMap(x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight());

	}
	return 0;
}


void TiledMap::RepaintHats(const RECT & repaintRect, bool clip)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth, &mapHeight);

	for (sint32 j = repaintRect.top; j < repaintRect.bottom;j++)
	{
		if (g_theWorld->IsYwrap() || ((j >= 0) && (j <= mapHeight)) || clip)
		{
			for (sint32 i = repaintRect.left; i <= repaintRect.right;i++)
			{
				if (g_theWorld->IsXwrap() || (i >= 0 && i < mapWidth)||clip)
					RedrawHat(NULL, j,i,clip);
			}
		}
	}
}

void TiledMap::RepaintBorders(const RECT & repaintRect, bool clip)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth, &mapHeight);

	for (sint32 j = repaintRect.top; j < repaintRect.bottom; j++)
	{
		if (g_theWorld->IsYwrap() || ((j >= 0) && (j <= mapHeight)) || clip)
		{
			for (sint32 i = repaintRect.left; i<= repaintRect.right; i++)
			{
				if (g_theWorld->IsXwrap() || (i >= 0 && i < mapWidth)||clip)
					RedrawBorders(NULL, j, i, clip);
			}
		}
	}
}

void TiledMap::RepaintEdgeX(const RECT & repaintRect)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);

	if (repaintRect.left < 0) {

		RECT erase = {0,0,(sint32)((k_TILE_PIXEL_WIDTH+k_TILE_PIXEL_WIDTH/2)*m_scale),m_surface->Height()};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		for (sint32 i = m_mapViewRect.top; i < m_mapViewRect.bottom; i++){
			if (g_theWorld->IsYwrap() || (i>=0 && i < mapHeight)) {
				for (sint32 j=0; j<1; j++) {
					if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}

	if (repaintRect.right > m_mapBounds.right-1) {

		RECT erase = {m_surface->Width() - (sint32)((repaintRect.right-(m_mapBounds.right-2))*k_TILE_PIXEL_WIDTH*m_scale),0,m_surface->Width(),m_surface->Height()};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		for (sint32 i = m_mapViewRect.top; i < m_mapViewRect.bottom; i++){
			if (g_theWorld->IsYwrap() || (i>=0 && i < mapHeight)) {
				for (sint32 j = m_mapBounds.right-2; j < m_mapBounds.right; j++) {
					if (g_theWorld->IsXwrap() || (j >=0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}
}

void TiledMap::RepaintEdgeY(const RECT & repaintRect)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);

	if (repaintRect.top < 0) {

		RECT erase = {0,0,m_surface->Width(),(sint32)(k_TILE_PIXEL_HEIGHT*2*m_scale)};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		for (sint32 i = 0; i < 1; i++) {
			if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
				for (sint32 j = m_mapViewRect.left; j < m_mapViewRect.right; j++) {
					if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}

	if (repaintRect.bottom > m_mapBounds.bottom) {

		RECT erase = {0,
						m_surface->Height()-(sint32)((k_TILE_PIXEL_HEIGHT*2)*m_scale),
						m_surface->Width(),
						m_surface->Height()};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		for (sint32 i = m_mapBounds.bottom-3; i < m_mapBounds.bottom; i++) {
			if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
				for (sint32 j = m_mapViewRect.left; j < m_mapViewRect.right; j++) {
					if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}
}

void TiledMap::ColorMagnitudeToRGB(uint8 col, sint32 *r, sint32 *g, sint32 *b)
{
	if (col < 128)
	{
		*r = 255 - (col * 2);
		*g = (col * 2);
		*b = 0;
	}
    else
    {
        // col >= 128
		*r = 0;
		*g = 255 - ((col-128) * 2);
		*b = (col - 128) * 2;
	}
}

void TiledMap::DrawSomeText
(
    bool            mixingPort,
	MBCHAR const *  text,
	sint32          tx,
    sint32          ty,
	COLORREF        bgColorRef,
	COLORREF        fgColorRef
)
{
	if (text == NULL) return;
	if (strlen(text) < 1) return;
	if (m_font == NULL) return;

	aui_Surface		*surface;
	if (mixingPort) {
		if (!g_screenManager) return;

		surface = g_screenManager->GetSurface();
		if (!surface) return;
	} else {
		surface = m_mapSurface;
	}

	sint32  width = m_font->GetStringWidth(text);
	sint32  height = m_font->GetMaxHeight();

	tx = tx - (width / 2);
	ty = ty - (height / 2);

	RECT		tempRect = {0, 0, width, height};
	OffsetRect(&tempRect, tx, ty);

	if (mixingPort) {
		g_screenManager->UnlockSurface();
	}

    m_font->DrawString(surface, &tempRect, &tempRect, text, 0, bgColorRef, 0);
	OffsetRect(&tempRect, -1, -1);
	m_font->DrawString(surface, &tempRect, &tempRect, text, 0, fgColorRef, 0);

	tempRect.right++;
	tempRect.bottom++;

	if (mixingPort) {
		g_screenManager->LockSurface(surface);
		AddDirtyRectToMix(tempRect);
	} else {
		AddDirtyRectToMap(tempRect);
	}
}

#define k_UNIT_1_OFFSET_X		10
#define k_UNIT_1_OFFSET_Y		-10
#define k_UNIT_2_OFFSET_X		-10
#define k_UNIT_2_OFFSET_Y		-10
#define k_UNIT_3_OFFSET_X		0
#define k_UNIT_3_OFFSET_Y		0

void TiledMap::PaintArmyActors(MapPoint &pos)
{
#if 0

	Unit		topUnit;
	UnitActor	*unitActor1 = NULL,
				*unitActor2 = NULL,
				*unitActor3 = NULL;

	if (!g_theWorld->GetTopVisibleUnit(pos, topUnit)) return;

	unitActor1 = topUnit.GetActor();

	Army		theArmy = topUnit.GetArmy();

	for (sint32 i=0; i<theArmy.Num(); i++) {
		Unit	unit = theArmy.Get(i);
		if (unit == topUnit)
			continue;

		if (unitActor2 == NULL) {
			unitActor2 = unit.GetActor();
		} else if (unitActor3 == NULL) {
			unitActor3 = unit.GetActor();
		}
	}

	sint32	x, y;
	double	scale = g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST);

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	Pixel16 *icon = m_tileSet->GetMapIconData(MAPICON_DAIS);

	if (icon) {
		DrawColorizedOverlayIntoMix(icon, x, y+24,
						g_colorSet->GetPlayerColor(theArmy.GetOwner()));
	}

	x += (k_ACTOR_CENTER_OFFSET_X * scale);
	y += (k_ACTOR_CENTER_OFFSET_Y * scale);

	if (unitActor2) {
		unitActor2->DrawDirect(NULL,
								x + k_UNIT_2_OFFSET_X,
								y + k_UNIT_2_OFFSET_Y,
								scale);
	}
	if (unitActor3) {
		unitActor3->DrawDirect(NULL,
								x + k_UNIT_3_OFFSET_X,
								y + k_UNIT_3_OFFSET_Y,
								scale);
	}
	if (unitActor1) {
		unitActor1->DrawDirect(NULL,
								x + k_UNIT_1_OFFSET_X,
								y + k_UNIT_1_OFFSET_Y,
								scale);
	}
#endif

}

#ifndef _PLAYTEST
BOOL g_show_ai_dbg = 0;
#endif

void TiledMap::PaintUnitActor(UnitActor *actor, bool fog)
{
	Assert(actor != NULL);
	if (actor == NULL) return;

	if (actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
	{

		if (actor->Draw(fog)) {

			RECT rect;

			actor->GetBoundingRect(&rect);
			AddDirtyRectToMix(rect);
		} else {





		}

		if (
		   (g_graphicsOptions && g_graphicsOptions->IsArmyTextOn())
		|| (g_theProfileDB->GetDebugAI()) //emod
		){
				Unit	u = actor->GetUnitID();

				if (u.IsValid() && u.GetArmy().m_id != 0) {

				Army		a = u.GetArmy();

				sint32		tx = actor->GetX() + GetZoomTilePixelWidth()/2;
				sint32 		ty = actor->GetY() + GetZoomTileHeadroom();

				uint8		col = a.GetData()->GetDebugStringColor();
				sint32		r,g,b;
				ColorMagnitudeToRGB(col, &r, &g, &b);

				COLORREF	fgColor = RGB(r, g, b);
				COLORREF    bgColor = RGB(0,0,0);

				DrawSomeText(true, a.GetData()->GetDebugString(), tx, ty, fgColor, bgColor);

			}
		}

		//EMOD to allow option for army names
		if (g_theProfileDB->GetShowArmyNames())
		{
			Unit	u = actor->GetUnitID();

			// Shouldn't this be the visible player?
			if(u.IsValid()
			&& u.GetArmy().m_id != 0
			&& g_player[u.GetOwner()]->IsHuman()
			){
				Army		a = u.GetArmy();

				const MBCHAR		*s = a.GetData()->GetName();

				sint32		tx = (sint32)(actor->GetX()+GetZoomTilePixelWidth()/2),
							ty = (sint32)(actor->GetY()+GetZoomTileHeadroom());

/*				sint32		r,g,b;
				uint8		col = a.GetData()->GetDebugStringColor();

				ColorMagnitudeToRGB(col, &r, &g, &b);

				COLORREF	fgColor = RGB(r, g, b),
							bgColor = RGB(0,0,0);*/

				DrawSomeText(true, s, tx, ty+40, g_colorSet->GetColorRef(COLOR_BLACK), g_colorSet->GetColorRef(COLOR_WHITE));
			}
		}
		//end emod

		if (g_show_ai_dbg || (g_isScenario && g_showUnitLabels) )
		{
			char text[80];
			text[0] = '\0';

			sint32	tx = (sint32)(actor->GetX()+(k_TILE_PIXEL_WIDTH*m_scale)/2),
					ty = (sint32)(actor->GetY()+(k_TILE_PIXEL_HEIGHT*m_scale));

			Cell *c = g_theWorld->GetCell(actor->GetMapPos());
			Unit city = c->GetCity();

			if ((city != Unit()) && g_show_ai_dbg)
			{
				strcpy(text, city.GetName());

				DrawSomeText(TRUE, text, tx, ty+10,
								g_colorSet->GetColorRef(COLOR_YELLOW),
								g_colorSet->GetColorRef(COLOR_PURPLE));
			}

			CellUnitList *al = c->UnitArmy();
			if (al) {
				strcpy(text, al->Access(0).GetName());

				DrawSomeText(TRUE, text, tx, ty,
					g_colorSet->GetColorRef(COLOR_BLACK),
					g_colorSet->GetColorRef(COLOR_WHITE));
			}

		}

	}
}

void TiledMap::PaintGoodActor(GoodActor *actor, bool fog)
{
	Assert(actor != NULL);
	if (actor == NULL) return;

	(void) actor->Draw(fog);

	RECT rect;
	actor->GetBoundingRect(&rect);
	AddDirtyRectToMix(rect);
}
















sint32 TiledMap::RepaintLayerSprites(const RECT & paintRect, sint32 layer)
{
	if(!ReadyToDraw())
		return 0;

#ifdef _DEBUG
	g_tiledMap->DrawRectMetrics();
	g_tiledMap->RectMetricNewLoop();
#endif

	for (sint32 i=paintRect.top; i<paintRect.bottom; i++) {
		for (sint32 j=paintRect.left; j<paintRect.right; j++) {

			if(!g_theWorld->IsXwrap() && (j < 0 || j >= g_theWorld->GetXWidth()))
				continue;

			sint32 tileX,tileY;
			maputils_WrapPoint(j,i,&tileX,&tileY);

			sint32      mapY    = tileY;
			sint32      mapX    = maputils_TileX2MapX(tileX,tileY);
			MapPoint    pos     = MapPoint(mapX, mapY);

#if 0
			if(g_theWorld->IsCity(pos))
			{
				Unit city=g_theWorld->GetCity(pos);

				sint32 pop;

				city.GetPop(pop);
				DrawCityRadius(pos, COLOR_WHITE ,pop);
			}
#endif

			if (g_theWorld->IsGood(pos) && m_localVision->IsExplored(pos))
			{
				TileInfo *curTileInfo = GetTileInfo(pos);
				Assert(curTileInfo);
				if (curTileInfo)
				{
                    GoodActor * curGoodActor = curTileInfo->GetGoodActor();
                    if (curGoodActor)
                    {
					    curGoodActor->PositionActor(pos);
					    PaintGoodActor
                            (curGoodActor, !m_localVision->IsVisible(pos));
                    }
				}
			}

			Unit		top;

			if (m_localVision && !m_localVision->IsExplored(pos)
				&& !g_theWorld->GetTopVisibleUnit(pos, top))
				continue;

			bool fog =
                (m_localVision && m_localVision->IsExplored(pos) &&
                 !m_localVision->IsVisible(pos)
                );

			UnseenCellCarton		ucell;

			// For visibility god mode and fog of war should be handled equally
			if(!g_fog_toggle
			&&  m_localVision
			&&  m_localVision->GetLastSeen(pos, ucell))
			{

				UnitActor	*actor = ucell.m_unseenCell->GetActor();

				if (actor)
				{
					PaintUnitActor(actor, fog);
				}
			}
			else
			{


				if (!g_theWorld->GetTopVisibleUnit(pos, top)) continue;

				if (g_drawArmyClumps)
                {
                    Army    a   = top.GetArmy();
					if (a.IsValid() && (a.Num() > 1))
                    {
						PaintArmyActors(pos);
						return 0;
					}
				}

				UnitActor *actor = top.GetActor();


				if (g_player[top.GetOwner()] && !Player::IsThisPlayerARobot(top.GetOwner())) {
                    actor->SetIsFortifying(top.IsEntrenching());
                }
				actor->SetIsFortified(top.IsEntrenched());

				actor->SetHiddenUnderStack(false);
				actor->SetUnitVisibility(top.GetVisibility());

				if (top.IsCity())
				{
					CityData *cityData = top.GetData()->GetCityData();

					actor->SetHasCityWalls(cityData->HasCityWalls());
					actor->SetHasForceField(cityData->HasForceField());
				}





				// For visibility god mode and fog of war should be handled equally
				if(!( actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
				&& !g_fog_toggle
				&& !g_god
				&& (g_player[g_selected_item->GetVisiblePlayer()]
				&& !g_player[g_selected_item->GetVisiblePlayer()]->m_hasGlobalRadar))
					continue;

				if (top.GetOwner() == g_selected_item->GetVisiblePlayer()
					&& top.CanSettle(top.RetPos()))
				{
					SELECT_TYPE		selectType;
					ID				selectedID;
					PLAYER_INDEX	selectedPlayer;

					g_selected_item->GetTopCurItem(selectedPlayer, selectedID, selectType);

					Unit		selectedUnit;
					COLOR		color = COLOR_BLACK;

					if(selectType == SELECT_TYPE_LOCAL_CITY)
					{
						selectedUnit = selectedID;
						color = COLOR_WHITE;
					}
					else
						if(selectType == SELECT_TYPE_LOCAL_ARMY)
						{
							color = COLOR_GREEN;
							selectedUnit = ((Army)selectedID).GetTopVisibleUnit(selectedPlayer);
						}

					if (selectedUnit.m_id == top.m_id)
						DrawCityRadius(top.RetPos(), color);
				}

			   	if (actor->IsActive())
			   	{
					Unit	second;
					if (g_theWorld->GetSecondUnit(pos, second))
					{
						top = second;
						actor = top.GetActor();
					}
			   	}

				const MapPoint & actorCurPos = actor->GetMapPos();
				if (actor->IsActive())
                {
				    // No action: already busy
                }
                else
                {
					PaintUnitActor
                        (actor, !m_localVision->IsVisible(actorCurPos));
				}

				if (top.IsCity()) {

					Unit hypotheticalUnit;

					if (!g_theWorld->GetTopVisibleUnitNotCity(pos, hypotheticalUnit)) {


						PLAYER_INDEX	s_player;
						ID				s_item;
                        SELECT_TYPE		s_state;

						g_selected_item->GetTopCurItem(s_player, s_item, s_state);

						if (s_player != top.GetOwner())
							continue;

						if (s_state != SELECT_TYPE_LOCAL_ARMY &&
							s_state != SELECT_TYPE_LOCAL_ARMY_UNLOADING)
							continue;

						Army		army(s_item);
						if (!army.IsValid())
							continue;

						MapPoint	armyPos;
						army.GetPos(armyPos);

						if (armyPos != top.RetPos())
							continue;

						top = army.Access(0);
					} else {
						top = hypotheticalUnit;
					}

					// For visibility god mode and fog of war should be handled equally
					if (top.GetOwner() != g_selected_item->GetVisiblePlayer()
					&& !g_fog_toggle
					&& !g_god)
						continue;

					actor = top.GetActor();
					if(!actor) continue;




					if (!(actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer())))
						continue;

					if (!actor->IsActive() && TileIsVisible(actorCurPos.x, actorCurPos.y))
                    {
						actor->SetHiddenUnderStack(false);
						PaintUnitActor
                            (actor, !m_localVision->IsVisible(actorCurPos));
					}
				}
			}
		}
	}
	return 0;
}

void
TiledMap::ProcessUnit(Unit unit)
{
    if (unit.IsValid())
    {
        UnitActor * actor = unit.GetActor();
        if (actor && !actor->IsActive())
        {
            actor->Process();
        }
    }
}

void
TiledMap::ProcessUnit(CellUnitList * a_List)
{
	if (a_List)
    {
    	for (sint32 index = 0; index < a_List->Num(); index++)
	    	ProcessUnit(a_List->Get(index));
    }
}

void TiledMap::ProcessLayerSprites(const RECT & paintRect, sint32 layer)
{
	if(!ReadyToDraw())
		return;

	UnseenCellCarton	ucell;
	sint32				tileX,tileY;
	sint32				i,j;
	sint32				mapX, mapY;
	MapPoint			pos;
	TileInfo			*curTileInfo;
	GoodActor			*curGoodActor;
	UnitActor			*curUnitActor;
	Unit				unit;
	Cell				*CurrentCell=NULL;

	for (i=paintRect.top; i<paintRect.bottom; i++)
	{
		for (j=paintRect.left; j<paintRect.right; j++)
		{
			maputils_WrapPoint(j,i,&tileX,&tileY);

			mapY = tileY;
			mapX = maputils_TileX2MapX(tileX,tileY);

			pos.x = (sint16)mapX;
			pos.y = (sint16)mapY;

			curTileInfo = GetTileInfo(pos);

			Assert(curTileInfo != NULL);

			CurrentCell = g_theWorld->GetCell(pos);

			if(g_theWorld->IsGood(pos) && m_localVision->IsExplored(pos))
			{
				if(curTileInfo && curTileInfo->HasGoodActor())
				{
					curGoodActor = curTileInfo->GetGoodActor();


					if (g_theWorld->GetCity(pos).m_id == 0)
						curGoodActor->Process();
				}
			}




// Added by Martin Gühmann
			// We want to something when we lift the fog of war
			if(!g_fog_toggle
			&&  m_localVision
			&&  m_localVision->GetLastSeen(pos, ucell))
			{

				curUnitActor = ucell.m_unseenCell->GetActor();

				if (curUnitActor)
					curUnitActor->Process();
			}
			else
			{
				ProcessUnit(CurrentCell->GetCity());
				ProcessUnit(CurrentCell->UnitArmy());
#if 0

				unit = g_theWorld->GetCell(pos)->GetCity();

				if (unit.IsValid())
				{
					curUnitActor = unit.GetActor();

					if (curUnitActor)
						curUnitActor->Process();
				}

				CellUnitList * unitList = g_theWorld->GetCell(pos)->UnitArmy();

				if (unitList)
				{
					for (index=0; index < unitList->Num(); index++)
					{
						unit= unitList->Get(index);

						if (unit.IsValid())
						{
							curUnitActor = unit.GetActor();

							if (curUnitActor)
							{
								if (!curUnitActor->IsActive())
									curUnitActor->Process();
							}
						}
					}
				}
#endif
			}
		}
	}
}

sint32 TiledMap::OffsetLayerSprites(const RECT & paintRect, sint32 deltaX, sint32 deltaY, sint32 layer)
{
	if(!ReadyToDraw())
		return 0;

	for (sint32 i=paintRect.top; i<paintRect.bottom; i++) {
		for (sint32 j=paintRect.left; j<paintRect.right; j++) {

			sint32 tileX,tileY;
			maputils_WrapPoint(j,i,&tileX,&tileY);

			sint32 mapX, mapY = tileY;
			mapX = maputils_TileX2MapX(tileX,tileY);

			MapPoint pos;

			pos.x = (sint16)mapX; pos.y = (sint16)mapY;

			sint32 pixelX, pixelY;

			maputils_MapXY2PixelXY(mapX, mapY, &pixelX, &pixelY);

			TileInfo *curTileInfo = GetTileInfo(pos);
			Assert(curTileInfo != NULL);

			if(g_theWorld->IsGood(pos) && m_localVision->IsExplored(pos)) {
				GoodActor *curGoodActor;
				if(curTileInfo && curTileInfo->HasGoodActor()) {
					curGoodActor = curTileInfo->GetGoodActor();

					curGoodActor->SetX(pixelX);
					curGoodActor->SetY(pixelY);
				}
			}

			if (m_localVision && !m_localVision->IsExplored(pos))
				continue;

			UnseenCellCarton		ucell;
			if (m_localVision && m_localVision->GetLastSeen(pos, ucell)) {

				UnitActor	*actor = ucell.m_unseenCell->GetActor();

				if (actor) {
					actor->SetX(pixelX);
					actor->SetY(pixelY);
				}
			} else {
				Unit		top;

				if (!g_theWorld->GetTopVisibleUnit(pos, top)) continue;

				UnitActor *actor = top.GetActor();

				if (actor->IsActive()) {
					Unit	second;
					if (g_theWorld->GetSecondUnit(pos, second)) {
						top = second;
						actor = top.GetActor();
					}
				}

				if (actor && !actor->IsActive()) {
					actor->SetX(pixelX);
					actor->SetY(pixelY);
				}

				if (top.IsCity()) {

					if (!g_theWorld->GetTopVisibleUnitNotCity(pos, top)) continue;

					actor = top.GetActor();

					if (actor && !actor->IsActive()) {
						actor->SetX(pixelX);
						actor->SetY(pixelY);
					}
				}
			}
		}
	}
	return 0;
}

sint32 TiledMap::OffsetSprites(const RECT & paintRect, sint32 deltaX, sint32 deltaY)
{
	OffsetLayerSprites(paintRect, deltaX, deltaY, 0);

	g_director->OffsetActors(-deltaX, -deltaY);

	return 0;
}

sint32 TiledMap::RepaintSprites(aui_Surface *surf, const RECT & paintRect, bool scrolling)
{
	if(!ReadyToDraw())
		return 0;

	sint32	mapWidth, mapHeight;
	GetMapMetrics(&mapWidth, &mapHeight);

	Assert(m_localVision);

	if (m_nextPlayer)
	{
		m_nextPlayer = FALSE;
	}

	g_screenManager->LockSurface(surf);

	RepaintLayerSprites(paintRect, 0);
	g_director->Draw(paintRect, 0);

	if (g_spriteEditWindow)
		g_spriteEditWindow->DrawSprite();

	g_tiledMap->DrawTerrainOverlay(surf);

	if (!scrolling)
	{
		DrawChatText();
	}

	g_screenManager->UnlockSurface();

	if (g_theProfileDB->GetShowCityNameAlpha() != pixelutils_TRANSPARENT)
	{
		g_tiledMap->DrawCityNames(surf, 0);
	}

	if (ScenarioEditor::ShowStartFlags())
	{
		g_tiledMap->DrawStartingLocations(surf, 0);
	}

	return 0;
}

void TiledMap::DrawStartingLocations(aui_Surface *surf, sint32 layer)
{
	if (ScenarioEditor::GetStartLocMode() == SCEN_START_LOC_MODE_NONE)
		return;

	Pixel16		*icon = GetTileSet()->GetMapIconData(MAPICON_FLAG);
	POINT		iconDim = GetTileSet()->GetMapIconDimensions(MAPICON_FLAG);

	MBCHAR		labelString[MAX_PATH];

	SCEN_START_LOC_MODE mode = ScenarioEditor::GetStartLocMode();

	for (sint32 i=0; i<g_theWorld->GetNumStartingPositions(); i++) {
		MapPoint		pos;
		sint32			playerOrCiv;

		pos = g_theWorld->GetStartingPoint(i);


		if (mode == SCEN_START_LOC_MODE_CIV) {
			playerOrCiv = g_theWorld->GetStartingPointCiv(i);
		} else {

			playerOrCiv = i+1;
		}

		if (TileIsVisible(pos.x, pos.y)) {

			sint32		x, y;

			maputils_MapXY2PixelXY(pos.x,pos.y,&x,&y);

			Pixel16		pixelColor;
			COLORREF	colorRef;

			if (mode == SCEN_START_LOC_MODE_PLAYER ||
				mode == SCEN_START_LOC_MODE_PLAYER_WITH_CIV) {

				pixelColor = g_colorSet->GetPlayerColor(playerOrCiv);
				colorRef = g_colorSet->GetColorRef(g_colorSet->ComputePlayerColor(playerOrCiv));
			} else
			if (mode == SCEN_START_LOC_MODE_CIV) {

				pixelColor = g_colorSet->GetColor(COLOR_WHITE);
				colorRef = g_colorSet->GetColorRef(COLOR_WHITE);
			} else {
				Assert(FALSE);
				return;
			}

			sint32 destX, destY;

			destX = x + (GetZoomTilePixelWidth()/2) - (iconDim.x / 2);
			destY = y + (GetZoomTileGridHeight()/2) - (iconDim.y / 2);

			DrawColorizedOverlay(icon, surf, destX, destY, pixelColor);

			AddDirtyToMix(destX, destY, iconDim.x, iconDim.y);

			ScenarioEditor::GetLabel(labelString, playerOrCiv);

			if (m_font) {
				RECT		rect, clipRect, boxRect;

				sint32 width = m_font->GetStringWidth(labelString);
				sint32 height = m_font->GetMaxHeight();

				rect.left = x;
				rect.top = y;
				rect.right = x+width;
				rect.bottom = y+height;

				boxRect = rect;

				InflateRect(&boxRect, 2, 1);

				clipRect = boxRect;

				if (clipRect.left < 0) clipRect.left = 0;
				if (clipRect.top < 0) clipRect.top = 0;
				if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
				if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

				primitives_PaintRect16(surf, &clipRect, g_colorSet->GetColor(COLOR_BLACK));

				InflateRect(&boxRect, 1, 1);

				clipRect = boxRect;

				if (clipRect.left < 0) clipRect.left = 0;
				if (clipRect.top < 0) clipRect.top = 0;
				if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
				if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

				primitives_FrameRect16(surf, &clipRect, g_colorSet->GetColor(COLOR_GREEN));

				clipRect = rect;

				if (clipRect.left < 0) clipRect.left = 0;
				if (clipRect.top < 0) clipRect.top = 0;
				if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
				if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

				m_font->DrawString(surf, &rect, &clipRect, labelString, 0,	colorRef,	0);

				AddDirtyRectToMix(clipRect);
			}
		}
	}
}

sint32 TiledMap::DrawCityRadius(const MapPoint &cpos, COLOR color, sint32 pop)
{
	if (g_theWorld->GetCell(cpos)->HasCity())
	{
		return 0; // Following code not used
#if 0
	    Pixel16 pixelColor = g_colorSet->GetColor(color);
		CityInfluenceIterator it(cpos, g_theWorld->GetCity(cpos).CD()->GetSizeIndex());

		for(it.Start(); !it.End(); it.Next()) {
			MapPoint neighbor;
			Cell *cell = g_theWorld->GetCell(it.Pos());

			if(it.Pos().GetNeighborPosition(NORTHWEST, neighbor)) {
				if(g_theWorld->GetCell(neighbor)->GetCityOwner().m_id !=
				   cell->GetCityOwner().m_id) {
					DrawColoredHitMaskEdge(g_screenManager->GetSurface(), it.Pos(), pixelColor, NORTHWEST);
				}
			}

			if(it.Pos().GetNeighborPosition(SOUTHWEST, neighbor)) {
				if(g_theWorld->GetCell(neighbor)->GetCityOwner().m_id !=
				   cell->GetCityOwner().m_id) {
					DrawColoredHitMaskEdge(g_screenManager->GetSurface(), it.Pos(), pixelColor, SOUTHWEST);
				}
			}

			if(it.Pos().GetNeighborPosition(NORTHEAST, neighbor)) {
				if(g_theWorld->GetCell(neighbor)->GetCityOwner().m_id !=
				   cell->GetCityOwner().m_id) {
					DrawColoredHitMaskEdge(g_screenManager->GetSurface(), it.Pos(), pixelColor, NORTHEAST);
				}
			}

			if(it.Pos().GetNeighborPosition(SOUTHEAST, neighbor)) {
				if(g_theWorld->GetCell(neighbor)->GetCityOwner().m_id !=
				   cell->GetCityOwner().m_id) {
					DrawColoredHitMaskEdge(g_screenManager->GetSurface(), it.Pos(), pixelColor, SOUTHEAST);
				}
			}

		}
#endif
	}
	else
	{
		DrawCityRadius1(cpos, color);
	}

	return 0;
}

//----------------------------------------------------------------------------
//
// Name       : TiledMap::DrawCityRadius1
//
// Description: Draw a "colored hit mask" in a radius of 1 around a city.
//
// Parameters : cpos    : city location on the map
//              color   : color to use when drawing
//
// Globals    : g_screenManager
//
// Returns    : sint32  : useless value, always 0
//
// Remark(s)  : The tile NORTH of the city is not drawn.
//              The tile of the city itself is drawn.
//              TODO: check whether this is intentional, or the original code
//                    was just wrong.
//
//----------------------------------------------------------------------------
sint32 TiledMap::DrawCityRadius1(const MapPoint &cpos, COLOR color)
{
	for (int dir = NORTH; dir <= NOWHERE; ++dir)
	{
		OrthogonalPoint	neighbour(cpos);
		neighbour.Move(WORLD_DIRECTION(dir));
		if (neighbour.IsValid())
		{
			DrawColoredHitMask
				(g_screenManager->GetSurface(), neighbour.GetRC(), color);
		}
	}

	return 0;
}


sint32 TiledMap::PaintColoredTile(sint32 x, sint32 y, COLOR color)
{
	uint8			*surfBase;
	sint32			surfWidth;
	sint32			surfHeight;
	sint32			surfPitch;
	aui_Surface		*surface;

	surface = g_screenManager->GetSurface();

	surfBase = g_screenManager->GetSurfBase();
	surfWidth = g_screenManager->GetSurfWidth();
	surfHeight = g_screenManager->GetSurfHeight();
	surfPitch = g_screenManager->GetSurfPitch();

	unsigned short	*destPixel;

	y+=k_TILE_PIXEL_HEADROOM;

if (x < 0) return 0;
if (x >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
if (y < 0) return 0;
if (y >= surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	sint32 startX, endX;

	uint32  blendRgbMask = pixelutils_GetBlend16RGBMask();
	Pixel16 pixelColor   = g_colorSet->GetColor(color);

	for(sint32 j=0; j<k_TILE_PIXEL_HEIGHT; j++) {
		if (j<=23) {
			startX = (23-j)*2;
		} else {
			startX = (j-24)*2;
		}
		endX = k_TILE_PIXEL_WIDTH - startX;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + ((x+startX) * 2));

		for (sint32 i=startX; i<endX; i++) {
			if (*destPixel == 352)  {
				*destPixel = 352;
			}
			*destPixel = pixelutils_Blend16(pixelColor, *destPixel, 160, blendRgbMask);
			destPixel++;
		}
	}

	AddDirtyToMix(x, y, k_TILE_PIXEL_WIDTH, k_TILE_PIXEL_HEIGHT);

	return 0;
}


sint32 TiledMap::Refresh(void)
{
	LPVOID      buffer;
	AUI_ERRCODE errcode = m_surface->Lock(NULL, &buffer, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode == AUI_ERRCODE_OK )
	{
		memset( buffer, 0x00, m_surface->Size() );
		m_surface->Unlock( buffer );
	} else {
		return AUI_ERRCODE_SURFACELOCKFAILED;
	}

	LockSurface();

	if (SmoothScrollAligned())
	{
		RepaintTiles(m_mapViewRect);
		RepaintHats (m_mapViewRect);
		RepaintBorders(m_mapViewRect);
	}
	else
	{
		RECT   altrect=m_mapViewRect;

		altrect.left   -= 1;
		altrect.top    -= 1;
		altrect.right  += 1;
		altrect.bottom -= 1;
		RepaintTilesClipped(altrect);
		RepaintHats(altrect, true);
		RepaintBorders(altrect, true);
	}

	RepaintImprovements(m_mapViewRect);

	UnlockSurface();

	return 0;
}

void TiledMap::ScrollPixels(sint32 deltaX, sint32 deltaY, aui_Surface *surf)
{
	char *      buffer;
	AUI_ERRCODE errcode = surf->Lock(NULL, (LPVOID *)&buffer, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK)
		return;

	sint32	h = surf->Height(),
			w = surf->Width(),
			copyWidth = (w - abs(deltaX))>>1,
			copyHeight = h - abs(deltaY);

	sint32		pitch = surf->Pitch();

	uint32		*srcPtr, *destPtr;
	sint32		dx = abs(deltaX),
				dy = abs(deltaY);
	sint32		i,j;

	sint32		slop;

	if (deltaX)
	{

		if (deltaX<0)
		{

			srcPtr =	(uint32 *)(buffer + (w - dx) * 2 - 4);
			destPtr =	(uint32 *)(buffer + w * 2 - 4);

			Assert((unsigned)srcPtr >=(unsigned)buffer);
			Assert((unsigned)destPtr>=(unsigned)buffer);

			slop = (pitch>>2) + copyWidth;

			for (i=0; i<h; i++)
			{
				for (j=0; j<copyWidth; j++)
					*destPtr-- = *srcPtr--;

				for (j=0; j<(dx>>1); j++)
					*destPtr-- = 0x00000000;

				srcPtr += slop;
				destPtr += (slop + (dx>>1));
			}
		}
		else
		{
			srcPtr =	(uint32 *)(buffer + dx * 2);
			destPtr =	(uint32 *)buffer;
			slop = (pitch / 4) - copyWidth;
			for (i=0; i<h; i++)
			{
				for (j=0; j<copyWidth; j++)
					*destPtr++ = *srcPtr++;
				for (j=0; j<dx/2; j++)
					*destPtr++ = 0x00000000;

				srcPtr += slop;
				destPtr += (slop - dx/2);
			}
		}

	}
	else
	{
		if (deltaY)
		{
			if (deltaY < 0)
			{
				srcPtr =	(uint32 *)(buffer + (pitch * (h - dy- 1)));
				destPtr =	(uint32 *)(buffer + (pitch * (h - 1)));
				slop = (pitch / 4) + (w >> 1);
				for (i=0; i<copyHeight; i++)
				{
					for (j=0; j<w>>1; j++)
						*destPtr++ = *srcPtr++;
					srcPtr -= slop;
					destPtr -= slop;
				}
				for (i=0; i<dy; i++)
				{
					for (j=0; j<w>>1; j++)
						*destPtr++ = 0x00000000;
					destPtr -= slop;
				}
			}
			else
			{
				srcPtr =	(uint32 *)(buffer + (pitch * dy));
				destPtr =	(uint32 *)(buffer);
				slop = (pitch / 4) - (w >> 1);
				for (i=0; i<copyHeight; i++)
				{
					for (j=0; j<w>>1; j++)
						*destPtr++ = *srcPtr++;

					srcPtr += slop;
					destPtr += slop;
				}
				for (i=0; i<dy; i++)
				{
					for (j=0; j<w>>1; j++)
						*destPtr++ = 0x00000000;

					destPtr += slop;
				}
			}
		}
	}

	errcode = surf->Unlock(buffer);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return;
}

RECT TiledMap::EnsureRectOverlapMap(const RECT & rect) const
{
	sint32 mapWidth  = GetMapWidth();
	sint32 mapHeight = GetMapHeight();

	RECT result = rect;
	if (result.right <= 0)
	{
		result.left += mapWidth;
		result.right += mapWidth;
		Assert(result.right > 0);
	}
	if (result.left >= mapWidth)
	{
		result.left -= mapWidth;
		result.right -= mapWidth;
		Assert(result.left < mapWidth);
	}
	if (result.bottom <= 0)
	{
		result.top += mapHeight;
		result.bottom += mapHeight;
		Assert(result.bottom > 0);
	}
	if (result.top >= mapHeight)
	{
		result.top -= mapHeight;
		result.bottom -= mapHeight;
		Assert(result.top < mapHeight);
	}
	return result;
}

bool TiledMap::ScrollMap(sint32 deltaX, sint32 deltaY)
{
	if (g_modalWindow) {
		return false;
	}

	RetargetTileSurface(g_background->TheSurface());

	if (!g_theWorld->IsXwrap())
	{
		if ((deltaX < 0) && (m_mapViewRect.left + deltaX < GetMinMapLeft()))
		{
			deltaX = 0;
		}
		if ((deltaX > 0) && (m_mapViewRect.right + deltaX > GetMaxMapRight()))
		{
			deltaX = 0;
		}
	}

	if (!g_theWorld->IsYwrap())
	{
		if ((deltaY < 0) && (m_mapViewRect.top + deltaY < GetMinMapTop()))
		{
			deltaY = 0;
		}

		if ((deltaY > 0) && (m_mapViewRect.bottom + deltaY > GetMaxMapBottom()))
		{
			deltaY = 0;
		}
	}

	RECT mapViewRect = m_mapViewRect;
	OffsetRect(&mapViewRect, deltaX, deltaY);
	UpdateAndClipMapViewRect(EnsureRectOverlapMap(mapViewRect));

	OffsetMixDirtyRects(deltaX, deltaY);

	sint32 hscroll = GetZoomTilePixelWidth();
	sint32 vscroll = GetZoomTilePixelHeight() / 2;
	OffsetSprites(mapViewRect, deltaX * hscroll, deltaY * vscroll);
	ScrollPixels(deltaX * hscroll, deltaY * vscroll, m_surface);

	LockSurface();

	RECT repaintRect = m_mapViewRect;
	if (deltaY == 0)
	{
		if (deltaX > 0) {
			if (m_mapViewRect.right > m_mapBounds.right && !g_theWorld->IsXwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.left = m_mapViewRect.right - deltaX;
			}
		} else {
			if (m_mapViewRect.left < m_mapBounds.left && !g_theWorld->IsXwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.right = m_mapViewRect.left - deltaX;
			}
		}
	} else if (deltaX == 0) {
		if (deltaY > 0) {
			if (m_mapViewRect.bottom > m_mapBounds.bottom && !g_theWorld->IsYwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.top = m_mapViewRect.bottom - deltaY;
			}
		} else {
			if (m_mapViewRect.top < m_mapBounds.top && !g_theWorld->IsYwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.bottom = m_mapViewRect.top - deltaY;
			}
		}
	}
	repaintRect = EnsureRectOverlapMap(repaintRect);

	RepaintTiles(repaintRect);

	RECT inflatedRepaintRect = repaintRect;

	if (deltaX == 1) {
		inflatedRepaintRect.left -= 1;
		inflatedRepaintRect.top += 1;
		inflatedRepaintRect.bottom += 1;
	}
	else if (deltaX == -1) {
		inflatedRepaintRect.right += 1;
		inflatedRepaintRect.top += 1;
		inflatedRepaintRect.bottom += 1;
	}
	if (deltaY == 1) {
		inflatedRepaintRect.right += 1;
		inflatedRepaintRect.top -= 2;
	}
	else if (deltaY == -1) {
		inflatedRepaintRect.right += 1;
		inflatedRepaintRect.bottom += 2;
	}

	RepaintHats(inflatedRepaintRect);
	RepaintBorders(inflatedRepaintRect);
	RepaintImprovements(inflatedRepaintRect);

	UnlockSurface();

	m_mapDirtyList->Flush();

	InvalidateMix();

	g_theTradePool->Draw(m_surface, inflatedRepaintRect);
	RepaintSprites(m_surface, inflatedRepaintRect, true);

	return true;
}

bool TiledMap::SmoothScrollAligned() const
{
	return ((!m_smoothOffsetX) && (!m_smoothOffsetY));
}

bool TiledMap::ScrollMapSmooth(sint32 pdeltaX, sint32 pdeltaY)
{
	RetargetTileSurface(g_background->TheSurface());

	sint32 signX  = (!pdeltaX ? 0 : (pdeltaX < 0 ? -1 : 1));
	sint32 signY  = (!pdeltaY ? 0 : (pdeltaY < 0 ? -1 : 1));

	if (!g_theWorld->IsXwrap())
	{
		if ((signX < 0) && (m_mapViewRect.left + signX < GetMinMapLeft()))
		{
			signX = 0;
		}
		if ((signX > 0) && (m_mapViewRect.right + signX > GetMaxMapRight()))
		{
			signX = 0;
		}
	}

	if (!g_theWorld->IsYwrap())
	{
		if ((signY < 0) && (m_mapViewRect.top + signY < GetMinMapTop()))
		{
			signY = 0;
		}

		if ((signY > 0) && (m_mapViewRect.bottom + signY > GetMaxMapBottom()))
		{
			signY = 0;
		}
	}

	sint32	hscroll		= GetZoomTilePixelWidth();
	sint32	vscroll		= GetZoomTilePixelHeight() >> 1;
	sint32  deltaX      = m_smoothOffsetX / hscroll;
	sint32  deltaY      = m_smoothOffsetY / vscroll;

	if (deltaX != 0) {
		m_smoothOffsetX -= deltaX * hscroll;
	}
	if (deltaY != 0) {
		m_smoothOffsetY -= deltaY * vscroll;
	}

	m_smoothLastX =	pdeltaX;
	m_smoothLastY =	pdeltaY;

	RECT mapViewRect = m_mapViewRect;
	OffsetRect(&mapViewRect, deltaX, deltaY);
	UpdateAndClipMapViewRect(EnsureRectOverlapMap(m_mapViewRect));

	OffsetMixDirtyRects(deltaX, deltaY);

	OffsetSprites(mapViewRect, pdeltaX, pdeltaY);
	ScrollPixels(pdeltaX, pdeltaY, m_surface);

	LockSurface();

	RECT repaintRect = m_mapViewRect;
	if (deltaY == 0)
	{
		if (deltaX > 0) {
			if (m_mapViewRect.right > m_mapBounds.right && !g_theWorld->IsXwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.left = m_mapViewRect.right - deltaX;
			}
		} else {
			if (m_mapViewRect.left < m_mapBounds.left && !g_theWorld->IsXwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.right = m_mapViewRect.left - deltaX;
			}
		}
	} else if (deltaX == 0) {
		if (deltaY > 0) {
			if (m_mapViewRect.bottom > m_mapBounds.bottom && !g_theWorld->IsYwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.top = m_mapViewRect.bottom - deltaY;
			}
		} else {
			if (m_mapViewRect.top < m_mapBounds.top && !g_theWorld->IsYwrap()) {
				repaintRect = RECT_INVISIBLE;
			} else {
				repaintRect.bottom = m_mapViewRect.top - deltaY;
			}
		}
	}
	repaintRect = EnsureRectOverlapMap(repaintRect);

	RepaintTiles(repaintRect);

	RECT inflatedRepaintRect = repaintRect;

	if (deltaX == 1) {
		inflatedRepaintRect.left -= 1;
		inflatedRepaintRect.top += 1;
		inflatedRepaintRect.bottom += 1;
	}
	else if (deltaX == -1) {
		inflatedRepaintRect.right += 1;
		inflatedRepaintRect.top += 1;
		inflatedRepaintRect.bottom += 1;
	}
	if (deltaY == 1) {
		inflatedRepaintRect.right += 1;
		inflatedRepaintRect.top -= 2;
	}
	else if (deltaY == -1) {
		inflatedRepaintRect.right += 1;
		inflatedRepaintRect.bottom += 2;
	}

	RepaintHats(inflatedRepaintRect);
	RepaintBorders(inflatedRepaintRect);
	RepaintImprovements(inflatedRepaintRect);

	UnlockSurface();

	m_mapDirtyList->Flush();

	InvalidateMix();

	g_theTradePool->Draw(m_surface, inflatedRepaintRect);
	RepaintSprites(m_surface, inflatedRepaintRect, true);

	return true;
}

sint32 TiledMap::RedrawHat(
			aui_Surface *surface,
			sint32 i,
			sint32 j,
			bool clip
			)
{
	sint32		drawx = j;

	maputils_WrapPoint(j,i,&j,&i);

	sint32      drawy = i;

	maputils_TileX2MapXAbs(drawx,drawy,&drawx);

	sint32 k = maputils_TileX2MapX(j,i);

	if (!TileIsVisible(k,i))
		return 0;

	MapPoint tempPos (k, i);


	if (!ReadyToDraw())
		return 0;

	if (!m_localVision->IsExplored(tempPos))
    {
		return 0;
	}

	TileInfo		*tileInfo;

	sint32		terrainType;
	bool		fog = !m_localVision->IsVisible(tempPos);
	if (fog)
    {
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(tempPos, ucell))
		{
			terrainType = ucell.m_unseenCell->GetTerrainType();
			tileInfo = ucell.m_unseenCell->GetTileInfo();
		}
		else
		{
			terrainType = g_theWorld->GetTerrain(tempPos.x,tempPos.y);
			tileInfo = GetTileInfo(tempPos);
		}
	}
	else
	{
		terrainType = g_theWorld->GetTerrain(tempPos.x, tempPos.y);
		tileInfo = GetTileInfo(tempPos);
	}

	maputils_MapXY2PixelXY(drawx,drawy,&drawx,&drawy);

	if (tileInfo == NULL)
		return -1;

	BaseTile *baseTile = m_tileSet->GetBaseTile(tileInfo->GetTileNum());

	if (baseTile == NULL)
		return -1;

	if (m_zoomLevel == k_ZOOM_LARGEST)
	{

		if (!fog)
			DrawOverlayClipped(surface, baseTile->GetHatData(), drawx, drawy);
		else
		{
			if (g_isFastCpu)
				DrawBlendedOverlay(surface, baseTile->GetHatData() ,drawx,drawy,k_FOW_COLOR,k_FOW_BLEND_VALUE);
			else
				DrawDitheredOverlay(surface, baseTile->GetHatData(),drawx,drawy,k_FOW_COLOR);
		}

		if (m_surface == m_mapSurface)
			AddDirtyToMap(drawx,drawy, k_TILE_PIXEL_WIDTH, k_TILE_PIXEL_HEIGHT);
	}
	else
	{


		if (!fog)
		{

			DrawScaledOverlay(surface, baseTile->GetHatData(),drawx,drawy,
									GetZoomTilePixelWidth(),
									GetZoomTileGridHeight());
		}
		else
		{
			if (g_isFastCpu)
			{
				DrawBlendedOverlayScaled(surface, baseTile->GetHatData(),drawx,drawy,
									GetZoomTilePixelWidth(),
									GetZoomTileGridHeight(),
									k_FOW_COLOR,
									k_FOW_BLEND_VALUE);
			}
			else
			{
				DrawDitheredOverlayScaled(surface, baseTile->GetHatData(),drawx,drawy,
									GetZoomTilePixelWidth(),
									GetZoomTileGridHeight(),
									k_FOW_COLOR);
			}
		}

		if (m_surface == m_mapSurface)
			AddDirtyToMap(drawx,drawy, GetZoomTilePixelWidth(), GetZoomTileGridHeight());

	}




	static MapPoint nw, ne;
	if(tempPos.GetNeighborPosition(NORTHWEST, nw)) {
		if (m_localVision->IsExplored(nw))
        {

			DrawNationalBorders(surface ? *surface : *m_surface, nw);
		}
	}

	if(tempPos.GetNeighborPosition(NORTHEAST, ne)) {
		if (m_localVision->IsExplored(ne))
        {
			DrawNationalBorders(surface ? *surface : *m_surface, ne);
		}
	}

	return 0;
}







sint32 TiledMap::RedrawBorders(
			aui_Surface *surface,
			sint32 i,
			sint32 j,
			bool clip
			)
{
	maputils_WrapPoint(j,i,&j,&i);

	sint32 k = maputils_TileX2MapX(j,i);

	if (!TileIsVisible(k,i))
		return 0;

	MapPoint tempPos (k, i);

	if(!ReadyToDraw())
		return 0;

	if(!m_localVision->IsExplored(tempPos))
    {
		return 0;
	}
	//DrawBorderIcon(surface, tempPos);  //emod?
	DrawNationalBorders(surface ? *surface : *m_surface, tempPos);

	return 0;
}






void TiledMap::RedrawTile
(
	const MapPoint *    point
)
{
	if (TileIsVisible(point->x, point->y))
    {
		MapPoint    pos;
	    sint32      tileX;

		LockSurface();
		if(point->GetNeighborPosition(NORTH,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			CalculateWrap(NULL,pos.y,tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		if(point->GetNeighborPosition(NORTHWEST,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			CalculateWrap(NULL,pos.y,tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		if(point->GetNeighborPosition(NORTHEAST,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			CalculateWrap(NULL,point->y,tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		if(point->GetNeighborPosition(EAST,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		BlackTile(m_surface, (MapPoint *)point);

		maputils_MapX2TileX(point->x,point->y,&tileX);

		CalculateWrap(NULL,point->y,tileX);

		RedrawHat(NULL, point->y,tileX);
		RedrawBorders(NULL, point->y, tileX);

		DrawImprovements(NULL, point->y, tileX, 0);

		if(point->GetNeighborPosition(WEST,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		if(point->GetNeighborPosition(SOUTHWEST,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		if(point->GetNeighborPosition(SOUTHEAST,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		if(point->GetNeighborPosition(SOUTH,pos)) {
			maputils_MapX2TileX(pos.x,pos.y,&tileX);
			RedrawHat(NULL,pos.y,tileX);
			RedrawBorders(NULL, pos.y, tileX);
			DrawImprovements(NULL, pos.y, tileX, 0);
		}

		UnlockSurface();
	}

	g_radarMap->RedrawTile(*point);
}







void TiledMap::BlackTile(aui_Surface *surface, const MapPoint *point)
{
	if (!TileIsVisible(point->x, point->y)) return;

	sint32 x,y;
	maputils_MapXY2PixelXY(point->x,point->y,&x,&y);

	if (m_zoomLevel == k_ZOOM_LARGEST)
		DrawBlackTile(surface, x, y);
	else {

		DrawBlackScaledLow(surface, *point, x, y, GetZoomTilePixelWidth(), GetZoomTilePixelHeight());
	}
}

void TiledMap::Blt(aui_Surface *surf)
{
	RECT			rect = {0, 0, surf->Width(), surf->Height()};
	extern C3UI		*g_c3ui;

	g_c3ui->TheBlitter()->Blt(surf, 0, 0, m_surface, &rect, 0);
}

bool TiledMap::TileIsCompletelyVisible(sint32 mapX, sint32 mapY, RECT *viewRect)
{
    RECT	shrunkMapViewRect   = viewRect ? *viewRect : m_mapViewRect;

	sint32  shrinkX             =
        (sint32)ceil((double)k_TILE_PIXEL_WIDTH / (double)GetZoomTilePixelWidth());
	sint32  shrinkY             =
        (sint32)ceil((double)k_TILE_GRID_HEIGHT / (double)GetZoomTileGridHeight());

	shrinkX *= 2;
	shrinkY *= 2;

	InflateRect(&shrunkMapViewRect, -shrinkX, -shrinkY);

	if(g_radarWindow->Height()) {
		shrunkMapViewRect.bottom -= (g_radarWindow->Height() / (GetZoomTilePixelHeight())) * 2;
	}
	RECT ul = shrunkMapViewRect;
	RECT ll = shrunkMapViewRect;
	RECT ur = shrunkMapViewRect;
	RECT lr = shrunkMapViewRect;

	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);
	sint32 tileX, tileY = mapY;
	maputils_MapX2TileX(mapX,mapY,&tileX);

	if (shrunkMapViewRect.left < 0) {
		ul.left = ll.left = 0;
		ul.right = ll.right = shrunkMapViewRect.right;
		ur.left = lr.left = shrunkMapViewRect.left + mapWidth;
		ur.right = lr.right = mapWidth;
	}
	if (shrunkMapViewRect.right >= mapWidth) {
		ul.left = ll.left = 0;
		ul.right = ll.right = shrunkMapViewRect.right - mapWidth;
		ur.left = lr.left = shrunkMapViewRect.left;
		ur.right = lr.right = mapWidth;
	}
	if (shrunkMapViewRect.top < 0) {
		ul.top = ur.top = 0;
		ul.bottom = ur.bottom = shrunkMapViewRect.bottom;
		ll.top = lr.top = shrunkMapViewRect.top + mapHeight;
		ll.bottom = lr.bottom = mapHeight;
	}
	if (shrunkMapViewRect.bottom >= mapHeight) {
		ll.top = lr.top = shrunkMapViewRect.top;
		ll.bottom = lr.bottom = mapHeight;
		ul.top = ur.top = 0;
		ul.bottom = ur.bottom = shrunkMapViewRect.bottom - mapHeight;
	}

	POINT point = {tileX,tileY};

	return (PtInRect(&ul,point) ||
			PtInRect(&ll,point) ||
			PtInRect(&ur,point) ||
			PtInRect(&lr,point)
           );
}

/// Determine whether a tile is visible
/// \param mapX X coordinate of tile
/// \param mapY Y coordinate of tile
/// \remarks The mapZ coordinate parameter is not used
#if defined(_SMALL_MAPPOINTS)
bool TiledMap::TileIsVisible(sint32 mapX, sint32 mapY)
#else
bool TiledMap::TileIsVisible(sint32 mapX, sint32 mapY, sint32 /* mapZ */)
#endif
{
	RECT ul = m_mapViewRect;
	RECT ll = m_mapViewRect;
	RECT ur = m_mapViewRect;
	RECT lr = m_mapViewRect;

	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);
	sint32 tileX, tileY = mapY;
	maputils_MapX2TileX(mapX,mapY,&tileX);

	if (m_mapViewRect.left < 0) {
		ul.left = ll.left = 0;
		ul.right = ll.right = m_mapViewRect.right;
		ur.left = lr.left = m_mapViewRect.left + mapWidth;
		ur.right = lr.right = mapWidth;
	}
	if (m_mapViewRect.right >= mapWidth) {
		ul.left = ll.left = 0;
		ul.right = ll.right = m_mapViewRect.right - mapWidth;
		ur.left = lr.left = m_mapViewRect.left;
		ur.right = lr.right = mapWidth;
	}
	if (m_mapViewRect.top < 0) {
		ul.top = ur.top = 0;
		ul.bottom = ur.bottom = m_mapViewRect.bottom;
		ll.top = lr.top = m_mapViewRect.top + mapHeight;
		ll.bottom = lr.bottom = mapHeight;
	}
	if (m_mapViewRect.bottom >= mapHeight) {
		ll.top = lr.top = m_mapViewRect.top;
		ll.bottom = lr.bottom = mapHeight;
		ul.top = ur.top = 0;
		ul.bottom = ur.bottom = m_mapViewRect.bottom - mapHeight;
	}

	POINT point = {tileX,tileY};

	if(!g_theWorld->IsXwrap()) {
		if(m_mapViewRect.left < 0) {
			return PtInRect(&ul, point) || PtInRect(&ll, point);
		}

		if(m_mapViewRect.right >= mapWidth) {
			return PtInRect(&ur, point) || PtInRect(&lr, point);
		}
	}

	if(!g_theWorld->IsYwrap()) {
		if(m_mapViewRect.top < 0) {
			return PtInRect(&ul, point) || PtInRect(&ur, point);
		}

		if(m_mapViewRect.bottom >= mapHeight) {
			return PtInRect(&ll, point) || PtInRect(&lr, point);
		}
	}

	return (PtInRect(&ul,point) || PtInRect(&ll,point) ||
			PtInRect(&ur,point) || PtInRect(&lr,point));
}

Pixel16 TiledMap::average(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4)
{
	short		r1, g1, b1,
				r2, g2, b2,
				r3, g3, b3,
				r4, g4, b4;
	short		r0, g0, b0;

	if (g_is565Format) {
		r1 = (pixel1 & 0xF800) >> 11;
		g1 = (pixel1 & 0x07E0) >> 5;
		b1 = (pixel1 & 0x001F);

		r2 = (pixel2 & 0xF800) >> 11;
		g2 = (pixel2 & 0x07E0) >> 5;
		b2 = (pixel2 & 0x001F);

		r3 = (pixel3 & 0xF800) >> 11;
		g3 = (pixel3 & 0x07E0) >> 5;
		b3 = (pixel3 & 0x001F);

		r4 = (pixel4 & 0xF800) >> 11;
		g4 = (pixel4 & 0x07E0) >> 5;
		b4 = (pixel4 & 0x001F);

		r0 = (r1 + r2 + r3 + r4) >> 2;
		g0 = (g1 + g2 + g3 + g4) >> 2;
		b0 = (b1 + b2 + b3 + b4) >> 2;

		return (r0 << 11) | (g0 << 5) | b0;
	} else {
		r1 = (pixel1 & 0x7C00) >> 10;
		g1 = (pixel1 & 0x03E0) >> 5;
		b1 = (pixel1 & 0x001F);

		r2 = (pixel2 & 0x7C00) >> 10;
		g2 = (pixel2 & 0x03E0) >> 5;
		b2 = (pixel2 & 0x001F);

		r3 = (pixel3 & 0x7C00) >> 10;
		g3 = (pixel3 & 0x03E0) >> 5;
		b3 = (pixel3 & 0x001F);

		r4 = (pixel4 & 0x7C00) >> 10;
		g4 = (pixel4 & 0x03E0) >> 5;
		b4 = (pixel4 & 0x001F);

		r0 = (r1 + r2 + r3 + r4) >> 2;
		g0 = (g1 + g2 + g3 + g4) >> 2;
		b0 = (b1 + b2 + b3 + b4) >> 2;

		return (r0 << 10) | (g0 << 5) | b0;
	}

}

void TiledMap::ProcessRun(Pixel16 **rowData1, Pixel16 **rowData2, Pixel16 *pix1, Pixel16 *pix2,
					sint32 pos, Pixel16 destPixel, short transparency, Pixel16 outlineColor,
					sint32 flags)
{
	static sint32		mode1;
	static sint32		mode2;
	static sint32		pos1;
	static sint32		pos2;
	static sint32		end1, end2;
	static sint32		alpha1, alpha2;
	static sint32		oldend1, oldend2;

	Pixel16				pixel1 = 0;
	Pixel16				pixel2 = 0;

	if (pos == -1) {
		end1 = ReadTag(&mode1, rowData1, &alpha1);
		end2 = ReadTag(&mode2, rowData2, &alpha2);

		pos1 = 0;
		pos2 = 0;

		oldend1 = 0;
		oldend2 = 0;

		*pix1 = k_MEDIUM_KEY;
		*pix2 = k_MEDIUM_KEY;

		return;
	}

	uint32 shadowRgbMask = pixelutils_GetShadow16RGBMask();
	while (pos1 <= pos) {
		switch (mode1) {
			case k_TILE_SKIP_RUN_ID	:
					pixel1 = k_MEDIUM_KEY;
				break;
			case k_TILE_COPY_RUN_ID			: {
					if (!(flags & k_OVERLAY_FLAG_SHADOWSONLY)) {
						pixel1 = **rowData1;
					} else {
						pixel1 = k_MEDIUM_KEY;
					}
					(*rowData1)++;
				}
				break;
			case k_TILE_SHADOW_RUN_ID : {
					if (!(flags & k_OVERLAY_FLAG_NOSHADOWS)) {
						pixel1 = pixelutils_Shadow16(**rowData1, shadowRgbMask);
					} else {
						pixel1 = k_MEDIUM_KEY;
					}
				}
				break;

			default :
				Assert(mode1 == k_TILE_SKIP_RUN_ID || mode1 == k_TILE_COPY_RUN_ID || mode1 == k_TILE_SHADOW_RUN_ID);
		}

		pos1++;

		if (pos1 >= end1) {
			oldend1 = end1;
			end1 = oldend1 + ReadTag(&mode1, rowData1, &alpha1);
		}
	}

	while (pos2 <= pos) {

		switch (mode2) {
			case k_TILE_SKIP_RUN_ID	:
					pixel2 = k_MEDIUM_KEY;
				break;
			case k_TILE_COPY_RUN_ID			:
				{
					if (!(flags & k_OVERLAY_FLAG_SHADOWSONLY)) {
						pixel2 = **rowData2;
					} else {
						pixel2 = k_MEDIUM_KEY;
					}
					(*rowData2)++;
				}
				break;
			case k_TILE_SHADOW_RUN_ID		:
				{
					if (!(flags & k_OVERLAY_FLAG_SHADOWSONLY)) {
						pixel2 = pixelutils_Shadow16(**rowData2, shadowRgbMask);
					} else {
						pixel2 = k_MEDIUM_KEY;
					}
				}
				break;

			default :
				Assert(mode2 == k_TILE_SKIP_RUN_ID || mode2 == k_TILE_COPY_RUN_ID || mode2 == k_TILE_SHADOW_RUN_ID);
		}

		pos2++;

		if (pos2 >= end2) {
			oldend2 = end2;
			end2 = oldend2 + ReadTag(&mode2, rowData2, &alpha2);
		}
	}

	*pix1 = pixel1;
	*pix2 = pixel2;
}

sint32 TiledMap::ReadTag(sint32 *mode, Pixel16 **rowData, sint32 *alpha)
{
	sint32			len;
	Pixel16		tag = **rowData;

	*mode = (tag & 0x0F00) >> 8;
	(*rowData)++;

	switch (*mode) {
		case k_TILE_SKIP_RUN_ID	:
			len = tag & 0x00FF;
			break;
		case k_TILE_COPY_RUN_ID :
			len = tag & 0x00FF;
		break;
		case k_TILE_SHADOW_RUN_ID :
			len = tag & 0x00FF;
			break;
		default :
			len = 1;
	}
	return len;
}

bool TiledMap::PointInMask(POINT hitPt) const
{
	sint32 x = (sint32)((double)hitPt.x / GetZoomScale(GetZoomLevel()));
	sint32 y = (sint32)(((double)hitPt.y / GetZoomScale(GetZoomLevel())) + k_TILE_PIXEL_HEADROOM);

	return (x >= m_tileHitMask[y].start) && (x <= m_tileHitMask[y].end);
}

bool TiledMap::MousePointToTilePos(POINT point, MapPoint &tilePos) const
{
	sint32      width   = GetZoomTilePixelWidth();
	sint32      height  = GetZoomTilePixelHeight();

    sint32  xoff,yoff;
	GetSmoothScrollOffsets(xoff,yoff);
  	sint32  x = point.x + xoff;
	sint32  y = point.y + yoff;


	if (!(m_mapViewRect.top & 1)) y -= GetZoomTileHeadroom();

	MapPoint		pos ((x / width) + m_mapViewRect.left,
                         (y / height) + m_mapViewRect.top/2
                        );

	POINT			hitPt;
 	hitPt.x = x % width;
	hitPt.y = y % height;

	sint16			maxX = static_cast<sint16>(m_mapBounds.right);

	if (!PointInMask(hitPt)) {

		pos.x = (sint16)((x + (width/2)) / width - 1 + m_mapViewRect.left);
		pos.y = (sint16)(((y + (height)/2) / height - 1) + m_mapViewRect.top/2);

		hitPt.x = (x + (width/2)) % width;
		hitPt.y = (y + (height)/2) % height;

		if (!PointInMask(hitPt))
			return false;
		else {
			if (pos.x >= pos.y) {
				tilePos.x = pos.x - pos.y;
			} else {
				tilePos.x = maxX + pos.x - pos.y;
			}
			tilePos.y = pos.y * 2 + 1;
		}
	} else {
		if (pos.x >= pos.y) {
			tilePos.x = pos.x - pos.y;
		} else {
			tilePos.x = maxX + pos.x - pos.y;
		}
		tilePos.y = pos.y * 2;
	}

	if(!g_theWorld->IsXwrap()) {
		if(pos.x < 0)
			return false;
		if(pos.x >= g_theWorld->GetXWidth())
			return false;
	}


	if((m_mapViewRect.top) & 1 && (m_mapViewRect.top < 0)) {
		tilePos.y -= 2;
    	tilePos.x++;
	}





	if (tilePos.x <0) tilePos.x = static_cast<sint16>(g_theWorld->GetWidth()) + tilePos.x;
	else if (g_theWorld->GetWidth() <= tilePos.x) tilePos.x = tilePos.x - static_cast<sint16>(g_theWorld->GetWidth());

	if (g_theWorld->IsYwrap()) {
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
			return false;
		} else if (g_theWorld->GetHeight() <= tilePos.y) {
			tilePos.y = static_cast<sint16>(g_theWorld->GetHeight() -1);
			return false;
		}
	}


    Assert (m_mapBounds.left <= tilePos.x);
    Assert (tilePos.x < m_mapBounds.right);
    Assert (m_mapBounds.top <= tilePos.y);
    Assert (tilePos.y < m_mapBounds.bottom);

	return true;
}

void TiledMap::HandleCheat(MapPoint &pos)
{
	if (ScenarioEditor::PlaceStartFlags())
    {
		ScenarioEditor::PlaceFlag(pos);
		return;
	}

	bool needPostProcess = false;
	if(ScenarioEditor::PaintHutMode()) {
		if(g_theWorld->GetCell(pos)->GetGoodyHut()) {
			g_theWorld->GetCell(pos)->DeleteGoodyHut();
		} else {
			g_theWorld->GetCell(pos)->CreateGoodyHut();
		}
		needPostProcess = true;
	}

	if(ScenarioEditor::PaintRiverMode()) {
		if (g_theWorld->IsRiver(pos)) {
			g_theWorld->UnsetRiver(pos.x, pos.y);
		} else {
			g_theWorld->SetRiver( pos );
		}

		needPostProcess = true;
	}

	if(ScenarioEditor::PaintGoodsMode()) {
		sint32 curGood;
		if(g_theWorld->GetGood(pos, curGood)) {
			g_theWorld->ClearGoods(pos.x, pos.y);
		} else {
			g_theWorld->SetGood(pos.x, pos.y, ScenarioEditor::PaintGood() + 1);
		}
		needPostProcess = true;
	}

	if(needPostProcess) {
		RadiusIterator it(pos, 1, 2.0);
		MapPoint mpos;
		for(it.Start(); !it.End(); it.Next()) {
			mpos = it.Pos();
			PostProcessTile(mpos, GetTileInfo(mpos));
			RedrawTile(&mpos);
		}
		return;
	}

	if ( ScenarioEditor::PaintTerrainImprovementMode() )
	{
		Player * p = g_player[g_selected_item->GetVisiblePlayer()];
		if (!p) return;
		TerrainImprovement theImprovement =
            g_theTerrainImprovementPool->Create
                (p->m_owner,
				 pos,
				 ScenarioEditor::PaintTerrainImprovement(),
				 0
                );

		if (g_theTerrainImprovementPool->IsValid(theImprovement.m_id))
        {
			p->m_terrainImprovements->Insert(theImprovement);
			theImprovement.Complete();
		}
	}

	if (g_placeGoodsMode || ScenarioEditor::PaintTerrainMode())
    {
		sint32      tileNum     = ScenarioEditor::PaintTerrainMode()
                                  ? ScenarioEditor::PaintTerrain()
                                  : 0;
        bool        river       = false;

		if (g_placeGoodsMode)
        {
			sint32 curGood;
			if(g_theWorld->GetGood(pos, curGood))
            {
				g_theWorld->ClearGoods(pos.x, pos.y);
			}
            else
            {
				g_theWorld->SetGood(pos.x, pos.y, g_placeGoodsMode);
			}
		}
        else
        {
			switch (tileNum)
            {
			case TILEPAD_TYPE_GOODY:
				if (g_theWorld->IsLand(pos))
                {
                    GoodyHut * hut = g_theWorld->GetGoodyHut(pos);
					if (hut)
                    {
						g_theWorld->GetCell(pos)->DeleteGoodyHut();
					}
                    else
                    {
						g_theWorld->GetCell(pos)->CreateGoodyHut();
					}
					river = true;
				}
				else
                {
					return;
				}
				break;

			case TILEPAD_TYPE_RIVER:
				if (g_theWorld->IsRiver(pos))
                {
					g_theWorld->UnsetRiver(pos.x, pos.y);
				}
                else
                {
					g_theWorld->SetRiver( pos );
				}
				river = true;
				break;

			default:
				// No action
				break;
			}
		}

		g_theWorld->CutImprovements(pos);

		if (!river && !g_placeGoodsMode)
        {
			sint32 radius = 1;
			switch(ScenarioEditor::BrushSize())
            {
			default:
            // case 2:
				break;
			case 1:
				radius = 0;
				break;
			case 4:
				radius = 2;
				break;
			}

			g_theWorld->SmartSetTerrain(pos, tileNum, radius);
		}

#if 0

		g_theWorld->GetCell(pos)->CalcTerrainMoveCost();

		if (g_theWorld->GetCell(pos)->IsAnyUnitInCell()) {
			if (!g_theWorld->GetCell(pos)->UnitArmy()->CanEnter(pos)) {
				g_theWorld->GetCell(pos)->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_CHEAT, -1);
			}
		}

		if (g_theWorld->HasCity(pos)) {
			if (!g_theWorld->CanEnter(pos, g_theWorld->GetCell(pos)->GetCity().GetMovementType())) {
				g_theWorld->GetCell(pos)->GetCity().KillUnit(CAUSE_REMOVE_ARMY_CHEAT, -1);
			}
		}
#endif





	}

	if ( ScenarioEditor::PlaceUnitsMode() || ScenarioEditor::PlaceCityMode()) {

		sint32 unitNum = ScenarioEditor::PlaceUnitsMode() ? ScenarioEditor::UnitIndex() : g_unitNum;

		Player *p = g_player[g_selected_item->GetVisiblePlayer()];
		if (!p) return;
		sint32 govType = p->GetGovernmentType();

		if(ScenarioEditor::PlaceCityMode()) {
			sint32 ui;
			for(ui = 0; ui < g_theUnitDB->NumRecords(); ui++) {
				if(g_theUnitDB->Get(ui, govType)->GetHasPopAndCanBuild() &&
				   g_theUnitDB->Get(ui, govType)->GetMovementTypeLand()) {
					unitNum = ui;
					break;
				}
			}
			Assert(ui < g_theUnitDB->NumRecords());
		}

		if (g_killMode) {

			Cell *cell = g_theWorld->GetCell(pos);
			if(cell->UnitArmy()) {
				cell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_TOE, -1);
				g_selected_item->Deselect(g_selected_item->GetVisiblePlayer());
			}

			if (0 != cell->GetCity().m_id) {
				cell->GetCity().KillUnit(CAUSE_REMOVE_ARMY_TOE, -1);
			}
		} else
		if (unitNum != -1) {
			if (g_theWorld->CanEnter(pos, g_theUnitDB->Get(unitNum, govType)->GetMovementType()) ||
				g_theUnitDB->Get(unitNum, govType)->GetHasPopAndCanBuild() ||
				g_theUnitDB->Get(unitNum, govType)->GetIsTrader()) {

				if(g_theUnitDB->Get(unitNum, govType)->GetHasPopAndCanBuild()) {
					if(g_theWorld->IsWater(pos) || g_theWorld->IsShallowWater(pos)) {
						sint32 i;
						for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
							if(g_theUnitDB->Get(i, govType)->GetHasPopAndCanBuild() &&
								g_theUnitDB->Get(i, govType)->GetMovementTypeSea()) {
								unitNum = i;
								break;
							}
						}
					}
					Unit id1 = p->CreateCity(unitNum, pos, CAUSE_NEW_CITY_CHEAT, NULL, -1);
					//Added by Martin Gühmann to make the created city selected.
					g_selected_item->SetSelectCity(id1);
					//End Add
				} else {

					if (g_theWorld->HasCity(pos)) {
						if (g_theWorld->GetCell(pos)->GetCity().GetOwner() == g_selected_item->GetVisiblePlayer()) {
							Unit id1 = p->CreateUnit(unitNum, pos, Unit(), FALSE, CAUSE_NEW_ARMY_CHEAT);
						}
					} else {

						Unit id1 = p->CreateUnit(unitNum, pos, Unit(), FALSE, CAUSE_NEW_ARMY_CHEAT);
					}
				}
			}
		}
	}
}







void TiledMap::AdjustForOverlappingSprite(POINT mousePt, MapPoint &pos)
{
	MapPoint		newPos;
	Unit			top;

	pos.GetNeighborPosition(SOUTH, newPos);
	if (g_theWorld->GetTopVisibleUnit(newPos, top)) {
		if (top.GetActor()) {
			if (top.GetActor()->HitTest(mousePt)) {
				pos = newPos;
				return;
			}
		}
	}

	pos.GetNeighborPosition(SOUTHWEST, newPos);
	if (g_theWorld->GetTopVisibleUnit(newPos, top)) {
		if (top.GetActor()) {
			if (top.GetActor()->HitTest(mousePt)) {
				pos = newPos;
				return;
			}
		}
	}

	pos.GetNeighborPosition(SOUTHEAST, newPos);
	if (g_theWorld->GetTopVisibleUnit(newPos, top)) {
		if (top.GetActor()) {
			if (top.GetActor()->HitTest(mousePt)) {
				pos = newPos;
				return;
			}
		}
	}

}

void TiledMap::MouseDrag(aui_MouseEvent *data)
{
	MapPoint		pos;

    if (GetMouseTilePos(pos)) {




		if (data->lbutton && !data->rbutton) {

			if (g_isCheatModeOn || ScenarioEditor::HandleClicks()) {

				if(ScenarioEditor::SelectRegion()) {
					ScenarioEditor::ExpandRegion(pos);
				} else {
					HandleCheat(pos);
				}
			} else {
				g_selected_item->RegisterClick(pos, data, FALSE,
											   true, false);
			}
		}
	}
}

void TiledMap::Click(aui_MouseEvent *data, bool doubleClick)
{
	MapPoint		pos;
	POINT			point = data->position;

	if (MousePointToTilePos(point, pos))
	{
		if (data->lbutton && !data->rbutton)
		{
			if (g_isCheatModeOn || ScenarioEditor::HandleClicks())
			{
				if(ScenarioEditor::SelectRegion())
				{
					ScenarioEditor::StartRegion(pos);
				}
				else if(ScenarioEditor::PasteMode())
				{
					ScenarioEditor::Paste(pos);
				}
				else
				{
					HandleCheat(pos);
				}
			}
			else if ( g_isTransportOn )
			{
				g_isTransportOn = FALSE;
				g_selected_item->Deselect( g_selected_item->GetVisiblePlayer() );
			}
			else if (g_tileImprovementMode)
			{
				// Nothing
			}
			else
			{
				g_selected_item->RegisterClick(pos, data, doubleClick,
											   false, false);
			}
		}
		else
		{
			if (data->rbutton && !data->lbutton)
			{
				if ( g_tileImprovementMode )
				{
					g_selected_item->Deselect( g_selected_item->GetVisiblePlayer() );
				}
				else
				{
					g_selected_item->RegisterClick(pos, data, doubleClick, false, false);
				}
			}
			else
			{
				// Nothing
			}
		}
	}
}

void TiledMap::Drop(aui_MouseEvent *data)
{
	MapPoint		pos;
	POINT           point = data->position;

	if (MousePointToTilePos(point, pos)) {

		if (g_isCheatModeOn) {

		}
		else if(ScenarioEditor::SelectRegion()) {
			ScenarioEditor::EndRegion(pos);
		}
		else if (g_tradeSelectedState)
		{

			g_tradeSelectedState = FALSE;
	        TradeRoute * route;
			g_grabbedItem->GetGrabbedItem(&route);

			route->SetPathSelectionState(k_TRADEROUTE_NO_PATH);

			if (!route->IsSelectedPathSame())
			{

				route->UpdateSelectedCellData(*route);
			}
			else route->ClearSelectedPath();
		}
		else {
			g_selected_item->RegisterClick(pos, data, FALSE,
										   false, true);
		}
	}
}

void TiledMap::Idle(void)
{
	MapPoint point;
	if (!GetMouseTilePos(point)) return;

	if (g_tradeSelectedState)
	{
	    TradeRoute * route;
		g_grabbedItem->GetGrabbedItem(&route);

		route->SetPathSelectionState(k_TRADEROUTE_SELECTED_PATH);
		route->GenerateSelectedPath(point);
	}
}

bool TiledMap::GetMousePos(POINT &pos) const
{
	Assert(g_c3ui && g_background);
	if (!g_c3ui || !g_background) return false;

	aui_Mouse * mouse = g_c3ui->TheMouse();
	if (mouse == NULL) return false;

	pos.x = mouse->X() - g_background->X();
	pos.y = mouse->Y() - g_background->Y();
	return true;
}

bool TiledMap::GetMouseTilePos(MapPoint &pt) const
{
	POINT   pos;
	return GetMousePos(pos) && MousePointToTilePos(pos, pt);
}

TileInfo *TiledMap::GetTileInfo(const MapPoint &pos)
{

	if (m_localVision != NULL) {
		if(!m_localVision->IsVisible(pos)) {
			UnseenCellCarton ucell;
			if(m_localVision->GetLastSeen(pos, ucell))
				return ucell.m_unseenCell->GetTileInfo();
		}
	}
	return g_theWorld->GetTileInfo(pos);
}

void TiledMap::NextPlayer(void)
{
	m_nextPlayer = TRUE;
}

void TiledMap::CopyVision()
{
	sint32  newPlayer   = g_selected_item->GetVisiblePlayer();
	if (g_player[newPlayer])
	{
		m_localVision->SetAmOnScreen(false);
		m_localVision = g_player[newPlayer]->m_vision;
		m_oldPlayer   = newPlayer;
		m_localVision->SetAmOnScreen(true);
	}

	Refresh();
	InvalidateMap();
}

//----------------------------------------------------------------------------
//
// Name       : TiledMap::ReadyToDraw
//
// Description: Determine whether the display should contain visible tiles.
//
// Parameters : -
//
// Globals    : g_network       : multiplayer information
//              g_slicEngine    : general game engine
//              g_selected_item : selected item on screen
//              g_turn          : turn information
//
// Returns    : bool            : tiles may be drawn
//
// Remark(s)  : The tiles may not be drawn in the following cases:
//              - For multiplayer games, when the network is not ready.
//              - When the game engine says so (e.g. between turns in hotseat).
//              - When there is no selected item (yet).
//              - Before the actual start of the game.
//
//----------------------------------------------------------------------------
bool TiledMap::ReadyToDraw() const
{
	if ((g_network.IsActive() || g_network.IsNetworkLaunch()) &&
	    !g_network.ReadyToStart()
       )
    {
		return false;
    }

    return g_slicEngine     && !g_slicEngine->ShouldScreenBeBlank() &&
           g_selected_item  &&
           g_turn           &&
                ((g_turn->GetRound() > 0) || (m_localVision->GetOwner() > 0));
}

sint32
TiledMap::DrawOverlayClipped(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, sint32 flags)
{
	uint8			*surfBase;
	sint32			surfWidth;
	sint32			surfHeight;
	sint32			surfPitch;
	sint32			errcode;

	if (data == NULL)
		return 0;

	if (surface)
	{
		errcode = surface->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);

		if ( errcode != AUI_ERRCODE_OK )
			return AUI_ERRCODE_SURFACELOCKFAILED;

		surfWidth	= surface->Width();
		surfHeight	= surface->Height();
		surfPitch	= surface->Pitch();
	}
	else
	{
		surfBase	= m_surfBase;
		surfWidth	= m_surfWidth;
		surfHeight	= m_surfHeight;
		surfPitch	= m_surfPitch;
	}

	if ((x>=surfPitch)||(y>=surfHeight))
	   return 0 ;

	if ((x < 0) || (y < 0))
		return 0;

	unsigned short	*destPixel;

	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	sint32 len,looplen;

	sint32 xoff=x;
	sint32 i;

	Pixel16		*rowData;
	Pixel16		tag;

	for (sint32 j = start; j <= end; j++)
	{
		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		if ((y+j) >= surfHeight)
			return 0;

		if ((sint16)table[j-start] == -1)
			continue;

		rowData = dataStart + table[j-start];

		do
		{
			tag = *rowData++;
			len = (tag & 0x00FF);

			switch ((tag & 0x0F00) >> 8)
			{
				case	k_TILE_SKIP_RUN_ID	:
						destPixel	+= len;
						xoff		+= len;
						break;

				case	k_TILE_COPY_RUN_ID			:

						looplen = len;

						if (xoff<0)
						{
							looplen   += xoff;
							destPixel -= xoff;
							rowData	  -= xoff;
						}
						else
							if (xoff>surfPitch)
								looplen -= (xoff-surfPitch);

						for (i=0; i<looplen; i++)
						{
							if (!(flags & k_OVERLAY_FLAG_SHADOWSONLY))

				destPixel[i] = rowData[i];
						}

						destPixel += len;
						rowData   += len;
						break;

				case	k_TILE_SHADOW_RUN_ID		:

						looplen = len;

						if (xoff<0)
						{
							looplen += xoff;
							destPixel -= xoff;
						}
						else
							if (xoff>surfPitch)
								looplen -= (xoff-surfPitch);

						for (i=0; i<looplen; i++)
						{
							if (!(flags & k_OVERLAY_FLAG_NOSHADOWS))
								destPixel[i] = pixelutils_Shadow16(destPixel[i], shadowRgbMask);
						}

						destPixel += len;
						break;
			}

		} while ((tag & 0xF000) == 0);
	}

	if (surface)
	{
		errcode = surface->Unlock((LPVOID *)surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return 0;
}

void
TiledMap::DrawTransitionTileClipped(aui_Surface *surface, MapPoint &pos, sint32 xpos, sint32 ypos)
{
	if (!surface)
		surface = m_surface;

	uint8 * pSurfBase			= m_surfBase;
	sint32 surfWidth	= m_surfWidth;
	sint32 surfHeight	= m_surfHeight;
	sint32 surfPitch	= m_surfPitch;

	ypos+=k_TILE_PIXEL_HEADROOM;

	if (xpos > (surfWidth-k_TILE_PIXEL_WIDTH))
		return;
	if (ypos > (surfPitch-k_TILE_PIXEL_HEIGHT))
		return;

	TileInfo * tileInfo = GetTileInfo(pos);
	Assert(tileInfo);
	if (tileInfo == NULL)
		return;

	uint16 index = tileInfo->GetTileNum();

	BaseTile * baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL)
		return;

	Pixel16 *data = baseTile->GetTileData();

	Pixel16	*t[4];
	Pixel16  defaults[4];

	t[0] = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(0), 0);
	t[1] = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(1), 1);
	t[2] = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(2), 2);
	t[3] = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(3), 3);

	defaults[0] = 0xF800;
	defaults[1] = 0x07E0;
	defaults[2] = 0x001F;
	defaults[3] = 0xF81F;

	Pixel16 * dataPtr = data;

	for (sint32 y = 0; y < k_TILE_PIXEL_HEIGHT; ++y)
	{
		sint32 const startX  = (y <= 23) ? (23-y)*2 : (y-24)*2;
		sint32 const endX    = k_TILE_PIXEL_WIDTH - startX;

		for (sint32 x = startX; x < endX; ++x)
		{
			Pixel16 srcPixel    = *dataPtr++;
			Pixel16 tindex      = srcPixel;

			if (tindex<4)
			{
				if (t[tindex])
				{
					srcPixel = *(t[tindex]);
					t[tindex] ++;
				}
				else
					srcPixel = defaults[tindex];
			}

			int ysrc = (y+ypos);

			if (ysrc<0)
				continue;

			if (ysrc>=surfHeight)
				return;

			int xsrc = (x+xpos);

			if (xsrc<0)
				continue;

			xsrc<<=1;

		  	if (xsrc>=surfPitch)
				continue;

			Pixel16 * pDestPixel = (Pixel16 *)(pSurfBase + (ysrc*surfPitch+xsrc));

			*pDestPixel = srcPixel;
		}
	}
}






void TiledMap::SetZoomLevel(sint32 level)
{

	Assert(level >= 0 && level < k_MAX_ZOOM_LEVELS);

	if (g_background)
	{
		sint32 xOffset;
		sint32 yOffset;
		if (level == k_ZOOM_LARGEST) {
			xOffset = -m_zoomTilePixelWidth[m_zoomLevel] - g_background->X();
			yOffset = -(m_zoomTilePixelHeight[m_zoomLevel] / 2) * 3 - g_background->Y();
		} else {
			xOffset = m_zoomTilePixelWidth[m_zoomLevel] - m_zoomTilePixelWidth[level];
			yOffset = (m_zoomTilePixelHeight[m_zoomLevel] - m_zoomTilePixelHeight[level]) * 2;
		}
		g_background->Offset(xOffset, yOffset);
		OffsetRect(&g_backgroundViewport, -xOffset, -yOffset);
	}

	m_zoomLevel = level;

	m_scale = m_zoomTileScale[m_zoomLevel];

	if(m_zoomCallback)
		m_zoomCallback();
}

bool TiledMap::CanZoomIn() const
{

	return(GetZoomLevel() < k_ZOOM_LARGEST);
}

bool TiledMap::ZoomIn(void)
{

	if(CanZoomIn()) {

		ZoomUpdate(GetZoomLevel() + 1);

		return(true);
	}

	return(false);
}

bool TiledMap::CanZoomOut() const
{

	if(GetZoomLevel() > k_ZOOM_SMALLEST) {

		RECT zoomViewRectangle;
		CalculateZoomViewRectangle(GetZoomLevel() - 1, zoomViewRectangle);

		sint32 width, height;
		GetMapMetrics(&width, &height);

		if((zoomViewRectangle.right <= width) &&
			(zoomViewRectangle.bottom <= height))
			return(true);
	}

	return(false);
}

bool TiledMap::ZoomOut(void)
{

	if(CanZoomOut()) {

		ZoomUpdate(GetZoomLevel() - 1);

		return(true);
	}

	return(false);
}

void TiledMap::CalculateZoomViewRectangle(sint32 zoomLevel, RECT &rectangle) const
{

	sint32 width = m_displayRect.right - m_displayRect.left;
	sint32 height = m_displayRect.bottom - m_displayRect.top;

	rectangle.left = rectangle.top = 0;

	rectangle.right = (width - (m_zoomTilePixelWidth[zoomLevel] / 2)) /
		m_zoomTilePixelWidth[zoomLevel];
	rectangle.bottom = (height - m_zoomTileHeadroom[zoomLevel]) /
		(m_zoomTilePixelHeight[zoomLevel] / 2) - 1;
}

sint32 TiledMap::GetZoomDisplayWidth() const
{
	// Total width = screen-width + 5/2 * max-tile-width, so zoomed width need to be adjusted 5/2 delta tile-width
	sint32 deltaTileWidth = m_zoomTilePixelWidth[k_ZOOM_LARGEST] - GetZoomTilePixelWidth();
	return m_displayRect.right - m_displayRect.left - (deltaTileWidth / 2) * 5;
}

sint32 TiledMap::GetZoomDisplayHeight() const
{
	// Total height = screen-height + 2 * max-tile-height, so zoomed height need to be adjusted 4 delta tile-height
	sint32 deltaTileHeight = m_zoomTilePixelHeight[k_ZOOM_LARGEST] - GetZoomTilePixelHeight();
	return m_displayRect.bottom - m_displayRect.top - deltaTileHeight * 4;
}

void TiledMap::ZoomHitMask()
{
	for (int i=0; i<k_TILE_GRID_HEIGHT;i++) {
   		m_tileHitMask[i].start = (sint16)(m_tileHitMask[i].d_start * m_scale);
		m_tileHitMask[i].end   = (sint16)(m_tileHitMask[i].d_end * m_scale);
	}
}

void TiledMap::ZoomUpdate(sint32 zoomLevel)
{

	sint32 mapViewCenterX = (m_mapViewRect.left + m_mapViewRect.right) / 2;
	sint32 mapViewCenterY = (m_mapViewRect.top + m_mapViewRect.bottom) / 2;
	sint32 mapViewCenterXWrap = 0, mapViewCenterYWrap = 0;
	maputils_WrapPoint(mapViewCenterX, mapViewCenterY,
		&mapViewCenterXWrap, &mapViewCenterYWrap);

	sint32 mapViewCenterXTile = maputils_TileX2MapX(mapViewCenterXWrap, mapViewCenterYWrap);

	SetZoomLevel(zoomLevel);

	CalculateMetrics();

	g_radarMap->CenterMap(MapPoint(mapViewCenterXTile, mapViewCenterYWrap));

	Refresh();

	InvalidateMap();

	ZoomHitMask();
}
