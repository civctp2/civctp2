#ifdef _DEBUG












#include "log.h"
#include "debugmemory.h"
#include "debugcallstack.h"
#include "debugassert.h"
#include "breakpoint.h"
#include <windows.h>		
#include <string.h>








struct DebugMemory;
struct MemoryHeapDescriptor;
struct AllocHeader;







struct DebugMemory
{
	
	bool open;

	
	
	
	
	bool initialised;

	
	MemoryHeap default_heap;

	
	int total_outstanding;		
	int total_allocations;		
	int total_user_memory;		
	int total_overhead_memory;	
	int max_user_memory;		
	int max_overhead_memory;	

	
	bool log_callstack;			
	bool log_module_totals;		
	bool log_malloc_totals;		

	int total_from_dll;	
	int total_from_exe; 
};





static DebugMemory *debug_memory = NULL;













struct MemoryHeapDescriptor 
{
	
	
	
	int type;

	
	HANDLE handle;

	
	const char *name;

	
	
	

#ifdef MEMORY_LOGGED
	
	const char *module_name;
	int module_line;

	
	bool is_default_heap;

	
	
	bool is_open;

	
	AllocHeader *first_memory_block;	
	int total_outstanding;		
	int total_allocations;		
	int total_user_memory;		
	int total_overhead_memory;	
	int max_user_memory;		
	int max_overhead_memory;	


	
	MemoryHeap next;
	MemoryHeap last;
#endif
};



















static void DebugMemory_CreateDefaultHeap (void)
{
	MemoryHeap heap;

	
	heap = (MemoryHeap) malloc (sizeof (MemoryHeapDescriptor));

	
	heap->name = _strdup ("Default Heap");

	
	heap->type = 'HEAP';

	
	heap->handle = GetProcessHeap();

#ifdef MEMORY_LOGGED
	
	heap->module_name = "DebugMemory";
	heap->module_line = __LINE__;
	heap->is_default_heap = true;
	heap->is_open = true;

	
	heap->first_memory_block = NULL;
	heap->total_outstanding = 0;
	heap->total_allocations = 0;
	heap->total_user_memory = 0;
	heap->total_overhead_memory = 0;
	heap->max_user_memory = 0;
	heap->max_overhead_memory = 0;

	
	heap->next = NULL;
	heap->last = NULL;
#endif MEMORY_LOGGED

	debug_memory->default_heap = heap;
}










void DebugMemory_Initialise (void)
{
	
	DebugMemory_CreateDefaultHeap();
	debug_memory->initialised = true;
}

void DebugMemory_Allocate(void)
{
	debug_memory = (DebugMemory *)malloc(sizeof(DebugMemory));

	debug_memory->open = FALSE;
	debug_memory->initialised = FALSE;
	debug_memory->default_heap = NULL;
	debug_memory->total_outstanding = 0;
	debug_memory->total_allocations = 0;
	debug_memory->total_user_memory = 0;
	debug_memory->total_overhead_memory = 0;
	debug_memory->max_user_memory = 0;
	debug_memory->max_overhead_memory = 0;
	debug_memory->log_callstack = TRUE;
	debug_memory->log_module_totals = FALSE;
	debug_memory->log_malloc_totals = FALSE;

	debug_memory->total_from_dll = 0;
	debug_memory->total_from_exe = 0;
}

static inline void DebugMemory_EnsureAllocated(void)
{
	if (!debug_memory)
		DebugMemory_Allocate();
}












static inline void DebugMemory_EnsureInitialised (void)
{
	DebugMemory_EnsureAllocated();

	if (!debug_memory->initialised)
	{
		DebugMemory_Initialise();
	}
}














void DebugMemory_Open (void)
{
	
	
	DebugMemory_EnsureAllocated();

	
	ASSERT (!debug_memory->open);

	
	debug_memory->open = true;

	DebugMemory_EnsureInitialised();
}














bool g_quitfast = false;

void DebugMemory_Close (void)
{
	
	ASSERT (debug_memory->open);

#ifdef MEMORY_LOGGED
	
	if (!g_quitfast)
		DebugMemory_LeaksShow(99999);
#endif MEMORY_LOGGED

	debug_memory->open = false;

	
	free(debug_memory);
	debug_memory = NULL;
}












void DebugMemory_SetDebugMemoryPtr(void *ptr)
{
	debug_memory = (DebugMemory *)ptr;
}

void *DebugMemory_GetDebugMemoryPtr(void)
{
	return (void *)debug_memory;
}

