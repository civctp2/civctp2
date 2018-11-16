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

/*---------------------------------------------------------------------------
 Adpater layer to convert Unix Berkeley sockets into a library suitable
 for use with ActiveNet.

 This version was created from uudp.c, which uses the ip address
 directly as a handle, and consequently cannot allow use of
 multiple ports.  This version assigns handles and does the
 appropriate matching.

 $Log: uudps.c $
 Revision 1.8  1997/09/10 21:35:31  lbennett
 Fixed handle translation in receive.
 Revision 1.7  1997/09/04 08:16:53  dkegel
 commScanAddr now defaults to right port number, just like winets.
 Revision 1.6  1997/09/02 19:30:08  dkegel
 Handle ping packets before handle lookup
 Revision 1.5  1997/09/02 18:19:14  dkegel
 Use one higher port than uudp so we can run without clashing,
 at least for the moment.
 Revision 1.4  1997/09/01 05:36:22  lbennett
 Added low-level ping response code.
 Revision 1.3  1997/09/01 04:07:18  lbennett
 Added support for received packet's address field (iff unknown handle).
 Revision 1.2  1997/08/31 22:57:14  lbennett
 Pack struct, add debugging support.
 Revision 1.1  1997/08/30 01:07:43  lbennett
 Initial revision
---------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "commapi.h"

#include "dictset.h"

#include "dp.h"

#include "dpmem.h"

/* enable snoopy packet logging in all but normal builds */
#include "logpkt.h"

#define DEBUG_MODULE	!FALSE
/* #define DPRINT(s) printf s */

/**
* Constants
*/

/* Magic Number for comm structure */
#define COMM_MAGIC 		0x5136

/* Largest number of active players we can support at one time */
#define MAX_HANDLES		2000

/* Length of a host name string */
#define HOSTNAMELEN		256

/* Special values for socket numbers */
#define UUDP_SOCKET_ANY		0
#define UUDP_SOCKET_DEFAULT	(0x52A1+commapi_NET_PORTOFFSET+1) /* +1 for uudps */

/* Error return from sockets methods */
#ifndef SOCKET_ERROR
	#define SOCKET_ERROR -1
#endif

/* Useful constants */
#ifndef TRUE
    #define TRUE        (1)
    #define FALSE       (0)
#endif

/* times() is a MUCH better timer than clock() */
#ifdef UNIX
	#define clock() times(NULL)
#endif

/**
* Types
*/

/* sockets-defined structures */
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

/* Holds an IP address and port */
typedef struct addr_s {
	in_addr_t addr PACK;	/* in network order */
	in_port_t port PACK;	/* in network order */
} addr_t;

/* Second address of a peer (can assume it's his private, internal adr) */
typedef struct uudps_peer2_s {
	addr_t peer;  /* address; must be first field */
} uudps_peer2_t;

/* Handle type */
typedef dcst_key_t handle_t;

/**
* Module data
*/
typedef struct {
	unsigned long	magic;				/* magic number used to validate the structure */
	int 			uudp_sock;			/* handle to driver socket */
	dcst_t 			uudp_handles;		/* addr_t; player handles */
	dcst_t 			uudp_secondary;		/* uudps_peer2_t; 2nd adr for each h */
	handle_t 		uudp_myHdl;			/* my handle */
	handle_t 		uudp_broadcastHdl;	/* broadcast handle */
	int		 		uudp_rxWait;		/* should commRx wait when checking for packets */
	addr_t 			adr;				/* kludge city, commPlayerInfo returns a pointer to this */
	uudps_peer2_t	adr2;				/* kludge city, take 2 */
	commSessInfo_t *sessinfo;			/* info sent to pinging hosts; pointer set using commSetParam by server.c */

#ifdef LOGPKT_ENABLED
	FILE 			*logpkt_fp;			/* file for packet logging */
#endif
} comm_t;


/**
* Macros
*/

/* Get the handle corresponding to the address */
#define getHandleFromAddress(d,a)	dcstFindKey((d),(a))

/* Get the address corresponding to the handle */
#define getAddressFromHandle(d,h)	((addr_t*)dcstFindValue((d),(h)))

/**
* Methods
*/

/*-------------------------------------------------------------------------
 DPRINT an ip address as ascii string (eg. 255.255.255.255)
-------------------------------------------------------------------------*/
static void printAdr(addr_t* adr)
{
	DPRINT(("%d.%d.%d.%d:%d",
		((unsigned char *)&(adr->addr))[0] & 0xFF,
		((unsigned char *)&(adr->addr))[1] & 0xFF,
		((unsigned char *)&(adr->addr))[2] & 0xFF,
		((unsigned char *)&(adr->addr))[3] & 0xFF,
		adr->port));
	(void) adr;
}

