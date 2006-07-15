//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// __MAKESPR__
// - Probably supposed to generate the sprite make tool.
//
// __TILETOOL__
// - Probably supposed to generate the tool for creating the *.til files.
//
// _JAPANESE
// - Use SJIS token parsing when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fix for japanese by t.s. 2003.12
// - Fix Token::Next() for japanese sjis code
// - Prevented crash on missing input file. 
// - Load default strings if they are missing in the database so that mods
//   also have a full set of strings. (Jan 30th 2006 Martin Gühmann)
// - Removed unused tokens. (July 15th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"

#include "c3files.h"

#include "Token.h"
#include "ErrMsg.h"
#include "Globals.h"
#include "StrDB.h"
#ifndef __MAKESPR__
#include "AdvanceRecord.h"
#include "BuildingRecord.h"
#endif

#if defined(_JAPANESE)
#include "japanese.h"
#endif

sint32 g_parse_line; 
sint32 g_saved_parse_line;
bool g_load_defaults;

extern sint32 g_abort_parse; 

extern StringDB *g_theStringDB;

TokenData g_allTokens [] = { 


	// The string database and common
	{TOKEN_STRING, "TOKEN_STRING"}, 
	{TOKEN_QUOTED_STRING,"TOKEN_QUOTED_STRING"},   
	{TOKEN_MISSING_QUOTE,"TOKEN_MISSING_QUOTE"},  
	{TOKEN_NUMBER, "TOKEN_NUMBER"},
	{TOKEN_EOF, "TOKEN_EOF"},
	{TOKEN_UNKNOWN, "TOKEN_UNKNOWN"},	
	
	{TOKEN_OPEN_BRACE, "{"}, 
	{TOKEN_CLOSE_BRACE, "}"}, 
	
	{TOKEN_IMPORT, "import"},

	// The sprite maker
	{TOKEN_UNIT_SPRITE,				"UNIT_SPRITE"},
	{TOKEN_PROJECTILE_SPRITE,		"PROJECTILE_SPRITE"},
	{TOKEN_EFFECT_SPRITE,			"EFFECT_SPRITE"},
	
	{TOKEN_SPRITE_NUM_FRAMES,		"SPRITE_NUM_FRAMES"},
	{TOKEN_SPRITE_FIRST_FRAME,		"SPRITE_FIRST_FRAME"},
	{TOKEN_SPRITE_WIDTH,			"SPRITE_WIDTH"},
	{TOKEN_SPRITE_HEIGHT,			"SPRITE_HEIGHT"},
	{TOKEN_SPRITE_HOT_POINT,			"SPRITE_HOT_POINT"},
	{TOKEN_SPRITE_HOT_POINTS,		"SPRITE_HOT_POINTS"},
	
	{TOKEN_UNIT_SPRITE_MOVE,			"UNIT_SPRITE_MOVE"},
	{TOKEN_UNIT_SPRITE_ATTACK,		"UNIT_SPRITE_ATTACK"},
	{TOKEN_UNIT_SPRITE_VICTORY,		"UNIT_SPRITE_VICTORY"},
	{TOKEN_UNIT_SPRITE_IDLE,			"UNIT_SPRITE_IDLE"},
	{TOKEN_UNIT_SPRITE_WORK,			"UNIT_SPRITE_WORK"},
	{TOKEN_UNIT_SPRITE_FIREPOINTS,	"UNIT_SPRITE_FIREPOINTS"},
	{TOKEN_UNIT_SPRITE_FIREPOINTS_WORK, "UNIT_SPRITE_FIREPOINTS_WORK"},
	{TOKEN_UNIT_SPRITE_MOVEOFFSETS,	"UNIT_SPRITE_MOVEOFFSETS"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS, "UNIT_SPRITE_SHIELDPOINTS"},
	
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_MOVE,	"UNIT_SPRITE_SHIELDPOINTS_MOVE"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_ATTACK,	"UNIT_SPRITE_SHIELDPOINTS_ATTACK"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_IDLE,	"UNIT_SPRITE_SHIELDPOINTS_IDLE"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_VICTORY, "UNIT_SPRITE_SHIELDPOINTS_VICTORY"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_WORK,	"UNIT_SPRITE_SHIELDPOINTS_WORK"},
	
	{TOKEN_UNIT_SPRITE_IS_DEATH, "UNIT_SPRITE_IS_DEATH"},
	{TOKEN_UNIT_SPRITE_ATTACK_IS_DIRECTIONAL, "UNIT_SPRITE_ATTACK_IS_DIRECTIONAL"},
	
	{TOKEN_PROJECTILE_SPRITE_MOVE,	"PROJECTILE_SPRITE_MOVE"},
	{TOKEN_PROJECTILE_SPRITE_FIREPOINTS,	"PROJECTILE_SPRITE_FIREPOINTS"},
	{TOKEN_PROJECTILE_SPRITE_MOVEOFFSETS,	"PROJECTILE_SPRITE_MOVEOFFSETS"},
	
	{TOKEN_EFFECT_SPRITE_PLAY,	"EFFECT_SPRITE_PLAY"},
	{TOKEN_EFFECT_SPRITE_FLASH,	"EFFECT_SPRITE_FLASH"},
	{TOKEN_EFFECT_SPRITE_FIREPOINTS,	"EFFECT_SPRITE_FIREPOINTS"},
	{TOKEN_EFFECT_SPRITE_MOVEOFFSETS,	"EFFECT_SPRITE_MOVEOFFSETS"},
	
	
	
	{TOKEN_GOOD_SPRITE,					"GOOD_SPRITE"},
	
	{TOKEN_GOOD_SPRITE_IDLE,			"GOOD_SPRITE_IDLE"},
	{TOKEN_GOOD_GOLD_VALUE,         "GOOD_GOLD_VALUE"},
	{TOKEN_GOOD_SOUND_ID,			"GOOD_SOUND_ID"},
	
	
	{TOKEN_ANIM,						"ANIM"},
	{TOKEN_ANIM_TYPE,				"ANIM_TYPE"},
	{TOKEN_ANIM_NUM_FRAMES,			"ANIM_NUM_FRAMES"},
	{TOKEN_ANIM_PLAYBACK_TIME,		"ANIM_PLAYBACK_TIME"},
	{TOKEN_ANIM_DELAY,				"ANIM_DELAY"},
	{TOKEN_ANIM_FRAME_DATA,			"ANIM_FRAME_DATA"},
	{TOKEN_ANIM_MOVE_DELTAS,			"ANIM_MOVE_DELTAS"},
	{TOKEN_ANIM_TRANSPARENCIES,		"ANIM_TRANSPARENCIES"},

	// The tile file generator
	{TOKEN_TILESET_TILE,			"TILESET_TILE"},
	{TOKEN_TILESET_TILE_BASE_TYPE,	"TILESET_TILE_BASE_TYPE"},
	{TOKEN_TILESET_TILE_TRANS_0,	"TILESET_TILE_TRANS_0"},
	{TOKEN_TILESET_TILE_TRANS_1,	"TILESET_TILE_TRANS_1"},
	{TOKEN_TILESET_TILE_TRANS_2,	"TILESET_TILE_TRANS_2"},
	{TOKEN_TILESET_TILE_TRANS_3,	"TILESET_TILE_TRANS_3"},
	{TOKEN_TILESET_MEGATILE,		"TILESET_MEGATILE"},
	{TOKEN_TILESET_MEGATILE_CONFIG,	"TILESET_MEGATILE_CONFIG"},
	{TOKEN_TILESET_MEGATILE_INFO,	"TILESET_MEGATILE_INFO"},
	
	{TOKEN_TILESET_TRANSFORM,		"TILESET_TRANSFORM"},
	{TOKEN_TILESET_TRANSFORM_TO,	"TILESET_TRANSFORM_TO"},
	{TOKEN_TILESET_TRANSFORM_TO_LIST,"TILESET_TRANSFORM_TO_LIST"},
	{TOKEN_TILESET_TRANSITION,		"TILESET_TRANSITION"},
	{TOKEN_TILESET_TRANSITION_FROM,	"TILESET_TRANSITION_FROM"},
	{TOKEN_TILESET_TRANSITION_TO,	"TILESET_TRANSITION_TO"},
	{TOKEN_TILESET_RIVER_TRANSFORM, "TILESET_RIVER_TRANSFORM"},
	{TOKEN_TILESET_RIVER_PIECE,		"TILESET_RIVER_PIECE"},

	{TOKEN_TILESET_END,				"TILESET_END"},

	//
	{TOKEN_COLORSET,				"COLORSET"},
	{TOKEN_COLORSET_COLOR,			"COLORSET_COLOR"},

	// Concept database
	{TOKEN_CONCEPT_DEFAULT_ICON,		"CONCEPT_DEFAULT_ICON"},
	{TOKEN_NO_INDEX, "NO_INDEX"},

	// For string database mod compatibility
	{TOKEN_DO_NOT_IMPORT_DEFAULTS,			"DoNotImportDefaults"},
	{TOKEN_MAX,								"TOKEN_MAX_DO_NOT_USE"},
	
};

