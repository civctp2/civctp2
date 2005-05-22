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
 Simple program to test network latency under different periodic update
 regimes.
 Models the behavior of an action game.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include "anet.h"
#include "eclock.h"
#include "assoctab.h"

#define MS2ECLOCKS(m) (((m)*ECLOCKS_PER_SEC)/1000)
#define ECLOCKS2MS(e) (((e)*1000)/ECLOCKS_PER_SEC)

/* Structures that are sent over the network must be packed, so
 * bracket them with the standard include files dppack1.h and dpunpack.h.
 */
#include "dppack1.h"

/* All the packets specific to this program start with B for convenience */

/* Sent by clients to tell the host they are ready to receive */
#define LOGIN_PACKET_ID dppt_MAKE('B','I')

/* Packets which simulate game information */
#define BAND_PACKET_ID dppt_MAKE('B','_')
typedef struct {
	unsigned long pkt_round PACK;  /* incremented when test params change */
	unsigned long pkt_num PACK;    /* incremented with each band packet sent */
} band_packet_t;

/* Sent as a reply to a band packet to allow measurement of latency */
#define LATENCY_PACKET_ID dppt_MAKE('B','R')
typedef struct {
	unsigned long pkt_num PACK;    /* this is a reply to packet# pkt_num */
	clock_t max_ping PACK;         /* the maximum ping I see locally */
} latency_packet_t;

/* Sent by host to notify clients of new test parameters */
#define TESTPARAMS_PACKET_ID dppt_MAKE('B','T')
typedef struct {
	long pkt_interval PACK;   /* the new interval between packet sends (ms) */
	long pkt_size PACK;       /* the new packet size to send (Bytes) */
} testParams_packet_t;

/* Sent by host to notify clients of the <dll>.ini settings at login time */
#define DLLINI_PACKET_ID dppt_MAKE('B','D')
typedef struct {
	int LimitLowNPPI;
	int LimitHiNPPI;
	int DivisorPing2NPPI;
} dllIni_packet_t;

typedef struct {
	dp_packetType_t type PACK;
	union {
		dp_user_addPlayer_packet_t addPlayer;
		dp_user_delPlayer_packet_t delPlayer;
		dp_objectDelta_packet_t objectDelta;
		band_packet_t band;
		latency_packet_t latency;
		testParams_packet_t testParams;
		dllIni_packet_t dllIni;
		unsigned char buf[dpio_MAXLEN_UNRELIABLE];
	} u PACK;
} pkt_t;

#include "dpunpack.h"

/*-----------------------------------------------------------------------------
  Any information about other players is stored in the ids table which is
  an associative array of this structure
-----------------------------------------------------------------------------*/
typedef struct {
	clock_t last_ping;      /* last ping to him */
	clock_t last_max_ping;  /* last maximum ping he reported seeing */
} playerInfo_t;


/*-----------------------------------------------------------------------------
  The information about each packet sent needed to match it with a ping reply
  and calculate the round trip ping.
-----------------------------------------------------------------------------*/
typedef struct {
	unsigned pkt_num;
	clock_t time_sent;
} sentList_entry_t;

/* number of elements to keep in the circular buffer of recently sent packets
 * used for looking up sent times when calulating pings
 */
#define SENT_LIST_LENGTH 200

