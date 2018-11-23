#pragma once

#ifndef __FRAME_STATE_H__
#define __FRAME_STATE_H__ 1

class AiMain;
class CivArchive;

enum FRAME_STATE_INDEX

{
    FS_NULL,

    FS_BEGIN_TURN,

    FS_FUZZY_BEGIN_TURN_PRE,
    FS_FUZZY_BEGIN_TURN_DIPLOMACY,
    FS_FUZZY_BEGIN_TURN_RULES,
    FS_FUZZY_BEGIN_TURN_POST,
    FS_FUZZY_END_TURN,
    FS_FUZZY_DIPLOMACY,

    FS_SET_TAXES,
    FS_CONNECT_TRADE_ROUTES,
    FS_COMPETE_FOR_INSTALLATIONS,
    FS_COMPETE_FOR_ALL_MOVEMENT,

    FS_ALLOC_SCIENCE,
    FS_COMPETE_FOR_GOLD,

    FS_BUILD_RECRUITMENT,
    FS_BEGIN_WAR,
    FS_UPDATE_THREAT,

    FS_ALLOC_POP,

    FS_USE_MARKED,
    FS_CLEAN_UP,

    FS_UPDATE_CONTINENT,
    FS_MST,

	FS_DIPLOMACY,
    FS_WILL_OF_THE_PEOPLE,

    FS_OPPORTUNITY_ACTIONS,

    FS_END
};

class FrameState {

public:

	virtual BOOL Execute(AiMain *ai,  sint32 &branch,
        FILE *fout) = 0;

    virtual void Serialize(CivArchive &archive) = 0;
};

#define FRAMESTATE(fsclass) class fsclass : public FrameState { \
\
public:\
\
    BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout); \
    void Serialize(CivArchive &archive); \
};

#endif __FRAME_STATE_H__
