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

                         Comm API Shims for Serial Protocol

                              (c) 1997 By Activision

**************************************************************************************/

// 31-OCT-96  File modified to support Win32  [M. Livesay]

// Wrapper to adapt the simple packetizing serial driver to Zobel's comm API.
//
// $Log: serdll.c $
// Revision 1.7  1997/10/01 22:05:19  anitalee
// took out DPRINT redefinition
// Revision 1.6  1997/10/01 21:43:42  dkegel
// return ser error codes
// Revision 1.5  1997/03/10 06:08:15  dkegel
// 1. Jerry's unified serial/modem driver code.
// 2. Dan hardcoded serial driver to ignore baud rate, always use 38400.
// Revision 1.4  1997/01/31 02:50:03  dkegel
// Switched to ddprint.h
// Revision 1.3  1996/12/13 23:17:14  jgraebner
// Fixed invalid pointer reference.
// Revision 1.2  1996/12/13 00:16:28  dkegel
// nodeID now a byte array instead of a structure.  Not used, just zero.
// Revision 1.1  1996/12/12 22:09:44  dkegel
// Initial revision
// Revision 1.15  1996/09/06 23:05:24  dkegel
// Replaced commGroupSubtract with commSetParam.
// Revision 1.14  1996/07/01 20:52:18  dkegel
// Check sessionID during ramp check; catches open cables.
// Revision 1.13  1996/06/30 22:22:22  dkegel
// Loopback test now triggered by -Ktest
// Revision 1.12  1996/06/30 21:38:18  dkegel
// If -Kpulse is given, call exchange_ramp to do a loopback test.  Kludge.
// Both sides must click on 'accept' at same time for test to work.
// Test takes two seconds.
// Revision 1.11  1996/06/29 18:02:25  dkegel
// Support nonstandard IRQ's and base addresses.
// Revision 1.10  1996/06/29 00:58:40  dkegel
// 1. Added better status value and log message if request wrong size.
// 2. Turn log on.
// Revision 1.9  1996/06/26 20:37:28  dkegel
// Added log file (serial.log), but left it disabled.
// I will hand compile it for the few people who need it.
// Revision 1.8  1996/05/29 00:44:11  dkegel
// 1. Pass addresses to and from ser_put and ser_get.
// 2. Use ser handles wherever possible directly as player handles.
// 3. Finally implemented commSayBye.
// Revision 1.7  1996/05/22 00:46:49  dkegel
// Removed the word "driver" from dll description.
// Revision 1.6  1996/05/15 02:11:53  dkegel
// Use standard comm_STATUS codes in commInit.
// Revision 1.5  1996/05/14 03:06:04  dkegel
// Minor dprint's added.
// Revision 1.4  1996/04/30 21:00:12  dkegel
// Added "comm_DRIVER_IS_FAST" to capabilities on theory that
// speed will always be 19200 and there will only be two users.
// Revision 1.3  1996/03/06 23:46:38  dkegel
// Shell can't handle drivers that don't need phone numbers and don't allow
// gamelists, so turn on the allows_gamelist bit.
// Revision 1.2  1996/02/29 01:36:48  dkegel
// 1. Implemented commSayHi etc.
// 2. Implemented new dpEnumTransports capabilities signature.
// 3. Implemented new parameters to commInit.
// Revision 1.1  1996/02/25 04:50:57  dkegel
// Initial revision
//

/*************************************************************************************

                                    Literals

**************************************************************************************/

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <crtdbg.h>

#define DRIVER_LOG  // uncomment this to turn on logging.

#ifdef DRIVER_LOG
#define __ERROR -1    // return value of open()
#include <io.h>
#include <fcntl.h>
#endif

#define DEBUG_MODULE  !FALSE
#include "ddprint.h"

#include "ser.h"
#include "commapi.h"

#define DRIVER_VERSION    VERSION(1,0)  // Version of this comm driver

#define PLAYER_YOU 0      // What we translate ser_HDL_YOU to.

