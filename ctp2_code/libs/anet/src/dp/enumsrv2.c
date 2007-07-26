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

/*-----------------------------------------------------------------------
 Code to list the available Activenet servers, and see how far away they are.

 $Log: enumsrv2.c $
 Revision 1.4  1997/10/07 23:27:50  lbennett
 Removed compiler warnings.
 Revision 1.3  1997/09/29 05:53:07  dkegel
 1. Don't clear ping result on addServer if already in table
 2. have addServer send first ping if not yet in table
 Revision 1.2  1997/09/29 02:24:36  dkegel
 Clear ping results when loading old server table.
 Revision 1.1  1997/09/28 03:54:39  dkegel
 Initial revision

 Copied from Revision 1.27 1997/09/16 of enumserv.c
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

#include "dp.h"
#include "dpprivy.h"	/* for dpSwapPingPacket(), dp_saveServerList(), dpSendObjectDelta() */

#include "eclock.h"
#define myclock eclock

/*--------------------------------------------------------------------------
 Strategy

 We have a hardcoded bootstrap list of servers in this source file.
 We also grab the current server (if any)'s list of servers (dp_KEY_SERVERS).
 Both of these are combined to form the list of servers to ping, which is
 kept in a table called dp_KEY_SERVERPINGS.
 Servers that do not respond are deleted from this table if they do not
 appear in either of the two source tables.
 This table also includes current ping results, and is written out to disk
 each time a new server is added to it or a server is deleted from it.
 Finally, this table includes fields for # of players and max # of players
 for future use; they are currently set to zero.

 We use a dptab table for the server ping results so that user code may
 use the dptab functions for enumerating servers.
--------------------------------------------------------------------------*/

/* Hidden definition of the reserved field in dp_serverInfo_t */
#include "dppack1.h"
typedef struct {
	long reserved PACK;
	short i_rtt PACK;					/* next entry in rtt_ms to overwrite */
	short rtt_ms[dp_SERVER_NPINGS] PACK;/* roundtrip time, msec; -1 if empty */
} dp_serverInfo_reserved_t;
#include "dppackn.h"

#define RES(p) ((dp_serverInfo_reserved_t *)((p)->reserved))

/* Where we store the server list locally between runs */
#define SERVERS_CACHE_FILE "servers.dat"

/*--------------------------------------------------------------------------
 Ping a server.
 Update its record to indicate a ping has been sent.
--------------------------------------------------------------------------*/
static void sendServerPing(dp_t *dp,dp_serverInfo_t *server, char *adrbuf, dp_species_t sessType)
{
	dp_result_t err;
	struct {
		dp_packetType_t tag;
		dp_ping_packet_t body;
	} PACK pkt;
	playerHdl_t h;
	dp_species_t sessbuf;
	
	/* Note: fields of ping packet are:
		dp_karma_t	karma;
		unsigned short	pktnum;
		unsigned char	len;
		unsigned char	data[dp_MAXLEN_UNRELIABLE-5];	// 0..len-1 
	 * We will store the current time in the data field.
	*/

	/* Send a ping request to the server.  Put time and adr in payload.
	 * Bypass dpio connection process.
	 */
	pkt.tag = dp_PING_PACKET_ID;
	pkt.body.karma = 0;
	pkt.body.len = sizeof(clock_t) + dp->dpio->myAdrLen + sizeof(sessType);
	pkt.body.pktnum = SERVER_PING_MAGIC_PKTNUM1;
	memset(pkt.body.data, 0, sizeof(pkt.body.data));
	memcpy(pkt.body.data, &dp->now, sizeof(clock_t));

	/* Save address for purpose of matching up with hostname later */
	memcpy(pkt.body.data+4, adrbuf, dp->dpio->myAdrLen);

	/* always store session type last */
	sessbuf = SwapBytes2(sessType);
	memcpy(pkt.body.data+(pkt.body.len-sizeof(sessType)),
			&sessbuf, sizeof(dp_species_t));

	/* Note that we sent a packet but have not received answer yet. */
	RES(server)->i_rtt++;
	if (RES(server)->i_rtt >= dp_SERVER_NPINGS)
		RES(server)->i_rtt = 0;
	RES(server)->rtt_ms[RES(server)->i_rtt] = -2;

	h = dpio_openHdlRaw(dp->dpio, adrbuf);
	if (h == PLAYER_NONE)
		return;

	DPRINT(("dpEnumServers: sending to h:%d, len %d, t:%d\n", h, pkt.body.len, dp->now));
	dpSwapPingPacket(&pkt.body);
	err = dpio_put_unreliable(dp->dpio, &h, 1, &pkt, 
			sizeof(pkt.tag)
			+sizeof(pkt.body.karma)
			+sizeof(pkt.body.len)
			+sizeof(pkt.body.pktnum)
			+sizeof(long)
			+dp->dpio->myAdrLen
			+sizeof(dp_species_t)
			,NULL);
	if (err != dp_RES_OK) {
		DPRINT(("dpEnumSessions: send fails, e:%d\n", err));
		return;
	}
	if ((h != dp->hMaster) && (h != dp->hGameServer))
		dpio_closeHdlRaw(dp->dpio, h);
}

