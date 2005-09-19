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

/*----------------------------------------------------------------------
 Multiplayer game networking - reliable and unreliable transport layer.
 Copyright (C) 1997, Activision.

 $Log: dpio.h $
 Revision 1.44  1997/09/27 05:34:51  lbennett
 Added dpio_assertValid().
 Revision 1.43  1997/09/23 02:28:40  lbennett
 Removed per-connection callback.
 Revision 1.42  1997/09/20 05:27:13  lbennett
 Added freeze/thaw for player handles.
 Revision 1.41  1997/09/04 06:43:21  dkegel
 Export dpio_scanAdr... this is a duplicate of dpResolveAddress, which
 should probably vanish.
 Revision 1.40  1997/09/01 19:09:19  dkegel
 dpio_findTimedOutHost no longer calls dpio_closeHdlImmed; it
 always just returns the suspect handle to the caller.
 Revision 1.39  1997/08/25 01:24:21  dkegel
 Moved to a TCP-like dpio_setHandleOptions() call for controlling
 keepalives.  Much easier to use.
 Old stuff still enabled if dp_ANET2 not defined.
 Revision 1.38  1997/08/23 20:45:04  dkegel
 Split part of dpio_findTimedOutHost() out into dpio_findResetHost()
 so even if we don't want to enforce keepalives, we can at least
 handle active close properly!
 Revision 1.37  1997/08/22 16:47:24  dkegel
 Added way to send keepalives to gameserver in v2.
 Revision 1.36  1997/08/11 01:27:52  dkegel
 The comm_driverInfo_t record is now saved in dpio_t..
 Revision 1.35  1997/08/06 21:30:00  dkegel
 dpio now adapts much quicker to actual round trip time at startup,
 so Dark Reign's ping time display doesn't take forever to ramp from
 500 milliseconds to the true value.
 Revision 1.34  1997/07/28 02:37:33  dkegel
 Whoops- dpio_create now uses dp_transport_t.
 Revision 1.33  1997/07/27 23:09:07  dkegel
 Removed dpdll.h for the moment - it does belong here, though.
 Revision 1.32  1997/06/20 20:58:18  dkegel
 1. Added new call dpio_getHdlTxPktsFree() for helping throttle
 dptab transmissions.
 2. Print out 'nq %d' in q_packet to indicate how full the tx window is.
 Revision 1.31  1997/06/14 02:56:55  geychaner
 added STATE_CLOSE_QFULL for the condition where closeHdl is called on a
 handle with a full tx queue
 Revision 1.30  1997/06/13 22:57:19  geychaner
 Added connection states for active connection close
 Revision 1.29  1997/05/31 22:42:20  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.28  1997/05/29 19:39:47  dkegel
 Added two methods needed by enumserv.c.
 Revision 1.27  1997/05/22 23:13:45  dkegel
 First try at supporting modem delay simulation.
 Revision 1.26  1997/04/29 18:59:30  dkegel
 Added dpio_get_player_latency.
 Revision 1.25  1997/04/21 01:44:35  dkegel
 Implemented dpHigherLevelBlanketAck.
 Triples performance of dpturn-based game at 2 players with recv pktloss
 of 20%!
 Revision 1.24  1997/03/26 19:08:39  geychaner
 Added clocksPerSec to dpio structure, in case we need it later.
 Revision 1.23  1997/03/26 04:35:23  geychaner
 Added the latency to dpio, for a RTT first guess, and dpio_get_latency.
 Revision 1.22  1997/03/26 03:45:11  geychaner
 Added maxTxInterval, and changed default_tx_interval to set_clocks
 Revision 1.21  1997/03/26 03:12:16  geychaner
 changed lastPktTime to more accurate name PktTime
 Revision 1.20  1997/03/20 04:25:48  geychaner
 Added round-trip calculation and dynamic retransmission variables.
 Revision 1.19  1997/03/15 03:35:13  geychaner
 Created a callback mechanism for incoming connections.
 Revision 1.18  1997/03/14 23:53:11  geychaner
 Added maxPlayerHdls and setMaxPlayerHdls to limit connections
 Revision 1.17  1997/03/14 06:38:39  geychaner
 Added ReTxFactor for exponential throttleback
 Revision 1.16  1997/03/13 04:03:21  dkegel
 hMaster now passed as value to dpio_setMaster rather than as pointer
 to dpio_create.
 dpio_setMaster now implemented.  Must be called when electing master.
 Revision 1.15  1997/03/12 01:21:35  geychaner
 Added getHdlState
 Revision 1.14  1997/03/11 05:59:13  geychaner
 Added a callback to openHdl; this callback is stored separately for each
 connection in the associative table; perhaps this is wrong.
 Revision 1.13  1997/03/11 03:27:10  geychaner
 Changed findTimedOutHost so it can take a handle to check
 Revision 1.12  1997/03/05 02:33:37  dkegel
 Added const on pointers to things inside dp to avoid warnings.
 Revision 1.11  1997/03/03 03:44:12  dkegel
 1. dpio_STATE_CLOSED now gets its own bit, so other bits don't interfere.
 2. Remember pktnum of syn, and reset connection if a syn with a different
 pktnum comes in.  Risky, but the only way to allow reconnects after
 crashes without port numbers, I think.
 3. state now dprinted uniformly as h:%d state:%x
 4. better error checking after dpio_window_get, better dprints
 Revision 1.10  1997/03/01 03:45:42  gceych
 Added closeHdlImmed
 Revision 1.9  1997/03/01 02:52:11  gceych
 Created new states for active closing of handles.
 Revision 1.8  1997/02/27 01:51:24  dkegel
 THE BIG REARRANGEMENT: split public API into anet.h; dp.h and dppkt.h
 remain for those who need the full API including the real dp_t;
 care must be taken to avoid circular includes and to avoid activating
 too much of anet.h when using the full API.
 Revision 1.7  1997/02/26 16:37:50  gceych
 Added dpio_getBroadcastHdls to expose list of all connections
 Revision 1.6  1997/02/16 03:57:54  dkegel
 1. dpio_thaw now part of dpio_create
 2. thawing now gets transport DLL name and comm parameters from freeze 
 file.
 Revision 1.5  1997/02/15 02:19:02  dkegel
 1. dpio_noteArrivalTime no longer public.
 2. Window bigger.
 This makes it less likely that user code will get FULL when sending
 reliable data.
 3. All connection data centralized in dpio_conn_t.
 4. Protocol state more explicit; added dpio_STATE_ flags for internal use.
 Revision 1.4  1997/02/12 03:08:42  dkegel
 1. Control structure packing; this helped fix a crash bug.
 2. dpio now knows its own node address.
 Revision 1.3  1997/02/10 07:52:07  dkegel
 Oops.  Fixed tiny typo.
 Revision 1.2  1997/02/10 07:03:54  dkegel
 Works.
 Revision 1.1  1997/02/08 01:02:48  dkegel
 Initial revision
----------------------------------------------------------------------*/