int DebugMemory_GetTotalFromEXE(void)
{
	return debug_memory->total_from_exe;
}

int DebugMemory_GetTotalFromDLL(void)
{
	return debug_memory->total_from_dll;
}










void *DebugMemory_FastMalloc  (unsigned size)
{
	void *memory;
	
	memory = malloc (size);

	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}


void *DebugMemory_FastCalloc  (unsigned size)
{
	void *memory;
	
	memory = calloc (1, size);

	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}


void *DebugMemory_FastRealloc (void *memory_block, unsigned size)
{
	void *memory;
	
	memory = realloc (memory_block, size);

	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}



char *DebugMemory_FastStrdup  (const char *string)
{
	char *memory;
	
	memory = strdup (string);

	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}  


void  DebugMemory_FastFree    (void **memory_block_ptr)
{
	free (*memory_block_ptr);
	*memory_block_ptr = NULL;
}







#ifdef MEMORY_FAST



















MemoryHeap DebugMemoryHeap_FastOpen (const char *name, unsigned size_initial, unsigned size_maximum)
{
	MemoryHeap heap;

	DebugMemory_EnsureInitialised();

	
	heap = (MemoryHeap) MALLOC (sizeof (MemoryHeapDescriptor));

	
	heap->name = STRDUP (name);

	
	heap->type = 'HEAP';

	
	heap->handle = HeapCreate (0, size_initial, size_maximum);

	
	return (heap);
}















void DebugMemoryHeap_FastClose (MemoryHeap heap)
{
	BOOL ok;

	DebugMemory_EnsureInitialised();

	
	ASSERT_CLASS (LOG_MEMORY_FAIL, heap->type == 'HEAP');

	
	heap->type = 'heap';

	
	ok = HeapDestroy (heap->handle);
	ASSERT_CLASS (LOG_MEMORY_FAIL, ok);

	
	FREE (heap->name);

	
	FREE (heap);
}














void *DebugMemoryHeap_FastMalloc  (MemoryHeap heap, unsigned size)
{
	void *memory;

	DebugMemory_EnsureInitialised();

	ASSERT_CLASS (LOG_MEMORY_FAIL, heap);
	ASSERT_CLASS (LOG_MEMORY_FAIL, heap->type == 'HEAP');

	memory = HeapAlloc (heap->handle, 0, size);
	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}






void *DebugMemoryHeap_FastCalloc  (MemoryHeap heap, unsigned size)
{
	void *memory;

	DebugMemory_EnsureInitialised();

	ASSERT_CLASS (LOG_MEMORY_FAIL, heap);
	ASSERT_CLASS (LOG_MEMORY_FAIL, heap->type == 'HEAP');

	memory = HeapAlloc (heap->handle, HEAP_ZERO_MEMORY, size);
	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}





void *DebugMemoryHeap_FastRealloc (MemoryHeap heap, void *memory_block, unsigned size)
{
	void *memory;

	DebugMemory_EnsureInitialised();

	ASSERT_CLASS (LOG_MEMORY_FAIL, heap);
	ASSERT_CLASS (LOG_MEMORY_FAIL, heap->type == 'HEAP');

	memory = HeapReAlloc (heap->handle, 0, memory_block, size);
	if (!memory)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

	return (memory);
}





char *DebugMemoryHeap_FastStrdup  (MemoryHeap heap, const char *string)
{
	char *copy_of_string;

	DebugMemory_EnsureInitialised();

	ASSERT (string);
	copy_of_string = (char *) DebugMemoryHeap_FastMalloc (heap, strlen (string + 1));
	strcpy (copy_of_string, string);

	return (copy_of_string);
}





void  DebugMemoryHeap_FastFree    (MemoryHeap heap, void **memory_block_ptr)
{
	BOOL ok;

	DebugMemory_EnsureInitialised();

	ASSERT_CLASS (LOG_MEMORY_FAIL, heap);
	ASSERT_CLASS (LOG_MEMORY_FAIL, heap->type == 'HEAP');

	ok = HeapFree (heap->handle, 0, *memory_block_ptr);
	ASSERT_CLASS (LOG_MEMORY_FAIL, ok);
	*memory_block_ptr = NULL;
}











#endif MEMORY_FAST
#ifdef MEMORY_LOGGED


























typedef void *MemPtr;


const unsigned char FILL_BYTE_USER = 0x66;		
const unsigned char FILL_BYTE_FREE = 0x67;		
const unsigned char FILL_BYTE_MARKER = 0x68;	
const unsigned char FILL_BYTE_REALLOC = 0x69;	