/*--------------------------------------------------------------------------
 Add a host to the array of hosts we're pinging.
 Hostname must not be NULL.
 Returns OK if server new, ALREADY if old.
--------------------------------------------------------------------------*/

static dp_result_t addServer(dp_t *dp, char *adrbuf, int adrlen, char *hostname, size_t hostnamelen)
{
	dp_serverInfo_t server;
	dp_serverInfo_t *pserver;
	size_t serverlen;
	int j;
	dp_result_t err;

	if (hostnamelen > sizeof(server.hostname))
		hostnamelen = sizeof(server.hostname);

	DPRINT(("addServer: ns %d, name %*.*s, adr %02x.%02x.%02x.%02x.%02x.%02x, len %d\n", 
			dptab_tableSize(dp->serverpings), hostnamelen, hostnamelen, hostname, 
			255 & adrbuf[0],
			255 & adrbuf[1],
			255 & adrbuf[2],
			255 & adrbuf[3],
			255 & adrbuf[4],
			255 & adrbuf[5],
			adrlen
			));

	/* Do we already have this server in our list?  If so, ignore new entry */
	err = dptab_get_bykey(dp->serverpings, adrbuf, dp->dpio->myAdrLen,
		(void **)&pserver, &serverlen);
	if (err == dp_RES_OK) {
		DPRINT(("addServer: already have server; ignoring\n"));
		return dp_RES_ALREADY;
	}

	memset(&server, 0, sizeof(server));

	memcpy(server.hostname, hostname, hostnamelen);

	server.rtt_ms_avg = 9999;
	server.loss_percent = 100;
	RES(&server)->i_rtt = 0;
	for (j=0; j<dp_SERVER_NPINGS; j++)
		RES(&server)->rtt_ms[j] = -1;

	dptab_set(dp->dt, dp->serverpings, adrbuf, dp->dpio->myAdrLen,
		&server, sizeof(server), 0, PLAYER_ME);

	assert(adrlen == dp->dpio->myAdrLen);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of server entries.
 Triggers a ping, if we're in the middle of pinging...
----------------------------------------------------------------------*/
int dp_PASCAL dp_servers_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dp_serverInfo_t *server;
	size_t serverlen;
	dp_result_t err;

	DPRINT(("dp_servers_cb: status %d, hostname %*.*s\n", status, total, total, buf));

	/* Ignore anything but new records */
	if (status != dp_RES_CREATED)
		return 0;

	if (addServer(dp, subkey, subkeylen, buf, total) == dp_RES_OK) {
		err = dptab_get_bykey(dp->serverpings, subkey, dp->dpio->myAdrLen, (void **)&server, &serverlen);
		if (err == dp_RES_OK) {
			dp_serverInfo_t serv = *server;
			sendServerPing(dp, &serv, subkey, dp->defaultSessionType);
			dptab_set(dp->dt, dp->serverpings, subkey, dp->dpio->myAdrLen, &serv, sizeof(serv), 1, PLAYER_ME);
		}
	}
	return 0;
}

