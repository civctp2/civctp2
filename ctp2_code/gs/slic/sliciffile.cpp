#include "c3.h"
#include "slicif.h"
#include "CivPaths.h"
#include "c3errors.h"
#include "StrDB.h"

extern CivPaths *g_civPaths;
extern StringDB *g_theStringDB;

int slicif_find_file(char *filename, char *fullpath)
{
	if(!g_civPaths->FindFile(C3DIR_GAMEDATA, filename, fullpath))
		return 0;
	return 1;
}

void slicif_report_error(char *s)
{
	c3errors_ErrorDialog("SLIC", s);
}

int slicif_is_valid_string(char *s)
{
	return g_theStringDB->GetNameStr(s) != NULL;
}
