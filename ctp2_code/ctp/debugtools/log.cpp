#ifdef _DEBUG



























#include "log.h"
#include "breakpoint.h"
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <crtdbg.h>
#include <mmsystem.h>


LogClass LOG_FATAL = "Fatal";
LogClass LOG_ERR = "Error";
LogClass LOG_WARN = "Warning";
LogClass LOG_DIAG = "Diagnostic";

LogClass LOG_ASSERT = "Assertion";
LogClass LOG_EXCEPTION = "Exception";
LogClass LOG_MEMORY_LEAK = "MemoryLeak";	
LogClass LOG_MEMORY_FAIL = "MemoryFail";	

static LogClass LOG_LOG_CLASSES = "LogClasses";  

#define k_DEBUG_MAX_LOG_FILE_SIZE		(5 * 1024 * 1024)		















static const int HashTableSize = 1024;  







struct HashTableEntry
{
	HashTableEntry *next;
	char *key;
};

typedef HashTableEntry * HashTableEntry_Ptr;






struct HashTable
{
	HashTableEntry_Ptr bucket[HashTableSize];
};






static HashTable hash_table;








static inline unsigned Hash (const char *hash_key)
{
	unsigned hash_value;		
	unsigned rotate_bits;		

	hash_value = 0;

	
	
	
	while (*hash_key)
	{
		
		
		
		hash_value = hash_value ^ (*hash_key & 0x1f);

		
		rotate_bits = hash_value >> 8;
		hash_value = hash_value << 2;
		hash_value = (hash_value & 0x03fc) | (rotate_bits & 0x0003);

		
		hash_key++;
	}

	return (hash_value);
}














void Hash_Init (void)
{
	int bucket;

	for (bucket = 0; bucket < HashTableSize; bucket++)
	{
		hash_table.bucket[bucket] = NULL;
	}
}












static void Hash_CloseEntry (HashTableEntry_Ptr entry)
{
	
	if (entry)
	{
		
		Hash_CloseEntry (entry->next);

		
		free (entry->key);
		
		
		free (entry);
	}
}


static void Hash_Close (void)
{
	int bucket;

	
	for (bucket = 0; bucket < HashTableSize; bucket++)
	{
		
		Hash_CloseEntry (hash_table.bucket[bucket]);
	}
}













static bool Hash_Exist (const char *hash_key)
{
	int					hash_value;		
	HashTableEntry_Ptr	hash_entry;		

	
	hash_value = Hash (hash_key);
	hash_entry = hash_table.bucket[hash_value];
	
	
	while (hash_entry)
	{
		
		if (_stricmp (hash_key, hash_entry->key) == 0)
		{
			return (true);
		}

		
		else
		{
			hash_entry = hash_entry->next;
		}
	}

	
	return (false);
}














void Hash_Add (const char *hash_key)
{
	int					hash_value;		
	HashTableEntry_Ptr	hash_entry;		

	
	if (!Hash_Exist (hash_key))
	{
		
		hash_value = Hash (hash_key);
		hash_entry = (HashTableEntry_Ptr) malloc (sizeof (HashTableEntry));

		
		hash_entry->key = _strdup (hash_key);
		hash_entry->next = hash_table.bucket[hash_value];

		
		hash_table.bucket[hash_value] = hash_entry;
	}
}















static void Hash_DumpEntry (HashTableEntry_Ptr entry, int bucket)
{
	char bucket2[11] = "0000000000";
	int i;
	int mask;

	
	if (entry)
	{
		
		Hash_DumpEntry (entry->next, bucket);

		
		for (i = 0, mask = 512; i < 10; i++, mask >>= 1)
		{
			if (bucket & mask)
			{
				bucket2[i] = '1';
			}
		}

		
		LOG (("LogClasses", "  %-20s [%04d-%s]", entry->key, bucket, bucket2));
	}
}


static void Hash_Dump (void)
{
	int bucket;

	
	for (bucket = 0; bucket < HashTableSize; bucket ++)
	{
		
		Hash_DumpEntry (hash_table.bucket[bucket], bucket);
	}
}











struct Logging
{
	
	bool				open;					
	const char *		module_name;			
	int					module_line;			

	
	bool				to_file;				
	bool				to_debugger;			
	FILE				*log_file;
	char *				log_filename;			
	bool				leave_open;				
	
