
#include "c3.h"

#include "DebugCallStack.h"

#include "Log.h"
#include "DebugCallStack.h"
#include <windows.h>		
#include <string.h>
#include <stdio.h>
#include <imagehlp.h>      

#ifndef _DEBUG
#include "log_off.h"
#endif

static bool debug_dump_whole_stack = false;





void Debug_FunctionNameInit (void);
int Debug_FunctionNameOpen (char *map_file_name);


BOOL CALLBACK	Debug_EnumSymbolsCallback(LPSTR symbolName, ULONG symbolAddress, ULONG symbolSize, PVOID userContext);
BOOL CALLBACK	Debug_EnumModulesCallback(LPSTR moduleName, ULONG dllBase, PVOID userContext);
int				Debug_FunctionNameOpenFromPDB(void);


void Debug_FunctionNameClose (void);
char *Debug_FunctionNameGet (unsigned address);


#define DEBUG_CODE_LIMIT 0x80000000 
#define BUFFER_SIZE      (2000)


#ifdef _DEBUG
#define k_MAP_FILE "civctp_dbg.map"
#else
#ifndef _BFR_
#define k_MAP_FILE "civctp_rel.map"
#else
#define k_MAP_FILE "ctp2.map"
#endif
#endif






#define k_STACK_TRACE_LEN 32768


const int k_CALL_STACK_SIZE = 29;			


static MBCHAR s_stackTraceString[k_STACK_TRACE_LEN];












void DebugCallStack_Open (void)
{
	
	Debug_FunctionNameOpen (k_MAP_FILE);
}














void DebugCallStack_Close (void)
{
	Debug_FunctionNameClose();
}














static LONG _cdecl MemoryAccessExceptionFilter (LPEXCEPTION_POINTERS ep)
{
  
  if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    return (EXCEPTION_EXECUTE_HANDLER);

  
  if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
    return (EXCEPTION_EXECUTE_HANDLER);


  
  return (EXCEPTION_CONTINUE_SEARCH);
}













static char *unknown = "(unknown)";  
static char *kernel  = "(kernel)";   
static int function_name_open = 0;   




typedef struct tagFUNCTION_ADDRESS {
  struct tagFUNCTION_ADDRESS * next;
  unsigned address;
  char *name;
} FUNCTION_ADDRESS;

static FUNCTION_ADDRESS *fa_first = NULL;  


void *Debug_GetFAFirst(void)
{
	return (void *)fa_first;
}

void Debug_SetFAFirst(void *ptr)
{
	fa_first = (FUNCTION_ADDRESS *)ptr;
	function_name_open = TRUE;
}




void Debug_AddFunction (char *name, unsigned address)
{
  FUNCTION_ADDRESS *new_function;
  FUNCTION_ADDRESS *pointer;
  FUNCTION_ADDRESS *last;

  
  new_function = (FUNCTION_ADDRESS *) malloc (sizeof (FUNCTION_ADDRESS));
  if (!new_function)
  {
    LOG ((LOG_FATAL, "Out of Memory"));
  }
  new_function->address = address;

  
  if (name[0] == '?')
  {
    char buff[BUFFER_SIZE];

#ifndef _BFR_
    if (UnDecorateSymbolName(name, buff, BUFFER_SIZE-1, 0))
    {
      new_function->name = strdup(buff);
    }
    else
#endif
    {
      new_function->name = strdup(name);
    }
  }

  
  else if (name[0] == '_')
  {
    new_function->name = strdup (name + 1);
  }

  
  else
  {
    new_function->name = strdup (name);
  }

  

  
  if (fa_first == NULL) {
    new_function->next = NULL;
    fa_first = new_function;
    return;
  }

  
  if (address >= fa_first->address) {
    new_function->next = fa_first;
    fa_first = new_function;
    return;
  }

  
  pointer = fa_first;
  while ((pointer) && (address < pointer->address)) {
    
    last = pointer;
    pointer = pointer->next;
  }

  
  new_function->next = last->next;
  last->next = new_function;
}














void Debug_FunctionNameInit (void)
{
	fa_first = NULL;
}

