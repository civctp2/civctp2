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

/* Private registry stuff, by Ben Siron */
// the functions defined here provide our standard installer interaction with the
// Registry.
#include "registry.h"
#include <stdio.h>
#include <winreg.h>

HKEY MyRegOpenKey( HKEY base, LPCSTR szSubKey )
{
	HKEY hKey;
	if( RegOpenKeyEx( base, szSubKey, 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS )
		return hKey;
	else
		return NULL;
}

char *MyRegQueryValueMalloc( HKEY hKey, LPCSTR szValueName, LPDWORD pType, LPDWORD pSize )
{
	unsigned char *pData;
	DWORD dwSize = 0;

	if( RegQueryValueEx( hKey, szValueName, 0, NULL, NULL, &dwSize ) != ERROR_SUCCESS )
		return NULL;

	pData = (unsigned char *)malloc( dwSize );
	if( RegQueryValueEx( hKey, szValueName, 0, pType, pData, &dwSize ) != ERROR_SUCCESS )
		return NULL;

	if( pSize )
		*pSize = dwSize;

	return (char *)pData;
}

char *MyRegQuickQueryMalloc( HKEY hKey, LPCSTR szSubKey, LPCSTR szValueName, LPDWORD pType, LPDWORD pSize )
{
	HKEY hNew;
	char *pData;

	if( !(hNew = MyRegOpenKey( hKey, szSubKey)) )
		return NULL;

	pData = MyRegQueryValueMalloc( hNew, szValueName, pType, pSize );

	RegCloseKey( hNew );
	return pData;
}


BOOL MyRegSetValue( HKEY hKey, LPCSTR szName, DWORD dwType, char *pData, DWORD cData )
{
	if( RegSetValueEx( hKey, szName, 0, dwType, (unsigned char *)pData, cData ) == ERROR_SUCCESS )
		return TRUE;
	else
		return FALSE;
}


HKEY MyRegCreateKey( HKEY base, LPCSTR szSubKey, DWORD *pDisposition )
{
	HKEY hKey;

	if( RegCreateKeyEx( base, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, pDisposition ) != ERROR_SUCCESS )
		return NULL;
	else
		return hKey;
}


char *MyRegEnumKeyMalloc( HKEY hKey, DWORD dwIndex )
{
	char *pData;
	DWORD dwSize = MAX_PATH;
	char szTemp[ MAX_PATH + 1 ];

	if( RegEnumKeyEx( hKey, dwIndex, szTemp, &dwSize, 0, NULL, 0, NULL ) != ERROR_SUCCESS )
		return NULL;

	szTemp[ dwSize ] = 0;

	pData = (char *)malloc( dwSize + 1 );
	strcpy( pData, szTemp );

	return pData;
}


char *MyRegEnumValueMalloc( HKEY hKey, DWORD dwIndex )
{
	char *pData;
	DWORD dwSize = MAX_PATH;
	char szTemp[ MAX_PATH + 1 ];

	if( RegEnumValue( hKey, dwIndex, szTemp, &dwSize, NULL, NULL, NULL, NULL ) != ERROR_SUCCESS )
		return NULL;

	szTemp[ dwSize ] = 0;

	pData = (char *)malloc( dwSize + 1 );
	strcpy( pData, szTemp );

	return pData;
}
