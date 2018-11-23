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
 Comm API Shims for loopback Protocol

 $Log: loop.c $
 Revision 1.7  1997/08/14 19:26:30  dkegel
 Added a dprint or two
 Revision 1.6  1997/07/08 21:51:37  dkegel
 Use sessionId passed in commInitReq, if present, to
 regain same network address when thawing.
 Revision 1.5  1997/06/10 01:39:32  dkegel
 D'oh.  Fixed scanf check.
 Revision 1.4  1997/06/04 20:13:42  dkegel
 Change driver name if broadcast not supported.
 Revision 1.3  1997/06/04 19:36:29  dkegel
 First crack at implementing printAddr and scanAddr (internet style)
 correctly
 Revision 1.2  1997/05/23 23:50:38  mrobin
 Added no broadcast versions of the DLLs
 Revision 1.1  1997/05/21 20:04:24  mrobin
 Initial revision
 Copied from ..\wipx
---------------------------------------------------------------------------*/


/*************************************************************************************

                                    Literals

**************************************************************************************/

#include <windows.h>
#include "loopwin.h"

#include "commapi.h"
#include <stdio.h>    // for sprintf and sscanf

#define DEBUG_MODULE 1
#include "ddprint.h"

#ifndef TRUE
  #define TRUE        (1)
  #define FALSE       (0)
#endif

/*************************************************************************************

                                    Globals

**************************************************************************************/

static loop_t *loop;

/*************************************************************************************

                                 loop2commHdl()

**************************************************************************************/

static playerHdl_t loop2commHdl(loop_hdl_t h)

{
  switch (h)
  {
    case loop_HDL_NONE:
    return PLAYER_NONE;

    case loop_HDL_ME:
    return PLAYER_ME;

    case loop_HDL_BROADCAST:
    return PLAYER_BROADCAST;

    default:
    break;
  }

  return ((playerHdl_t) h);
}

/*************************************************************************************

                                 commHdl2loop()

**************************************************************************************/

static loop_hdl_t commHdl2loop(playerHdl_t h)

