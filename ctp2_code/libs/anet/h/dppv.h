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

/*------- Player Variables -------------------------------------------------
 Each player (or group) can have 0 or more data elements associated with it.
 These are available automatically to all players in the session.
 If desired, they can be made available to players not in the session.
 (This might be done with a data element indicating mission or vehicle name.)

 The system reserves the first few tags for its own use, so
 games should only use tags dp_PLAYERDATA_KEY_USERDEF through
 dp_PLAYERDATA_KEY_MAX.

 Example uses:
 * Player's long name (set dp_PLAYERDATA_FLAG_PUBLIC to give lobby access)
 * Custom vehicle or logos
 * Gross position and damage data
 * Game secrets (the password for my fortress is SPAM)
 * Voice taunts
 * Downloadable mission filename and checksum
 * Downloadable missions (set dp_PLAYERDATA_FLAG_NOFLOOD); first call
   to dpGetPlayerData for that key starts the retrieval.

 (See also Linda http://www.sca.com, a more powerful virtual shared memory
  system.  Our system only allows 'triplets' where the
  first two members are the player id and the key number, and the third
  is the payload data; Linda is a bit more general.  Linda also blocks
  if you try to read a variable that isn't set yet, which we can't do,
  since we're single-threaded.)

 $Log: dppv.h $
 Revision 1.6  1997/05/31 22:42:22  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.5  1997/02/26 23:04:56  dkegel
 Moved pv_user_playerData_packet_t to dppkt.h as dp_user_ etc.
 Revision 1.4  1997/02/25 01:42:38  dkegel
 1. Player variables now preserved across freeze/thaw cycles.
 2. pv_get now supports getting just the length by passing NULL buffer.
 Revision 1.3  1997/02/23 05:51:38  dkegel
 Rewrote to use pv_update instead of sending everything immediately.
 Should be much more robust.
 Revision 1.1  1997/02/12 03:06:47  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef pv_h
#define pv_h

#include "assoctab.h"
#include "dppkt.h"

/* Use pack(1) because all the structures in this file are
 * serialized at one time or another to disk or to the network.
 */
#include "dppack1.h"

/* Values for pv_var_t.flags */
#define dp_PLAYERDATA_FLAG_NOFLOOD 0x01	/* Not sent except on request */
#define dp_PLAYERDATA_FLAG_PUBLIC 0x02	/* Available outside session */
#define dp_PLAYERDATA_FLAG_DIRTY 0x10	/* pv_set called since last broadcast */

#define dp_PLAYERDATA_KEY_NONE 0xffff	/* illegal key */
#define dp_PLAYERDATA_KEY_USERMAX 0xff00/* max user key; other vars reserved */
#define dp_PLAYERDATA_NKEYS_MAX 32		/* max number of used keys */
#define dp_PLAYERDATA_LEN_MAX 0x20000	/* max size of variable is 128KB */

#define pv_VAR_TYPE_LEN 4	/* MS-DOS filename extension, null terminated */

/* A single variable owned by a particular peer */
typedef struct {
	unsigned short key PACK;/* tag for this data element */
	short flags PACK;		/* see #defines above */
	size_t len PACK;		/* length of user data */
	long crc PACK;			/* checksum */
	char type[pv_VAR_TYPE_LEN] PACK;/* MS-DOS filename extension, null terminated */
	size_t offset PACK;		/* if not yet fully transferred, next byte to transfer */
	void *buf PACK;			/* pointer to malloc'd user data */
} pv_var_t;

/* A peer and all its variables */
typedef struct {
	assoctab_t *vars PACK;	/* current variables */
	int dirty PACK;			/* boolean set by pv_set, cleared by pv_update. */
	size_t allocated PACK;	/* bytes allocated for incoming.buf */
	pv_var_t incoming PACK;	/* incoming variable held here until complete */
} pv_peer_t;

/* pv_BYTES_PER_SEC is the bandwidth you wish to allocate to
 * pv_t's broadcasts. Must yield < 1/dpio::minTxInterval pkts per sec
 * to avoid locking out dpio's primitive retransmission scheme.
 */
#define pv_BYTES_PER_SEC 500

/* The mother object that holds all the players' variables
 * and knows how to propagate them.
 */
typedef struct {
	assoctab_t	 *peers;	/* assoctab_t* of pv_peer_t */

	struct dp_s *dp;

	/* Alas, because of the lack of multithreading, we must
	 * explicitly represent our progress through the various
	 * update cycles rather than using local variables.
	 * The pv_update() routine tries to send a few packets at a time,
	 * stops if it senses that the output channel is nearly full, and
	 * stores its progress in the following variables until the next call.
	 */

	/* On each call to pv_update(), we update one or more hosts about
	 * (part of) a variable's value.  The following variables manage that.
	 */

	/* We decide how often to send by guessing the capacity of the
	 * output channel.  Initially, this will be fixed at pv_BYTES_PER_SEC.
	 * Each time we send, we schedule our next send to be when we
	 * expect the data to have cleared the channel.
	 */
	clock_t next_send;

	/* cur_offset is the start of the region of the variable we're on */
 	size_t cur_offset;
 
	/* cur_keys lists the variables we're updating this cycle;
	 * cur_keys[cur_key_index] is the key of the variable we're on.
 	 */
 	int cur_keys[dp_PLAYERDATA_NKEYS_MAX];
	int cur_nkeys;
 	int cur_key_index;

	/* cur_dests lists the hosts we're updating this cycle;
	 * new_dests lists the hosts that have been added since last cycle.
	 * A playerHdl_t[] is used because that's just what dpio_put wants.
 	 */
	playerHdl_t cur_dests[dp_MAXPLAYERS] PACK;
	int cur_ndests PACK;
	playerHdl_t new_dests[dp_MAXPLAYERS] PACK;
	int new_ndests PACK;
} pv_t;

