












#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_directsurface.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_bitmapfont.h"

#include "c3_ranger.h"
#include "c3ui.h"
#include "pattern.h"
#include "aui_static.h"
#include "c3_static.h"
#include "aui_item.h"

#include "primitives.h"
#include "textutils.h"
#include "colorset.h"

#include "textbox.h"


extern C3UI			*g_c3ui;


TextBox::TextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL, k_TEXTBOX_MAXTEXT * k_TEXTBOX_MAXITEMS ),
	aui_TextBox()
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

	*retval = aui_SoundBase::InitCommon((MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(FALSE);
	Assert( AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;

	*retval = CreateRangers(NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


TextBox::TextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_TextBox()
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_ListBox::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;

	*retval = CreateRangers(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE TextBox::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon(TRUE);
}



AUI_ERRCODE TextBox::InitCommon( BOOL fromLDL )
{
	m_numItems = 0,
	m_curItem = 0;
	m_curColor = k_AUI_UI_NOCOLOR;
	m_curBold = FALSE;
	m_curItalic = FALSE;

	sint32 i;
	aui_Static **itemPtr = m_items;


	
	TextReloadFont();


	for (i=0; i<k_AUI_TEXTBOX_MAXITEMS; i++) {
		(*itemPtr) = NULL;
		itemPtr++;
	}

	aui_BitmapFont	*font = GetTextFont();

	if (!font) {
		aui_TextBase::SetTextFont(k_AUI_TEXTBASE_DEFAULT_FONTNAME);
		aui_TextBase::TextReloadFont();
		font = GetTextFont();
	}

	MBCHAR *fontName = font->GetTTFFile();

	itemPtr = m_items;
	for (i = k_AUI_TEXTBOX_MAXITEMS; i; i--, itemPtr++ )
	{
		AUI_ERRCODE		errcode;


		*itemPtr = new c3_Static(
			&errcode,
			aui_UniqueId(),
			10, 0, m_width-20, 14,
			NULL, NULL, k_AUI_TEXTBOX_MAXTEXT, 0 ,0 );

		(*itemPtr)->SetTextFont( fontName);
		(*itemPtr)->SetTextFontSize(9);
		(*itemPtr)->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT;
		(*itemPtr)->SetTextColor(GetTextColor());
		if (GetTextShadow()) (*itemPtr)->SetTextShadow(TRUE);
		(*itemPtr)->SetTextShadowColor(GetTextShadowColor());
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE TextBox::CreateRangers( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		*patternFilename = NULL;

	if (m_pattern)
		patternFilename = m_pattern->GetFilename();

	
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		
		if ( m_header )
		{
			RemoveChild( m_header->Id() );
			delete m_header;
			m_header = NULL;
		}

		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_HEADER );

		
        if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_header = new aui_Header(
				&errcode,
				aui_UniqueId(),
				block );
	}

	if ( !m_header )
		m_header = new aui_Header(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0 );

	Assert( AUI_NEWOK(m_header,errcode) );
	if ( !AUI_NEWOK(m_header,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_header );

	
	ListPos position = m_header->ChildList()->GetHeadPosition();
	for ( sint32 i = m_header->ChildList()->L(); i; i-- )
		m_widthList->AddTail(
			m_header->ChildList()->GetNext( position )->Width() );

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_RANGERY );

		
        if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_verticalRanger = new c3_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				ListBoxRangerActionCallback,
				this );
	}

	if ( !m_verticalRanger )
		m_verticalRanger = new c3_Ranger(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_VERTICAL,
			patternFilename,
			ListBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_verticalRanger,errcode) );
	if ( !AUI_NEWOK(m_verticalRanger,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_verticalRanger );

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_LISTBOX_LDL_RANGERX );

		
        if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_horizontalRanger = new c3_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				ListBoxRangerActionCallback,
				this );
	}

	if ( !m_horizontalRanger )
		m_horizontalRanger = new c3_Ranger(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_HORIZONTAL,
			patternFilename,
			ListBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_horizontalRanger,errcode) );
	if ( !AUI_NEWOK(m_horizontalRanger,errcode) )
		return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_horizontalRanger );

    sint32 maxRangerSize = 
        std::max(m_verticalRanger->Width(), m_horizontalRanger->Height());

    if (maxRangerSize)
		SetRangerSize( maxRangerSize ); 
	else
		RepositionRangers();

	CalculateDimensions();
	RepositionHeaderSwitches();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE TextBox::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	


	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

#define k_VERTICALBORDER 5
#define k_HORIZONTALBORDER 5


AUI_ERRCODE TextBox::RepositionItems( void )
{
	Assert( m_pane );
	if ( !m_pane ) return AUI_ERRCODE_INVALIDPARAM;

	sint32 minVertical = m_verticalRanger->GetValueY();
	sint32 maxVertical = 
        std::min<sint32>(m_numRows, minVertical + m_itemsPerHeight);

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = 0; i < m_numRows; i++ )
	{
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );
		if ( minVertical <= i && i < maxVertical )
		{
			if ( !IsHidden() ) item->Show();
			item->Move( k_VERTICALBORDER, ( i - minVertical ) * m_maxItemHeight );

			
			if ( m_widthList->L() ) {
				item->Resize( m_widthList->GetHead()-k_HORIZONTALBORDER*2, item->Height());
			}

			sint32 minHorizontal =
				m_horizontalRanger->GetValueX();
			sint32 maxHorizontal =
				minHorizontal + ItemsPerWidth( minHorizontal );

			sint32 x = 0;

			if ( minHorizontal > 0 )
				item->HideThis();
			else
				x = ColumnWidth( 0 );

			ListPos subPosition = item->ChildList()->GetHeadPosition();
			for ( sint32 j = 1; j < m_numColumns; j++ )
			{
				
				if ( !subPosition ) break;

				aui_Item *subItem =
					(aui_Item *)item->ChildList()->GetNext( subPosition );
				if ( minHorizontal <= j && j < maxHorizontal )
				{
					subItem->Move( x, 0 );

					
					ListPos widthPosition = m_widthList->FindIndex( j );
					if ( widthPosition )
						subItem->Resize(
							m_widthList->GetAt( widthPosition ),
							subItem->Height() );

					x += ColumnWidth( j );
				}
				else
					subItem->Hide();
			}
		}
		else
		{
			item->Hide();
		}
	}

	m_draw = TRUE;

	return AUI_ERRCODE_OK;
}
