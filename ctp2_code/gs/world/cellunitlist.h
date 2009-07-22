//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handling of a list of Units in the same cell
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft extensions removed.
// - MovementTypeLand added, as suggested by NelsonAndBronte.
// - Option added to report sync errors only once.
// - Standartized army strength computation. (30-Apr-2008 Martin Gühmann)
// - Position strength can now be calculated independently from position. (13-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// \file CellUnitList.cpp
/// \brief List of units (e.g. army) at the same location of the map.

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef CELL_UNIT_LIST_H__
#define CELL_UNIT_LIST_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

class CellUnitList;

#define k_CULF_IGNORES_ZOC          0x01
#define k_CULF_CANT_CAPTURE_CITY    0x02
#define k_CULF_CAN_SPACE_LAUNCH     0x04
#define k_CULF_CAN_SPACE_LAND       0x08
#define k_CULF_IN_SPACE             0x10
#define k_CULF_EXECUTED_THIS_TURN   0x80

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "Unit.h"
#include "gstypes.h"

template <class T> class DynamicArray;

class CivArchive;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class CellUnitList
{
public:
	CellUnitList() {m_nElements = 0;}
	CellUnitList(const sint32 size) {Assert(size == k_MAX_ARMY_SIZE); m_nElements = 0;}
	CellUnitList(const DynamicArray<Unit> &copyme);
	void Serialize(CivArchive &archive);

	virtual bool Insert(Unit id);
	sint32 Del(const Unit &delme);
	sint32 DelIndex(const sint32 index);

	Unit &operator [] (const sint32 i)
	{
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	const Unit & operator [] (const sint32 i) const
	{
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	const Unit &Get(const sint32 i) const
	{
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	Unit &Access(const sint32 i)
	{
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}
	
	void Clear() { m_nElements = 0; }

	sint32 Num() const { return m_nElements; }
	void KillList(CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);
	bool IsPresent(const Unit &u);

	bool CanEnter(const MapPoint &pos) const;
	bool HasWormholeProbe() const;
	virtual void GetPos(MapPoint &pos) const;
	virtual PLAYER_INDEX GetOwner() const;
	bool IsAtLeastOneMoveLand() const;
	bool IsAtLeastOneMoveWater() const;
	bool IsAtLeastOneMoveShallowWater() const;
	bool IsAtLeastOneMoveAir() const;
	bool IsAtLeastOneMoveSpace() const;
	bool IsAtLeastOneMoveMountain() const;
	bool IsEnemy(PLAYER_INDEX player) const;
	bool IsEnemy(Unit defender) const;
	bool IsEnemy(CellUnitList &defender) const;
	bool CanAtLeastOneCaptureCity() const;
	bool CanBeExpelled(); 

	bool IsVisible(PLAYER_INDEX player) const;
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
	
	bool CanBeSued() const;
	bool ExertsZOC() const;

	void ForceVisibleThisTurn(const PLAYER_INDEX to_me);
	double GetHPModifier() const;
	void DoVictoryEnslavement(sint32 origOwner, sint32 enemySize);

	bool CanMoveIntoCell(const MapPoint &pos,
						 bool &zocViolation,
						 bool ignoreZoc,
						 bool &alliedCity);
	bool IsMovePointsEnough(const double cost) const;
	bool IsMovePointsEnough(const MapPoint &pos) const;
	bool GetMovementTypeAir() const;
	bool HighestMoveBonusUnit(sint32 & value) const;
	bool GetMovementTypeLand() const;
	bool CanBeCargoPodded() const;
	bool CanSpaceLand() const;
	bool CanSpaceLaunch() const;
	bool IsIgnoresZOC() const;

	void UpdateMoveIntersection();

	
	void ComputeStrength(double & attack,
						 double & defense,
						 double & ranged,
						 sint16 & defend_unit_count,
						 sint16 & ranged_unit_count,
						 double & land_bombard,
						 double & water_bombard,
						 double & air_bombard,
						 double & total_value,
						 const bool terrainIndependent) const;


	double  GetAverageHealthPercentage() const;
	uint8   GetFlags() const
	{
		return m_flags;
	};

protected:
	/// List of units - implemented as a fixed size array
	Unit    m_array[k_MAX_ARMY_SIZE];
	/// Common move properties of all units
	uint32  m_moveIntersection;
	/// Properties of the group - see the k_CULF_... constants.
	uint8   m_flags;
	/// Number of units (valid entries in m_array)
	sint32  m_nElements;
};

#endif
