




















#include "c3.h"

#include "SelItem.h"


#include "aui.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"

#include "aui_static.h"
#include "aui_surface.h"


#include "textbox.h"


#include "textbutton.h"
#include "aui_button.h"
#include "aui_window.h"


#include "message.h"


#include "c3windows.h"
#include "messageiconwindow.h"
#include "messagewindow.h"
#include "messageactions.h"
#include "messagelist.h"
#include "messagemodal.h"
#include "messageadvice.h"
#include "messagewin.h"

#include "controlpanelwindow.h"


extern C3UI				*g_c3ui;
extern SelectedItem		*g_selected_item; 

tech_WLList<MessageList *>		*g_messageUserList = NULL;
PLAYER_INDEX			g_currentPlayerMessages = 0;

extern MessageModal *g_modalMessage;



uint16 g_messageReadPositionX = 32;
uint16 g_messageReadPositionY = 241;
uint8 g_messageMaxVisible = 6;

uint8 g_messageRespButtonSpacing = 4;
uint8 g_messageRespTextPadding = 5;
uint8 g_messageRespButtonWidth = 50;
uint8 g_messageRespDropPadding = 25;
uint8 g_messageEyeDropWidth = 70;
uint8 g_messageEyeGreatPadding = 3;
uint16 g_messageMoreX = 32;
uint16 g_messageMoreY = 30;
uint16 g_messageLessX = 32;
uint16 g_messageLessY = 276;
uint8 g_messageIconSpacing = 2;
uint8 g_messageIconHeight = 33;
uint8 g_messageIconWidth = 33;






















