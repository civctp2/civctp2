









#pragma once
#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

enum QUADRANT {
	QUADRANT_NE,
	QUADRANT_SE,
	QUADRANT_SW,
	QUADRANT_NW,
	QUADRANT_ERROR
};

#include "DynArr.h"







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
				 DynamicArray<T> &list, 
				 sint16 x, sint16 y, 
				 sint16 width, sint16 height);

	~QuadTreeNode();

	QUADRANT FindQuadrant(const MapPoint &pos);
	QuadTreeNode<T> *AddLeaf(QUADRANT quad, T obj);
	QuadTreeNode<T> *AddLeaf(QUADRANT quad, DynamicArray<T> &list);

	
	void AddList(DynamicArray<T> &list);
	void AddObject(T obj);

	void RemoveObject(T obj);
	BOOL RemoveAt(const MapPoint &point, T &removedObj);
	BOOL RemoveAt(const MapPoint &point, DynamicArray<T> &array);
	void RemoveBranch(QuadTreeNode<T>* node, BOOL recurse = TRUE);
	BOOL GetAt(const MapPoint &point, T &obj);
	BOOL GetAt(const MapPoint &point, DynamicArray<T> &array);
	sint32 GetCount(MapPoint &point);

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
		: m_width(width),
		  m_height(height),
		  m_top(NULL),
		  m_isYWrap(yWrap)
	{
		m_degenerateCount = 0;
	}

	QuadTree()
		: m_top(NULL)
	{
		m_degenerateCount = 0;
	}

	virtual ~QuadTree()
	{
		if(m_top) {
			delete m_top;
			m_top = NULL;
		}
	}

	void Insert(T obj);
	void Remove(T obj);
	BOOL RemoveAt(const MapPoint &point, T &removedObj);
	BOOL RemoveAt(const MapPoint &point, DynamicArray<T> &array);
	BOOL GetAt(const MapPoint &point, T &obj);
	BOOL GetAt(const MapPoint &point, DynamicArray<T> &array);
	sint32 GetCount(MapPoint &point);

	virtual void Clear();
	void SearchRect(DynamicArray<T> &array, 
					MapPoint pos, 
					sint16 width, sint16 height,
					uint32 mask = 0xffffffff);
	void BuildList(DynamicArray<T> &array, uint32 mask = 0xffffffff) const;
	void RemoveTop() 
	{
		Assert(m_top);
		if(m_top) {
			delete m_top; 
			m_top = NULL; 
		}
	}

	
	
	
	virtual MapPoint GetPos(T obj);
	virtual void Convert(MapPoint &pos);
	
	
	
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
	: m_ne(NULL),
	  m_se(NULL),
	  m_sw(NULL),
	  m_nw(NULL),
	  m_parent(parent),
	  m_isLeaf(TRUE),
	  m_x(x), m_y(y),
	  m_width(width), m_height(height),
	  m_tree(tree)
{
	m_array.Insert(val);
}

template <class T>
QuadTreeNode<T>::QuadTreeNode(QuadTree<T> *tree,
							  QuadTreeNode<T>* parent, 
							  DynamicArray<T> &list, 
							  sint16 x, sint16 y, 
							  sint16 width, sint16 height)
	: m_ne(NULL),
	  m_se(NULL),
	  m_sw(NULL),
	  m_nw(NULL),
	  m_parent(parent),
	  m_isLeaf(TRUE),
	  m_x(x), m_y(y),
	  m_width(width), m_height(height),
	  m_tree(tree)
{
	m_array.Concat(list);
}

template <class T>
QuadTreeNode<T>::~QuadTreeNode()
{
	if(m_ne) {
		delete m_ne;
		m_ne = NULL;
	}
	
	if(m_se) {
		delete m_se;
		m_se = NULL;
	}
	
	if(m_sw) {
		delete m_sw;
		m_sw = NULL;
	}
	
	if(m_nw) {
		delete m_nw;
		m_nw = NULL;
	}
}

template <class T> QUADRANT
QuadTreeNode<T>::FindQuadrant(const MapPoint &pos)
{
	Assert(pos.x >= m_x &&
		   pos.x < m_x + m_width &&
		   pos.y >= m_y &&
		   pos.y < m_y + m_height);
	if(pos.x >= QCX && pos.y < QCY) return QUADRANT_NE;
	if(pos.x >= QCX && pos.y >= QCY) return QUADRANT_SE;
	if(pos.x < QCX && pos.y >= QCY) return QUADRANT_SW;
	if(pos.x < QCX && pos.y < QCY) return QUADRANT_NW;
#pragma warning( disable : 4127)									
		Assert(FALSE);
#pragma warning( default : 4127)									
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
	Assert(FALSE);
	return NULL;
}

