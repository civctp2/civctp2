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

/*
 *	CommAPI.h
 *
 *	API to access network MW2 communications.
 *
 * $Log: commapi.h $
 * Revision 1.38  1997/10/03 00:32:41  anitalee
 * set some predef's for commInitReq.dialing_method
 * Revision 1.37  1997/09/03 18:09:19  lbennett
 * Changed value for comm_PORT_ANY to reflect usage.
 * Revision 1.36  1997/08/31 21:53:54  lbennett
 * Fixed it.
 * Revision 1.35  1997/08/31 02:52:51  lbennett
 * Added adr field to receive packet struct.
 * Revision 1.34  1997/08/26 02:24:02  dkegel
 * Bump DLL compatibility signiture, too.
 * Revision 1.33  1997/08/26 00:36:55  dkegel
 * Bump up one port number if dp_ANET2
 * Revision 1.32  1997/08/22 23:24:28  lbennett
 * Added value for comm_PORT_ANY
 * Revision 1.31  1997/05/31 22:42:18  dkegel
 * Moved 'pragma pack' to dp*pack*.h for portability.
 * Revision 1.30  1997/05/29 19:54:50  dkegel
 * Added initial mac support.
 * Revision 1.29  1997/05/21 19:40:13  dkegel
 * No longer hope someone has already defined u16 and u8.
 * Revision 1.28  1997/05/12 19:05:26  anitalee
 * fixed problem with alignment of structures for SUNOS and PowerMac
 * Revision 1.27  1997/03/05 02:39:04  dkegel
 * ini.h and dprint.h now integrated into anet.h;
 * to compile or use DP as a DLL, define DP_DLL;
 * win\build.bat now creates debugging and nondebugging versions
 * of both the static dp.lib and the dynamic anetdll.dll.
 * Revision 1.26  1997/02/27 01:49:16  dkegel
 * THE BIG REARRANGEMENT: split public API into anet.h; dp.h and dppkt.h
 * remain for those who need the full API including the real dp_t;
 * care must be taken to avoid circular includes and to avoid activating
 * too much of anet.h when using the full API.
 * Revision 1.25  1997/02/12 04:58:28  dkegel
 * Added commapi_NET_PORTOFFSET for use by DLL's in picking port number.
 * Since protocol changed, updated both commapi_NET_PORTOFFSET
 * and comm_DRIVER_SIGNATURE so old programs and new won't use each
 * other's DLL's or talk to each other on the net.
 * Revision 1.24  1997/02/06 23:15:55  dkegel
 * Moved error codes into commerr.h for convenience of drivers.
 * Changed signature.
 * Revision 1.23  1997/01/31 07:47:38  dkegel
 * Settled the structure packing question by using
 * #pragma pack(x) / #pragma pack() pairs at top and bottom of
 * all .h files that declare structures.  Was careful to put
 * #pragma pack(x) after last #include (to avoid wierd nesting effects
 * due to lack of #pragma pack(pop).
 * Revision 1.22  1997/01/11 01:35:28  jgraebner
 * Added comment about windows packing.
 * Revision 1.21  1996/12/18 18:20:22  administrator
 * Change to c style comments for benefit of gcc.
 * Revision 1.20  1996/12/18 18:13:59  administrator
 * 1. Changed playerHdl_t to a long so it can hold an internet address!
 * 2. Changed PLAYER_BROADCAST to internet broadcast address.
 * 3. Changed comm_DRIVER_SIGNATURE so old programs don't try to load
 * new drivers, and vice versa.
 * Revision 1.19  1996/12/13 16:02:31  dkegel
 * Added definitions for commInitReq_t.portnum values COM1 etc.
 * Revision 1.18  1996/12/13 00:01:39  dkegel
 * Now supports Windows.
 * Revision 1.17  1996/12/07 01:25:58  dkegel
 * No longer depend on userID.h or nodeID_t.
 * Revision 1.16  1996/09/25 22:43:18  dkegel
 * added comm_STATUS_HOST_UNREACHABLE for internet driver.
 * Revision 1.15  1996/09/06 22:50:56  dkegel
 * Replaced commGroupSubtract with commSetParam.
 * Revision 1.14  1996/08/15 01:20:08  dkegel
 * Added NO_BROADCAST capability to indicate internet driver.
 * Revision 1.13  1996/07/05 01:15:02  dkegel
 * Needed to include stdlib.h because we use size_t in our interface.
 * Revision 1.12  1996/06/30 22:23:18  dkegel
 * Added definitions for bits of commInitReq_t.flags.
 * Revision 1.11  1996/06/29 17:58:17  dkegel
 * Incremented driver signature belatedly to celebrate the change in
 * size of commInitReq_t.
 * Revision 1.10  1996/06/25 20:07:17  dkegel
 * Added new error status: comm_STATUS_BAD_VERSION.
 * Revision 1.9  1996/05/31 02:24:17  dkegel
 * Added comment describing abuse of commGroupAdd to support dpReportScore.
 * Revision 1.8  1996/05/21 21:09:36  dkegel
 * Added network specific error message, so network drivers don't
 * produce modem errors.
 * Revision 1.7  1996/05/15 02:13:00  dkegel
 * Added standard comm_STATUS codes.
 * Revision 1.6  1996/05/11 18:17:35  dkegel
 * 1. No longer do DLL's import atexit and onexit from the main .exe.
 * They have their own in dlllib.c.
 * DLL's now export new function, exit_cleanup, from dlllib.c to the
 * main .exe (to be called when unloading a dll).
 * .DLL signature in commapi.h changed to reflect broken compatibility.
 * Revision 1.5  1996/05/09 02:08:46  dkegel
 * Added flags paramter to commInit and commTerm to control dialing/hangup.
 * Revision 1.4  1996/05/06 17:09:16  dkegel
 * Added comm_DRIVER_PREFERS_BROADCAST for dwango's benefit.
 * Revision 1.3  1996/04/30 21:01:10  dkegel
 * Added "comm_DRIVER_IS_FAST" to capabilities for drivers who
 * expect to provide enough bandwidth for five updates a second.
 * Revision 1.2  1996/02/29 01:38:49  dkegel
 * 1. Added new dpEnumTransport signature capabilities.
 * 2. Added new parameters to commInit.
 * Revision 1.1  1996/02/20 05:17:39  dkegel
 * Initial revision
 *
 *	10/12/95	dz
 *		Parts split out into CDLLHdr.h.
 *	8/30/95		dz
 *		Initial rev.
 */

