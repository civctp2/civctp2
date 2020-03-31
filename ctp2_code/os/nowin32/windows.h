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

#define HAVE_UNISTD_H

#include "ctp2_config.h" // Needs HAVE_CONFIG_H defined, defines HAVE_INTTYPES_H and HAVE_STDINT_H

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include "ctp2_inttypes.h"

/* Windows data types */
#define BOOL uint32
#define FALSE 0
#define TRUE 1
typedef char CHAR;
typedef char TCHAR;
typedef uint32 COLORREF;
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
typedef void * PVOID;

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
#ifdef __cplusplus
typedef struct _GUID
{
    uint32 Data1;
    uint16 Data2;
    uint16 Data3;
    uint8  Data4[8];
} GUID;

inline bool operator==(const GUID & guidOne, const GUID & guidOther)
{
	return memcmp(&guidOne, &guidOther, sizeof(GUID)) == 0;
}

inline bool operator!=(const GUID & guidOne, const GUID & guidOther)
{
	return memcmp(&guidOne, &guidOther, sizeof(GUID)) != 0;
}
#endif

typedef struct tagBITMAPFILEHEADER
{
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagRGBQUAD
{
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

typedef struct tagRECT
{
  sint32 left;
  sint32 top;
  sint32 right;
  sint32 bottom;
} RECT;

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

#define INVALID_HANDLE_VALUE ((HANDLE) ~0UL)

// MessageBox buttons
#define MB_OK                0x00000000L
#define MB_OKCANCEL          0x00000001L
#define MB_ABORTRETRYIGNORE  0x00000002L
#define MB_YESNOCANCEL       0x00000003L
#define MB_YESNO             0x00000004L
#define MB_RETRYCANCEL       0x00000005L
#define MB_CANCELTRYCONTINUE 0x00000006L
#define MB_HELP              0x00004000L

// MessageBox default button
#define MB_DEFBUTTON1        0x00000000L
#define MB_DEFBUTTON2        0x00000100L
#define MB_DEFBUTTON3        0x00000200L
#define MB_DEFBUTTON4        0x00000300L

// MessageBox signs
#define MB_ICONSTOP          0x00000010L
#define MB_ICONERROR         0x00000010L
#define MB_ICONHAND          0x00000010L
#define MB_ICONQUESTION      0x00000020L
#define MB_ICONEXCLAMATION   0x00000030L
#define MB_ICONWARNING       0x00000030L
#define MB_ICONINFORMATION   0x00000040L
#define MB_ICONASTERISK      0x00000040L

// MessageBox return values
#define IDFAIL      0
#define IDOK        1
#define IDCANCEL    2
#define IDABORT     3
#define IDRETRY     4
#define IDIGNORE    5
#define IDYES       6
#define IDNO        7
#define IDTRYAGAIN 10
#define IDCONTINUE 11

/* Constants */
#define _MAX_FNAME 256
#define _MAX_DIR _MAX_FNAME
#define _MAX_PATH PATH_MAX
#define DBL_MAX        1.7976931348623158e+308
#define MAX_PATH PATH_MAX
#define SND_ASYNC 0x0001
#define SND_FILENAME 0x00020000L
#define SND_NOWAIT   0x00002000L

#define VK_BACK   0x08
#define VK_TAB    0x09
#define VK_RETURN 0x0D
#define VK_SHIFT  0x10
#define VK_ESCAPE 0x1B
#define VK_END    0x23
#define VK_HOME   0x24
#define VK_LEFT   0x25
#define VK_UP     0x26
#define VK_RIGHT  0x27
#define VK_DOWN   0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_F1     0x70
#define VK_F2     0x71
#define VK_F3     0x72
#define VK_F4     0x73
#define VK_F5     0x74
#define VK_F6     0x75
#define VK_F7     0x76
#define VK_F8     0x77
#define VK_F9     0x78
#define VK_F10    0x79
#define VK_F11    0x7A
#define VK_F12    0x7B

#define WM_CLOSE       0x0010
#define WM_QUIT        0x0012
#define WM_KEYDOWN     0x0100
#define WM_KEYUP       0x0101
#define WM_CHAR        0x0102
#define WM_SYSKEYDOWN  0x0104
#define WM_SYSKEYUP    0x0105
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP   0x0202
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP   0x0205
#define WM_MBUTTONDOWN 0x0207
#define WM_MBUTTONUP   0x0208
/* Makros */
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
char* _fullpath(char*, const char*, size_t);
void _splitpath(const char*,char*,char*,char*,char*);
void CopyRect(RECT *dest, RECT *src);
uint32 GetTickCount();
uint8 GetRValue(COLORREF c);
uint8 GetGValue(COLORREF c);
uint8 GetBValue(COLORREF c);
sint32 MessageBox(HWND parent, const CHAR* msg, const CHAR* title, sint32 flags);
void InflateRect(RECT *pr, int x, int y);
void OffsetRect(RECT *pr, int x, int y);
BOOL PtInRect(RECT* pr, struct POINT m);
void SetRect(RECT* R, int left, int top, int right, int bottom);
void SubtractRect(RECT* r, const RECT* a, const RECT* b);
#define lstrlen(s) strlen(s)
#ifndef stricmp
int stricmp(const char* s1, const char* s2);
#endif
#ifndef strnicmp
int strnicmp(const char *str1, const char *str2, size_t n);
#endif
int _stricoll(const char *str1, const char *str2);
char* strupr(char* str);

#endif