const int DATA_GUARD_SIZE = 4;			
const int HEADER_GUARD_SIZE = 4;		

const int CALL_STACK_SIZE = 29;			

struct CallStack 
{
  unsigned function[CALL_STACK_SIZE];
};




struct AllocHeader
{
	unsigned char	block_marker_begin[HEADER_GUARD_SIZE];
	MemoryHeap		heap;						
	int				reference_count;			
	bool			allocated_after_open;		
	AllocHeader		*prev;						
	AllocHeader		*next;						

	const char		*filename;					
	int				line_number;				
	CallStack		call_stack;					
	int				size;						
	unsigned char	block_marker_end[HEADER_GUARD_SIZE];
};



struct AllocBuffer 
{
  unsigned char alloc_buffer[DATA_GUARD_SIZE];
};












unsigned Debug_MemoryAllocated (void)
{
  return (debug_memory->total_user_memory);
}













static inline MemPtr Debug_HeaderToData (AllocHeader *alloc_header)
{
  return ((MemPtr) ((unsigned) alloc_header + sizeof (AllocHeader) + sizeof (AllocBuffer)));
}


static inline AllocBuffer *Debug_HeaderToFirstBuffer (AllocHeader *alloc_header)
{
  return ((AllocBuffer *) ((unsigned) alloc_header + sizeof (AllocHeader)));
}


static inline AllocBuffer *Debug_HeaderToSecondBuffer (AllocHeader *alloc_header)
{
  return ((AllocBuffer *) ((unsigned) alloc_header + sizeof (AllocHeader) + sizeof (AllocBuffer) + alloc_header->size));
}


static inline AllocHeader *Debug_DataToHeader (MemPtr user_data)
{
  return ((AllocHeader *) ((unsigned) user_data - sizeof (AllocHeader) - sizeof (AllocBuffer)));
}




















int Debug_GuardedDataTest (AllocHeader * header, const char *file, int line_number)
{
  AllocBuffer *buffer_begin;
  AllocBuffer *buffer_end;
  int begin_ok;
  int end_ok;
  int i;

  buffer_begin = Debug_HeaderToFirstBuffer (header);
  buffer_end = Debug_HeaderToSecondBuffer (header);

  
  begin_ok = 1;
  end_ok = 1;
  for (i = 0; i < DATA_GUARD_SIZE; i++) {
    if (buffer_begin->alloc_buffer[i] != i)
      begin_ok = 0;
    if (buffer_end->alloc_buffer[i] != i)
      end_ok = 0;
  }

  
  if (!begin_ok) {
    LOG_INDIRECT(header->filename, header->line_number, (LOG_MEMORY_FAIL, "(Allocator) Beginning of memory block corrupted"));
    DebugCallStack_Show (LOG_MEMORY_FAIL, header->call_stack.function, CALL_STACK_SIZE);
    LOG_INDIRECT(file, line_number, (LOG_MEMORY_FAIL, "(Freeer)    Beginning of memory block corrupted"));
    DebugCallStack_Dump (LOG_MEMORY_FAIL);
  }
  if (!end_ok) {
    LOG_INDIRECT(header->filename, header->line_number, (LOG_MEMORY_FAIL, "(Allocator) End of memory block corrupted"));
    DebugCallStack_Show (LOG_MEMORY_FAIL, header->call_stack.function, CALL_STACK_SIZE);
    LOG_INDIRECT(file, line_number, (LOG_MEMORY_FAIL, "(Freeer)    End of memory block corrupted"));
    DebugCallStack_Dump (LOG_MEMORY_FAIL);
  }

  return (begin_ok && end_ok);
}












LONG _cdecl MemoryAccessExceptionFilter (LPEXCEPTION_POINTERS ep)
{
  
  if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    return (EXCEPTION_EXECUTE_HANDLER);

  
  if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
    return (EXCEPTION_EXECUTE_HANDLER);


  
  return (EXCEPTION_CONTINUE_SEARCH);
}







