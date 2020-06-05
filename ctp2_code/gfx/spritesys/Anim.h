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

#include <windows.h>    // FILE, POINT

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Anim;

enum ANIMTYPE
{
	ANIMTYPE_SEQUENTIAL,
	ANIMTYPE_LOOPED,
	ANIMTYPE_IDLE,
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
	static Anim * MakeFakeDeath();
	static Anim * MakeFaceoff();
	static Anim * CreateSequential(const Anim & copy) { return CopySetType(copy, ANIMTYPE_SEQUENTIAL); }
	static Anim * CreateLooping(const Anim & copy) { return CopySetType(copy, ANIMTYPE_LOOPED); }
	static Anim * CreateIdle(const Anim & copy) { return CopySetType(copy, ANIMTYPE_IDLE); }
	static Anim * CreateFromTokens(Token * tokens);

	// TODO: deprecate
	Anim(const Anim & copy);
	~Anim();

	uint16 GetNumFrames() const { return m_numFrames; }
	uint16 GetPlaybackTime() const { return m_playbackTime; }
	uint16 GetDelay() const { return m_delay; }
	uint16 GetCurrentFrame() const;
	uint16 GetCurrentTransparency() const;

	void   Process();
	bool   IsFinished() const { return m_finished; }
	void   Rewind();

	void   Export(FILE * file);

protected:
	friend class SpriteFile;

	static Anim * CopySetType(const Anim & copy, ANIMTYPE type);

	Anim();
	bool ParseFromTokens(Token * tokens);

	ANIMTYPE m_type;
	uint16   m_numFrames;
	uint16 * m_frames;
	POINT  * m_moveDeltas;
	uint16 * m_transparencies;

	sint32   m_animPos;
	bool     m_finished;

	uint16   m_playbackTime;
	uint16   m_delay; // Not used but read and set from sprite file
};

#endif
