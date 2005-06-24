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
#ifndef __GAME_OBJ_H__
#define __GAME_OBJ_H__ 1

class CivArchive ;

#define k_GAMEOBJ_VERSION_MAJOR	0									
#define k_GAMEOBJ_VERSION_MINOR	0									

typedef class GAMEOBJ GameObj; 

class GAMEOBJ { 

protected:
	GameObj *m_lesser, *m_greater; 

public:
	uint32 m_id; 
	uint8 m_isFromPool; 

	GAMEOBJ(const uint32 h); 
	virtual ~GAMEOBJ();
	void operator delete(void *ptr);
	void operator delete(void *ptr, size_t size);
	void operator delete [] (void *ptr, size_t size);

	virtual void Serialize(CivArchive &archive) ;

	friend GameObj * GameObj_Access(GameObj *p, const uint32 i);
	friend const GameObj *GameObj_Get(GameObj *p, const uint32 i);
	friend const sint32 GameObj_Valid(GameObj *p, const uint32 id);
   
	friend void GameObj_Insert(GameObj **p, GameObj *ins);
	friend sint32 GameObj_Delete(GameObj **p, const uint32 i);
}; 


uint32 GameObj_GameObj_GetVersion(void) ;
GameObj * GameObj_Access(GameObj *p, const uint32 i);
const GameObj * GameObj_Get(GameObj *p, const uint32 i);
const sint32 GameObj_Valid(GameObj *p, const uint32 id);
void GameObj_Insert(GameObj **p, GameObj *ins);
sint32 GameObj_Delete(GameObj **p, const uint32 i);
void GameObj_Slurp(CivArchive &archive, GameObj *p) ;

#endif