#ifdef _WINDOWS
  #include <windows.h>

  #ifdef DYNALINK
    #define DLLIMPORT  __declspec(dllimport)
    #define DLLEXPORT  __declspec(dllexport)
  #else
    #define DLLIMPORT
    #define DLLEXPORT
  #endif
#else
  #define DLLIMPORT
  #define DLLEXPORT
#endif

#ifndef _CommAPI_h_
#define _CommAPI_h_

#include <stdlib.h>		/* for size_t */
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------- commapi types & constants ----------------------------*/
/* For commInitReq_t.portnum */
#define COM1  (int)0
#define COM2  1
#define COM3  2
#define COM4  3
#define COM5  4
#define COM6  5
#define COM7  6
#define COM8  7

/* For drivers which have a port number parameter:
 * add this constant to your default port.
 * Increment this when it is desired to break backwards compatibility.
 * (There must be five of these little version numbers... sorry to add
 * another one...)
 * See also comm_DRIVER_SIGNATURE
 */
#ifdef dp_ANET2
#define commapi_NET_PORTOFFSET 3
#else
#define commapi_NET_PORTOFFSET 2
#endif

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.  Let's pick 1.
 * Note: Pragma pack must come after all other #include's!
 */
#include "dppack1.h"

typedef	unsigned short	version_t;			/* Version number (major.minor) */
#define VERSION(maj,min)	((unsigned short)(unsigned char)(maj)<<8 | (unsigned char) (min))

/* Player handles now longs to hold an entire Internet address; */
/* other transports will continue to use them as small integer handles. */
/* PLAYER_BROADCAST value changed to match Internet broadcast address. */
typedef unsigned long	playerHdl_t;		/* Handle for a player or group */
#define	PLAYER_NONE      ((playerHdl_t) 0xfffffffc) /* -4 No player */
#define	PLAYER_UNKNOWN   ((playerHdl_t) 0xfffffffd) /* -3 Unknown player */
#define	PLAYER_ME        ((playerHdl_t) 0xfffffffe) /* -2 This player */
#define	PLAYER_BROADCAST ((playerHdl_t) 0xffffffff) /* -1 All players within earshot */

#if defined(UNIX) && !defined(COMM_INST)
#define COMM_INST
#endif

