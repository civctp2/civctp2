//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Game event callbacks
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// _DEBUG
// Generate extra debugging output.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Redesigned (replaced PointerList with std::list) to repair memory leaks.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __GAME_EVENT_HOOKS_H__
#define __GAME_EVENT_HOOKS_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <list>                 // std::list

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class GameEventHook;
class GameEventHookCallback;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "GameEventArgList.h"   // GameEventArgList
#include "GameEventTypes.h"     // GAME_EVENT...
#include "pointerlist.h"

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class GameEventHookCallback 
{
public:
    virtual GAME_EVENT_HOOK_DISPOSITION GEVHookCallback
    (
        GAME_EVENT         type, 
        GameEventArgList * args
    ) 
    { 
        return GEV_HD_Continue;
    };

    virtual void GetDescription(char * str, sint32 maxsize) 
    { 
        strncpy(str, DESCRIPTION_MISSING, maxsize);
    };

    static char const DESCRIPTION_MISSING[];
};

	
class GameEventHook 
{
public:
    struct Node 
    {
	    Node(GameEventHookCallback * cb, GAME_EVENT_PRIORITY pri) 
        :   m_cb        (cb),
            m_priority  (pri)
        { ; };

	    GameEventHookCallback * m_cb;
	    GAME_EVENT_PRIORITY     m_priority;
    };

	GameEventHook(GAME_EVENT type);
	virtual ~GameEventHook();

	void AddCallback(GameEventHookCallback * cb, GAME_EVENT_PRIORITY pri);
	void RemoveCallback(GameEventHookCallback * cb);

	GAME_EVENT_ERR      Activate
    (
        GameEventArgList *  args, 
        sint32              startIndex, 
        sint32 &            resumeIndex
    );


#if defined(_DEBUG)
	void Dump(FILE * f) const;
#endif

private:
    GAME_EVENT_ERR      Run
    (
        std::list<Node>::iterator &     startAt,
        GameEventArgList *              args, 
        sint32 &                        resumeIndex
    );

	GAME_EVENT              m_type;
    std::list<Node>         m_callbacks;
};


#endif
