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

#ifdef	_MPW_MOT_
#include "MotoPrefix.h"
#endif

#include <stdlib.h>   // for malloc()
#include <Memory.h>   // for DisposePtr()
#include <string.h>   // for memcpy()

//#include	"LocalSwitches.h"
#include	"MacMem.h"
//#include	"3DStruct.h"
//#include	"Util.h"

#define USEMALLOC
#define USEMALLOCOBJ

#undef free
#undef malloc

#define	DRAWMARGIN	(8*1024)

void *MyNewPtr(long x) {
	return NewPtr(x);
}

void *MyNewPtrClear(long x) {
	return NewPtrClear(x);
}

void* MyRealloc(void* x, long y) {
	void*	newx;

	if (x == nil) {
	
		//	no block yet, just create a new one
		
		x = NewPtr(y);
	} else {
		OSErr err;
	
		SetPtrSize(x, y);
		err = MemError();
		if (err != noErr) {
			long		originalsize;
		
			//	there isn't room to increase the size, allocate a new
			//	block and copy all of the data
			
			originalsize = GetPtrSize(x);
			newx = NewPtr(y);
			if (newx != nil) {
			
				//	copy the data to the new block and dispose of the old one
			
				memcpy(newx, x, originalsize);
				DisposePtr(x);
				x = newx;
				
			} else {
				
				//	we are out of luck. Get rid of the current pointer and return
				
				DisposePtr(x);
				x = nil;
			}
		}
	
	}

	return x;
}

OSErr MyDisposePtr(void *x) {
	OSErr	error;

	DisposePtr((Ptr)x);
	error = MemError();
	return error;
}

void MEM_free_lock(void* thePtr) {
	MyDisposePtr(thePtr);
}

long Mymsize(void *x) {
	return GetPtrSize((Ptr)x);
}

unsigned char *HeapReAlloc(HANDLE hHeap, unsigned long flags, char * obj, size_t size) {
	Ptr				newBlock;
	OSErr			error;
	unsigned long	originalsize;

	/* Get the current size */
	originalsize = GetPtrSize(obj);

	/* Allocate a new pointer, of the new size */
	newBlock = NewPtr(size);

	error = MemError();
	if ((newBlock == nil) || (error != noErr)) {
		if (newBlock != nil) {
			DisposePtr(newBlock);
		}
		newBlock = nil;
	} else {
		/* Get the data in the new area */
		memcpy(newBlock, obj, originalsize);
		/* de-allocate the old one */
		DisposePtr(obj);
	}

	return (unsigned char *)newBlock;
}

#pragma segment Main
void* NewPtrAlign(long numbytes) {
	unsigned char*	p = nil;
	unsigned char		d;
	
#define	kAlignTo		32
#define	kAlignExtra		(kAlignTo*2)
#define	kAlignToMask	(kAlignTo-1)
	
	//	this routine allocates and clears an aligned block of memory. The memory
	//	is aligned to 32 byte boundaries
  
	p = (unsigned char*) NewPtrClear(numbytes + kAlignExtra + DRAWMARGIN);
	d = kAlignExtra - ((unsigned long) p & kAlignToMask);
	p += d;  
	p[-1] = d;				//	save offset from real pointer to returned pointer
	p += DRAWMARGIN / 2;	//	leave some extra space before and after drawing buffer

	return (void*) p;
}


#pragma segment Main
void DisposePtrAlign(void* thePtr) {
	unsigned char*	p;
  
	if (thePtr != nil) {
	
		//	compute the real pointer and dispose of it
		
		p = (unsigned char*) thePtr;
		p -= DRAWMARGIN / 2;			//	find the base address we started from
		p -= p[-1];
		DisposePtr((Ptr) p);
		
	}
  
}

#ifdef USEMALLOCOBJ

//	these versions just use malloc and free which are optimized in CW7 to allocate big
//	chunks, and, dole it out in aligned sections.

#if 0
void* GetNewObject(void) {
	return malloc(sizeof(OBJECT));
}

void FreeObject(void* theObject) {
	free(theObject);
}
#endif

#else

#define	kObjectCacheSize	2048

OBJECT**	gObjectCache = nil;
short		gObjectStack = 0;

void* GetNewObject(void) {
	OBJECT*		theObject;

	//	if we don't have a collection yet, create one
	
	if (gObjectCache == nil) {
		gObjectCache = (OBJECT**) NewPtrClear(kObjectCacheSize * sizeof(OBJECT*));
		gObjectStack = kObjectCacheSize;
	}
	
	if (gObjectStack < kObjectCacheSize) {
	
		//	there is something in the cache we can use
		
		theObject = gObjectCache[gObjectStack];
		gObjectStack++;
		
	} else {
	
		//	there is nothing in the cache, allocate it now
		
		theObject = (OBJECT*) malloc(sizeof(OBJECT));
	}
	
	return (void*) theObject;
}

