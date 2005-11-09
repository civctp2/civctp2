//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Animation handling
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired funny combination of protected data with non-virtual destructor.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __ANIM_H__
#define __ANIM_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <windows.h>    // BOOL, FILE, POINT

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Anim;

enum ANIMTYPE 
{
	ANIMTYPE_SEQUENTIAL,
	ANIMTYPE_LOOPED,
	ANIMTYPE_MAX
};

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"  // uint16, etc.
#include "Token.h"          // Token

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class Anim 
{
public:
	Anim();
    Anim(Anim const & copy);
    Anim const & operator = (Anim const & copy);
	~Anim();

	uint16		GetType(void) { return m_type; }
	uint16		GetNumFrames(void) { return m_numFrames; }
	uint16		*GetFrames(void) { return m_frames; }
	uint16		GetPlaybackTime(void);
	uint16		GetDelay(void) { return m_delay; }
	void		AdjustDelay(uint32 val) { m_delay += (uint16)val ; }

	POINT		*GetDeltas(void) { return m_moveDeltas; }
	uint16		*GetTransparencies(void) { return m_transparencies; }
	void		SetType(uint16 type) { m_type = (ANIMTYPE)type; }
	void		SetNumFrames(uint16 frames) { m_numFrames = frames; }
	void		SetPlaybackTime(uint16 time) { m_playbackTime = time; }
	void		SetDelay(uint16 time) { m_delay = time; }
	void		SetFrames(uint16 *frames) { m_frames = frames; }
	void		SetDeltas(POINT *deltas) { m_moveDeltas = deltas; }
	void		SetTransparencies(uint16 *t) { m_transparencies = t; }

	uint16		GetFrame(sint32 animPos);
	uint32		GetDelayEnd(void) {return m_delayEnd; } 
	uint32		GetElapsed(void) { return m_elapsed; }
	uint32		GetLastFrameTime(void) { return m_lastFrameTime; }
	sint32		GetNextPosition(sint32 animPos);

	void		SetDelayEnd(uint32 delayEnd) {m_delayEnd = delayEnd; } 
	void		SetElapsed(uint32 elapsed) {m_elapsed = elapsed; } 
	void		SetLastFrameTime(uint32 lastFrameTime) { m_lastFrameTime = lastFrameTime; }

	sint32		ParseFromTokens(Token *theToken);
	void		Export(FILE *file);

	BOOL		isLoopFinished(void) { return m_loopFinished; }
	BOOL		Finished(void) { return m_finished; }
	void		SetFinished(BOOL fin) { m_finished = fin; }

	BOOL		GetWeAreInDelay(void) { return m_weAreInDelay; }
	void		SetWeAreInDelay(BOOL val) { m_weAreInDelay = val; }

	void		SetNoIdleJustDelay(BOOL val) { m_noIdleJustDelay = val; }

protected:
	uint16		m_type;
	uint16		m_numFrames;
	uint16		*m_frames;			
	POINT		*m_moveDeltas;		
	uint16		*m_transparencies;	

	uint16		m_playbackTime;		
	uint16		m_delay;			
	uint32		m_delayEnd;			

	uint32		m_lastFrameTime;	
	uint32		m_elapsed;			

	BOOL		m_loopFinished;		
	BOOL		m_finished;			

	BOOL		m_weAreInDelay;		
	BOOL		m_noIdleJustDelay; 
};

#endif
