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
#include "aui_ui.h"
#include "aui_ldl.h"

#include "aui_stringtable.h"

#include "StrDB.h"

extern StringDB	*g_theStringDB;


aui_StringTable::aui_StringTable(
	AUI_ERRCODE *retval,
	const MBCHAR *ldlBlock )
{
	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}


aui_StringTable::aui_StringTable(
	AUI_ERRCODE *retval,
	sint32 numStrings )
{
	*retval = InitCommon( numStrings );
	Assert( AUI_SUCCESS(*retval) );
}


AUI_ERRCODE aui_StringTable::InitCommonLdl(const MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	sint32 numStrings = FindNumStringsFromLdl( block );

	AUI_ERRCODE errcode = InitCommon( numStrings );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( block->GetBool(k_AUI_STRINGTABLE_LDL_NODATABASE) ) {
		for ( sint32 i = 0; i < m_numStrings; i++ )
		{
			sprintf( temp, "%s%d", k_AUI_STRINGTABLE_LDL_STRING, i );
			SetString(
				block->GetString( temp ),
				i );
		}
	}
	else {
		for ( sint32 i = 0; i < m_numStrings; i++ )
		{
			sprintf( temp, "%s%d", k_AUI_STRINGTABLE_LDL_STRING, i );
			SetString(
				g_theStringDB->GetNameStr( block->GetString(temp) ),
				i );
		}
	}

	return AUI_ERRCODE_OK;
}


sint32 aui_StringTable::FindNumStringsFromLdl( ldl_datablock *block )
{
	if ( block->GetAttributeType( k_AUI_STRINGTABLE_LDL_NUMSTRINGS ) ==
			ATTRIBUTE_TYPE_INT )
		return block->GetInt( k_AUI_STRINGTABLE_LDL_NUMSTRINGS );

	sint32 i = -1;
	BOOL found = TRUE;
	MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ];
	while ( found )
	{
		i++;
		found = FALSE;
		sprintf( temp, "%s%d", k_AUI_STRINGTABLE_LDL_STRING, i );
		if ( block->GetString( temp ) ) found = TRUE;
	}

	return i;
}


AUI_ERRCODE aui_StringTable::InitCommon( sint32 numStrings )
{
	if ((m_numStrings = numStrings))
	{
		m_strings = new MBCHAR *[ numStrings ];
		Assert( m_strings != NULL );
		if ( !m_strings ) return AUI_ERRCODE_MEMALLOCFAILED;

		memset( m_strings, 0, m_numStrings * sizeof( MBCHAR * ) );
	}
	else
		m_strings = NULL;

	return AUI_ERRCODE_OK;
}


aui_StringTable::~aui_StringTable()
{
	if ( m_strings )
	{
		for (sint32 i = 0; i < m_numStrings; ++i)
		{
			delete [] m_strings[i];
		}
		delete [] m_strings;
	}
}


sint32 aui_StringTable::SetNumStrings( sint32 numStrings )
{
	Assert( numStrings >= 0 );
	if ( numStrings < 0 ) return AUI_ERRCODE_INVALIDPARAM;

	sint32 prevNumStrings = m_numStrings;

	MBCHAR **		strings	= NULL;
	size_t const	copyEnd	= std::min(m_numStrings, numStrings);

	if (numStrings)
	{
		strings	= new MBCHAR *[numStrings];
		std::copy(m_strings, m_strings + copyEnd, strings);
		std::fill(strings + copyEnd, strings + numStrings, (MBCHAR *) NULL);
	}

	for (sint32 i = copyEnd; i < m_numStrings; ++i)
	{
		delete [] m_strings[i];
	}
	delete [] m_strings;

	m_strings		= strings;
	m_numStrings	= numStrings;

	return prevNumStrings;
}


const MBCHAR *aui_StringTable::GetString( sint32 index )
{
	Assert( index >= 0 && index < m_numStrings );
	if ( index < 0 || index >= m_numStrings ) return NULL;

	return m_strings[ index ];
}


AUI_ERRCODE aui_StringTable::SetString( const MBCHAR *text, sint32 index )
{
	Assert( index >= 0 && index < m_numStrings );
	if ( index < 0 || index >= m_numStrings ) return AUI_ERRCODE_INVALIDPARAM;

	if (text)
	{
		size_t const oldSize = m_strings[index] ? 1 + strlen(m_strings[index]) : 0;
		size_t const newSize = 1 + strlen(text);

		if (oldSize < newSize)
		{
			delete m_strings[index];
			m_strings[index] = new MBCHAR[newSize];
		}

		strcpy(m_strings[index], text);
	}
	else
	{
		delete m_strings[index];
		m_strings[index] = NULL;
	}

	return AUI_ERRCODE_OK;
}
