









#pragma once
#ifndef _UNSEEN_CELL_QUAD_TREE_H_
#define _UNSEEN_CELL_QUAD_TREE_H_

#include "QuadTree.h"
#include "UnseenCell.h"

class UnseenCellQuadTree : public QuadTree<UnseenCellCarton>
{

public:
	uint32 GetFlags(UnseenCellCarton cell) { return 0xffffffff; }

	UnseenCellQuadTree(sint16 width, sint16 height, BOOL yWrap) :
		QuadTree<UnseenCellCarton>(width, height, yWrap)
	{
	}
	void Convert(MapPoint &pos) {}
	void Clear();
	
	
	~UnseenCellQuadTree()
	{
		
	}
};

void UnseenCellQuadTree::Clear()
{
	DynamicArray<UnseenCellCarton> array;
	BuildList(array);
	sint32 i, n = array.Num();
	for(i = 0; i < n; i++) {
		delete array[i].m_unseenCell;
	}
	QuadTree<UnseenCellCarton>::Clear();
}

#endif

