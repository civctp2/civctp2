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
//
/// \file   Vision.h
/// \brief  Handling of tile visibility (declarations)

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef VISION_H_
#define VISION_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Vision;

#define k_EXPLORED_BIT 0x8000
#define k_VISIBLE_REFERENCE_MASK (~(k_EXPLORED_BIT))

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "MapPoint.h"
#include "TradeRoute.h"

class CivArchive;
class UnseenCellQuadTree;
class UnseenCellCarton;
template <class T> class DynamicArray;
class UnseenCell;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class Vision
{
private:
	/// Operation to perform
	enum CIRCLE_OP
	{
		CIRCLE_OP_ADD,          ///< add to normal vision
		CIRCLE_OP_SUBTRACT,     ///< remove from normal vision
		CIRCLE_OP_ADD_RADAR,    ///< add to radar vision
		CIRCLE_OP_MERGE         ///< merge 2 visions
	};

//----------------------------------------------------------------------------
// Sort the member variables by size to save memory
// Before sorting the size was 32 bytes
// After sorting and BOOL to bool conversion 24 bytes
//----------------------------------------------------------------------------

	// The pointers 4 bytes in a 32 bit and 8 bytes in a 64 bit program
	uint16 **m_array;
	UnseenCellQuadTree *m_unseenCells;
	Vision *m_mergeFrom;

	// 4 bytes
	sint32 m_owner;

	// 2 bytes
	sint16 m_width; // Size 32 bytes
	sint16 m_height;
	sint16 m_xyConversion;   // Unused

	// 1 byte
	bool   m_isYwrap;
	bool   m_amOnScreen;

//----------------------------------------------------------------------------
// End member variable section
//----------------------------------------------------------------------------

	friend class NetVision;
	friend class MapFile;

	void FillCircle
	(
		MapPoint const &			center,
		double const				radius,
		CIRCLE_OP					op,
		DynamicArray<MapPoint> *	removeadd = NULL
	);
	void DoFillCircleOp(const MapPoint &pos, CIRCLE_OP op,
						DynamicArray<MapPoint> *removeadd);

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

	void AddVisible(MapPoint pos, double radius,
					DynamicArray<MapPoint> *removeadd = NULL);
	void RemoveVisible(MapPoint pos, double radius,
					   DynamicArray<MapPoint> *removeadd = NULL);
	bool IsVisible(MapPoint pos) const;

	bool GetLastSeen(const MapPoint &point, UnseenCellCarton &ucell) const;

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

	void MergeMap(Vision *src);

	void AddUnseen(const MapPoint &point);
	void AddUnseen(UnseenCell *ucell);
	void UpdateUnseen(const MapPoint &point);
	void GetUnseenCellList(DynamicArray<UnseenCellCarton> &array);

	void RevealTradeRouteState(const MapPoint &iso);
	void RevealTradeRouteTiles(TradeRoute route);
	void RevealTradeRouteCities(TradeRoute route);
	void RevealCity(Unit city);
	void Clear();

	void CopyCircle(Vision *src, const MapPoint &center, sint32 radius);
	bool MergePoint(sint32 x, sint32 y);
	void ModifyPoint(Vision *src, sint32 x, sint32 y);

	void Serialize(CivArchive &archive);

	void DeleteUnseenCells();
	void ClearUnseen();

	sint32 GetOwner() const { return m_owner; }

	void SetAmOnScreen(bool amOnScreen) { m_amOnScreen = amOnScreen; };

#if 0
	void PrintSizeOfClass()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Size of Vision class:\n"));
		DPRINTF(k_DBG_AI, ("Vision:                 %d\n", sizeof(Vision)));
		DPRINTF(k_DBG_AI, ("m_width:                %d\n", sizeof(m_width)));
		DPRINTF(k_DBG_AI, ("m_height:               %d\n", sizeof(m_height)));
		DPRINTF(k_DBG_AI, ("m_owner:                %d\n", sizeof(m_owner)));
		DPRINTF(k_DBG_AI, ("m_xyConversion:         %d\n", sizeof(m_xyConversion)));
		DPRINTF(k_DBG_AI, ("m_isYwrap:              %d\n", sizeof(m_isYwrap)));
		DPRINTF(k_DBG_AI, ("m_amOnScreen:           %d\n", sizeof(m_amOnScreen)));
		DPRINTF(k_DBG_AI, ("m_array:                %d\n", sizeof(m_array)));
		DPRINTF(k_DBG_AI, ("m_unseenCells:          %d\n", sizeof(m_unseenCells)));
		DPRINTF(k_DBG_AI, ("m_mergeFrom:            %d\n", sizeof(m_mergeFrom)));
		DPRINTF(k_DBG_AI, ("\n"));
	}

	void PrintData()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Data of Vision class:\n"));
		DPRINTF(k_DBG_AI, ("m_width:                 %d\n", m_width));
		DPRINTF(k_DBG_AI, ("m_height:                %d\n", m_height));
		DPRINTF(k_DBG_AI, ("m_owner:                 %d\n", m_owner));
		DPRINTF(k_DBG_AI, ("m_xyConversion:          %d\n", m_xyConversion));
		DPRINTF(k_DBG_AI, ("m_isYwrap:               %d\n", m_isYwrap));
		DPRINTF(k_DBG_AI, ("m_amOnScreen:            %d\n", m_amOnScreen));
	//	DPRINTF(k_DBG_AI, ("m_array:                 %d\n", m_array));
	//	DPRINTF(k_DBG_AI, ("m_unseenCells:           %d\n", m_unseenCells));
	//	DPRINTF(k_DBG_AI, ("m_mergeFrom:             %d\n", m_mergeFrom));
		DPRINTF(k_DBG_AI, ("\n"));
	}
#endif
};

#endif
