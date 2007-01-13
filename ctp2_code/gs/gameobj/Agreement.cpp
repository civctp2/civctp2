








#include "c3.h"

#include "civarchive.h"
#include "gold.h"
#include "player.h"
#include "Unit.h"
#include "AgreementData.h"
#include "Agreement.h"
#include "AgreementPool.h"
#include "network.h"
#include "net_info.h"
#include "net_action.h"

	extern	Player	**g_player ;

	extern	AgreementPool*	g_theAgreementPool ;









void Agreement::KillAgreement()
	{
	Agreement	tmp(*this) ;
	tmp.RemoveAllReferences() ;
	}









void Agreement::RemoveAllReferences()
{
	if(g_player[GetRecipient()]) {
		g_player[GetRecipient()]->RemoveAgreementReferences(*this) ;	
	}
	if(g_player[GetOwner()]) {
		g_player[GetOwner()]->RemoveAgreementReferences(*this) ;		
	}

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_AGREEMENT,
									  m_id));
	} else if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_KILL_AGREEMENT,
										   m_id));
	}

	g_theAgreementPool->Del(*this) ;
}


























const AgreementData* Agreement::GetData() const
	{
	return (g_theAgreementPool->GetAgreement(*this)) ;
	}









AgreementData* Agreement::AccessData()
	{
	return (g_theAgreementPool->AccessAgreement(*this)) ;
	}

void Agreement::Break()
{
	AccessData()->Break();
}

BOOL Agreement::IsBroken() const
{
	return GetData()->IsBroken();
}

void Agreement::BeginTurnOwner()
{
	AccessData()->BeginTurnOwner();
}

void Agreement::BeginTurnRecipient()
{
	AccessData()->BeginTurnRecipient();
}