char *token_GetKeyword(const sint32 i)

{ 
	Assert(0 <= i); 
	return  g_allTokens[i].keyword; 
}

sint32 Token::ValidateAllTokens()

{  sint32 i; 

for (i=0; i<TOKEN_MAX; i++) { 
	if (g_allTokens[i].tok != i) { 
		return FALSE; 
	} 
} 
return TRUE;
}








Token::Token
(
    char *      fn, 
    C3DIR       dir
) 
:	m_fin		        (c3files_fopen(dir, fn, "r")),
    m_len               (0),
	m_index             (0),
	m_val_string_len    (0), 
	m_current_type		(TOKEN_UNKNOWN), 
	m_val_number        (0),
	m_cur               (' '),
    m_num_it            (0),
	m_imported_tokens   (0),
	m_dir				(dir),
	m_importFile		(NULL),
	m_savedLineNumber   (0),
	m_savedFin			(NULL)
{
	Assert(ValidateAllTokens()); 
	
	strcpy(m_filename, fn);

    if (m_fin)
	{
		g_parse_line		= 0;
		g_saved_parse_line	= 0;
		Next();
	}
	else
	{
		c3errors_ErrorDialog("Token.cpp", "Could not open %s", m_filename); 
		g_abort_parse		= TRUE; 
	}
}


