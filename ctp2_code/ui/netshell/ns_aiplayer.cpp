









#include "c3.h"

#include "aui_stringtable.h"
#include "netshell.h"
#include "ns_aiplayer.h"

#include "ns_tribes.h"
#include "ns_aiplayersetup.h"



int ns_AIPlayer::count = 4;
ns_AIPlayer::Struct ns_AIPlayer::list[] = {
	{STRING,	(Data)&m_name},

	{STRING,	(Data)&m_tribe},
	{INT,		(Data)&m_civpoints},
	{INT,		(Data)&m_pwpoints}
};


void ns_AIPlayer::Update( NETFunc::AIPlayer *aiplayer ) {
	m_name = aiplayer->GetName();



	nf_AIPlayer aiplayersetup( aiplayer );
	m_tribe = g_nsTribes->GetStrings()->GetString( aiplayersetup.GetTribe() );
	m_civpoints = aiplayersetup.GetCivPoints();
	m_pwpoints = aiplayersetup.GetPwPoints();
}
