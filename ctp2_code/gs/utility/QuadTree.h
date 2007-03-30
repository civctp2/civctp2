#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

enum QUADRANT {
	QUADRANT_NE,
	QUADRANT_SE,
	QUADRANT_SW,
	QUADRANT_NW,
	QUADRANT_ERROR
};

#include "dynarr.h"







#define k_MAX_DEGENERATE_COUNT 100

template <class T> class QuadTree;
template <class T>	class QuadTreeNode
{
	














#define QCX (m_x + ((m_width) / 2))
#define QCY (m_y + ((m_height) / 2))

private:
	DynamicArray<T> m_array;
	sint16 m_x, m_y;
	sint16 m_width, m_height;
	QuadTreeNode<T>* m_ne;
	QuadTreeNode<T>* m_se;
	QuadTreeNode<T>* m_sw;
	QuadTreeNode<T>* m_nw;
	QuadTreeNode<T>* m_parent;
	BOOL m_isLeaf;

	QuadTree<T> *m_tree;
	friend class QuadTree<T>;
public:
	QuadTreeNode(QuadTree<T>* tree, 
				 QuadTreeNode<T>* parent, T val, 
				 sint16 x, sint16 y, 
				 sint16 width, sint16 height); 
	QuadTreeNode(QuadTree<T>* tree,
				 QuadTreeNode<T>* parent, 
				 DynamicArray<T> & a_List, 
				 sint16 x, sint16 y, 
				 sint16 width, sint16 height);

	~QuadTreeNode();

	QUADRANT FindQuadrant(const MapPoint &pos) const;
	QuadTreeNode<T> *AddLeaf(QUADRANT quad, T obj);
	QuadTreeNode<T> *AddLeaf(QUADRANT quad, DynamicArray<T> & a_List);

	
	void AddList(DynamicArray<T> & a_List);
	void AddObject(T obj);

	void RemoveObject(T obj);
	bool RemoveAt(const MapPoint &point, T &removedObj);
	bool RemoveAt(const MapPoint &point, DynamicArray<T> &array);
	void RemoveBranch(QuadTreeNode<T>* node, BOOL recurse = TRUE);
	bool GetAt(const MapPoint &point, T &obj);
	bool GetAt(const MapPoint &point, DynamicArray<T> &array);
	size_t GetCount(MapPoint const &point) const;

	void SearchRect(DynamicArray<T> &array, 
					MapPoint &pos, 
					sint16 width, sint16 height,
					uint32 mask);
	void BuildList(DynamicArray<T> &array, uint32 mask) const;
	void RemoveDegenerate(DynamicArray<T> &rebuildList);
};

template <class T> class QuadTree
{
public:
	QuadTree(sint16 width, sint16 height, BOOL yWrap) 
	: 
        m_top               (NULL),
        m_width             (width),
		m_height            (height),
        m_isYWrap           (yWrap),
        m_degenerateCount   (0)
	{};

	QuadTree()
    :
        m_top               (NULL),
        m_width             (0),
		m_height            (0),
        m_isYWrap           (FALSE),
        m_degenerateCount   (0)
	{};

	virtual ~QuadTree()
	{
		delete m_top;
	}

	void Insert(T obj);
	void Remove(T obj);
	bool RemoveAt(const MapPoint &point, T &removedObj);
	bool RemoveAt(const MapPoint &point, DynamicArray<T> &array);
	bool GetAt(const MapPoint &point, T &obj);
	bool GetAt(const MapPoint &point, DynamicArray<T> &array);
	size_t GetCount(MapPoint const & point) const;

	virtual void Clear();
	void SearchRect(DynamicArray<T> &array, 
					MapPoint pos, 
					sint16 width, sint16 height,
					uint32 mask = 0xffffffff);
	void BuildList(DynamicArray<T> &array, uint32 mask = 0xffffffff) const;
	void RemoveTop() 
	{
		delete m_top; 
		m_top = NULL; 
	}

	
	
	
	virtual MapPoint GetPos(T obj) const;
	virtual void Convert(MapPoint &pos) const;
	
	
	
	virtual uint32 GetFlags(T obj);

	void IncrementDegenerateCount();
	void GarbageCollector();

	friend class QuadTreeNode<T>;

private:
	QuadTreeNode<T>* m_top;
	sint16 m_width, m_height;
	BOOL m_isYWrap;
	sint32 m_degenerateCount;
};