int Debug_GuardedHeaderTest (AllocHeader * header, const char *file, int line_number)
{
  __try {
    int ok;
    int i;

    
    ok = (header->reference_count == 1) && (header->filename);

    
    for (i = 0; i < HEADER_GUARD_SIZE; i++) {
      if ((header->block_marker_begin[i] != i) ||
          (header->block_marker_end[i] != i))
        ok = 0;
    }

    
    if (!ok) {
      LOG_INDIRECT(file, line_number, (LOG_MEMORY_FAIL, "(Invalid)    Operation on unallocated or corrupted memory block"));
      DebugCallStack_Dump (LOG_MEMORY_FAIL);
    }

    return (ok);
  }

  __except (MemoryAccessExceptionFilter (GetExceptionInformation()))
  {
    
    LOG_INDIRECT (file, line_number, (LOG_MEMORY_FAIL, "(Invalid)    Operation on unallocated or corrupted memory block"));
    DebugCallStack_Dump (LOG_MEMORY_FAIL);

    return (0);
  }
}









bool DebugMemoryHeapContainsBlock (MemoryHeap heap, MemPtr memory_block_ptr)
{
	__try 
	{
		AllocHeader *header = Debug_DataToHeader (memory_block_ptr);

		return (header->heap == heap);
	}

	__except (MemoryAccessExceptionFilter (GetExceptionInformation()))
	{
		return (false);
	}
}










bool DebugMemoryHeapValid (MemoryHeap heap)
{
	__try 
	{
		return ((heap->type == 'HEAP') && (heap->is_open));
	}

	__except (MemoryAccessExceptionFilter (GetExceptionInformation()))
	{
		return (false);
	}
}















MemPtr DebugMemory_GuardedBlockAlloc (
	const char *module_name, 
	int module_line, 
	MemoryHeap heap,
	int size,
	bool fill,
	unsigned char fill_value,
	bool track_memory)
{
	unsigned total_size;  
	AllocHeader *header;
	AllocBuffer *buffer_begin;
	AllocBuffer *buffer_end;
	unsigned i;

	
	ASSERT_CLASS (LOG_MEMORY_FAIL, DebugMemoryHeapValid (heap));

	
	ASSERT_INDIRECT (module_name, module_line, size > 0);

	
	total_size = size + sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);
	header = (AllocHeader *) malloc (total_size);
	if (header == NULL)
	{
		LOG ((LOG_FATAL, "Out of Memory"));
	}

#ifdef _AIDLL
	debug_memory->total_from_dll += size;
#else
	debug_memory->total_from_exe += size;
#endif

	
	
	

	
	header->heap = heap;

	
	header->filename = module_name;

	
	header->reference_count = 1;
	header->line_number = module_line;
	header->size = size;
	header->allocated_after_open = debug_memory->open;
	DebugCallStack_Save (header->call_stack.function, CALL_STACK_SIZE, 0);

	
	header->prev = NULL;
	header->next = heap->first_memory_block;
	heap->first_memory_block = header;

	
	if (header->next)
	{
		header->next->prev = header;
	}

	
	buffer_begin = Debug_HeaderToFirstBuffer (header);
	buffer_end = Debug_HeaderToSecondBuffer (header);

	
	for (i = 0; i < HEADER_GUARD_SIZE; i++) 
	{
		header->block_marker_begin[i] = (unsigned char) i;
		header->block_marker_end[i] = (unsigned char) i;
	}

	
	for (i = 0; i < DATA_GUARD_SIZE; i++) 
	{
		buffer_begin->alloc_buffer[i] = (unsigned char) i;
		buffer_end->alloc_buffer[i] = (unsigned char) i;
	}

	
	if (fill)
	{
		memset (Debug_HeaderToData (header), fill_value, size);
	}

	
	if (track_memory)
	{
		
		debug_memory->total_outstanding++;
		debug_memory->total_allocations ++;
		debug_memory->total_user_memory += size;
		debug_memory->total_overhead_memory += sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);

		if (debug_memory->total_user_memory > debug_memory->max_user_memory)
		{
			debug_memory->max_user_memory = debug_memory->total_user_memory;
		}

		if (debug_memory->total_overhead_memory > debug_memory->max_overhead_memory)
		{
			debug_memory->max_overhead_memory = debug_memory->total_overhead_memory;
		}

		
		heap->total_outstanding++;
		heap->total_allocations ++;
		heap->total_user_memory += size;
		heap->total_overhead_memory += sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);

		if (heap->total_user_memory > heap->max_user_memory)
		{
			heap->max_user_memory = heap->total_user_memory;
		}

		if (heap->total_overhead_memory > heap->max_overhead_memory)
		{
			heap->max_overhead_memory = heap->total_overhead_memory;
		}
	}

	
	return (Debug_HeaderToData (header));
}











