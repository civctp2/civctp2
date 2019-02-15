#include "c3.h"
#include "debugmemory.h"
#include "c3debug.h"

sint32 g_check_mem;

#if !defined(_DEBUG_MEMORY) // Some error checking for the final version

void* operator new(const size_t size)
{
	Assert (0 < size);
	void* ptr = malloc(size);
	Assert(ptr != NULL);

	if(ptr == NULL)
	{
#if defined(_AIDLL)
#if defined(_DEBUG)
		MBCHAR s[256];
		sprintf(s, "EXE: Failed to allocate Block of size %ld\n", size);
		c3ai_Log(s);
#endif
#else
		DPRINTF(k_DBG_FIX, ("AI: Failed to allocate block of size %ld\n", size));
#endif
		exit(-1);
	}

#if defined(WIN32)
	if (g_check_mem)
	{
		Assert(_CrtCheckMemory());
	}
#endif

	return ptr;
}

void operator delete(void *ptr)
{
	if(ptr == NULL)
		return;

	free(ptr);
	ptr = NULL;

#if defined(WIN32)
	if (g_check_mem)
	{
		Assert(_CrtCheckMemory());
	}
#endif
}

#endif