Token::Token
(
	 char *         fn,
	 sint32         n, 
	 TokenData *    it,
	 C3DIR	        dir
) 
:	m_fin		        (c3files_fopen(dir, fn, "r")),
    m_len               (0),
	m_index             (0),
	m_val_string_len    (0), 
	m_current_type		(TOKEN_UNKNOWN), 
	m_val_number        (0),
	m_cur               (' '),
    m_num_it            (n),
	m_imported_tokens   (it),
	m_dir				(dir),
	m_importFile		(NULL),
	m_savedLineNumber   (0),
	m_savedFin			(NULL)
{
	Assert(ValidateAllTokens()); 
	
	strcpy(m_filename, fn);
	
	if (m_fin)
	{
		g_parse_line		= 0;
		g_saved_parse_line	= 0;
		Next();
	}
	else
	{
		c3errors_ErrorDialog("Token.cpp", "Could not open %s", m_filename); 
		g_abort_parse		= TRUE; 
	}
}


Token::~Token()

{ 
	if (m_fin)
	{
		fclose(m_fin);
	}

	if (m_savedFin)
	{
		fclose(m_savedFin);
	}
}





















sint32 Token::IsNumber(char *str)
{
	
	
	
	if (*str == '-' || *str == '+')
		str++ ;
	
	
	if (!isdigit(*str++))
		return (FALSE) ;
	
	
	while (isdigit(*str))
		str++ ;
	
	
	if (0 == *str)
		return (TRUE) ;
	
	
	if (*str++ != '.')
		return (FALSE) ;
	
	
	if (!isdigit(*str++))
		return (FALSE) ;
	
	
	while (isdigit(*str))
		str++ ;
	
	
	if (*str)
		return (FALSE) ;											
	
	return (TRUE) ;													
}


sint32 Token::GetType ()

{ 
	return m_current_type; 
} 


void Token::NextNumber()

{ 
	m_current_type = TOKEN_NUMBER; 
	m_val_number = atof(m_buf); 
}

sint32 Token::IsWhitespace(const char c) 

{   return (c == ' ') || (c == '\t') || (c == 13) || (c == 10); 
} 

void Token::NextString()

{ 
	sint32 i, searching = TRUE; 
	
	for (i=TOKEN_UNKNOWN+1; i<TOKEN_MAX && searching; i++) { 
		if (strcmp(m_buf, g_allTokens[i].keyword) == 0) { 
			m_current_type = i; 
			searching = FALSE;
		}
	}
	
	if ((searching) && (m_num_it)) { 
		for (i=0; i<m_num_it && searching ; i++) { 
			if (strcmp(m_buf, m_imported_tokens[i].keyword) == 0) { 
				m_current_type = m_imported_tokens[i].tok; 
				searching = FALSE;
			}
		}
	}
	
	if (searching) { 
		m_current_type = TOKEN_STRING; 
		strcpy (m_val_string, m_buf); 
		m_val_string_len = strlen(m_val_string);    
	}
} 

