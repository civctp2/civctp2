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
 Class to extract information about installed modems from the windows
 registry.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <windows.h>
#include <winreg.h>
#include "regmo.h"

//#define DPRINT(s) printf s
#define DPRINT(s)
//#define DEBUG_MODULE 1
//#include "ddprint.h"

#define MAX_REG_BUFLEN 64

/*-------------------------------------------------------------------------
 Search all the direct subkeys of 'hKeySearchRoot' for the first one that
 points to a registry key with the given ('name', 'value') pair.
 The pointer is a registry string named 'pointerName' which is used as a
 subkey of 'hKeyPointerRoot'.
 All string comparisons are case insensitive.
 On success,
	places name of pointed-to key in output buffer and returns its handle.
 	Assumes outputBuf is big enough.
 On failure,
 	returns NULL.

 For example:
 if hKeySearchRoot contains keys "foo" and "bar",
	 and each of those contain a string named "hardwarekey",
	 with values "fooDev" and "barDev" respectively,
 and hKeyPointerRoot contains keys "fooDev" and "barDev",
	 and each of these contain a string named "Driver",
	 with values "fooDriver" and "barDriver" respectively,
 then calling findKeyPointingToPair(hKeySearchRoot, hKeyPointerRoot,
	 "hardwarekey", "Driver", "barDriver", outputBuf)
	 will return an hKey for hKeySearchRoot/barDev, and will place "barDev"
	 into outputBuf.
-------------------------------------------------------------------------*/
HKEY findKeyPointingToPair(HKEY hKeySearchRoot, HKEY hKeyPointerRoot,
	const char *pointerName, const char *name, const char *value,
	char * outputBuf)
{
	char szEnum[MAX_REG_BUFLEN];
	HKEY hKeyEnum;
	char pointerVal[MAX_REG_BUFLEN];
	HKEY hKeyPointer;
	char tempbuf[MAX_REG_BUFLEN];
	DWORD werr;
	int i;

	for (i=0; ; i++) {
		long len;

		hKeyEnum = NULL;
		werr = RegEnumKey(hKeySearchRoot, i, szEnum, sizeof(szEnum));
		if (werr != ERROR_SUCCESS) {
			if (werr != ERROR_NO_MORE_ITEMS) {
				DPRINT(("findKeyPointingToPair: RegEnumKey rCode:%d\n", werr));
			}
			if (werr == ERROR_MORE_DATA)
				continue;
			break;
		}
		/* a key is found, not sure if it contain the right name */
		werr = RegOpenKeyEx(hKeySearchRoot, szEnum, 0, KEY_EXECUTE, &hKeyEnum);
		if (werr != ERROR_SUCCESS) {
			DPRINT(("findKeyPointingToPair: RegOpenKeyEx/%s rCode:%d\n",szEnum,werr));
			continue;
		}
		/* retrieve the pointer value */
		len = MAX_REG_BUFLEN;
		werr = RegQueryValueEx( hKeyEnum, pointerName, NULL, NULL, pointerVal, &len);
		CloseHandle(hKeyEnum);
		if (werr != ERROR_SUCCESS) {
			DPRINT(("findKeyPointingToPair: Can't find %s in subkey %s, error %d\n", pointerName, szEnum, werr));
			continue;
		}
		/* Open the key pointed to by the pointer */
		werr = RegOpenKeyEx(hKeyPointerRoot, pointerVal, 0, KEY_EXECUTE, &hKeyPointer);
		if (werr != ERROR_SUCCESS) {
			DPRINT(("findKeyPointingToPair: pointer RegOpenKeyEx/%s rCode:%d\n", pointerVal,werr));
			continue;
		}
		/* retrieve the value if it is the right name */
		len = MAX_REG_BUFLEN;
		werr = RegQueryValueEx( hKeyPointer, name, NULL, NULL, tempbuf, &len);
		if (werr != ERROR_SUCCESS) {
			DPRINT(("findKeyPointingToPair: Can't find %s in subkey %s, error %d\n", name, szEnum, werr));
			CloseHandle(hKeyPointer);
			continue;
		}
		/* check if the retrieved value is the same as the parameter value */
		DPRINT(("findKeyPointingToPair: Enum %s, Pointed-to %s, Driver %s\n", szEnum, pointerVal, tempbuf));
		if (strcmpi(tempbuf, value) != 0) {
			CloseHandle(hKeyPointer);
			continue;
		}
		strcpy(outputBuf, pointerVal);
		DPRINT(("findKeyPointingToPair: success found key: %s\n", pointerVal));
		break;
	}
	return hKeyPointer;
}

