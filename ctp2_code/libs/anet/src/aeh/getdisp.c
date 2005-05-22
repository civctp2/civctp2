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

/* Finds display info, by Ben Siron */
#ifndef _getdisp_dll
#define _getdisp_dll
#endif


#include "windows.h"
#include "winver.h"
#include "registry.h"
#include "getdisp.h"


char szScratch[ MAX_PATH ];



BOOL GetMyFileVersionInfo( LPSTR szFileName, struct display_t *p )
{
	VS_FIXEDFILEINFO *pv;
	DWORD dwSize, dwGarbage;
	void *buf;

	dwSize = GetFileVersionInfoSize( szFileName, &dwGarbage );
	buf = malloc( dwSize );

	GetFileVersionInfo( szFileName, 0, dwSize, buf );
	if (VerQueryValue( buf, "\\", (void *)&pv, &dwSize ) && (dwSize > 0)) {
		p->dwFileVersionMS = pv->dwFileVersionMS;
		p->dwFileVersionLS = pv->dwFileVersionLS;
	} else {
		p->dwFileVersionMS = 0;
		p->dwFileVersionLS = 0;
	}

	free( buf );
	return TRUE;
}


void ClearEntry( struct display_t *pEntry )
{
	memset( pEntry->szName, 0, 256 );
	memset( pEntry->szDriver, 0, MAX_PATH + 1 );
	pEntry->dwFileVersionMS = 0;
	pEntry->dwFileVersionLS = 0;
}


BOOL GetDriverSpecs( struct display_t *pEntry, char *szSubKey )
{
	HKEY hKey, hKey2;
	char *p;
	char szTemp[ MAX_PATH + 1 ] = "", szTemp2[ MAX_PATH + 1 ] = "";

	hKey = MyRegOpenKey( HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\Class" );
	if( hKey )
		hKey2 = MyRegOpenKey( hKey, szSubKey );
	else
		return FALSE;

	RegCloseKey( hKey );
	if( !hKey2 )
		return FALSE;

	if( !(p = MyRegQueryValueMalloc( hKey2, "DriverDesc", NULL, NULL ) ) )
		return FALSE;
	strcpy( pEntry->szName, p );
	free( p );

	hKey = MyRegOpenKey( hKey2, "DEFAULT" );
	if( !hKey )
		return FALSE;

	if( !(p = MyRegQueryValueMalloc( hKey, "drv", NULL, NULL ) ) )
		return FALSE;
	strcpy( pEntry->szDriver, p );

	// if no drive is specified in the driver filename, assume a path relative to the windows
	// system directory
	if( !strchr( p, ':' ) )
	{
		GetSystemDirectory( szTemp, MAX_PATH );
		strcat( szTemp, "\\" );
		strcat( szTemp, p );
	}
	else
		strcpy( szTemp, p );

	free( p );

	if( !GetMyFileVersionInfo( szTemp, pEntry ) )
		return FALSE;

	if( strlen( szTemp ) )
		SetCurrentDirectory( szTemp );

	RegCloseKey( hKey );
	RegCloseKey( hKey2 );

	return TRUE;
}


DWORD SearchAllSubkeysForDisplays( HKEY hKey, struct display_t *pDispEntry, DWORD nMaxInt, DWORD nFound )
{
	DWORD index = 0;
	HKEY hSubKey;
	char *name = NULL;
	char *data = NULL;

	if( !hKey )
		return nFound;

	if( !(nFound < nMaxInt) )
		return nFound;

	while( name = MyRegEnumKeyMalloc( hKey, index ) )
	{
		nFound = SearchAllSubkeysForDisplays( hSubKey = MyRegOpenKey( hKey, name ), pDispEntry, nMaxInt, nFound );
		if( hSubKey )
			data = MyRegQueryValueMalloc( hSubKey, "Class", NULL, NULL );
		if( data )
		{
			if( strnicmp( data, "Display", 8 ) == 0 )
			{
				free( data );
				data = MyRegQueryValueMalloc( hSubKey, "Driver", NULL, NULL );
				if( data )
				{
					if( nFound < nMaxInt )
					{
						if( GetDriverSpecs( (pDispEntry + nFound), data ) )
							nFound++;
						else
							ClearEntry( pDispEntry + nFound );
					}
					free( data );
				}
			}
		}
		if( hSubKey )
			RegCloseKey( hSubKey );
		free( name );
		index++;
	}

	return nFound;
}


DWORD WINAPI GetDisplayProfileData( struct display_t *pDispEntry, DWORD nMaxInt )
{
	HKEY hKey;
	DWORD nFound = 0, i;

	for( i = 0; i < nMaxInt; i++ )
		ClearEntry( pDispEntry + i );

	hKey = MyRegOpenKey( HKEY_LOCAL_MACHINE, "Enum" );
	nFound = SearchAllSubkeysForDisplays( hKey, pDispEntry, nMaxInt, 0 );
	if( hKey )
		RegCloseKey( hKey );

	return nFound;
}