/*-------------------------------------------------------------------------
 Validate a comm_t pointer
-------------------------------------------------------------------------*/
int validateComm(void *_comm)
{
	comm_t *comm = (comm_t *)_comm;
/*	DPRINT(("validateComm: comm:%p magic:%x\n", comm, comm?comm->magic:0)); */
	return (comm && (comm->magic == COMM_MAGIC));
}

/*-------------------------------------------------------------------------
 ** do nothing **
 Returns TRUE in all cases.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commNoOp(
	commNoOpReq_t* req __attribute__((unused)),
	commNoOpResp_t* resp,	/* (output) status */
	void *commPtr)
{
	commNoOpResp_t	respDummy;

	DPRINT(("commNoOp: uudps\n"));

	/* Protect against invalid arguments */
	assert(validateComm((comm_t *)commPtr));
	if (NULL == resp)
		resp = &respDummy;

	/* Return status */
	resp->status = comm_STATUS_OK;
	return TRUE;
}

/*--------------------------------------------------------------------------
 Return this host's ip address.
 May block.

 Note:
 The upper layer likes to embed our IP address inside packets, and
 needs that embedded address to match the sender's address when the packet
 arrives.
 This means we don't currently support firewalls that do IP address
 translation!
 Also, if this machine has multiple IP addresses, we have a problem, because
 we can't easily determine which address is the one that will be slapped
 on packets sent to other players.
 Under Unix, we work around this by reading the 'right' ip address from
 the environment variable UUDP_HOSTNAME.  When running a game server,
 this variable needs to be set manually in the shell script that invokes
 the game server.  This needs to be documented better!
 (We'll need a real fix for this later.  Probably we can ping a faraway
 host and have it tell us the address it saw on our packet.)

 Returns TRUE on success, FALSE on failure.
--------------------------------------------------------------------------*/
static int				/* success boolean */
gethostaddr(
	addr_t* address)	/*(output) our address */
{
	char *me;

	/* Check parameters */
	assert(NULL != address);

	/* Check the environment, and then the system's data */
	if ((me = (char *) getenv("UUDP_HOSTNAME"))) {
		address->addr = inet_addr(me);
	} else {
		char my_name[HOSTNAMELEN];
		int err;
		err = gethostname(my_name, HOSTNAMELEN);
		if(err != SOCKET_ERROR) {
			struct hostent *hp = gethostbyname(my_name);
			if (NULL != hp) {
				memcpy(&(address->addr), hp->h_addr_list[0], hp->h_length);
			} else {
				DPRINT(("gethostaddr: gethostbyname %s fails; errno %d\n", my_name, errno));
				return FALSE;
			}
		}
	}
	return TRUE;
}


/*-------------------------------------------------------------------------
 Allocate a comm structure and initialize it
-------------------------------------------------------------------------*/
int cdecl commAlloc(commAllocReq_t *req, commAllocResp_t *resp)
{
	comm_t	*comm;
	commAllocResp_t respDummy;

	/*
	 * protect against NULL arguments
	 */
	assert(NULL != req);

	if (NULL == resp)
	{
		resp = &respDummy;
	}

   	DPRINT(("commAlloc: allocating %d bytes for comm_t\n", sizeof (comm_t)));

	comm = dp_MALLOC(sizeof (comm_t));

	if (!comm)
	{
	   	DPRINT(("commAlloc: failed to allocate memory\n"));
		resp->status = comm_STATUS_MEMORY;
		return (FALSE);
	}

	/*
	 * Initialize the structure
	 */
	DPRINT(("commAlloc: Initializing structure\n"));

   	comm->magic = COMM_MAGIC;
	comm->uudp_sock = SOCKET_ERROR;
	comm->uudp_handles = NULL;
	comm->uudp_secondary = NULL;
	comm->uudp_myHdl = dcst_INVALID_KEY;
	comm->uudp_broadcastHdl = dcst_INVALID_KEY;
	comm->uudp_rxWait = 1;
	comm->sessinfo = NULL;

#ifdef LOGPKT_ENABLED
	comm->logpkt_fp = NULL;
#endif

	/*
	 * Store the pointer
	 */
	*req->ptr = comm;
	resp->status = comm_STATUS_OK;

   	assert(validateComm(*req->ptr));

	return (TRUE);
}


