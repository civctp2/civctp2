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
//   - Dec. 23rd 2004 - Martin Gühmann
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
#include "ResourceRecord.h"
#include "profileDB.h"

// Added by Martin Gühmann
extern sint32 g_isFastCpu; // Actual permernent set to 1

TileControl::TileControl(AUI_ERRCODE * retval, uint32 id, MBCHAR * ldlBlock )
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Control(retval, id, ldlBlock),
	m_goodActor(NULL),
	m_nextProcessTime(0)
{
}

TileControl::TileControl(AUI_ERRCODE * retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
		ControlActionCallback *ActionFunc, void * cookie)
:
	aui_ImageBase((sint32) 0),
	aui_TextBase(NULL),
	aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
	m_goodActor(NULL),
	m_nextProcessTime(0)
{
}

TileControl::~TileControl()
{
	delete m_goodActor;
	m_goodActor = NULL;
}

void TileControl::PostChildrenCallback(aui_MouseEvent * mouseData)
{
	aui_Control::PostChildrenCallback(mouseData);

	// Ensure that goods will be animated
	if (m_goodActor && GetTickCount() > m_nextProcessTime)
	{
		m_goodActor->Process();
		m_nextProcessTime = GetTickCount() + 75;
		ShouldDraw();
	}
}

void TileControl::SetMouseTile(const MapPoint & pos)
{
	delete m_goodActor;
	m_goodActor = NULL;

	m_currentTile = pos;

	sint32 goodIndex = 0;
	if (g_theWorld->GetGood(pos, goodIndex))
	{
		m_goodActor = new GoodActor(g_theResourceDB->Get(goodIndex)->GetSpriteID(), pos);
		if (g_theProfileDB->IsGoodAnim()) {
			m_goodActor->Reload(LOADTYPE_FULL);
		}
	}
}

AUI_ERRCODE TileControl::DrawThis(aui_Surface * surface, sint32 x, sint32 y)
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
	primitives_PaintRect16(surface, &rect, g_colorSet->GetColor(COLOR_BLACK));

	sint32 tileX = m_x + (m_width / 2) - (k_TILE_PIXEL_WIDTH / 2);
	sint32 tileY = m_y - k_TILE_PIXEL_HEADROOM + (m_height / 2) - (k_TILE_PIXEL_HEIGHT / 2);
	if (!DrawTile(surface, tileX, tileY)) {
		return AUI_ERRCODE(-1);
	}

	if (surface == m_window->TheSurface()) {
		m_window->AddDirtyRect(&rect);
	}

	return AUI_ERRCODE_OK;
}

bool TileControl::DrawTile(aui_Surface * surface, sint32 x, sint32 y)
{
	TileInfo * tileInfo = g_tiledMap->GetTileInfo(m_currentTile);
	if (!tileInfo) {
		return false;
	}

	BaseTile * baseTile = g_tiledMap->GetTileSet()->GetBaseTile(tileInfo->GetTileNum());
	if (!baseTile) {
		return false;
	}
	Pixel16 * hatData = baseTile->GetHatData();

	sint16    riverPiece = tileInfo->GetRiverPiece();
	Pixel16 * river = (riverPiece != -1) ? g_tiledMap->GetTileSet()->GetRiverData(riverPiece) : NULL;

	g_tiledMap->LockThisSurface(surface);

	// Added by Martin Gühmann
	bool fog = g_tiledMap->GetLocalVision()
			&& g_tiledMap->GetLocalVision()->IsExplored(m_currentTile)
			&& !g_tiledMap->GetLocalVision()->IsVisible(m_currentTile);
	if (!fog)
	{
		g_tiledMap->DrawTransitionTile(NULL, m_currentTile, x, y);
		g_tiledMap->DrawOverlay(NULL, hatData, x, y);
		if (river) {
			g_tiledMap->DrawOverlay(NULL, river, x, y);
		}
	}
	else
	{
		if (g_isFastCpu)
		{
			g_tiledMap->DrawBlendedTile(NULL, m_currentTile, x, y, k_FOW_COLOR,k_FOW_BLEND_VALUE);
			g_tiledMap->DrawBlendedOverlay(NULL, hatData, x, y, k_FOW_COLOR,k_FOW_BLEND_VALUE);
			if (river) {
				g_tiledMap->DrawBlendedOverlay(NULL, river, x, y, k_FOW_COLOR, k_FOW_BLEND_VALUE);
			}
		}
		else
		{
			g_tiledMap->DrawDitheredTile(NULL, x, y, k_FOW_COLOR);
			g_tiledMap->DrawDitheredOverlay(NULL, hatData, x, y, k_FOW_COLOR);
			if (river) {
				g_tiledMap->DrawDitheredOverlay(NULL, river, x, y, k_FOW_COLOR);
			}
		}
	}

	g_tiledMap->DrawImprovementsLayer(NULL, m_currentTile, x, y);

	g_tiledMap->UnlockSurface();

	if (m_goodActor) {
		m_goodActor->DrawDirect(surface, x, y, 1.0);
	}

	return true;
}
