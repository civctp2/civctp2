#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_HASH_H_
#define _NET_HASH_H_

#define k_NET_HASH_TABLE_SIZE 512

#include "pointerlist.h"

class NetHash
{
public:
	NetHash()
	{
		for(uint32 i = 0; i < k_NET_HASH_TABLE_SIZE; i++) {
			m_table[i] = NULL;
		}
		m_numEntries = 0;
		m_lowKey = k_NET_HASH_TABLE_SIZE;
		m_highKey = 0;
	}
	~NetHash()
	{
		Clear();
	}

	void Add(uint32 id);
	BOOL IsPresent(uint32 id);
	void Clear();
	void Remove(uint32 id);

	class Entry {
	public:
		Entry(uint32 id) : m_id(id) {}
		uint32 m_id;
	};
private:
	uint32 Key(uint32 id)
	{

		return (id % k_NET_HASH_TABLE_SIZE);
	}

	PointerList<Entry>* m_table[k_NET_HASH_TABLE_SIZE];
	uint32 m_numEntries;
	uint32 m_lowKey, m_highKey;
};

inline void
NetHash::Add(uint32 id)
{
	uint32 key = Key(id);
	if(!m_table[key]) {
		m_table[key] = new PointerList<Entry>;
	}
	m_table[key]->AddTail(new Entry(id));
	if(key < m_lowKey)
		m_lowKey = key;

	if(key > m_highKey)
		m_highKey = key;

	m_numEntries++;
}

inline void
NetHash::Remove(uint32 id)
{
	uint32 key = Key(id);




	Assert(m_table[key] != NULL);
	if(!m_table[key]) {
		return;
	}

	PointerList<Entry>::Walker walk(m_table[key]);

	while(walk.IsValid()) {
		if(walk.GetObj()->m_id == id) {

			walk.Remove();

			uint32 hkey = key;
			while(key == m_lowKey &&
				  key < k_NET_HASH_TABLE_SIZE &&
				  !m_table[key]) {
				key++;
				m_lowKey++;
			}
			while(hkey == m_highKey &&
				  hkey > 0 &&
				  !m_table[hkey]) {
				hkey--;
				m_highKey--;
			}
			return;
		}
		walk.Next();
	}
	Assert(FALSE);
}

inline BOOL
NetHash::IsPresent(uint32 id)
{
	uint32 key = Key(id);
	if(!m_table[key]) {
		return FALSE;
	}
	PointerList<Entry>::Walker walk(m_table[key]);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_id == id)
			return TRUE;
		walk.Next();
	}
	return FALSE;
}

inline void
NetHash::Clear()
{
	for(uint32 i = m_lowKey; i < m_highKey + 1; i++) {
		if(m_table[i]) {
			while(!m_table[i]->IsEmpty()) {
				Entry* ent = m_table[i]->RemoveHead();
				delete ent;
				m_numEntries--;
			}
			delete m_table[i];
			m_table[i] = NULL;
			if(m_numEntries <= 0)
				break;
		}
	}
	m_lowKey = k_NET_HASH_TABLE_SIZE;
	m_highKey = 0;
	m_numEntries = 0;
}

#else
class NetHash;
#endif