/*************************************************************************************

                                    Globals

**************************************************************************************/

static ser_t *ser = NULL;

#ifdef DRIVER_LOG
static int logfd = __ERROR;

/*************************************************************************************

                                 commHdlser()

**************************************************************************************/

static ser_hdl_t commHdlser(playerHdl_t h)

{
  switch (h)
  {
    case PLAYER_NONE:
    return ser_HDL_NONE;

    case PLAYER_ME:
    return ser_HDL_ME;

    case PLAYER_BROADCAST:
    return ser_HDL_BROADCAST;

    case PLAYER_UNKNOWN:
    return ser_HDL_NONE;

    default:
    break;
  }

  return ((ser_hdl_t) h);
}

/*************************************************************************************

                                    fdclose()

**************************************************************************************/

static void fdclose(void)
{
  if (logfd != __ERROR)  close(logfd);
}

/*************************************************************************************

                                    fdprint()

**************************************************************************************/

void fdprint(char *msg)
{
  if (logfd != __ERROR)  write(logfd, msg, strlen(msg));
}
#endif

/*************************************************************************************

                                    commNoOp()

**************************************************************************************/

/*
 *  Do nothing.
 *
 *  Return FALSE on error.
 */

int cdecl commNoOp( commNoOpReq_t *req, commNoOpResp_t *resp)

{
  DPRINT(("commNoOp(): \n"));
  resp->status = ser_RES_OK;
  (void) req;
  (void) resp;

  return(TRUE);
}

