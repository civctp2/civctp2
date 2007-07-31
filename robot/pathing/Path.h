












#pragma once
#ifndef __PATH_H__
#define __PATH_H__

#include "AstarPnt.h"
#include "DynArr.h"
#include "directions.h"

struct Direction { 

sint8 dir; 

Direction () { dir = sint8(NOWHERE); }

Direction (WORLD_DIRECTION val) { dir = val; } 

void Castrate() {}
void DelPointers() {}  

sint8 operator=(sint8 val) { return dir = val; } 
sint8 operator=(WORLD_DIRECTION val) { return dir = sint8(val); } 

void Serialize(CivArchive &archive); 
};

class Path { 
	
	
	
    sint32 m_next_dir; 
	
	

	
	
	
	MapPoint m_start; 
	DynamicArray<Direction> m_step; 
	
	

	
	
	
	
    MapPoint m_current; 
    sint32 m_next; 
	
	

	friend class NetOrder;

public:

    Path(); 
	Path(const Path *copy);
    void FlattenAstarList(AstarPoint *best);
    void FlattenNormalizedPointList(const MapPoint &start, DynamicArray<MapPoint> &pixel);
    void SetStart(const MapPoint &s) { m_start = s; m_step.Clear(); } 
    void Clear(); 
    sint32 Num() { return m_step.Num() + ((m_start.x == -1) ? 0 : 1); }

    void Start(MapPoint &p); 
	void Restart(MapPoint &p);
    void JustSetStart(const MapPoint &p);


    sint32 IsEnd() const; 
    sint32 Next(MapPoint &p); 
    void GetCurrentPoint(MapPoint &p) const; 
    void ClipStartToCurrent();

    void GetStartPoint(MapPoint &pos); 
    void StartDir(WORLD_DIRECTION &d);
    void GetCurrentDir(WORLD_DIRECTION &d);
    sint32 IsEndDir();
    void IncDir();

	void ConcatReturnPath();
	void Concat(Path &otherpath);

    void Serialize(CivArchive &archive); 
    void InsertFront(const MapPoint &pos);
    void PrependDir (sint32 dir);
    void AddDir(const WORLD_DIRECTION d); 

    void InsertEnd(const MapPoint &pos); 
    void SnipEnd();
    void SpaceMove(const MapPoint &start, const MapPoint &dest); 

	sint32 GetMovesRemaining();
	sint32 GetStep() { return m_next_dir; }
	void SetStep(sint32 next) { m_next_dir = next; }

	MapPoint GetEnd() const;

	
	sint32 GetNextIndex() const;
	void RestoreIndexAndCurrentPos(const sint32 & index);

	
	void SnipEndUntilCanEnter(const uint32 & movement_type);

};

#endif