/* Macro to allow older drivers to compile */
#ifdef COMM_INST
#define COMM_3RDPARAM ,void *commPtr
#else
#define COMM_3RDPARAM
#endif

/* Values for status field of result structures. */
#include "commerr.h"

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Warning: anet.h includes a copy of the following information */

/* The largest possible address */
#define comm_MAX_ADR_LEN 10	/*  Size of ipx's fullNetworkAddr_t. */

/*
 * commAlloc
 *
 * commAlloc is not used by all drivers at this stage but eventually,
 * they will be converted.  This functions is used to convert
 */
typedef struct {					/* Request (filled in by caller) */
	void **ptr;						/* Pointer to pointer to data area */
} commAllocReq_t;

typedef struct {					/* Request (filled in by caller) */
	comm_status_t		status;		/* Command status */
} commAllocResp_t;

#if COMM_FNPTRS
typedef int
(*COMMALLOC)(
	commAllocReq_t *		req,	/* Request (or NULL) */
	commAllocResp_t *	resp)		/* Response (or NULL) */
;
extern COMMALLOC commAlloc;
#else
DLLEXPORT int cdecl
commAlloc(
	commAllocReq_t *		req,	/* Request (or NULL) */
	commAllocResp_t *	resp)		/* Response (or NULL) */
;
#endif

#ifdef UNIX
int validateComm(void *comm);
#endif

/*--------------------------------------------------------------------------
 Information available before loading driver:

 Programs need to know things about a driver before they load it.  For
 instance, they need to know what parameters the driver's init routine
 needs, whether the driver should be visible in a list of drivers presented
 to the user, and they need the long name of the driver.
 This info is made availible by putting it in a structure prefixed with a
 signature which can be read directly out of the file by dpEnumTransports.
 Increment this signature each time you break backward compatibility,
 to prevent users from trying to load a DLL that is incompatible.

 commInit will return a copy of the same structure to the user program.
--------------------------------------------------------------------------*/
#ifdef dp_ANET2
#define comm_DRIVER_SIGNATURE		"AVKEGL8"	/* length must be multiple of 4 */
#else
#define comm_DRIVER_SIGNATURE		"AVKEGL7"	/* length must be multiple of 4 */
#endif
#define comm_DRIVER_SIGLEN			8
#define comm_DRIVER_NAMELEN			32

typedef struct {
	char signature[comm_DRIVER_SIGLEN];
	size_t recordLen;				/* sizeof(comm_driverInfo_t) */
	char name[comm_DRIVER_NAMELEN];	/* Name to present to user */
	short version;					/* Major, minor rev. in high, low byte */
	short capabilities;				/* What driver can do/wants to do */
	short needs;					/* What fields in commInitReq_t to fill in */
} comm_driverInfo_t;

/* Special value for commInitReq_t.portnum */
#define comm_PORT_ANY	(0)			/* let NOS pick a port number */

/* Bits of comm_driverInfo_t.capabilities */
#define comm_DRIVER_IS_VISIBLE		1		/* Whether to show driver to user */
#define comm_DRIVER_KNOWS_PLAYERLST	2		/* Whether to call dpCommThaw */
#define comm_DRIVER_ALLOWS_GAMELIST	4		/* Whether to call dpEnumSessions */
#define comm_DRIVER_IS_FAST			8		/* Whether to set update rate down */
#define comm_DRIVER_PREFERS_BROADCAST	16	/* Whether to use broadcast for frequent data */
#define comm_DRIVER_NO_BROADCAST	32		/* Whether to avoid broadcast entirely */
#define comm_DRIVER_JUMBO_PACKETS	64		/* Can handle 520 byte packets */

/* Bits of comm_driverInfo_t.needs */
#define comm_INIT_NEEDS_SESSIONID	2
#define comm_INIT_NEEDS_PORTNUM		4
#define comm_INIT_NEEDS_BAUD		8
#define comm_INIT_NEEDS_BASEADR		0x10
#define comm_INIT_NEEDS_HWIRQ		0x20
#define comm_INIT_NEEDS_SWINT		0x40
#define comm_INIT_NEEDS_PHONENUM	0x80
#define comm_INIT_NEEDS_MODEMINISTR	0x100
/*	Needs added for Mac version */
#define	comm_INIT_NEEDS_UI						0x1000
#define	comm_INIT_NEEDS_SESSION_NAME	0x2000