/*----------------------------------------------------------------------
 Callback for server ping results.
----------------------------------------------------------------------*/
int dp_PASCAL dp_serverpings_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dp_serverInfo_t *server = (dp_serverInfo_t *)buf;
	dp_result_t err;

	DPRINT(("dp_serverpings_cb: status %d, hostname %s\n", status, server->hostname));

	/* Inform caller via local message, if desired */
	if (dp->monitor_object_servers) {
		err = dpSendObjectDelta(dp, status, (dp_object_t *) server, table, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dp_serverpings_cb: can't send object delta, err:%d\n", err));
		}
	}

	return 0;
}

/* an (ip, host) entry in the bootstrap list */
typedef struct {
	char *ip;
	char *hostname;
} myhostname_t;

/* Default bootstrap server list */
static myhostname_t default_bootstrap[] = {
	{"192.215.160.71",  "us1.alink.activision.com"},
	{"192.215.160.72",  "us2.alink.activision.com"},
	{"194.131.8.12",    "uk1.activision.co.uk"},

	/*{"203.35.167.100",  "australia1.activision.com"},*/
	/*{"207.82.192.39",   "california12.activision.com"},*/
	/*{"209.1.100.223", "california4.activision.com"},*/
	/*{"207.82.192.20", "california11.activision.com"},*/
	/*{"207.137.137.137",	"a10.thenet.com"},*/
	/*{"204.119.24.237", 	"i76.polar.net"},*/
	/*{"204.92.55.47",	"i76.ican.net" },*/
	/*{"206.41.128.205",	"ags.ionet.net"},*/
	/*{"207.170.70.21", 	"activision.fc.net"},*/
	/*{"194.198.184.222",	"stockholm.alink.fragzone.se"},*/
	/*{"203.12.160.64",	"activision.tpgi.com.au"},*/
	{NULL,NULL},		/* for overwriting with server name from ini file */
	{NULL,NULL},		/* This one terminates the list. */
};

/*----------------------------------------------------------------------
 Returns the bootstrap list read from bootserv.txt, or the default
 hard-coded list if the file does not exist or contains no hosts.
----------------------------------------------------------------------*/
static myhostname_t *get_bootstrap_list()
{
	FILE *fp;
	int nhosts = 0;
	int nalloc = 0;
	myhostname_t *bootstrap = NULL;

	fp = fopen("bootserv.txt", "r");
	if (fp) {
		char line[128];

		line[127] = '\0';
		while (NULL != fgets(line, 127, fp)) {
			char thisip[128], thishost[128];
			int lenip, lenhost;

			if (2 != sscanf(line, "%s %s", thisip, thishost))
				continue;
			if (nhosts+1 + 2 > nalloc) {
				/* alloc for nhosts+2 slots so list always ends in >= 2
				 * {NULL,NULL} entries for proper termination.
				 */
				if (!bootstrap) {
					bootstrap = (myhostname_t *)malloc(16*sizeof(myhostname_t));
					if (!bootstrap) {
						DPRINT(("get_bootstrap_list: malloc(16 entries) failed\n"));
						break;
					}
					memset(bootstrap, 0, 16*sizeof(myhostname_t));
					nalloc = 16;
				} else {
					myhostname_t *temp = (myhostname_t *)realloc(bootstrap, (nalloc+16)*sizeof(myhostname_t));
					if (!temp) {
						DPRINT(("get_bootstrap_list: realloc(to %d entries) failed\n", nalloc+16));
						break;
					}
					bootstrap = temp;
					memset(bootstrap + nalloc, 0, 16*sizeof(myhostname_t));
					nalloc += 16;
				}
			}
			lenip = strlen(thisip) + 1;
			lenhost = strlen(thishost) + 1;
			bootstrap[nhosts].ip = (char *)malloc(lenip + lenhost);
			if (!bootstrap[nhosts].ip) {
				DPRINT(("get_bootstrap_list: malloc host[%d] failed\n", nhosts));
				break;
			}
			bootstrap[nhosts].hostname = bootstrap[nhosts].ip + lenip;
			strncpy(bootstrap[nhosts].ip, thisip, lenip);
			strncpy(bootstrap[nhosts].hostname, thishost, lenhost);
			assert(bootstrap[nhosts].ip[lenip - 1] == '\0');
			assert(bootstrap[nhosts].hostname[lenhost - 1] == '\0');
			DPRINT(("get_bootstrap_list: read %s %s\n", bootstrap[nhosts].ip,
				bootstrap[nhosts].hostname));
			nhosts++;
		}
		if (!nhosts) {
			/* Return the default if read no hosts from bootserv.txt */
			if (bootstrap)
				free(bootstrap);
			DPRINT(("get_bootstrap_list: no hosts in bootserv.txt, using default list\n"));
			return default_bootstrap;
		}
		DPRINT(("get_bootstrap_list: read %d hosts from bootserv.txt\n", nhosts));
		assert(!bootstrap[nhosts].ip && !bootstrap[nhosts].hostname);
		assert(!bootstrap[nhosts + 1].ip && !bootstrap[nhosts + 1].hostname);
		return bootstrap;
	}
	/* Return the default if there is no bootserv.txt */
	DPRINT(("get_bootstrap_list: no bootserv.txt, using default list\n"));
	return default_bootstrap;
}

