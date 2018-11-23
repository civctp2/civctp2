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

/* The tool help functions reside in the operating system kernel. The following example provides a platform-independent approach to accessing the tool help functions. */
#include "tlstuff.h"
#include "aeh.h"  /* for result codes */

/* File scope globals. These pointers are declared because of the need
 * to dynamically link to the functions.  They are exported only by
 * the Windows 95 kernel. Explicitly linking to them will make this
 * application unloadable in Microsoft(R) Windows NT(TM) and will
 * produce an ugly system dialog box. */
int InitSnapshot(Toolhlp *tool)
{
	HANDLE hKernel = NULL;
	aeh_SetCurrent(__LINE__, __FILE__);
	tool->pCreate32Snapshot = NULL;
	tool->pModule32First  = NULL;
	tool->pModule32Next   = NULL;
    hKernel = GetModuleHandle("KERNEL32.DLL");
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!hKernel ||
		!(tool->pCreate32Snapshot = (CREATESNAPSHOT)GetProcAddress(hKernel, "CreateToolhelp32Snapshot")) ||
		!(tool->pModule32First = (MODULEWALK)GetProcAddress(hKernel, "Module32First")) ||

		!(tool->pModule32Next = (MODULEWALK)GetProcAddress(hKernel, "Module32Next"))) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BUG;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

UNDECORATESYMNAME InitUnDecorateSymbolName(void)
{
	HANDLE hImage = NULL;
	UNDECORATESYMNAME pUnDecorateSymbolName = NULL;
    hImage = GetModuleHandle("IMAGEHLP.DLL");
	if (!hImage)  /* maybe dll not loaded yet */
    	hImage = LoadLibrary("IMAGEHLP.DLL");
	if (!hImage ||
		!(pUnDecorateSymbolName = (UNDECORATESYMNAME)GetProcAddress(hImage,
									"UnDecorateSymbolName")))
		return NULL;
	return pUnDecorateSymbolName;
}