	bool				log_all;				


	int					base_time;				
	CRITICAL_SECTION	entered;				
};





static Logging *logging = NULL;

void Log_Allocate(void)
{
	logging = (Logging *)malloc(sizeof(Logging));
	logging->open = FALSE;
	logging->module_name = NULL;
	logging->module_line = 0;
	logging->to_file = FALSE;
	logging->to_debugger = FALSE;
	logging->log_filename = NULL;
	logging->leave_open = FALSE;
	logging->log_all = FALSE;
	logging->base_time = 0;
	logging->log_file = NULL;
}

static inline void Log_EnsureLogAllocated(void)
{
	if (!logging)
		Log_Allocate();
}





void Log_SetLoggingPtr(void *ptr)
{
	logging = (Logging *)ptr;
}

void *Log_GetLoggingPtr(void)
{
	return (void *)logging;
}








static void	Log_InitReadConfig (const char *config_filename, const char *log_filename)
{
	FILE *config_file;			
	FILE *log_file;				
	char *ok;					
	const int buffer_size = 100;
	char buffer[buffer_size];
	int i;

	
	

	{
		logging->log_all = true;
		logging->to_debugger = false;
		logging->to_file = true;
	}

	
	if (config_filename)
	{
		config_file = fopen (config_filename, "rt");
		if (config_file)
		{
			do
			{
				
				ok = fgets (buffer, buffer_size, config_file);
				for (i = 0; buffer[i] > ' '; i++) {};
				buffer[i] = 0;

				
				if ((ok) && (buffer[0] != '#') && (buffer[0] != ';') && (buffer[0] != '/') && (buffer[0] != 0))
				{
					
					if ((buffer[0] == '-') && (buffer[1] == '>'))
					{
						if (strcmp (&buffer[2], "file") == 0)
						{
							logging->to_file = true;
						}
						else if (strcmp (&buffer[2], "debugger") == 0)
						{
							logging->to_debugger = true;
						}

					}

					
					else
					{
						Hash_Add (buffer);
					}

				}
			} while (ok);

			
			fclose (config_file);
		}
	}


	
	if (!log_filename)
	{
		logging->to_file = false;
	}


	
	if (logging->to_file)
	{
		log_file = fopen (log_filename, "wt");
		if (log_file)
		{
			if (!logging->leave_open) {
				fclose (log_file);
			} else {
				logging->log_file = log_file;
			}
		}
		else
		{
			Breakpoint();  
			logging->to_file = false;
		}
	}

	
	if (logging->to_file)
	{
		logging->log_filename = _strdup (log_filename);
	}
}








































































void Log_AddLoggingClasses(void)
{
	Hash_Add (LOG_LOG_CLASSES);
	Hash_Add (LOG_FATAL);
	Hash_Add (LOG_ERR);
	Hash_Add (LOG_WARN);
	Hash_Add (LOG_ASSERT);
	Hash_Add (LOG_EXCEPTION);
	Hash_Add (LOG_MEMORY_LEAK);
	Hash_Add (LOG_MEMORY_FAIL);

}

void Log_Open (const char *config_file, const char *log_file)
{
	Log_EnsureLogAllocated();

	
	if (logging->open)
	{
		Breakpoint();
		return;
	}

	
	InitializeCriticalSection (&logging->entered);
	EnterCriticalSection (&logging->entered);

	
	Hash_Init();
	Log_AddLoggingClasses();

	
	logging->open = true;
	logging->module_name = NULL;
	logging->module_line = 0;
	logging->base_time = timeGetTime();
	logging->to_debugger = false;
	logging->to_file = false;
	logging->log_filename = NULL;
	logging->log_all = false;
	
	logging->leave_open = TRUE;


	
	Log_InitReadConfig (config_file, log_file);

	
	LeaveCriticalSection (&logging->entered);

	
	LOG (("LogClasses", "Logging the Following LogClasses:"));
	Hash_Dump();
}











void Log_Close (void)
{
	Log_EnsureLogAllocated();

	
	if (!logging->open)
	{
		Breakpoint();
		return;
	}

	if (logging->leave_open) {
		fclose(logging->log_file);
		logging->log_file = NULL;
	}

	Hash_Close();
	logging->open = false;
	
	if (logging->log_filename)
	{
		free (logging->log_filename);
	}

	DeleteCriticalSection (&logging->entered);

	
	free(logging);
	logging = NULL;

}