/* Bits of commInitReq_t.flags */
#define comm_INIT_FLAGS_RESUME		0x01		/* modem connection already estab. */
#define comm_INIT_FLAGS_TEST		0x02		/* perform loopback test on init if not resuming */
#define comm_INIT_FLAGS_CONN_ADDR	0x04		/* dialing_method is a pointer to a NULL terminated list of addresses */
												/* kludge ... this is used by DPIO */

/* Bits of commInitReq_t.dialing_method */
#define comm_INIT_DIALING_METHOD_PULSE 0
#define comm_INIT_DIALING_METHOD_TONE  1

typedef struct {			/* Request (filled in by caller) */
	size_t	reqLen;			/* Sizeof(commInitReq_t) */
	long 	sessionId;		/* Random number chosen by stub, same for shell and sim. */
	long	portnum;
	long	baud;
	long	baseadr;		/* ignored by Windows */
	long	hwirq;			/* ignored by Windows */
	long	swint;			/* ignored by Windows */
	char	*phonenum;
	char	*modeministr;
	long	flags;			/* controls whether to dial and/or test */
	long	dialing_method;	/* parameter to HMSetDialingMethod */
} commInitReq_t;

typedef struct {
	int portnum;			/* Value for commInitReq->portnum (e.g. 0) */
	char name[64];			/* Name to present to user (e.g. COM1) */
} commPortName_t;

/* Warning: anet.h includes a copy of the above information */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*----------------- commapi function parameters ---------------------------*/
/* If using Mac shared library, declare function pointer type and
 * an instance rather than a function prototype.
 */
#if (defined(__MWERKS__) && !defined(BUILDTRANSPORTCFM))
#define COMM_FNPTRS 1
#else
#define COMM_FNPTRS 0
#endif

/*
 *	Spin your wheels -- and get paid for it!
 *
 *	Return FALSE on error.
 */

typedef struct {			/* Request (filled in by caller) */
	char			dummy;		/* Dummy */
} commNoOpReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commNoOpResp_t;

