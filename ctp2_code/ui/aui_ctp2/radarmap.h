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
			Cookie cookie = nullptr);
	RadarMap(AUI_ERRCODE * errCode, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height, const MBCHAR * pattern,
			ControlActionCallback * ActionFunc = NULL, Cookie cookie = nullptr);

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

	// saveToProfile defaults to true for the normal case (the main
	// minimap's own toggle buttons, where the user's choice should
	// persist). Pass false for a RadarMap instance that isn't the user's
	// shared minimap - e.g. ScienceVictoryDialog embeds its own RadarMap
	// for a construction-coverage view and configures its display flags
	// in its constructor; since g_theProfileDB is a single global/shared
	// profile object (not per-RadarMap-instance), calling these with the
	// default would silently overwrite the user's real saved minimap
	// preference every time that unrelated dialog is constructed.
	void DisplayUnits(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayUnits = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayUnits(status);
	}
	void DisplayCities(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayCities = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayCities(status);
	}
	void DisplayBorders(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayBorders = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayBorders(status);
	}
	void DisplayOverlay(bool status = true)
	{
		m_radarProperties.m_displayOverlay = status;
		Update();
	}
	void DisplayTrade(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayTrade = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayTrade(status);
	}
	void DisplayTerrain(bool status = true, bool saveToProfile = true) {
		m_radarProperties.m_displayTerrain = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayTerrain(status);
	}
	void DisplayPolitical(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayPolitical = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayPolitical(status);
	}
	void DisplayCapitols(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayCapitols = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayCapitols(status);
	}
	void DisplayRelations(bool status = true, bool saveToProfile = true)
	{
		m_radarProperties.m_displayRelations = status;
		Update();
		if (saveToProfile)
			g_theProfileDB->SetDisplayRelations(status);
	}

	bool IsDisplayUnits() const { return m_radarProperties.m_displayUnits; }
	bool IsDisplayCities() const { return m_radarProperties.m_displayCities; }
	bool IsDisplayBorders() const { return m_radarProperties.m_displayBorders; }
	bool IsDisplayOverlay() const { return m_radarProperties.m_displayOverlay; }
	bool IsDisplayTrade() const { return m_radarProperties.m_displayTrade; }
	bool IsDisplayTerrain() const { return m_radarProperties.m_displayTerrain; }
	bool IsDisplayPolitical() const { return m_radarProperties.m_displayPolitical; }
	bool IsDisplayCapitols() const { return m_radarProperties.m_displayCapitols; }
	bool IsDisplayRelations() const { return m_radarProperties.m_displayRelations; }

	struct RadarProperties {
		bool    m_displayCities;
		bool    m_displayUnits;
		bool    m_displayTerrain;
		bool    m_displayBorders;
		bool    m_displayRelations;
		bool    m_displayPolitical;
		bool    m_displayTrade;
		bool    m_displayCapitols;
		bool    m_displayOverlay;
		COLOR * m_mapOverlay;
	};

private:
	Player * GetVisiblePlayerToRender();
	void     UpdateMap(aui_Surface & surface, sint32 x, sint32 y);
	void     RenderViewRect(aui_Surface & surface, sint32 x, sint32 y);
	void     DoRedrawTile(aui_Surface & surface, const Player & player, const MapPoint & pos);

	MapPoint PosWorldToPosRadar(const MapPoint & worldPos);

	RadarProperties m_radarProperties;

	bool m_isInteractive;

	aui_Surface * m_mapSurface;
	MapPoint    * m_mapSize;
	void        * m_mapTexture;
	double        m_tilePixelWidth;
	double        m_tilePixelHeight;
	RECT          m_mapViewRect;
	MapPoint      m_lastCenteredPoint;
	MapPoint      m_displayOffset[k_MAX_PLAYERS]; // Shifted x and y value for each player (Hotseat)
};

extern RadarMap *g_radarMap;

#endif
