







#include "c3.h"

#include "civarchive.h"
#include "player.h"
#include "Unit.h"
#include "DiplomaticRequestData.h"
#include "DiplomaticRequest.h"
#include "MessageData.h"
#include "message.h"
#include "MessagePool.h"
#include "network.h"
#include "net_info.h"

#include "aui.h"
#include "c3ui.h"
#include "messagewindow.h"
#include "messageiconwindow.h"
#include "messageactions.h"
#include "aui_button.h"

#include "SlicButton.h"
#include "DiplomaticRequestPool.h"

extern DiplomaticRequestPool *g_theDiplomaticRequestPool;

extern	Player	**g_player ;

extern	MessagePool	*g_theMessagePool ;

extern  C3UI *g_c3ui;








void Message::KillMessage()
	{
	Message	tmp(*this) ;
	tmp.RemoveAllReferences() ;
	}









void Message::RemoveAllReferences()
	{
	SlicButton *closeEvent = AccessData()->GetCloseEvent();
	if(closeEvent) {
		closeEvent->Callback();
	}

	if (g_player && g_player[GetOwner()])
		g_player[GetOwner()]->RemoveMessageReferences(*this) ;			

	AccessData()->KillMessageWindow();
	g_theMessagePool->Del(*this) ;
	}









const MessageData* Message::GetData() const
	{
	return (g_theMessagePool->GetMessage(*this)) ;
	}









MessageData* Message::AccessData()
	{
	return (g_theMessagePool->AccessMessage(*this)) ;
	}











void Message::Show()
{
	
	
	if (!AccessData()) return;
	if (!AccessData()->GetMessageWindow()) return;
	if (!AccessData()->GetMessageWindow()->GetIconWindow()) return;
	

	g_c3ui->AddAction(new MessageOpenAction(AccessData()->GetMessageWindow()->GetIconWindow()));
	
	
	
	SetRead();

}

void Message::SetSelectedAdvance(AdvanceType adv)
{
	AccessData()->SetSelectedAdvance(adv);
}

AdvanceType Message::GetSelectedAdvance() const
{
	return GetData()->GetSelectedAdvance();
}

void Message::SetDuration(sint32 duration)
{
	AccessData()->SetDuration(duration);
}

sint32 Message::GetExpiration() const
{
	return GetData()->GetExpiration();
}

void Message::MinimizeMessage() 
{
	AccessData()->GetMessageWindow()->ShowWindow( FALSE );
	AccessData()->GetMessageWindow()->GetIconWindow()->SetCurrentIconButton(NULL);
}
