//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 hyper text box
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_window.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_control.h"
#include "aui_action.h"

#include "c3ui.h"
#include "c3_static.h"
#include "c3_ranger.h"
#include "pattern.h"
#include "patternbase.h"
#include "pixelutils.h"
#include "primitives.h"
#include "c3_hypertextbox.h"
#include "SlicEngine.h"
#include "colorset.h"
#include "ctp2_listbox.h"


#include "greatlibrary.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;

#define k_C3_HYPERTEXTBOX_BEVELWIDTH	2
#define k_C3_HYPERTEXTBOX_INSETWIDTH	5

extern C3UI *g_c3ui;


c3_HyperTextBox::c3_HyperTextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie,
	bool draw_frame
	)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_HyperTextBox(),
	PatternBase(ldlBlock, NULL)
{
	
	m_draw_frame = draw_frame;
	m_bevelWidth = k_C3_HYPERTEXTBOX_BEVELWIDTH;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Region::InitCommonLdl(id, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl(ldlBlock, ActionFunc, cookie);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_HyperTextBase::InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_HyperTextBox::InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRanger( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_HyperTextBox::c3_HyperTextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_HyperTextBox(retval, id, x,y, width, height, ActionFunc, cookie),
	PatternBase(pattern)
{
	
	m_draw_frame = false;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRanger( NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_HyperTextBox::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if(block->GetAttribute(k_CTP2_LISTBOX_LDL_BEVELWIDTH)) {
		m_bevelWidth = block->GetInt(k_CTP2_LISTBOX_LDL_BEVELWIDTH);
	} else {
		m_bevelWidth = k_C3_HYPERTEXTBOX_BEVELWIDTH;
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_HyperTextBox::InitCommon( void )
{
	m_hyperLinkList = new tech_WLList<c3_HyperLink *>;
	Assert( m_hyperLinkList != NULL );
	if ( !m_hyperLinkList ) return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}


c3_HyperTextBox::~c3_HyperTextBox()
{
	if (m_hyperLinkList) 
    {
		RemoveHyperLinks();
		delete m_hyperLinkList;
	}
}



AUI_ERRCODE c3_HyperTextBox::CreateRanger( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_HYPERTEXTBOX_LDL_RANGERY );

		
        if (aui_Ldl::FindDataBlock( block ) )
			m_ranger = new c3_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				HyperTextBoxRangerActionCallback,
				this );
	}

    MBCHAR * pattern = (m_pattern) ? m_pattern->GetFilename() : NULL;

	if ( !m_ranger )
		m_ranger = new c3_Ranger(
			&errcode,
			aui_UniqueId(),
			m_width,
			k_CONTROL_DEFAULT_SIZE,
			m_height - 2 * k_CONTROL_DEFAULT_SIZE,
			k_CONTROL_DEFAULT_SIZE,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_VERTICAL,
			pattern,
			HyperTextBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_ranger,errcode) );
	if ( !AUI_NEWOK(m_ranger,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_ranger );

	SetRangerSize(m_ranger->Width());
	return RepositionRanger();
}


AUI_ERRCODE c3_HyperTextBox::AddHyperStatics( const MBCHAR *hyperText )
{
	if (!hyperText)
	{
		RemoveHyperStatics();
		RemoveHyperLinks();
		m_curStaticPos.x = 0;
		m_curStaticPos.y = 0;
		m_virtualHeight = 0;
		hyperText = m_hyperText;
	}

	uint32 len = strlen( hyperText );
	if ( !len ) return AUI_ERRCODE_OK;

	const MBCHAR *ptr = hyperText;
	const MBCHAR *stop = ptr + len;

	sint32      adjWidth        = m_width;
	sint32      hyperLinkDB     = 0;
	sint32      hyperLinkIndex  = 0;
	bool        isHyperLink     = false;
	COLORREF    oldColor        = 0;
	sint32      oldUnderline    = 0;

	while ( ptr != stop )
	{
		if ( *ptr == '<' )
		{
			switch ( *++ptr )
			{
			case 't':
				sscanf( ++ptr, ":%[^>]", m_hyperTtffile );
				break;

			case 'p':
				sscanf( ++ptr, ":%d>", &m_hyperPointSize );
				break;

			case 'b':
				sscanf( ++ptr, ":%d>", &m_hyperBold );
				break;

			case 'i':
				sscanf( ++ptr, ":%d>", &m_hyperItalic );
				break;

			case 'c':
			{
				uint8 r, g, b;
				sscanf( ++ptr, ":%u,%u,%u>", &r, &g, &b );
				m_hyperColorOld = m_hyperColor;
				m_hyperColor = RGB(r,g,b);
			}
			break;

			
			case 's':
				sscanf( ++ptr, ":%d>", &m_hyperShadow );
				break;

			
			case 'h':
			{
				uint8 r, g, b;
				sscanf( ++ptr, ":%u,%u,%u>", &r, &g, &b );
				m_hyperShadowColor = RGB(r,g,b);
			}
			break;

			case 'u':
				sscanf( ++ptr, ":%d>", &m_hyperUnderline );
				break;

			
			case 'l':
				sscanf( ++ptr, ":%d,%d>", &hyperLinkDB, &hyperLinkIndex );
				
				
				
				
				oldUnderline = m_hyperUnderline;
				oldColor = m_hyperColor;
				m_hyperColorOld = m_hyperColor;

				m_hyperUnderline = 1;
				m_hyperColor = RGB(0,0,100);
				isHyperLink = true;
				break;

			
			case 'L':
				{

				
				char hyperLinkDB_name[255];
				char hyperLinkIndex_name[255];

				
				char * copy_to = hyperLinkDB_name;

				
				ptr++;

				
				ptr++;

				
				while (!isalnum(*ptr))
					ptr++;

				
				while (isalnum(*ptr) || (*ptr == '_'))
				{
					*copy_to++ = *ptr++;
				}
				*copy_to = 0;

				
				while (!isalnum(*ptr))
					ptr++;

				
				copy_to = hyperLinkIndex_name;

				
				while (isalnum(*ptr) || (*ptr == '_'))
				{
					*copy_to++ = *ptr++;
				}
				*copy_to = 0;

				
				while (*ptr != '>' && *ptr)
					ptr++;

				




				


				
				hyperLinkDB = GreatLibrary::Get_Database_From_Name(hyperLinkDB_name);
				hyperLinkIndex = 
					GreatLibrary::Get_Object_Index_From_Name(hyperLinkDB, hyperLinkIndex_name);
		

				
				
				
				
				oldUnderline = m_hyperUnderline;
				oldColor = m_hyperColor;
				m_hyperColorOld = m_hyperColor;

				m_hyperUnderline = 1;
				m_hyperColor = RGB(0,0,100);
				isHyperLink = TRUE;
				break;
				}

			case 'e':
				m_hyperColor = m_hyperColorOld;
				break;

			default:
				
				Assert( FALSE );
				return AUI_ERRCODE_INVALIDPARAM;
			}

			
			while ( *ptr != '>' && *ptr)
				ptr++;

			
			ptr++;
		}
		else
		{
			
			
			

			
			
			
			const MBCHAR *nextCmd = strchr( ptr, '<' );
			len = nextCmd ? nextCmd - ptr : strlen( ptr );

			
			
			
			
			
			const MBCHAR *nextLine = strchr( ptr, '\n' );
			if ( nextLine && ( !nextCmd || nextLine < nextCmd ) )
				len = nextLine - ptr + 1;

			
			
			
			const MBCHAR *subStop = ptr + len;

			
			while ( ptr != subStop )
			{
				
				
				
				
#if 0
				
				if (*ptr == '\n' && len == 1) {
					ptr++;
					
					
					
					m_curStaticPos.x = 0;
					sint32 offset = m_virtualHeight - m_curStaticPos.y;
					m_curStaticPos.y += offset;
					m_virtualHeight += offset;
					continue;
				}
#endif

				
				aui_Static *hs = CreateHyperStatic(
					ptr,
					len,
					m_hyperTtffile,
					m_hyperPointSize,
					m_hyperBold,
					m_hyperItalic,
					m_hyperColor,
					m_hyperUnderline,
					m_hyperShadow,
					m_hyperShadowColor,
					m_hyperFlags );
				Assert( hs != NULL );
				if ( !hs ) return AUI_ERRCODE_MEMALLOCFAILED;

				
				MBCHAR *cReturn = strrchr( hs->GetText(), '\r' );
				if ( cReturn ) {
					*cReturn = ' ';
				}

				
				
				m_hyperStaticList->AddTail( hs );

				
				
				
				sint32 nextX = m_curStaticPos.x + hs->Width();
				sint32 nextY = m_curStaticPos.y + hs->Height();

				
				if ( nextX > adjWidth )
				{
					RECT wrap =
					{
						0,
						0,
						adjWidth - m_curStaticPos.x,
						0
					};

					POINT penPos = { 0, 0 };

					const MBCHAR *start = ptr;
					hs->GetTextFont()->GetLineInfo(
						&wrap,
						&penPos,
						NULL,
						NULL,
						&ptr,
						subStop );

					if ( ptr == subStop )
					{
						hs->Move(
							m_curStaticPos.x = 0,
							m_curStaticPos.y = m_virtualHeight );

						m_virtualHeight += hs->Height();

						if ( hs->Width() > adjWidth  )
							m_curStaticPos.y = m_virtualHeight;
						else
							m_curStaticPos.x = hs->Width();
					}
					else
					{
						uint32 truncLen = ptr - start;

						penPos.x = penPos.y = 0;
						const MBCHAR *testPtr = start;
						const MBCHAR *testSubStop = testPtr + truncLen;
						hs->GetTextFont()->GetLineInfo(
							&wrap,
							&penPos,
							NULL,
							NULL,
							&testPtr,
							testSubStop );

						if ( penPos.x > wrap.right )
						{
							hs->Move(
								m_curStaticPos.x = 0,
								m_curStaticPos.y = m_virtualHeight );

							m_virtualHeight += hs->Height();

							if ( hs->Width() > adjWidth  )
								m_curStaticPos.y = m_virtualHeight;
							else
								m_curStaticPos.x = hs->Width();

							ptr = subStop;
						}
						else
						{
							len -= truncLen;
							hs->SetText( start, truncLen );

							hs->Move( m_curStaticPos.x, m_curStaticPos.y );
							hs->Resize( penPos.x, hs->Height() );

							if ( nextY > m_virtualHeight )
								m_virtualHeight = nextY;

							m_curStaticPos.x = 0;
							m_curStaticPos.y = m_virtualHeight;
						}
					}
				}
				else
				{
					
					
					ptr = subStop;

					
					
					hs->Move( m_curStaticPos.x, m_curStaticPos.y );

					m_curStaticPos.x = nextX;

					if ( nextY > m_virtualHeight )
						m_virtualHeight = nextY;
				}

				
				
				if ( *(ptr - 1) == '\n' )
				{
					m_curStaticPos.x = 0;
					m_curStaticPos.y = m_virtualHeight;
				} 

				
				

				
				
				if ( m_hyperStaticList->L() > k_AUI_HYPERTEXTBOX_LDL_MAXSTATICS )
				{
					delete m_hyperStaticList->RemoveHead();

					sint32 topY = m_hyperStaticList->GetHead()->Y();
					ListPos pos = m_hyperStaticList->GetHeadPosition();
					for ( sint32 i = m_hyperStaticList->L()-1; i; i-- )
						m_hyperStaticList->GetNext( pos )->Offset( 0, -topY );
					
					m_virtualHeight -= topY;
					m_curStaticPos.y -= topY;
				}

				
				if ( isHyperLink ) {
					c3_HyperLink * hl = new c3_HyperLink;

					hl->m_static = hs;
					hl->m_db = hyperLinkDB;
					hl->m_index = hyperLinkIndex;
					hl->m_frame = FALSE;
					hl->m_oldColor = m_hyperColor;
					hl->m_selectColor = RGB(0,0,255);

					m_hyperLinkList->AddTail( hl );
				}
			}
			
			if ( isHyperLink ) {
				isHyperLink = FALSE;
				m_hyperUnderline = oldUnderline;
				m_hyperColor = oldColor;
			}
		}
	}

	return AUI_ERRCODE_OK;
}




void c3_HyperTextBox::RemoveHyperLinks( void )
{
	for (sint32 i = m_hyperLinkList->L(); i; --i)
    {
		delete m_hyperLinkList->RemoveTail();
    }
    m_selectedHyperLink = NULL;
}


AUI_ERRCODE c3_HyperTextBox::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	rect.left -= k_C3_HYPERTEXTBOX_INSETWIDTH;
	rect.top -= k_C3_HYPERTEXTBOX_INSETWIDTH;
	rect.bottom += k_C3_HYPERTEXTBOX_INSETWIDTH;

	if ( m_pattern ) {
		if ( m_srcWidthPix || m_srcHeightPix ) {
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &rect, &srcRect );
		}
		else {
			m_pattern->Draw( surface, &rect );
		}
		if(m_bevelWidth > 0) {
			primitives_BevelRect16( surface, &rect, m_bevelWidth, 1, 16, 16);
		}

		


	}




	DrawThisStateImage(
		0,
		surface,
		&rect );

	
	rect.left += k_C3_HYPERTEXTBOX_INSETWIDTH;
	rect.top += k_C3_HYPERTEXTBOX_INSETWIDTH;
	rect.bottom -= k_C3_HYPERTEXTBOX_INSETWIDTH;

	DrawThisHyperText(
		surface,
		&rect,
		0,
		-m_ranger->GetValueY() );
















	
	rect.left -= k_C3_HYPERTEXTBOX_INSETWIDTH;
	rect.top -= k_C3_HYPERTEXTBOX_INSETWIDTH;
	rect.bottom += k_C3_HYPERTEXTBOX_INSETWIDTH;




	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


void c3_HyperTextBox::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == this )
		{
			
			ReleaseMouseOwnership();

			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			
			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

			


			
			
			
			m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;

			POINT pos;
			pos.x = mouseData->position.x - m_x;
			pos.y = mouseData->position.y - m_y;

			
			pos.y += m_ranger->GetValueY();

			
			m_selectedHyperLink = NULL;

			ListPos lp = m_hyperLinkList->GetHeadPosition();
			for ( uint32 i = 0;i < m_hyperLinkList->L();i++ ) {
				c3_HyperLink *hl = m_hyperLinkList->GetNext( lp );
				RECT rect;
				rect.left = hl->m_static->X();
				rect.top = hl->m_static->Y();
				rect.right = rect.left + hl->m_static->Width();
				rect.bottom = rect.top + hl->m_static->Height();

				if ( PtInRect(&rect, pos) ) {
					hl->m_frame = TRUE;
					m_selectedHyperLink = hl;
				}
				else {
					hl->m_frame = FALSE;
					hl->m_static->SetTextColor( hl->m_oldColor );
				}
			}

			if ( m_selectedHyperLink ) {

				
				m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;

				

				if ( !HandleGameSpecificLeftClick( this ) )
				if ( m_ActionFunc )
					m_ActionFunc( this, C3_HYPERLINK_ACTION_EXECUTE, 0, m_cookie );
				else if ( m_action )
					m_action->Execute( this, C3_HYPERLINK_ACTION_EXECUTE, 0 );
			}
			else {
				m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
			}
		}
		else
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		}
	}
	else
		MouseLDropOutside( mouseData );
}


