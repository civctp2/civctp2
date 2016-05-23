//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database Record template class
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
// - Arrays can now be parsed sequentially, if their values are in sequence
//   without being separated of any other tokens. (Sep 3rd 2005 Martin Gühmann)
// - Repaired memory leaks
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CTPRecord.h"

#include "CTPDatabase.h"
#include "DBLexer.h"
#include "DBTokens.h"
#include "StrDB.h"

bool CTPRecord::ParseIntInArray(DBLexer *lex, sint32 **array, sint32 *numElements)
{
	if(lex->PeekAhead() != k_Token_Int) {
		DBERROR(("Expected integer"));
		return false;
	}

	do{
		lex->GetToken();
		sint32 value = atoi(lex->GetTokenText());
		if(*numElements > 0) {
			sint32 *oldArray = *array;
			*array = new sint32[*numElements + 1];
			memcpy(*array, oldArray, (*numElements) * sizeof(sint32));
			delete [] oldArray;
		} else {
            delete [] *array;
			*array = new sint32[1];
		}
		(*array)[*numElements] = value;
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_Int);
	return true;
}

bool CTPRecord::ParseFloatInArray(DBLexer *lex, double **array, sint32 *numElements)
{
	if(lex->PeekAhead() != k_Token_Int && lex->PeekAhead() != k_Token_Float) {
		DBERROR(("Expected number"));
		return false;
	}

	do{
		lex->GetToken();
		double value = atof(lex->GetTokenText());
		if(*numElements > 0) {
			double *oldArray = *array;
			*array = new double[*numElements + 1];
			memcpy(*array, oldArray, (*numElements) * sizeof(double));
			delete [] oldArray;
		} else {
            delete [] *array;
			*array = new double[1];
		}
		(*array)[*numElements] = value;
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_Int || lex->PeekAhead()== k_Token_Float);
	return true;
}

bool CTPRecord::ParseFileInArray(DBLexer *lex, char ***array, sint32 *numElements)
{
	if(lex->PeekAhead() != k_Token_String) {
		DBERROR(("Expected filename"));
		return false;
	}

	do{
		lex->GetToken();
		const char * value = lex->GetTokenText();

		if(*numElements > 0) {
			char **oldArray = *array;
			*array = new char *[*numElements + 1];
			memcpy(*array, oldArray, (*numElements) * sizeof(char *));
			delete [] oldArray;
		} else {
            delete [] *array;
			*array = new char *[1];
		}
		(*array)[*numElements] = new char[strlen(value) + 1];
		strcpy((*array)[*numElements], value);
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_String);
	return true;
}

bool CTPRecord::ParseStringIdInArray(DBLexer *lex, sint32 **array, sint32 *numElements)
{
	if(lex->PeekAhead() != k_Token_Name) {
		DBERROR(("Expected stringid"));
		return false;
	}

	do{
		lex->GetToken();
		const char * value = lex->GetTokenText();

		if(*numElements > 0) {
			sint32 *oldArray = *array;
			*array = new sint32[(*numElements) + 1];
			memcpy(*array, oldArray, (*numElements) * sizeof(sint32));
			delete [] oldArray;
		} else {
            delete [] *array;
			*array = new sint32[1];
		}
		sint32 id;
		if(!g_theStringDB->GetStringID(value, id)) {
			DBERROR(("%s not in string database", value));
			return false;
		}
		(*array)[*numElements] = id;
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_Name);
	return true;
}

bool CTPRecord::ParseIntInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize)
{
	if(lex->PeekAhead() != k_Token_Int) {
		DBERROR(("Expected integer"));
		return false;
	}

	do{
		lex->GetToken();
		sint32 value = atoi(lex->GetTokenText());
		if(*numElements >= maxSize) {
			DBERROR(("Too many entries"));
			return false;
		}

		array[*numElements] = value;
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_Int);
	return true;
}

bool CTPRecord::ParseFloatInArray(DBLexer *lex, double *array, sint32 *numElements, sint32 maxSize)
{
	if(lex->PeekAhead() != k_Token_Int && lex->PeekAhead() != k_Token_Float) {
		DBERROR(("Expected number"));
		return false;
	}

	do{
		lex->GetToken();
		double value = atof(lex->GetTokenText());
		if(*numElements >= maxSize) {
			DBERROR(("Too many entries"));
			return false;
		}

		array[*numElements] = value;
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_Int || lex->PeekAhead() == k_Token_Float);
	return true;
}

bool CTPRecord::ParseFileInArray(DBLexer *lex, char **array, sint32 *numElements, sint32 maxSize)
{
	if(lex->PeekAhead() != k_Token_String) {
		DBERROR(("Expected quoted string"));
		return false;
	}

	do{
		lex->GetToken();
		const char * value = lex->GetTokenText();

		if(*numElements >= maxSize) {
			DBERROR(("Too many entries"));
			return false;
		}
		array[*numElements] = new char[strlen(value) + 1];
		strcpy(array[*numElements], value);
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_String);
	return true;
}

bool CTPRecord::ParseStringIdInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize)
{
	if(lex->PeekAhead() != k_Token_Name) {
		DBERROR(("Expected string id"));
		return false;
	}

	do{
		sint32 tok = lex->GetToken();
		const char * value = lex->GetTokenText();

		if(*numElements >= maxSize) {
			DBERROR(("Too many entries"));
			return false;
		}

		sint32 id;
		if(!g_theStringDB->GetStringID(value, id)) {
			DBERROR(("%s not in string database. Token: %i, TokenName: %i, Next: %i", value, tok, k_Token_Name, lex->PeekAhead()));
			return false;
		}
		array[*numElements] = id;
		*numElements += 1;
	}while(lex->PeekAhead() == k_Token_Name);
	return true;
}

void CTPRecord::SetTextName(const char *text)
{
	Assert(text);
	if (text)
    {
	    m_name      = INDEX_INVALID;
        delete [] m_textName;
	    m_textName  = new char[strlen(text) + 1];
	    strcpy(m_textName, text);
    }
}

const char *CTPRecord::GetIDText() const
{
	Assert(m_name >= 0);
    return (m_name >= 0) ? g_theStringDB->GetIdStr(m_name) : "NO_ID";
}

const char *CTPRecord::GetNameText() const
{
    return (m_textName) ? m_textName : g_theStringDB->GetNameStr(m_name);
}
