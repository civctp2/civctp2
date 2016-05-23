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
#include <assert.h>
#include <stdlib.h>
#include "serio.h"

#if 0					/* Change this to #if 1 to send DPRINT's to log file */
#define _DEBUG
#include <time.h>
#define DEBUG_MODULE 1
#include "ddprint.h"
#elif 0					/* Change this to #elif 1 to send DPRINT's to stdout */
#include <time.h>
#include <stdio.h>
#define DPRINT(s) printf s
#else
#define DPRINT(s)
#endif

/* Undefine this to turn on simple but blocking strawman version of code. */
#define USE_OVERLAP

/*-------------------------------------------------------------------------
 Sanity checks
-------------------------------------------------------------------------*/
#define serio_q_ASSERT(q) \
	assert((q).head >= 0 && (q).head < serio_BUFSIZE); \
	assert((q).tail >= 0 && (q).tail < serio_BUFSIZE); \
	assert((q).pending == FALSE || (q).pending == TRUE);

#define serio_ASSERT(serio) \
	assert(serio); \
	serio_q_ASSERT((serio)->rx); \
	serio_q_ASSERT((serio)->tx); \

/*-------------------------------------------------------------------------
 Given a Win32 comm handle already initialized, build a serio_t around it.
 Call with a pointer to an uninitizlized serio_t.
 If you call this, don't call serio_open().
-------------------------------------------------------------------------*/
serio_res_t serio_open_handle(serio_t *serio, HANDLE h)
{
	memset(serio, 0, sizeof(*serio));

	serio->port = h;

	/* must create event or WriteFile will block under
	 * Windows 95
	 */
	serio->tx.overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (serio->tx.overlap.hEvent == NULL) {
		DPRINT(("serio_open: CreateEvent failed\n"));
		return serio_RES_BUG;
	}
	serio->rx.overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (serio->rx.overlap.hEvent == NULL) {
		DPRINT(("serio_open: CreateEvent failed\n"));
		return serio_RES_BUG;
	}

	serio_ASSERT(serio);
	return serio_RES_OK;
}

/*-------------------------------------------------------------------------
 Return the Win32 handle used to access the comm port.
-------------------------------------------------------------------------*/
HANDLE serio_get_handle(serio_t *serio)
{
	return serio->port;
}