void  DebugMemory_GuardedBlockFree    (
	const char *module_name, 
	int module_line, 
	MemoryHeap heap, 
	void **memory_block_ptr)
{
	AllocHeader * header;
	MemPtr mem = *memory_block_ptr;
	char *user_data;
	int i;

	
	if (!mem)
	{
		LOG_INDIRECT(module_name, module_line, (LOG_MEMORY_FAIL, "(Freeer)    FREE NULL pointer"));
		DebugCallStack_Dump (LOG_MEMORY_FAIL);
		Breakpoint();
		return;
	}

	
	ASSERT_CLASS (LOG_MEMORY_FAIL, DebugMemoryHeapValid (heap));

	


	
	if (!DebugMemoryHeapContainsBlock(heap, *memory_block_ptr)) return;

	
	header = Debug_DataToHeader (mem);

	
	if (Debug_GuardedHeaderTest (header, module_name, module_line)) 
	{
		Debug_GuardedDataTest (header, module_name, module_line);

		
		
		

		
		if (header->next)
		{
			header->next->prev = header->prev;
		}

		
		if (header->prev)
		{
			header->prev->next = header->next;
		}

		
		else
		{
			heap->first_memory_block = header->next;
		}

		
		if (header->allocated_after_open)
		{
			
			debug_memory->total_user_memory -= header->size;
			debug_memory->total_overhead_memory -= sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);
			debug_memory->total_outstanding--;

			
			heap->total_user_memory -= header->size;
			heap->total_overhead_memory -= sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);
			heap->total_outstanding--;

#ifdef _AIDLL
	debug_memory->total_from_dll -= header->size;
#else
	debug_memory->total_from_exe -= header->size;
#endif
		}

		
		header->reference_count = 0;
		header->filename = NULL;
		
		for (i = 0; i < HEADER_GUARD_SIZE; i++) 
		{
			header->block_marker_begin[i] = FILL_BYTE_MARKER;
			header->block_marker_end[i] = FILL_BYTE_MARKER;
		}

		

		user_data = (char *) mem;
		for (i = 0; i < header->size; i++)
		{
			user_data[i] = FILL_BYTE_FREE;
		}

		
		free (header);

	}

	
	*memory_block_ptr = NULL;
}













MemPtr DebugMemory_GuardedBlockRealloc (
	const char *module_name, 
	int module_line, 
	MemoryHeap heap, 
	MemPtr old_mem, 
	int new_size)
{
	MemPtr           new_mem;
	AllocHeader * old_header;
	AllocHeader * new_header;
	unsigned char   *old_byte;
	unsigned char   *new_byte;
	int              index;
	int              max_index;
	int              old_size;

	
	ASSERT_INDIRECT (module_name, module_line, new_size);
	ASSERT_INDIRECT (module_name, module_line, old_mem);

	
	old_header = Debug_DataToHeader (old_mem);
	old_size = old_header->size;

	
	if (!Debug_GuardedHeaderTest (old_header, module_name, module_line)) {
		LOG_INDIRECT(module_name, module_line, (LOG_MEMORY_FAIL, "(Realloc)   Re-allocate unallocated or corrupted memory"));
		ASSERT_CLASS (LOG_MEMORY_FAIL, 1 == 0);
	}

	
	ASSERT_CLASS (LOG_MEMORY_FAIL, DebugMemoryHeapValid (heap));

	
	ASSERT_CLASS (LOG_MEMORY_FAIL, DebugMemoryHeapContainsBlock (heap, old_mem));

	
	Debug_GuardedDataTest (old_header, module_name, module_line);

	
	new_mem = DebugMemory_GuardedBlockAlloc (module_name, module_line, heap, new_size, false, 0, old_header->allocated_after_open);
	new_header = Debug_DataToHeader (new_mem);

	
	index = 0;
	old_byte = (unsigned char *) old_mem;
	new_byte = (unsigned char *) new_mem;
	max_index = (old_size < new_size) ? old_size : new_size;

	
	while (index < max_index) {
		*new_byte = *old_byte;
		new_byte++;
		old_byte++;
		index ++;
	}

	
	while (index < new_size) {
		*new_byte = FILL_BYTE_REALLOC;
		new_byte++;
		index ++;
	}

	
	new_header->allocated_after_open = old_header->allocated_after_open;

	
	DebugMemory_GuardedBlockFree (module_name, module_line, heap, &old_mem);

	
	return (new_mem);
}