template <class T> QuadTreeNode<T>*
QuadTreeNode<T>::AddLeaf(QUADRANT quad, DynamicArray<T> &list)
{
	sint16 neww = ((m_width / 2) == 0) ? 1 : (m_width / 2);
	sint16 newh = ((m_height / 2) == 0) ? 1 : (m_height / 2);
	m_isLeaf = FALSE;
	switch(quad) {
		case QUADRANT_NE:
			return m_ne = new QuadTreeNode<T>(m_tree,
											  this, list, QCX, m_y, 
											  neww + ((m_width%2) ? 1 : 0), newh);
		case QUADRANT_SE:
			return m_se = new QuadTreeNode<T>(m_tree,
											  this, list, QCX, QCY, 
											  neww + ((m_width%2) ? 1 : 0), 
											  newh + ((m_height%2) ? 1 : 0));
		case QUADRANT_SW:
			return m_sw = new QuadTreeNode<T>(m_tree,
											  this, list, m_x, QCY, neww, 
											  newh + ((m_height%2) ? 1 : 0));
		case QUADRANT_NW:
			return m_nw = new QuadTreeNode<T>(m_tree,
											  this, list, m_x, m_y, neww, newh);
	}
	Assert(FALSE);
	return NULL;
}


template <class T> void
QuadTreeNode<T>::AddList(DynamicArray<T> &list)
{
	Assert(list.Num() > 0);
	MapPoint pos;
	pos = m_tree->GetPos(list[0]);
	QUADRANT newQuad = FindQuadrant(pos);
	QuadTreeNode<T> *newNode;
	
	if(m_isLeaf) {
		Assert(m_array.Num() > 0);
		MapPoint oldPos;
		oldPos = m_tree->GetPos(m_array[0]);
		
		
		
		
		
		if(pos.x == oldPos.x && pos.y == oldPos.y) {
			m_array.Concat(list);
		} else {
			QUADRANT oldQuad = FindQuadrant(oldPos);
			if(oldQuad != newQuad) {
				AddLeaf(newQuad, list);
				AddLeaf(oldQuad, m_array);
				m_array.Clear();
			} else {
				newNode = AddLeaf(newQuad, list);
				newNode->AddList(m_array);
				m_array.Clear();
				m_isLeaf = FALSE;
			}
		}
	} else {
		switch(newQuad) {
			case QUADRANT_NE:
				if(!m_ne) AddLeaf(newQuad, list);
				else m_ne->AddList(list);
				break;
			case QUADRANT_SE:
				if(!m_se) AddLeaf(newQuad, list);
				else m_se->AddList(list);
				break;
			case QUADRANT_SW:
				if(!m_sw) AddLeaf(newQuad, list);
				else m_sw->AddList(list);
				break;
			case QUADRANT_NW:
				if(!m_nw) AddLeaf(newQuad, list);
				else m_nw->AddList(list);
				break;
		}
	}
}

