#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#ifndef WIN32

#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <ctype.h>
#include "windows.h"
#include <SDL.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef USE_GTK
#include <gtk/gtk.h>
#endif

char*
_fullpath(char* absolute, const char* relative, size_t bufsize)
{
	char ret[MAX_PATH] = { 0 };
	char *dest = (absolute == NULL) ? ret : absolute;
	size_t size = (absolute == NULL) ? MAX_PATH : bufsize;

	if (!relative) {
		getcwd(dest, size - 1);
		dest[size - 1] = '\0';
	} else if (relative[0] == FILE_SEPC) {
		strncpy(dest, relative, size - 1);
		dest[size - 1] = '\0';
	} else {
#ifdef __USE_GNU
		char *abs = canonicalize_file_name(relative);
		if (abs) {
			strncpy(dest, abs, size - 1);
			dest[size - 1] = '\0';
			free(abs);
		} else {
			return NULL;
		}
#elif defined(BSD) || defined(__USE_BSD)
		char rlpath[PATH_MAX] = { 0 };
		char *abs = realpath(relative, rlpath);
		if (abs) {
			if (!absolute) {
				return strdup(rlpath);
			} else {
				strncpy(dest, rlpath, size - 1);
				dest[size - 1] = '\0';
				return dest;
			}
		} else {
			return NULL;
		}
#else
#error "_fullpath() not implemented for this platform."
#endif
	}

	if (absolute)
		return dest;
	else
		return strdup(dest);
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

namespace {
	int mbRetVal = 0;
}

sint32
MessageBox(HWND parent, const CHAR* msg, const CHAR* title, sint32 flags)
{
	fprintf(stderr, "Messagebox(%s): %s\n",
	        (title ? title : "null"), (msg ? msg : "null"));
#ifdef USE_GTK
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(
		NULL,
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_NONE,
		"%s\n\n%s",
		title,
		msg
	);
	if ((flags & MB_YESNO) == MB_YESNO) {
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),
			GTK_STOCK_YES,
			GTK_STOCK_NO,
			NULL);
	} else {
		gtk_dialog_add_buttons(GTK_DIALOG(dialog),
			GTK_STOCK_OK,
			NULL);
	}
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
#endif

	return 0;
}

void
SubtractRect(RECT* dst, const RECT* r1, const RECT* r2)
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

void CopyRect(RECT *dest, RECT *src) {
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
void InflateRect(RECT *pr, int x, int y)
{
	if (!pr)
		return;

	pr->bottom += y;
	pr->left -= x;
	pr->right += x;
	pr->top -= y;
}
void OffsetRect(RECT *pr, int x, int y)
{
	if (!pr)
		return;

	pr->bottom += y;
	pr->left += x;
	pr->right += x;
	pr->top += y;
}
BOOL PtInRect(RECT* pr, struct POINT m)
{
	if (!pr)
		return FALSE;

	return pr->left <= m.x && m.x < pr->right && pr->top <= m.y && m.y < pr->bottom;
}

void SetRect(RECT* R, int left, int top, int right, int bottom)
{
	R->left = left;
	R->right = right;
	R->top = top;
	R->bottom = bottom;
}
#ifndef stricmp
int stricmp(const char* s1, const char* s2)
{
	return strcasecmp(s1, s2);
}
#endif
#ifndef strnicmp
int strnicmp(const char *str1, const char *str2, size_t n)
{
	return strncasecmp(str1, str2, n);
}
#endif

int _stricoll(const char *str1, const char *str2)
{
	char *sl1 = NULL;
	char *sl2 = NULL;

	if (str1) {
		size_t s1 = strxfrm(sl1, str1, 0);
		s1++;
		sl1 = (char *) malloc(s1);
		if (!sl1)
			return -1;
		strxfrm(sl1, str1, --s1);
		sl1[s1] = '\0';
	}
	if (str2) {
		size_t s2 = strxfrm(sl2, str2, 0);
		s2++;
		sl2 = (char *) malloc(s2);
		if (!sl2) {
			if (sl1)
				free(sl1);
			return 1;
		}
		strxfrm(sl2, str2, --s2);
		sl2[s2] = '\0';
	}

	int ret = strcasecmp(sl1, sl2);

	if (sl1)
		free(sl1);
	if (sl2)
		free(sl2);

	return ret;
}

#endif // !WIN32
