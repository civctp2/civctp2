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
		const MBCHAR *  ldlBlock
	);
	aui_StringTable
	(
		AUI_ERRCODE *   retval,
		size_t          numStrings = 0
	);
	virtual ~aui_StringTable();

	size_t    GetNumStrings( void ) const
	{
		return m_Strings.size();
	};

	const MBCHAR *    GetString(size_t index) const;
	AUI_ERRCODE	SetString(const MBCHAR * text, size_t index);

private:
	size_t		FindNumStringsFromLdl(ldl_datablock * block);

	std::vector<MBCHAR *>   m_Strings;
};

#endif