int Debug_FunctionNameOpen (char *map_file_name)
{
  FILE *fp;
  char buffer[BUFFER_SIZE];
  int done;
  int crap;
  char name[BUFFER_SIZE];
  unsigned address;
#ifdef REQUIRE_CORRECT_LOG
  int log_correct;
#endif

  
  fp = fopen (map_file_name, "rt");
  if (!fp) {
#ifdef REQUIRE_CORRECT_LOG
    LOG ((LOG_FATAL, "I need the correct map file to run."));
#else
    return (0);
#endif
  }

  
  done = 0;
  while (!done) {
    fgets (buffer, BUFFER_SIZE, fp);

    
    if (feof (fp))
      done = 1;

    
    if (sscanf (buffer, "%s", name) == 1) {
      if (strcmp (name, "Address") == 0)
        done = 1;
    }
  }

  
  fgets (buffer, BUFFER_SIZE, fp);

  
  done = 0;
  while (!done) {

    
    fgets (buffer, BUFFER_SIZE, fp);

    
    if ((buffer[0] == ' ') &&
        (buffer[1] == '0') &&
        (buffer[5] == ':') &&

        
        (sscanf (buffer, "%x:%x %s %x", &crap, &crap, name, &address) == 4))
    {
      Debug_AddFunction (name, address);
    }

    
    if (feof (fp))
      done = 1;
  }

  
  fclose (fp);


  function_name_open = 1;
  return (1);

  #undef BUFFER_SIZE
}



static HANDLE	hProc,					
				hThread;				






BOOL CALLBACK Debug_EnumSymbolsCallback(LPSTR symbolName, ULONG symbolAddress, 
								ULONG symbolSize, PVOID userContext)
{
	
	Debug_AddFunction ((char *)symbolName, (unsigned)symbolAddress);

	return TRUE;
}






BOOL CALLBACK Debug_EnumModulesCallback(LPSTR moduleName, ULONG dllBase, PVOID userContext)
{
	int					err;

#ifndef _BFR_
	
	if (!SymEnumerateSymbols(hProc, dllBase, Debug_EnumSymbolsCallback, userContext)) {
		err = GetLastError();
		LOG ((LOG_FATAL, "SymEnumerateSymbols failed in module '%s' with error %d", moduleName, err));
	
		return FALSE;
	}
#endif
	
	return TRUE;
}











void Debug_FunctionNameClose (void)
{
  FUNCTION_ADDRESS *pointer = fa_first;
  FUNCTION_ADDRESS *next;
#ifdef GENERATE_ADDRESS_LOG
  FILE *fh;
#endif

#ifdef GENERATE_ADDRESS_LOG
  fh = fopen("address.log","w");
#endif

  function_name_open = 0;

  
  while (pointer) {
#ifdef GENERATE_ADDRESS_LOG
    fprintf(fh,"%08X %s\n",pointer->address,pointer->name);
#endif

    next = pointer->next;
    free (pointer->name);
    free (pointer);
    pointer = next;
  }

#ifdef GENERATE_ADDRESS_LOG
  fclose (fh);
#endif
}












char *Debug_FunctionNameGet (unsigned address)
{
  FUNCTION_ADDRESS *pointer;

  if (address > DEBUG_CODE_LIMIT)
  {
    return (kernel);
  }

  
  if (!function_name_open)
  {
    return (unknown);
  }
  else 
  {
    
    pointer = fa_first;

    
    while (pointer) {
      if (address >= pointer->address)
        return (pointer->name);

      pointer = pointer->next;
    }

    
    return (unknown);
  }
}



char *Debug_FunctionNameAndOffsetGet (unsigned address, int *offset)
{
	FUNCTION_ADDRESS *pointer;
	*offset = 0;
	
	if (address > DEBUG_CODE_LIMIT)
	{
		return (kernel);
	}
	
	
	if (!function_name_open)
	{
		return (unknown);
	}
	else 
	{
		
		pointer = fa_first;
		
		
		while (pointer) {
			if (address >= pointer->address) {
				*offset = address - pointer->address;
				return (pointer->name);
			}
			
			pointer = pointer->next;
		}
		
		
		return (unknown);
	}
}




























unsigned char Debug_NumToChar (unsigned char byte)
{
  if ((byte >= 32) && (byte <= 127))
    return (byte);
  else
    return ('.');
}











