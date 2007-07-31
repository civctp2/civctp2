












#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "primitives.h"
#include "TiledMap.h"
#include "BaseTile.h"
#include "TileInfo.h"
#include "maputils.h"
#include "primitives.h"
#include "aui_control.h"
#include "tilecontrol.h"
#include "GoodActor.h"

extern TiledMap			*g_tiledMap;






TileControl::TileControl(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock )
	: aui_Control(retval,id,ldlBlock), 	aui_TextBase( ldlBlock, (MBCHAR *)NULL ), aui_ImageBase( ldlBlock )

{

}

TileControl::TileControl(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie )
:
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie ),
	aui_TextBase(NULL),
	aui_ImageBase( (sint32)0 )
{

}



AUI_ERRCODE TileControl::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;
	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	primitives_PaintRect16( surface, &rect, 0x0000 );
	
	sint32 i,jukeY(7),jukeX(0);
	maputils_MapX2TileX(m_currentTile.x, m_currentTile.y, &i);
	





	if (DrawTile(surface, m_currentTile.y,i,
				m_x + (m_width/2) - (k_TILE_PIXEL_WIDTH/2), 
				m_y - k_TILE_PIXEL_HEADROOM + (m_height/2) - (k_TILE_PIXEL_HEIGHT/2)) == -1) return AUI_ERRCODE(-1);

	if (surface == m_window->TheSurface()) m_window->AddDirtyRect(&rect);

	


	return AUI_ERRCODE_OK;
}

sint32 TileControl::DrawTile(
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
	
	
	

	g_tiledMap->LockThisSurface(surface);

	g_tiledMap->DrawTransitionTile(NULL, pos, x, y);

	
	g_tiledMap->DrawOverlay(NULL, baseTile->GetHatData(), x, y);
	
	
	if (river != -1)
		g_tiledMap->DrawOverlay(NULL, g_tiledMap->GetTileSet()->GetRiverData(river), x, y);

	g_tiledMap->DrawImprovementsLayer(NULL, pos, x, y);

	g_tiledMap->UnlockSurface();

	GoodActor *good = tileInfo->GetGoodActor();
	if (good) {
		good->DrawDirect(surface, x, y, 1.0);
	}

	return 0;
}