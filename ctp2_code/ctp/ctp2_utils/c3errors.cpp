//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source file
// Description  : Civilisation 3 error dialogs
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
// _DEBUG
// - Generate debug version
//
// _BFR_
// - Force CD checking when set (build final release).
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added to the database error dialog the possibility to close the program.
//   This dialog is also used for slic errors and therefore also very useful.
//   (Aug 26th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"

#include "aui_ui.h"
#include "StrDB.h"      // g_theStringDB

extern aui_UI *g_ui;

void c3errors_FatalDialog(const char* module, const char* fmt, ...)
{
	va_list		list;
	char str[_MAX_PATH];

    va_start(list, fmt);
	vsprintf(str, fmt, list);
	va_end(list);

	c3errors_ErrorDialog(module, str);
	va_end(list);

	Assert(FALSE);

	Report("Fatal error.  Aborting.\n");

#ifdef WIN32
#ifndef _DEBUG
#ifndef _BFR_
	sint32 *s = 0;
	*s = 0;
#endif
#endif
#endif

	exit(-1);
}

void c3errors_FatalDialogFromDB(const char *module, const char *err, ...)
{
	MBCHAR *    dbTitle;
	if (!g_theStringDB->GetText(module, &dbTitle))
		c3errors_FatalDialog("string db", "%s missing from string db", module);

	MBCHAR *    dbError;
	if (!g_theStringDB->GetText(err, &dbError))
		c3errors_FatalDialog("string db", "%s missing from string db", err) ;

	va_list		list;
	MBCHAR	    str[_MAX_PATH];

	// TODO: I've changed the second argument in the following from dbError (which made no sense)
	//   into err.  I think that this is what was originally intended, but since the feature this
	//   code implements is never actually used anywhere, I expect it makes little difference.
	//   nevertheless, that this works should be checked at some point.  The same applies to the
	//   next function (c3errors_ErrorDialogFromDB) - JJB
	va_start(list, err) ;
	vsprintf(str, dbError, list) ;
	va_end(list) ;

	MessageBox(NULL, str, dbTitle, MB_OK | MB_ICONEXCLAMATION);

	Assert(FALSE);

	Report("Fatal error.  Aborting.\n");

#if defined(WIN32)
#ifndef _DEBUG
#ifndef _BFR_
	sint32 *s = 0;
	*s = 0;
#endif
#endif
#endif

	exit(-1) ;
}

void c3errors_ErrorDialogFromDB(const char *module, const char *err, ...)
{
    MBCHAR *    dbTitle;
	if (!g_theStringDB->GetText(module, &dbTitle))
		c3errors_FatalDialog("string db", "%s missing from string db", module) ;

    MBCHAR *    dbError;
	if (!g_theStringDB->GetText(err, &dbError))
		c3errors_FatalDialog("string db", "%s missing from string db", err) ;

	va_list		list;
	MBCHAR	    str[_MAX_PATH];
	va_start(list, err);
	vsprintf(str, dbError, list);
	va_end(list);

	MessageBox(NULL, str, dbTitle, MB_OK | MB_ICONEXCLAMATION) ;
}

void c3errors_ErrorDialog(const char* module, const char* fmt, ...)
{
	LPTSTR			szTitle;
	LPCTSTR			szTitleText = "%s Error";

    LPCTSTR  szTmp = (module) ? (LPCTSTR) module : (LPCTSTR) "CTP 2";

#if defined(WIN32)
	if ((szTitle = (LPTSTR)LocalAlloc(LMEM_FIXED, (lstrlen(szTmp) +
			lstrlen(szTitleText) + lstrlen(fmt) + 33000)*sizeof(TCHAR))) == NULL)
		return;

   wsprintf(szTitle, szTitleText, szTmp);
#else
   if ((szTitle = (LPTSTR)malloc((lstrlen(szTmp) + lstrlen(szTitleText) +
                                  lstrlen(fmt) + 33000
                                 )*sizeof(TCHAR)
                                )
       ) == NULL)
      return;

   sprintf(szTitle, szTitleText, szTmp);
#endif

	LPTSTR  szFmtTmp    = szTitle + (lstrlen(szTitle)+2)*sizeof(TCHAR);

	va_list list;
	va_start(list, fmt);
	vsprintf(szFmtTmp, fmt, list);
	char Tmp[2000];
	sprintf(Tmp, "%s\n\nContinue?", szFmtTmp);
	va_end(list);

	// TODO: Make it work with LPTSTR szFmtTmp if it is worth the efforts at all.
//	MessageBox(NULL, szFmtTmp, szTitle, MB_OK | MB_ICONEXCLAMATION);
	sint32 result = MessageBox(NULL, Tmp, szTitle, MB_YESNO | MB_ICONEXCLAMATION);

	DPRINTF(k_DBG_FIX, ("Error: %s, %s\n", szTitle, szFmtTmp));

#if defined(WIN32)
	LocalFree(szTitle);
#else
   free(szTitle);
#endif

#ifndef _DEBUG
	extern bool g_autoAltTab;
	if(g_autoAltTab && g_ui) {
		g_ui->AltTabIn();
	}
#endif

	if (result == IDNO) {
		exit(1);
	}

	return;
}
