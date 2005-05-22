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

/*************************************************************************************

                           IPX Protocol Driver Functions For Win32

                              (c) 1996 By Activision

  Revision Log:
  $Log: ipx2win.c $
  Revision 1.6  1997/06/24 18:55:30  dkegel
  Return dp_RES_FULL if buffer too small on receive.
  Revision 1.5  1997/03/07 19:52:49  dkegel
  Use dpmem.h's dp_MALLOC etc. instead of malloc().
  Revision 1.4  1997/02/01 00:46:16  dkegel
  1. Don't check so nastily that caller's buffer can hold a bigger packet
  than we can on receive!  (Problem inherited from DOS driver.)
  2. Tidied up DPRINT's, more informative now.
  Revision 1.3  1997/01/31 07:40:38  dkegel
  Now give port to winsock in network order, like it wants it.
  Communicates with DOS version now.
  Revision 1.2  1997/01/31 02:37:58  dkegel
  Converted to ddprint.h
  22-OCT-96  File Created [MSL]
**************************************************************************************/

/*************************************************************************************

                                    Literals

**************************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dpmem.h"


#include "ipx2win.h"


#define DEBUG_MODULE 1
#include "ddprint.h"


/*************************************************************************************

                                    Globals

**************************************************************************************/



/*************************************************************************************

                                 getsockerror()

**************************************************************************************/

static int getsockerror(void) 

{
  int err;
  
  err = WSAGetLastError();

  DPRINT(("Winsock error: %i\n", (int) err));

  return(err);
}


/*************************************************************************************

                                  printaddr()

**************************************************************************************/

void printaddr(IPXPEER *peer, char *string)

{
  dp_dprintf("%s: %u.%u.%u.%u,%X.%X.%X.%X.%X.%X\n",
              string,
              (int) peer->sa_netnum[0]  & 0xff,
              (int) peer->sa_netnum[1]  & 0xff,
              (int) peer->sa_netnum[2]  & 0xff,
              (int) peer->sa_netnum[3]  & 0xff,
              (int) peer->sa_nodenum[0] & 0xff,
              (int) peer->sa_nodenum[1] & 0xff,
              (int) peer->sa_nodenum[2] & 0xff,
              (int) peer->sa_nodenum[3] & 0xff,
              (int) peer->sa_nodenum[4] & 0xff,
              (int) peer->sa_nodenum[5] & 0xff);
}                   


/*************************************************************************************

                                  gethostaddr()

**************************************************************************************/

static BOOL gethostaddr(IPXINSTANCE *ipx)

{
  ULONG      uniqueID;
  IPXPEER *  peerptr;
  ULONG      nobytes;
  int        addrlen;
  IPXPEER    addr;
  clock_t    waittime;
  ULONG      result;




  // Note: This code may seem a long way of getting the host IP address, but its 
  // necessary, as getsockname() will always return INADDR_ANY on UDP sockets, and
  // the gethostname() / gethostbyname() technique will check for a DNS server if
  // the user has a modem setup (it causes the dialer to come up).  Why this
  // has to be so diffucult is a mystery to me...


  // Broadcast a packet containing a unique ID
  uniqueID = (ULONG) clock();
  peerptr = &ipx->peers[ipx2_HDL_BROADCAST];
  nobytes = sendto(ipx->socket, (char *) &uniqueID, 4, 0, (struct sockaddr *) peerptr, sizeof(*peerptr));
  if ((nobytes == 0) || (nobytes == SOCKET_ERROR))  {
	  getsockerror();
	  return(0);
  }


  // wait for packet to come back
  waittime = clock() + CLOCKS_PER_SEC * 5;
  while (clock() < waittime)
  {
    addrlen = sizeof(addr);
    nobytes = recvfrom(ipx->socket, (char *) &result, 4, 0, (struct sockaddr *) &addr, &addrlen);
    if ((nobytes != 0) && (nobytes != SOCKET_ERROR) && (result == uniqueID))
    { // received our broadcast packet
      ipx->peers[ipx2_HDL_ME] = addr;

      printaddr(&addr, "Host Address: ");

      return(1);
    }
  }

  return(0);
}


/*************************************************************************************

                                  IPXWIN_Create()

**************************************************************************************/

// Create an IPX driver.
// Returns NULL on failure.
// Parameter is socket number (analogous to UDP/IP port number) to
// use for all traffic.
// Only one of these may exist at a time, since multiple ones would
// compete for the same source of rx packets.
// Port is in host byte order.

IPXINSTANCE * IPXWIN_Create(int port, int *status)

