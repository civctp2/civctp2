











#pragma once
#ifndef __STRING_DB_H__
#define __STRING_DB_H__ 1

class StringRecord;
class Token;

#include "dbtypes.h"


#define STRDB_NUM_HEADS 547

typedef class StringDB { 

    
	
	
	sint32 m_nStr; 
	
	

	
	
	

	StringRecord **m_all; 
	StringRecord **m_head;
	
	

    StringRecord **GetHead(const char *id) const;

public:

	StringDB(); 
    ~StringDB(); 

	sint32 InsertStr(const char *add_id, const char *new_text);   
	sint32 AddStrNode(StringRecord **ptr, const char *add_id, 
					  const char *new_text, StringRecord *&newPtr);
   
	char * GetIdStr(const StringId index) const;
	sint32 GetStringID(char str_id[], StringId &index) const;
	sint32 GetStringID(const char *str_id, StringId &index) const;

	sint32 GetText(const char *get_id, char **new_text); 
	sint32 GetStrNode(StringRecord *ptr, const char *add_id, 
		char **new_text);
   
	void Btree2Array(); 
	void AssignIndex(StringRecord *ptr, StringId &count);
   
	sint32 GetIndexNode(StringRecord *ptr, 
						const char *str_id,
						StringId &index) const;

	const MBCHAR *GetNameStr (StringId n) const;
	const MBCHAR *GetNameStr(const char *s) ;

	sint32 ParseAStringEntry(Token *strToken);
	sint32 Parse(MBCHAR *filename);
} StringDB; 

extern StringDB *g_theStringDB;

#endif
