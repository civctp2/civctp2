//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : String utilities
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
//       The blocks that are inactiThe blocks that are active for _MSC_VER value 
//       1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft extensions marked.
// - Made the interpretation text size a parameter to support the German 
//   version.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

class SlicContext;

#include "dbtypes.h"

#define k_MAX_INTERP_LEN 4096

void stringutils_HackColor(BOOL on);

#if defined(ACTIVISION_ORIGINAL)
void stringutils_Interpret(const MBCHAR *msg, SlicContext &slicObj, MBCHAR *sInterpreted);
#else
void stringutils_Interpret
(
	MBCHAR const *	msg,
	SlicContext &	slicObj,
	MBCHAR *		sInterpreted,
	size_t const	a_Capacity	= k_MAX_INTERP_LEN
);
#endif

void stringutils_SetStaticStringId(StringId & stringId, char * stringName);

#endif
