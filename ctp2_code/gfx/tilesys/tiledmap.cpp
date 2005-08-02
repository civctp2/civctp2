//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile map handling
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
// - _DEBUG
//   Set when generating the debug version
// - __BIG_DIRTY_BLITS__
// - __USING_SPANS__
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
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tiledmap.h"

#include "c3errors.h"

#include "Cell.h"
#include "player.h"
#include "cellunitlist.h"
#include "SelItem.h"
#include "civarchive.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_stringtable.h"
#include "aui_Factory.h"
#include "aui_blitter.h"
#include "aui_dirtylist.h"
#include "c3window.h"

#include "ctp2_button.h"
#include "spriteeditor.h"

#include "background.h"

#include "primitives.h"
#include "pixelutils.h"
#include "tileutils.h"
#include "tiffutils.h"

#include "MapPoint.h"
#include "UnseenCell.h"
#include "GoodyHuts.h"
#include "SpriteStateDB.h"
#include "TerrainRecord.h"
#include "StrDB.h"

#include "screenmanager.h"
#include "Sprite.h"
#include "UnitSpriteGroup.h"
#include "UnitActor.h"

#include "EffectActor.h"
#include "GoodActor.h"
#include "director.h"

#include "TileInfo.h"
#include "BaseTile.h"
#include "TileInfo.h"
#include "TerrImproveData.h"
#include "maputils.h"
#include "tileset.h"
#include "TileDrawRoad.h"

#include "CivPaths.h"

#include "colorset.h"
#include "radarmap.h"

#include "buttonbank.h"
#include "controlpanelwindow.h"

#include "pointerlist.h"

#include "profileDB.h"
#include "AICause.h"
#include "UnitPool.h"




#include "TradeRoute.h"
#include "TradeRouteData.h"
#include "grabitem.h"
#include "ArmyPool.h"
#include "ArmyData.h"

#include "CityRadius.h"
#include "network.h"
#include "TurnCnt.h"


#include "terrainutil.h"
#include "TerrainRecord.h"
#include "TerrImprove.h"
#include "TerrainImprovementRecord.h"
#include "TerrImprovePool.h"		


#include "c3_popupwindow.h"



#include "SlicEngine.h"

#include "UnitData.h"
#include "citydata.h"
#include "UnitRecord.h"
#include "ResourceRecord.h"

#include "MoveFlags.h"

#include "CityInfluenceIterator.h"


#include "gfx_options.h"

#include "scenarioeditor.h"


#include "gamefile.h"

#include "radarwindow.h"
#include "ctp2_Window.h"

extern World		*g_theWorld;
extern UnitPool		*g_theUnitPool;

extern ColorSet			*g_colorSet;
extern CivPaths			*g_civPaths;
extern Player			**g_player;
extern C3UI				*g_c3ui;
extern sint32			g_is565Format;
extern SelectedItem		*g_selected_item;
extern Director			*g_director;
extern Background		*g_background;
extern RadarMap			*g_radarMap;
extern RECT				g_backgroundViewport;
extern ScreenManager	*g_screenManager;
extern SpriteStateDB	*g_theGoodsSpriteStateDB;
extern StringDB			*g_theStringDB;
extern ControlPanelWindow *g_controlPanel;
extern TurnCount        *g_turn;
extern ArmyPool			*g_theArmyPool;
extern SpriteEditWindow *g_spriteEditWindow;


extern GrabItem			*g_grabbedItem;
extern sint32			g_tradeSelectedState;

extern sint32			g_fog_toggle;
extern sint32			g_god; 

extern sint32		g_isCheatModeOn;

extern sint32		g_isCityPadOn;

sint32		g_unitNum = -1;
sint32		g_cityNum = -1;
BOOL g_killMode = FALSE;

extern sint32		g_toolbarCurPlayer;	

extern sint32		g_isLegalMode;

sint32		g_tileImprovementMode = 0;
extern sint32		g_specialAttackMode;
extern ProfileDB	*g_theProfileDB; 
extern TiledMap		*g_tiledMap;




BOOL	g_isTransportOn = FALSE;

extern BOOL g_show_ai_dbg; 
extern sint32				g_modalWindow;

Pixel16 tcolor;

sint32 g_isFastCpu = 1;
sint32 g_isGridOn = 0;

sint32 s_zoomTilePixelWidth[k_MAX_ZOOM_LEVELS]	=	{48,56,68,76,88,96};
sint32 s_zoomTilePixelHeight[k_MAX_ZOOM_LEVELS]	=	{24,28,34,38,44,48};
sint32 s_zoomTileGridHeight[k_MAX_ZOOM_LEVELS] =	{36,42,51,57,66,72};
sint32 s_zoomTileHeadroom[k_MAX_ZOOM_LEVELS] =		{12,14,17,19,22,24};
double s_zoomTileScale[k_MAX_ZOOM_LEVELS] =			{0.50526, 0.58947, 0.71578, 0.8, 0.92631, 1.0};



sint32 g_placeGoodsMode = FALSE;





BOOL	g_drawArmyClumps;



#define k_GRID_CHUNKS_X		10
#define k_GRID_CHUNKS_Y		6


void TiledMap::InitLUT(void)
{
	for (sint32 i=0; i<k_MAX_ZOOM_LEVELS; i++) {
		m_zoomTilePixelWidth[i] = s_zoomTilePixelWidth[i];
		m_zoomTilePixelHeight[i] = s_zoomTilePixelHeight[i];
		m_zoomTileGridHeight[i] = s_zoomTileGridHeight[i];
		m_zoomTileHeadroom[i] = s_zoomTileHeadroom[i];
		m_zoomTileScale[i] = s_zoomTileScale[i];
	}

	SetZoomLevel(k_ZOOM_NORMAL);
}

TiledMap::TiledMap(MapPoint &size)
:
	m_isScrolling         (false),
	m_lockedSurface       (NULL),
	m_surfBase            (NULL),
	m_surfWidth           (0),
	m_surfHeight          (0),
	m_surfPitch           (0),
	m_surfIsLocked        (false),
	m_overlayRec          (NULL),
	m_drawHilite          (false)
#if defined(_DEBUG)
	,
	m_showPopHack         (false)
#endif
{	
	Assert(g_theWorld != NULL);
	if (g_theWorld == NULL) 
	{
		
        c3errors_ErrorDialog ("TileMap","Could not allocate memory"); 
		return;
	}

	m_zoomCallback = NULL;

	m_surface = NULL;
	m_mapSurface = NULL;
	m_overlayActive	=false;

	SetRect(&m_displayRect, 0, 0, 0, 0);
	SetRect(&m_surfaceRect, 0, 0, 0, 0);
	SetRect(&m_mapBounds, 0, 0, size.x, size.y);
	SetRect(&m_mapViewRect, 0, 0, 0, 0);

	m_scale = 1.0;

	InitLUT();

	m_tileSet = NULL;

	m_nextPlayer = FALSE; 

	
	m_smoothOffsetX = 0;
	m_smoothOffsetY = 0;
	m_smoothLastX   = 0;
	m_smoothLastY   = 0;

	
	GenerateHitMask();

#ifdef __USING_SPANS__
	
	m_mixDirtyList = NULL;
	m_oldMixDirtyList = NULL;
	m_mapDirtyList = NULL;
#else
	m_mixDirtyList = new aui_DirtyList;
	m_oldMixDirtyList = new aui_DirtyList;
	m_mapDirtyList = new aui_DirtyList;
#endif 

	m_localVision = NULL;

	m_oldPlayer = PLAYER_INDEX_INVALID;

	m_font = NULL;

	
	AUI_ERRCODE errcode;
	aui_StringTable		*stringTable = new aui_StringTable(&errcode, "TiledMapFontStringTable");
	Assert( AUI_NEWOK(stringTable, errcode) );

	if (AUI_NEWOK(stringTable, errcode) ) {
		MBCHAR		*fontNameString,
					*fontSizeString;
		sint32		fontSize;

		fontNameString = stringTable->GetString(0);
		fontSizeString = stringTable->GetString(1);
		fontSize = atoi(fontSizeString);

		m_font = g_c3ui->LoadBitmapFont(fontNameString);
		Assert(m_font);

		m_font->SetPointSize(fontSize);

		MBCHAR *fString = stringTable->GetString(2);

		strncpy(m_fortifyString, fString, 3);
		m_fortifyString[3] = 0;
	}

	
	delete stringTable;

#ifdef _DEBUG
	ClearRectMetrics();
#endif


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
	delete m_localVision;
	delete m_tileSet;
	// m_surface	    not deleted: reference only			
	// m_surfBase       not deleted: reference only
	// m_overlayRec     not deleted: reference only
	// m_lockedSurface  not deleted: reference only	
}

