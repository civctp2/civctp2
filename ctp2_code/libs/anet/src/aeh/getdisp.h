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
#ifndef _header_getdisp
#define _header_getdisp


struct display_t {
	DWORD dwFileVersionMS;
	DWORD dwFileVersionLS;
	char szName[ 256 ];
	char szDriver[ MAX_PATH+1 ];
};


DWORD WINAPI GetDisplayProfileData( struct display_t *pDispEntry, DWORD nMaxint );

#ifdef _getdisp_dll

BOOL GetMyFileVersionInfo( LPSTR szFileName, struct display_t *p );
DWORD SearchAllSubkeysForDisplays( HKEY hKey, struct display_t *pDispEntry, DWORD nMaxInt, DWORD nFound );
BOOL GetDriverSpecs( struct display_t *pEntry, char *szSubkey );
void ClearEntry( struct display_t *pEntry );


#endif //_getdisp_dll

#endif //_header_getdisp
