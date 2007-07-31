









#include "c3.h"
#include "installationpool.h"
#include "Player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "CivArchive.h"
#include "Globals.h"


InstallationPool::InstallationPool() : ObjPool(k_BIT_GAME_OBJ_TYPE_INSTALLATION)
{
}

InstallationPool::InstallationPool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_INSTALLATION)
{
	Serialize(archive);
}

Installation InstallationPool::Create(sint32 owner,
									  MapPoint &pnt,
									  sint32 type)
{
	InstallationData *newData;
	Installation newInstallation(NewKey(k_BIT_GAME_OBJ_TYPE_INSTALLATION));

	newData = new InstallationData(newInstallation, owner, pnt, type);

	Insert(newData);
	g_player[owner]->AddInstallation(newInstallation);
	g_theWorld->InsertInstallation(newInstallation, pnt);
	newData->DoVision();
	return newInstallation;
}

void InstallationPool::Serialize(CivArchive &archive)
{
	sint32	i,
			count = 0 ;

	InstallationData *instData;

#define INSTPOOL_MAGIC 0x529FA3D1

    CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.PerformMagic(INSTPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++)
			if(m_table[i])
				count++;

		archive<<count;
		for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) {
			if(m_table[i])
				((InstallationData*)(m_table[i]))->Serialize(archive);
		}
	} else {
		archive.TestMagic(INSTPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		archive>>count;
		for (i=0; i<count; i++) {
			instData = new InstallationData(archive);
			Insert(instData);
		}
	}
}

void InstallationPool::RebuildQuadTree()
{
	sint32 i;
	for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) {
		if(m_table[i])
			((InstallationData*)(m_table[i]))->RebuildQuadTree();
	}
}


