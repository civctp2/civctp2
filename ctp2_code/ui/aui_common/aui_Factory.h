//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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

class aui_Factory {
public:
	static aui_Surface *new_Surface(AUI_ERRCODE &retval,
	                                const sint32 &width, const sint32 &height,
	                                const sint32 &bpp, void *data = 0,
					const BOOL &isPrimary = FALSE
	                               );
};

#endif

