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

                          Comm API Shims for IPX2 Protocol

                              (c) 1996 By Activision

**************************************************************************************/

// 22-OCT-96  File modified to support Win32  [M. Livesay]

/*---------------------------------------------------------------------------
 Shim to turn ipx2.h's interface into commapi.h's interface
 $Log: ipx2dll.c $
 Revision 1.9  1997/03/07 19:51:29  dkegel
 Got rid of _WINDOWS ifdef.
 Revision 1.8  1997/02/15 20:32:02  dkegel
 IPX loves broadcast, c'mon!
 Revision 1.7  1997/02/12 04:55:09  dkegel
 Changed port number; now controlled from commapi.h.
 Revision 1.6  1997/02/01 00:43:48  dkegel
 1. Added newline to end of all DPRINT's.  Futzed with them a bit.
 2. printAdr and scanAdr were amazingly confused.  Returned to the
    portable implementation from the DOS driver.
 Revision 1.5  1997/01/31 02:37:46  dkegel
 Converted to ddprint.h
 Revision 1.4  1997/01/21 00:41:34  jgraebner
 Modified functions to allow DP to pass in raw address structure and have
 dll pack it into IPX address structure.
 Revision 1.3  1996/12/13 23:12:36  jgraebner
 Fixed incorrect pointer reference.
 Revision 1.2  1996/12/13 00:16:04  dkegel
 nodeID now a byte array instead of a structure.  Not used, just zero.
 Revision 1.1  1996/12/12 22:09:15  dkegel
 Initial revision
 Revision 1.10  1996/09/06 23:13:42  dkegel
 Replaced commGroupSubtract with commSetParam.
 Revision 1.9  1996/06/30 19:40:33  dkegel
 Added NO_BROADCAST option for researching WAN behavior...
 Revision 1.8  1996/05/22 00:46:11  dkegel
 Removed word "Driver" from dll description.
 Revision 1.7  1996/05/21 21:09:07  dkegel
 Issue more appropriate error if can't access IPX.
 Revision 1.6  1996/05/15 02:12:28  dkegel
 Use standard comm_STATUS codes in commInit.
 Revision 1.5  1996/04/30 20:59:35  dkegel
 Added "comm_DRIVER_IS_FAST" to capabilities bit field.
 Revision 1.4  1996/03/05 19:47:31  dkegel
 Implemented new driver signature stuff.
 Revision 1.3  1996/02/13 20:19:11  dkegel
 Report cause of error(in a kludgy way) when commInit fails.
 Revision 1.2  1996/02/12 03:14:28  dkegel
 Ignore packets from self.
 Revision 1.1  1996/02/12 01:57:41  dkegel
 Initial revision
---------------------------------------------------------------------------*/


/*************************************************************************************

                                    Literals

**************************************************************************************/

#include <windows.h>
#include "ipx2win.h"
#include "nw.h"

#include "commapi.h"
#include <stdio.h>    // for sprintf and sscanf

#define DEBUG_MODULE 1
#include "ddprint.h"

#include "logpkt.h"
#ifdef LOGPKT_ENABLED
FILE *logpkt_fp = NULL;
#endif

#define SOCKET_MW2    (0x52A1+commapi_NET_PORTOFFSET)    // (('M'<<4 + 'W')<<4 + '1')

#ifndef TRUE
  #define TRUE        (1)
  #define FALSE       (0)
#endif

/*************************************************************************************

                                    Globals

**************************************************************************************/

static ipx2_t *ipx;

/*************************************************************************************

                                 ipx2commHdl()

**************************************************************************************/

static playerHdl_t ipx2commHdl(ipx2_hdl_t h)

{
  switch (h)
  {
    case ipx2_HDL_NONE:
    return PLAYER_NONE;

    case ipx2_HDL_ME:
    return PLAYER_ME;

    case ipx2_HDL_BROADCAST:
    return PLAYER_BROADCAST;

    default:
    break;
  }

  return ((playerHdl_t) h);
}

/*************************************************************************************

                                 commHdl2ipx()

**************************************************************************************/

static ipx2_hdl_t commHdl2ipx(playerHdl_t h)

{
  switch (h)
  {
    case PLAYER_NONE:
    return ipx2_HDL_NONE;

    case PLAYER_ME:
    return ipx2_HDL_ME;

    case PLAYER_BROADCAST:
    return ipx2_HDL_BROADCAST;

    case PLAYER_UNKNOWN:
    return ipx2_HDL_NONE;

    default:
    break;
  }

  return ((ipx2_hdl_t) h);
}

/*************************************************************************************

                                 commNoOp()

**************************************************************************************/

