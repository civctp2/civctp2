

#include "c3.h"
#include "appstrings.h"

static MBCHAR **s_appStrings;
static sint32	s_numAppStrings;

void appstrings_Initialize(void)
{
	s_appStrings = NULL;
	s_numAppStrings = 0;

	
	FILE *inFile = fopen("appstr.txt", "rt");
	if (!inFile) return;

	s_appStrings = new MBCHAR *[APPSTR_MAX];
	s_numAppStrings = APPSTR_MAX;

	MBCHAR inStr[_MAX_PATH];

	for (sint32 i=0; i<s_numAppStrings; i++) {
		if (!fgets(inStr, _MAX_PATH, inFile)) {
			c3errors_FatalDialog("appstr.txt", "Error in appstr.txt.  Terminating app.");
		}
		inStr[strlen(inStr)-1] = '\0';
		s_appStrings[i] = new MBCHAR[sizeof(inStr) + 1];
		strcpy(s_appStrings[i], inStr);
	}

}

void appstrings_Cleanup(void)
{
	if (!s_appStrings) return;

	for (sint32 i=0; i<s_numAppStrings; i++) {
		if (s_appStrings[i])
			delete[] s_appStrings[i];
	}
	
	delete[] s_appStrings;
}

MBCHAR *appstrings_GetString(APPSTR stringID)
{
	if (stringID >= 0 && stringID < s_numAppStrings) {
		return s_appStrings[stringID];
	} else {
		return NULL;
	}
}

