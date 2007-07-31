









#pragma once
#ifndef _INSTALLATIONPOOL_H_
#define _INSTALLATIONPOOL_H_

#include "ObjPool.h"
#include "Installation.h"

class CivArchive;

class InstallationPool : public ObjPool
{
public:
	InstallationPool();
	InstallationPool(CivArchive &archive);
	
	InstallationData *AccessInstallation(const Installation id)
	{
		return (InstallationData*)Access(id);
	}

	InstallationData *GetInstallation(const Installation id)
	{
		return (InstallationData*)Get(id);
	}

	Installation Create(sint32 owner,
						MapPoint &pnt,
						sint32 type);
	void Remove(Installation id);

	void Serialize(CivArchive &archive);
	void RebuildQuadTree();
};

extern InstallationPool *g_theInstallationPool;

#endif
