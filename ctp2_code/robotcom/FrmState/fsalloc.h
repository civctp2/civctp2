#ifndef __FS_ALLOC_H__
#define __FS_ALLOC_H__ 1

#include "FrameState.h"

FRAMESTATE(FSSetTaxes);

class FSConnectTradeRoutes : public FrameState {

public:
    FSConnectTradeRoutes();
    void Serialize(CivArchive &archive);

    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

    void FindUnconnectedGoods(AiMain *ai);
    void CollectCityBids(AiMain *ai);
    void ConnectGoods(AiMain *ai, BOOL &cross_talk);

};

class FSCompeteForInstallations : public FrameState {

public:
    FSCompeteForInstallations();
    ~FSCompeteForInstallations();
    void Serialize (CivArchive &archive);
    BOOL Execute(AiMain *ai, sint32 &branch,FILE *fout);

    void MakeCostBenefitList(AiMain *ai);
    void SpendProduction(AiMain *ai);

    void LayCellInst(AiMain *ai, sint32 inst_type, sint32 utility);
    void LayRoads(AiMain *ai);

};

#endif __FS_ALLOC_H__