#ifndef dpio_h
#define dpio_h

#include <stdio.h>
#include <time.h>
#include "commapi.h"	/* for playerHdl_t */
#define dp_NO_FUNCTIONS
#include "anet.h"		/* for error codes and dpid_t */
#undef dp_NO_FUNCTIONS
#include "dppkt.h"		/* for dpio_ packet definitions */
#include "assoctab.h"
#include "q.h"
#include "delay.h"

/* Bit flags set by dpio_get() */
#define dpio_GET_RELIABLE        1
#define dpio_GET_LATENCY_MEASUREMENT 64
#define dpio_GET_CHECK_TIMEOUT 128

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****** Note: Pragma pack must come after all #include's! **********
 */
#include "dppack1.h"

/*------- dpio Configuration --------------------------------------------------- */
/* How many packets may be outstanding per link.
 * Must be power of two!
 * Each packet for each link takes up 0.5 kilobytes, so 8 players * 16 packets
 * = 64 kilobytes.  Whacky, eh?
 */
#define dpio_WINDOWSIZE 32

/*------- dpio Types ----------------------------------------------------------- */

/* Note: All clock_t variables have units 'clock ticks', that is,
 * 1/clocksPerSec seconds.
 */

/* Info about the packets not yet completly sent or received to/from a peer. */
/* Conceptually, this contains an infinite array of packets and of acked */
/* flags.  To save space, we will throw away that part of the infinite */
/* arrays from pktnum 0 to the acked packet just before (the first packet */
/* which has not been acked, which we call windowBase). */

typedef struct {
	clock_t 		PktTime;		/* On RX, last pkt.  On tx, NEXT RELIABLE Tx time */
	int 			ReTxFactor;		/* Current exponential backoff factor */
	dpio_wrapped_data_packet_t outstanding[dpio_WINDOWSIZE];
	int				bAcked[dpio_WINDOWSIZE];	/* Whether outstanding[i] acked. */
	clock_t			pktQTime[dpio_WINDOWSIZE];	/* Time this packet entered queue */
	int				nRetries[dpio_WINDOWSIZE];	/* Number of retries this packet has had */
	unsigned short	windowBase;
	unsigned short	origWindowBase;	/* pktnum of SYN packet */
	unsigned short	next_pktnum;	/* Next unused packet number. */
	/* ack flag for pktnum x is bAcked[x % dpio_WINDOWSIZE] */
	/* Put next packet in outstanding[next_pktnum % dpio_WINDOWSIZE] */
	/* Warning: dpio_WINDOWSIZE must be power of 2. */
	/* 0 <= (next_pktnum-windowBase) <= dpio_WINDOWSIZE */
} dpio_window_t;

