

#pragma once

#ifndef __THUMBNAILMAP_H__
#define __THUMBNAILMAP_H__

#include "PatternBase.h"
#include "aui_control.h"
#include "ColorSet.h"

#include "Unit.h"
#include "DynArr.h"

#define k_THUMBNAIL_CITY_BLINK_RATE		1000


typedef BOOL (CityFilterProc)(Unit city);




enum C3_THUMBNAIL_ACTION {
	C3_THUMBNAIL_ACTION_NULL,
	C3_THUMBNAIL_ACTION_SELECTEDCITY,
	C3_THUMBNAIL_ACTION_SELECTEDROUTE,

	C3_THUMBNAIL_ACTION_MAX
};




enum COLOR;
class CivArchive;
struct CityInfo {
	Unit		city;
	RECT		cityRect;
	BOOL		cityBlink;
	COLOR		cityBlinkColor;
	uint32		cityBlinkTime;

	virtual void Serialize(CivArchive &archive) {}
	virtual void Castrate() {}
    virtual void DelPointers() {}
};


class aui_Surface;
class MapPoint;
class TradeRoute;

class ThumbnailMap : public aui_Control, public PatternBase
{
public:
	ThumbnailMap(AUI_ERRCODE *retval, 
					sint32 id, 
					MBCHAR *ldlBlock,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL);
	ThumbnailMap(AUI_ERRCODE *retval,
					uint32 id,
					sint32 x,
					sint32 y,
					sint32 width,
					sint32 height,
					MBCHAR *pattern,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL);

	virtual ~ThumbnailMap();

	void		InitCommonLdl(MBCHAR *ldlBlock);
	void		InitCommon(void);
	virtual		AUI_ERRCODE	Resize( sint32 width, sint32 height );

	void		BuildCityList(void);

	

	void		SetCityBlink(Unit city, BOOL blink, COLOR blinkColor = COLOR_BLACK);

	
	
	
	void		ClearMapOverlay(void);
	void		SetMapOverlayCell(MapPoint &pos, COLOR color);

	void		SetSelectedCity(Unit city) { m_selectedCity = city; }
	Unit		GetSelectedCity(void) { return m_selectedCity; }

	void		SetSelectedRoute(TradeRoute *route) { m_selectedRoute = route; }
	TradeRoute *GetSelectedRoute(void) { return m_selectedRoute; }

	
	void		SetCityFilterProc(CityFilterProc *proc) { m_cityFilterProc = proc; }
	CityFilterProc *GetCityFilterProc(void) { return m_cityFilterProc; }

	
	void		CalculateMetrics(void);
	
	
	void		RenderMap(aui_Surface *surf);
	void		RenderTradeRoute(aui_Surface *surf, TradeRoute *route);
	void		RenderTradeRoutes(aui_Surface *surf);
	void		RenderUnitMovement(aui_Surface *surf);
	void		RenderAll(aui_Surface *surf);

	
	POINT		MapToPixel(sint32 x, sint32 y);
	POINT		MapToPixel(MapPoint *pos);

	
	void		UpdateCities(aui_Surface *surf, sint32 x, sint32 y);
	void		UpdateMap(aui_Surface *surf, sint32 x, sint32 y);
	void		UpdateAll( void );

	
	virtual AUI_ERRCODE			DrawThis(aui_Surface *surface, sint32 x, sint32 y);
	virtual MouseEventCallback	MouseLGrabInside;
	virtual MouseEventCallback	MouseRGrabInside;
	virtual MouseEventCallback	MouseNoChange;
	virtual MouseEventCallback	MouseMoveInside;

	virtual AUI_ERRCODE			Idle( void );

	BOOL ShowTipWindow( aui_MouseEvent *mouseData );

private:
	aui_Surface						*m_mapSurface;		
	MapPoint						*m_mapSize;
	COLOR							*m_mapOverlay;		
	MapPoint						m_clickedCell;		

	sint32							m_centerX,			
									m_centerY;

	double							m_tilePixelWidth,
									m_tilePixelHeight;

	TradeRoute						*m_selectedRoute;
	Unit							m_selectedCity;

	DynamicArray<CityInfo>			*m_cityList;

	BOOL							m_displayUnits;
	BOOL							m_displayLandOwnership;
	BOOL							m_displayCities;
	BOOL							m_displayCityNames;
	BOOL							m_displayTradeRoutes;
	BOOL							m_displayDirectRoutes;
#ifdef BATTLE_FLAGS
	BOOL							m_displayBattleFlags;
#endif
	BOOL							m_displayUnitMovement;
	BOOL							m_displayOverlay;

	CityFilterProc					*m_cityFilterProc;	
};

#endif
