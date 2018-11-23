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
 Module for receiving small binary blocks of data from multiple clients at
 the same time.
------------------------------------------------------------------------*/

#ifndef sbdserv_H
#define sbdserv_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "assoctab.h"
#include "eclock.h"
#include "sbd.h"

#define sbdserv_CONN_TIMEOUT	30	/* secs after connection to time out */

/* data associated with a client connection and its state */
typedef struct {
	/* private: */
	char buf[sbd_HEADERLEN + sbd_MAXLEN];
	int pos;		/* current index in buf */
	size_t len;		/* length of data expected or zero if not read yet */
	clock_t t_connect;	/* time this handle connected */
} sbdserv_conn_t;

/* Instance data for sbdserv module */
typedef struct {
	/* private: */
	int sockin;		/* socket on which to listen for connections */
	fd_set fds;		/* fd_set of all currently listening sockets */
	int sockmax;	/* maximum socket in fds */
	assoctab_t *conns;	/* sbdserv_conn_t's indexed by sockfd */
	clock_t t_last_poll;	/* eclock() of last poll */
	int cursock;	/* next sock to check in poll */
	int nsocks;		/* number of socks to check in poll */
} sbdserv_t;

/*------------------------------------------------------------------------
 Create an instance of the sbdserv module, listening on port for new
 connections.
------------------------------------------------------------------------*/
sbdserv_t *sbdserv_create(unsigned short port);

/*------------------------------------------------------------------------
 Destroy an instance of the sbdserv module.
------------------------------------------------------------------------*/
void sbdserv_destroy(sbdserv_t *sbdserv);

/*------------------------------------------------------------------------
 Gets the set of sockets that need reading into *rfds.
 Returns the maximum socket set.
------------------------------------------------------------------------*/
int sbdserv_getfds(sbdserv_t *sbdserv, fd_set *rfds);

/*------------------------------------------------------------------------
 Initialize sbdserv for a set of calls to sbdserv_poll.
 nsocks is the return value of select.

 Call once after each select and before calling sbdserv_poll.
------------------------------------------------------------------------*/
void sbdserv_startRead(sbdserv_t *sbdserv, int nsocks);

/*------------------------------------------------------------------------
 Handle any new connections, data, or handle state changes.
 rfds should be the same fd_set* that was passed to select as it's
 second parameter.

 After a select (and regardless of whether select times out), call this
 repeatedly until it returns no data or an error.

 Returns the length of data received if any, and returns the data in buf.
 		The caller should call sbdserv_poll again, since multiple handles
		might have data.
 Returns 0 if no data is available now.
 Returns -1 on error.
------------------------------------------------------------------------*/
int sbdserv_poll(sbdserv_t *sbdserv, fd_set *rfds, char *buf);

#endif