/* Connection states.  See figure 18.12, "TCP/IP Illustrated, Volume 1",
 * but note that dpio doesn't piggyback acks on top of data packets (pity).
 */
/* Values */
#define dpio_STATE_LISTEN 0		/* No SYN sent or received yet. Passive open. */
#define dpio_STATE_ESTABLISHED 7 /* Sent syn, got syn ack and SYN, ready! */
#define dpio_STATE_CLOSED 56	 /* Sent fin, got fin ack and fin, close! */
/* Bits */
#define dpio_STATE_SYN_SENT 1	/* Sent a SYN */
#define dpio_STATE_SYN_RCVD 2	/* Got a SYN */
#define dpio_STATE_SYN_RCVDACK 4 /* Got an syn ack */
#define dpio_STATE_FIN_SENT 8	 /* Sent a FIN packet */
#define dpio_STATE_FIN_RCVD 16   /* Received a FIN packet */
#define dpio_STATE_FIN_RCVDACK 32 /* Received a fin ack */
#define dpio_STATE_RST_RCVD 64   /* Received an RST packet */
#define dpio_STATE_CLOSE_QFULL 128 /* Tried to close handle, but queue full */

#ifdef dp_ANET2
#define dpio_OPTION_KEEPALIVE		0x1	/* Check and warn if connection goes down */
#define dpio_OPTION_PIGGYBACK_PING	0x2	/* Send piggyback pings */
#define dpio_OPTION_FORCED_PING		0x4	/* Send forced pings */
#endif
#define dpio_REMCAP_NEWTAB	0x01	/* Other side can handle new dptab delete */
#define dpio_REMCAP_GATHER	0x02	/* Other side can handle dpio pkt gather */
#define dpio_REMCAP_JUMBO_PACKETS	0x04	/* Other side can handle 520 byte packets */

#define dpio_OPENHDL_RAW_2NDLIVE	0x01	/* Second address is LIVE */

#define dpio_MAXLEN_GATHER 520		/* maximum size of gathered packet */
#define dpio_LN2_PING_TIME_SENT_N 4
#define dpio_PING_TIME_SENT_N (1 << dpio_LN2_PING_TIME_SENT_N)

/*----------------------------------------------------------------------
 Function type called when a handle is opened.
 Warning: any code that calls dpio_setIncomingCallback() must be compiled
 with same options as dpio.c.  Usually, only dp2.c calls that, so that
 shouldn't be any problem.
----------------------------------------------------------------------*/
typedef void (*dpioOpenHdlCallback_t)(playerHdl_t hdl, int n_hdls, dp_result_t err, void *context);

/* All the info that makes up a connection except the comm handle. */
typedef struct {
	dpio_window_t rx;			/* incoming reliable packets not yet delivered */
	dpio_window_t tx;			/* outgoing reliable packets not yet ack'd */
	short state;				/* see dpio_STATE_* */
#ifdef dp_ANET2
	short options;				/* see dpio_OPTION_* */
#endif
	short remote_capabilities;	/* remote version; see dpio_REMCAP_* */

	/* Callback called when connection is opened or closed */
	/* dpioOpenHdlCallback_t openHdl_callback;
	void *openHdl_context; */

	int ackNeeded;				/* this peer needs an ack */

	/* Following is for holding unreliable packets for transmission
	 * gathered together into one big packet.
	 */
	char txGatherBuf[dpio_MAXLEN_GATHER];	/* room for tag & len */
	size_t txGatherBufLen;		/* # of chars in buffer */

	/* Following fields used for tracking connection statistics */
	clock_t	RTTime;			/* Current round trip time */
	int		RTT_nsamp;	    /* 0 initially, inc when updating RTTime; max 10 */
	unsigned char ping_current_pktnum;	/* pktnum of last ping sent */
	clock_t ping_next_piggyback;	/* time to send next ping */
	clock_t ping_next_forced;		/* time to send a forced ping */
	clock_t ping_next_keepalive;	/* time to send a keepalive ping */
				/* indexed by ping_current_pktnum % * dpio_PING_TIME_SENT_N */
	clock_t ping_time_sent[dpio_PING_TIME_SENT_N];	
	short	ping_bits;		/* Bit i set if i'th newest ping is still outstanding */
} dpio_conn_t;

