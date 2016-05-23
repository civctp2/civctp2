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

/*--------------------------------------------------------------------------
  Extra functions for handling wide character strings.
  Warning: this is a hodge podge of routines handling all of the various
  sorts of wide character strings - 2 byte network and native byte order
  unicode, and native byte order wchar_t unicode.
--------------------------------------------------------------------------*/

#ifndef MYWCS_H
#define MYWCS_H

#include <stdlib.h>
#include "aneterr.h"

/*--------------------------------------------------------------------------
 Convert up to a maximum of maxlen characters from a wchar_t string wcs
 to a 2 byte network byte order unicode string ncs.
 Zero fills to maxlen if 'from' is null terminated.
 Returns the length copied.
--------------------------------------------------------------------------*/
int mywcs_wchar2netchar(dp_netchar_t *ncs, const wchar_t *wcs, const int maxlen);

/*--------------------------------------------------------------------------
 Convert up to a maximum of maxlen characters from a 2 byte network byte
 order unicode string ncs to a wchar_t string wcs.
 Zero fills to maxlen if 'from' is null terminated.
 Returns the length copied.
--------------------------------------------------------------------------*/
int mywcs_netchar2wchar(wchar_t *wcs, const dp_netchar_t *ncs, const int maxlen);

/*--------------------------------------------------------------------------
  Return the length of a null terminated UCS2 string (of either byte order),
  up to length maxlen.
--------------------------------------------------------------------------*/
int mywcs_lenn(const short *ucs2, const int maxlen);

/*--------------------------------------------------------------------------
  Copy up to a maximum of maxlen characters from 'from' to 'to',
  retaining the byte order of 'from'.
  Zero fills to maxlen if 'from' is null terminated.
  Returns the length copied (i.e. mywcs_lenn(from, maxlen))
--------------------------------------------------------------------------*/
int mywcs_ncpy0(short *to, const short *from, const int maxlen);

/*--------------------------------------------------------------------------
  Copy up to a maximum of maxlen characters from 'from' to 'to',
  swapping byte order from network to native or vice versa.
  Zero fills to maxlen if 'from' is null terminated.
  Returns the length copied (i.e. mywcs_lenn(from, maxlen))
--------------------------------------------------------------------------*/
int mywcs_swabncpy0(short *to, const short *from, const int maxlen);

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
int mywcs_frommbs(wchar_t *wcbuf, int wcbuflen, const char *mbstr);

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
int mywcs_tombs(char *buf, int buflen, const wchar_t *wstr);

/*--------------------------------------------------------------------------
 Compare at most n bytes of s1 and s2.
 Return difference of the first nonidentical character, or zero.
--------------------------------------------------------------------------*/
int mywcs_ncmp(const short *s1, const short *s2, size_t n);

#endif
