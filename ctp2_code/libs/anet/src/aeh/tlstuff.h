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

#include <windows.h>
#include <tlhelp32.h> // needed for tool help declarations  
 
// Type definitions for pointers to call tool help functions. 
typedef BOOL (WINAPI *MODULEWALK)(HANDLE hSnapshot, LPMODULEENTRY32 lpme); 
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID); 
typedef DWORD (WINAPI *UNDECORATESYMNAME)(LPSTR DecoratedName, LPSTR UnDecoratedName, DWORD UndecoratedLength, DWORD Flags);
 
typedef struct {
	CREATESNAPSHOT pCreate32Snapshot;
	MODULEWALK  pModule32First;
	MODULEWALK  pModule32Next;
} Toolhlp;

int InitSnapshot(Toolhlp *tool);
UNDECORATESYMNAME InitUnDecorateSymbolName();