/*------- The dpio_t itself. ----------------------------------------------- */
typedef struct dpio_s {
	/* Reliable transport stuff. */
	int reliable_enabled;	/* if FALSE, do nothing more than ACK *old* pkts. */
	clock_t nextTxTime;		/* When to look for next packet to send */
	clock_t minTxInterval;	/* Floor for packet retransmission time (RTO) */
	clock_t maxTxInterval;	/* Ceiling for packet retransmission (RTO) */
	clock_t latency;		/* Best current guess of average RTT */
	assoctab_t *conns;		/* Array of dpio_conn_t's indexed by playerHdl. */
	int nextconncheck;		/* next connection to check in dpio_get */
 	int max_playerHdls;		/* Maximum number of connection handles allowed */
	int ackNeeded;			/* some peer needs an ack */

	/* Whether some peer's gather buffer contains bytes that need sending */
	int txGatherNeeded;

	clock_t nextFlush;		/* next time to dpio_flush on 10ms beacon. */
 	int appWillFlush;		/* TRUE if app calls dpFlush() after tx done */

	/* Following is for holding unreliable packets for reception
	 * gathered together into one big packet.
	 */
	char rxGatherBuf[dpio_MAXLEN_GATHER];	/* room for tag & len */
	int rxGatherBufLen;				/* # of chars in buffer */
	int rxGatherBufUsed;			/* # of chars read so far from buffer */
	playerHdl_t rxGatherSrc;		/* source of packet in gather buf */
	unsigned char	rxGatherAdr[dp_MAX_ADR_LEN];	/* address as seen externally */

	/* Callback called when incoming connections are opened or closed */
	dpioOpenHdlCallback_t callback;
	void *context;

	/* Timeout-related stuff. */
	clock_t 	maxRxInterval;		/* How long to wait before timing out host. */

#ifndef dp_ANET2
	clock_t		keepalive_deadline;	/* only used by client; last time sent to master. */
	int			sentToMaster;		/* whether we have sent a packet to the master lately. */
	playerHdl_t hMaster;

#endif

	int			clocksPerSec;		/* set by dpio_set_clocks */
	const clock_t     *now;			/* Really in container... hmm. */

	/* Queue used for explicit loopback messages. */
	q_t			 *localMsgQ;	/* for intra-computer messages. */

	/* Debugging stuff. */
	delay_t		*delay_q;		/* Delay incoming packets if in debug mode */
	int			rxDropPercent;	/* drop this many percent of received packets */

	/* Basic info. */
	int			myAdrLen;
	unsigned char	myAdr[dp_MAX_ADR_LEN];	/* address as seen externally */
	unsigned char	myAdr2[dp_MAX_ADR_LEN]; /* detected address */
	int publicAddressKnown;		/* Set when first SYN pkt tells us our outside adr */

	/* Restoring-from-disk stuff. */
	commInitReq_t	commInitReq;	/* Baud rate, etc. */
	comm_driverInfo_t *driverinfo;	/* Stuff the driver knows about itself */
	/* comm driver context information */
	void *commPtr;

	dp_transport_t	transportDLLname;	/* Path to dll currently loaded. */

#ifdef dp_STATS
#define dpio_NUM_STATS 10		/* dp_STAT_DPIO_REL_BYTES...dp_STAT_DPIO_CONNECTED -1 */
	/* Statistics */
	dp_stat_t stats[dpio_NUM_STATS];
#endif

	/* Variables to support latency measurement.
	 * See design comment for dpio_setPingIntervals(). */ 
	clock_t ping_piggyback_interval;	/* in ECLOCKS */
	clock_t ping_forced_interval;		/* in ECLOCKS */
} dpio_t;

/* Obscure our entry point names */
#define dpio_scanAdr _dpbs

/*-----------------------------------------------------------------------
 Deallocate all resources allocated by dpio_create.
 Shut down the network API.
 If flags is nonzero, and the network is a modem, don't hang up.
-----------------------------------------------------------------------*/
void dpio_destroy(dpio_t *dpio, long flags);

/*------------------------------------------------------------------------
 Create a dpio_t.  Initialize the network API.

 On entry:
 pdpio is the address of an uninitialized dpio_t *.
 transportDLLname is the path to the desired transport DLL (e.g. serial.dll).
 commInitReq is filled with the comm parameters (e.g. .baud = 9600).
 now is a pointer to a variable that contains the current value of clock().

 Restoring from disk:
 If thawfp is not NULL, transportDLLName and commInitReq
 are read from the given file pointer instead of the parameters,
 and the dpio's connection state is restored from the file.

 On successful exit:
 Initializes *pp, and returns dp_RES_OK.

 On failure:
 Returns a dp_RES_* code indicating the reason for failure.
-----------------------------------------------------------------------*/
dp_result_t dpio_create(dpio_t **pdpio, const dp_transport_t *transportDLLname, commInitReq_t *commInitReq, const clock_t *now, FILE *thawfp);

/*----------------------------------------------------------------------
 Returns dp_RES_BUSY if dpio has reliable transmissions pending,
 and sets *pHdl to the handle of the machine causing us to wait.
----------------------------------------------------------------------*/
dp_result_t dpio_ReadyToFreeze(dpio_t* dpio, playerHdl_t *pHdl);

