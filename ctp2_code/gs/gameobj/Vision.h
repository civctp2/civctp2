//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Map visibility handling 
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - Corrected strange vision behaviour at the top row.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
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
	sint16 m_xyConversion;   // Unused
	BOOL   m_isYwrap;        // Unused
	BOOL   m_amOnScreen;
	
//----------------------------------------------------------------------------
// Changing the order below this should not break anything.
//----------------------------------------------------------------------------
	
	uint16 **m_array;
	UnseenCellQuadTree *m_unseenCells;
	
	

	friend class NetVision;  
	friend class MapFile;
	
	
	
	

	Vision *m_mergeFrom; 
	bool m_revealedUnexplored;

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
	Vision(sint32 owner, bool amOnScreen = false);
	~Vision();

	void Copy(const Vision *copy);

	void AddExplored(MapPoint pos, double radius);
    void SetTheWholeWorldExplored();
	void SetTheWholeWorldUnexplored();
	void SetTheWholeWorldUnseen();
	bool IsExplored(MapPoint pos) const;

	void AddRadar(MapPoint pos, double radius);

	void AddVisible(MapPoint pos, double radius, bool &revealed_unexplored, 
					DynamicArray<MapPoint> *removeadd = NULL);
	void RemoveVisible(MapPoint pos, double radius,
					   DynamicArray<MapPoint> *removeadd = NULL);
	bool IsVisible(MapPoint pos) const;

	
	
	
	bool GetLastSeen(const MapPoint &point, UnseenCellCarton &ucell);

	void Convert(MapPoint &pos) const 
    {
        int l_ResultY   = pos.y + pos.x;
		while (l_ResultY >= m_height) 
        {
			l_ResultY  -= m_height;
		}
        pos.y = static_cast<sint16>(l_ResultY);
	}

	void Unconvert(MapPoint & pos) const 
    {
        int l_ResultX   = pos.x;

        while (l_ResultX < 0)
        {
			l_ResultX += m_width;
        }
		while (l_ResultX >= m_width)
        {
			l_ResultX -= m_width;
        }
        pos.x = static_cast<sint16>(l_ResultX);

        int l_ResultY   = pos.y - l_ResultX;
		while (l_ResultY < 0) 
        {
			l_ResultY += m_height;
		}
        pos.y = static_cast<sint16>(l_ResultY);
	}


	void MergeMap(Vision *src) ;									

	void AddUnseen(const MapPoint &point);
	void AddUnseen(UnseenCell *ucell); 
	void GetUnseenCellList(DynamicArray<UnseenCellCarton> &array);

	void Clear();

	
	void CopyCircle(Vision *src, const MapPoint &center, sint32 radius);
	bool MergePoint(sint32 x, sint32 y);
	void ModifyPoint(Vision *src, sint32 x, sint32 y);

	void Serialize(CivArchive &archive);

	void DeleteUnseenCells();
	void ClearUnseen();

	sint32 GetOwner() const { return m_owner; }
};

#endif
