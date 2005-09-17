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

/*----------------------------------------------------------------------------
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been modified by the Apolyton CtP2 Source Code Project. 
 * Contact the authors at ctp2source@apolyton.net.
 *
 * Modifications from the Activision Anet 0.10 code:
 *
 * - 2005/03/11: Removed (unused, missing file) updater functionality.
 *
 *----------------------------------------------------------------------------
 */

/*--------------------------------------------------------------------------
 Enumerate applications installed on this machine.

 $Log: enumapp.c $
 Revision 1.15  1997/10/08 21:32:14  lbennett
 In progress; Adding server info.
 Revision 1.14  1997/10/08 19:12:21  lbennett
 Really fixed.
 Revision 1.13  1997/10/08 18:28:13  lbennett
 Fixed V.1 build.
 Revision 1.12  1997/10/08 03:24:24  lbennett
 Adding extra info.
 Revision 1.11  1997/08/13 20:42:07  dkegel
 Now saves and restores name of current .ini file,
 preventing problems that showed up in programs that
 call dpEnumApp() before dp.ini had been fully read.
 Revision 1.10  1997/08/06 22:38:04  anitalee
 Fixed bug when initializing sessionType.
 Revision 1.9  1997/08/06 22:08:38  anitalee
 Changed return statements to return dp_RES_*.
 Revision 1.8  1997/07/25 18:21:55  anitalee
 Minor changes.
 Revision 1.7  1997/07/15 21:50:22  dkegel
 Added support for ShellOpts.
 Fixed bug (there was a printf still in there).
 Revision 1.6  1997/07/15 18:52:10  anitalee
 Put in a unit test and cleaned up some bugs.
 Revision 1.5  1997/07/12 03:49:57  anitalee
 Read in some dp_appParam values from anet.inf.
 Revision 1.4  1997/07/10 19:10:17  dkegel
 (oops, I meant pointer, not a literal).
 Revision 1.3  1997/07/10 19:09:24  dkegel
 Fixed bug- sessionType value from registry is literal, not a pointer
 Revision 1.2  1997/06/26 20:02:23  anitalee
 Changed to make consistent with added member sessionType in struct dp_appParam.
 Revision 1.1  1997/06/21 02:23:09  anitalee
 Initial revision
--------------------------------------------------------------------------*/
 
#ifdef WIN32
 /* Enumerates applications in the registry specified by SubKey */

#define STRICT
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <winreg.h>
#include <winerror.h>
#include "dp.h"
#include "portable.h"
#include "contract.h"


/**
* Constants
*/

/* Where we look for the list of currently installed apps */
#define hKeyRoot HKEY_LOCAL_MACHINE
#define SubKey "SOFTWARE\\Activision\\Activenet\\Applications"

/* Where we store the apps list */
#define APPS_CACHE_FILE "feif.dat"

/* Maximum length of a full pathname.  Defined as 260 in windef.h. */
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

/* Maximum length of name of a registry value */
#define MAX_VALUE_NAME              128


