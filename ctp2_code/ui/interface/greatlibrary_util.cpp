
#include "c3.h"
#include "greatlibrary.h"
#include "SlicObject.h"
#include "stringutils.h"
#include "Text_Hasher.h"

const MBCHAR *glutil_LoadText(const char *filename, SlicContext &so)
{
    char *text;

	
	char * lower_case_filename = new char[strlen(filename)+1];
	strcpy(lower_case_filename, filename);

	
	for (int j = 0; j < strlen(lower_case_filename); j++)
		lower_case_filename[j] = tolower(lower_case_filename[j]);

	
	text = GreatLibrary::m_great_library_info->Look_Up_Data(lower_case_filename);

    if (text == NULL) {
		delete [] lower_case_filename;
        return(NULL);
    }

	static MBCHAR interpreted[k_MAX_GL_ENTRY];
	stringutils_Interpret(text, so, interpreted);
	
	delete [] lower_case_filename;

	return interpreted;
}
