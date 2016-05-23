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

/*----------------------------------------------------------------------------
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been modified by the Apolyton CtP2 Source Code Project.
 * Contact the authors at ctp2source@apolyton.net.
 *
 * Modifications from the Activision Anet 0.10 code:
 *
 * - 2005/03/11: Corrected detection of own address, based on remarks from
 *               tchu at the yahoo anet group.
 *
 *----------------------------------------------------------------------------
 */

/*****************************************************************************
 Multiport Winsock TCP Protocol Driver Functions For Win32
 (c) 1997 By Activision Inc.  All rights reserved.

  (Original Winsock module by M. Livesay)
  $Log: wstcp.c $
  Revision 1.7  1997/09/27 20:45:22  dkegel
  Fixed horrible bug that caused true port number to not be saved,
  and sometimes caused driver open to fail because broadcast failed
  Revision 1.6  1997/09/20 05:16:16  lbennett
  Fixed saved port number to use host byte ordering, as expected.
  Revision 1.5  1997/09/19 21:33:01  lbennett
  Write actual port number to commInitReq.
  Revision 1.4  1997/08/29 01:19:07  lbennett
  Removed MSVC-specific pragmas from non-WIN32 builds.
  Revision 1.3  1997/08/27 23:34:01  lbennett
  Converted to use dictset for handles, changed TCPPEER.
  Revision 1.1  1997/08/22 22:35:16  lbennett
  Initial revision
*****************************************************************************/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#include "wstcp.h"
#include "dpmem.h"

/* fancy dance to allow including dp.h */
#define DEBUG_MODULE 0
#define dp_dprintf __bogus_dprintf

#include "dp.h"		/* for the ping packet stuff; includes ddprint.h */

/* now the real dprint definitions */
#undef dp_dprintf
#undef DPRINT
#undef DEBUG_MODULE
#define DEBUG_MODULE 1		/* Set to 1 to enable debug prints */
#include "ddprint.h"

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows linked code has six of these ) */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/**
* Constants
*/

#define WAIT_SECONDS	5

/**
* Methods
*/

/*****************************************************************************
 Return the last socket error.  If in debug mode, also print error.
*****************************************************************************/
static int			/* error number */
getsockerror(void)
{
	int err;
	char string[256];

	err = WSAGetLastError();
	sprintf(string, "Socket Error: %i\n", (int) err);
	OutputDebugString(string);

	return(err);
}

/*-------------------------------------------------------------------------
 Broadcast a packet with a unique identifier, and wait for it to come
 back.  Read the IP address from it. This may seem a long way of getting
 the host IP address, but it is necessary, as getsockname() will return
 INADDR_ANY on UDP sockets, and the gethostname()/gethostbyname()
 technique on Win95 will check for a DNS server if the user has a TCP
 modem setup (it causes the dialer to come up).

 If found, the address is copied into pTcp->peers[TCP_HDL_ME].
-------------------------------------------------------------------------*/
static BOOL				/* success BOOL */
gethostaddr_ping(
	TCPINSTANCE *pTcp)	/* (modified) Current tcp instance */
{
	TCPPEER addr;
	int addrlen;
	int bAddressFound = FALSE;
	TCPPEER* pAddr;
	SOCKADDR_IN stRmtAddr;
	clock_t waittime;
	ULONG cBytes;
	ULONG result;
	ULONG uniqueID;

	/* Broadcast a packet containing a unique ID */
	uniqueID = (ULONG) clock();
	pAddr = (TCPPEER*)dcstFindValue(pTcp->handles, pTcp->broadcastHandle);
	if(pAddr == NULL)
		return(FALSE);
	stRmtAddr.sin_family = AF_INET;
	stRmtAddr.sin_port = pAddr->port;
	stRmtAddr.sin_addr.s_addr = pAddr->addr;
	cBytes = sendto(pTcp->socket, (char *) &uniqueID, 4, 0,
				(struct sockaddr *) &stRmtAddr, sizeof(stRmtAddr));
	if ((cBytes == 0) || (cBytes == SOCKET_ERROR)) {
		getsockerror();
		return(FALSE);
	}

	/* wait for packet to come back */
	waittime = clock() + CLOCKS_PER_SEC * WAIT_SECONDS;
	while ((clock() < waittime) && (!bAddressFound)) {
		addrlen = sizeof(stRmtAddr);
		cBytes = recvfrom(pTcp->socket, (char *) &result, 4, 0,
					(struct sockaddr *) &stRmtAddr, &addrlen);
		if ((cBytes != 0)&&(cBytes != SOCKET_ERROR)&&(result == uniqueID)) {
			addr.addr = stRmtAddr.sin_addr.s_addr;
			addr.port = stRmtAddr.sin_port;
			pTcp->myHandle = dcstReplace(pTcp->handles, pTcp->myHandle, &addr);
			DPRINT(("gethostaddr_ping: Host Address Retrieved: [%s:%d]\n", (char *) inet_ntoa(stRmtAddr.sin_addr), ntohs(stRmtAddr.sin_port)));
			bAddressFound = TRUE;
		}
	}
	return bAddressFound;
}

