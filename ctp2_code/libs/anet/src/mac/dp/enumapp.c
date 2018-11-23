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

/*--------------------------------------------------------------------------
 Enumerate applications installed on this machine.
 Macintosh-specific file.
 $Log: enumapp.c $
 Revision 1.2  1997/08/19 23:32:28  anitalee
 took out redefinition of DPRINT
 Revision 1.1  1997/08/15 19:04:53  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef __POWERPC__

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <Strings.h>
#include <Folders.h>
#include <Files.h>
#include <Aliases.h>
#include <Memory.h>
#include <Errors.h>
#include "dp.h"
#include "contract.h"

#define myPrefName "\pActivenet Preferences"
#define INIFile "anet.inf"

/* Where we store the apps list */
#define APPS_CACHE_FILE "feif.dat"

/*----------------------------------------------------------------------
Pascal/nonstandard string utilities -- might want to stick into another file.
----------------------------------------------------------------------*/
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

char *pStrcat(unsigned char *dest, unsigned char *src)
{
	long sLen = MIN(*src, 255 - *dest);
	BlockMove(src + 1, dest + *dest + 1, sLen);
	*dest += sLen;
	return (char *)dest;
}

char *pStrcpy(unsigned char *dest, unsigned char *src)
{
	BlockMove(src, dest, (long) *src + 1);
	return (char*)dest;
}

char* strdup(const char* src)
{
    char* dest = malloc(strlen(src) + 1);
    if(dest != NULL)
        strcpy(dest, src);
    return dest;
}

