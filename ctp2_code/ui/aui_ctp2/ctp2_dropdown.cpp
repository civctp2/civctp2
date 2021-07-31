//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : CTP2 drop down menu
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
// -None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - ForceSelect can now be disabled. (Feb 4th 2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_ranger.h"

#include "c3ui.h"
#include "c3_button.h"
#include "c3_static.h"
#include "ctp2_listitem.h"
#include "ctp2_listbox.h"

#include "pattern.h"
#include "pixelutils.h"
#include "primitives.h"

#include "ctp2_dropdown.h"
#include <string>

extern C3UI		*g_c3ui;

ctp2_DropDown::ctp2_DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	sint32 buttonSize,
	sint32 windowSize,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_DropDown(),
	PatternBase(pattern)
{
	*retval = aui_Region::InitCommon(id, x, y, width, height);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon(ActionFunc, cookie);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon( (const MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_DropDown::InitCommon(buttonSize, windowSize);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents(NULL);
	Assert( AUI_SUCCESS(*retval) );
}

ctp2_DropDown::ctp2_DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_DropDown(),
	PatternBase(ldlBlock, NULL)
{
	*retval = aui_Region::InitCommonLdl(id, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl(ldlBlock, ActionFunc, cookie);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_DropDown::InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
}


AUI_ERRCODE ctp2_DropDown::CreateComponents( const MBCHAR *ldlBlock )
{
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	const MBCHAR *    pattern = (m_pattern) ? m_pattern->GetFilename() : NULL;

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_STATICPANE );

		if (aui_Ldl::FindDataBlock( block ) )
		{
			m_staticPane = new c3_Static(
				&errcode,
				aui_UniqueId(),
				block );

			m_staticPane->SetBlindness(TRUE);

			Assert( AUI_NEWOK(m_staticPane,errcode) );
			if ( !AUI_NEWOK(m_staticPane,errcode) ) return errcode;

			AddChild(m_staticPane);
		}

		sprintf(block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_BUTTON );
		aui_Ldl::BuildHierarchyFromRoot(std::string(ldlBlock).append(".autobutton").c_str());
		m_button = static_cast<aui_Button*>(aui_Ldl::GetObject(ldlBlock, "autobutton"));

		if (m_button)
		{
			m_button->SetActionFuncAndCookie(DropDownButtonActionCallback, this);
			m_buttonSize = 0;
		}
		else if (aui_Ldl::FindDataBlock(block))
		{
			m_button = new aui_Button(
				&errcode,
				aui_UniqueId(),
				block,
				DropDownButtonActionCallback, this );

			if ( m_button ) {
				m_buttonSize = 0;
				AddChild( m_button );
			}
		}
	}

	if ( !m_button )

		m_button = new aui_Button(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			DropDownButtonActionCallback, this );

	Assert( AUI_NEWOK(m_button,errcode) );
	if ( !AUI_NEWOK(m_button,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_WINDOW );

		if (aui_Ldl::FindDataBlock( block ) ) {
			m_listBoxWindow = new aui_Window(
				&errcode,
				aui_UniqueId(),
				block,
				16, AUI_WINDOW_TYPE_POPUP );

			aui_Ldl::Remove( m_listBoxWindow );

			if ( m_listBoxWindow )
			{
				m_windowSize = m_listBoxWindow->Height();
				m_listBoxWindow->Resize( m_width + m_buttonSize, m_windowSize );
			}

			sprintf( block, "%s.%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_WINDOW, k_AUI_DROPDOWN_LDL_LISTBOX );

			if (aui_Ldl::FindDataBlock( block ) ) {
				m_listBox = new ctp2_ListBox(
						&errcode,
						aui_UniqueId(),
						block,
						DropDownListBoxActionCallback, this);
			}

			if(m_listBoxWindow && m_listBox)
				m_listBoxWindow->AddChild( m_listBox );
		}
	}

	if ( !m_listBoxWindow ) {
		m_listBoxWindow = new aui_Window(
			&errcode,
			aui_UniqueId(),
			0, 0, m_width + m_buttonSize, m_windowSize,
			16, AUI_WINDOW_TYPE_POPUP );

		if(m_listBoxWindow && m_listBox)
			m_listBoxWindow->AddChild( m_listBox );
	}

	Assert( AUI_NEWOK(m_listBoxWindow,errcode) );
	if ( !AUI_NEWOK(m_listBoxWindow,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	if ( !m_listBox ) {
		m_listBox = new ctp2_ListBox(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			pattern, 0, 0,
			DropDownListBoxActionCallback, this );

		if(m_listBoxWindow && m_listBox)
			m_listBoxWindow->AddChild( m_listBox );
	}

	Assert( AUI_NEWOK(m_listBox,errcode) );
	if ( !AUI_NEWOK(m_listBox,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_listBox->SetForceSelect( TRUE );

	RepositionButton();
	RepositionListBoxWindow();

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_DropDown::RepositionListBoxWindow( void )
{
	if ( m_window )
	{
		POINT screenLocation = { m_x, m_y + m_height };
		if(m_inverted) {
			screenLocation.y = m_y - m_listBoxWindow->Height();
		}
		ToScreen(&screenLocation);

		m_listBoxWindow->Move( screenLocation.x, screenLocation.y );
	}

	sint32 listBoxWidth = m_listBoxWindow->Width();
	if ( m_listBox->GetVerticalRanger()->GetMaximumY() )
		listBoxWidth -= m_listBox->GetRangerSize();

	ctp2_ListBox	*myListBox = (ctp2_ListBox *)m_listBox;

	myListBox->Resize( listBoxWidth - (myListBox->GetBevelWidth() * 2),
						m_windowSize - (myListBox->GetBevelWidth() * 2));
	myListBox->Move( myListBox->GetBevelWidth(), myListBox->GetBevelWidth() );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_DropDown::Draw(aui_Surface *surface, sint32 x, sint32 y)
{
	AUI_ERRCODE errcode = DrawChildren( surface, x, y );

	if(!IsHidden() && (errcode == AUI_ERRCODE_OK))
		DrawThis( surface, x, y );

	m_draw = 0;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_DropDown::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawSelectedItem( surface, 2, 2 );

	if (m_window)
		if ( surface == m_window->TheSurface() )
			m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_DropDown::DoneInstantiatingThis(const MBCHAR * ldlBlock)
{
	aui_DropDown::DoneInstantiatingThis(ldlBlock);
	m_listBoxWindow->DoneInstantiating();
	RepositionListBoxWindow();
}

AUI_ERRCODE ctp2_DropDown::RepositionButton( void )
{
	m_button->Resize( m_width, m_button->Height() );

	return AUI_ERRCODE_OK;
}

void ctp2_DropDown::Clear(void)
{
	((ctp2_ListBox *)m_listBox)->Clear();
}

AUI_ERRCODE ctp2_DropDown::AddItem(ctp2_ListItem *item)
{
	Assert(m_listBox);
	if (m_listBox == NULL) return AUI_ERRCODE_INVALIDPARAM;

	m_listBox->AddItem((aui_Item *)item);

	SetWindowSizeThis();
	return AUI_ERRCODE_OK;
}

void ctp2_DropDown::BuildListStart(void)
{
	Assert(m_listBox);
	if (m_listBox == NULL) return;

	m_listBox->BuildListStart();
}

void ctp2_DropDown::BuildListEnd(void)
{
	Assert(m_listBox);
	if (m_listBox == NULL) return;

	m_listBox->BuildListEnd();
	SetWindowSizeThis();
}

void ctp2_DropDown::SetForceSelect(bool forceSelect)
{
	m_listBox->SetForceSelect(forceSelect);
}

sint32 ctp2_DropDown::SetWindowSizeThis()
{
	ctp2_ListBox * myListBox = (ctp2_ListBox *) m_listBox;

	sint32 borderHeight = 2 * myListBox->GetBevelWidth() + 2 * myListBox->GetBorderOffset().y;
	sint32 maxHeight = borderHeight + ((350 - borderHeight) / myListBox->GetMaxItemHeight()) * myListBox->GetMaxItemHeight();

	sint32 height = myListBox->GetMaxItemHeight() * m_listBox->NumItems() + borderHeight;

	return SetWindowSize(height < maxHeight ? height : maxHeight);
}
