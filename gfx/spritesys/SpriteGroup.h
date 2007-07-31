










 
#pragma once
#ifndef __SPRITEGROUP_H__
#define __SPRITEGROUP_H__

#include "Action.h"
#include "FacedSprite.h"

enum TOKEN_TYPES;

#define k_NUM_FIREPOINTS		8

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

class Sprite;
class aui_Surface;
class aui_DirectSurface;

class SpriteGroup 
{
public:
	SpriteGroup(GROUPTYPE type);
	virtual ~SpriteGroup();

	virtual void	LoadBasic   (char *filename){};
	virtual void	LoadFull    (char *filename){};
	virtual void	LoadIndexed (char *filename,GAME_ACTION action){};

	virtual void	Save(char *filename,unsigned version_id,unsigned compression_mode){};
	virtual sint32	Parse(uint16 id,GROUPTYPE group){ return FALSE;};

	virtual void	DeallocateStorage(void);
	virtual void	DeallocateFullLoadAnims(void) {}

	virtual void	Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, 
					  uint16 transparency, Pixel16 outlineColor, uint16 flags);
	virtual void	DrawText(sint32 x, sint32 y, char *s);

	virtual void	AddRef(void);
	virtual void	Release(void);
	
	virtual void	AddFullLoadRef(void);
	virtual void	ReleaseFullLoad(void);

	sint32			GetRefCount(void) { return m_usageRefCount; }
	sint32			GetFullLoadRefCount(void) { return m_fullLoadRefCount; }

	GROUPTYPE		GetType(void) { return m_type; }

	LOADTYPE		GetLoadType(void) { return m_loadType; }
	void			SetLoadType(LOADTYPE type) { m_loadType = type; }

	
	Sprite			*GetGroupSprite(GAME_ACTION action) { return m_sprites[action]; }
	void			SetGroupSprite (GAME_ACTION action, Sprite *sprite) { m_sprites[action] = sprite; }

	Anim			*GetGroupAnim(uint32 action) { return m_anims[action]; }
	void			SetGroupAnim (GAME_ACTION action, Anim *anim) { m_anims[action] = anim; }

	Anim			*GetAnim(GAME_ACTION action) { return m_anims[action]; }

	sint32			GetWidth(void) { return m_width; };
	sint32			GetHeight(void) { return m_height; };

	sint32			GetNumFrames(GAME_ACTION action);
    virtual void   	ExportSpriteGroup(FILE *file,GAME_ACTION action,TOKEN_TYPES main_token,TOKEN_TYPES sub_token,BOOL sub_value=FALSE);

	BOOL			HasDirectional(void) { return m_hasDirectional; }
	void			SetHasDirectional(BOOL val) { m_hasDirectional = val; }
 
	BOOL			HasDeath(void) { return m_hasDeath; }
	void			SetHasDeath(BOOL val) { m_hasDeath = val; }

  
  

private:
	
	GROUPTYPE		m_type;

protected:
	sint32			m_width, m_height;
	
	sint32			m_usageRefCount;			
	sint32			m_fullLoadRefCount;			

	LOADTYPE		m_loadType;

	Sprite			*m_sprites[ACTION_MAX];
	Anim			*m_anims  [ACTION_MAX];

	BOOL			m_hasDeath;
	BOOL			m_hasDirectional;

  
  
};


#endif