/*----------------------------------------------------------------------
Full path name returned as C string in fullpath for input file specification FSSpec.
----------------------------------------------------------------------*/
static dp_result_t fullpathfromid(FSSpec spec, char *fullpath)
{
	CInfoPBRec myCPB;
	char directoryName[FILENAME_MAX];
	OSErr err;

	pStrcpy(fullpath, spec.name);
	myCPB.dirInfo.ioNamePtr = &directoryName[0];
	myCPB.dirInfo.ioDrParID = spec.parID;
	/* recurse up directory tree till hit root dir */
	do {
		myCPB.dirInfo.ioVRefNum = spec.vRefNum;
		myCPB.dirInfo.ioFDirIndex = -1;
		myCPB.dirInfo.ioDrDirID = myCPB.dirInfo.ioDrParID;
		err = PBGetCatInfo(&myCPB, false);
		if (err != noErr) {
			DPRINT(("dpEnumApp: getting full path name error %d\n", err));
			return dp_RES_BAD;
		}
		pStrcat(directoryName, "\p:");
		pStrcat(directoryName, fullpath);
		pStrcpy(fullpath, directoryName);
	} while(myCPB.dirInfo.ioDrDirID != fsRtDirID);
	fullpath = p2cstr(fullpath);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Gets appParam parameters from an anet.inf file.
 path is name of directory containing anet.inf.
 Not needed by most user programs.
----------------------------------------------------------------------*/
DP_API dp_result_t dpReadAnetInf(const char *path, dp_appParam_t *pAppParam)
{
	OSErr err;
	char ini[FILENAME_MAX];
    char oldinibuf[FILENAME_MAX];
	const char *oldini;
	char *pbuf;

	/* Locate the directory containing the .ini file */
	strcpy(ini, path);
	pbuf = strrchr(ini, ':');
	if (!pbuf) {
		DPRINT(("dpReadAnetInf: unrecognized path format %s\n", path));
		return dp_RES_BAD;
	}
	*(pbuf + 1) = '\0';

	/* Save it for caller */
	strcpy(pAppParam->cwd, ini);

	/* Open the .ini file and find the [ActiveNet] section */
	strcat(ini, INIFile);
	oldini = dpini_GetFile();
	strcpy(oldinibuf, oldini);
	dpini_SetFile(ini);
	err = dpini_findSection("ActiveNet");

	/* Read info from the .ini file */
	if (!err) {
        const char* buf;
        unsigned int val, val2; /* dummy vars to prevent sscanf from overwriting */
	    strcpy(pAppParam->name, dpini_readParameter("Name", 0));
		strcpy(pAppParam->path, dpini_readParameter("Run", 0));
	    strcpy(pAppParam->args, dpini_readParameter("Cmdline", 1));
	    strcpy(pAppParam->shellOpts, dpini_readParameter("ShellOpts", 0));
	    pAppParam->sessionType = atoi(dpini_readParameter("SessionType",0));
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
        /* UNDONE */
	}

	/* Restore the .ini file */
	dpini_SetFile(oldinibuf);

	return err;
}

#if defined(dp_ANET2)

/*--------------------------------------------------------------------------
Save the apps list to disk.
(Called at quit time to provide quick info on next startup).
--------------------------------------------------------------------------*/
dp_result_t			/* status */
dp_freezeAppsList(
	dp_t *dp)		/* Owner of apps table */
{
	FILE* fp;
	dp_result_t err = dp_RES_OK;

	precondition(dp != NULL);

	/* Get the file */
	fp = fopen(APPS_CACHE_FILE, "wb");
	if (fp == NULL)
		return dp_RES_FULL;

	/* Write the info (either NULL or a table) */
	if(dp->apps != NULL) {
		char c = 1;
		if(fwrite(&c, sizeof(char), 1, fp) != 1) {
			fclose(fp);
			return dp_RES_FULL;
		}
		err = dptab_table_freeze(dp->apps, dp->dpio, fp);
	} else {
		char c = 0;
		if(fwrite(&c, sizeof(char), 1, fp) != 1) {
			fclose(fp);
			return dp_RES_FULL;
		}
	}

	/* Finish the file */
	fclose(fp);
	return err;
}

/*----------------------------------------------------------------------
Create the apps table.  If we can, read the table from disk;
otherwise, create a new table.
----------------------------------------------------------------------*/
dp_result_t			/* status */
dp_createAppsList(
	dp_t *dp)		/* Owner of apps table */
{
	char key[dptab_KEY_MAXLEN];
	FILE* fp;
	dp_result_t err = dp_RES_OK;

	precondition(dp != NULL);

	/* If the table's there, we don't do this */
	if(dp->apps != NULL) {
		DPRINT(("dp_createAppsList: won't overwrite\n"));
		return dp_RES_OK;
    }

	/* Restore the table, if possible */
	key[0] = dp_KEY_APPLICATIONS;
	fp = fopen(APPS_CACHE_FILE, "rb");
	if (fp != NULL) {
		char c;
		if((fread(&c, sizeof(char), 1, fp) != 1) || ((c != 0) && (c != 1)))
			DPRINT(("dp_createAppsList: bad initial char; can't restore\n"));
		if (c == 1) {
			err = dptab_table_thaw(dp->dt, dp->dpio, fp);
			if (err != dp_RES_OK) {
				DPRINT(("dp_createAppsList: can't restore, err:%d\n", err));
			} else
				dp->apps = dptab_getTable(dp->dt, key, 1);
			fclose(fp);
		}
	}

	/* Make sure there's a table */
	if (!dp->apps) {
		err = dptab_createTable(dp->dt, &dp->apps, key, 1, sizeof(dp_version_t), NULL,NULL,NULL,NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_createAppsList: can't create, err:%d\n", err));
			return err;
		}
	}
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
Subscribe to the server's table.
----------------------------------------------------------------------*/
dp_result_t			/* status */
dp_subscribeAppsList(
	dp_t *dp)		/* Owner of apps table */
{
	char key[dptab_KEY_MAXLEN];
	dp_result_t err = dp_RES_OK;

	precondition(dp != NULL);

	/* If we need it, create the table */
	if (dp->apps == NULL) {
		err = dp_createAppsList(dp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribeAppsList: creating list, err:%d\n",err));
			return err;
		}
	}

	/* Remove existing publish/subscribe */
	/* Not necessary; taken care of when we disconnect from the
	   existing publisher */

	/* If we have a server, ask it to send us the table info. */
	if ((dp->hGameServer != PLAYER_NONE) && (dp->hGameServer != PLAYER_BROADCAST)) {
        key[0] = dp_KEY_APPLICATIONS;
		err = dptab_addPublisher(dp->dt, dp->apps, key, 1, dp->hGameServer);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribeAppsList: adding publisher for apps table, err:%d\n",err));
			return err;
		}
		err = dptab_requestSubscription(dp->dt, key, 1, dp->hGameServer, NULL, NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribeAppsList: requesting subscription to apps table, err:%d\n",err));
			return err;
		}
	}
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Gets latest version info from server's table.
----------------------------------------------------------------------*/
static dp_result_t			/* status */
getLatest(
	dp_t* dp,				/* (input) source of table of latest */
	dp_appParam_t *pApp)	/* (output) latest version info */
{
	dptab_table_t *ptApps;
	char key[dptab_KEY_MAXLEN];
	int keylen = 0;
	dp_result_t err;

	assert(dp != NULL);
	assert(pApp != NULL);

	/* Set up default values */
	pApp->latest.major = dp_VERS_UNKNOWN;
	pApp->latest.minor = dp_VERS_UNKNOWN;

	/* Get table of applications */
	keylen = 0;
	key[keylen++] = dp_KEY_APPLICATIONS;
	ptApps = dptab_getTable(dp->dt, key, keylen);

	/* If table present, find app info */
	if(ptApps != NULL) {
		dp_version_t ver;
		dp_version_t* pVer = &ver;
		size_t len;

		keylen = 0;
		key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(pApp->sessionType);
		key[keylen++] = (char) dpGETSHORT_SECONDBYTE(pApp->sessionType);
		key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(pApp->platform);
		key[keylen++] = (char) dpGETSHORT_SECONDBYTE(pApp->platform);
		key[keylen++] = (char) pApp->language;
		err = dptab_get_bykey(ptApps, key, keylen, &pVer, &len);
		if (err != dp_RES_OK) {
			DPRINT(("enumapp.getLatest: game not in table\n"));
			return err;
		}
		pApp->latest = *pVer;
	}

	return dp_RES_OK;
}

#endif /* dp_ANET2 */

/*-------------------------------------------------------------------------
 Callback triggered by listing applications.
-------------------------------------------------------------------------*/
void dp_PASCAL dp_appVersion_cb(dp_appParam_t *app, void *context)
{
	dp_appParam_t *result = (dp_appParam_t *)context;
	if (!app || !context) return;
	DPRINT(("dp_appVersion_cb: name:%s path:%s args:%s cwd:%s type:%d shellopts:%s"
			" ver:%u.%u latest:%u.%u lang:%d plat:%d; result->path %s\n",
		app->name,
		app->path, app->args,
		app->cwd,
		app->sessionType,
		app->shellOpts,
		app->current.major, app->current.minor,
		app->latest.major, app->latest.minor,
		app->language,
		app->platform, result->path));

	if (stricmp(app->cwd, result->cwd))
        return;

	DPRINT(("dp_appVersion_cb: match!\n"));

	*result = *app;
	result->name = strdup(app->name);
	result->path = strdup(app->path);
	result->args = strdup(app->args);
	result->cwd = strdup(app->cwd);
	result->shellOpts = strdup(app->shellOpts);
}

/*----------------------------------------------------------------------
 Determine the current application's version, and whether it needs to be
 updated.
 Returns info about the current app in *app.  Strings are allocated with
 strdup.

 Returns dp_RES_OK if update needed,
 dp_RES_EMPTY if no update needed,
 other values on failure.
----------------------------------------------------------------------*/
DP_API dp_result_t			/* status */
dpGetAppVersion(
	dp_t* dp,
	dp_appParam_t *app)
{
	dp_result_t err;
	char cwd[256];
	int len;

	if (!dp || !app) {
		DPRINT(("dpGetAppVersion: null arguments not allowed\n"));
		return dp_RES_BAD;
	}

	/* Find the current application */
	memset(app, 0, sizeof(*app));
	if (!getcwd(cwd, sizeof(cwd)))
		return dp_RES_BUG;
	app->cwd = cwd;			/* so callback can compare */
	err = dpEnumApp(dp, dp_appVersion_cb, app);
	if (err != dp_RES_OK)
		return err;
	if (app->path == NULL) {
		DPRINT(("dpGetAppVersion: no info available for this app?\n"));
		return dp_RES_EMPTY;
	}

    /* Check the version number */
	if ((app->latest.major == dp_VERS_UNKNOWN)
	||  (app->latest.minor == dp_VERS_UNKNOWN)) {
		DPRINT(("dpGetAppVersion: no version info available for this app?\n"));
		return dp_RES_EMPTY;
	}
	if ((app->current.major >= app->latest.major)
	&&  (app->current.minor >= app->latest.minor)) {
		DPRINT(("dpGetAppVersion: this app is up to date.\n"));
		return dp_RES_EMPTY;
	}
	DPRINT(("dpGetAppVersion: app needs updating\n"));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Call to download the patch corresponding to the given product,
 but only if dpGetAppVersion says you need to.
 app is the structure filled in by dpGetAppVersion.
 Returns dp_RES_OK if patch is downloaded and running;
 dp_RES_EMPTY if no patch needed,
 dp_RES_BAD if can't find updater dll (anet/dkupddll.dll).
 If it returns dp_RES_OK, you should exit your application immediately!
-------------------------------------------------------------------------*/
/**
* WARNING: Dummy version supplied for Macintosh.
*/
DP_API dp_result_t dpDownloadUpdate(dp_t *dp, const dp_appParam_t *app)
{
    /* UNDONE: Implement for Macintosh. */
    return dp_RES_BAD;
}

/*----------------------------------------------------------------------
 Enumerate applications installed on this machine.
 Installed applications should be aliased in Activenet Preferences folder.
 Callback is called once for each application found.
 Callback ends when dpEnumApp() exits.
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnumApp(dp_t *dp, dpEnumAppCallback_t cb, void *context)
{
	int i;
	short myVRef=0;
	long myDirID=0;
	dp_appParam_t appParam;
	CInfoPBRec myCPB;
	FSSpec mySpec;
	OSErr err;
	dp_result_t dperr;
	char FIObuf[sizeof(myPrefName)];
	char path[FILENAME_MAX];
	char name[FILENAME_MAX];
	char args[FILENAME_MAX];
	char cwd[FILENAME_MAX];
	char shellOpts[FILENAME_MAX];

	appParam.path = &path[0];
	appParam.name = &name[0];
	appParam.args = &args[0];
	appParam.cwd = &cwd[0];
	appParam.shellOpts = &shellOpts[0];

	/* find Activenet Preferences folder */
	err = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, &myVRef, &myDirID);
	if (err != noErr) {
		DPRINT(("dpEnumApp: preferences folder not found error %d\n", err));
		return dp_RES_BAD;
	}
	myCPB.hFileInfo.ioCompletion = NULL;
	myCPB.hFileInfo.ioNamePtr = &FIObuf[0];
	pStrcpy(myCPB.hFileInfo.ioNamePtr, myPrefName);
	myCPB.hFileInfo.ioVRefNum = myVRef;
	myCPB.hFileInfo.ioACUser = 0;
	myCPB.hFileInfo.ioDirID = myDirID;
	myCPB.hFileInfo.ioFDirIndex = 0;
	err = PBGetCatInfo(&myCPB, false);
	if (err != noErr) {
		DPRINT(("dpEnumApp: Activenet preferences folder not found error %d\n", err));
		return dp_RES_BAD;
	}
	myDirID = myCPB.hFileInfo.ioDirID;
	myCPB.hFileInfo.ioVRefNum = myVRef;
	for (i = 1; err == noErr; i++) {		/* loop through files in Activenet Preferences folder */
		Boolean isFolder, wasAliased;
		myCPB.hFileInfo.ioDirID = myDirID;
		myCPB.hFileInfo.ioFDirIndex = i;
		myCPB.hFileInfo.ioACUser = 0;
		err = PBGetCatInfo(&myCPB, false);
		if (err != noErr) {
			if (err != fnfErr) {
				DPRINT(("dpEnumApp: can't access prefs files error %d; noErr = %d\n", err, noErr));
				return dp_RES_BAD;
			} else {
				continue;
			}
		}
		err = FSMakeFSSpec(myVRef, myDirID, myCPB.hFileInfo.ioNamePtr, &mySpec);
		/* check for errors other than no error and file not found error */
		if (err != noErr) {
			DPRINT(("dpEnumApp: can't make prefs files FSS error %d\n", err));
			return dp_RES_BAD;
		}
		err = ResolveAliasFile(&mySpec, true, &isFolder, &wasAliased);
		if (err != noErr) {
			if (err == fnfErr) {
				DPRINT(("dpEnumApp: alias points to nonexistant file\n"));
				err = noErr;
				continue;
			}
			DPRINT(("dpEnumApp: can't resolve alias error %d\n", err));
			return dp_RES_BAD;
		}
		/* if file found isn't alias or if it's a folder, skip it; otherwise enumerate it */
		if (!wasAliased || isFolder) continue;
		*appParam.name = '\0';
		*appParam.path = '\0';
		*appParam.args = '\0';
		*appParam.cwd = '\0';
		*appParam.shellOpts = '\0';
		appParam.sessionType = 0;
		dperr = fullpathfromid(mySpec, appParam.path);
		if (dperr != dp_RES_OK) {
			DPRINT(("dpEnumApp: getting .ini file path error %d\n", dperr));
			continue;
		}
		dperr = dpReadAnetInf(appParam.cwd, &appParam);
		if (dperr == dp_RES_OK && *appParam.name && *appParam.path && *appParam.cwd) {
			cb(&appParam, context);
		} else {
			DPRINT(("dpEnumApp: got app but not other info err %d\n", dperr));
			DPRINT(("appParam{n,c,p,a,sO,sT} = %s, %s, %s, %s, %s, %d\n", appParam.name, appParam.cwd, appParam.path, appParam.args, appParam.shellOpts, appParam.sessionType));
		}
	}
	return dp_RES_OK;
}

#endif
