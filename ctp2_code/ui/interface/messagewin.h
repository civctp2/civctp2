#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __MESSAGEWIN_H__
#define __MESSAGEWIN_H__

#define SHOW_MESSAGE_OFFSET_ABSOLUTE 0
#define SHOW_MESSAGE_OFFSET_RELATIVE 1

#include "player.h"     // PLAYER_INDEX

class Message;
class MessageList;
class MessageWindow;
class MessageIconWindow;

#define BOUNCE_IT		g_theProfileDB->IsBounceMessage()


AUI_ERRCODE messagewin_InitializeMessages( void );
MessageList *messagewin_InitializePlayerMessage( PLAYER_INDEX index );

int messagewin_CreateMessage( Message data, BOOL bRecreate = FALSE );
int messagewin_CreateModalMessage( Message data );

MessageList *messagewin_GetPlayerMessageList( PLAYER_INDEX index );
int messagewin_PrepareDestroyWindow( MessageWindow *window );
int messagewin_FastKillWindow(MessageWindow *window);
int messagewin_CleanupMessage( MessageIconWindow *iconWindow );
int messagewin_CleanupMessage( MessageWindow *window );
int messagewin_CleanupMessage( MessageIconWindow *iconWindow, MessageWindow *window );
void messagewin_PurgeMessages(void);
void messagewin_Cleanup(void);

void messagewin_BeginTurn( PLAYER_INDEX index );
void messagewin_EndTurn( PLAYER_INDEX index );
int messagewin_MoreMessagesIcon( BOOL make, BOOL destroy = FALSE );
int messagewin_LessMessagesIcon( BOOL make, BOOL destroy = FALSE );

int messagewin_IsModalMessageDisplayed();

#endif
