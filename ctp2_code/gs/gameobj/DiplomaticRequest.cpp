#include "ctp/c3.h"

#include "robot/aibackdoor/civarchive.h"
#include "gs/gameobj/Player.h"
#include "gs/gameobj/Unit.h"
#include "gs/gameobj/DiplomaticRequestData.h"
#include "gs/gameobj/DiplomaticRequest.h"
#include "gs/gameobj/DiplomaticRequestPool.h"
#include "net/general/network.h"
#include "net/general/net_info.h"
#include "gs/outcom/AICause.h"

extern	Player	**g_player ;

extern	DiplomaticRequestPool	*g_theDiplomaticRequestPool ;









void DiplomaticRequest::KillRequest()
	{
	DiplomaticRequest	tmp(*this) ;
	tmp.RemoveAllReferences() ;
	}









void DiplomaticRequest::RemoveAllReferences()
	{
	if(g_network.IsHost())
		{
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_DIP_REQUEST,
									  (uint32)m_id));
		}
    sint32 r = GetRecipient();

	if(g_player[r]) {
		g_player[r]->RemoveDiplomaticReferences(*this) ;
	}

    sint32 o = GetOwner();

	if(g_player[o]) {
		g_player[o]->RemoveDiplomaticReferences(*this) ;
	}

	g_theDiplomaticRequestPool->Del(*this) ;
	}









const DiplomaticRequestData* DiplomaticRequest::GetData() const
	{
	return (g_theDiplomaticRequestPool->GetDiplomaticRequest(*this)) ;
	}









DiplomaticRequestData* DiplomaticRequest::AccessData()
	{
	return (g_theDiplomaticRequestPool->AccessDiplomaticRequest(*this)) ;
	}

void DiplomaticRequest::Complete()
{
	AccessData()->Complete();
}

sint32 DiplomaticRequest::GetTone() const
{
	return GetData()->GetTone();
}

sint32 DiplomaticRequest::GetRound() const
{
	return GetData()->GetRound();
}
