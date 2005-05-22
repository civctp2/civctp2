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
 Self-test for the score reporting modules.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#ifndef UNIX
/* The self test will compile under Windows, but will just
 * return an error message when run.
 */
#define dpscoret_CLIENT_ONLY
#endif

#include "dp2.h"
#include "dptab.h"
#include "contract.h"
#include "dpprivy.h"
#ifndef dpscoret_CLIENT_ONLY
#include "servscor.h"
#endif
#include "tserv.h"
#include "mywcs.h"
#include "raw.h"
#include "eclock.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* globals for use by main, echo_server/echo_client */
int n_connects = 0;

/* globals for use by main, ProcessCommandLine */
char commDLLName[256];
int server;
char servernames[4][256];
int n_servers = 0;
commInitReq_t commInitReq; /*  Parameters needed by some comm drivers. */
char phonenum[256];
char modeministr[256];

char user1[tcapw_LEN_USERNAME+1];  /* username and pass to use in speed test */
char pass1[tcapw_LEN_PW+1];

/* Global for simulated email */
wchar_t secretcode[tcapw_LEN_PW];

/* Globals for detecting callbacks and other events */
int waiting_for_update = FALSE;
int got_callback = FALSE;
char sessname[255];
dpid_t id;

#define dpscoret_SESSID "dpstsess"
#define dpscoret_SESSIDLEN  8
#define dpscoret_SESSTYPE  65  /* use chat, since it actually has rooms */

#ifdef _DEBUG
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 256; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}
#endif

#ifdef _DEBUG
/* Convert a key to ASCII for debug printing */
char *key2buf(const char *key, int keylen, char *buf)
{
	int i;

	if (keylen > dp_KEY_MAXLEN)
		return "key too long";

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		/* DPRINT(("key[%d] %d\n", i, key[i])); */
		sprintf(buf + strlen(buf), "%d%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
	}
	return buf;
}

/* Convert a key to ASCII for debug printing */
char key2a_buf[256];
char key2a_buf2[256];

/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)
#endif

/* Globals related to simulated network. */
#ifdef SIMNET
#undef dpscoret_DPOPEN

#define SIMNET_H_SERVER 0
#define SIMNET_H_CLIENT 1

/* There are two addresses: 0 and 1.  All packets are from the other address. */
struct {
	int len;
	char buf[256];
} simnetbuf[2], simlocalbuf[2];	 /* receive buffer & local buffer */
int sim_my_rx = 0;		/* offset I receive into */

dpioOpenHdlCallback_t simnet_openhdlcb = NULL;
void *simnet_openhdlcontext = NULL;

/* Get a packet, if there is one */
dp_result_t dpio_get(
	dpio_t   *dpio,
	playerHdl_t *psrc,
	void *buffer,
	size_t *psize,
	int *flags)
{
	(void) dpio; 
	if (simnetbuf[sim_my_rx].len) {
		*psrc = 1 - sim_my_rx;
		memcpy(buffer, simnetbuf[sim_my_rx].buf, simnetbuf[sim_my_rx].len);
		*psize = simnetbuf[sim_my_rx].len;
		simnetbuf[sim_my_rx].len = 0;
		printf("sim dpio_get: %d got packet of size %d from %d: %s\n", sim_my_rx, *psize, *psrc, hexstring((unsigned char *)buffer, *psize));		
		
		if (flags)
			*flags = dpio_GET_RELIABLE;
		return dp_RES_OK;
	}
	if (simlocalbuf[sim_my_rx].len) {
		*psrc = PLAYER_ME;
		memcpy(buffer, simlocalbuf[sim_my_rx].buf, simlocalbuf[sim_my_rx].len);
		*psize = simlocalbuf[sim_my_rx].len;
		simlocalbuf[sim_my_rx].len = 0;
		printf("sim dpio_get: %d got local packet of size %d: %s\n", sim_my_rx, *psize, hexstring((unsigned char *)buffer, *psize));

		if (flags)
			*flags = dpio_GET_RELIABLE;
		return dp_RES_OK;
	}
	return dp_RES_EMPTY;
}

int retry_fraction = 0;

/* Send a packet */
/* Returns dp_RES_FULL retry_fraction percent of the time to test the packet
 * retry system if retry_fraction is set.
 */
dp_result_t dpio_put_reliable2(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest, /* If error occurs, dest with error indicated here */
	int flags)
{
	static retry_sum = 0;

	DPRINT(("sim dpio_put: h:%d sending packet of size %d to h:%x\n", sim_my_rx, size, (int)(*dests)));
	if ((int)(*dests) == (1 - sim_my_rx)) {
		if (simnetbuf[1 - sim_my_rx].len) {
			printf("sim dpio_put: output buffer full\n");
			exit(1);
		}
		retry_sum += retry_fraction;	
		if (retry_sum >= 100) {
			retry_sum -= 100;
			printf("sim dpio_put: full queue simulated\n");
			return dp_RES_FULL;
		}
		memcpy(simnetbuf[1 - sim_my_rx].buf, buffer, size);
		simnetbuf[1 - sim_my_rx].len = size;
		return dp_RES_OK;
	} else if (*dests == PLAYER_ME || (int)(*dests) == sim_my_rx) {
		if (simlocalbuf[sim_my_rx].len) {
			printf("sim dpio_put: local buffer full\n");
			exit(1);
		}
		memcpy(simlocalbuf[sim_my_rx].buf, buffer, size);
		simlocalbuf[sim_my_rx].len = size;
		return dp_RES_OK;
	} else {
		printf("sim dpio_put: bad dest\n");
		exit(1);
	}
}

dp_result_t dpio_put_unreliable(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest) /* If error occurs, dest with error indicated here */
{
	return dpio_put_reliable(dpio, dests, nDests, buffer, size, errDest);
}

playerHdl_t dpio_openHdl2(dpio_t *dpio, void *adr, void *adr2) 
{
	/* Should return *adr, but let's be paranoid */
	if (sim_my_rx != SIMNET_H_CLIENT) {
		printf("simnet_openHdl2: server called?\n");
		exit(1);
	}
	if (*(char *)adr != SIMNET_H_SERVER) {
		printf("simnet_openHdl2: adr %d not server's?\n", *(char *)adr);
		exit(1);
	}
	DPRINT(("simnet_openHdl2: client opened handle to server\n"));
	/* Call callback only if client */
	if ((sim_my_rx == SIMNET_H_CLIENT) && simnet_openhdlcb) {
		/* Must swap sim_my_rx since transferring control to other task! */
		sim_my_rx = 1 - sim_my_rx;
		DPRINT(("dpio_openHdl: calling callback h:%d\n", SIMNET_H_CLIENT));
		simnet_openhdlcb(SIMNET_H_CLIENT, 1, dp_RES_OPENED, simnet_openhdlcontext);
		DPRINT(("dpio_openHdl: callback done\n"));
		sim_my_rx = 1 - sim_my_rx;
	}

	return SIMNET_H_SERVER;
}

short dpio_getHdlRemoteCapabilities(dpio_t *dpio, playerHdl_t h)
{
	return 0x01;
}
int dpio_getHdlTxPktsFree(dpio_t *dpio, playerHdl_t h)
{
	if (((int)h == (1 - sim_my_rx) && simnetbuf[1 - sim_my_rx].len) ||
		(((int)h == sim_my_rx || h==PLAYER_ME) && simlocalbuf[sim_my_rx].len))
		return 0;
	else
		return 4;
	/* dptab tries to leave 3 free for other subsystems -
	 * if we report fewer than 4, it will never send.
	 */
}

