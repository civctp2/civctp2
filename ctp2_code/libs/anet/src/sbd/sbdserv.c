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

/* #define VERBOSE */
/*------------------------------------------------------------------------
 Module for receiving small binary blocks of data from multiple clients at
 the same time.
------------------------------------------------------------------------*/

#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

#include "anet.h"
#include "sbdserv.h"
#include "portable.h"

#if defined(_DEBUG) || defined(DPRNT)
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
static const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 8; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}

static void sbdserv_assertValid(sbdserv_t *sbdserv)
{
	int h, n = 0;
	for (h = 0; h <= sbdserv->sockmax; h++)
		if (FD_ISSET(h, &sbdserv->fds))
			n++;
	assert(n == sbdserv->conns->n_used);
	assert(sbdserv->sockin != -1);
	assert((sbdserv->nsocks >= 0) && (sbdserv->nsocks <= n+1));
	assert((sbdserv->nsocks == 0) || ((sbdserv->cursock >= 0) && (sbdserv->cursock <= sbdserv->sockmax+1)));
}
#else
#define hexstring(str,len) "NDEBUG"
#define sbdserv_assertValid(s)
#endif

/*------------------------------------------------------------------------
 Create an instance of the sbdserv module, listening on port for new
 connections.
------------------------------------------------------------------------*/
sbdserv_t *sbdserv_create(unsigned short port)
{
	clock_t now = eclock();
	sbdserv_t *sbdserv;
	struct protoent *pe;
	struct sockaddr_in addr;
	int sockin;
	int flags;

	DPRINT(("sbdserv_create: creating my socket\n"));
	pe = getprotobyname("tcp");
	if (!pe) {
		DPRINT(("sbdserv_create: unknown protocol:TCP\n"));
		return NULL;
	}
	if ((sockin = socket(AF_INET, SOCK_STREAM, pe->p_proto)) == -1) {
		DPRINT(("sbdserv_create: socket error:%d\n", errno));
		return NULL;
	}
	flags = fcntl(sockin, F_GETFL, 0);
	if ((-1 == flags)
	||  (-1 == fcntl(sockin, F_SETFL, (flags | O_NONBLOCK)))) {
		DPRINT(("sbdserv_create: fcntl(sock:%d) error:%d\n", sockin, errno));
		return NULL;
	}

	DPRINT(("sbdserv_create: binding socket:%d\n", sockin));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(sockin, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		DPRINT(("sbdserv_create: bind error:%d\n", errno));
		return NULL;
	}

	DPRINT(("sbdserv_create: starting listen\n"));
	if (listen(sockin, 10) < 0) {
		DPRINT(("sbdserv_create: listen error:%d\n", errno));
		return NULL;
	}

	sbdserv = (sbdserv_t *)malloc(sizeof(sbdserv_t));
	if (!sbdserv)
		return NULL;
	sbdserv->conns = assoctab_create(sizeof(sbdserv_conn_t));
	if (!sbdserv->conns) {
		free(sbdserv);
		return NULL;
	}
	FD_ZERO(&sbdserv->fds);
	sbdserv->sockin = sockin;
	sbdserv->sockmax = sbdserv->sockin;
	sbdserv->cursock = 0;
	sbdserv->nsocks = 0;
	sbdserv->t_last_poll = now;
	return sbdserv;
}

/*------------------------------------------------------------------------
 Destroy an instance of the sbdserv module.
------------------------------------------------------------------------*/
void sbdserv_destroy(sbdserv_t *sbdserv)
{
	int i;

	if (!sbdserv)
		return;
	close(sbdserv->sockin);
	for (i = 0; i < sbdserv->conns->n_used; i++) {
		assoctab_item_t *pi = assoctab_getkey(sbdserv->conns, i);
		close(pi->key);
	}
	assoctab_destroy(sbdserv->conns);
	memset(sbdserv, 0, sizeof(sbdserv_t));
	free(sbdserv);
}

