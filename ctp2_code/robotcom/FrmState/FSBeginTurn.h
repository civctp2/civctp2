#pragma once

#ifndef __FS_BEGIN_TURN__
#define __FS_BEGIN_TURN__ 1

#include "FrameState.h"

class ArmyAgent;
class CivArchive;


FRAMESTATE(FSBeginTurn);
FRAMESTATE(FSUpdateContinent);
FRAMESTATE(FSWillOfThePeople);

template <class T> class DynamicArray;

class FSAllocatePopulation : public FrameState {

public:

    FSAllocatePopulation ();
    ~FSAllocatePopulation();

    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

    void Serialize(CivArchive &archive);

};





























class FSCleanup : public FrameState {

    DynamicArray<ArmyAgent *> *m_dead_agents;

public:

    FSCleanup();
	~FSCleanup();

    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

    void Serialize(CivArchive &archive);
};


#endif __FS_BEGIN_TURN__