template <class T> 
QuadTreeNode<T>::QuadTreeNode(QuadTree<T> *tree,
							  QuadTreeNode<T>* parent, 
							  T val, 
							  sint16 x, sint16 y, 
							  sint16 width, sint16 height)
	: 
	  m_x(x), m_y(y),
	  m_width(width), m_height(height),
	  m_ne(NULL),
	  m_se(NULL),
	  m_sw(NULL),
	  m_nw(NULL),
	  m_parent(parent),
	  m_isLeaf(TRUE),
	  m_tree(tree)
{
	m_array.Insert(val);
}

template <class T>
QuadTreeNode<T>::QuadTreeNode(QuadTree<T> *tree,
							  QuadTreeNode<T>* parent, 
							  DynamicArray<T> & a_List, 
							  sint16 x, sint16 y, 
							  sint16 width, sint16 height)
	: 
	  m_x(x), m_y(y),
	  m_width(width), m_height(height),
	  m_ne(NULL),
	  m_se(NULL),
	  m_sw(NULL),
	  m_nw(NULL),
	  m_parent(parent),
	  m_isLeaf(TRUE),
	  m_tree(tree)
{
	m_array.Concat(a_List);
}

template <class T>
QuadTreeNode<T>::~QuadTreeNode()
{
	delete m_ne;
	delete m_se;
	delete m_sw;
	delete m_nw;
}

template <class T> QUADRANT
QuadTreeNode<T>::FindQuadrant(const MapPoint &pos) const
{
	Assert(pos.x >= m_x &&
		   pos.x < m_x + m_width &&
		   pos.y >= m_y &&
		   pos.y < m_y + m_height);
	if(pos.x >= QCX && pos.y < QCY) return QUADRANT_NE;
	if(pos.x >= QCX && pos.y >= QCY) return QUADRANT_SE;
	if(pos.x < QCX && pos.y >= QCY) return QUADRANT_SW;
	if(pos.x < QCX && pos.y < QCY) return QUADRANT_NW;

    Assert(false);
	return QUADRANT_ERROR;
}

template <class T> QuadTreeNode<T> *
QuadTreeNode<T>::AddLeaf(QUADRANT quad, T obj)
{
	sint16 neww = ((m_width / 2) == 0) ? 1 : (m_width / 2);
	sint16 newh = ((m_height / 2) == 0) ? 1 : (m_height / 2);
	m_isLeaf = FALSE;
	switch(quad) {
		case QUADRANT_NE:
			return m_ne = new QuadTreeNode<T>(m_tree,
											  this, obj, QCX, m_y,
											  neww + ((m_width%2) ? 1 : 0), newh);
		case QUADRANT_SE:
			return m_se = new QuadTreeNode<T>(m_tree,
											  this, obj, QCX, QCY,
											  neww + ((m_width%2) ? 1 : 0), 
											  newh + ((m_height%2) ? 1 : 0));
		case QUADRANT_SW:
			return m_sw = new QuadTreeNode<T>(m_tree,
											  this, obj, m_x, QCY, neww,
											  newh + ((m_height%2) ? 1 : 0));
		case QUADRANT_NW:
			return m_nw = new QuadTreeNode<T>(m_tree,
											  this, obj, m_x, m_y, neww, newh);
	}
	Assert(false);
	return NULL;
}

