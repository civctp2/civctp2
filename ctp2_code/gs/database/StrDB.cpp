//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : String database
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Crash prevention.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3files.h"

#include "StrRec.h"
#include "StrDB.h"

#include "Token.h"

extern sint32	g_abort_parse;

#if !defined(ACTIVISION_ORIGINAL)

namespace
{
	
	StringId const	INDEX_INVALID	= -1;

} // namespace

#endif



StringDB::StringDB()

{
m_all = NULL; 
m_head = (StringRecord **)calloc(STRDB_NUM_HEADS, sizeof(StringRecord *));
m_nStr=0;
}


StringDB::~StringDB()

{
    int i; 

    for (i=0; i<m_nStr; i++) { 
        delete m_all[i]; 
    }

    delete m_all; 
    free(m_head);
}
StringRecord **StringDB::GetHead(const char *id) const
{
    unsigned short hash;

    __asm {
          push eax              ; save registers
          push ecx
          push edx
          xor  edx, edx         ; zero out working registers
          xor  eax, eax
          mov  ecx, id          ; get pointer to start of string
      hashLoop:
          mov  al, [ecx]        ; get character from string
          add  dx, ax           ; add it to the running hash
          rol  dx, 1            ; rotate the hash by one place
          inc  ecx              ; point to next character in string
          cmp  al, 0            ; check for end of string
          jnz  hashLoop
          mov  hash, dx         ; save the result
          pop  edx              ; restore registers
          pop  ecx
          pop  eax
    }          

    hash %= STRDB_NUM_HEADS;
    return(&(m_head[hash]));
}








sint32 StringDB::InsertStr(const char *add_id, const char *new_text)

{
	sint32 r;
	StringRecord *newRec;

	r = AddStrNode(GetHead(add_id), add_id, new_text, newRec); 
	
	if (r) { 
		if(m_all) {
			
			
			StringRecord **newArray = new StringRecord*[m_nStr + 1];
			memcpy(newArray, m_all, (m_nStr) * sizeof(StringRecord *));
			delete [] m_all;
			m_all = newArray;

			sint32 count = m_nStr;
			m_nStr++;
			
			AssignIndex(newRec, count);
		} else {
			m_nStr++;
		}
	} else {
        
        
        
        

        char *countstr;
        int count;
		char tempstr[256];
        
        
        sprintf(tempstr, "%s#COUNT", add_id);
        if (GetStrNode(*GetHead(tempstr), tempstr, &countstr) == FALSE) {
            AddStrNode(GetHead(tempstr), tempstr, "1     ", newRec); m_nStr++;
            GetStrNode(*GetHead(tempstr), tempstr, &countstr);
        }
        count = atoi(countstr);
        
        
        sprintf(tempstr, "%s#%d", add_id, count);
        AddStrNode(GetHead(tempstr), tempstr, new_text, newRec); m_nStr++; 
        
        
        sprintf(countstr, "%d", ++count);

        r = TRUE;
    }

	return r;
}








sint32 StringDB::AddStrNode(
   StringRecord **ptr, 
   const char *add_id, 
   const char *new_text,  
   StringRecord *&newPtr) 

{ 
	sint32 r, l;
	
	if (*ptr == NULL) { 
		*ptr = new StringRecord(); 
		Assert(*ptr); 
		
		(*ptr)->m_id = new char[strlen(add_id) + 1];

		strcpy((*ptr)->m_id, add_id); 
		
		l = strlen(new_text);
		(*ptr)->m_text = new char[l+1];	
		
		Assert((*ptr)->m_text);
		strcpy((*ptr)->m_text, new_text); 
		newPtr = *ptr;
		return TRUE; 
		
	} else { 
		
		r = strcmp(add_id, (*ptr)->m_id);
		
		if (r < 0) { 
			return AddStrNode(&((*ptr)->m_lesser), add_id, new_text, newPtr); 
		} else if (0 < r) { 
			return AddStrNode(&((*ptr)->m_greater), add_id, new_text, newPtr); 
		} else { 
			return FALSE;  
		}   
	}
}








sint32 StringDB::GetText(const char *get_id, char **new_text)
{
return GetStrNode (*GetHead(get_id), get_id, new_text); 
}









char * StringDB::GetIdStr(const StringId index) const

{ 
Assert(0 <= index);
Assert(index < m_nStr); 
#if !defined(ACTIVISION_ORIGINAL)
	if ((index < 0) || (index > m_nStr))
	{
		return NULL;
	}
#endif
return m_all[index]->m_id;
}








sint32 StringDB::GetStrNode(
    StringRecord *ptr, 
    const char *add_id, 
    char **new_text) 

