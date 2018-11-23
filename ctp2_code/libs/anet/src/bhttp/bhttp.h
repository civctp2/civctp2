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

#ifndef bhttp_H
#define bhttp_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "assoctab.h"
#include "eclock.h"

#define bhttp_CONN_TIMEOUT	30	/* secs after connection to time out */

#define bhttp_ISTATE_INITIAL 0
#define bhttp_ISTATE_CR 1
#define bhttp_ISTATE_LF 2
#define bhttp_ISTATE_CRLF 3
#define bhttp_ISTATE_CRLFCR 4
#define bhttp_ISTATE_END 5
#define bhttp_IBUF_LEN 1024
#define bhttp_OBUF_LEN 1024

/* data associated with a client connection and its state */
typedef struct {
	/* private: */
	/* Input buffer */
	char ibuf[bhttp_IBUF_LEN];
	int ipos;			/* current index in buf */
	size_t ilen;		/* length of data expected or zero if not read yet */
	int istate;			/* bhttp_ISTATE_* */
	/* Output buffer */
	char obuf[bhttp_OBUF_LEN];
	int opos;			/* current index in obuf */
	size_t olen;		/* Total length of data to send */
	/* Misc */
	clock_t t_connect;	/* time this handle connected */
	char *pmethod;
	char *purl;
	char *pprotocol;
} bhttp_conn_t;

/* Structure for callback to pass back the results of fetching a given URL */
typedef struct {
	int httpResultCode;
	char mimeType[64];
} bhttp_url2buf_result_t;

typedef int (*bhttp_url2buf_t)(char *context, const char *url, char *buf, int buflen, bhttp_url2buf_result_t *result);

/* Instance data for bhttp module */
typedef struct {
	/* private: */
	int sockin;				/* socket on which to listen for connections */
	fd_set rfds;			/* fd_set of all currently listening sockets */
	fd_set wfds;			/* fd_set of all currently writing sockets */
	int sockmax;			/* maximum socket in fds */
	assoctab_t *conns;		/* bhttp_conn_t's indexed by sockfd */
	clock_t t_last_poll;	/* eclock() of last poll */
	int cursock;			/* next sock to check in poll */
	int nsocks;				/* number of socks to check in poll */
	bhttp_url2buf_t url2buf_cb;	/* Get content for URL */
	void *url2buf_context;
} bhttp_t;

/*------------------------------------------------------------------------
 Create an instance of the bhttp module, listening on port for new
 connections.
------------------------------------------------------------------------*/
bhttp_t *bhttp_create(unsigned short port, bhttp_url2buf_t url2buf_cb, void *url2buf_context);

/*------------------------------------------------------------------------
 Destroy an instance of the bhttp module.
------------------------------------------------------------------------*/
void bhttp_destroy(bhttp_t *bhttp);

/*------------------------------------------------------------------------
 Gets the set of sockets that need reading into *rfds, and writing
 into *wfds.
 Returns the maximum socket set.
------------------------------------------------------------------------*/
int bhttp_getfds(bhttp_t *bhttp, fd_set *rfds, fd_set *wfds);

/*------------------------------------------------------------------------
 Handle any new connections, data, or handle state changes.
 rfds and wfds should be the same fd_set*'s that were passed to select
 as it's second and third parameters.
 nsocks is the return value of select.

 Call this once after each select, regardless of whether select times out.

 Returns 0 upon success.
 Returns -1 on error; detailed error message is printed to log file if
 this is a debug build.
------------------------------------------------------------------------*/
int bhttp_poll(bhttp_t *bhttp, fd_set *rfds, fd_set *wfds, int nsocks);

#endif
