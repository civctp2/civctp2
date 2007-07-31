

#pragma once

#ifndef __FS_FUZZY_H__
#define __FS_FUZZY_H__ 1

#include "FrameState.h"

class AiMain; 
class CivArchive; 

class FSFuzzyBeginTurnPre : public FrameState { 
private:

    friend AiMain; 

public:
	

    void Serialize(CivArchive &archive); 
   
    
    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

    void ComputeLandContinentFull(AiMain *ai, double &continent_full, bool include_enemies);

} ;

class FSFuzzyBeginTurnDiplomacy : public FrameState{ 

private:
    void ComputeNormalizedStr(AiMain *ai);
    void UpdateGeneralDiplomacy(AiMain *ai);
	void ComputeRelativeStrongestEnemyStr(AiMain *ai);

public:

     void Serialize(CivArchive &archive); 

     BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

};

class FSFuzzyBeginTurnRules : public FrameState { 

public: 

     BOOL m_registered_variables;

     FSFuzzyBeginTurnRules() { m_registered_variables = FALSE; }

     void Serialize(CivArchive &archive); 

     BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);
}; 

FRAMESTATE(FSFuzzyBeginTurnPost);

FRAMESTATE(FSFuzzyEndTurn);
FRAMESTATE(FSFuzzyDiplomacy);

#endif __FS_FUZZY_H__
