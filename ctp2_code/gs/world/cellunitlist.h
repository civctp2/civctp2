//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handling of a list of Units in the same cell
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
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are inactiThe blocks that are active for _MSC_VER value 
//       1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft C++ extensions marked for future GCC compiliation.
// - MovementTypeLand added, as suggested by NelsonAndBronte.
// - Option added to report sync errors only once.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif // _MSC_VER

#ifndef __CELL_UNIT_LIST_H__
#define __CELL_UNIT_LIST_H__

#include "Unit.h"
#include "gstypes.h"

template <class T> class DynamicArray;

class CivArchive;

#define k_CULF_IGNORES_ZOC 0x01
#define k_CULF_CANT_CAPTURE_CITY 0x02
#define k_CULF_CAN_SPACE_LAUNCH 0x04
#define k_CULF_CAN_SPACE_LAND 0x08
#define k_CULF_IN_SPACE       0x10


#define k_CULF_EXECUTED_THIS_TURN 0x80

class CellUnitList
{
public:
	Unit m_array[k_MAX_ARMY_SIZE];

	uint32 m_moveIntersection;
	uint8 m_flags;

	
	sint32 m_nElements;

	CellUnitList() {m_nElements = 0;}
	CellUnitList(const sint32 size) {Assert(size == k_MAX_ARMY_SIZE); m_nElements = 0;}
	CellUnitList(const DynamicArray<Unit> &copyme);
	void Serialize(CivArchive &archive);

	virtual sint32 Insert(Unit id);
	sint32 Del(const Unit &delme);
	sint32 DelIndex(const sint32 index);

	Unit &operator [] (const sint32 i) {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	const Unit & operator [] (const sint32 i) const {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	const Unit &Get(const sint32 i) const {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	Unit &Access(const sint32 i) {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}
	
	void Clear() { m_nElements = 0; }

	const sint32 Num() const { if(!this) return 0; else return m_nElements; }
	void KillList(CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);
	sint32 IsPresent(const Unit &u);

	BOOL CanEnter(const MapPoint &pos) const;
	BOOL HasWormholeProbe() const;
	virtual void GetPos(MapPoint &pos) const;
	virtual PLAYER_INDEX GetOwner() const;
	BOOL IsAtLeastOneMoveLand() const;
	BOOL IsAtLeastOneMoveWater() const;
	BOOL IsAtLeastOneMoveShallowWater() const;
	BOOL IsAtLeastOneMoveAir() const;
	BOOL IsAtLeastOneMoveSpace() const;
	BOOL IsAtLeastOneMoveMountain() const;
	BOOL IsEnemy(PLAYER_INDEX player) const;
	BOOL IsEnemy(Unit defender) const;
	BOOL IsEnemy(CellUnitList &defender) const;
	BOOL CanAtLeastOneCaptureCity() const;
    BOOL CanBeExpelled(); 

	BOOL IsVisible(PLAYER_INDEX player) const;
	bool GetTopVisibleUnitOfMoveType
	(
		PLAYER_INDEX const	looker,
		uint32 const		moveType,
		sint32 &			maxi,
		bool &				isResyncReported
	) const;
	Unit GetTopVisibleUnit
	(
		PLAYER_INDEX const	looker
	) const;
	
	BOOL CanBeSued() const;
	BOOL ExertsZOC() const;

    void ForceVisibleThisTurn(const PLAYER_INDEX to_me);
	double GetHPModifier();
	void DoVictoryEnslavement(sint32 origOwner);

	BOOL CanMoveIntoCell(const MapPoint &pos,
						 BOOL &zocViolation,
						 BOOL ignoreZoc,
						 BOOL &alliedCity);
	BOOL IsMovePointsEnough(const double cost) const;
	BOOL IsMovePointsEnough(const MapPoint &pos);
    BOOL GetMovementTypeAir() const; 
	bool GetMovementTypeLand() const; 
	BOOL CanBeCargoPodded() const;
	BOOL CanSpaceLand() const;
	BOOL CanSpaceLaunch() const;
	BOOL IsIgnoresZOC() const;

	void UpdateMoveIntersection();

	
	void ComputeStrength(double & attack, 
						 double & defense, 
						 double & ranged, 
						 sint16 & defend_unit_count, 
						 sint16 & ranged_unit_count,
						 double & land_bombard,
						 double & water_bombard,
						 double & air_bombard) const;

	
	double GetAverageHealthPercentage();
};

#endif