void DebugCallStack_DumpAddress (LogClass log_class, unsigned address)
{
	char	*caller_name;
	int		offset;


	caller_name = Debug_FunctionNameAndOffsetGet (address, &offset);

	LOG_INDIRECT (NULL, 0, (log_class, "  0x%08x  [%s + 0x%x]\n", address, caller_name, offset));
}







void DebugCallStack_Dump (LogClass log_class)
{
	
	unsigned base_pointer;   
	__asm mov base_pointer, ebp;

	
	DebugCallStack_DumpFrom (log_class, base_pointer);
}






void DebugCallStack_DumpFrom (LogClass log_class, unsigned base_pointer)
{
  unsigned frame_limit;    
  unsigned frame_pointer;  
  unsigned caller;         
  int finished;            
  int registers_printed;   

  int debug_dump_whole_stack = 0; 

  unsigned regs_eax;
  unsigned regs_ebx;
  unsigned regs_ecx;
  unsigned regs_edx;
  unsigned regs_esi;
  unsigned regs_edi;
  unsigned regs_esp;
  unsigned regs_ebp;

  
  __asm mov regs_eax, eax;
  __asm mov regs_ebx, ebx;
  __asm mov regs_ecx, ecx;
  __asm mov regs_edx, edx;
  __asm mov regs_esi, esi;
  __asm mov regs_edi, edi;
  __asm mov regs_esp, esp;
  __asm mov regs_ebp, ebp;

  
  finished = 0;
  registers_printed = 0;
  while (!finished) 
  {

    
    __try
    {
      caller = *((unsigned *) (base_pointer + 4));
    }
    __except (MemoryAccessExceptionFilter (GetExceptionInformation()))
    {
      finished = 1;
    }


    
    if (caller >= DEBUG_CODE_LIMIT)
    {
      finished = 1;
    }

    
    if (!finished) 
    {
      
		DebugCallStack_DumpAddress (log_class, caller);

      
      
      if ((debug_dump_whole_stack) && (!registers_printed)) 
	  {
        LOG_INDIRECT (NULL, 0, (log_class, "    EAX %08x  EBX %08x  ECX %08x  EDX %08x\n",
          regs_eax,
          regs_ebx,
          regs_ecx,
          regs_edx));
        LOG_INDIRECT (NULL, 0, (log_class, "    ESI %08x  EDI %08x  ESP %08x  EBP %08x\n",
          regs_esi,
          regs_edi,
          regs_esp,
          regs_ebp));

        registers_printed = 1;
      }

      
      
      
      
      
      
      
      
      
      
      
      
      
      

      frame_pointer = base_pointer + 8;             
      frame_limit = * ((unsigned *) base_pointer);  
      if (debug_dump_whole_stack) 
	  {
        while (frame_pointer < frame_limit) 
		{
          LOG_INDIRECT(NULL, 0, (log_class, "    %02x %02x %02x %02x    '%c%c%c%c'\n",
            (unsigned) (* ((unsigned char *) (frame_pointer + 0))),
            (unsigned) (* ((unsigned char *) (frame_pointer + 1))),
            (unsigned) (* ((unsigned char *) (frame_pointer + 2))),
            (unsigned) (* ((unsigned char *) (frame_pointer + 3))),
            Debug_NumToChar (* ((unsigned char *) (frame_pointer + 0))),
            Debug_NumToChar (* ((unsigned char *) (frame_pointer + 1))),
            Debug_NumToChar (* ((unsigned char *) (frame_pointer + 2))),
            Debug_NumToChar (* ((unsigned char *) (frame_pointer + 3)))
          ));
          frame_pointer += 4;
        }
      }

      
      base_pointer = frame_limit;
    }
  }
}


















