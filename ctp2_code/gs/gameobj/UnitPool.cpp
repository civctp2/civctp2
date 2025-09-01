//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The unit pool
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
// _NO_GAME_WATCH
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Made government modified work here
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "UnitPool.h"

#include "civarchive.h"
#include "Globals.h"
#include "gstypes.h"
#include "UnitData.h"
#include "StrDB.h"


extern StringDB     *g_theStringDB;


#include "civ3_main.h"

UnitPool::UnitPool () : ObjPool (k_BIT_GAME_OBJ_TYPE_UNIT)
{
}

UnitPool::UnitPool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_UNIT)
{
	Serialize(archive);
}

Unit UnitPool::Create (
    const sint32 t,
    const PLAYER_INDEX owner,
    const MapPoint &pos,
    const Unit hc,
    UnitActor *actor)
{
	UnitData *ptr = NULL;
	Unit id(NewKey(k_BIT_GAME_OBJ_TYPE_UNIT));

	Assert(owner < PLAYER_INDEX_INVALID);

	sint32 trans_t = 0;
	g_theUnitDB->Get(t, g_player[owner]->GetGovernmentType())->GetTransType(trans_t);
	ptr = new UnitData(t, trans_t, id, owner, pos, hc, actor);

	Assert(ptr);

	Insert(ptr);
	return id;
}


Unit UnitPool::Create (
    const sint32 t,
    const PLAYER_INDEX owner,
    const MapPoint &actor_pos)
{
	UnitData *ptr = NULL;
	Unit id(NewKey(k_BIT_GAME_OBJ_TYPE_UNIT));

	Assert(owner < PLAYER_INDEX_INVALID);

	sint32 trans_t = 0;
	g_theUnitDB->Get(t, g_player[owner]->GetGovernmentType())->GetTransType(trans_t);
	ptr = new UnitData(t, trans_t, id, owner, actor_pos);

	Assert(ptr);

	Insert(ptr);
	return id;
}

void UnitPool::Serialize(CivArchive &archive)
{
	UnitData *unitData;

	sint32    count = 0;

#define UNITPOOL_MAGIC 0xA4D27DED

	CHECKSERIALIZE

	if (archive.IsStoring())
	{
		archive.PerformMagic(UNITPOOL_MAGIC);
		ObjPool::Serialize(archive);

		for(size_t i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++)
			if(m_table[i])
				count++;

		archive<<count;
		for(size_t i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++)
			if (m_table[i])
				((UnitData *)(m_table[i]))->Serialize(archive);
	}
	else
	{
		archive.TestMagic(UNITPOOL_MAGIC);
		ObjPool::Serialize(archive);

		archive>>count;
		for(sint32 i=0; i < count; i++)
		{
			unitData = new UnitData(archive);
			Insert(unitData);
		}
	}
}

void UnitPool::RebuildQuadTree()
{
	for(size_t i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++)
	{
		if(m_table[i])
		{
			((UnitData *)(m_table[i]))->RebuildQuadTree();
		}
	}
}

uint32 UnitPool_UnitPool_GetVersion(void)
{
	return (k_UNITPOOL_VERSION_MAJOR<<16 | k_UNITPOOL_VERSION_MINOR);
}

const UnitRecord *UnitPool::GetDBRec(const Unit id) const
{
	Player *    player  = g_player[id.GetOwner()];

	if(player)
	{
		return g_theUnitDB->Get(id.GetType(), player->GetGovernmentType());
	}
	else
	{
		return g_theUnitDB->Get(id.GetType());
	}
}
