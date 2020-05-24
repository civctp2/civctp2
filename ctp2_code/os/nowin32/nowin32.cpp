#include "c3.h"

#ifndef WIN32

#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <unistd.h>
#include <ctype.h>
#include <SDL2/SDL_timer.h>

#include "windows.h"

#ifdef USE_SDL
  #include "SDL2/SDL.h"
#endif

// @ToDo: _fullpath is the version of ptitSeb's branch, check whether there
// is anything usefull from RolandTaverner's branch
char* _fullpath(char* absolute, const char* relative, size_t bufsize)
{
	static char ret[MAX_PATH] = {0};
	char *dest = (absolute == NULL) ? ret : absolute;
	size_t size = (absolute == NULL) ? MAX_PATH : bufsize;

	if(!relative)
	{
		getcwd(dest, size - 1);
		dest[size - 1] = '\0';
	}
	else if(relative[0] == FILE_SEPC)
	{
		strncpy(dest, CI_FixName(relative), size - 1);
		dest[size - 1] = '\0';
	}
	else
	{
#ifdef __USE_GNU
		const char *fixedName = CI_FixName(relative);
		char *abs = canonicalize_file_name(fixedName);
		if(abs)
		{
			strncpy(dest, abs, size - 1);
			dest[size - 1] = '\0';
			free(abs);
		}
		else
		{
			// NOTE: canonicalize_file_name will return NULL if last element does not exist
			//   as this method should not return NULL in that case we test dirname on existence
			char *fixedNameDuplicate = strdup(fixedName);
			abs = canonicalize_file_name(dirname(fixedNameDuplicate));
			free(fixedNameDuplicate);
			if (abs)
			{
				strncpy(dest, abs, size - 1);
				dest[size - 1] = '\0';
				free(abs);

				// add basename
				int spaceLeft = size - strlen(dest);
				Assert(spaceLeft >= 0);
				strncat(dest, FILE_SEP, spaceLeft);
				fixedNameDuplicate = strdup(fixedName);
				strncat(dest, basename(fixedNameDuplicate), spaceLeft - strlen(FILE_SEP));
				free(fixedNameDuplicate);
			}
			else
			{
				return NULL;
			}
		}
#elif defined(BSD) || defined(__USE_BSD)
		char rlpath[PATH_MAX] = {0};
		char *abs = realpath(CI_FixName(relative), rlpath);
		if(abs)
		{
			if(!absolute)
			{
				return strdup(rlpath);
			}
			else
			{
				strncpy(dest, rlpath, size - 1);
				dest[size - 1] = '\0';
				return dest;
			}
		}
		else
		{
			return NULL;
		}
#else
#error "_fullpath() not implemented for this platform."
#endif
	}

	if(absolute)
		return dest;
	else
		return strdup(dest);
}

void _splitpath( const char *path,
                       char *drive,
                       char *dir,
                       char *fname,
                       char *ext 
               ) // http://msdn.microsoft.com/en-us/library/e737s6tf%28v=VS.100%29.aspx
{
  drive= NULL;
  char *dirc, *basc, *extc;
  dirc = strdup(path);
  basc = strdup(path);
  extc = strdup(path);
  if(dir)
    strcpy(dir, dirname(dirc)); // expecting pre-allocted array by caller
  if(fname)
    strcpy(fname, basename(basc)); // expecting pre-allocted array by caller
  const char *dot = strrchr(basename(extc), '.');
  if(ext){
    if(!dot || dot == fname)
      ext= "";
    else
      strcpy(ext, dot + 1); // expecting pre-allocted array by caller
}
  // fprintf(stderr, "%s L%d: %s %s %s %s!\n", __FILE__, __LINE__, drive, dir, fname, ext);
}

uint32 GetTickCount()
{
	return SDL_GetTicks();
}

