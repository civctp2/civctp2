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
 Trivial test for the dp-level wrapper on the tserv module.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "dp2.h"
#include "raw.h"
#include "eclock.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* globals for use by main, echo_server/echo_client */
dp_t *myDP = NULL;
clock_t	now;

/* globals for use by main, ProcessCommandLine */
char commDLLName[256];
char servernames[4][256];
int n_servers = 0;
commInitReq_t commInitReq; /*  Parameters needed by some comm drivers. */
char phonenum[256];
char modeministr[256];
unsigned short sessiontype = 65;

char secretcode[16];
int disconnect_wait;  /* time to wait to disconnect after dpReadyToFreeze */
long total_logins = 1000;  /* total number of logins for speed test. 0=infinite */
char user1[tcapw_LEN_USERNAME+1];  /* username and pass to use in speed test */
char pass1[tcapw_LEN_PW+1];

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
A dp-level client server login and authentication test for the tserv module.\n\
If -P=serveraddr is specified, act as a client to a server at serveraddr.\n\
If not, act as a server and print out the network address for clients to\n\
connect to.\n");
				exit(1);
			} /*  end switch */
		}
	}
}

/*----------------------------------------------------------------------------
 The client calls this periodically; it implements his internal state machine,
 which goes thru the normal login sequence.
 Returns TRUE when test complete.
----------------------------------------------------------------------------*/
int client_poll(int init)
{
	dp_result_t err;
	dpid_t src;
	dpid_t dest;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	dp_account_packet_t *pkt;
	size_t pktlen;
	tserv_event_t result;
	int got_result;
	int done = FALSE;
	char *servername = servernames[0];

	static int client_state = 0;
	static dp_uid_t uid = dp_UID_NONE;
	static clock_t deadline;

	now = eclock();
	if (init) {
		client_state = 0;
		deadline = now;
		return FALSE;
	}

	got_result = FALSE;

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpReceive(myDP, &src, &dest, 0, pktbuf, &pktlen);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (err == dp_RES_EMPTY) {
	} else if (err != dp_RES_OK) {
		DPRINT(("client: dpReceive returned err:%d\n", err));
		return done;
	} else {
		/* printf("client: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen); */
		pkt = (dp_account_packet_t *)(pktbuf + sizeof(dp_packetType_t));
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_ACCOUNT_PACKET_ID:
			got_result = TRUE;
			result.h = PLAYER_ME;
			result.reason = pkt->reason;
			result.uid = pkt->uid;
			break;
		default:
			break;
		}
	}

	switch (client_state) {
	case 0:
		client_state = 20;
		printf("client: going to state %d\n", client_state);
		break;

	case 20:	   	/* Connect */
		DPRINT(("client: Connecting... (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 30:	   	/* Send username and password */
		DPRINT(("client: Setting username and password (%d)\n", client_state));
		err = dpAccountLogin(myDP, "user2", "pass2");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 40:        /* Wait for user2 authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d\n", result.uid);
			uid = result.uid;
			assert(uid != dp_UID_NONE);
			client_state = 100;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 100:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 110:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 115:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
			client_state -= 5;
			printf("client: still busy? going back to state %d\n", client_state);
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 120:		/* Connect */
		DPRINT(("client: Connecting... (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 130:			/* Send username and bad password */
		DPRINT(("client: Setting username and bad password (%d)\n", client_state));
		err = dpAccountLogin(myDP, "user2", "pass2x");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 140:			/* Wait for user2 authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
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
		err = dpAccountLogin(myDP, "user2", "pass2");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 160:			/* Wait for user2 authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d\n", result.uid);
			uid = result.uid;
			assert(uid != dp_UID_NONE);
			client_state = 200;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 200:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 210:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 215:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
			client_state -= 5;
			printf("client: still busy? going back to state %d\n", client_state);
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 220:		/* Connect */
		DPRINT(("client: Connecting... (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 230:			/* Send bad username and bad password */
		DPRINT(("client: Setting bad username and password (%d)\n", client_state));
		err = dpAccountLogin(myDP, "user2x", "pass2x");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 240:			/* Wait for user2 authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
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
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 310:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 315:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
			client_state -= 5;
			printf("client: still busy? going back to state %d\n", client_state);
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 320:		/* Connect */
		DPRINT(("client: Connecting... (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 330:			/* Send new username and password */
		secretcode[0] = 0;
		DPRINT(("client: Sending new username and password (%d)\n", client_state));
		err = dpAccountCreate(myDP, "newuser", "newpass", 3, "jscanlin@activision.com");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 340:			/* Wait for newuser authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
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
		if ((err = dpRequestEmail(myDP)) == dp_RES_OK) {
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
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 410:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 415:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
			client_state -= 5;
			printf("client: still busy? going back to state %d\n", client_state);
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 420:		/* Connect */
		DPRINT(("client: Connecting... (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 430:	   /* Send newuser's username and password */
		DPRINT(("client: Setting new user's username and password (%d)\n", client_state));
		err = dpAccountLogin(myDP, "newuser", "newpass");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 440:			/* Wait for newuser authorization */
		if (got_result) {
			DPRINT(("client: Got authorization (%d)\n", client_state));
			if (result.reason != dp_RES_NOTYET) {
				printf("client: fail: expected result.reason %d (NOTYET), got %d\n", dp_RES_NOTYET, result.reason);
				exit(1);
			}
			printf("client: logged in, uid %d, not activated\n", result.uid);
			uid = result.uid;
			assert(uid != dp_UID_NONE);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 450:       /* submit incorrect secret code */
        DPRINT(("client: sending wrong secret code %s (%d)\n", secretcode+1, client_state));
		err = dpAccountActivate(myDP, secretcode+1);
		client_state += 5;
		printf("client: going to state %d\n", client_state);
		break;

	case 455:			/* Wait for activation notice */
		if (got_result) {
			DPRINT(("client: Receiving activation validation (%d)\n", client_state));
			if (result.reason != dp_RES_NOTYET) {
				printf("client: fail: expected result.reason %d (NOTYET), got %d\n", dp_RES_NOTYET, result.reason);
			}
			printf("client: logged in, uid %d, not activated (as expected)\n", result.uid);
			uid = result.uid;
			assert(uid != dp_UID_NONE);
			client_state += 2;
			printf("client: going to state %d\n", client_state);
			secretcode[0] = 0;
			printf("client: Please enter secret code: ");
		}
		break;

	case 457:       /* submit correct secret code */
		if (secretcode[0]) {
			DPRINT(("client: sending secret code %s (%d)\n", secretcode, client_state));
			err = dpAccountActivate(myDP, secretcode);
			client_state += 3;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 460:			/* Wait for activation notice */
		if (got_result) {
			DPRINT(("client: Receiving activation validation (%d)\n", client_state));
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason %d (OK), got %d\n", dp_RES_OK, result.reason);
				secretcode[0] = 0;
				client_state -= 3;
				printf("client: Please reenter secret code: ");
				break;
			}
			printf("client: logged in, uid %d, activated!\n", result.uid);
			uid = result.uid;
			assert(uid != dp_UID_NONE);
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 470:	    /* Ask to change newuser's password */
		DPRINT(("client: Sending a new password request (%d)\n", client_state));
		err = dpChangePassword(myDP, "newpass", "newpass2", 3, "jscanlin@activision.com");
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
		err = dpChangePassword(myDP, "newpass", "newpassx", 0, "jscanlin@activision.com");
		client_state += 5;
		printf("client: going to state %d\n", client_state);
		break;

	case 495:			/* Wait for pwchange authorization */
		if (got_result) {
			DPRINT(("client: Receiving authorization (%d)\n", client_state));
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
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 510:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 515:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
			client_state -= 5;
			printf("client: still busy? going back to state %d\n", client_state);
			break;
		}
		if ((long)(now - deadline) >= 0) {
			DPRINT(("client: Done waiting for peer to close (%d)\n", client_state));
			client_state += 5;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 520:		/* Connect */
		DPRINT(("client: Connecting... (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 530:	    /* Send newuser's username and new password */
		DPRINT(("client: Sending newuser's username and new password (%d)\n", client_state));
		err = dpAccountLogin(myDP, "newuser", "newpass2");
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 540:			/* Wait for login authorization */
		if (got_result) {
			DPRINT(("client: Receiving login validation (%d)\n", client_state));
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason %d (OK), got %d\n", dp_RES_OK, result.reason);
				exit(1);
			}
			printf("client: logged in as uid %d\n", result.uid);
			client_state = 1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 1000:		/* Ready to Disconnect? */
		DPRINT(("client: Disconnecting (%d)\n", client_state));
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		printf("client: going to state %d\n", client_state);
		break;

	case 1010:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
			printf("client: going to state %d\n", client_state);
		}
		break;

	case 1015:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
			client_state -= 5;
			printf("client: still busy? going back to state %d\n", client_state);
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
	dpid_t src;
	dpid_t dest;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	dp_account_packet_t *pkt;
	size_t pktlen;
	tserv_event_t result;
	int got_result;
	int done = FALSE;
	char *servername;

	static int client_state = 0;
	static dp_uid_t uid = dp_UID_NONE;
	static clock_t deadline;
	static clock_t t_start = 0;
	static int n_logins = 0;

	now = eclock();
	if (init) {
		client_state = 0;
		deadline = now;
		t_start = now;
		servername = servernames[0];
		return FALSE;
	}

	servername = servernames[n_logins%n_servers];
	got_result = FALSE;

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpReceive(myDP, &src, &dest, 0, pktbuf, &pktlen);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (err == dp_RES_EMPTY) {
	} else if (err != dp_RES_OK) {
		DPRINT(("client: dpReceive returned err:%d\n", err));
		return done;
	} else {
		/* printf("client: received a packet of type %c%c kind %c length %d\n", pktbuf[0], pktbuf[1], pktbuf[2], pktlen); */
		pkt = (dp_account_packet_t *)(pktbuf + sizeof(dp_packetType_t));
		switch (*((dp_packetType_t *)pktbuf)) {
		case dp_ACCOUNT_PACKET_ID:
			got_result = TRUE;
			result.h = PLAYER_ME;
			result.reason = pkt->reason;
			result.uid = pkt->uid;
			break;
		default:
			break;
		}
	}

	switch (client_state) {
	case 0:
		client_state = 20;
		printf("client: going to state %d\n", client_state);
		break;

	case 20:	   	/* Connect */
		DPRINT(("client: t:%d Connecting to %s... (%d)\n", now, servername, client_state));
		err = dpSetGameServerEx(myDP, servername, sessiontype);
		if (err == dp_RES_OK) {
			client_state += 10;
		}
		break;

	case 30:	   	/* Send username and password */
		DPRINT(("client: t:%d Logging in to %s... (%d)\n", now, servername, client_state));
		err = dpAccountLogin(myDP, user1, pass1);
		client_state += 10;
		break;

	case 40:        /* Wait for user1 authorization */
		if (got_result) {
			DPRINT(("client: t:%d Got response from %s... (%d)\n", now, servername, client_state));
			if (result.reason != dp_RES_OK) {
				printf("client: fail: expected result.reason ok, got %d\n", result.reason);
				exit(1);
			}
			uid = result.uid;
			DPRINT(("client: logged in with uid %d, now disconnecting...\n", uid));
			assert(uid != dp_UID_NONE);
			client_state = 100;
		}
		break;

	case 100:		/* Ready to Disconnect? */
		DPRINT(("client: t:%d Disconnecting from %s... (%d)\n", now, servername, client_state));
		err = dpSetGameServerEx(myDP, NULL, 0);
		if (err != dp_RES_OK) {
			printf("client: tried to disconnect, but got err:%d\n", err);
			exit(1);
		}
		client_state += 10;
		break;

	case 110:		/* Disconnect */
		if (dpReadyToFreeze(myDP) != dp_RES_BUSY) {
			DPRINT(("client: t:%d ReadyToFreeze %s... (%d)\n", now, servername, client_state));
			client_state += 5;
			deadline = now + (disconnect_wait * ECLOCKS_PER_SEC)/1000;
		}
		break;

	case 115:
		if (dpReadyToFreeze(myDP) == dp_RES_BUSY) {
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

void tserv_test(int which)
{
	dp_result_t err;
	dp_transport_t transport;
	char kbbuf[80];
	char *c;

	c = kbbuf;
	now = eclock();
	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, commDLLName);

	printf("Creating dp...\n");
	/* Initialize the networking layer */
	DPRINT(("setting sessionId to %x for benefit of serial drivers.\n", commInitReq.sessionId));
	printf("setting sessionId to %x for benefit of serial drivers.\n", commInitReq.sessionId);
	err = dpCreate(&myDP, &transport, &commInitReq, NULL);
	if (err != dp_RES_OK) {
		printf("Unable to start communications");
		exit(2);
	}

	DPRINT(("tservt: initializing client; 1 2 3 is %d %d %d\n", 1, 2, 3));
	if (which == 1)
		client_poll_speedtest(1);
	else
		client_poll(1);  /* initialize */

	raw_init();
	while (1) {
		int charFromUser = 0;
		if (raw_kbhit()) {
			charFromUser = raw_getc();
			if (charFromUser == 27 /* Esc */) {
				printf("Escape hit.  Terminating.\n");
				break;
			} else if (charFromUser == 10 || charFromUser == 13 /* Return */) {
				printf("\n");
				*c = 0;
				strcpy(secretcode, kbbuf);
				printf("got secretcode %d\n", atoi(kbbuf));
				c = kbbuf;
			} else if (isprint(charFromUser)) {
				printf("%c", charFromUser);
				*(c++) = charFromUser;
			}
		}
		if (which == 1) {
			if (client_poll_speedtest(0))
				break;
		} else {
			if (client_poll(0))
				break;
		}
	}
	raw_end();
}

int main(int argc, char *argv[])
{
	int exitCode;

	/* The modem driver and loopback drivers use the random number
	 * generator, so initialize it here.  Games should do this, too.
	 */
	srand(time(0));
	/* Assert command will set this if it detects an assertion failure,
	 * so we can pass info about the assertion failure to the calling
	 * program in our exit status.
	 */
	exitCode = 0;

	if (argc < 2) {
		printf("\
Usage: %s -N=comm.dll [-P=serveraddr][-S=sessionId][-T=disconnect_wait]\n\
    [-L=number_of_logins(in speed test)][-U=username][-V=password]\n\
    [-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n\
    [-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n", argv[0]);
		printf("\
A dp-level client server login and authentication test for the tserv module.\n\
If -P=serveraddr is specified, act as a client to a server at serveraddr.\n\
If not, act as a server and print out the network address for clients to\n\
connect to.\n");
		exit(0);
	}

	disconnect_wait = 20;
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
	strcpy(user1, "user2");
	strcpy(pass1, "pass2");
	commDLLName[0] = 0;

	ProcessCommandLine(argc, argv);

	if (!commDLLName[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}
	if (!servernames[0][0] || n_servers == 0) {
		printf("No servers were specified.\n");
		exit(1);
	}
	printf("\n\n*** Starting login speed test ***\n");
	tserv_test(1);
	printf("\n\n*** Starting dpuser test ***\n");
	tserv_test(0);

	return exitCode;
}
