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
// - Added new token so that the loading of default strings in scenarios
//   can be skipped. (9-Apr-2007 Martin Gühmann)
// - Old default string skip token does not prevent loading default strings
//   in scenarios anymore. (9-Apr-2007 Martin Gühmann)
// - Scenario path can now be ignored. (9-Apr-2007 Martin Gühmann)
// - Improved handling of "unexpected" EOF, caused by incorrect input.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Token.h"

#include "c3errors.h"
#include "c3files.h"
#include "ErrMsg.h"
#include "Globals.h"
#include "StrDB.h"              // g_theStringDB
#include "CivPaths.h"           // g_CivPaths
#ifndef __MAKESPR__
#include "AdvanceRecord.h"
#include "BuildingRecord.h"
#endif

#if defined(_JAPANESE)
#include "japanese.h"
#endif

extern sint32 g_abort_parse;

sint32      g_parse_line;
sint32      g_saved_parse_line;
bool        g_load_defaults;

TokenData   g_allTokens [] =
{
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

	// Color database
	{TOKEN_COLORSET,				"COLORSET"},
	{TOKEN_COLORSET_COLOR,			"COLORSET_COLOR"},

	// For string database mod compatibility
	{TOKEN_DO_NOT_IMPORT_DEFAULTS,			"DoNotImportDefaults"},
	{TOKEN_SCENARIO_DO_NOT_IMPORT_DEFAULTS,	"ScenarioDoNotImportDefaults"},
	{TOKEN_MAX,								"TOKEN_MAX_DO_NOT_USE"},

};

char const * token_GetKeyword(sint32 i)
{
	Assert (0 <= i);
	return g_allTokens[i].keyword;
}

bool Token::ValidateAllTokens()
{
    for (int i = 0; i <TOKEN_MAX; ++i)
    {
	  if (g_allTokens[i].tok != i)
        {
            return false;
        }
    }

    return true;
}

Token::Token
(
	 char const *   fn,
	 C3DIR	        dir,
	 sint32         a_ImportCount,
	 TokenData *    it
)
:
    m_fin               (c3files_fopen(dir, fn, "r")),
    m_len               (0),
    m_index             (0),
    m_val_string_len    (0),
    m_current_type      (TOKEN_UNKNOWN),
    m_val_number        (0),
    m_cur               (' '),
    m_num_it            (a_ImportCount),
    m_imported_tokens   (it),
    m_dir               (dir),
    m_importFile		(NULL),
    m_savedLineNumber   (0),
    m_savedFin          (NULL),
	m_checkScenario     (true)
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

/// @todo Remove when no longer referenced
/// This is the same as above: only the parameter order differs.
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
    m_current_type      (TOKEN_UNKNOWN),
    m_val_number        (0),
    m_cur               (' '),
    m_num_it            (n),
    m_imported_tokens   (it),
    m_dir               (dir),
    m_importFile		(NULL),
    m_savedLineNumber   (0),
    m_savedFin          (NULL),
	m_checkScenario     (true)
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

// [<sign>] <one or more digits> [<decimal separator><one or more digits>]
// OK      : "0", "-4", "+2.00"
// not OK  : "five", "1E2", "+", "0.", ".3", "1 2"
bool Token::IsNumber(char const * str)
{
	if (*str == '-' || *str == '+')
		str++;

	if (!isdigit(*str++))
		return false;

	while (isdigit(*str))
		str++;

	if (0 == *str)
		return true;

	if (*str++ != '.')
		return false;

	if (!isdigit(*str++))
		return false;

	while (isdigit(*str))
		str++;

	return (0 == *str);
}

sint32 Token::GetType() const
{
	return m_current_type;
}

void Token::NextNumber()
{
	m_current_type = TOKEN_NUMBER;
	m_val_number = atof(m_buf);
}

bool Token::IsWhitespace(int c)
{
    return (c == ' ') || (c == '\t') || (c == 13) || (c == 10);
}

void Token::NextString()
{
    bool searching = true;

	for (sint32 i=TOKEN_UNKNOWN+1; i<TOKEN_MAX && searching; i++) {
		if (strcmp(m_buf, g_allTokens[i].keyword) == 0) {
			m_current_type = i;
			searching = false;
		}
	}

	if ((searching) && (m_num_it)) {
		for (sint32 i = 0; i<m_num_it && searching ; i++) {
			if (strcmp(m_buf, m_imported_tokens[i].keyword) == 0) {
				m_current_type = m_imported_tokens[i].tok;
				searching = false;
			}
		}
	}

	if (searching) {
		m_current_type = TOKEN_STRING;
		strcpy (m_val_string, m_buf);
		m_val_string_len = strlen(m_val_string);
	}
}

bool Token::HandleImport(void)
{
	if (Next() != TOKEN_QUOTED_STRING)
    {
		Assert(FALSE);
		return false;
	}

	MBCHAR		fileName[_MAX_PATH];
	GetString(fileName);

	g_saved_parse_line = g_parse_line;
	g_parse_line = 0;

	strcpy(m_savedFilename, m_filename);
	strcpy(m_filename, fileName);

	m_savedFin = m_fin;
	m_importFile = c3files_fopen(m_dir, fileName, "r", m_checkScenario);

	if (!m_importFile)
    {
		c3errors_ErrorDialog ("Token.cpp", "Could not open import file '%s'", fileName);
		g_abort_parse = TRUE;
        return false;
	}

    m_fin = m_importFile;
	return true;
}

