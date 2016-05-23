/*
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifdef _WIN32
#include <windows.h>
#endif

#include "mywcs.h"

#if defined(SPARC) || defined(__POWERPC__)
#define mywcs_SWAB(v) (v)
#else
#define mywcs_SWAB(v) ((unsigned short)((v) << 8) | (unsigned char)((v) >> 8))
#endif

/*--------------------------------------------------------------------------
 Convert up to a maximum of maxlen characters from a wchar_t string wcs
 to a 2 byte network byte order unicode string ncs.
 Zero fills to maxlen if 'from' is null terminated.
 Returns the length copied.
--------------------------------------------------------------------------*/
int mywcs_wchar2netchar(dp_netchar_t *ncs, const wchar_t *wcs, const int maxlen)
{
	int i;
	int len;
	for (len = 0; wcs[len] != 0 && len < maxlen; len++)
		ncs[len] = mywcs_SWAB((unsigned short)wcs[len]);
	for (i = len; i < maxlen; i++)
		ncs[i] = 0;
	return len;
}

/*--------------------------------------------------------------------------
 Convert up to a maximum of maxlen characters from a 2 byte network byte
 order unicode string ncs to a wchar_t string wcs.
 Zero fills to maxlen if 'from' is null terminated.
 Returns the length copied.
--------------------------------------------------------------------------*/
int mywcs_netchar2wchar(wchar_t *wcs, const dp_netchar_t *ncs, const int maxlen)
{
	int i;
	int len;
	for (len = 0; ncs[len] != 0 && len < maxlen; len++)
		wcs[len] = (wchar_t)mywcs_SWAB(ncs[len]);
	for (i = len; i < maxlen; i++)
		wcs[i] = 0;
	return len;
}

/*--------------------------------------------------------------------------
  Return the length of a null terminated UCS2 string (of either byte order),
  up to length maxlen.
--------------------------------------------------------------------------*/
int mywcs_lenn(const short *ucs2, const int maxlen)
{
	int i;
	for (i = 0; ucs2[i] != 0 && i < maxlen; i++);
	return i;
}

/*--------------------------------------------------------------------------
  Copy up to a maximum of maxlen characters from 'from' to 'to',
  retaining the byte order of 'from'.
  Zero fills to maxlen if 'from' is null terminated.
  Returns the length copied (i.e. mywcs_lenn(from, maxlen))
--------------------------------------------------------------------------*/
int mywcs_ncpy0(short *to, const short *from, const int maxlen)
{
	int i;
	int len;
	for (len = 0; from[len] != 0 && len < maxlen; len++)
		to[len] = from[len];
	for (i = len; i < maxlen; i++)
		to[i] = 0;
	return len;
}

/*--------------------------------------------------------------------------
  Copy up to a maximum of maxlen characters from 'from' to 'to',
  swapping byte order from network to native or vice versa.
  Zero fills to maxlen if 'from' is null terminated.
  Returns the length copied (i.e. mywcs_lenn(from, maxlen))
--------------------------------------------------------------------------*/
int mywcs_swabncpy0(short *to, const short *from, const int maxlen)
{
	int i;
	int len;
	for (len = 0; from[len] != 0 && len < maxlen; len++)
		to[len] = mywcs_SWAB(from[len]);
	for (i = len; i < maxlen; i++)
		to[i] = 0;
	return len;
}

/*--------------------------------------------------------------------------
 Convert a string from the local character set into a native byte order
 Unicode string.
 The input string is null terminated.
 If there is room, the wide-character output string is null-terminated.

 Parameters

 wcbuf    The address of a buffer to put the resulting wide characters
 wsbuflen The number of wchar_t's that will fit in wcbuf
 mbstr    The address of a sequence of multibyte characters

 Return Value

 If mywcs_frommbs successfully converts the source string, it returns the
 number of OUTPUT characters used, not including the terminating NUL.
 If mywcs_frommbs encounters an invalid multibyte character, it returns -1.

 Note: this is different in several ways from the standard C function
 mbstowcs().
--------------------------------------------------------------------------*/
int mywcs_frommbs(wchar_t *wcbuf, int wcbuflen, const char *mbstr)
{
	int wlen;

	if (!wcbuf || !wcbuflen || !mbstr || ((char *)wcbuf == mbstr))
		return -1;

#ifdef _WIN32

	wlen = MultiByteToWideChar(
		CP_ACP,         // CodePage (Ansi)
		0,              // dwFlags
		mbstr,          // lpMultiByteStr
		-1,             // cchMultiByte
		wcbuf,         // lpWideCharStr
		wcbuflen); // cchWideChar

	/* Distinguish between empty input string and error */
	if (wlen == 0 && *mbstr)
		wlen = -1;
	/* Don't count NUL char at end */
	if ((wlen > 1) && !wcbuf[wlen-1])
		wlen--;
#else

	/* Only support 7-bit ASCII; zero fill the upper 9 bits. */
	for (wlen=0; mbstr[wlen] && (wlen < wcbuflen); wlen++) {
		if ((unsigned)mbstr[wlen] > 0x7f) {
			wlen = -1;
			break;
		}
		wcbuf[wlen] = (wchar_t) mbstr[wlen];
	}
	if ((wlen > -1) && (wlen < wcbuflen))
		wcbuf[wlen] = 0;

#endif
	return wlen;
}

/*--------------------------------------------------------------------------
 Convert a native byte order Unicode string into the local character set.
 The input string is null terminated.
 If there is room, the output string is null-terminated.

 Parameters

 buf    The address of a buffer to put the resulting characters
 buflen The number of char's that will fit in buf
 wstr   The address of a sequence of wide characters

 Return Value

 If mywcs_tombs successfully converts the source string, it returns the number
 of OUTPUT bytes used, not including the terminating NUL.
 If mywcs_tombs encounters an invalid Unicode character, or one that
 could not be converted, it returns -1.

 Note: this is different in several ways from the standard C function
 wcstombs().
--------------------------------------------------------------------------*/
int mywcs_tombs(char *buf, int buflen, const wchar_t *wstr)
{
	int bytes;

	if (!buf || !buflen || !wstr || (buf == (char *)wstr))
		return -1;

#ifdef _WIN32

	bytes = WideCharToMultiByte(
		CP_ACP,
		0,
		wstr,
		-1,
		buf,
		buflen,
		NULL,
		NULL);

	/* Distinguish between empty input string and error */
	if (bytes == 0 && *wstr)
		bytes = -1;
	/* Don't count NUL char at end */
	if ((bytes > 1) && !buf[bytes-1])
		bytes--;
#else

	/* Only support 7-bit ASCII. */
	for (bytes=0; wstr[bytes] && (bytes < buflen); bytes++) {
		if ((unsigned long)wstr[bytes] > 0x7f) {
			bytes = -1;
			break;
		}
		buf[bytes] = (char) wstr[bytes];
	}
	if ((bytes > -1) && (bytes < buflen))
		buf[bytes] = 0;

#endif

	return bytes;
}

/*--------------------------------------------------------------------------
 Compare at most n bytes of s1 and s2.
 Return difference of the first nonidentical character, or zero.
--------------------------------------------------------------------------*/
int mywcs_ncmp(const short *s1, const short *s2, size_t n)
{
	if (n == 0)
		return (0);
	do {
		if (*s1 != *s2++)
			return (s1[0] - s2[-1]);
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	return (0);
}