MemoryHeap DebugMemoryHeap_GuardedOpen (const char *file, int line, const char *name, unsigned size_initial, unsigned size_maximum)
{
	MemoryHeap heap;
	MemoryHeap descriptor_ptr;

	
	heap = (MemoryHeap) malloc (sizeof (MemoryHeapDescriptor));

	
	heap->name = _strdup (name);

	
	heap->type = 'HEAP';

	
	heap->handle = HeapCreate (HEAP_GENERATE_EXCEPTIONS, size_initial, size_maximum);

	
	heap->module_name = file;
	heap->module_line = line;
	heap->is_default_heap = false;
	heap->is_open = true;

	
	heap->first_memory_block = NULL;
	heap->total_outstanding = 0;
	heap->total_allocations = 0;
	heap->total_user_memory = 0;
	heap->total_overhead_memory = 0;
	heap->max_user_memory = 0;
	heap->max_overhead_memory = 0;

	
	
	
	descriptor_ptr = debug_memory->default_heap;
	while (descriptor_ptr->next)
	{
		descriptor_ptr = descriptor_ptr->next;
	}
	heap->next = NULL;
	heap->last = descriptor_ptr;
	descriptor_ptr->next = heap;

	return (heap);
}
















void DebugMemoryHeap_GuardedClose    (const char *file, int line, MemoryHeap heap)
{
	BOOL ok;

	
	ASSERT_CLASS (LOG_MEMORY_FAIL, DebugMemoryHeapValid (heap));

	
	heap->is_open = false;

	
	if (heap->total_outstanding == 0)
	{
		ok = HeapDestroy (heap->handle);
		ASSERT_CLASS (LOG_MEMORY_FAIL, ok);
	}

	


	

}






















int Debug_GuardedValidate (const char *file, int line, void *p)
{
  AllocHeader *header;

  header = Debug_DataToHeader (p);

  if (! Debug_GuardedHeaderTest (header, file, line))
    return (0);

  else if (! Debug_GuardedDataTest (header, file, line))
    return (0);

  else
    return (1);
}








int Debug_GuardedValidateHeap (const char *file, int line, MemoryHeap *heap)
{
  AllocHeader *header;
  MemPtr mem;
  int ok;

  ok = 1;

  
  header = debug_memory->default_heap->first_memory_block;

  
  while (header) {
    mem = Debug_HeaderToData (header);
    if (!Debug_GuardedValidate (file, line, mem))
      ok = 0;

    header = header->next;
  }


  return (ok);
}









int Debug_GuardedValidateAll (const char *file, int line)
{
  AllocHeader *header;
  MemPtr mem;
  int ok;

  ok = 1;

  
  header = debug_memory->default_heap->first_memory_block;

  
  while (header) {
    mem = Debug_HeaderToData (header);
    if (!Debug_GuardedValidate (file, line, mem))
      ok = 0;

    header = header->next;
  }


  return (ok);
}





















struct MemNode
{
	MemNode            *next;
	const char         *filename;
	int                 line_number;
	int					size;
	int                 reference_count;
	CallStack           call_stack;
};

static MemNode *mem_node = NULL;











void Debug_MemNodeAdd (const char *filename, int line_number, int size, CallStack call_stack)
{
  MemNode *node;
  MemNode *new_node;

  
  if (!mem_node) {
    new_node = (MemNode *) malloc (sizeof (MemNode));

    if (!new_node)
    {
		LOG ((LOG_FATAL, "Out of Memory"));
    }

    new_node->next = NULL;
    new_node->filename = filename;
    new_node->line_number = line_number;
    new_node->reference_count = 1;
    new_node->call_stack = call_stack;
	new_node->size = size;
    mem_node = new_node;
    return;
  }


  
  node = mem_node;
  while (node) {
    int match = 0;
    int i;



	
	if (node->size == size) {
		match = 1;
		
		for (i = 0; i < CALL_STACK_SIZE; i++) {
			if (node->call_stack.function[i] != call_stack.function[i]) {
				match = 0;
				break;
			}
		}
	}

















    
    if (match) {
      node->reference_count ++;
      return;
    }

    
    if (node->next == NULL) {
      new_node = (MemNode *) malloc (sizeof (MemNode));
      if (!new_node)
      {
		LOG ((LOG_FATAL, "Out of Memory"));
      }
      new_node->next = NULL;
      new_node->filename = filename;
      new_node->line_number = line_number;
      new_node->reference_count = 1;
      new_node->call_stack = call_stack;
	  new_node->size = size;
      node->next = new_node;
      return;
    }

    
    node = node->next;
  }
}








