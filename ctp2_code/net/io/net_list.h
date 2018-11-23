#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef _NET_LIST_H_
#define _NET_LIST_H_

#if 0

template <class T> class NetList {
public:
	class NetListNode {
	public:
		NetListNode(T* obj) :
			m_next(NULL),
			m_prev(NULL),
			m_obj(obj)
		{
		};

		~NetListNode() {
			if(m_next)
				m_next->m_prev = m_prev;
			if(m_prev)
				m_prev->m_next = m_next;
		}

		T* GetObj() { return m_obj; };

		T* m_obj;
		NetListNode* m_next;
		NetListNode* m_prev;
	};

	NetList() : m_head(NULL),
				m_tail(NULL)
	{
	};

	~NetList()
	{
		while(m_head) {
			NetListNode* node = m_head;
			m_head = m_head->m_next;
			delete node;
		}
	};

	void AddTail(T* obj);
	void AddHead(T* obj);
	T* RemoveHead();
	T* RemoveTail();
	T* GetHead();
	T* GetTail();
	NetListNode *GetTailNode();
	void InsertAt(NetList<T>::NetListNode *node, T *obj);

	void Remove(NetListNode* node);
	BOOL  IsEmpty() { return m_head == NULL; };

	NetListNode *Find(T *obj);

	class Walker {
	public:
		Walker(NetList* list) :
			m_node(list->m_head),
			m_list(list)
		{
		}
		Walker() :
			m_node(NULL),
			m_list(NULL)
		{
		}

		void SetList(NetList *list) {
			m_node = list->m_head;
			m_list = list;
		}
		void Next() { Assert(m_node); m_node = m_node->m_next; }
		void Prev() { Assert(m_node); m_node = m_node->m_prev; }
		int IsValid() { return m_node != NULL; }
		T *GetObj() { return m_node->m_obj; }
		void Remove()
		{
			NetListNode* nextNode = m_node->m_next;
			m_list->Remove(m_node);
			m_node = nextNode;
		}
	private:
		NetListNode* m_node;
		NetList<T>* m_list;
	};

private:
	NetListNode* m_head;
	NetListNode* m_tail;
	friend class Walker;
};

template <class T> inline void NetList<T>::AddTail(T *obj)
{
	NetListNode* node = new NetListNode(obj);
	if(m_tail) {
		m_tail->m_next = node;
		node->m_prev = m_tail;
		m_tail = node;
	} else
		m_tail = m_head = node;
}

template <class T> inline void NetList<T>::AddHead(T *obj)
{
	NetListNode* node = new NetListNode(obj);
	if(m_head) {
		m_head->m_prev = node;
		node->m_next = m_head;
		m_head = node;
	} else
		m_tail = m_head = node;
}

template <class T> inline T* NetList<T>::RemoveHead()
{
	if(!m_head)
		return NULL;
	NetListNode* node = m_head;
	m_head = node->m_next;
	if(m_head == NULL) {
		m_tail = NULL;
	}
	T* obj = node->m_obj;
	delete node;
	return obj;
}

template <class T> inline T* NetList<T>::GetHead()
{
	if(!m_head)
		return NULL;
	return m_head->m_obj;
}

template <class T> inline T* NetList<T>::GetTail()
{
	if(!m_tail)
		return NULL;
	return m_tail->m_obj;
}

template <class T> inline T* NetList<T>::RemoveTail()
{
	if(!m_tail)
		return NULL;
	NetListNode* node = m_tail;
	m_tail = node->m_prev;
	if(m_tail == NULL) {
		m_head = NULL;
	}
	T* obj = node->m_obj;
	delete node;
	return obj;
}

template <class T> inline void NetList<T>::Remove(NetListNode* node)
{
	if(node == m_head) {
		m_head = node->m_next;
		if(m_head == NULL)
			m_tail = NULL;
	}
	if(node == m_tail) {
		m_tail = node->m_prev;
		if(m_tail == NULL)
			m_head = NULL;
	}
	delete node;
}

template <class T> inline NetList<T>::NetListNode *NetList<T>::GetTailNode()
{
	return m_tail;
}

template <class T> inline void NetList<T>::InsertAt(NetList<T>::NetListNode *node, T *obj)
{
	NetListNode *newNode = new NetListNode(obj);
	if(!node) {
		if(m_head) {
			newNode->m_next = m_head;
			m_head->m_prev = newNode;
			m_head = newNode;
		} else {
			m_head = newNode;
		}
	} else {
		newNode->m_next = node->m_next;
		newNode->m_prev = node;
		node->m_next = newNode;
		if(newNode->m_next) {
			newNode->m_next->m_prev = newNode;
		} else {
			m_tail = newNode;
		}
	}
}

template <class T> NetList<T>::NetListNode *NetList<T>::Find(T *obj)
{
	NetListNode *search = m_head;
	while(search) {
		if(search->m_obj == obj)
			return search;
		search = search->m_next;
	}
	return NULL;
}
#endif

#else
class NetList;
#endif