/*--------------------------------------------------------------------------
 Create a pv_t.
--------------------------------------------------------------------------*/
pv_t *pv_create(struct dp_s *dp);

/*--------------------------------------------------------------------------
 Destroy a pv_t.
--------------------------------------------------------------------------*/
void pv_destroy(pv_t *pv);

/*--------------------------------------------------------------------------
 Call pv_update() periodically to propagate player variables to our peers.
 Each call results in sending at most one packet to one or more destinations,
 so it must be called frequently (>= 5 times/sec) to keep the data flowing.
 Only propagate the variables owned by the local player!  Since there can be
 several local players, punt and make the variable owner a parameter
 rather than propagating all local player's variables.  
--------------------------------------------------------------------------*/
dp_result_t pv_update(pv_t *pv, dpid_t owner);

/*--------------------------------------------------------------------------
 Delete a particular player from the table.
--------------------------------------------------------------------------*/
dp_result_t pv_deletePlayer(pv_t *pv, int player);

/*--------------------------------------------------------------------------
 Notify pv that a new node has entered the session,
 so pv can send him the variable table.
--------------------------------------------------------------------------*/
dp_result_t pv_addNode(pv_t *pv, playerHdl_t node);

/*--------------------------------------------------------------------------
 Set variable 'key' for a player.
 Flags currently ignored.
--------------------------------------------------------------------------*/
dp_result_t pv_set(pv_t *pv, int player, int key, size_t len, void *buf, int flags);

/*--------------------------------------------------------------------------
 Get a variable for a player.
 Flags currently ignored.
--------------------------------------------------------------------------*/
dp_result_t pv_get(pv_t *pv, int player, int key, size_t *len, void *buf, int flags);

/* See dp_user_playerData_packet_t and dp_USER_PLAYERDATA_PACKET_ID
 * in dppkt.h
 */
#if 0
/*--------------------------------------------------------------------------
 Packet carrying a variable change.
 Note: large variables will require the reliable data transport to
 fragment and reassemble large packets...
--------------------------------------------------------------------------*/
#define pv_USER_PLAYERDATA_PACKET_ID		dppt_MAKE('d','u')
typedef struct {
	size_t len PACK;			/* let them be huge */
	dpid_t id PACK;
	unsigned short key PACK;
	void *data PACK;			/* only sent on local machine, ptrs ok */
} pv_user_playerData_packet_t;
#endif

/* First (and maybe last) packet is sent with this header */
#define pv_PLAYERDATA_INITIAL_PACKET_ID		dppt_MAKE('d','v')
typedef struct {
	unsigned short key PACK;	/* tag for this data element */
	short flags PACK;				/* see #defines above */
	size_t len PACK;			/* length of user data */
	long crc PACK;				/* checksum */
	char type[pv_VAR_TYPE_LEN] PACK;	/* MS-DOS filename extension or 0 */
	dpid_t id PACK;
} pv_playerData_initial_packet_t;
#define pv_PLAYERDATA_INITIAL_MAXLEN (dpio_MAXLEN_RELIABLE - sizeof(pv_playerData_initial_packet_t) - sizeof(dp_packetType_t))

/* 2nd, 3rd, etc. packets are sent with this packet type and header. */
#define pv_PLAYERDATA_BODY_PACKET_ID		dppt_MAKE('d','w')
typedef struct {
	dpid_t id PACK;				/* wish we didn't have to send that */
} pv_playerData_body_packet_t;
#define pv_PLAYERDATA_BODY_MAXLEN (dpio_MAXLEN_RELIABLE - sizeof(pv_playerData_body_packet_t) - sizeof(dp_packetType_t))

/* Recipient of one of the above can send this to report an error
 * or reject a transmission.
 */
#define pv_PLAYERDATA_REPLY_PACKET_ID		dppt_MAKE('d','x')
typedef struct {
	/* Enough info to identify the transmission in question. */
	long crc PACK;				/* checksum */
	char type[pv_VAR_TYPE_LEN] PACK;	/* MS-DOS filename extension or 0 */
	dpid_t id PACK;
	/* The error - either BAD (for bad CRC) or ALREADY (for stop sending,
	 * please, I'm british :-)
	 */
	dp_result_t err PACK;
} pv_playerData_reply_packet_t;

/*--------------------------------------------------------------------------
 Process a network message generated by this module on another computer.
 Caller should check to make sure 1st two bytes are pv_PLAYERDATA_PACKET_ID	

 Kludge: caller should also pass the same messages to the user code.

 Note: player deletion should be noticed by some other handler, and
 should cause a call to pv_deletePlayer().
--------------------------------------------------------------------------*/
dp_result_t pv_handlePacket(pv_t *pv, size_t len, void *buf);

/*-----------------------------------------------------------------------
 Save the state of a pv_t to disk.
-----------------------------------------------------------------------*/
dp_result_t pv_freeze(pv_t *pv, FILE *fp);

/*-----------------------------------------------------------------------
 Restore the state of a pv_t from disk.
 Call after creating a virgin pv_t.
-----------------------------------------------------------------------*/
dp_result_t pv_thaw(pv_t *pv, FILE *fp);

#include "dpunpack.h"

#endif



