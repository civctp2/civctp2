











#pragma once
#ifndef __UNITPOOL_H__
#define __UNITPOOL_H__ 1

#include "ObjPool.h"

#include "Unit.h"

class UnitData;
class CivArchive ;
class MapPoint; 
class UnitData;
class UnitRecord;

#define k_UNITPOOL_VERSION_MAJOR	0								
#define k_UNITPOOL_VERSION_MINOR	0								


class UnitPool : public ObjPool { 
    
	

	
	

	
	
	

	
	

public:
    
	inline UnitData * AccessUnit(uint32 id){  return (UnitData * ) Access(id); };  
	inline UnitData * GetUnit(uint32 id) const {  return (UnitData * ) Get(id); };

	UnitPool();
	UnitPool(CivArchive &archive) ;

	Unit Create (sint32 t, const PLAYER_INDEX owner, const MapPoint &pos, const Unit hc, UnitActor *actor = NULL);
	Unit Create (sint32 t, const PLAYER_INDEX owner, const MapPoint &actor_pos);

	const UnitRecord * GetDBRec(const Unit id) const;
  
	void Serialize(CivArchive &archive) ;
	void RebuildQuadTree();
};

extern UnitPool *g_theUnitPool;

uint32 UnitPool_UnitPool_GetVersion(void) ;
#else

class UnitPool; 

#endif