/*-----------------------------------------------------------------------------
  State of the bandwidth testing algorithm is stored here (globally)  
-----------------------------------------------------------------------------*/
struct {
	int pkt_interval;           /* interval in ms between band packets */
	int pkt_size;               /* size in bytes of band packets */
	int round_duration;         /* duration of current test round */
	int adaptive_interval;      /* actual interval to use when pkt_interval = 0
								 * indicates we should adapt based on pings
								 */
	clock_t max_local_ping;     /* maximum of pings seen from here to others */
	clock_t last_receive_time;  /* last time packet was rec'd from another */
	unsigned pkt_num;           /* sequence number of current packet */
	unsigned pkt_round;         /* index of current round */
	int I_am_host;              /* boolean, I am host */
	int I_am_logged_in;         /* boolean, I have sent login packet to host */
	dp_t *dp;                   /* the network transport object */
	dpid_t myid;                /* my dp id */
	dpid_t hostid;              /* the hosts dp id */
	clock_t next_send_time;     /* time to send next band packet */ 
	clock_t next_round_time;    /* time to end this test round (host only) */ 
	clock_t next_ping_time;     /* time to send next ping reply */
	clock_t next_update_time;   /* time to send next interval update, used when
								 * pkt_interval = -1 indicates host should
								 * adapt global interval based on his pings.
								 */
	                            /* tuning parameters from <dll>.ini: */
	int LimitLowNPPI;           /* lower limit (ms) on adaptive intervals */
	int LimitHiNPPI;            /* upper limit (ms) on adaptive intervals */
	int DivisorPing2NPPI;       /* number (1/100ths) to divide ping by to get
								 * adaptive interval.
								 */
	assoctab_t *ids;            /* information associated with other players */
	dpid_t replytoid;           /* id to send the next ping reply to */
	int replyto_idx;            /* index of replytoid in *ids */
	int quitState;              /* state var handling behavior at quit time */
	clock_t tfinish;            /* time to really exit, used at quit time */
	FILE *infp;                 /* pointer to test parameters input file */ 
	sentList_entry_t sentList[SENT_LIST_LENGTH];
	                            /* list of info about recently sent packets
								 * used to match ping replies to find pings.
								 */
} band;


void quit(int errcode)
{
	/* get a key so we can see messages before the console window closes */
	printf("Press any key to continue\n");
	getch();
	exit(errcode);
}


void print_usage(char *errmsg)
{
	printf("\
Usage: band [-h] [-f infile]\n\
    -h prints this usage message\n\
    -f specifies the name of an input file.  stdin is used otherwise.\n\
    the input file should contain lines with 3 integers each:\n\
    pkt_interval pkt_size round_duration (remainder of line ignored)\n\
    each is run in order and starts a test of duration round_duration secs\n\
    with all participants transmitting a pkt_size byte packet every\n\
    pkt_interval ms for positive values of pkt_interval.\n\
\n\
    pkt_interval = 0 results in a special adaptive interval behavior\n\
    in which the interval is set to the maximum ping seen by anyone divided\n\
    by DivisorPing2NPPI read from the tuning parameters file <dll>.ini.\n\
\n\
    pkt_interval = -1 results in a similar adaptive bahavior, but one in\n\
    which the host sets the interval for everyone based only on the maximum\n\
    ping seen locally, updating the others every 2 seconds.\n\
\n%s\n", errmsg);
	quit(1);
}


/*-----------------------------------------------------------------------------
  Send the current test parameters over the net to one or all other players.
-----------------------------------------------------------------------------*/
int send_testParams(dp_t *dp, dpid_t myid, dpid_t dest, int pkt_interval, int pkt_size)
{
	dp_result_t err;
	pkt_t pkt;

	pkt.type = TESTPARAMS_PACKET_ID;
	pkt.u.testParams.pkt_interval = pkt_interval;
	pkt.u.testParams.pkt_size = pkt_size;
	err = dpSend(dp, myid, dest, dp_SEND_RELIABLE, &pkt,
				 sizeof(pkt.u.testParams) + sizeof(pkt.type));
	return err;
}


