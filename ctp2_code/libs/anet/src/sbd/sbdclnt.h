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

/*------------------------------------------------------------------------
 Tiny module for sending small binary blocks of data to a server.
------------------------------------------------------------------------*/

#ifndef sbdclnt_H
#define sbdclnt_H

#ifdef UNIX
#define my_EINPROGRESS	EINPROGRESS
#define my_EWOULDBLOCK	EWOULDBLOCK
#define my_EMSGSIZE		EMSGSIZE
#define my_CLOSE(s)		close(s)
#else
#ifdef _WIN32
#include <winsock.h>
#define my_EINPROGRESS	WSAEINPROGRESS
#define my_EWOULDBLOCK	WSAEWOULDBLOCK
#define my_EMSGSIZE		WSAEMSGSIZE
#define my_CLOSE(s)		closesocket(s)
#else
/* not UNIX, not _WIN32, not supported */
#endif
#endif

#include "sbd.h"

typedef struct {
	/* public: (read only) */
	int sock;		/* TCP socket handle */

	/* private: */
	char buf[sbd_HEADERLEN + sbd_MAXLEN];	/* data to send */
	size_t len;		/* total length of data */
	size_t pos;		/* current position in buf */
} sbdclnt_t;

/*------------------------------------------------------------------------
 Create an instance of the sbdclnt module, and connect to hostname:port
 in preparation for sending the len bytes of binary data in buf.
 Returns NULL on failure.
------------------------------------------------------------------------*/
sbdclnt_t *sbdclnt_create(const char *buf, size_t len, const char *hostname, short port);

/*------------------------------------------------------------------------
 Destroy an instance of the sbdclnt module.
------------------------------------------------------------------------*/
void sbdclnt_destroy(sbdclnt_t *sbdclnt);

/*------------------------------------------------------------------------
 Continue sending the small binary block of data to a server using
 non-blocking I/O.
 Returns the number of bytes sent (not including the header) on success,
 		   when all data has been passed to the OS's TCP layer,
         0 if more calls are needed,
        -1 on error.
------------------------------------------------------------------------*/
int sbdclnt_poll(sbdclnt_t *sbdclnt);

#endif