#if COMM_FNPTRS
typedef int
(*COMMNOOP)(
	commNoOpReq_t *		req,		/* Request (or NULL) */
	commNoOpResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMNOOP commNoOp;
#else
DLLEXPORT int cdecl
commNoOp(
	commNoOpReq_t *		req,		/* Request (or NULL) */
	commNoOpResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*-------------------------------------------------------------------------
 List the available comm ports.
 Implemented only for serial and modem drivers.
-------------------------------------------------------------------------*/

typedef struct {			/* Request (filled in by caller) */
	size_t	reqLen;			/* Sizeof(commInitReq_t) */
	int maxports;			/* Number of elements in buf */
	commPortName_t *buf;	/* Caller must allocate array to fill */
} commEnumPortsReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t status;	/* Command status */
	int numports;			/* Number of elements of req->buf used */
} commEnumPortsResp_t;

#if COMM_FNPTRS
typedef int
(*COMMENUMPORTS)(
	commEnumPortsReq_t *	req,		/* Request (or NULL) */
	commEnumPortsResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
extern COMMENUMPORTS commEnumPorts;
#else
DLLEXPORT int cdecl
commEnumPorts(
	commEnumPortsReq_t *	req,		/* Request (or NULL) */
	commEnumPortsResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif

/*
 *	Initialize the communications driver.
 *  The pre-executable chooses the sessionId, and passes it to both the shell
 *  and the sim on the command line.  This is for the benefit of the null
 *  modem and modem drivers, so they have an address to call their own
 *  in commPlayerInfo.  That's the price of writing everything in terms of
 *  a LAN -- you get so used to having addresses around, that you need to
 *  support them even when they don't really make much sense.
 *
 *	Return FALSE on error.
 */

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commInitResp_t;

#if COMM_FNPTRS
typedef int
(*COMMINIT)(
	commInitReq_t *		req,		/* Request (or NULL) */
	commInitResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMINIT commInit;
#else
DLLEXPORT int cdecl
commInit(
	commInitReq_t *		req,		/* Request (or NULL) */
	commInitResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*
 *	Tear down the communications driver.
 *
 *	Return FALSE on error.
 */

typedef struct {			/* Request (filled in by caller) */
	long flags;					/* if non-zero, do not hang up */
} commTermReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commTermResp_t;

#if COMM_FNPTRS
typedef int
(*COMMTERM)(
	commTermReq_t *		req,		/* Request (or NULL) */
	commTermResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMTERM commTerm;
#else
DLLEXPORT int cdecl
commTerm(
	commTermReq_t *		req,		/* Request (or NULL) */
	commTermResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*
 *	Retrieve info about the communications driver.
 *
 *	Return TRUE if info was retrieved.
 */

typedef struct {			/* Request (filled in by caller) */
	char			dummy;		/* Dummy */
} commDriverInfoReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
	comm_driverInfo_t *info;
} commDriverInfoResp_t;

#if COMM_FNPTRS
typedef int
(*COMMDRIVERINFO)(
	commDriverInfoReq_t *	req,  	/* Request (or NULL) */
	commDriverInfoResp_t *	resp 	/* Response (or NULL) */
	COMM_3RDPARAM)				  	/* comm context pointer */
;
extern COMMDRIVERINFO commDriverInfo;
#else
DLLEXPORT int cdecl
commDriverInfo(
	commDriverInfoReq_t *	req,  	/* Request (or NULL) */
	commDriverInfoResp_t *	resp 	/* Response (or NULL) */
	COMM_3RDPARAM)		 		  	/* comm context pointer */
;
#endif

/*
 *	Retrieve info about a player, including ourselves.
 *
 *	Return TRUE if the player is known and is not a group.
 */

/* Bits of commPlayerInfoResp_t.flags */
#define comm_PLAYERINFO_2NDLIVE		0x01	/* Second address is live */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t		player;		/* Player handle; PLAYER_ME = this user */
} commPlayerInfoReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t	status;		/* Command status */
	char *			name;		/* Player name (NUL-terminated) */
	void *			address;	/* (in format returned by commScanAddr) address data */
	size_t			addrLen;	/* Length of address data */
	char			nodeID[8];	/* Unique identifier (unused) */
	void *			address2;	/* Secondary address (optional, may be NULL) */
	long			flags;		/* Flags */
} commPlayerInfoResp_t;

#if COMM_FNPTRS
typedef int
(*COMMPLAYERINFO)(
	commPlayerInfoReq_t *  	req,		/* Request (or NULL) */
	commPlayerInfoResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				 		/* comm context pointer */
;
extern COMMPLAYERINFO commPlayerInfo;
#else
DLLEXPORT int cdecl
commPlayerInfo(
	commPlayerInfoReq_t *	req,		/* Request (or NULL) */
	commPlayerInfoResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif

/*
 *	Find out whether the transmit queue is full.
 *
 *	Return TRUE if no more packets can be queued for transmission at this
 *	time.
 */

typedef struct {			/* Request (filled in by caller) */
	char			dummy;		/* Dummy */
} commTxFullReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commTxFullResp_t;

#if COMM_FNPTRS
typedef int
(*COMMTXFULL)(
	commTxFullReq_t *  	req,		/* Request (or NULL) */
	commTxFullResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMTXFULL commTxFull;
#else
DLLEXPORT int cdecl
commTxFull(
	commTxFullReq_t * 	req,		/* Request (or NULL) */
	commTxFullResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*
 *	Send a packet.  Upon return, the buffer can be discarded, although the
 *	packet may not be sent until later.
 *
 *	Return FALSE on error.  Note that a TRUE return value does not guarantee
 *	that the packet has been (or ever will be) sent.
 */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t		dest;		/* Receiving player's handle */
	void *			buffer;		/* Packet buffer */
	size_t			length;		/* Length of packet */
	unsigned long	flags;		/* Packet flags */
} commTxPktReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commTxPktResp_t;

#if COMM_FNPTRS
typedef int
(*COMMTXPKT)(
	commTxPktReq_t * 	req,		/* Request (or NULL) */
	commTxPktResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMTXPKT commTxPkt;
#else
DLLEXPORT int cdecl
commTxPkt(
	commTxPktReq_t *	req,		/* Request (or NULL) */
	commTxPktResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif


/*
 *	Get information about a pending incoming packet.
 *
 *	Return TRUE if a packet was retrieved.
 */

typedef struct {			/* Request (filled in by caller) */
	void *			buffer;		/* Space to copy packet into (or NULL) */
	size_t			size;		/* Size of packet buffer */
} commPeekPktReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
	playerHdl_t		src;		/* Sending player's handle */
	size_t			length;		/* Length of packet */
} commPeekPktResp_t;

#if COMM_FNPTRS
typedef int
(*COMMPEEKPKT)(
	commPeekPktReq_t *	req,		/* Request (or NULL) */
	commPeekPktResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMPEEKPKT commPeekPkt;
#else
DLLEXPORT int cdecl
commPeekPkt(
	commPeekPktReq_t *	req,		/* Request (or NULL) */
	commPeekPktResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*
 *	Retrieve a pending incoming packet.
 *
 *	Return TRUE if a packet was retrieved.
 */

typedef struct {			/* Request (filled in by caller) */
	void *			buffer;		/* Space to copy packet into (or NULL) */
	size_t			size;		/* Size of packet buffer */
} commRxPktReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t status;		/* Command status */
	playerHdl_t src;			/* Sending player's handle */
	size_t length;				/* Length of packet */
	unsigned char adr[comm_MAX_ADR_LEN];/* Sending player's observed address */
} commRxPktResp_t;

#if COMM_FNPTRS
typedef int
(*COMMRXPKT)(
	commRxPktReq_t *	req,		/* Request (or NULL) */
	commRxPktResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMRXPKT commRxPkt;
#else
DLLEXPORT int cdecl
commRxPkt(
	commRxPktReq_t *	req,		/* Request (or NULL) */
	commRxPktResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*
 *	Attempt to parse a NUL-terminated address string into a free-format
 *	buffer.
 *
 *	Return TRUE if the string was parsed successfully.
 */

typedef struct {			/* Request (filled in by caller) */
	void *			address;	/* Free-format address buffer (or NULL) */
	size_t			size;		/* Size of address buffer */
	char *			printable;	/* Printable string (NUL-terminated) */
} commScanAddrReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
	size_t			length;		/* Length of address data */
} commScanAddrResp_t;

#if COMM_FNPTRS
typedef int
(*COMMSCANADDR)(
	commScanAddrReq_t *		req,		/* Request (or NULL) */
	commScanAddrResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				 		/* comm context pointer */
;
extern COMMSCANADDR commScanAddr;
#else
DLLEXPORT int cdecl
commScanAddr(
	commScanAddrReq_t *		req,		/* Request (or NULL) */
	commScanAddrResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif

/*
 *	Attempt to format an (in format returned by commScanAddr) address buffer into a NUL-terminated
 *	string.
 *
 *	Return TRUE if the buffer was formatted successfully.
 */

typedef struct {			/* Request (filled in by caller) */
	char *			printable;	/* For NUL-term printable string (or NULL) */
	size_t			size;		/* Size of string buffer */
	const void *	address;	/* (in format returned by commScanAddr) address data */
	size_t			length;		/* Length of address data */
} commPrintAddrReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commPrintAddrResp_t;

#if COMM_FNPTRS
typedef int
(*COMMPRINTADDR)(
	commPrintAddrReq_t *	req,		/* Request (or NULL) */
	commPrintAddrResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
extern COMMPRINTADDR commPrintAddr;
#else
DLLEXPORT int cdecl
commPrintAddr(
	commPrintAddrReq_t *	req,		/* Request (or NULL) */
	commPrintAddrResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				/* comm context pointer */
;
#endif


/*
 *	Generate a pseudo-player handle referring to a group of players.  Handy
 *	for multicasting.  A group can have zero players.
 *
 *	Return TRUE if the pseudo-player handle was generated.
 */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t *	players;	/* List of players */
	size_t			count;		/* Number of players */
} commGroupAllocReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
	playerHdl_t		group;		/* Pseudo-player handle for the group */
} commGroupAllocResp_t;

#if COMM_FNPTRS
typedef int
(*COMMGROUPALLOC)(
	commGroupAllocReq_t *	req,		/* Request (or NULL) */
	commGroupAllocResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				 		/* comm context pointer */
;
extern COMMGROUPALLOC commGroupAlloc;
#else
DLLEXPORT int cdecl
commGroupAlloc(
	commGroupAllocReq_t *	req,		/* Request (or NULL) */
	commGroupAllocResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif

/*
 *	Invalidate a pseudo-player handle referring to a group of players.
 *
 *	Return TRUE if the pseudo-player handle was invalidated.
 */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t		group;		/* Pseudo-player handle for the group */
} commGroupFreeReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commGroupFreeResp_t;

#if COMM_FNPTRS
typedef int
(*COMMGROUPFREE)(
	commGroupFreeReq_t *	req,		/* Request (or NULL) */
	commGroupFreeResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
extern COMMGROUPFREE commGroupFree;
#else
DLLEXPORT int cdecl
commGroupFree(
	commGroupFreeReq_t *	req,		/* Request (or NULL) */
	commGroupFreeResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif


/*
 *	Add one or more players to a group.
 *  Kludge: use to report current player's score to dwango!
 *
 *	Return TRUE if the players were all added.
 */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t		group;		/* Pseudo-player handle for the group - PLAYER_NONE for dwango */
	playerHdl_t *	players;	/* List of playersa - NULL for dwango */
	size_t			count;		/* Number of players -- used as score by dwango! */
} commGroupAddReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commGroupAddResp_t;

#if COMM_FNPTRS
typedef int
(*COMMGROUPADD)(
	commGroupAddReq_t *		req,		/* Request (or NULL) */
	commGroupAddResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				 		/* comm context pointer */
;
extern COMMGROUPADD commGroupAdd;
#else
DLLEXPORT int cdecl
commGroupAdd(
	commGroupAddReq_t *		req,		/* Request (or NULL) */
	commGroupAddResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif

#if 0
/*
 *	Subtract one or more players from a group.  Do not delete the group,
 *	even if it is left empty.
 *
 *	Return TRUE unless there was a problem subtracting one or more players.
 */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t		group;		/* Pseudo-player handle for the group */
	playerHdl_t *	players;	/* List of players */
	size_t			count;		/* Number of players */
} commGroupSubtractReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commGroupSubtractResp_t;

#if COMM_FNPTRS
typedef int
(*COMMGROUPSUBTRACT)(
	commGroupSubtractReq_t *	req,	/* Request (or NULL) */
	commGroupSubtractResp_t *	resp 	/* Response (or NULL) */
	COMM_3RDPARAM)					  	/* comm context pointer */
;
extern COMMGROUPSUBTRACT commGroupSubtract;
#else
DLLEXPORT int cdecl
commGroupSubtract(
	commGroupSubtractReq_t *	req,	/* Request (or NULL) */
	commGroupSubtractResp_t *	resp	/* Response (or NULL) */
	COMM_3RDPARAM)						/* comm context pointer */
;
#endif

#else

/*-------------------------------------------------------------------------
 Set a parameter to a value.  Meant for doing things like reporting scores,
 noting system clock speed changes, etc...
-------------------------------------------------------------------------*/

#define comm_PARAM_SCORE		0x1101	/* value == score, value2 == playerHdl_t */
#define comm_PARAM_TICKSPERSEC	0x1102	/* value == system clocks per sec (default 18) */
#define comm_PARAM_RANK 	0x1103  /* 1 = top player in game, 2 = runner-up */
#define comm_PARAM_KILLS 	0x1104  /* # of times player killed somebody else */
#define comm_PARAM_DEATHS	0x1105	/* # of times given player has died */
#define comm_PARAM_VICTORY	0x1106	/* 1 if player won current game, else 0 */
#define comm_PARAM_FINALREPORT	0x1107	/* 1 if entering end of game report, 0 if leaving end of game report */
#define comm_PARAM_SESSIONINFO  0x1108  /* value == pointer to commSessInfo_t */
#define comm_PARAM_DPRINTF	0x2100	/* Address of dp_dprintf() */

#define comm_PARAM_RXWAIT		0x4100	    /* RxPkt Wait Mode (linux only!) */
#define comm_PARAM_FILEDESC	0x4200			/* File Descriptor for the socket (linux only!) */

#define comm_PARAM_BOGUSVALUE -1	/* placeholder value */

/* following parameters will just return existing value if you pass
 * comm_PARAM_BOGUSVALUE in both param_value and param_value2.
 */
#define comm_PARAM_SYNCJOIN 0x3001  /* 0 if dynamic, 1 if sync launch */

typedef struct {
	size_t			reqLen;		/* sizeof(commSetParamReq_t) */
	long param_num;				/* parameter to set */
	long param_value;			/* value to set it to */
	long param_value2;			/* second part of value, if needed */
} commSetParamReq_t;

/* Note: You must set respLen before calling commSetParam!
 * commSetParam will not write to the structure if respLen is too
 * short.
 */
typedef struct {
	comm_status_t		status;		/* contains comm_STATUS_* describing error */
	size_t respLen;				/* sizeof(commSetParamResp_t) */
	long param_num;				/* parameter that was to be set */
	long param_value;			/* its new value */
	long param_value2;			/* second part of value, if needed */
} commSetParamResp_t;

/* info sent by server to pinging hosts */
#define SERVER_PING_MAGIC_PKTNUM0 0x8000 /*ping pktnum used by older dp code*/
#define SERVER_PING_MAGIC_PKTNUM1 0x8001 /*ping pktnum used by current dp code*/
typedef struct {
	short cur_players;		/* current users on server */
	short max_players;		/* max users allowed on server */
	short players[65536];	/* current users on server indexed by session type*/
	/*following two fields added when started using SERVER_PING_MAGIC_PKTNUM1*/
	short cur_games;		/* games currently running */
	short games[65536];		/* games currently running indexed by session type*/
} commSessInfo_t;

#if COMM_FNPTRS
typedef int
(*COMMSETPARAM)(
	commSetParamReq_t *		req,		/* Request (or NULL) */
	commSetParamResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				 		/* comm context pointer */
;
extern COMMSETPARAM commSetParam;
#else
DLLEXPORT int cdecl
commSetParam(
	commSetParamReq_t *		req,		/* Request (or NULL) */
	commSetParamResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)				 		/* comm context pointer */
;
#endif

#endif

/*
 *	Establish a data link to a player and shake hands with him.  This does
 *	not actually establish a connection in the IP sense of the word.
 *
 *	Return TRUE if the link was established and we shook hands.
 */

/* Bits of commSayHiReq_t.flags */
#define comm_SAYHI_2NDLIVE			0x01	/* Second address is live */

typedef struct {					/* Request (filled in by caller) */
	void *			address;		/* Address data (in format returned by commScanAddr) */
	size_t			length;			/* Length of address data (0=read as a string) */
	void *			address2;		/* Secondary address (in format returned by commScanAddr) can be NULL */
	long			flags;			/* Flags for second address */
} commSayHiReq_t;

typedef struct {					/* Response (filled in by routine) */
	comm_status_t	status;			/* Command status */
	playerHdl_t		player;			/* Player handle for this player */
} commSayHiResp_t;

#if COMM_FNPTRS
typedef int
(*COMMSAYHI)(
	commSayHiReq_t *	req,		/* Request (or NULL) */
	commSayHiResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMSAYHI commSayHi;
#else
DLLEXPORT int cdecl
commSayHi(
	commSayHiReq_t *	req,		/* Request (or NULL) */
	commSayHiResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/*
 *	Tear down a data link to a player.  The link or the player may already be
 *	down, so don't shake hands.
 *
 *	Return TRUE if the link was successfully broken.
 */

typedef struct {			/* Request (filled in by caller) */
	playerHdl_t		player;		/* Player handle for this player */
} commSayByeReq_t;

typedef struct {			/* Response (filled in by routine) */
	comm_status_t		status;		/* Command status */
} commSayByeResp_t;

#if COMM_FNPTRS
typedef int
(*COMMSAYBYE)(
	commSayByeReq_t *	req,		/* Request (or NULL) */
	commSayByeResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
extern COMMSAYBYE commSayBye;
#else
DLLEXPORT int cdecl
commSayBye(
	commSayByeReq_t *	req,		/* Request (or NULL) */
	commSayByeResp_t *	resp		/* Response (or NULL) */
	COMM_3RDPARAM)					/* comm context pointer */
;
#endif

/* Undo effect of previous #pragma. */
#include "dpunpack.h"

/*-------------- Mac Stuff ------------------------*/
#ifdef __MWERKS__
#include <Types.h>   /* for Boolean */

/*
 *	Check to see if this transport should be listed.
 *
 *	Return FALSE on error.
 */

typedef Boolean
(*COMMLOADTEST)(void);

/*
 *	Check to see if this transport currently needs UI.
 *
 *	Return TRUE if it needs the UI.
 */

typedef Boolean
(*COMMNEEDSUI)(commInitReq_t* req);

/* See the code in dpdll - these are used briefly there. */
typedef void (*SETSCREENRECT)(Rect* theRect);
typedef void (*FORCESCREENUPDATE)(void);
typedef void (*SETSCREENFUNC)(FORCESCREENUPDATE theFunc);

#if !COMM_FNPTRS
void SetScreenRect(Rect* theRect);
void ForceScreenUpdate(void);
void SetScreenFunc(FORCESCREENUPDATE theFunc);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