/*-------------------------------------------------------------------------
 Strategy from Quinn & Shute, *Windows Sockets Network Programming
 Get UDP socket, connect to arbitrary port and address (not loopback),
 and get the local address from it.

 If found, the address is copied into pTcp->peers[TCP_HDL_ME].
 Note that the connect() call will trigger dial-up if dial-up if set.
-------------------------------------------------------------------------*/
static BOOL				/* success BOOL */
gethostaddr_connect(
	TCPINSTANCE *pTcp)	/* (modified) Current tcp instance */
{
	TCPPEER addr;
	int addrlen;
	int bAddressFound = FALSE;
	SOCKET hSock;
	SOCKADDR_IN stRmtAddr;
	SOCKADDR_IN stLclAddr;
	int nRet;

	hSock = socket(AF_INET, SOCK_DGRAM, 0);
	if(hSock != INVALID_SOCKET) {
		stRmtAddr.sin_family = AF_INET;
		stRmtAddr.sin_port = htons(IPPORT_ECHO);
		stRmtAddr.sin_addr.s_addr = inet_addr("128.127.50.1");
 		/* Note: connect() triggers dial-up if dial-up is set; this
		 * can take on order of a minute. */
		nRet = connect(hSock, (LPSOCKADDR)&stRmtAddr, sizeof(SOCKADDR));
		if(nRet != SOCKET_ERROR) {
			addrlen = sizeof(stLclAddr);
			getsockname(hSock, (LPSOCKADDR)&stLclAddr, (int FAR*)&addrlen);
			if(INADDR_ANY != stLclAddr.sin_addr.s_addr) {
#if defined(ANET_ORIGINAL)
				addr.addr = stRmtAddr.sin_addr.s_addr;
				addr.port = stRmtAddr.sin_port;
				pTcp->myHandle = dcstReplace(pTcp->handles, pTcp->myHandle, &addr);
				bAddressFound = TRUE;
				DPRINT(("gethostaddr_connect: Host Address Retrieved: [%s:%d]\n", (char *) inet_ntoa(stRmtAddr.sin_addr), ntohs(stRmtAddr.sin_port)));
#else
				/* Based on remarks from tchu at the yahoo anet group */
				TCPPEER * pAddr;
				pAddr = (TCPPEER *) dcstFindValue(pTcp->handles, pTcp->myHandle);

				addr.addr = stLclAddr.sin_addr.s_addr;
				addr.port = pAddr->port;
				DPRINT(("gethostaddr_connect: Host Address Retrieved: [%s:%d]\n", (char *) inet_ntoa(stLclAddr.sin_addr), ntohs(addr.port)));
				pTcp->myHandle = dcstReplace(pTcp->handles, pTcp->myHandle, &addr);
				bAddressFound = TRUE;
#endif
			}
		}
		closesocket(hSock);
	}
	return bAddressFound;
}

/*--------------------------------------------------------------------------
 Yet another strategy to find the local machines IP
 Uses gethostname followed by gethostbyname
--------------------------------------------------------------------------*/

static BOOL gethostaddr_gethostname(TCPINSTANCE *pTcp)
{
	char			name[256];
	int				err;
	HOSTENT			*host;
	unsigned char	**ptr;
	BOOL			found;
	BOOL			secondary;

	err = gethostname(name, 256);

	/*
	 * Test to see if an error occured
	 */
	if (err)
	{
		if (err == SOCKET_ERROR)
		{
			switch (WSAGetLastError())
			{
				case WSAEFAULT:
					DPRINT(("gethostaddr_gethostname: The namelen parameter is too small.\n"));
					break;

				case WSANOTINITIALISED:
					DPRINT(("gethostaddr_gethostname: A successful WSAStartup must occur before using this function.\n"));
					break;

				case WSAENETDOWN:
					DPRINT(("gethostaddr_gethostname: The Windows Sockets implementation has detected that the network subsystem has failed.\n"));
					break;

				case WSAEINPROGRESS:
					DPRINT(("gethostaddr_gethostname: A blocking Windows Sockets operation is in progress.\n"));
					break;

				case WSAHOST_NOT_FOUND:
					DPRINT(("gethostaddr_gethostname: Host not found\n"));
					break;

				default:
					DPRINT(("gethostaddr_gethostname: Unknown winsock error\n"));
					break;
			}
		}
		else
		{
			DPRINT(("gethostaddr_gethostname: Unknown result\n"));
		}
		return (FALSE);
	}

	/*
	 * Is the string empty ?
	 */
	if (name[0] == '\0')
	{
		DPRINT(("gethostaddr_gethostname: Empty string returned\n"));
		return (FALSE);
	}

	/*
	 * Attempt to get the host information by name
	 */
	host = gethostbyname(name);

	/*
	 * Check the return value
	 */
	if (host == NULL)
	{
		switch (WSAGetLastError())
		{
			case WSANOTINITIALISED:
				DPRINT(("gethostaddr_gethostname: A successful WSAStartup must occur before using this function.\n"));
				break;

			case WSAENETDOWN:
				DPRINT(("gethostaddr_gethostname: The Windows Sockets implementation has detected that the network subsystem has failed.\n"));
				break;

			case WSAHOST_NOT_FOUND:
				DPRINT(("gethostaddr_gethostname: Authoritative Answer Host not found.\n"));
				break;

			case WSATRY_AGAIN:
				DPRINT(("gethostaddr_gethostname: Non-Authoritative Host not found, or SERVERFAIL.\n"));
				break;

			case WSANO_RECOVERY:
				DPRINT(("gethostaddr_gethostname: Nonrecoverable errors: FORMERR, REFUSED, NOTIMP.\n"));
				break;

			case WSANO_DATA:
				DPRINT(("gethostaddr_gethostname: Valid name, no data record of requested type.\n"));
				break;

			case WSAEINPROGRESS:
				DPRINT(("gethostaddr_gethostname: A blocking Windows Sockets operation is in progress.\n"));
				break;

			case WSAEINTR:
				DPRINT(("gethostaddr_gethostname: The (blocking) call was canceled using WSACancelBlockingCall.\n"));
				break;

			default:
				DPRINT(("gethostaddr_gethostname: Unknown winsock error\n"));
				break;
		}
		return (FALSE);
	}

	ptr = (unsigned char **) host->h_addr_list;
	if (!ptr)
	{
		DPRINT(("gethostaddr_gethostname: No addresses in host entry ?\n"));
		return (FALSE);
	}

	DPRINT(("gethostaddr_gethostname: Found the following address(es)\n"));

	found = FALSE;
	secondary = FALSE;

	while (*ptr)
	{
		TCPPEER peer;
		TCPPEER *broadcast;

		DPRINT(("gethostaddr_gethostname: %d.%d.%d.%d\n", (*ptr)[0], (*ptr)[1], (*ptr)[2], (*ptr)[3]));

		/*
		 * Have we found any addresses ?
		 */
		if (!found)
		{
			/*
			 * Copy the address accross
			 */
			memcpy(&peer.addr, *ptr, host->h_length);

			/*
			 * Use the port number from the broadcast handle
			 */
			broadcast = dcstFindValue(pTcp->handles, pTcp->broadcastHandle);
			if (broadcast)
			{
				peer.port = broadcast->port;
			}
			else
			{
				peer.port = 0;
			}

			/*
			 * Replace my handle
			 */
			pTcp->myHandle = dcstReplace(pTcp->handles, pTcp->myHandle, &peer);
			found = TRUE;
		}
		else
		{
			/*
			 * Have we found a secondary address ?
			 */
			if (!secondary)
			{
				TCPPEER2 peer2;

				/*
				 * Reset its status
				 */
				peer2.status = 0x00;

				/*
				 * Copy the address accross
				 */
				memcpy(&peer2.peer.addr, *ptr, host->h_length);

				/*
				 * Use the port number from the broadcast handle
				 */
				broadcast = dcstFindValue(pTcp->handles, pTcp->broadcastHandle);
				if (broadcast)
				{
					peer2.peer.port = broadcast->port;
				}
				else
				{
					peer2.peer.port = 0;
				}

				/*
				 * Replace my handle secondary handle
				 */
				pTcp->myHandle = dcstAddEx(pTcp->secondary, pTcp->myHandle, &peer2);
				secondary = TRUE;
			}
		}
		ptr++;
	}
	return (TRUE);
}


