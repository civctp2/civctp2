

#pragma once
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
	BOOL GetTopVisibleUnitOfMoveType(const sint32 looking_player, const uint32 move, sint32 &maxi) const;
	Unit GetTopVisibleUnit(PLAYER_INDEX looker) const;
	
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
