//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface radar map (mini map?)
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
// - #01 Allow shifing the X and Y axis in the radar map with RMouse click
//   (L. Hirth 6/2004)
// -Added m_displayPolitical variable and associated functions (6-Jul-2009 EPW)
// -Added m_displayCapitol and functions(5-Jan-10 EPW)
// -Added m_displayRelations and functions(7-Jan-10 EPW)
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef ___BMH_RADAR_MAP_HEADER
#define ___BMH_RADAR_MAP_HEADER

#include "aui_control.h"
#include "MapPoint.h"
#include "patternbase.h"
#include "Unit.h"
#include "pixeltypes.h"
#include "profileDB.h"
#include "colorset.h"	// COLOR

enum C3_RADAR_ACTION {
	C3_RADAR_ACTION_NULL,
	C3_RADAR_ACTION_MAX
};

class aui_Surface;
class MapPoint;
class Player;
//enum COLOR;

class RadarMap : public aui_Control, public PatternBase
{
public:
	RadarMap(AUI_ERRCODE *retval,
					sint32 id,
					MBCHAR *ldlBlock,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL);
	RadarMap(AUI_ERRCODE *retval,
					uint32 id,
					sint32 x,
					sint32 y,
					sint32 width,
					sint32 height,
					MBCHAR *pattern,
					ControlActionCallback *ActionFunc = NULL,
					void *cookie = NULL);

	virtual ~RadarMap();

	void		InitCommonLdl(MBCHAR *ldlBlock);
	void		InitCommon(void);
	virtual		AUI_ERRCODE	Resize( sint32 width, sint32 height );




	void		ClearMapOverlay(void);
	void		SetMapOverlayCell(MapPoint const & pos, COLOR color);

	void		CalculateMetrics(void);

	void RenderMap(aui_Surface *surface);
	void RenderViewRect(aui_Surface *surf, sint32 x, sint32 y);

	POINT		MapToPixel(sint32 x, sint32 y);
	POINT		MapToPixel(MapPoint *pos);

	void		UpdateMap(aui_Surface *surf, sint32 x, sint32 y);

	BOOL		IncludePointInView(MapPoint &pos, sint32 radius);
	MapPoint	ComputeCenteredMap(MapPoint const & pos, RECT *viewRect);
	MapPoint	CenterMap(const MapPoint &pos );
	void		Setup( void );
	void		Update( void );
	void		RedrawTile( const MapPoint *point );
	void		SetSelectedCity( Unit city ) { m_selectedCity = city; };

	aui_Surface *GetMapSurface(void) const { return m_mapSurface; }

	virtual AUI_ERRCODE			DrawThis(aui_Surface *surface, sint32 x, sint32 y);

	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);

	virtual AUI_ERRCODE			Idle( void );

	bool IsInteractive() const { return(m_isInteractive); }


	void SetInteractive(bool status = true)
	{ m_isInteractive = status; ShouldDraw(); }

	void DisplayUnits(bool status = true)
		{ m_displayUnits = status; Update(); g_theProfileDB->SetDisplayUnits(status);}
	void DisplayCities(bool status = true)
		{ m_displayCities = status; Update(); g_theProfileDB->SetDisplayCities(status);}
	void DisplayBorders(bool status = true)
		{ m_displayBorders = status; Update(); g_theProfileDB->SetDisplayBorders(status);}
	void DisplayOverlay(bool status = true)
		{ m_displayOverlay = status; Update(); }
	void Filter(bool status = true)
		{ m_filter = status; Update(); g_theProfileDB->SetDisplayFilter(status);}
	void DisplayTrade(bool status = true)
		{ m_displayTrade = status; Update(); g_theProfileDB->SetDisplayTrade(status);}
	void DisplayTerrain(bool status = true)
		{ m_displayTerrain = status; Update(); g_theProfileDB->SetDisplayTerrain(status);}
	void DisplayPolitical(bool status = true)
		{ m_displayPolitical = status; Update(); g_theProfileDB->SetDisplayPolitical(status);}
	void DisplayCapitols(bool status = true)
		{ m_displayCapitols = status; Update(); g_theProfileDB->SetDisplayCapitols(status);}
	void DisplayRelations(bool status = true)
		{ m_displayRelations = status; Update(); g_theProfileDB->SetDisplayRelations(status);}

	bool IsDisplayUnits() { return(m_displayUnits); }
	bool IsDisplayCities() { return(m_displayCities); }
	bool IsFilter() { return(m_filter); }
	bool IsDisplayBorders() { return(m_displayBorders); }
	bool IsDisplayOverlay() { return(m_displayOverlay); }
	bool IsDisplayTrade() { return m_displayTrade; }
	bool IsDisplayTerrain() { return m_displayTerrain; }
	bool IsDisplayPolitical() { return m_displayPolitical; }
	bool IsDisplayCapitols() { return m_displayCapitols; }
	bool IsDisplayRelations() { return m_displayRelations; }

