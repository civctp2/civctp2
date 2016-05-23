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

/*--- raw.c --------------------------------------------------------------
 MS-DOS routines to set and reset raw mode, enable or disable break checking,
 and check for characters on stdin.
 Written by Dan Kegel (dank@blacks.jpl.nasa.gov).  Enjoy.
 Modified by Stan Barber (sob@bcm.tmc.edu) for Microsoft C version 7.0
 26 Nov 1992, Dan Kegel: added raw_getc

 $Log: rawmsdos.c $
 Revision 1.2  1997/03/07 22:17:33  geychaner
 Changed to compile under Watcom C (regs.x.[bd]x to regs.w.[bd]x)
 Revision 1.1  1997/03/06 05:26:17  geychaner
 Initial revision
 * Revision 1.5  91/11/29  14:35:28  dan_kegel
 * Changed name of function to not clash with Turbo C fn of same name.
 * Turbo's getchar() still annoyingly ignores Enter when in RAW mode;
 * Turbo C users should use getch() rather than getchar().  Blech.
--------------------------------------------------------------------------*/
#include <stdio.h>
#include <fcntl.h>
#include "raw.h"

#include <dos.h>
#include <io.h>

/* If not using Microsoft C 7.0, try uncommenting the following line. */
/* # define NONC700 */

#ifdef NONC700
#define _REGS REGS
#endif

/* IOCTL GETBITS/SETBITS bits. */
#define DEVICE		0x80
#define RAW		0x20

/* IOCTL operations */
#define GETBITS		0
#define SETBITS		1
#define GETINSTATUS	6

/* DOS function numbers. */
#define BREAKCHECK	0x33
#define IOCTL		0x44

/* A nice way to call the DOS IOCTL function */
static int
dos_ioctl(int handle, int mode, unsigned setvalue)
{
	union _REGS regs;

	regs.h.ah = IOCTL;
	regs.h.al = (char) mode;
	regs.w.bx = handle;
	regs.h.dl = (char) setvalue;
	regs.h.dh = 0;			/* Zero out dh */
	intdos(&regs, &regs);
	return (regs.w.dx);
}

/*--------------------------------------------------------------------------
 Call this routine to determinte whether the device associated with
 the given file is in RAW mode.
 Returns FALSE if not in raw mode, TRUE if in raw mode.
 Example: old_raw = raw_get(fileno(stdin));
--------------------------------------------------------------------------*/
static int
raw_get(fd)
	int fd;
{
	return ( RAW == (RAW & dos_ioctl(fd, GETBITS, 0)));
}

/*--------------------------------------------------------------------------
 Call this routine to set or clear RAW mode for the device associated with
 the given file.
 Example: raw_set(fileno(stdout), TRUE);
--------------------------------------------------------------------------*/
static void
raw_set(fd, raw)
	int fd;
	int raw;
{
	int bits;
	bits = dos_ioctl(fd, GETBITS, 0);
	if (DEVICE & bits) {
		if (raw)
			bits |= RAW;
		else
			bits &= ~RAW;
		(void) dos_ioctl(fd, SETBITS, bits);
	}
}

/* A nice way to call the DOS BREAKCHECK function */
static int
breakctl(int mode, int setvalue)
{
	union _REGS regs;

	regs.h.ah = BREAKCHECK;
	regs.h.al = (char) mode;
	regs.h.dl = (char) setvalue;
	intdos(&regs, &regs);
	return (regs.w.dx & 0xff);
}

/*--------------------------------------------------------------------------
 Call this routine to determine whether DOS is checking for break (Control-C)
 before it executes any DOS function call.
 Return value is FALSE if it only checks before console I/O function calls,
 TRUE if it checks before any function call.
--------------------------------------------------------------------------*/
static int
break_get(void)
{
	return ( 0 != breakctl(GETBITS, 0));
}

/*--------------------------------------------------------------------------
 Call this routine with TRUE to tell DOS to check for break (Control-C)
 before it executes any DOS function call.
 Call this routine with FALSE to tell DOS to only check for break before
 it executes console I/O function calls.
--------------------------------------------------------------------------*/
void
static break_set(check)
	int check;
{
	(void) breakctl(SETBITS, check);
}

/*--------------------------------------------------------------------------
 One routine to set (or clear) raw mode on stdin and stdout,
 clear (or restore) break checking, and turn off input buffering on stdin.
 This is the most common configuration; under MS-DOS, since setting raw mode
 on stdout sometimes sets it on stdin, it's best to set it on both & be done
 with it.
--------------------------------------------------------------------------*/
void
raw_set_stdio(raw)
	int raw;	/* TRUE -> set raw mode; FALSE -> clear raw mode */
{
	static int was_break_checking = 0;

	raw_set(fileno(stdin), raw);
	raw_set(fileno(stdout), raw);
	if (raw) {
		setbuf(stdin, NULL);	/* so getchar() won't hang */
		setmode(fileno(stdin), O_BINARY);	/* so ^Z won't end */
		was_break_checking = break_get();
		break_set(0);
	} else {
		break_set(was_break_checking);
		setmode(fileno(stdin), O_TEXT);		/* so ^Z will be EOF */
	}
}

/*--------------------------------------------------------------------------
 If any input is ready on stdin, return a nonzero value.
 Else return zero.
 This works for both input files and devices.
 In RAW mode, if break checking is turned off, does not check for ^C.
 (The kbhit() that comes with Microsoft C seems to always check for ^C.)
----------------------------------------------------------------------------*/
int
raw_kbhit()
{
	union _REGS regs;

	regs.h.ah = IOCTL;
	regs.h.al = GETINSTATUS;
	regs.w.bx = fileno(stdin);
	intdos(&regs, &regs);
	return (0xff & regs.h.al);
}

/*--------------------------------------------------------------------------
 If any input is ready on stdin, get a char and return it.
 Else return -1.
 Does not check for ^C.
----------------------------------------------------------------------------*/
int
raw_getc()
{
	union _REGS regs;

	regs.h.ah = 0x6;
	regs.h.dl = 0xff;
	intdos(&regs, &regs);
#ifdef NONC700
	if (regs.x.flags & 0x40) return -1;
#else
	if (regs.x.cflag & 0x40) return -1;
#endif
	return (0xff & regs.h.al);
}
