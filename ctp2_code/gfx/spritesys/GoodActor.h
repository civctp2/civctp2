//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handling of good animations on screen
// Id           : $Id$
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//----------------------------------------------------------------------------
///
/// \file   GoodActor.h
/// \brief  Handling of good animations on screen

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef GOODACTOR_H__
#define GOODACTOR_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

class GoodActor;

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "Actor.h"              // Actor
#include "civarchive.h"         // CivArchive
#include "ctp2_inttypes.h"      // sint32, uint16
#include "GoodSpriteGroup.h"    // GOODACTION
#include "Queue.h"              // Queue
#include "SpriteGroup.h"        // LOADTYPE
#include "World.h"              // MapPoint

class Action;
class Anim;
class aui_Surface;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class GoodActor : public Actor
{
public:
	GoodActor(sint32 index, const MapPoint & pos);
	GoodActor(const GoodActor & a_Original);
	GoodActor(CivArchive & archive);

	virtual ~GoodActor();
	GoodActor & operator = (const GoodActor & a_Original);

	void     Process();
	void     Reload(LOADTYPE loadType);

	void     PositionActor(const MapPoint & pos);
	void     AddAction(Action * action);

	bool     Draw(bool fogged = false) const;
	void     DrawDirect(aui_Surface * surf, sint32 x, sint32 y, double scale) const;

	MapPoint GetPos() const { return m_pos; }
	uint16   GetWidth() const;
	uint16   GetHeight() const;
	POINT    GetHotpoint() const;
	void     GetBoundingRect(RECT * rect) const;

	void     Serialize(CivArchive & archive);

protected:
	void   AddIdle();
	void   GetNextAction();

	Anim * CreateAnim(GOODACTION action) const;

	void   FullLoad();
	void   DumpFullLoad();

	void   DrawSelectionBrackets() const;
	void   DrawText(sint32 x, sint32 y, const MBCHAR * goodText) const;

	sint32            m_facing;
	sint32            m_frame;
	uint16            m_transparency;
	sint32            m_index;
	MapPoint          m_pos;
	GoodSpriteGroup	* m_goodSpriteGroup;
	Action          * m_curAction;
	GOODACTION        m_curGoodAction;
	Queue<Action *>   m_actionQueue;
	LOADTYPE          m_loadType;
	uint32            m_nextIdleAction;
};

#endif