/*-------------------------------------------------------------------------
 Get the IP address of the host we're connected to.

 Two strategies are attempted.
 If found, the address is copied into pTcp->peers[TCP_HDL_ME].
 Return TRUE if the address is found, and FALSE otherwise.
-------------------------------------------------------------------------*/
static BOOL	gethostaddr(TCPINSTANCE *pTcp)	/* (modified) Current tcp instance */
{
	BOOL bAddressFound = FALSE;

	assert(pTcp != NULL);

	/*
	 * Try this method first since it will cause a dial-up connection
	 * to be established, even if it does not succeed...
	 */
	bAddressFound = gethostaddr_connect(pTcp);

	if (!bAddressFound)
	{
		/*
		 * Try the gethostname/gethostbyname method
		 */
		bAddressFound = gethostaddr_gethostname(pTcp);
	}

	if (!bAddressFound)
	{
		/*
		 * Try the ping method
		 */
		bAddressFound = gethostaddr_ping(pTcp);
	}

#if 0
	if (!bAddressFound)
	{
		/*
		 * Since an address has not been found then put in a dud one, the first
		 * packet we receive will inform dpio that our address is incorrect and fix
		 * the problem.
		 */
		TCPPEER peer;

		DPRINT(("gethostaddr: forced to use dud address for the time being\n"));
		peer.addr = inet_addr("127.0.0.0");
		peer.port = 0;
		pTcp->myHandle = dcstReplace(pTcp->handles, pTcp->myHandle, &peer);

		bAddressFound = TRUE;
	}
#endif

	return(bAddressFound);
}

