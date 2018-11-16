//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Multiplayer game handling
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
// - Standardised function declaration syntax.
// - Patch 1.1 reimplementation: updated GAMEID.
//
//----------------------------------------------------------------------------

#ifndef __NETSHELL_GAME_H__
#define __NETSHELL_GAME_H__

#include "aui_action.h"

// Should match the value in anet.inf
#define GAMEID 1504

class DestroyInitialPlayScreenAction : public aui_Action
{
public:
	DestroyInitialPlayScreenAction() {}
	virtual ~DestroyInitialPlayScreenAction() {}

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);
};

#endif
