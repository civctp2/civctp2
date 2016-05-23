//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Game object
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
// - Corrected return types
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif
#ifndef __GAME_OBJ_H__
#define __GAME_OBJ_H__ 1

class CivArchive;

#define k_GAMEOBJ_VERSION_MAJOR	0
#define k_GAMEOBJ_VERSION_MINOR	0

class GameObj
{
protected:
	GameObj *m_lesser, *m_greater;
	bool m_killMeSoon;

public:
	uint32 m_id; // Only serialized member
	uint8 m_isFromPool;

	GameObj(uint32 h);
	virtual ~GameObj();
	void operator delete(void *ptr);
	void operator delete(void *ptr, size_t size);
	void operator delete [] (void *ptr, size_t size);

	void SetKillMeSoon(){ m_killMeSoon = true; };
	bool KillMeSoon(){ return m_killMeSoon; };
	virtual void Serialize(CivArchive &archive) ;

	friend GameObj * GameObj_Access(GameObj *p, const uint32 i);
	friend const GameObj *GameObj_Get(GameObj *p, const uint32 i);
	friend bool GameObj_Valid(GameObj *p, uint32 id);

	friend void GameObj_Insert(GameObj **p, GameObj *ins);
	friend void GameObj_Delete(GameObj **p, uint32 i);
};

uint32 GameObj_GameObj_GetVersion(void) ;
GameObj * GameObj_Access(GameObj *p, const uint32 i);
const GameObj * GameObj_Get(GameObj *p, const uint32 i);
bool GameObj_Valid(GameObj *p, uint32 id);
void GameObj_Insert(GameObj **p, GameObj *ins);
void GameObj_Delete(GameObj **p, uint32 i);
void GameObj_Slurp(CivArchive &archive, GameObj *p) ;

#endif