/*-------------------------------------------------------------------------
 Return true if the given com port is available.
-------------------------------------------------------------------------*/
static int commPortIsAvailable(const char *name)
{
	SECURITY_ATTRIBUTES SecurityAttributes;
	HANDLE h;

	memset(&SecurityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
	SecurityAttributes.nLength = sizeof( SECURITY_ATTRIBUTES );

	h = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
		0,                    // exclusive access
		&SecurityAttributes,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE != h) {
		CloseHandle(h);
		return TRUE;
	}
	return FALSE;
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
	int i;
	int nports;
	commPortName_t *pPort;

	if (!resp) {
		DPRINT(("commEnumPorts: bug: resp is null!\n"));
		return FALSE;
	}
	if (!req) {
		DPRINT(("commEnumPorts: bug: req is null!\n"));
		resp->numports = 0;
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	if (req->reqLen != sizeof(commEnumPortsReq_t)) {
		DPRINT(("commEnumPorts: bug: req is wrong size!\n"));
		resp->numports = 0;
		resp->status = comm_STATUS_BAD_VERSION;
		return FALSE;
	}

	nports = 0;
	pPort = req->buf;

	/* list available com ports */
	for (i=0; (i<10) && (nports < req->maxports); i++) {
		char			szPort[ 15 ];
		sprintf( szPort, "%s%d", "COM", i + 1) ;

		if (commPortIsAvailable(szPort)) {
			pPort->portnum = i;
			strncpy(pPort->name, szPort, sizeof(pPort->name));
			pPort->name[sizeof(pPort->name)-1] = 0;
			nports++;
			pPort++;
		}
	}
	resp->numports = nports;
	resp->status = comm_STATUS_OK;
	DPRINT(("commEnumPorts: %d port(s) found.\n", nports));
	return TRUE;
}

/*************************************************************************************

                                    commInit()

**************************************************************************************/

/*
 *  Initialize the communications driver.
 *
 *  Return FALSE on error.
 */

int cdecl commInit(commInitReq_t * req, commInitResp_t * resp)
{
	commInitResp_t respDummy;
	char			szPort[ 15 ];

	if (resp == NULL)
		resp = &respDummy;

	if (!req) {
		DPRINT(("commInit: bug: req is null!\n"));
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	if (req->reqLen != sizeof(commInitReq_t)) {
		DPRINT(("commInit: bug: req is wrong size!\n"));
		resp->status = comm_STATUS_BAD_VERSION;
		return FALSE;
	}
	ser = ser_create();
	if (!ser) {
		DPRINT(("commInit: bug: ser_create failed\n"));
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	if (ser_adr2hdl(ser, &req->sessionId, TRUE) != ser_HDL_ME) {
		DPRINT(("commInit: bug: ser_adr2hdl failed!\n"));
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	sprintf( szPort, "%s%d", "COM", req->portnum + 1) ;
	/* Hard code baud rate.  No sense in users having to guess right speed. */
	if ((resp->status = ser_config(ser, 38400, szPort)) != ser_RES_OK) {
		DPRINT(("commInit: bug: ser_config failed!\n"));
		return FALSE;
	}
	resp->status = comm_STATUS_OK;
	DPRINT(("commInit: return TRUE successfully!\n"));
	return TRUE;
}


/*************************************************************************************

                                    commTerm()

**************************************************************************************/

/*
 *  Tear down the communications driver.
 *
 *  Return FALSE on error.
 */

int cdecl commTerm(commTermReq_t *req, commTermResp_t *resp)

{
  if (ser)
  {
    ser_destroy(ser);
    ser = NULL;
  }

  (void) req;
  (void) resp;
  return(TRUE);
}


/*************************************************************************************

                              commDriverInfo()

**************************************************************************************/

/*
 *  Retrieve info about the communications driver.
 *
 *  Return TRUE if info was retrieved.
 */

int cdecl commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp)

{
  // We plan to make dpEnumTransports scan through the DLL file looking
  // for this signature.  It will use the info to present a driver name
  // to the user, and discover driver properties such as "expects to be
  // called by external shell (e.g. dwango)".
  static comm_driverInfo_t commDriverInfo =
  {
    comm_DRIVER_SIGNATURE,
    sizeof(comm_driverInfo_t),
    "Null Modem",
    0x0100,
    (comm_DRIVER_IS_VISIBLE | comm_DRIVER_IS_FAST | comm_DRIVER_ALLOWS_GAMELIST),
    (comm_INIT_NEEDS_SESSIONID | comm_INIT_NEEDS_PORTNUM | comm_INIT_NEEDS_BAUD)
	/* bug: hard codes baud - shouldn't set 'needs baud' */
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

/*
 *  Retrieve info about a player, including ourselves.
 *
 *  Return TRUE if the player is known and is not a group.
 *
 *  Given a player handle returned by commSayHi, you can get back
 *  which network address it is.  The network address is portable-
 *  that is, it must be unique among all computers currently connected,
 *  and it must retain its meaning when put in a packet and sent to
 *  another station.
 */

int cdecl commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp)

{
  static ser_adr_t kludgeAdr;
  ser_hdl_t h;

  if (resp == NULL) return(FALSE);
  if (req  == NULL) {resp->status = ser_RES_BAD; return(FALSE);}

  if (req->player == PLAYER_ME)
	  h = ser_HDL_ME;
  else if (req->player == PLAYER_YOU)
	  h = ser_HDL_YOU;
  else
  {
	  resp->status = 1;
	  return FALSE;
  }
  // index into nodes table and fill in response
  if ((resp->status = ser_hdl2adr(ser, h, &kludgeAdr)) != ser_RES_OK)
  {
    return FALSE;
  }

  resp->status    = 0;
  resp->name      = NULL;
  resp->address   = &kludgeAdr;
  resp->addrLen   = sizeof(ser_adr_t);
  memset(&resp->nodeID, 0, sizeof(resp->nodeID));

  return(TRUE);
}

/*************************************************************************************

                                   commTxFull()

**************************************************************************************/

/*
 *  Find out whether the transmit queue is full.
 *
 *  Return TRUE if no more packets can be queued for transmission at this
 *  time.
 */

int cdecl commTxFull(commTxFullReq_t *req, commTxFullResp_t *resp)

{
  commTxFullReq_t   reqDummy;
  commTxFullResp_t  respDummy;

  if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  // Not implemented.
  resp->status = 1;
  return(FALSE);
}

/*************************************************************************************

                                   commTxPkt()

**************************************************************************************/

/*
 *  Send a packet.  Upon return, the buffer can be discarded, although the
 *  packet may not be sent until later.
 *
 *  Return FALSE on error.  Note that a TRUE return value does not guarantee
 *  that the packet has been (or ever will be) sent.
 */

int cdecl commTxPkt(commTxPktReq_t *req, commTxPktResp_t *resp)

{
  commTxPktResp_t   respDummy;

  if (resp == NULL) resp = &respDummy;
  if (req == NULL)
  {
    resp->status = 3;
    return FALSE;
  }

  // Abort if no good place to send packet.
  if (req->dest == PLAYER_ME)
  {
    resp->status = 1;
    return FALSE;
  }

  resp->status = ser_put(ser, req->buffer, req->length);

  return(resp->status == ser_RES_OK);
}

/*************************************************************************************

                                   commPeekPkt()

**************************************************************************************/

/*
 *  Get information about a pending incoming packet.
 *
 *  Return TRUE if a packet was retrieved.
 */

int cdecl commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp)

{
  commPeekPktReq_t  reqDummy;
  commPeekPktResp_t respDummy;

  if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  resp->status = 1;
  return(FALSE);
}

/*************************************************************************************

                                   commRxPkt()

**************************************************************************************/

/*
 *  Retrieve a pending incoming packet.
 *
 *  Return TRUE if a packet was retrieved.
 */

int cdecl commRxPkt(commRxPktReq_t *req, commRxPktResp_t *resp)

{
  commRxPktReq_t    reqDummy;
  commRxPktResp_t   respDummy;
  ser_result_t      res;

  if (req  == NULL) req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  resp->length = req->size;

  res = ser_get(ser, req->buffer, &resp->length);

  resp->status = res;     // Fortunately, ser_RES_OK == 0
  resp->src = PLAYER_YOU;
  return (resp->status == ser_RES_OK);
}

/*************************************************************************************

                              commScanAddr()

**************************************************************************************/

/*
 *  Attempt to parse a NUL-terminated address string into a free-format
 *  buffer.
 *
 *  Return TRUE if the string was parsed successfully.
 */

int cdecl commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp)

{
  commScanAddrReq_t  reqDummy;
  commScanAddrResp_t respDummy;
  ser_adr_t          adr;

  if (req  == NULL) req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  // convert into our internal address
  if (sscanf(req->printable, "SERIAL:%x", &adr) != 1) {resp->status = 1; return(FALSE);}

  resp->length = sizeof(adr);

  if (req->address != NULL) memcpy(req->address, &adr, min(req->size, resp->length));

  resp->status = 0;
  return (TRUE);
}

/*************************************************************************************

                              commPrintAddr()

**************************************************************************************/

/*
 *  Attempt to format a free-format address buffer into a NUL-terminated
 *  string.
 *
 *  Return TRUE if the buffer was formatted successfully.
 */

int cdecl commPrintAddr( commPrintAddrReq_t *req, commPrintAddrResp_t *resp)

{
  commPrintAddrReq_t  reqDummy;
  commPrintAddrResp_t respDummy;
  char printable[50];

  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  if ((req->length != sizeof(ser_adr_t)) || (req->address == NULL))
  {
    DPRINT(("length != sizeof(ser_adr_t) or req-address == NULL\n"));
    resp->status = 1;
    return(FALSE);
  }

  sprintf(printable, "SERIAL:%x", *(ser_adr_t *) req->address);

  if (req->printable != NULL)
  {
    if (req->size < strlen(printable) + 1)
    {
      resp->status = 2;
      return(FALSE);
    }

    strcpy(req->printable, printable);
  }

  resp->status = 0;
  return(TRUE);
}

/*************************************************************************************

                              commGroupAlloc()

**************************************************************************************/

/*
 *  Generate a pseudo-player handle referring to a group of players.  Handy
 *  for multicasting.  A group can have zero players.
 *
 *  Return TRUE if the pseudo-player handle was generated.
 */

int cdecl commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp)

{
  req  = req;
  resp = resp;

  return(FALSE);
}

/*************************************************************************************

                              commGroupFree()

**************************************************************************************/

/*
 *  Invalidate a pseudo-player handle referring to a group of players.
 *
 *  Return TRUE if the pseudo-player handle was invalidated.
 */

int cdecl commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp)

