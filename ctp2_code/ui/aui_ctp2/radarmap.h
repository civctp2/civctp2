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
// - #01 Allow shifting the X and Y axis in the radar map with RMouse click
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

class aui_Surface;
class MapPoint;
class Player;

class RadarMap : public aui_Control, public PatternBase
{
public:
	RadarMap(AUI_ERRCODE * errCode, sint32 id, const MBCHAR * ldlBlock, ControlActionCallback * ActionFunc = NULL,
			void *cookie = NULL);
	RadarMap(AUI_ERRCODE * errCode, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height, const MBCHAR * pattern,
			ControlActionCallback * ActionFunc = NULL, void * cookie = NULL);

	virtual ~RadarMap();
	virtual AUI_ERRCODE	Resize(sint32 width, sint32 height);

	void InitCommonLdl(const MBCHAR * ldlBlock);
	void InitCommon();

	void ClearMapOverlay();
	void SetMapOverlayCell(const MapPoint & pos, COLOR color);

	void CalculateMetrics();

	void RenderMap(aui_Surface * surface);

	MapPoint ComputeCenteredMap(const MapPoint & pos, RECT * viewRect);
	MapPoint CenterMap(const MapPoint & pos);
	void Setup();
	void Update();
	void RedrawTile(const MapPoint & pos);

	aui_Surface * GetMapSurface() const { return m_mapSurface; }

	virtual AUI_ERRCODE DrawThis(aui_Surface * surface, sint32 x, sint32 y);

	virtual void MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void MouseRGrabInside(aui_MouseEvent * mouseData);

	virtual AUI_ERRCODE Idle();

	bool IsInteractive() const { return m_isInteractive; }
	void SetInteractive(bool status = true)
	{
		m_isInteractive = status;
		ShouldDraw();
	}

	void DisplayUnits(bool status = true)
	{
		m_displayUnits = status;
		Update();
		g_theProfileDB->SetDisplayUnits(status);
	}
	void DisplayCities(bool status = true)
	{
		m_displayCities = status;
		Update();
		g_theProfileDB->SetDisplayCities(status);
	}
	void DisplayBorders(bool status = true)
	{
		m_displayBorders = status;
		Update();
		g_theProfileDB->SetDisplayBorders(status);
	}
	void DisplayOverlay(bool status = true)
	{
		m_displayOverlay = status;
		Update();
	}
	void DisplayTrade(bool status = true)
	{
		m_displayTrade = status;
		Update();
		g_theProfileDB->SetDisplayTrade(status);
	}
	void DisplayTerrain(bool status = true) {
		m_displayTerrain = status;
		Update();
		g_theProfileDB->SetDisplayTerrain(status);
	}
	void DisplayPolitical(bool status = true)
	{
		m_displayPolitical = status;
		Update();
		g_theProfileDB->SetDisplayPolitical(status);
	}
	void DisplayCapitols(bool status = true)
	{
		m_displayCapitols = status;
		Update();
		g_theProfileDB->SetDisplayCapitols(status);
	}
	void DisplayRelations(bool status = true)
	{
		m_displayRelations = status;
		Update();
		g_theProfileDB->SetDisplayRelations(status);
	}

	bool IsDisplayUnits() const { return m_displayUnits; }
	bool IsDisplayCities() const { return m_displayCities; }
	bool IsDisplayBorders() const { return m_displayBorders; }
	bool IsDisplayOverlay() const { return m_displayOverlay; }
	bool IsDisplayTrade() const { return m_displayTrade; }
	bool IsDisplayTerrain() const { return m_displayTerrain; }
	bool IsDisplayPolitical() const { return m_displayPolitical; }
	bool IsDisplayCapitols() const { return m_displayCapitols; }
	bool IsDisplayRelations() const { return m_displayRelations; }

private:
	Player * GetVisiblePlayerToRender();
	void     UpdateMap(aui_Surface * surf, sint32 x, sint32 y);
	void     RenderViewRect(aui_Surface & surf, sint32 x, sint32 y);
	void     DoRedrawTile(const Player & player, const MapPoint & pos);

	void    RenderMapTileColor(const Player & player);
	Pixel16 MapTileColor(const Player & player, const MapPoint & position, const MapPoint & worldPos);
	void    FillColorMapBorders();
	COLOR   RadarTileRelationsColor(const MapPoint & position, const Player & player, sint32 unitOwner = -1);

	void    RenderTiles(aui_Surface & surface);
	void    RenderTile(aui_Surface & surface, const RECT & tileRectangle, Pixel16 * colorMap, bool isLand);
	void    RenderTileCrossing(aui_Surface & surface, const RECT & rect, Pixel16 * colorMap, bool isLand);

	void    RenderTileBorder(
				aui_Surface & surface, const MapPoint & position, const MapPoint & worldPos, const Player & player);
	uint8   RadarTileBorder(const Player & player, const MapPoint & position);
	Pixel16 RadarTileBorderColor(const MapPoint & position, const Player & player);
	void    RenderMapTileBorder(
				aui_Surface & surface, const MapPoint & position, uint8 borderFlags, Pixel16 borderColor);

	void    RenderCapitol(
				aui_Surface & surface, const MapPoint & position, const MapPoint & worldPos, const Player & player);

	void    RenderTrade(
				aui_Surface & surface, const MapPoint & position, const MapPoint & worldPos, const Player & player);
	void    RenderTradeRoute(aui_Surface & surface, const RECT & tileRectangle);

	MapPoint PosWorldToPosRadar(const MapPoint & worldPos);

	bool m_displayUnits;
	bool m_displayCities;
	bool m_displayBorders;
	bool m_displayOverlay;
	bool m_displayTerrain;
	bool m_displayTrade;
	bool m_displayPolitical;
	bool m_displayCapitols;
	bool m_displayRelations;

	bool m_isInteractive;

	aui_Surface * m_mapSurface;
	MapPoint    * m_mapSize;
	COLOR       * m_mapOverlay;
	Pixel16     * m_colorMap;
	double        m_tilePixelWidth;
	double        m_tilePixelHeight;
	RECT          m_mapViewRect;
	MapPoint      m_lastCenteredPoint;
	MapPoint      m_displayOffset[k_MAX_PLAYERS]; // Shifted x and y value for each player (Hotseat)
};

extern RadarMap *g_radarMap;

#endif