template <class T> void
QuadTreeNode<T>::AddObject(T obj)
{
	MapPoint pos;
	pos = m_tree->GetPos(obj);
	QUADRANT newQuad = FindQuadrant(pos);
	QuadTreeNode<T> *newNode;
	
	if(m_isLeaf) {
		Assert(m_array.Num() > 0);
		MapPoint oldPos;
		oldPos = m_tree->GetPos(m_array[0]);
		
		
		if(pos.x == oldPos.x && pos.y == oldPos.y) {
			m_array.Insert(obj);
		} else {
			QUADRANT oldQuad = FindQuadrant(oldPos);
			if(oldQuad != newQuad) {
				AddLeaf(newQuad, obj);
				AddLeaf(oldQuad, m_array);
				m_array.Clear();
			} else {
				newNode = AddLeaf(newQuad, obj);
				newNode->AddList(m_array);
				m_array.Clear();
				m_isLeaf = FALSE;
			}
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
		MapPoint pos;
		pos = m_tree->GetPos(obj);
		QUADRANT quad = FindQuadrant(pos);
		switch(quad) {
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

template <class T> BOOL
QuadTreeNode<T>::GetAt(const MapPoint &point, T &obj)
{
	if(m_isLeaf) {
        Assert(m_tree); 
		
		
		
		
		sint32 i = 0;
		for(i = 0; i < m_array.m_nElements; i++) {
			if(m_tree->GetPos(m_array[i]) == point) {
				obj = m_array[i];
				return TRUE;
			}
		}
		
		
		
		
		return FALSE;
	} else {
		QUADRANT quad = FindQuadrant(point);
		switch(quad) {
			case QUADRANT_NE:
				if(!m_ne) return FALSE;
				return m_ne->GetAt(point, obj);
			case QUADRANT_SE:
				if(!m_se) return FALSE;
				return m_se->GetAt(point, obj);
			case QUADRANT_SW:
				if(!m_sw) return FALSE;
				return m_sw->GetAt(point, obj);
			case QUADRANT_NW:
				if(!m_nw) return FALSE;
				return m_nw->GetAt(point, obj);
		}
		Assert(FALSE);
		return FALSE;
	}
}
		
template <class T> BOOL
QuadTreeNode<T>::GetAt(const MapPoint &point, DynamicArray<T> &array)
{
	if(m_isLeaf) {
		MapPoint objPos = m_tree->GetPos(m_array[0]);
		if(objPos.x == point.x && objPos.y == point.y) {
			array.Clear();
			sint32 i;
			for(i = 0; i < m_array.Num(); i++) {
				if(m_tree->GetPos(m_array[i]) == point) {
					array.Insert(m_array[i]);
				}
			}
			if(array.Num() > 0)
				return TRUE;
			else
				return FALSE;
		}
		return FALSE;
	} else {
		QUADRANT quad = FindQuadrant(point);
		switch(quad) {
			case QUADRANT_NE:
				if(!m_ne) return FALSE;
				return m_ne->GetAt(point, array);
			case QUADRANT_SE:
				if(!m_se) return FALSE;
				return m_se->GetAt(point, array);
			case QUADRANT_SW:
				if(!m_sw) return FALSE;
				return m_sw->GetAt(point, array);
			case QUADRANT_NW:
				if(!m_nw) return FALSE;
				return m_nw->GetAt(point, array);
		}
#pragma warning (disable : 4127)									
		Assert(FALSE);
#pragma warning (default : 4127)									
		return FALSE;
	}
}
		
template <class T> sint32
QuadTreeNode<T>::GetCount(MapPoint &point)
{
	if(m_isLeaf) {
		MapPoint objPos = m_tree->GetPos(m_array[0]);
		if(objPos.x == point.x && objPos.y == point.y) {
			return m_array.Num();
		} else {
			return 0;
		}
	} else {
		QUADRANT quad = FindQuadrant(point);
		switch(quad) {
			case QUADRANT_NE:
				if(!m_ne) return 0;
				return m_ne->GetCount(point);
			case QUADRANT_SE:
				if(!m_se) return 0;
				return m_se->GetCount(point);
			case QUADRANT_SW:
				if(!m_sw) return 0;
				return m_sw->GetCount(point);
			case QUADRANT_NW:
				if(!m_nw) return 0;
				return m_nw->GetCount(point);
		}
		Assert(FALSE);
		return 0;
	}
}

template <class T> BOOL
QuadTreeNode<T>::RemoveAt(const MapPoint &point, T &removedObj)
{
	if(m_isLeaf) {
		MapPoint objPos;
		objPos = m_tree->GetPos(m_array[0]);
		if(objPos.x != point.x || objPos.y != point.y)
			return FALSE;

		removedObj = m_array[0];

		sint32 i;
		for(i = m_array.m_nElements - 1; i >= 0; i--) {
			m_array.DelIndex(i);
		}

		if(m_array.Num() <= 0) {
			if(m_parent) {
				m_parent->RemoveBranch(this);
			} else {
				Assert(m_tree->m_top == this);
				m_tree->RemoveTop();
			}
		}
		return TRUE;
		return FALSE;
	} else {
		QUADRANT quad = FindQuadrant(point);
		switch(quad) {
			case QUADRANT_NE:
				if(!m_ne) return FALSE;
				return m_ne->RemoveAt(point, removedObj);
			case QUADRANT_SE:
				if(!m_se) return FALSE;
				return m_se->RemoveAt(point, removedObj);
			case QUADRANT_SW:
				if(!m_sw) return FALSE;
				return m_sw->RemoveAt(point, removedObj);
			case QUADRANT_NW:
				if(!m_nw) return FALSE;
				return m_nw->RemoveAt(point, removedObj);
		}
		Assert(FALSE);
		return FALSE;
	}
}

template <class T> BOOL
QuadTreeNode<T>::RemoveAt(const MapPoint &point, DynamicArray<T> &array)
{
	if(m_isLeaf) {
		MapPoint objPos;
		objPos = m_tree->GetPos(m_array[0]);
		if(objPos.x != point.x || objPos.y != point.y)
			return FALSE;
		array.Concat(m_array);
		if(m_parent) {
			m_parent->RemoveBranch(this);
		} else {
			Assert(m_tree->m_top == this);
			m_tree->RemoveTop();
		}
		return TRUE;
	} else {
		QUADRANT quad = FindQuadrant(point);
		switch(quad) {
			case QUADRANT_NE:
				if(!m_ne) return FALSE;
				return m_ne->RemoveAt(point, array);
			case QUADRANT_SE:
				if(!m_se) return FALSE;
				return m_se->RemoveAt(point, array);
			case QUADRANT_SW:
				if(!m_sw) return FALSE;
				return m_sw->RemoveAt(point, array);
			case QUADRANT_NW:
				if(!m_nw) return FALSE;
				return m_nw->RemoveAt(point, array);
		}
		Assert(FALSE);
		return FALSE;
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
	if(m_isLeaf)
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
	
	if(!m_top)
		return;

	m_top->RemoveObject(obj);
}

template <class T> BOOL QuadTree<T>::RemoveAt(const MapPoint &point, T &removedObj)
{
	BOOL ret;
	
	if(!m_top)
		return FALSE;

	MapPoint pos = point;

	Convert(pos);
	ret = m_top->RemoveAt(pos, removedObj);
	return ret;
}

template <class T> BOOL QuadTree<T>::RemoveAt(const MapPoint &point, DynamicArray<T> &array)
{
	BOOL ret;
	if(!m_top)
		return FALSE;

	MapPoint pos = point;
	Convert(pos);
	ret = m_top->RemoveAt(pos, array);
	return ret;
}

template <class T> BOOL QuadTree<T>::GetAt(const MapPoint &point, T &obj)
{
	if(!m_top)
		return FALSE;

	MapPoint pos = point;
	Convert(pos);
	return m_top->GetAt(pos, obj);
}

template <class T> BOOL QuadTree<T>::GetAt(const MapPoint &point, 
										   DynamicArray<T> &array)
{
	if(!m_top)
		return FALSE;

	MapPoint pos = point;
	Convert(pos);
	return m_top->GetAt(pos, array);
}

template <class T> sint32 QuadTree<T>::GetCount(MapPoint &point)
{
	if(!m_top)
		return 0;

	MapPoint pos = point;
	Convert(pos);
	return m_top->GetCount(pos);
}

template <class T> void QuadTree<T>::Clear()
{
	if(m_top != NULL) {
		delete m_top;
		m_top = NULL;
	}
}


template <class T> void
QuadTree<T>::Convert(MapPoint &pos)
{
	pos.y += pos.x;
	pos.y = pos.y % m_height;
}

template <class T> MapPoint
QuadTree<T>::GetPos(T obj)
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
		sint16 eheight;
		MapPoint nePos;
		nePos.x = 0;
		nePos.y = pos.y-m_width;
		if(nePos.y < 0)
			nePos.y += m_height;

		eheight = height;
		if(nePos.y + height >= m_height) {
			eheight = m_height - nePos.y;
		}

		m_top->SearchRect(array, nePos, width - curwidth, eheight, mask);

		if(eheight != height) {
			MapPoint sePos;
			sePos.x = 0;
			sePos.y = 0;
			m_top->SearchRect(array, sePos, width - curwidth, height - eheight, mask);
		}
	}

	if(curheight != height) {
		MapPoint swPos;
		swPos.x = pos.x;
		swPos.y = 0;
		m_top->SearchRect(array, swPos, width, height - curheight, mask);
	}
}




template <class T> void
QuadTree<T>::BuildList(DynamicArray<T> &array, uint32 mask) const
{
	if(!m_top)
		return;

	m_top->BuildList(array, mask);
}

template <class T> void
QuadTree<T>::GarbageCollector()
{
	static BOOL collecting = FALSE;
	if(collecting)
		return;
	if(!m_top)
		return;

	if(m_degenerateCount > k_MAX_DEGENERATE_COUNT) {
		collecting = TRUE;
		DynamicArray<T> rebuildList;
		m_top->RemoveDegenerate(rebuildList);

		for(sint32 i = 0; i < rebuildList.Num(); i++) {
			Insert(rebuildList[i]);
		}
		m_degenerateCount = 0;
		collecting = FALSE;
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