{
  req = req;
  resp = resp;

  return (FALSE);
}

/*************************************************************************************

                              commGroupAdd()

**************************************************************************************/

/*
 *  Add one or more players to a group.
 *
 *  Return TRUE if the players were all added.
 */

int cdecl commGroupAdd(commGroupAddReq_t *req, commGroupAddResp_t *resp)

{
  req = req;
  resp = resp;

  return(FALSE);
}

/*************************************************************************************

                              commSetParam()

**************************************************************************************/

/*
 *  Set driver parameters.
 */

int cdecl commSetParam(commSetParamReq_t * req,  commSetParamResp_t *resp)

{
	commSetParamReq_t   reqDummy;
	commSetParamResp_t  respDummy;

	if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)  resp = &respDummy;

	switch (req->param_num) {
	case comm_PARAM_DPRINTF:
		dp_dprintf_set((dp_dprintf_t) req->param_value);
		break;
	default:
		resp->status = comm_STATUS_UNIMPLEMENTED;
		return FALSE;
	}

	DPRINT(("commSetParam: set param 0x%x to 0x%x\n", req->param_num, req->param_value));
	resp->status = 0;
	return TRUE;
}

/*************************************************************************************

                               commSayHi()

**************************************************************************************/

/*
 *  Establish a data link to a player and shake hands with him.  This does
 *  not actually establish a connection in the IP sense of the word.
 *
 *  Return TRUE if the link was established and we shook hands.
 *
 *  req->address is a ser_adr_t, which is just a random long int.
 *
 */