/*-----------------------------------------------------------------------
 Save the state of a dpio_t to disk.
 (Restore later by passing file pointer to dpio_create.)
-----------------------------------------------------------------------*/
dp_result_t dpio_freeze(dpio_t *dpio, FILE *fp);

/*-------------------------------------------------------------------------
Save a player handle to disk.
-------------------------------------------------------------------------*/
dp_result_t dpio_freezeHdl(dpio_t* dpio, playerHdl_t hdl, FILE* file);

/*-------------------------------------------------------------------------
Restore a player handle from disk.
This function must be called *after* the dpio is thawed (dpio_create()).
-------------------------------------------------------------------------*/
dp_result_t dpio_thawHdl(dpio_t* dpio, playerHdl_t* hdl, FILE* file);

/*-----------------------------------------------------------------------
 Set the callback for incoming connection requests
-----------------------------------------------------------------------*/
dp_result_t dpio_setIncomingCallback(dpio_t *dpio, dpioOpenHdlCallback_t cb, void *context);

/*-----------------------------------------------------------------------
 Calculates the latency of the current driver; actually just a guess
 for now.  (May eventually calculate from the RTT of the open handles.)
 Results in milliseconds.
 Returns 0 upon error.
-----------------------------------------------------------------------*/
int dpio_get_latency (dpio_t *dpio, int flags);

/*-----------------------------------------------------------------------
 Calculates the latency of the current player according to the reliable
 transport system and pings if requested by dpio_setHandleOptions.
 Results in milliseconds.
 If ploss is not NULL, sets *ploss to round trip packet loss in percent.
 Returns 0 upon error.
-----------------------------------------------------------------------*/
int dpio_get_player_latency (dpio_t *dpio, playerHdl_t h, int flags, int *ploss);

/*-----------------------------------------------------------------------
 Set the maximum number of player handles we will allow to be created
-----------------------------------------------------------------------*/
void dpio_setMaxPlayerHdls(dpio_t *dpio, int maxHdls);

/*-----------------------------------------------------------------------
 Get the remote capabilies of a connection by player handle
 This is used by dptab to see if the other side is recent enough to
 handle the new style of dptab_delete.
 The return value is a bitfield full of dpio_REMCAP_*.
-----------------------------------------------------------------------*/
short dpio_getHdlRemoteCapabilities(dpio_t *dpio, playerHdl_t h);

/*-----------------------------------------------------------------------
 Get the current state of a connection by player handle
-----------------------------------------------------------------------*/
short dpio_getHdlState(dpio_t *dpio, playerHdl_t h);

/*-----------------------------------------------------------------------
 Get the current age in eclock units of a connection by player handle
 This number goes to zero when we receive a reliable packet or keepalive,
 and increases thereafter.
 Returns 0 for PLAYER_ME;
 returns -1 for other predefined handles;
 returns -2 for bad handle.
-----------------------------------------------------------------------*/
clock_t dpio_getHdlAge(dpio_t *dpio, playerHdl_t h);

/*-----------------------------------------------------------------------
 Get the remaining number of packets free in the TX queue for a handle.
 Returns -1 on error.
-----------------------------------------------------------------------*/
int dpio_getHdlTxPktsFree(dpio_t *dpio, playerHdl_t h);

/*-----------------------------------------------------------------------
 Given an ASCII network address and a buffer length, fill a buffer with the 
 binary address.
 Returns length of address in bytes, or 0 upon error.
 Output buffer must be big enough, or buffer won't be valid.
-----------------------------------------------------------------------*/
DP_API int dpio_scanAdr(dpio_t *dpio, char *hostname, unsigned char *adrbuf, size_t buflen);

/*-----------------------------------------------------------------------
 Convert a binary network address (e.g. one from dpio_scanAdr),
 convert it to ASCII (in a form suitable for reading by dpio_scanAdr).

 On entry,
   adrbuf holds a binary network address,
   adrlen is the length of the address,
   buf    points to a buffer to hold the string,
   buflen is the size of buf.
 On success,
   buf contains a null-terminated ASCII string.

 Returns dp_RES_OK on success,
 Returns dp_RES_BAD if buflen is less than the size required to hold the string
 Returns dp_RES_BAD on any other failure
-----------------------------------------------------------------------*/
DP_API dp_result_t dpio_printAdr(dpio_t *dpio, const char *adrbuf, size_t adrlen, char *buf, size_t buflen);

