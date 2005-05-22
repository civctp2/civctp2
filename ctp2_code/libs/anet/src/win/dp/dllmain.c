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
 Hook for Win32 to increment and decrement this DLL's reference count(s).

 $Log: dllmain.c $
 Revision 1.2  1997/03/07 00:40:47  dkegel
 I don't understand why we're getting the "too many threads" warning
 in I76, but it doesn't seem to keep it from working, so comment
 out the warning for now.  BUG.
 Revision 1.1  1997/03/05 02:42:53  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <windows.h>

BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	static int procRefCount = 0;
	static int threadRefCount = 0;

	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		procRefCount++;
		break;
	
	case DLL_PROCESS_DETACH:
		procRefCount--;
		break;
	
	case DLL_THREAD_ATTACH:
		threadRefCount++;
		break;
		
	case DLL_THREAD_DETACH:
		threadRefCount--;
		break;

	}

	if (procRefCount < 0) {
		MessageBox( NULL, "Bug - negative processes?", "DP DLL Error", MB_OK|MB_ICONERROR );
		return FALSE;
	}
	if (procRefCount > 1) {
		MessageBox( NULL, "Bug - too many processes trying to use DP", "DP DLL Error", MB_OK|MB_ICONERROR );
		return FALSE;
	}
	if (threadRefCount < 0) {
		MessageBox( NULL, "Bug - negative threads?", "DP DLL Error", MB_OK|MB_ICONERROR );
		return FALSE;
	}
	/* Only the first thread is allowed to join? */
	if (threadRefCount > 0) {
		//MessageBox( NULL, "Bug - too many threads trying to use DP", "DP DLL Error", MB_OK|MB_ICONERROR );
		return FALSE;
	}
	
	return(TRUE);
}
 
