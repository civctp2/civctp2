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

/*--------------------------------------------------------------------------
 Essential anet types and constants.
 These have been split off from anet.h so they can be used in places where
 it is inconvenient to include all of anet.h.
--------------------------------------------------------------------------*/
#ifndef aneterr_h
#define aneterr_h

/* Delivery service types */
#define dp_SEND_UNRELIABLE 0
#define dp_SEND_RELIABLE 1

/* Delivery service options - bitwise OR'd with delivery service types */
/* These set how much of the send queue you want to leave empty
 * Using dp_SEND_RESERVE_NONE means that you can completely fill up the
 * reliable send queue.
 * Using dp_SEND_RESERVE_THREEQUARTERS means that dpSend will return FULL
 * when the reliable send queue is one quarter or more full.
 * Use dp_SEND_RESERVE_THREEQUARTERS for reliable packets that can be
 * merged together for more efficient sending; when dpSend returns FULL,
 * you should merge your packets together to reduce the number of sends.
 */
#define dp_SEND_RESERVE_NONE          0
#define dp_SEND_RESERVE_QUARTER       2
#define dp_SEND_RESERVE_HALF          4
#define dp_SEND_RESERVE_THREEQUARTERS 6

/*  Packet ID definitions */
typedef unsigned short dp_packetType_t;

#define sizeof_dpio_reliable_header 5

/* Max packet size Limit for dpio_put_unreliable. */
#define dpio_MAXLEN_UNRELIABLE 255
/* Limit for dpio_put_reliable. */
#define dpio_MAXLEN_RELIABLE (dpio_MAXLEN_UNRELIABLE - sizeof_dpio_reliable_header)

#define dp_MAXDPIDS		64000   /* kludge.  Should be unlimited. */
#define dp_FIRST_DPID      40	/* dpid's less than dp_FIRST_DPID are reserved
								 * for system use as group ids, etc.
								 */
#define dp_ID_NONE		(dp_MAXDPIDS+1)
typedef unsigned short dpid_t;	/* Id for a player.  Unique among players
								 * currently in the session, but may be reused
								 * after ~16000 players have come and gone.
								 */

typedef unsigned long dp_uid_t;			/*  Unique id for user. Never reused. */
#define dp_UID_NONE 0

#define dp_KARMA_NONE	0
typedef short dp_karma_t;	/*  Sort of like a UUID or GUID, but dynamic. */

/* 2 byte network byte order unicode character type */
typedef short dp_netchar_t;

/*  return type of all dp functions. */
typedef int dp_result_t;

/* type to distinguish one game type from another */
typedef unsigned short dp_species_t;
/* Special value for dp_species_t: */
#define DP_ILLEGAL_SPECIES	0xFFFF

/* Error codes common to many dp functions.
 * (Note to maintainers: Once one of these numbers is assigned a meaning,
 * it should never be reused with a new meaning.)
 */
#define dp_RES_OK					0
#define dp_RES_NOMEM				1	/* Out of memory.  Very bad. */
#define dp_RES_EMPTY				2	/* Requested data not present. */
#define dp_RES_FULL					3	/* Can't write; buffer too full. */
#define dp_RES_BADSIZE				4	/* Arg wrong size, or internal error */
#define dp_RES_BAD					5	/* fatal or at least nasty; caller's fault */
#define dp_RES_BUG					8	/* fatal; library's fault */
#define dp_RES_NETWORK_NOT_RESPONDING	15
#define dp_RES_HOST_NOT_RESPONDING		16

#define dp_RES_BUSY					6	/* resource in use; try later */
#define dp_RES_ALREADY				7	/* too late - can't do that anymore */
#define dp_RES_NOTYET				32	/* Something else must be done first */
#define dp_RES_AGAIN				28	/* internal retry; not seen by user */

#define dp_RES_USER_CANCELLED		29	/* User cancelled some action */
#define dp_RES_CANT_WRITE			30	/* file permission, can't write */
#define dp_RES_ACCESS				31	/* access denied: bad password, etc. */
#define dp_RES_HELP					33	/* User needs help */

#define dp_RES_PEER_CLOSED			24	/* caller referred to player who is closing, or for whom dpSend can't find a network address */

#define dp_RES_UNIMPLEMENTED		101

/* Error codes specific to dpCreate */
#define dp_RES_MODEM_BUSY			9
#define dp_RES_MODEM_NOANSWER		10
#define dp_RES_MODEM_NODIALTONE		11
#define dp_RES_MODEM_NORESPONSE		12
#define dp_RES_MODEM_BAD_INITSTRING	22
#define dp_RES_NETWORK_NOT_PRESENT	13
#define dp_RES_COMMDLL_BAD_VERSION	14

/* Some connection state types for callbacks */
#define dp_RES_OPENED				17
#define dp_RES_CLOSED				18
/* Some transfer state values for callbacks */
#define dp_RES_STARTED				19
#define dp_RES_FINISHED				20
#define dp_RES_DELETED				21
#define dp_RES_CREATED				26	/* new object */
#define dp_RES_CHANGED				27	/* old object, but new value */

#endif
