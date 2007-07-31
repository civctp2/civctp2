









#pragma once
#ifndef _NET_GAMEOBJ_H_
#define _NET_GAMEOBJ_H_

#include "net_hash.h"

class GAMEOBJ;

class NetGameObj {
private:
	class GameObjRecord {
	public:
		GameObjRecord(GAMEOBJ *obj);
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

	void AddCreated(GAMEOBJ* obj);
	void ACKObject(uint32 id);
	void NAKObject(uint32 myId, uint32 realId);
	void CheckReceived(uint32 id);
	void FixKey(uint32 id);
};

#endif