/* needed to avoid link errors.  Only used by the functions
 * dptab_table_freeze and thaw, which are not used by this test program.
 */
dp_result_t dpio_freezeHdl(dpio_t* dpio, playerHdl_t hdl, FILE* file)
{
	return dp_RES_UNIMPLEMENTED;  /* an easy to notice error */
}
dp_result_t dpio_thawHdl(dpio_t* dpio, playerHdl_t* hdl, FILE* file)
{
	return dp_RES_UNIMPLEMENTED;  /* an easy to notice error */
}

dp_result_t dpio_create(dpio_t **pdpio, const dp_transport_t *transportDLLname,
	commInitReq_t *commInitReq, const clock_t *now, FILE *thawfp)
{
	static dpio_t dummy;
	
	memset(&dummy, 0, sizeof(dpio_t));
	dummy.now = now;  /* needed by dptab */
	*pdpio = &dummy;
	return dp_RES_OK;
}

#define dpio_destroy(dpio, flags) 

dp_result_t dpio_printAdr(dpio_t *dpio, const char *adrbuf, size_t adrlen, char *buf, size_t buflen)
{
	sprintf(buf, "%d", *adrbuf);
	return dp_RES_OK;
}
int dpio_scanAdr(dpio_t *dpio, char *hostname, char *adrbuf, size_t buflen)
{
	int adr;
	sscanf(hostname, "%d", &adr);
	*adrbuf = adr;
	DPRINT(("simnet_scanAdr(%s) -> %d\n", hostname, adr));
	return 1;
}
dp_result_t dpio_hdl2adr(dpio_t *dpio, playerHdl_t h, void *adr, int *len)
{
	if (h != PLAYER_ME) {
		printf("dpio_hdl2adr: bad\n");
		exit(1);
	}
	((char *)adr)[0] = sim_my_rx;
	*len = 1;
	return dp_RES_OK;
}

#define dpio_update(dpio) dp_RES_OK
#define dpio_closeHdlImmed(dpio, h)
#define dpio_findTimedOutHost(dpio) PLAYER_NONE

dp_result_t dpio_setIncomingCallback(dpio_t *dpio, dpioOpenHdlCallback_t cb, void *context)
{
	simnet_openhdlcb = cb;
	simnet_openhdlcontext = context;
	return dp_RES_OK;
}

dp_result_t dpio_closeHdl(dpio_t *dpio, playerHdl_t h)
{
	/* Call callback only if client */
	if ((sim_my_rx == SIMNET_H_CLIENT) && simnet_openhdlcb) {
		/* Must swap sim_my_rx since transferring control to other task! */
		sim_my_rx = 1 - sim_my_rx;
		DPRINT(("dpio_closeHdl: calling callback on h:%d\n", h));
		simnet_openhdlcb(SIMNET_H_CLIENT, 0, dp_RES_CLOSED, simnet_openhdlcontext);
		DPRINT(("dpio_closeHdl: callback done\n"));
		sim_my_rx = 1 - sim_my_rx;
	}
	return dp_RES_OK;
}

dp_result_t dpio_hdl2adr2(dpio_t *dpio, playerHdl_t h, void *adr, void *adr2, int *len)
{
	DPRINT(("simnet dpio_hdl2adr2: not implemented\n"));
	return dp_RES_BUG;
}

#define dpReadyToFreeze(dp) dp_RES_OK

dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt) 
{
	printf("dpReportAssertionFailure(%d, %s, %s)\n",
			lineno, file, linetxt);
	exit(1);
	return dp_RES_OK;
}

int cdecl
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
{
#include <stdarg.h>
	va_list argptr = NULL;
	int len = 0;

	if (__format) {
		va_start(argptr, __format);
		len = vprintf(__format, argptr);
		va_end(argptr);
	}

	return len;
}

/* Fake dp & session management functions.  Uses a hardcoded session list. */

DP_API dp_result_t dpCreate(
	dp_t **pdp,
	dp_transport_t *transport,
	commInitReq_t *params,
	char *thawFilename)
{
    dp_t *dp;
	dp_result_t		dpio_err;

	precondition(pdp != NULL);
#ifndef SIMNET
	precondition(transport != NULL);
#endif

	DPRINT(("dpCreate: enter(%p,", pdp));
	if(transport == NULL) {DPRINT(("NULL,"));} else {DPRINT(("%s,", transport));}
	if(params == NULL) {DPRINT(("NULL,"));} else {DPRINT(("%p,", params));}

	/* Allocate and clear the object */
	dp = (dp_t *)malloc(sizeof(dp_t));
	*pdp = NULL;
	if (!dp)
		return dp_RES_NOMEM;
	memset(dp, 0, sizeof(dp_t));

	dp->hMaster = PLAYER_NONE;
	dp->hGameServer = PLAYER_NONE;
	dp->ping_dest = PLAYER_NONE;
	
	DPRINT(("dpCreate: Creating new dp from scratch\n"));

	/* Initialize the transport layer.
	 * ifdef SIMNET, fake dpio functions will handle this trivially.
	 */
	dp->now = eclock();
	dpio_err = dpio_create(&(dp->dpio), transport, params, &dp->now, NULL);
	if ((dpio_err != comm_STATUS_OK_AM_MASTER) && (dpio_err != dp_RES_OK)) {
		DPRINT(("dpCreate: dpio_create fails, err:%d\n", dpio_err));
		return dpio_err;
	}
	/* reset dp->now in case dpio_create took a long time */
	dp->now = eclock();
	/* Bogus kludge: initialize external addr to same as internal addr. */
	memcpy(dp->my_outside_adr, dp->dpio->myAdr, dp->dpio->myAdrLen);

    DPRINT(("dpCreate: dp->now:%d *(dp->dpio->now):%d\n", dp->now, *(dp->dpio->now)));
	/* Initialize the table transfer layer. */
    dp->dt = dptab_create(dp->dpio);
    if (!dp->dt)
		return dp_RES_NOMEM;

	/* Set up hardcoded session parameters */
	dp->defaultSessionType = dpscoret_SESSTYPE;
	dp->sess_subkeylen = dpscoret_SESSIDLEN;
	memcpy(dp->sess_subkey, dpscoret_SESSID, dpscoret_SESSIDLEN);
	
	/* Success. */
	*pdp = dp;
	if (dpio_err == comm_STATUS_OK_AM_MASTER)
		return comm_STATUS_OK_AM_MASTER;
	return dp_RES_OK;
}

dp_result_t dpDestroy(dp_t *dp, int flags)
{
	precondition(dp);
	DPRINT(("dpDestroy: entering, flags:%d\n", flags));

	if (dp->tserv) {
		tserv_destroy(dp->tserv);
		dp->tserv = NULL;
		tca_destroy(dp->tca);
		dp->tca = NULL;
	}

	dptab_destroy(dp->dt);
	dpio_destroy(dp->dpio, flags);
	memset(dp, 0, sizeof(dp_t));
	free(dp);

	DPRINT(("dpDestroy: returning OK\n"));
	return dp_RES_OK;
}