/******************************************************************************
 Create a TCP driver.

 Note that only one of these may exist at a time per port number, since
 multiple ones would compete for the same source of rx packets.  Choosing
 TCP_SOCKET_ANY for the port is guaranteed to give an unused port.
******************************************************************************/
TCPINSTANCE *			/* TCP driver, or NULL for failure */
TCPWIN_Create(
	u_short maxHandles,	/* (input) Maximum number of allowed handles */
	u_short* port,		/* (modified) Port (socket) number, or TCP_SOCKET_ANY */
	int *status)		/* (output) status */
{
	TCPINSTANCE* pTcp;
	TCPPEER peer;
	SOCKADDR_IN sockAddr;
	u_short truePort;	/* in host format */
	int err;
	WSADATA wsa;

	DPRINT(("TCPWIN_Create() Called..\n"));

	assert(status != NULL);

	/*
	 * Fireup winsock :)
	 */
	err = WSAStartup(MAKEWORD(1, 1), &wsa);

	if (err)
	{
		DPRINT(("Could not initialize Winsock !\n"));
		switch (WSAGetLastError())
		{
			case WSASYSNOTREADY:
				DPRINT(("TCPWIN_Create: Indicates that the underlying network subsystem is not ready for network communication.\n"));
				break;
			case WSAVERNOTSUPPORTED:
				DPRINT(("TCPWIN_Create: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.\n"));
				break;
			case WSAEINPROGRESS:
				DPRINT(("TCPWIN_Create: A blocking Windows Sockets 1.1 operation is in progress.\n"));
				break;
			case WSAEPROCLIM:
				DPRINT(("TCPWIN_Create: Limit on the number of tasks supported by the Windows Sockets implementation has been reached.\n"));
				break;
			case WSAEFAULT:
				DPRINT(("TCPWIN_Create: The lpWSAData is not a valid pointer.\n"));
				break;
			default:
				DPRINT(("TCPWIN_Create: Unknown Winsock Error\n"));
				break;
		}
		*status = comm_STATUS_BAD;
	 	return NULL;
	}

	/*
	 * Print out what we found
	 */
	DPRINT(("TCPWIN_Create: Winsock Initialized\n"));
	DPRINT(("TCPWIN_Create: Version    : %d.%d [%d.%d]\n",
		HIBYTE(wsa.wVersion),
		LOBYTE(wsa.wVersion),
		HIBYTE(wsa.wHighVersion),
		LOBYTE(wsa.wHighVersion)));
	DPRINT(("TCPWIN_Create: Description: %s\n", wsa.szDescription));
	DPRINT(("TCPWIN_Create: Status     : %s\n", wsa.szSystemStatus));
	DPRINT(("TCPWIN_Create: MaxSockets : %d\n", wsa.iMaxSockets));
	DPRINT(("TCPWIN_Create: MaxUDPdg   : %d\n", wsa.iMaxUdpDg));
/*	DPRINT(("TCPWIN_CREATE: Vendor     : %s\n", wsa.lpVendorInfo)); */

	/* allocate and initialize an instance structure */
	pTcp = (TCPINSTANCE *) dp_MALLOC(sizeof(TCPINSTANCE));
	if (pTcp == NULL)
	{
		DPRINT(("TCPWIN_Create: Out of memory\n"));
		*status = comm_STATUS_MEMORY;
		return NULL;
	}
	memset(pTcp, 0, sizeof(TCPINSTANCE));

	/* create primary handles */
	pTcp->handles = dcstCreate(sizeof(TCPPEER), maxHandles, maxHandles/3);
	if (pTcp->handles == NULL)
	{
		DPRINT(("TCPWIN_Create: Could not create primary handles\n"));
		dp_FREE(pTcp);
		*status = comm_STATUS_MEMORY;
		return NULL;
	}

	/* create secondary set of handles */
	pTcp->secondary = dcstCreate(sizeof(TCPPEER2), maxHandles, maxHandles/3);
	if (pTcp->secondary == NULL)
	{
		DPRINT(("TCPWIN_Create: Could not create secondary handles\n"));
		dp_FREE(pTcp);
		*status = comm_STATUS_MEMORY;
		return NULL;
	}
	pTcp->myHandle = TCP_HDL_NONE;
	pTcp->broadcastHandle = TCP_HDL_NONE;

	/* Kluge to force broadcastHandle to have value 0 */
	pTcp->broadcastHandle = dcstReplace(pTcp->handles, pTcp->broadcastHandle, &peer);

	/* open a socket */
	pTcp->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (pTcp->socket == INVALID_SOCKET)
	{
		DPRINT(("TCPWIN_Create: Could not create socket\n"));

		switch (WSAGetLastError())
		{
			case WSANOTINITIALISED:
				DPRINT(("TCPWIN_Create: A successful WSAStartup must occur before using this API.\n"));
				break;

			case WSAENETDOWN:
				DPRINT(("TCPWIN_Create: The network subsystem or the associated service provider has failed.\n"));
				break;

			case WSAEAFNOSUPPORT:
				DPRINT(("TCPWIN_Create: The specified address family is not supported.\n"));
				break;

			case WSAEINPROGRESS:
				DPRINT(("TCPWIN_Create: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n"));
				break;

			case WSAEMFILE:
				DPRINT(("TCPWIN_Create: No more socket descriptors are available.\n"));
				break;

			case WSAENOBUFS:
				DPRINT(("TCPWIN_Create: No buffer space is available. The socket cannot be created.\n"));
				break;

			case WSAEPROTONOSUPPORT:
				DPRINT(("TCPWIN_Create: The specified protocol is not supported.\n"));
				break;

			case WSAEPROTOTYPE:
				DPRINT(("TCPWIN_Create: The specified protocol is the wrong type for this socket.\n"));
				break;

			case WSAESOCKTNOSUPPORT:
				DPRINT(("TCPWIN_Create: The specified socket type is not supported in this address family.\n"));
				break;

			default:
				DPRINT(("TCPWIN_Create: Unknown Winsock Error\n"));
				break;
		}

		dcstDestroy(pTcp->handles);
		dcstDestroy(pTcp->secondary);
		dp_FREE(pTcp);
		*status = comm_STATUS_BAD;
		return NULL;
	}

	/* enable broadcasting on the socket */
	{
		BOOL broadcast = TRUE;
		err = setsockopt(pTcp->socket, SOL_SOCKET, SO_BROADCAST,
					(char *) &broadcast, sizeof(broadcast));

		if (err == SOCKET_ERROR)
		{
	      DPRINT(("TCPWIN_Create: Could not set socket options\n"));
		}
	}

	/* bind the socket */
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	memset(&peer, 0, sizeof(peer));
	peer.addr = sockAddr.sin_addr.S_un.S_addr;

#define TCP_PORT_RANGE_START 21100
#define TCP_PORT_RANGE_LEN   200
	err = SOCKET_ERROR;
	if (*port == TCP_SOCKET_ANY) {
		/* If user specified 'any old port', try picking a random socket
		 * number inside our range.
		 */
		unsigned short randport = (eclock() % TCP_PORT_RANGE_LEN) + TCP_PORT_RANGE_START;
		sockAddr.sin_port = htons(randport);
		peer.port = sockAddr.sin_port;
		DPRINT(("TCPWIN_Create: Binding to %s:%d from range %d...%d\n", inet_ntoa(sockAddr.sin_addr), randport, TCP_PORT_RANGE_START, TCP_PORT_RANGE_START + TCP_PORT_RANGE_LEN-1));

		err = bind(pTcp->socket, (SOCKADDR *) &sockAddr, sizeof(sockAddr));
	}
	if (SOCKET_ERROR == err)  {
		/* If user specified anything else, or if our random port didn't
		 * work, go with what he specified.
		 */
		sockAddr.sin_port = htons(*port);
		peer.port = sockAddr.sin_port;

		DPRINT(("TCPWIN_Create: Binding to %s:%d\n", inet_ntoa(sockAddr.sin_addr), *port));

		err = bind(pTcp->socket, (SOCKADDR *) &sockAddr, sizeof(sockAddr));
	}
	if (SOCKET_ERROR == err)
	{
		DPRINT(("TCPWIN_Create: Could not bind socket\n"));
		dcstDestroy(pTcp->handles);
		dp_FREE(pTcp);
		*status = comm_STATUS_BAD;
		getsockerror();
		return(NULL);
	}

	/* determine the actual port number */
	if (TCP_SOCKET_ANY == *port)
	{
		int addrlen = sizeof(sockAddr);
		memset(&sockAddr, 0, sizeof(sockAddr));
		err = getsockname(pTcp->socket, (LPSOCKADDR)&sockAddr, (int FAR*)&addrlen);
		if (SOCKET_ERROR == err)
		{
			DPRINT(("TCPWIN_Create: could net get socket name\n"));
			err = WSAGetLastError();
	    }
		truePort = ntohs(sockAddr.sin_port);
		*port = truePort;
	}
	else
	{
		truePort = *port;
	}

	peer.port = htons(truePort);
	pTcp->myHandle = dcstReplace(pTcp->handles, pTcp->myHandle, &peer);

	/* Set up the broadcast peer */
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	sockAddr.sin_port = htons(truePort);
	memset(&peer, 0, sizeof(peer));
	peer.addr = INADDR_BROADCAST;
	peer.port = htons(truePort);
	pTcp->broadcastHandle = dcstReplace(pTcp->handles, pTcp->broadcastHandle, &peer);

	/* put the socket into non-blocking mode */
	{
		ULONG nonblocking = TRUE;
		if (ioctlsocket(pTcp->socket, FIONBIO, &nonblocking))
		{
			DPRINT(("TCPWIN_Create: error setting socket to non-blocking\n"));
			getsockerror();
		}
	}

	/* Store our address for future use */
	if (!gethostaddr(pTcp))
	{
		DPRINT(("TCPWIN_Create: could not get local address\n"));
		dcstDestroy(pTcp->handles);
		dp_FREE(pTcp);
		*status = comm_STATUS_BAD;
		getsockerror();
		return(NULL);
	}

	DPRINT(("TCPWIN_Create: success\n"));
	*status = comm_STATUS_OK;
	return (pTcp);
}

