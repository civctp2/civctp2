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

/*-------------------------------------------------------------------------
 Simple nonblocking serial I/O module for Win32.
 Win32's native comm functions don't provide buffering, and force you to
 watch an overlap structure, which gets kind of messy.
 This module take care of that for you.

 Dan Kegel
 Copyright Activision 1998
-------------------------------------------------------------------------*/

#ifndef serio_h
#define serio_h

#include <windows.h>

#define serio_BUFSIZE 4096		/*  Must be power of 2 for serio_WRAP! */

/* Macro to wrap a head or tail offset around the end of a serio_q_t's buf. */
#define serio_WRAP(x) ((x) & (serio_BUFSIZE - 1))

/* Private queue structure used for both sending and receiving. */
/* User code should neither declare nor peek at these. */
typedef struct {
	OVERLAPPED overlap;			/* Shared status from operating system */
	int pending;				/* TRUE after issuing read or write */
	char buf[serio_BUFSIZE];	/* empty if head == tail */
	int head;					/* offset of next byte to remove */
	int tail;					/* offset at which to add next byte */
} serio_q_t;

/* Private structure manipulated by the public serio_* methods below. */
/* User code should declare one of these, but not look inside it. */
typedef struct {
	HANDLE  port;				/* Result of CreateFile(portname, ...) */
	serio_q_t rx;				/* Input Queue */
	serio_q_t tx;				/* Output Queue */
} serio_t;

/* A result.  Lets you know if a call failed, and why. */
typedef int serio_res_t;	/* Error/success status type. */
#include "commerr.h"
#define serio_RES_OK		comm_STATUS_OK
#define serio_RES_FULL		comm_STATUS_FULL
#define serio_RES_EMPTY		comm_STATUS_EMPTY
#define serio_RES_BAD		comm_STATUS_BAD
#define serio_RES_BUG		comm_STATUS_BUG

/*-------------------------------------------------------------------------
 Open a comm port and get ready for I/O
 Call with a pointer to an uninitizlized serio_t.
 If you call this, don't call serio_open_handle().
-------------------------------------------------------------------------*/
serio_res_t serio_open(serio_t *serio, long baud, const char *portname);

/*-------------------------------------------------------------------------
 Given a Win32 comm handle already initialized, build a serio_t around it.
 Call with a pointer to an uninitizlized serio_t.
 If you call this, don't call serio_open().
-------------------------------------------------------------------------*/
serio_res_t serio_open_handle(serio_t *serio, HANDLE h);

/*-------------------------------------------------------------------------
 Return the Win32 handle used to access the comm port.
-------------------------------------------------------------------------*/
HANDLE serio_get_handle(serio_t *serio);

/*-------------------------------------------------------------------------
 Close a comm port
-------------------------------------------------------------------------*/
serio_res_t serio_close(serio_t *serio);

/*-------------------------------------------------------------------------
 Write the given number of bytes to the comm port.
-------------------------------------------------------------------------*/
serio_res_t serio_write(serio_t *serio, void *buf, size_t len);

/*-------------------------------------------------------------------------
 Read up to the given number of bytes from the comm port.
 Places number of bytes read into *n_received.
-------------------------------------------------------------------------*/
serio_res_t serio_read(serio_t *serio, void *buf, size_t len, size_t *n_received);

/*-------------------------------------------------------------------------
 Handle any system calls that need to be made,
 or check up on any that are already in progress.
-------------------------------------------------------------------------*/
serio_res_t serio_poll(serio_t *serio);

/*-------------------------------------------------------------------------
 Purge input buffer.  Any characters received by the serial port are
 thrown away.
-------------------------------------------------------------------------*/
serio_res_t serio_purge_read(serio_t *serio);

/*-------------------------------------------------------------------------
 Purge output buffer.  Any characters not yet sent by the serial port are
 thrown away.
-------------------------------------------------------------------------*/
serio_res_t serio_purge_write(serio_t *serio);

#endif
