//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface window
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
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
// - Removed compiler complaint.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __RESOURCEMAP_H__
#define __RESOURCEMAP_H__

#include "patternbase.h"
#include "aui_control.h"
#include "Unit.h"
#include "MapPoint.h"

#define k_RESOURCEMAP_DEFAULT_SCALE				0
#define k_RESOURCEMAP_LDL_SCALE					"scale"

class aui_Surface;
class c3_UpdateAction;
class MapPoint;
class GoodActor;
class aui_StringTable;

class ResourceMap : public aui_Control, public PatternBase {
public:

	typedef BOOL (ResourceMapDrawFunc)(aui_Surface *surf, const MapPoint &pos, void *context);

	ResourceMap(AUI_ERRCODE *retval,
					sint32 id,
					MBCHAR *ldlBlock,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL);
	ResourceMap(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ResourceMap();

	void InitCommonLdl(MBCHAR *ldlBlock);
	void InitCommon( sint32 scale );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void DrawView(void);

	void NotifyPopChanged();

	BOOL		DrawSprites(aui_Surface *pSurface, RECT *destRect);
	sint32		DrawSurface(void);
	sint32		DrawSpaceImprovements( aui_Surface *pSurface, sint32 xOff, sint32 yOff );

	static ResourceMapDrawFunc	DrawACity;
	static ResourceMapDrawFunc	DrawALandCity;
	static ResourceMapDrawFunc	DrawAGood;
	static ResourceMapDrawFunc	DrawATile;







	BOOL		DrawResourceMapThing(aui_Surface *pSurface, ResourceMapDrawFunc *func);

	void		SetUnit(const Unit &unit) { m_unit = unit; }
	Unit		GetUnit(void) { return m_unit; }

	sint32		CalculateWrap(aui_Surface *surface, sint32 i, sint32 j, sint32 x, sint32 y);
	sint32		DrawImprovements(aui_Surface *surface, sint32 i, sint32 j, sint32 x, sint32 y);
	void		DrawCityName(aui_Surface *surface, sint32 x, sint32 y, const Unit &unit);

	sint32		UpdateFromSurface(aui_Surface *destSurface, RECT *rect);

	void		Click(aui_MouseEvent *data);
	BOOL		PointInMask(POINT hitPt);
	BOOL		MousePointToTilePos(POINT point, MapPoint &tilePos);
	void		SetHiliteMouseTile(MapPoint &pos);
	void		DrawHiliteMouseTile(aui_Surface *destSurf, RECT *destRect);

	void		HandlePop( MapPoint point );

	void		SetUpdateAction(c3_UpdateAction *action) { m_updateAction = action; }
	c3_UpdateAction *GetUpdateAction(void) { return m_updateAction; }

	virtual AUI_ERRCODE			Idle( void );

	RECT		*GetNormalizedViewRect(void) { return &m_normalizedViewRect; }
	sint32		GetLeftEdge(void) { return m_leftEdge; }
	sint32		GetTopEdge(void) { return m_topEdge; }
	MapPoint	*GetTopLeftPos(void) { return &m_topLeftPos; }
	sint32		GetScale(void) { return m_scale; }

	void		GetOwningCity(Unit &c) { c = m_unit; }

protected:
	aui_Surface *   m_surface;

	Unit		m_unit;

	RECT		m_mapViewRect;
	RECT		m_mapBounds;

	MapPoint	m_hiliteMouseTile;
	BOOL		m_drawHilite;

	sint32		m_totalFood;
	sint32		m_totalProd;
	sint32		m_totalGold;

	aui_StringTable *m_string;

	c3_UpdateAction *m_updateAction;

	MapPoint			m_current_mouse_tile;

	sint32		m_scale;

	RECT		m_normalizedViewRect;
	sint32		m_leftEdge;
	sint32		m_topEdge;
	MapPoint	m_topLeftPos;

	RECT        m_usedRect;

	// Event callback handlers
#if defined(_MSC_VER)
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseMoveInside;
	virtual MouseEventCallback MouseMoveAway;
#else
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway(aui_MouseEvent * mouseData);
#endif

};

#endif