void c3_HyperTextBox::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	m_selectedHyperLink = NULL;

	ListPos lp = m_hyperLinkList->GetHeadPosition();
	for ( uint32 i = 0;i < m_hyperLinkList->L();i++ ) {
		c3_HyperLink *hl = m_hyperLinkList->GetNext( lp );

		hl->m_frame = FALSE;
		hl->m_static->SetTextColor( hl->m_oldColor );
	}

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;
}


void c3_HyperTextBox::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		
		HideTipWindow();

		
		SetMouseOwnership();
		SetKeyboardFocus();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		
		m_attributes |= k_CONTROL_ATTRIBUTE_DOWN;
		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;

		POINT pos;
		pos.x = mouseData->position.x - m_x;
		pos.y = mouseData->position.y - m_y;

		
		pos.y += m_ranger->GetValueY();

		
		m_selectedHyperLink = NULL;

		ListPos lp = m_hyperLinkList->GetHeadPosition();
		for ( uint32 i = 0;i < m_hyperLinkList->L();i++ ) {
			c3_HyperLink *hl = m_hyperLinkList->GetNext( lp );
			RECT rect;
			rect.left = hl->m_static->X();
			rect.top = hl->m_static->Y();
			rect.right = rect.left + hl->m_static->Width();
			rect.bottom = rect.top + hl->m_static->Height();

			if ( PtInRect(&rect, pos) ) {
				hl->m_frame = TRUE;
				hl->m_static->SetTextColor( hl->m_selectColor );
				m_selectedHyperLink = hl;
			}
			else {
				hl->m_frame = FALSE;
				hl->m_static->SetTextColor( hl->m_oldColor );
			}
		}
		







	}
	else
		MouseLGrabOutside( mouseData );
}




