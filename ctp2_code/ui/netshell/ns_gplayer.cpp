//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_stringtable.h"
#include "netshell.h"
#include "ns_gplayer.h"

#include "ns_tribes.h"
#include "ns_playersetup.h"



ns_GPlayer::ns_GPlayer(NETFunc::Player * player) 
:	ns_Object<NETFunc::Player, ns_GPlayer>(player) 
{
	list.push_back(Struct(ICON,		&m_host));
	list.push_back(Struct(ICON,		&m_launched));
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(INT,		&m_ping));
	list.push_back(Struct(STRING,	&m_tribe));
	list.push_back(Struct(INT,		&m_civpoints));
	list.push_back(Struct(INT,		&m_pwpoints));
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