template <class T> QuadTreeNode<T>*
QuadTreeNode<T>::AddLeaf(QUADRANT quad, DynamicArray<T> &a_List)
{
	sint16 neww = ((m_width / 2) == 0) ? 1 : (m_width / 2);
	sint16 newh = ((m_height / 2) == 0) ? 1 : (m_height / 2);
	m_isLeaf = FALSE;
	switch(quad) {
		case QUADRANT_NE:
			return m_ne = new QuadTreeNode<T>(m_tree,
											  this, a_List, QCX, m_y, 
											  neww + ((m_width%2) ? 1 : 0), newh);
		case QUADRANT_SE:
			return m_se = new QuadTreeNode<T>(m_tree,
											  this, a_List, QCX, QCY, 
											  neww + ((m_width%2) ? 1 : 0), 
											  newh + ((m_height%2) ? 1 : 0));
		case QUADRANT_SW:
			return m_sw = new QuadTreeNode<T>(m_tree,
											  this, a_List, m_x, QCY, neww, 
											  newh + ((m_height%2) ? 1 : 0));
		case QUADRANT_NW:
			return m_nw = new QuadTreeNode<T>(m_tree,
											  this, a_List, m_x, m_y, neww, newh);
	}
	Assert(false);
	return NULL;
}


template <class T> void
QuadTreeNode<T>::AddList(DynamicArray<T> & a_List)
{
	Assert(a_List.Num() > 0);
	MapPoint pos     = m_tree->GetPos(a_List[0]);
	QUADRANT newQuad = FindQuadrant(pos);
	
	if(m_isLeaf) {
		Assert(m_array.Num() > 0);
		MapPoint oldPos = m_tree->GetPos(m_array[0]);
		
		
		
		
		
		if(pos.x == oldPos.x && pos.y == oldPos.y) {
			m_array.Concat(a_List);
		} else {
			QUADRANT oldQuad = FindQuadrant(oldPos);
			if (oldQuad == newQuad) 
            {
				QuadTreeNode<T> * newNode = AddLeaf(newQuad, a_List);
				newNode->AddList(m_array);
				m_isLeaf = FALSE;
			} 
            else 
            {
				AddLeaf(newQuad, a_List);
				AddLeaf(oldQuad, m_array);
			}
			m_array.Clear();
		}
	} else {
		switch(newQuad) {
			case QUADRANT_NE:
				if(!m_ne) AddLeaf(newQuad, a_List);
				else m_ne->AddList(a_List);
				break;
			case QUADRANT_SE:
				if(!m_se) AddLeaf(newQuad, a_List);
				else m_se->AddList(a_List);
				break;
			case QUADRANT_SW:
				if(!m_sw) AddLeaf(newQuad, a_List);
				else m_sw->AddList(a_List);
				break;
			case QUADRANT_NW:
				if(!m_nw) AddLeaf(newQuad, a_List);
				else m_nw->AddList(a_List);
				break;
		}
	}
}

template <class T> void
QuadTreeNode<T>::AddObject(T obj)
{
	MapPoint pos = m_tree->GetPos(obj);
	QUADRANT newQuad = FindQuadrant(pos);
	
	if(m_isLeaf) {
		Assert(m_array.Num() > 0);
		MapPoint oldPos = m_tree->GetPos(m_array[0]);
		
		
		if(pos.x == oldPos.x && pos.y == oldPos.y) {
			m_array.Insert(obj);
		} else {
			QUADRANT oldQuad = FindQuadrant(oldPos);
			if (oldQuad == newQuad) 
            {
	            QuadTreeNode<T> * newNode = AddLeaf(newQuad, obj);
				newNode->AddList(m_array);
				m_isLeaf = FALSE;
			} 
            else 
            {
				AddLeaf(newQuad, obj);
				AddLeaf(oldQuad, m_array);
			}
			m_array.Clear();
		}
	} else {
		switch(newQuad) {
			case QUADRANT_NE:
				if(!m_ne) AddLeaf(newQuad, obj);
				else m_ne->AddObject(obj);
				break;
			case QUADRANT_SE:
				if(!m_se) AddLeaf(newQuad, obj);
				else m_se->AddObject(obj);
				break;
			case QUADRANT_SW:
				if(!m_sw) AddLeaf(newQuad, obj);
				else m_sw->AddObject(obj);
				break;
			case QUADRANT_NW:
				if(!m_nw) AddLeaf(newQuad, obj);
				else m_nw->AddObject(obj);
				break;
		}
	}
}

