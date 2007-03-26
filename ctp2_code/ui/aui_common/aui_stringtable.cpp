//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : String table user interface object
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
// - Memory leaks repaired.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_stringtable.h"

#include <algorithm>
#include "aui_ldl.h"
#include "aui_ui.h"
#include "StrDB.h"              // g_theStringDB

#define k_AUI_STRINGTABLE_LDL_NUMSTRINGS		"numstrings"
#define k_AUI_STRINGTABLE_LDL_STRING			"string"
#define k_AUI_STRINGTABLE_LDL_NODATABASE		"nodatabase"

aui_StringTable::aui_StringTable
(
	AUI_ERRCODE *   retval,
	size_t          numStrings 
)
:
	m_Strings       (numStrings, (MBCHAR *) NULL)
{
    *retval = AUI_ERRCODE_OK;
}

aui_StringTable::aui_StringTable
(
	AUI_ERRCODE *   retval,
	MBCHAR const *  ldlBlock 
)
:
	m_Strings       ()
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert(block);
	if (!block)
    {
        *retval = AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
        return;
    }

	m_Strings.resize(FindNumStringsFromLdl(block), (MBCHAR *) NULL);

    MBCHAR temp[k_AUI_LDL_MAXBLOCK + 1];

	if ( block->GetBool(k_AUI_STRINGTABLE_LDL_NODATABASE) )
    {
		for (size_t i = 0; i < m_Strings.size(); ++i)
		{
			sprintf(temp, "%s%d", k_AUI_STRINGTABLE_LDL_STRING, i);
			SetString(block->GetString(temp), i);
		}
	}
	else 
    {
		for (size_t i = 0; i < m_Strings.size(); ++i)
		{
			sprintf(temp, "%s%d", k_AUI_STRINGTABLE_LDL_STRING, i);
			SetString(g_theStringDB->GetNameStr(block->GetString(temp)), i);
		}
	}

	*retval = AUI_ERRCODE_OK;
}


aui_StringTable::~aui_StringTable()
{
    for 
    (
        std::vector<MBCHAR *>::iterator p = m_Strings.begin();
        p != m_Strings.end();
        ++p
    )
	{
		delete [] *p;
	}
    
    std::vector<MBCHAR *>().swap(m_Strings);
}

size_t aui_StringTable::FindNumStringsFromLdl(ldl_datablock * block)
{
    sint32  stringCount = 0;

	if (ATTRIBUTE_TYPE_INT == 
            block->GetAttributeType(k_AUI_STRINGTABLE_LDL_NUMSTRINGS)
       )
    {
        // Use the numstrings entry
		stringCount = block->GetInt(k_AUI_STRINGTABLE_LDL_NUMSTRINGS);
        Assert(stringCount >= 0);
    }
    else
    {
        // Look for string0, string1, etc. entries and count
	    MBCHAR  temp[k_AUI_LDL_MAXBLOCK + 1];
	    bool    isAtEnd = false;
        
        while (!isAtEnd)
        {
		    sprintf(temp, "%s%d", k_AUI_STRINGTABLE_LDL_STRING, stringCount);
            if (block->GetString(temp))
            {
                ++stringCount;
            }
            else
            {
                isAtEnd = true;
            }
	    }
    }

	return static_cast<size_t>(stringCount);
}






MBCHAR * aui_StringTable::GetString( sint32 index ) const
{
	Assert(index >= 0 && static_cast<size_t>(index) < m_Strings.size());
	if (index < 0 || static_cast<size_t>(index) >= m_Strings.size()) return NULL;

	return m_Strings[index];
}



AUI_ERRCODE aui_StringTable::SetString(const MBCHAR *text, sint32 index)
{
	Assert(index >= 0 && static_cast<size_t>(index) < m_Strings.size());
	if (index < 0 || static_cast<size_t>(index) >= m_Strings.size()) 
        return AUI_ERRCODE_INVALIDPARAM;

	if (text)
	{
		size_t const oldSize = m_Strings[index] ? 1 + strlen(m_Strings[index]) : 0;
		size_t const newSize = 1 + strlen(text);

		if (oldSize < newSize)
		{
			delete [] m_Strings[index];
			m_Strings[index] = new MBCHAR[newSize];
		}

		strcpy(m_Strings[index], text);
	}
	else
	{
		delete [] m_Strings[index];
		m_Strings[index] = NULL;
	}

	return AUI_ERRCODE_OK;
}
