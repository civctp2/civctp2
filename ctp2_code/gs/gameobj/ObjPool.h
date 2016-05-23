//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
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
// - #pragma once commented out.
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __OBJ_POOL_H__
#define __OBJ_POOL_H__ 1

#include "GameObj.h"
#include "ID.h"

#define k_OBJ_POOL_TABLE_SIZE 1024
#define k_OBJ_POOL_TABLE_SIZE_MASK 0x000003ff

#define k_ID_TYPE_MASK 0xf0000000
#define k_ID_KEY_MASK (~k_ID_TYPE_MASK)

#define k_ID_MASK_SHIFT 28

#define k_BIT_GAME_OBJ_TYPE_UNIT (k_GAME_OBJ_TYPE_UNIT << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_POP (k_GAME_OBJ_TYPE_POP << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE (k_GAME_OBJ_TYPE_TRADE_ROUTE << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_TRADE_OFFER (k_GAME_OBJ_TYPE_TRADE_OFFER << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT (k_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST (k_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_INSTALLATION (k_GAME_OBJ_TYPE_INSTALLATION << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_CIVILISATION (k_GAME_OBJ_TYPE_CIVILISATION << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_AGREEMENT (k_GAME_OBJ_TYPE_AGREEMENT << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_MESSAGE (k_GAME_OBJ_TYPE_MESSAGE << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_ARMY (k_GAME_OBJ_TYPE_ARMY << k_ID_MASK_SHIFT)
#define k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB (k_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT_DB << k_ID_MASK_SHIFT)

#define k_OBJPOOL_VERSION_MAJOR		0
#define k_OBJPOOL_VERSION_MINOR		0

class CivArchive ;

class ObjPool {


	uint32 m_id_type;

protected:
	sint32 m_nObjs;






	GameObj *m_table[k_OBJ_POOL_TABLE_SIZE];


private:

	inline uint32 Key(const ID &id) const {

		Assert(id.m_id);
		Assert((id.m_id & k_ID_TYPE_MASK)== m_id_type);

		return  id.m_id & k_OBJ_POOL_TABLE_SIZE_MASK;
	}
	inline uint32 Key(const uint32 id) const {
		Assert(id);
		Assert((id & k_ID_TYPE_MASK)== m_id_type);

		return id & k_OBJ_POOL_TABLE_SIZE_MASK;
	}

public:

	ObjPool(const uint32 it);
	virtual ~ObjPool();

	uint32 NewKey(const sint32 t);
	sint32 IsValidKey (const ID &id, uint32 &val) const;
	sint32 IsValidKey (const uint32 id, uint32 &val) const;

	void   HackSetKey(const sint32 k);
	sint32 HackGetKey() { return m_nObjs; }







	inline const GameObj* Get(const ID &id) const
	{
		return GameObj_Get(m_table[Key(id)], id.m_id);
	}

	inline const GameObj* Get(const uint32 id) const
	{
		return GameObj_Get(m_table[Key(id)], id);
	}










	inline sint32 IsValid(const ID &id) const
	{
		uint32 val;

		if (!IsValidKey(id, val))
		return FALSE;

		return GameObj_Valid(m_table[val], id.m_id);
	}

	inline sint32 IsValid(const uint32 id) const
	{
		uint32 val;
		if(!IsValidKey(id, val))
			return FALSE;

		return GameObj_Valid(m_table[val], id);
	}







	inline GameObj* Access(const ID &id) const
	{
		return GameObj_Access(m_table[Key(id)], id.m_id);
	}

	inline GameObj* Access(const uint32 id) const
	{
		return GameObj_Access(m_table[Key(id)], id);
	}

	void Insert(GameObj *p);
	sint32 Del(GameObj *p);
	sint32 Del(const ID &id);
	sint32 Num(void) ;
	virtual void Serialize(CivArchive &archive);

};

uint32 ObjPool_ObjPool_GetVersion(void) ;
#endif
