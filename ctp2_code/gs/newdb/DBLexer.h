//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database lexer (Lexical Analyzer Generator) header
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
// - Implemented GovernmentsModified subclass (allowing cdb files including
//   a GovernmentsModified record to produce parsers capable of reading and
//   storing subrecords for Government types.)
//   See http://apolyton.net/forums/showthread.php?s=&threadid=107916 for
//   more details  _____ by MrBaggins Jan-04
//
//   * Added PeekAheadText member function prototype
// - Parser for struct ADVANCE_CHANCES of DiffDB.txt can now be generated. (Jan 3rd 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef __DBLEXER_H__
#define __DBLEXER_H__

template <class T> class StringHash;
template <class T> class PointerList;

#define k_MAX_DBL_TOKEN 1024
#define k_TOKEN_HISTORY_SIZE 2

#include "c3files.h"

class DBToken
{
private:
	char *m_name;
	sint32 m_value;

public:
	DBToken(const char *name, sint32 value);
	~DBToken();

	const char *GetName() { return m_name; }
	sint32 GetValue() { return m_value; }
};

class DBCustomTokens
{
public:
	char **m_tokens;
	sint32 m_numTokens;

	DBCustomTokens(char **tokens, sint32 numTokens) {
		m_tokens = tokens;
		m_numTokens = numTokens;
	}
};

class DBLexer
{
	FILE *m_file;
	StringHash<DBToken> *m_tokenHash;
	char m_tokenText[k_TOKEN_HISTORY_SIZE][k_MAX_DBL_TOKEN];
	sint32 m_tokenLine[k_TOKEN_HISTORY_SIZE];
	sint32 m_whichTokenText;
	sint32 m_nextToken;
	bool m_atEnd;
	PointerList<DBCustomTokens> *m_customTokenStack;
	char m_filename[_MAX_PATH];
	sint32 m_currentToken;

public:
	DBLexer(const C3DIR & c3dir, const char *file);
	~DBLexer();

	bool EndOfInput() { return m_atEnd; }
	bool GetIntAssignment(sint32 &value);
	bool GetFloatAssignment(double &value);
	bool GetBitIndex(const char **bitnames, sint32 numNames, sint32 &index);
	bool GetStringIdAssignment(sint32 &strId);
	bool GetFileAssignment(char *&filename);

	bool GetInt(sint32 &value);
	bool GetFloat(double &value);
	bool GetStringId(sint32 &strId);
	bool GetFile(char *&filename);

	void SetTokens(char **tokens, sint32 maxToken);
	void RestoreTokens();

	sint32 GetCurrentToken();
	sint32 GetToken();
	sint32 PeekAhead();

//	const char *PeekAheadText(); //GovMod

	const char *GetTokenText();

	void ReportError(char *fmt, ...);
	sint32 GetLineNumber();
	const char *GetFilename();
};

#endif
