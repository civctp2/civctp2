









#include "c3.h"

#include "aui_stringtable.h"
#include "netshell.h"
#include "ns_gplayer.h"

#include "ns_tribes.h"
#include "ns_playersetup.h"



int ns_GPlayer::count = 7;
ns_GPlayer::Struct ns_GPlayer::list[] = {
	{ICON,		(Data)&m_host},
	{ICON,		(Data)&m_launched},
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_ping},
	{STRING,	(Data)&m_tribe},

	{INT,		(Data)&m_civpoints},
	{INT,		(Data)&m_pwpoints}
};


void ns_GPlayer::Update( NETFunc::Player *player ) {
	SetMine(player->IsMe());
	m_launched = player->IsReadyToLaunch() ? g_netshell->GetTrueBmp() : 0;
	m_host = player->IsHost() ? g_netshell->GetTrueBmp() : 0;
	m_name = player->GetName();
	m_ping = player->GetLatency();



	nf_PlayerSetup ps( player );
	m_tribe = g_nsTribes->GetStrings()->GetString( ps.GetTribe() );
	m_civpoints = ps.GetCivPoints();
	m_pwpoints = ps.GetPwPoints();
}
