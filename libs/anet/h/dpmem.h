






















#ifndef dpmem_h
#define dpmem_h

#if defined(_WIN32)

#include <windows.h>

#define dp_MALLOC(s)     GlobalAlloc(GMEM_FIXED, s)
#define dp_REALLOC(p, s) GlobalReAlloc(p, s, GMEM_MOVEABLE)
#define dp_FREE(p)       GlobalFree(p)
#else
#include <stdlib.h>
#define dp_MALLOC(s)     malloc(s)
#define dp_REALLOC(p, s) realloc(p, s)
#define dp_FREE(p)       free(p)
#endif

#endif
