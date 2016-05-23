//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Message box action handlers
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
// - Option added to close a message box automatically on eyepoint clicking.
// - Messages are closed if an open command is executed and there is already
//   an open messages, enables left click close. (Oct 16th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_button.h"

#include "SlicButton.h"

#include "colorset.h"
#include "pixelutils.h"
#include "radarmap.h"
#include "c3_dropdown.h"

#include "messageiconwindow.h"
#include "messagewin.h"
#include "messagewindow.h"
#include "messagelist.h"
#include "messageadvice.h"
#include "messagemodal.h"
#include "messageactions.h"

#include "MessagePool.h"

#include "c3ui.h"

#include "SelItem.h"
#include "player.h"
#include "director.h"
#include "profileDB.h"				// g_theProfileDB

extern RadarMap			*g_radarMap;
extern C3UI				*g_c3ui;

void MessageOpenAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != 0) return;

	Message *   message = m_iconWindow->GetMessage();

	if (!message) return;
	if (!g_theMessagePool->IsValid(*message)) return;

	if ( data ) {

		if(!message->AccessData()->IsCloseDisabled()) {
			message->Kill();
		}
		return;
	}

	if ( !g_c3ui->GetWindow( m_iconWindow->GetWindow()->Id( ))) {
		m_iconWindow->SetCurrentIconButton( m_iconWindow->GetIconButton() );
		m_iconWindow->GetWindow()->ShowWindow( TRUE );
	}
	else{
		m_iconWindow->SetCurrentIconButton( NULL );
		m_iconWindow->GetWindow()->ShowWindow( FALSE );
		return;
	}

	MBCHAR const *  wavName = message->AccessData()->GetMsgOpenSound();
	if (wavName)
    {
		MBCHAR filename[_MAX_PATH];
		g_civPaths->FindFile(C3DIR_SOUNDS, wavName, filename);
		PlaySound(filename, NULL, SND_ASYNC | SND_FILENAME);
	}
}


void MessageMinimizeAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	m_window->ShowWindow( FALSE );

	m_window->GetIconWindow()->SetCurrentIconButton( NULL );

	sint32 p = g_selected_item->GetVisiblePlayer();
	if(g_player[p]) {
		sint32 m;
		for(m = 0; m < g_player[p]->m_messages->Num(); m++) {
			Message msg = g_player[p]->m_messages->Access(m);
			if(!msg.IsRead() && msg.IsInstantMessage()) {
				g_director->AddMessage(msg);
				break;
			}
		}
	}
}









void MessageDismissUIAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	if(!g_theMessagePool->IsValid(*m_window->GetMessage()))
		return;




}









void MessageDismissAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	g_c3ui->AddAction(new MessageDismissUIAction(m_window));
}


void MessageLibraryAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	if(!g_theMessagePool->IsValid(*m_window->GetMessage()))
		return;

	m_window->GetMessage()->AccessData()->GreatLibraryCallback();

}


//----------------------------------------------------------------------------
//
// Name       : MessageStandardEyePointAction::Execute
//
// Description: Handle eyepoint click on message box
//
// Parameters : control				: screen area of message box (not used)
//              action				: type of click
//				data				: not used
//
// Globals    : g_theProfileDB		: user preferences
//				g_theMessagePool	: currently active messages
//
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void MessageStandardEyePointAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	Message *message;
	if ( m_window )
		message = m_window->GetMessage();
	else
		message = m_modal->GetMessage();

	if(!g_theMessagePool->IsValid(*message))
		return;

	message->AccessData()->EyePointCallback( 0 );

	if (g_theProfileDB->GetValueByName("CloseOnEyepoint"))
	{
		message->Minimize();
	}
}


void MessageDropdownEyePointAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	Message *message;

	if ( m_window )
		message = m_window->GetMessage();
	else
		message = m_modal->GetMessage();

	sint32 index = ((aui_DropDown *)m_dropdown)->GetListBox()->GetSelectedItemIndex();

	if(!g_theMessagePool->IsValid(*message))
		return;

	if(index >= 0) {
		if(((aui_DropDown *)m_dropdown)->ExtractEndUserTriggeredEvent(data)) {
			message->AccessData()->EyePointCallback( index );
		}
	}
}


void MessageDropdownAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_LISTBOX_ACTION_SELECT) return;

	Message *message;

	if ( m_window )
		message = m_window->GetMessage();
	else
		message = m_modal->GetMessage();

	sint32 index = ((aui_DropDown *)m_dropdown)->GetListBox()->GetSelectedItemIndex();

	if ( index < 0 )
		return;

	if(!g_theMessagePool->IsValid(*message))
		return;

	if(((aui_DropDown *)m_dropdown)->ExtractEndUserTriggeredEvent(data)) {
		message->AccessData()->EyeDropdownCallback( index );
	}
}


void MessageListboxEyePointAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	Message *message;

	if ( m_window )
		message = m_window->GetMessage();
	else
		message = m_modal->GetMessage();

	m_index += m_change;
	if ( m_index > m_max ) m_index = m_min;
	if ( m_index < m_min ) m_index = m_max;

	if(!g_theMessagePool->IsValid(*message))
		return;

	MapPoint pos;
	message->AccessData()->GetEyePointMapPosition( m_index, pos );
	g_radarMap->CenterMap( pos );




	m_action->SetIndex( m_index );

}


void MessageResponseUIAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	Message *   message = m_window->GetMessage();

    if (message && g_theMessagePool->IsValid(*message))
    {
    	m_window->GetMessage()->AccessData()->GetButton( m_response )->Callback();
    }
}

void MessageResponseAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;


	g_c3ui->AddAction(new MessageResponseUIAction(m_window, m_response));
}


void MessageModalResponseAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	if(g_theMessagePool->IsValid(*m_message)) {

		m_message->AccessData()->GetButton( m_response )->Callback();
	}

	g_c3ui->AddAction( new MessageModalDestroyAction( ) );
}




void MessageModalDestroyAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	messagemodal_DestroyModalMessage();
}


void MessageResponseSubmitAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_LISTBOX_ACTION_SELECT ) return;

	Message *   message = m_window->GetMessage();

	if (message && g_theMessagePool->IsValid(*message))
    {
    	sint32 index = ((aui_ListBox *) m_dropdown)->GetSelectedItemIndex();
	    message->AccessData()->GetButton(index)->Callback();
    }
}


void MessageCleanupAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	messagewin_CleanupMessage( m_iconWindow );
	MessageList *   messagelist = messagewin_GetPlayerMessageList(m_index);
	messagelist->CheckVisibleMessages( );
}


void ChangeOffsetMessageIconButtonAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	if(m_messagelist)
		m_messagelist->ChangeOffset( m_offset, m_offsetflag );
};


void ChangeOffsetDestroyButtonAction::Execute( aui_Control *control, uint32 action, uint32 data )
{

	if ( m_more )
		messagewin_MoreMessagesIcon( FALSE );
	else
		messagewin_LessMessagesIcon( FALSE );
};


void MessageAdviceDismissAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	g_c3ui->AddAction( new MessageAdviceDestroyAction( ) );

}


void MessageAdviceDestroyAction::Execute( aui_Control *control, uint32 action, uint32 data )
{
	messageadvice_DestroyWindow();
}