/*-------------------------------------------------------------------------
 Open a handle to the root of the modem area in the registry.
 Returns 0 on failure.
-------------------------------------------------------------------------*/
static HKEY OpenModemRoot(void)
{
	const char *modemroot;
	OSVERSIONINFO VersionInfo;
	HKEY   hKeyModemroot;
	DWORD  werr;

	/* The modem info is stored slightly differently by WinNT and Win95. */
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&VersionInfo))
		return 0;
	if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		modemroot = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}";

	} else if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		modemroot = "SYSTEM\\CurrentControlSet\\Services\\Class\\Modem";
	} else {
		return 0;
	}

	/* Open the key for list of modems and get info about it. */
	werr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, modemroot, 0, KEY_EXECUTE, &hKeyModemroot);
	if (werr) {
		DPRINT(("regmo: RegOpenKeyEx/%s rCode:%d\n", modemroot, werr));
		return 0;
	}
	return hKeyModemroot;
}

/*--------------------------------------------------------------------------
 Enumerate installed modems.
 The given array is filled with each installed modem's key.
 The return value is the number of keys placed in the array.
--------------------------------------------------------------------------*/
int regmo_enum(int *keys, int maxkeys)
{
	char modemSubkey[64];
	DWORD  werr;
	HKEY   hKeyModemroot;		/* Key handle of SubKey. */
	int i;
	int nkeys;

	hKeyModemroot = OpenModemRoot();
	if (!hKeyModemroot) {
		DPRINT(("regmo_enum: OpenModemRoot failed !\n"));
		return 0;
	}

	werr = ERROR_SUCCESS;
	for (i=0, nkeys=0; (werr==ERROR_SUCCESS) && (nkeys<maxkeys); i++) {
		/* Get a modem, open its key, and get info. */
		werr = RegEnumKey(hKeyModemroot, i, modemSubkey, MAX_PATH);
		if (werr != ERROR_SUCCESS) {
			if (werr == ERROR_NO_MORE_ITEMS) {
				break;
			} else {
				DPRINT(("regmo_enum: RegEnumKey rCode:%d\n", werr));
	 			continue;
			}
		}
		if (!modemSubkey) {
			DPRINT(("regmo_enum: found a NULL modemSubkey\n"));
			continue;
		}
		keys[nkeys++] = atoi(modemSubkey);
	}
	CloseHandle(hKeyModemroot);
	return nkeys;
}

/* Convert escaped strings */
static void regmo_unescape(char *p)
{
	while ((p=(strchr(p, '<'))) != NULL) {
		int kill=0;
		if (strcmp(p, "<cr>") == 0) {
			*p++ = '\r';
			kill = 3;
		} else if (strcmp(p, "<lf>") == 0) {
			*p++ = '\n';
			kill = 3;
		}
		if (kill)
			memmove(p, p+kill, strlen(p+kill)+1);
	}
}

