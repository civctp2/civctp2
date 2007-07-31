









#pragma once
#ifndef _VISION_H_
#define _VISION_H_

#define k_EXPLORED_BIT 0x8000
#define k_VISIBLE_REFERENCE_MASK (~(k_EXPLORED_BIT))

#include "MapPoint.h"

class CivArchive;
class UnseenCellQuadTree;
class UnseenCellCarton;
template <class T> class DynamicArray;
class UnseenCell;

enum CIRCLE_OP {
	CIRCLE_OP_ADD,
	CIRCLE_OP_SUBTRACT,
	CIRCLE_OP_ADD_RADAR,
	CIRCLE_OP_MERGE
};

class Vision
{
private:
	
	
	sint16 m_width;
	sint16 m_height;
	sint32 m_owner;
	sint16 m_xyConversion;
	BOOL   m_isYwrap;
	BOOL   m_amOnScreen;
	
	

	
	
	
	uint16 **m_array;
	UnseenCellQuadTree *m_unseenCells;
	
	

	friend class NetVision;  
	friend class MapFile;
	
	
	
	

	Vision *m_mergeFrom; 
	BOOL m_revealedUnexplored; 
	
	void FillCircle(sint32 xc, sint32 yc, double r, CIRCLE_OP op,
					DynamicArray<MapPoint> *removeadd = NULL);
	BOOL CheckWrap(MapPoint &pos, const MapPoint &center);
	void DoFillCircleOp(const MapPoint &pos, CIRCLE_OP op, 
						DynamicArray<MapPoint> *removeadd);

#if 0
	void RasterFill(sint32 xc, sint32 yc, sint32 x, sint32 y,
			   CIRCLE_OP op);
#endif

public:
	Vision(sint32 owner, BOOL amOnScreen = FALSE);
	~Vision();

	void Copy(const Vision *copy);

	void AddExplored(MapPoint pos, double radius);
    void SetTheWholeWorldExplored();
	void SetTheWholeWorldUnexplored();
	void SetTheWholeWorldUnseen();
	BOOL IsExplored(MapPoint pos) const;

	void AddRadar(MapPoint pos, double radius);

	void AddVisible(MapPoint pos, double radius, BOOL &revealed_unexplored, 
					DynamicArray<MapPoint> *removeadd = NULL);
	void RemoveVisible(MapPoint pos, double radius,
					   DynamicArray<MapPoint> *removeadd = NULL);
	BOOL IsVisible(MapPoint pos) const;

	
	
	
	BOOL GetLastSeen(const MapPoint &point, UnseenCellCarton &ucell);

	inline void Convert(MapPoint &pos) const {
		pos.y += pos.x;
		while(pos.y >= m_height) {
			pos.y -= m_height;
		}
	}

	inline void Unconvert(MapPoint &pos) const {
		while(pos.x < 0)
			pos.x += m_width;
		while(pos.x >= m_width)
			pos.x -= m_width;
		
		pos.y -= pos.x;
		while(pos.y < 0) {
			pos.y += m_height;
		}
	}


	void MergeMap(Vision *src) ;									

	void AddUnseen(const MapPoint &point);
	void AddUnseen(UnseenCell *ucell); 
	void GetUnseenCellList(DynamicArray<UnseenCellCarton> &array);

	void Clear();

	
	void CopyCircle(Vision *src, const MapPoint &center, sint32 radius);
	BOOL MergePoint(sint32 x, sint32 y);
	void ModifyPoint(Vision *src, sint32 x, sint32 y);

	void Serialize(CivArchive &archive);

	void DeleteUnseenCells();
	void ClearUnseen();

	sint32 GetOwner() const { return m_owner; }
};

#endif
