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
 Trivial test for the tserv module.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "dp2.h"
#include "raw.h"
#include "eclock.h"
#include "tserv.h"
#include "mywcs.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* globals for use by main, echo_server/echo_client */
dpio_t *dpio = NULL;
clock_t	now;
int n_connects = 0;

/* globals for use by main, ProcessCommandLine */
char commDLLName[256];
int server;
char servernames[4][256];
int n_servers = 0;
commInitReq_t commInitReq; /*  Parameters needed by some comm drivers. */
char phonenum[256];
char modeministr[256];

int disconnect_wait;  /* time to wait to disconnect after dpReadyToFreeze */ 
long total_logins = 1000;  /* number of logins in speed test.  0=infinite */
char user1[tcapw_LEN_USERNAME+1];  /* username and pass to use in speed test */
char pass1[tcapw_LEN_PW+1];

/* Global for simulated email */
wchar_t secretcode[tcapw_LEN_PW];

/* Globals related to simulated network. */
#ifdef SIMNET

#define SIMNET_H_SERVER 0
#define SIMNET_H_CLIENT 1

/* There are two addresses: 0 and 1.  All packets are from the other address. */
struct {
	int len;
	char buf[256];
} simnetbuf[2];			/* receive buffer */
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
		size_t i;
		
		*psrc = 1 - sim_my_rx;
		memcpy(buffer, simnetbuf[sim_my_rx].buf, simnetbuf[sim_my_rx].len);
		*psize = simnetbuf[sim_my_rx].len;
		simnetbuf[sim_my_rx].len = 0;
		printf("sim dpio_get: %d got packet of size %d from %d:", sim_my_rx, *psize, *psrc);
		for (i = 0; i < *psize; i++)
			printf(" %02x", ((unsigned char *)buffer)[i]);
		printf("\n");
		
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
	playerHdl_t *errDest,  /* If error occurs, dest with error indicated here */
	int flags)
{
	static retry_sum = 0;
	
	DPRINT(("sim dpio_put: h:%d sending packet of size %d to h:%x\n", sim_my_rx, size, (int)(*dests))); 
	if ((int)(*dests) != (1 - sim_my_rx)) {
		printf("dpio_put_reliable: bad dest\n");
		exit(1);
	}
	if (simnetbuf[1 - sim_my_rx].len) {
		printf("dpio_put_reliable: output buffer full\n");
		exit(1);
	}
	retry_sum += retry_fraction;	
	if (retry_sum >= 100) {
		retry_sum -= 100;
		printf("dpio_put_reliable: full queue simulated\n");
		return dp_RES_FULL;
	}
	memcpy(simnetbuf[1 - sim_my_rx].buf, buffer, size);
	simnetbuf[1 - sim_my_rx].len = size;
	return dp_RES_OK;
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

dp_result_t dpio_create(dpio_t **pdpio, const dp_transport_t *transportDLLname,
	commInitReq_t *commInitReq, const clock_t *now, FILE *thawfp)
{
	static dpio_t dummy;
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

#define dpio_ReadyToFreeze(dpio, h) dp_RES_OK

dp_result_t dpio_freezeHdl(dpio_t* dpio, playerHdl_t hdl, FILE* file)
{
	(dpio_t *)dpio;  (playerHdl_t)hdl;  (FILE *)file;
	return dp_RES_OK;
}

dp_result_t dpio_thawHdl(dpio_t* dpio, playerHdl_t* hdl, FILE* file)
{
	(dpio_t *)dpio;  (playerHdl_t *)hdl;  (FILE *)file;
	return dp_RES_OK;
}

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
			case 'L':   /*  Set total number of logins in speed test */
				total_logins = atol(chptr+1);
				if (total_logins < 0) total_logins = 0;
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
			case 'T':   /*  Set wait time at disconnect */
				disconnect_wait = atoi(chptr+1);
				if (disconnect_wait < 0) disconnect_wait = 0;
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
Usage: %s -N=comm.dll [-P=serveraddr][-S=sessionId][-T=disconnect_wait]\n\
    [-L=number_of_logins(in speed test)][-U=username][-V=password]\n\
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


/*----------------------------------------------------------------------------
 Callback (ick) called by dpio when a handle is opened or closed.
 err is either dp_RES_OPENED, dp_RES_HOST_NOT_RESPONDING, or dp_RES_CLOSED.
----------------------------------------------------------------------------*/
void server_openhdl_cb(playerHdl_t hdl, int n_hdls, dp_result_t err, void *context)
{
	tserv_t *tserv = context;
	if (!context) return;

	DPRINT(("server_openhdl_cb: h:%x, n:%d, err:%d, context:%p\n",
			hdl, n_hdls, err, context));
	if (err == dp_RES_OPENED)
		tserv_client_add(tserv, hdl);
	else
		tserv_client_delete(tserv, hdl);
}

/*----------------------------------------------------------------------------
 Callback (ick) called by dpio when a handle is opened or closed.
 err is either dp_RES_OPENED, dp_RES_HOST_NOT_RESPONDING, or dp_RES_CLOSED.
----------------------------------------------------------------------------*/
void client_openhdl_cb(playerHdl_t hdl, int n_hdls, dp_result_t err, void *context)
{
	if (err == dp_RES_OPENED)
		n_connects++;
	else if (err == dp_RES_CLOSED)
		n_connects--;
	DPRINT(("client_openhdl_cb: h:%x, n:%d, err:%d, n_connects=%d\n",
			hdl, n_hdls, err, n_connects));
}

#ifndef SIMNET
/*----------------------------------------------------------------------------
 Freeze, shutdown comm, restart, and thaw to test the freeze/thaw system.
----------------------------------------------------------------------------*/
dp_result_t server_restart(tserv_t **tserv)
{
	dp_result_t err;
	FILE *fp;
	tca_t *tca;
	int nClients = (*tserv)->clients->n_used;

	assert(nClients < 1000);
		
	DPRINT(("server_restart: freezing server (%d clients)\n", nClients));
	if ((fp = fopen("freeze.dat", "wb")) == NULL) {
		DPRINT(("server_restart: can't write freeze.dat\n"));
		return dp_RES_BUG;
	}
	err = dpio_freeze(dpio, fp);
	if (err != dp_RES_OK) {
		DPRINT(("server_restart: dpio_freeze returns err:%d\n", err));
		return err;
	}
	err = tserv_Freeze(*tserv, fp);
	if (err != dp_RES_OK) {
		DPRINT(("server_restart: tserv_Freeze returns err:%d\n", err));
		return err;
	}
	fclose(fp);

	dpio_destroy(dpio, 0);
	tca_destroy((*tserv)->tca);
	tserv_destroy(*tserv);

	DPRINT(("server_restart: thawing server\n"));
	if ((fp = fopen("freeze.dat", "rb")) == NULL) {
		DPRINT(("server_restart: can't read freeze.dat\n"));
		return dp_RES_BUG;
	}

	now = eclock();
    err = dpio_create(&dpio, NULL, &commInitReq, &now, fp);
	if (err != dp_RES_OK) {
		DPRINT(("server_restart: dpio_create returns fatal error: %d\n", err));
		return err;
	}
	assert(dpio->conns);
	
	tca = tca_create();
	if (tca == NULL) {
		DPRINT(("server_restart: could not create tca\n"));
		return dp_RES_NOMEM;
	}
	*tserv = tserv_create(dpio, tca);
	if (*tserv == NULL) {
		DPRINT(("server_restart: could not create tserv\n"));
		return dp_RES_NOMEM;
	}
	err = dpio_setIncomingCallback(dpio, server_openhdl_cb, (*tserv));
	assert((*tserv)->dpio == dpio);
	assert((*tserv)->tca == tca);
	err = tserv_Thaw(*tserv, fp);
	if (err != dp_RES_OK) {
		DPRINT(("server_restart: tserv_Thaw returns err:%d\n", err));
		return err;
	}
	fclose(fp);

	assert(nClients == (*tserv)->clients->n_used);

	DPRINT(("server_restart: done.\n"));
	return dp_RES_OK;
}
#endif

/*----------------------------------------------------------------------------
 The server calls this periodically to get and process packets.
----------------------------------------------------------------------------*/
void server_poll(int init)
{
	dp_result_t err;
	playerHdl_t src;
	int flags;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;
	int res;
	tserv_event_t event;

	static tserv_t *tserv;

	if (init) {
		tca_t *tca;

		printf("Server initializing...");
		/* Open the password database */
		tca = tca_create();
		assert(tca);
		res = tca_openpw(tca, "pw.dat");
		assert(res != dp_RES_EMPTY);

		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		err = dpio_setIncomingCallback(dpio, server_openhdl_cb, tserv);
		printf(" done.\n");
		return;
	}

	now = eclock();
	if (tserv) tserv_assertValid(tserv);

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpio_get(dpio, &src, pktbuf, &pktlen, &flags);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (tserv) tserv_assertValid(tserv);
	if (err != dp_RES_OK) {
		playerHdl_t h;
		
		if (tserv)
			tserv_poll(tserv);
		err = dpio_update(dpio);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		h = dpio_findTimedOutHost(dpio);
		if (h != PLAYER_NONE)
			dpio_closeHdlImmed(dpio, h);
		return;
	}
	/* printf("server: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen); */
	/* Process the packet */
	switch (*((dp_packetType_t *)pktbuf)) {
	case dp_TSERV_PACKET_ID:
		err = tserv_handle_packet(tserv, src, flags, (tserv_packet_t *)(pktbuf+2), &event);
		assert(err == dp_RES_OK || err == dp_RES_EMPTY);
		if (err == dp_RES_OK) {
			printf("server: event h:%x, uid:%d, reason:%d\n",
					event.h, event.uid, event.reason);
			if (event.reason == dp_RES_HELP ||
				event.reason == dp_RES_CREATED) {
				tcapw_entry_t entry;
				dp_netchar_t ncs[tcapw_LEN_PW];
				
				/* An email request just came in, pull the secret code out
				 * of the database and "email" it.
				 */
				err = tserv_uid2info(tserv, event.uid, &entry);
				assert(!err);
				err = tcapw_get_secretcode(&entry, ncs);
				assert(!err);
				mywcs_netchar2wchar(secretcode, ncs, tcapw_LEN_PW);
				DPRINT(("server: 'emailing' secretcode %s (%d) to %s\n", tcapw_u2ascii(ncs, tcapw_LEN_PW), entry.secretcode, entry.email));
				DPRINT(("server: secretcode[0] = %d\n", secretcode[0]));
#ifndef SIMNET
				fprintf(stderr, "server: And The Secret Code is... %d\n", entry.secretcode);
#endif
			}
		}
#ifndef SIMNET
		/* test tserv_Freeze/Thaw */
		err = server_restart(&tserv);
		if (err != dp_RES_OK) {
			fprintf(stderr, "server: restart failed with err:%d\n", err);
			DPRINT(("server: restart failed with err:%d\n", err));
			exit(1);
		} 
		if (tserv) tserv_assertValid(tserv);
#endif
		break;
		
	default:
		printf("server: unexpected packet type %02x%02x\n", pktbuf[0], pktbuf[1]);
	}
	if (tserv) tserv_assertValid(tserv);
}

/* Scan and open the given ASCII address */
playerHdl_t openServer(const char *servername)
{
	playerHdl_t serverhdl;
	char serveraddr[dp_MAX_ADR_LEN];
	int addrlen;

	addrlen = dpio_scanAdr(dpio, (char *)servername, serveraddr, dp_MAX_ADR_LEN);
	if (addrlen == 0) {
		printf("dpio_scanAdr(%s) failed\n", servername);
		exit(1);
	}
	serverhdl = dpio_openHdl2(dpio, serveraddr, NULL);
	if (serverhdl == PLAYER_NONE) {
		printf("dpio_openHdl2 failed\n");
		exit(1);
	}
	return serverhdl;
}

/*----------------------------------------------------------------------------
 The client calls this periodically; it implements his internal state machine,
 which goes thru the normal login sequence.
 Returns TRUE when test complete.
----------------------------------------------------------------------------*/
int client_poll(int init)
{
	dp_result_t err;
	playerHdl_t src;
	playerHdl_t phdl;
	int flags;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;
	tserv_event_t result;
	int got_result;
	wchar_t pw1[tcapw_LEN_PW], pw2[tcapw_LEN_PW];
	wchar_t uname1[tcapw_LEN_USERNAME];
	wchar_t email[tcapw_MAXLEN_EMAIL];
	int done = FALSE;
	char *servername = servernames[0];
	
	static int client_state = 0;
	static tserv_t *tserv = NULL;
	static tca_t *tca = NULL;
	static tcapw_uid_t uid = tcapw_UID_NONE;
	static playerHdl_t serverhdl = PLAYER_NONE;
	static clock_t deadline;

	if (init) {
		client_state = 0;
		deadline = now;
		tca = tca_create();
		assert(tca);
		tserv = NULL;
		secretcode[0] = 0;
		err = dpio_setIncomingCallback(dpio, client_openhdl_cb, tserv);
		return FALSE;
	}

	now = eclock();

	got_result = FALSE;

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpio_get(dpio, &src, pktbuf, &pktlen, &flags);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (err != dp_RES_OK) {
		playerHdl_t h;
		
		if (tserv)
			err = tserv_poll(tserv);
		if (err == dp_RES_HOST_NOT_RESPONDING) {
			printf("client: Server reply timed out!\n");
			exit(1);
		}	  
		err = dpio_update(dpio);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		h = dpio_findTimedOutHost(dpio);
		if (h != PLAYER_NONE)
			dpio_closeHdlImmed(dpio, h);
	} else {
		printf("client: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen);
		/* Process the packet, possibly transforming it into a tserv_event_t */
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_TSERV_PACKET_ID:
			if (!tserv) {
				printf("client: got tserv packet, but no tserv\n");
				exit(1);
			}
			err = tserv_handle_packet(tserv, src, flags, (tserv_packet_t *)(pktbuf+2), &result);
			assert(err == dp_RES_OK || err == dp_RES_EMPTY);
			if (err == dp_RES_OK) {
				printf("client: event h:%x, uid:%d, reason:%d\n",
						result.h, result.uid, result.reason);
				got_result = TRUE;
			}
			break;
		default:
			printf("client: unexpected packet type %02x%02x\n", pktbuf[0], pktbuf[1]);
		}
	}

	switch (client_state) {
	case 0:
		client_state = 20;
		printf("client: going to state %d\n", client_state);
		break;

	case 20:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 30:	   	/* Send username and password */
		DPRINT(("client: Setting username and password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "user1");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "pass1");
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		printf("client: going to state %d\n", client_state);			
		break;

	case 40:        /* Wait for user1 authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d\n", result.uid);
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state = 100;
			printf("client: going to state %d\n", client_state);				
		}
		break;

	case 100:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 110:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Done closing (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 115:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 120:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 130:			/* Send username and bad password */
		DPRINT(("client: Setting username and bad password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "user1");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "bogus");
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 140:			/* Wait for user1 authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason == dp_RES_OK) {
				printf("client: fail: expected result.reason not ok, got %d\n", result.reason);
				exit(1);
			}
			printf("client: not logged in (as expected), reason %d\n", result.reason);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 150:			/* Send username and good password */
		DPRINT(("client: Setting username and correct password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "user1");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "pass1");
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 160:			/* Wait for user1 authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d\n", result.uid);
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state = 200;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 200:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 210:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 215:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 220:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 230:			/* Send bad username and bad password */
		DPRINT(("client: Setting bad username and password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "bogus");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "bogus");
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 240:			/* Wait for user1 authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason == dp_RES_OK) {
				printf("client: fail: expected result.reason not ok, got %d\n", result.reason);
				exit(1);
			}
			printf("client: not logged in (as expected), reason %d\n", result.reason);
			client_state = 300;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 300:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 310:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 315:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 320:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 330:			/* Send new username and password */
		secretcode[0] = 0;
		DPRINT(("client: Sending new username and password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "newuser");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "newpass");
		mywcs_frommbs(email, tcapw_MAXLEN_EMAIL, "jscanlin@activision.com");
		err = tserv_account_createW(tserv, uname1, pw1, 0, email);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 340:			/* Wait for newuser authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_NOTYET) {
				printf("client: fail: expected result.reason %d (NOTYET), got %d\n", dp_RES_NOTYET, result.reason);
				exit(1);
			}
			printf("client: logged in and created uid %d, but not activated\n", result.uid);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 350:  /* Ask for an emailed secret code */
#if 0		
		if ((err = tserv_request_email(tserv)) == dp_RES_OK) {
			DPRINT(("client: Sent email request (%d)\n", client_state));
			client_state = 400;
			printf("client: going to state %d\n", client_state);
		} else if (err == dp_RES_BUSY) {
			DPRINT(("client: queue full\n", client_state));
		} else {
			printf("client: tried to request email, but got err:%d\n", err);
			exit(1);
		}
#else
		client_state = 400;  /* skip it since creation should trigger email */
#endif
		break;
		
	case 400:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 410:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 415:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 420:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;
		
	case 430:	   /* Send newuser's username and password */
		DPRINT(("client: Setting new user's username and password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "newuser");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "newpass");
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 440:			/* Wait for newuser authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_NOTYET) {
				printf("client: fail: expected result.reason %d (NOTYET), got %d\n", dp_RES_NOTYET, result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d, not activated\n", result.uid);
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 450:       /* submit incorrect secret code */
#ifdef SIMNET
		assert(secretcode[0]);
		if (secretcode[0])
#endif
		{
			dp_netchar_t ncs[tcapw_LEN_PW];
			
			mywcs_wchar2netchar(ncs, secretcode+1, tcapw_LEN_PW);
			DPRINT(("client: sending wrong secret code %s (%d)\n", tcapw_u2ascii(ncs, tcapw_LEN_PW), client_state));
			err = tserv_account_activateW(tserv, secretcode+1);
			client_state += 5;
			printf("client: going to state %d\n", client_state);			
		}
		break;

	case 455:			/* Wait for activation notice */
		if (got_result) {
			DPRINT(("client: Receiving activation validation (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_NOTYET) {
				printf("client: fail: expected result.reason %d (NOTYET), got %d\n", dp_RES_NOTYET, result.reason);
#ifdef SIMNET
				exit(1);
#endif
			}
			printf("client: logged in, uid %d, not activated (as expected)\n", result.uid);
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state += 2;
			printf("client: going to state %d\n", client_state);
#ifndef SIMNET
			secretcode[0] = 0;
			printf("client: Please enter secret code: ");
#endif
		}
		break;
		
	case 457:       /* submit correct secret code */
		if (secretcode[0]) {
			dp_netchar_t ncs[tcapw_LEN_PW];

			mywcs_wchar2netchar(ncs, secretcode, tcapw_LEN_PW);
			DPRINT(("client: sending secret code %s (%d)\n", tcapw_u2ascii(ncs, tcapw_LEN_PW), client_state));
			err = tserv_account_activateW(tserv, secretcode);
			client_state += 3;
			printf("client: going to state %d\n", client_state);			
		}
		break;		

	case 460:			/* Wait for activation notice */
		if (got_result) {
			DPRINT(("client: Receiving activation validation (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason %d (OK), got %d\n", dp_RES_OK, result.reason);
#ifdef SIMNET
				exit(1);
#else
				secretcode[0] = 0;
				client_state -= 3;
				printf("client: Please reenter secret code: ");
				break;
#endif
			}
			printf("client: logged in, uid %d, activated!\n", result.uid);
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 470:	    /* Ask to change newuser's password */
		DPRINT(("client: Sending a new password request (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "newuser");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "newpass");
		mywcs_frommbs(pw2, tcapw_LEN_PW, "newpass2");
		mywcs_frommbs(email, tcapw_MAXLEN_EMAIL, "jscanlin@activision.com");
		err = tserv_change_passwordW(tserv, pw1, pw2, 0, email);
		if (err != dp_RES_OK) {
			DPRINT(("client: change_password returns %d\n", err));
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 480:			/* Wait for pwvalidate */
		if (got_result) {
			DPRINT(("client: Receiving pwchange validation (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_CHANGED) {
				printf("client: fail: expected result.reason %d (CHANGED), got %d\n", dp_RES_CHANGED, result.reason);
				exit(1);
			}
			printf("client: changed password successfully\n");
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 490:	    /* Ask to change newuser's password incorrectly */
		DPRINT(("client: Sending an incorrect new password request (%d)\n", client_state));
		mywcs_frommbs(pw1, tcapw_LEN_PW, "newpassx");
		mywcs_frommbs(pw2, tcapw_LEN_PW, "newpassx");
		mywcs_frommbs(email, tcapw_MAXLEN_EMAIL, "jscanlin@activision.com");
		err = tserv_change_passwordW(tserv, pw1, pw2, 0, email);
		client_state += 5;
		printf("client: going to state %d\n", client_state);
		break;

	case 495:			/* Wait for pwchange authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_ACCESS) {
				printf("client: expected result.reason %d (ACCESS), got %d\n", dp_RES_ACCESS, result.reason);
				exit(1);
			}
			printf("client: password change unsuccessful (as expected)\n");
			client_state = 500;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 500:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 510:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 515:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 520:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 530:	    /* Send newuser's username and new password */
		DPRINT(("client: Sending newuser's username and new password (%d)\n", client_state));
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, "newuser");
		mywcs_frommbs(pw1, tcapw_LEN_PW, "newpass2");
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 540:			/* Wait for login authorization */
		if (got_result) {
			DPRINT(("client: Receiving login validation (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason %d (OK), got %d\n", dp_RES_OK, result.reason);
				exit(1);
			}
			printf("client: logged in as uid %d\n", result.uid);
			client_state = 600;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 600:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 610:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 615:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 620:		/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting (%d)\n", client_state));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 630:	   /* Send newuser's username and password, ascii */
		DPRINT(("client: Setting new user's username and password - Ascii (%d)\n", client_state));
		err = tserv_account_loginA(tserv, "newuser", "newpass2");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 640:			/* Wait for newuser authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason %d (OK), got %d\n", dp_RES_OK, result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d\n", result.uid);
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 650:	    /* Ask to change newuser's password */
		DPRINT(("client: Sending a new password request - Ascii (%d)\n", client_state));
		err = tserv_change_passwordA(tserv, "newpass2", "newpass", 0, "jscanlin@activision.com");
		if (err != dp_RES_OK) {
			DPRINT(("client: change_password returns %d\n", err));
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 660:			/* Wait for pwchange authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_CHANGED) {
				printf("client: fail: expected result.reason %d (CHANGED), got %d\n", dp_RES_CHANGED, result.reason);
				exit(1);
			}
			printf("client: changed password successfully\n");
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 670:	    /* Ask to change newuser's password incorrectly */
		DPRINT(("client: Sending an incorrect new password request - Ascii (%d)\n", client_state));
		err = tserv_change_passwordA(tserv, "bogus", "bogus2", 0, "jscanlin@activision.com");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 680:			/* Wait for pwchange authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_ACCESS) {
				printf("client: expected result.reason %d (ACCESS), got %d\n", dp_RES_ACCESS, result.reason);
				exit(1);
			}
			printf("client: password change unsuccessful (as expected)\n");
			client_state = 1000;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 1000:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		printf("client: going to state %d\n", client_state);
		client_state += 10;
		break;

	case 1010:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		assert(n_connects == 0 || n_connects == 1);
		break;
		
	case 1015:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			DPRINT(("client: Guess we weren't done closing\n"));
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;
		
	case 1020:		/* done */
		done = TRUE;
		break;
	}

	return done;
}

/*----------------------------------------------------------------------------
 The client calls this periodically; it implements his internal state machine,
 which goes thru the normal login sequence repeatedly to measure speed.
 Returns TRUE when test complete.
----------------------------------------------------------------------------*/
int client_poll_speedtest(int init)
{
	dp_result_t err;
	playerHdl_t src;
	playerHdl_t phdl;
	int flags;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;
	tserv_event_t result;
	int got_result;
	wchar_t pw1[tcapw_LEN_PW];
	wchar_t uname1[tcapw_LEN_USERNAME];
	int done = FALSE;
	char *servername;
	
	static int client_state = 0;
	static tserv_t *tserv = NULL;
	static tca_t *tca = NULL;
	static tcapw_uid_t uid = tcapw_UID_NONE;
	static playerHdl_t serverhdl = PLAYER_NONE;
	static clock_t deadline;
	static clock_t t_start;
	static int n_logins = 0;

	now = eclock();
	if (init) {
		client_state = 0;
		tca = tca_create();
		assert(tca);
		tserv = NULL;
		uid = tcapw_UID_NONE;
		serverhdl = PLAYER_NONE;
		deadline = now;
		t_start = now;
		n_logins = 0;
		err = dpio_setIncomingCallback(dpio, client_openhdl_cb, tserv);
		return FALSE;
	}

	got_result = FALSE;
	servername = servernames[n_logins%n_servers];
	
	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpio_get(dpio, &src, pktbuf, &pktlen, &flags);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (err != dp_RES_OK) {
		playerHdl_t h;
		
		if (tserv)
			err = tserv_poll(tserv);
		if (err == dp_RES_HOST_NOT_RESPONDING) {
			printf("client: Server reply timed out!\n");
			exit(1);
		}
		err = dpio_update(dpio);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		h = dpio_findTimedOutHost(dpio);
		if (h != PLAYER_NONE)
			dpio_closeHdlImmed(dpio, h);
	} else {
		/* printf("client: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen); */
		/* Process the packet, possibly transforming it into a tserv_event_t */
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_TSERV_PACKET_ID:
			if (!tserv) {
				printf("client: got tserv packet, but no tserv\n");
				exit(1);
			}
			err = tserv_handle_packet(tserv, src, flags, (tserv_packet_t *)(pktbuf+2), &result);
			assert(err == dp_RES_OK || err == dp_RES_EMPTY);
			if (err == dp_RES_OK) {
				DPRINT(("client: event h:%x, uid:%d, reason:%d\n",
						result.h, result.uid, result.reason));
				got_result = TRUE;
			}
			break;
		default:
			printf("client: unexpected packet type %02x%02x\n", pktbuf[0], pktbuf[1]);
		}
	}

	switch (client_state) {
	case 0:
		client_state = 20;
		break;

	case 20:	   	/* Connect */
		assert(n_connects == 0);
		DPRINT(("client: Connecting...\n"));
		tserv = tserv_create(dpio, tca);
		if (!tserv) {
			printf("Can't init tserv\n");
			exit(1);
		}
		serverhdl = openServer(servername);
		tserv_server_add(tserv, serverhdl);
		client_state += 10;
		break;

	case 30:	   	/* Send username and password */
		mywcs_frommbs(uname1, tcapw_LEN_USERNAME, user1);
		mywcs_frommbs(pw1, tcapw_LEN_PW, pass1);
		err = tserv_account_loginW(tserv, uname1, pw1);
		client_state += 10;
		break;

	case 40:        /* Wait for user1 authorization */
		if (got_result) {
			if (result.h != PLAYER_ME) {
				printf("client: fail: expected my authorization from server, but result.h = %d, != %d\n", result.h, PLAYER_ME);
				exit(1);
			}
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}
			DPRINT(("client: logged in with uid %d, now disconnecting...\n", result.uid));
			uid = result.uid;
			assert(uid != tcapw_UID_NONE);
			client_state = 100;
		}
		break;

	case 100:		/* Ready to Disconnect? */
		err = dpio_closeHdl(dpio, serverhdl);
		if (err != dp_RES_OK) {
			printf("client: tried to close server handle, but got err:%d\n", err);
			exit(1);
		}
		/*tserv_server_delete(tserv);*/
		client_state += 10;
		break;

	case 110:		/* Disconnect */
		if (n_connects == 0 && dpio_ReadyToFreeze(dpio, &phdl) != dp_RES_BUSY) {
			DPRINT(("client: Disconnected (%d)\n", client_state));
			tserv_destroy(tserv);
			tserv = NULL;
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
		}
		assert(n_connects == 0 || n_connects == 1);
		break;
		
	case 115:
		if (n_connects != 0 || dpio_ReadyToFreeze(dpio, &phdl) == dp_RES_BUSY) {
			client_state -= 5;
			break;
		}
		if ((long)(now - deadline) >= 0) {
			n_logins++;
			printf(".");
			if (!(n_logins%20)) {
				printf("\nclient: %5d logins, 20 in %5.1fs, %6.2f/s\n",
					   n_logins,
					   (float)(now - t_start)/ECLOCKS_PER_SEC,
					   (float)(20*ECLOCKS_PER_SEC)/(now - t_start));
				t_start = now;
			}
			client_state = 20;
			if (total_logins && n_logins > total_logins)
				client_state = 1000;
		}
		break;

	case 1000:
		done = TRUE;
		break;
	}
	return done;
}

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
	DPRINT(("create_db: create pw.dat\n"));
	tcapw = tcapw_create();
	assert(tcapw);
	remove("pw.dat");
	res = tcapw_open(tcapw, "pw.dat");
	assert(res == dp_RES_EMPTY);

	/* Add an activated user to it */
	DPRINT(("create_db: adding a user\n"));
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
	DPRINT(("create_db: done.\n"));
}


int tserv_test(int which)
{
	dp_result_t err;
	dp_transport_t transport;

#ifdef SIMNET
	DPRINT(("\n\n*** Starting test with retry_fraction %d%% ***\n", retry_fraction));
#else
	char kbbuf[80];
	char *c;

	c = kbbuf;
#endif

	now = eclock();
	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, commDLLName);
    err = dpio_create(&dpio, &transport, &commInitReq, &now, NULL);
	if (err == dp_RES_OK) {
		printf("dpio_create succeeded.\n");
	} else {
		printf("dpio_create returns fatal error: %d\n", err);
		exit(1);
	}
	
#ifndef SIMNET
	if (!server)
#endif
	{
		DPRINT(("tservt: initializing client; 1 2 3 is %d %d %d\n", 1, 2, 3));
#ifndef SIMNET
		if (which == 1)
			client_poll_speedtest(1);
		else
#endif
			client_poll(1);  /* initialize */
	}
#ifndef SIMNET
	else
#endif
	{
		char serveraddr[dp_MAX_ADR_LEN];
		int addrlen = dp_MAX_ADR_LEN;
		char printable[50];

		DPRINT(("tservt: initializing server\n"));
		/* print my address if I am server, so clients can connect */
		err = dpio_hdl2adr(dpio, PLAYER_ME, serveraddr, &addrlen);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		if (dpio_printAdr(dpio, serveraddr, addrlen, printable, 50)) {
			printf("I am server, unable to print my address\n");
			exit(1);
		}
		printf("I am server, my address is %s\n", printable);
		create_db();
		server_poll(1);  /* initialize */
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
#ifndef SIMNET
			else if (charFromUser == 10 || charFromUser == 13 /* Return */) {
				printf("\n");
				*c = 0;
				mywcs_frommbs(secretcode, tcapw_LEN_PW, kbbuf);
				printf("got secretcode %d\n", atoi(kbbuf));
				c = kbbuf;
			} else if (isprint(charFromUser)) {
				printf("%c", charFromUser);
				*(c++) = charFromUser;
			}
#endif			
		}
#ifdef SIMNET		
		sim_my_rx = 0;
		server_poll(0);
		sim_my_rx = 1;
		if (client_poll(0))
			break;
#else
		if (server) {
			server_poll(0);
		} else if (which == 1) {
			if (client_poll_speedtest(0))
			break;
		} else {
			if (client_poll(0))
			break;
		}
#endif
	}
	raw_end();
	
	printf("dpio_destroy unloading comm drivers...");
	dpio_destroy(dpio, 0);
	printf(" done.\n");

    return 0;
}

int main(int argc, char *argv[])
{
#ifndef SIMNET
	if (argc < 2) {
		printf("\
Usage: %s -N=comm.dll [-P=serveraddr][-S=sessionId][-T=disconnect_wait]\n\
    [-L=number_of_logins(in speed test)][-U=username][-V=password]\n\
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
	disconnect_wait = 0;  /* default to no wait for peer to disconnect */
	
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
	n_servers = 1;
	strcpy(servernames[0], "0");
#endif

#ifdef SIMNET
	retry_fraction = 0;
	if (tserv_test(0)) {
		printf("Test failed\n");
		return 1;
	}
	retry_fraction = 45;
	if (tserv_test(0)) {
		printf("Test failed\n");
		return 1;
	}
#else
	if (server) {
		printf("\n\n*** Starting server ***\n");
		if (tserv_test(0)) {
			printf("Test failed\n");
			return 1;
		}
	} else {
		printf("\n\n*** Starting speed test ***\n");
		if (tserv_test(1)) {
			printf("Test failed\n");
			return 1;
		}
		printf("\n\n*** Starting self-test ***\n");
		if (tserv_test(0)) {
			printf("Test failed\n");
			return 1;
		}
	}
#endif

	printf("No tests failed\n");
	return 0;
}