void Token::CloseImport(void)
{
	Assert(m_importFile);
	fclose(m_importFile);
	m_fin = m_savedFin;
	m_savedFin   = NULL;	// for safe destruction
	m_importFile = NULL;
	strcpy(m_filename, m_savedFilename);
	g_parse_line = g_saved_parse_line;
	g_saved_parse_line = 0;
}

sint32 Token::Next()

{
	while (1) {

		for ( ; IsWhitespace(m_cur) ; m_cur = getc(m_fin))
		{
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
				case EOF:
					m_current_type = TOKEN_MISSING_QUOTE;
					m_val_string[m_val_string_len] = 0;
					return m_current_type;
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
					m_val_string[m_val_string_len] = static_cast<char>(m_cur);
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

				m_val_string[m_val_string_len] = static_cast<char>(m_cur);
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
		m_buf[0] = static_cast<char>(m_cur);
		m_index = 0;
		do {
			m_cur = getc(m_fin);
			if (EOF == m_cur)
			{
				break;
			}
			else
			{
				m_index++;
				m_buf[m_index] = static_cast<char>(m_cur);
			}
		} while ((m_buf[m_index] == '_') || (m_buf[m_index] == '.') || isalnum(m_buf[m_index]));

		m_cur = m_buf[m_index];
		m_buf[m_index] = 0 ;

		if (IsNumber(m_buf))
			NextNumber() ;
		else
			NextString() ;

	}

	if (GetType() == TOKEN_IMPORT)
    {
		if (m_importFile)
        {
			c3errors_FatalDialog("Token", "Nested import is not supported, nested import ignored.\n");
		}
        else if (HandleImport())
        {
			m_cur = getc(m_fin);
		}

		return Next();
	}

	if(GetType() == TOKEN_DO_NOT_IMPORT_DEFAULTS
	|| GetType() == TOKEN_SCENARIO_DO_NOT_IMPORT_DEFAULTS
	){
		if(g_civPaths->GetCurScenarioPath()     == NULL        // Load only defaults if this isn't a scenario
		&& g_civPaths->GetCurScenarioPackPath() == NULL
		|| GetType() == TOKEN_SCENARIO_DO_NOT_IMPORT_DEFAULTS  // But not if you have this token
		){
			g_load_defaults = false;
		}

		return Next();
	}

	return GetType();
}

void Token::GetString(char * str)
{
    if ((m_current_type == TOKEN_STRING) || (m_current_type == TOKEN_QUOTED_STRING))
    {
        std::copy(m_val_string, m_val_string + m_val_string_len, str);
        str[m_val_string_len] = 0;
    }
    else
    {
        c3errors_ErrorDialog (ErrStr(), "current type is not string");
        g_abort_parse = TRUE;
        str[0] = 0;
    }
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

void Token::GetNumber(sint32 &n)
{
	if (m_current_type != TOKEN_NUMBER) {
		c3errors_ErrorDialog (ErrStr(), "Token is not number");
		g_abort_parse = TRUE;
	}

	n = (sint32)m_val_number;
}

char const * Token::ErrStr()
{
	sprintf(m_estr, "%s line %d:", m_filename, g_parse_line);
	return m_estr;
}

char const * Token::GetKeyword(sint32 tok)
{
    return g_allTokens[tok].keyword;
}


bool token_ParseKeywordNext(Token *aToken, sint32 t)
{
    return aToken->Next() == t;
}


bool token_ParseValNext(Token *aToken, sint32 t, sint32 &val)
{
    if (token_ParseKeywordNext(aToken, t))
    {
		if (aToken->Next() == TOKEN_NUMBER) {
			aToken->GetNumber(val);
			return true;
		} else {
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
		}
    }
    else
    {
        TokenData & data = (TOKEN_MAX < t)
                           ? aToken->m_imported_tokens[t - (TOKEN_MAX + 1)]
                           : g_allTokens[t];

	    c3errors_ErrorDialog
            (aToken->ErrStr(), "Expected keyword %s not found", data.keyword);
    }

    g_abort_parse = TRUE;
    return false;
}


bool token_ParseFloatNext(Token *aToken, sint32 t, double &val)
{
    if (token_ParseKeywordNext(aToken, t))
    {
		if (aToken->Next() == TOKEN_NUMBER)
        {
			aToken->GetFloat(val);
			return true;
		}
        else
        {
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
		}
    }
    else
    {
        TokenData & data = (TOKEN_MAX < t)
                           ? aToken->m_imported_tokens[t - (TOKEN_MAX + 1)]
                           : g_allTokens[t];

	    c3errors_ErrorDialog
            (aToken->ErrStr(), "Expected keyword %s not found", data.keyword);
	}

    g_abort_parse = TRUE;
    return false;
}


bool token_ParseAnOpenBraceNext(Token *aToken)
{
    if (aToken->Next() == TOKEN_OPEN_BRACE)
    {
		return true;
    }

    c3errors_ErrorDialog(aToken->ErrStr(), "Expected open brace missing");
    g_abort_parse = TRUE;
    return false;
}


bool token_ParseAnCloseBraceNext(Token *aToken)
{
    if (aToken->Next() == TOKEN_CLOSE_BRACE)
    {
		return true;
    }

    c3errors_ErrorDialog(aToken->ErrStr(), "Expected close brace missing");
    g_abort_parse = TRUE;
    return false;
}


bool token_ParseAnCloseBrace(Token *aToken)
{
    if (aToken->GetType() == TOKEN_CLOSE_BRACE)
    {
		return true;
    }

    c3errors_ErrorDialog(aToken->ErrStr(), "Expected close brace missing");
    g_abort_parse = TRUE;
    return false;
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
