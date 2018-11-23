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

#include <stdio.h>
#include <assert.h>

#ifdef UNIX
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#endif

#include "portable.h"
#include "sbdclnt.h"

#include "logprint.h"

/*------------------------------------------------------------------------
 Get and clear any waiting I/O errors.
 If msg is not NULL, it may be filled with an explanatory message.
 Returns 0 if no error was waiting,
         a system dependent numeric code if an error occurred.
------------------------------------------------------------------------*/
static int get_socket_error(char *msg)
{
	int err = 0;

#ifdef UNIX
	err = errno;
	errno = 0;
	if (!msg)
		return err;
	msg[0] = '\0';
	/* could sprintf berkeley error constant names into msg... */
	return err;
#else
#ifdef WIN32
	err = WSAGetLastError();
	if (!msg)
		return err;
	if (!err) {
		msg[0] = '\0';
		return err;
	}
	switch (err) {
	case WSASYSNOTREADY:
		sprintf(msg, "Indicates that the underlying network subsystem is not ready for network communication.");
		break;
	case WSAVERNOTSUPPORTED:
		sprintf(msg, "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
		break;
	case WSAEINPROGRESS:
		sprintf(msg, "A blocking Windows Sockets 1.1 operation is in progress.");
		break;
	case WSAEPROCLIM:
		sprintf(msg, "Limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
		break;
	case WSAEFAULT:
		sprintf(msg, "The lpWSAData is not a valid pointer.");
		break;
	case WSANOTINITIALISED:
		sprintf(msg, "A successful WSAStartup must occur before using this API.");
		break;
	case WSAENETDOWN:
		sprintf(msg, "The network subsystem or the associated service provider has failed.");
		break;
	case WSAEAFNOSUPPORT:
		sprintf(msg, "The specified address family is not supported.");
		break;
	case WSAEMFILE:
		sprintf(msg, "No more socket descriptors are available.");
		break;
	case WSAENOBUFS:
		sprintf(msg, "No buffer space is available. The socket cannot be created.");
		break;
	case WSAEPROTONOSUPPORT:
		sprintf(msg, "The specified protocol is not supported.");
		break;
	case WSAEPROTOTYPE:
		sprintf(msg, "The specified protocol is the wrong type for this socket.");
		break;
	case WSAESOCKTNOSUPPORT:
		sprintf(msg, "The specified socket type is not supported in this address family.");
		break;
	default:
		sprintf(msg, "Unknown Winsock Error");
		break;
	}
#endif /* _WIN32 */
#endif /* !UNIX */
	return err;
}

/*------------------------------------------------------------------------
 Make a socket non-blocking.
 Returns 0 on success,
 		 non-zero system dependent error code on failure.
------------------------------------------------------------------------*/
static int set_socket_nonblocking(int sock)
{
	int err;
#ifdef UNIX
	int flags;
#else
#ifdef _WIN32
	ULONG nonblocking;
#endif
#endif

	if (sock == -1)
		return -1;

#ifdef UNIX
	flags = fcntl(sock, F_GETFL, 0);
	if ((-1 == flags)
	||  (-1 == fcntl(sock, F_SETFL, (flags | O_NONBLOCK)))) {
		err = get_socket_error(NULL);
		DPRINT(("set_socket_nonblocking(%d): fcntl error:%d\n", sock, err));
		return err;
	}
#else
#ifdef _WIN32
	nonblocking = TRUE;
	if (ioctlsocket(sock, FIONBIO, &nonblocking)) {
		err = get_socket_error(NULL);
		DPRINT(("set_socket_nonblocking(%d): ioctlsocket error:%d\n", sock, err));
		return err;
	}
#endif
#endif
	return 0;
}

#ifdef _WIN32
/*------------------------------------------------------------------------
 Initialize Winsock.
 Returns 0 on success,
 		 non-zero WSA error code on failure.
------------------------------------------------------------------------*/
static int Winsock_init(void)
{
	WSADATA wsa;
	int err;

	DPRINT(("Winsock_init() called...\n"));

	/* Start Winsock */
	err = WSAStartup(MAKEWORD(1, 1), &wsa);
	if (err) {
		char msg[256];
		err = get_socket_error(msg);
		DPRINT(("Winsock_init: WSAStartup(1.1) error(%d):%s\n", err, msg));
	 	return err;
	}

	/* Success: Print out what we found */
	DPRINT(("Winsock_init: Winsock Initialized\n"));
	DPRINT(("Winsock_init: Version    : %d.%d [%d.%d]\n",
		HIBYTE(wsa.wVersion), LOBYTE(wsa.wVersion),
		HIBYTE(wsa.wHighVersion), LOBYTE(wsa.wHighVersion)));
	DPRINT(("Winsock_init: Description: %s\n", wsa.szDescription));
	DPRINT(("Winsock_init: Status     : %s\n", wsa.szSystemStatus));
	DPRINT(("Winsock_init: MaxSockets : %d\n", wsa.iMaxSockets));
	DPRINT(("Winsock_init: MaxUDPdg   : %d\n", wsa.iMaxUdpDg));
/*	DPRINT(("Winsock_init: Vendor     : %s\n", wsa.lpVendorInfo)); */

	return 0;
}
#endif

/*------------------------------------------------------------------------
 Create an instance of the sbdclnt module, and connect to hostname:port
 in preparation for sending the len bytes of binary data in buf.
 Returns NULL on failure.
------------------------------------------------------------------------*/
sbdclnt_t *sbdclnt_create(const char *buf, size_t len, const char *hostname, short port)
{
	int err;
	sbdclnt_t *sbdclnt;
	int proto;
	struct sockaddr_in hostaddr;
	struct hostent *he;
#ifdef UNIX
	struct protoent *pe;
#endif

	if ((len == 0) || (len > sbd_MAXLEN)) {
		DPRINT(("sbdclnt_create: len:%d is out of range\n", len));
		return NULL;
	}
	sbdclnt = (sbdclnt_t *)malloc(sizeof(sbdclnt_t));
	if (!sbdclnt) {
		DPRINT(("sbdclnt_create: malloc failed\n"));
		return NULL;
	}
	memcpy(sbdclnt->buf, sbd_MAGIC, 2);
	sbdclnt->buf[2] = dpGETSHORT_FIRSTBYTE(len);
	sbdclnt->buf[3] = dpGETSHORT_SECONDBYTE(len);
	memcpy(sbdclnt->buf + sbd_HEADERLEN, buf, len);
	sbdclnt->len = len + sbd_HEADERLEN;
	sbdclnt->pos = 0;

#ifdef _WIN32
	err = Winsock_init();
	if (err) {
		DPRINT(("sbdclnt_create: Winsock_init error:%d\n", err));
		free(sbdclnt);
		return NULL;
	}
#endif

	he = gethostbyname(hostname);
	if (!he) {
		DPRINT(("sbdclnt_create: host unknown:%s\n", hostname));
		free(sbdclnt);
		return NULL;
	}
#ifdef UNIX
	pe = getprotobyname("tcp");
	if (!pe) {
		DPRINT(("sbdclnt_create: no such protocol:TCP?\n"));
		free(sbdclnt);
		return NULL;
	}
	proto = pe->p_proto;
#else
#ifdef _WIN32
	proto = IPPROTO_TCP;
#endif
#endif
	sbdclnt->sock = socket(AF_INET, SOCK_STREAM, proto);
	if (-1 == sbdclnt->sock) {
		DPRINT(("sbdclnt_create: socket error:%d\n", get_socket_error(NULL)));
		free(sbdclnt);
		return NULL;
	}
	if (err = set_socket_nonblocking(sbdclnt->sock)) {
		DPRINT(("sbdclnt_create: can't set sock:%d non-blocking\n", sbdclnt->sock));
		free(sbdclnt);
		return NULL;
	}

	/* non-blocking connect - may return EINPROGRESS */
	memset(&hostaddr, 0, sizeof(struct sockaddr_in));
	memcpy(&hostaddr.sin_addr, he->h_addr, he->h_length);
	hostaddr.sin_family = he->h_addrtype;
	hostaddr.sin_port = htons(port);
	if (-1 == connect(sbdclnt->sock, (struct sockaddr *)&hostaddr,
	                   sizeof(struct sockaddr))) {
		err = get_socket_error(NULL);
#ifdef UNIX
		if (err != EINPROGRESS)
#else
#ifdef _WIN32
		if (err != WSAEWOULDBLOCK)
#endif
#endif
		{
			DPRINT(("sbdclnt_create: connect error:%d\n", err));
			my_CLOSE(sbdclnt->sock);
			free(sbdclnt);
			return NULL;
		}
	}
	return sbdclnt;
}

/*------------------------------------------------------------------------
 Destroy an instance of the sbdclnt module.
------------------------------------------------------------------------*/
void sbdclnt_destroy(sbdclnt_t *sbdclnt)
{
	if (!sbdclnt)
		return;
	if (sbdclnt->sock != -1)
		my_CLOSE(sbdclnt->sock);
	memset(sbdclnt, 0, sizeof(sbdclnt_t));
	free(sbdclnt);
#ifdef _WIN32
	WSACleanup();
#endif
}

/*------------------------------------------------------------------------
 Continue sending the small binary block of data to a server using
 non-blocking I/O.
 Returns the number of bytes sent (not including the header) on success,
 		   when all data has been passed to the OS's TCP layer,
		 0 if more calls are needed,
		-1 on error.
------------------------------------------------------------------------*/
int sbdclnt_poll(sbdclnt_t *sbdclnt)
{
	int err;
	int nToSend;
	int nSent;

	assert((sbdclnt->pos >= 0) && (sbdclnt->pos <= sbdclnt->len));
	if (sbdclnt->pos == sbdclnt->len) {
		assert(sbdclnt->sock == -1);
		return (sbdclnt->len - sbd_HEADERLEN);
	}
	if (sbdclnt->sock == -1)
		return -1;

	nToSend = sbdclnt->len - sbdclnt->pos;
	nSent = send(sbdclnt->sock, sbdclnt->buf + sbdclnt->pos, nToSend, 0);
	if (nSent <= 0) {
		err = get_socket_error(NULL);
		if ((nSent == 0) || ((nSent == -1) && (err == my_EWOULDBLOCK))) {
			DPRINT(("sbdclnt_poll: send would block\n"));
			return 0;
		} else if (err == my_EMSGSIZE) {
			DPRINT(("sbdclnt_poll: buf too large to send()!\n"));
			return -1;
		}
		DPRINT(("sbdclnt_poll: send error:%d\n", err));
		my_CLOSE(sbdclnt->sock);
		sbdclnt->sock = -1;
		return -1;
	}
	sbdclnt->pos += nSent;
	if (sbdclnt->pos == sbdclnt->len) {
		/* completed successfully */
		DPRINT(("sbdclnt_poll: finished sending %d bytes\n", sbdclnt->len));
		my_CLOSE(sbdclnt->sock);
		sbdclnt->sock = -1;
		return (sbdclnt->len - sbd_HEADERLEN);
	}
	/* still have more bytes to write */
	return 0;
}