/*----------------------------------------------------------------------
 Call this to subscribe to the server list from the game server.
----------------------------------------------------------------------*/
static dp_result_t dp_subscribeServerList(dp_t *dp)
{
	dp_result_t err;
	char key[dptab_KEY_MAXLEN];

	DPRINT(("dp_subscribeServerList: hGameServer h:%x, servertab:%p\n",
				dp->hGameServer, dp->servertab));

	if (dp->hGameServer == PLAYER_NONE) 
		return dp_RES_OK;

	/* If we already have a server table, assume we don't need to update it */
	if (dp->servertab)
		return dp_RES_OK;

	/* Create the table of servers. */
	key[0] = dp_KEY_SERVERS;
	err = dptab_createTable(dp->dt, &dp->servertab, key, 1, 0, NULL,NULL,dp_servers_cb, dp);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribeServerList: Can't create servers table\n"));
		return err;
	}

	err = dptab_addPublisher(dp->dt, dp->servertab, key, 1, dp->hGameServer);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribeServerList: Can't add publisher for servers table\n"));
		return err;
	}

	err = dptab_requestSubscription(dp->dt,key, 1, dp->hGameServer, NULL, NULL);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribeServerList: Can't request subscription to servers table\n"));
		return err;
	}

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Call this to get the latest server list and initialize the server ping
 reply array.
