//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Token handling for the old style database files.
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
// - #pragma once commented out.
// - Added new token so that the loading of default strings can be 
//   prevented. (Jan 30th 2006 Martin Gühmann)
// - Removed unused tokens. (July 15th 2006 Martin Gühmann)
// - Added new token so that the loading of default strings in scenarios
//   can be skipped. (9-Apr-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef TOKEN_H__
#define TOKEN_H__

class Token;

enum TOKEN_TYPES 
{ 
	TOKEN_STRING,
	TOKEN_QUOTED_STRING,
	TOKEN_MISSING_QUOTE,

	TOKEN_NUMBER,
	TOKEN_EOF,

	TOKEN_UNKNOWN,

	TOKEN_OPEN_BRACE,
	TOKEN_CLOSE_BRACE,

	TOKEN_IMPORT,

	TOKEN_UNIT_SPRITE,
	TOKEN_PROJECTILE_SPRITE,
	TOKEN_EFFECT_SPRITE,

	TOKEN_SPRITE_NUM_FRAMES,
	TOKEN_SPRITE_FIRST_FRAME,
	TOKEN_SPRITE_WIDTH,
	TOKEN_SPRITE_HEIGHT,
	TOKEN_SPRITE_HOT_POINT,
	TOKEN_SPRITE_HOT_POINTS,

	TOKEN_UNIT_SPRITE_MOVE,
	TOKEN_UNIT_SPRITE_ATTACK,
	TOKEN_UNIT_SPRITE_VICTORY,
	TOKEN_UNIT_SPRITE_IDLE,
	TOKEN_UNIT_SPRITE_WORK,
	TOKEN_UNIT_SPRITE_FIREPOINTS,
	TOKEN_UNIT_SPRITE_FIREPOINTS_WORK,
	TOKEN_UNIT_SPRITE_MOVEOFFSETS,
	TOKEN_UNIT_SPRITE_SHIELDPOINTS,
	TOKEN_UNIT_SPRITE_SHIELDPOINTS_MOVE,
	TOKEN_UNIT_SPRITE_SHIELDPOINTS_ATTACK,
	TOKEN_UNIT_SPRITE_SHIELDPOINTS_IDLE,
	TOKEN_UNIT_SPRITE_SHIELDPOINTS_VICTORY,
	TOKEN_UNIT_SPRITE_SHIELDPOINTS_WORK,
	TOKEN_UNIT_SPRITE_IS_DEATH,
	TOKEN_UNIT_SPRITE_ATTACK_IS_DIRECTIONAL,

	TOKEN_PROJECTILE_SPRITE_MOVE,
	TOKEN_PROJECTILE_SPRITE_FIREPOINTS,
	TOKEN_PROJECTILE_SPRITE_MOVEOFFSETS,

	TOKEN_EFFECT_SPRITE_PLAY,
	TOKEN_EFFECT_SPRITE_FLASH,
	TOKEN_EFFECT_SPRITE_FIREPOINTS,
	TOKEN_EFFECT_SPRITE_MOVEOFFSETS,

	TOKEN_GOOD_SPRITE,
	TOKEN_GOOD_SPRITE_IDLE,
	TOKEN_GOOD_GOLD_VALUE,
	TOKEN_GOOD_SOUND_ID,

	TOKEN_ANIM,
	TOKEN_ANIM_TYPE,
	TOKEN_ANIM_NUM_FRAMES,
	TOKEN_ANIM_PLAYBACK_TIME,
	TOKEN_ANIM_DELAY,
	TOKEN_ANIM_FRAME_DATA,
	TOKEN_ANIM_MOVE_DELTAS,
	TOKEN_ANIM_TRANSPARENCIES,



	TOKEN_TILESET_TILE,
	TOKEN_TILESET_TILE_BASE_TYPE,
	TOKEN_TILESET_TILE_TRANS_0,
	TOKEN_TILESET_TILE_TRANS_1,
	TOKEN_TILESET_TILE_TRANS_2,
	TOKEN_TILESET_TILE_TRANS_3,
	TOKEN_TILESET_MEGATILE,
	TOKEN_TILESET_MEGATILE_CONFIG,
	TOKEN_TILESET_MEGATILE_INFO,
	TOKEN_TILESET_TRANSFORM,
	TOKEN_TILESET_TRANSFORM_TO,
	TOKEN_TILESET_TRANSFORM_TO_LIST,
	TOKEN_TILESET_TRANSITION,
	TOKEN_TILESET_TRANSITION_FROM,
	TOKEN_TILESET_TRANSITION_TO,
	TOKEN_TILESET_RIVER_TRANSFORM,
	TOKEN_TILESET_RIVER_PIECE,
	TOKEN_TILESET_END,

	TOKEN_COLORSET,
	TOKEN_COLORSET_COLOR,

	TOKEN_DO_NOT_IMPORT_DEFAULTS,
	TOKEN_SCENARIO_DO_NOT_IMPORT_DEFAULTS,
	TOKEN_MAX
};

#define k_MAX_TOKEN_LEN _MAX_PATH

#include "c3files.h"
#include "dbtypes.h"
class StringDB; 

struct TokenData { 
	sint32 tok; 
	char keyword[k_MAX_NAME_LEN]; 
};

class Token 
{ 
public:
	Token(char const * fn, C3DIR dir, sint32 a_Count = 0, TokenData * il = NULL);
    /// @todo Remove when no longer referenced (backwards compatibility)
	Token(char *fn, sint32, TokenData *il, C3DIR dir);
	~Token(); 

    static char const * GetKeyword(sint32 tok);
    static bool IsWhitespace(char c);
    static bool IsNumber(char const * str);
    static bool ValidateAllTokens();

	sint32 GetType() const;
	void NextNumber();
	void NextString();

	bool    HandleImport(void);
	void    CloseImport(void);

	sint32 Next();
	void GetString(char *str);
	void GetNumber(sint32 &n); 
	void GetFloat(double &n);
	void SetCheckScenario(bool checkScenario){ m_checkScenario = checkScenario; };
	
	char const * ErrStr(); 
	
private:	
	FILE			*m_fin;
	char			m_filename[_MAX_PATH];
	char			m_buf[k_MAX_TOKEN_LEN]; 
	sint32			m_len, 
					m_index, 
					m_val_string_len, 
					m_current_type; 
	double			m_val_number ;
	char			m_val_string[k_MAX_TEXT_LEN]; 
	char			m_cur; 
	char			m_estr[_MAX_PATH + 10];
	
	sint32			m_num_it; 
	TokenData		*m_imported_tokens;
	
	C3DIR			m_dir;
	FILE			*m_importFile;

	MBCHAR			m_savedFilename[_MAX_PATH];
	sint32			m_savedLineNumber;
	FILE			*m_savedFin;
	bool			m_checkScenario;
	
	
	friend bool token_ParseValNext(Token *aToken, sint32 t, sint32 &val);
	friend bool token_ParseFloatNext(Token *aToken, sint32 t, double &val);
};


char const *    token_GetKeyword(sint32 i);
bool            token_ParseKeywordNext(Token *aToken, sint32 t);
bool            token_ParseValNext(Token *aToken, sint32 t, sint32 &val);
bool            token_ParseFloatNext(Token *aToken, sint32 t, double &val);
bool            token_ParseAnOpenBraceNext(Token *aToken);
bool            token_ParseAnCloseBraceNext(Token *aToken);
bool            token_ParseAnCloseBrace(Token *aToken);
POINT           token_ParsePoint(Token *theToken);

#endif
