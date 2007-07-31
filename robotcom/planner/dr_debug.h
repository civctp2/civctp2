







#ifndef __DR_DEBUG_H
#define __DR_DEBUG_H

#include "DebugMemory.h"



#ifndef _DEBUG_MEMORY
	#ifndef REALLOC
		#define REALLOC(buf,size) realloc (buf,size)
	#endif

	#ifndef MALLOC
		#define MALLOC(size) malloc(size)
	#endif

	#ifndef CALLOC
		#define CALLOC(count,size) calloc(count,size)
	#endif

	#ifndef STRDUP
		#define STRDUP(str) strdup(str)
	#endif

	#ifndef FREE
		#define FREE(ptr) free(ptr)
	#endif
#endif


#define FCLOSE(fp) fclose(fp)
#define ATEXIT(func) atexit(func)
#define ASSERT(x) Assert(x)

#endif 