/*------------------------------------------------------------------------
 Add a handle to the sbdserv.
 Returns 0 on success,
 		 -1 on failure.
------------------------------------------------------------------------*/
static int sbdserv_handleOpened(sbdserv_t *sbdserv, int h)
{
	clock_t now = eclock();
	sbdserv_conn_t *pc;

	if (!sbdserv || (h == -1))
		return -1;
	pc = (sbdserv_conn_t *)assoctab_subscript(sbdserv->conns, h);
	if (pc) {
		DPRINT(("sbdserv_handleOpened: h:%d already open\n", h));
		return -1;
	}
	pc = (sbdserv_conn_t *)assoctab_subscript_grow(sbdserv->conns, h);
	if (!pc) {
		DPRINT(("sbdserv_handleOpened: can't grow table for h:%d\n", h));
		return -1;
	}
	memset(pc, 0, sizeof(sbdserv_conn_t));
	pc->pos = 0;
	pc->len = 0;
	FD_SET(h, &sbdserv->fds);
	pc->t_connect = now;
	if (h > sbdserv->sockmax)
		sbdserv->sockmax = h;
	sbdserv_assertValid(sbdserv);
	DPRINT(("sbdserv_handleOpened(h:%d)\n", h));
	return 0;
}

/*------------------------------------------------------------------------
 Read data from handle h.
 Handle must be set non-blocking and have data waiting.
 Returns 0 if data read successfully, but more is expected,
 		 -1 on error caller should close the handle and call handleClosed,
 On completion, the length read is returned and *bufp is set to the
 address of the buffer containing the data received from that handle.
------------------------------------------------------------------------*/
static int sbdserv_readData(sbdserv_t *sbdserv, int h, char **bufp)
{
	int nRecvd;
	int nToRecv;
	sbdserv_conn_t *pc;

	*bufp = NULL;
	if (!sbdserv || (h == -1))
		return -1;
	pc = (sbdserv_conn_t *)assoctab_subscript(sbdserv->conns, h);
	if (!pc) {
		DPRINT(("sbdserv_readData: no such handle h:%d\n", h));
		return -1;
	}
	nToRecv = sbd_HEADERLEN + sbd_MAXLEN - pc->pos;
	if (nToRecv <= 0) {
		DPRINT(("sbdserv_readData: bug: h:%d tried to read %dB (pos:%d)\n", h, nToRecv, pc->pos));
		assert(nToRecv > 0);
	}
	errno = 0;
	nRecvd = recv(h, pc->buf + pc->pos, nToRecv, 0);
#ifdef VERBOSE
	DPRINT(("sbdserv_readData: recv(h:%d, pos:%d, n:%d) returns %d (errorr:%d)\n", h, pc->pos, nToRecv, nRecvd, errno));
#endif
	if (nRecvd <= 0) {
		if ((nRecvd == 0) || (errno == ENOTCONN)) {
			DPRINT(("sbdserv_readData: h:%d closed connection\n", h));
			return -1;	/* error - connection closed from other end */
		} else if (errno == EWOULDBLOCK) {
			DPRINT(("sbdserv_readData: no data for h:%d\n", h));
			return 0;	/* error - selected, but no data. */
		} else if (errno == EINTR) {
			DPRINT(("sbdserv_readData: h:%d recv interupted by signal\n", h));
			return -1;
		}
		DPRINT(("sbdserv_readData: read error:%d on h:%d\n", errno, h));
		return -1;	/* error - read error */
	}
#ifdef VERBOSE
	DPRINT(("sbdserv_readData: recv'd %dB from h:%d:%s\n", nRecvd, h, hexstring(pc->buf + pc->pos, nRecvd)));
#endif
	pc->pos += nRecvd;
	if (!pc->len) {
		if (pc->pos < sbd_HEADERLEN)
			return 0;	/* still waiting for header bytes */
		if (memcmp(pc->buf, sbd_MAGIC, 2)) {
			DPRINT(("sbdserv_readData: magic doesn't match for h:%d\n", h));
#ifdef VERBOSE
			DPRINT(("sbdserv_readData: %s\n", hexstring(pc->buf, pc->pos)));
#endif
			return -1;	/* error - magic doesn't match */
		}
		pc->len = dpMAKESHORT(pc->buf[2], pc->buf[3]);
		if ((pc->len == 0) || (pc->len > sbd_MAXLEN)) {
			DPRINT(("sbdserv_readData: len:%d out of range for h:%d\n", pc->len, h));
			pc->len = 0;
			return -1;	/* error - len out of range */
		}
	}
	if (pc->pos > sbd_HEADERLEN + pc->len) {
		DPRINT(("sbdserv_readData: recd more than len:%d data from h:%d\n", pc->len, h));
		return -1;	/* error - received more than len data */
	} else if (pc->pos == sbd_HEADERLEN + pc->len) {
		*bufp = pc->buf + sbd_HEADERLEN;
		return pc->len;
	}
	return 0;	/* still waiting for data */
}

