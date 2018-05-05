#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _INSTALALLATIONTREE_H_
#define _INSTALALLATIONTREE_H_

#include "gs/utility/QuadTree.h"
#include "gs/gameobj/installation.h"

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