// Spin your wheels -- and get paid for it!

// Return FALSE on error.

DLLEXPORT int cdecl commNoOp(commNoOpReq_t *req,  commNoOpResp_t *resp)

{
  commNoOpReq_t   reqDummy;
  commNoOpResp_t  respDummy;

  DPRINT(("@IPX commNoOp():\n"));

  if (req ==  NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = ipx2_RES_OK;

  return (TRUE);
}

/*-------------------------------------------------------------------------
 List the available comm ports.
 Implemented only for serial and modem drivers.
-------------------------------------------------------------------------*/
DLLEXPORT int cdecl
commEnumPorts(
	commEnumPortsReq_t *	req,		/* Request (or NULL) */
	commEnumPortsResp_t *	resp)		/* Response (or NULL) */
{
	if (resp)
		resp->status = comm_STATUS_BUG;
	return FALSE;
}

/*************************************************************************************

                                 commInit()

**************************************************************************************/

//  Initialize the communications driver.
//  Call this function only once.

//  Return FALSE on error.

DLLEXPORT int cdecl commInit(commInitReq_t *req, commInitResp_t *resp)

{
  int             status;
  commInitReq_t   reqDummy;
  commInitResp_t  respDummy;

  /* Already set via commSetParam(); */
  /* dp_dprintf_set(ddprintf); */
  DPRINT(("@IPX commInit():\n"));

#ifdef LOGPKT_ENABLED
	logpkt_fp = logPkt_open();
#endif

  if (req ==  NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  ipx = ipx2_create(SOCKET_MW2, &status);
  if (!ipx)
  {
    DPRINT(("IPX is not present, status %d\n", status));
    resp->status = comm_STATUS_NETWORK_NOT_PRESENT;   // ipx2_RES_BAD;
    return FALSE;
  }

  resp->status = comm_STATUS_OK;

  DPRINT(("commInit returns %u\n", resp->status));

  return TRUE;
}

/*************************************************************************************

                                 commTerm()

**************************************************************************************/

//  Tear down the communications driver.
//  Return FALSE on error.

DLLEXPORT int cdecl commTerm(commTermReq_t *req,  commTermResp_t *resp)

{
  commTermReq_t   reqDummy;
  commTermResp_t  respDummy;

  DPRINT(("@IPX commTerm():\n"));

	logPkt_close(logpkt_fp);

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  ipx2_destroy(ipx);

  resp->status = ipx2_RES_OK;

  return TRUE;
}

/*************************************************************************************

                                 commDriverInfo()

**************************************************************************************/

//  Retrieve info about the communications driver.
//
//  Return TRUE if info was retrieved.

DLLEXPORT int cdecl commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp)

{
  // We plan to make dpEnumTransports scan through the DLL file looking
  // for this signature.  It will use the info to present a driver name
  // to the user, and discover driver properties such as "expects to be
  // called by external shell (e.g. dwango)".
  static comm_driverInfo_t commDriverInfo =
  {
    comm_DRIVER_SIGNATURE,
    sizeof(comm_driverInfo_t),
#ifdef _DEBUG
		"Debug "
#endif
    "IPX",
    0x0100,
    (comm_DRIVER_IS_VISIBLE | comm_DRIVER_IS_FAST | comm_DRIVER_ALLOWS_GAMELIST),
    0
  };

  static commDriverInfoResp_t   commDriverInfoResp =
  {
    0,
    &commDriverInfo
  };

  if (resp == NULL) return FALSE;

  *resp = commDriverInfoResp;

  resp->status = 0;

  (void) req;

  return (TRUE);
}

/*************************************************************************************

                                 commPlayerInfo()

**************************************************************************************/

// Retrieve info about a player, including ourselves.
// Return TRUE if the player is known and is not a group.
// Pointers returned in the response point to static buffers
// which will be overwritten by the next call.

DLLEXPORT int cdecl commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp)