/*-------------------------------------------------------------------------
 Initialize the communications layer.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commInit(
	commInitReq_t* req,		/* (input) port */
	commInitResp_t* resp,	/* (output) status */
	void *commPtr)				/* comm context */
{
	sockaddr_in sockAddr;
	commInitResp_t respDummy;
	addr_t addr;
	u_short port;	/* in host byte ordering */
	comm_t	*comm = (comm_t *) commPtr;

	DPRINT(("commInit: uudps\n"));

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(NULL != req);
	if (NULL == resp)
		resp = &respDummy;
	if(NULL == req) {
		DPRINT(("commInit: NULL req\n"));
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Set up handles dictionary */
	comm->uudp_handles = dcstCreate(sizeof(addr_t), MAX_HANDLES, MAX_HANDLES/3);
	if(NULL == comm->uudp_handles) {
		DPRINT(("commInit: Unable to allocate dict.\n"));
		resp->status = comm_STATUS_MEMORY;
		return FALSE;
	}
	comm->uudp_secondary = dcstCreate(sizeof(uudps_peer2_t), MAX_HANDLES, MAX_HANDLES/3);
	if(NULL == comm->uudp_secondary) {
		dcstDestroy(comm->uudp_handles);
		DPRINT(("commInit: Unable to allocate dict2.\n"));
		resp->status = comm_STATUS_MEMORY;
		return FALSE;
	}

	/* Get requested port number */
	port = req->portnum;
#if 0
	if(0 == port)		/* For backwards compatibility */
		port = UUDP_SOCKET_DEFAULT;
	else if(comm_PORT_ANY == port)
#endif
	if(comm_PORT_ANY == port)
		port = UUDP_SOCKET_ANY;

	/* create the datagram socket */
	comm->uudp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (comm->uudp_sock < 0) {
		DPRINT(("commInit: socket < 0\n"));
		dcstDestroy(comm->uudp_handles);
		dcstDestroy(comm->uudp_secondary);
		resp->status = comm_STATUS_FULL;
		return FALSE;
	}

	/* Enable broadcast on that socket */
	{
		int allow_broadcast = TRUE;
		setsockopt(comm->uudp_sock, SOL_SOCKET, SO_BROADCAST,
				(char *) &allow_broadcast, sizeof(allow_broadcast));
	}

	/* Set the socket to non-blocking if nowait mode is enabled */
	{
		int flags;
		flags = fcntl(comm->uudp_sock, F_GETFL, 0);
		flags |= O_NONBLOCK;
		fcntl(comm->uudp_sock, F_SETFL, flags);
	}

	/* Set up a sockaddr to listen on port */
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(port);
	if (bind(comm->uudp_sock, (const sockaddr *)&sockAddr, sizeof(sockAddr))) {
		DPRINT(("commInit: bind failure\n"));
		dcstDestroy(comm->uudp_handles);
		dcstDestroy(comm->uudp_secondary);
		close(comm->uudp_sock);
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* determine the actual port number */
	if(UUDP_SOCKET_ANY == port) {
		int addrlen = sizeof(sockAddr);
		int err;
		memset(&sockAddr, 0, sizeof(sockAddr));
		err = getsockname(comm->uudp_sock, (sockaddr*)&sockAddr, (socklen_t *)&addrlen);
		if(SOCKET_ERROR == err)
			err = errno;
		else
			port = ntohs(sockAddr.sin_port);
	}

	/* Set up packet logging */
#ifdef LOGPKT_ENABLED
	comm->logpkt_fp = logPkt_open();
#endif

	/* Assign and save handle for broadcast address */
	addr.addr = -1;
	addr.port = htons(port);
	comm->uudp_broadcastHdl = dcstReplace(comm->uudp_handles, comm->uudp_broadcastHdl, &addr);

	/* Assign and save handle for our address */
	if (!gethostaddr(&addr)) {
		DPRINT(("commInit: gethostaddr fail\n"));
		close(comm->uudp_sock);
		dcstDestroy(comm->uudp_handles);
		dcstDestroy(comm->uudp_secondary);
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	addr.port = htons(port);
	comm->uudp_myHdl = dcstReplace(comm->uudp_handles, comm->uudp_myHdl, &addr);

	/* Postconditions */
	assert(SOCKET_ERROR != comm->uudp_sock);
	assert(NULL != comm->uudp_handles);
	assert(dcst_INVALID_KEY != comm->uudp_myHdl);
	assert(dcst_INVALID_KEY != comm->uudp_broadcastHdl);

	/* Return status */
	resp->status = comm_STATUS_OK;
	return TRUE;
}

/*-------------------------------------------------------------------------
 Remove the communications layer.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commTerm(
	commTermReq_t* req,		/* ignored */
	commTermResp_t* resp,	/* (output) status */
	void *commPtr)			/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;

	DPRINT(("commTerm: uudps\n"));
	(void) req;

	assert(validateComm(comm));

	/* Stop packet logging */
	logPkt_close(comm->logpkt_fp);

	/* Clean up */
	close(comm->uudp_sock);
	comm->uudp_sock = -1;
	dcstDestroy(comm->uudp_handles);
	dcstDestroy(comm->uudp_secondary);
	memset(comm, 0x00, sizeof (comm_t));
	free(comm);

	/* Return status */
	if (resp != NULL)
		resp->status = comm_STATUS_OK;
	return TRUE;
}

/**
* publicly readable data for commDriverInfo().  This is publicly
* readable so that ~\commdll\src\unix\dp\dpdll.c can implement
* its "horrible kluge".
*/
comm_driverInfo_t uudp_commDriverInfo =
{
	comm_DRIVER_SIGNATURE,
	sizeof(comm_driverInfo_t),
	"Unix Internet S",				/* name */
	0x0100,							/* version */
	(comm_DRIVER_IS_VISIBLE
	| comm_DRIVER_ALLOWS_GAMELIST
	| comm_DRIVER_NO_BROADCAST
	| comm_DRIVER_JUMBO_PACKETS),
	0
};

/*-------------------------------------------------------------------------
 Retrieve info about the communications driver.
 Return TRUE if info was retrieved.
 Return FALSE if called with NULL resp.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commDriverInfo(
	commDriverInfoReq_t*req,
	commDriverInfoResp_t*resp,
	void *commPtr)				/* comm context */
{
	/* Protect against invalid arguments */
	(void) req;
	(void) resp;
    if (NULL == resp)
		return FALSE;

	/* copy the data */
    resp->info = &uudp_commDriverInfo;

	/* Return status */
    resp->status = comm_STATUS_OK;
    return (TRUE);
}

/*-------------------------------------------------------------------------
 Retrieve info about a player, including ourselves.
 Return TRUE if the player is known and is not a group.
 Pointers returned in the response point to static buffers
 which will be overwritten by the next call.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commPlayerInfo(
	commPlayerInfoReq_t* req,	/* (input) */
	commPlayerInfoResp_t* resp, /* (output) */
	void *commPtr)				/* comm context */
{
	commPlayerInfoResp_t respDummy;
	comm_t	*comm = (comm_t *) commPtr;

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(NULL != req);
	if (NULL == resp)
		resp = &respDummy;
	if(NULL == req) {
		DPRINT(("commPlayerInfo: NULL req\n"));
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	/* Can't ask about pseudo-handles */
	assert(PLAYER_NONE != req->player);
	assert(PLAYER_UNKNOWN != req->player);
	assert(PLAYER_BROADCAST != req->player);
	/* Protect against compilers that don't really pack */
	assert(6 == sizeof(addr_t));

	/* Get the address from the handle */
	if (req->player == PLAYER_ME)
	{
		addr_t *ptr;
		ptr = getAddressFromHandle(comm->uudp_handles, comm->uudp_myHdl);

		if (!ptr)
		{
			DPRINT(("commPlayerInfo: player %d (ME) : no address\n", req->player));
			resp->status = comm_STATUS_BUG;
			return FALSE;
		}
		comm->adr = *ptr;
   	}
	else
	{
		addr_t *ptr;
		ptr = getAddressFromHandle(comm->uudp_handles, req->player);

		if (!ptr)
		{
			DPRINT(("commPlayerInfo: player %d : no address\n", req->player));
			resp->status = comm_STATUS_BUG;
			return FALSE;
		}
		comm->adr = *ptr;
	}

	DPRINT(("commPlayerInfo: player %d -> adr %x\n", req->player, comm->adr));

	/* copy the data and return */
	resp->name = "";	/* This layer doesn't know about names. */
	resp->address = &comm->adr;
	resp->addrLen = sizeof(addr_t);
	memset(resp->nodeID, 0, sizeof(resp->nodeID));

	/* Set status */
	resp->status = comm_STATUS_OK;

	/* Check for second address */
	if (req->player != PLAYER_ME) {
		uudps_peer2_t *ptr2;
		ptr2 = (uudps_peer2_t*)dcstFindValue(comm->uudp_secondary, req->player);
		if (ptr2) {
			comm->adr2 = *ptr2;
			resp->address2 = &comm->adr2;
			resp->status = comm_STATUS_DUAL_ADDRESS;
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------------
 ** unimplemented **

 Find out whether the transmit queue is full.
 Return TRUE if no more packets can be queued for transmission at this
 time.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commTxFull(
	commTxFullReq_t* req,	/* */
	commTxFullResp_t* resp,	/* */
	void *commPtr)			/* comm context */
{
	commTxFullReq_t reqDummy;
	commTxFullResp_t respDummy;

	DPRINT(("commTxFull:\n"));

	assert(validateComm((comm_t *)commPtr));

	/* Protect against invalid arguments */
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	/* Return status */
	resp->status = comm_STATUS_UNIMPLEMENTED;
	return FALSE;
}

/*-------------------------------------------------------------------------
 Send a packet.  Upon return, the buffer can be discarded, although the
 packet may not be sent until later.
 Return FALSE on error.  Note that a TRUE return value does not guarantee
 that the packet has been (or ever will be) sent.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commTxPkt(
	commTxPktReq_t*	req,	/* (input) destination, packet */
	commTxPktResp_t* resp,	/* (output) status */
	void *commPtr)			/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commTxPktResp_t	respDummy;
	sockaddr_in sockAddr;
	addr_t* pAddr;

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(req != NULL);
	assert(req->buffer != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if ((NULL == req) || (NULL == req->buffer)) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	/*DPRINT(("commTxPkt: pkt %c%c, player %x, length %d\n",
		((char *)req->buffer)[0], ((char *)req->buffer)[1],
		req->dest, req->length));*/

	/* Don't send to pseudo-handles */
	if ((req->dest == PLAYER_NONE) || (req->dest == PLAYER_UNKNOWN) ||
										(req->dest == PLAYER_ME)) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Get address from handle */
	if (req->dest == PLAYER_BROADCAST)
		pAddr = getAddressFromHandle(comm->uudp_handles, comm->uudp_broadcastHdl);
	else
		pAddr = getAddressFromHandle(comm->uudp_handles, req->dest);
	if (!pAddr) {
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = pAddr->addr;
	sockAddr.sin_port = pAddr->port;

	/* Send the packet */
/*	DPRINT(("commTxPkt: dest = ")); printAdr(pAddr); DPRINT(("\n"));
	DPRINT(("commTxPkt: sendto(%d, %p, %d, 0, %x, %d)\n",
					uudp_sock, req->buffer, req->length,
					(const sockaddr *)&sockAddr, sizeof(sockAddr)));*/
	if (sendto(comm->uudp_sock, req->buffer, req->length, 0,
			(const sockaddr *)&sockAddr, sizeof(sockAddr)) < 0) {
		DPRINT(("commTxPkt: sendto fails, errno %d\n", errno));
		resp->status = comm_STATUS_FULL;
		return FALSE;
	}

	logPkt(comm->logpkt_fp, req->buffer, req->length, req->dest, "tx");

	/* Return status */
	resp->status = comm_STATUS_OK;
	return TRUE;
}

/*-------------------------------------------------------------------------
 ** unimplemented **

 Get information about a pending incoming packet.
 Return TRUE if a packet was retrieved.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commPeekPkt(
	commPeekPktReq_t* req,		/* */
	commPeekPktResp_t* resp,	/* */
	void *commPtr)			/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commPeekPktReq_t reqDummy;
	commPeekPktResp_t respDummy;

	DPRINT(("commPeekPkt:\n"));

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	/* Return status */
	resp->status = comm_STATUS_UNIMPLEMENTED;
	return FALSE;
}

/*-------------------------------------------------------------------------
 Retrieve a pending incoming packet.

 Blocks briefly if packet not ready.

 Status is zero on success, nonzero on error.
 Return TRUE if a packet was retrieved, FALSE otherwise.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commRxPkt(
	commRxPktReq_t* req,	/* (modified) buffer size/buffer*/
	commRxPktResp_t* resp,	/* (output) packet size, status */
	void *commPtr)			/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commRxPktResp_t	respDummy;
	sockaddr_in sockAddr;
	addr_t addr;
	size_t nBytes;
	int fromlen;

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	resp->length = 0;
	resp->src = PLAYER_UNKNOWN;
	if (NULL == req) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* look for activity on socket, if none found after one sec, return */
	if (comm->uudp_rxWait)
	{
		fd_set fds;
		struct timeval timeout;
		int n;
		FD_ZERO(&fds);
		FD_SET(comm->uudp_sock, &fds);
		timeout.tv_usec = 20000;	/* 20 milliseconds */
		timeout.tv_sec = 0;
		n = select(comm->uudp_sock + 1, &fds, NULL, NULL, &timeout);
		if (n != 1) {
			/*DPRINT(("commRxPkt: select returns %d\n", n)); */
			resp->status = comm_STATUS_EMPTY;
			return FALSE;
		}
	}

	/* packet waiting, get it */
	fromlen = sizeof(sockAddr);
	nBytes = recvfrom(comm->uudp_sock, req->buffer, req->size, 0,
			(sockaddr *)&sockAddr, (socklen_t*) &fromlen);
	/*DPRINT(("commRxPkt: recvfrom(%d,,%d,0,,%d) returns %d.\n", uudp_sock, req->size, fromlen, nBytes));*/
	if (nBytes == (unsigned) -1) {
		resp->status = comm_STATUS_EMPTY;
		if (errno != EWOULDBLOCK) {
			DPRINT(("commRxPkt: recvfrom returns -1.  errno %d\n", errno));
		}
		return FALSE;
	}
	/* Handle ping packets immediately */
	if (dp_PING_PACKET_ID == *(dp_packetType_t*)req->buffer) {
		dp_ping_packet_t *pkt = (dp_ping_packet_t *)(req->buffer + 2);

		logPkt(comm->logpkt_fp, req->buffer, nBytes, resp->src, "rx");
		*(dp_packetType_t*)req->buffer = dp_PING_RESP_PACKET_ID;
		if ((nBytes > 5) && (pkt->len > 8) && comm->sessinfo) {
			/* Old pings with a sessiontype stuffed in at the end of pkt->data
			 * cause the server to return of information about the sessiontype.
			 */
			short players;
			short cur_players = SwapBytes2(comm->sessinfo->cur_players);
			short max_players = SwapBytes2(comm->sessinfo->max_players);
			unsigned char *p = pkt->data + (pkt->len - sizeof(dp_species_t));
			dp_species_t sessType = SwapBytes2(*((dp_species_t*)p));
			players = SwapBytes2((comm->sessinfo->players)[sessType]);
			p += sizeof(dp_species_t); *((short *)p) = cur_players;
			p += 2; *((short *)p) = max_players;
			p += 2; *((short *)p) = players;
			pkt->len += 6;
			nBytes += 6;
		}
		sendto(comm->uudp_sock, req->buffer, nBytes, 0,
			(const sockaddr *)&sockAddr, sizeof(sockAddr));
		DPRINT(("commRxPkt: ping packet handled.\n"));
		logPkt(comm->logpkt_fp, req->buffer, nBytes, resp->src, "tx");
		resp->status = comm_STATUS_EMPTY;
		return FALSE;
	}
	resp->length = nBytes;

	/* Get handle of sender, copy into resp */
	addr.addr = sockAddr.sin_addr.s_addr;
	addr.port = sockAddr.sin_port;
	resp->src = getHandleFromAddress(comm->uudp_handles, &addr);
	if((unsigned) dcst_INVALID_KEY == resp->src) {
		resp->src = PLAYER_NONE;
		assert(sizeof(addr_t) <= comm_MAX_ADR_LEN);
		memcpy(resp->adr, &addr, sizeof(addr_t));
	} else if (resp->src == (unsigned) comm->uudp_myHdl) {
		resp->src = PLAYER_ME;
	}
	assert(resp->src != (unsigned) comm->uudp_broadcastHdl);	/* erroneous sockAddr from recvfrom */

	logPkt(comm->logpkt_fp, req->buffer, resp->length, resp->src, "rx");

	/* Return status */
	resp->status = comm_STATUS_OK;
	/*DPRINT(("commRxPkt: got packet %c%c, length %d, src %x\n",
		((char*)req->buffer)[0], ((char*)req->buffer)[1], nBytes, resp->src));*/
	return TRUE;
}

