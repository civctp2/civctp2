










 
#pragma once
#ifndef __TILEINFO_H__
#define __TILEINFO_H__

#include "MapPoint.h"

#define k_NUM_TRANSITIONS	4

#define k_MASK_MEGA_NEXT	0x0F
#define k_MASK_MEGA_LAST	0xF0
#define k_SHIFT_MEGA_NEXT	0
#define k_SHIFT_MEGA_LAST	4

typedef uint16 TILEINDEX;

class GoodActor;
class CivArchive;

class TileInfo {
public:
	TileInfo();
	TileInfo(TileInfo *copy);
	TileInfo(CivArchive &archive);
	~TileInfo();

	uint8 GetTerrainType(void) const { return m_terrainType; }
	void SetTerrainType(uint8 type) { m_terrainType = type; }

	uint8 GetTransform() const { return m_transform; }
	void SetTransform(uint8 trans) { m_transform = trans; }


	TILEINDEX GetTileNum(void);

	void SetTileNum(TILEINDEX index) { m_tileNum = index; }

	sint16 GetRiverPiece(void) const { return m_riverPiece; }
	void SetRiverPiece(sint16 river) { m_riverPiece = (sint8)river; }

	sint32 SetGoodActor(sint32 index, MapPoint pos);
	GoodActor *GetGoodActor(void) {return m_goodActor;}
	sint32 HasGoodActor(void) {return (m_goodActor != NULL);}
	void DeleteGoodActor(void);

	void SetTransition(uint16 transNum, uint16 index) { m_transitions[transNum] = (sint8)index; }
	uint16 GetTransition(uint16 transNum) { return m_transitions[transNum]; }

	sint32 GetNextMega(void) { return (sint32)((m_megaInfo & k_MASK_MEGA_NEXT) >> k_SHIFT_MEGA_NEXT); }
	sint32 GetLastMega(void) { return (sint32)((m_megaInfo & k_MASK_MEGA_LAST) >> k_SHIFT_MEGA_LAST); }
	BOOL IsMega(void) { return (BOOL)(m_megaInfo != 0); }
	void SetNextMega(sint32 dir) { m_megaInfo = (m_megaInfo & k_MASK_MEGA_NEXT) | ((dir << k_SHIFT_MEGA_NEXT) & k_MASK_MEGA_NEXT);}
	void SetLastMega(sint32 dir) { m_megaInfo = (m_megaInfo & k_MASK_MEGA_LAST) | ((dir << k_SHIFT_MEGA_LAST) & k_MASK_MEGA_LAST);}

	uint8 GetMega() { return m_megaInfo; }
	void SetMega(uint8 m) { m_megaInfo = m; }

	void Serialize(CivArchive &archive);

private:
	
	
	sint8			m_riverPiece;
	uint8			m_megaInfo;
	uint8			m_terrainType;
	uint8           m_transform;

	TILEINDEX		m_tileNum;

	uint8			m_transitions[k_NUM_TRANSITIONS];
	
	

	
	
	
	GoodActor		*m_goodActor;
	
	
	

	friend class NetCellList;
	friend class NetCell;
	friend class NetUnseenCell;
};

#endif
