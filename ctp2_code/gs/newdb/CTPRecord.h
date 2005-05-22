#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CTP_RECORD_H__
#define __CTP_RECORD_H__

#include "CTPDatabase.h"

class DBLexer;

class CTPRecord
{
protected:
	sint32 m_index;
	char *m_textName; 

public:
	sint32 m_name;

	CTPRecord() { m_index = -1; m_textName = NULL; m_name = -1;}
	virtual ~CTPRecord() { if(m_textName) delete [] m_textName; }

	sint32 GetIndex() const { return m_index; }
	void SetIndex(sint32 index) { m_index = index; }

	sint32 GetName() const { return m_name; }
	const char *GetIDText() const;
	virtual const char *GetNameText() const;
	void SetTextName(const char *text);

	bool ParseIntInArray(DBLexer *lex, sint32 **array, sint32 *numElements);
	bool ParseFloatInArray(DBLexer *lex, double **array, sint32 *numElements);
	bool ParseFileInArray(DBLexer *lex, char ***array, sint32 *numElements);
	bool ParseStringIdInArray(DBLexer *lex, sint32 **array, sint32 *numElements);

	bool ParseIntInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize);
	bool ParseFloatInArray(DBLexer *lex, double *array, sint32 *numElements, sint32 maxSize);
	bool ParseFileInArray(DBLexer *lex, char **array, sint32 *numElements, sint32 maxSize);
	bool ParseStringIdInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize);
};

#endif