sint32 MessageBox(HWND parent, const CHAR* msg, const CHAR* title, sint32 flags)
{
	fprintf(stderr, "Messagebox(%s): %s\n", (title ? title : "null"), (msg ? msg : "null"));
#ifdef USE_SDL
	SDL_MessageBoxData data;

	const SDL_MessageBoxColorScheme colorScheme = {
		{
			// background
			{ 0xDE, 0xD2, 0xB4 },
			// text
			{ 0x32, 0x31, 0x32 },
			// button border
			{ 0x02, 0x02, 0x02 },
			// button background
			{ 0xE6, 0xDa, 0xC5 },
			// button text
			{ 0x02, 0x02, 0x02 }
		}
	};

	const int MAXIMUM_NUMBER_OF_BUTTONS = 3;
	SDL_MessageBoxButtonData buttonData[MAXIMUM_NUMBER_OF_BUTTONS];
	memset(buttonData, 0, sizeof(SDL_MessageBoxButtonData)*MAXIMUM_NUMBER_OF_BUTTONS);
	// filter button
	switch (flags & 7) {
		case MB_OK:
		case MB_OKCANCEL:
			data.numbuttons = 1;
			buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			buttonData[0].buttonid = IDOK;
			buttonData[0].text = "OK";
			if (flags == MB_OKCANCEL) {
				data.numbuttons = 2;
				buttonData[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
				buttonData[1].buttonid = IDCANCEL;
				buttonData[1].text = "Cancel";
			}
			break;

		case MB_ABORTRETRYIGNORE:
			data.numbuttons = 3;
			buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			buttonData[0].buttonid = IDABORT;
			buttonData[0].text = "Abort";
			buttonData[1].flags = 0;
			buttonData[1].buttonid = IDRETRY;
			buttonData[1].text = "Retry";
			buttonData[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
			buttonData[2].buttonid = IDIGNORE;
			buttonData[2].text = "Ignore";
			break;

		case MB_YESNO:
		case MB_YESNOCANCEL:
			data.numbuttons = 2;
			buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			buttonData[0].buttonid = IDYES;
			buttonData[0].text = "Yes";
			buttonData[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
			buttonData[1].buttonid = IDNO;
			buttonData[1].text = "No";
			if (flags == MB_YESNOCANCEL) {
				data.numbuttons = 3;
				buttonData[1].flags = 0;
				buttonData[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
				buttonData[2].buttonid = IDCANCEL;
				buttonData[2].text = "Cancel";
			}
			break;

		case MB_RETRYCANCEL:
			data.numbuttons = 2;
			buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			buttonData[0].buttonid = IDRETRY;
			buttonData[0].text = "Retry";
			buttonData[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
			buttonData[1].buttonid = IDCANCEL;
			buttonData[1].text = "Cancel";
			break;

		case MB_CANCELTRYCONTINUE:
			data.numbuttons = 3;
			buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			buttonData[0].buttonid = IDCONTINUE;
			buttonData[0].text = "Continue";
			buttonData[1].flags = 0;
			buttonData[1].buttonid = IDTRYAGAIN;
			buttonData[1].text = "Try";
			buttonData[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
			buttonData[2].buttonid = IDCANCEL;
			buttonData[2].text = "Cancel";
			break;

		default:
			data.numbuttons = 1;
			buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			buttonData[0].buttonid = IDOK;
			buttonData[0].text = "OK";
			break;
	}

	data.buttons = buttonData;
	data.flags = SDL_MESSAGEBOX_ERROR;
	data.title = title ? title : "null";
	data.message = msg;
	data.colorScheme = &colorScheme;
	data.window = NULL;

	int selectedButton = IDFAIL;
	SDL_ShowMessageBox(&data, &selectedButton);
	if (selectedButton == -1) {
		selectedButton = buttonData[0].buttonid;
	}
	return selectedButton;
#else
	return IDFAIL;
#endif
}

void SubtractRect(RECT* dst, const RECT* r1, const RECT* r2)
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
	} else if (r1->top >= r2->top && r1->bottom > r2->bottom) {
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

char* strupr(char* str)
{
	char *buf = str;
	if (!buf)
	{
		return NULL;
	}

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
	{
		return;
	}

	pr->bottom += y;
	pr->left -= x;
	pr->right += x;
	pr->top -= y;
}
void OffsetRect(RECT *pr, int x, int y)
{
	if (!pr)
	{
		return;
	}

	pr->bottom += y;
	pr->left += x;
	pr->right += x;
	pr->top += y;
}
BOOL PtInRect(RECT* pr, struct POINT m)
{
	if (!pr) {
		return FALSE;
	}

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

	if(str1)
	{
		size_t s1 = strxfrm(sl1, str1, 0);
		s1++;
		sl1 = (char *)malloc(s1);
		if(!sl1)
			return -1;
		strxfrm(sl1, str1, --s1);
		sl1[s1] = '\0';
	}
	if(str2)
	{
		size_t s2 = strxfrm(sl2, str2, 0);
		s2++;
		sl2 = (char *)malloc(s2);
		if(!sl2)
		{
			if(sl1)
				free(sl1);
			return 1;
		}
		strxfrm(sl2, str2, --s2);
		sl2[s2] = '\0';
	}

	int ret = strcasecmp(sl1, sl2);

	if(sl1)
		free(sl1);
	if(sl2)
		free(sl2);

	return ret;
}
#endif // !WIN32