/*------------------------------------------------------------------------
 Clean up any data associated with the given handle.
------------------------------------------------------------------------*/
static void sbdserv_handleClosed(sbdserv_t *sbdserv, int h)
{
	if (!sbdserv || (h == -1)) {
		DPRINT(("sbdserv_handleClosed: bad params (h:%d)\n", h));
		return;
	}
	if (assoctab_subscript_delete(sbdserv->conns, h)) {
		DPRINT(("sbdserv_handleClosed: no such handle h:%d\n", h));
		return;
	}
	FD_CLR(h, &sbdserv->fds);
	if (h == sbdserv->sockmax)
		for (sbdserv->sockmax--; sbdserv->sockmax >= 0; sbdserv->sockmax--)
			if (FD_ISSET(sbdserv->sockmax, &sbdserv->fds)
			||  (sbdserv->sockmax == sbdserv->sockin))
				break;
	DPRINT(("sbdserv_handleClosed(h:%d)\n", h));
}

/*------------------------------------------------------------------------
 Gets the set of sockets that need reading into *rfds.
 Returns the maximum socket set.
------------------------------------------------------------------------*/
int sbdserv_getfds(sbdserv_t *sbdserv, fd_set *rfds)
{
	int sock;

	FD_SET(sbdserv->sockin, rfds);	/* always listen for new connections */
#ifdef VERBOSE
	DPRINT(("sbdserv_getfds: checking socks:%d", sbdserv->sockin));
#endif
	for (sock = 0; sock <= sbdserv->sockmax; sock++) {
		if (FD_ISSET(sock, &sbdserv->fds)) {
			FD_SET(sock, rfds);
#ifdef VERBOSE
			DPRINT((",%d", sock));
#endif
		}
	}
#ifdef VERBOSE
	DPRINT((" sockmax:%d total:%d\n", sbdserv->sockmax, (1 + sbdserv->conns->n_used)));
#endif
	return sbdserv->sockmax;
}