{
  int           i;
  IPXINSTANCE * ipxptr;
  int           err;
//  int           len;
  IPXPEER *     peerptr;
  BOOL          broadcast;


  DPRINT(("IPXWIN_Create() Called..\n"));


  // allocate an instance structure
  ipxptr = (IPXINSTANCE *) dp_MALLOC(sizeof(IPXINSTANCE));
  if (!ipxptr)  {*status = 100;  return NULL;}

  // init the instance structure
  memset(ipxptr, 0, sizeof(IPXINSTANCE));
  ipxptr->nexthandle = ipx2_HDL_ME + 1;

  // open an IPX socket
  ipxptr->socket = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
  if (ipxptr->socket == INVALID_SOCKET)  {
	  dp_FREE(ipxptr);  *status = 130; getsockerror(); return(NULL);
  }

  // enable broadcasting on the socket
  broadcast = TRUE;
  setsockopt(ipxptr->socket, SOL_SOCKET, SO_BROADCAST, (char *) &broadcast, sizeof(broadcast));

  // bind the socket
  memset(&ipxptr->peers[ipx2_HDL_ME], 0, sizeof(ipxptr->peers[ipx2_HDL_ME]));
  ipxptr->peers[ipx2_HDL_ME].sa_family = AF_IPX;
  ipxptr->peers[ipx2_HDL_ME].sa_socket = htons(port);
  err = bind(ipxptr->socket, (SOCKADDR *) &ipxptr->peers[ipx2_HDL_ME], sizeof(ipxptr->peers[ipx2_HDL_ME]));
  if (err) {
	  dp_FREE(ipxptr);  *status = 131;  getsockerror(); return(NULL);
  }

//  // put the bound socket address in the "ME" peer handle data
//  len = sizeof(ipxptr->peers[ipx2_HDL_ME]);
//  err = getsockname(ipxptr->socket, (SOCKADDR *) &ipxptr->peers[ipx2_HDL_ME], &len);
//  if (err) {dp_FREE(ipxptr);  *status = 132;  getsockerror(); return(NULL);}
//  printaddr(&ipxptr->peers[ipx2_HDL_ME], "Host Address: ");

  // setup the broadcast peer
  peerptr = &ipxptr->peers[ipx2_HDL_BROADCAST];
  memset(peerptr, 0, sizeof(*peerptr));
  peerptr->sa_family = AF_IPX;
  peerptr->sa_socket = htons(port);
//  for (i = 0; i < 4; i++)  peerptr->sa_netnum[i]  = (char) 0xFF;
  for (i = 0; i < 6; i++)  peerptr->sa_nodenum[i] = (char) 0xFF;

  // put the socket into non-blocking mode
  {
    ULONG nonblocking = TRUE;
    if (ioctlsocket(ipxptr->socket, FIONBIO, &nonblocking)) getsockerror();  
  }

//  if (!gethostaddr(ipxptr)) getsockerror();
  if (!gethostaddr(ipxptr)) {dp_FREE(ipxptr);  *status = 132;  getsockerror(); return(NULL);}

  return(ipxptr);
}


/*************************************************************************************

                                  IPXWIN_Destroy()

**************************************************************************************/

// Destroy an IPX driver.

void IPXWIN_Destroy(IPXINSTANCE *ipxptr)

{
  DPRINT(("IPXWIN_Destroy() Called..\n"));

  closesocket(ipxptr->socket);

  dp_FREE(ipxptr);
}


/*************************************************************************************

                            IPXWIN_Address2Handle()

**************************************************************************************/

// Given an IPX peer address, return a handle that can be used to send an
// address to that address.  Handles are integers starting at 0.
// If no handle exists for that address,
//   If insert is non-zero,
//    allocate one.
//   else
//    return ipx2_HDL_NONE
// Multiple calls with the same arg will return the same result.
// There is no upper bound on handles.
// (There is no way provided to close handles, and no particular need,
// since nobody will be creating more than 100 or so, and they only take up
// 16 bytes each.)
// Returns ipx2_HDL_NONE on failure.

IPXHANDLE IPXWIN_Address2Handle(IPXINSTANCE *ipx, IPXPEER *addr, int insert)

{
  IPXHANDLE h;

  //printaddr(addr, "Address2Handle: ");

  // Search peer table and return handle if found
  for (h = 0; h < ipx->nexthandle; h++)  
  {
    if (  (memcmp(ipx->peers[h].sa_netnum, addr->sa_netnum, sizeof(addr->sa_netnum)) == 0) 
         &&
          (memcmp(ipx->peers[h].sa_nodenum, addr->sa_nodenum, sizeof(addr->sa_nodenum)) == 0) )
    {
	  //DPRINT(("Address2Handle: got %d\n", h));
      return(h);
    }
  }

  // handle not found: add it to peer table, if desired
  if ((!insert) || (ipx->nexthandle >= MAX_PEERS))
	return(ipx2_HDL_NONE);
  ipx->peers[ipx->nexthandle] = addr[0];
  DPRINT(("Address2Handle: created new handle %d == %d\n", h, ipx->nexthandle));
  ipx->nexthandle++;

  return(h);
}


/*************************************************************************************

                            IPXWIN_Handle2Address()

**************************************************************************************/

// Given a handle, return the corresponding IPX address.
// Returns ipx2_RES_EMPTY on failure.

int IPXWIN_Handle2Address(IPXINSTANCE *ipx, IPXHANDLE handle, IPXPEER *addr)

{
  addr[0] = ipx->peers[handle];

  return(ipx2_RES_OK);
}


/*************************************************************************************

                                  IPXWIN_PutPacket()

**************************************************************************************/

