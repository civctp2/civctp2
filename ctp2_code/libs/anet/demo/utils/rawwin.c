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
 Dummy rawwin.c for Windows/Visual C, which have good raw I/O routines.

 $Log: rawwin.c $
 Revision 1.1  1997/03/07 02:00:40  geychaner
 Initial revision
 Revision 1.1  1997/03/06 05:25:53  geychaner
 Initial revision
 * Revision 1.2  90/07/14  09:00:55  dan_kegel
 * Added raw_set_stdio(), which should make it easy to use RAW mode.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <conio.h>

void raw_set_stdio( int raw ) { (void) raw; }

/* Check for char on stdin.  Returns nonzero if one is waiting, 0 if not. */
int raw_kbhit(/* void */) { return kbhit(); }

/* Return -1 if no char available, otherwise return char. 
 * Return 3 if user hits the interrupt key (usually ^C, hence 3)
 */
int raw_getc(/* void */) { return getch(); }