void FreeObject(void* theObject) {

	gObjectStack--;

	if (gObjectStack < 0) {
	
		//	there is no room in the list for another pointer

		#ifdef MACDEBUG
			ErrorCodeAlert(MemError(), "\pObject Cache Is Too Small. Call Gregory. (P1)");
		#endif
		
		gObjectStack = 0;
		free((Ptr) theObject);
		
	} else {
	
		gObjectCache[gObjectStack] = (OBJECT*) theObject;
		
	}

}

#endif

#ifndef USEMALLOC

#define	kMemoryCaches		40
#define	kMemoryShift		8

void**		gMemoryCache[kMemoryCaches] = { nil };
short		gMemoryStackIndex[kMemoryCaches] = { 0 };

void FreeSomeMemory(long theSize) {
	long		totalFreed = 0;
	long		currentFreeSize;
	short		whichCache;
	void*		thePtr = nil;
	Boolean		done = false;
	long		bigmem;

	//	this routine goes through the list of free blocks and deallocates blocks until
	//	more than the amount specified has been freed, or, there is nothing else to free
	
	while (!done) {
	
		currentFreeSize = 0;
		done = true;		//	we are done if we don't find anything to free
	
		for (whichCache = 0; whichCache < kMemoryCaches; whichCache++) {
		
			currentFreeSize += 1 << kMemoryShift;
		
			//	if there is a block at the start of this cache, free it and keep track of
			//	the amount free
			
			if ( (gMemoryStackIndex[whichCache] < kObjectCacheSize) && (gMemoryCache[whichCache] != nil) ) {
			
				//	there is something in the cache we can use
				
				thePtr = gMemoryCache[whichCache][gMemoryStackIndex[whichCache]];
				gMemoryStackIndex[whichCache]++;
				
				//	we have something to free, so, we will make another pass through this loop
				//	unless we have freed enough memory after we get done with this pass
				
				done = false;
				totalFreed += currentFreeSize;
				
				DisposePtr((Ptr) thePtr);
				
			}	
		}
		
		//	have we freed enough yet
		
		if (totalFreed >= theSize) {
			done = true;
		}
		
	}

	MaxMem(&bigmem);
}

#endif

void* GetMemory(long theSize) {
	void*		thePtr = nil;

	#ifdef USEMALLOC
	
		thePtr = malloc(theSize);
	
	#else
	
		long		whichCache;
	
		//	there are kMemoryCaches possible lists of memory blocks in 128 byte increments. We need
		//	to compute which list the memory block is in and then allocate it
		
		whichCache = (theSize - 1) >> kMemoryShift;
	
		if (whichCache < kMemoryCaches) {
	
			//	if we don't have a collection yet, create one
			
			if (gMemoryCache[whichCache] == nil) {
				gMemoryCache[whichCache] = (void**) NewPtrClear(kObjectCacheSize * sizeof(void*));
				gMemoryStackIndex[whichCache] = kObjectCacheSize;
			}
			
			if (gMemoryStackIndex[whichCache] < kObjectCacheSize) {
			
				//	there is something in the cache we can use
				
				thePtr = gMemoryCache[whichCache][gMemoryStackIndex[whichCache]];
				gMemoryStackIndex[whichCache]++;
				
			} else {
			
				//	there is nothing in the cache, allocate it now. we recalculate the size
				//	so that this block can be used for anything which uses this cache of blocks
				
				theSize = (whichCache + 1) << kMemoryShift;			//	smallest is 128, biggest is 10,240
				
				thePtr = (void*) NewPtr(theSize);
				if (thePtr == nil) {
				
					FreeSomeMemory(2 * theSize);
				
					//	try to allocate it again
		
					thePtr = NewPtr(theSize);
				}
			}
		
		} else {
		
			//	this object is too big to be cached, so, we just allocate it normally
			
			thePtr = NewPtr(theSize);
			if (thePtr == nil) {
			
				FreeSomeMemory(2 * theSize);
			
				//	try to allocate it again
	
				thePtr = NewPtr(theSize);
			}
			
		}
	
	#endif
	
	return thePtr;
}

void FreeMemory(void* theObject) {

	#ifdef USEMALLOC
	
		free(theObject);
	
	#else
	
		long		whichCache;
		long		theSize;
	
		theSize = GetPtrSize(theObject);
		whichCache = (theSize - 1) >> kMemoryShift;
	
		if (whichCache < kMemoryCaches) {
	
			gMemoryStackIndex[whichCache]--;
		
			if (gMemoryStackIndex[whichCache] < 0) {
			
				//	there is no room in the list for another pointer
		
				#ifdef MACDEBUG
					ErrorCodeAlert(whichCache, "\pMemory Cache Is Too Small. Call Gregory. Code P1");
				#endif
				
				gMemoryStackIndex[whichCache] = 0;
				DisposePtr((Ptr) theObject);
				
			} else {
			
				gMemoryCache[whichCache][gMemoryStackIndex[whichCache]] = (void*) theObject;
				
			}
			
		} else {
		
			DisposePtr((Ptr) theObject);
		
		}

	#endif
}