/*-------------------------------------------------------------------------
 Attempt to parse a NUL-terminated address string into an addr_t.

 Address strings are of the form <address>[:<port>], where <port> is a
 two-byte decimal number, and <address> is either four individual one-byte
 decimal numbers separated by '.'s (e.g., 255.255.255.255 or 1.1.1.1),
 or a string that can be resolved by a name lookup.  If there is no port
 specified in the string, the address buffer is given a port of
 UUDP_SOCKET_DEFAULT.

 Return TRUE if the buffer was formatted successfully, FALSE otherwise.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commScanAddr(
	commScanAddrReq_t* req,		/* (modified) string/address */
	commScanAddrResp_t* resp,	/* (output) addr len, status */
	void *commPtr)				/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commScanAddrResp_t respDummy;
	u_long adr;
	u_short port = UUDP_SOCKET_DEFAULT;
	char *pc;

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	if (req->size < sizeof(addr_t)) {
		resp->status = comm_STATUS_FULL;
		return FALSE;
	}
	resp->status = comm_STATUS_OK;

	DPRINT(("commScanAddr(%s):\n", req->printable));

	/* Strip port from req's string */
	for(pc = req->printable; *pc != '\0'; pc++) {
		if(':' == *pc) {
			*pc = '\0';
			*pc++;
			if(*pc != '\0')
				port = (unsigned short) atoi(pc);
			break;
		}
	}

	/* Convert address if it's in numeric format (e.g., 255.255.255.255) */
	adr = inet_addr(req->printable);

	/* Get address from name if not numeric */
	if (adr == INADDR_NONE) {
		struct hostent *he;
		he = gethostbyname(req->printable);
		if (NULL == he) {
			resp->status = comm_STATUS_BAD;
			return FALSE;
		}
		memcpy(&adr, he->h_addr, sizeof(u_long));
	}

	/* Save and return */
	/* Note: yes, it really expects the returned address in a field
	   in the req parameter, and the length of that address in a field
	   in the resp parameter. */
	if (NULL != req->address) {
		((addr_t *)req->address)->addr = adr;
		((addr_t *)req->address)->port = htons(port);
	}
	resp->length = sizeof(addr_t);

	/* Return status */
	return (TRUE);
}