/*-----------------------------------------------------------------------
 Open a connection to a new peer (or an old one).
 It's ok to call it twice on the same address, it returns 
 the same handle, and doesn't clear any buffers or timers.
 The callback cb is ignored; the parameter will be removed eventually.
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdl(dpio_t *dpio, void *adr, dpioOpenHdlCallback_t cb, void *context);

/*-----------------------------------------------------------------------
 Functionally equivalent to the above function except that TWO addresses
 can be supplied.  Two addresses may be necessary if a machine has two
 network addresses for a given topology.  The most common scenario is
 a Win32 machine with TCP/IP for LAN and TCP/IP for Dial-Up networking.
 The other major case is when machines are behind renumbering firewalls
 or NATs.

 When two addresses are supplied the following occurs:

  Initially both addresses are associated with the player handle.
  When sending packets to this handle, the packet will be sent to BOTH
  addresses.  It is noted that only SYN packets should be sent whilst
  the handle is in this state.

  The first packet to arrive from EITHER address indicates that this
  address provides a connection to a this player handle and all future
  packets sent to this player handle will be sent to ONLY that address.

  The other address will be stored but packets will never be sent to it
  even if packets arrive from that address at a later time.

 Note also that the callback paramater (and the associated context) has
 been dropped from the above function definition since it is not used 
 and this may be an attempt to have it erased alltogether :)
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdl2(dpio_t *dpio, void *adr, void *adr2);

/*-----------------------------------------------------------------------
 Delete a peer.
 When you're done talking to all the players at a particular machine,
 call this function to free up the low level comm handle to that machine.
-----------------------------------------------------------------------*/
dp_result_t dpio_closeHdl(dpio_t *dpio, playerHdl_t h);

/*-----------------------------------------------------------------------
 Delete a peer.
 This deletes a handle without waiting for tx/rx to finish or doing a
 controlled close.
-----------------------------------------------------------------------*/
dp_result_t dpio_closeHdlImmed(dpio_t *dpio, playerHdl_t h);

/*-----------------------------------------------------------------------
 Given a network address, return the handle associated with it.
 If no handle is currently associated with that address, create a handle.
 Use with caution!  Does not create a reliable connection!
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdlRaw(dpio_t *dpio, void *adr);

/*-----------------------------------------------------------------------
 Similar to dpio_openHdlRaw except that TWO addresses are supplied.
 Refer to dpio_openHdl2 for reasons why you would want to do this.
 
 The flags can be a combination of:
	dpio_OPENHDL_RAW_2NDLIVE: the second address is to be treated as LIVE
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdlRaw2(dpio_t *dpio, void *adr, void *adr2, long flags);

/*-----------------------------------------------------------------------
 This deletes a raw handle without deleting the connection queue or any
 other info.  Internal use only.
-----------------------------------------------------------------------*/
dp_result_t dpio_closeHdlRaw(dpio_t *dpio, playerHdl_t h);

/*--------------------------------------------------------------------------
 Find out the network address for a given handle opened with dpio_openHdl.
--------------------------------------------------------------------------*/
dp_result_t dpio_hdl2adr(dpio_t *dpio, playerHdl_t h, void *adr, int *len);

/*--------------------------------------------------------------------------
 Find out the network address for a given handle opened with dpio_openHdl2
--------------------------------------------------------------------------*/
dp_result_t dpio_hdl2adr2(dpio_t *dpio, playerHdl_t h, void *adr, void *adr2, int *len);

/*--------------------------------------------------------------------------
 Find out the player name for a given handle opened with dpio_openHdl.
 "name" is a pointer to the buffer to copy the name to,
 "name_size" is the number of bytes available in that buffer.
 If name_size <= strlen(player name), returns dp_RES_FULL.
 Only supported by transports which have the comm_DRIVER_KNOWS_PLAYERLST 
 bit set in comm_driverInfo_t.capabilities,  
--------------------------------------------------------------------------*/
dp_result_t dpio_hdl2name(dpio_t *dpio, playerHdl_t h, char *name, size_t name_size);

#ifndef dp_ANET2

/*-----------------------------------------------------------------------
 Set dpio's idea of who the master is.
 The master has special timeout duties.
 Caller should already have deleted the old master's handle, if desired.

 hMaster is the handle of the session leader
 (PLAYER_ME if we are the session leader, or PLAYER_NONE if there is none).
-----------------------------------------------------------------------*/
dp_result_t dpio_setMaster(dpio_t *dpio, playerHdl_t hMaster);

#endif


/*------------------------------------------------------------------------
 Set dpio's Rx/Tx timeout and restransmission parameters.
 2nd parameter is actual system realtime clock speed.
-----------------------------------------------------------------------*/
void dpio_set_clocks(dpio_t *dpio, int clocksPerSec);

