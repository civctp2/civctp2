










#include "c3.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_ldl.h"
#include "aui_ranger.h"
#include "aui_window.h"
#include "aui_surface.h"

#include "aui_textbox.h"



aui_TextBox::aui_TextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ListBox( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_TextBox::aui_TextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *text,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ListBox( retval, id, x, y, width, height, ActionFunc, cookie ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL, k_AUI_TEXTBOX_MAXTEXT * k_AUI_TEXTBOX_MAXITEMS )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_TextBox::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}



AUI_ERRCODE aui_TextBox::InitCommon( void )
{
	m_numItems = 0,
	m_curItem = 0;
	m_curColor = k_AUI_UI_NOCOLOR;
	m_curBold = 0;
	m_curItalic = 0;

	
	TextReloadFont();

	
	aui_Static **itemPtr = m_items;
	for ( uint32 i = k_AUI_TEXTBOX_MAXITEMS; i; i--, itemPtr++ )
	{
		AUI_ERRCODE errcode;
		*itemPtr = new aui_Static(
			&errcode,
			aui_UniqueId(),
			0, 0, m_width, m_textfont->GetMaxHeight(),
			NULL, k_AUI_TEXTBOX_MAXTEXT );

		(*itemPtr)->SetBlindness( TRUE );

		
		(*itemPtr)->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT;

		
		
		(*itemPtr)->SetTextFont( m_textfont->GetTTFFile() );
		(*itemPtr)->SetTextFontSize( m_textfont->GetPointSize() );
	}

	return AUI_ERRCODE_OK;
}



aui_TextBox::~aui_TextBox()
{
	aui_Static **itemPtr = m_items;
	for ( uint32 i = k_AUI_TEXTBOX_MAXITEMS; i; i-- )
		delete *itemPtr++;
}



AUI_ERRCODE aui_TextBox::SetText(
	const MBCHAR *text,
	uint32 maxlen )
{
	AUI_ERRCODE errcode = aui_TextBase::SetText( text, maxlen );

	CalculateItems();

	return errcode;
}



AUI_ERRCODE aui_TextBox::AppendText( MBCHAR *text )
{
	return AppendText( text, k_AUI_UI_NOCOLOR );
}



AUI_ERRCODE aui_TextBox::AppendText(
	MBCHAR *text,
	COLORREF color,
	sint32 bold,
	sint32 italic )
{
	Assert( text != NULL );
	if ( !text ) return AUI_ERRCODE_INVALIDPARAM;

	
	if ( !strlen( text ) ) return AppendText( " ", color, bold, italic );

	
	m_curColor = color;
	m_curBold = bold;
	m_curItalic = italic;

	
	
	CalculateAppendedItems( text );

	
	Assert( m_curLength + strlen( text ) <= m_maxLength );

	
	strncat( m_text, text, m_maxLength - m_curLength );

	m_curLength = strlen( m_text );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_TextBox::CalculateItems( MBCHAR *text )
{
	
	
	
	sint32 rangerValue = m_verticalRanger->GetValueY();
	BOOL moveToEnd = rangerValue == m_verticalRanger->GetMaximumY();

	if ( !text )
	{
		text = m_text;

		aui_Static **itemPtr = m_items;
		for ( sint32 i = m_numItems; i; i-- )
			RemoveItem( (*itemPtr++)->Id() );

		m_numItems = 0;
		m_curItem = 0;
	}

	
	MBCHAR *cur = text;
	MBCHAR *stop = text + strlen( text );

	
	if ( cur == stop ) return AUI_ERRCODE_OK;

	uint32 length = 0;
	aui_Static **itemPtr = m_items + m_curItem;

	

	{
		do
		{
			
			const MBCHAR *start = cur;
			RECT wrap = { 0, 0, m_width, 0 };
			POINT penPos = { 0, 0 };

			m_textfont->GetLineInfo(
				&wrap,
				&penPos,
				NULL,
				NULL,
				&start,
				stop,
				TRUE );

			length = start - cur;

			if ( !length )
			{
				
				MBCHAR *token = FindNextToken( cur, " \t\n", 1 );
				if ( token )
					length = token - cur + 1;
				else
					length = stop - cur;
			}

			
			Assert( cur + length <= stop );
			if ( cur + length > stop ) length = stop - cur;

			
			MBCHAR tempChar = cur[ length ];
			cur[ length ] = '\0';

			
			if ( ++m_numItems > k_AUI_TEXTBOX_MAXITEMS )
			{
				m_numItems = k_AUI_TEXTBOX_MAXITEMS;
				RemoveItem( (*itemPtr)->Id() );

				
				memmove( m_text, m_text + m_maxLength - length, length );
				memset( m_text + m_maxLength - length, '\0', length + 1 );
			}

			
			(*itemPtr)->SetText( cur );

			
			COLORREF color = m_curColor;

			
			if ( color == k_AUI_UI_NOCOLOR )
			{
				if ( !m_curItem )
					color = m_items[ k_AUI_TEXTBOX_MAXITEMS - 1 ]->
						GetTextColor();
				else
					color = (*(itemPtr - 1))->GetTextColor();
			}
			(*itemPtr)->SetTextColor( color );

			
			(*itemPtr)->SetTextBold( m_curBold );

			
			(*itemPtr)->SetTextItalic( m_curItalic );

			
			AddItem( (aui_Item *)*itemPtr++ );

			
			cur[ length ] = tempChar;

			if ( ++m_curItem == k_AUI_TEXTBOX_MAXITEMS )
			{
				m_curItem = 0;
				itemPtr = m_items;
			}
		} while ( (cur += length) < stop );
	}

	
	

	
	if ( moveToEnd ) rangerValue = m_verticalRanger->GetMaximumY();
	m_verticalRanger->SetValue( 0, rangerValue );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_TextBox::CalculateAppendedItems( MBCHAR *text )
{
	return CalculateItems( text );
}



AUI_ERRCODE aui_TextBox::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawThisStateImage(
		0,
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}




void aui_TextBox::SetTextFont( MBCHAR *ttffile )
{
	aui_TextBase::SetTextFont( ttffile );

	aui_Static **itemPtr = m_items;
	for ( uint32 i = k_AUI_TEXTBOX_MAXITEMS; i; i--, itemPtr++ )
		if (*itemPtr)
			(*itemPtr)->SetTextFont( ttffile );

	CalculateItems();
}




void aui_TextBox::SetTextFontSize( sint32 pointSize )
{
	aui_TextBase::SetTextFontSize( pointSize );

	sint32 textHeight = m_textfont->GetMaxHeight();

	aui_Static **itemPtr = m_items;
	for ( uint32 i = k_AUI_TEXTBOX_MAXITEMS; i; i--, itemPtr++ )
	{
		if (*itemPtr)
		{
			(*itemPtr)->SetTextFontSize( pointSize );
			(*itemPtr)->Resize( m_width, textHeight );
		}
	}

	CalculateItems();
}
