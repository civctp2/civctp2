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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are active for _MSC_VER value 1200 are the original 
//       Activision code.
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


#if defined(ACTIVISION_ORIGINAL)
#define GAMEID 1503
#else
// Should match the value in anet.inf
#define GAMEID 1504	
#endif


class DestroyInitialPlayScreenAction : public aui_Action
{
public:
	DestroyInitialPlayScreenAction() {}
	virtual ~DestroyInitialPlayScreenAction() {}

#if defined(_MSC_VER)	
	virtual ActionCallback Execute;
#else
	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);
#endif
};


#endif 