/*------------------------------------------------------------------------
 Set the intervals used by the latency measurement system.
 If both intervals are 0, only ACKs of non-retransmitted reliable packets
	will generate new measurements.  No extra bandwidth is used, but 
	measurements may be infrequent if there are few reliable packets or
	there is heavy packet loss.
 If piggbackPingInterval is set, additional small ping packets and
	responses will be piggybacked onto gathered packets every
	piggybackPingInterval ms when possible.  No extra physical packets
	will be generated, and any network traffic at all will generate
	latency and packet loss measurements.
 If forcedPingInterval is set and no piggybacked pings have been sent in
	the past forcedPingInterval ms, an additional ping packet will be
	sent to get a latency measurement.

 Pings will never be sent more than once per round trip time, regardless
 of the intervals requested, and are only sent to handles with the
 appropriate mode bit set (dpio_OPTION_PIGGYBACK_PING or 
 dpio_OPTION_FORCED_PING) using dpio_setHandleOptions().

 Regardless of round trip time or the parameters to this function, pings
 will not be sent more often than is reasonable.

 Returns dp_RES_OK on success, or dp_RES_BAD on bad parameters.
------------------------------------------------------------------------*/
dp_result_t dpio_setPingIntervals(
	dpio_t *dpio,
	int piggybackPingIntervalMS,
	int forcedPingIntervalMS);

/*------------------------------------------------------------------------
 For debugging purposes only.
 Force dpio to drop a given percentage of all packets (randomly) on
 reception.
-----------------------------------------------------------------------*/
void dpio_forceRxDropPercent(dpio_t *dpio, int rxDropPercent);

/*----------------------------------------------------------------------
 Receive a packet.  Packet might come from the network, the local
 queue, or the reliable transport windows; the caller won't be able
 to distinguish between the sources.

 All parameters have already been validated; *dpio->now is up to date.
 This routine notes the last packet arrival time according to source,
 if the packet looks interesting (it knows about dp's packet types).
 It also immediately handles reliable data or ack packets.
 This routine knows nothing about envelopes.

 On entry:
 *size is the size of the receive buffer.

 On exit:
 Sets *src to player handle of sender.
 Sets *size to length of packet.
 If flags is a valid pointer (not NULL), sets *flags to the dpSend() flags
  dp_SEND_RELIABLE or dp_SEND_UNRELIABLE telling whether the returned packet
  was sent reliably or unreliably.
----------------------------------------------------------------------*/
dp_result_t dpio_get(
	dpio_t   *dpio,
	playerHdl_t *src,
	void *buffer,
	size_t *size,
	int *flags);

/*----------------------------------------------------------------------
 Send a packet unreliably.
 Supports two ways to send to multiple destinations:
 1. List of destinations.  Set nDests to number of destinations, dests
    to address of array of playerHdl_t's (such as an array maintained
	for a player list).
 2. Hardware broadcast.  Set nDests to 1, and nDests to the address
    of a playerHdl_t holding PLAYER_BROADCAST.  

 If the network driver supports vector sends, this routine will pass
 the vector send through to the driver to reduce network overhead.
----------------------------------------------------------------------*/
dp_result_t dpio_put_unreliable(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest);  /* If error occurs, dest with error indicated here */

/*----------------------------------------------------------------------
 Send a packet reliably, possibly to multiple destinations.
 On entry,
 nDests is the destination count
 dests is the address of an array of playerHdl_t's of destinations.

 The packet will be placed in the outgoing window buffers for the
 given machines in case it needs retransmission later.  When it
 is acknowledged by each destination, it is removed from that machine's
 window buffer.

 Flags should be dp_SEND_RESERVE_NONE, dp_SEND_RESERVE_QUARTER,
 dp_SEND_RESERVE_HALF, or dp_SEND_RESERVE_THREEQUARTERS.
 This controls how much of the reliable send queue to reserve for later
 sends.
----------------------------------------------------------------------*/
dp_result_t dpio_put_reliable2(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest,  /* If error occurs, dest with error indicated here */
	int flags);

#define dpio_put_reliable(dpio, dests, nDests, buffer, size, errDest) \
	dpio_put_reliable2(dpio, dests, nDests, buffer, size, errDest, dp_SEND_RESERVE_NONE)

/*----------------------------------------------------------------------
 Send any outstanding packets or acks queued recently.
----------------------------------------------------------------------*/
dp_result_t dpio_flush(dpio_t *dpio);

/*--------------------------------------------------------------------------
 Returns the number of packets in the local message queue.
--------------------------------------------------------------------------*/
int dpio_localQ_npkts(dpio_t *dpio);

/*--------------------------------------------------------------------------
 Get a list of currently active connection handles
 Returns number of handles.  Will not overflow buffer; if buffer size is
 returned, call again with a larger buffer.
--------------------------------------------------------------------------*/
int dpio_getBroadcastHdls(dpio_t *dpio, playerHdl_t hdls[], int max_nhdls);

