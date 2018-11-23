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

// the functions defined here provide our standard installer interaction with the
// Registry.

#ifndef registry_h
#define registry_h

#include <windows.h>

#ifndef MAX_STRING
#define MAX_STRING 1024
#endif




// these functions provide a simplified intuitive interface to the win95 registry (that is, no
// security features or class name support). All necessary registry functions are handled here,
// except for deleting of keys.  use RecursiveDeleteKey for this...

// return value of TRUE indicates success.
HKEY MyRegOpenKey( HKEY base, LPCSTR szSubkey );

// this combines reading a value and allocating space for it into one operation.   pType recieves
// the data type.  pSize recieves the data length.  Either of these can be NULL.  The pointer to the
// new space holding the data is returned (or NULL, in case of failure).  if hKey is NULL, NULL is
// returned.
char *MyRegQueryValueMalloc( HKEY hKey, LPCSTR szValueName, LPDWORD pType, LPDWORD pSize );

// these functions simplify the above calls by handling opening and closing of keys automatically...
// use them if you are not expecting to need error information or an intermediate HKEY.
char *MyRegQuickQueryMalloc( HKEY base, LPCSTR szSubKey, LPCSTR szValueName, LPDWORD pType, LPDWORD pSize );

// this is a very slightly simplified version of RegSetValueEx.  return value of TRUE means success.
BOOL MyRegSetValue( HKEY hKey, LPCSTR szName, DWORD dwType, char *pData, DWORD cData );

// return value of NULL indicates failure.  pDisposition returns either REG_OPENED_EXISTING_KEY or
// REG_CREATED_NEW_KEY.  pDisposition may be NULL.
HKEY MyRegCreateKey( HKEY base, LPCSTR szSubKey, DWORD *pDisposition );

// these functions return the names of the immediate subkeys of a key (they take care of allocating
// space for the string, and return NULL if the index is out of range)
char *MyRegEnumKeyMalloc( HKEY hKey, DWORD dwIndex );

// these functions return the names of the values of a key (they take care of allocating
// space for the string, and return NULL if the index is out of range)
char *MyRegEnumValueMalloc( HKEY hKey, DWORD dwIndex );


#endif
