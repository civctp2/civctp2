//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Cell and Army text
// Id           : $Id:$
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
	uint8	      m_color;
	MBCHAR *      m_text;
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
	void CellTextOn(void);
	void CellTextOff(void);

	CellText * GetCellText(MapPoint const & pos);
	bool AddTextToCell(const MapPoint &pos, const char * text, const uint8 & colorMagnitude);
	void ResetCellText(const MapPoint &pos);

private:
	bool                      m_armyTextOn;
	bool                      m_cellTextOn;
	bool                      m_armyNameOn;
	AvlTree<CellText *> *     m_cellAVL;
};

extern GraphicsOptions *      g_graphicsOptions;

#endif