/*****************************************************************************
 Destroy a TCP driver.
*****************************************************************************/
void
TCPWIN_Destroy(
	TCPINSTANCE *pTcp)	/* (modified) Current TCP instance */
{
	assert(pTcp != NULL);
	DPRINT(("TCPWIN_Destroy() Called..\n"));
	closesocket(pTcp->socket);
	dcstDestroy(pTcp->handles);
	dp_FREE(pTcp);
	WSACleanup();
}

/*****************************************************************************
 Given a TCP peer address, return a handle that can be used to send a
 message to that address.

 Handles are integers starting at 0.  If a handle has been assigned to that
 address, return it.  If no handle has been assigned to the address, fewer
 than the maximum number of handles have been assigned, and insert is non-zero,
 assign one; otherwise return TCP_HDL_NONE.

 WARNING: Potential resource lock.  Original TCP driver had this comment
 about the handle allocation scheme:
	There is no way provided to close handles, and no particular
	need, since no one should be assigning the maximim number,
	and they only take up 16 bytes each.
 The maximum number of handles is determined by TCP_MAX_PEERS, in wstcp.h

 There is much additionaly functionality added by the second address.

 If BOTH addresses are supplied then the following occurs:
   If either the first or second address is matched in the primary handle
   list then that handle is returned.  If neither existed in the primaries
   then a new pair of handles is created (if the insert flag is set to TRUE)
   and the second address is marked as LIVE (if the live flag is set to TRUE).
   In the event that one of the address does match then the other supplied
   address is tested agains that other stored address (if there is one at all),
   and the stored address is replaced.  If the LIVE flag is set then this
   second address is marked as LIVE.

 If a only the first address is supplied then the following occurs:
   If the address appears in the primary handle list then a check is done
   to ensure that if the handle appears in the secondary handle list then
   it is marked as NON-LIVE.  If the address appears in the secondary
   handle list AND the secondary address is marked as LIVE then the
   primary and secondary addresses are EXCHANGED and the secondary is
   marked as NON-LIVE.

 Returns TCP_HDL_NONE on failure, and a valid TCPHANDLE otherwise.
*****************************************************************************/
TCPHANDLE				/* Corresponding handle */
TCPWIN_Address2Handle(
	TCPINSTANCE *tcp,	/* (modified) Current TCP instance */
	TCPPEER  *pAddr,	/* (input) Address to search for */
	TCPPEER  *pAddr2,	/* (input) Address2 to search for */
	BOOL insert,		/* (input) Whether or not to assign a new handle */
	BOOL live)			/* (input) Whether the second address is live */
{
	TCPHANDLE h;

	assert(tcp != NULL);
	assert(pAddr != NULL);
	assert((insert == TRUE) || (insert == FALSE));

	DPRINT(("TCPWIN_Address2Handle : "));
	if (insert)
	{
	    DPRINT(("canINS "));
	}

	if (live)
	{
		DPRINT(("LIVE "));
	}

	/* Have both addresses been supplied ? */
	if (pAddr && pAddr2)
	{
		DPRINT(("AddrX2 %s:%d ", inet_ntoa(*(struct in_addr *) &pAddr->addr), ntohs(pAddr->port)));
		DPRINT(("%s:%d ", inet_ntoa(*(struct in_addr *) &pAddr2->addr), ntohs(pAddr2->port)));

		/* Search handle set and return if found */
		h = dcstFindKey(tcp->handles, pAddr);

		/* Was the first address found ? */
		if (h != TCP_HDL_NONE)
		{
			TCPPEER2	*p2;

			DPRINT(("1st "));

			/*
			 * We have been given two addresses
			 * Is there a second address ?
			 */
			p2 = dcstFindValue(tcp->secondary, h);

			if (p2)
			{
				/*
				 * Compare the second address with this one
				 */
				if (memcmp(&p2->peer, pAddr2, sizeof (TCPPEER)))
				{
					TCPPEER2	peer2;

					/*
					 * They are different, replace it !
					 * Copy the status from the existing
					 */
					peer2.status = p2->status;
					peer2.peer = *pAddr2;

					DPRINT(("REPLACE 2nd "));

					if (p2->status & TCP_HDL2_LIVE)
					{
						DPRINT(("LIVE "));
					}

					/* replace it in the secondary handles */
					dcstReplaceEx(tcp->secondary, h, &peer2);
				}
			}
			else
			{
				/*
				 * Add the second address to the first
				 */
				TCPPEER2 peer2;

				DPRINT(("APPEND 2nd "));

				/*
				 * Create a TCPPEER2
				 * When adding a secondary address it gets marked as live
				 * (if flagged) until such time that it is marked non-live
				 */
				peer2.status = (unsigned char) ((live) ? TCP_HDL2_LIVE : 0);
				peer2.peer = *pAddr2;

				/* add it to the secondary handles */
				dcstAddEx(tcp->secondary, h, &peer2);
			}
		}

		/* If the handle was not found then try the second address if one was supplied */
		else
		{
			DPRINT(("n1st "));

			h = dcstFindKey(tcp->handles, pAddr2);

			/* Was the second address found ? */
			if (h != TCP_HDL_NONE)
			{
				TCPPEER2 *p2;

				DPRINT(("2nd "));

				/*
				 * Test to see if the first address is in the secondary position
				 */
				p2 = dcstFindValue(tcp->secondary, h);

				if (p2)
				{
					/*
					 * Compare the first address with this one
					 */
					if (memcmp(&p2->peer, pAddr, sizeof (TCPPEER)))
					{
						TCPPEER2	peer2;

						/*
						 * They are different, replace it !
						 * Copy the status from the existing
						 */
						peer2.status = p2->status;
						peer2.peer = *pAddr;

						DPRINT(("REPLACE 2nd "));

						if (p2->status & TCP_HDL2_LIVE)
						{
							DPRINT(("LIVE "));
						}

						/* replace the secondary handle */
						dcstReplaceEx(tcp->secondary, h, &peer2);
					}
				}
			}
			else
			{
				/* No handle was found  */
				DPRINT(("n2nd "));

				/* If allowed insert new */
				if (insert)
				{
					TCPPEER2 p2;

					DPRINT(("INS "));

					/* Insert first address into primary handles */
  					h = dcstAdd(tcp->handles, pAddr);

					/*
					 * Create a TCPPEER2
					 * When adding a secondary address it gets marked as live until
					 * such time that it is marked non-live
					 */
					p2.peer = *pAddr2;
					p2.status = (unsigned char) ((live) ? TCP_HDL2_LIVE : 0);

					/* add it to the secondary handles */
					dcstAddEx(tcp->secondary, h, &p2);
				}
			}
		}
	}
	else
	{
		/*
		 * A single address has been supplied
		 */
		DPRINT(("AddrX1 %s:%d ", inet_ntoa(*(struct in_addr *) &pAddr->addr), ntohs(pAddr->port)));

		/*
		 * Does the address appear in the primary handle list
		 */
		h = dcstFindKey(tcp->handles, pAddr);

		/*
		 * Did we find a handle ?
		 */
		if (h != TCP_HDL_NONE)
		{
			TCPPEER2 *p2;
			TCPPEER2 peer2;

			DPRINT(("1st "));

			/*
			 * Check to see if there is a second address for this handle
			 */
			p2 = dcstFindValue(tcp->secondary, h);

			/*
			 * Is the second address LIVE
			 */
			if (p2 && (p2->status & TCP_HDL2_LIVE))
			{
				DPRINT(("2nd LIVE "));

		        /*
				 * Mark as non live and replace the value
				 */
		        peer2.status = 0;
		        peer2.peer = p2->peer;
		        dcstReplaceEx(tcp->secondary, h, &peer2);
			}
		}
		else
		{
			/*
			 * Since we did not find the address in the primary handle list
			 * check to see if a LIVE address exists in the secondary list
			 */
			TCPPEER2 peer2;
			peer2.status = TCP_HDL2_LIVE;
			peer2.peer = *pAddr;

			DPRINT(("n1st "));
			h = dcstFindKey(tcp->secondary, &peer2);

			/*
			 * Did we find a handle ?
			 */
			if (h != TCP_HDL_NONE)
			{
				/*
				 * Since a secondary LIVE address has been found we must now
				 * exchange the primary and secondary address and mark the
				 * secondary address as NON-LIVE
				 */
				TCPPEER peer;
				TCPPEER *peerOut;

				DPRINT(("2nd LIVE XCHNG "));

				/* Copy the secondary to the primary */
				peer = peer2.peer;

				/* Get the primary */
				peerOut = dcstFindValue(tcp->handles, h);

				/* A primary address MUST exist */
				assert(peerOut);

				/* Copy the primary to the secondary */
				peer2.status = 0;
				peer2.peer = *peerOut;

				/* After the exchange replace them both */
				dcstReplaceEx(tcp->handles, h, &peer);
				dcstReplaceEx(tcp->secondary, h, &peer2);
			}
			else
			{
		        /* Did not find in second list */
		        DPRINT(("n2nd "));

				/* Check to see if there is a NON-LIVE version
				   in the second group of addresses */
				peer2.status = 0;
				peer2.peer = *pAddr;

				h = dcstFindKey(tcp->secondary, &peer2);

				/*
				 * Did we find a handle ?
				 */
				if (h != TCP_HDL_NONE)
				{
					DPRINT(("2nd-nonlive "));
				}
				else
				{
					/* Are we allowed to insert ? */
					if (insert)
					{
						DPRINT(("INS "));
						h = dcstAdd(tcp->handles, pAddr);
					}
				}
			}
		}
	}

	DPRINT((": %d\n", h));
	return (h);
}

