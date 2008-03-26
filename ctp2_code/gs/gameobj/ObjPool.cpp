//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The object pool
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ObjPool.h"

#include <algorithm>        // std::fill
#include "civarchive.h"

#define k_OBJPOOL_VERSION_MAJOR		0
#define k_OBJPOOL_VERSION_MINOR		0


ObjPool::ObjPool(uint32 it) 
:
    m_id_type   (it),
    m_nObjs     (0)
{
	std::fill(m_table, m_table + k_OBJ_POOL_TABLE_SIZE, (GameObj *) NULL);
}

ObjPool::~ObjPool()
{
	for (size_t i = 0 ; i < k_OBJ_POOL_TABLE_SIZE; ++i)
	{
		while (m_table[i])
		{
			Del(m_table[i]);
		}
	}
}

uint32 ObjPool::NewKey(uint32 t)
{
	ID  tmpId;
	
	do
	{
		tmpId.m_id = (t | (m_nObjs & k_ID_KEY_MASK));
		++m_nObjs;
	} while (IsValid(tmpId));   // exists already
	
	return tmpId.m_id;
}

void ObjPool::HackSetKey(uint32 k)
{
	m_nObjs = k;
}

bool ObjPool::IsValidKey(uint32 id, uint32 &val) const
{
	if (id == 0)
		return false;

	if ((id & k_ID_TYPE_MASK) != m_id_type )
		return false;

	val = id & k_OBJ_POOL_TABLE_SIZE_MASK;

	return true;
}

void ObjPool::Insert(GameObj *p)
{
	DPRINTF(k_DBG_GAMESTATE, ("ObjPool: Inserting object id %lx\n", p->m_id));
	Assert(p->m_id != 0);
	GameObj_Insert(&m_table[Key(p->m_id)], p); 
}

void ObjPool::Del(GameObj *p)
{
	DPRINTF(k_DBG_GAMESTATE, ("ObjPool: Deleting object id %lx\n", p->m_id));
	GameObj_Delete(&m_table[Key(p->m_id)], p->m_id); 
}

void ObjPool::Del(const ID &id) 
{
	DPRINTF(k_DBG_GAMESTATE, ("ObjPool: Deleting object id %lx\n", (uint32)id));
	GameObj_Delete(&m_table[Key(id.m_id)], id.m_id); 
}

sint32 ObjPool::Num(void) const
{
	sint32 count = 0;

	for (size_t i = 0; i < k_OBJ_POOL_TABLE_SIZE; ++i)
		if (m_table[i])
			count++ ;

	return count;
}

uint32 ObjPool_ObjPool_GetVersion(void)
{
	return (k_OBJPOOL_VERSION_MAJOR<<16 | k_OBJPOOL_VERSION_MINOR);
}

void ObjPool::Serialize(CivArchive &archive)
{
	if (archive.IsStoring()) 
	{
		archive.StoreChunk((uint8 *)&m_id_type, ((uint8 *)&m_nObjs)+sizeof(m_nObjs));
	}
	else
	{
		archive.LoadChunk((uint8 *)&m_id_type, ((uint8 *)&m_nObjs)+sizeof(m_nObjs));
	}
}
