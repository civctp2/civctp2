#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __MESSAGELIST_H__
#define __MESSAGELIST_H__

#include "tech_wllist.h"

class Message;
class MessageIconWindow;
class MessageWindow;


class MessageList
{
public:
	MessageList( PLAYER_INDEX player );

	virtual ~MessageList();

	PLAYER_INDEX	GetPlayer( void ) { return m_player; }
	tech_WLList<MessageIconWindow *>	*GetList( void ) { return m_iconList; }

	AUI_ERRCODE CreateMessage( Message data );

	void HideVisibleWindows( void );
	void CheckVisibleMessages( void );
	void CheckMaxMessages( void ); 
	void ChangeOffset( sint32 offset, int flag );
	void Remove( MessageIconWindow *iconWindow, MessageWindow *window );

	uint32 GetOffset( void ) { return m_offset; }

private:
	PLAYER_INDEX							m_player;
	tech_WLList<MessageIconWindow *>		*m_iconList;
	uint32									m_offset;  
	
};




#endif 