/*****************************************************************************
 Given a handle, return the corresponding TCPPEER address.

 There may or may not be two addresses

 Returns TCP_RES_OK on success, TCP_RES_EMPTY on failure.
*****************************************************************************/
int						/* status */
TCPWIN_Handle2Address(
	TCPINSTANCE *tcp,	/* (input) Current TCP instance */
	TCPHANDLE handle,	/* (input) Handle to translate */
	TCPPEER   *pAddr,	/* (output) Corresponding address */
	TCPPEER2  *pAddr2)	/* (output) Secondary address (if it exists) */
{
	TCPPEER*  p;
	TCPPEER2* p2;

	assert(tcp != NULL);
	assert(pAddr != NULL);
	assert(pAddr2 != NULL);
	assert(handle != TCP_HDL_NONE);

	DPRINT(("TCPWIN_Handle2Address: "));

	p = ((TCPPEER *) dcstFindValue(tcp->handles, handle));

	/* If no address was found then return empty */
	if (NULL == p)
	{
		DPRINT(("not found\n"));
		return(TCP_RES_EMPTY);
	}

	DPRINT(("1st "));

	/* Copy the found address */
	*pAddr = *p;

	/* Attempt to find a secondary address */
	p2 = ((TCPPEER2 *) dcstFindValue(tcp->secondary, handle));

	/* If not found then mark as none otherwise copy it over */
	if (p2 == NULL)
	{
	    DPRINT(("n2nd "));
	    pAddr2->status = TCP_HDL2_NONE;
	}
	else
	{
	    DPRINT(("2nd "));
	    *pAddr2 = *p2;
	}

	DPRINT(("OK\n"));
	/* ok :) */
	return(TCP_RES_OK);
}