/*-------------------------------------------------------------------------
  Attempt to format an addr_t into a NUL-terminated string.  The string
  will be of the form 255.255.255.255:65535, where the first four numbers
  are the address and the final number is the port (socket) number.

  Return TRUE if the buffer was formatted successfully, FALSE otherwise.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commPrintAddr(
	commPrintAddrReq_t* req,	/* (modified) address/buffer */
	commPrintAddrResp_t* resp,  /* (output) status */
	void *commPtr)				/* comm context */
{
	commPrintAddrResp_t respDummy;
	char * strAddr;
	char strPort[128];
	struct in_addr inetAddr;

	DPRINT(("@TCP commPrintAddr(): "));

	/* Protect against invalid parameters */
	assert(validateComm((comm_t *)commPtr));
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Make sure the address is the correct size */
	if (req->length != sizeof(addr_t)) {
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}
	/* Get printable version */
	sprintf(strPort, ":%d", ntohs(((addr_t *)(req->address))->port));
	inetAddr.s_addr = ((addr_t *)(req->address))->addr;
	strAddr = inet_ntoa(inetAddr);
	if (!strAddr) {
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}

	/* Copy printable version to response */
	if (req->printable != NULL) {
		if (req->size < strlen(strAddr) + strlen(strPort) + 1) {
			resp->status = comm_STATUS_FULL;
			return FALSE;
		}
		strcpy(req->printable, strAddr);
		strcat(req->printable, strPort);
	}

	/* Save status and return */
	resp->status = comm_STATUS_OK;
	return TRUE;
}

