
#pragma once
#ifndef __ARMY_POOL_H__
#define __ARMY_POOL_H__

#include "ObjPool.h"
#include "Army.h"

class Army;
class ArmyData;
class UnitDynamicArray;
class Unit;

class ArmyPool : public ObjPool
{
	
	
	
	

	
	
	
	

public:
	ArmyData *AccessArmy(const uint32 id)
	{
		return (ArmyData *)Access(id);
	}

	ArmyData *GetArmy(const uint32 id) const
	{
		return (ArmyData *)Get(id);
	}

	ArmyData *AccessArmy(const Army &id)
	{
		return (ArmyData *)Access(id);
	}

	ArmyData *GetArmy(const Army &id) const
	{
		return (ArmyData *)Get(id);
	}

	ArmyPool();
	ArmyPool(CivArchive &archive);
	~ArmyPool();

	void Serialize(CivArchive &archive);

	Army Create(UnitDynamicArray &units);
	Army Create(CellUnitList &units);
	Army Create(const Unit &u);
	Army Create();

	void Remove(Army army);
};

extern ArmyPool *g_theArmyPool;
	
#endif
