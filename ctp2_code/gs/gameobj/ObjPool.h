//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __OBJ_POOL_H__
#define __OBJ_POOL_H__ 1

class ObjPool;

#include "ctp2_inttypes.h"          // uint32
#include "GameObj.h"
#include "globals.h"                // k_GAME_OBJ_TYPE_...
#include "ID.h"


size_t const    k_OBJ_POOL_TABLE_SIZE       = 1024;

uint32 const    k_OBJ_POOL_TABLE_SIZE_MASK  = k_OBJ_POOL_TABLE_SIZE - 1;
uint32 const    k_ID_TYPE_MASK              = 0xf0000000u;
uint32 const    k_ID_KEY_MASK               = ~k_ID_TYPE_MASK;
uint32 const    k_ID_MASK_SHIFT             = 28;

uint32 const    k_BIT_GAME_OBJ_TYPE_UNIT    = 
    (uint32) k_GAME_OBJ_TYPE_UNIT << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_POP     = 
    (uint32) k_GAME_OBJ_TYPE_POP << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE = 
    (uint32) k_GAME_OBJ_TYPE_TRADE_ROUTE << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_TRADE_OFFER = 
    (uint32) k_GAME_OBJ_TYPE_TRADE_OFFER << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT = 
    (uint32) k_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST = 
    (uint32) k_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_INSTALLATION = 
    (uint32) k_GAME_OBJ_TYPE_INSTALLATION << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_CIVILISATION = 
    (uint32) k_GAME_OBJ_TYPE_CIVILISATION << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_AGREEMENT = 
    (uint32) k_GAME_OBJ_TYPE_AGREEMENT << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_MESSAGE = 
    (uint32) k_GAME_OBJ_TYPE_MESSAGE << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_ARMY    = 
    (uint32) k_GAME_OBJ_TYPE_ARMY << k_ID_MASK_SHIFT;
uint32 const    k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB  = 
    (uint32) k_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT_DB << k_ID_MASK_SHIFT;

class CivArchive;

class ObjPool
{

//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	uint32 m_id_type;

protected:
	uint32 m_nObjs;

	GameObj *m_table[k_OBJ_POOL_TABLE_SIZE];

//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------

private:

	uint32 Key(const ID &id) const
	{
		return Key(id.m_id);
	}
	uint32 Key(uint32 id) const
	{
		Assert(id); 
		Assert((id & k_ID_TYPE_MASK)== m_id_type);

		return id & k_OBJ_POOL_TABLE_SIZE_MASK;
	}

public:

	ObjPool(uint32 it);
	virtual ~ObjPool();

	uint32  NewKey(uint32 t);
	bool    IsValidKey (const ID &id, uint32 &val) const
	{
		return IsValidKey(id.m_id, val);
	}
	bool    IsValidKey (uint32 id, uint32 &val) const;
	
	void    HackSetKey(uint32 k);
	uint32  HackGetKey() { return m_nObjs; }

	const GameObj * Get(const ID &id) const
	{
		return Get(id.m_id); 
	}

	const GameObj* Get(uint32 id) const
	{
		return GameObj_Get(m_table[Key(id)], id);
	}

	bool IsValid(const ID &id) const
	{
		return IsValid(id.m_id);
	}

	bool IsValid(uint32 id) const
	{
		uint32 val;
		if(!IsValidKey(id, val))
			return false;

		return GameObj_Valid(m_table[val], id);
	}

	GameObj* Access(const ID &id) const
	{
		return Access(id.m_id);
	}

	GameObj* Access(uint32 id) const
	{
		return GameObj_Access(m_table[Key(id)], id);
	}

	void    Insert(GameObj * p);
	void    Del(GameObj * p);
	void    Del(const ID & id);
	sint32  Num(void) const;
	virtual void Serialize(CivArchive &archive);

};

uint32 ObjPool_ObjPool_GetVersion(void);
#endif