/*-----------------------------------------------------------------------------
 Call this in between each simulation cycle of the game.
 Handles all network I/O for the game.
 Return value is 0 on success,
                 1 if test over,
				 2 on session lost
-----------------------------------------------------------------------------*/
int band_poll()
{
	char buf[dpio_MAXLEN_UNRELIABLE];
	clock_t now = eclock();
	dp_result_t err;

	/* receive until we get dp_RES_EMPTY (there are no more packets) or an error */
	while (1) {
		dpid_t idFrom, idTo;
		pkt_t pkt;
		size_t rxsize = sizeof(pkt);

		err = dpReceive(band.dp, &idFrom, &idTo, 0, &pkt, &rxsize);
		if (err != dp_RES_EMPTY)
			DPRINT(("band: dpReceive returned %d idFrom %d idTo %d rxsize %d pkt %c%c\n", err, idFrom, idTo, rxsize, (pkt.type & 0xff), ((pkt.type & 0xff00) >> 8)));
		switch (err) {
		case dp_RES_MODEM_NORESPONSE:
			printf("band: modem reports NO CARRIER!\n");
			break;
		case dp_RES_HOST_NOT_RESPONDING:
			printf("band: connection to game server lost!\n");
			break;
		case dp_RES_EMPTY:
			break;
		case dp_RES_OK:
			break;
		default :
			printf("band: error %d receiving packet.\n", err);
			break;
		}
		if (err != dp_RES_OK)
			break;
		/* printf("band: packet type %c%c received\n",
		   (pkt.type & 0xff), ((pkt.type & 0xff00) >> 8)); */
		switch (pkt.type) {
		case dp_OBJECTDELTA_PACKET_ID:
			if (pkt.u.objectDelta.key[0] == dp_KEY_PLAYERS) {
				dpid_t *pid;

				switch (pkt.u.objectDelta.status) {
				case dp_RES_CREATED:
					if (pkt.u.objectDelta.flags & dp_OBJECTDELTA_FLAG_LOCAL) {
						band.myid = pkt.u.objectDelta.data.p.id;
						printf("band: my id is %d\n", band.myid);
						if (pkt.u.objectDelta.flags & dp_OBJECTDELTA_FLAG_ISHOST) {
							band.I_am_host = TRUE;
							printf("band: I am the host, reading params\n");
						}
					} 
					if (pkt.u.objectDelta.flags & dp_OBJECTDELTA_FLAG_ISHOST) {
						band.hostid = pkt.u.objectDelta.data.p.id;
						printf("band: host id is %d\n", band.hostid);
					}
					/* Record this new player's id */
					pid = assoctab_subscript_grow(band.ids, pkt.u.objectDelta.data.p.id);
					assert(pid);
					break;
				case dp_RES_DELETED:
					/* Delete this old player's id */
					assoctab_subscript_delete(band.ids, pkt.u.objectDelta.data.p.id);
					if (pkt.u.objectDelta.data.p.id == band.replytoid)
						band.replytoid = dp_ID_NONE;
					if (pkt.u.objectDelta.data.p.id == band.hostid &&
						band.quitState == 0)
						band.quitState++;  /* host is quitting, I should too */
					break;
				}
			}
			break;
		case LOGIN_PACKET_ID:
			/* Gotcha #1: 
			 * External launchers eat even reliable packets received 
			 * before we were launched (or before dpCreate returns).
			 * To avoid losing any packets, don't send game stuff until 
			 * the client notifies us that he has entered the game proper,
			 * and is ready to receive.
			 */
			assert(band.I_am_host && band.round_duration != 0);
			printf("band: player %d logged in, sending params\n", idFrom);
			
			/* Send them a <dll>.ini packet so everyone's net parameters agree */
			{
				pkt_t dllinipkt;

				dllinipkt.type = DLLINI_PACKET_ID;
				dllinipkt.u.dllIni.LimitLowNPPI = band.LimitLowNPPI;
				dllinipkt.u.dllIni.LimitHiNPPI = band.LimitHiNPPI;
				dllinipkt.u.dllIni.DivisorPing2NPPI = band.DivisorPing2NPPI;	
				err = dpSend(band.dp, band.myid, idFrom, dp_SEND_RELIABLE, &dllinipkt,
							 sizeof(pkt.type) + sizeof(dllIni_packet_t));
				if (err != dp_RES_OK)
					printf("band: warning: dpSend returned %d on <dll>.ini packet\n", err);
			}
			
			/* send them test parameters, to bring them into the current test round */
			err = send_testParams(band.dp, band.myid, idFrom, band.pkt_interval, band.pkt_size);
			if (err != dp_RES_OK)
				printf("band: warning: send_testParams returned %d\n", err);
			break;
		case TESTPARAMS_PACKET_ID:
			assert(!band.I_am_host);
			band.pkt_interval = pkt.u.testParams.pkt_interval;
			band.pkt_size = pkt.u.testParams.pkt_size;
			printf("band: received new test params: interval %d size %d\n", ECLOCKS2MS(band.pkt_interval), band.pkt_size); 
			band.pkt_round++;
			break;
		case DLLINI_PACKET_ID:
			assert(!band.I_am_host);
			band.LimitLowNPPI = pkt.u.dllIni.LimitLowNPPI;
			band.LimitHiNPPI = pkt.u.dllIni.LimitHiNPPI;
			band.DivisorPing2NPPI = pkt.u.dllIni.DivisorPing2NPPI; 
			printf("band: received <dll>.ini params: LLNPPI %d LHNPPI %d DP2N %.2f\n",
				   ECLOCKS2MS(band.LimitLowNPPI), ECLOCKS2MS(band.LimitHiNPPI), (float)band.DivisorPing2NPPI/16.0);
			break;
		case BAND_PACKET_ID:
			band.last_receive_time = now;
			if ((band.replytoid != dp_ID_NONE) && (idFrom == band.replytoid)) {
				pkt_t replypkt;
					
				memset(&replypkt, 0, dp_MAXLEN_UNRELIABLE);					
				/*printf("band: sending a reply packet to %d.\n", band.replytoid);*/
				replypkt.type = LATENCY_PACKET_ID;
				replypkt.u.latency.pkt_num = pkt.u.band.pkt_num;
				replypkt.u.latency.max_ping = band.max_local_ping;
				err = dpSend(band.dp, band.myid, band.replytoid, dp_SEND_UNRELIABLE, &replypkt, sizeof(dp_packetType_t)+sizeof(latency_packet_t));
				if (err != dp_RES_OK)
					printf("band: warning: dpSend returned %d on ping reply attempt\n", err);
				/* Only reply to one person every second */
				band.replytoid = dp_ID_NONE;
			}
			break;
		case LATENCY_PACKET_ID:
			band.last_receive_time = now;
			{
				int sent_idx;
				playerInfo_t *p;
				assoctab_item_t *pe;
				clock_t ping;
				clock_t max_global_ping;
				int i;

				p = (playerInfo_t *)assoctab_subscript(band.ids, idFrom);
				p->last_max_ping = pkt.u.latency.max_ping;
				
				/* Look up the packet he replied to */
				sent_idx = pkt.u.latency.pkt_num % SENT_LIST_LENGTH;
				if (pkt.u.latency.pkt_num == band.sentList[sent_idx].pkt_num) {
					ping = (now - band.sentList[sent_idx].time_sent);
					if (p != NULL)
						p->last_ping = ping;
				} else {
					if (p != NULL)
						p->last_ping = MS2ECLOCKS(30000);
				}

				/* Calculate the maximum ping we see locally and globally */
				band.max_local_ping = 0;
				max_global_ping = 0;
				for (i=0; i<band.ids->n_used; i++) {
					pe = assoctab_getkey(band.ids, i);
					assert(pe);
					if ((dpid_t)pe->key != band.myid) {
						p = (playerInfo_t *)assoctab_subscript(band.ids, pe->key);
						if (p->last_ping > band.max_local_ping)
							band.max_local_ping = p->last_ping;
						if (p->last_max_ping > max_global_ping)
							max_global_ping = p->last_max_ping;
					}
				}
				if (band.max_local_ping > max_global_ping)
					max_global_ping = band.max_local_ping;

				printf("band: ping to player %2d is %5d, max_local_ping %5d max_global_ping %5d\n",
					   idFrom, ECLOCKS2MS(ping), ECLOCKS2MS(band.max_local_ping), ECLOCKS2MS(max_global_ping));
				DPRINT(("band: id:%d ping:%d max_local_ping:%d, max_global_ping:%d\n",
						idFrom, ECLOCKS2MS(ping), ECLOCKS2MS(band.max_local_ping), ECLOCKS2MS(max_global_ping)));
				
				if (band.pkt_interval == 0) {
					/* special case, band.pkt_interval == 0 means
					 * band.pkt_interval == max_global_ping*16/DivisorPing2NPPI
					 * clamped to [LimitLowNPPI, LimitHiNPPI]
					 */
					band.adaptive_interval = (max_global_ping << 4) / band.DivisorPing2NPPI;
					if (band.adaptive_interval > band.LimitHiNPPI)
						band.adaptive_interval = band.LimitHiNPPI;
					if (band.adaptive_interval < band.LimitLowNPPI)
						band.adaptive_interval = band.LimitLowNPPI;
					DPRINT(("band: adaptive interval:%d\n", ECLOCKS2MS(band.adaptive_interval)));
					printf("band: adaptive interval:%d\n", ECLOCKS2MS(band.adaptive_interval));
				}
			}
		break;
		case dp_SESSIONLOST_PACKET_ID:
			printf("band: Session lost, quitting now.\n");
			if (band.quitState == 0)
				band.quitState++;
			break;
		default:
			break;
		}
	}
	
	if (band.quitState == 1) {
		/* Somebody decided we should quit.  Leave the session, 
		 * then start waiting for network traffic to finish.
		 */
		printf("band: Now Quitting\n");
		dpClose(band.dp);
		/* Since we know we are really going offline, tell the server so
		 * he doesn't have to time out our handle later.
		 */
		dpSetGameServerEx(band.dp, NULL, 0);
		band.quitState++;
		return 0;  /* don't send any more packets, just receive */
	} else if (band.quitState == 2) {
		/* Has network traffic finished yet? */
		if (dpReadyToFreeze(band.dp) != dp_RES_BUSY) {
			clock_t t_wait;
			
		    t_wait = ECLOCKS_PER_SEC + band.max_local_ping * 3;
			if (t_wait > 10*ECLOCKS_PER_SEC)
				t_wait = 10*ECLOCKS_PER_SEC;
			printf("band: Network traffic has finished, leaving in %d s\n",
				   t_wait/ECLOCKS_PER_SEC);
			band.tfinish = now + t_wait;
			band.quitState++;
		}
		return 0;  /* don't send any more packets, just receive */
	} else if (band.quitState == 3) {
		/* Wait a little longer in case we must retransmit anything */
		if ((now - band.tfinish) < 0)
			return 1;  /* really quit now */
	}

	/* Can't send without knowing myid */
	if (band.myid == dp_ID_NONE)
		return 0;
	
	if (((long)(now - band.next_ping_time) >= 0) &&
		(band.ids->n_used > 1)) {
		assoctab_item_t *pe;

		/* Pick player to reply to next, skip myid in the list.
		 * This triggers a reply to the next band packet received from
		 * replytoid.  Each player replies to one player every second
		 * allowing that player to calculate the latency between them.
		 */
		if (band.replytoid != dp_ID_NONE) {
			printf("band: warning: no packet from playerid %d\n", band.replytoid);
			DPRINT(("band: warning: no packet from playerid %d\n", band.replytoid));
		}

		band.replyto_idx = (band.replyto_idx + 1) % (band.ids->n_used);
		pe = assoctab_getkey(band.ids, band.replyto_idx);
		assert(pe);
		if ((dpid_t)pe->key == band.myid) {
			band.replyto_idx = (band.replyto_idx + 1) % (band.ids->n_used);
			pe = assoctab_getkey(band.ids, band.replyto_idx);
			assert(pe);
		}
		band.replytoid = (dpid_t)pe->key;
		/*printf("band: now replying to playerid %d\n", band.replytoid);*/
		band.next_ping_time = now + ECLOCKS_PER_SEC;
	}

	/* If we're hosting, and the previous test round is finished,
	 * read in the new test parameters and send them to the clients.
	 */
	if (band.I_am_host && ((long)(now - band.next_round_time) >= 0)) {
		char input[256];
		int temp_interval, temp_duration;
		
		if (fgets(input, 256, band.infp) &&
			sscanf(input, "%d %d %d", &temp_interval, &band.pkt_size, &temp_duration) == 3 &&
			temp_interval >= -1 && band.pkt_size >= 0 && temp_duration > 0) {
			if (temp_interval > 0)
				band.pkt_interval = MS2ECLOCKS(temp_interval);
			else    /* 0, negative intervals have special meanings */
				band.pkt_interval = temp_interval;
			band.round_duration = temp_duration * ECLOCKS_PER_SEC;
			if (band.pkt_interval == -1) {
				/* special case, locally interpretted by host to mean
				 * 'use adaptive interval based on host's max_local_ping
				 */
				printf("band: changing to adaptive interval based on host's max_ping (size %d)\n", band.pkt_size);
				band.next_update_time = now;
			} else {
				printf("band: broadcasting new params - interval %d size %d (duration %d)\n", temp_interval, band.pkt_size, temp_duration);
				err = send_testParams(band.dp, band.myid, dp_ID_BROADCAST, band.pkt_interval, band.pkt_size);
				if (err != dp_RES_OK)
					printf("band: warning: send_testParams returned %d\n", err);
			}
		} else {
			printf("band: exiting, no more parameters available\n");
			band.quitState++;  /* quit nicely */
		}
		band.pkt_round++;
		band.next_round_time = now + band.round_duration;
		printf("band: sending packets with interval %d size %d duration %d\n",
			   ECLOCKS2MS(band.pkt_interval), band.pkt_size, band.round_duration/ECLOCKS_PER_SEC);
	}

	/* If we're hosting and using the host's pings only to determine an
	 * adaptive interval, send out a new testParams packet every 2 seconds
	 * with the appropriate values.
	 */
	if (band.I_am_host && (band.pkt_interval == -1) && ((long)(now - band.next_update_time) >= 0)) {
		band.adaptive_interval = (band.max_local_ping << 4) / band.DivisorPing2NPPI;

		if (band.adaptive_interval > band.LimitHiNPPI)
			band.adaptive_interval = band.LimitHiNPPI;
		if (band.adaptive_interval < band.LimitLowNPPI)
			band.adaptive_interval = band.LimitLowNPPI;
		printf("band: broadcasting new params - interval %d size %d (host adaptive)\n",
			   ECLOCKS2MS(band.adaptive_interval), band.pkt_size);
		err = send_testParams(band.dp, band.myid, dp_ID_BROADCAST, band.adaptive_interval, band.pkt_size);
		if (err != dp_RES_OK)
			printf("band: warning: send_testParams returned %d\n", err);
		band.next_update_time = now + 2*ECLOCKS_PER_SEC;
  	}
	
	/* Gotcha #1: 
	 * External launchers eat even reliable packets received 
	 * before we were launched (or before dpCreate returns).
	 * To avoid losing any packets, notify the host that this player
	 * has entered the game proper, and is ready to receive.
	 */
	if (band.hostid != dp_ID_NONE && !band.I_am_logged_in && !band.I_am_host) {
		*((int *)buf) = LOGIN_PACKET_ID;
		err = dpSend(band.dp, band.myid, band.hostid, dp_SEND_RELIABLE, buf, 2);
		if (err != dp_RES_OK)
			printf("band: warning: dpSend returned %d at login\n", err);
		band.I_am_logged_in = TRUE;
	}

	if ((long)(now - band.next_send_time) >= 0) {
		int sent_idx;
		pkt_t pkt;
		clock_t dt;
			
		memset(&pkt, 0, dpio_MAXLEN_UNRELIABLE);
		pkt.type = BAND_PACKET_ID;
		pkt.u.band.pkt_round = band.pkt_round;
		pkt.u.band.pkt_num = band.pkt_num;
		err = dpSend(band.dp, band.myid, dp_ID_BROADCAST, dp_SEND_UNRELIABLE, &pkt, band.pkt_size);
		if (err != dp_RES_OK)
			printf("band: warning: dpSend returned %d on a band packet\n", err);
		sent_idx = band.pkt_num % SENT_LIST_LENGTH;
		band.sentList[sent_idx].time_sent = now;
		band.sentList[sent_idx].pkt_num = band.pkt_num;
		band.pkt_num++;

		/* Look for total network dropouts in the incoming packets. */
		dt = (now - band.last_receive_time);
		if (((long)(dt - band.LimitHiNPPI * 2)) > 0) {
			printf("band: overdue; last rx %d ms ago!\n", ECLOCKS2MS(dt));
			DPRINT(("band: overdue; last rx %d ms ago!\n", ECLOCKS2MS(dt)));
		}

		if (band.pkt_interval == 0 || band.pkt_interval == -1) {
			band.next_send_time = now + band.adaptive_interval;
		} else {
			band.next_send_time = now + band.pkt_interval;
		}
	}
	return 0;
}


