//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Multiplayer object packet handling.
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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_GAMEOBJ_H_
#define _NET_GAMEOBJ_H_

#include "net_hash.h"

class GameObj;

class NetGameObj {
private:
	class GameObjRecord {
	public:
		GameObjRecord(GameObj *obj);
		uint32 m_id;
	};

	class LimboID {
	public:
		LimboID(uint32 id) : m_id(id) {}
		uint32 m_id;
	};

	PointerList<GameObjRecord> *m_created;
	PointerList<LimboID> *m_limbo;
	NetHash m_createdHash;

	void TheReaper(void);
	void KillObject(uint32 id);
public:
	NetGameObj();
	~NetGameObj();

	void AddCreated(GameObj* obj);
	void ACKObject(uint32 id);
	void NAKObject(uint32 myId, uint32 realId);
	void CheckReceived(uint32 id);
	void FixKey(uint32 id);
};

#endif
