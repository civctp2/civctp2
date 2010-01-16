












#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "c3scroller.h"
#include "c3ui.h"
#include "pattern.h"
#include "aui_static.h"
#include "textswitch.h"

#include "primitives.h"
#include "textutils.h"
#include "colorset.h"

#include "texttable.h"
#include "c3windows.h"


extern C3UI			*g_c3ui;

TextTable::TextTable(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie,
	aui_Surface *surface)
	:
	aui_ListBox(retval, id, ldlBlock, ActionFunc, cookie),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_ImageBase( ldlBlock )
{














	*retval = aui_SoundBase::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( ldlBlock, surface );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangers();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


TextTable::TextTable(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 columns,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie,
	uint32 size,
	aui_Surface *surface)
	:
	aui_ListBox(),
	aui_TextBase(NULL, k_TEXTTABLE_MAXTEXT * k_TEXTTABLE_MAXITEMS),
	aui_ImageBase( (sint32)0 )
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_ListBox::InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon( (MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	

	
	*retval = InitCommon(columns, surface);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRangers();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE TextTable::InitCommon( MBCHAR *ldlBlock, aui_Surface *surface )
{
	
	uint32 columns = 0;
	uint32 size = 0;

	aui_Ldl *theLdl = g_c3ui->GetLdl();
	
	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	columns = block->GetInt( k_TEXTTABLE_LDL_COLUMNS );
	size = block->GetInt( k_TEXTTABLE_LDL_TEXTSIZE );

	

	


	
	if (!columns) return AUI_ERRCODE_OK;

	
	InitCommon ( columns, surface );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE TextTable::InitCommon( uint32 columns, aui_Surface *surface )
{
	m_numItems = 0;

	
	m_item_width = m_width / columns;
	m_item_height = 0;

	MBCHAR strbuf[256];
	uint32 i = 0;

	
	if (columns > k_TEXTTABLE_MAXHEADERS) m_columns = k_TEXTTABLE_MAXHEADERS;
	else m_columns = columns;
	
	m_rows = 0;
	sint32 textHeight = 0;

	
	if (surface) 
	{
		textHeight = textutils_GetFontHeight(surface, m_fontsize );
	}
	else
	{
		
		textHeight = textutils_GetHeight(g_c3ui->Secondary(), "Wj" );
	}
	
	
	m_item_height = textHeight;

	AUI_ERRCODE errcode;

	
	aui_Static **itemPtr = m_items;
	for ( i = k_TEXTTABLE_MAXITEMS; i; i-- )
	{
		*itemPtr = new aui_Static(
			&errcode,
			aui_UniqueId(),
			0, 0, m_item_width, m_item_height,
			NULL, 
			k_TEXTTABLE_MAXTEXT);

		(*itemPtr)->SetTextFontSize(m_fontsize);

		itemPtr++;
	}

	TextSwitch **headerPtr = m_table_headers;
	for ( i = 0; i < m_columns; i++ )
	{
		sprintf(strbuf,"col-%d",i);
		*headerPtr = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0, m_item_width, m_item_height,
			k_PatternName,
			strbuf );
		(*headerPtr)->IgnoreEvents(TRUE);
		*headerPtr++;
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE TextTable::CreateRangers( void )
{
	AUI_ERRCODE errcode;

	m_verticalRanger = new C3Scroller(
		&errcode,
		aui_UniqueId(),
		0, 0, 0, 0,
		TRUE,
		m_pattern->GetFilename(),
		ListBoxRangerActionCallback,
		this );
	if ( !m_verticalRanger ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_horizontalRanger = new C3Scroller(
		&errcode,
		aui_UniqueId(),
		0, 0, 0, 0,
		FALSE,
		m_pattern->GetFilename(),
		ListBoxRangerActionCallback,
		this );
	if ( !m_horizontalRanger ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_verticalRanger );
	AddChild( m_horizontalRanger );

	RepositionRangers();

	
	
	return AUI_ERRCODE_OK;
}



TextTable::~TextTable()
{
	uint32 i = 0;

	aui_Static	**itemPtr = m_items;
	for ( i = k_TEXTTABLE_MAXITEMS; i; i-- )
		delete *itemPtr++;

	
	




}


void TextTable::InitHeaders( void )
{
	uint32 i = 0;

	
	if ( !m_columns ) return;

	TextSwitch **headerPtr = m_table_headers;
	for ( i = 0; i < m_columns ; i++ )
		AddHeaderSwitch(*headerPtr++);
}



BOOL TextTable::AddColumn( uint32 pos, MBCHAR *text )
{
	AUI_ERRCODE errcode;
	uint32 i = 0;
	MBCHAR strbuf[256];

	TextSwitch **headerPtr = m_table_headers;
	
	
	if (!pos || (pos > m_columns)) pos = m_columns;
	
	
	if (!text)
		sprintf(strbuf,"col-%d",pos);
	else strcpy(strbuf, text);

	
	m_item_width = m_width / (m_columns + 1);

	
	for ( i = 0 ; i < pos ; i++ ) 
		*headerPtr++;

	
	if (pos == m_columns)
	{
		
		*headerPtr = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0, m_item_width, m_item_height,
			k_PatternName,
			strbuf );
	}
	
	return TRUE;
}


BOOL TextTable::AddRow( uint32 pos, MBCHAR *text )
{
	

	uint32 i = 0;
	MBCHAR strbuf[256];

	
	if (!pos || (pos > m_rows)) pos = m_rows;
	
	
	if (!text)
		sprintf(strbuf,"row-%d",pos);
	else strcpy(strbuf, text);

	
	

	
	if ( (m_numItems + m_columns) > k_TEXTTABLE_MAXITEMS ) return FALSE;

	
	aui_Static **itemPtr = m_items;
	aui_Static *parent = NULL;
	
	for ( i = 0 ; i < m_numItems ; i++ ) 
		*itemPtr++;
	
	
	parent = *itemPtr++;
	parent->SetText(strbuf);

	
	for ( i = 1 ; i < m_columns ; i++ )
	{


		parent->AddChild(*itemPtr++);
	}

	
	m_numItems = m_numItems + m_columns;
	m_rows++;

	
	AddItem((aui_Item *)parent);
	return TRUE;
}


BOOL TextTable::SetTextEntry( uint32 row, uint32 column, MBCHAR *text )
{
	uint32 i,j;

	
	if (( row >= m_rows ) || ( column >= m_columns )) return FALSE;

	
	aui_Static **itemPtr = m_items;
	
	
	for ( i = 0 ; i < m_rows ; i++ )
	{
		
		if ( i == row )
		{
			
			for ( j = 0 ; j < column ; j++ )
				*itemPtr++;
			(*itemPtr)->SetText(text);
			break;
		}
		else 
		{
			for ( j = 0 ; j < m_columns ; j++ )
				*itemPtr++;
		}
	}

	return TRUE;
}


BOOL TextTable::GetTextEntry( uint32 row, uint32 column, MBCHAR *strbuf )
{
	uint32 i,j;

	
	if (( row >= m_rows ) || ( column >= m_columns )) return FALSE;

	
	aui_Static **itemPtr = m_items;
	
	
	for ( i = 0 ; i < m_rows ; i++ )
	{
		
		if ( i == row )
		{
			
			for ( j = 0 ; j < column ; j++ )
				*itemPtr++;
			strcpy(strbuf,(*itemPtr)->GetText());
			break;
		}
		else 
		{
			for ( j = 0 ; j < m_columns ; j++ )
				*itemPtr++;
		}
	}

	return TRUE;
}


BOOL TextTable::SetTextHeader( uint32 pos, MBCHAR *text )
{
	uint32 i = 0;

	
	if (pos >= m_columns) return FALSE;

	TextSwitch **headerPtr = m_table_headers;

	
	for( i = 0 ; i < pos ; i++ )
		*headerPtr++;

	(*headerPtr)->SetText(text);

	return TRUE;
}


BOOL TextTable::GetTextHeader( uint32 pos, MBCHAR *strbuf )
{
	uint32 i = 0;

	
	if (pos >= m_columns) return FALSE;

	TextSwitch **headerPtr = m_table_headers;

	
	for( i = 0 ; i < pos ; i++ )
		*headerPtr++;

	strcpy(strbuf,(*headerPtr)->GetText());

	return TRUE;
}



BOOL TextTable::CleanTextTable( void )
{
	uint32 i,j;

	aui_Static **itemPtr = m_items;
	aui_Static *parent = NULL;

	
	for ( i = 0 ; i < m_rows ; i++ )
	{
		parent = *itemPtr++;
		RemoveItem(parent->Id());
		
		for ( j = 1 ; j < m_columns ; j++ )
		{
			parent->RemoveChild((*itemPtr)->Id());
			*itemPtr++;
		}
	}

	
	m_numItems = 0;
	m_rows = 0;

	return TRUE;
}


AUI_ERRCODE TextTable::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	


	

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	if ( IsActive() )
	{
		
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
