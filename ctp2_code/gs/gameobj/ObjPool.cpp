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
#include "civarchive.h"

ObjPool::ObjPool (const uint32 it)
:   m_id_type  (it),
    m_nObjs    (0)
//  m_table
{
	sint32 i;
	for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++) {
		m_table[i] = NULL;
	}
}

ObjPool::~ObjPool()
{
	sint32 i;
	for(i = 0 ; i < k_OBJ_POOL_TABLE_SIZE; i++) {
		while(m_table[i]) {
			Del(m_table[i]);
		}
	}
}

uint32 ObjPool::NewKey(const sint32 t) 
{
	sint32 tmp;

	ID tmpId;
	do {
		tmp = m_nObjs++;
		tmpId.m_id = (t | (tmp & k_ID_KEY_MASK));
	} while(IsValid(tmpId));
	
	return uint32(t | (tmp & k_ID_KEY_MASK));
}

void ObjPool::HackSetKey(const sint32 k)
{
	m_nObjs = k;
}

bool ObjPool::IsValidKey (const ID &id, uint32 &val) const
{
	if (!id.m_id)
		return false;

	if ((id.m_id & k_ID_TYPE_MASK) != m_id_type )
		return false;

	val = (id.m_id) & k_OBJ_POOL_TABLE_SIZE_MASK;

	return true;
}

bool ObjPool::IsValidKey (const uint32 id, uint32 &val) const
{
	if (!id)
		return false;

	if ((id & k_ID_TYPE_MASK) != m_id_type )
		return false;

	val = (id) & k_OBJ_POOL_TABLE_SIZE_MASK;

	return true;
}

void ObjPool::Insert(GameObj *p)
{
	DPRINTF(k_DBG_GAMESTATE, ("ObjPool: Inserting object id %lx\n", p->m_id));
	Assert(p->m_id != 0);
	GameObj_Insert(&m_table[Key(p->m_id)], p);
}

bool ObjPool::Del(GameObj *p)
{
	DPRINTF(k_DBG_GAMESTATE, ("ObjPool: Deleting object id %lx\n", p->m_id));
	return GameObj_Delete(&m_table[Key(p->m_id)], p->m_id);
}

bool ObjPool::Del(const ID &id)
{
	DPRINTF(k_DBG_GAMESTATE, ("ObjPool: Deleting object id %lx\n", (uint32)id));
	return GameObj_Delete(&m_table[Key(id.m_id)], id.m_id);
}


sint32 ObjPool::Num(void)
{
	sint32 count = 0;
	sint32 i;

	for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++)
		if(m_table[i])
			count++;

	return count;
}

uint32 ObjPool_ObjPool_GetVersion(void)
{
	return (k_OBJPOOL_VERSION_MAJOR<<16 | k_OBJPOOL_VERSION_MINOR);
}

void ObjPool::Serialize(CivArchive &archive) 
{
	if (archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_id_type, ((uint8 *)&m_nObjs)+sizeof(m_nObjs));
	} else {
		archive.LoadChunk((uint8 *)&m_id_type, ((uint8 *)&m_nObjs)+sizeof(m_nObjs));
	}
}
