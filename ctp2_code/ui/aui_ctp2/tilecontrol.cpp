//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile help window draw handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Tile help window now dims tiles under the fog of war.
//   - Dec. 23rd 2004 - Martin G�hmann
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "primitives.h"
#include "tiledmap.h"
#include "BaseTile.h"
#include "TileInfo.h"
#include "maputils.h"
#include "primitives.h"
#include "aui_control.h"
#include "tilecontrol.h"
#include "GoodActor.h"

extern TiledMap			*g_tiledMap;

// Added by Martin G�hmann
extern sint32			g_isFastCpu; // Actual permernent set to 1




TileControl::TileControl(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock )
:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Control(retval,id,ldlBlock)
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
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie )
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

	sint32 i;
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

// Added by Martin G�hmann
	bool fog =((   g_tiledMap->GetLocalVision()
	            && g_tiledMap->GetLocalVision()->IsExplored(pos)
	            &&!g_tiledMap->GetLocalVision()->IsVisible(pos)));

	if (!fog) {
		g_tiledMap->DrawTransitionTile(NULL, pos, x, y);
		g_tiledMap->DrawOverlay(NULL, baseTile->GetHatData(), x, y);
		if (river != -1)
			g_tiledMap->DrawOverlay(NULL, g_tiledMap->GetTileSet()->GetRiverData(river), x, y);
	}
	else {
		if (g_isFastCpu) {
			g_tiledMap->DrawBlendedTile(NULL, pos,x,y,k_FOW_COLOR,k_FOW_BLEND_VALUE);
			g_tiledMap->DrawBlendedOverlay(NULL, baseTile->GetHatData(),x,y,k_FOW_COLOR,k_FOW_BLEND_VALUE);
			if (river != -1)
				g_tiledMap->DrawBlendedOverlay(NULL, g_tiledMap->GetTileSet()->GetRiverData(river),x,y,k_FOW_COLOR,k_FOW_BLEND_VALUE);
		}
		else {
			g_tiledMap->DrawDitheredTile(NULL, x,y,k_FOW_COLOR);
			g_tiledMap->DrawDitheredOverlay(NULL, baseTile->GetHatData(),x,y,k_FOW_COLOR);
			if (river != -1)
				g_tiledMap->DrawDitheredOverlay(NULL, g_tiledMap->GetTileSet()->GetRiverData(river),x,y,k_FOW_COLOR);
		}
	}

	g_tiledMap->DrawImprovementsLayer(NULL, pos, x, y);

	g_tiledMap->UnlockSurface();

	GoodActor *good = tileInfo->GetGoodActor();
	if (good) {
		good->DrawDirect(surface, x, y, 1.0);
	}

	return 0;
}
