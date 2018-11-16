/*
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/* ***********************************************************************
 *                               RCS HEADER
 * $Locker: $
 * $Log: ini.c $
 * Revision 1.15  1997/09/22 22:16:45  lbennett
 * Removed MSVC L4 warnings.
 * Revision 1.14  1997/08/13 20:37:03  dkegel
 * Added way to save and restore the name of the current ini file.
 * Used by win/dp/enumapp.c.  (Still can't save/restore the current section.)
 * Revision 1.13  1997/08/12 23:17:30  dkegel
 * ARRGH - can't rely on global IniFileName, so hardcode it.
 * Might be other statics screwed up... watch out...
 * Revision 1.12  1997/07/22 22:44:15  dkegel
 * Fixed bug caused by lack of newline on last line of file (notepad does this)
 * Revision 1.11  1997/05/29 19:39:00  dkegel
 * Fixed minor type mismatches caught by Codewarrior.
 * Revision 1.10  1997/03/05 02:39:26  dkegel
 * ini.h and dprint.h now integrated into anet.h;
 * to compile or use DP as a DLL, define DP_DLL;
 * win\build.bat now creates debugging and nondebugging versions
 * of both the static dp.lib and the dynamic anetdll.dll.
 * Revision 1.9  1997/02/21 05:44:22  dkegel
 * Fixed crash bug trying to write to constant string.
 * Revision 1.8  1997/02/16 23:02:50  dkegel
 * Added dpini_SetFile.
 * Revision 1.7  1996/12/18 19:22:03  dkegel
 * Changed #include "Ini.h" to all lowercase to accomodate case sensitive
 * file systems.
 * Revision 1.6  1996/12/18 19:14:13  administrator
 * Changed to c style comments to accomodate gcc.
 * Revision 1.5  1996/12/16 18:39:46  administrator
 * Don't modify arg to stripSurroundingWhiteSpace
 * if it doesn't need it.  This works around a crash bug under unix
 * when called with constant strings.
 * Revision 1.4  1996/02/17 03:25:20  dkegel
 * Allow makefile to override ini filename
 * Revision 1.3  1995/10/27 11:11:52  dzobel
 * Caller can choose whether to strip surrounding white space and comments.
 * Fixed bug which ignored all .INI sections after the first.
 * Revision 1.2  1995/08/16 15:03:26  dzobel
 * Discard leading and trailing white space.
 * Revision 1.1  1994/11/09 10:02:51  ADMINISTRATOR
 * Initial revision
 * Revision 1.1  1994/08/31  10:08:04  clarkej
 * Initial revision
 *
 * Revision 1.1  1994/07/07  22:37:49  zobeld
 * Initial revision
 *
 * *********************************************************************** */

/*
	INI.C
	.INI file.
	dz 7/7/94
*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>		/* Character handling */
#include <limits.h>
#include <string.h>
#ifdef __MWERKS__
#include "stricmp.h"
#endif

#include "dp.h"		/* .INI file */

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/* Where we read the currently-installed app info */
#define INIFile "anet.inf"
#define	INI_MAXLINELEN	132				/* Maximum length of a .INI file line */
#ifndef MAX_PATH
#define MAX_PATH 255
#endif

/* What .ini file to read for options */
static char IniFileName[INI_MAXLINELEN] = dpini_DEFAULT_FILENAME;

/* Offset within the .INI file (ULONG_MAX = EOF) */
static unsigned long sectionOffset = ULONG_MAX;	/* Line after section header */

/*--------------------------------------------------------------------------
 Change the .INI file accessed by future calls.
--------------------------------------------------------------------------*/
DP_API void dpini_SetFile(
	char *iniFileName)
{
	strncpy(IniFileName, iniFileName, sizeof(IniFileName));
	IniFileName[sizeof(IniFileName)-1] = 0;
	DPRINT(("dpini_SetFile: IniFileName=%s\n", IniFileName));
}

/*--------------------------------------------------------------------------
 Return the name of the current .INI file.
 Value is a pointer to a static string; the caller should copy it
 to a buffer immediately.
--------------------------------------------------------------------------*/
DP_API const char *dpini_GetFile(void)
{
	DPRINT(("dpini_GetFile: IniFileName=%s\n", IniFileName));
	return IniFileName;
}

/*
 *	Strip leading and trailing white space off a string.  Leave embedded space
 *	as is.  The stripping is done in place.
 *
 *	Return a pointer to the stripped string.
 */

