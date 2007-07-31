










#include "c3.h"
#include "aui_ui.h"
#include "aui_ldl.h"

#include "aui_stringtable.h"

#include "StrDB.h"

extern StringDB	*g_theStringDB;



aui_StringTable::aui_StringTable(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock )
{
	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_StringTable::aui_StringTable(
	AUI_ERRCODE *retval,
	sint32 numStrings )
{
	*retval = InitCommon( numStrings );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_StringTable::InitCommonLdl( MBCHAR *ldlBlock )
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
	if ( m_numStrings = numStrings )
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
		for ( sint32 i = 0; i < m_numStrings; i++ )
			if ( m_strings[ i ] )
				delete[ strlen( m_strings[ i ] ) + 1 ] m_strings[ i ];

		delete[ m_numStrings ] m_strings;
		m_strings = 0;

		m_numStrings = 0;
	}
}



sint32 aui_StringTable::SetNumStrings( sint32 numStrings )
{
	Assert( numStrings >= 0 );
	if ( numStrings < 0 ) return AUI_ERRCODE_INVALIDPARAM;

	sint32 prevNumStrings = m_numStrings;

	if ( numStrings == 0 )
	{
		if ( m_strings ) delete[ m_numStrings ] m_strings;
		m_strings = NULL;
		m_numStrings = 0;
	}
	else
	{
		MBCHAR **strings = new MBCHAR *[ numStrings ];
		Assert( strings != NULL );
		if ( strings )
		{
			sint32 minNumStrings =
				m_numStrings < numStrings ? m_numStrings : numStrings;

			memset( strings, 0, numStrings * sizeof( MBCHAR * ) );
			memcpy( strings, m_strings, minNumStrings * sizeof( MBCHAR * ) );

			delete[ m_numStrings ] m_strings;
			m_strings = strings;

			m_numStrings = numStrings;
		}
	}

	return prevNumStrings;
}



MBCHAR *aui_StringTable::GetString( sint32 index )
{
	Assert( index >= 0 && index < m_numStrings );
	if ( index < 0 || index >= m_numStrings ) return NULL;

	return m_strings[ index ];
}



AUI_ERRCODE aui_StringTable::SetString( const MBCHAR *text, sint32 index )
{
	Assert( index >= 0 && index < m_numStrings );
	if ( index < 0 || index >= m_numStrings ) return AUI_ERRCODE_INVALIDPARAM;

	if ( !text )
	{
		if ( m_strings[ index ] )
			delete[ strlen( m_strings[ index ] ) + 1 ] m_strings[ index ];
		m_strings[ index ] = NULL;
	}
	else
	{
		sint32 oldlen;
		sint32 newlen = strlen( text );

		if ( !m_strings[ index ] || (oldlen = strlen( m_strings[ index ] )) < newlen )
		{
			if ( m_strings[ index ] ) delete[ oldlen + 1 ] m_strings[ index ];
			m_strings[ index ] = new MBCHAR[ newlen + 1 ];
			Assert( m_strings[ index ] != NULL );
			if ( !m_strings[ index ] ) return AUI_ERRCODE_MEMALLOCFAILED;
		}

		strcpy( m_strings[ index ], text );
	}

	return AUI_ERRCODE_OK;
}
