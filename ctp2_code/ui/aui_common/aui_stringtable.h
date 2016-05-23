#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_STRINGTABLE_H__
#define __AUI_STRINGTABLE_H__

#include <vector>           // std::vector

class aui_StringTable;

#include "aui_base.h"
#include "auitypes.h"       // AUI_ERRCODE
#include "ctp2_inttypes.h"  // sint32

class ldl_datablock;
// MBCHAR

class aui_StringTable : aui_Base
{
public:
	aui_StringTable
    (
		AUI_ERRCODE *   retval,
		MBCHAR const *  ldlBlock
    );
	aui_StringTable
    (
		AUI_ERRCODE *   retval,
		size_t          numStrings = 0
    );
	virtual ~aui_StringTable();

	sint32	    GetNumStrings( void ) const
    {
        return static_cast<sint32>(m_Strings.size());
    };

	MBCHAR *    GetString(sint32 index) const;
	AUI_ERRCODE	SetString(const MBCHAR * text, sint32 index);

private:
	size_t		FindNumStringsFromLdl(ldl_datablock * block);

    std::vector<MBCHAR *>   m_Strings;
};

#endif
