//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : String hash table handling.
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
// - Repaired memory leak or illegal access when loading a file with a 
//   different table size.
// - Repaired crash when computing the key of a zero-length string.
//
//----------------------------------------------------------------------------

#ifndef __STRING_HASH_H__
#define __STRING_HASH_H__

#include "CivArchive.h"

#ifdef COM_INTERFACE
#define ARCHIVE IC3CivArchive *
#else
#define ARCHIVE CivArchive &
#endif

template <class T> class StringHashNode {
public:
	
	T *m_obj;
	StringHashNode<T> *m_next;

	StringHashNode(const char *string, T *obj) {
		
		
		m_obj = obj;
		m_next = NULL;
	}
	StringHashNode(ARCHIVE archive)
	{
		Serialize(archive);
	}

	~StringHashNode() {



		if(m_obj)
			delete m_obj;
	}
	void Serialize(ARCHIVE archive) {
#ifndef COM_INTERFACE
		uint8 isPresent;
		if(archive.IsStoring()) {



			m_obj->Serialize(archive);

			isPresent = (m_next != NULL);
			archive << isPresent;
			if(m_next)
				m_next->Serialize(archive);
		} else {



			m_obj = new T(archive);
			archive >> isPresent;
			if(isPresent) {
				m_next = new StringHashNode<T>(archive);
			} else {
				m_next = NULL;
			}
		}
#endif
	}
};

template <class T> class StringHash {
protected:
	StringHashNode<T> **m_table;
	sint32 m_table_size;

public:
	StringHash(sint32 table_size);
	virtual ~StringHash();
	void Serialize(ARCHIVE archive);

	uint16 Key(const char *str);

	T * operator [] (const char *str) {
		return Access(str);
	}

	const T *Get(const char *str);
	T *Access(const char *str);
	virtual void Add(const char *str, T *obj);
	virtual void Add(T *obj);
	T *Del(const char *str);
	void Clear();
};

template <class T> StringHash<T>::StringHash(sint32 table_size)
{
	// Assert validity of % and cast to uint16.
	Assert(table_size > 0);
	Assert(table_size < 0x10000);
	m_table = new StringHashNode<T> *[table_size];
	m_table_size = table_size;
	for(sint32 i = 0; i < table_size; i++) {
		m_table[i] = NULL;
	}
}

template <class T> StringHash<T>::~StringHash()
{
	if(m_table) {
		for(sint32 i = 0; i < m_table_size; i++) {
			while(m_table[i]) {
				StringHashNode<T> *node = m_table[i];
				m_table[i] = node->m_next;
				delete node;
			}
		}
		delete [] m_table;
	}
}

template <class T> void StringHash<T>::Clear()
{
	if(m_table) {
		for(sint32 i = 0; i < m_table_size; i++) {
			while(m_table[i]) {
				StringHashNode<T> *node = m_table[i];
				m_table[i] = node->m_next;
				node->m_obj = NULL;
				delete node;
			}
		}
	}
}

template <class T> void StringHash<T>::Serialize(ARCHIVE archive)
{
#ifndef COM_INTERFACE
	uint8 isPresent;

	if(archive.IsStoring()) {
		archive << m_table_size;

		for(sint32 i = 0; i < m_table_size; i++) {
			isPresent = m_table[i] != NULL;
			archive << isPresent;
			if(isPresent) {
				m_table[i]->Serialize(archive);
			}
		}
	} else {
		Clear();
		delete [] m_table;
		archive >> m_table_size;

		m_table = new StringHashNode<T> *[m_table_size];
		for(sint32 i = 0; i < m_table_size; i++) {
			archive >> isPresent;
			if(isPresent) {
				m_table[i] = new StringHashNode<T>(archive);
			} else {
				m_table[i] = NULL;
			}
		}
	}
#endif
}

template <class T> uint16 StringHash<T>::Key(const char *str)
{
	uint16			key = 0;
	size_t const	l	= strlen(str);

	for (size_t i = 0; (i + 1) < l; ++i) 
	{
		key += uint16(tolower(str[i]) << 8 | tolower(str[i + 1])) + i;
	}

	if (l > 0) 
	{
		key += uint16(tolower(str[l - 1]) << 8) + (l - 1);
	}

	return static_cast<uint16>(key % m_table_size);
}

template <class T> const T *StringHash<T>::Get(const char *str)
{
	uint16 index = Key(str);
	StringHashNode<T> *node = m_table[index];
	while(node) {
		if(stricmp(node->m_obj->GetName(), str) == 0)
			return node->m_obj;
		node = node->m_next;
	}
	return NULL;
}

template <class T> T *StringHash<T>::Access(const char *str)
{
	uint16 index = Key(str);
	StringHashNode<T> *node = m_table[index];
	while(node) {
		if(stricmp(node->m_obj->GetName(), str) == 0)
			return node->m_obj;
		node = node->m_next;
	}
	return NULL;
}

template <class T> void StringHash<T>::Add(const char *str, T *obj)
{
	StringHashNode<T> *node = new StringHashNode<T>(str, obj);
	uint16 index = Key(str);
	node->m_next = m_table[index];
	m_table[index] = node;
}

template <class T> void StringHash<T>::Add(T *obj)
{
	Add(obj->GetName(), obj);
}

template <class T> T *StringHash<T>::Del(const char *str)
{
	uint16 index = Key(str);
	StringHashNode<T> *node = m_table[index];
	StringHashNode<T> *last = NULL;
	while(node) {
		if(stricmp(node->m_obj->GetName(), str) == 0) {
			if(last) {
				last->m_next = node->m_next;
			} else {
				m_table[index] = node->m_next;
			}
			T *obj = node->m_obj;
			node->m_obj = NULL;
			delete node;
			return obj;
		}
		last = node;
		node = node->m_next;
	}
	return NULL;
}

#endif