template <class T> void
QuadTreeNode<T>::RemoveObject(T obj)
{
	if(m_isLeaf) {
		m_array.Del(obj);
		if(m_array.Num() <= 0) {
			if(m_parent) {
				m_parent->RemoveBranch(this);
			} else {
				Assert(m_tree->m_top == this);
				m_tree->RemoveTop();
			}
		}
	} else {
		MapPoint pos = m_tree->GetPos(obj);
		switch (FindQuadrant(pos)) {
			case QUADRANT_NE: 
				Assert(m_ne); 
				if(!m_ne) return;
				m_ne->RemoveObject(obj); 
				break;
			case QUADRANT_SE: 
				Assert(m_se); 
				if(!m_se) return;
				m_se->RemoveObject(obj); 
				break;
			case QUADRANT_SW: 
				Assert(m_sw); 
				if(!m_sw) return;
				m_sw->RemoveObject(obj); 
				break;
			case QUADRANT_NW: 
				Assert(m_nw); 
				if(!m_nw) return;
				m_nw->RemoveObject(obj); 
				break;
		}
	}
}

template <class T> bool
QuadTreeNode<T>::GetAt(const MapPoint &point, T &obj)
{
	if (m_isLeaf) 
    {
        Assert(m_tree); 
		
		for (sint32 i = 0; i < m_array.Num(); i++) 
        {
			if (m_tree->GetPos(m_array[i]) == point) 
            {
				obj = m_array[i];
				return true;
			}
		}
		
		return false;
	} 
    else 
    {
		switch (FindQuadrant(point)) 
        {
        default:
            Assert(false);
            return false;
		case QUADRANT_NE:
			return m_ne && m_ne->GetAt(point, obj);
		case QUADRANT_SE:
			return m_se && m_se->GetAt(point, obj);
		case QUADRANT_SW:
			return m_sw && m_sw->GetAt(point, obj);
		case QUADRANT_NW:
			return m_nw && m_nw->GetAt(point, obj);
		}
	}
}
		
template <class T> bool
QuadTreeNode<T>::GetAt(const MapPoint &point, DynamicArray<T> & a_Array)
{
	if (m_isLeaf) 
    {
		MapPoint objPos = m_tree->GetPos(m_array[0]);
		if (objPos.x == point.x && objPos.y == point.y) 
        {
			a_Array.Clear();

            for (sint32 i = 0; i < m_array.Num(); i++) 
            {
				if (m_tree->GetPos(m_array[i]) == point) 
                {
					a_Array.Insert(m_array[i]);
				}
			}
			
            return a_Array.Num() > 0;
		}

		return false;
	} 
    else 
    {
		switch (FindQuadrant(point)) 
        {
        default:
            Assert(false);
            return false;
		case QUADRANT_NE:
			return m_ne && m_ne->GetAt(point, a_Array);
		case QUADRANT_SE:
			return m_se && m_se->GetAt(point, a_Array);
		case QUADRANT_SW:
			return m_sw && m_sw->GetAt(point, a_Array);
		case QUADRANT_NW:
			return m_nw && m_nw->GetAt(point, a_Array);
		}
	}
}
		
template <class T> 
size_t QuadTreeNode<T>::GetCount(MapPoint const & point) const
{
	if (m_isLeaf) 
    {
		MapPoint objPos = m_tree->GetPos(m_array[0]);
		if (objPos.x == point.x && objPos.y == point.y) 
        {
			return m_array.Num();
		} 
        else 
        {
			return 0;
		}
	} 
    else 
    {
		switch (FindQuadrant(point)) 
        {
        default:
            Assert(false);
            return 0;
		case QUADRANT_NE:
            return (m_ne) ? m_ne->GetCount(point) : 0;
		case QUADRANT_SE:
            return (m_se) ? m_se->GetCount(point) : 0;
		case QUADRANT_SW:
            return (m_sw) ? m_sw->GetCount(point) : 0;
		case QUADRANT_NW:
            return (m_nw) ? m_nw->GetCount(point) : 0;
		}
	}
}

