#pragma once
#ifndef __FS_GOLD_H__
#define __FS_GOLD_H__ 1

#include "FrameState.h"

class FSAllocScience : public FrameState {

public:
    FSAllocScience ();
    void Serialize(CivArchive &archive);

    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);
};

class CityAgent;
class FSCompeteForGold : public FrameState {

public:
    FSCompeteForGold();
    void Serialize(CivArchive &archive);

    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

    double EstimateExpense(AiMain *ai);

    void MakeUtilityList(AiMain *ai);
    BOOL PretestOvertime(AiMain *ai, CityAgent *ca);
    void SpendGold(AiMain *ai);
};

#endif __FS_GOLD_H__
