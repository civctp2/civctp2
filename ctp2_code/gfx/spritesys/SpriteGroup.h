//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Common sprite handling
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE 
#pragma once
#endif

#ifndef SPRITEGROUP_H__
#define SPRITEGROUP_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include "windows.h"          // BOOL, FILE

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

#define k_NUM_FIREPOINTS		8

class SpriteGroup;

enum GROUPTYPE 
{
	GROUPTYPE_GROUP,

	GROUPTYPE_UNIT,
	GROUPTYPE_PROJECTILE,
	GROUPTYPE_EFFECT,
	GROUPTYPE_CITY,
	GROUPTYPE_GOOD,


	GROUPTYPE_MAX
};

enum LOADTYPE {
	LOADTYPE_NONE = -1,

	LOADTYPE_BASIC,
	LOADTYPE_FULL,
	LOADTYPE_INDEXED,

	LOADTYPE_MAX
};

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "Action.h"
#include "ctp2_inttypes.h"	// sint32, uint16
#include "FacedSprite.h"
#include "Token.h"

class Anim;
class aui_Surface;
class Sprite;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------
 
class SpriteGroup 
{
public:
	SpriteGroup(GROUPTYPE type);
	virtual ~SpriteGroup();

	virtual void	LoadBasic   (MBCHAR const * filename) {};
	virtual void	LoadFull    (MBCHAR const * filename) {};
	virtual void	LoadIndexed (MBCHAR const * filename, GAME_ACTION action) {};

	virtual void	Save(MBCHAR const * filename, unsigned int version_id, unsigned int compression_mode){};
	virtual sint32	Parse(uint16 id,GROUPTYPE group){ return FALSE;};

	virtual void	DeallocateStorage(void);
	virtual void	DeallocateFullLoadAnims(void);

	virtual void	Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, 
					  uint16 transparency, Pixel16 outlineColor, uint16 flags);
	virtual void	DrawText(sint32 x, sint32 y, MBCHAR const * s);

	virtual void	AddRef(void);
	virtual void	Release(void);
	
	virtual void	AddFullLoadRef(void);
	virtual void	ReleaseFullLoad(void);

	sint32			GetRefCount(void) const { return m_usageRefCount; }
	sint32			GetFullLoadRefCount(void) const { return m_fullLoadRefCount; }

	GROUPTYPE		GetType(void) const { return m_type; }

	LOADTYPE		GetLoadType(void) const { return m_loadType; }
	void			SetLoadType(LOADTYPE type) { m_loadType = type; }

	
	Sprite *        GetGroupSprite(GAME_ACTION action) const { return m_sprites[action]; }
	void			SetGroupSprite (GAME_ACTION action, Sprite *sprite) { m_sprites[action] = sprite; }

	Anim *          GetGroupAnim(uint32 action) const { return m_anims[action]; }
	void			SetGroupAnim (GAME_ACTION action, Anim *anim) { m_anims[action] = anim; }

	Anim *          GetAnim(GAME_ACTION action) const { return m_anims[action]; }

	sint32			GetWidth(void) const { return m_width; };
	sint32			GetHeight(void) const { return m_height; };

	size_t			GetNumFrames(GAME_ACTION action) const;
    virtual void   	ExportSpriteGroup(FILE *file,GAME_ACTION action,TOKEN_TYPES main_token,TOKEN_TYPES sub_token,BOOL sub_value=FALSE);

	bool			HasDirectional(void) const { return m_hasDirectional; }
	void			SetHasDirectional(bool val) { m_hasDirectional = val; }
 
	bool			HasDeath(void) const { return m_hasDeath; }
	void			SetHasDeath(bool val) { m_hasDeath = val; }

  
  

private:
	
	GROUPTYPE		m_type;

protected:
	sint32			m_width, m_height;
	
	sint32			m_usageRefCount;			
	sint32			m_fullLoadRefCount;			

	LOADTYPE		m_loadType;

	Sprite			*m_sprites[ACTION_MAX];
	Anim			*m_anims  [ACTION_MAX];

	bool			m_hasDeath;
	bool			m_hasDirectional;
};


#endif
