//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Loads and interprets Great Library strings
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Made the interpretation text size a parameter to support the German 
//   version.
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "greatlibrary.h"
#include "SlicObject.h"
#include "stringutils.h"
#include "text_hasher.h"

//----------------------------------------------------------------------------
//
// Name       : glutil_LoadText
//
// Description: Load and interpret a library string.
//
// Parameters : filename            : (case insensitive) library string name 
//              so                  : context to look up values of variables
//                                    during interpretation
//
// Globals    : GreatLibrary::m_great_library_info
//                                  : library (string) data
//
// Returns    : MBCHAR const *      : pointer to an interpreted string
//
// Remark(s)  : When the library string name is not found, NULL is returned.
//
//----------------------------------------------------------------------------
const MBCHAR *glutil_LoadText(const char *filename, SlicContext &so)
{
	char *text;
	
	char * lower_case_filename = new char[strlen(filename)+1];
	strcpy(lower_case_filename, filename);
	
	for (size_t j = 0; j < strlen(lower_case_filename); ++j)
	{
		lower_case_filename[j] = tolower(lower_case_filename[j]);
	}
	
	text = GreatLibrary::s_great_library_info->Look_Up_Data(lower_case_filename);
	delete [] lower_case_filename;

	if (text) 
	{
		static MBCHAR interpreted[k_MAX_GL_ENTRY];
		stringutils_Interpret(text, so, interpreted, k_MAX_GL_ENTRY);
		return interpreted;
	}
	else
	{
		return NULL;
	}
}
