//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database lexer (tokenizer/scanner)
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leaks.
// - Prevented files staying open.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "DBLexer.h"
#include "StringHash.h"
#include "PointerList.h"
#include "DBTokens.h"
#include "StrDB.h"
#include "c3files.h"

#include <ctype.h>

extern "C" FILE *dblin;
extern "C" char *dbltext;
extern "C" int dbllex();
extern "C" int g_dblexerLineNumber;
extern "C" void dblrestart(FILE *file);

namespace
{
	size_t const	SIZE_HASH_TABLE	= 64;
	sint32 const	TOKEN_UNDEFINED	= 0;
}

DBToken::DBToken(const char *name, sint32 value)
{
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);
	m_value = value;
}

DBToken::~DBToken()
{
	delete [] m_name;
}




DBLexer::DBLexer(const C3DIR & c3dir, const char *file)
{
	
	m_tokenHash = new StringHash<DBToken>(SIZE_HASH_TABLE);

	strcpy(m_filename, file);
	m_file = c3files_fopen(c3dir, m_filename, "r");
	Assert(m_file);

	if(m_file) {
		dblin = m_file;
		dblrestart(m_file);
	}

	m_atEnd = false;
	m_customTokenStack = new PointerList<DBCustomTokens>;
	sint32 i;
	for(i = 0; i < k_TOKEN_HISTORY_SIZE; i++) {
		m_tokenText[i][0] = 0;
		m_tokenLine[i] = 1;
	}

	g_dblexerLineNumber = 1;
	m_nextToken = TOKEN_UNDEFINED;
	m_whichTokenText = 0;

	
	
	GetToken();
}

DBLexer::~DBLexer()
{
	delete m_tokenHash;

	if(m_customTokenStack) {
		m_customTokenStack->DeleteAll();
		delete m_customTokenStack;
	}

	if (m_file)
	{
		fclose(m_file);
	}	
}

void DBLexer::SetTokens(char **tokens, sint32 maxToken)
{
	
	Assert(maxToken > k_Token_Custom_Base);
	if(maxToken < k_Token_Custom_Base)
		return;

	
	
	DBCustomTokens *cust = new DBCustomTokens(tokens, maxToken - k_Token_Custom_Base);
	m_customTokenStack->AddTail(cust);

	
	
	delete m_tokenHash;
	m_tokenHash = new StringHash<DBToken>(SIZE_HASH_TABLE);


	
	sint32 i;
	for(i = 0; i < cust->m_numTokens; i++) {
		m_tokenHash->Add(new DBToken(cust->m_tokens[i], k_Token_Custom_Base + i));
	}
}

void DBLexer::RestoreTokens()
{
	
	
	delete m_tokenHash;
	m_tokenHash = new StringHash<DBToken>(SIZE_HASH_TABLE);

	DBCustomTokens *old = m_customTokenStack->RemoveTail();
	Assert(old);
	delete old;
	
	if(m_customTokenStack->GetTail()) {
		DBCustomTokens *cust = m_customTokenStack->GetTail();

		sint32 i;
		for(i = 0; i < cust->m_numTokens; i++) {
			m_tokenHash->Add(new DBToken(cust->m_tokens[i], k_Token_Custom_Base + i));
		}
	}
}

sint32 DBLexer::GetToken()
{
	if (m_atEnd)
	{
		return TOKEN_UNDEFINED;
	}
	
	

	
	m_whichTokenText++;
	m_whichTokenText %= k_TOKEN_HISTORY_SIZE;
	
	sint32 tok = m_nextToken;	

	
	m_nextToken = dbllex();
	sint32 nextTokenText = (m_whichTokenText + 1) % k_TOKEN_HISTORY_SIZE;
	m_tokenLine[nextTokenText] = g_dblexerLineNumber;

	if(m_nextToken != k_Token_String) {
		strcpy(m_tokenText[nextTokenText], dbltext);
	} else {
		
		strcpy(m_tokenText[nextTokenText], dbltext + 1);

		
		m_tokenText[nextTokenText][strlen(m_tokenText[nextTokenText]) - 1] = 0;
	}

	m_atEnd = (TOKEN_UNDEFINED == m_nextToken);

	DBToken *dbtok;

	if(tok == k_Token_Name) {
		if(dbtok = m_tokenHash->Access(m_tokenText[m_whichTokenText])) {
			return dbtok->GetValue();
		} else {
			return k_Token_Name;
		}
	} else {
		return tok;
	}
}

sint32 DBLexer::PeekAhead()
{
	
	return m_nextToken;
}

const char *DBLexer::GetTokenText()
{
	return m_tokenText[m_whichTokenText];
}

bool DBLexer::GetIntAssignment(sint32 &value)
{
	sint32 tok = GetToken();
	if(tok != k_Token_Int) {
		return false;
	}

	value = atoi(GetTokenText());
	return true;
}

bool DBLexer::GetFloatAssignment(double &value)
{
	sint32 tok = GetToken();
	if(tok == k_Token_Float || tok == k_Token_Int) {
		value = atof(GetTokenText());
		return true;
	}
	return false;
}

bool DBLexer::GetStringIdAssignment(sint32 &strId)
{
	sint32 tok = GetToken();
	if(tok != k_Token_Name)
		return false;

	return g_theStringDB->GetStringID(GetTokenText(), strId) != 0;
}

bool DBLexer::GetFileAssignment(char *&filename)
{
	sint32 tok = GetToken();
	if(tok != k_Token_String)
		return false;

	delete [] filename;
	filename = new char[strlen(m_tokenText[m_whichTokenText]) + 1];
	strcpy(filename, m_tokenText[m_whichTokenText]);
	return true;
}

bool DBLexer::GetBitIndex(const char **bitnames, sint32 numBitnames, sint32 &index)
{
	sint32 tok;
	tok = GetToken();
	if(tok != ':')
		return false;

	tok = GetToken();
	if(tok != k_Token_Name)
		return false;

	for(index = 0; index < numBitnames; index++) {
		if(!(stricmp(bitnames[index], m_tokenText[m_whichTokenText]))) {
			return true;
		}
	}
	return false;
}

void DBLexer::ReportError(char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);

	char buf[1024];
	sprintf(buf, "%s:%d: ", GetFilename(), GetLineNumber());

	vsprintf(buf + strlen(buf), fmt, list);

	va_end(list);

	c3errors_ErrorDialog("Database", buf);
}
	
sint32 DBLexer::GetLineNumber()
{
	return m_tokenLine[m_whichTokenText];
}

const char *DBLexer::GetFilename()
{
	return m_filename;
}