void DebugCallStack_Save  (unsigned *call_stack, int number, unsigned long Ebp)
{
  unsigned base_pointer;   
  unsigned caller;         
  int finished;            
  int index;               

  if(Ebp == 0) {
	  
	  __asm mov base_pointer, ebp;
  } else {
	  base_pointer = Ebp;
  }

  
  finished = 0;
  caller = 0;
  index = 0;
  while ((!finished) && ( index < number)) 
  {

    
    if ((caller >= DEBUG_CODE_LIMIT) || (base_pointer == 0))
    {
      finished = 1;
    }
    else
    {
      
      __try
      {
        caller = *((unsigned *) (base_pointer + 4));
      }
      __except (MemoryAccessExceptionFilter (GetExceptionInformation()))
      {
        finished = 1;
      }
    }

    
    if (!finished) {

      
      call_stack[index] = caller;


      
      base_pointer = * ((unsigned *) base_pointer);

	  
	  if ((index != 0) && (call_stack[index - 1] == call_stack[index]))
	  {
		  
	  }
	  else
	  {
	      
		  index ++;
	  }
    }
  }

  
  while (index < number) {
    call_stack[index] = 0;

    index ++;
  }
}








void DebugCallStack_Show  (LogClass log_class, unsigned *call_stack, int number)
{
  unsigned caller;         
  char *caller_name;       
  int index;               


  
  index = 0;
  while ((index < number) && (call_stack[index] != 0)) {

    
    caller = call_stack[index];

	int offset;
	
	caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

	LOG_INDIRECT (NULL, 0, (log_class, "  0x%08x  [%s + 0x%x]\n", caller, caller_name, offset));






    index ++;
  }
}


void DebugCallStack_ShowToFile  (LogClass log_class, unsigned *call_stack, int number, FILE *file)
{
	unsigned caller;         
	char *caller_name;       
	int index;               
	
	char allocator_name[1024];
	
	
	index = 0;
	while ((index < number) && (call_stack[index] != 0)) {
		
		
		caller = call_stack[index];
		
		int offset;
		
		caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);
		
		if (index == 3) {
			strcpy(allocator_name, caller_name);
		}

		fprintf(file, "0x%08x [%s+0x%x] / ", caller, caller_name, offset);
		
		
		
		index ++;
	}

	fprintf(file, "\t%s\n", allocator_name);
}








char * c3debug_StackTrace(void)
{
	unsigned caller;					
	char *caller_name;					
	int index;							
	MBCHAR function_name[16384];	

	
	unsigned callstack_function[k_CALL_STACK_SIZE];

	
	DebugCallStack_Save(callstack_function, k_CALL_STACK_SIZE, 0);

	
	s_stackTraceString[0] = 0;

	
	index = 0;
	while ((index < k_CALL_STACK_SIZE) && (callstack_function[index] != 0)) 
	{
		
		caller = callstack_function[index];

		
		int offset;

		
		caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

		
		sprintf(function_name, "  0x%08x  [%s + 0x%x]\n", caller, caller_name, offset);

		
		if (strlen(s_stackTraceString) + strlen(function_name) < k_STACK_TRACE_LEN - 1 )
			strcat(s_stackTraceString, function_name);

		
		index ++;
	}

	
	return s_stackTraceString;
}

char * c3debug_ExceptionStackTrace(LPEXCEPTION_POINTERS exception)
{
	if(!function_name_open) {
		DebugCallStack_Open();
	}

	unsigned caller;					
	char *caller_name;					
	int index;							
	MBCHAR function_name[_MAX_PATH];	

	
	unsigned callstack_function[k_CALL_STACK_SIZE];

	
	callstack_function[0] = exception->ContextRecord->Eip;

	
	DebugCallStack_Save(&callstack_function[1], k_CALL_STACK_SIZE - 1, exception->ContextRecord->Ebp);

	
	s_stackTraceString[0] = 0;

	
	index = 0;
	while ((index < k_CALL_STACK_SIZE) && (callstack_function[index] != 0)) 
	{
		
		caller = callstack_function[index];

		
		int offset;


		if(function_name_open) {
			
			caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

			
			sprintf(function_name, "  0x%08x  [%s + 0x%x]\n", caller, caller_name, offset);
			
		} else {
			sprintf(function_name, "  0x%08x\n", caller);
		}

		
		if (strlen(s_stackTraceString) + strlen(function_name) < k_STACK_TRACE_LEN - 1 )
			strcat(s_stackTraceString, function_name);

		
		
		index ++;
	}

	
	return s_stackTraceString;
}
 
