

#include "c3.h"

#include "ArmyPool.h"
#include "Army.h"
#include "ArmyData.h"
#include "CivArchive.h"
#include "Globals.h"

ArmyPool::ArmyPool() : ObjPool(k_BIT_GAME_OBJ_TYPE_ARMY)
{
}

ArmyPool::ArmyPool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_ARMY)
{
	Serialize(archive);
}

ArmyPool::~ArmyPool()
{
}

Army ArmyPool::Create(UnitDynamicArray &units)
{
	ArmyData *newData;
	Army newArmy(NewKey(k_BIT_GAME_OBJ_TYPE_ARMY));

	newData = new ArmyData(newArmy, units);
	Insert(newData);
	return newArmy;
}

Army ArmyPool::Create(CellUnitList &units)
{
	ArmyData *newData;
	Army newArmy(NewKey(k_BIT_GAME_OBJ_TYPE_ARMY));

	newData = new ArmyData(newArmy, units);
	Insert(newData);
	return newArmy;
}

Army ArmyPool::Create(const Unit &u)
{
	ArmyData *newData;
	Army newArmy(NewKey(k_BIT_GAME_OBJ_TYPE_ARMY));

	Unit bleah(u);
	newData = new ArmyData(newArmy, bleah);
	Insert(newData);
	return newArmy;
}
	
Army ArmyPool::Create()
{
	ArmyData *newData;
	Army newArmy(NewKey(k_BIT_GAME_OBJ_TYPE_ARMY));
	newData = new ArmyData(newArmy);
	Insert(newData);
	return newArmy;
}

void ArmyPool::Remove(Army army)
{
	Del(army);
}

void ArmyPool::Serialize(CivArchive &archive)
{
	sint32 count, i;

#define ARMYPOOL_MAGIC 0xBeefCafe
	if(archive.IsStoring()) {
		archive.PerformMagic(ARMYPOOL_MAGIC);
		ObjPool::Serialize(archive);

		count = 0;
		for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) {
			if(m_table[i])
				count++;
		}

		archive << count;
		for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) {
			if(m_table[i])
				((ArmyData*)(m_table[i]))->Serialize(archive);
		}
	} else {
		archive.TestMagic(ARMYPOOL_MAGIC);
		ObjPool::Serialize(archive);
		archive >> count;
		for(i = 0; i < count; i++) {
			ArmyData *data = new ArmyData(archive);
			Insert(data);
		}
	}
}
