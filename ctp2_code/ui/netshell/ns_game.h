//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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

#ifndef __NS_GAME_H__
#define __NS_GAME_H__

#include "ns_object.h"


class ns_Game : public ns_Object<NETFunc::Game, ns_Game>
{
public:
	ns_Game(NETFunc::Game * game);
	void Update( NETFunc::Game *game );

	char *m_locked;	
	char *m_closed;
	char *m_launched;
	char *m_name;

	int m_players;
};


#endif 
