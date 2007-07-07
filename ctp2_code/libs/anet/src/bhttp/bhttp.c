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
#include "bhttp.h"
#include "portable.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

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

static void bhttp_assertValid(bhttp_t *bhttp)
{
	int h, n = 0;
	for (h = 0; h <= bhttp->sockmax; h++)
		if (FD_ISSET(h, &bhttp->rfds) || FD_ISSET(h, &bhttp->wfds))
			n++;
	assert(n == bhttp->conns->n_used);
	assert(bhttp->sockin != -1);
	assert((bhttp->nsocks >= 0) && (bhttp->nsocks <= n+1));
	assert((bhttp->nsocks == 0) || ((bhttp->cursock >= 0) && (bhttp->cursock <= bhttp->sockmax+1)));
}
#else
#define hexstring(str,len) "NDEBUG"
#define bhttp_assertValid(s)
#endif

/*------------------------------------------------------------------------
 Create an instance of the bhttp module, listening on port for new
 connections.
------------------------------------------------------------------------*/
bhttp_t *bhttp_create(unsigned short port, bhttp_url2buf_t url2buf_cb, void *url2buf_context)
{
	clock_t now = eclock();
	bhttp_t *bhttp;
	struct protoent *pe;
	struct sockaddr_in addr;
	int sockin;
	int flags;

	if (!url2buf_cb)
		return NULL;
	DPRINT(("bhttp_create: creating my socket\n"));
	pe = getprotobyname("tcp");
	if (!pe) {
		DPRINT(("bhttp_create: unknown protocol:TCP\n"));
		return NULL;
	}
	if ((sockin = socket(AF_INET, SOCK_STREAM, pe->p_proto)) == -1) {
		DPRINT(("bhttp_create: socket error:%d\n", errno));
		return NULL;
	}
	flags = fcntl(sockin, F_GETFL, 0);
	if ((-1 == flags)
	||  (-1 == fcntl(sockin, F_SETFL, (flags | O_NONBLOCK)))) {
		DPRINT(("bhttp_create: fcntl(sock:%d) error:%d\n", sockin, errno));
		return NULL;
	}

	DPRINT(("bhttp_create: binding socket:%d\n", sockin));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = htons(INADDR_ANY); 
	if (bind(sockin, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		DPRINT(("bhttp_create: bind error:%d\n", errno));
		return NULL;
	}

	DPRINT(("bhttp_create: starting listen\n"));
	if (listen(sockin, 10) < 0) {
		DPRINT(("bhttp_create: listen error:%d\n", errno));
		return NULL;
	}

	bhttp = (bhttp_t *)malloc(sizeof(bhttp_t));
	if (!bhttp)
		return NULL;
	bhttp->conns = assoctab_create(sizeof(bhttp_conn_t));
	if (!bhttp->conns) {
		free(bhttp);
		return NULL;
	}
	FD_ZERO(&bhttp->rfds);
	FD_ZERO(&bhttp->wfds);
	bhttp->sockin = sockin;
	bhttp->sockmax = bhttp->sockin;
	bhttp->cursock = 0;
	bhttp->nsocks = 0;
	bhttp->t_last_poll = now;
	bhttp->url2buf_cb = url2buf_cb;
	bhttp->url2buf_context = url2buf_context;
	return bhttp;
}

/*------------------------------------------------------------------------
 Destroy an instance of the bhttp module.
------------------------------------------------------------------------*/
void bhttp_destroy(bhttp_t *bhttp)
{
	int i;

	if (!bhttp)
		return;
	close(bhttp->sockin);
	for (i = 0; i < bhttp->conns->n_used; i++) {
		assoctab_item_t *pi = assoctab_getkey(bhttp->conns, i);
		close(pi->key);
	}	
	assoctab_destroy(bhttp->conns);
	memset(bhttp, 0, sizeof(bhttp_t));
	free(bhttp);
}

/*------------------------------------------------------------------------
 Add a handle to the bhttp.
 Returns 0 on success,
 		 -1 on failure.
------------------------------------------------------------------------*/
static int bhttp_handleOpened(bhttp_t *bhttp, int h)
{
	clock_t now = eclock();
	bhttp_conn_t *pc;

	if (!bhttp || (h == -1))
		return -1;
	pc = (bhttp_conn_t *)assoctab_subscript(bhttp->conns, h);
	if (pc) {
		DPRINT(("bhttp_handleOpened: h:%d already open\n", h));
		return -1;
	}
	pc = (bhttp_conn_t *)assoctab_subscript_grow(bhttp->conns, h);
	if (!pc) {
		DPRINT(("bhttp_handleOpened: can't grow table for h:%d\n", h));
		return -1;
	}
	memset(pc, 0, sizeof(bhttp_conn_t));
	pc->ilen = 0;
	pc->ipos = 0;
	FD_SET(h, &bhttp->rfds);
	pc->t_connect = now;
	if (h > bhttp->sockmax)
		bhttp->sockmax = h;
	bhttp_assertValid(bhttp);
	DPRINT(("bhttp_handleOpened(h:%d)\n", h));
	return 0;
}

static char*
bufgets( bhttp_conn_t *pc )
{
	int i;
	char c;

	for ( i = pc->ipos; pc->ipos < pc->ilen; ++pc->ipos) {
		c = pc->ibuf[pc->ipos];
		if ( c == '\n' || c == '\r' ) {
			pc->ibuf[pc->ipos] = '\0';
			++pc->ipos;
			if ( c == '\r' && pc->ipos < pc->ilen 
			&&   pc->ibuf[pc->ipos] == '\n' ) {
				pc->ibuf[pc->ipos] = '\0';
				++pc->ipos;
			}
			return &(pc->ibuf[i]);
		}
	}
	return (char*) 0;
}

/*------------------------------------------------------------------------
 Parse the http header already received for this connection.
 Return 0 on success.
------------------------------------------------------------------------*/
static int parse_request(bhttp_conn_t *pc)
{
	char *line;

	pc->ipos = 0;
	line = bufgets(pc);
	pc->pmethod = strtok(line, "\040\t");
	pc->purl = strtok(NULL, "\040\t");
	pc->pprotocol = strtok(NULL, "\040\t");

	if (pc->pprotocol)
		return 0;
	else
		return -1;
}

/*------------------------------------------------------------------------
 Read data from handle h.
 Handle must be set non-blocking and have data waiting.
 Returns 0 if data read successfully, but more is expected,
 		 -1 on error caller should close the handle and call handleClosed,
 On completion, the length read is returned.
------------------------------------------------------------------------*/
static int bhttp_readData(bhttp_conn_t *pc, int h)
{
	int nRecvd;
	int nToRecv;
	int old_istate;

	if (!pc || (h == -1))
		return -1;
	old_istate = pc->istate;
	nToRecv = bhttp_IBUF_LEN - pc->ilen;
	if (nToRecv <= 0) {
		DPRINT(("bhttp_readData: bug: h:%d tried to read %dB (pos:%d)\n", h, nToRecv, pc->ilen));
		assert(nToRecv > 0);
	}
	errno = 0;
	nRecvd = recv(h, pc->ibuf + pc->ilen, nToRecv, 0);
#ifdef VERBOSE
	DPRINT(("bhttp_readData: recv(h:%d, pos:%d, n:%d) returns %d (errorr:%d)\n", h, pc->ilen, nToRecv, nRecvd, errno));
#endif
	if (nRecvd <= 0) {
		if ((nRecvd == 0) || (errno == ENOTCONN)) {
			DPRINT(("bhttp_readData: h:%d closed connection\n", h));
			return -1;	/* error - connection closed from other end */
		} else if (errno == EWOULDBLOCK) {
			DPRINT(("bhttp_readData: no data for h:%d\n", h));
			return 0;	/* error - selected, but no data. */
		} else if (errno == EINTR) {
			DPRINT(("bhttp_readData: h:%d recv interupted by signal\n", h));
			return -1;
		}
		DPRINT(("bhttp_readData: read error:%d on h:%d\n", errno, h));
		return -1;	/* error - read error */
	}
#ifdef VERBOSE
	DPRINT(("bhttp_readData: recv'd %dB from h:%d:%s\n", nRecvd, h, hexstring(pc->ibuf + pc->ilen, nRecvd)));
#endif
	pc->ilen += nRecvd;
	/* Return 1 if we have reached the end of the http headers */
	while (pc->ipos < pc->ilen) {
		char c = pc->ibuf[pc->ipos++];
		switch(pc->istate) {
		case bhttp_ISTATE_INITIAL:
			switch(c) {
			case '\n':
				pc->istate = bhttp_ISTATE_LF;
				break;
			case '\r':
				pc->istate = bhttp_ISTATE_CR;
				break;
			}
			break;
		case bhttp_ISTATE_LF:
			switch(c) {
			case '\n':
				/* Two newlines detected, end request */
				pc->istate = bhttp_ISTATE_END;
				break;
			case '\r':
				pc->istate = bhttp_ISTATE_CR;
				break;
			default:
				pc->istate = bhttp_ISTATE_INITIAL;
				break;
			}
			break;
		case bhttp_ISTATE_CR:
			switch(c) {
			case '\n':
				pc->istate = bhttp_ISTATE_CRLF;
				break;
			case '\r':
				/* Two returns in a row, end request */
				pc->istate = bhttp_ISTATE_END;
				break;
			default:
				pc->istate = bhttp_ISTATE_INITIAL;
				break;
			break;
			}
		case bhttp_ISTATE_CRLF:
			switch(c) {
			case '\n':
				/* Two newlines in a row, end request */
				pc->istate = bhttp_ISTATE_END;
				break;
			case '\r':
				pc->istate = bhttp_ISTATE_CRLFCR;
				break;
			default:
				pc->istate = bhttp_ISTATE_INITIAL;
				break;
			}
			break;
		case bhttp_ISTATE_CRLFCR:
			switch(c) {
			case '\n':
			case '\r':
				/* Two CRLFs or two CRs in a row, end request */
				pc->istate = bhttp_ISTATE_END;
				break;
			default:
				pc->istate = bhttp_ISTATE_INITIAL;
				break;
			}
			break;
		case bhttp_ISTATE_END:
			/* Should never be reached */
			break;
		}
	}
	if (pc->istate == bhttp_ISTATE_END && pc->istate != old_istate) {
		if (parse_request(pc))
			return -1;
		return pc->ipos;
	}
	return 0;
}

/*------------------------------------------------------------------------
 Write data to handle h.
 Handle must be set non-blocking and be ready for writing.
 Returns 0 if data written successfully, but more is waiting,
 		 -1 on error; caller should close the handle and call handleClosed,
 On completion, the length written is returned and the files descriptor
 is removed from wfds.
------------------------------------------------------------------------*/
static int bhttp_writeData(bhttp_conn_t *pc, int h)
{
	int nWritten;
	int nToWrite;

	if (!pc || (h == -1))
		return -1;
	nToWrite = pc->olen - pc->opos;
	if (nToWrite <= 0) {
		DPRINT(("bhttp_writeData: bug: h:%d tried to write %dB (pos:%d)\n", h, nToWrite, pc->opos));
		assert(nToWrite > 0);
	}
	errno = 0;
	nWritten = send(h, pc->obuf + pc->opos, nToWrite, 0);
#ifdef VERBOSE
	DPRINT(("bhttp_writeData: send(h:%d, pos:%d, n:%d) returns %d (errorr:%d)\n", h, pc->opos, nToWrite, nWritten, errno));
#endif
	if (nWritten <= 0) {
		if ((nWritten == 0) || (errno == ENOTCONN)) {
			DPRINT(("bhttp_writeData: h:%d closed connection\n", h));
			return -1;	/* error - connection closed from other end */
		} else if (errno == EWOULDBLOCK) {
			DPRINT(("bhttp_writeData: h:%d would block\n", h));
			return 0;	/* error - selected, but no data. */
		} else if (errno == EINTR) {
			DPRINT(("bhttp_writeData: h:%d send interupted by signal\n", h));
			return -1;
		}
		DPRINT(("bhttp_writeData: send error:%d on h:%d\n", errno, h));
		return -1;	/* error - send error */
	}
#ifdef VERBOSE
	DPRINT(("bhttp_writeData: sent %dB to h:%d:%s\n", nWritten, h, hexstring(pc->obuf + pc->opos, nWritten)));
#endif
	pc->opos += nWritten;
	if (pc->opos > pc->olen) {
		DPRINT(("bhttp_writeData: sent more than len:%d data to h:%d\n", pc->olen, h));
		return -1;
	} else if (pc->opos == pc->olen) {
		return pc->olen;
	}
	return 0;	/* still waiting to send more data */
}

/*------------------------------------------------------------------------
 Clean up any data associated with the given handle.
------------------------------------------------------------------------*/
static void bhttp_handleClosed(bhttp_t *bhttp, int h)
{
	if (!bhttp || (h == -1)) {
		DPRINT(("bhttp_handleClosed: bad params (h:%d)\n", h));
		return;
	}
	if (assoctab_subscript_delete(bhttp->conns, h)) {
		DPRINT(("bhttp_handleClosed: no such handle h:%d\n", h));
		return;
	}
	FD_CLR(h, &bhttp->rfds);
	FD_CLR(h, &bhttp->wfds);
	if (h == bhttp->sockmax)
		for (bhttp->sockmax--; bhttp->sockmax >= 0; bhttp->sockmax--)
			if (FD_ISSET(bhttp->sockmax, &bhttp->rfds)
			||  FD_ISSET(bhttp->sockmax, &bhttp->wfds)
			||  (bhttp->sockmax == bhttp->sockin))
				break;
	DPRINT(("bhttp_handleClosed(h:%d)\n", h));
}

/*------------------------------------------------------------------------
 Gets the set of sockets that need reading into *rfds, and writing 
 into *wfds.
 Returns the maximum socket set.
------------------------------------------------------------------------*/
int bhttp_getfds(bhttp_t *bhttp, fd_set *rfds, fd_set *wfds)
{
	int sock;

	FD_SET(bhttp->sockin, rfds);	/* always listen for new connections */
	DPRINT(("bhttp_getfds: checking socks:%d", bhttp->sockin));
	for (sock = 0; sock <= bhttp->sockmax; sock++) {
		if (FD_ISSET(sock, &bhttp->rfds)) {
			FD_SET(sock, rfds);
			DPRINT((",%d", sock));
		}
		if (FD_ISSET(sock, &bhttp->wfds)) {
			FD_SET(sock, wfds);
		}
	}
	DPRINT(("\n"));
#ifdef VERBOSE
	DPRINT((" sockmax:%d total:%d\n", bhttp->sockmax, (1 + bhttp->conns->n_used)));
#endif
	return bhttp->sockmax;
}

/*------------------------------------------------------------------------
 Return descriptive name for given http result code.
------------------------------------------------------------------------*/
static const char *httpResult2a(int result)
{
	switch (result) {
	case 200: return "OK";
	case 400: return "Bad Request";
	case 404: return "Not Found";
	default: return "Unknown Error Code";
	}
}

/*------------------------------------------------------------------------
 Look at the request from the given handle, and formulate an appropriate
 http response.

 Returns 0 on success.
 Returns -1 on failure.
------------------------------------------------------------------------*/
static int bhttp_formatOutputBuffer(bhttp_t *bhttp, int h, int status, char *type, char *content)
{
	bhttp_conn_t *pc;
	time_t now;
	time_t mod = 0;
	char nowbuf[100];
	char modbuf[100];
	static const char* rfc1123fmt = "%a, %d %b %Y %H:%M:%S GMT";

	if (!bhttp || (h == -1))
		return -1;
	pc = (bhttp_conn_t *)assoctab_subscript(bhttp->conns, h);
	if (!pc) {
		DPRINT(("bhttp_formatOutputBuffer: no such handle h:%d\n", h));
		return -1;
	}
    now = time( (time_t*) 0 );
    if ( mod == (time_t) 0 )
        mod = now;
    (void) strftime( nowbuf, sizeof(nowbuf), rfc1123fmt, gmtime( &now ) );
    (void) strftime( modbuf, sizeof(modbuf), rfc1123fmt, gmtime( &mod ) );
    pc->olen = sprintf( pc->obuf, "%.20s %d %s\r\nServer: %s\r\nContent-type: %s\r\nDate: %s\r\nLast-modified: %s\r\nConnection: close\r\n",
        "HTTP/1.0", status, httpResult2a(status), "bhttp", type, nowbuf,
        modbuf );
	if (content) {
		/* Terminate the http header with a blank line, follow it with the content for this URL */
		pc->olen += sprintf(pc->obuf+pc->olen, "Content-length: %d\r\n\r\n%s", strlen(content), content);
	} else {
		/* Terminate the http header with a blank line */
		pc->olen += sprintf(pc->obuf+pc->olen, "\r\n");
	}
	pc->opos = 0;
	return 0;
}

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
int bhttp_poll(bhttp_t *bhttp, fd_set *rfds, fd_set *wfds, int nsocks)
{
	clock_t now;
	int sock;
	int i;

	if (!bhttp || !bhttp->conns)
		return -1;
	bhttp_assertValid(bhttp);
	assert(nsocks >= 0);
	bhttp->cursock = 0;	/* FIXME: should be min(socks) */
	bhttp->nsocks = nsocks;
	if (FD_ISSET(bhttp->sockin, rfds) && (bhttp->nsocks > 0)) {
		int newsock;
		struct sockaddr_in client_addr;
		int len = sizeof(struct sockaddr_in);

		newsock = accept(bhttp->sockin, (struct sockaddr *)&client_addr,&len);
		if (newsock == -1) {
			DPRINT(("bhttp_poll: accept error:%d on sock:%d\n", errno, bhttp->sockin));
		} else {
			int flags;

#ifdef VERBOSE
			DPRINT(("bhttp_poll: accepting connection from %s on sock:%d\n", inet_ntoa(client_addr.sin_addr), newsock));
#endif
			flags = fcntl(newsock, F_GETFL, 0);
			if ((-1 == flags)
			||  (-1 == fcntl(newsock, F_SETFL, (flags | O_NONBLOCK)))) {
				DPRINT(("bhttp_poll: fcntl error:%d on sock:%d\n", errno, newsock));
				close(newsock);
				return -1;
			} else if (-1 == bhttp_handleOpened(bhttp, newsock)) {
				DPRINT(("bhttp_poll: can't add h:%d\n", newsock));
				close(newsock);
				return -1;
			}
		}
		bhttp->nsocks--;
	}

	while ((bhttp->cursock <= bhttp->sockmax) && (bhttp->nsocks > 0)) {
		int used = FALSE;
		int len;
		int h = bhttp->cursock;
		bhttp_conn_t *pc = NULL;

		if (FD_ISSET(h, rfds)
		&&  FD_ISSET(h, &bhttp->rfds)) {

			pc = (bhttp_conn_t *)assoctab_subscript(bhttp->conns, h);
			if (!pc) {
				DPRINT(("bhttp_poll: no such handle h:%d\n", h));
				return -1;
			}
			len = bhttp_readData(pc, h);
			if (len == -1) {
				DPRINT(("bhttp_readData error\n"));
				bhttp_handleClosed(bhttp, h);
				close(h);
				/* silently fails */
			} else if (len > 0) {
				char buf[1024];
				int buflen;
				bhttp_url2buf_result_t urlResult;
#ifdef VERBOSE
				DPRINT(("bhttp_poll: h:%d finished receiving data\n", h)); 
#endif
				memset(&urlResult, 0, sizeof(urlResult));
				buflen = bhttp->url2buf_cb(bhttp->url2buf_context, pc->purl, buf, sizeof(buf), &urlResult);
				buf[buflen] = 0;
				/* Finished reading http header, formulate http response */
				bhttp_formatOutputBuffer(bhttp, h, urlResult.httpResultCode, urlResult.mimeType, buf);
				FD_SET(h, &bhttp->wfds);
			} else {
#ifdef VERBOSE
				DPRINT(("bhttp_poll: h:%d waiting for more data\n", h));
#endif
			}
			used = TRUE;
		} 
		if (FD_ISSET(h, wfds)
		&&  FD_ISSET(h, &bhttp->wfds)) {
			if (!pc) pc = (bhttp_conn_t *)assoctab_subscript(bhttp->conns, h);
			if (!pc) {
				DPRINT(("bhttp_poll: no such handle h:%d\n", h));
				return -1;
			}
			len = bhttp_writeData(pc, h);
			if (len == -1) {
				DPRINT(("bhttp_writeData error\n"));
				bhttp_handleClosed(bhttp, h);
				close(h);
				/* silently fails */
			} else if (len > 0) {
#ifdef VERBOSE
				DPRINT(("bhttp_poll: h:%d finished sending data\n", h)); 
#endif
				bhttp_handleClosed(bhttp, h);
				if (-1 == close(h)) {
					DPRINT(("bhttp_poll: close error:%d\n", errno));
				}
			} else {
#ifdef VERBOSE
				DPRINT(("bhttp_poll: h:%d waiting for more data\n", h));
#endif
			}
			used = TRUE;
		}
		if (used)
			bhttp->nsocks--;
		bhttp->cursock++;
	}

	now = eclock();
	if ((long)(now - bhttp->t_last_poll) < 10 * ECLOCKS_PER_SEC)
		return 0;	/* don't poll more than once every 10s */
	bhttp->t_last_poll = now;
#ifdef VERBOSE
	DPRINT(("bhttp_poll: t:%d checking %d conns for timeouts\n", now, bhttp->conns->n_used));
#endif

	/* Iterate through handles, deleting those that have timed out,
	 * in reverse order, since deleting will change indices of later entries.
	 */
	for (i = bhttp->conns->n_used - 1; i >= 0; i--) {
		assoctab_item_t *pi;
		bhttp_conn_t *pc;
		int h;
 		pi = assoctab_getkey(bhttp->conns, i);
		if (!pi) {
			DPRINT(("bhttp_poll: assoctab_getkey(%d) returns NULL?\n", i));
			return -1;
		}
		h = pi->key;
		pc = (bhttp_conn_t *)pi->value;	
		if (!pc) {
			DPRINT(("bhttp_poll: conns[%d] == NULL?\n", i));
			return -1;
		}
		if ((long)(now - pc->t_connect) > bhttp_CONN_TIMEOUT * ECLOCKS_PER_SEC) {
			DPRINT(("bhttp_poll: h:%d timed out\n", h));
			bhttp_handleClosed(bhttp, h);
			if (-1 == close(h)) {
				DPRINT(("bhttp_poll: close(%d) error:%d\n", h, errno));
				return -1;
			}
		}
	}
	return 0;
}
