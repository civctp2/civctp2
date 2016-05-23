//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Pointer list
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
// - Class export declaration changed.
// - Modifications to support VC++ .NET.
// - Rewritten insert functions (missing m_tail updates) to prevent crashes.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef _POINTER_LIST_H_
#define _POINTER_LIST_H_

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

template <class T> class PointerList;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

template <class T> class PointerList {
public:
	class PointerListNode {
	public:
		PointerListNode(T* obj) :
			m_next(NULL),
			m_prev(NULL),
			m_obj(obj)
        { ; };

		virtual ~PointerListNode()
        { ; };

		T* GetObj() { return m_obj; };

		PointerListNode *GetNext() { return m_next; };
		PointerListNode *GetPrev() { return m_prev; };
	private:
		friend class PointerList<T>;
		T* m_obj;

		PointerListNode* m_next;
		PointerListNode* m_prev;
	};

	PointerList() : m_head(NULL),
				m_tail(NULL),
				m_count(0)
    { ; };

	virtual ~PointerList()
	{
		while (m_head)
        {
			PointerListNode * node = m_head;
			m_head = m_head->m_next;
			delete node;
		}
	};

	void DeleteAll()
	{
		while (m_head)
        {
			PointerListNode * node = m_head;
			m_head = m_head->m_next;
			delete node->m_obj;
			delete node;
		}
		m_tail = NULL;
		m_count = 0;
	};

	void AddTail(T* obj);
	void AddHead(T* obj);
	T* RemoveHead();
	T* RemoveTail();
	T* GetHead();
	T* GetTail();

	PointerListNode *GetHeadNode()
	{
		return m_head;
	};
	PointerListNode *GetTailNode()
	{
		return m_tail;
	};

	void InsertAt(PointerListNode *node, T *obj);
	void InsertBefore(PointerListNode *node, T *obj);

	void Remove(PointerListNode* node);
	BOOL  IsEmpty() const { return m_head == NULL; };
	sint32 GetCount() const { return m_count; }

	PointerListNode *Find(T *obj);

	void Swap(PointerListNode *node, PointerListNode *swap) {
		PointerListNode *oldNext = node->m_next;
		PointerListNode *oldPrev = node->m_prev;
		PointerListNode *swapNext = swap->m_next;
		PointerListNode *swapPrev = swap->m_prev;

		if(oldPrev) {
			oldPrev->m_next = swap;
		}

		if(oldNext) {
			oldNext->m_prev = swap;
		}

		if(oldPrev == swap) {
			swap->m_prev = node;
		} else {
			swap->m_prev = oldPrev;
		}

		if(oldNext == swap) {
			swap->m_next = node;
		} else {
			swap->m_next = oldNext;
		}

		if(swapNext == node) {
			node->m_next = swap;
		} else {
			node->m_next = swapNext;
		}

		if(swapPrev == node) {
			node->m_prev = swap;
		} else {
			node->m_prev = swapPrev;
		}

		if(node->m_prev) {
			node->m_prev->m_next = node;
		} else {
			m_head = node;
		}

		if(node->m_next) {
			node->m_next->m_prev = node;
		} else {
			m_tail = node;
		}

		if(!swap->m_next) {
			m_tail = swap;
		}

		if(!swap->m_prev) {
			m_head = swap;
		}
	}

	class Walker {
	public:
		Walker(PointerList * list = NULL)
        :
            m_node  (list ? list->m_head : NULL),
			m_list  (list)
		{
		};

		void SetList(PointerList *list)
        {
            m_node = list ? list->m_head : NULL;
			m_list = list;
		}
		void Next() { Assert(m_node); m_node = m_node->GetNext(); }
		void Prev() { Assert(m_node); m_node = m_node->GetPrev(); }
		bool IsValid() { return m_node != NULL; }
		T *GetObj() { Assert(m_node); return m_node->GetObj(); }
		T *Remove()
		{
			T *obj = m_node->GetObj();
			PointerListNode* nextNode = m_node->GetNext();
			m_list->Remove(m_node);
			m_node = nextNode;
			return obj;
		}
		void Insert(T *obj) {
			m_list->InsertBefore(m_node, obj);
		}
		void InsertAfter(T *obj) {
			m_list->InsertAt(m_node, obj);
		}

		void MoveUp() {
			if(!m_node->GetPrev()) {

				return;
			}

			m_list->Swap(m_node, m_node->GetPrev());
		}

		void MoveDown() {
			if(!m_node->GetNext()) {

				return;
			}
			m_list->Swap(m_node, m_node->GetNext());
		}

	private:
		PointerListNode* m_node;
		PointerList<T>* m_list;
	};

private:
	PointerListNode* m_head;
	PointerListNode* m_tail;

	sint32 m_count;

	friend class Walker;
};