----------------------------------------------------------------------*/
void dp_initEnumServers(dp_t *dp)
{
	int sn;
	dp_serverInfo_t *server;
	char key[dptab_KEY_MAXLEN];
	dp_result_t err;

	/* Create the table of server ping responses. */
	key[0] = dp_KEY_SERVERPINGS;
	if (!dp->serverpings) {
		int i;
		FILE *fp;

		/* While we're here, initialize the timer that controls pings */
		dp->next_serverping = dp->now;

		/* Check to see if it's on disk first. */
		fp = fopen(SERVERS_CACHE_FILE, "rb");
		if (fp) {
			err = dptab_table_thaw(dp->dt, dp->dpio, fp);
			if (err != dp_RES_OK) {
				DPRINT(("dp_initEnumServers: can't restore.  err:%d\n", err));
				if (dptab_getTable(dp->dt, key, 1))
					dptab_deleteTable(dp->dt, key, 1);
			}
			fclose(fp);
		}
		dp->serverpings = dptab_getTable(dp->dt, key, 1);
		if (dp->serverpings) {
			char subkey[dp_KEY_MAXLEN];
			int subkeylen;
			void *buf;
			size_t len;
			
			/* Blow away the table if it doesn't have the correct record size */
			err = dptab_get_byindex(dp->serverpings, 1, &buf, &len, subkey, &subkeylen);
			if ((err != dp_RES_OK) || (len != sizeof(dp_serverInfo_t))) {
				dptab_deleteTable(dp->dt, key, 1);
				dp->serverpings = NULL;
				DPRINT(("dp_initEnumServers: deleted bogus serverpings table, recordlen:%d != %d || err:%d\n", len, sizeof(dp_serverInfo_t), err));
			}
		}
		if (dp->serverpings) {
			/* Clear out the old ping results */
			for (i=dptab_tableSize(dp->serverpings); i-- > 0; ) {
				char subkey[dptab_KEY_MAXLEN];
				int subkeylen;
				size_t serverlen;
				dp_result_t err = dptab_get_byindex(dp->serverpings, i, (void **)&server, &serverlen, subkey, &subkeylen);
				if (err == dp_RES_OK) {
					int j;
					/* not kosher to modify table directly - oh, well. */
					server->rtt_ms_avg = 9999;
					server->loss_percent = 100;
					RES(server)->i_rtt = 0;
					for (j=0; j<dp_SERVER_NPINGS; j++)
						RES(server)->rtt_ms[j] = -1;
					if (dp->monitor_object_servers) {
						/* when thawing table, inform user as if servers were new */
						err = dpSendObjectDelta(dp, dp_RES_CREATED, (dp_object_t *) server, dp->serverpings, subkey, subkeylen);
						if (err != dp_RES_OK) {
							DPRINT(("dp_initEnumServers: can't send object delta, err:%d\n", err));
						}
					}
				}
			}
			err = dptab_setTableCallback(dp->serverpings, dp_serverpings_cb, dp);
			assert((err == dp_RES_OK) || (err == dp_RES_ALREADY));
		}
	}
	/* Create it harder :-) */
	if (!dp->serverpings) {
		myhostname_t *bootstrap;

		err = dptab_createTable(dp->dt, &dp->serverpings, key, 1, sizeof(dp_serverInfo_t), NULL,NULL,NULL,NULL);
		if (err != dp_RES_OK)
			return;
		err = dptab_setTableCallback(dp->serverpings, dp_serverpings_cb, dp);
		assert((err == dp_RES_OK) || (err == dp_RES_ALREADY));

		/* Add in our bootstrap list. */
		bootstrap = get_bootstrap_list();
		for (sn=0; bootstrap[sn].ip; sn++) {
			int adrlen;
			char adrbuf[dp_MAX_ADR_LEN];

			/* Don't use DNS - it's too slow.  We really need to update
			 * this list dynamically!
			 */
			adrlen = dpio_scanAdr(dp->dpio, bootstrap[sn].ip, 
					adrbuf, sizeof(adrbuf));
			if (adrlen == dp->dpio->myAdrLen)
				addServer(dp, adrbuf, adrlen,
					bootstrap[sn].hostname, strlen(bootstrap[sn].hostname));
		}
	}

	DPRINT(("dp_initEnumServers: hGameServer:%x servertab:%p\n", dp->hGameServer, dp->servertab));
	/* If we're connected to a server, ask it to send us the new table. */
	if ((dp->hGameServer != PLAYER_NONE) 
	&&   !dp->servertab) {
		err = dp_subscribeServerList(dp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_initEnumServers: Can't request subscription to servers table, err:%d\n", err));
		}
	}
}

/*--------------------------------------------------------------------------
 Call this at quit time to save the servers list to disk.
--------------------------------------------------------------------------*/
void dp_saveServerList(dp_t *dp)
{
	FILE *fp;

	if (!dp) 
		return;
	if (!dp->serverpings)
		return;

	fp = fopen(SERVERS_CACHE_FILE, "wb");
	if (!fp)
		return;

	dptab_table_freeze(dp->serverpings, dp->dpio, fp);

	fclose(fp);
}

