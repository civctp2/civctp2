









#include "c3.h"
#include "DebugMemory.h"
#include "c3debug.h"

sint32 g_check_mem; 

#ifndef _DEBUG_MEMORY

void* operator new(const unsigned int size)
{
    Assert (0 < size); 
	void* ptr = malloc(size);
	Assert(ptr != NULL);

	if(ptr == NULL) {
#ifdef _AIDLL
#ifdef _DEBUG
		MBCHAR s[256];
		sprintf(s, "EXE: Failed to allocate Block of size %ld\n", size);
		c3ai_Log(s);
#endif
#else
		DPRINTF(k_DBG_FIX, ("AI: Failed to allocate block of size %ld\n", size));
#endif
		exit(-1);
	}
  
	if (g_check_mem) { 
        Assert(_CrtCheckMemory()); 
    }

	return ptr;
}

void operator delete(void *ptr)
{
	Assert(ptr != NULL);
	if(ptr == NULL)
		return;
	free(ptr);
    ptr = NULL; 

    if (g_check_mem) { 
        Assert(_CrtCheckMemory());  
    }
}

#endif