sint32 Token::HandleImport(void)
{
	MBCHAR		fileName[256];
	
	if (Next() != TOKEN_QUOTED_STRING) {
		Assert(FALSE);
		return 0;
	}
	
	GetString(fileName);
	
	g_saved_parse_line = g_parse_line;
	g_parse_line = 0;
	
	strcpy(m_savedFilename, m_filename);
	strcpy(m_filename, fileName);

	m_savedFin = m_fin;
	m_importFile = c3files_fopen(m_dir, fileName, "r");
	
	if (!m_importFile) { 
		c3errors_ErrorDialog ("Token.cpp", "Could not open import file '%s'", fileName); 
		g_abort_parse = TRUE; 
		return 0;
	}
	
	m_fin = m_importFile;

	return 0;
}

sint32 Token::CloseImport(void)
{
	Assert(m_importFile);

	fclose(m_importFile);

	m_fin = m_savedFin;
	m_savedFin   = NULL;	// for safe destruction
	m_importFile = NULL;
	strcpy(m_filename, m_savedFilename);
	g_parse_line = g_saved_parse_line;
	g_saved_parse_line = 0;

	return 0;
}

sint32 Token::Next() 

{
	while (1) { 
		
		
		for ( ; (m_cur != EOF) && IsWhitespace(m_cur) ; m_cur = getc(m_fin)) {  
			if ((m_cur == 13) || (m_cur == 10)) { 
				g_parse_line++;
			}
		} 
		
		
		if (m_cur == '#') { 
			for ( ; (m_cur != EOF) && (m_cur != 10); m_cur = getc(m_fin)) { 
				if ((m_cur == 13) || (m_cur == 10)) { 
					g_parse_line++;
				}
			} 
		} else { 
			break; 
		} 
	} 


	if (m_cur == EOF) {  
		
		if (m_importFile != NULL) {
			CloseImport();

			m_cur = getc(m_fin);
			
			return Next();
		} else {
			m_current_type = TOKEN_EOF; 
			m_cur = ' '; 
		}
	} else if (m_cur == '{') { 
		m_current_type = TOKEN_OPEN_BRACE; 
		m_cur = ' '; 
	} else if (m_cur == '}') { 
		m_current_type = TOKEN_CLOSE_BRACE; 
		m_cur = ' '; 
	} else if (m_cur == '\"') { // HACK - FIX ME - check if this can deal with international 
		// charater set. 
		m_current_type = TOKEN_QUOTED_STRING; 
		m_cur = getc(m_fin);
		m_val_string_len = 0; 
		while (m_cur != '\"' && m_val_string_len < k_MAX_TEXT_LEN) { 
			
			







			if (m_cur == EOF){ 
				m_current_type = TOKEN_MISSING_QUOTE; 
				m_val_string[m_val_string_len] = 0; 
				return m_current_type; 
			} else if ((m_cur == 10) || (m_cur == 13)){ 
				m_val_string[m_val_string_len] = ' '; 
				m_val_string_len++;        
				m_cur = getc(m_fin); 
				g_parse_line++;                
				
			} else if((m_cur == '\\')) {
				m_cur = getc(m_fin);
				switch(m_cur) {
				case 'n':
					m_val_string[m_val_string_len] = 10;
					break;
				case 'r':
					m_val_string[m_val_string_len] = 13;
					break;
				case 't':
					m_val_string[m_val_string_len] = 8;
					break;
				default:
					m_val_string[m_val_string_len] = m_cur;
					break;
				}
				m_val_string_len++;
				m_cur = getc(m_fin);
			} else {    
				if (k_MAX_TEXT_LEN <= m_val_string_len) { 
					m_current_type = TOKEN_MISSING_QUOTE; 
					m_val_string[m_val_string_len] = 0; 
					return m_current_type; 
				} 
				
				m_val_string[m_val_string_len] = m_cur; 
				m_val_string_len++;
#if defined(_JAPANESE)
				 // japanese sjis-code
				if ( IS_SJIS_1ST(m_cur) ) {
					m_cur =  getc(m_fin); 
					if ( IS_SJIS_2ND(m_cur) ) {
						m_val_string[m_val_string_len] = m_cur; 
						m_val_string_len++;
						m_cur =  getc(m_fin); 
					}
				} else {
					m_cur =  getc(m_fin); 
				}
#else
				m_cur =  getc(m_fin); 
#endif
			}
		}
		m_val_string[m_val_string_len] = 0; 
		m_cur = ' ';
	} else  { 
		m_buf[0] = m_cur;
		m_index = 0; 
		do { 
			m_index++;
			m_buf[m_index] = getc(m_fin); 
		} while ((m_buf[m_index] == '_') || (m_buf[m_index] == '.') || isalnum(m_buf[m_index])); 
		
		m_cur = m_buf[m_index]; 
		m_buf[m_index] = 0 ; 
		
		
		if (IsNumber(m_buf))										
			NextNumber() ;											
		else
			NextString() ;											
		
		
	}
	
	if (GetType() == TOKEN_IMPORT) {
		if (m_importFile != NULL) {
			c3errors_FatalDialog("Token", "Nested import is not supported, nested import ignored.\n");
			return Next();
		} else {
			HandleImport();
			m_cur = getc(m_fin);
			return Next();
		}
	}

	if (GetType() == TOKEN_DO_NOT_IMPORT_DEFAULTS){
			g_load_defaults = false;
			return Next();
	}
	
	return GetType(); 
}