/*****************************************************************************
 Send a packet to the given destination.

 Copies len bytes from buf into internal packet queue.
 Must have previously gotten handle for destination by calling TCP_adr2hdl.
*****************************************************************************/
int						/* status */
TCPWIN_PutPacket(
	TCPINSTANCE *tcp,	/* (input) current TCP instance */
	void *bufptr,		/* (input) packet to send */
	ULONG len,			/* (input) length in bytes of packet to send */
	TCPHANDLE hdest)	/* (input) destination for packet */
{
	int cBytes;
	TCPPEER* pPeer;
	TCPPEER2 *pPeer2;
	SOCKADDR_IN sockAddr;

	assert(tcp != NULL);
	assert(bufptr != NULL);
	assert(len <= TCP_MAX_USERDATALEN);
	assert(hdest != TCP_HDL_NONE);

	/* do sanity checks */
	if ((len < 1) || (TCP_HDL_NONE == hdest) ||
		(len > TCP_MAX_USERDATALEN) || (NULL == bufptr))
		return TCP_RES_BAD;

	/* get ptr to peer address from peer handle */
	pPeer = (TCPPEER*)dcstFindValue(tcp->handles, hdest);
	if (NULL == pPeer)
	{
		return TCP_RES_BAD;	/* Sending to unused handle */
	}

	/* send the packet */
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = pPeer->port;
	sockAddr.sin_addr.s_addr = pPeer->addr;
	cBytes = sendto(tcp->socket, (char *) bufptr, (int) len, 0,
				(struct sockaddr *) &sockAddr, sizeof(sockAddr));
	if ((cBytes == 0) || (cBytes == SOCKET_ERROR)) {
		getsockerror();
		return(TCP_RES_FULL);
	}

	DPRINT(("TCPWIN_PutPacket: PKT %c%c Tx To %s:%d ",
		((char *) bufptr)[0], ((char *) bufptr)[1],
		 (char *) inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port)));

	/* get ptr to peer2 address from peer handle */
	pPeer2 = (TCPPEER2 *) dcstFindValue(tcp->secondary, hdest);

	/* if there is a second address and it is marked as live then
		send the packet to the second address */
	if (pPeer2 && (pPeer2->status == TCP_HDL2_LIVE))
	{
		/* only SYN && ACK && PING packets should be sent to multiple addresses
		   note that User packets are allowed through since they are sent unreliably */
#if 0
		assert(
			(*(char *)bufptr) != (dp_PACKET_INITIALBYTE) ||
			(*(dp_packetType_t *)bufptr) == (dpio_SYN_PACKET_ID) ||
			(*(dp_packetType_t *)bufptr) == (dpio_ACK_PACKET_ID) ||
			(*(dp_packetType_t *)bufptr) == (dp_PING_PACKET_ID));
#else
		/* Kludge: in the case where we establish a dpio connection
		 * to someone using only one address, and then much later openHdl2
		 * is called with a 2nd address, we will notice it here.
		 * The proper action is to turn off the 2nd address, I think.
		 *
		 * If this kludge doesn't work,
		 * this could be done instead at a higher level, in dpio_openHdl2,
		 * by testing whether a connection is established before
		 * creating a new handle, but that might require a flag to commSayHi
		 * to tell it whether to insert or not.
		 */
		if ((*(char *)bufptr) == (dp_PACKET_INITIALBYTE)
		&&  (*(dp_packetType_t *)bufptr) != (dpio_SYN_PACKET_ID)
		&&  (*(dp_packetType_t *)bufptr) != (dpio_ACK_PACKET_ID)
		&&  (*(dp_packetType_t *)bufptr) != (dp_PING_PACKET_ID)) {
			DPRINT(("TCPWIN_PutPacket: kludge: sent non-SYN; marking 2nd adr dead\n"));
			pPeer2->status &= ~TCP_HDL2_LIVE;
		} else {

#endif

		/* fire off the packet */
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = pPeer2->peer.port;
		sockAddr.sin_addr.s_addr = pPeer2->peer.addr;
		cBytes = sendto(tcp->socket, (char *) bufptr, (int) len, 0,	(struct sockaddr *) &sockAddr, sizeof(sockAddr));
		if ((cBytes == 0) || (cBytes == SOCKET_ERROR))
		{
			getsockerror();
			return(TCP_RES_FULL);
		}
		DPRINT(("and %s:%d ", (char *) inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port)));
#if 1
		}