{ 
sint32 r;

if (ptr == NULL) { 
    return FALSE;
} else { 

    r = strcmp(add_id, ptr->m_id);
    if (r < 0) { 
        return GetStrNode(ptr->m_lesser, add_id, new_text); 
    } else if (0 < r) { 
       return GetStrNode(ptr->m_greater, add_id, new_text); 
    } else { 
       *new_text = ptr->m_text; 
       return TRUE;  
    }   

}
}








void StringDB::Btree2Array()

{ 

	Assert (0 < m_nStr);
	m_all = new StringRecord*[m_nStr];
	Assert(m_all);
	sint32 count = 0; 
	
	for(int i=0; i<STRDB_NUM_HEADS; i++) 
		AssignIndex (m_head[i], count); 
}








void StringDB::AssignIndex(StringRecord *ptr, StringId &count)

{
	if (ptr->m_lesser) { 
		AssignIndex(ptr->m_lesser, count); 
	} 
	
	Assert(count < m_nStr); 
	
	ptr->m_index = count; 
	m_all[count] = ptr; 
	count++; 
	
	if (ptr->m_greater) { 
		AssignIndex(ptr->m_greater, count); 
	} 
}








sint32 StringDB::GetStringID(char str_id[k_MAX_NAME_LEN], StringId &index) const  

{ 
	return GetIndexNode(*GetHead(str_id), str_id, index); 
}

sint32 StringDB::GetStringID(const char *str_id, StringId &index) const
{
	return GetIndexNode(*GetHead(str_id), str_id, index);
}








sint32 StringDB::GetIndexNode(StringRecord * ptr, 
							  const char *str_id, 
							  StringId &index)  const 
{

	sint32 r;
	
	if (ptr == NULL) { 
#if !defined(ACTIVISION_ORIGINAL)
		index = INDEX_INVALID;
#endif
		return FALSE;
	} else { 
		
		r = strcmp(str_id, ptr->m_id);
		if (r < 0) { 
			return GetIndexNode(ptr->m_lesser, str_id, index); 
		} else if (0 < r) { 
			return GetIndexNode(ptr->m_greater, str_id, index); 
		} else { 
			index = ptr->m_index; 
			return TRUE;  
		}   
	}
}








const MBCHAR *StringDB::GetNameStr (StringId n) const

{
   Assert(n >= 0);
   if(n < 0 || n > m_nStr) {
	   return "BADSTRING";
   }
   return m_all[n]->m_text;
}








const MBCHAR *StringDB::GetNameStr(const char *s)
	{
	char	*tmp ;

	Assert(s != NULL);
	if(s == NULL) return NULL;

	if(GetText(s, &tmp)) {
		return (tmp) ;
	} else {
		return NULL;
	}
		
	}


sint32 StringDB::ParseAStringEntry(Token *strToken)

{
	char id[k_MAX_TOKEN_LEN]; 
	char text[k_MAX_TEXT_LEN]; 

	if (strToken->GetType() == TOKEN_EOF) { 
		return FALSE; 
	} 
	
   	if (strToken->GetType() != TOKEN_STRING) { 
		c3errors_ErrorDialog (strToken->ErrStr(), "Missing string id");
        g_abort_parse = TRUE;
		return FALSE; 
	} else { 
		strToken->GetString(id); 
	}

    
    strToken->Next();
    
    if ( strToken->GetType() == TOKEN_MISSING_QUOTE)  { 
   		c3errors_ErrorDialog (strToken->ErrStr(), "Could not find end quote for id %s", id);
        g_abort_parse = TRUE;
		return FALSE; 
    } else if ( strToken->GetType() != TOKEN_QUOTED_STRING) { 
		c3errors_ErrorDialog (strToken->ErrStr(), "Could not find text for id %s", id);
        g_abort_parse = TRUE;
		return FALSE; 
	} else { 
		strToken->GetString(text); 
	}

    strToken->Next();

	if (!InsertStr(id, text)) {
			c3errors_ErrorDialog (strToken->ErrStr(), "Could not inset string into string db"); 
            g_abort_parse = TRUE;
			return FALSE; 
	}

	return TRUE; 
}

sint32 StringDB::Parse(MBCHAR *filename)
{
	Token *strToken = NULL; 
	
	strToken = new Token(filename, C3DIR_GAMEDATA); 
	Assert(strToken); 

	while (ParseAStringEntry(strToken));
    if (g_abort_parse) return FALSE; 
	
	InsertStr("UNIT_MYSTERY", "???");
	Btree2Array(); 
	
	delete strToken;
	
	return TRUE; 
}
