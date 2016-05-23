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
 Simple emulation of "conio" console io functions as commonly supported by
 compilers on Microsoft operating systems.

 Code from Ron Liechty of Metrowerks.

 $Log: rawmac.c $
 Revision 1.6  1997/07/27 01:29:28  dkegel
 Whoops, CW Pro's standard kbhit and getch don't work quite as I need; use mine.
 Revision 1.5  1997/07/26 20:05:09  dkegel
 Converted to CW Pro 1
 Revision 1.4  1997/06/02 01:32:08  dkegel
 Return 3 instead of exiting if cmd-. detected; this gives
 calling program a chance to unload network drivers
 (and the responsibility to check for this special return value).
 Revision 1.3  1997/05/31 22:48:12  dkegel
 Works better with gtest now.  Mouse events serviced even if no
 keystrokes detected.
 Revision 1.2  1997/05/30 21:26:12  dkegel
 raw_set_stdio's argument is mandatory.
 Revision 1.1  1997/05/30 17:37:52  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <Events.h>
#include <Sioux.h>

#include <stdio.h>
#include <stdlib.h>

int raw_kbhit(void)
{
	EventRecord event;

	// Allow SIOUX response for the mouse, drag, zoom, or resize.
	if (GetNextEvent(mDownMask|mUpMask,&event))
		SIOUXHandleOneEvent(&event);

	/* Check for keystroke. */
	return EventAvail(keyDownMask, &event);
}

/* Return -1 if no char available, otherwise return char.
 * Return 3 if user hits the interrupt key (usually ^C, hence 3)
 *
 * Bug: This version does not set unbuffered mode on stdout,
 * so be sure to fflush(stdout) early and often...
 */
int raw_getc(void)
{
	int c;
	EventRecord event;

	// Allow SIOUX response for the mouse, drag, zoom, or resize.
	if (GetNextEvent(mDownMask|mUpMask,&event))
		SIOUXHandleOneEvent(&event);

	/* Check for keystroke. */
	if (GetNextEvent(keyDownMask,&event)) {
		/* Get ASCII value of keystroke. */
		c=event.message&charCodeMask;
		if (c=='.' && (event.modifiers&cmdKey)) {
			return 3;
			//exit(1);
		}

		return c;
	}

	return -1;  /* No character avail */
}

void raw_set_stdio(int raw)
{
	(void) raw;
}
