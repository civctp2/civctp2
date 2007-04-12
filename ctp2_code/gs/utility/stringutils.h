//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : String utilities
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
// - Removed unneeded inlcude file. (Aug 20th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

class SlicContext;

#define k_MAX_INTERP_LEN 4096

void stringutils_HackColor(BOOL on);

void stringutils_Interpret
(
	MBCHAR const *  msg,
	SlicContext &   slicObj,
	MBCHAR *        sInterpreted,
	size_t const    a_Capacity = k_MAX_INTERP_LEN
);

void stringutils_SetStaticStringId(StringId & stringId, char * stringName);

#endif