DP_API dp_result_t DP_APIX dpReceive(
	dp_t   *dp,
	dpid_t *idFrom,
	dpid_t *idTo,
	int    flags,
	void *buffer,
	size_t *size)
{
	dp_result_t err;
	int get_flags;
	playerHdl_t src;
	char *pktbuf = (char *)buffer;
	tserv_event_t result;
	
	err = dpio_get(dp->dpio, &src, pktbuf, size, &get_flags);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	*idTo = (dpid_t)PLAYER_ME;
	*idFrom = (dpid_t)src; 
	if (err != dp_RES_OK) {
		playerHdl_t h;

		if (((long)(dp->now - dp->next_beacon)) > 0) {
			/* poll to propagate any dptab table changes twice a second */
			dp->next_beacon = dp->now + ECLOCKS_PER_SEC/2;
			DPRINT(("sim dpReceive: calling dptab_update t:%d next_t:%d\n", dp->now, dp->next_beacon));
			err = dptab_update(dp->dt);
			if (err != dp_RES_OK && err != dp_RES_EMPTY) {
				DPRINT(("sim dpReceive: dptab_update returns err:%d\n", err));
				exit(1);
			}
			waiting_for_update = FALSE;
		}
		if (dp->hGameServer == PLAYER_ME &&
			((long)(dp->now - dp->next_beacon2)) > 0) {
			/* server polls for complete score reports every 3 seconds or so */
			dp->next_beacon2 = dp->now + ECLOCKS_PER_SEC * 3;
			DPRINT(("sim dpReceive: calling servscor_poll t:%d next_t:%d\n", dp->now, dp->next_beacon2));
#ifndef dpscoret_CLIENT_ONLY
			servscor_poll(dp);
#endif
		}
		if (dp->tserv)
			err = tserv_poll(dp->tserv);
		if (err == dp_RES_HOST_NOT_RESPONDING) {
			printf("sim dpReceive: Host reply timed out!\n");
			exit(1);
		}
		err = dpio_update(dp->dpio);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		h = dpio_findTimedOutHost(dp->dpio);
		if (h != PLAYER_NONE)
			dpio_closeHdlImmed(dp->dpio, h);
	} else {
		DPRINT(("sim dpReceive: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], *size));
		/* Process the packet, possibly transforming it into a tserv_event_t */
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_TSERV_PACKET_ID:
			if (!dp->tserv) {
				printf("sim dpReceive: got tserv packet, but no tserv\n");
				exit(1);
			}
			err = tserv_handle_packet(dp->tserv, *idFrom, get_flags, (tserv_packet_t *)(pktbuf+2), &result);
			assert(err == dp_RES_OK || err == dp_RES_EMPTY);
			if (err == dp_RES_OK) {				
				dp_account_packet_t *pkt = (dp_account_packet_t *)(pktbuf + sizeof(dp_packetType_t));
				*((dp_packetType_t *)pktbuf) = dp_ACCOUNT_PACKET_ID;
				*size = sizeof(dp_packetType_t) + sizeof(dp_account_packet_t);
				pkt->reason = result.reason;
				pkt->uid = result.uid;
				printf("sim dpReceive: event h:%x, uid:%d, reason:%d\n",
						result.h, result.uid, result.reason);
				return dp_RES_OK;  /* got a packet */
			}
			break;

		case dptab_SUBSCRIBE_PACKET_ID:
			/* make sure packet was sent reliably */
			if ((get_flags & dpio_GET_RELIABLE) == 0)
				DPRINT(("sim dpReceive: got reliable packet not sent reliably\n"));
			DPRINT(("sim dpReceive: Handing off subscribe packet from h:%x to dptab\n", *idFrom));
			err = dptab_handleSubscriptionRequest(dp->dt, *idFrom, (dptab_subscribe_packet_t *)(pktbuf + sizeof(dp_packetType_t)), *size - sizeof(dp_packetType_t));
			if (err != dp_RES_OK) {
				DPRINT(("sim dpReceive: dptab_handleSubscriptionRequest returns err:%d\n", err));
				exit(1);
			}
			dptab_assertValid(dp->dt);
			break;

		case dptab_DELETE_PACKET_ID:
		case dptab_INITIAL_PACKET_ID:
		case dptab_BODY_PACKET_ID:
		case dptab_SMALL_PACKET_ID:
			/* if internet, check that all dptab packets were sent reliably */
			if ((get_flags & dpio_GET_RELIABLE) == 0 &&
				(dp->dpio->driverinfo->capabilities & comm_DRIVER_NO_BROADCAST)) {
				DPRINT(("sim dpReceive: got inet dptab packet not sent reliably\n"));
				exit(1);
			}
			DPRINT(("sim dpReceive: Handing off packet from h:%x to dptab\n", *idFrom));
			DPRINT(("sim dpReceive: pkt:%s\n", hexstring(pktbuf, *size)));
			err = dptab_handlePacket(dp->dt, *idFrom, *size, pktbuf);
			if (err != dp_RES_OK) {
				DPRINT(("sim dpReceive: dptab_handlePacket returns error err:%d\n", err));
				exit(1);
			}
			break;

		default:
			return dp_RES_OK;  /* got a packet to pass out */
			break;
		}
	}
	return dp_RES_EMPTY;
}

DP_API dp_result_t DP_APIX dpSetGameServerEx(dp_t *dp, char *masterHostName, dp_species_t sessionType)
{
	dp_result_t err;
	char serveraddr[dp_MAX_ADR_LEN];
	int addrlen;

	if (masterHostName) {  /* We are connecting */
		dp->tca = tca_create();
		assert(dp->tca);
		dp->tserv = tserv_create(dp->dpio, dp->tca);
		if (!dp->tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		
		addrlen = dpio_scanAdr(dp->dpio, (char *)masterHostName, serveraddr, dp_MAX_ADR_LEN);
		if (addrlen == 0) {
			printf("sim dpSetGameServerEx: dpio_scanAdr(%s) failed\n", masterHostName);
			exit(1);
		}
		dp->hGameServer = dpio_openHdl2(dp->dpio, serveraddr, NULL);
		if (dp->hGameServer == PLAYER_NONE) {
			printf("sim dpSetGameServerEx: dpio_openHdl2 failed\n");
			exit(1);
		}
		tserv_server_add(dp->tserv, dp->hGameServer);
		err = dptab_addPeer(dp->dt, dp->hGameServer);
		assert(!err);
		err = dpscore_client_init(dp);
		assert(!err);
	} else {  /* masterHostName == NULL, we are cleaning up */
		err = dpio_closeHdl(dp->dpio, dp->hGameServer);
		if (err != dp_RES_OK) {
			printf("sim dpSetGameServerEx: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		tserv_destroy(dp->tserv);
		dp->tserv = NULL;
		err = dptab_deletePeer(dp->dt, dp->hGameServer);
		assert(!err);
		dp->hGameServer = PLAYER_NONE;
		err = dpscore_client_cleanup(dp);
		assert(!err);
	}
	return dp_RES_OK;
}

dp_result_t dpAccountLogin(dp_t *dp, const char *username, const char *password)
{
	return tserv_account_loginA(dp->tserv, username, password);
}

dp_uid_t dpGetPlayerUid(dp_t *dp, dpid_t id)
{
	/* This is only called by client in dpscore.c,
	 * so just cheat and access the innards of the client's tserv.
	 */
	if (!dp || !dp->tserv)
		return dp_UID_NONE;
	if (!dp->tserv->activated)  /* only return uid if we really could get it */
		return dp_UID_NONE;
	return dp->tserv->userinfo.uid;
}

dp_result_t dp_uid2sessid(dp_t *dp, dp_uid_t uid, char *sessid, int *sessidlen, dp_species_t *sessType)
{
	if (PLAYER_NONE != tserv_uid2hdl(dp->tserv, uid)) {
		/* If the uid is logged in, it's in this session. */
		*sessidlen = dpscoret_SESSIDLEN;
		*sessType = dpscoret_SESSTYPE;
		memcpy(sessid, dpscoret_SESSID, *sessidlen);
		return dp_RES_OK;
	}
	return dp_RES_EMPTY;
}

dp_result_t dpSendObjectDelta(
	dp_t *dp,
	dp_result_t status,
	dp_object_t *data,
	dptab_table_t *tab,
	char *subkey,
	int subkeylen)
{
	playerHdl_t dest;
	size_t pktlen;
	size_t datalen;
	dp_result_t err;
	struct {
		dp_packetType_t   tag PACK;
 		dp_objectDelta_packet_t body PACK;
	} pkt;

	precondition(dp);
	precondition(data);
	precondition(tab);
	precondition(subkeylen);

	memset(&pkt, 0, sizeof(pkt));

	DPRINT(("dpSendObjectDeltaPacket: status e:%d, ", status));
	switch(tab->key[0]) {
	case dp_KEY_SCORES:
		datalen = sizeof(data->score.nScoreTypes) +
			data->score.nScoreTypes * ( sizeof(data->score.scoreIds[0]) +
										sizeof(data->score.scores[0]) );
		DPRINT(("uid %d", dpMAKELONG(subkey[0], subkey[1], subkey[2], subkey[3])));
		break;
	default:
		return dp_RES_BAD;
	}
	DPRINT(("\n"));

	pkt.tag = dp_OBJECTDELTA_PACKET_ID;
	pkt.body.status = status;
	pkt.body.reserved = 0;
	pkt.body.keylen = tab->keylen;
	memcpy(pkt.body.key, tab->key, tab->keylen);
	pkt.body.subkeylen = subkeylen;
	memcpy(pkt.body.subkey, subkey, subkeylen);
	memcpy(&pkt.body.data, data, datalen);

	dest = PLAYER_ME;
	pktlen = sizeof(pkt) - sizeof(pkt.body.data) + datalen;

	err = dpio_put_reliable(dp->dpio, &dest, 1, &pkt, pktlen, NULL);
	return err;
}

assoctab_t *getSessTypes(dp_t *dp)
{
	assoctab_t *types;		/* indexed by session type; no payload */

	assert(dp != NULL);
	types = assoctab_create(0);
	if (!types) {
		DPRINT(("getSessTypes: error: can't alloc types\n"));
		return NULL;
	}
	/* create a trivial sesstypes table */
	if (assoctab_subscript_grow(types, dpscoret_SESSTYPE) == NULL) {
		DPRINT(("getSessTypes: error: can't alloc type:%d\n", dpscoret_SESSTYPE));
		return NULL;
	}
	return types;
}

dpid_t dpGetMyId(dp_t *dp)
{
	/* fake dpids from uids, since we have no real sessions */
	if (!dp || !dp->tserv)
		return dp_ID_NONE;
	if (dp->tserv->userinfo.uid == dp_UID_NONE)
		return dp_ID_NONE;
	return ((dpid_t)dp->tserv->userinfo.uid);
}
/* End of fake dp & session management functions */

/* End of SIMNET functions*/
#endif

/*----------------------------------------------------------------------------
  Processes the command line arguments.  Some arguments are handled
  directly by this routine.  Others simply result in an update to
  the global switches data structure.
----------------------------------------------------------------------------*/
void ProcessCommandLine(int argc, char **argv)
{
	char *chptr;
	int   i;

	servernames[n_servers][0] = '\0';
	commDLLName[0] = '\0';
	
	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
			/* deal with args that start with - or / */
			if (argv[i][1] == 0) {
				printf("Expected letter after -\n");
				exit(1);
			}
			if ((chptr = strchr(argv[i], '=')) == NULL &&
				(chptr = strchr(argv[i], ':')) == NULL) {
				printf("Expected '=' or ':' after -%c\n", argv[i][1]);
				exit(1);
			}
			switch(toupper(argv[i][1])) {
			case 'D':
				/*  Turn on some sort of debugging mode */
				break;				
			case 'I':   /*  Set modem init string */
				strncpy(modeministr, chptr+1, sizeof(modeministr));
				modeministr[sizeof(modeministr)-1] = '\0';
				break;
			case 'K':	/*  Comm parameters; Kport, Kirq, Kpulse */
				if (stricmp(argv[i], "-Kpulse") == 0) {
					/*  No need to set default case  */
					/*  TOUCH_TONE == 0 */
					/*  PULSE == 1 */
					commInitReq.dialing_method = 1;
					break;
				}
				if (stricmp(argv[i], "-Ktest") == 0) {
					/* Do loopback test. Other side must have -Ktest, too. */
					commInitReq.flags |= comm_INIT_FLAGS_TEST;
					break;
				}
				*chptr = 0;
				if (stricmp(argv[i], "-Kport") == 0) {
					sscanf(chptr+1, "%lx", &commInitReq.baseadr);
				} else if (stricmp(argv[i], "-Kirq") == 0) {
					sscanf(chptr+1, "%ld", &commInitReq.hwirq);
				} else {
					*chptr = '=';
					printf("unknown parameter %s", argv[i]);
					exit(1);
				}
				break;
			case 'N':   /*  Communications DLL name */
				strncpy(commDLLName, chptr+1, sizeof(commDLLName));
				commDLLName[sizeof(commDLLName)-1] = '\0';
				break;
			case 'P':
				server = FALSE;
				if (n_servers >= 4) {
					printf("Maximum number of supported servers is 4, ignoring %s\n", chptr);
				} else {
					strncpy(servernames[n_servers], chptr+1, 256);
					servernames[n_servers][255] = '\0';
					printf("Read servername %d: %s\n", n_servers, servernames[n_servers]);
					n_servers++;
				}
				break;
			case 'S':   /*  Set session id */
				commInitReq.sessionId = atoi(chptr+1);
				break;
			case 'U':
				strncpy(user1, chptr+1, tcapw_LEN_USERNAME);
				user1[tcapw_LEN_USERNAME] = '\0';
				break;
			case 'V':
				strncpy(pass1, chptr+1, tcapw_LEN_PW);
				pass1[tcapw_LEN_PW] = '\0';
				break;
			case 'W':   /*  Set phone number */
				strncpy(phonenum, chptr+1, sizeof(phonenum));
				phonenum[sizeof(phonenum)-1] = '\0';
				break;
			case 'Y':   /*  Set comm port */
				sscanf(chptr+1, "%ld", &commInitReq.portnum);
				break;
			case 'Z':   /*  Set baud rate */
				sscanf(chptr+1, "%ld", &commInitReq.baud);
				break;
			default:
				printf("\
Usage: %s -N=comm.dll [-P=serveraddr][-S=sessionId]\n\
    [-U=username][-V=password]\n\
    [-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n\
    [-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n\
Unknown arg %s\n", argv[0], argv[i]);
				printf("\
A client server login and authentication self test for the tserv module.\n\
If -P=serveraddr is specified, act as a client to a server at serveraddr.\n\
If not, act as a server and print out the network address for clients to\n\
connect to.\n");
				exit(1);
			} /*  end switch */
		}
	}
}

#ifndef dpscoret_CLIENT_ONLY
/*----------------------------------------------------------------------------
 Callback (ick) called by dpio when a handle is opened or closed.
 err is either dp_RES_OPENED, dp_RES_HOST_NOT_RESPONDING, or dp_RES_CLOSED.
----------------------------------------------------------------------------*/
void server_openhdl_cb(playerHdl_t hdl, int n_hdls, dp_result_t err, void *context)
{
	dp_t *dp = context;
	if (!context) return;

	DPRINT(("server_openhdl_cb: h:%x, n:%d, err:%d, context:%p\n",
			hdl, n_hdls, err, context));
	if (err == dp_RES_OPENED) {
		dp_result_t res;
		res = tserv_client_add(dp->tserv, hdl);
		if (res != dp_RES_OK)
			DPRINT(("server_openhdl_cb: tserv_client_add returns err:%d\n", res));
		servscor_addClient(dp, hdl);
		DPRINT(("server_openhdl_cb: Added client h:%x, reason %d\n", hdl, err));
	} else {
		dp_result_t res;
		char key[dptab_KEY_MAXLEN];
		key[0] = dp_KEY_MYSCORES;
		res = tserv_client_delete(dp->tserv, hdl);
		if (res != dp_RES_OK)
			DPRINT(("server_openhdl_cb: tserv_client_delete returns err:%d\n", res));
	    res = dptab_deletePublisher(dp->dt, key, 1, hdl);
		if (res != dp_RES_OK)
			DPRINT(("server_openhdl_cb: dptab_deletePublisher returns err:%d\n", res));
		res = dptab_deletePeer(dp->dt, hdl);
		if (res != dp_RES_OK)
			DPRINT(("server_openhdl_cb: dptab_deletePeer returns err:%d\n", res));
		DPRINT(("server_openhdl_cb: Deleted client h:%x, reason %d\n", hdl, err));
	}
}
#endif

/*----------------------------------------------------------------------------
 Callback (ick) called by dpio when a handle is opened or closed.
 err is either dp_RES_OPENED, dp_RES_HOST_NOT_RESPONDING, or dp_RES_CLOSED.
----------------------------------------------------------------------------*/
void client_openhdl_cb(playerHdl_t hdl, int n_hdls, dp_result_t err, void *context)
{
	dp_result_t dptab_err;
	dp_t *dp = context;
	if (!context) return;
	
	if (err == dp_RES_OPENED) {
		n_connects++;
	} else if (err == dp_RES_CLOSED) {
		n_connects--;
	}
	DPRINT(("client_openhdl_cb: h:%x, n:%d, err:%d, n_connects=%d, dterr:%d\n",
			hdl, n_hdls, err, n_connects, dptab_err));
}

#ifdef dpscoret_DPOPEN
/*-------------------------------------------------------------------------
 Callback triggered by dpOpen when joining a session.
-------------------------------------------------------------------------*/
int dp_PASCAL dpscoret_join_sess_cb(dp_session_t *ps, long *pTimeout, long flags, void *context) {
	if (ps) {
		DPRINT(("join_sess_cb: Joined session %s.\n", ps->sessionName));
		printf("join_sess_cb: Joined session %s.\n", ps->sessionName);
	} else {
		if (context) {
			DPRINT(("join_sess_cb: Failed to join session %s.\n", context));
			printf("join_sess_cb: Failed to join session %s.\n", context);
		} else {
			DPRINT(("join_sess_cb: Failed to join any session.\n"));
			printf("join_sess_cb: Failed to join any session.\n");
		}
		exit(1);
	}
	got_callback = TRUE;

	(void) context;
	return FALSE;
}

/*-------------------------------------------------------------------------
 Callback triggered by creating a player.
-------------------------------------------------------------------------*/
static void dp_PASCAL dpscoret_create_player_cb(dpid_t id, char_t *name, long flags, void *context)
{
	dpid_t *pMy_id = (dpid_t *)context;

	if (id != dp_ID_NONE) {
		printf("create_player_cb: Created player id %d, name %s\n", id, name);
		DPRINT(("create_player_cb: Created player id %d, name %s\n", id, name));
	} else {
		printf("create_player_cb: Failed to create player\n");
		DPRINT(("create_player_cb: Failed to create player\n"));
		exit(1);
	}
	if (pMy_id) *pMy_id = id;
	got_callback = TRUE;

	(void) context;
	(void) flags;
}
#endif

#ifndef dpscoret_CLIENT_ONLY
/*----------------------------------------------------------------------------
 The server calls this periodically to get and process packets.
----------------------------------------------------------------------------*/
void server_poll(dp_t *myDP, int init)
{
	dp_result_t err;
	dpid_t src, dest;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;
	int res;
	tserv_event_t event;

	myDP->now = eclock();
	
	assert(myDP);
	if (init) {
		printf("Server initializing...\n");
		/* Open the password database */
		myDP->tca = tca_create();
		assert(myDP->tca);
		res = tca_openpw(myDP->tca, "pw.dat");
		assert(res != dp_RES_EMPTY);
		myDP->tserv = tserv_create(myDP->dpio, myDP->tca);
		if (!myDP->tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
#ifdef SIMNET
		myDP->hGameServer = PLAYER_ME;
		err = dpio_setIncomingCallback(myDP->dpio, server_openhdl_cb, myDP);
		assert(!err);
#endif
		myDP->next_beacon = myDP->now;
		myDP->next_beacon2 = myDP->now;
		servscor_init(myDP);
		printf(" done.\n");
		return;
	}

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpReceive(myDP, &src, &dest, 0, pktbuf, &pktlen);
	if (err == dp_RES_EMPTY) {
	} else if (err != dp_RES_OK) {
		DPRINT(("SERVER: dpReceive returned err:%d\n", err));
	} else {
		/* printf("SERVER: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen); */
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_ACCOUNT_PACKET_ID:
			{
				dp_account_packet_t *pkt = (dp_account_packet_t *)(pktbuf + sizeof(dp_packetType_t));
				printf("SERVER: event h:%x, uid:%d, reason:%d\n",
					   event.h, event.uid, event.reason);
				event.h = PLAYER_ME;
				event.reason = pkt->reason;
				event.uid = pkt->uid;
				if (event.reason == dp_RES_HELP ||
					event.reason == dp_RES_CREATED) {
					tcapw_entry_t entry;
					dp_netchar_t ncs[tcapw_LEN_PW];
					
					/* An email request just came in, pull the secret code out
					 * of the database and "email" it.
					 */
					err = tserv_uid2info(myDP->tserv, event.uid, &entry);
					assert(!err);
					err = tcapw_get_secretcode(&entry, ncs);
					assert(!err);
					mywcs_netchar2wchar(secretcode, ncs, tcapw_LEN_PW);
					DPRINT(("SERVER: 'emailing' secretcode %s (%d) to %s\n", tcapw_u2ascii(ncs, tcapw_LEN_PW), entry.secretcode, entry.email));
					DPRINT(("SERVER: secretcode[0] = %d\n", secretcode[0]));
#ifndef SIMNET
					fprintf(stderr, "SERVER: And The Secret Code is... %d\n", entry.secretcode);
#endif
				}
			}
		    break;

		default:
			break;
		}
	}
}
#endif /* dpscoret_CLIENT_ONLY */

/*----------------------------------------------------------------------------
 The client calls this periodically; it implements his internal state machine,
 which goes thru the normal login sequence.
 Returns TRUE when test complete.
----------------------------------------------------------------------------*/
int client_poll(dp_t *myDP, int init)
{
	dp_result_t err;
	dpid_t src, dest;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;
	int got_result = FALSE;
	int got_delta = FALSE;
	tserv_event_t result;
	dp_objectDelta_packet_t *delta;
	int done = FALSE;
	char *servername = servernames[0];

	static int client_state = 0;
	static dp_uid_t uid = dp_UID_NONE;
	static clock_t deadline;
	static long oldscoreVal[6];
	static int got_scoreval[6];
	static int got_oldmax[2];
	
	myDP->now = eclock();
	
	if (init) {
		client_state = 0;
		secretcode[0] = 0;
#ifdef SIMNET
		myDP->next_beacon = myDP->now;
		err = dpio_setIncomingCallback(myDP->dpio, client_openhdl_cb, myDP);
		assert(!err);
#endif
		id = dp_ID_NONE;
		return FALSE;
	}

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpReceive(myDP, &src, &dest, 0, pktbuf, &pktlen);
	if (err == dp_RES_EMPTY) {
	} else if (err != dp_RES_OK) {
		DPRINT(("CLIENT: dpReceive returned err:%d\n", err));
		return done;
	} else {
		/* printf("CLIENT: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen); */
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_ACCOUNT_PACKET_ID:
			{
				dp_account_packet_t *pkt = (dp_account_packet_t *)(pktbuf + sizeof(dp_packetType_t));
				got_result = TRUE;
				result.h = PLAYER_ME;
				result.reason = pkt->reason;
				result.uid = pkt->uid;
			}
			break;

		case dp_OBJECTDELTA_PACKET_ID:
			delta = (dp_objectDelta_packet_t *)(pktbuf + sizeof(dp_packetType_t));
			if (delta->keylen == 3 && delta->key[0] == dp_KEY_SCORES &&
				dpMAKESHORT(delta->key[1],delta->key[2])==dpscoret_SESSTYPE) {
				got_delta = TRUE;
				DPRINT(("CLIENT: got scores object delta packet\n"));
			} else {
				DPRINT(("CLIENT: got object delta packet key:%s\n", key2a(delta->key, delta->keylen)));
			}
			break;

		default:
			break;
		}
	}

	switch (client_state) {
	case 0:
		client_state = 100;
		printf("CLIENT: going to state %d\n", client_state);
		break;

	case 100:  /* Connect */
		DPRINT(("CLIENT: Connecting... (%d)\n", client_state));
		assert(n_connects == 0);
		err = dpSetGameServerEx(myDP, servername, dpscoret_SESSTYPE);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 110:  /* Send username and password */
		DPRINT(("CLIENT: Setting username and password (%d)\n", client_state));
		err = dpAccountLogin(myDP, "user1", "pass1");
		assert(!err);
		client_state += 10;
		printf("client: going to state %d\n", client_state);			
		break;

	case 120:  /* (tserv) Wait for user1 authorization */
		if (got_result) {
			DPRINT(("CLIENT: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("CLIENT: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("CLIENT: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}	
			uid = result.uid;
			assert(uid != dp_UID_NONE);
#ifdef SIMNET
			id = dpGetMyId(myDP);
			assert(id != dp_ID_NONE);
			printf("CLIENT: logged in, id %d\n", id);
#endif
			printf("CLIENT: logged in, uid %d\n", uid);
			/* tell DP we are the local player */
			myDP->firstId = (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1));
			client_state += 10;
			printf("CLIENT: going to state %d\n", client_state);
		}
		break;

	case 130:  /* Open a session */
#ifdef dpscoret_DPOPEN
		got_callback = FALSE;
		{
			dp_session_t sess;

			DPRINT(("CLIENT: Creating a session (%d)\n", client_state));
			sess.flags = dp_SESSION_FLAGS_CREATESESSION;
			sess.sessionType = dpscoret_SESSTYPE;
			sess.maxPlayers = 2;
			strcpy(sess.sessionName, "dpstsess");
			strcpy(sessname, "dpstsess");
			err = dpOpen(myDP, &sess, dpscoret_join_sess_cb, sessname);
			assert(!err);
		}
#else
		got_callback = TRUE;
#endif
 	    client_state += 10;
	    printf("CLIENT: going to state %d\n", client_state);
	    break;

	case 140:  /* (dp) wait for session join callback, then create a player */
		if (!got_callback)
			break;
#ifdef dpscoret_DPOPEN
		DPRINT(("CLIENT: Joined session, now creating player (%d)\n", client_state));
		got_callback = FALSE;
		err = dpCreatePlayer(myDP, dpscoret_create_player_cb, &id, "user1");
		assert(!err);
#else
		got_callback = TRUE;
#endif
		client_state += 10;
		printf("CLIENT: going to state %d\n", client_state);
		break;

	case 150:  /* (dp) wait for create player callback */ 
		if (!got_callback)
			break;
#ifdef dpscoret_DPOPEN
		DPRINT(("CLIENT: Created player (%d)\n", client_state));
#endif
		client_state += 10;
		printf("CLIENT: going to state %d\n", client_state);
		break;

	case 160:  /* (dpscore) Subscribe to scores table */
		DPRINT(("CLIENT: Subscribing to scores (%d)\n", client_state));
#ifdef SIMNET
		err = dpscore_client_subscribe(myDP, dpscoret_SESSTYPE);
		assert(!err);
#else
		{
			char key[dp_KEY_MAXLEN];
			key[0] = dp_KEY_SCORES;
			key[1] = dpGETSHORT_FIRSTBYTE(dpscoret_SESSTYPE);
			key[2] = dpGETSHORT_SECONDBYTE(dpscoret_SESSTYPE);
			err = dpRequestObjectDeltas(myDP, TRUE, key, 1);
			assert(!err);
		}
#endif
		memset(oldscoreVal, 0, sizeof(long) * 6);
		memset(got_oldmax, 0, sizeof(int) * 2);
		deadline = myDP->now + ECLOCKS_PER_SEC * 2;
		client_state += 10;
		printf("CLIENT: going to state %d\n", client_state);
		break;

	case 170:  /* (dpscore) Wait for deltas.  Won't get any if server
				* hasn't seen scores from our sesstype before */
		if ((long)(myDP->now - deadline) >= 0) {
			DPRINT(("CLIENT: Received no new deltas by deadline, leaving.\n"));
			client_state += 10;
			printf("CLIENT: going to state %d\n", client_state);
		}
		if (got_delta) {
			scorerep_player_t player;
			dp_scoreInfo_t *psi;
			scorerep_buf_t repbuf;
			dp_uid_t delta_uid;
			int i;

			/* delta packet:
			 *  flags:  dp_OBJECTDELTA_FLAG_LOCAL/INOPENSESS/ISHOST;
			 *  status: dp_RES_CREATED/CHANGED/DELETED;
			 *  keylen;
			 *  key[dp_KEY_MAXLEN]:    dp_KEY_SCORES;
			 *  subkeylen;
			 *  subkey[dp_KEY_MAXLEN]: uid;
			 *  data:   scorerep_buf_t(scorerep_player_t);
			 */
			delta_uid = dpMAKELONG(delta->subkey[0], delta->subkey[1], delta->subkey[2], delta->subkey[3]);
			DPRINT(("CLIENT: got delta, flags:%d status:%d key:%s subkey:%s (uid:%d)\n", delta->flags, delta->status, key2a(delta->key, delta->keylen), key2a2(delta->subkey, delta->subkeylen), delta_uid));
			assert(uid != dp_UID_NONE);
			psi = &(delta->data.score);
			repbuf.len = sizeof(psi->nScoreTypes) + psi->nScoreTypes *
				(sizeof(psi->scoreIds[0]) + sizeof(psi->scores[0]));
			memcpy(repbuf.buf, psi, repbuf.len);
			DPRINT(("CLIENT: got delta, data: %s ...\n", hexstring(repbuf.buf, repbuf.len)));
			err = scorerep_player_fromBuf(&player, &repbuf);
			assert(!err);
			DPRINT(("CLIENT: got delta, scores:"));
			assert(player.scores);
			for (i = 0; i < player.scores->n_used; i++) {
				int scoreId;
				long scoreVal;
				assoctab_item_t *pe = assoctab_getkey(player.scores, i);
				assert(pe);
				scoreId = pe->key & ~(dp_SCOREID_MAX | dp_SCOREID_SUM);
				scoreVal = *((long *)pe->value);
				DPRINT((" %s%s%d->%d", ((pe->key & dp_SCOREID_MAX) ? "MAX" : ""), ((pe->key & dp_SCOREID_SUM) ? "SUM" : ""), scoreId, scoreVal));
				if (delta_uid == uid && scoreId >= 1 && scoreId < 3) {
					if (pe->key & dp_SCOREID_MAX) {
						oldscoreVal[scoreId-1 + 4] = scoreVal;
						got_oldmax[scoreId-1] = TRUE;
					} else if (pe->key & dp_SCOREID_SUM) {
						oldscoreVal[scoreId-1 + 2] = scoreVal;
					} else {
						oldscoreVal[scoreId-1 + 2] = scoreVal;
					}
				}
			}
			DPRINT(("\n"));
			deadline = myDP->now + ECLOCKS_PER_SEC * 2;
		}
		break;

	case 180:  /* (dpscore) report a few scores */
		DPRINT(("CLIENT: Reporting scores (%d)\n", client_state));
		err = dpReportScoreStart(myDP, 0);
		assert(!err);
		{
			int scoreId;
			long scoreVal;
			/* Send the previous max - 10 and + 10 */ 
			for (scoreId = 1; scoreId < 3; scoreId++) {
				scoreVal = oldscoreVal[scoreId-1 + 4] + (long)scoreId * 20 - 30;
				err = dpReportScore2(myDP, id, scoreId, scoreVal);
				assert(!err);
			}
		}
		err = dpReportScoreEnd(myDP);
		assert(!err);
#ifdef SIMNET
  		err = dpscore_client_playerLeaving(myDP, id);
		assert(!err);
#else
		err = dpDestroyPlayer(myDP, id);
		                      /* This will trigger client_playerLeaving
							   * via the dp_players_cb callback?
							   */
		assert(!err);
#endif
		waiting_for_update = TRUE;
		client_state += 10;
		printf("CLIENT: going to state %d\n", client_state);
		break;

	case 190:  /* Wait for an update so the scores get sent */
#ifndef SIMNET
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY)
			waiting_for_update = FALSE;
#endif
		memset(got_scoreval, 0, sizeof(int) * 6);
		/* May have to wait up to 61 secs for a servscor_poll, dptab_update */
		deadline = myDP->now + ECLOCKS_PER_SEC * 70;
		client_state += 10;
		printf("CLIENT: going to state %d\n", client_state);
		break;

	case 200:  /* (dpscore) Wait for deltas */
		if ((long)(myDP->now - deadline) >= 0) {
			int i;
			DPRINT(("CLIENT: Received no new deltas by deadline, leaving.\n"));
			DPRINT(("CLIENT: got_scorevals :"));
			for (i = 0; i < 6; i++) {
				assert(got_scoreval[i] == TRUE);
				DPRINT((" %d", i));
			}
			DPRINT(("\n"));
			client_state = 296;
			printf("CLIENT: going to state %d\n", client_state);
		}
		if (got_delta) {
			scorerep_player_t player;
			dp_scoreInfo_t *psi;
			scorerep_buf_t repbuf;
			dp_uid_t delta_uid;
			int i;
			
			/* delta packet:
			 *  flags:  dp_OBJECTDELTA_FLAG_LOCAL/INOPENSESS/ISHOST;
			 *  status: dp_RES_CREATED/CHANGED/DELETED;
			 *  keylen:                3
			 *  key[dp_KEY_MAXLEN]:    dp_KEY_SCORES, sessType;
			 *  subkeylen:             4  
			 *  subkey[dp_KEY_MAXLEN]: uid;
			 *  data:   scorerep_buf_t(scorerep_player_t);
			 */			
			delta_uid = dpMAKELONG(delta->subkey[0], delta->subkey[1], delta->subkey[2], delta->subkey[3]);
			DPRINT(("CLIENT: got delta, flags:%d status:%d key:%s subkey:%s (uid:%d)\n", delta->flags, delta->status, key2a(delta->key, delta->keylen), key2a2(delta->subkey, delta->subkeylen), delta_uid));
			assert(uid != dp_UID_NONE);
			psi = &(delta->data.score);
			repbuf.len = sizeof(psi->nScoreTypes) + psi->nScoreTypes *
				(sizeof(psi->scoreIds[0]) + sizeof(psi->scores[0]));
			memcpy(repbuf.buf, psi, repbuf.len);
			DPRINT(("CLIENT: got delta, data: %s ...\n", hexstring(repbuf.buf, repbuf.len)));
			err = scorerep_player_fromBuf(&player, &repbuf);
			assert(!err);
			DPRINT(("CLIENT: got delta, scores:"));
			assert(player.scores);
			for (i = 0; i < player.scores->n_used; i++) {
				int scoreId;
				long scoreVal;
				assoctab_item_t *pe = assoctab_getkey(player.scores, i);
				assert(pe);
				scoreId = pe->key & ~(dp_SCOREID_MAX | dp_SCOREID_SUM);
				scoreVal = *((long *)pe->value);
				DPRINT((" %s%s%d:", ((pe->key & dp_SCOREID_MAX) ? "MAX" : ""), ((pe->key & dp_SCOREID_SUM) ? "SUM" : ""), scoreId));
				if (delta_uid == uid && scoreId >= 1 && scoreId < 3) {
					long expectedVal;
					if (pe->key & dp_SCOREID_MAX) {
						/* expect max(oldmax, oldmax +/- 10);
						 * if oldmax is undefined, submitted value
						 * (+/- 10) becomes the new max.
						 */
						expectedVal = oldscoreVal[scoreId-1 + 4];
						if (scoreId == 2)
							expectedVal = oldscoreVal[scoreId-1 + 4] + 10;
						if (!got_oldmax[scoreId-1])
							expectedVal = scoreId * 20 - 30;
						DPRINT(("%d->%d(=?%d) ", oldscoreVal[scoreId-1 + 4], scoreVal, expectedVal));
						assert(scoreVal == expectedVal);
						got_scoreval[scoreId-1 + 4] = TRUE;
					} else if (pe->key & dp_SCOREID_SUM) {
						/* expect sum(oldsum, oldmax +/- 10);
						 * oldsum, oldmax = 0 if undefined.
						 */
						expectedVal = oldscoreVal[scoreId-1 + 2] + oldscoreVal[scoreId-1 + 4] + (long)scoreId*20 - 30;
						DPRINT(("%d->%d(=?%d) ", oldscoreVal[scoreId-1 + 2], scoreVal, expectedVal));
						assert(scoreVal == expectedVal);
						got_scoreval[scoreId-1 + 2] = TRUE;
					} else {
						/* expect oldmax +/- 10; (oldmax = 0 if undefined) */
						expectedVal = oldscoreVal[scoreId-1 + 4] + (long)scoreId*20 - 30;
						DPRINT(("%d->%d(=?%d) ", oldscoreVal[scoreId-1], scoreVal, expectedVal));
						assert(scoreVal == expectedVal);
						got_scoreval[scoreId-1] = TRUE;
					}
				} else
					DPRINT(("->%d ", scoreVal));
			}
			DPRINT(("\n"));
			/* Any further deltas for this poll should come shortly */
			deadline = myDP->now + ECLOCKS_PER_SEC * 2;
		}
		break;

	case 296:  /* Ready to Close? */
#ifdef dpscoret_DPOPEN
		DPRINT(("client: Closing session (%d)\n", client_state));
		err = dpClose(myDP);
		if (err != dp_RES_OK) {
			printf("client: tried to close session, but got err:%d\n", err);
			exit(1);
		}
#endif
		client_state++;
		printf("CLIENT: going to state %d\n", client_state);		
		break;

	case 297:  /* Wait for network traffic to finish */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
#ifdef dpscoret_DPOPEN
			DPRINT(("CLIENT: Session closed (%d)\n", client_state));
#endif
			client_state++;
			printf("CLIENT: going to state %d\n", client_state);
		}
		break;

	case 298:  /* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		uid = dp_UID_NONE;
		id = dp_ID_NONE;
		client_state++;
		printf("client: going to state %d\n", client_state);
		break;

	case 299:  /* Wait for disconnect, then clean up. */
		if (n_connects == 0 && dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			DPRINT(("CLIENT: Disconnected (%d)\n", client_state));
			client_state++;
			printf("CLIENT: going to state %d\n", client_state);
		}
		break;

	case 300:
		client_state = 1000;
		break;

	case 1000:		/* done */
		done = TRUE;
		break;
	}

	return done;
}

#ifndef dpscoret_CLIENT_ONLY
void create_db()
{
	tcapw_t *tcapw = NULL;

	tcapw_entry_t entry1;
	tcapw_pw_t    pw1;
	tcapw_hpw_t   hpw1;
	tcapw_uname_t uname1;
	tcapw_uid_t uid1;
	wchar_t wcbuf[256];
	
	dp_result_t res;

	memset(&entry1, 0, sizeof(entry1));

	/* Create a password database, add a user to it,
	 * make sure you can look him up various ways,
	 * change his password,
	 * save it to disk, restore it from disk.
	 */

	/* Create the database */
	tcapw = tcapw_create();
	assert(tcapw);
	remove("pw.dat");
	res = tcapw_open(tcapw, "pw.dat");
	assert(res == dp_RES_EMPTY);

	/* Add an activated user to it */
	mywcs_frommbs(wcbuf, tcapw_LEN_USERNAME, "user1");
	mywcs_wchar2netchar(uname1.uname, wcbuf, tcapw_LEN_USERNAME);
	mywcs_frommbs(wcbuf, tcapw_LEN_PW, "pass1");
	mywcs_wchar2netchar(pw1.pw, wcbuf, tcapw_LEN_PW);
	res = tcapw_password_hash(tcapw, &pw1, &hpw1);
	assert(!res);
	res = tcapw_entry_create(tcapw, &uname1, &hpw1, tcapw_entry_FLAGS_EMAIL_VALID, "jscanlin@activision.com", &uid1);
	printf("res = %d\n", res);
	assert(!res);

	res = tcapw_close(tcapw);
	assert(!res);
	tcapw_destroy(tcapw);
}
#endif /* not dpscoret_CLIENT_ONLY */

int dpscore_test()
{
	dp_result_t err;
	dp_transport_t transport;
	dp_t *clientDP;
	dp_t *serverDP;

#ifdef SIMNET
	DPRINT(("\n\n*** Starting test with retry_fraction %d%% ***\n", retry_fraction));
#else
	char kbbuf[80];
	char *c;

	c = kbbuf;
#endif

	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, commDLLName);
	
#ifndef SIMNET
	if (!server)
#endif
	{
		DPRINT(("dpscoret: initializing client\n"));
		err = dpCreate(&clientDP, &transport, &commInitReq, NULL);
		if (err == dp_RES_OK) {
			printf("dpCreate for client succeeded.\n");
		} else {
			printf("dpCreate for client returns fatal error: %d\n", err);
			exit(1);
		}

		client_poll(clientDP, 1);  /* initialize */
	}
#ifndef SIMNET
	else
#endif
	{
#ifdef dpscoret_CLIENT_ONLY
		DPRINT(("dpscoret: Server only runs under UNIX!\n"));
		exit(1);
#else
		char serveraddr[dp_MAX_ADR_LEN];
		int addrlen = dp_MAX_ADR_LEN;
		char printable[50];

		DPRINT(("dpscoret: initializing server\n"));
		err = dpCreate(&serverDP, &transport, &commInitReq, NULL);
		if (err == dp_RES_OK) {
			printf("dpCreate for server succeeded.\n");
		} else {
			printf("dpCreate for server returns fatal error: %d\n", err);
			exit(1);
		}

		/* print my address if I am server, so clients can connect */
		err = dpio_hdl2adr(serverDP->dpio, PLAYER_ME, (void *)serveraddr, &addrlen);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		if (dpio_printAdr(serverDP->dpio, serveraddr, addrlen, printable, 50)) {
			printf("I am server, unable to print my address\n");
			exit(1);
		}
		printf("I am server, my address is %s\n", printable);
		create_db();
		server_poll(serverDP, 1);  /* initialize */
#endif
	}
	
	raw_init();
	while (1) {
		int charFromUser = 0;
		if (raw_kbhit()) {
			charFromUser = raw_getc();
			if (charFromUser == 27 /* Esc */) {
				printf("Escape hit.  Terminating.\n");
				return -1;
			}
		}
#ifdef SIMNET
		sim_my_rx = 0;
#ifndef dpscoret_CLIENT_ONLY
		server_poll(serverDP, 0);
#endif
		sim_my_rx = 1;
		if (client_poll(clientDP, 0))
			break;
#else
		if (server) {
#ifndef dpscoret_CLIENT_ONLY
			server_poll(serverDP, 0);
#endif
		} else {
			if (client_poll(clientDP, 0))
			break;
		}
#endif
	}
	raw_end();
	
	printf("dpDestroy unloading comm drivers...");
#ifndef SIMNET
	if (!server)
		dpDestroy(clientDP, 0);
#ifndef dpscoret_CLIENT_ONLY
	else
		dpDestroy(serverDP, 0);
#endif
#else
	dpDestroy(clientDP, 0);
	dpDestroy(serverDP, 0);
#endif
	printf(" done.\n");

    return 0;
}

