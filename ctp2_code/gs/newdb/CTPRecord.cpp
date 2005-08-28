//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database Record template class
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CTPDatabase.h"
#include "CTPRecord.h"
#include "DBLexer.h"
#include "DBTokens.h"
#include "StrDB.h"

bool CTPRecord::ParseIntInArray(DBLexer *lex, sint32 **array, sint32 *numElements)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Int) {
		DBERROR(("Expected integer"));
		return false;
	}

	sint32 value = atoi(lex->GetTokenText());
	if(*numElements > 0) {
		sint32 *oldArray = *array;
		*array = new sint32[*numElements + 1];
		memcpy(*array, oldArray, (*numElements) * sizeof(sint32));
		delete oldArray;
	} else {
		*array = new sint32[1];
	}		
	(*array)[*numElements] = value;
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseFloatInArray(DBLexer *lex, double **array, sint32 *numElements)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Int && tok != k_Token_Float) {
		DBERROR(("Expected number"));
		return false;
	}

	double value = atof(lex->GetTokenText());
	if(*numElements > 0) {
		double *oldArray = *array;
		*array = new double[*numElements + 1];
		memcpy(*array, oldArray, (*numElements) * sizeof(double));
		delete oldArray;
	} else {
		*array = new double[1];
	}		
	(*array)[*numElements] = value;
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseFileInArray(DBLexer *lex, char ***array, sint32 *numElements)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_String) {
		DBERROR(("Expected filename"));
		return false;
	}
	
	const char * value = lex->GetTokenText();

	if(*numElements > 0) {
		char **oldArray = *array;
		*array = new char *[*numElements + 1];
		memcpy(*array, oldArray, (*numElements) * sizeof(char *));
		delete oldArray;
	} else {
		*array = new char *[1];
	}		
	(*array)[*numElements] = new char[strlen(value) + 1];
	strcpy((*array)[*numElements], value);
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseStringIdInArray(DBLexer *lex, sint32 **array, sint32 *numElements)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Name) {
		DBERROR(("Expected stringid"));
		return false;
	}
	
	const char * value = lex->GetTokenText();

	if(*numElements > 0) {
		sint32 *oldArray = *array;
		*array = new sint32[(*numElements) + 1];
		memcpy(*array, oldArray, (*numElements) * sizeof(sint32));
		delete oldArray;
	} else {
		*array = new sint32[1];
	}		
	sint32 id;
	if(!g_theStringDB->GetStringID(value, id)) {
		DBERROR(("%s not in string database", value));
		return false;
	}
	(*array)[*numElements] = id;
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseIntInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Int) {
		DBERROR(("Expected integer"));
		return false;
	}

	sint32 value = atoi(lex->GetTokenText());
	if(*numElements >= maxSize) {
		DBERROR(("Too many entries"));
		return false;
	}

	array[*numElements] = value;
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseFloatInArray(DBLexer *lex, double *array, sint32 *numElements, sint32 maxSize)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Int && tok != k_Token_Float) {
		DBERROR(("Expected number"));
		return false;
	}

	double value = atof(lex->GetTokenText());
	if(*numElements >= maxSize) {
		DBERROR(("Too many entries"));
		return false;
	}

	array[*numElements] = value;
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseFileInArray(DBLexer *lex, char **array, sint32 *numElements, sint32 maxSize)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_String) {
		DBERROR(("Expected quoted string"));
		return false;
	}
	
	const char * value = lex->GetTokenText();

	if(*numElements >= maxSize) {
		DBERROR(("Too many entries"));
		return false;
	}
	array[*numElements] = new char[strlen(value) + 1];
	strcpy(array[*numElements], value);
	*numElements += 1;
	return true;
}

bool CTPRecord::ParseStringIdInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize)
{
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Name) {
		DBERROR(("Expected string id"));
		return false;
	}
	
	const char * value = lex->GetTokenText();

	if(*numElements >= maxSize) {
		DBERROR(("Too many entries"));
		return false;
	}

	sint32 id;
	if(!g_theStringDB->GetStringID(value, id)) {
		DBERROR(("%s not in string database", value));
		return false;
	}
	array[*numElements] = id;
	*numElements += 1;
	return true;
}

void CTPRecord::SetTextName(const char *text)
{
	Assert(text);
	if(!text)
		return;

	m_name = -1;
	m_textName = new char[strlen(text) + 1];
	strcpy(m_textName, text);
}

const char *CTPRecord::GetIDText() const
{
	Assert(m_name >= 0);
	if(m_name >= 0)
		return g_theStringDB->GetIdStr(m_name);

	return "NO_ID";
}

const char *CTPRecord::GetNameText() const
{
	if(m_textName) {
		return m_textName;
	} else {
		Assert(m_name >= 0);
		return g_theStringDB->GetNameStr(m_name);
	}
}
