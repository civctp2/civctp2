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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
#include "ns_aiplayer.h"

#include "ns_tribes.h"
#include "ns_aiplayersetup.h"



#if defined(ACTIVISION_ORIGINAL)
int ns_AIPlayer::count = 4;
ns_AIPlayer::Struct ns_AIPlayer::list[] = {
	{STRING,	(Data)&m_name},

	{STRING,	(Data)&m_tribe},
	{INT,		(Data)&m_civpoints},
	{INT,		(Data)&m_pwpoints}
};
#else
ns_AIPlayer::ns_AIPlayer(NETFunc::AIPlayer * player) 
:	ns_Object<NETFunc::AIPlayer, ns_AIPlayer>(player) 
{
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(STRING,	&m_tribe));
	list.push_back(Struct(INT,		&m_civpoints));
	list.push_back(Struct(INT,		&m_pwpoints));
};
#endif


void ns_AIPlayer::Update( NETFunc::AIPlayer *aiplayer ) {
	m_name = aiplayer->GetName();



	nf_AIPlayer aiplayersetup( aiplayer );
	m_tribe = g_nsTribes->GetStrings()->GetString( aiplayersetup.GetTribe() );
	m_civpoints = aiplayersetup.GetCivPoints();
	m_pwpoints = aiplayersetup.GetPwPoints();
}