static char *
stripSurroundingWhiteSpace(
	char *	s)				/* The string */
{
	int i;					/* Temp */

	if (s != NULL) {
		while (isspace(*s))		/* Strip off leading whitespace */
			++s;

		i = strlen(s);			/* Strip off trailing whitespace */
		while (i-- > 0 && isspace(s[i]))
			;
		if (i != (int) strlen(s) - 1)
			s[i + 1] = '\0';
	}

	return (s);
}
/*--------------------------------------------------------------------------
 Search the current .INI file for a section header.
 Section headers are case-insensitive.

 Return non-zero on any error.
--------------------------------------------------------------------------*/
DP_API int
dpini_findSection(
	char *	sectionWant)	/* Section we want, minus "[" and "]"; case-blind */
{
	char	line[INI_MAXLINELEN + 1];	/* Line we've read from .INI file */
	FILE *	fp;							/* .INI file */
	char *	sectionGot;					/* Section header we got */
	char *	sectionEnd;					/* End of section name */

	sectionOffset = ULONG_MAX;			/* Assume we won't find the section */

										/* Open .INI file */
	if ((fp = fopen(IniFileName, "rt")) == NULL) {
        /* Cannot call DPRINT from this because DPRINT calls us */
        /* DPRINT(("dpini_findSection: Cannot open file %s\n", IniFileName)); */
		return (__LINE__);
    }

									/* Strip white space off section name */
	sectionWant = stripSurroundingWhiteSpace(sectionWant);

	for ( ; ; ) {				/* Search for the section header */
									/* Quit at end of file */
		if (fgets(line, sizeof(line), fp) == NULL) {
            /* Cannot call DPRINT from this because DPRINT calls us */
            /* DPRINT(("dpini_findSection: Section %s not found in %s\n", sectionWant, IniFileName)); */
			fclose(fp);
			return (__LINE__);
		}

									/* Skip lines not starting with '[' */
		if (*(sectionGot = line + strspn(line, " \t")) != '[')
			continue;
									/* Skip section name if not followed by ']' */
		if ((sectionEnd = strchr(++sectionGot, ']')) == NULL)
			continue;
		*sectionEnd = '\0';			/* NUL-terminate section name */
									/* Done if found a matching section name */
		if (stricmp(stripSurroundingWhiteSpace(sectionGot), sectionWant) == 0)
			break;
	}

	sectionOffset = ftell(fp);			/* Remember where we are */
	fclose(fp);							/* Done with the .INI file */

	return (0);
}

/*--------------------------------------------------------------------------
 Search the current section of the current .INI file for the parameter.
 Parameter names are case-insensitive.
 If dpini_findSection() has not yet been called, returns the empty string.

 If the {verbatim} parameter is true, returns a pointer to the entire rest
 of the line following the "=" sign in the .INI file.
 If {verbatim} is false, any comment beginning with a semicolon is stripped
 out, and white space surrounding the parameter value is deleted.
 In either case, the returned string ends with a NUL but no CR or LF.

 The next call to readINIParameter() will overwrite the buffer.
--------------------------------------------------------------------------*/

DP_API const char *
dpini_readParameter(
	char *	paramWant,		/* Parameter we want, minus "="; case-blind */
	int		verbatim)		/* Return everything after the "=" (else clean)? */
{
	FILE *		fp;							/* .INI file */
	char *		paramGot;					/* Parameter we got */
	char *		valueGot;					/* Value we got; everything but '=' */
	size_t		len;						/* Length of value we got */

	static char	line[INI_MAXLINELEN + 1];	/* Line we've read from .INI file */

	if (sectionOffset == ULONG_MAX	/* If we haven't chosen a section yet, quit */
			|| (fp = fopen(IniFileName, "rt")) == NULL)	/* Open .INI file */
		return ("");

	fseek(fp, sectionOffset, SEEK_SET);	/* Skip to place to start search from */

									/* Strip white space off parameter name */
	paramWant = stripSurroundingWhiteSpace(paramWant);

	for ( ; ; ) {				/* Search for the parameter in current section */
		valueGot = "";				/* Assume we'll read nothing */
									/* Quit at file end */
		if (fgets(line, sizeof(line), fp) == NULL)
			break;
									/* Quit at section end */
		if (*(paramGot = line + strspn(line, " \t")) == '[')
			break;
									/* Skip parameter if not followed by '=' */
		if ((valueGot = strchr(paramGot, '=')) == NULL)
			continue;
		*valueGot++ = '\0';			/* NUL-terminate parameter name */
									/* Done if found a matching parameter name */
		if (stricmp(stripSurroundingWhiteSpace(paramGot), paramWant) == 0)
			break;
	}

	fclose(fp);							/* Done with the .INI file */
	if (!valueGot[0]) return valueGot;	/* Return before we futz if empty */

	if (((len = strlen(valueGot)) > 0)	/* Delete any newline terminator */
	&&  (valueGot[len - 1] == '\n'))
		valueGot[len - 1] = '\0';

	if (!verbatim) {					/* Clean up the parameter value? */
										/* Delete any comment */
		valueGot[strcspn(valueGot, ";")] = '\0';
										/* Strip surrounding white space */
		valueGot = stripSurroundingWhiteSpace(valueGot);
	}

	return (valueGot);
}

