#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __GFX_OPTIONS_H__
#define __GFX_OPTIONS_H__

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

	bool AddTextToArmy(Army army, const char *text, const uint8 &colorMagnitude);
	void ResetArmyText(Army army);

	uint32 PackCellAVLKey(MapPoint const & pos);

	bool IsCellTextOn(void) const { return m_cellTextOn; }
	void CellTextOn(void);
	void CellTextOff(void);

	CellText * GetCellText(MapPoint const & pos);
	bool AddTextToCell(const MapPoint &pos, const char * text, const uint8 & colorMagnitude);
	void ResetCellText(const MapPoint &pos);

private:
    bool                      m_armyTextOn;
    bool                      m_cellTextOn;
    AvlTree<CellText *> *     m_cellAVL;
};

extern GraphicsOptions *      g_graphicsOptions;

#endif