/*-------------------------------------------------------------------------
 Open a comm port and get ready for I/O
 Call with a pointer to an uninitizlized serio_t.
 If you call this, don't call serio_open_handle().
-------------------------------------------------------------------------*/
serio_res_t serio_open(serio_t *serio, long baud, const char *portname)
{
	COMMTIMEOUTS	CommTimeOuts ;
	DCB				dcb ;
	SECURITY_ATTRIBUTES SecurityAttributes;
	HANDLE h;

	/*--- Set up Win32 stuff ---*/

	if		(baud <= CBR_9600)	baud = CBR_9600;
	else if (baud <= CBR_19200)	baud = CBR_19200;
	else if (baud <= CBR_38400)	baud = CBR_38400;
	else						baud = CBR_57600;

	/* Let child processes inherit this handle. */
	memset(&SecurityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
	SecurityAttributes.nLength =				sizeof( SECURITY_ATTRIBUTES );
	SecurityAttributes.lpSecurityDescriptor =	NULL;
	SecurityAttributes.bInheritHandle =			TRUE;

	h = CreateFile(portname, GENERIC_READ | GENERIC_WRITE,
		0,                    // exclusive access
		&SecurityAttributes,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL
#ifdef USE_OVERLAP
		| FILE_FLAG_OVERLAPPED // overlapped I/O
#endif
		, NULL);

	if (INVALID_HANDLE_VALUE == h) {
		DPRINT(("serio_open: CreateFile(%s...) failed\n", portname));
		return serio_RES_BAD;
	}

	// Set the size of the input and output buffer.
	if (!SetupComm( h, 4096, 0 )) {
		DPRINT(("serio_open: SetupComm failed\n"));
		return serio_RES_BUG;
	}

	// purge any information in the buffer
	if (!PurgeComm( h, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)) {
		DPRINT(("serio_open: PurgeComm failed\n"));
		return serio_RES_BUG;
	}

	// set the time-out parameters for all read and write operations
#if 1
	// Cause ReadFile to never wait.
	// Works OK in Win95.
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 0 ;
#elif 0
	// Cause ReadFile to wait 50 milliseconds, then time out unconditionally.
	// Fails sometimes in Win95; the overlapped read *never* completes.
	CommTimeOuts.ReadIntervalTimeout = 0 ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 50 ;
#elif 0
	// Cause ReadFile to wait 100 milliseconds for traffic to start, but
	// wait only 10 milliseconds for traffic to resume if it goes silent
	// after starting.
	// Fails sometimes in Win95; the overlapped read *never* completes.
	CommTimeOuts.ReadIntervalTimeout = 10 ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 100 ;
#endif
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 1000 ;	/* will writes timeout? */

	if (!SetCommTimeouts( h, &CommTimeOuts )) {
		DPRINT(("serio_open: SetCommTimeouts failed\n"));
		return serio_RES_BUG;
	}

	dcb.DCBlength = sizeof( DCB ) ;

	if (!GetCommState( h, &dcb)) {
		DPRINT(("serio_open: GetCommState failed\n"));
		return serio_RES_BUG;
	}

	dcb.BaudRate = baud;
	dcb.Parity = FALSE ;
	dcb.fBinary = TRUE ;
	dcb.fOutxCtsFlow = FALSE ;
	dcb.fOutxDsrFlow = FALSE ;
	dcb.Parity = NOPARITY ;
	dcb.ByteSize = 8 ;
	dcb.StopBits = ONESTOPBIT ;

	if (!SetCommState( h, &dcb)) {
		DPRINT(("serio_open: SetCommState failed\n"));
		return serio_RES_BUG;
	}

	return serio_open_handle(serio, h);
}

/*-------------------------------------------------------------------------
 Close a comm port
-------------------------------------------------------------------------*/
serio_res_t serio_close(serio_t *serio)
{
	serio_ASSERT(serio);
	if (!serio)
		return serio_RES_BAD;

	CloseHandle(serio->port);
	CloseHandle(serio->tx.overlap.hEvent);
	CloseHandle(serio->rx.overlap.hEvent);
	return serio_RES_OK;
}

#ifdef USE_OVERLAP
/*-------------------------------------------------------------------------
 Handle any system calls that need to be made,
 or check up on any that are already in progress.
-------------------------------------------------------------------------*/
serio_res_t serio_poll(serio_t *serio)
{
	size_t cbTransfer;
	int len;
	int osErr;

	serio_ASSERT(serio);
	DPRINT(("%d serio_poll: entry: tx.Internal %d pending %d\n", clock(), serio->tx.overlap.Internal, serio->tx.pending));
	/* If no write is in progress, clean up and start a new one. */
	/* If a write has completed, update the head. */
	if (serio->tx.pending) {
		if (GetOverlappedResult(serio->port, &serio->tx.overlap, &cbTransfer, FALSE)) {
			int old_head = serio->tx.head;
			serio->tx.pending = FALSE;
			serio->tx.head += cbTransfer;
			serio->tx.head = serio_WRAP(serio->tx.head);
			DPRINT(("%d serio_poll: GetOverlappedResult ok: old_head %d, len %d; new head %d\n", clock(), old_head, cbTransfer, serio->tx.head));
		} else {
			osErr = GetLastError();
			if (osErr != ERROR_IO_INCOMPLETE) {
				DPRINT(("%d serio_poll: GetOverlappedResult failed, osErr %d\n", clock(), osErr));
				serio->tx.pending = FALSE;
				return serio_RES_BUG;
			}
		}
	}

	if (!serio->tx.pending) {
		DPRINT(("%d serio_poll: tx completed; head %d =? tail %d\n", clock(), serio->tx.head, serio->tx.tail));
		/* check to see if there are bytes we can write */
		if (serio->tx.head != serio->tx.tail) {
			/* Calculate number of bytes to write; don't wrap. */
			len = serio->tx.tail - serio->tx.head;
			if (len < 0)
				len = serio_BUFSIZE - serio->tx.head;
			assert(len > 0);
			DPRINT(("%d serio_poll: WriteFile(%d bytes)\n", clock(), len));
			if (WriteFile(serio->port, serio->tx.buf+serio->tx.head, len, &cbTransfer, &serio->tx.overlap)) {
				int old_head = serio->tx.head;
				serio->tx.head += len;
				serio->tx.head = serio_WRAP(serio->tx.head);
				DPRINT(("%d serio_poll: WriteFile completed immediately?! old_head %d, len %d; new head %d\n", clock(), old_head, len, serio->tx.head));
				return serio_RES_OK;
			} else {
				osErr = GetLastError();
				if (osErr != ERROR_IO_PENDING) {
					DPRINT(("%d serio_poll: WriteFile failed, osErr %d\n", clock(), osErr));
					return serio_RES_BUG;
				} else {
					DPRINT(("%d serio_poll: WriteFile pending; setting tx.pending.\n", clock()));
					serio->tx.pending = TRUE;
				}
			}
		}
	}
	/* read */
	serio_ASSERT(serio);
	DPRINT(("%d serio_poll: rx: entry: rx.Internal %d pending %d head %d tail %d\n", clock(), serio->rx.overlap.Internal, serio->rx.pending, serio->rx.head, serio->rx.tail));
	/* If a read has completed, update the tail. */
	if (serio->rx.pending) {
		if (GetOverlappedResult(serio->port, &serio->rx.overlap, &cbTransfer, FALSE)) {
			int old_tail = serio->rx.tail;
			serio->rx.pending = FALSE;
			serio->rx.tail += cbTransfer;
			serio->rx.tail = serio_WRAP(serio->rx.tail);
			DPRINT(("%d serio_poll: rx: GetOverlappedResult ok: old_tail %d, len %d; new tail %d\n", clock(), old_tail, cbTransfer, serio->rx.tail));
		} else {
			osErr = GetLastError();
			if (osErr != ERROR_IO_INCOMPLETE) {
				DPRINT(("%d serio_poll: rx: GetOverlappedResult failed, osErr %d\n", clock(), osErr));
				serio->rx.pending = FALSE;
				return serio_RES_BUG;
			}
		}
	}

	/* If no read is in progress, and there's room in the queue, start a read.
	 */
	if (!serio->rx.pending) {
		/* Calculate free space from tail on without wrapping */
		if (serio->rx.tail < serio->rx.head)
			len = serio->rx.head - serio->rx.tail - 1;
		else if (serio->rx.tail)
			len = serio_BUFSIZE - serio->rx.tail;
		else
			len = serio_BUFSIZE - 1;
		DPRINT(("%d serio_poll: rx: rx completed; free_bytes without wrapping %d >? 0\n", clock(), len));
		/* check to see if there is any space in the queue to read into */
		if (len > 0) {
			DPRINT(("%d serio_poll: rx: ReadFile(%d bytes)\n", clock(), len));
			if (ReadFile(serio->port, serio->rx.buf+serio->rx.tail, len, &cbTransfer, &serio->rx.overlap)) {
				int old_tail = serio->rx.tail;
				serio->rx.tail += cbTransfer;
				serio->rx.tail = serio_WRAP(serio->rx.tail);
				DPRINT(("%d serio_poll: rx: ReadFile completed immediately?! old_tail %d, len %d; new tail %d\n", clock(), old_tail, len, serio->rx.tail));
				return serio_RES_OK;
			} else {
				osErr = GetLastError();
				if (osErr != ERROR_IO_PENDING) {
					DPRINT(("%d serio_poll: rx: ReadFile failed, osErr %d\n", clock(), osErr));
					return serio_RES_BUG;
				} else {
					DPRINT(("%d serio_poll: rx: ReadFile pending; setting rx.pending.\n", clock()));
					serio->rx.pending = TRUE;
				}
			}
		}
	}
	DPRINT(("%d serio_poll: rx: returning OK.\n", clock()));
	serio_ASSERT(serio);
	return serio_RES_OK;
}
#endif

/*-------------------------------------------------------------------------
 Write the given number of bytes to the comm port.
-------------------------------------------------------------------------*/
serio_res_t serio_write(serio_t *serio, void *buf, size_t len)
{
#ifndef USE_OVERLAP
	DWORD dwBytesWritten;
	serio_ASSERT(serio);

	if (!serio)
		return serio_RES_BAD;

	if (!WriteFile(serio->port, buf, len, &dwBytesWritten, NULL) || (dwBytesWritten != len)) {
		return serio_RES_FULL;
	}
	return serio_RES_OK;
#else
	size_t free_bytes;
	size_t n_copy;
	int old_tail;

	serio_ASSERT(serio);

	if (!serio)
		return serio_RES_BAD;

	/* make sure there is room */
	free_bytes = serio_WRAP(serio_BUFSIZE - serio->tx.tail + serio->tx.head - 1);
	DPRINT(("%d serio_write: entry: len %d >? free_bytes %d\n", clock(), len, free_bytes));
	if (len > free_bytes) {
		/* Give the system a chance to flush the buffer, then return FULL */
		serio_poll(serio);
		return serio_RES_FULL;
	}

	/* copy the request into the queue */
	/* handle the part before the wrap */
	n_copy = len;
	if (n_copy >= (size_t)(serio_BUFSIZE - serio->tx.tail))
		n_copy = serio_BUFSIZE - serio->tx.tail;
	memcpy(serio->tx.buf + serio->tx.tail, buf, n_copy);
	old_tail = serio->tx.tail;
	serio->tx.tail += n_copy;
	serio->tx.tail = serio_WRAP(serio->tx.tail);
	((int)buf) += n_copy;
	len -= n_copy;

	/* handle the part after the wrap */
	if (len > 0) {
		memcpy(serio->tx.buf + serio->tx.tail, buf, len);
		serio->tx.tail += len;
		serio->tx.tail = serio_WRAP(serio->tx.tail);
	}
	DPRINT(("%d serio_write: old_tail %d, len1 %d, len2 %d; new tail %d\n", clock(), old_tail, n_copy, len, serio->tx.tail));

	serio_ASSERT(serio);

	/* trigger the actual write */
	return serio_poll(serio);
#endif
}

/*-------------------------------------------------------------------------
 Read up to the given number of bytes from the comm port.
 Places number of bytes read into *n_received.
-------------------------------------------------------------------------*/
serio_res_t serio_read(serio_t *serio, void *buf, size_t len, size_t *n_received)
{
#ifndef USE_OVERLAP
	COMSTAT	ComStat;
	DWORD	dwErrorFlags;

	serio_ASSERT(serio);

	if (ClearCommError(serio->port, &dwErrorFlags, &ComStat)) {
		len = min(len, ComStat.cbInQue);
		/* DPRINT(("ser_get: dwReadLength is %d, empty %d, cbInQue %d\n", dwReadLength, len, ComStat.cbInQue)); */
		if (len > 0) {
			if (ReadFile(serio->port, buf, len, n_received, NULL)) {
				return serio_RES_OK;
			}
		}
	}

	serio_ASSERT(serio);
	return serio_RES_EMPTY;
#else
	size_t n_copy;
	int old_head;
	int n;

	serio_ASSERT(serio);

	if (!serio)
		return serio_RES_BAD;

	/* Give the system a chance to fill the buffer */
	serio_poll(serio);

	/* make sure there is something in the queue */
	if (serio->rx.head == serio->rx.tail) {
		return serio_RES_EMPTY;
	}

	/* copy the requested number of bytes from the queue */
	/* handle the part before the wrap */
	n_copy = len;
	n = serio_BUFSIZE - serio->rx.head;
	if (n_copy >= (size_t)n)
		n_copy = n;
	n = serio->rx.tail - serio->rx.head;
	if ((n > 0) && ((int)n_copy > n))
		n_copy = (size_t)n;
	memcpy(buf, serio->rx.buf + serio->rx.head, n_copy);
	old_head = serio->rx.head;
	serio->rx.head += n_copy;
	serio->rx.head = serio_WRAP(serio->rx.head);
	((int)buf) += n_copy;
	len -= n_copy;
	*n_received = n_copy;

	/* handle the part after the wrap */
	if ((len > 0) && (serio->rx.tail < serio->rx.head)) {
		assert(serio->rx.tail < serio->rx.head);
		if (len > (size_t)serio->rx.tail)
			len = (size_t)serio->rx.tail;
		memcpy(buf, serio->rx.buf + serio->rx.head, len);
		serio->rx.head += len;
		*n_received += len;
	}
	DPRINT(("%d serio_read: old_head %d, len1 %d, *n_received %d; new head %d\n", clock(), old_head, n_copy, *n_received, serio->rx.head));

	serio_ASSERT(serio);

	/* trigger the actual read */
	return serio_poll(serio);
#endif
}

/*-------------------------------------------------------------------------
 Purge input buffer.  Any characters received by the serial port are
 thrown away.
-------------------------------------------------------------------------*/
serio_res_t serio_purge_read(serio_t *serio)
{
	PurgeComm(serio->port, PURGE_RXABORT | PURGE_RXCLEAR);
#ifdef USE_OVERLAP
	serio_poll(serio);
	serio->rx.head = serio->rx.tail;
#endif
	return serio_RES_OK;
}

/*-------------------------------------------------------------------------
 Purge output buffer.  Any characters not yet sent by the serial port are
 thrown away.
-------------------------------------------------------------------------*/
serio_res_t serio_purge_write(serio_t *serio)
{
	PurgeComm(serio->port, PURGE_TXABORT | PURGE_TXCLEAR);
#ifdef USE_OVERLAP
	serio_poll(serio);
	serio->tx.tail = serio->tx.head;
#endif
	return serio_RES_OK;
}
