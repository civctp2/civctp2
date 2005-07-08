#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#ifndef WIN32

#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include "windows.h"
#include <SDL.h>

char*
_fullpath(char*, const char*, int)
{
	assert(0);
}

void
_splitpath(const char*,char*,char*,char*,char*)
{
	assert(0);
}

uint32
GetTickCount()
{
	return SDL_GetTicks();
}

sint32
MessageBox(HWND parent, const CHAR* msg, const CHAR* title, sint32 flags)
{
	return 0;
}

void
SubtractRect(struct RECT* dst, const struct RECT* r1, const struct RECT* r2)
{
    bool xoverlap =
	r1->left >= r2->left &&
	r1->right <= r2->right;
    bool yoverlap =
	r1->top >= r2->top &&
	r1->bottom <= r2->bottom;
    if (xoverlap && yoverlap) {
	dst->left = dst->right = 0;
	dst->top = dst->bottom = 0;
	return;
    }
    // all cases not handled below return r1
    *dst = *r1;
    if (xoverlap) {
	if (r1->top < r2->top && r1->bottom <= r2->bottom) {
	    // keep upper part
	    //      ------  r1
	    //     | dst  |
	    //   --+------+---  r2
	    //  |  |      |   |
	    //  |   ------    |
	    //   -------------
	    dst->bottom = r2->top;
	} else if (r1->top >= r2->top && r1->bottom > r1->bottom) {
	    // keep lower part
	    //   -------------
	    //  |   ------    |
	    //  |  |      |   |
	    //   --+------+---  r2
	    //     | dst  |
	    //      ------  r1
	    dst->top = r2->bottom;
	}
    } else if (yoverlap) {
	if (r1->left < r2->left && r1->right <= r2->bottom) {
	    // keep left part
	    //      ---- r2
	    // r1  |    |
	    //  ---+-   |
	    // |dst| |  |
	    //  ---+-   |
	    //      ----
	    dst->right = r2->left;
	} else if (r1->left >= r2->left && r1->right > r2->right) {
	    // keep right part
	    //  ---- r2
	    // |    |
	    // |  --+--- r1
	    // | |  |dst|
	    // |  --+---
	    //  ----
	    dst->left = r2->right;
	}
    }
}

char*
strupr(char* str)
{
	char *buf = str;
	if (!buf)
		return NULL;

	while (*buf != '\0') {
		*buf = toupper(*buf);
		++buf;
	}

	return str;
}


void CopyRect(struct RECT *dest, struct RECT *src) {
	*dest = *src;
}

uint8 GetRValue(COLORREF c)
{
	return c & 0xff;
}
uint8 GetGValue(COLORREF c)
{
	return (c>>8) & 0xff;
}
uint8 GetBValue(COLORREF c)
{
	return (c>>16) & 0xff;
}
void InflateRect(struct RECT *pr, int x, int y)
{
	if (!pr)
		return;
	
	pr->bottom += y;
	pr->left -= x;
	pr->right += x;
	pr->top -= y;
}
void OffsetRect(struct RECT *pr, int x, int y)
{
	if (!pr)
		return;

	pr->bottom += y;
	pr->left += x;
	pr->right += x;
	pr->top += y;
}
BOOL PtInRect(struct RECT* pr, struct POINT m)
{
	if (!pr)
		return FALSE;
		
	return pr->left <= m.x && m.x < pr->right && pr->top <= m.y && m.y < pr->bottom;
}

void SetRect(struct RECT* R, int left, int top, int right, int bottom)
{ 
	R->left = left;
	R->right = right;
	R->top = top;
	R->bottom = bottom;
}
int stricmp(const char* s1, const char* s2)
{
	return strcasecmp(s1, s2);
}
int strnicmp(const char *str1, const char *str2, size_t n)
{
	return strncasecmp(str1, str2, n);
}

#endif // !WIN32
