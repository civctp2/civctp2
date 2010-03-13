/// \file aui_Factory.h
/// \brief Factory for native aui_* Instance creation
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Factory for native aui_* Instance creation
// Id           : $Id$
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __aui_Factory__h__
#define __aui_Factory__h__ 1

#include "aui_surface.h"
#include "aui_mouse.h"
#include "aui_keyboard.h"

class aui_Factory {
public:
	static aui_Surface *new_Surface(AUI_ERRCODE &retval,
	                                const sint32 &width,
	                                const sint32 &height,
	                                void *data = 0,
	                                const BOOL &isPrimary = FALSE,
	                                const BOOL &useVideoMemory = FALSE,
	                                const BOOL &takeOwnership = FALSE
	                               );

	static aui_Mouse *new_Mouse(AUI_ERRCODE &retval,
	                            MBCHAR      *ldlBlock,
	                            const BOOL  &useExclusiveMode = FALSE
	                           );

	static aui_Keyboard *new_Keyboard(AUI_ERRCODE &retval);
};

#endif

