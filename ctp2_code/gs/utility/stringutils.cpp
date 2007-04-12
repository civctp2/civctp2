//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// _JAPANESE
// - Add provisions for handling SJIS characters.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - fixed for japanese by t.s. 2003.12
// - Made the interpretation text size a parameter to support the German 
//   version.
// - Removed unneeded inlcude files. (Aug 20th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "StrDB.h"
#include "SlicContext.h"
#include "stringutils.h"
#include "sliccmd.h"
#include "ysc.tab.h"

#if defined(_JAPANESE)
#include "japanese.h"
#endif

void stringutils_HackColor(BOOL on)
{
}

//----------------------------------------------------------------------------
//
// Name       : stringutils_Interpret
//
// Description: Fill the variable and language dependent parts of a string.
//
// Parameters : msg             : original string
//              slicObj         : context to look up values of variables
//              sInterpreted    : (space reserved for) interpreted string
//              a_Capacity      : size available for interpreted string
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : When the original string does not contain any variables, it 
//              will be copied literally.
//              The interpreted string is truncated at a_Capacity characters.
//              Any individual (sub)expression will still be truncated at 
//              k_MAX_INTERP_LEN characters.
//              TODO: Reimplementation with std::string/stringstream.
//              TODO: Check all calls of this function to pass the proper
//                    capacity, instead of relying on the default 
//                    k_MAX_INTERP_LEN value to work "because it worked in the
//                    original version".
//
//----------------------------------------------------------------------------
void stringutils_Interpret
(
	MBCHAR const *  msg,
	SlicContext &   slicObj,
	MBCHAR *        sInterpreted,
	size_t const    a_Capacity
)
{
	const char *input = msg;
	char *output = sInterpreted;
	MBCHAR * const	end	= sInterpreted + (a_Capacity - 1);
	char expression[k_MAX_INTERP_LEN];
	char catString[k_MAX_INTERP_LEN];
	bool filledBuiltins = false;

	// '{','}','#' have no meanings in sjis second byte.
	while(*input && (output < end)) {
#if !defined(_JAPANESE)
		if(*input != '{') {
#else
		if(*input != '{' || ( input != msg && IS_SJIS_1ST( *(input-1) ) ) ) {
#endif
			*output++ = *input++;
			continue;
		}

		const char *closeBrace = input + 1;
#if !defined(_JAPANESE)
			while(*closeBrace && *closeBrace != '}' && *closeBrace != '#' )
#else
			while(*closeBrace && ( *closeBrace != '}' && *closeBrace != '#' || IS_SJIS_1ST(*(closeBrace-1)) ) )
#endif
			closeBrace++;

		Assert(*closeBrace);
		if(!(*closeBrace)) {
			*output++ = *input++;
			continue;
		}

		
		strncpy(expression, input + 1, closeBrace - input - 1);
		expression[closeBrace - input - 1] = 0;		

		if(*closeBrace == '#') {
			const char *catStrPtr = closeBrace + 1;
#if !defined(_JAPANESE)
			while(*closeBrace && *closeBrace != '}' ) {
#else
			while(*closeBrace && ( *closeBrace != '}' || IS_SJIS_1ST(*(closeBrace-1)) ) ) {
#endif
				closeBrace++;
			}
			Assert(*closeBrace);
			if(!(*closeBrace)) {
				*output++ = *input++;
				continue;
			}
			strncpy(catString, catStrPtr, closeBrace - catStrPtr);
			catString[closeBrace - catStrPtr] = 0;
		} else {
			catString[0] = 0;
		}

		if(!filledBuiltins) {
			slicObj.FillBuiltins();
			filledBuiltins = true;
		}

		sint32 res;
		char valbuf[k_MAX_INTERP_LEN];
		res = sliccmd_parse(SLICCMD_REPLACE, expression, valbuf, k_MAX_INTERP_LEN, 1, catString);
		Assert(res == 0);
		if(res != 0) {
			*output++ = *input++;
			continue;
		}

		char *valptr = valbuf;
		while(*valptr && (output < end)) {
			*output++ = *valptr++;
		}
		input = closeBrace + 1;
	}
	*output = '\0';
}


void stringutils_SetStaticStringId(StringId & stringId, char * stringName)
{
	if (stringId < 0)
		{
			BOOL found = 
				g_theStringDB->GetStringID(stringName, stringId);
			Assert(found);
		}
}