template <class T> bool
QuadTreeNode<T>::RemoveAt(const MapPoint &point, T &removedObj)
{
	if (m_isLeaf) 
    {
		MapPoint objPos = m_tree->GetPos(m_array[0]);
		if (objPos.x != point.x || objPos.y != point.y)
			return false;

		removedObj = m_array[0];

		for (size_t i = m_array.Num(); i > 0; --i) 
        {
			m_array.DelIndex(i - 1);
		}

		if (m_array.Num() == 0) 
        {
			if (m_parent) 
            {
				m_parent->RemoveBranch(this);
			} 
            else 
            {
				Assert(m_tree->m_top == this);
				m_tree->RemoveTop();
			}
		}

		return true;
	} 
    else 
    {
		switch (FindQuadrant(point)) 
        {
        default:
	        Assert(false);
	        return false;
		case QUADRANT_NE:
			return m_ne && m_ne->RemoveAt(point, removedObj);
		case QUADRANT_SE:
			return m_se && m_se->RemoveAt(point, removedObj);
		case QUADRANT_SW:
			return m_sw && m_sw->RemoveAt(point, removedObj);
		case QUADRANT_NW:
			return m_nw && m_nw->RemoveAt(point, removedObj);
		}
	}
}

template <class T> bool
QuadTreeNode<T>::RemoveAt(const MapPoint &point, DynamicArray<T> & a_Array)
{
	if (m_isLeaf) 
    {
		MapPoint objPos = m_tree->GetPos(m_array[0]);
		if (objPos.x != point.x || objPos.y != point.y)
			return false;

		a_Array.Concat(m_array);
		if (m_parent) 
        {
			m_parent->RemoveBranch(this);
		} 
        else 
        {
			Assert(m_tree->m_top == this);
			m_tree->RemoveTop();
		}
		return true;
	} 
    else 
    {
		switch (FindQuadrant(point)) 
        {
        default:
            Assert(false);
            return false;
		case QUADRANT_NE:
			return m_ne && m_ne->RemoveAt(point, a_Array);
		case QUADRANT_SE:
			return m_se && m_se->RemoveAt(point, a_Array);
		case QUADRANT_SW:
			return m_sw && m_sw->RemoveAt(point, a_Array);
		case QUADRANT_NW:
			return m_nw && m_nw->RemoveAt(point, a_Array);
		}
	}
}

template <class T> void
QuadTreeNode<T>::RemoveBranch(QuadTreeNode<T> *node, BOOL recurse)
{
	if(m_ne == node) {
		delete m_ne;
		m_ne = NULL;
	} else if(m_se == node) {
		delete m_se;
		m_se = NULL;
	} else if(m_sw == node) {
		delete m_sw;
		m_sw = NULL;
	} else if(m_nw == node) {
		delete m_nw;
		m_nw = NULL;
	} else {
		Assert(FALSE);
	}

	if(!recurse)
		return;

	sint32 count = 0;
	if(m_ne)
		count++;
	if(m_nw)
		count++;
	if(m_se)
		count++;
	if(m_sw)
		count++;

	if(count == 0) {
		
		if(m_parent != NULL) {
			m_parent->RemoveBranch(this);
		} else {
			Assert(m_tree->m_top == this);
			m_tree->RemoveTop();
		}
	} else if(count == 1) {
		m_tree->IncrementDegenerateCount();
	}
}

