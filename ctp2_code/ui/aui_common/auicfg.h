//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifndef __AUICFG_H__
#define __AUICFG_H__

#if defined(WIN32)
#define __AUI_USE_DIRECTX__ 1
#define __AUI_USE_DIRECTMEDIA__ 1
#else
#define __AUI_USE_SDL__ 1
#endif

#include "aui_gamespecific.h"


#endif 
