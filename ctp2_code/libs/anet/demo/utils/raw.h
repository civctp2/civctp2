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
 Include file for users of raw.c.
 For details, see raw.c.
 26 Nov 1992, dan kegel: added raw_init(), raw_end(), and raw_getc().

 $Log: raw.h $
 Revision 1.3  1997/05/30 21:26:02  dkegel
 raw_set_stdio's argument is mandatory.
 Revision 1.2  1997/03/07 02:01:53  geychaner
 Some changes for Win32 and UNIX compatibility (and intercompiles)
 Revision 1.1  1997/03/06 05:25:53  geychaner
 Initial revision
 * Revision 1.2  90/07/14  09:00:55  dan_kegel
 * Added raw_set_stdio(), which should make it easy to use RAW mode.
--------------------------------------------------------------------------*/
#ifndef raw_h
#define raw_h

void raw_set_stdio(int raw);

/* Set stdin and stdout to raw mode */
#define raw_init() raw_set_stdio(1)

/* Set stdin and stdout to cooked mode */
#define raw_end() raw_set_stdio(0)

/* Check for char on stdin.  Returns nonzero if one is waiting, 0 if not. */
int raw_kbhit(/* void */);

/* Return -1 if no char available, otherwise return char. 
 * Return 3 if user hits the interrupt key (usually ^C, hence 3)
 */
int raw_getc(/* void */);

#ifdef UNIX
/* This allows us to use DOS-y putch on UNIX system, sort of */
#define putch(c) putchar(c)
#endif

#ifdef _WIN32
#include <conio.h>
#endif

#endif