/*----------------------------------------------------------------------
 Takes a relative path and current working directory (eg. ../foo/foo.exe
 and c:/mw/foo1) and makes it an absolute path (eg. c:/mw/foo/foo.exe).
----------------------------------------------------------------------*/
static dp_result_t reltoabspath(char *relpath, char *cwd) {
	char buf[MAX_PATH];
	char rbuf[MAX_PATH];
	char *tmpbuf;
	char *pbuf;
	char *prbuf;

	pbuf = &buf[0];
	prbuf = &rbuf[0];
	strcpy(pbuf, cwd);
	strcpy(prbuf, relpath);

	/* strip trailing / or \ */
	if ((*(pbuf + strlen(pbuf)-1) == '/')
	||  (*(pbuf + strlen(pbuf)-1) == '\\')) *(pbuf + strlen(pbuf)-1) = '\0';
	if ((*(prbuf + strlen(prbuf)-1) == '/')
	||  (*(prbuf + strlen(prbuf)-1) == '\\')) *(prbuf + strlen(prbuf)-1) = '\0';

	while (*prbuf == '.') {
		if (*(prbuf + 1) == '.') {
			if ((*(prbuf + 2) == '\\') || (*(prbuf + 2) == '/')
			||  !*(prbuf + 2)) {
				prbuf += 3;
				/* tmpbuf = strrchr(pbuf, '/' || '\\') */
				for (tmpbuf = pbuf + strlen(pbuf)-1;
					 (*tmpbuf != '/') && (*tmpbuf != '\\');
					 tmpbuf--) {
					if (tmpbuf < pbuf) {
						tmpbuf = NULL;
						break;
					}
				}
				if (tmpbuf) {
					*tmpbuf = '\0';
				} else {
					return dp_RES_BUG;
				}
			} else {
				return dp_RES_BUG;
			}
		} else if((*(prbuf + 1) == '/') || (*(prbuf + 1) == '\\')) {
			prbuf += 2;
		} else {
			break;
		}
	}
	strcat(pbuf, "/");
	strcpy(relpath, pbuf);
	strcat(relpath, prbuf);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Gets appParam parameters from an anet.inf file.
 path is name of directory containing anet.inf.
 Caller must allocate space for strings pointed to by pAppParam.
 Not needed by most user programs.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReadAnetInf(const char *path, dp_appParam_t *pAppParam)
{
	int err;
	char ini[MAX_PATH];
	char oldinibuf[MAX_PATH];
	const char *oldini;
	const char* buf;
	char *p;

	assert(pAppParam);
	assert(pAppParam->cwd);
	assert(pAppParam->name);
	assert(pAppParam->path);
	assert(pAppParam->args);
	assert(pAppParam->shellOpts);

	/* Locate the directory containing the .ini file */
	strcpy(ini, path);
	p = ini+strlen(ini)-1;
	if ((*p == '\\') || (*p == '/'))
		*p = 0;

	/* Save it for caller */
	strcpy(pAppParam->cwd, ini);

	/* Open the .ini file and find the [ActiveNet] section */
	strcat(ini, "/");
	strcat(ini, INIFile);
	oldini = dpini_GetFile();
	strcpy(oldinibuf, oldini);
	DPRINT(("dpReadAnetInf: opening %s\n", ini));
	dpini_SetFile(ini);
	err = dpini_findSection("ActiveNet");
	if (err) {
		DPRINT(("dpini_findSection: error on line %d of ini.c\n", err));
		err = dp_RES_EMPTY;
	} else {
		/* Read info from the .ini file */
        unsigned int val, val2; /* dummies to prevent sscanf from overwriting */
		strcpy(pAppParam->name, dpini_readParameter("Name", 0));
		strcpy(pAppParam->path, dpini_readParameter("Run", 0));
		strcpy(pAppParam->args, dpini_readParameter("Cmdline", 1));
		strcpy(pAppParam->shellOpts, dpini_readParameter("ShellOpts", 0));
		pAppParam->sessionType = atoi(dpini_readParameter("SessionType", 0));
		#if defined(dp_ANET2)
			buf = dpini_readParameter("Version", 1);
			if (sscanf(buf, "%d%*c%d", &val, &val2) != 2) {
				DPRINT(("dpReadAnetInf: Bad version number\n"));
				pAppParam->current.major = dp_VERS_UNKNOWN;
				pAppParam->current.minor = dp_VERS_UNKNOWN;
			}
            pAppParam->current.major = val;
            pAppParam->current.minor = val2;
			buf = dpini_readParameter("Platform", 1);
			if (sscanf(buf, "%u", &val) != 1) {
				DPRINT(("dpReadAnetInf: Bad platform spec\n"));
				pAppParam->platform = dp_PLAT_UNKNOWN;
			}
            pAppParam->platform = val;
			buf = dpini_readParameter("Language", 1);
			if (sscanf(buf, "%u", &val) != 1) {
				DPRINT(("dpReadAnetInf: Bad language spec\n"));
				pAppParam->language = dp_LANG_UNKNOWN;
			}
            pAppParam->language = val;
		#else
			(void) buf;
		#endif

		/* check if path is relative and if so, change it to an absolute path */
		if (*pAppParam->path && (*pAppParam->path != '/') && (*pAppParam->path != '\\') && *(pAppParam->path + 1) != ':') {
			reltoabspath(pAppParam->path, pAppParam->cwd);
		}
		DPRINT(("dpReadAnetInf: name:%s path:%s args:%s cwd:%s type:%d"
			" shellopts:%s ver:%u.%u latest:%u.%u lang:%d plat:%d\n",
			pAppParam->name,
			pAppParam->path, pAppParam->args,
			pAppParam->cwd,
			pAppParam->sessionType,
			pAppParam->shellOpts,
			pAppParam->current.major, pAppParam->current.minor,
			pAppParam->latest.major, pAppParam->latest.minor,
			pAppParam->language,
			pAppParam->platform));
	}

	/* Restore the .ini file */
	dpini_SetFile(oldinibuf);

	return err;
}

#ifdef UNITTEST

#include <assert.h>
#define TEST_MAX 8

typedef struct{
	char relpath[MAX_PATH];
	char cwd[MAX_PATH];
	char abspath[MAX_PATH];
} testcase;

testcase test[TEST_MAX] = {
	"foo.exe", "c:\\mw\\foo", "c:\\mw\\foo\\foo.exe",
	"foo.exe", "c:\\mw\\foo\\", "c:\\mw\\foo\\foo.exe",
	"..\\foo\\foo.exe", "c:\\mw\\foo1", "c:\\mw\\foo\\foo.exe",
	"..\\foo\\foo.exe", "c:\\mw\\foo1\\", "c:\\mw\\foo\\foo.exe"
	"foo", "/mw/foo", "/mw/foo/foo",
	"foo", "/mw/foo/", "/mw/foo/foo",
	"../foo/foo", "/mw/foo1", "/mw/foo/foo",
	"../foo/foo", "/mw/foo1/", "/mw/foo/foo"
};

void path_test(void)
{
	int word = 0;
	int i, c;
	char relpath[200];
	char cwd[200];
	char *pr = &relpath[0];
	char *pc = &cwd[0];

	for (i = 0; i < TEST_MAX; i++) {
		reltoabspath(test[i].relpath, test[i].cwd);
		printf("%d: %s, %s\n", i, test[i].relpath, test[i].abspath);
		assert(!strcmp(test[i].relpath, test[i].abspath));
	}
	while ((c = getchar()) != EOF) {
		if (c != '\n') {
			if (word == 0)
				*pr++ = c;
			else
				*pc++ = c;
		} else if (word == 0) {
			*pr = '\0';
			word = 1;
		} else {
			*pc = '\0';
			printf("cwd, rel. path, abs path: %s, %s, ", cwd, relpath);
			reltoabspath(relpath, cwd);
			printf("%s\n", relpath);
			pr = &relpath[0];
			pc = &cwd[0];
			word = 0;
		}
	}
}

void main(void)
{
	path_test();
}

#endif
