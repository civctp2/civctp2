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

#ifndef _MacMem_
#define _MacMem_

#include <Types.h>
#include "types.h"
typedef Handle HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

	void MEM_free_lock(void* thePtr);

	void *MyNewPtr(sint32 x);
	void *MyNewPtrClear(sint32 x);
	void* MyRealloc(void *x, sint32 y);
	OSErr MyDisposePtr(void *x);
	sint32 Mymsize(void *x);

	uint8 *HeapReAlloc(HANDLE hHeap, uint32 flags, char * obj, size_t size);

	//	algigned memory calls

	void* NewPtrAlign(sint32 size);
	void DisposePtrAlign(void* thePtr);

	//	memory cache calls

	void* GetNewObject(void);
	void FreeObject(void* theObject);

	void* GetMemory(sint32 theSize);
	void FreeMemory(void* theObject);

#ifdef __cplusplus
}
#endif

#endif
