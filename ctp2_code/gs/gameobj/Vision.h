//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Map visibility handling 
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - Corrected strange vision behaviour at the top row.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

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
	
//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------
	
	sint16 m_width;
	sint16 m_height;
	sint32 m_owner;
	sint16 m_xyConversion;
	BOOL   m_isYwrap;
	BOOL   m_amOnScreen;
	
//----------------------------------------------------------------------------
// Changing the order below this should not break anything.
//----------------------------------------------------------------------------
	
	

	
	
	
	uint16 **m_array;
	UnseenCellQuadTree *m_unseenCells;
	
	

	friend class NetVision;  
	friend class MapFile;
	
	
	
	

	Vision *m_mergeFrom; 
	BOOL m_revealedUnexplored; 

	void FillCircle
	(
		MapPoint const &			center, 
		double const				radius, 
		CIRCLE_OP					op,
		DynamicArray<MapPoint> *	removeadd = NULL
	);
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