/**
* Methods
*/

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
	precondition(dp->dpio != NULL);

	DPRINT(("dp_freezeAppsList:\n"));

	if (!dp->apps || (dptab_tableSize(dp->apps) == 0)) {
		DPRINT(("dptab_table_freeze: table has no entry\n"));
		return dp_RES_OK;
	}

	/* Get the file */
	fp = fopen(APPS_CACHE_FILE, "wb");
	if (fp == NULL) {
		DPRINT(("dp_freezeAppsList: can't open %s, errno %d\n", APPS_CACHE_FILE, errno));
		return dp_RES_FULL;
	}

	/* Write the info (either NULL or a table) */
	if(dp->apps != NULL) {
		char c = 1;
		if(fwrite(&c, sizeof(char), 1, fp) != 1) {
			fclose(fp);
			DPRINT(("dp_freezeAppsList: can't write %s, errno %d\n", APPS_CACHE_FILE, errno));
			return dp_RES_FULL;
		}
		err = dptab_table_freeze(dp->apps, dp->dpio, fp);
		DPRINT(("dp_freezeAppsList: fp now at %ld\n", ftell(fp)));
	} else {
		char c = 0;
		if(fwrite(&c, sizeof(char), 1, fp) != 1) {
			fclose(fp);
			DPRINT(("dp_freezeAppsList: can't write %s, errno %d\n", APPS_CACHE_FILE, errno));
			return dp_RES_FULL;
		}
	}

	/* Finish the file */
	fclose(fp);
	DPRINT(("dp_freezeAppsList: returning err:%d\n", err));
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

	DPRINT(("dp_createAppsList: \n"));
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
				if (dptab_getTable(dp->dt, key, 1))
					dptab_deleteTable(dp->dt, key, 1);
			} else
				dp->apps = dptab_getTable(dp->dt, key, 1);
			fclose(fp);
		}
	} else {
		DPRINT(("dp_createAppsList: Can't open %s, errno=%d\n", APPS_CACHE_FILE, errno));
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

/*----------------------------------------------------------------------
 Determine whether application *app needs to be updated.
 Returns latest app version from game server in *app.
 Not needed by most user programs.

 Returns dp_RES_OK if update needed,
 dp_RES_EMPTY if no update needed.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCheckAppVersion(
	dp_t* dp,
	dp_appParam_t *app)
{
    precondition(dp);
    precondition(app);

	getLatest(dp, app);   /* Ignore errors */

	if ((app->latest.major == dp_VERS_UNKNOWN)
	||  (app->latest.minor == dp_VERS_UNKNOWN)) {
		DPRINT(("dpCheckAppVersion: no version info available for this app?\n"));
		return dp_RES_EMPTY;
	}
	if ((app->current.major > app->latest.major)
	||  ((app->current.major == app->latest.major)
	   && (app->current.minor >= app->latest.minor))) {
		DPRINT(("dpCheckAppVersion: this app is up to date.\n"));
		return dp_RES_EMPTY;
	}

	DPRINT(("dpCheckAppVersion: app needs updating: version cur %d/%d < latest %d/%d\n", 
			app->current.major, app->current.minor,
			app->latest.major, app->latest.minor));
	return dp_RES_OK;
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
DP_API dp_result_t DP_APIX 	/* status */
dpGetAppVersion(
	dp_t* dp,
	dp_appParam_t *app)
{
	dp_result_t err;
	char cwd[256];
	int len;
	CHAR	 GameName[MAX_VALUE_NAME];
	CHAR	 GamePath[MAX_PATH];
	CHAR	 GameArgs[MAX_PATH];
	CHAR	 GameCwd[MAX_PATH];
	CHAR	 GameShellOpts[MAX_PATH];
	dp_appParam_t appParam;

    precondition(dp);
    precondition(app);

	DPRINT(("dpGetAppVersion:\n"));

	/* Get current directory, minus any trailing slash */
	if (!getcwd(cwd, sizeof(cwd)))
		return dp_RES_BUG;
	len = strlen(cwd);
	if ((len > 3) && (cwd[len - 1] == '\\'))
		cwd[len - 1] = 0;							

	memset(&appParam, 0, sizeof(*app));
	appParam.name = GameName;
	appParam.path = GamePath;
	appParam.args = GameArgs;
	appParam.cwd = GameCwd;
	appParam.shellOpts = GameShellOpts;
	err = dpReadAnetInf(cwd, &appParam);
	if (err != dp_RES_OK) {
		DPRINT(("dpGetAppVersion: dpReadAnetInf fails, err %d?\n", err));
		return dp_RES_BUG;
	}
	*app = appParam;
	app->name = strdup(appParam.name);
	app->path = strdup(appParam.path);
	app->args = strdup(appParam.args);
	app->cwd = strdup(appParam.cwd);
	app->shellOpts = strdup(appParam.shellOpts);
	return dpCheckAppVersion(dp, &appParam);   /* Ignore errors */
}

#if 0
static void showErr()
{
	LPVOID lpMsgBuf;
	 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	// Display the string.
	MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
	 
}
#endif


/*-------------------------------------------------------------------------
 Call to download the patch corresponding to the given product,
 but only if dpGetAppVersion says you need to.
 app is the structure filled in by dpGetAppVersion.
 If it returns dp_RES_OK, you should exit your application immediately!
 See also doc/updater.htm.

 Returns dp_RES_OK if patch is downloaded and running;
 dp_RES_EMPTY if no patch needed,
 dp_RES_BAD if can't find updater dll (anet/dkupddll.dll).
 dp_RES_USER_CANCELLED if user offered patch, but declined it.
-------------------------------------------------------------------------*/
#define BIND_UPDATER_LATE
#if defined(ANET_ORIGINAL)
#include "..\3rdparty\updater\update.h"
#endif
DP_API dp_result_t DP_APIX dpDownloadUpdate(dp_t *dp, const dp_appParam_t *app)
{
#if defined(ANET_ORIGINAL)
	int res;
	char productCode[256];

#ifdef BIND_UPDATER_LATE
	HINSTANCE hdll = NULL;
	DownloadPatch4_t DownloadPatch4;
	const char *dllname;

	dllname = dkupddll_DLL_NAME_1;
	hdll = LoadLibrary(dkupddll_DLL_NAME_1);
	if (!hdll) {
		dllname = dkupddll_DLL_NAME_2;
		hdll = LoadLibrary(dkupddll_DLL_NAME_2);
	}
	if (!hdll) {
		//showErr();
		DPRINT(("dpDownloadUpdate: can't load dll %s or %s; GetLastError %d\n",
				dkupddll_DLL_NAME_1, dkupddll_DLL_NAME_2, GetLastError()));
		return dp_RES_BAD;
	}

	DownloadPatch4 = (void *) GetProcAddress(hdll, dkupddll_NAME_DOWNLOADPATCH4);
	if (!DownloadPatch4)  {
		DPRINT(("dpDownloadUpdate: can't find fn %s in dll %s; GetLastError %d\n", dkupddll_NAME_DOWNLOADPATCH4, dllname, GetLastError()));
		FreeLibrary(hdll);
		return dp_RES_BUG;
	}
#endif

	(void) dp;
	/*sprintf(productCode,"http://updater.activision.com/updater/%d.%d.%d/Version.txt", app->sessionType, app->platform, app->language);*/
	sprintf(productCode,"%d.%d.%d", app->sessionType, app->platform, app->language);
	DPRINT(("dpDownloadUpdate: productCode %s, cur version %d, build %d\n", 
			productCode, app->current.major, app->current.minor));
	res = DownloadPatch4(GetForegroundWindow(), productCode, app->current.major, app->current.minor, 0, dp_dprintf);
	DPRINT(("dpDownloadUpdate: DownloadUpdate returns %d\n", res));
#ifdef BIND_UPDATER_LATE
	FreeLibrary(hdll);
#endif
	if (res == dkupddll_ERR_USER) {
		DPRINT(("dpDownloadUpdate: user cancelled the download!\n"));
		return dp_RES_USER_CANCELLED;
	}
	if (res == dkupddll_ERR_OK)
		return dp_RES_OK;
#endif /* ANET_ORIGINAL */
	return dp_RES_EMPTY;
}
#endif


/*----------------------------------------------------------------------
 Enumerate applications installed on this machine.
 Callback is called once for each application found.
 Callback ends when dpEnumApp() exits.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX	/* status */
dpEnumApp(
	dp_t* dp,				/* (input) info about latest versions */
	dpEnumAppCallback_t cb,	/* (input) callback for app info */
	void *context)			/* (input) context for callback */
{
	CHAR	 GameName[MAX_VALUE_NAME];
	CHAR	 GamePath[MAX_PATH];
	CHAR	 GameArgs[MAX_PATH];
	CHAR	 GameCwd[MAX_PATH];
	CHAR	 GameShellOpts[MAX_PATH];

	CHAR     ClassName[MAX_PATH] = ""; /* Buffer for class name. */
	DWORD    dwcClassLen = MAX_PATH;   /* Length of class string. */
	DWORD    dwcSubKeys;               /* Number of sub keys. */
	DWORD    dwcMaxSubKey;             /* Longest sub key size. */
	DWORD    dwcMaxClass;              /* Longest class string. */
	DWORD    dwcValues;                /* Number of values for this key. */
	DWORD    dwcMaxValueName;          /* Longest Value name. */
	DWORD    dwcMaxValueData;          /* Longest Value data. */
	DWORD    dwcSecDesc;               /* Security descriptor. */
	FILETIME ftLastWriteTime;          /* Last write time. */
	
	HKEY   hKeyApp;				/* Key handle of SubKey. */
	HKEY   hKeyVal;				/* Key handle of specific application. */
	CHAR   AppName[MAX_PATH];	/* Name of specific application. */
	INT    loopApp;  			/* Application index. */
	INT	   loopVal;  			/* Value index. */
	CHAR   ValueName[MAX_VALUE_NAME];   /* Name of value. */
	DWORD  cbValueName;			/* Size of value name. */
	DWORD  dwType;     			/* Type of data. */
	CHAR   *bData;   	/* Data buffer. */
	DWORD  cbData;    	/* Size of data buffer. */
	DWORD  retCodeApp;	/* Error when accessing application list handles. */
	DWORD  retCodeVal;	/* Error when accessing application handles. */
	CHAR   *Param;
	dp_result_t err = dp_RES_OK;

	dp_appParam_t appParam;

    precondition(cb != NULL);

	DPRINT(("dpEnumApp:\n"));

	appParam.name = GameName;
	appParam.path = GamePath;
	appParam.args = GameArgs;
	appParam.cwd = GameCwd;
	appParam.shellOpts = GameShellOpts;

	#if defined(dp_ANET2)
		/* Set up the server-list table (the list of latest version number) */
        if(dp != NULL) {
		    if(dp->apps == NULL) {
			    err = dp_createAppsList(dp);
			    if (err != dp_RES_OK) {
				    DPRINT(("dpEnumApp: Could not create apps, err:%d\n",err));
				    return dp_RES_BAD;
			    }
                err = dp_subscribeAppsList(dp);
			    if (err != dp_RES_OK) {
				    DPRINT(("dpEnumApp: Could not subscribe apps, err:%d\n",err));
				    return dp_RES_BAD;
			    }
		    }
        }
	#endif

	/* Open the key for list of games and get info about it. */
	retCodeApp = RegOpenKeyEx(hKeyRoot, SubKey, 0, KEY_EXECUTE, &hKeyApp);
	if (retCodeApp) {
		DPRINT(("dpEnumApp: RegOpenKeyEx/hKeyApp rCode:%d\n", retCodeApp));
		return dp_RES_BAD;
	}
	retCodeApp = RegQueryInfoKey(hKeyApp, ClassName, &dwcClassLen, NULL,
				&dwcSubKeys, &dwcMaxSubKey, &dwcMaxClass, &dwcValues,
				&dwcMaxValueName, &dwcMaxValueData, &dwcSecDesc, &ftLastWriteTime);
	/*DPRINT(("dpEnumApp: ClassName=%s, dwcClassLen=%d, dwcSubKeys=%d, dwcMaxClass=%d,\n\t dwcValues=%d; dwcMaxValueName=%s, dwcMaxValueData=%s\n", ClassName, dwcClassLen, dwcSubKeys, dwcMaxClass, dwcValues, dwcMaxValueName, dwcMaxValueData)); */
	if (retCodeApp) {
		DPRINT(("dpEnumApp: RegQIK/hKeyApp rCode:%d\n", retCodeApp));
		return dp_RES_BAD;
	}

	retCodeApp = ERROR_SUCCESS;
	for (loopApp = 0; retCodeApp == ERROR_SUCCESS; loopApp++) {
		/* Get a game name, open its key, and get info. */
		retCodeApp = RegEnumKey(hKeyApp, loopApp, AppName, MAX_PATH);
		if (retCodeApp != ERROR_SUCCESS) {
			if (retCodeApp == ERROR_NO_MORE_ITEMS) {
				continue;
			} else {
				DPRINT(("dpEnumApp: RegEnumKey/hKeyApp rCode:%d\n", retCodeApp));
	 			return dp_RES_BAD;
			}
		}
		if (!AppName) {
			DPRINT(("dpEnumApp: found a NULL AppName\n"));
			continue;
		}

		retCodeApp = RegOpenKeyEx(hKeyApp, AppName, 0, KEY_EXECUTE, &hKeyVal);
		if (retCodeApp != ERROR_SUCCESS) {
			DPRINT(("dpEnumApp: RegOpenKeyEx/hKeyVal rCode:%d\n", retCodeApp));
 			return dp_RES_BAD;
		}

		dwcClassLen = MAX_PATH;
		retCodeApp = RegQueryInfoKey(hKeyVal, ClassName, &dwcClassLen, NULL,
					 &dwcSubKeys, &dwcMaxSubKey, &dwcMaxClass, &dwcValues,
					 &dwcMaxValueName, &dwcMaxValueData, &dwcSecDesc,
					 &ftLastWriteTime);
		if (retCodeApp != ERROR_SUCCESS) {
			DPRINT(("dpEnumApp: RegQIK/hKeyVal rCode:%d\n", retCodeApp));
 			return dp_RES_BAD;
		}

		*appParam.name = '\0';
		*appParam.path = '\0';
		*appParam.args = '\0';
		*appParam.cwd = '\0';
		*appParam.shellOpts = '\0';
		appParam.sessionType = 0;

		bData = malloc(dwcMaxValueData);
		assert(bData);
		for (loopVal = 0, retCodeVal = ERROR_SUCCESS; retCodeVal ==
			 ERROR_SUCCESS; loopVal++) {

			bData[0] = '\0';
			ValueName[0] = '\0';
			cbData = dwcMaxValueData;
			cbValueName = MAX_VALUE_NAME;
			
			/* Enumerate the key values. */
			retCodeVal = RegEnumValue(hKeyVal, loopVal, ValueName,
								&cbValueName, NULL, &dwType, bData, &cbData);
			if (retCodeVal != ERROR_SUCCESS) {
				if (dwType < REG_FULL_RESOURCE_DESCRIPTOR &&
									retCodeVal != ERROR_NO_MORE_ITEMS) {
					DPRINT(("dpEnumApp: RegEnumValue = %d cbData = %d line %d\n"
						   , retCodeVal, cbData, __LINE__));
 					return dp_RES_BAD;
				}
			} else if (ValueName) {
				/*DPRINT(("dpEnumApp: loopVal=%d, ValueName=%s, cbValueName=%d, dwType=%d,\n\t bData=%s, cbData=%d; retCodeVal=%d\n", loopVal, ValueName, cbValueName, dwType, bData, cbData, retCodeVal)); */
				/* Process the value accordingly. */

				if (bData) {
					Param = _strlwr(ValueName);
					if(!strcmp("cwd", Param)) {
						int len = strlen(bData);
						strcpy(appParam.cwd, bData);
						/* remove trailing slashes */
						if ((len > 3) && (appParam.cwd[len - 1] == '\\')) {
							appParam.cwd[len - 1] = 0;							
						}
//					} else if (!strcmp("name", Param)) {
//						strcpy(appParam.name, bData);
//					} else if(!strcmp("run", Param)) {
//						strcpy(appParam.path, bData);
//					} else if(!strcmp("cmdline", Param)) {
//						strcpy(appParam.args, bData);
//					} else if(!strcmp("sessiontype", Param)) {
//						appParam.sessionType = (dp_species_t) (*(int *)bData);
					}
				} else {
					DPRINT(("dpEnumApp: got a NULL bData\n"));
				}
			} else {
				DPRINT(("dpEnumApp: got a NULL Value name\n"));
			}
		}

		/* Get the rest of the parameters from the ini file */
		if (*appParam.cwd) {
			/* Get info from app's .inf file */
			err = dpReadAnetInf(appParam.cwd, &appParam);

			/* Get info from server's table */
			#if defined(dp_ANET2)
				if ((err == dp_RES_OK) && (dp != NULL))
					getLatest(dp, &appParam);   /* Ignore errors */
			#endif

			/* Call callback function to return data about this game. */
			if (err == dp_RES_OK && *appParam.name && *appParam.path && *appParam.cwd) {
				cb(&appParam, context);
			} else {
                DPRINT(("dpEnumApp: Getting appParam, err:%d\n",err));
//				DPRINT(("dpEnumApp: Error: App(name,path,cwd) = (%s, %s, %s)\n",
//						*(appParam.name), *(appParam.path), *(appParam.cwd)));
			}
		} else {
			DPRINT(("dpEnumApp: Error getting .ini file path\n"));
		}
		if (bData) free(bData);
	}
	return dp_RES_OK;
}

#endif
