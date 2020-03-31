/**
 * $Id$
 */

#include "c3.h"
#include "appstrings.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "c3errors.h"
#include "ctp2_config.h"
#include "c3types.h"

static MBCHAR **    s_appStrings        = NULL;
static size_t       s_numAppStrings     = 0;

void appstrings_Initialize(void)
{
	FILE *inFile = fopen("appstr.txt", "rt");
	if (!inFile)
	{
		c3errors_FatalDialog("appstr.txt", "Unable to open appstr.txt. Terminating app.");
	}

	s_appStrings = new MBCHAR *[APPSTR_MAX];
	s_numAppStrings = APPSTR_MAX;

	MBCHAR inStr[_MAX_PATH];

	for (size_t i=0; i<s_numAppStrings; i++)
	{
		if (!fgets(inStr, _MAX_PATH, inFile))
		{
			c3errors_FatalDialog("appstr.txt", "Error in appstr.txt.  Terminating app.");
		}

		size_t len = strlen(inStr);
		inStr[--len] = '\0';
		if(inStr[len - 1] == '\r')
			inStr[--len] = '\0';
		s_appStrings[i] = new MBCHAR[len + 1];
		strcpy(s_appStrings[i], inStr);
	}
}

void appstrings_Cleanup(void)
{
	if (!s_appStrings) return;

	for (size_t i = 0; i < s_numAppStrings; i++)
	{
		delete [] s_appStrings[i];
	}

	delete[] s_appStrings;
	s_appStrings    = NULL;
	s_numAppStrings = 0;
}

MBCHAR *appstrings_GetString(APPSTR stringID)
{
	if (stringID >= 0 && static_cast<size_t>(stringID) < s_numAppStrings)
	{
		return s_appStrings[stringID];
	}
	else
	{
		return NULL;
	}
}