sint32 TiledMap::Initialize(RECT *viewRect)
{
	sint32			w = viewRect->right - viewRect->left;
	sint32			h = viewRect->bottom - viewRect->top;
	AUI_ERRCODE		errcode;

	m_mapSurface = aui_Factory::new_Surface(errcode, w, h, 16);
	Assert( m_mapSurface != NULL );
	if ( !m_mapSurface ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	
	
	m_surface = m_mapSurface;

#ifdef __USING_SPANS__
	
	
	m_mixDirtyList = new aui_DirtyList( TRUE, w, h );
	m_oldMixDirtyList = new aui_DirtyList( TRUE, w, h );
	m_mapDirtyList = new aui_DirtyList( TRUE, w, h );
#endif 
	
	m_displayRect = m_surfaceRect = *viewRect;
	OffsetRect(&m_surfaceRect, -m_surfaceRect.left, -m_surfaceRect.top);

	CalculateMetrics();
	
	m_localVision = new Vision(g_selected_item->GetVisiblePlayer(), TRUE);

	
	Assert(m_localVision);
	
	if(g_player[g_selected_item->GetVisiblePlayer()])
		m_localVision->Copy(g_player[g_selected_item->GetVisiblePlayer()]->m_vision);





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






void TiledMap::CheckRectAgainstGrid(RECT &rect, aui_DirtyList *list)
{
	GridRect		*gr;

	
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
			gr = &m_gridRects[i][j];

			
			

			




















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
			list->AddRect(&gr->rect);

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
	AUI_ERRCODE		errcode;

	m_lockedSurface = surface;

	errcode = surface->Lock(NULL, (LPVOID *)&m_surfBase, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return;

	m_surfWidth = surface->Width();
	m_surfHeight = surface->Height();
	m_surfPitch = surface->Pitch();

	m_surfIsLocked = TRUE;
}

void TiledMap::UnlockSurface(void)
{
	AUI_ERRCODE		errcode;

	errcode = m_lockedSurface->Unlock((LPVOID)m_surfBase);
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return;

	m_surfBase = NULL;
	m_surfWidth = 0;
	m_surfHeight = 0;
	m_surfPitch = 0;

	m_surfIsLocked = FALSE;
}

void TiledMap::AddDirty(sint32 left, sint32 top, sint32 width, sint32 height, aui_DirtyList *list)
{
	RECT		rect;

	rect.left = left;
	rect.top = top;
	rect.right = left + width;
	rect.bottom = top + height;

	AddDirtyRect(rect, list);
}

#define __GRIDDED_BLITS__


void TiledMap::AddDirtyRect(RECT &rect, aui_DirtyList *list)
{	

	Assert(list != NULL);

	if (list) {
		RECT tempRect = rect;

#ifdef __GRIDDED_BLITS__
	if (list == m_mixDirtyList) {
		CheckRectAgainstGrid(rect, list);
	} else {
		
		tempRect.left = tempRect.left & 0xFFFFFFFC;
		tempRect.right = (tempRect.right & 0xFFFFFFFC)+4;

		
		tempRect.top = tempRect.top & 0xFFFFFFFE;
		tempRect.bottom = (tempRect.bottom & 0xFFFFFFFE) + 2;

		list->AddRect(&tempRect);
	}
#else
	#ifdef __BIG_DIRTY_BLITS__
		
		tempRect.left = tempRect.left & 0xFFFFFF00;
		tempRect.right = (tempRect.right & 0xFFFFFF00)+256;
		
		tempRect.top = tempRect.top & 0xFFFFFFF0;
		tempRect.bottom = (tempRect.bottom & 0xFFFFFFF0) + 16;

		list->AddRect(&tempRect);
	#else
		
		tempRect.left = tempRect.left & 0xFFFFFFFC;
		tempRect.right = (tempRect.right & 0xFFFFFFFC)+4;

		
		tempRect.top = tempRect.top & 0xFFFFFFFE;
		tempRect.bottom = (tempRect.bottom & 0xFFFFFFFE) + 2;

		list->AddRect(&tempRect);
	#endif
#endif

	}
}

void TiledMap::AddDirtyTile(MapPoint &pos, aui_DirtyList *list)
{
	sint32		x, y;

	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	RECT		rect;

	rect.left = x;
	rect.top = y;
	rect.right = x + k_TILE_GRID_WIDTH;
	rect.bottom = y + k_TILE_GRID_HEIGHT;

	AddDirtyRect(rect, list);


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

void TiledMap::AddDirtyRectToMix(RECT &rect)
{
	AddDirtyRect(rect, m_mixDirtyList);
}

void TiledMap::AddDirtyTileToMix(MapPoint &pos)
{
	AddDirtyTile(pos, m_mixDirtyList);
}



void TiledMap::ClearMixDirtyRects(void)
{
	Assert (m_mixDirtyList != NULL);

	if (m_mixDirtyList)
		m_mixDirtyList->Flush();

	ClearGrid();
}

void TiledMap::CopyMixDirtyRects(aui_DirtyList *dest)
{
	ListPos			position;
	RECT			*rect;
	sint32			i;

	
	dest->Flush();

#ifdef __USING_SPANS__




	
	position = m_oldMixDirtyList->GetHeadPosition();
	for (i = m_oldMixDirtyList->L(); i; i-- ) {
		dest->AddRect(m_oldMixDirtyList->GetNext(position));
	}

#else
	
	
	
	position = m_oldMixDirtyList->GetHeadPosition();
	for (i = m_oldMixDirtyList->L(); i; i-- ) {
		dest->AddRect(m_oldMixDirtyList->GetNext(position));
	}
#endif 

	
	m_oldMixDirtyList->Flush();

#ifdef __USING_SPANS__
	m_oldMixDirtyList->SetSpans( m_mixDirtyList );

	
	position = m_mixDirtyList->GetHeadPosition();
	for (i = m_mixDirtyList->L(); i; i-- ) {
		RECT *rect = m_mixDirtyList->GetNext(position);		
		dest->AddRect(rect);





m_oldMixDirtyList->AddRect(rect);		

	}
#else
	
	position = m_mixDirtyList->GetHeadPosition();
	for (i = m_mixDirtyList->L(); i; i-- ) {
		rect = m_mixDirtyList->GetNext(position);		
		
		
		
		dest->AddRect(rect);
		
		m_oldMixDirtyList->AddRect(rect);		

	}
#endif 
}






void TiledMap::RestoreMixFromMap(aui_Surface *destSurf)
{
#ifdef __USING_SPANS__
	ListPos			position;
	RECT			*rect;
	sint32			i;

	g_c3ui->TheBlitter()->SpanBlt(
		destSurf,
		0,
		0,
		m_surface,
		m_oldMixDirtyList,
		k_AUI_BLITTER_FLAG_COPY );
#else
	
	ListPos			position;
	RECT			*rect;
	sint32			i;

	m_oldMixDirtyList->Minimize();

	position = m_oldMixDirtyList->GetHeadPosition();

	for (i = m_oldMixDirtyList->L(); i; i-- ) {
		rect = m_oldMixDirtyList->GetNext(position);



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
	
	ListPos			position;
	RECT			*rect;
	sint32			i;

	position = m_oldMixDirtyList->GetHeadPosition();

	for (i = m_oldMixDirtyList->L(); i; i-- ) {
		rect = m_oldMixDirtyList->GetNext(position);

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
	ListPos			position;
	RECT			*rect;
	sint32			i;

	position = m_mapDirtyList->GetHeadPosition();

	for (i = m_mapDirtyList->L(); i; i-- ) {
		
		rect = m_mapDirtyList->GetNext(position);

		
		m_mixDirtyList->AddRect(rect);
	}

	g_c3ui->TheBlitter()->SpanBlt(
		mixSurf,
		0,
		0,
		m_surface,
		m_mapDirtyList,
		k_AUI_BLITTER_FLAG_COPY );

	m_mapDirtyList->Flush();
#else
	ListPos			position;
	RECT			*rect;
	sint32			i;

	if(m_mapDirtyList->L() > 500) {
		
		InvalidateMap();
	} else {
		m_mapDirtyList->Minimize();
	}

	position = m_mapDirtyList->GetHeadPosition();

	for (i = m_mapDirtyList->L(); i; i-- ) {
		
		rect = m_mapDirtyList->GetNext(position);


		
		
		g_c3ui->TheBlitter()->Blt(mixSurf, rect->left, rect->top, m_surface, rect, k_AUI_BLITTER_FLAG_COPY);
	
		
		m_mixDirtyList->AddRect(rect);
	}

	m_mapDirtyList->Flush();
#endif 
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
	sint16		tc, tn, tne, te, tse, ts, tsw, tw, tnw;
	sint32		i;

	if (!bc) return -1;

	for (i=0; i<m_tileSet->GetNumRiverTransforms(); i++) {

		tn =	m_tileSet->GetRiverTransform((uint16)i, 2);
		tne =	m_tileSet->GetRiverTransform((uint16)i, 5);
		te =	m_tileSet->GetRiverTransform((uint16)i, 6);
		tse =	m_tileSet->GetRiverTransform((uint16)i, 7);
		tc =	m_tileSet->GetRiverTransform((uint16)i, 4);
		ts =	m_tileSet->GetRiverTransform((uint16)i, 6);
		tsw =	m_tileSet->GetRiverTransform((uint16)i, 3);
		tw =	m_tileSet->GetRiverTransform((uint16)i, 0);
		tnw =	m_tileSet->GetRiverTransform((uint16)i, 1);

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

BOOL TiledMap::TryTransforms(MapPoint &pos, uint16 c, uint16 n, uint16 ne, uint16 e, uint16 se, 
							 uint16 s, uint16 sw, uint16 w, uint16 nw, uint16 *newIndex)
{
	sint32		i;

	
	for (i=0; i<m_tileSet->GetNumTransforms(); i++) {
		sint16		tc, tn, tne, te, tse, ts, tsw, tw, tnw;
		sint16		tNew;

		tn = (sint16)m_tileSet->GetTransform((uint16)i,2);
		tne = (sint16)m_tileSet->GetTransform((uint16)i,5);
		te = (sint16)m_tileSet->GetTransform((uint16)i,8);
		tse = (sint16)m_tileSet->GetTransform((uint16)i,7);
		tc = (sint16)m_tileSet->GetTransform((uint16)i,4);
		ts = (sint16)m_tileSet->GetTransform((uint16)i,6);
		tsw = (sint16)m_tileSet->GetTransform((uint16)i,3);
		tw = (sint16)m_tileSet->GetTransform((uint16)i,0);
		tnw = (sint16)m_tileSet->GetTransform((uint16)i,1);

		tNew = (sint16)m_tileSet->GetTransform((uint16)i,9);

		if (











			MATCH_GLOB(tc,c) &&
			MATCH_GLOB(tn, n) &&
			MATCH_GLOB(tne, ne) &&
			MATCH_GLOB(te, e) &&
			MATCH_GLOB(tse, se) &&
			MATCH_GLOB(ts, s) &&
			MATCH_GLOB(tsw, sw) &&
			MATCH_GLOB(tw, w) &&
			MATCH_GLOB(tnw, nw))
			
		{
			if (tNew == k_TRANSFORM_TO_LIST_ID) {
				
				sint16		xform;
				BOOL		legal = FALSE;
				sint32		j;
				
				
				for (j=0; j<k_MAX_TRANSFORM_TO_LIST; j++) {
					xform = (sint16)m_tileSet->GetTransform((uint16)i, (uint16)(k_TRANSFORM_TO_LIST_FIRST + j));
					if (xform != k_TRANSFORM_TO_LIST_ID) legal = TRUE;
				}

				
				if (!legal) return FALSE;

				sint32		which = -2;
				
				TileInfo *ti = GetTileInfo(pos);
				which = ti->GetTransform() % k_MAX_TRANSFORM_TO_LIST;
				do { 
					
					
					
					
					
					
					
					
					
					

					xform = (sint16)m_tileSet->GetTransform((uint16)i, (uint16)(k_TRANSFORM_TO_LIST_FIRST + which));
					which--;
					if(which < 0) {
						which = k_MAX_TRANSFORM_TO_LIST;
					}
					if(which == ti->GetTransform() % k_MAX_TRANSFORM_TO_LIST) {
						return FALSE; 
					}
				} while (xform == k_TRANSFORM_TO_LIST_ID || m_tileSet->GetBaseTile(xform) == NULL);

				*newIndex = xform;
			} else {
				
				*newIndex = tNew;
			}
			return TRUE;
		}

	}

	return FALSE;
}

void TiledMap::TryMegaTiles(MapPoint &pos, BOOL regenTilenum)
{
	uint16		pathPos, pathLen;
	MapPoint	newPos = pos;
	sint32		i,j;
	MapPoint	goodPath[k_MAX_MEGATILE_STEPS];
	uint16		goodPathTiles[k_MAX_MEGATILE_STEPS];
	uint8		goodPathLastDirs[k_MAX_MEGATILE_STEPS];
	uint8		goodPathNextDirs[k_MAX_MEGATILE_STEPS];
	sint32		tilesTried;


	MegaTileStep	step;
	MapPoint	emptyPos(0,0);

	
	if (m_tileSet->GetNumMegaTiles() < 1) 
		return;

	i = rand() % m_tileSet->GetNumMegaTiles();

	tilesTried = 0;
	while (tilesTried < m_tileSet->GetNumMegaTiles()) {

		
		for (j=0; j<k_MAX_MEGATILE_STEPS; j++) {
			goodPath[j] = emptyPos;
			goodPathTiles[j]  = 0;
			goodPathLastDirs[j] = 0;
			goodPathNextDirs[j] = 0;
		}

		pathLen = m_tileSet->GetMegaTileLength(i);
		pathPos = 0;
		step = m_tileSet->GetMegaTileStep(i, pathPos);

		while (pathPos < pathLen) {
			sint32			terrainType;
			TileInfo		*tileInfo;


			tileInfo = GetTileInfo(newPos);
			terrainType = tileInfo->GetTerrainType();
			
			if (tileInfo->GetTileNum() >= k_FIRST_VARIATION) break;

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

		if (pathPos == pathLen) {
			TileInfo	*theTileInfo;

			if(regenTilenum) {
				
				for (j=0; j<pathLen; j++) {
					theTileInfo = g_theWorld->GetTileInfo(goodPath[j]);
					
					theTileInfo->SetTileNum(goodPathTiles[j]);
					
					theTileInfo->SetLastMega(goodPathLastDirs[j]);
					theTileInfo->SetNextMega(goodPathNextDirs[j]);
				}
			}
		}
		
		if (++i >= m_tileSet->GetNumMegaTiles()) i = 0;
		tilesTried++;
	}
}


void TiledMap::PostProcessTile(MapPoint &pos, TileInfo *theTileInfo,
							   BOOL regenTilenum)
{
	uint8			index;
	sint32			goodIndex;
	sint32			goodSpriteID;

	if (theTileInfo->HasGoodActor())
		theTileInfo->DeleteGoodActor();

	
	
	if (g_theWorld->IsGood(pos)) {
		
		
		MBCHAR name[_MAX_PATH];
		g_theWorld->GetCell(pos)->GetGoodsIndex(goodIndex);
		strcpy(name, g_theTerrainDB->Get(g_theWorld->GetTerrainType(pos))->GetResources(goodIndex)->GetIDText());

		
		
		
		sint32 goodSpriteIndex = g_theGoodsSpriteStateDB->FindTypeIndex(name);
		Assert(goodSpriteIndex >= 0);
		if (goodSpriteIndex >= 0) {
			goodSpriteID = g_theGoodsSpriteStateDB->GetDefaultVal(goodSpriteIndex);
			if (goodSpriteID >= 0)
				theTileInfo->SetGoodActor(goodSpriteID, pos);
		}
	}

	index = static_cast<uint8>(g_theWorld->GetTerrain(pos.x, pos.y));
	if(regenTilenum)
		theTileInfo->SetTileNum(static_cast<TILEINDEX>(g_theTerrainDB->Get(index)->GetTilesetIndex()));

	theTileInfo->SetTerrainType(index);




	MapPoint	newPos;
	uint16		c, n, ne, e, se, s, sw, w, nw;
	BOOL		rc, rn, rne, re, rse, rs, rsw, rw, rnw;

	c = static_cast<uint16>(g_theTerrainDB->Get(index)->GetTilesetIndex());

	rc = g_theWorld->IsRiver(pos.x, pos.y);

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
	MapPoint		*size;
	TILEINDEX       origTilenum;
	uint8           origMega;

	Assert (g_theWorld != NULL);
	if (g_theWorld == NULL) return;

	size = g_theWorld->GetSize();

	sint32		width = size->x;

	TileInfo		*theTileInfo;

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
	TileInfo		*tileInfo;
	sint32			next, last;
	MapPoint		curPos;

	

	tileInfo = GetTileInfo(pos);

	Assert(tileInfo != NULL);
	if (tileInfo == NULL) return;

	
	tileInfo->SetTileNum((TILEINDEX)g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex());
	PostProcessTile(pos, tileInfo);
	RedrawTile(&pos);

	
	curPos = pos;
	while ((next = tileInfo->GetNextMega())) {
		switch (next) {
		case k_MEGATILE_DIRECTION_N : 
			curPos.GetNeighborPosition(NORTHWEST, curPos); 
			break;
		case k_MEGATILE_DIRECTION_E : 
			curPos.GetNeighborPosition(NORTHEAST, curPos); 
			break;
		case k_MEGATILE_DIRECTION_S : 
			curPos.GetNeighborPosition(SOUTHEAST, curPos); 
			break;
		case k_MEGATILE_DIRECTION_W : 
			curPos.GetNeighborPosition(SOUTHWEST, curPos); 
			break;			
		default: 
			Assert(FALSE);
		}

		tileInfo = GetTileInfo(curPos);
		tileInfo->SetTileNum((TILEINDEX)g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex());
		PostProcessTile(curPos, tileInfo);
		RedrawTile(&curPos);
	}

	
	curPos = pos;
	while ((last = tileInfo->GetLastMega())) {
		switch (last) {
		case k_MEGATILE_DIRECTION_N : 
			curPos.GetNeighborPosition(NORTHWEST, curPos); 
			break;
		case k_MEGATILE_DIRECTION_E : 
			curPos.GetNeighborPosition(NORTHEAST, curPos); 
			break;
		case k_MEGATILE_DIRECTION_S : 
			curPos.GetNeighborPosition(SOUTHEAST, curPos); 
			break;
		case k_MEGATILE_DIRECTION_W : 
			curPos.GetNeighborPosition(SOUTHWEST, curPos); 
			break;			
		default: 
			Assert(FALSE);
		}

		tileInfo = GetTileInfo(curPos);
		tileInfo->SetTileNum((TILEINDEX)g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex());
		PostProcessTile(curPos, tileInfo);
		RedrawTile(&curPos);
	}
}

void TiledMap::TileChanged(MapPoint &pos)
{
	TileInfo	*tileInfo;

	tileInfo = GetTileInfo(pos);

	if (tileInfo != NULL)
		if (tileInfo->IsMega()) BreakMegaTile(pos);
}



void TiledMap::ReloadGoodActors(void)
{
	MapPoint		pos;
	sint16			i,j;
	
	Assert (g_theWorld != NULL);
	if (g_theWorld == NULL) return;

	LOADTYPE		loadType = LOADTYPE_BASIC;
	if (g_theProfileDB->IsGoodAnim())
		loadType = LOADTYPE_FULL;

	TileInfo		*theTileInfo;

	
	for (i=0; i<m_mapBounds.bottom; i++) {
		for (j=0; j<m_mapBounds.right; j++) {

			pos.x = j;
			pos.y = i;

			
			theTileInfo = g_theWorld->GetTileInfoStoragePtr(pos);
			if (theTileInfo) {
				GoodActor	*goodActor;

				
				goodActor = theTileInfo->GetGoodActor();
				if (goodActor) {
					
					if (goodActor->GetLoadType() != loadType) {
						
						if (loadType == LOADTYPE_FULL) {
							
							goodActor->FullLoad();
						} else {
							
							goodActor->DumpFullLoad();
						}
					}
				}
			}
		}
	}
}

void TiledMap::GenerateHitMask(void)
{
	sint32		i;
	uint16		startPos, endPos;
	sint32		startLine, midLine, endLine;

	startLine = k_TILE_PIXEL_HEADROOM;
	midLine = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM) / 2;
	endLine = k_TILE_GRID_HEIGHT-1;

	for (i=0; i<k_TILE_PIXEL_HEADROOM; i++) 
	{
		m_tileHitMask[i].start = 1;
		m_tileHitMask[i].end = 0;
		m_tileHitMask[i].d_start = 1.0;
		m_tileHitMask[i].d_end   = 0.0;
	
	
	}

	startPos = k_TILE_GRID_WIDTH/2 - 1;
	endPos = k_TILE_GRID_WIDTH/2;

	for (i=k_TILE_PIXEL_HEADROOM; i<midLine; i++) 
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

	for (i=midLine; i<=endLine; i++) 
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

	if(ScenarioEditor::DrawRegion()) {
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

	if(g_tileImprovementMode) {
		
		
	}

	if ( g_specialAttackMode ) {
		
	}
}

sint32 TiledMap::RecalculateViewRect(RECT &myRect)
{
	
	CalculateZoomViewRectangle(GetZoomLevel(), myRect);

	
	return(0);
}

sint32 TiledMap::CalculateMetrics(void)
{
	sint32			w = m_displayRect.right - m_displayRect.left;
	sint32			h = m_displayRect.bottom - m_displayRect.top;

	m_mapViewRect.left = 0;
	m_mapViewRect.top = 0;
	m_mapViewRect.right = (sint32)((w-(GetZoomTilePixelWidth()/2))/(GetZoomTilePixelWidth()));
	m_mapViewRect.bottom = (sint32)((h-GetZoomTileHeadroom())/(GetZoomTilePixelHeight()/2)-1);

	return 0;
}









extern sint32 g_is_debug_map_color;
extern uint16 myRGB(sint32 r,  sint32 g, sint32 b);

sint32 TiledMap::CalculateWrap(
			aui_Surface *surface,
			sint32 i,	
			sint32 j	
			)
{
	sint32		x, y;
	
	MapPoint	pos;
	sint32		terrainType;
	sint16		river = -1;
	sint32		fog = 0;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	sint32 k = maputils_TileX2MapX(j,i);

	MapPoint tempPos (k, i);

	Assert(m_localVision != NULL);


	if(
	   (!m_localVision->IsExplored(tempPos) && !g_fog_toggle && !g_god)
	   || !ReadyToDraw()) {
		BlackTile(surface,&tempPos);

		return 0;
	}


	if(!m_localVision->IsVisible(tempPos) && !g_fog_toggle && !g_god) {
		UnseenCellCarton ucell;

		
		if(m_localVision->GetLastSeen(tempPos, ucell)) {
			
			terrainType = ucell.m_unseenCell->GetTerrainType();
		} else {
			
			terrainType = g_theWorld->GetTerrain(tempPos.x,tempPos.y);
		}

		
		fog = 1;
	} else {
		
		
		terrainType = g_theWorld->GetTerrain(tempPos.x, tempPos.y);
	}
	
	pos = tempPos;

	
	maputils_MapXY2PixelXY(pos.x,pos.y,&x,&y);
	
	
	if(x < m_surfaceRect.left || x > (m_surfaceRect.right - GetZoomTilePixelWidth()) ||
		y < m_surfaceRect.top || y > (m_surfaceRect.bottom - (GetZoomTilePixelHeight() + GetZoomTileHeadroom()))) {
		return 0;
	}

	
	TileInfo *tileInfo = GetTileInfo(pos);
	if (tileInfo == NULL) return -1;
	
	
	river = tileInfo->GetRiverPiece();
	
	
	BaseTile *baseTile = m_tileSet->GetBaseTile(tileInfo->GetTileNum());
	if (baseTile == NULL) return -1;
	

	
	if (m_zoomLevel == k_ZOOM_LARGEST) 
	{
		
		if (!fog) {
			

			
			DrawTransitionTile(surface, pos, x, y);

			
			if (river != -1)
				DrawOverlay(surface, m_tileSet->GetRiverData(river), x, y);
		} else {
			
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
			DrawTileBorder(surface, x,y,0xffff);



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

	
	if (g_graphicsOptions) {
		if (g_graphicsOptions->IsCellTextOn()) {
			CellText *cellText = g_graphicsOptions->GetCellText(pos);
			if (cellText != NULL) {
				COLORREF bgColor = RGB(0,0,0);
				
				uint8 col = cellText->m_color;
				sint32 r,g,b;
				
				ColorMagnitudeToRGB(col, &r, &g, &b);

				COLORREF fgColor = RGB(r, g, b);
				
				DrawSomeText(FALSE, 
								cellText->m_text, 
								x + GetZoomTilePixelWidth()/2, 
								y + GetZoomTilePixelHeight(),
								bgColor, 
								fgColor);	
			}
		}
	}

#ifdef _DEBUG
	
    if (g_is_debug_map_color) { 

        sint32 color = g_theWorld->GetColor(pos); 
        if (0 < color) { 

            
            
            
	        

            
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
            DrawNumber(surface, color, 
	             c, 
				(sint32)(x+(k_TILE_PIXEL_WIDTH*m_scale)/2), 
				(sint32)(y+(k_TILE_PIXEL_HEIGHT*m_scale)));
        }
        









       

    }
#endif

	return 0;
}






sint32 
TiledMap::CalculateWrapClipped(
			aui_Surface *surface,
			sint32 i,	
			sint32 j	
			)
{
	MapPoint	pos;
	sint32		drawx,drawy;
	sint32		terrainType;
	sint16		river = -1;
	sint32		fog = 0;
	sint32      baseX,baseY;

	
	drawx = j;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	drawy = i;

	maputils_TileX2MapXAbs(drawx,drawy,&drawx);

	
	sint32 k = maputils_TileX2MapX(j,i);

	MapPoint tempPos (k, i);

	Assert(m_localVision != NULL);


	if(
	   (!m_localVision->IsExplored(tempPos) && !g_fog_toggle && !g_god)
	   || !ReadyToDraw()) {
		return 0;
	}


	if(!m_localVision->IsVisible(tempPos) && !g_fog_toggle && !g_god) 
	{
		UnseenCellCarton ucell;

		
		if(m_localVision->GetLastSeen(tempPos, ucell)) 
			
			terrainType = ucell.m_unseenCell->GetTerrainType();
		else 
			
			terrainType = g_theWorld->GetTerrain(tempPos.x,tempPos.y);

		
		fog = 1;
	} 
	else 
	{
		
		
		terrainType = g_theWorld->GetTerrain(tempPos.x, tempPos.y);
	}
	
	pos = tempPos;

	
	maputils_MapXY2PixelXY(drawx,drawy,&drawx,&drawy);
	
	maputils_TileX2MapXAbs(m_mapViewRect.left,m_mapViewRect.top,&baseX);

	maputils_MapXY2PixelXY(baseX,m_mapViewRect.top,&baseX,&baseY);

	
	
	
	
	

	
	TileInfo *tileInfo = GetTileInfo(pos);
	
	if (tileInfo == NULL) 
		return -1;
	
	
	river = tileInfo->GetRiverPiece();
	
	
	BaseTile *baseTile = m_tileSet->GetBaseTile(tileInfo->GetTileNum());
	
	if (baseTile == NULL) 
		return -1;
	
	
	if (m_zoomLevel == k_ZOOM_LARGEST) 
	{
		
		if (!fog) 
		{
			

			
		   	DrawTransitionTileClipped(surface, pos, drawx, drawy);

			
		   
		   
		} 
		else 
		{
			
			if (g_isFastCpu) 
			{
				
			 
			
			
			
			} 
			else 
			{
				
			  

			  
			  
			}
		}

		
	   
	   

		
		AddDirtyToMap(drawx, drawy, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);
	} 
	else 
	{ 
		
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
	sint32		x, y;

	MapPoint	pos;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	sint32 k = maputils_TileX2MapX(j,i);

	MapPoint tempPos (k, i);

	pos = tempPos;
	
	
	

	UnseenCellCarton	ucell;
	Cell				*cell;
	uint32				env;
	sint32				numImprovements,numDBImprovements;
	BOOL				hasGoody = FALSE;

	bool visiblePlayerOwnsThis = g_selected_item->GetVisiblePlayer() == g_theWorld->GetOwner(pos);

// Added by Martin Gühmann
	if(!g_fog_toggle // The sense of toogling off the fog is to see something
	&& !visiblePlayerOwnsThis
	&& m_localVision->GetLastSeen(pos, ucell) 
//	&& ucell.m_unseenCell->GetImprovements()->GetCount() > 0
	){
		env = ucell.m_unseenCell->GetEnv();
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

	maputils_MapXY2PixelXY(pos.x,pos.y,&x,&y);
	
	
	if(x < m_surfaceRect.left || x > (m_surfaceRect.right - GetZoomTilePixelWidth()) ||
		y < m_surfaceRect.top || y > (m_surfaceRect.bottom - GetZoomTileGridHeight())) {
		return 0;
	}
	
	
	if (m_zoomLevel == k_ZOOM_LARGEST) 
	{
		
		if(DrawImprovementsLayer(surface, pos, x, y,clip)) {
			AddDirtyToMap(x, y, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);
		}
	} 
	else 
	{
		if(DrawImprovementsLayer(surface, pos, x, y,clip)) {

			
			AddDirtyToMap(x, y, GetZoomTilePixelWidth(), GetZoomTileGridHeight());
		}
	}
		
	return 0;
}









void TiledMap::RetargetTileSurface(aui_Surface *surf)
{
	m_surface = (surf) ? surf : m_mapSurface;
}







sint32 TiledMap::RepaintTiles(RECT *repaintRect)
{

	sint32		mapWidth, mapHeight;

	GetMapMetrics(&mapWidth,&mapHeight);

	for (sint32 i=repaintRect->top; i<repaintRect->bottom; i++){ 
		if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
			for (sint32 j=repaintRect->left; j<repaintRect->right; j++) {
				if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
					CalculateWrap(NULL,i,j);
				}
			}
		}
	}

	return 0;
}







sint32 
TiledMap::RepaintTilesClipped(RECT *repaintRect)
{
	sint32		mapWidth, mapHeight;

	static   unsigned  which_color=0;

	which_color ++;

	if( which_color & 1)
		tcolor = 0xF000;
	else
		tcolor = 0x0F00;

	GetMapMetrics(&mapWidth,&mapHeight);

	for (sint32 i=repaintRect->top; i<=repaintRect->bottom; i++)
	{ 
		for (sint32 j=repaintRect->left; j<=repaintRect->right; j++) 
			CalculateWrapClipped(NULL,i,j);
	}

	return 0;
}






sint32 TiledMap::RepaintImprovements(RECT *repaintRect,bool clip)
{
	sint32		mapWidth, mapHeight;
	sint32		i;

	GetMapMetrics(&mapWidth,&mapHeight);

	for (i=repaintRect->top; i<repaintRect->bottom; i++){ 
		if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
			for (sint32 j=repaintRect->left; j<repaintRect->right; j++) {
				if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
					DrawImprovements(NULL,i,j,0);
				}
			}
		}
	}

	return 0;
}

sint32 TiledMap::CalculateHatWrap(
			aui_Surface *surface,
			sint32 i,	
			sint32 j	
			)
{
	sint32		x, y;

	sint32		fog = 0;

	
	maputils_WrapPoint(i,j,&i,&j);

	
	sint32 mapX = maputils_TileX2MapX(i,j);

	


	MapPoint pos(mapX, j);

	TileInfo *tileInfo;



	if(!m_localVision->IsVisible(pos) && !g_fog_toggle && !g_god)
	{
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell)) {
			tileInfo = ucell.m_unseenCell->GetTileInfo();
		} else {
			tileInfo = GetTileInfo(pos);
		}
		fog = 1;
	} else {
		tileInfo = GetTileInfo(pos);
	}


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



sint32 TiledMap::RepaintHats(RECT *repaintRect,bool clip)
{

	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth, &mapHeight);
	
	RECT tempRect = *repaintRect;

	for (sint32 j = tempRect.top;j < tempRect.bottom;j++) 
	{
		if (g_theWorld->IsYwrap() || ((j >= 0) && (j <= mapHeight)) || clip) 
		{
			for (sint32 i = tempRect.left;i<=tempRect.right;i++) 
			{
				if (g_theWorld->IsXwrap() || (i >= 0 && i < mapWidth)||clip)
					RedrawHat(NULL, j,i,clip);
			}
		}
	}
	
	return 0;
}

sint32 TiledMap::RepaintBorders(RECT *repaintRect, bool clip)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth, &mapHeight);
	
	RECT tempRect = *repaintRect;

	for (sint32 j = tempRect.top;j < tempRect.bottom;j++) 
	{
		if (g_theWorld->IsYwrap() || ((j >= 0) && (j <= mapHeight)) || clip) 
		{
			for (sint32 i = tempRect.left;i<=tempRect.right;i++) 
			{
				if (g_theWorld->IsXwrap() || (i >= 0 && i < mapWidth)||clip)
					RedrawBorders(NULL, j,i,clip);
			}
		}
	}
	
	return 0;
}







sint32 TiledMap::RepaintEdgeX(RECT *repaintRect)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);

	if (repaintRect->left < 0) {
		
		RECT erase = {0,0,(sint32)((k_TILE_PIXEL_WIDTH+k_TILE_PIXEL_WIDTH/2)*m_scale),m_surface->Height()};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		
		for (sint32 i=m_mapViewRect.top; i<m_mapViewRect.bottom; i++){ 
			if (g_theWorld->IsYwrap() || (i>=0 && i < mapHeight)) {
				for (sint32 j=0; j<1; j++) {
					if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}
	if (repaintRect->right > m_mapBounds.right-1) {
		
		RECT erase = {m_surface->Width() - (sint32)((repaintRect->right-(m_mapBounds.right-2))*k_TILE_PIXEL_WIDTH*m_scale),0,m_surface->Width(),m_surface->Height()};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		
		for (sint32 i=m_mapViewRect.top; i<m_mapViewRect.bottom; i++){ 
			if (g_theWorld->IsYwrap() || (i>=0 && i < mapHeight)) {
				for (sint32 j=m_mapBounds.right-2; j<m_mapBounds.right; j++) {
					if (g_theWorld->IsXwrap() || (j >=0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}
	return 0;
}







sint32 TiledMap::RepaintEdgeY(RECT *repaintRect)
{
	sint32 mapWidth, mapHeight;
	GetMapMetrics(&mapWidth,&mapHeight);

	if (repaintRect->top < 0) {
		
		RECT erase = {0,0,m_surface->Width(),(sint32)(k_TILE_PIXEL_HEIGHT*2*m_scale)};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		
		for (sint32 i=0; i<1; i++) { 
			if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
				for (sint32 j=m_mapViewRect.left; j<m_mapViewRect.right; j++) {
					if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}
	if (repaintRect->bottom > m_mapBounds.bottom) {
		
		RECT erase = {0,
						m_surface->Height()-(sint32)((k_TILE_PIXEL_HEIGHT*2)*m_scale),
						m_surface->Width(),
						m_surface->Height()};
		primitives_PaintRect16(m_surface,&erase,0x0000);

		
		for (sint32 i=m_mapBounds.bottom-3; i<m_mapBounds.bottom; i++) { 
			if (g_theWorld->IsYwrap() || (i >= 0 && i < mapHeight)) {
				for (sint32 j=m_mapViewRect.left; j<m_mapViewRect.right; j++) {
					if (g_theWorld->IsXwrap() || (j >= 0 && j < mapWidth)) {
						CalculateWrap(NULL,i,j);
					}
				}
			}
		}
	}
	return 0;
}



void TiledMap::ColorMagnitudeToRGB(uint8 col, sint32 *r, sint32 *g, sint32 *b)
{
	if (col < 128) {
		*r = 255 - (col * 2);
		*g = (col * 2);
		*b = 0;
	} else {
		if (col >= 128) {
			*r = 0;
			*g = 255 - ((col-128) * 2);
			*b = (col - 128) * 2;
		}
	}
}


void TiledMap::DrawSomeText(BOOL mixingPort,
							MBCHAR *text, 
							sint32 tx, sint32 ty, 
							COLORREF bgColorRef, 
							COLORREF fgColorRef )
{
	aui_Surface		*surface;

	if (text == NULL) return;
	if (strlen(text) < 1) return;
	if (m_font == NULL) return;

	if (mixingPort) {
		if (!g_screenManager) return;

		surface = g_screenManager->GetSurface();
		if (!surface) return;
	} else {
		surface = m_mapSurface;
	}

	sint32 width, height;

	width = m_font->GetStringWidth(text);
	height = m_font->GetMaxHeight();

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

void TiledMap::PaintUnitActor(UnitActor *actor, BOOL fog)
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

		if (g_graphicsOptions) {
			if (g_graphicsOptions->IsArmyTextOn()) {
				Unit	u = actor->GetUnitID();
				
				if (g_theUnitPool->IsValid(u) && u.GetArmy().m_id != 0) {
					Army		a = u.GetArmy();

					MBCHAR		*s = a.GetData()->GetDebugString();

					sint32		tx = (sint32)(actor->GetX()+GetZoomTilePixelWidth()/2),
								ty = (sint32)(actor->GetY()+GetZoomTileHeadroom());

					sint32		r,g,b;
					uint8		col = a.GetData()->GetDebugStringColor();

					ColorMagnitudeToRGB(col, &r, &g, &b);

					COLORREF	fgColor = RGB(r, g, b),
								bgColor = RGB(0,0,0);

					DrawSomeText(TRUE, s, tx, ty, fgColor, bgColor);
				}
			}
		}

		
		
		if (g_show_ai_dbg || (g_isScenario && g_showUnitLabels) )
		{
			MapPoint pos = actor->GetPos();

			char text[80]; 
			text[0] = '\0';
			
			sint32	tx = (sint32)(actor->GetX()+(k_TILE_PIXEL_WIDTH*m_scale)/2),
					ty = (sint32)(actor->GetY()+(k_TILE_PIXEL_HEIGHT*m_scale));

			Cell *c = g_theWorld->GetCell(pos); 
			Unit city = c->GetCity(); 

			
			if ((city != Unit(0)) && g_show_ai_dbg)
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

void TiledMap::PaintGoodActor(GoodActor *actor, BOOL fog)
{
	Assert(actor != NULL);
	if (actor == NULL) return;

	if (actor->Draw(fog)) 
	{
		
		RECT rect;
		actor->GetBoundingRect(&rect);
		AddDirtyRectToMix(rect);
	} else {
		
		RECT rect;

		actor->GetBoundingRect(&rect);
		AddDirtyRectToMix(rect);
	}
}
















void TiledMap::PaintEffectActor(EffectActor *actor)
{
	Assert(actor != NULL);
	if (actor == NULL) return;

	
	actor->Draw();

	
	RECT rect;
	actor->GetBoundingRect(&rect);
	AddDirtyRectToMix(rect);
}

sint32 TiledMap::RepaintLayerSprites(RECT *paintRect, sint32 layer)
{
	BOOL			fog;

	if(!ReadyToDraw())
		return 0;

#ifdef _DEBUG
	g_tiledMap->DrawRectMetrics();
	g_tiledMap->RectMetricNewLoop();
#endif
	
	for (sint32 i=paintRect->top; i<paintRect->bottom; i++) {
		for (sint32 j=paintRect->left; j<paintRect->right; j++) {
			
			if(!g_theWorld->IsXwrap() && (j < 0 || j >= g_theWorld->GetXWidth()))
				continue;

			sint32 tileX,tileY;
			maputils_WrapPoint(j,i,&tileX,&tileY);
			
			sint32 mapX, mapY = tileY;
			mapX = maputils_TileX2MapX(tileX,tileY);
			
			MapPoint pos;
			
			pos.x = (sint16)mapX; pos.y = (sint16)mapY;
  
#if 0
			if(g_theWorld->IsCity(pos))
			{
				Unit city=g_theWorld->GetCity(pos);
									
				
				sint32 pop;
				
				city.GetPop(pop);
				DrawCityRadius(pos, COLOR_WHITE ,pop);
			}
#endif

			
			if(g_theWorld->IsGood(pos) && m_localVision->IsExplored(pos)) 
			{
				GoodActor *curGoodActor;
				TileInfo *curTileInfo = GetTileInfo(pos);
				Assert(curTileInfo != NULL);
				if(curTileInfo && curTileInfo->HasGoodActor())
				{
					curGoodActor = curTileInfo->GetGoodActor();

					if (!m_localVision->IsVisible(pos))
						fog = TRUE;
					else
						fog = FALSE;
					
					curGoodActor->PositionActor(pos);
					PaintGoodActor(curGoodActor,fog);
					
				}
			}

			Unit		top;

			if (m_localVision && !m_localVision->IsExplored(pos)
				&& !g_theWorld->GetTopVisibleUnit(pos, top))
				continue;

			if (m_localVision && m_localVision->IsExplored(pos) && !m_localVision->IsVisible(pos))
				fog = TRUE;
			else
				fog = FALSE;

			UnseenCellCarton		ucell;
			
// Added by Martin Gühmann
			// For visibility god mode and fog of war should be handled equally
			if(!g_fog_toggle
			&&  m_localVision 
			&&  m_localVision->GetLastSeen(pos, ucell))
			{
				
				UnitActor	*actor = ucell.m_unseenCell->GetActor();

				if (actor) 
				{
					PaintUnitActor(actor,fog);						
					
				}
			} 
			else 
			{
				

				
				if (!g_theWorld->GetTopVisibleUnit(pos, top)) continue;

				if (g_drawArmyClumps) {
					if (g_theArmyPool->IsValid(top.GetArmy())) {

						sint32 size = top.GetArmy().Num();
						
						if (size > 1) {
							PaintArmyActors(pos);

							return 0;
						}

					}

				}
				
				UnitActor *actor = top.GetActor();

				
                
				if (g_player[top.GetOwner()] && !Player::IsThisPlayerARobot(top.GetOwner())) { 
                    actor->SetIsFortifying(top.IsEntrenching());
                }
				actor->SetIsFortified(top.IsEntrenched());

				actor->SetHiddenUnderStack(FALSE);
				actor->SetUnitVisibility(top.GetVisibility());

				if (top.IsCity()) 
				{
					CityData *cityData = top.GetData()->GetCityData();

					actor->SetHasCityWalls(cityData->HasCityWalls());
					actor->SetHasForceField(cityData->HasForceField());
				}





// Added by Martin Gühmann
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
					COLOR		color;

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

				
				MapPoint actorCurPos = actor->GetPos();
				if (!actor->IsActive() 

					) {
					
					if (!m_localVision->IsVisible(actor->GetPos()))
						fog = TRUE;
					else
						fog = FALSE;
					

					PaintUnitActor(actor, fog);

				}  else {
					
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
						if (!g_theArmyPool->IsValid(army))
							continue;
						
						
						MapPoint	armyPos;
						army.GetPos(armyPos);

						if (armyPos != top.RetPos())
							continue;

						top = army.Access(0);
					} else {
						top = hypotheticalUnit;
					}

// Added by Martin Gühmann
					// For visibility god mode and fog of war should be handled equally
					if (top.GetOwner() != g_selected_item->GetVisiblePlayer()
					&& !g_fog_toggle
					&& !g_god)
						continue;

					actor = top.GetActor();
					if(!actor) continue; 




					if (!(actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer())))
						continue;

					if (!actor->IsActive() && TileIsVisible(actorCurPos.x, actorCurPos.y)) {


						
						actor->SetHiddenUnderStack(FALSE);
						if (!m_localVision->IsVisible(actor->GetPos()))
							fog = TRUE;
						else
							fog = FALSE;

						PaintUnitActor(actor, fog);

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
  
  UnitActor *actor;

  if ((unit.m_id!=0) && g_theUnitPool->IsValid(unit) && (actor=unit.GetActor()))
  {	
	if(!actor->IsActive())
  		actor->Process();
  }
}

void
TiledMap::ProcessUnit(CellUnitList *list)
{
	
	if(!list)
	  return; 
   
	
	for(sint32 index=0; index < list->Num(); index++) 
		ProcessUnit(list->Get(index));
}


void TiledMap::ProcessLayerSprites(RECT *paintRect, sint32 layer)
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

	for (i=paintRect->top; i<paintRect->bottom; i++) 
	{
		for (j=paintRect->left; j<paintRect->right; j++) 
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

				if ((unit.m_id!=0) && g_theUnitPool->IsValid(unit)) 
				{
					curUnitActor = unit.GetActor();
				
					if (curUnitActor)
						curUnitActor->Process();
				}

				
				list = g_theWorld->GetCell(pos)->UnitArmy();

				if (list) 
				{
					for (index=0; index < list->Num(); index++) 
					{
						unit= list->Get(index);
					   
						if (unit.m_id != 0 && g_theUnitPool->IsValid(unit)) 
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

sint32 TiledMap::OffsetLayerSprites(RECT *paintRect, sint32 deltaX, sint32 deltaY, sint32 layer)
{
	if(!ReadyToDraw())
		return 0;

	for (sint32 i=paintRect->top; i<paintRect->bottom; i++) {
		for (sint32 j=paintRect->left; j<paintRect->right; j++) {
			
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

				MapPoint actorCurPos = actor->GetPos();
				if (!actor->IsActive()) {
					if (actor) {
						actor->SetX(pixelX);
						actor->SetY(pixelY);
					}
				}

				
				if (top.IsCity()) {
					
					if (!g_theWorld->GetTopVisibleUnitNotCity(pos, top)) continue;

					actor = top.GetActor();

					if (!actor->IsActive()) {
						if (actor) {
							actor->SetX(pixelX);
							actor->SetY(pixelY);
						}
					}
				}
			}
		}
	}




































































































	return 0;
}

sint32 TiledMap::OffsetSprites(RECT *paintRect, sint32 deltaX, sint32 deltaY)
{
	OffsetLayerSprites(paintRect, deltaX, deltaY, 0);

	g_director->OffsetActiveUnits(-deltaX, -deltaY);
	g_director->OffsetActiveEffects(-deltaX, -deltaY);
	g_director->OffsetTradeRouteAnimations(-deltaX, -deltaY);


	return 0;
}

sint32 TiledMap::RepaintSprites(aui_Surface *surf, RECT *paintRect, bool scrolling)
{
	if(!ReadyToDraw())
		return 0;

	sint32				mapWidth, mapHeight;

	GetMapMetrics(&mapWidth,&mapHeight);
	
	Assert(m_localVision != NULL);

	if(m_nextPlayer == TRUE)
	{


		m_nextPlayer = FALSE;
	}

	
	g_screenManager->LockSurface(surf);

	
	RepaintLayerSprites(paintRect, 0);
	g_director->DrawTradeRouteAnimations(paintRect, 0);
	g_director->DrawActiveUnits(paintRect, 0);
	g_director->DrawActiveEffects(paintRect, 0);
 
	
	if (g_spriteEditWindow!=NULL)
		g_spriteEditWindow->DrawSprite();
	
	


	

	g_tiledMap->DrawTerrainOverlay(surf);



	if(!scrolling) {
		DrawChatText();
	}

	g_screenManager->UnlockSurface();

	
	if (g_theProfileDB->GetShowCityNames()) {
		
		
		g_tiledMap->DrawCityNames((aui_Surface *)surf, 0);
	}

	
	
	
	if (ScenarioEditor::ShowStartFlags()) {
		g_tiledMap->DrawStartingLocations((aui_Surface *)surf, 0);
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
	Pixel16 pixelColor = g_colorSet->GetColor(color);
	
	if(g_theWorld->GetCell(cpos)->HasCity()) 
	{
		return 0;
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
// Parameters : cpos	: city location on the map
//              color	: color to use when drawing
//
// Globals    : g_screenManager
//
// Returns    : sint32	: useless value, always 0
//
// Remark(s)  : The tile NORTH of the city is not drawn.
//				The tile of the city itself is drawn.
//              TODO: check whether this is intentional, or the original code
//                    was just wrong.
//
//----------------------------------------------------------------------------

sint32 TiledMap::DrawCityRadius1(const MapPoint &cpos, COLOR color)
{
	for (int dir = NORTHEAST; dir <= NOWHERE; ++dir)
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

	Pixel16		pixelColor = g_colorSet->GetColor(color);

	
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
			*destPixel = pixelutils_BlendFast(*destPixel, pixelColor, 20);
			destPixel++;
		}
	}

	AddDirtyToMix(x, y, k_TILE_PIXEL_WIDTH, k_TILE_PIXEL_HEIGHT);

	return 0;
}



sint32 TiledMap::Refresh(void)
{

	
	LPVOID buffer;

	sint32 errcode = m_surface->Lock( NULL, &buffer, 0 );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode == AUI_ERRCODE_OK )
	{
		memset( buffer, 0x00, m_surface->Size() );
		m_surface->Unlock( buffer );
	} else {
		return AUI_ERRCODE_SURFACELOCKFAILED;
	}

	LockSurface();

	if (!SmoothScrollAligned())
	{
		RECT   altrect=m_mapViewRect;
	
		altrect.left   -= 1;
		altrect.top    -= 1;
		altrect.right  += 1;
		altrect.bottom -= 1;
		RepaintTilesClipped(&altrect);
		RepaintHats(&altrect,true);
		RepaintBorders(&altrect, true);
	}
	else
	{
		RepaintTiles(&m_mapViewRect);
		RepaintHats (&m_mapViewRect);
		RepaintBorders(&m_mapViewRect);
	}
	
	RepaintImprovements(&m_mapViewRect);

	UnlockSurface();

	return 0;
}







void TiledMap::ScrollPixels(sint32 deltaX, sint32 deltaY, aui_Surface *surf)
{
	sint32		errcode;
	char		*buffer;

	errcode = surf->Lock(NULL, (LPVOID *)&buffer, 0);
	
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

#ifdef WIN32
			_ASSERTE((unsigned)srcPtr >=(unsigned)buffer);
			_ASSERTE((unsigned)destPtr>=(unsigned)buffer);
#endif

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

#define kMV_LeftMin (-1)
#define kMV_RightMax 2
#define kMV_TopMin (-4)
#define kMV_BottomMax 12







BOOL TiledMap::ScrollMap(sint32 deltaX, sint32 deltaY)
{
	if (g_modalWindow) 
		return FALSE;

	RECT	repaintRect;
	RECT	oldMapViewRect;

	
	
	
	RetargetTileSurface(g_background->TheSurface());

	sint32		mapWidth = g_theWorld->GetWidth();
	sint32		mapHeight = g_theWorld->GetHeight();
	sint32		hscroll = GetZoomTilePixelWidth();
	sint32		vscroll = GetZoomTilePixelHeight()/2;

	
	
	

	if (!g_theWorld->IsXwrap())
	{
		if ((deltaX < 0) && (m_mapViewRect.left + deltaX < kMV_LeftMin ))
		{
			deltaX = kMV_LeftMin - m_mapViewRect.left;
			if (deltaX > 0) deltaX = 0;
		}
		if ((deltaX > 0) && (m_mapViewRect.right + deltaX > m_mapBounds.right + kMV_RightMax))
		{
			deltaX = m_mapBounds.right + kMV_RightMax - m_mapViewRect.right;
			if (deltaX < 0) deltaX = 0;
		}
	}

	if (!g_theWorld->IsYwrap())
	{
		if ((deltaY < 0) && (m_mapViewRect.top + deltaY < kMV_TopMin))
		{
			deltaY = kMV_TopMin - m_mapViewRect.top;
			if (deltaY > 0) deltaY = 0;
		}

		if ((deltaY > 0) && (m_mapViewRect.bottom + deltaY > m_mapBounds.bottom + kMV_BottomMax)) 
		{
			deltaY = m_mapBounds.bottom + kMV_BottomMax - m_mapViewRect.bottom;
			if (deltaY  < 0) deltaY = 0;
		}
	}

	oldMapViewRect = m_mapViewRect;

	OffsetRect(&m_mapViewRect, deltaX, deltaY);

	SubtractRect(&repaintRect, &m_mapViewRect, &oldMapViewRect);

	OffsetMixDirtyRects(deltaX, deltaY);

	
	if (m_mapViewRect.right <= 0)
	{
		m_mapViewRect.left += mapWidth;
		m_mapViewRect.right += mapWidth;
		Assert(m_mapViewRect.right > 0);
	}

	if (m_mapViewRect.left >= mapWidth)
	{
		m_mapViewRect.left -= mapWidth;
		m_mapViewRect.right -= mapWidth;
		Assert(m_mapViewRect.left < mapWidth);
	}
	if (m_mapViewRect.bottom <= 0)
	{
		m_mapViewRect.top += mapHeight;
		m_mapViewRect.bottom += mapHeight;
		Assert(m_mapViewRect.bottom > 0);
	}
	if (m_mapViewRect.top >= mapHeight)
	{
		m_mapViewRect.top -= mapHeight;
		m_mapViewRect.bottom -= mapHeight;
		Assert(m_mapViewRect.top < mapHeight);
	}

	RECT tempRect = repaintRect;

	if (deltaX == 1) {
		tempRect.left -= 1;
		tempRect.top += 1;
		tempRect.bottom += 1;
	}
	else if (deltaX == -1) {
		tempRect.right += 1;
		tempRect.top += 1;
		tempRect.bottom += 1;
	}

	if (deltaY == 1) {
		tempRect.right += 1;
		tempRect.top -= 2;
	}
	else if (deltaY == -1) {
		tempRect.right += 1;
		tempRect.bottom += 2;

	}

	OffsetSprites(&tempRect, deltaX*hscroll, deltaY*vscroll);
	ScrollPixels((sint32)(deltaX*hscroll), (sint32)(deltaY*vscroll), m_surface);	





	
	LockSurface();

	RepaintTiles(&repaintRect);

	if (!g_theWorld->IsXwrap())
		if (m_mapViewRect.left + deltaX < 0 ||
			m_mapViewRect.right + deltaX > m_mapBounds.right-1) 
			RepaintEdgeX(&repaintRect);

	if (!g_theWorld->IsYwrap())
		if (m_mapViewRect.top + deltaY < 0 ||
			m_mapViewRect.bottom + deltaY > m_mapBounds.bottom-2)
			RepaintEdgeY(&repaintRect);


	RepaintHats(&tempRect);
	RepaintBorders(&tempRect);

	RepaintImprovements(&tempRect);

	
	UnlockSurface();


	m_mapDirtyList->Flush();

	
	InvalidateMix();



	RepaintSprites(m_surface, &tempRect, true);

	return TRUE;
}





bool 
TiledMap::SmoothScrollAligned()
{
	return ((!m_smoothOffsetX) && (!m_smoothOffsetY));
}




#ifdef IANSCROLL






BOOL TiledMap::ScrollMapSmooth(sint32 pdeltaX, sint32 pdeltaY)
{
	
	if (g_modalWindow) 
		return FALSE;

	
	m_smoothOffsetX += pdeltaX;
	m_smoothOffsetY	+= pdeltaY;

	
	sint32		hscroll		= GetZoomTilePixelWidth();
	sint32		vscroll		= GetZoomTilePixelHeight()>>1;

	
	sint32 deltaX =	(m_smoothOffsetX/hscroll);
	sint32 deltaY =	(m_smoothOffsetY/vscroll);

	
	if (deltaX != 0)
	{
		
		m_smoothOffsetX -= deltaX * hscroll;

		

	}

	
	if (deltaY != 0)
	{
		
		m_smoothOffsetY -= deltaY * vscroll;

		

	}

	
	OffsetRect(&m_mapViewRect , deltaX, deltaY);

	OffsetMixDirtyRects(deltaX,deltaY);

	
	
	RetargetTileSurface(g_background->TheSurface());

	
	ScrollPixels((sint32)(pdeltaX), (sint32)(pdeltaY), m_surface);
	
	
	LockSurface();

	RECT repaintRect = m_mapViewRect;

	
	repaintRect.top--;
	repaintRect.left--;
	repaintRect.bottom++;
	repaintRect.right++;



	OffsetSprites(&repaintRect, pdeltaX, pdeltaY);
	RepaintTilesClipped (&repaintRect);
	RepaintImprovements (&repaintRect);
	RepaintHats			(&repaintRect,true);
	RepaintBorders      (&repaintRect, true);

	
	UnlockSurface();

	
	InvalidateMix();

	
 	RepaintSprites(m_surface, &repaintRect, true);

	return TRUE;
}

#else







BOOL TiledMap::ScrollMapSmooth(sint32 pdeltaX, sint32 pdeltaY)
{
	
	if (g_modalWindow) 
		return FALSE;

	



	
	m_smoothOffsetX += pdeltaX;
	m_smoothOffsetY	+= pdeltaY;

	
	sint32 signX  = (!pdeltaX ? 0 :(pdeltaX<0? -1:1));
	sint32 signY  = (!pdeltaY ? 0 :(pdeltaY<0? -1:1));

	
	
	RetargetTileSurface(g_background->TheSurface());

	sint32		mapWidth	= g_theWorld->GetWidth();
	sint32		mapHeight	= g_theWorld->GetHeight();
	sint32		hscroll		= GetZoomTilePixelWidth();
	sint32		vscroll		= GetZoomTilePixelHeight()>>1;

	
	sint32 deltaX =	(m_smoothOffsetX/hscroll);
	sint32 deltaY =	(m_smoothOffsetY/vscroll);

	
	
	if (deltaX != 0)
	{
		
		m_smoothOffsetX -= deltaX * hscroll;
	}

	
	if (deltaY != 0)
	{
		
		m_smoothOffsetY -= deltaY * vscroll;
	}


	
	m_smoothLastX =	pdeltaX;
	m_smoothLastY =	pdeltaY;

	
   	if ((!g_theWorld->IsXwrap())&&signX)
   	{
   		if ((m_mapViewRect.left + signX) <= 0) 
		{
			m_smoothOffsetX=0;
			m_mapViewRect.top = 0;
			return FALSE;
		}
   		
		if ((m_mapViewRect.right + signX) >= (m_mapBounds.right)) 
		{
			m_smoothOffsetX=0;
			m_mapViewRect.top = 0;
			return FALSE;
		}
   	}

	
   	if ((!g_theWorld->IsYwrap())&&(signY))
   	{
   		if ((m_mapViewRect.top+signY) < 0) 
		{
			m_mapViewRect.top = 0;
			m_smoothOffsetY=0;
			return FALSE;
		}
		if ((m_mapViewRect.bottom+signY) >= (m_mapBounds.bottom)) 
		{
			m_smoothOffsetY=0;
			m_mapViewRect.bottom = m_mapBounds.bottom;
			return FALSE;
		}
   	}

	RECT oldMapViewRect = m_mapViewRect;
	RECT repaintRect;

	OffsetRect(&m_mapViewRect, deltaX, deltaY);

	SubtractRect(&repaintRect, &m_mapViewRect, &oldMapViewRect);

	OffsetMixDirtyRects(deltaX, deltaY);

	
	if (m_mapViewRect.right <= 0)
	{
		m_mapViewRect.left += mapWidth;
		m_mapViewRect.right += mapWidth;
		Assert(m_mapViewRect.right > 0);
	}

	if (m_mapViewRect.left >= mapWidth)
	{
		m_mapViewRect.left -= mapWidth;
		m_mapViewRect.right -= mapWidth;
		Assert(m_mapViewRect.left < mapWidth);
	}
	if (m_mapViewRect.bottom <= 0)
	{
		m_mapViewRect.top += mapHeight;
		m_mapViewRect.bottom += mapHeight;
		Assert(m_mapViewRect.bottom > 0);
	}
	if (m_mapViewRect.top >= mapHeight)
	{
		m_mapViewRect.top -= mapHeight;
		m_mapViewRect.bottom -= mapHeight;
		Assert(m_mapViewRect.top < mapHeight);
	}

	RECT tempRect = repaintRect;

	if (signX == 1) {
		tempRect.left -= 1;
		tempRect.top += 1;
		tempRect.bottom += 1;
	}
	else if (signX == -1) {
		tempRect.right += 1;
		tempRect.top += 1;
		tempRect.bottom += 1;
	}

	if (signY == 1) {
		tempRect.right += 1;
		tempRect.top -= 2;
	}
	else if (signY == -1) {
		tempRect.right += 1;
		tempRect.bottom += 2;

	}


	OffsetSprites(&tempRect, pdeltaX, pdeltaY);

	ScrollPixels((sint32)(pdeltaX), (sint32)(pdeltaY), m_surface);	




	
	LockSurface();

	RepaintTiles(&repaintRect);

	if (!g_theWorld->IsXwrap())
		if (m_mapViewRect.left + deltaX < 0 ||
			m_mapViewRect.right + deltaX > m_mapBounds.right-1) 
			RepaintEdgeX(&repaintRect);

	if (!g_theWorld->IsYwrap())
		if (m_mapViewRect.top + deltaY < 0 ||
			m_mapViewRect.bottom + deltaY > m_mapBounds.bottom-2)
			RepaintEdgeY(&repaintRect);


	RepaintHats(&tempRect);
	RepaintBorders(&tempRect);

	RepaintImprovements(&tempRect);

	
	UnlockSurface();


	
	InvalidateMix();



	RepaintSprites(m_surface, &tempRect, true);

	return TRUE;
}






















































































































































































































































#endif







sint32 TiledMap::RedrawHat(
			aui_Surface *surface,
			sint32 i,	
			sint32 j,	
			bool clip  
			)
{
	MapPoint	pos;
	sint32		terrainType;
	sint32		fog = 0;
	sint32		drawx,drawy;
	
	
	drawx = j;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	drawy = i;

	maputils_TileX2MapXAbs(drawx,drawy,&drawx);

	
	sint32 k = maputils_TileX2MapX(j,i);

	
	if (!TileIsVisible(k,i)) 
		return 0;

	MapPoint tempPos (k, i);



	if(!ReadyToDraw())
		return 0;

	if(!m_localVision->IsExplored(tempPos) && !g_fog_toggle && !g_god) {
		return 0;
	}

	TileInfo		*tileInfo;


	if(!m_localVision->IsVisible(tempPos) && !g_fog_toggle && !g_god) {
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
		fog = 1;
	} 
	else 
	{
		terrainType = g_theWorld->GetTerrain(tempPos.x, tempPos.y);
		tileInfo = GetTileInfo(tempPos);
	}

	pos = tempPos;

	
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
		if(m_localVision->IsExplored(nw) || g_fog_toggle || g_god) {
			DrawNationalBorders(surface, nw);
		}
	}

	if(tempPos.GetNeighborPosition(NORTHEAST, ne)) {
		if(m_localVision->IsExplored(ne) || g_fog_toggle || g_god) {
			DrawNationalBorders(surface, ne);
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
	MapPoint	pos;
	sint32		fog = 0;

	
	maputils_WrapPoint(j,i,&j,&i);

	
	sint32 k = maputils_TileX2MapX(j,i);

	
	if (!TileIsVisible(k,i)) 
		return 0;

	MapPoint tempPos (k, i);

	if(!ReadyToDraw())
		return 0;

	if(!m_localVision->IsExplored(tempPos) && !g_fog_toggle && !g_god) {
		return 0;
	}

	DrawNationalBorders(surface, tempPos);

	return 0;
}






void TiledMap::RedrawTile(
		const MapPoint *point		
		)
{
	sint32 tileX;

	
	if(TileIsVisible(point->x,point->y)) {
		LockSurface();

		
		MapPoint pos;
		
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

	g_radarMap->RedrawTile( point );
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


BOOL TiledMap::TileIsVisible(sint32 mapX, sint32 mapY, sint32 mapZ)
{
	if(!TileIsVisible(mapX, mapY))
		return FALSE;

	return TRUE;
}


BOOL TiledMap::TileIsCompletelyVisible(sint32 mapX, sint32 mapY, RECT *viewRect)
{
	RECT	shrunkMapViewRect;

	if (viewRect == NULL)
		shrunkMapViewRect = m_mapViewRect;
	else
		shrunkMapViewRect = *viewRect;

	sint32	shrinkX, shrinkY;

	shrinkX = (sint32)ceil((double)k_TILE_PIXEL_WIDTH / (double)GetZoomTilePixelWidth());
	shrinkY = (sint32)ceil((double)k_TILE_GRID_HEIGHT / (double)GetZoomTileGridHeight());

	
	
	
	
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

	BOOL visible = (PtInRect(&ul,point) || 
					PtInRect(&ll,point) || 
					PtInRect(&ur,point) || 
					PtInRect(&lr,point));

	if (!visible) 
		return FALSE;

	return TRUE;
}


BOOL TiledMap::TileIsVisible(sint32 mapX, sint32 mapY)
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

	return (PtInRect(&ul,point) || PtInRect(&ll,point) ||
			PtInRect(&ur,point) || PtInRect(&lr,point));
}

inline Pixel16 TiledMap::average(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4)
{
	short		r1, g1, b1, 
				r2, g2, b2,
				r3, g3, b3,
				r4, g4, b4;
	short		r0, g0, b0;
	sint32			c=0;

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

	Pixel16			pixel1, pixel2;

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
						pixel1 = pixelutils_Shadow(**rowData1);
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
						pixel2 = pixelutils_Shadow(**rowData2);
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

inline sint32 TiledMap::ReadTag(sint32 *mode, Pixel16 **rowData, sint32 *alpha)
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

UnitActor *TiledMap::GetClickedUnit(aui_MouseEvent *data)
{
	sint32				mapWidth, mapHeight;
	sint32				x, y;
	POINT				point;
	UnitActor			*clickedActor = NULL;

	GetMapMetrics(&mapWidth,&mapHeight);

	point = data->position;
	
	for (sint32 i=m_mapViewRect.top; i<m_mapViewRect.bottom; i++) {
		for (sint32 j=m_mapViewRect.left; j<m_mapViewRect.right; j++) { 
			
			sint32 tileX, tileY;
			maputils_WrapPoint(j,i,&tileX,&tileY);
			
			sint32 mapX, mapY = tileY;
			mapX = maputils_TileX2MapX(tileX,tileY);
			
			maputils_MapXY2PixelXY(mapX,mapY,&x,&y);
			
			MapPoint pos(mapX,mapY);
			
			
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
			
			if (!actor->IsActive()) {
				RECT	actorRect;
				
				SetRect(&actorRect, x, y, x+(sint32)actor->GetWidth(), y+(sint32)actor->GetHeight());
				
				if (PtInRect(&actorRect, point)) {
					return actor;
				}
			}
		}
	}

	if (clickedActor == NULL) {
		return g_director->GetClickedActiveUnit(data);
	}


	return NULL;
}

BOOL TiledMap::PointInMask(POINT hitPt)
{






































	sint32		x, y;



	x = (sint32)((double)hitPt.x / GetZoomScale(GetZoomLevel()));
	y = (sint32)(((double)hitPt.y / GetZoomScale(GetZoomLevel())) + k_TILE_PIXEL_HEADROOM);

	if (x >= m_tileHitMask[y].start &&
		x <= m_tileHitMask[y].end) 
		return TRUE;

	return FALSE;
}

BOOL TiledMap::MousePointToTilePos(POINT point, MapPoint &tilePos)
{
	sint32			width, height;
	MapPoint		pos;
	POINT			hitPt;
	sint32			x, y;
	sint16			maxX;
	sint32			headroom = (sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);




	width = GetZoomTilePixelWidth();
	height = GetZoomTilePixelHeight();

	x = point.x;
	y = point.y;

	
	sint32 xoff,yoff;
	GetSmoothScrollOffsets(xoff,yoff);
  	x += xoff;
	y += yoff;

	

	if (!(m_mapViewRect.top & 1)) y -= GetZoomTileHeadroom();

	pos.x = (sint16)(x / width + m_mapViewRect.left);
	pos.y = (sint16)((y / height) + m_mapViewRect.top/2);
 
 	hitPt.x = x % width;
	hitPt.y = y % height;

	maxX = static_cast<sint16>(m_mapBounds.right);

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

	if(!g_theWorld->IsXwrap()) {
		if(pos.x < 0)
			return FALSE;
		if(pos.x >= g_theWorld->GetXWidth())
			return FALSE;
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
			return FALSE; 
		} else if (g_theWorld->GetHeight() <= tilePos.y) { 
			tilePos.y = static_cast<sint16>(g_theWorld->GetHeight() -1);
			return FALSE; 
		} 
	}

	

    Assert (m_mapBounds.left <= tilePos.x); 
    Assert (tilePos.x < m_mapBounds.right); 
    Assert (m_mapBounds.top <= tilePos.y); 
    Assert (tilePos.y < m_mapBounds.bottom); 

	return TRUE;
}

void TiledMap::HandleCheat(MapPoint &pos)
{
	TileInfo *tileinfo;
	BOOL river = FALSE;
	GoodyHut *hut = NULL;

	


	
	
	
	





	
	extern sint32 g_isStartingPadOn;

	if(ScenarioEditor::PlaceStartFlags()) {
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
		RadiusIterator it(pos, 1);
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
		Player *p = g_player[g_selected_item->GetVisiblePlayer()];
		if (!p) return;
		TerrainImprovement theImprovement(0);
		theImprovement = g_theTerrainImprovementPool->Create(p->m_owner,
																pos,
																ScenarioEditor::PaintTerrainImprovement(),
																0);

		if(g_theTerrainImprovementPool->IsValid(theImprovement.m_id)) {
			p->m_terrainImprovements->Insert(theImprovement);
			theImprovement.Complete();
		}
	}

	
	if (g_placeGoodsMode || ScenarioEditor::PaintTerrainMode()) {
		tileinfo = g_theWorld->GetTileInfo(pos);

		sint32 tileNum = ScenarioEditor::PaintTerrainMode() ? ScenarioEditor::PaintTerrain() : 0;
		if(g_placeGoodsMode) {
			sint32 curGood;
			if(g_theWorld->GetGood(pos, curGood)) {
				g_theWorld->ClearGoods(pos.x, pos.y);
			} else {
				g_theWorld->SetGood(pos.x, pos.y, g_placeGoodsMode);
			}
		} else {
			
			switch (tileNum) {
				case TILEPAD_TYPE_GOODY:
					hut = g_theWorld->GetGoodyHut( pos );

					
					if ( g_theWorld->IsLand(pos) ) {
						if ( !hut ) {
							g_theWorld->GetCell(pos)->CreateGoodyHut();
						} else {
							
							g_theWorld->GetCell(pos)->DeleteGoodyHut();
						}
				
						river = TRUE;
					}
					else {
						return;
					}

					break;

				case TILEPAD_TYPE_RIVER:
					if (g_theWorld->IsRiver(pos)) {
						g_theWorld->UnsetRiver(pos.x, pos.y);
					} else {
						g_theWorld->SetRiver( pos );
					}
					river = TRUE;
					break;

				default:
					BOOL TerrainNotHandled = FALSE;
					
					break;

			}
		}

		
		g_theWorld->CutImprovements(pos);

		if ( !river && !g_placeGoodsMode) {
			sint32 radius = 0;
			switch(ScenarioEditor::BrushSize()) {
				case 1:
					radius = 0;
					break;
				case 2:
					radius = 1;
					break;
				case 4:
					radius = 2;
					break;
				default: 
					radius = 1;
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

		if(ScenarioEditor::PlaceCityMode()) {
			sint32 ui;
			for(ui = 0; ui < g_theUnitDB->NumRecords(); ui++) {
				if(g_theUnitDB->Get(ui)->GetHasPopAndCanBuild() &&
				   g_theUnitDB->Get(ui)->GetMovementTypeLand()) {
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
			if (g_theWorld->CanEnter(pos, g_theUnitDB->Get(unitNum)->GetMovementType()) ||
				g_theUnitDB->Get(unitNum)->GetHasPopAndCanBuild() ||
				g_theUnitDB->Get(unitNum)->GetIsTrader()) {
				
				
				
				
				
				Player *p = g_player[g_selected_item->GetVisiblePlayer()];
				if (!p) return;
	

				if(g_theUnitDB->Get(unitNum)->GetHasPopAndCanBuild()) {
					if(g_theWorld->IsWater(pos) || g_theWorld->IsShallowWater(pos)) {
						sint32 i;
						for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
							if(g_theUnitDB->Get(i)->GetHasPopAndCanBuild() &&
								g_theUnitDB->Get(i)->GetMovementTypeSea()) {
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
							Unit id1 = p->CreateUnit(unitNum, pos, Unit(0), FALSE, CAUSE_NEW_ARMY_CHEAT);
						}
					} else {
						
						Unit id1 = p->CreateUnit(unitNum, pos, Unit(0), FALSE, CAUSE_NEW_ARMY_CHEAT);
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
	POINT			point;

	point = data->position;
	
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

void TiledMap::Click(aui_MouseEvent *data, BOOL doubleClick)
{	
	UnitActor		*actor=NULL;
	MapPoint		pos;
	Unit			top;
	POINT			point;

	point = data->position;

	if (MousePointToTilePos(point, pos)) {
		
		
		
		
		
		

		
		if (data->lbutton && !data->rbutton) {
			
			if (g_isCheatModeOn || ScenarioEditor::HandleClicks()) {	
				
				if(ScenarioEditor::SelectRegion()) {
					ScenarioEditor::StartRegion(pos);
				} else if(ScenarioEditor::PasteMode()) {
					ScenarioEditor::Paste(pos);
				} else {
					HandleCheat(pos);
				}
			}
			else if ( g_isTransportOn ) {

				g_isTransportOn = FALSE;
				g_selected_item->Deselect( g_selected_item->GetVisiblePlayer() );
			}
			else if (g_tileImprovementMode)
			{
				
				
				
			}
			else







			
			{
				g_selected_item->RegisterClick(pos, data, doubleClick, 
											   false, false); 
			}
		} else {
			if (data->rbutton && !data->lbutton) {
				
				
















				if ( g_tileImprovementMode ) {
					g_selected_item->Deselect( g_selected_item->GetVisiblePlayer() );
				} 
				else {
					g_selected_item->RegisterClick(pos, data, doubleClick, false, false);
				}
			} else {
				
			}
		}
	}
}

void TiledMap::Drop(aui_MouseEvent *data)
{
	MapPoint		pos;
	Unit			top;
	POINT			point;
	TradeRoute		*route;

	point = data->position;

	if (MousePointToTilePos(point, pos)) {
		
		if (g_isCheatModeOn) {
		
		}
		else if(ScenarioEditor::SelectRegion()) {
			ScenarioEditor::EndRegion(pos);
		}
		else if (g_tradeSelectedState)
		{
			
			g_tradeSelectedState = FALSE;
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
	TradeRoute *route;
	MapPoint point;

	
	
	if (!GetMouseTilePos(point)) return;

	
	if (g_tradeSelectedState)
	{
		
		g_grabbedItem->GetGrabbedItem(&route);

		
		
		
		route->SetPathSelectionState(k_TRADEROUTE_SELECTED_PATH);

		route->GenerateSelectedPath(point);
	}
}


BOOL TiledMap::GetMousePos(POINT &pos)
{
	aui_Mouse	*mouse;

	Assert(g_c3ui);
	if (!g_c3ui) return FALSE;

	Assert(g_background);
	if (!g_background) return FALSE;

	mouse = g_c3ui->TheMouse();
	if (mouse == NULL) return FALSE;

	pos.x = mouse->X() - g_background->X();
	pos.y = mouse->Y() - g_background->Y();

	return TRUE;
}

BOOL TiledMap::GetMouseTilePos(MapPoint &pt)
{
	POINT	pos;

	if (!GetMousePos(pos)) return FALSE;

	if (!MousePointToTilePos(pos, pt)) return FALSE;



	return TRUE;
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
	
	if(g_player[g_selected_item->GetVisiblePlayer()]) {
		m_localVision->Copy(g_player[g_selected_item->GetVisiblePlayer()]->m_vision);
		m_oldPlayer = g_selected_item->GetVisiblePlayer();
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
// Returns    : BOOL            : tiles may be drawn
//
// Remark(s)  : The tiles may not be drawn in the following cases:
//              - For multiplayer games, when the network is not ready.
//              - When the game engine says so (e.g. between turns in hotseat).
//              - When there is no selected item (yet).
//              - Before the actual start of the game.
//
//----------------------------------------------------------------------------
BOOL TiledMap::ReadyToDraw() const
{
	if ((g_network.IsActive() || g_network.IsNetworkLaunch()) &&
	    !g_network.ReadyToStart()
       )
    {
		return FALSE;
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
	unsigned short  *srcPixel = (unsigned short *)data;

	uint16		start	= (uint16)*data++;
	uint16		end		= (uint16)*data++;
	Pixel16		*table	= data;
	Pixel16		*dataStart = table + (end - start + 1);

	register sint32 j;
	register sint32 len,looplen;

	
	sint32 xoff=x;
	sint32 i;

	
	Pixel16		*rowData;
	Pixel16		tag;

	
	for(j=start; j<=end; j++) 
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
				destPixel[i] = pixelutils_Shadow(destPixel[i]);
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
	Pixel16		*dataPtr;
	sint32		x, y;
	sint32		startX, endX;
	TileInfo	*tileInfo;
	BaseTile	*baseTile;
	uint16		index;
	uint8		*pSurfBase;

	if (!surface) 
		surface = m_surface;

	
	pSurfBase			= m_surfBase;
	sint32 surfWidth	= m_surfWidth;
	sint32 surfHeight	= m_surfHeight;
	sint32 surfPitch	= m_surfPitch;

	ypos+=k_TILE_PIXEL_HEADROOM;

	

	
	


	



	
	if (xpos > (surfWidth-k_TILE_PIXEL_WIDTH)) 
		return;
	if (ypos > (surfPitch-k_TILE_PIXEL_HEIGHT)) 
		return;

	
	tileInfo = GetTileInfo(pos);

	Assert(tileInfo != NULL);
   
	if (tileInfo == NULL) 
		return;

	index = tileInfo->GetTileNum();

	baseTile = m_tileSet->GetBaseTile(index);
	
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

	dataPtr = data;

	
	Pixel16 srcPixel,tindex;
	uint16 *pDestPixel;
	bool    firstpixel=true;

	int xsrc,ysrc;

	for (y=0; y<k_TILE_PIXEL_HEIGHT; y++) 
	{
	 	firstpixel=true;
		
		if (y<=23) 
			startX = (23-y)*2;
		else 
			startX = (y-24)*2;
		
		endX = k_TILE_PIXEL_WIDTH - startX;

		for (x=startX;x<endX;x++) 
		{
			tindex = srcPixel = *dataPtr++;

			
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
			
			ysrc = (y+ypos);
			
			
			if (ysrc<0)
				continue;

			if (ysrc>=surfHeight)
				return;
			
			
			xsrc = (x+xpos);

			if (xsrc<0)
				continue;

			xsrc<<=1;

		  	if (xsrc>=surfPitch)
				continue;

			pDestPixel = (Pixel16 *)(pSurfBase + (ysrc*surfPitch+xsrc));

			*pDestPixel = srcPixel;

			
			
			
			
			

			firstpixel = false;
		}
	}
}






void TiledMap::SetZoomLevel(sint32 level)
{
	
	Assert(level >= 0 && level < k_MAX_ZOOM_LEVELS); 

	
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

void TiledMap::ReallocateVision()
{
	if(m_localVision)
		delete m_localVision;

	m_localVision = new Vision(g_selected_item->GetVisiblePlayer(), TRUE);
	CopyVision();
}