int main(int argc, char *argv[])
{
#ifndef SIMNET
	if (argc < 2) {
		printf("\
Usage: %s -N=comm.dll [-P=serveraddr][-S=sessionId]\n\
    [-U=username][-V=password]\n\
    [-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n\
    [-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n", argv[0]);
		printf("\
A client server login and authentication self test for the tserv module.\n\
If -P=serveraddr is specified, act as a client to a server at serveraddr.\n\
If not, act as a server and print out the network address for clients to\n\
connect to.\n");
		exit(0);
	}

	modeministr[0] = 0;
	phonenum[0] = 0;
	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.modeministr = modeministr;
	commInitReq.phonenum = phonenum;
	commInitReq.baud = 19200;
	commInitReq.hwirq = 0;
	commInitReq.portnum = 1;
	commInitReq.sessionId = eclock();
	commInitReq.reqLen = sizeof(commInitReq_t);
	server = TRUE;  /* I am server unless -P is specified */
	strcpy(user1, "user1");
	strcpy(pass1, "pass1");
	commDLLName[0] = 0;
	
	ProcessCommandLine(argc, argv);

	if (!commDLLName[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}
	if (server == FALSE && (!servernames[0][0] || n_servers == 0)) {
		printf("I am a client but no servers were specified.\n");
		exit(1);
	}
#else
#ifdef dpscoret_CLIENT_ONLY
    printf("dpscoret: Simnet test only runs under UNIX!\n");
	exit(1);
#endif
	n_servers = 1;
	strcpy(servernames[0], "0");
	/* clear the simnet buffers just in case there's random junk in them */
	simnetbuf[0].len = 0;
	simnetbuf[1].len = 0;
	simlocalbuf[0].len = 0;
	simlocalbuf[1].len = 0;
#endif

#ifdef SIMNET
	retry_fraction = 0;
	if (dpscore_test()) exit(1);
 /* retry_fraction = 45;
  * if (dpscore_test()) exit(1);
  */
#else
	if (dpscore_test()) exit(1);
#endif

	printf("dpscore_test: Test Succeeded.\n");
	return(0);
}
