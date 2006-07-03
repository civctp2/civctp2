//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface dropdown menu
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
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_button.h"
#include "aui_item.h"
#include "aui_listbox.h"
#include "aui_ranger.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_static.h"

#include "aui_dropdown.h"



aui_DropDown::aui_DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_DropDown::aui_DropDown(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 buttonSize,
	sint32 windowSize,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( buttonSize, windowSize );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_DropDown::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	sint32 buttonSize = block->GetInt( k_AUI_DROPDOWN_LDL_BUTTONSIZE );
	sint32 windowSize = block->GetInt( k_AUI_DROPDOWN_LDL_WINDOWSIZE );

	AUI_ERRCODE errcode = InitCommon( buttonSize, windowSize );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	m_alwaysPopup = block->GetBool( k_AUI_DROPDOWN_LDL_ALWAYSPOPUP );
	m_inverted = block->GetBool(k_AUI_DROPDOWN_LDL_INVERTED);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::InitCommon( sint32 buttonSize, sint32 windowSize )
{
	m_button = NULL,
	m_listBox = NULL,
	m_listBoxWindow = NULL,
	m_staticPane = NULL,
	m_buttonSize = buttonSize ? buttonSize : k_CONTROL_DEFAULT_SIZE,
	m_windowSize = windowSize ? windowSize : m_width,
	m_alwaysPopup = FALSE,
	m_selectedItem = -1;
	m_inverted = false;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::CreateComponents( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_STATICPANE );

        if (aui_Ldl::GetLdl()->FindDataBlock(block))
		{
			m_staticPane = new aui_Static(
				&errcode,
				aui_UniqueId(),
				block );

			m_staticPane->SetBlindness(TRUE);

			Assert( AUI_NEWOK(m_staticPane,errcode) );
			if ( !AUI_NEWOK(m_staticPane,errcode) ) return errcode;
		}
	}

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_BUTTON );

        if (aui_Ldl::GetLdl()->FindDataBlock(block))
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

        if (aui_Ldl::GetLdl()->FindDataBlock(block))
		{
			m_listBoxWindow = new aui_Window(
				&errcode,
				aui_UniqueId(),
				block,
				g_ui->Primary()->BitsPerPixel(), AUI_WINDOW_TYPE_POPUP );

			
			
			aui_Ldl::Remove( m_listBoxWindow );

			if ( m_listBoxWindow )
			{
				m_windowSize = m_listBoxWindow->Height();
				m_listBoxWindow->Resize( m_width + m_buttonSize, m_windowSize );
			}

			sprintf( block, "%s.%s.%s", ldlBlock, k_AUI_DROPDOWN_LDL_WINDOW, k_AUI_DROPDOWN_LDL_LISTBOX );

            if (aui_Ldl::GetLdl()->FindDataBlock(block))
				m_listBox = new aui_ListBox(
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
			g_ui->Primary()->BitsPerPixel(), AUI_WINDOW_TYPE_POPUP );

	Assert( AUI_NEWOK(m_listBoxWindow,errcode) );
	if ( !AUI_NEWOK(m_listBoxWindow,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	if ( !m_listBox )
		m_listBox = new aui_ListBox(
			&errcode,
			aui_UniqueId(),
			0, 0, 0, 0,
			DropDownListBoxActionCallback, this );

	Assert( AUI_NEWOK(m_listBox,errcode) );
	if ( !AUI_NEWOK(m_listBox,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_listBox->SetForceSelect( TRUE );

	if ( m_staticPane ) AddChild( m_staticPane );
	AddChild( m_button );

	m_listBoxWindow->AddChild( m_listBox );

	RepositionButton();
	RepositionListBoxWindow();

	return AUI_ERRCODE_OK;
}



aui_DropDown::~aui_DropDown()
{
	if (g_ui && m_listBoxWindow)
	{
		g_ui->RemoveWindow( m_listBoxWindow->Id() );
	}

	delete m_listBoxWindow;
    delete m_listBox;
	delete m_button;
	delete m_staticPane;
}



sint32 aui_DropDown::SetWindowSize( sint32 windowSize )
{
	sint32 prevWindowSize = m_windowSize;
	sint32 width;
	if(m_listBox) {
		width = m_listBox->Width(); 
		if(m_listBox->GetVerticalRanger()->GetMaximumY())
			width += m_listBox->GetRangerSize();
	} else
		width = m_listBoxWindow->Width();

	m_listBoxWindow->Resize(
		width,
		m_windowSize = windowSize );

	RepositionListBoxWindow();

	return prevWindowSize;
}



AUI_ERRCODE aui_DropDown::Resize( sint32 width, sint32 height )
{
	aui_Control::Resize( width, height );

	RepositionButton();
	RepositionListBoxWindow();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::Hide( void )
{
	g_ui->RemoveWindow( m_listBoxWindow->Id() );

	return aui_Control::Hide();
}



AUI_ERRCODE aui_DropDown::AddItem( aui_Item *item )
{
	m_listBox->AddItem( item );

	tech_WLList<sint32> *selectedList = m_listBox->GetSelectedList();
	selectedList->L() ?
		SetSelectedItem( selectedList->GetHead(), 1 ) :
		SetSelectedItem( -1, 1 );

	
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::RemoveItem( uint32 itemId )
{
	m_listBox->RemoveItem( itemId );

	tech_WLList<sint32> *selectedList = m_listBox->GetSelectedList();
	selectedList->L() ?
		SetSelectedItem( selectedList->GetHead(), 1 ) :
		SetSelectedItem( -1, 1 );

	
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::RepositionButton( void )
{
	m_button->Resize( m_buttonSize, m_height );
	m_button->Move( m_width, 0 );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::RepositionListBoxWindow( void )
{
	

	
	if ( m_window )
	{
		POINT screenLocation = { m_x, m_y + m_height };
		if(m_inverted) {
			screenLocation.y = m_y - m_listBoxWindow->Height();
		}
		ToScreen( &screenLocation );

		m_listBoxWindow->Move( screenLocation.x, screenLocation.y );
	}


	

	sint32 listBoxWidth = m_listBoxWindow->Width();
	
	
	

	m_listBox->Resize( listBoxWidth, m_windowSize );
	m_listBox->Move( 0, 0 );


	return AUI_ERRCODE_OK;
}



sint32 aui_DropDown::SetSelectedItem( sint32 itemIndex, uint32 data )
{
	sint32 prevIndex = m_selectedItem;
	m_selectedItem = itemIndex;
	if ( m_listBox->GetSelectedItemIndex() != itemIndex )
		m_listBox->SelectItem( itemIndex, data );
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
	return prevIndex;
}



AUI_ERRCODE aui_DropDown::ShowListBoxWindow( BOOL showIt )
{
	if ( showIt )
	{
		
		if ( ( m_alwaysPopup || m_listBox->NumItems() > 1 )
		&&   !g_ui->GetWindow( m_listBoxWindow->Id() ) )
		{
			RepositionListBoxWindow();
			g_ui->AddWindow( m_listBoxWindow );



		}
	}
	else
	{
		if ( g_ui->GetWindow( m_listBoxWindow->Id() ) )
		{
			g_ui->RemoveWindow( m_listBoxWindow->Id() );
		}
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::ToggleListBoxWindow( void )
{
	if ( !g_ui->GetWindow( m_listBoxWindow->Id() ) )
	{
		
		if ( m_alwaysPopup || m_listBox->NumItems() > 1 )
		{
			RepositionListBoxWindow();
			g_ui->AddWindow( m_listBoxWindow );



		}
	}
	else
	{
		g_ui->RemoveWindow( m_listBoxWindow->Id() );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DropDown::DrawSelectedItem( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( m_staticPane || m_selectedItem < 0 ) return AUI_ERRCODE_OK;

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	ListPos position =
		m_listBox->GetPane()->ChildList()->FindIndex( m_selectedItem );

	if (!position)
		return AUI_ERRCODE_INVALIDPARAM;

	aui_Item *item =
		(aui_Item *)m_listBox->GetPane()->ChildList()->GetAt( position );

	
	POINT dropDownPoint = { m_x + x, m_y + y };
	ToWindow( &dropDownPoint );
	m_window->AddDirtyRect(
		dropDownPoint.x,
		dropDownPoint.y,
		dropDownPoint.x + item->Width(),
		dropDownPoint.y + item->Height() );

	
	POINT itemPoint = { item->X(), item->Y() };
	item->ToWindow( &itemPoint );

	
	BOOL wasHidden = item->IsHidden();
	item->Show();

	AUI_ERRCODE errcode = item->Draw(
		surface,
		dropDownPoint.x - itemPoint.x,
		dropDownPoint.y - itemPoint.y );

	if ( wasHidden ) item->Hide();

	return errcode;
}



AUI_ERRCODE aui_DropDown::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	aui_Control::DrawThis( surface, x, y );

	return DrawSelectedItem( m_window->TheSurface(), x, y );
}




AUI_ERRCODE aui_DropDown::UpdateSelectedItem( BOOL update )
{
	if ( update )
	{
		sint32 index = m_listBox->GetSelectedItemIndex();

		
		SetSelectedItem( index );
	}

	ShowListBoxWindow( FALSE );

	if ( m_ActionFunc )
		m_ActionFunc( this, AUI_DROPDOWN_ACTION_SELECT, 0, m_cookie );
	else if ( m_action )
		m_action->Execute( this, AUI_DROPDOWN_ACTION_SELECT, 0 );

	return AUI_ERRCODE_OK;
}








void aui_DropDown::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		
		HideTipWindow();

		SetMouseOwnership();
		SetKeyboardFocus();

		if ( !HandleGameSpecificLeftClick( this ) )
		ToggleListBoxWindow();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
	else
		MouseLGrabOutside( mouseData );
}


void aui_DropDown::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		
		HideTipWindow();

		SetMouseOwnership();
		SetKeyboardFocus();

		if ( !HandleGameSpecificRightClick( this ) )


		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
	else
		MouseLGrabOutside( mouseData );
}



void aui_DropDown::MouseLGrabOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !g_ui->GetWindow( m_listBoxWindow->Id() ) ) return;

	
	if ( !m_button->IsInside(
		mouseData->position.x - m_x,
		mouseData->position.y - m_y ) )
	{
		
		POINT screenLocation = mouseData->position;
		ToScreen( &screenLocation );

		
		if ( !m_listBoxWindow->IsInside( &screenLocation ) )
			ShowListBoxWindow( FALSE );

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}




void aui_DropDown::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !g_ui->GetWindow( m_listBoxWindow->Id() ) ) return;

	
	if ( !m_button->IsInside(
		mouseData->position.x - m_x,
		mouseData->position.y - m_y ) )
	{
		
		POINT windowLocation = mouseData->position;
		ToScreen( &windowLocation );
		windowLocation.x -= m_listBoxWindow->X();
		windowLocation.y -= m_listBoxWindow->Y();

		
		if ( m_listBox->IsInside( &windowLocation ) )
			ShowListBoxWindow( FALSE );

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}










void DropDownButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == (uint32)AUI_BUTTON_ACTION_PRESS
	&&   ((aui_Button *)control)->ExtractRepeatCount( data ) == 0 )
	{
		aui_DropDown *dropdown = (aui_DropDown *)cookie;
		dropdown->ToggleListBoxWindow();
	}
}



void DropDownListBoxActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == (uint32)AUI_LISTBOX_ACTION_SELECT )
	{
		aui_DropDown *dropdown = (aui_DropDown *)cookie;

		
		
		dropdown->UpdateSelectedItem();
	}
}
