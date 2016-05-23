#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __REGARD_H__
#define __REGARD_H__ 1

class CivArchive;

#define k_REGARD_VERSION_MAJOR	0
#define k_REGARD_VERSION_MINOR	0

#include "gstypes.h"

class Regard
	{

	public:

		REGARD_TYPE	m_regard[k_MAX_PLAYERS] ;









	public:
		Regard() ;
		void SetForPlayer(const PLAYER_INDEX player, const REGARD_TYPE regard) ;
		REGARD_TYPE GetForPlayer(PLAYER_INDEX player) ;

        REGARD_TYPE GetUpdatedRegard(const PLAYER_INDEX me, const PLAYER_INDEX him);

		void Serialize(CivArchive &archive) ;
	} ;

	uint32 Regard_Regard_GetVersion(void) ;
#endif