void Debug_MemNodeList (int turn_count)
{
	MemNode *node;

	char fname[120];
	sprintf(fname, "CTP_LEAKS_%#.3ld.TXT", turn_count);
	FILE *leakFile = fopen(fname, "w");

	if (!leakFile)
		return;

	node = mem_node;
	int counter = 0;

	if (node == NULL) {
		fprintf(leakFile, "None.\n");
		fclose(leakFile);
		return;
	}

	fprintf(leakFile, "Num\tSize\tTotal\tStack\tAllocator\n");

	while (node) {
		fprintf(leakFile, "%ld\t%ld\t%ld\t", node->reference_count, node->size, 
												node->reference_count * node->size);

		
		


		DebugCallStack_ShowToFile(LOG_MEMORY_LEAK, node->call_stack.function, CALL_STACK_SIZE, leakFile);
		
		node = node->next;
		counter++;
	}

	fclose(leakFile);


}






void Debug_MemNodeOpen (void)
{
  mem_node = NULL;
}







void Debug_MemNodeClose ()
{
  MemNode *node;
  MemNode *next_node;

  node = mem_node;
  while (node) {
    next_node = node->next;
    free (node);
    node = next_node;
  }
}










void DebugMemoryHeap_LeaksShow (MemoryHeap heap, int turn_count)
{
	AllocHeader *header;

	
	LOG ((LOG_MEMORY_LEAK, "-------------------------------------------"));
	LOG ((LOG_MEMORY_LEAK, "Heap: '%s' %s", heap->name, (heap->is_default_heap ? "(Default)" : (heap->is_open ? "(OPEN!)" : "(Closed)"))));

	
	if (heap->total_outstanding != 0)
	{
		LOG ((LOG_MEMORY_LEAK, "(Memcheck)  %d outstanding memory allocations for %d bytes",
			heap->total_outstanding,
			heap->total_user_memory));
	}
	else 
	{
		LOG ((LOG_MEMORY_LEAK, "(Memcheck)  0 outstanding memory allocations for 0 bytes"));
	}

	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Total allocations %8d", heap->total_allocations));
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Max reserved      %8d bytes", heap->max_user_memory));
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Max overhead      %8d bytes", heap->max_overhead_memory));
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Total reserved    %8d bytes", heap->max_user_memory + heap->max_overhead_memory));

	
	Debug_MemNodeOpen();
	header = heap->first_memory_block;

	
	
	int counter = 0;
	while (header) 
	{
		
		if (header->allocated_after_open)
		{
			Debug_MemNodeAdd (header->filename, header->line_number, header->size, header->call_stack);
		}
		header = header->next;
		counter++;

	}

	
	Debug_MemNodeList(turn_count);
	Debug_MemNodeClose();
}









void DebugMemoryHeap_LeaksClear(MemoryHeap heap)
{
	AllocHeader		*header;

	header = heap->first_memory_block;

	while (header) {
		
		if (header->allocated_after_open)
		{
			
			header->allocated_after_open = FALSE;
		
			
			debug_memory->total_user_memory -= header->size;
			debug_memory->total_overhead_memory -= sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);
			debug_memory->total_outstanding--;
			debug_memory->total_allocations--;

			
			heap->total_user_memory -= header->size;
			heap->total_overhead_memory -= sizeof (AllocHeader) + sizeof (AllocBuffer) + sizeof (AllocBuffer);
			heap->total_outstanding--;
			heap->total_allocations--;
		}

		header = header->next;
	}

	debug_memory->total_outstanding = 0;
	debug_memory->total_allocations = 0;
	debug_memory->total_user_memory = 0;
	debug_memory->total_overhead_memory = 0;
	debug_memory->max_user_memory = 0;
	debug_memory->max_overhead_memory = 0;
	debug_memory->total_from_dll = 0;
	debug_memory->total_from_exe = 0;

	heap->total_outstanding = 0;
	heap->total_allocations = 0;
	heap->total_user_memory = 0;
	heap->total_overhead_memory = 0;
	heap->max_user_memory = 0;
	heap->max_overhead_memory = 0;
}