/*-------------------------------------------------------------------------
 Tear down a data link to a player.  The link or the player may already be
 down, so don't shake hands.

 Return TRUE if the link was successfully broken.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commSayBye(
	commSayByeReq_t* req,	/* */
	commSayByeResp_t* resp,	/* */
	void *commPtr)			/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commSayByeResp_t		respDummy;

	DPRINT(("commSayBye:\n"));

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Check the handle */
	if(NULL == dcstFindValue(comm->uudp_handles, req->player)) {
		assert(0);	/* Attempt to say bye to a nonexistant player */
	    resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Recycle the handle */
	dcstDeleteKey(comm->uudp_handles, req->player);
	if (NULL != dcstFindValue(comm->uudp_secondary, req->player))
		dcstDeleteKey(comm->uudp_secondary, req->player);

	/* Return status */
    resp->status = comm_STATUS_OK;
    return TRUE;
}

/*-------------------------------------------------------------------------
Establish a data link to a player and shake hands with him.  This does
not actually establish a connection in the IP sense of the word.

Assign a handle to the player's address.  The driver only allows sending
to handles.

Return TRUE if the handle was assigned.
-------------------------------------------------------------------------*/
int cdecl					/* success boolean */
commSayHi(
	commSayHiReq_t* req,	/* (input) address */
	commSayHiResp_t* resp,	/* (output) handle, status */
	void *commPtr)				/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commSayHiResp_t respDummy;

	DPRINT(("commSayHi: uudps\n"));

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Check length */
	if (req->length != (sizeof(addr_t))) {
		DPRINT(("commSayHi: bad addr len\n"));
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Find or assign a handle for this address */
	resp->player = getHandleFromAddress(comm->uudp_handles, req->address);
	if(resp->player == (unsigned) dcst_INVALID_KEY) {
		resp->player = dcstAdd(comm->uudp_handles, req->address);
		if (req->address2) {
			dcstAddEx(comm->uudp_secondary, resp->player, req->address2);
		}
	}

	/* Translate handles to commapi values */
	if(resp->player == (unsigned) dcst_INVALID_KEY) {
		resp->player = PLAYER_UNKNOWN;
		resp->status = comm_STATUS_EMPTY;
		return FALSE;
	} else if(resp->player == (unsigned) comm->uudp_myHdl)
		resp->player = PLAYER_ME;
	else if(resp->player == (unsigned) comm->uudp_broadcastHdl)
		resp->player = PLAYER_BROADCAST;

	/* Return status */
	resp->status = comm_STATUS_OK;
	return TRUE;
}