/*------------------------------------------------------------------------
 Initialize sbdserv for a set of calls to sbdserv_poll.
 nsocks is the return value of select.

 Call once after each select and before calling sbdserv_poll.
------------------------------------------------------------------------*/
void sbdserv_startRead(sbdserv_t *sbdserv, int nsocks)
{
	assert(nsocks >= 0);
	sbdserv->cursock = 0;	/* FIXME: should be min(socks) */
	sbdserv->nsocks = nsocks;
}

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
int sbdserv_poll(sbdserv_t *sbdserv, fd_set *rfds, char *buf)
{
	clock_t now;
	int sock;
	int i;

	if (!sbdserv || !sbdserv->conns || !buf)
		return -1;
	sbdserv_assertValid(sbdserv);
	if (FD_ISSET(sbdserv->sockin, rfds) && (sbdserv->nsocks > 0)) {
		int newsock;
		struct sockaddr_in client_addr;
		int len = sizeof(struct sockaddr_in);

		newsock = accept(sbdserv->sockin, (struct sockaddr *)&client_addr,&len);
		if (newsock == -1) {
			DPRINT(("sbdserv_poll: accept error:%d on sock:%d\n", errno, sbdserv->sockin));
		} else {
			int flags;

#ifdef VERBOSE
			DPRINT(("sbdserv_poll: accepting connection from %s on sock:%d\n", inet_ntoa(client_addr.sin_addr), newsock));
#endif
			flags = fcntl(newsock, F_GETFL, 0);
			if ((-1 == flags)
			||  (-1 == fcntl(newsock, F_SETFL, (flags | O_NONBLOCK)))) {
				DPRINT(("sbdserv_poll: fcntl error:%d on sock:%d\n", errno, newsock));
				close(newsock);
				return -1;
			} else if (-1 == sbdserv_handleOpened(sbdserv, newsock)) {
				DPRINT(("sbdserv_poll: can't add h:%d\n", newsock));
				close(newsock);
				return -1;
			}
		}
		sbdserv->nsocks--;
	}

	while ((sbdserv->cursock <= sbdserv->sockmax) && (sbdserv->nsocks > 0)) {
		if (FD_ISSET(sbdserv->cursock, rfds)
		&&  FD_ISSET(sbdserv->cursock, &sbdserv->fds)) {
			char *bufp = NULL;
			int len;

			len = sbdserv_readData(sbdserv, sbdserv->cursock, &bufp);
			if (len == -1) {
				DPRINT(("sbdserv_readData error\n"));
				sbdserv_handleClosed(sbdserv, sbdserv->cursock);
				close(sbdserv->cursock);
				/* silently fails */
			} else if (len > 0) {
#ifdef VERBOSE
				DPRINT(("sbdserv_poll: h:%d finished sending data\n", sbdserv->cursock));
#endif
				/* must return copy, since closing will destroy original */
				memcpy(buf, bufp, len);
				sbdserv_handleClosed(sbdserv, sbdserv->cursock);
				if (-1 == close(sbdserv->cursock)) {
					DPRINT(("sbdserv_poll: close error:%d\n", errno));
				}
				sbdserv->nsocks--;
				sbdserv->cursock++;
				return len;
			} else {
#ifdef VERBOSE
				DPRINT(("sbdserv_poll: h:%d waiting for more data\n", sbdserv->cursock));
#endif
			}
			sbdserv->nsocks--;
		}
		sbdserv->cursock++;
	}

	now = eclock();
	if ((long)(now - sbdserv->t_last_poll) < 10 * ECLOCKS_PER_SEC)
		return 0;	/* don't poll more than once every 10s */
	sbdserv->t_last_poll = now;
#ifdef VERBOSE
	DPRINT(("sbdserv_poll: t:%d checking %d conns for timeouts\n", now, sbdserv->conns->n_used));
#endif

	/* Iterate through handles, deleting those that have timed out,
	 * in reverse order, since deleting will change indices of later entries.
	 */
	for (i = sbdserv->conns->n_used - 1; i >= 0; i--) {
		assoctab_item_t *pi;
		sbdserv_conn_t *pc;
		int h;
 		pi = assoctab_getkey(sbdserv->conns, i);
		if (!pi) {
			DPRINT(("sbdserv_poll: assoctab_getkey(%d) returns NULL?\n", i));
			return -1;
		}
		h = pi->key;
		pc = (sbdserv_conn_t *)pi->value;
		if (!pc) {
			DPRINT(("sbdserv_poll: conns[%d] == NULL?\n", i));
			return -1;
		}
		if ((long)(now - pc->t_connect) > sbdserv_CONN_TIMEOUT * ECLOCKS_PER_SEC) {
			DPRINT(("sbdserv_poll: h:%d timed out\n", h));
			sbdserv_handleClosed(sbdserv, h);
			if (-1 == close(h)) {
				DPRINT(("sbdserv_poll: close(%d) error:%d\n", h, errno));
				return -1;
			}
		}
	}
	return 0;
}