AUI_ERRCODE messagewin_InitializeMessages( void )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	g_messageUserList = new tech_WLList<MessageList *>;
	Assert( g_messageUserList != NULL );
	if ( g_messageUserList == NULL ) return AUI_ERRCODE_MEMALLOCFAILED;

	BOOL valid = theLdl->IsValid( "MessageboxAttributes" );
	Assert( valid );
	if ( !valid ) {
		return AUI_ERRCODE_HACK;
	}

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( "MessageboxAttributes" );
	Assert( block != NULL );
	if ( !block ) {
		return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
	}

	
	if ( block->GetAttributeType( "readx" )
			== ATTRIBUTE_TYPE_INT )
		g_messageReadPositionX = block->GetInt( "readx" );
		
	if ( block->GetAttributeType( "ready" )
			== ATTRIBUTE_TYPE_INT )
		g_messageReadPositionY = block->GetInt( "ready" );
		
	if ( block->GetAttributeType( "maxvisible" )
			== ATTRIBUTE_TYPE_INT )
		g_messageMaxVisible = block->GetInt( "maxvisible" );

	if ( block->GetAttributeType( "buttonspacing" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespButtonSpacing = block->GetInt( "buttonspacing" );

	if ( block->GetAttributeType( "textpadding" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespTextPadding = block->GetInt( "textpadding" );

	if ( block->GetAttributeType( "buttonwidth" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespButtonWidth = block->GetInt( "buttonwidth" );

	if ( block->GetAttributeType( "dropdownpadding" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespDropPadding = block->GetInt( "dropdownpadding" );

	if ( block->GetAttributeType( "eyewidth" )
			== ATTRIBUTE_TYPE_INT )
		g_messageEyeDropWidth = block->GetInt( "eyewidth" );

	if ( block->GetAttributeType( "greatlibrary" )
			== ATTRIBUTE_TYPE_INT )
		g_messageEyeGreatPadding = block->GetInt( "greatlibrary" );

	if ( block->GetAttributeType( "morex" )
			== ATTRIBUTE_TYPE_INT )
		g_messageMoreX = block->GetInt( "morex" );

	if ( block->GetAttributeType( "morey" )
			== ATTRIBUTE_TYPE_INT )
		g_messageMoreY = block->GetInt( "morey" );

	if ( block->GetAttributeType( "lessx" )
			== ATTRIBUTE_TYPE_INT )
		g_messageLessX = block->GetInt( "lessx" );

	if ( block->GetAttributeType( "lessy" )
			== ATTRIBUTE_TYPE_INT )
		g_messageLessY = block->GetInt( "lessy" );

	if ( block->GetAttributeType( "miconspacing" )
			== ATTRIBUTE_TYPE_INT )
		g_messageIconSpacing = block->GetInt( "miconspacing" );

	if ( block->GetAttributeType( "miconheight" )
			== ATTRIBUTE_TYPE_INT )
		g_messageIconHeight = block->GetInt( "miconheight" );

	if ( block->GetAttributeType( "miconwidth" )
			== ATTRIBUTE_TYPE_INT )
		g_messageIconWidth = block->GetInt( "miconwidth" );


	
	
	sint32		distanceFromScreenBottom;
	extern sint32 g_ScreenWidth, g_ScreenHeight;

	if (g_ScreenWidth < 1024) {
		distanceFromScreenBottom = 270;
	} else {
		distanceFromScreenBottom = 300;
	}

	g_messageReadPositionX = 16;

	g_messageReadPositionY = (uint16)(g_ScreenHeight - distanceFromScreenBottom);

	g_messageMaxVisible = (g_messageReadPositionY / 
		(g_messageIconHeight + g_messageIconSpacing)) - 3;

	g_messageMoreX = g_messageReadPositionX;
	g_messageMoreY = g_messageReadPositionY - 
						(g_messageIconHeight + g_messageIconSpacing) * g_messageMaxVisible;
	g_messageLessX = g_messageReadPositionX;
	g_messageLessY = g_messageReadPositionY + g_messageIconHeight + g_messageIconSpacing;

	return AUI_ERRCODE_OK;
}


MessageList *messagewin_InitializePlayerMessage( PLAYER_INDEX index )
{
	MessageList *list = NULL;
		
	if ( !g_messageUserList ) return NULL;
	
	if ( messagewin_GetPlayerMessageList( index )) return NULL;

	list = new MessageList( index );
	Assert( list != NULL );
	if ( list == NULL ) return NULL;

	g_messageUserList->AddTail( list );

	return list;
}



MessageList *messagewin_GetPlayerMessageList( PLAYER_INDEX index )
{
	ListPos position;
	MessageList *list = NULL;

	if ( !g_messageUserList ) return NULL;

	position = g_messageUserList->GetHeadPosition();

	for ( uint32 count = g_messageUserList->L(); count; count-- ) {
		list = g_messageUserList->GetNext( position );
		if ( list->GetPlayer() == index )
			return list;
	}

	
	return NULL;
}














int messagewin_CreateMessage( Message data, BOOL bRecreate )
{
	AUI_ERRCODE			errcode;



	if ( data.IsHelpBox() ) {
		
		return 0;
	}

	
	g_controlPanel->AddMessage(data);

	
	MessageList *messagelist = messagewin_GetPlayerMessageList( data.GetOwner() );

	if ( !messagelist ) {
		
		messagelist = messagewin_InitializePlayerMessage( data.GetOwner( ));
		Assert( messagelist != NULL );
		if ( messagelist == NULL ) return -1;
	}

	errcode = messagelist->CreateMessage( data );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -1;

	
	
	if ( !bRecreate ) {
		MBCHAR *wavName = NULL;
		if ( wavName = ( MBCHAR * ) data.AccessData()->GetMsgSound() ) {
			MBCHAR filename[ _MAX_PATH ]; 
			g_civPaths->FindFile( C3DIR_SOUNDS, wavName, filename );
			PlaySound( filename, NULL, SND_ASYNC | SND_FILENAME ); 
		}
	}

	return 1;
}



int messagewin_CreateModalMessage( Message data )
{
	
	

	messagemodal_CreateModalMessage( data );	

	MBCHAR *wavName = NULL;
	if ( wavName = ( MBCHAR * ) data.AccessData()->GetMsgSound() ) {
		MBCHAR filename[ _MAX_PATH ]; 
		g_civPaths->FindFile( C3DIR_SOUNDS, wavName, filename );
		PlaySound( filename, NULL, SND_ASYNC | SND_FILENAME ); 
	}

	return 1;
}



int messagewin_PrepareDestroyWindow( MessageWindow *window )
{
	Assert( window != NULL );
	if ( !window ) return -1;

	
	

	g_c3ui->AddDestructiveAction( new MessageCleanupAction( window, window->GetPlayer() ) );
	return 1;
}








int messagewin_FastKillWindow(MessageWindow *window)
{
	sint32			player = window->GetPlayer();
	MessageList		*messagelist;

	messagewin_CleanupMessage( window );

	messagelist = messagewin_GetPlayerMessageList( player );
	
	
	messagelist->CheckVisibleMessages( );

	return 1;
}













int messagewin_CleanupMessage( MessageWindow *window )
{
	MessageIconWindow *iconWindow = window->GetIconWindow();

	return messagewin_CleanupMessage( iconWindow, window );
}



int messagewin_CleanupMessage( MessageIconWindow *iconWindow )
{
	MessageWindow *window = iconWindow->GetWindow();

	return messagewin_CleanupMessage( iconWindow, window );
}



int messagewin_CleanupMessage( MessageIconWindow *iconWindow, 
							   MessageWindow *window )
{
	
	Assert ( g_messageUserList != NULL );
	if ( g_messageUserList == NULL ) return -1;

	MessageList *messagelist = messagewin_GetPlayerMessageList( window->GetPlayer() );
	Assert( messagelist != NULL );
	if ( messagelist == NULL ) return -1;

	
	
	g_controlPanel->RemoveMessage(*window->GetMessage());

	messagelist->Remove( iconWindow, window );
	

	return 1;
}





int messagewin_Cleanup( void )
{
	
	ListPos position;
	MessageList			*messagelist = NULL;

	if ( !g_messageUserList ) return 0; 

	
	messagewin_LessMessagesIcon( FALSE, TRUE );
	messagewin_MoreMessagesIcon( FALSE, TRUE );

	position = g_messageUserList->GetHeadPosition();
	for ( uint32 count = g_messageUserList->L(); count; count-- ) {
		messagelist = g_messageUserList->GetNext( position );
		delete messagelist;
		messagelist = NULL;
	}

	g_messageUserList->DeleteAll();
	delete g_messageUserList;
	g_messageUserList = NULL;

	return 1;
}



int messagewin_PurgeMessages( void )
{
	
	ListPos position;
	MessageList			*messagelist = NULL;

	if ( !g_messageUserList ) return 0; 
	
	
	messagewin_LessMessagesIcon( FALSE, TRUE );
	messagewin_MoreMessagesIcon( FALSE, TRUE );

	position = g_messageUserList->GetHeadPosition();
	for ( uint32 count = g_messageUserList->L(); count; count-- ) {
		messagelist = g_messageUserList->GetNext( position );
		delete messagelist;
		messagelist = NULL;
	}

	g_messageUserList->DeleteAll();

	return 1;
}



void messagewin_EndTurn( PLAYER_INDEX index )
{
	
	MessageList		*messagelist = NULL;

	
	messagelist = messagewin_GetPlayerMessageList( index );
	if ( !messagelist ) return;

	messagelist->HideVisibleWindows();

	
	messagewin_MoreMessagesIcon( FALSE );
	messagewin_LessMessagesIcon( FALSE );

}



void messagewin_BeginTurn( PLAYER_INDEX index )
{
	if ( g_currentPlayerMessages != index ) {
		messagewin_EndTurn( g_currentPlayerMessages );
	}

	g_currentPlayerMessages = index; 

	
	MessageList *messagelist = messagewin_GetPlayerMessageList( index );
	if ( !messagelist ) {
		messagelist = messagewin_InitializePlayerMessage( index );		
		Assert( messagelist != NULL );
		if ( messagelist == NULL ) return;
	}

	messagelist->CheckVisibleMessages( );
}



int messagewin_MoreMessagesIcon( BOOL make, BOOL destroy )
{

return 1;

	static aui_Window *window = NULL;
	static aui_Button *button = NULL;
	static ChangeOffsetMessageIconButtonAction *action = NULL;

	if ( make ) {

		AUI_ERRCODE retval = AUI_ERRCODE_OK;

		if ( !window ) {
			MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
			MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

			strcpy( windowBlock, "MoreMessagesIconWindow" );

			
			window = new aui_Window( &retval, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_STANDARD );
			Assert( AUI_NEWOK( window, retval ));
			if ( !AUI_NEWOK( window, retval )) return -1;

			sprintf( buttonBlock, "%s.%s", windowBlock, "icon" );
			button = new aui_Button( &retval, aui_UniqueId(), buttonBlock );
			Assert( AUI_NEWOK( button, retval ));
			if ( !AUI_NEWOK( button, retval )) return -1;

			action = new ChangeOffsetMessageIconButtonAction( 1, 
							messagewin_GetPlayerMessageList( g_selected_item->GetVisiblePlayer( )), 
							SHOW_MESSAGE_OFFSET_RELATIVE );
			Assert( action != NULL );
			if ( action == NULL ) return -1;

			button->SetAction( action );

			retval = window->AddChild( button );
			Assert( retval == AUI_ERRCODE_OK );
			if ( retval != AUI_ERRCODE_OK ) return -1;
		}

		action->SetList(messagewin_GetPlayerMessageList(g_selected_item->GetVisiblePlayer()));
		window->Move( g_messageMoreX, g_messageMoreY );
		retval = g_c3ui->AddWindow( window );
		Assert( retval == AUI_ERRCODE_OK );
		if ( retval != AUI_ERRCODE_OK ) return -1;

	} else {
		if ( window )
			if ( g_c3ui->GetWindow( window->Id( )))
				g_c3ui->RemoveWindow( window->Id( ));
		
		if ( destroy ) {
			if ( button ) {
				if ( window )
					window->RemoveChild( button->Id( ));	
				delete button;
				button = NULL;
			}

			if ( window ) {
				delete window;
				window = NULL;
			}

			if ( action ) {
				delete action;
				action = NULL;
			}
		}
	}

	return 1;
}


int messagewin_LessMessagesIcon( BOOL make, BOOL destroy )
{

return 1;

	static aui_Window *window = NULL;
	static aui_Button *button = NULL;
	static ChangeOffsetMessageIconButtonAction	*action = NULL;

	if ( make ) {

		AUI_ERRCODE retval = AUI_ERRCODE_OK;

		if ( !window ) {
			MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
			MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

			strcpy( windowBlock, "LessMessagesIconWindow" );

			
			window = new aui_Window( &retval, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_STANDARD );
			Assert( AUI_NEWOK( window, retval ));
			if ( !AUI_NEWOK( window, retval )) return -1;

			sprintf( buttonBlock, "%s.%s", windowBlock, "icon" );
			button = new aui_Button( &retval, aui_UniqueId(), buttonBlock );
			Assert( AUI_NEWOK( button, retval ));
			if ( !AUI_NEWOK( button, retval )) return -1;

			action = new ChangeOffsetMessageIconButtonAction( -1, 
							messagewin_GetPlayerMessageList( g_selected_item->GetVisiblePlayer( )), 
							SHOW_MESSAGE_OFFSET_RELATIVE );
			Assert( action != NULL );
			if ( action == NULL ) return -1;

			button->SetAction( action );

			retval = window->AddChild( button );
			Assert( retval == AUI_ERRCODE_OK );
			if ( retval != AUI_ERRCODE_OK ) return -1;
		}

		action->SetList(messagewin_GetPlayerMessageList(g_selected_item->GetVisiblePlayer()));
		window->Move( g_messageLessX, g_messageLessY );
		retval = g_c3ui->AddWindow( window );
		Assert( retval == AUI_ERRCODE_OK );
		if ( retval != AUI_ERRCODE_OK ) return -1;

	} else {
		if ( window ) 
			if ( g_c3ui->GetWindow( window->Id( )))
				g_c3ui->RemoveWindow( window->Id( ));
		
		if ( destroy ) {
			if ( button ) {
				if ( window )
					window->RemoveChild( button->Id( ));	
				delete button;
				button = NULL;
			}
			if ( window ) {
				delete window;
				window = NULL;
			}
			if ( action ) {
				delete action;
				action = NULL;
			}
		}
	}
	return 1;
}

int messagewin_IsModalMessageDisplayed()
{
	return g_modalMessage != NULL;
}