/*-------------------------------------------------------------------------
 ** unimplemented **

 Generate a pseudo-player handle referring to a group of players.  Handy
 for multicasting.  A group can have zero players.
 Return TRUE if the pseudo-player handle was generated.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commGroupAlloc(
	commGroupAllocReq_t* req,	/* */
	commGroupAllocResp_t* resp,	/* */
	void *commPtr)				/* comm context */
{
	commGroupAllocReq_t reqDummy;
	commGroupAllocResp_t respDummy;

	DPRINT(("commGroupAlloc:\n"));

	/* Protect against invalid arguments */
	assert(validateComm((comm_t *)commPtr));
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	/* Return status */
	resp->status = comm_STATUS_UNIMPLEMENTED;
	return FALSE;
}

/*-------------------------------------------------------------------------
 ** unimplemented **

 Invalidate a pseudo-player handle referring to a group of players.
 Return TRUE if the pseudo-player handle was invalidated.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commGroupFree(
	commGroupFreeReq_t* req,	/* */
	commGroupFreeResp_t* resp,	/* */
	void *commPtr)				/* comm context */
{
	commGroupFreeReq_t reqDummy;
	commGroupFreeResp_t respDummy;

	DPRINT(("commGroupFree:\n"));

	/* Protect against invalid arguments */
	assert(validateComm((comm_t *)commPtr));
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	/* Return status */
	resp->status = comm_STATUS_UNIMPLEMENTED;
	return FALSE;
}

