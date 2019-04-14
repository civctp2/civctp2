//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Cell and Army text
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
// -None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - The army text now appears in the debug log. (13-Aug-2008 Martin Gühmann)
// - Added a debug player for the debug cell text, so that we can select whose
//   stuff is shown. (30-Dec-2018 Martin Gühmann)
// - Added constructors and a destructor for CellText, to fix memory leaks.
//   This should improve the handling, but it is still leaky, but obviously
//   only with cell text on. (03-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef GFX_OPTIONS_H__
#define GFX_OPTIONS_H__

class CellText;
class GraphicOptions;

#include "Army.h"             // Army
#include "ctp2_inttypes.h"    // uintN
class MapPoint;
template <class T> class AvlTree;

class CellText
{
public:
	uint32        m_key;
	uint8         m_color;
	MBCHAR *      m_text;

	CellText() : m_key(0), m_color(0), m_text(NULL) {};
	CellText(const CellText &copyMe) : m_key(copyMe.m_key), m_color(copyMe.m_color), m_text(NULL)
	{
		m_text = new MBCHAR[strlen(copyMe.m_text) + 1];
		strcpy(m_text, copyMe.m_text);
	}
	~CellText() { delete[] m_text; }
};

class GraphicsOptions
{
public:
	GraphicsOptions();
	~GraphicsOptions();

	static void Initialize(void);
	static void Cleanup(void);

	bool IsArmyTextOn(void) const { return m_armyTextOn; }
	void ArmyTextOn(void);
	void ArmyTextOff(void);

	bool AddTextToArmy(Army army, const char *text, const uint8 &colorMagnitude, const sint32 goalType = -1) const;
	void ResetArmyText(Army army);

	bool IsArmyNameOn(void) const { return m_armyNameOn; }
	void ArmyNameOn(void);
	void ArmyNameOff(void);

	bool IsCellTextOn(void) const { return m_cellTextOn; }
	void CellTextOn(PLAYER_INDEX debugCellPlayer);
	void CellTextOff(void);

	CellText * GetCellText(MapPoint const & pos);
	bool AddTextToCell(const MapPoint &pos, const char * text, const uint8 & colorMagnitude, const PLAYER_INDEX playerId);
	void ResetCellText(const MapPoint &pos);
	void ResetAllCellTexts();

private:
	bool                      m_armyTextOn;
	bool                      m_cellTextOn;
	bool                      m_armyNameOn;
	PLAYER_INDEX              m_debugCellPlayer;
	AvlTree<CellText *> *     m_cellAVL;
};

extern GraphicsOptions *      g_graphicsOptions;

#endif