template <class T> void
QuadTreeNode<T>::SearchRect(DynamicArray<T> &array,
							MapPoint &pos, sint16 width, sint16 height,
							uint32 mask)
{
	MapPoint srch;
	sint16 subwidth, subheight;
	if(pos.x + width > m_x + m_width)
		width = m_width - (pos.x - m_x);

	if(pos.y + height > m_y + m_height)
		height = m_height - (pos.y - m_y);

	if(m_isLeaf) {
		Assert(m_array.Num() > 0);
		if(!(mask & m_tree->GetFlags(m_array[0])))
			return;

		srch = m_tree->GetPos(m_array[0]);
		if(srch.x >= pos.x && srch.x < pos.x + width &&
		   srch.y >= pos.y && srch.y < pos.y + height) {
			
			array.Concat(m_array);
		}
	} else {
		if(pos.y < QCY) { 
			if(m_ne && (pos.x + width) >= QCX) {
				if(pos.x >= QCX) {
					srch.x = pos.x;
					subwidth = width;
				} else {
					srch.x = QCX;
					subwidth = width - (srch.x - pos.x);
				}
				srch.y = pos.y;
				subheight = ((pos.y + height) < QCY) ? height : (QCY - srch.y);
				m_ne->SearchRect(array, srch, subwidth, subheight, mask);
			}
			if(m_nw && pos.x < QCX) {
				srch.x = pos.x;
				srch.y = pos.y;
				subwidth = ((pos.x + width) < QCX) ? width : (QCX - srch.x);
				subheight = ((pos.y + height) < QCY) ? height : (QCY - srch.y);
				m_nw->SearchRect(array, srch, subwidth, subheight, mask);
			}
		}
		if(pos.y + height >= QCY) { 
			if(m_se && (pos.x + width) >= QCX) {
				if(pos.x >= QCX) {
					srch.x = pos.x;
					subwidth = width;
				} else {
					srch.x = QCX;
					subwidth = width - (srch.x - pos.x);
				}
				if(pos.y >= QCY) {
					srch.y = pos.y;
					subheight = height;
				} else {
					srch.y = QCY;
					subheight = height - (srch.y - pos.y);
				}
				m_se->SearchRect(array, srch, subwidth, subheight, mask);
			}
			if(m_sw && (pos.x < QCX)) {
				srch.x = pos.x;
				subwidth = ((pos.x + width) < QCX) ? width : (QCX - srch.x);
				if(pos.y >= QCY) {
					srch.y = pos.y;
					subheight = height;
				} else {
					srch.y = QCY;
					subheight = height - (srch.y - pos.y);
				}
				m_sw->SearchRect(array, srch, subwidth, subheight, mask);
			}
		}
	}
}

template <class T> void QuadTreeNode<T>::BuildList(DynamicArray<T> &array,
												   uint32 mask) const
{
	if(m_isLeaf) {
		Assert(m_array.Num() > 0);
		if(!(mask & m_tree->GetFlags(m_array.Get(0))))
			return;

		array.Concat(m_array);
	} else {
		if(m_nw) m_nw->BuildList(array, mask);
		if(m_ne) m_ne->BuildList(array, mask);
		if(m_se) m_se->BuildList(array, mask);
		if(m_sw) m_sw->BuildList(array, mask);
	}
}

template <class T> void 
QuadTreeNode<T>::RemoveDegenerate(DynamicArray<T> &rebuildList)
{
	if (m_isLeaf)
		return;

	sint32 count = 0;
	if(m_ne)
		count++;
	if(m_se)
		count++;
	if(m_sw)
		count++;
	if(m_nw)
		count++;

	Assert(count > 0);
	if(count == 1) {
		
		if(m_ne) {
			m_ne->BuildList(rebuildList, 0xffffffff);
		} else if(m_se) {
			m_se->BuildList(rebuildList, 0xffffffff);
		} else if(m_sw) {
			m_sw->BuildList(rebuildList, 0xffffffff);
		} else if(m_nw) {
			m_nw->BuildList(rebuildList, 0xffffffff);
		}
		if(m_parent) {
			m_parent->RemoveBranch(this, FALSE);
		} else {
			Assert(m_tree->m_top == this);
			m_tree->RemoveTop();
		}
	} else if(count > 1) {
		if(m_ne)
			m_ne->RemoveDegenerate(rebuildList);
		if(m_se)
			m_se->RemoveDegenerate(rebuildList);
		if(m_sw)
			m_sw->RemoveDegenerate(rebuildList);
		if(m_nw)
			m_nw->RemoveDegenerate(rebuildList);
	}
}

template <class T> void QuadTree<T>::Insert(T newObj)
{
	MapPoint pos = GetPos(newObj);
	Assert(pos.x >= 0 && pos.x < m_width);
	Assert(pos.y >= 0 && pos.y < m_height);

	if(m_top) {
		m_top->AddObject(newObj);
	} else {
		m_top = new QuadTreeNode<T>(this, NULL, newObj, 0, 0, m_width, m_height);
	}
}

template <class T> void QuadTree<T>::Remove(T obj)
{
	if (m_top)
    {
    	m_top->RemoveObject(obj);
    }
}

template <class T> bool QuadTree<T>::RemoveAt(const MapPoint &point, T &removedObj)
{
	if(!m_top)
		return false;

	MapPoint pos = point;
	Convert(pos);
	return m_top->RemoveAt(pos, removedObj);
}

