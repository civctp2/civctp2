//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : User interface hypertext link box
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
// - Improved robustness.
// - Interpret a zero-length link text as display of the text from gl_str.txt.
//   Example: <L:DATABASE_UNITS,UNIT_STEALTH_BOMBER><e> will display a
//            "Stealth Bomber" hyperlink when using the English version.
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
#include "ctp2_hypertextbox.h"
#include "SlicEngine.h"
#include "colorset.h"

#include "ctp2_listbox.h"
#include "greatlibrary.h"
#include "screenutils.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;
extern ColorSet		*g_colorSet;

#define k_CTP2_HYPERTEXTBOX_BEVELWIDTH	2
#define k_CTP2_HYPERTEXTBOX_INSETWIDTH	5

extern C3UI *g_c3ui;

#include <algorithm>

namespace
{

size_t const	MAX_SIZE_IDENTIFIER	= 255;

//----------------------------------------------------------------------------
//
// Name       : IsIdentifierBegin
//
// Description: Test whether a character may start an identifier.
//
// Parameters : a_Char		: character under test
//
// Globals    : -
//
// Returns    : bool		: character may start an identifier
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool IsIdentifierBegin(MBCHAR a_Char)
{
	return isalnum(a_Char);
}

//----------------------------------------------------------------------------
//
// Name       : IsNotInIdentifier
//
// Description: Test whether a character may belong to an identifier.
//
// Parameters : a_Char		: character under test
//
// Globals    : -
//
// Returns    : bool		: character does *not* belong to an identifier
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool IsNotInIdentifier(MBCHAR a_Char)
{
	return ('_' != a_Char) && !isalnum(a_Char);
}

} // namespace

//----------------------------------------------------------------------------
//
// Name       : ctp2_HyperTextBox::FormatText
//
// Description: Format a text by wrapping it into a text box.
//
// Parameters : a_Text		: start of text to add
//				a_TextEnd	: (one past) end of text to add
//				a_Database	: link to database
//				a_Index		: link to index within database
//				a_IsLink	: text is a hyperlink
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void ctp2_HyperTextBox::FormatText
(
	MBCHAR const *	a_Text,
	MBCHAR const *	a_TextEnd,
	sint32			a_Database,
	sint32			a_Index,
	bool			a_IsLink
)
{
	for (MBCHAR const * ptr = a_Text; ptr < a_TextEnd; )
	{
		aui_Static *	hs		= 
			CreateHyperStatic(ptr,
							  a_TextEnd - ptr,	// may be corrected later
							  m_hyperTtffile,
							  m_hyperPointSize,
							  m_hyperBold,
							  m_hyperItalic,
							  m_hyperColor,
							  m_hyperUnderline,
							  m_hyperShadow,
							  m_hyperShadowColor,
							  m_hyperFlags 
							 );
		Assert(hs);
		if (!hs) return;

		MBCHAR *		cReturn	= strrchr(hs->GetText(), '\r');
		if (cReturn) 
		{
			*cReturn = ' ';
		}

		m_hyperStaticList->AddTail(hs);

		sint32	nextX	= m_curStaticPos.x + hs->Width();
		sint32	nextY	= m_curStaticPos.y + hs->Height();

		if (nextX <= m_width)
		{
			// The whole text will fit on the current line.
			hs->Move(m_curStaticPos.x, m_curStaticPos.y);
			
			m_curStaticPos.x = nextX;
			if (nextY > m_virtualHeight)
			{
				m_virtualHeight = nextY;
			}

			ptr = a_TextEnd;
		}
		else
		{
			RECT			wrap	= { 0, 0, m_width - m_curStaticPos.x, 0 };
			POINT			penPos	= { 0, 0 };
			MBCHAR const *	start	= ptr;
			
			hs->GetTextFont()->GetLineInfo
				(&wrap, &penPos, NULL, NULL, &ptr, a_TextEnd);
			
			if (ptr == a_TextEnd)
			{
				// Not even the first word will fit on the current line.
				hs->Move(m_curStaticPos.x = 0,
						 m_curStaticPos.y = m_virtualHeight 
						);

				m_virtualHeight += hs->Height();

				if (hs->Width() > m_width)
				{
					m_curStaticPos.y = m_virtualHeight;
				}
				else
				{
					m_curStaticPos.x = hs->Width();
				}
			}
			else
			{
				// The section from start to ptr may fit on the current line.
				MBCHAR const *	testPtr		= start;
				MBCHAR const *	testSubStop = ptr;

				penPos.x = 0;
				penPos.y = 0;
				hs->GetTextFont()->GetLineInfo
					(&wrap,	&penPos, NULL, NULL, &testPtr, testSubStop);

				if (penPos.x > wrap.right)
				{
					// Move everything to the next line.
					hs->Move(m_curStaticPos.x = 0,
							 m_curStaticPos.y = m_virtualHeight 
							);

					m_virtualHeight += hs->Height();

					if (hs->Width() > m_width)
					{
						m_curStaticPos.y = m_virtualHeight;
					}
					else
					{
						m_curStaticPos.x = hs->Width();
					}

					ptr = a_TextEnd;
				}
				else
				{
					// Put a part on this line, and continue on the next line.
					hs->SetText(start, ptr - start);
					hs->Move(m_curStaticPos.x, m_curStaticPos.y);
					hs->Resize(penPos.x, hs->Height());

					if (nextY > m_virtualHeight)
					{
						m_virtualHeight = nextY;
					}
	
					m_curStaticPos.x = 0;
					m_curStaticPos.y = m_virtualHeight;
				}
			}
		}
		
		// Check for line breaks		
		if (*(ptr - 1) == '\n')
		{
			m_curStaticPos.x = 0;
			m_curStaticPos.y = m_virtualHeight;
		} 

		// Removed the arbitrary limit of k_AUI_HYPERTEXTBOX_LDL_MAXSTATICS
		// (= 100) hyperstatic list items, because it resulted in a mismatch
		// between the hyperstatic and hyperlink lists, causing a crash later.

		if (a_IsLink)
		{
			ctp2_HyperLink * hl	= new ctp2_HyperLink;
			hl->m_static		= hs;
			hl->m_db			= a_Database;
			hl->m_index			= a_Index;
			hl->m_frame			= FALSE;
			hl->m_oldColor		= m_hyperColor;
			hl->m_selectColor	= RGB(0,0,255);
			m_hyperLinkList->AddTail(hl);
		}
	} // for
}