/*-------------------------------------------------------------------------
 ** unimplemented **

 Add one or more players to a group.
 Return TRUE if the players were all added.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commGroupAdd(
	commGroupAddReq_t* req,		/* */
	commGroupAddResp_t* resp,	/* */
	void *commPtr)				/* comm context */
{
	commGroupAddReq_t reqDummy;
	commGroupAddResp_t respDummy;

	DPRINT(("commGroupAdd:\n"));

	/* Protect against invalid arguments */
	assert(validateComm((comm_t *)commPtr));
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	/* Return status */
	resp->status = comm_STATUS_UNIMPLEMENTED;
	return FALSE;
}

/*-------------------------------------------------------------------------
 ** unimplemented **
 Set driver parameters.
-------------------------------------------------------------------------*/
int cdecl						/* success boolean */
commSetParam(
	commSetParamReq_t* req,		/* */
	commSetParamResp_t* resp,	/* */
	void *commPtr)				/* comm context */
{
	comm_t	*comm = (comm_t *) commPtr;
	commSetParamReq_t	reqDummy;
	commSetParamResp_t	respDummy;

	DPRINT(("commSetParam: reqLen %d, pnum %ld, pvalue %ld, pvalue2 %ld\n",
		req->reqLen, req->param_num, req->param_value, req->param_value2));

	/* Protect against invalid arguments */
	assert(validateComm(comm));
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;
	if (req->reqLen != sizeof(commSetParamReq_t)) {
		resp->status = comm_STATUS_BAD_VERSION;
		return FALSE;
	}

	/* Get the parameter */
	resp->status = comm_STATUS_UNIMPLEMENTED;
	switch (req->param_num) {
		case comm_PARAM_SCORE:
			/* Do stuff here */
			break;
		case comm_PARAM_TICKSPERSEC:
			/* Do stuff here */
			break;
		case comm_PARAM_SESSIONINFO:
			if (req->param_value) {
				DPRINT(("commSetParam: set new sessioninfo successfully\n"));
				comm->sessinfo = (commSessInfo_t *) req->param_value;
				resp->status = comm_STATUS_OK;
			} else
				resp->status = comm_STATUS_BAD;
			break;

		case comm_PARAM_RXWAIT:
			if (req->param_value != comm_PARAM_BOGUSVALUE)
			{
				comm->uudp_rxWait = req->param_value;
				DPRINT(("commSetParam: wait mode set to %d\n", comm->uudp_rxWait));
			}
			resp->param_value = comm->uudp_rxWait;
			resp->status = comm_STATUS_OK;
			break;

		case comm_PARAM_FILEDESC:
			resp->param_value = comm->uudp_sock;
			resp->status = comm_STATUS_OK;
			break;

		default:
			DPRINT(("commSetParam: unknown parameter %d!\n", req->param_num));
			resp->status = comm_STATUS_BAD;
			break;
	}

	/* Return success or failure */
	return (resp->status == comm_STATUS_OK);
}