void Token::GetString (char *str) 
{   
	sint32 i; 

	if ((m_current_type != TOKEN_STRING) && (m_current_type != TOKEN_QUOTED_STRING)) { 
		c3errors_ErrorDialog (ErrStr(), "current type is not string");
		g_abort_parse = TRUE; 
		str[0] = 0; 
		return;
	} 
	for (i=0; i<m_val_string_len; i++) { 
		str[i] = m_val_string[i]; 
	} 
	str[i] = 0; 
} 

void Token::GetFloat(double &n)
{
	if (m_current_type != TOKEN_NUMBER)
	{
		c3errors_ErrorDialog (ErrStr(), "Token is not number"); 
		g_abort_parse = TRUE; 
	}
	
	n = m_val_number; 
}


void Token::GetNumber (sint32 &n)

{ 
	if (m_current_type != TOKEN_NUMBER) { 
		c3errors_ErrorDialog (ErrStr(), "Token is not number"); 
		g_abort_parse = TRUE; 
	} 
	n = (sint32)m_val_number; 
} 


char * Token::ErrStr()

{ 
	sprintf (m_estr, "%s line %d:", m_filename, g_parse_line); 
	return m_estr; 
} 

char * Token::GetKeyword(const sint32 tok) const

{  return g_allTokens[tok].keyword;
}



sint32 token_ParseKeywordNext(Token *aToken, const sint32 t) 

{
	if (aToken->Next() != t) { 
		return FALSE; 
	} else { 
		return TRUE; 
	}
}



sint32 token_ParseValNext(Token *aToken, const sint32 t, sint32 &val)

{
	if (!token_ParseKeywordNext(aToken, t)) { 
		
		if (TOKEN_MAX < t) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", aToken->m_imported_tokens[t - (TOKEN_MAX + 1)].keyword);
		} else { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", g_allTokens[t].keyword);
		}
		g_abort_parse = TRUE; 
		return FALSE; 
	} else { 
		if (aToken->Next() != TOKEN_NUMBER) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
			g_abort_parse = TRUE; 
			return FALSE;
		} else { 
			aToken->GetNumber(val); 
			return TRUE; 
		}
	}
}



sint32 token_ParseFloatNext(Token *aToken, const sint32 t, double &val)

{
	if (!token_ParseKeywordNext(aToken, t)) { 
		
		if (TOKEN_MAX < t) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", aToken->m_imported_tokens[t - (TOKEN_MAX + 1)].keyword);
		} else { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", g_allTokens[t].keyword);
		}
		g_abort_parse = TRUE; 
		return FALSE; 
	} else { 
		if (aToken->Next() != TOKEN_NUMBER) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
			g_abort_parse = TRUE; 
			return FALSE;
		} else { 
			aToken->GetFloat(val); 
			return TRUE; 
		}
	}
}



sint32 token_ParseAnOpenBraceNext(Token *aToken)

{
	if (aToken->Next() == TOKEN_OPEN_BRACE) { 
		return TRUE; 
	} else { 
		c3errors_ErrorDialog(aToken->ErrStr(), "Expected open brace missing");
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
}



sint32 token_ParseAnCloseBraceNext(Token *aToken)

{
	if (aToken->Next() == TOKEN_CLOSE_BRACE) { 
		return TRUE; 
	} else { 
		c3errors_ErrorDialog(aToken->ErrStr(), "Expected close brace missing");
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
}



sint32 token_ParseAnCloseBrace(Token *aToken)

{
	if (aToken->GetType() == TOKEN_CLOSE_BRACE) { 
		return TRUE; 
	} else { 
		c3errors_ErrorDialog(aToken->ErrStr(), "Expected close brace missing");
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
}




POINT token_ParsePoint(Token *theToken)
{
	POINT		p = {-1, -1};
	sint32		tmp;
	
	if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
	else return p;
	
	p.x = tmp;
	
	if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
	else return p;
	
	p.y = tmp;
	
	return p;
}
