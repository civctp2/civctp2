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
// - Repaired memory leaks.
// - Reimplemented ** as vector of *, to make it less error prone.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3files.h"

#include "StrRec.h"
#include "StrDB.h"

#include "Token.h"

extern sint32	g_abort_parse;


namespace
{
	
	StringId const			INDEX_INVALID	= -1;
	unsigned short const	STRDB_NUM_HEADS	= 547;	// hash table size


//----------------------------------------------------------------------------
//
// Name       : ComputeHashIndex
//
// Description: Compute a hash index from a string.
//
// Parameters : id			: the string
//
// Globals    : -
//
// Returns    : size_t		: index in the hash table
//
// Remark(s)  : Returned value will be in [0, tableSize>
//
//----------------------------------------------------------------------------

	size_t ComputeHashIndex(MBCHAR const * id)
	{
		unsigned short hash;

		__asm 
		{
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
		
		return static_cast<size_t>(hash % STRDB_NUM_HEADS);
	}

} // namespace



StringDB::StringDB()
:	m_all(),					
	m_head(STRDB_NUM_HEADS, NULL)
{
}


StringDB::~StringDB()
{
	for 
	(
		std::vector<StringRecord *>::iterator p	= m_all.begin();
		p != m_all.end();
		++p
	)
	{
		delete (*p);
	}
}


StringRecord const * const & StringDB::GetHead(MBCHAR const * id) const
{
	return m_head[ComputeHashIndex(id)];
}

StringRecord * & StringDB::GetHead(MBCHAR const * id)
{
	return m_head[ComputeHashIndex(id)];
}









//----------------------------------------------------------------------------
//
// Name       : StringDB::InsertStr
//
// Description: Add a new string mapping to the database
//
// Parameters : add_id		: (language independent) id
//				new_text	: (language dependent) real text
//
// Globals    : -
//
// Returns    : bool		: new string has been added
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool StringDB::InsertStr
(
	MBCHAR const *			add_id, 
	MBCHAR const *			new_text
) 
{
	StringRecord *	newRec;

	if (AddStrNode(GetHead(add_id), add_id, new_text, newRec))
	{
		// No action: the new string was added successfully.
	}
	else
	{
		// A string with the same id already exists:
		// Generate a new one with a key (add_id + '#' + number) and maintain
		// a special key (add_id + "#COUNT") to keep track of the (sequence)
		// number.
        
		MBCHAR * 	tempstr	= new MBCHAR[strlen(add_id) + 7];
        sprintf(tempstr, "%s#COUNT", add_id);

        MBCHAR *	countstr;
		if (GetStrNode(GetHead(tempstr), tempstr, &countstr))
		{
			// No action: the special key has been created already.
		}
		else
		{
			// Create a new special key, initialised with count 1.
			AddStrNode(GetHead(tempstr), tempstr, "1     ", newRec);
			GetStrNode(GetHead(tempstr), tempstr, &countstr);
        }

		// Create a new numbered key.
        int const	count	= atoi(countstr);
        sprintf(tempstr, "%s#%d", add_id, count);
        AddStrNode(GetHead(tempstr), tempstr, new_text, newRec);

		// Update the sequence counter.
        sprintf(countstr, "%d", count + 1);

		delete [] tempstr;
    }

	return true;
}







//----------------------------------------------------------------------------
//
// Name       : StringDB::AddStrNode
//
// Description: Add a new node to the string tree.
//
// Parameters : ptr			: top of tree
//				add_id		: key (string)
//				new_text	: stored value (string)
//
// Globals    : -
//
// Returns    : bool		: a new node has been added
//				newPtr		: the new node, when added
//
// Remark(s)  : When the key already exists, the new node will not be added.
//
//----------------------------------------------------------------------------

bool StringDB::AddStrNode
(
	StringRecord * &		ptr,
	MBCHAR const *			add_id,
	MBCHAR const *			new_text,
	StringRecord * &		newPtr
)
{ 
	if (ptr)
	{
		// At a branch: traverse the tree to find the right place to add.
		int const	r	= strcmp(add_id, ptr->m_id);
		if (r < 0) 
		{ 
			return AddStrNode(ptr->m_lesser, add_id, new_text, newPtr); 
		} 
		else if (0 < r) 
		{ 
			return AddStrNode(ptr->m_greater, add_id, new_text, newPtr); 
		} 
		else 
		{ 
			// Key exists: do not add
			return false;
		}   
	}
	else
	{ 
		// At a leaf: add here. 
		ptr				= new StringRecord(); 
		ptr->m_id		= new char[strlen(add_id) + 1];
		strcpy(ptr->m_id, add_id); 
		ptr->m_text		= new char[strlen(new_text) + 1];	
		strcpy(ptr->m_text, new_text); 
		AssignIndex(ptr);
		newPtr			= ptr;
		return true;
	}
}






bool StringDB::GetText(MBCHAR const * get_id, MBCHAR ** new_text) const
{
	return GetStrNode(GetHead(get_id), get_id, new_text);
}








//----------------------------------------------------------------------------
//
// Name       : StringDB::GetIdStr
//
// Description: Get the language independent id key of an index.
//
// Parameters : index	: index of the string
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Returns NULL for invalid indices.
//
//----------------------------------------------------------------------------

MBCHAR * StringDB::GetIdStr(StringId const & index) const
{ 
	Assert(0 <= index);
	Assert(index < m_all.size());
	return (index < 0) || (index >= m_all.size()) ? NULL : m_all[index]->m_id;
}






bool StringDB::GetStrNode
(
    StringRecord const *	ptr, 
    MBCHAR const *			add_id, 
    MBCHAR **				new_text
) const
{ 
	if (ptr)
	{ 
		int const r	= strcmp(add_id, ptr->m_id);
		if (r < 0) 
		{ 
			return GetStrNode(ptr->m_lesser, add_id, new_text); 
		} 
		else if (0 < r) 
		{ 
			return GetStrNode(ptr->m_greater, add_id, new_text); 
		} 
		else 
		{ 
			*new_text = ptr->m_text; 
			return true;  
		}   
	}

	return false;
}








//----------------------------------------------------------------------------
//
// Name       : StringDB::Btree2Array
//
// Description: (Re)sort the string indices from the table.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void StringDB::Btree2Array(void)
{
	m_all.clear();

	for 
	(
		std::vector<StringRecord *>::iterator p	= m_head.begin();
		p != m_head.end();
		++p
	)
	{
		AssignIndex(*p);
	}
}








//----------------------------------------------------------------------------
//
// Name       : StringDB::AssignIndex
//
// Description: Assign indices to a - sorted - record tree.
//
// Parameters : ptr		: top of the tree
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Appends the - flattened - tree node references at the end of 
//              m_all.
//
//----------------------------------------------------------------------------

void StringDB::AssignIndex(StringRecord * & ptr)

{
	if (ptr->m_lesser) 
	{ 
		AssignIndex(ptr->m_lesser); 
	} 
	
	ptr->m_index = m_all.size();
	m_all.push_back(ptr);
	
	if (ptr->m_greater) 
	{ 
		AssignIndex(ptr->m_greater); 
	} 
}






bool StringDB::GetStringID(MBCHAR const * str_id, StringId & index) const
{
	return GetIndexNode(GetHead(str_id), str_id, index); 
}






bool StringDB::GetIndexNode
(
	StringRecord const *	ptr,
	MBCHAR const *			str_id,
	StringId &				index
) const
{

	sint32 r;
	
	if (ptr == NULL) { 
		index = INDEX_INVALID;	// fill index to prevent crash
		return false;
	} else { 
		
		r = strcmp(str_id, ptr->m_id);
		if (r < 0) { 
			return GetIndexNode(ptr->m_lesser, str_id, index); 
		} else if (0 < r) { 
			return GetIndexNode(ptr->m_greater, str_id, index); 
		} else { 
			index = ptr->m_index;
			return true;
		}   
	}
}








//----------------------------------------------------------------------------
//
// Name       : StringDB::GetNameStr
//
// Description: Get the language dependent text of an index.
//
// Parameters : index	: index of the string
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Returns "BADSTRING" for invalid indices.
//
//----------------------------------------------------------------------------

MBCHAR const * StringDB::GetNameStr(StringId const & n) const
{
	Assert(n >= 0);
	return (n < 0) || (n >= m_all.size()) ? "BADSTRING" : m_all[n]->m_text;
}





	

MBCHAR const * StringDB::GetNameStr(MBCHAR const * s) const
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

//----------------------------------------------------------------------------
//
// Name       : StringDB::Parse
//
// Description: Read the string database from an text file.
//
// Parameters : filename	: file to read from
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool StringDB::Parse(MBCHAR * filename)
{
	Token *	strToken = new Token(filename, C3DIR_GAMEDATA); 

	while (ParseAStringEntry(strToken))
	{
		// No action: ParseAStringEntry fills m_head and m_all.
	}

	delete strToken;	// or make it an auto_ptr

    if (g_abort_parse)
	{
		return false;
	}

	InsertStr("UNIT_MYSTERY", "???");
	Btree2Array();		// re-index all entries 
	return true; 
}
