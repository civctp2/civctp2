









#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_ldl.h"
#include "aui_button.h"

#include "message.h"
#include "messagedata.h"
#include "messagepool.h"

#include "messageiconwindow.h"
#include "messagewindow.h"
#include "messagewin.h"

#include "test.h"

#include "c3ui.h"

extern C3UI			*g_c3ui;
extern	MessagePool					*g_theMessagePool;

BOOL bInsult = FALSE;


void Button1Action::Execute( aui_Control *control, uint32 action, uint32 data )
{
	static sint32 count = 0;
	MBCHAR			string[ 256 ];

	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	if ( bInsult ) {
		strcpy( string, "hey, i don't have to take that kind of crap from any lame diplomatic messages!" );
		bInsult = FALSE;
	} else {

		switch( count++ )
		{
		case 0:
			strcpy( string, "Temporary 'Event' type message .. " );
			break;
		case 1:
			strcpy( string, "Well, that's pretty nifty.  Here's another 'event' type message." );
			break;
		case 2:
			strcpy( string, "hmm .. i wonder how many of these event-messages we could fit on the screen?" );
			break;
		case 3:
			strcpy( string, "boy, wont it be great when we have art for these event messages! .. it'll add so much to it!" );
			break;
		case 4:
			strcpy( string, "ho hum .. la-de-dah .. what to do what to do .. " );
			break;
		case 5:
			strcpy( string, "Annal-nathrak uthvas-bethod donnel dokdienthe" );
			break;
		case 6:
			strcpy( string, "okay .. that's enough, i'm going to loop through these messages." );
			count = 0;
			break;
		}
	}
	
 	Message message = g_theMessagePool->Create( 1,
									1,
									MESSAGE_TYPE_EVENT,
									string );
	messagewin_CreateMessage( message );

}


void Button2Action::Execute( aui_Control *control, uint32 action, uint32 data )
{
	if ( action != ( uint32 )AUI_BUTTON_ACTION_EXECUTE ) return;

	
 	Message message = g_theMessagePool->Create( 1,
									1,
									MESSAGE_TYPE_DIPLOMATIC,
									"This is a temporary diplomatic message.  Don't look for me to change by opening up a bunch of my type of messages .. i'm not a cheesy 'event' type message." );
	bInsult = TRUE;
	messagewin_CreateMessage( message );

}

