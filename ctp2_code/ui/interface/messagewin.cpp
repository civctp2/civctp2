
#include "c3.h"
#include "messagewin.h"

#include "Globals.h"        // allocated::...
#include "SelItem.h"        // g_selected_item


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

#include "controlpanelwindow.h"

#include "ldl_data.hpp"

extern C3UI	*           g_c3ui;
extern MessageModal *   g_modalMessage;
extern sint32           g_ScreenWidth;
extern sint32           g_ScreenHeight;

tech_WLList<MessageList *>		*g_messageUserList = NULL;

PLAYER_INDEX			g_currentPlayerMessages = 0;

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
	g_messageUserList = new tech_WLList<MessageList *>;

	ldl_datablock * block = aui_Ldl::FindDataBlock("MessageboxAttributes");
	Assert( block != NULL );
	if ( !block ) {
		return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
	}

	if ( block->GetAttributeType( "readx" )
			== ATTRIBUTE_TYPE_INT )
		g_messageReadPositionX = static_cast<uint16>(block->GetInt("readx"));
		
	if ( block->GetAttributeType( "ready" )
			== ATTRIBUTE_TYPE_INT )
		g_messageReadPositionY = static_cast<uint16>(block->GetInt("ready"));
		
	if ( block->GetAttributeType( "maxvisible" )
			== ATTRIBUTE_TYPE_INT )
		g_messageMaxVisible = static_cast<uint8>(block->GetInt("maxvisible"));

	if ( block->GetAttributeType( "buttonspacing" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespButtonSpacing = static_cast<uint8>(block->GetInt("buttonspacing"));

	if ( block->GetAttributeType( "textpadding" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespTextPadding = static_cast<uint8>(block->GetInt("textpadding"));

	if ( block->GetAttributeType( "buttonwidth" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespButtonWidth = static_cast<uint8>(block->GetInt("buttonwidth"));

	if ( block->GetAttributeType( "dropdownpadding" )
			== ATTRIBUTE_TYPE_INT )
		g_messageRespDropPadding = static_cast<uint8>(block->GetInt("dropdownpadding"));

	if ( block->GetAttributeType( "eyewidth" )
			== ATTRIBUTE_TYPE_INT )
		g_messageEyeDropWidth = static_cast<uint8>(block->GetInt("eyewidth"));

	if ( block->GetAttributeType( "greatlibrary" )
			== ATTRIBUTE_TYPE_INT )
		g_messageEyeGreatPadding = static_cast<uint8>(block->GetInt("greatlibrary"));

	if ( block->GetAttributeType( "morex" )
			== ATTRIBUTE_TYPE_INT )
		g_messageMoreX = static_cast<uint16>(block->GetInt("morex"));

	if ( block->GetAttributeType( "morey" )
			== ATTRIBUTE_TYPE_INT )
		g_messageMoreY = static_cast<uint16>(block->GetInt("morey"));

	if ( block->GetAttributeType( "lessx" )
			== ATTRIBUTE_TYPE_INT )
		g_messageLessX = static_cast<uint16>(block->GetInt("lessx"));

	if ( block->GetAttributeType( "lessy" )
			== ATTRIBUTE_TYPE_INT )
		g_messageLessY = static_cast<uint16>(block->GetInt("lessy"));

	if ( block->GetAttributeType( "miconspacing" )
			== ATTRIBUTE_TYPE_INT )
		g_messageIconSpacing = static_cast<uint8>(block->GetInt("miconspacing"));

	if ( block->GetAttributeType( "miconheight" )
			== ATTRIBUTE_TYPE_INT )
		g_messageIconHeight = static_cast<uint8>(block->GetInt("miconheight"));

	if ( block->GetAttributeType( "miconwidth" )
			== ATTRIBUTE_TYPE_INT )
		g_messageIconWidth = static_cast<uint8>(block->GetInt("miconwidth"));


    sint32 distanceFromScreenBottom = (g_ScreenWidth < 1024) ? 270 : 300;

    g_messageReadPositionX = 16;
	g_messageReadPositionY = (uint16)(g_ScreenHeight - distanceFromScreenBottom);

	g_messageMaxVisible = static_cast<uint8>
        ((g_messageReadPositionY / (g_messageIconHeight + g_messageIconSpacing)) - 3);

	g_messageMoreX = g_messageReadPositionX;
	g_messageMoreY = g_messageReadPositionY - 
						(g_messageIconHeight + g_messageIconSpacing) * g_messageMaxVisible;
	g_messageLessX = g_messageReadPositionX;
	g_messageLessY = g_messageReadPositionY + g_messageIconHeight + g_messageIconSpacing;

	return AUI_ERRCODE_OK;
}


MessageList *messagewin_InitializePlayerMessage( PLAYER_INDEX index )
{
	if ( !g_messageUserList ) return NULL;
	
	if ( messagewin_GetPlayerMessageList( index )) return NULL;

	MessageList *list = new MessageList(index);

	g_messageUserList->AddTail( list );

	return list;
}



MessageList *messagewin_GetPlayerMessageList( PLAYER_INDEX index )
{
	if (g_messageUserList)
    {
	    ListPos position = g_messageUserList->GetHeadPosition();

	    for (size_t count = g_messageUserList->L(); count > 0; --count) 
        {
		    MessageList * l_List = g_messageUserList->GetNext(position);
		    
            if (l_List->GetPlayer() == index)
            {
			    return l_List;
            }
        }
	}
	
	return NULL;
}














int messagewin_CreateMessage( Message data, BOOL bRecreate )
{
	if (data.IsHelpBox()) 
    {
		return 0;
	}

	g_controlPanel->AddMessage(data);

	MessageList *messagelist = messagewin_GetPlayerMessageList( data.GetOwner() );

	if ( !messagelist ) {
		
		messagelist = messagewin_InitializePlayerMessage( data.GetOwner( ));
		Assert( messagelist != NULL );
		if ( messagelist == NULL ) return -1;
	}

	AUI_ERRCODE errcode = messagelist->CreateMessage(data);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -1;

	
	if (!bRecreate) 
    {
		MBCHAR const * wavName = data.AccessData()->GetMsgSound();
		if (wavName) 
        {
			MBCHAR filename[_MAX_PATH]; 

			if (g_civPaths->FindFile(C3DIR_SOUNDS, wavName, filename))
            {
			    PlaySound(filename, NULL, SND_ASYNC | SND_FILENAME); 
            }
		}
	}

	return 1;
}



int messagewin_CreateModalMessage(Message data)
{
	messagemodal_CreateModalMessage(data);

	MBCHAR const *  wavName = data.AccessData()->GetMsgSound();
	if (wavName) 
    {
		MBCHAR filename[ _MAX_PATH ]; 
		if (g_civPaths->FindFile( C3DIR_SOUNDS, wavName, filename))
        {
		    PlaySound(filename, NULL, SND_ASYNC | SND_FILENAME); 
        }
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

	messagewin_CleanupMessage( window );

	MessageList	* messagelist = messagewin_GetPlayerMessageList(player);
	
	if (messagelist)
    {
	    messagelist->CheckVisibleMessages();
    }

	return 1;
}













int messagewin_CleanupMessage( MessageWindow *window )
{
	return messagewin_CleanupMessage(window->GetIconWindow(), window);
}



int messagewin_CleanupMessage( MessageIconWindow *iconWindow )
{
	return messagewin_CleanupMessage(iconWindow, iconWindow->GetWindow());
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





void messagewin_Cleanup(void)
{
	messagewin_PurgeMessages();
    allocated::clear(g_messageUserList);
}



void messagewin_PurgeMessages(void)
{
	if (g_messageUserList) 
    {
	    messagewin_LessMessagesIcon( FALSE, TRUE );
	    messagewin_MoreMessagesIcon( FALSE, TRUE );

	    ListPos position = g_messageUserList->GetHeadPosition();
	    for (size_t count = g_messageUserList->L(); count; --count) 
        {
		    MessageList * & messagelist = g_messageUserList->GetNext(position);
		    delete messagelist;
		    messagelist = NULL;
	    }

	    g_messageUserList->DeleteAll();
    }
}



void messagewin_EndTurn( PLAYER_INDEX index )
{
	MessageList	* messagelist = messagewin_GetPlayerMessageList(index);
	if (messagelist)
    {
	    messagelist->HideVisibleWindows();
	    messagewin_MoreMessagesIcon( FALSE );
	    messagewin_LessMessagesIcon( FALSE );
    }
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
#if 0   // CtP1 code?
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

			delete window;
			window = NULL;

			delete action;
			action = NULL;
		}
	}

	return 1;
#endif
}


int messagewin_LessMessagesIcon( BOOL make, BOOL destroy )
{

return 1;
#if 0   // CtP1 code?
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
			delete window;
			window = NULL;
			delete action;
			action = NULL;
		}
	}
	return 1;
#endif
}

int messagewin_IsModalMessageDisplayed()
{
	return g_modalMessage != NULL;
}