template <class T> bool QuadTree<T>::RemoveAt(const MapPoint &point, DynamicArray<T> &array)
{
	if(!m_top)
		return false;

	MapPoint pos = point;
	Convert(pos);
	return m_top->RemoveAt(pos, array);
}

template <class T> bool QuadTree<T>::GetAt(const MapPoint &point, T &obj)
{
	if(!m_top)
		return false;

	MapPoint pos = point;
	Convert(pos);
	return m_top->GetAt(pos, obj);
}

template <class T> bool QuadTree<T>::GetAt(const MapPoint &point, 
										   DynamicArray<T> &array)
{
	if(!m_top)
		return false;

	MapPoint pos = point;
	Convert(pos);
	return m_top->GetAt(pos, array);
}

template <class T> size_t QuadTree<T>::GetCount(MapPoint const &point) const
{
	if(!m_top)
		return 0;

	MapPoint pos = point;
	Convert(pos);
	return m_top->GetCount(pos);
}

template <class T> void QuadTree<T>::Clear()
{
	delete m_top;
	m_top = NULL;
}


template <class T> void
QuadTree<T>::Convert(MapPoint &pos) const
{
	pos.y = ((pos.x + pos.y) % m_height);
}

template <class T> MapPoint
QuadTree<T>::GetPos(T obj) const
{
	MapPoint pos;
    obj.GetPos(pos);
	Convert(pos);
	return pos;
}

template <class T> uint32
QuadTree<T>::GetFlags(T obj)
{
	return (1 << obj.GetOwner());
}













    
template <class T> void 
QuadTree<T>::SearchRect(DynamicArray<T> &array, MapPoint pos, sint16 width, sint16 height, uint32 mask)
{
	if(!m_top) {
		
		return;
	}

	
	if(pos.x < 0) pos.x += m_width;
	if(pos.y < 0) {
		pos.y += m_height;
		pos.x += (m_width - (m_height / 2)) % m_width;
	}

	
	
	if(pos.x > m_width) pos.x -= m_width;
	if(pos.y > m_height) {
		pos.y -= m_height;
		pos.x -= (m_width - (m_height / 2)) % m_width;
	}

	
	Convert(pos);

	sint16 curwidth = width, curheight = height;
	
	


	if(pos.x + width >= m_width) {
		curwidth = m_width - pos.x;
	}

	if(pos.y + height >= m_height) {
		curheight = m_height - pos.y;
	}

	m_top->SearchRect(array, pos, curwidth, curheight, mask);

	if(curwidth != width) {
		MapPoint nePos(0, pos.y - m_width);
		if(nePos.y < 0)
			nePos.y += m_height;

		sint16 eheight = height;
		if(nePos.y + height >= m_height) {
			eheight = m_height - nePos.y;
		}

		m_top->SearchRect(array, nePos, width - curwidth, eheight, mask);

		if(eheight != height) {
			MapPoint sePos  (0,0);
			m_top->SearchRect(array, sePos, width - curwidth, height - eheight, mask);
		}
	}

	if(curheight != height) {
		MapPoint swPos  (pos.x, 0);
		m_top->SearchRect(array, swPos, width, height - curheight, mask);
	}
}




template <class T> void
QuadTree<T>::BuildList(DynamicArray<T> & a_Array, uint32 mask) const
{
	if (m_top)
    {
    	m_top->BuildList(a_Array, mask);
    }
}

template <class T> void
QuadTree<T>::GarbageCollector()
{
	static bool collecting = false;
	if(collecting)
		return;
	if(!m_top)
		return;

	if(m_degenerateCount > k_MAX_DEGENERATE_COUNT) {
		collecting = true;
		DynamicArray<T> rebuildList;
		m_top->RemoveDegenerate(rebuildList);

		for(sint32 i = 0; i < rebuildList.Num(); i++) {
			Insert(rebuildList[i]);
		}
		m_degenerateCount = 0;
		collecting = false;
	}
}

template <class T> void
QuadTree<T>::IncrementDegenerateCount()
{
	m_degenerateCount++;
}

class Unit;
extern QuadTree<Unit> *g_theUnitTree;
#endif