#endif
	}

	DPRINT(("\n"));
	return(TCP_RES_OK);
}

/*****************************************************************************
 Receive a packet.
 Caller must put size of buffer in len before calling.
 Packets larger than buffer will be silently ignored.
 On failure, returns
  TCP_RES_BAD if *len looked bogus, or buf or srcHdl were NULL,
         or packet was too big to fit in buffer, or 0 size.
  TCP_RES_EMPTY if no good packet was availible.
 On success,
  returns TCP_RES_OK,
  sets *len to the packet length,
  sets *hsrc to the handle of the sender (or TCP_HDL_NONE if one
    hasn't been assigned yet with TCPWIN_Address2Handle), and
  sets *srcAdr to the netware address of the sender if srcAdr is not NULL.

 To reply if source was TCP_HDL_NONE, you must first call
 TCPWIN_Address2Handle to turn the source's address into a handle.
 (TCP_get could do this for you, but then the peer table might fill up
 with all sorts of garbage addresses from hosts who sent you junk mail.)
*****************************************************************************/
int						/* status */
TCPWIN_GetPacket(
	TCPINSTANCE *tcp,	/* (input) Current TCP instance */
	void *bufptr,		/* (output) Contents of packet */
	ULONG *pLen,		/* (modified) size of bufptr/number of bytes in packet*/
	TCPHANDLE *hsrc,	/* (output) Handle of sender */
	TCPPEER *srcaddr)	/* (output) Address of sender */
{
	int addrlen;
	ULONG cBytes;
	TCPPEER addr;
	SOCKADDR_IN sockAddr;

	/* do sanity checks */
	assert(tcp != NULL);
	assert(bufptr != NULL);
	assert(pLen != NULL);
	assert(*pLen < 10*TCP_MAX_USERDATALEN);
	assert(hsrc != NULL);
	if ((*pLen < 1) || (*pLen > 10*TCP_MAX_USERDATALEN) ||
				(bufptr == NULL) || (hsrc == NULL)) {
		DPRINT(("TCPWIN_GetPacket(): Sanity Check Failed!\n"));
		return(TCP_RES_BAD);
	}

	/* read packet */
	addrlen = sizeof(sockAddr);
	cBytes = recvfrom(tcp->socket, (char *) bufptr, (int) *pLen, 0,
				(struct sockaddr *) &sockAddr, &addrlen);
	if ((cBytes == 0) || (cBytes == SOCKET_ERROR))
		return(TCP_RES_EMPTY);
	if ((cBytes > TCP_MAX_USERDATALEN) || (cBytes > *pLen))
		return(TCP_RES_BAD);

	/* set return info */
	addr.addr = sockAddr.sin_addr.s_addr;
	addr.port = sockAddr.sin_port;

	DPRINT(("TCPWIN_GetPacket(): Rx From %s:%d\n", (char *) inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port)));
	*pLen = cBytes;

	/*
	 * If the packet is a SYN packet and it is from an address which is
	 * different to the embedded address then we will attempt to translate
	 * the embeded address into a handle which will allow the address system
	 * to ADD the address it is from.  The point of this whole exersise is
	 * so that when the source address is resolved by an upper layer then
	 * it will get the correct handle.
	 */
	{
		unsigned char	*pkt = bufptr;

		if (((*(dp_packetType_t *)bufptr) == (dpio_SYN_PACKET_ID)) && memcmp(&addr, pkt + 7, sizeof (TCPPEER)))
		{
			TCPPEER	peer;

			peer = *(TCPPEER *) (pkt + 7);

			DPRINT(("TCPWIN_GetPacket(): Got SYN with mismatched addresses\n"));
			DPRINT(("TCPWIN_GetPacket(): %08x:%d %08x:%d\n",
				ntohl(addr.addr), ntohs(addr.port), ntohl(peer.addr), ntohs(peer.port) ));

			*hsrc = TCPWIN_Address2Handle(tcp, &peer, &addr, FALSE, FALSE);
		}
		else
		{
			*hsrc = TCPWIN_Address2Handle(tcp, &addr, NULL, FALSE, FALSE);
		}
	}

	if (srcaddr != NULL)
	{
		*srcaddr = addr;
	}

	return(TCP_RES_OK);
}
