









#pragma once
#ifndef _INSTALALLATIONTREE_H_
#define _INSTALALLATIONTREE_H_

#include "QuadTree.h"
#include "installation.h"

class InstallationQuadTree : public QuadTree<Installation>
{
public:

	InstallationQuadTree(sint16 width, sint16 height, BOOL yWrap) :
		QuadTree<Installation>(width, height, yWrap)
	{
	}
};

extern InstallationQuadTree *g_theInstallationTree;
#endif
