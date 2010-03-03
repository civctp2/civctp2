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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef DB_TYPES_H_
#define DB_TYPES_H_

#include "ctp2_inttypes.h"  // sint32

typedef sint32 StringId; 
StringId const STRING_INDEX_INVALID = -1;
typedef sint32 PopDBIndex;

#define k_MAX_TEXT_LEN 16384
#define k_MAX_NAME_LEN 512

#define k_NUM_MERIDIANS 6

#endif
