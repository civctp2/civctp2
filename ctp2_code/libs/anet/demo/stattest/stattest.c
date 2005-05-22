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

/*-------------------------------------------------------------------------
 Network status test program.
 Launch using anetdrop.
 Sends a single packet, then makes sure that dpGetStats agrees with our 
 idea of how many bytes were sent.
-------------------------------------------------------------------------*/
#include <stdio.h>
#include <assert.h>
#include "anet.h"
#include "dppack1.h"
#include "raw.h"
#include "eclock.h"

#define CHAT_PACKET_ID dppt_MAKE('C', 'H')
#define MAX_MESSAGE 200
#define CLIENT_WAIT .5
#define HOST_WAIT 10.5

int my_nPlayer;
dpid_t my_Player[dp_MAXREALPLAYERS];
dpid_t lowestId = dp_ID_NONE;

/*-------------------------------------------------------------------------
 Call to quit chat.
-------------------------------------------------------------------------*/
static void quit(dp_t *myDP, int exit_code)
{
	if (myDP) {
		dpClose(myDP);
		dpDestroy(myDP, 0);	/* go ahead and hang up */
	}
	raw_end();
	printf("stattest: Exiting with status %d.\n", exit_code);
	exit(exit_code);
}

/*-------------------------------------------------------------------------
 Callback triggered by listing players.
-------------------------------------------------------------------------*/
static void dp_FAR dp_PASCAL my_listPlayers_cb(dpid_t id, char_t * name, long flags, void *context)
{
	(void) context;

	if (id != dp_ID_NONE) {
		/* printf("stattest: my_listPlayers_cb: Found player id %d, name %s.\n", id, name); */
		if ((lowestId == dp_ID_NONE) || (lowestId > id)) {
			lowestId = id;
		}
		if ((flags & dp_EPC_FLAGS_LOCAL) > 0) {
			/* printf("stattest: my_listPlayers_cb: ...and it is me....\n"); */
			my_Player[my_nPlayer] = id;
			my_nPlayer++;
		}
	}
}

/*-------------------------------------------------------------------------
 Call to figure out our own ID.
-------------------------------------------------------------------------*/
dp_result_t countMyPlayers(dp_t *dp)
{
	dp_result_t err;

	my_nPlayer = 0;
	err = dpEnumPlayers(dp, NULL, my_listPlayers_cb, dp, 1750L);
	return err;
}

/*-------------------------------------------------------------------------
 Call to display welcome messages to user.
-------------------------------------------------------------------------*/
void chat_welcome(dp_t *myDP)
{
	dp_result_t err;
	dp_session_t sDesc;
	size_t size = sizeof(sDesc);
	char nameBuf[256];

	printf("     &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     \n");
	printf("     &&&&&&&&&&&&&&            Chat Session            &&&&&&&&&&&&&&     \n"); 
	printf("     &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     \n\n");
	my_Player[0] = dp_ID_NONE;
	err = dpGetSessionDesc(myDP, &sDesc, &size);
	if (err != dp_RES_OK) {
		printf("stattest: error %d calling dpGetSessionDesc.\n", err);
		exit(1);
	}
	err = countMyPlayers(myDP);
	if (err != dp_RES_OK) {
		printf("stattest: countMyPlayers: error %d calling dpEnumPlayers.\n", err);
		exit(1);
	}
	if (my_Player[0] == dp_ID_NONE) {
		printf("Error: stattest: don't know who you are.\n");
		exit(1);
	}
	err = dpGetPlayerName(myDP, my_Player[0], nameBuf, sizeof(nameBuf));
	if (err != dp_RES_OK) {
		printf("Error: stattest: can't get your name.\n");
		exit(1);
	}
	printf("Hi, you are in session %s.\n", sDesc.sessionName);
	if (my_Player[0] == lowestId) {
		printf("You are host.\n", nameBuf);
	}
	printf("Your user name is %s.\n", nameBuf);
	printf("You can end this session by pressing ^D on a line by itself.\n"); 
	printf("Chat away ...............\n\n");
}

/*-------------------------------------------------------------------------
 Call to send chat message.
-------------------------------------------------------------------------*/
dp_result_t chat_broadcast(dp_t *myDP, char *message)
{
	char buf[MAX_MESSAGE + 6] = "";
	struct {
		dp_packetType_t type;
		char buf[MAX_MESSAGE];
		char skip[6];
	} pkt;
	dp_result_t err;

	pkt.type = CHAT_PACKET_ID;
	strcpy(pkt.buf, message);
	err = dpSend(myDP, my_Player[0], dp_ID_BROADCAST, dp_SEND_RELIABLE, &pkt, sizeof(pkt) - sizeof(pkt.skip));
	DPRINT(("stattest: Broadcasting message\n"));
	return err;
}