ctp2_HyperTextBox::ctp2_HyperTextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:

	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_HyperTextBox(),
	PatternBase(ldlBlock, NULL)
{



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



ctp2_HyperTextBox::ctp2_HyperTextBox(
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
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRanger( NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE ctp2_HyperTextBox::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if(block->GetAttribute(k_CTP2_LISTBOX_LDL_BEVELWIDTH)) {
		m_bevelWidth = block->GetInt(k_CTP2_LISTBOX_LDL_BEVELWIDTH);
	} else {
		m_bevelWidth = k_CTP2_HYPERTEXTBOX_BEVELWIDTH;
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ctp2_HyperTextBox::InitCommon( void )
{
	m_hyperLinkList = new tech_WLList<ctp2_HyperLink *>;
	Assert( m_hyperLinkList != NULL );
	if ( !m_hyperLinkList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_hyperColor = RGB(50,50,50);

	return AUI_ERRCODE_OK;
}


ctp2_HyperTextBox::~ctp2_HyperTextBox()
{
	if ( m_hyperLinkList ) {
		RemoveHyperLinks();

		delete m_hyperLinkList;
		m_hyperLinkList = NULL;
	}
}



AUI_ERRCODE ctp2_HyperTextBox::CreateRanger( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	aui_Ldl *theLdl = g_c3ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_HYPERTEXTBOX_LDL_RANGERY );

		
		if ( theLdl->GetLdl()->FindDataBlock( block ) )
			m_ranger = new c3_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				HyperTextBoxRangerActionCallback,
				this );
	}

	MBCHAR *pattern;

	if (m_pattern)
		pattern = m_pattern->GetFilename();

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


AUI_ERRCODE ctp2_HyperTextBox::AddHyperStatics( const MBCHAR *hyperText )
{
	if ( !hyperText )
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



	sint32 hyperLinkDB = 0;
	sint32 hyperLinkIndex = 0;
	sint32 isHyperLink = FALSE;
	COLORREF oldColor;
	sint32 oldUnderline;

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
				isHyperLink = TRUE;
				break;

			
			
			case 'L':
				{

				
				MBCHAR			hyperLinkDB_name[GL_MAX_DB_NAME_SIZE];
				MBCHAR			hyperLinkIndex_name[MAX_SIZE_IDENTIFIER + 1];
				MBCHAR const *	copyBegin;
				MBCHAR const *	copyEnd;
				MBCHAR const *	copySafeEnd; 
				
				copyBegin	= std::find_if(ptr + 2, stop, IsIdentifierBegin);
				copyEnd		= std::find_if(copyBegin, stop, IsNotInIdentifier);
				copySafeEnd	= std::min(copyEnd, copyBegin + GL_MAX_DB_NAME_SIZE);

				std::copy(copyBegin, copySafeEnd, hyperLinkDB_name);
				std::fill(hyperLinkDB_name + (copySafeEnd - copyBegin),
						  hyperLinkDB_name + GL_MAX_DB_NAME_SIZE,
						  static_cast<MBCHAR const>(0)
						 );

				copyBegin	= std::find_if(copyEnd, stop, IsIdentifierBegin);
				copyEnd		= std::find_if(copyBegin, stop, IsNotInIdentifier);
				copySafeEnd	= std::min(copyEnd, copyBegin + MAX_SIZE_IDENTIFIER);

				std::copy(copyBegin, copySafeEnd, hyperLinkIndex_name);
				std::fill(hyperLinkIndex_name + (copySafeEnd - copyBegin),
						  hyperLinkIndex_name + MAX_SIZE_IDENTIFIER + 1,
						  static_cast<MBCHAR const>(0)
						 );

				ptr			= std::find(copyEnd, stop, '>');




				


				
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
				if (isHyperLink)
				{
					// <L> or <l> directly followed by <e>.
					// Perform database text lookup.
					if (hyperLinkDB)
					{
						MBCHAR const *	ptr	= g_greatLibrary->GetItemName
												(hyperLinkDB, hyperLinkIndex);
						
						FormatText(ptr, 
							       ptr + strlen(ptr), 
								   hyperLinkDB, 
								   hyperLinkIndex, 
								   true
								  );

					}

					m_hyperUnderline	= oldUnderline;
					isHyperLink			= FALSE;
				}
				m_hyperColor = m_hyperColorOld;
				break;

			default:
				
				Assert( FALSE );
				return AUI_ERRCODE_INVALIDPARAM;
			}

			ptr = std::find(ptr, stop, '>');
			if (ptr != stop)
			{
				++ptr;
			}
		}
		else
		{

			MBCHAR const *  nextLine	= std::find(ptr, stop, '\n');
			MBCHAR const *	nextStop	= std::find(ptr, nextLine, '<');

			if ((nextStop != stop) && (*nextStop == '\n'))
			{
				// Include the end of line marker in the string
				++nextStop;
			}

			FormatText(ptr, nextStop, hyperLinkDB, hyperLinkIndex, isHyperLink);
			ptr	= nextStop;

			
			if ( isHyperLink ) {
				isHyperLink = FALSE;
				m_hyperUnderline = oldUnderline;
				m_hyperColor = oldColor;
			}
		}
	}

	return AUI_ERRCODE_OK;
}




void ctp2_HyperTextBox::RemoveHyperLinks( void )
{
	ListPos position = m_hyperLinkList->GetHeadPosition();
	for ( sint32 i = m_hyperLinkList->L(); i; i-- )
		DestroyHyperLink( m_hyperLinkList->RemoveTail() );
}


void ctp2_HyperTextBox::DestroyHyperLink( ctp2_HyperLink *hl )
{
	Assert( hl != NULL );
	if ( !hl ) return;

	delete hl;
}


AUI_ERRCODE ctp2_HyperTextBox::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	rect.left -= k_CTP2_HYPERTEXTBOX_INSETWIDTH;
	rect.top -= k_CTP2_HYPERTEXTBOX_INSETWIDTH;
	rect.bottom += k_CTP2_HYPERTEXTBOX_INSETWIDTH;

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

	
	rect.left += k_CTP2_HYPERTEXTBOX_INSETWIDTH;
	rect.top += k_CTP2_HYPERTEXTBOX_INSETWIDTH;
	rect.bottom -= k_CTP2_HYPERTEXTBOX_INSETWIDTH;

	DrawThisHyperText(
		surface,
		&rect,
		0,
		-m_ranger->GetValueY() );
















	
	rect.left -= k_CTP2_HYPERTEXTBOX_INSETWIDTH;
	rect.top -= k_CTP2_HYPERTEXTBOX_INSETWIDTH;
	rect.bottom += k_CTP2_HYPERTEXTBOX_INSETWIDTH;




	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


void ctp2_HyperTextBox::MouseLDropInside( aui_MouseEvent *mouseData )
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
				ctp2_HyperLink *hl = m_hyperLinkList->GetNext( lp );
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
					m_ActionFunc( this, CTP2_HYPERLINK_ACTION_EXECUTE, 0, m_cookie );
				else if ( m_action )
					m_action->Execute( this, CTP2_HYPERLINK_ACTION_EXECUTE, 0 );
				else {
					
					open_GreatLibrary();
					if(g_greatLibrary) {
						g_greatLibrary->SetLibrary(m_selectedHyperLink->m_index, (DATABASE)m_selectedHyperLink->m_db);
					}
				}
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


void ctp2_HyperTextBox::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	m_selectedHyperLink = NULL;

	ListPos lp = m_hyperLinkList->GetHeadPosition();
	for ( uint32 i = 0;i < m_hyperLinkList->L();i++ ) {
		ctp2_HyperLink *hl = m_hyperLinkList->GetNext( lp );

		hl->m_frame = FALSE;
		hl->m_static->SetTextColor( hl->m_oldColor );
	}

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;
}


void ctp2_HyperTextBox::MouseLGrabInside( aui_MouseEvent *mouseData )
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
			ctp2_HyperLink *hl = m_hyperLinkList->GetNext( lp );
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




