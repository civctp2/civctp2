/*
	fixed for japanese by t.s. 2003.12

	fix
		void stringutils_Interpret(const MBCHAR *msg, SlicContext &slicObj, MBCHAR *sInterpreted)
*/

#include "c3.h"

#include "StrDB.h"
#include "AgeRecord.h"
#include "CivilisationDB.h"
#include "WonderRecord.h"
#include "AdvanceRecord.h"
#include "GovernmentRecord.h"

#include "TurnCnt.h"

#include "Civilisation.h"
#include "Player.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "MessageData.h"

#include "SlicContext.h"
#include "RandGen.h"

#include "pixelutils.h"
#include "ColorSet.h"

#include "stringutils.h"
#include "BuildingRecord.h"

#include "SlicSymbol.h"
#include "SlicEngine.h"
#include "SlicNamedSymbol.h"
#include "UnitRecord.h"
#include "GovernmentRecord.h"
#include "ResourceRecord.h"

#include "sliccmd.h"
#include "sc.tab.h"

#if defined(_JAPANESE)
#include "japanese.h"
#endif

void stringutils_HackColor(BOOL on)
{
}



void stringutils_Interpret(const MBCHAR *msg, SlicContext &slicObj, MBCHAR *sInterpreted)
{
	const char *input = msg;
	char *output = sInterpreted;
	char *end = sInterpreted + k_MAX_INTERP_LEN - 1;

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