int main(int argc, char **argv)
{
	int exitCode = 0;
	dp_t *myDP;
	dp_result_t err;
	char freezefile[100];
	char kbuf[MAX_MESSAGE] = "";
	clock_t tstart = 0;
	clock_t tfinish = 0;
	dp_stat_t before, after;
	int bytes_sent;
	
	if (argc < 2) {
		strcpy(freezefile, "freeze.dat");
	} else {
		strcpy(freezefile, *(argv + 1));
	}
	err = dpCreate(&myDP, NULL, NULL, freezefile);
	if (err != dp_RES_OK) {
		printf("stattest: error %d thawing %s.\n", err, freezefile);
		exit(1);
	}
		
	chat_welcome(myDP);
	err = dpGetStats(myDP, dp_STAT_DPIO_TX_UNREL_BYTES, &before, sizeof(before));
	if (err != dp_RES_OK) {
		printf("stattest: error %d getting stats\n", err);
		exit(1);
	}
	err = dpSend(myDP, my_Player[0], dp_ID_BROADCAST, dp_SEND_UNRELIABLE, "hello", 5);
	if (err != dp_RES_OK) {
		printf("stattest: error %d sending\n", err);
		exit(1);
	}
	err = dpGetStats(myDP, dp_STAT_DPIO_TX_UNREL_BYTES, &after, sizeof(after));
	if (err != dp_RES_OK) {
		printf("stattest: error %d getting stats\n", err);
		exit(1);
	}
	bytes_sent = after.out - before.out;

	printf("stattest: bytes sent %d\n", bytes_sent);
	DPRINT(("stattest: bytes sent %d\n", bytes_sent));
	/* Assume one player per machine */
	assert(bytes_sent == (dpNumPlayers(myDP)-1) * (5 + sizeof_dp_envelope_t));

	raw_init();
	while(1) {			/* input and receive loop */
		dpid_t idFrom;
		dpid_t idTo;
		struct {
			dp_packetType_t type PACK;
			union {
				dp_user_addPlayer_packet_t addPlayer;
				dp_user_delPlayer_packet_t delPlayer;
				unsigned char buf[dpio_MAXLEN_UNRELIABLE];
			} u PACK;
		} pkt;
		size_t pktsize;
		clock_t twait;

		if (tstart != 0) {
			/* kludge --- wait twait for player delete before quit */
			tfinish = eclock();
			if (twait < tfinish - tstart) break;
		}
		if (raw_kbhit()) {
			int len = strlen(kbuf);
			int ch = 0;
			if (len >= MAX_MESSAGE) {
				printf("\nstattest: Message too long. Truncating and sending.\n");
				err = chat_broadcast(myDP, kbuf);
				if (err != dp_RES_OK) {
					printf("stattest: error %d sending message.\n", err);
				}
				kbuf[0] = '\0';
				continue;
			}
			ch = raw_getc();
			if (ch == -1) {
				/* guess kbhit was wrong */
				continue;
			}
			if (ch == 4 || ch == 17) {		/* ^D, ^Q */
				int i;
				if (my_Player[0] == lowestId) {
					twait = HOST_WAIT * ECLOCKS_PER_SEC;
					printf("\n .... ten seconds to self-destruct .......\n");
				} else {
					twait = CLIENT_WAIT * ECLOCKS_PER_SEC;
					printf("\nquitting .... half a sec .......\n");
				}
				DPRINT(("stattest: got quit request; deleting local players.\n"));
				for (i = 0; i < my_nPlayer; i++) {
					dpDestroyPlayer(myDP, my_Player[i]);
				}
				tstart = eclock();
				if (ch == 17) {
					exitCode = 1;			/* if stub exists, tell it to die */
				}
				continue;
			}
			putchar(ch);
			fflush(stdout);
			if (ch == '\r') {					/* ^M */
				putchar('\n');
			}
			if (ch == 8) {						/* BSp */
				if (len > 0)
					kbuf[len-1] = 0;
				continue;
			}
			if (ch != '\r' && ch != '\n') {
				/* Append to buffer. */
				kbuf[len++] = ch;
				kbuf[len] = 0;
				continue;
			}
			/* Process the keyboard buffer. */
			if (!kbuf[0])
				continue;
	
			err = chat_broadcast(myDP, kbuf);
			if (err != dp_RES_OK) {
				printf("stattest: error %d sending message.\n", err);
			}
	
			/* Empty it. */
			kbuf[0] = 0;
		}

		/* Receive and process packets */
		pktsize = sizeof(pkt);
		err = dpReceive(myDP, &idFrom, &idTo, 0, &pkt, &pktsize);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			printf("stattest: error %d receiving packet.\n", err);
			continue;
		}
		if (err == dp_RES_EMPTY) continue;
		switch (pkt.type) {
		case dp_USER_ADDPLAYER_PACKET_ID:
			printf("stattest: Player %s joining.\n", pkt.u.addPlayer.name);
			break;
		case dp_USER_DELPLAYER_PACKET_ID:
			printf("stattest: Player %s exiting.\n", pkt.u.delPlayer.name);
			break;
		case dp_USER_HOST_PACKET_ID:
			printf("stattest: Host is dead. You are now host.\n");
			break;
		case CHAT_PACKET_ID:
			{
			char nameBuf[256];
			size_t size;
			err = dpGetPlayerName(myDP, idFrom, nameBuf, sizeof(nameBuf));
			if (err != dp_RES_OK) {
				printf("stattest: chat packet from unknown id %d.\n", idFrom);
			}
			pkt.u.buf[pktsize-sizeof(dp_packetType_t)] = 0;	/* Null terminate */
			DPRINT(("%s: %s\n", nameBuf, (char *)pkt.u.buf));
			printf("%s: %s\n", nameBuf, (char *)pkt.u.buf);
			break;
			}
		case dp_SESSIONLOST_PACKET_ID:
			printf("Sorry session lost, you might as well quit\n");
			break;
		default:
			break;
		}
	}
	quit(myDP, exitCode);
	return 1;
}
