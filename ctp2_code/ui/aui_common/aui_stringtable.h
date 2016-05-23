#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_STRINGTABLE_H__
#define __AUI_STRINGTABLE_H__

#include "aui_base.h"


#define k_AUI_STRINGTABLE_LDL_NUMSTRINGS		"numstrings"
#define k_AUI_STRINGTABLE_LDL_STRING			"string"

#define k_AUI_STRINGTABLE_LDL_NODATABASE		"nodatabase"

class ldl_datablock;


class aui_StringTable : aui_Base
{
public:

	aui_StringTable(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock );
	aui_StringTable(
		AUI_ERRCODE *retval,
		sint32 numStrings = 0 );
	virtual ~aui_StringTable();

protected:
	aui_StringTable() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 numStrings );
	sint32		FindNumStringsFromLdl( ldl_datablock *block );

public:
	sint32	GetNumStrings( void ) const { return m_numStrings; }
	sint32	SetNumStrings( sint32 numStrings );

	MBCHAR		*GetString( sint32 index );
	AUI_ERRCODE	SetString( const MBCHAR *text, sint32 index );

protected:
	sint32	m_numStrings;
	MBCHAR	**m_strings;
};

#endif