// Send a packet via IPX to the given destination.
// Copies len bytes from buf into internal packet queue.
// Must have previously gotten handle for destination by calling ipx2_adr2hdl.

int IPXWIN_PutPacket(IPXINSTANCE *ipx, void *bufptr, ULONG len, IPXHANDLE hdest)

{
  int        nobytes;
  IPXPEER *  peerptr;


//  DPRINT(("IPXWIN_PutPacket() Called..\n"));

  // do sanity checks
  if (len < 1) {
	  DPRINT(("IPXWIN_PutPacket: len:%d < 1\n", len));
	  return ipx2_RES_BAD;
  } else if (hdest >= ipx->nexthandle) {
	  DPRINT(("IPXWIN_PutPacket: hdest:%d >= nexth:%d\n", hdest, ipx->nexthandle));
	  return ipx2_RES_BAD;
  } else if (len > IPX2_MAX_USERDATALEN) {
	  DPRINT(("IPXWIN_PutPacket: len:%d > max:%d\n", len, IPX2_MAX_USERDATALEN));
	  return ipx2_RES_BAD;
  } else if (!bufptr) {
	  DPRINT(("IPXWIN_PutPacket: null bufptr\n"));
	  return ipx2_RES_BAD;
  }

  // get ptr to peer address from peer handle
  peerptr = &ipx->peers[hdest];

  //printaddr(peerptr, "IPXWIN_PutPacket(): ");

  // send the packet
  nobytes = sendto(ipx->socket, (char *) bufptr, (int) len, 0, (struct sockaddr *) peerptr, sizeof(SOCKADDR_IPX));
  if ((nobytes == 0) || (nobytes == SOCKET_ERROR))  {
	  getsockerror();
	  return(ipx2_RES_FULL);
  }

  return(ipx2_RES_OK);
}


/*************************************************************************************

                                  IPXWIN_GetPacket()

**************************************************************************************/

// Receive a packet via IPX.
// Caller must put size of buffer in len before calling.
// Packets larger than buffer will be silently ignored.
// On failure, returns
//  ipx2_RES_BAD if *len looked bogus, or buf or srcHdl were NULL,
//         or packet was too big to fit in buffer, or 0 size.
//  ipx2_RES_EMPTY if no good packet was availible.
// On success,
//  returns ipx2_RES_OK,
//  sets *len to the packet length,
//  sets *hsrc to the handle of the sender (or ipx2_HDL_NONE if one
//    hasn't been assigned yet with ipx2_adr2hdl), and
//  sets *srcAdr to the netware address of the sender if srcAdr is not NULL.
//
// To reply if source was ipx2_HDL_NONE, you must first call ipx2_adr2hdl
// to turn the source's address into a handle.
// (ipx2_get could do this for you, but then the peer table might fill up
// with all sorts of garbage addresses from hosts who sent you junk mail.)

int IPXWIN_GetPacket(IPXINSTANCE *ipx, void *bufptr, ULONG *plen, IPXHANDLE *hsrc, IPXPEER *srcaddr)

{
  int     addrlen;
  ULONG   nobytes;
  IPXPEER addr;
  int origlen = *plen;


//  DPRINT(("IPXWIN_GetPacket() Called..\n"));

  // do sanity checks.  Only complain if buffer length looks really crazy.
  if ((origlen < 1) || (origlen > IPX2_MAX_USERDATALEN * 10) || (!bufptr) || (!hsrc)) {
	DPRINT(("IPXWIN_GetPacket: insane: *plen %d, bufptr %p, hsrc %p.\n",
	  origlen, bufptr, hsrc));
	return(ipx2_RES_BAD);
  }

  // read next packet
  addrlen = sizeof(SOCKADDR_IPX);
  nobytes = recvfrom(ipx->socket, (char *) bufptr, (int) *plen, 0, (struct sockaddr *) &addr, &addrlen);
  if ((nobytes == 0) || (nobytes == SOCKET_ERROR))  {
	if (nobytes == SOCKET_ERROR) {
		int lastErr = WSAGetLastError();
		if (lastErr == WSAEWOULDBLOCK)
			return ipx2_RES_EMPTY;

		DPRINT(("IPXWIN_GetPacket: Winsock error: %i\n", lastErr));
		if (lastErr == WSAEMSGSIZE) {
			DPRINT(("IPXWIN_GetPacket: buflen was %d, got WSAEMSGSIZE\n", origlen));
			return ipx2_RES_FULL;
		}
	}
	return ipx2_RES_EMPTY;
  }

  if ((nobytes < 1) || (nobytes > IPX2_MAX_USERDATALEN) || (nobytes > *plen)) {
	DPRINT(("IPXWIN_GetPacket: pkt -1 or too big for buffer.\n"));
	return(ipx2_RES_BAD);
  }

  // setup return info
  plen[0] = nobytes;
  hsrc[0] = IPXWIN_Address2Handle(ipx, &addr, 0);
  if (srcaddr)  srcaddr[0] = addr;

  //printaddr(&addr, "IPXWIN_GetPacket(): ");
  return(ipx2_RES_OK);
}