char * c3debug_ExceptionStackTraceFromFile(FILE *f)
{
	if(!function_name_open) {
		DebugCallStack_Open();
	}


	unsigned caller;
	char line[1024];
	char *caller_name;					

	int offset;

	MBCHAR function_name[_MAX_PATH];	
	s_stackTraceString[0] = 0;
	while(!feof(f)) {
		if(fgets(line, 1024, f)) {
			if(sscanf(line, "  0x%08x", &caller) == 1) {
				caller_name = Debug_FunctionNameAndOffsetGet(caller, &offset);
				
				
				sprintf(function_name, "  0x%08x  [%s + 0x%x]\n", caller, caller_name, offset);

				
				if (strlen(s_stackTraceString) + strlen(function_name) < k_STACK_TRACE_LEN - 1 )
					strcat(s_stackTraceString, function_name);
			}
		}
	}
	return s_stackTraceString;
}






















cDebugCallStackSet::cDebugCallStackSet(int depth)
{
	m_depth = depth;
	m_blockSize = m_depth + 1;	
	m_numStacks = 0;
	
	m_maxNumStacks = 16;
	m_caller = 0;	
	
	
	m_stacks = new unsigned[m_blockSize*m_maxNumStacks];
	
	m_curStack = new unsigned[m_depth+2];

}

cDebugCallStackSet::~cDebugCallStackSet()
{		
	delete [] m_stacks;
	delete [] m_curStack;
}

static int qsortDebugCallStack(const void *a,const void *b)
{
	return (int)(*((unsigned*)b)) - (int)(*((unsigned*)a));
}




void cDebugCallStackSet::Add()
{
	
	
	DebugCallStack_Save(m_curStack,m_depth+2,0);
	
	
	int i;
	for (i=0;i<m_numStacks;++i) {
		if (memcmp(m_curStack+2,&m_stacks[m_blockSize*i+1],sizeof(unsigned)*m_depth)==0) {
			m_stacks[m_blockSize*i]++;

			if (m_stacks[m_blockSize*i]%16 == 0) {
				qsort(m_stacks,m_numStacks,m_blockSize*sizeof(unsigned),qsortDebugCallStack);
			}

			return;
		}
	}

	
	
	if (m_numStacks>=m_maxNumStacks) {
		
		int newMax = m_maxNumStacks*=2;
		unsigned *pNewStack = new unsigned[m_blockSize * newMax];
		
		memcpy(pNewStack,m_stacks,m_numStacks*m_blockSize*sizeof(unsigned));
		
		delete [] m_stacks;
		m_stacks = pNewStack;
		m_maxNumStacks = newMax;
	}

	m_caller = m_curStack[1];
	memcpy(&m_stacks[m_blockSize*i+1],m_curStack+2,sizeof(unsigned)*m_depth);
	m_stacks[m_blockSize*i] = 1;
	m_numStacks++;

	
	
	
	qsort(m_stacks,m_numStacks,m_blockSize*sizeof(unsigned),qsortDebugCallStack);

	
	if (m_numStacks > 1) {
		Assert(m_stacks[0] >= m_stacks[m_blockSize]);
	}
}


void cDebugCallStackSet::Dump(const char *filename)
{		
	const char *caller_name;	
	int offset;

	
	qsort(m_stacks,m_numStacks,m_blockSize*sizeof(unsigned),qsortDebugCallStack);

	
	
	FILE *fp = fopen(filename,"wt");

	
	caller_name = Debug_FunctionNameAndOffsetGet(m_caller,&offset);


	int totalCalls = 0;
	int i;
	for (i=0;i<m_numStacks;++i) {
		totalCalls += m_stacks[m_blockSize*i];
	}

	fprintf(fp,"callstack dump for function:\n%s\ncalled %d times\n",caller_name,totalCalls);

	for (i=0;i<m_numStacks;++i) {
		int called = m_stacks[m_blockSize*i];
		float perCalled = (float)(100 * called)/(float)totalCalls;
		fprintf(fp,"\nnum times called: %d, %.2f percent\n",called,perCalled);
		int j;
		for (j=0;j<m_depth;++j) {
			unsigned caller = m_stacks[(m_blockSize*i)+1+j];
			caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);
			fprintf(fp,"%s\n",caller_name);
		}
	}
	fclose(fp);
}






