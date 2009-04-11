//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface ?
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __WORKMAP_H__
#define __WORKMAP_H__

#include "patternbase.h"
#include "aui_control.h"
#include "Unit.h"
#include "MapPoint.h"

#define k_MAX_WORKERS	21

#define k_WORKMAP_DEFAULT_SCALE				0
#define k_WORKMAP_LDL_SCALE					"scale"

class aui_Surface;
class WorkerActor;
class c3_UpdateAction;
class MapPoint;
class GoodActor;
class aui_StringTable;

enum WM {
	WM_FOOD,
	WM_PROD,
	WM_GOLD,

	WM_MAX
};

class WorkMap : public aui_Control, public PatternBase {
public:

	typedef BOOL (WorkMapDrawFunc)(aui_Surface *surf, MapPoint const & pos, void *context);

	
	WorkMap(AUI_ERRCODE *retval, 
					sint32 id, 
					MBCHAR *ldlBlock,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL);
	WorkMap(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~WorkMap();

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

	static WorkMapDrawFunc	DrawACity;
	static WorkMapDrawFunc	DrawALandCity;
	static WorkMapDrawFunc	DrawAGood;
	static WorkMapDrawFunc	DrawATile;







	BOOL		DrawWorkMapThing(aui_Surface *pSurface, WorkMapDrawFunc *func);

	void		SetUnit(const Unit &unit) { m_unit = unit; }
	Unit		GetUnit(void) { return m_unit; }

	sint32		CalculateWrap(aui_Surface *surface, sint32 i, sint32 j, sint32 x, sint32 y);
	sint32		DrawImprovements(aui_Surface *surface, sint32 i, sint32 j, sint32 x, sint32 y);
	void		DrawCityName(aui_Surface *surface, sint32 x, sint32 y, const Unit &unit);
	void		DrawResourceIcons(aui_Surface *surface, sint32 x, sint32 y, MapPoint &pos);
	void		CenterNumber( RECT *rect, sint32 &x, sint32 &y, sint32 width, sint32 height );

	sint32		UpdateFromSurface(aui_Surface *destSurface, RECT *rect);
	void		DrawALabel( aui_Surface *surface, MBCHAR *label, sint32 x, sint32 y, sint32 width, sint32 height );
	void		DrawLabels( aui_Surface *surface );

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

	WorkerActor	*GetWorker(sint32 index) 
				{ 
					Assert(index >= 0 && index < k_MAX_WORKERS);
					if (index < 0 && index >= k_MAX_WORKERS) return NULL; 
					return m_worker[index]; 
				}
	
	void		SetWorker(sint32 index, WorkerActor *actor) 
				{
					Assert(index >= 0 && index < k_MAX_WORKERS);
					if (index < 0 && index >= k_MAX_WORKERS) return; 

					Assert(actor);
					m_worker[index] = actor;
				}

	sint32		GetNumWorkers(void) { return m_numWorkers; }
	void		SetNumWorkers(sint32 num) { m_numWorkers = num; }

protected:		
	aui_Surface *m_surface;

	Unit		m_unit;

	sint32		m_numWorkers;
	WorkerActor *m_worker[k_MAX_WORKERS];

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

	// Event handlers
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway(aui_MouseEvent * mouseData);
};

#endif