{
  switch (h)
  {
    case PLAYER_NONE:
    return loop_HDL_NONE;

    case PLAYER_ME:
    return loop_HDL_ME;

    case PLAYER_BROADCAST:
    return loop_HDL_BROADCAST;

    case PLAYER_UNKNOWN:
    return loop_HDL_NONE;

    default:
    break;
  }

  return ((loop_hdl_t) h);
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

  DPRINT(("@LOOP commNoOp():\n"));

  if (req ==  NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = loop_RES_OK;

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
//  SessionId passed to loopback code so freeze/thaw works properly.

//  Return FALSE on error.

DLLEXPORT int cdecl commInit(commInitReq_t *req, commInitResp_t *resp)

{
  int             status;
  commInitReq_t   reqDummy;
  commInitResp_t  respDummy;

  dp_dprintf_set(ddprintf);	/* allow direct calls to dp_dprintf */
  DPRINT(("@LOOP commInit():\n"));

  if (req ==  NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  loop = loop_create(&status, req->sessionId);
  if (!loop)
  {
    DPRINT(("Loopback layer is not present, status %d\n", status));
    resp->status = comm_STATUS_NETWORK_NOT_PRESENT;   // loop_RES_BAD;
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

  DPRINT(("@LOOP commTerm():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  loop_destroy(loop);

  resp->status = loop_RES_OK;

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
  // dpEnumTransports scans through the DLL file looking for this signature.
  // Since the DLL might be compressed, this record is duplicated in
  // a resource (wloopi.rc), which Lee's .dll compressor leaves alone.
  // Be careful when changing this structure to also change the copy in
  // wloopi.rc, and vice versa.  (Hmm, maybe they should be in an .h file...)
  //
  // dpEnumTransports will use the info to present a driver name
  // to the user, and discover driver properties.
  static comm_driverInfo_t commDriverInfo =
  {
    comm_DRIVER_SIGNATURE,
    sizeof(comm_driverInfo_t),
#ifdef NO_BROADCAST
    "Loopback (No Broadcast)",
    0x0101,
    (comm_DRIVER_IS_VISIBLE | comm_DRIVER_IS_FAST | comm_DRIVER_ALLOWS_GAMELIST | comm_DRIVER_NO_BROADCAST),
#else
    "Loopback",
    0x0101,
    (comm_DRIVER_IS_VISIBLE | comm_DRIVER_IS_FAST | comm_DRIVER_ALLOWS_GAMELIST),
#endif
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
  loop_result_t         err;

    LOOPADDR             adr;

  DPRINT(("@LOOP commPlayerInfo():\n"));

  if (req ==  NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  // This comm layer doesn't know about user names.
  resp->name = "";

  err = loop_hdl2adr(loop, commHdl2loop(req->player), &adr);
  if (err != loop_RES_OK)
  {
    resp->status = err;
    return FALSE;
  }

  // fill in response struct

  resp->address   = &adr;
  resp->addrLen   = sizeof(adr);
  memset(&resp->nodeID, 0, sizeof(resp->nodeID));
  resp->status    = loop_RES_OK;

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

  DPRINT(("@LOOP commTxFull():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = loop_RES_OK;

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
  loop_result_t err;
  loop_hdl_t    h;

  //DPRINT(("@LOOP commTxPkt():\n"));

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

  h = commHdl2loop(req->dest);

  err = loop_put(loop, req->buffer, req->length, h);
  resp->status = err;

  return (err == loop_RES_OK);
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

  DPRINT(("@LOOP commPeekPkt():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  // Not implemented
  resp->status = loop_RES_BAD;

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
  loop_result_t   err;
  loop_hdl_t      hdl;

  //DPRINT(("@LOOP commRxPkt(): "));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->length = req->size;

  err = loop_get( loop, req->buffer, &resp->length, &hdl, NULL );
  resp->src = loop2commHdl(hdl);
  //if (!err)
	  //DPRINT(("commRxPkt: got pkt, len %d, internal hdl %d, resp->src %x\n",
			  //resp->length, hdl, resp->src));

  // ignore packets from myself
  if (resp->src == PLAYER_ME)  {
	  err = loop_RES_EMPTY;
	  DPRINT(("commRxPkt: ignoring packet from myself\n"));
  }
  resp->status = err;

  return (err == loop_RES_OK);
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

  DPRINT(("@LOOP commScanAddr(%s):\n", req->printable));

  if (req->size < sizeof(LOOPADDR)) {
      resp->status = loop_RES_FULL;
      return FALSE;
  }

  i = sscanf(req->printable, "%d.%d.%d.%d",
             &tmpInt[0], &tmpInt[1], &tmpInt[2], &tmpInt[3]);

  if (i != 4) {
    resp->status = loop_RES_BAD;
    DPRINT(("IPX commScanAddr(%s): sscanf returned %d < 4", req->printable, i));
    return FALSE;
  }

  for (i = sizeof(LOOPADDR);  i--;) ((char *) req->address)[i]  = tmpInt[i];
  resp->length = sizeof(LOOPADDR);

  resp->status = loop_RES_OK;
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
  const unsigned char *raw;

  DPRINT(("@LOOP commPrintAddr():\n"));

  if (resp == NULL)  resp = &respDummy;
  if (!req || (req->length != sizeof(LOOPADDR))) {
	  resp->status = loop_RES_BAD;
	  return FALSE;
  }

  raw = (unsigned char *)req->address;
  sprintf(printable, "%d.%d.%d.%d",
            raw[0]&0xff,
            raw[1]&0xff,
            raw[2]&0xff,
            raw[3]&0xff);

  if (req->printable != NULL) {
    if (req->size < strlen(printable) + 1) {
      resp->status = loop_RES_FULL;
      return FALSE;
    }

    strcpy(req->printable, printable);
  }
  DPRINT(("@LOOP commPrintAddr(): result %s\n", printable));

  resp->status = loop_RES_OK;
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

  DPRINT(("@LOOP commGroupAlloc():\n"));

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

  DPRINT(("@LOOP commGroupFree():\n"));

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

  DPRINT(("@LOOP commGroupAdd():\n"));

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

  DPRINT(("@LOOP commSetParam():\n"));

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  resp->status = comm_STATUS_UNIMPLEMENTED;

  return FALSE;
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
  loop_hdl_t        h;

  LOOPADDR			params;
  LOOPADDR *		adr;
  void printaddr(LOOPADDR *addr, char *string);

  DPRINT(("@LOOP commSayHi():\n"));

  /* Sanity check arguments. */
  if (resp == NULL)  resp = &respDummy;
  if (!req || (req->length != sizeof(*adr))) {
	DPRINT(("@LOOP commSayHi: req->length %d != sizeof(*adr) %d!\n", req->length, sizeof(*adr)));
    resp->status = loop_RES_BAD;
    return FALSE;
  }

  /* Create an LOOPADDR from req->address and our own LOOPADDR. */
  adr = req->address;
  loop_hdl2adr(loop, loop_HDL_ME, &params);
  //printaddr(&params, "commSayHi me: ");
  params = *adr;
  printaddr(&params, "commSayHi: ");

  /* Look up the LOOPADDR in the table, or add it if it's not there yet. */
  h = loop_adr2hdl(loop, params, TRUE);
  if (h == loop_HDL_NONE) {
		DPRINT(("commSayHi: can't open handle.\n"));
		resp->status = loop_RES_BAD;
		return FALSE;
  }

  /* Convert the handle to one that fits commapi.h and return. */
  resp->player = loop2commHdl(h);
  DPRINT(("commSayHi: opened handle %d, translated to player %d.\n", h, resp->player));
  resp->status = loop_RES_OK;
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

  DPRINT(("@LOOP commSayBye():\n"));

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