template <class T> void PointerList<T>::AddTail(T *obj)
{
	PointerListNode* node = new PointerListNode(obj);
	if(m_tail) {
		m_tail->m_next = node;
		node->m_prev = m_tail;
		m_tail = node;
	} else
		m_tail = m_head = node;
	m_count++;
}

template <class T> void PointerList<T>::AddHead(T *obj)
{
	PointerListNode* node = new PointerListNode(obj);
	if(m_head) {
		m_head->m_prev = node;
		node->m_next = m_head;
		m_head = node;
	} else
		m_tail = m_head = node;
	m_count++;
}

template <class T> T* PointerList<T>::RemoveHead()
{
	if(!m_head)
		return NULL;

	PointerListNode* node = m_head;
	m_head = node->m_next;
	if (m_head)
    {
        m_head->m_prev = NULL;
    }
    else
    {
		m_tail = NULL;
	}
	T* obj = node->m_obj;
	delete node;

	m_count--;

	return obj;
}

template <class T> T* PointerList<T>::GetHead()
{
	if(!m_head)
		return NULL;
	return m_head->m_obj;
}

template <class T> T* PointerList<T>::GetTail()
{
	if(!m_tail)
		return NULL;
	return m_tail->m_obj;
}

template <class T> T* PointerList<T>::RemoveTail()
{
	if(!m_tail)
		return NULL;

	PointerListNode* node = m_tail;
	m_tail = node->m_prev;
	if (m_tail)
    {
        m_tail->m_next = NULL;
    }
    else
    {
		m_head = NULL;
	}
	T* obj = node->m_obj;
	delete node;

	m_count--;

	return obj;
}

template <class T> void PointerList<T>::Remove(PointerListNode* node)
{
    if (!node)
        return;

	if (node == m_head)
    {
		m_head = node->m_next;
        if (m_head)
        {
            m_head->m_prev = NULL;
        }
        else
        {
			m_tail = NULL;
        }
	}
    else if (node == m_tail)
    {
		m_tail = node->m_prev;
		if (m_tail)
        {
            m_tail->m_next = NULL;
        }
        else
        {
			m_head = NULL;
        }
	}
    else
    {
        if (node->m_next)
        {
            node->m_next->m_prev = node->m_prev;
        }
        if (node->m_prev)
        {
            node->m_prev->m_next = node->m_next;
        }
    }

	m_count--;
	delete node;
}

template <class T> void PointerList<T>::InsertAt(PointerListNode *node, T *obj)
{
	if (node)
    {
	    PointerListNode * newNode = new PointerListNode(obj);
		newNode->m_next = node->m_next;
		newNode->m_prev = node;
        if (node->m_next)
        {
            node->m_next->m_prev = newNode;
        }
        else
        {
            m_tail      = newNode;
        }
        node->m_next    = newNode;
    	m_count++;
	}
    else
    {
		AddHead(obj);   // AddTail would have been more logical
	}
}

template <class T> void PointerList<T>::InsertBefore(PointerListNode *node, T *obj)
{
    if (node)
    {
	    PointerListNode * newNode = new PointerListNode(obj);
		newNode->m_prev = node->m_prev;
		newNode->m_next = node;
        if (node->m_prev)
        {
		    node->m_prev->m_next = newNode;
        }
		else
        {
			m_head      = newNode;
		}
        node->m_prev    = newNode;
    	m_count++;
    }
    else
    {
        AddHead(obj);
    }
}

template <class T> typename PointerList<T>::PointerListNode *PointerList<T>::Find(T *obj)
{
	for (PointerListNode * search = m_head; search; search = search->m_next)
    {
		if (search->m_obj == obj)
        {
			return search;
        }
	}

	return NULL;
}

#endif
