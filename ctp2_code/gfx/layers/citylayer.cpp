//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City layer
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui.h"
#include "pixelutils.h"
#include "tileutils.h"
#include "primitives.h"
#include "tiledmap.h"
#include "MapPoint.h"
#include "World.h"
#include "maputils.h"
#include "dynarr.h"
#include "director.h"
#include "SelItem.h"
#include "aui_surface.h"

extern TiledMap			*g_tiledMap;
extern SelectedItem		*g_selected_item;

#include "background.h"
extern Background		*g_background;

#include "screenmanager.h"
extern ScreenManager	*g_screenManager;

static const uint32 LINE_PATTERN_TRADE_DASH        = 0b00000000000000001111000000001111;
static const uint32 LINE_PATTERN_TRADE_DASH_LENGTH = 16;

void DrawTradeRouteSegment(aui_Surface *surf, MapPoint &pos, WORLD_DIRECTION dir, uint16 route, uint16 outline)
{
	sint32		x1 = 0, y1 = 0,
				x2 = 0, y2 = 0;

	maputils_MapXY2PixelXY(pos.x, pos.y, &x1, &y1);

	switch (dir) {
	case NORTH :
		x2 = x1;
		y2 = y1 - g_tiledMap->GetZoomTilePixelHeight()/2;
		break;
	case NORTHEAST :
		x2 = x1 + g_tiledMap->GetZoomTilePixelWidth()/4;
		y2 = y1 - g_tiledMap->GetZoomTilePixelHeight()/4;
		break;
	case EAST :
		x2 = x1 + g_tiledMap->GetZoomTilePixelWidth()/2;
		y2 = y1;
		break;
	case NORTHWEST :
		x2 = x1 - g_tiledMap->GetZoomTilePixelWidth()/4;
		y2 = y1 - g_tiledMap->GetZoomTilePixelHeight()/4;
		break;
	case SOUTHEAST :
		x2 = x1 + g_tiledMap->GetZoomTilePixelWidth()/4;
		y2 = y1 + g_tiledMap->GetZoomTilePixelHeight()/4;
		break;
	case WEST :
		x2 = x1 - g_tiledMap->GetZoomTilePixelWidth()/2;
		y2 = y1;
		break;
	case SOUTHWEST :
		x2 = x1 - g_tiledMap->GetZoomTilePixelWidth()/4;
		y2 = y1 + g_tiledMap->GetZoomTilePixelHeight()/4;
		break;
	case SOUTH :
		x2 = x1;
		y2 = y1 + g_tiledMap->GetZoomTilePixelHeight()/2;
		break;
	}

	x1 += g_tiledMap->GetZoomTilePixelWidth()/2;
	y1 += g_tiledMap->GetZoomTilePixelHeight();
	x2 += g_tiledMap->GetZoomTilePixelWidth()/2;
	y2 += g_tiledMap->GetZoomTilePixelHeight();

	sint32 top, left, right, bottom;

	if (x1 < x2) {
		left = x1;
		right = x2;
	} else {
		left = x2;
		right = x1;
	}

	if (y1 < y2) {
		top = y1;
		bottom = y2;
	} else {
		top = y2;
		bottom = y1;
	}

	RECT tempRect;
	SetRect(&tempRect, left, top, right, bottom);

	primitives_ClippedPatternLine16(*surf, x1, y1, x2, y2, route, LINE_PATTERN_TRADE_DASH,
			LINE_PATTERN_TRADE_DASH_LENGTH);

	if (g_tiledMap->GetZoomLevel() > k_ZOOM_SMALLEST + 2) {
		if (x1 == x2) {
			x1++;
			x2++;
		} else {
			y1++;
			y2++;
		}

		tempRect.right++;
		tempRect.bottom++;

		primitives_ClippedPatternLine16(*surf, x1, y1, x2, y2, route, LINE_PATTERN_TRADE_DASH,
				LINE_PATTERN_TRADE_DASH_LENGTH);
	}

	if (x1==x2) {
		x1++;
		x2++;
	} else {
		y1++;
		y2++;
	}

	tempRect.right++;
	tempRect.bottom++;

	primitives_ClippedPatternLine16(*surf, x1, y1, x2, y2, outline, LINE_PATTERN_TRADE_DASH,
			LINE_PATTERN_TRADE_DASH_LENGTH);

	g_tiledMap->AddDirtyRectToMix(tempRect);
}

void DrawReversedTradeRouteSegment(aui_Surface *surf, MapPoint &pos, WORLD_DIRECTION dir, uint16 route, uint16 outline)
{
	WORLD_DIRECTION revDir = NOWHERE;

	switch (dir) {
	case NORTH :		revDir = SOUTH;		break;
	case NORTHEAST :	revDir = SOUTHWEST;	break;
	case EAST :			revDir = WEST;		break;
	case NORTHWEST :	revDir = SOUTHEAST;	break;
	case SOUTHEAST :	revDir = NORTHWEST;	break;
	case WEST :			revDir = EAST;		break;
	case SOUTHWEST :	revDir = NORTHEAST;	break;
	case SOUTH :		revDir = NORTH;		break;
	}

	DrawTradeRouteSegment(surf, pos, revDir, route, outline);
}


void DrawTradeRoute(aui_Surface *pSurface, DynamicArray<MapPoint> *pRoute, const RECT & paintRect, uint16 route,
		uint16 outline)
{
	Assert(pSurface);
	if (pSurface==NULL) return;
	Assert(pRoute);
	if (pRoute==NULL) return;

	if (outline == 0x0000)
		outline = 0x0001;

	MapPoint prev, curr;
	MapPoint screenPrev, screenCurr;

	WORLD_DIRECTION	dir = NOWHERE, oldDir = NOWHERE;

	for (sint32 i = 0;i < pRoute->Num()-1;i++)
	{
		prev = pRoute->Get(i);
		curr = pRoute->Get(i+1);

		sint32 tileX;
		maputils_MapX2TileX(prev.x, prev.y, &tileX);
		if (g_tiledMap->TileIsVisible(prev.x, prev.y) && maputils_TilePointInTileRect(tileX, prev.y, paintRect))
		{
			dir = prev.GetNeighborDirection(curr);

			if (!g_tiledMap->GetLocalVision()->IsExplored(prev)) {
				oldDir = dir;
				continue;
			}

			uint16 routeColor = route;

			if (!g_tiledMap->GetLocalVision()->IsVisible(prev)) {
				routeColor = pixelutils_PercentDarken(routeColor, 64);
			}

			if (oldDir != NOWHERE)
				DrawReversedTradeRouteSegment(pSurface, prev, oldDir, routeColor, outline);

			DrawTradeRouteSegment(pSurface, prev, dir, routeColor, outline);

			if ((i == pRoute->Num()-1) && g_tiledMap->GetLocalVision()->IsExplored(curr)) {
				routeColor = route;
				if (!g_tiledMap->GetLocalVision()->IsVisible(curr)) {
					routeColor = pixelutils_PercentDarken(routeColor, 64);
				}
				DrawReversedTradeRouteSegment(pSurface, curr, dir, routeColor, outline);
			}

			oldDir = dir;
		}
	}
}
