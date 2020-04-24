//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Debug memory handling
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG
// - Generate debug version
//
// MEMORY_LOGGED
// - ??
//
// MEMORY_FAST
// - ??
//
// _AIDLL
// - ??
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Debug memory uses size_t for things that are memory sizes (20-Jan-2019 Martin Gühmann)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __DEBUGMEMORY_H
#define __DEBUGMEMORY_H

#ifdef _DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DEBUG_MEMORY
	#define MEMORY_FAST
#else
	#define MEMORY_LOGGED
#endif

void DebugMemory_Open (void);
void DebugMemory_Close (void);
void DebugMemory_LeaksShow(int turn_count);
void DebugMemory_LeaksClear(void);
void DebugMemory_SetDebugMemoryPtr(void *ptr);
void *DebugMemory_GetDebugMemoryPtr(void);
size_t DebugMemory_GetTotalFromEXE(void);
size_t DebugMemory_GetTotalFromDLL(void);

struct MemoryHeapDescriptor;
typedef struct MemoryHeapDescriptor *MemoryHeap;

int Debug_GuardedValidate     (const char *file, int line, void *p);
int Debug_GuardedValidateHeap (const char *file, int line, MemoryHeap *heap);
int Debug_GuardedValidateAll  (const char *file, int line);

#define DM_VALIDATE(x) Debug_GuardedValidate(__FILE__, __LINE__, x)
#define DM_VALIDATE_ALL() Debug_GuardedValidateAll(__FILE__, __LINE__)

void *DebugMemory_FastMalloc  (size_t size);
void *DebugMemory_FastCalloc  (size_t size);
void *DebugMemory_FastRealloc (void *memory_block, size_t size);
char *DebugMemory_FastStrdup  (const char *string);
void  DebugMemory_FastFree    (void **memory_block_ptr);

void *DebugMemory_GuardedMalloc  (const char *file, int line, size_t size);
void *DebugMemory_GuardedCalloc  (const char *file, int line, size_t size);
void *DebugMemory_GuardedRealloc (const char *file, int line, void *memory_block, size_t size);
char *DebugMemory_GuardedStrdup  (const char *file, int line, const char *string);
void  DebugMemory_GuardedFree    (const char *file, int line, void **memory_block_ptr);

#ifdef MEMORY_FAST

	#define MALLOC(size)        DebugMemory_FastMalloc  (size)
	#define CALLOC(count,size)  DebugMemory_FastCalloc  ((count) * (size))
	#define REALLOC(buf,size)   DebugMemory_FastRealloc (buf,size)
	#define STRDUP(str)         DebugMemory_FastStrdup  (str)
	#define FREE(ptr)           DebugMemory_FastFree    ((void**)&(ptr))

#else /* if MEMORY_LOGGED */

	#define MALLOC(size)        DebugMemory_GuardedMalloc  ((__FILE__), (__LINE__), (size))
	#define CALLOC(count,size)  DebugMemory_GuardedCalloc  ((__FILE__), (__LINE__), (count) * (size))
	#define REALLOC(buf,size)   DebugMemory_GuardedRealloc ((__FILE__), (__LINE__), (buf), (size))
	#define STRDUP(str)         DebugMemory_GuardedStrdup  ((__FILE__), (__LINE__), (str))
	#define FREE(ptr)           DebugMemory_GuardedFree    ((__FILE__), (__LINE__), (void **) & (ptr))

#endif

void *DebugMemoryHeap_FastMalloc    (MemoryHeap heap, size_t size);
void *DebugMemoryHeap_FastCalloc    (MemoryHeap heap, size_t size);
void *DebugMemoryHeap_FastRealloc   (MemoryHeap heap, void *memory_block, size_t size);
char *DebugMemoryHeap_FastStrdup    (MemoryHeap heap, const char *string);
void  DebugMemoryHeap_FastFree      (MemoryHeap heap, void **memory_block_ptr);
MemoryHeap DebugMemoryHeap_FastOpen (const char *name, size_t size_initial, size_t size_maximum);
void DebugMemoryHeap_FastClose      (MemoryHeap heap);

void *DebugMemoryHeap_GuardedMalloc    (const char *file, int line, MemoryHeap heap, size_t size);
void *DebugMemoryHeap_GuardedCalloc    (const char *file, int line, MemoryHeap heap, size_t size);
void *DebugMemoryHeap_GuardedRealloc   (const char *file, int line, MemoryHeap heap, void *memory_block, size_t size);
char *DebugMemoryHeap_GuardedStrdup    (const char *file, int line, MemoryHeap heap, const char *string);
void  DebugMemoryHeap_GuardedFree      (const char *file, int line, MemoryHeap heap, void **memory_block_ptr);
#ifdef WIN32
MemoryHeap DebugMemoryHeap_GuardedOpen (const char *file, int line, const char *name, size_t size_initial, size_t size_maximum);
void DebugMemoryHeap_GuardedClose      (const char *file, int line, MemoryHeap heap);
#endif

#ifdef MEMORY_FAST

	#define HEAP_MALLOC(heap,size)        DebugMemoryHeap_FastMalloc  (heap, size)
	#define HEAP_CALLOC(heap,count,size)  DebugMemoryHeap_FastCalloc  (heap, (count) * (size))
	#define HEAP_REALLOC(heap,buf,size)   DebugMemoryHeap_FastRealloc (heap, buf,size)
	#define HEAP_STRDUP(heap,str)         DebugMemoryHeap_FastStrdup  (heap, str)
	#define HEAP_FREE(heap,ptr)           DebugMemoryHeap_FastFree    (heap, (void**)&(ptr))
	#define HEAP_OPEN(name,s_i,s_m)       DebugMemoryHeap_FastOpen    (name, s_i, s_m)
	#define HEAP_CLOSE(heap)              DebugMemoryHeap_FastClose   (heap)

#else /* if MEMORY_LOGGED */

	#define HEAP_MALLOC(heap,size)        DebugMemoryHeap_GuardedMalloc  ((__FILE__), (__LINE__), heap, (size))
	#define HEAP_CALLOC(heap,count,size)  DebugMemoryHeap_GuardedCalloc  ((__FILE__), (__LINE__), heap, (count) * (size))
	#define HEAP_REALLOC(heap,buf,size)   DebugMemoryHeap_GuardedRealloc ((__FILE__), (__LINE__), heap, (buf), (size))
	#define HEAP_STRDUP(heap,str)         DebugMemoryHeap_GuardedStrdup  ((__FILE__), (__LINE__), heap, (str))
	#define HEAP_FREE(heap,ptr)           DebugMemoryHeap_GuardedFree    ((__FILE__), (__LINE__), heap, (void **) & (ptr))
	#define HEAP_OPEN(name,s_i,s_m)       DebugMemoryHeap_GuardedOpen    ((__FILE__), (__LINE__), name, s_i, s_m)
	#define HEAP_CLOSE(heap)              DebugMemoryHeap_GuardedClose   ((__FILE__), (__LINE__), heap)

#endif

#ifdef __cplusplus
}
#endif

#endif // _DEBUG

#endif // __DEBUGMEMORY_H