/*--------------------------------------------------------------------------
 Given a pointer to a server record, compute the average delay and loss.
--------------------------------------------------------------------------*/
static void calc_delay(dp_serverInfo_t *server)
{
	int total_rtt;
	int n_replies;
	int n_lost;
	int j;

	n_replies = 0;
	n_lost = 0;
	total_rtt = 0;
	for (j=0; j < dp_SERVER_NPINGS; j++) {
		if (RES(server)->rtt_ms[j] == -2) {
			/* Sent but no answer */
			n_lost++;
		} else if (RES(server)->rtt_ms[j] != -1) {
			/* Got answer */
			n_replies++;
			total_rtt += RES(server)->rtt_ms[j];
		}
	}
	if (n_replies)
		server->rtt_ms_avg = total_rtt / n_replies;
	else
		server->rtt_ms_avg = 9999;
	if (n_replies + n_lost > 0)
		server->loss_percent = n_lost * 100 / (n_replies + n_lost);
	else
		server->loss_percent = 100;
	/*DPRINT(("calc_delay: hostname %s, n_lost %d, n_sent %d, %% %d\n", 
		server->hostname, n_lost, n_lost+n_replies, server->loss_percent));*/
}

/*----------------------------------------------------------------------
 Call this from dpPoll or dpEnumServers when it's time to terminate
 the current dpEnumServers enumeration.
----------------------------------------------------------------------*/
void dp_endEnumServers(
	dp_t *dp)
{
	dpEnumServersExCallback_t cb;
	DPRINT(("dp_endEnumServers\n"));
	cb = dp->enumServersEx_callback;
	if (cb) {
		/* clear callback early to support callbacks that set themselves up again */
		dp->enumServersEx_callback = NULL;
		cb(NULL, 0, NULL, NULL, 0, dp->enumServersEx_context);
	}
}

/*----------------------------------------------------------------------
 Wrapper for dpEnumServers callback.
----------------------------------------------------------------------*/
static int dp_PASCAL serversEx_cb(const char *hostname, long roundtrip_ms,
		dp_serverInfo_t *server, long *pTimeout, long flags, void *context)
{
	dp_t *dp = (dp_t *)context;
	if (!dp) return dp_RES_BAD;
	if (dp->oldenumServers_callback)
		return dp->oldenumServers_callback(hostname, roundtrip_ms, pTimeout,
										flags, dp->oldenumServers_context);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Calls the callback function (cb) once with a description,
 including the server's hostname and delay,
 of each game server that this object could connect to.
 Servers may be reported multiple times.
 Finishes by calling the callback with a null server pointer.

 Since our functions must be non-blocking, returns immediately,
 before any calls to the callback. The callback is called by dpReceive,
 which must be called frequently. 
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumServers(
	dp_t *dp,
	long timeout,				/* How long in milliseconds to wait. */
	dpEnumServersCallback_t cb,
	void *context)
{
	if (!dp) {
		DPRINT(("dpEnumServers: dp is NULL!\n"));
		return dp_RES_BUG;
	}

	/* End enumServers activity if already active. */
	if (dp->enumServersEx_callback)
		dp_endEnumServers(dp);

	dp->oldenumServers_callback = cb;
	dp->oldenumServers_context = context;
	return dpEnumServersEx(dp, timeout, 0, serversEx_cb, dp);
}

/*----------------------------------------------------------------------
 Similar to dpEnumServers() but returns more server information during callback.

 dpEnumServersEx() takes an additional parameter for session type and returns
 additional server information in a dp_serverInfo_t structure during callback.
 This info includes latency, packet loss, number of users of given session
 type, total number of users, and maximum allowed users.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumServersEx(
	dp_t *dp,
	long timeout,				/* How long in milliseconds to wait. */
	dp_species_t sessType,
	dpEnumServersExCallback_t cb,
	void *context)
{
	int i;
	dp_serverInfo_t *server;

	if (!dp) {
		DPRINT(("dpEnumServersEx: dp is NULL!\n"));
		return dp_RES_BUG;
	}

	/* End enumServers activity if already active. */
	if (dp->enumServersEx_callback)
		dp_endEnumServers(dp);

	/* Restore the table of serverpings, and start pinging. */
	dp_initEnumServers(dp);

	/* Set dp->defaultSessionType if not set */
	if (dp->defaultSessionType == 0)
		dp->defaultSessionType = sessType;

	dp->enumServersEx_context = context;
	dp->enumServersEx_callback = cb;
	dp->now = myclock();	/* can't afford for this to be stale here */
	dp->enumServers_deadline = dp->now + (timeout*dp->dpio->clocksPerSec)/1000;

	DPRINT(("dpEnumServersEx: pinging %d hosts\n", dptab_tableSize(dp->serverpings)));
	for (i=dptab_tableSize(dp->serverpings); i-- > 0; ) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		size_t serverlen;
		dp_result_t err = dptab_get_byindex(dp->serverpings, i, (void **)&server, &serverlen, subkey, &subkeylen);
		DPRINT(("dpEnumServersEx: getting host %d, err:%d; adr %02x.%02x.%02x.%02x.%02x.%02x\n", i, err, 
				0xff & subkey[0],
				0xff & subkey[1], 
				0xff & subkey[2],
				0xff & subkey[3],
				0xff & subkey[4], 
				0xff & subkey[5]
				));
		DPRINT(("dpEnumServersEx: subkeylen %d, dp->dpio->myAdrLen %d \n", subkeylen, dp->dpio->myAdrLen)); 
		assert(subkeylen == dp->dpio->myAdrLen);
		if (err == dp_RES_OK) {
			dp_serverInfo_t serv = *server;
			calc_delay(&serv);
			sendServerPing(dp, &serv, subkey, dp->defaultSessionType);
			dptab_set(dp->dt, dp->serverpings, subkey, dp->dpio->myAdrLen, &serv, sizeof(serv), 1, PLAYER_ME);
		}
	}

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Call this periodically from dpPoll().
 If the user has called dpRequestObjectDeltas(...dp_KEY_SERVERPINGS...),
 this routine will ping the servers for him with an interval between pings
 somewhere between 1 and 10 seconds.
 If no response is received from any server, decreases the period.
 otherwise, increases the period.  
