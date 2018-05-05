#include "ctp/c3.h"

#include "robot/aibackdoor/civarchive.h"
#include "gs/gameobj/Player.h"
#include "gs/gameobj/Unit.h"
#include "gs/gameobj/DiplomaticRequestData.h"
#include "gs/gameobj/DiplomaticRequest.h"
#include "gs/gameobj/MessageData.h"
#include "gs/gameobj/message.h"
#include "gs/gameobj/MessagePool.h"
#include "net/general/network.h"
#include "net/general/net_info.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_ctp2/c3ui.h"
#include "ui/interface/messagewindow.h"
#include "ui/interface/messageiconwindow.h"
#include "ui/interface/messageactions.h"
#include "ui/aui_common/aui_button.h"

#include "gs/slic/SlicButton.h"
#include "gs/gameobj/DiplomaticRequestPool.h"

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
