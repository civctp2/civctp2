//
//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Windows stub header for non-Windows systems
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
//----------------------------------------------------------------------------
#ifndef __WINDOWS_H__
#define __WINDOWS_H__ 1

#if defined(WIN32)
#error "WIN32 defined, but included windows.h from os/nowin32!"
#endif

#include "tchar.h"

/* Windows data types */
typedef uint32 BOOL;
const BOOL FALSE = 0;
const BOOL TRUE = 1;
typedef char   CHAR;
typedef uint32 COLORREF;
typedef sint32 GUID;
typedef sint32 HRESULT;
typedef sint32 LPARAM;
typedef const CHAR *LPCSTR;
typedef const TCHAR *LPCTSTR;
typedef CHAR * LPSTR;
typedef TCHAR * LPTSTR;
typedef void * LPVOID;
typedef sint32 LRESULT;
typedef CHAR * PSTR;
typedef sint32 (*WNDPROC)();
typedef sint32 WPARAM;
typedef void * HMONITOR;
typedef void * LPDDSURFACEDESC;

/* Standard datatypes */
typedef uint8  BYTE;
typedef uint16 WORD;
typedef uint32 DWORD;
typedef sint32 INT;
typedef sint32 LONG;
typedef uint32 UINT;
typedef uint32 ULONG;

/* Structs */
typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;
struct POINT {
  sint32 x;
  sint32 y;
};
typedef struct POINT * LPPOINT;

struct RECT {
  sint32 left;
  sint32 top;
  sint32 right;
  sint32 bottom;
};

/* Stub definitions */
typedef struct critical_section_t {} CRITICAL_SECTION;
typedef CRITICAL_SECTION *LPCRITICAL_SECTION;

typedef struct handle_t* HANDLE;
typedef struct hbitmap_t* HBITMAP;
typedef struct hdc_t* HDC;
typedef struct hinstance_t* HINSTANCE;
typedef struct hwnd_t* HWND;

/* Constants */
#define MB_OK 0
#define MB_ICONEXCLAMATION 0

/* Makros */
#define CALLBACK
#define RGB(r,g,b) ((BYTE)(b) << 16 | (BYTE)(g) << 8 | (BYTE)(r))
#define WINAPI

/* stub functions */
sint32 MessageBox(HWND parent, const CHAR* msg, const CHAR* title, sint32 flags);
inline void InflateRect(struct RECT *pr, int x, int y)
{
	if (!pr)
		return;
	
	pr->bottom += y;
	pr->left -= x;
	pr->right += x;
	pr->top -= y;
}
inline void OffsetRect(struct RECT *pr, int x, int y)
{
	if (!pr)
		return;

	pr->bottom += y;
	pr->left += x;
	pr->right += x;
	pr->top += y;
}
#define lstrlen(s) strlen(s)
inline int stricmp(const char* s1, const char* s2)
{
	return strcasecmp(s1, s2);
}
inline int strnicmp(const char *str1, const char *str2, size_t n)
{
	return strncasecmp(str1, str2, n);
}


#endif