----------------------------------------------------------------------*/
dp_result_t dpEnumServersPoll(dp_t *dp)
{
	/* DPRINT(("dpEnumServersPoll: entered; monitor_os %d, next_sp %d, now %d\n", dp->monitor_object_servers, dp->next_serverping, dp->now)); */
	if (!dp->monitor_object_servers)
		return dp_RES_OK;

	if ((long)(dp->next_serverping - dp->now) > 0)
		return dp_RES_OK;
	dp->next_serverping = dp->now + dp->serverping_interval;

	if (dp->serverping_rx_count_old == dp->serverping_rx_count)
		dp->serverping_interval /= 2;
	else
		dp->serverping_interval *= 2;

	if (dp->serverping_interval < dp->dpio->clocksPerSec)
		dp->serverping_interval = dp->dpio->clocksPerSec;
	else if (dp->serverping_interval > (20 * dp->dpio->clocksPerSec))
		dp->serverping_interval = 20 * dp->dpio->clocksPerSec;

	dp->serverping_rx_count_old = dp->serverping_rx_count;
	return dpEnumServersEx(dp, 0, dp->monitor_object_servers_sessType, NULL, NULL);
}

/*----------------------------------------------------------------------
 Handle a server ping response.
 A server ping response is like a user ping response, but has the
 high bit of the packet number set.
 Returns dp_RES_EMPTY if not a server ping response.
----------------------------------------------------------------------*/
dp_result_t dpHandleServerPingResponsePacket(
	dp_t *dp,
	unsigned char *buf,
	size_t len,
	playerHdl_t src)
{
	dp_serverInfo_t *server;
	dp_serverInfo_t serv;
	size_t serverlen;

	struct dpHSPRP_pkt_s {
		dp_packetType_t tag;
		dp_ping_packet_t body;
	} PACK *pkt = (struct dpHSPRP_pkt_s *)buf;
	clock_t tStart;
	unsigned char adrbuf[dp_MAX_ADR_LEN];
	dp_result_t err;

	/* Size of block of data in ping packet body */
	const size_t timeAndAdrlen = sizeof(clock_t) + dp->dpio->myAdrLen;
	short *ps;	/* points to array of shorts at end of ping packet */

	DPRINT(("dpHandleServerPingResponsePacket: karma %d, pktnum %d\n", pkt->body.karma, pkt->body.pktnum));
	if (!(pkt->body.pktnum & SERVER_PING_MAGIC_PKTNUM1)) {
		/* It's not my server ping */
		return dp_RES_EMPTY;
	}
	dp->serverping_rx_count++;

	if (!dp->serverpings)
		return dp_RES_BUG;

	/* Get a pointer to the server record */
	memcpy(adrbuf, pkt->body.data+sizeof(clock_t), dp->dpio->myAdrLen);
	err = dptab_get_bykey(dp->serverpings, adrbuf, dp->dpio->myAdrLen, (void **)&server, &serverlen);
	if (err != dp_RES_OK) {
		/* Odd - we didn't ping them... */
		DPRINT(("dpHandleServerPingResponsePacket: Can't find server adr %u.%u.%u.%u\n", adrbuf[0], adrbuf[1], adrbuf[2], adrbuf[3]));
		return dp_RES_EMPTY;
	}
	serv = *server;

	/* Figure out the round-trip time delay and store it. */
	memcpy(&tStart, pkt->body.data, sizeof(clock_t));
	RES(&serv)->rtt_ms[RES(&serv)->i_rtt] = (short) (((dp->now - tStart)*1000L)/dp->dpio->clocksPerSec);

	ps = (short *)(pkt->body.data + timeAndAdrlen);
	serv.sessType = SwapBytes2(*ps); ps++;
	if (pkt->body.len >= timeAndAdrlen + 4*sizeof(short)) {
		serv.cur_users = SwapBytes2(*ps); ps++;
		serv.max_users = SwapBytes2(*ps); ps++;
		serv.cur_sessTypeUsers = SwapBytes2(*ps); ps++;
		if (pkt->body.len >= timeAndAdrlen + 6*sizeof(short)) {
			serv.cur_games = SwapBytes2(*ps); ps++;
			serv.cur_sessTypeGames = SwapBytes2(*ps); ps++;
		} else {
			serv.cur_games = 0;
			serv.cur_sessTypeGames = 0;
		}
	} else {   /* got ping from old server; don't print junk fields */
		serv.cur_users = 0;
		serv.max_users = 0;
		serv.cur_sessTypeUsers = 0;
		serv.cur_games = 0;
		serv.cur_sessTypeGames = 0;
	}
	DPRINT(("sessType %d, cur_users %d, max_users %d, sessTypeUsers %d, cur_games %d, sessTypeGames %d\n",
			serv.sessType, serv.cur_users, serv.max_users,
			serv.cur_sessTypeUsers, serv.cur_games, serv.cur_sessTypeGames));

	/* Figure out the average round-trip delay and the packet loss rate. */
	calc_delay(&serv);

	/* Save result of update back in table.  Note that entry is in
	 * local byte order, even in table.
	 * This may trigger a callback which informs the user via local message.
	 */
	dptab_set(dp->dt, dp->serverpings, adrbuf, dp->dpio->myAdrLen, &serv, sizeof(serv), 1, PLAYER_ME);


	DPRINT(("dpHandleServerPingResponsePacket: host %s, rtt %d, loss %d%%; sent at T:%d, now t:%d\n", 
			serv.hostname, serv.rtt_ms_avg, serv.loss_percent, tStart, dp->now));

	/* Inform caller via callback, if desired */
	if (dp->enumServersEx_callback) {
		long timeout = 1;
		dp->enumServersEx_callback(serv.hostname, (long)serv.rtt_ms_avg,
								&serv, &timeout, 0, dp->enumServersEx_context);
	}
	return dp_RES_OK;
}