/*--------------------------------------------------------------------------
 Read the key'th modem definition from the Windows registry.
 Key is a value gotten from regmo_enum; usually, 0 <= key <= 10.
 Returns 0 on success, non-zero on failure.

 One cause of failure is "wrong type"; some things reported as modem
 drivers aren't really, and we filter them out here rather than in
 regmo_enum for efficiency's sake.
--------------------------------------------------------------------------*/
int regmo_get(regmo_t * regmo, int key)
{
	HKEY   hKeyModemroot;
	HKEY   hKeyModem;
	HKEY   hKeyInit;
	char   subkey[100];
	char   subkeyNameModem[100];
	long   len;
	DWORD  werr;
	int i;
	char deviceType;
	int n_init;

	memset(regmo, 0, sizeof(regmo_t));

	hKeyModemroot = OpenModemRoot();
	if (!hKeyModemroot) {
		DPRINT(("regmo_get: OpenModemRoot failed !\n"));
		return __LINE__;
	}

	sprintf(subkeyNameModem, "Modem\\%04d", key);
	sprintf(subkey, "%04d", key);
	werr = RegOpenKeyEx(hKeyModemroot, subkey, 0, KEY_EXECUTE, &hKeyModem);
	CloseHandle(hKeyModemroot);
	if (werr) {
		DPRINT(("regmo_get: RegOpenKeyEx/%s rCode:%d\n", subkey, werr));
		return __LINE__;
	}

	/* Read device type.  If it ain't 01, ignore this modem! */
	len = sizeof(deviceType);
	werr = RegQueryValueEx( hKeyModem, "DeviceType", NULL, NULL, &deviceType, &len);
	if (werr) {
		DPRINT(("regmo_get: Can't get DeviceType rCode:%d\n", werr));
		CloseHandle(hKeyModem);
		return __LINE__;
	}
	if ((deviceType != 1) && (deviceType != 2)) {
		DPRINT(("regmo_get: DeviceType %d, wanted 1 or 2\n", deviceType));
		CloseHandle(hKeyModem);
		return __LINE__;
	}

	/* Read modem name */
	len = regmo_STRLEN;
	werr = RegQueryValueEx( hKeyModem, "DriverDesc", NULL, NULL, regmo->DriverDesc, &len);
	if (werr) {
		DPRINT(("regmo_get: Can't get DriverDesc rCode:%d\n", werr));
		CloseHandle(hKeyModem);
		return __LINE__;
	}

	/* Read what comm port it uses */
	len = regmo_STRLEN;
	werr = RegQueryValueEx( hKeyModem, "AttachedTo", NULL, NULL, regmo->AttachedTo, &len);
	if (werr) {
		HKEY hKey2;
		HKEY hKeyDynConfigMan;
		HKEY hKeyMachineEnum;
		char subkeyName2[MAX_REG_BUFLEN];

		DPRINT(("regmo_get: Can't get AttachedTo rCode:%d\n", werr));
		if (deviceType != 2) {
			CloseHandle(hKeyModem);
			return __LINE__;
		}
		/* winmodems sometimes (always?) omit AttachedTo in favor of
		 * PORTNAME in config manager.
		 * Here are the three areas of the registry that are involved:
		 *
		 * HKEY_DYN_DATA/ConfigManager/Enum/xxx/Hardwarekey ----
		 *												        |
		 *					             -----------------------
		 *                              |
		 * HKEY_LOCAL_MACHINE/Enum/[Hardwarekey]
		 * 							/ Class
		 * 							/ PORTNAME
		 * 							/ DRIVER -------------------
		 * 														|
		 * 														|
		 * HKEY_LOCAL_MACHINE/System/CurrentControlSet/Class/[DRIVER]
		 * 													  / AttachedTo
		 * 													  / DeviceType
		 * 													  / DriverDesc
		 *
		 * Where [...] means value of ...
		 * The above is for Win95.  WinNT might not even support WinModems...
		 */

		werr = RegOpenKeyEx(HKEY_DYN_DATA, "Config Manager\\Enum", 0, KEY_EXECUTE, &hKeyDynConfigMan);
		if (werr) {
			DPRINT(("regmo_get: RegOpenKeyEx/ConfigManager/Enum rCode:%d\n", werr));
			CloseHandle(hKeyModem);
			return __LINE__;
		}
		werr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Enum", 0, KEY_EXECUTE, &hKeyMachineEnum);
		if (werr) {
			DPRINT(("regmo_get: RegOpenKeyEx/Enum rCode:%d\n", werr));
			CloseHandle(hKeyModem);
			CloseHandle(hKeyDynConfigMan);
			return __LINE__;
		}
		hKey2 = findKeyPointingToPair(hKeyDynConfigMan, hKeyMachineEnum,
			"Hardwarekey", "DRIVER", subkeyNameModem, subkeyName2);
		CloseHandle(hKeyDynConfigMan);
		CloseHandle(hKeyMachineEnum);
		if (!hKey2) {
			CloseHandle(hKeyModem);
			return __LINE__;
		}

		len = regmo_STRLEN;
		werr = RegQueryValueEx( hKey2, "PORTNAME", NULL, NULL, regmo->AttachedTo, &len);
		CloseHandle(hKey2);
		if (werr) {
			DPRINT(("regmo_get: Can't get PORTNAME rCode:%d\n", werr));
			CloseHandle(hKeyModem);
			return __LINE__;
		}
	}

	/* Read up to four init strings from Init */
	werr = RegOpenKeyEx(hKeyModem, "Init", 0, KEY_EXECUTE, &hKeyInit);
	if (werr) {
		CloseHandle(hKeyModem);
		DPRINT(("regmo_get: RegOpenKeyEx/%s rCode:%d\n", "Init", werr));
		return __LINE__;
	}
	for (n_init=0, i=1; n_init<regmo_N_INIT; i++, n_init++) {
		char name[10];
		sprintf(name, "%d", i);
		len = regmo_STRLEN;
		werr = RegQueryValueEx(hKeyInit,name,NULL,NULL,regmo->Init[n_init],&len);
		if (werr) {
			DPRINT(("regmo_get: can't read subkey %s = Init[%d], err %d\n", name, n_init, werr));
			break;
		}

		/* Convert escaped strings */
		regmo_unescape(regmo->Init[n_init]);
	}
	CloseHandle(hKeyInit);

	/* Read more init strings from Settings\\Init */
	werr = RegOpenKeyEx(hKeyModem, "Settings\\Init", 0, KEY_EXECUTE, &hKeyInit);
	CloseHandle(hKeyModem);
	if (werr) {
		CloseHandle(hKeyModem);
		DPRINT(("regmo_get: RegOpenKeyEx/%s rCode:%d\n", "Settings/Init", werr));
		if (n_init < regmo_N_INIT) {
			/* Until we implement the Responses key... */
			DPRINT(("regmo_get: kludge: appending ATV1\n"));
			strcpy(regmo->Init[n_init++], "ATV1\r");
		}
		return 0;
	}
	for (i=1; n_init<regmo_N_INIT; i++, n_init++) {
		char name[10];
		sprintf(name, "%d", i);
		len = regmo_STRLEN;
		werr = RegQueryValueEx(hKeyInit,name,NULL,NULL,regmo->Init[n_init],&len);
		if (werr) {
			DPRINT(("regmo_get: can't read subkey %s = Init[%d], err %d\n", name, n_init, werr));
			break;
		}

		/* Convert escaped strings */
		regmo_unescape(regmo->Init[n_init]);
	}
	CloseHandle(hKeyInit);
	if (n_init < regmo_N_INIT) {
		/* Until we implement the Responses key... */
		DPRINT(("regmo_get: kludge: appending ATV1\n"));
		strcpy(regmo->Init[n_init++], "ATV1\r");
	}

	return 0;
}

#if 0
void main(void)
{
	int keys[10];
	int nkeys = regmo_enum(keys, 10);
	regmo_t regmo;
	int i;
	int err;

	printf("nkeys %d\n", nkeys);
	for (i=0; i<nkeys; i++) {
		printf("keys[%d] %d\n", i, keys[i]);
		err = regmo_get(&regmo, keys[i]);
		if (!err) {
			int j;
			printf("DriverDesc %s\n", regmo.DriverDesc);
			printf("AttachedTo %s\n", regmo.AttachedTo);
			for (j=0; (j<regmo_N_INIT) && (regmo.Init[j][0]); j++) {
				printf("Init[%d] %s\n", j, regmo.Init[j]);
			}
		} else
			printf("err: %d, can't get info on key %d\n", err, keys[i]);
	}

}
#endif
