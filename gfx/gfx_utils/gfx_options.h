

#ifndef __GFX_OPTIONS_H__
#define __GFX_OPTIONS_H__

class MapPoint;
class Army;
template <class T> class AvlTree;

class CellText {
public:
	uint32		m_key;
	uint8		m_color;
	MBCHAR		*m_text;
};

class GraphicsOptions {
public:
	GraphicsOptions();
	~GraphicsOptions();

	
	static void Initialize(void);
	static void Cleanup(void);

	
	
	
	
	

	
	bool IsArmyTextOn(void) { return m_armyTextOn; }

	void ArmyTextOn(void);
	void ArmyTextOff(void);

	
	bool AddTextToArmy(const Army &army, const char *text, const uint8 &colorMagnitude);
	
	
	void ResetArmyText(const Army &army);
	
	
	
	
	inline bool IsCellTextOn(void) { return m_cellTextOn; }
	
	uint32 PackCellAVLKey(MapPoint &pos);

	
	
	CellText *GetCellText(MapPoint &pos);

	void CellTextOn(void);
	void CellTextOff(void);

	
	bool AddTextToCell(const MapPoint &pos, const char *text, const uint8 &colorMagnitude);
	
	
	void ResetCellText(const MapPoint &pos);

	

private:

	uint32					m_armyTextOn:1,
							m_cellTextOn:1,
							fill:30;

	AvlTree<CellText *>		*m_cellAVL;
};

extern GraphicsOptions		*g_graphicsOptions;

#endif