void Log_Enable (LogClass log_class)
{
	
	Log_EnsureLogAllocated();

	
	if (!logging->open)
	{
		Breakpoint();
		return;
	}

	
	Hash_Add (log_class);
}















int Log_Enabled (LogClass log_class)
{
	
	Log_EnsureLogAllocated();

	
	if (!logging->open)
	{
		Breakpoint();
		return (false);
	}

	
	return (Hash_Exist (log_class) ? 1 : 0);
}













int Log_OutputEnabled (void)
{
	
	Log_EnsureLogAllocated();

	
	if (!logging->open)
	{		
		return (false);
	}

	
	return (logging->to_file | logging->to_debugger) ? 1 : 0;
}














void Log_Begin (const char *module_name, int module_line)
{
	
	Log_EnsureLogAllocated();

	
	if (!logging->open)
	{
		return;
	}

	
	EnterCriticalSection (&logging->entered);

	
	
	if (module_name)
	{
		logging->module_name = module_name;
	}
	else
	{
		logging->module_name = "";
	}

	logging->module_line = module_line;
}


















static inline void Log_MiddleToDebugger (char *message)
{
	
	Log_EnsureLogAllocated();

	if (logging->to_debugger)
	{
	    _RPT0 (_CRT_WARN, message);
	    _RPT0 (_CRT_WARN, "\n");
	}
}






static inline void Log_MiddleToFile (char *message)
{
	FILE *log_file;

	
	Log_EnsureLogAllocated();

	
	if (logging->to_file)
	{
		if (!logging->leave_open) 
			log_file = fopen (logging->log_filename, "a+t");
		else
			log_file = logging->log_file;

		if (log_file)
		{
			fprintf (log_file, "%s\n", message);

			if (!logging->leave_open)
				fclose (log_file);
	    }
	}
}





static inline void Log_MiddleCreateMessage (char *message, const char *message_text, LogClass log_class)
{
	int elapsed_time;
	char *adjusted_text;		
	char *adjusted_text_scan;
	char *adjusted_module;
	char *adjusted_module_scan;

	
	elapsed_time = timeGetTime() - logging->base_time;

	
	
	

	
	adjusted_module = _strdup (logging->module_name);
	adjusted_module_scan = adjusted_module + (strlen (adjusted_module));

	
	
	while ((adjusted_module != adjusted_module_scan) && 
	       (* (adjusted_module_scan - 1) != '\\'))
	{
		
		if (*adjusted_module_scan == '.')
		{
			*adjusted_module_scan = 0;
		}

		adjusted_module_scan--;
	}

	
	
	

	adjusted_text = _strdup (message_text);
	for (adjusted_text_scan = adjusted_text; *adjusted_text_scan; adjusted_text_scan++)
	{
		if (*adjusted_text_scan < ' ')
		{
			*adjusted_text_scan = ' ';
		}
	}


	
	
	sprintf (message, "[%12s::%15s::%5d@%7d] %s", 
		log_class, 
		adjusted_module_scan, 
		logging->module_line, 
		elapsed_time,
		adjusted_text);

	
	free (adjusted_text);
	free (adjusted_module);
}






void __cdecl Log_Middle (LogClass log_class, const char *format, ...)
{
	
	Log_EnsureLogAllocated();

	
	
	if (!logging->open)
	{
		Breakpoint();
		return;
	}

	
	if (logging->to_file || logging->to_debugger)
	{
		
		if (Hash_Exist (log_class))
		{
			
			va_list       variable_argument_list;	
			char message[1000];						
			char message_text[1000];				


			
			va_start (variable_argument_list, format);
			vsprintf (message_text, format, variable_argument_list);
			va_end (variable_argument_list);

			
			Log_MiddleCreateMessage (message, message_text, log_class);

			
			Log_MiddleToFile (message);
			Log_MiddleToDebugger (message);
		}
	}

	
	if (log_class == LOG_FATAL)
	{
		Breakpoint();
	}
}













void Log_End (void)
{
	
	Log_EnsureLogAllocated();

	
	if (!logging->open)
	{
		return;
	}

	
	LeaveCriticalSection (&logging->entered);
}

#endif 