void DebugMemory_LeaksShow (int turn_count)
{
	MemoryHeap heap;

	if (!debug_memory) return;

		
	LOG ((LOG_MEMORY_LEAK, "-------------------------------------------"));
	LOG ((LOG_MEMORY_LEAK, "Totals from all heaps"));

	
	if (debug_memory->total_outstanding != 0)
	{
		LOG ((LOG_MEMORY_LEAK, "(Memcheck)  %d outstanding memory allocations for %d bytes",
			debug_memory->total_outstanding,
			debug_memory->total_user_memory));
	}
	else
	{
		LOG ((LOG_MEMORY_LEAK, "(Memcheck)  0 outstanding memory allocations for 0 bytes"));
	}

	
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Total allocations %8d", debug_memory->total_allocations));
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Max reserved      %8d bytes", debug_memory->max_user_memory));
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Max overhead      %8d bytes", debug_memory->max_overhead_memory));
	LOG ((LOG_MEMORY_LEAK, "(Memcheck)  Total reserved    %8d bytes", debug_memory->max_user_memory + debug_memory->max_overhead_memory));

	
	heap = debug_memory->default_heap;
	while (heap)
	{
		DebugMemoryHeap_LeaksShow (heap, turn_count);
		heap = heap->next;
	}
}






void DebugMemory_LeaksClear(void)
{
	MemoryHeap heap;

	if (!debug_memory) return;

	
	heap = debug_memory->default_heap;
	while (heap)
	{
		DebugMemoryHeap_LeaksClear(heap);
		heap = heap->next;
	}
}





























void *DebugMemory_GuardedMalloc  (const char *file, int line, unsigned size)
{
	DebugMemory_EnsureInitialised();
	return (DebugMemory_GuardedBlockAlloc (file, line, debug_memory->default_heap, size, true, FILL_BYTE_USER, debug_memory->open));
}

void *DebugMemory_GuardedCalloc  (const char *file, int line, unsigned size)
{
	DebugMemory_EnsureInitialised();
	return (DebugMemory_GuardedBlockAlloc (file, line, debug_memory->default_heap, size, true, 0, debug_memory->open));
}

void *DebugMemory_GuardedRealloc (const char *file, int line, void *memory_block, int size)
{
	DebugMemory_EnsureInitialised();
	return (DebugMemory_GuardedBlockRealloc (file, line, debug_memory->default_heap, memory_block, size));
}

char *DebugMemory_GuardedStrdup  (const char *file, int line, const char *string)
{
	char  *ptr;

	
	DebugMemory_EnsureInitialised();
	ptr = (char *) DebugMemory_GuardedBlockAlloc (file, line, debug_memory->default_heap, strlen (string) + 1, false, 0, debug_memory->open);
	strcpy (ptr, string);
	return (ptr);
}

void  DebugMemory_GuardedFree    (const char *file, int line, void **memory_block_ptr)
{
	DebugMemory_EnsureInitialised();
	DebugMemory_GuardedBlockFree (file, line, debug_memory->default_heap, memory_block_ptr);
}

















void *DebugMemoryHeap_GuardedMalloc  (const char *file, int line, MemoryHeap heap, unsigned size)
{
	DebugMemory_EnsureInitialised();
	return (DebugMemory_GuardedBlockAlloc (file, line, heap, size, true, FILL_BYTE_USER, debug_memory->open));
}


void *DebugMemoryHeap_GuardedCalloc  (const char *file, int line, MemoryHeap heap, unsigned size)
{
	DebugMemory_EnsureInitialised();
	return (DebugMemory_GuardedBlockAlloc (file, line, heap, size, true, 0, debug_memory->open));
}


void *DebugMemoryHeap_GuardedRealloc (const char *file, int line, MemoryHeap heap, void *memory_block, int size)
{
	DebugMemory_EnsureInitialised();
	return (DebugMemory_GuardedBlockRealloc (file, line, heap, memory_block, size));
}


char *DebugMemoryHeap_GuardedStrdup  (const char *file, int line, MemoryHeap heap, const char *string)
{
	char  *ptr;

	DebugMemory_EnsureInitialised();
	
	ptr = (char *) DebugMemory_GuardedBlockAlloc (file, line, heap, strlen (string) + 1, false, 0, debug_memory->open);
	strcpy (ptr, string);
	return (ptr);
}


void  DebugMemoryHeap_GuardedFree    (const char *file, int line, MemoryHeap heap, void **memory_block_ptr)
{
	DebugMemory_EnsureInitialised();
	DebugMemory_GuardedBlockFree (file, line, heap, memory_block_ptr);
}















#ifdef _DEBUG_MEMORY

void * CDECL operator new (size_t size)
{
	return (MALLOC (size));
}






void CDECL operator delete (void *mem)
{
	
	
	if (mem)
	{
		FREE (mem);
	}






}

#endif




#endif MEMORY_LOGGED

#endif 