void __cdecl main(int argc, char **argv)
{
	int i;
	int banderr;
	dp_result_t err;
	dp_session_t sess;
	size_t size = sizeof(sess);
	char key[dp_KEY_MAXLEN+1];	
	int keylen;
	dp_transport_t dllname;
	comm_driverInfo_t dllinfo;
	
	band.replyto_idx = 0;
	band.infp = stdin;
	memset(band.sentList, 0, SENT_LIST_LENGTH * sizeof(sentList_entry_t));
	
	for (i=1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'f':
				if (i + 1 >= argc)
					print_usage("no filename found after -f");
				i++;
				if ((band.infp = fopen(argv[i], "r")) == NULL)
					print_usage("could not open file specified");
				break;
			case 'h':
				print_usage("");
				break;
			default :
				print_usage("invalid argument");
				break;
			}
		} else
			print_usage("invalid argument");
	}

	/* Allocate tables */
	band.ids = assoctab_create(sizeof(playerInfo_t));  /* indexed by dpid */

	/* note: dpCreate may eat the first few packets we receive! */
	err = dpCreate(&band.dp, NULL, NULL, "freeze.dat");
	printf("band: dpCreate returns %d\n", err);
	if (err != dp_RES_OK) {
		printf("band: error %d thawing freeze.dat.\n", err);
		quit(1);
	}

	/* default tuning parameters */
	band.LimitLowNPPI = MS2ECLOCKS(100);  /* 100 ms in ECLOCKS */
	band.LimitHiNPPI = MS2ECLOCKS(500);   /* 500 ms in ECLOCKS */
	band.DivisorPing2NPPI = (300 * 4)/ 25;  /* 3.00 */

	/* Load the network tuning parameters from <dll>.ini */
	err = dpGetCurrentTransportInfo(band.dp, &dllname, &dllinfo);
	DPRINT(("band: dpGetCurrentTransportInfo returns %d\n", err));
	if (err == dp_RES_OK) {
		char *dot = strrchr(dllname.fname, '.');
		
		DPRINT(("band: dpGetCurrentTransportInfo says fname %s\n", dllname.fname));
		if (dot) {
			strcpy(dot, ".ini");
			band.LimitLowNPPI = GetPrivateProfileInt("Net", "LimitLowNPPI", band.LimitLowNPPI, dllname.fname);
			band.LimitHiNPPI = GetPrivateProfileInt("Net", "LimitHiNPPI", band.LimitHiNPPI, dllname.fname);
			band.DivisorPing2NPPI = GetPrivateProfileInt("Net", "DivisorPing2NPPI", band.DivisorPing2NPPI, dllname.fname);

			/* Limits are in ms, but ECLOCKS are used internally */
			band.LimitLowNPPI = MS2ECLOCKS(band.LimitLowNPPI);
			band.LimitHiNPPI = MS2ECLOCKS(band.LimitHiNPPI);
			/* Divisors are in 100ths (i.e. 300 -> 3.00), but 16ths are used internally */
			band.DivisorPing2NPPI = (band.DivisorPing2NPPI * 4) / 25;
			
			DPRINT(("band: loaded tuning data from %s; LLNPPI %d, LHNPPI %d, DP2NPPI %.2f\n", dllname.fname, ECLOCKS2MS(band.LimitLowNPPI), ECLOCKS2MS(band.LimitHiNPPI),  (float)band.DivisorPing2NPPI/16.0));
			printf("band: loaded tuning data from %s; LLNPPI %d, LHNPPI %d, DP2NPPI %.2f\n", dllname.fname, ECLOCKS2MS(band.LimitLowNPPI), ECLOCKS2MS(band.LimitHiNPPI), (float)band.DivisorPing2NPPI/16.0);
		}
	}

	err = dpGetSessionDesc(band.dp, &sess, &size);
	if (err != dp_RES_OK) {
		printf("band: dpGetSessionDesc returns error %d\n", err);
		quit(1);
	}

	key[0] = dp_KEY_PLAYERS;
	err = dpGetSessionId(band.dp, &sess, &key[1], &keylen);
	keylen++;
	if (err != dp_RES_OK) {
		printf("band: dpGetSessionId returns error %d\n", err);
		quit(1);
	}
	
	err = dpRequestObjectDeltas(band.dp, TRUE, key, keylen);	
	if (err != dp_RES_OK) {
		printf("band: error %d from dpRequestObjectDeltas\n", err);
		quit(1);
	}

	band.pkt_num = 1;
	band.pkt_round = 0;
	band.I_am_host = 0;
	band.I_am_logged_in = 0;
	band.pkt_interval = 0;
	band.pkt_size = 0;
	band.round_duration = 0;
	band.next_send_time = eclock();              /* = now, i.e. ASAP */
	band.next_round_time = band.next_send_time;  /* = now */
	band.next_ping_time = band.next_send_time;   /* = now */
	band.next_update_time = band.next_send_time; /* = now */
	band.last_receive_time = band.next_send_time; /* = now */
	band.myid = dp_ID_NONE;
	band.hostid = dp_ID_NONE;
	band.replytoid = dp_ID_NONE;
	band.adaptive_interval = MS2ECLOCKS(300);  /* a safe initial interval - 300ms */
	band.max_local_ping = MS2ECLOCKS(500);
	band.quitState = 0;
	banderr = 0;
	while (!banderr) {
		banderr = band_poll();
	}
	dpDestroy(band.dp, 0);
	quit(0);
}
