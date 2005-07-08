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

#include <assert.h>
#include <strings.h>
#include "tchar.h"

/* Windows data types */
#define BOOL uint32
#define FALSE 0
#define TRUE 1
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
typedef sint16 SHORT;
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
typedef struct tagRGBQUAD {
	uint8 rgbBlue;
	uint8 rgbGreen;
	uint8 rgbRed;
	uint8 rgbReserved;
} RGBQUAD;
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
typedef struct hfont_t* HFONT;
typedef struct hinstance_t* HINSTANCE;
typedef struct hpalette_t* HPALETTE;
typedef struct hwnd_t* HWND;

/* Constants */
#define _MAX_FNAME 256
#define _MAX_DIR _MAX_FNAME
#define _MAX_PATH PATH_MAX
#define MAX_PATH PATH_MAX
#define MB_OK 0
#define MB_ICONEXCLAMATION 0
#define SND_ASYNC 0x0001
#define SND_FILENAME 0x00020000L
#define SND_NOWAIT   0x00002000L

/* Makros */
#define _ASSERTE(x) assert(x)
#define CALLBACK
#ifdef WORDS_BIGENDIAN
#define HIWORD(w) ((WORD)(w))
#define LOWORD(w) ((WORD)((w)>>16))
#else
#define LOWORD(w) ((WORD)(w))
#define HIWORD(w) ((WORD)((w)>>16))
#endif
#define RGB(r,g,b) ((BYTE)(b) << 16 | (BYTE)(g) << 8 | (BYTE)(r))
#define WINAPI

/* stub functions */
char* _fullpath(char*, const char*, int);
void _splitpath(const char*,char*,char*,char*,char*);
void CopyRect(struct RECT *dest, struct RECT *src);
uint32 GetTickCount();
uint8 GetRValue(COLORREF c);
uint8 GetGValue(COLORREF c);
uint8 GetBValue(COLORREF c);
sint32 MessageBox(HWND parent, const CHAR* msg, const CHAR* title, sint32 flags);
void InflateRect(struct RECT *pr, int x, int y);
void OffsetRect(struct RECT *pr, int x, int y);
BOOL PtInRect(struct RECT* pr, struct POINT m);
void SetRect(struct RECT* R, int left, int top, int right, int bottom);
void SubtractRect(struct RECT* r, const struct RECT* a, const struct RECT* b);
#define lstrlen(s) strlen(s)
int stricmp(const char* s1, const char* s2);
int strnicmp(const char *str1, const char *str2, size_t n);
char* strupr(char* str);

#endif
