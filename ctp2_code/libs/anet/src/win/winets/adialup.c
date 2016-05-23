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
 Functions to find out about Windows 95's dialup Internet system.

 Copyright 1998 Activision.
 Dan Kegel
--------------------------------------------------------------------------*/
#include <windows.h>
#include <ras.h>

#if 0
#include <stdio.h>
#define DPRINT(s) printf s
#else
#define DPRINT(s)
#endif

/*--------------------------------------------------------------------------
 Return TRUE if autodial is enabled.
--------------------------------------------------------------------------*/
int adialup_autodial_enabled(void)
{
	HKEY hKey;
	unsigned long werr;
	int enableAutodial;
	size_t len;

	/* First, check to see if EnableAutodial is set in the registry. */

	/* Open the registry folder */
	werr = RegOpenKeyEx(HKEY_CURRENT_USER,
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			0, KEY_EXECUTE, &hKey);
	if (werr != ERROR_SUCCESS) {
		DPRINT(("autodial_enabled: RegOpenKeyEx(%s) fails, rCode:%d\n",
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			werr));
		return FALSE;
	}
	/* retrieve the value */
	enableAutodial = FALSE;
	len = sizeof(enableAutodial);
	werr = RegQueryValueEx(hKey,
			"EnableAutodial",
			NULL, NULL, (void *)&enableAutodial, &len);
	CloseHandle(hKey);
	if (werr != ERROR_SUCCESS) {
		DPRINT(("autodial_enabled: Can't find %s in subkey %s, error %d\n",
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			"EnableAutodial",
			werr));
		return FALSE;
	}

	if (!enableAutodial) {
		DPRINT(("autodial_enabled: autodial is off.\n"));
		return FALSE;
	}

	DPRINT(("autodial_enabled: autodial is on.\n"));
	return TRUE;
}

/*--------------------------------------------------------------------------
 Enable or disable autodial.
 Return TRUE on success.
--------------------------------------------------------------------------*/
int adialup_autodial_set(int enable)
{
	HKEY hKey;
	unsigned long werr;
	size_t len;

	/* Open the registry folder */
	werr = RegOpenKeyEx(HKEY_CURRENT_USER,
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			0, KEY_SET_VALUE, &hKey);
	if (werr != ERROR_SUCCESS) {
		DPRINT(("autodial_set: RegOpenKeyEx(%s) fails, rCode:%d\n",
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			werr));
		return FALSE;
	}

	/* Set the registry key to the value of enable */
	werr = RegSetValueEx(hKey, "EnableAutodial", 0, REG_DWORD, (char *)&enable, sizeof(enable));
	CloseHandle(hKey);
	if (werr != ERROR_SUCCESS) {
		DPRINT(("autodial_set: RegSetValueEx(%s) fails, rCode:%d\n",
				"EnableAutodial", werr));
		return FALSE;
	}

	return TRUE;
}

#define adialup_MAXCONNS 10

typedef DWORD (APIENTRY *pfnRasEnumConnections_t)(LPRASCONN, LPDWORD, LPDWORD);
typedef DWORD (APIENTRY *pfnRasGetConnectStatus_t)(HRASCONN, LPRASCONNSTATUS);

/*--------------------------------------------------------------------------
 Return TRUE if there is an active, ready-for-data, dialup connection,
 or if there are no dialup connections, active or not.
 Return FALSE if there are dialup connections, but none are active.
--------------------------------------------------------------------------*/
int adialup_is_active(void)
{
	/* See if RAS reports any good connections. */
	HRASCONN hrasconn;
	RASCONNSTATUS rasconnstatus;
	RASCONN rasconnArray[adialup_MAXCONNS];
	DWORD cConnections;
	DWORD werr;
	size_t rasconnLen;
	int i;
	HANDLE hlib;
	pfnRasEnumConnections_t pfnRasEnumConnections = NULL;
	pfnRasGetConnectStatus_t pfnRasGetConnectStatus = NULL;

	hlib = LoadLibrary("rasapi32.dll");

	if (NULL == hlib) {
		DPRINT(("adialup_is_active: can't load library rasapi32.dll\n"));
		return FALSE;
	}

	pfnRasEnumConnections = (pfnRasEnumConnections_t)GetProcAddress(hlib, "RasEnumConnectionsA");
	pfnRasGetConnectStatus = (pfnRasGetConnectStatus_t) GetProcAddress(hlib, "RasGetConnectStatusA");

	if (!pfnRasEnumConnections || !pfnRasGetConnectStatus) {
		DPRINT(("adialup_is_active: can't get fns from library rasapi32.dll\n"));
		FreeLibrary(hlib);
		return FALSE;
	}

	rasconnArray[0].dwSize = sizeof(rasconnArray[0]);
	rasconnLen = sizeof(rasconnArray);
	cConnections = adialup_MAXCONNS;
	werr = pfnRasEnumConnections( rasconnArray, &rasconnLen,  &cConnections);
	if (werr) {
		DPRINT(("adialup_is_active: RasEnumConnections fails, err %d\n", werr));
		FreeLibrary(hlib);
		return FALSE;
	}
	DPRINT(("adialup_is_active: RasEnumConnections reports %d connections\n", cConnections));
	if (cConnections < 1) {
		FreeLibrary(hlib);
		return TRUE;
	}

	for (i=0; i<cConnections; i++) {
		rasconnstatus.dwSize = sizeof(rasconnstatus);
		werr = pfnRasGetConnectStatus( rasconnArray[i].hrasconn, &rasconnstatus);
		if (rasconnstatus.rasconnstate == RASCS_Connected) {
			DPRINT(("adialup_is_active: cnxn %d is active!\n", i));
			FreeLibrary(hlib);
			return TRUE;
		}
	}

	DPRINT(("adialup_is_active: none of the connections are active\n"));
	FreeLibrary(hlib);
	return FALSE;
}

/*--------------------------------------------------------------------------
 Return TRUE if Internet activity is likely to trigger an autodial.
--------------------------------------------------------------------------*/
int adialup_willdial(void)
{
	return adialup_autodial_enabled() && !adialup_is_active();
}

#if 0
/*-------------------------------------------------------------------------
 Unit test (interactive).
 Compile with
 cl adialup.c advapi32.lib
--------------------------------------------------------------------------*/
main(int argc, char *argv[])
{
	printf("adialup_willdial returns %d\n", adialup_willdial());
	if (argc == 2)
		adialup_autodial_set(atoi(argv[1]));
	printf("adialup_autodial_enabled returns %d\n", adialup_autodial_enabled());
}
#endif
