//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 dropdown menu
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
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"
#include "aui_directsurface.h"
#include "aui_ranger.h"

#include "c3ui.h"
#include "c3_listbox.h"
#include "c3_button.h"
#include "c3_static.h"

#include "pattern.h"
#include "pixelutils.h"
#include "primitives.h"

#include "c3_dropdown.h"

extern C3UI		*g_c3ui;


c3_DropDown::c3_DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
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

	*retval = aui_SoundBase::InitCommon( (MBCHAR **)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_DropDown::InitCommon(buttonSize, windowSize);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents(NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_DropDown::c3_DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
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
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_DropDown::CreateComponents( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

    MBCHAR * pattern = (m_pattern) ? m_pattern->GetFilename() : NULL;

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_STATICPANE );

        if (aui_Ldl::FindDataBlock(block))
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
	}

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_BUTTON );

        if (aui_Ldl::FindDataBlock(block))
		{
			
			m_button = new aui_Button(
				&errcode,
				aui_UniqueId(),
				block,
				DropDownButtonActionCallback, this );

			if ( m_button )
				m_buttonSize = m_button->Width();
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

        if ( aui_Ldl::FindDataBlock(block))
		{
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

            if (aui_Ldl::FindDataBlock(block))
				m_listBox = new c3_ListBox(
					&errcode,
					aui_UniqueId(),
					block,
					DropDownListBoxActionCallback, this );
		}
	}

	if ( !m_listBoxWindow )
		m_listBoxWindow = new aui_Window(
			&errcode,
			aui_UniqueId(),
			0, 0, m_width + m_buttonSize, m_windowSize,


			16, AUI_WINDOW_TYPE_POPUP );

	Assert( AUI_NEWOK(m_listBoxWindow,errcode) );
	if ( !AUI_NEWOK(m_listBoxWindow,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	if ( !m_listBox )
		m_listBox = new c3_ListBox(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			pattern, 0, 0,
			DropDownListBoxActionCallback, this );

	Assert( AUI_NEWOK(m_listBox,errcode) );
	if ( !AUI_NEWOK(m_listBox,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_listBox->SetForceSelect( TRUE );

	AddChild( m_button );

	m_listBoxWindow->AddChild( m_listBox );

	RepositionButton();
	RepositionListBoxWindow();

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_DropDown::RepositionListBoxWindow( void )
{
	

	
	if ( m_window )
	{
		POINT screenLocation = { m_x, m_y + m_height };
		ToScreen( &screenLocation );

		m_listBoxWindow->Move( screenLocation.x, screenLocation.y );
	}

	

	sint32 listBoxWidth = m_listBoxWindow->Width();
	if ( m_listBox->GetVerticalRanger()->GetMaximumY() )
		listBoxWidth -= m_listBox->GetRangerSize();

	c3_ListBox	*myListBox = (c3_ListBox *)m_listBox;

	myListBox->Resize( listBoxWidth - (myListBox->GetBevelWidth() * 2), 
						m_windowSize - (myListBox->GetBevelWidth() * 2) );
	myListBox->Move( myListBox->GetBevelWidth(), myListBox->GetBevelWidth() );


	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_DropDown::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width + m_buttonSize, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern ) 
		m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 2, 0, 16, 16 );

	
	DrawSelectedItem( surface, 2, 2 );

	
	
	if ( IsDisabled() && rect.left < rect.right && rect.top < rect.bottom )
	{
		

		uint16 *pixel;

		AUI_ERRCODE errcode = surface->Lock( &rect, (void **)&pixel, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( AUI_SUCCESS(errcode) )
		{
			
			uint16 *origPixel = pixel;

			
			const sint32 pitch = surface->Pitch() / 2;
			const sint32 width = rect.right - rect.left;
			const sint32 diff = pitch - width;

			
			uint16 *stopHorizontal = pixel + width;
			const uint16 *stopVertical = pixel +
				pitch * ( rect.bottom - rect.top );

			do
			{
				do
				{
					*pixel = pixelutils_BlendFast(
						*pixel,	
						0x0000,	
						24 );	

				} while ( ++pixel != stopHorizontal );

				stopHorizontal += pitch;
			} while ( (pixel += diff) != stopVertical );

			errcode = surface->Unlock( origPixel );
			Assert( AUI_SUCCESS(errcode) );
		}
	}

	if (m_window) 
		if ( surface == m_window->TheSurface() )
			m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_DropDown::RepositionButton( void )
{
	
	m_button->Resize( m_buttonSize - 2, m_height - 4 );

	

	
	

	m_button->Move( m_width, 2 );
	
	return AUI_ERRCODE_OK;
}


void c3_DropDown::Clear(void)
{
	((c3_ListBox *)m_listBox)->Clear();
}