int cdecl commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp)
{
  commSayHiReq_t  reqDummy;
  commSayHiResp_t respDummy;
  ser_hdl_t       h;

  if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  if (req->length == 0)
  {
	  resp->status = 1;
	  return FALSE;
  }
  h = ser_adr2hdl(ser, req->address, TRUE);
  if (h == ser_HDL_ME)
  {
    resp->player = PLAYER_ME;
    resp->status = 0;
    return TRUE;
  }

  if (h == ser_HDL_YOU)
  {
    resp->player = PLAYER_YOU;
    resp->status = 0;
    return TRUE;
  }
  resp->status = 2;

  return FALSE;
}

/*************************************************************************************

                               commSayBye()

**************************************************************************************/

/*
 *  Tear down a data link to a player.  The link or the player may already be
 *  down, so don't shake hands.
 *
 *  Return TRUE if the link was successfully broken.
 */

int cdecl commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp)

{
  commSayByeResp_t  respDummy;
  ser_hdl_t         h;

  if (resp == NULL) resp = &respDummy;
  if (req  == NULL) {resp->status = 1; return FALSE;}

  if (req->player == PLAYER_ME)
	  h = ser_HDL_ME;
  else
	  h = req->player;
  resp->status = 0;
  return (TRUE);
}

/*************************************************************************************

                               DllMain()

**************************************************************************************/

BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)

{
  switch (fdwReason)
  {
    case DLL_PROCESS_ATTACH:
      // do any startup stuff here
    return(TRUE);

    case DLL_PROCESS_DETACH:
      // do any shutdown stuff here
    return(TRUE);

    case DLL_THREAD_ATTACH:
    return(TRUE);

    case DLL_THREAD_DETACH:
    return(TRUE);
  }

  return(TRUE);
}