#ifdef dp_ANET2
/*-----------------------------------------------------------------------
 Set the options on the given handle.
 Supported options are one or more flag bits or'd together.
 Right now, there is only one flag bit:

 dpio_OPTION_KEEPALIVE 
  If we have not received a packet on this handle in the last 
  dp_PLAYER_TIMEOUT seconds, declare the handle timed out
  (dpio_findTimedOutHost will find it on the next call after this happens).

  If we have not received a packet on this handle in the last 
  dp_PLAYER_TIMEOUT/2 seconds, send a garbage packet to the host reliably,
  which should force it to ack the packet and reset the keepalive timer
  above.

 Returns 
   dp_RES_BAD if handle invalid,
   dp_RES_BUG if option invalid,
   dp_RES_OK on success.
-----------------------------------------------------------------------*/
dp_result_t dpio_setHandleOptions(dpio_t *dpio, playerHdl_t h, int options);

/*-----------------------------------------------------------------------
 Call occasionally (once/second).
 If it returns a handle, call dpio_closeHdlImmed() on the handle.

 Scans through all open handles, and checks each handle h as follows:

 1) If h has dpio_OPTION_KEEPALIVE set but no packet has been received
 on h in a long time, it calls 
 dpio->openHdl_callback(h, ..., dp_RES_HOST_NOT_RESPONDING),
 and returns h.
 Bug: if the connection had an explicit openHdl callback,
 it is confusing that the generic dpio->openHdl_callback() is called here.
 
 2) If h has dpio_OPTION_KEEPALIVE set but no packet has been sent
 to h in a long time, it sends them a keepalive packet.

 3) Detects hosts which have actively reset the connection by
 sending us a new SYN with a new packetnumber.
 Also watches handles which dpio_closeHdl() has been called on;
 when their tx/rx buffers are empty, it declares them ready for 
 immediate closing by returning them to the caller.

 On exit, returns handle of a player that needs immediate closing, 
 or PLAYER_NONE.
-----------------------------------------------------------------------*/
playerHdl_t dpio_findTimedOutHost(dpio_t *dpio);

#else

/*-----------------------------------------------------------------------
 Call occasionally (once/second).
 If it returns a good handle, call dpio_closeHdlImmed() on the handle.
 Note: can trigger a callback to dpio->openHdl_callback().

 Detects hosts that have sent us at least one
 interesting packet, but have not sent us a packet in a long time.
 When passed a handle, checks that handle for timeout; if passed
 PLAYER_NONE, checks all available handles for timeout.
 If it finds such a host, it calls 
 dpio->openHdl_callback(h, ..., dp_RES_HOST_NOT_RESPONDING) 
 where h is a handle to the timed-out host, and returns h.

 Note: if the connection had an explicit openHdl callback,
 it is confusing that the generic dpio->openHdl_callback() is called here.
 (We should look at whether we should scrap the per-connection callbacks
 and just have a generic one for all connections.)
 
 Also detects those hosts which have actively reset the connection by
 sending us a new SYN with a new packetnumber.
 Will also delete handles which closeHdl has set to close when their
 tx/rx buffers are empty.

 On exit, returns handle of a player that has timed out, or PLAYER_NONE.
-----------------------------------------------------------------------*/
playerHdl_t dpio_findTimedOutHost(dpio_t *dpio, playerHdl_t hdl);

#endif

/*-----------------------------------------------------------------------
 Call frequently to handle retransmissions of packets that have not been
 acknowledged, and to send keepalive packets if needed.
-----------------------------------------------------------------------*/
dp_result_t dpio_update(dpio_t *dpio);

/*-------------------------------------------------------------------------
 Pretend an ACK has been received for all outstanding packets with the
 given tag in the given window.
 (The tag is the first taglen bytes of the packet.)

 This is useful only for higher-level protocols which somehow know
 that packets have gotten through.  For instance, in a turn-based game,
 when the token makes it all the way back around, you know that all the
 packets you sent out have been received, so there's no sense in waiting
 for explicit ACKs anymore.  This can improve performance under conditions
 of heavy packet loss (when the ACKs are likely to be lost).

 Returns dp_RES_OK if one or more packets were affected;
         dp_RES_EMPTY if no packets were affected;
		 dp_RES_BAD if passed invalid arguments;
		 dp_RES_BUG if passed null dpio, or internal error.
-------------------------------------------------------------------------*/
dp_result_t dpio_higherLevelBlanketAck(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	char *tag,
	size_t taglen);


/*-------------------------------------------------------------------------
Debug-only support
-------------------------------------------------------------------------*/
#if !defined(NDEBUG)

#if 0
#define dpio_assertValid(d) {\
	assert((d) != NULL);\
	assoctab_assertValid((d)->conns);\
	validateComm((d)->commPtr);\
}
#else
/* Do an internal consistancy check on the reliable transport */
void dpio_assertReliableValid(dpio_t *dpio);

#define dpio_assertValid(d) {\
	assert((d) != NULL);\
	assoctab_assertValid((d)->conns);\
	dpio_assertReliableValid((d));\
}
#endif

#else
#define dpio_assertValid(d)
#endif

/* Return to default packing. */
#include "dpunpack.h"

#ifdef __cplusplus
}
#endif

#endif