private:

	Player *GetVisiblePlayerToRender();


	Pixel16 RadarTileColor(const Player *player, const MapPoint &position, uint32 &flags);

	Pixel16 RadarTileBorderColor(const MapPoint &position, const Player *player);

	Pixel16 RadarTileRelationsColor    (const MapPoint &position, const Player *player, sint32 unitOwner = -1);
	Pixel16 RadarTileRelationsDarkColor(const MapPoint &position, const Player *player, sint32 unitOwner = -1);

	uint8 RadarTileBorder(const Player *player, const MapPoint &position);

	void RenderTradeRoute(aui_Surface *surf, const RECT &tileRectangle);

	void RenderCapitol(aui_Surface *surface, const MapPoint &position, const MapPoint &worldpos, Player *player);


	void RenderSpecialTile(aui_Surface *surface, const MapPoint &screenPosition,
		Pixel16 color, uint32 flags);


	void RenderSpecialTileBorder(aui_Surface *surface, const MapPoint &screenPosition,
		uint8 borderFlags, Pixel16 borderColor);

	void RenderNormalTile(aui_Surface *surface, const MapPoint &screenPosition,
		Pixel16 color, uint32 flags);

	void RenderNormalTileBorder(aui_Surface *surface, const MapPoint &screenPosition,
		uint8 borderFlags, Pixel16 borderColor);

	void RenderMapTile(aui_Surface *surface, const MapPoint &screenPosition,
		Pixel16 color, uint32 flags);

	void RenderMapTileBorder(aui_Surface *surface, const MapPoint &screenPosition,
		uint8 borderFlags, Pixel16 borderColor);

	Pixel16 RadarTileColor(const Player *player, const MapPoint &position,
							const MapPoint &worldpos, uint32 &flags);

	void RenderTile(aui_Surface *surface, const MapPoint &position,
		const MapPoint &worldpos, Player *player);

	void RenderTrade(aui_Surface *surface, const MapPoint &position, const MapPoint &worldpos, Player *player);

	void RenderTileBorder(aui_Surface *surface, const MapPoint &position,
						  const MapPoint &worldpos, Player *player);

	MapPoint MapOffset(const MapPoint oldPosition);
	MapPoint PosWorldToPosRadar(const MapPoint worldpos);

	bool		m_displayUnits;
	bool		m_displayCities;
	bool		m_displayBorders;
	bool		m_displayOverlay;
	bool		m_filter;
	bool        m_displayTerrain;
	bool        m_displayTrade;
	bool		m_displayPolitical;
	bool		m_displayCapitols;
	bool		m_displayRelations;


	bool		m_isInteractive;

	aui_Surface	*m_mapSurface;
	MapPoint	*m_mapSize;
	COLOR		*m_mapOverlay;
	MapPoint	m_clickedCell;
	double		m_tilePixelWidth,
				m_tilePixelHeight;
	aui_Surface *m_tempSurface;
	uint8		*m_tempBuffer;
	RECT		m_mapViewRect;
	MapPoint	m_lastCenteredPoint;
	Unit		m_selectedCity;
	MapPoint	m_displayOffset[k_MAX_PLAYERS]; // Shifted x and y value
											    // for each player (Hotseat)
};

extern RadarMap *g_radarMap;

#endif