{
  commPlayerInfoReq_t   reqDummy;
  commPlayerInfoResp_t  respDummy;
  ipx2_result_t         err;

  #ifdef _WINDOWS
    IPXPEER             adr;
	static nw_adr_t		adr2;
  #else
    static nw_adr_t     adr;
  #endif

  DPRINT(("@IPX commPlayerInfo():\n"));

  if (req ==  NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  // This comm layer doesn't know about user names.
  resp->name = "";

  err = ipx2_hdl2adr(ipx, commHdl2ipx(req->player), &adr);
  if (err != ipx2_RES_OK)
  {
    resp->status = err;
    return FALSE;
  }

  // fill in response struct

#ifdef _WINDOWS
  memcpy (adr2.net, adr.sa_netnum, sizeof(adr2.net));
  memcpy (adr2.node, adr.sa_nodenum, sizeof(adr2.node));
  resp->address = &adr2;
  resp->addrLen = sizeof(adr2);
#else
  resp->address   = &adr;
  resp->addrLen   = sizeof(adr);
#endif
  memset(&resp->nodeID, 0, sizeof(resp->nodeID));
  resp->status    = ipx2_RES_OK;

  return(TRUE);
}

/*************************************************************************************

                                 commTxFull()

**************************************************************************************/

//  Find out whether the transmit queue is full.
//  Return TRUE if no more packets can be queued for transmission at this
//  time.

DLLEXPORT int cdecl commTxFull(commTxFullReq_t * req, commTxFullResp_t *resp)

{
  commTxFullReq_t   reqDummy;
  commTxFullResp_t  respDummy;

  DPRINT(("@IPX commTxFull():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = ipx2_RES_OK;

  return FALSE;
}

/*************************************************************************************

                                 commTxPkt()

**************************************************************************************/

//  Send a packet.  Upon return, the buffer can be discarded, although the
//  packet may not be sent until later.
//  Return FALSE on error.  Note that a TRUE return value does not guarantee
//  that the packet has been (or ever will be) sent.

DLLEXPORT int cdecl commTxPkt(commTxPktReq_t *  req, commTxPktResp_t * resp)

{
  commTxPktReq_t  reqDummy;
  commTxPktResp_t respDummy;
  ipx2_result_t err;
  ipx2_hdl_t    h;

  //DPRINT(("@IPX commTxPkt():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

//#define NO_BROADCAST    // Uncomment this line to simulate WAN lacking broadcast
  #ifdef NO_BROADCAST
    if (req->dest == PLAYER_BROADCAST)
    {
      resp->status = comm_STATUS_BAD;
      return 0;
    }
  #endif

	logPkt(logpkt_fp, req->buffer, req->length, req->dest, "tx");

  h = commHdl2ipx(req->dest);

  err = ipx2_put(ipx, req->buffer, req->length, h);
  resp->status = err;

  return (err == ipx2_RES_OK);
}

/*************************************************************************************

                                 commPeekPkt()

**************************************************************************************/

//  Get information about a pending incoming packet.

//  Return TRUE if a packet was retrieved.

DLLEXPORT int cdecl commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp)

{
  commPeekPktReq_t  reqDummy;
  commPeekPktResp_t respDummy;

  DPRINT(("@IPX commPeekPkt():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  // Not implemented
  resp->status = ipx2_RES_BAD;

  return FALSE;
}

/*************************************************************************************

                                 commRxPkt()

**************************************************************************************/

// Retrieve a pending incoming packet.

//  Return TRUE if a packet was retrieved.
//  Status is zero on success, nonzero on error!

DLLEXPORT int cdecl commRxPkt(commRxPktReq_t *req,  commRxPktResp_t *resp)

{
  commRxPktReq_t  reqDummy;
  commRxPktResp_t respDummy;
  ipx2_result_t   err;
  ipx2_hdl_t      hdl;
  IPXPEER addr;

  //DPRINT(("@IPX commRxPkt(): "));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->length = req->size;

  err = ipx2_get( ipx, req->buffer, &resp->length, &hdl, &addr );
  resp->src = ipx2commHdl(hdl);

  // ignore packets from myself
  if (resp->src == PLAYER_ME)  err = ipx2_RES_EMPTY;

  // For debugging, print source address of packets from unknown players
  if ((err == 0) && (resp->src == PLAYER_NONE)) {
	DPRINT(("IPX commRxPkt: unknown src %02x.%02x.%02x.%02x;%02x.%02x.%02x.%02x.%02x.%02x\n",
		0xff & ((unsigned)addr.sa_netnum[0]),
		0xff & ((unsigned)addr.sa_netnum[1]),
		0xff & ((unsigned)addr.sa_netnum[2]),
		0xff & ((unsigned)addr.sa_netnum[3]),
		0xff & ((unsigned)addr.sa_nodenum[0]),
		0xff & ((unsigned)addr.sa_nodenum[1]),
		0xff & ((unsigned)addr.sa_nodenum[2]),
		0xff & ((unsigned)addr.sa_nodenum[3]),
		0xff & ((unsigned)addr.sa_nodenum[4]),
		0xff & ((unsigned)addr.sa_nodenum[5])));
  }
  resp->status = err;

  if (err == ipx2_RES_OK) {
	logPkt(logpkt_fp, req->buffer, resp->length, resp->src, "rx");
  }

  return (err == ipx2_RES_OK);
}

/*************************************************************************************

                                 commScanAddr()

**************************************************************************************/

// Take an ASCII network/node address, and convert it to binary.
// String is in same format as produced by commPrintAddr.
// Format of both ASCII and binary addresses is same for all platforms that
// support the IPX protocol.
// Return TRUE if the string was parsed successfully.

DLLEXPORT int cdecl commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp)

{
  commScanAddrReq_t   reqDummy;
  commScanAddrResp_t  respDummy;
  int                 tmpInt[32];
  int                 i;

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  DPRINT(("@IPX commScanAddr(%s):\n", req->printable));

  if (req->size < sizeof(nw_adr_t)) {
      resp->status = ipx2_RES_FULL;
      return FALSE;
  }

  i = sscanf(req->printable, "%u.%u.%u.%u,%X.%X.%X.%X.%X.%X",
             &tmpInt[0], &tmpInt[1], &tmpInt[2], &tmpInt[3],
             &tmpInt[4], &tmpInt[5], &tmpInt[6], &tmpInt[7],
             &tmpInt[8], &tmpInt[9]);

  if (i != 10) {
    resp->status = ipx2_RES_BAD;
    DPRINT(("IPX commScanAddr(%s): sscanf returned %d < 10", req->printable, i));
    return FALSE;
  }

  for (i = sizeof(nw_netAdr_t);  i--;) ((nw_adr_t *) req->address)->net[i]  = tmpInt[i];
  for (i = sizeof(nw_nodeAdr_t); i--;) ((nw_adr_t *) req->address)->node[i] = tmpInt[i + sizeof(nw_netAdr_t)];
  resp->length = sizeof(nw_adr_t);

  resp->status = ipx2_RES_OK;
  return (TRUE);
}

/*************************************************************************************

                                 commPrintAddr()

**************************************************************************************/

//  Convert the compact binary network address in req->address
//  into an ASCII string in req->printable.
//  Return TRUE if the buffer was formatted successfully.
//  Fails if input is wrong size or output buffer is null or too small.

DLLEXPORT int cdecl commPrintAddr(commPrintAddrReq_t *req, commPrintAddrResp_t *resp)

{
  commPrintAddrResp_t respDummy;
  char                printable[80];
  const nw_adr_t *params;

  DPRINT(("@IPX commPrintAddr():\n"));

  if (resp == NULL)  resp = &respDummy;
  if (!req || (req->length != sizeof(*params))) {
	  resp->status = ipx2_RES_BAD;
	  return FALSE;
  }

  params = req->address;
  sprintf(printable, "%u.%u.%u.%u,%X.%X.%X.%X.%X.%X",
            params->net[0]&0xff,
            params->net[1]&0xff,
            params->net[2]&0xff,
            params->net[3]&0xff,
            params->node[0]&0xff,
            params->node[1]&0xff,
            params->node[2]&0xff,
            params->node[3]&0xff,
            params->node[4]&0xff,
            params->node[5]&0xff);

  if (req->printable != NULL) {
    if (req->size < strlen(printable) + 1) {
      resp->status = ipx2_RES_FULL;
      return FALSE;
    }

    strcpy(req->printable, printable);
  }
  DPRINT(("@IPX commPrintAddr(): result %s\n", printable));

  resp->status = ipx2_RES_OK;
  return TRUE;
}

/*************************************************************************************

                               commGroupAlloc()

**************************************************************************************/

//  Generate a pseudo-player handle referring to a group of players.  Handy
//  for multicasting.  A group can have zero players.

//  Return TRUE if the pseudo-player handle was generated.

DLLEXPORT int cdecl commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp)

{
  commGroupAllocReq_t   reqDummy;
  commGroupAllocResp_t  respDummy;

  DPRINT(("@IPX commGroupAlloc():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = 0;

  return FALSE;
}

/*************************************************************************************

                               commGroupFree()

**************************************************************************************/

//  Invalidate a pseudo-player handle referring to a group of players.

//  Return TRUE if the pseudo-player handle was invalidated.

DLLEXPORT int cdecl commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp)

{
  commGroupFreeReq_t  reqDummy;
  commGroupFreeResp_t respDummy;

  DPRINT(("@IPX commGroupFree():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = 0;

  return FALSE;
}

/*************************************************************************************

                               commGroupAdd()

**************************************************************************************/

//  Add one or more players to a group.

//  Return TRUE if the players were all added.

DLLEXPORT int cdecl commGroupAdd(commGroupAddReq_t *req,  commGroupAddResp_t *resp)

{
  commGroupAddReq_t   reqDummy;
  commGroupAddResp_t  respDummy;

  DPRINT(("@IPX commGroupAdd():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = 0;

  return FALSE;
}

/*************************************************************************************

                               commSetParam()

**************************************************************************************/

//  Set driver parameters.

DLLEXPORT int cdecl commSetParam(commSetParamReq_t *req,  commSetParamResp_t *resp)

{
	commSetParamReq_t   reqDummy;
	commSetParamResp_t  respDummy;

	DPRINT(("@IPX commSetParam():\n"));

	if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)  resp = &respDummy;

	DPRINT(("commSetParam: num %x, value %ld, value2 %ld\n", req->param_num, req->param_value, req->param_value2));
	switch (req->param_num) {
	case comm_PARAM_DPRINTF:
		dp_dprintf_set((dp_dprintf_t) req->param_value);
		break;
	default:
		resp->status = comm_STATUS_UNIMPLEMENTED;
		return FALSE;
	}

	resp->status = 0;
	return TRUE;
}

/*************************************************************************************

                               commSayHi()

**************************************************************************************/

//  Establish a data link to a player and shake hands with him.  This does
//  not actually establish a connection in the IP sense of the word.

//  Return TRUE if the link was established and we shook hands.

DLLEXPORT int cdecl commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp)

{
  commSayHiResp_t   respDummy;
  ipx2_hdl_t        h;
  const unsigned char IPX_Netnum_Broadcast[4] = { 0xff, 0xff, 0xff, 0xff };
  IPXPEER           params;
  nw_adr_t *		adr;
  void printaddr(IPXPEER *peer, char *string);

  DPRINT(("@IPX commSayHi():\n"));

  /* Sanity check arguments. */
  if (resp == NULL)  resp = &respDummy;
  if (!req || (req->length != sizeof(*adr))) {
	DPRINT(("@IPX commSayHi: req->length %d != sizeof(*adr) %d!\n", req->length, sizeof(*adr)));
    resp->status = ipx2_RES_BAD;
    return FALSE;
  }

  /* Create an IPXPEER from req->address and our own IPXPEER. */
  adr = req->address;
  ipx2_hdl2adr(ipx, ipx2_HDL_ME, &params);
  //printaddr(&params, "commSayHi me: ");
  memcpy(params.sa_netnum, adr->net, sizeof(adr->net));
  memcpy(params.sa_nodenum, adr->node, sizeof(adr->node));

  /* Kludge: If the player tries to tell us their netnum is ff.ff.ff.ff
   * (broadcast), don't even bother trying to connect; that would
   * go looking for all broadcast recipients (we think).
   */
  if (!memcmp(params.sa_netnum, IPX_Netnum_Broadcast, sizeof(adr->net))) {
		DPRINT(("commSayHi: host has bad net number.\n"));
		resp->status = comm_STATUS_HOST_UNREACHABLE;
		return FALSE;
  }

  printaddr(&params, "commSayHi: ");

  /* Look up the IPXPEER in the table, or add it if it's not there yet. */
  h = ipx2_adr2hdl(ipx, &params, TRUE);
  if (h == ipx2_HDL_NONE) {
		DPRINT(("commSayHi: can't open handle.\n"));
		resp->status = ipx2_RES_BAD;
		return FALSE;
  }

  /* Convert the handle to one that fits commapi.h and return. */
  resp->player = ipx2commHdl(h);
  DPRINT(("commSayHi: opened handle %d, translated to player %d.\n", h, resp->player));
  resp->status = ipx2_RES_OK;
  return TRUE;
}

/*************************************************************************************

                               commSayBye()

**************************************************************************************/

//  Tear down a data link to a player.  The link or the player may already be
//  down, so don't shake hands.

//  Return TRUE if the link was successfully broken.

DLLEXPORT int cdecl commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp)

{
  commSayByeReq_t     reqDummy;
  commSayByeResp_t    respDummy;

  DPRINT(("@IPX commSayBye():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = 0;

  return TRUE;
}

#ifdef _WINDOWS
/*************************************************************************************

                               DllMain()

**************************************************************************************/

BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)

{
  WSADATA wsaData;

  switch (fdwReason)
  {
    case DLL_PROCESS_ATTACH:
      WSAStartup(MAKEWORD(1, 1), &wsaData);
    return(TRUE);

    case DLL_PROCESS_DETACH:
      WSACleanup();
    return(TRUE);

    case DLL_THREAD_ATTACH:
    return(TRUE);

    case DLL_THREAD_DETACH:
    return(TRUE);
  }

  return(TRUE);
}

#endif
