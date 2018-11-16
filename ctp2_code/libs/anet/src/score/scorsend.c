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
 Program to connect to a server and dump the contents of it's score
 tables to the screen.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "dp2.h"
#include "eclock.h"

#define PRINT(a) printf a

#define MSG_CONNECTING "Connecting %d"
#define MSG_LAUNCHING  "Launching"
#define MSG_ERR_LAUNCH "Error %d launching game at line %d"
#define MSG_WIND_TITLE "Game Launcher"
#define MSG_ERROR_TITLE "Error"

#define MAX_PATH 255	/* Should be identical to one in dp2.c */
#define MAXARGS 10
#define DQUOTECHAR '"'
#define SPACECHAR (char)0x20
#define TABCHAR   (char)0x08

/*--------------------------------------------------------------------------
 Specific application code goes below here.
--------------------------------------------------------------------------*/

static void usage()
{
	static char *szUsage = "\
scorsend: send a score report to a server\n\
 Options:\n\
-n=path            	# which transport to use, e.g. -n=anet/winets2.dll\n\
-h                 	# host a session\n\
-m=maxplayers      	# Max number of players in session\n\
-j                 	# join a session\n\
-J=16              	# join any internet session (16=lobby, 0=game)\n\
-s=mygame          	# session name\n\
-p=myname          	# player name\n\
-a=206.43.1.0      	# address (of host; used when joining if no broadcast)\n\
-g=server_name  	# Name of the server to connect to \n\
-u=username passwd  # username and password for login\n\
-z=score         # submit a score report containing this score on exit\n\
-v               	# Allow player variables\n\
-w                 	# Don't exit until command does; return its exit code.\n\
-q=2000             # Wait 2000ms after disconnecting to handle retries\n\
-b=9600            	# Baud rate\n\
-c=0               	# COM port to use (0=COM1)\n\
-f=5555555        	# Phone number to dial when using modem, omit to answer\n\
-i=ATZ             	# Modem initialization string\n\
-d=1               	# Dial method (1=Tone, 0=Pulse)\n\
-o=206.41.1.1;206.41.1.2 # open connections to the list of addresses\n\
-o=@ADDRESSFILE          # open connections to the address in the file ADDRESSFILE\n\
\n\
 Example:\n\
    scorsend -n=anet/winets2d.dll -h -g=california4 -s=1 100 -u=name passwd\n\
";
	PRINT((szUsage));
	exit(1);
}

int dp_PASCAL myCommThaw_cb(int status, void *context)
{
	static int status_old = -1;

	if (status != status_old) {
		PRINT(("Connecting status:%d\n", status));
		status_old = status;
	}
	return TRUE;  /* don't cancel */
}

int main(int argc, char **argv)
{
	int argi;
	dp_t *dp = NULL;
	dp_result_t err;
	dp_appParam_t app;
	dp_launchParams_t params;
	int score;
	char scorebuf[3];
	int send_score = FALSE;
	int quit_wait = 1000;

	char	GameName[MAX_PATH]="";
	char	GamePath[MAX_PATH]="";
	char	GameArgs[MAX_PATH]="";
	char	GameCwd[MAX_PATH]="";
	char	GameShellOpts[MAX_PATH]="";

	FILE *fp;

	app.name = GameName;
	app.path = GamePath;
	app.args = GameArgs;
	app.cwd = GameCwd;
	app.shellOpts = GameShellOpts;

	memset(&params, 0, sizeof(params));
	params.Maxplayers=4;

	for (argi=1; argi<argc; argi++) {
		int c = argv[argi][0];
		int c1 = argv[argi][1];
		int c2 = argv[argi][2];
		char *arg;

		if (c != '-') continue;

		/* Parse argument, if present */
		/* Allow either = or : for argument separator */
		arg = NULL;
		if ((c2 == '=') || (c2 == ':'))
			arg = argv[argi]+3;

		/* Verify argument for those options that need it */
		if (!arg && (strchr("nspamgJuzq", c1))) {
			PRINT(("Bad argument '%s'; -%c requires parameter\n", argv[argi], c1));
			usage();
		}
		if ((c1 == 'u') && argc-argi < 2) {
			PRINT(("Bad argument '%s'; -%c requires 2 parameters\n", c1, argv[argi]));
		}

		switch (c1) {
		case 'b': params.commInitReq.baud=atoi(arg); break;
		case 'c': params.commInitReq.portnum=atoi(arg); break;
		case 'f': strcpy(params.Phonenum, arg); break;
		case 'i': strcpy(params.Modeministr, arg); break;
		case 'd': params.commInitReq.dialing_method=atoi(arg);break;
		case 'h': params.Host=1; break;
		case 'j': if (params.Join != 0) {
					PRINT(("Must specify only one of -j or -z.\n"));
				  	usage();
				  }
				  params.Join=1; break;
		case 'J': if (params.Join != 0) {
					PRINT(("Must specify only one of -j or -z.\n"));
				  	usage();
				  }
				  params.JoinAnyFlags=atoi(arg);
				  params.Join=dp_LAUNCHPARAMS_JOIN_ANY | 0x01; break;
		case 'w': params.Wait=1; break;
		case 'v': params.EnablePlayervars=1; break;
		case 'n': strcpy(params.Driver,arg); break;
		case 's': strcpy(params.Sessname, arg); break;
		case 'p': strcpy(params.Playname, arg); break;
		case 'a': strcpy(params.Adr,arg); break;
		case 'm': params.Maxplayers = atoi(arg); break;
		case 'g': strcpy(params.GameServer,arg); break;
		case 'u': strncpy(params.Username, arg, 16);
			      argi++;
				  strncpy(params.Password, argv[argi], 16);
				  break;
		case 'z': score = atoi(arg);
			scorebuf[0] = dpGETSHORT_FIRSTBYTE(score);
			scorebuf[1] = dpGETSHORT_SECONDBYTE(score);
			scorebuf[2] = 0;
			      send_score = TRUE;
				  break;
		case 'q': quit_wait = atoi(arg); break;
		case 'o':
		{
			/*
			 * Addresses are separated by ';'
			 * We convert this to the following
			 *
			 * "text address1"\0
			 * "text address2"\0
			 * "text address3"\0
			 * \0
			 *
			 */
			char	*ptr;

			params.commInitReq.flags |= comm_INIT_FLAGS_CONN_ADDR;
			strcpy(params.OpenAddresses, arg);

			/*
			 * If the first character of the arg is '@' then the argument is a file
			 */
			if (*arg == '@')
			{
				/*
				 * The file MUST be in the following format
				 *
				 * "text address1";"text address2";"text address3"EOF
				 */
				FILE *fp;

				fp = fopen(arg + 1, "rt");

				if (fp == NULL)
				{
					PRINT(("Could not open address file \"%s\"", arg + 1));
					usage();
				}

				if (fread(params.OpenAddresses, 1, 1024, fp) == 0)
				{
					PRINT(("File \"%s\" contains no data!", arg + 1));
					usage();
				}

				fclose(fp);
			}

			/*
			 * Add \0 to end
			 */
			ptr = params.OpenAddresses + strlen(params.OpenAddresses) + 1;
			*ptr = '\0';

			/*
			 * Convert ';' to \0
			 */
			ptr = params.OpenAddresses;
			while (ptr = strchr(ptr, ';'))
			{
				*ptr = '\0';
				ptr++;
			}
			break;
		}

		default:
			PRINT(("bad argument '%s'\n", argv[argi]));
			usage();
		}
	}

	/* Got arguments. */
	/* Check arguments. */
	if (params.Adr[0] && !params.Host && params.Join)
		params.Join |= dp_LAUNCHPARAMS_JOIN_ANY;
	if (params.Host + (params.Join & 0x01) != 1) {
		PRINT(("Must specify either host or join with -h or -j or -z.\n"));
		usage();
	}
	if (!params.Driver[0]) {
		PRINT(("Must specify a driver with -n.\n"));
		usage();
	}
	if ((params.Maxplayers < 2) || (params.Maxplayers > dp_MAXREALPLAYERS)) {
		PRINT(("params.Maxplayers must be between 2 and %d.\n", dp_MAXREALPLAYERS));
		usage();
	}
	if (params.GameServer[0] && params.Adr[0]) {
		PRINT(("cannot specify both address and game server.\n"));
		usage();
	}
	if ((params.Join & dp_LAUNCHPARAMS_JOIN_ANY) &&
		!(params.GameServer[0] || params.Adr[0])) {
		PRINT(("cannot specify joinany without specifying either address or game server.\n"));
		usage();
	}
	if ((params.Join & dp_LAUNCHPARAMS_JOIN_ANY) &&
		(params.JoinAnyFlags != 16) && params.GameServer[0]) {
		PRINT(("on game server, can specify joinany only with lobby session.\n"));
		usage();
	}

	if (!params.Playname[0]) {
		if (!params.Username[0]) {
			if (params.Host)
				strcpy(params.Playname, "host");
			else
				strcpy(params.Playname, "player");
		} else
			strcpy(params.Playname, params.Username);
	}

    params.commInitReq.sessionId = rand() ^ (rand() << 16) ^ time(0) ^ clock();
	params.commInitReq.reqLen = sizeof(commInitReq_t);

	fp = fopen("freeze.dat", "wb");
	if (!fp) {
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}
	if ((fwrite(dp_LAUNCHPARAMS_T_SIG, sizeof(dp_LAUNCHPARAMS_T_SIG), 1, fp) != 1) ||
		(fwrite(&params, sizeof(params), 1, fp) != 1) ||
		(fwrite(dp_LAUNCHPARAMS_T_SIG, sizeof(dp_LAUNCHPARAMS_T_SIG), 1, fp) != 1)) {
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}
	fclose(fp);

	fp = fopen("freeze.dat", "rb");
	if (!fp) {
		PRINT((MSG_ERR_LAUNCH, dp_RES_BAD, __LINE__));
		exit(1);
	}
	err = dpCommThaw(&dp, fp, myCommThaw_cb, NULL);
	if (err != dp_RES_OK) {
		PRINT((MSG_ERR_LAUNCH, err, __LINE__));
		exit(1);
	}
	fclose(fp);

	assert(dp);
	dp->now = eclock();

	if (dp) {
		char key[dp_KEY_MAXLEN];
		int keylen;
		dp_session_t sess;
		size_t size = sizeof(sess);
		dpid_t myid = dp_ID_NONE;
		int do_shutdown = 0;

		err = dpGetSessionDesc(dp, &sess, &size);
		if (err != dp_RES_OK) {
			printf("scorsend: dpGetSessionDesc returns error %d\n", err);
			exit(1);
		}
		key[0] = dp_KEY_PLAYERS;
		err = dpGetSessionId(dp, &sess, &key[1], &keylen);
		keylen++;
		if (err != dp_RES_OK) {
			printf("scorsend: dpGetSessionId returns error %d\n", err);
			exit(1);
		}
		err = dpRequestObjectDeltas(dp, TRUE, key, keylen);
		if (err != dp_RES_OK) {
			printf("scorsend: error %d from dpRequestObjectDeltas\n", err);
			exit(1);
		}

		while (1) {
			dpid_t idFrom, idTo;
			char pktbuf[dpio_MAXLEN_UNRELIABLE];
			size_t rxsize = dpio_MAXLEN_UNRELIABLE;

			err = dpReceive(dp, &idFrom, &idTo, 0, &pktbuf, &rxsize);
			switch (err) {
			case dp_RES_MODEM_NORESPONSE:
				printf("scorsend: modem reports NO CARRIER!\n");
				break;
			case dp_RES_HOST_NOT_RESPONDING:
				printf("scorsend: connection to game server lost!\n");
				break;
			case dp_RES_EMPTY:
				break;
			case dp_RES_OK:
				break;
			default :
				printf("scorsend: error %d receiving packet.\n", err);
				break;
			}
			if (err == dp_RES_OK) {
				dp_objectDelta_packet_t *delta;

				switch (*((dp_packetType_t *)pktbuf)) {
				case dp_OBJECTDELTA_PACKET_ID:
					delta = (dp_objectDelta_packet_t *)(pktbuf + sizeof(dp_packetType_t));
					if (delta->key[0] != dp_KEY_PLAYERS)
						break;
					switch (delta->status) {
					case dp_RES_CREATED:
						if (delta->flags & dp_OBJECTDELTA_FLAG_LOCAL) {
							myid = delta->data.p.id;
							printf("scorsend: My id is %d.\n", myid);
						}
						break;
					case dp_RES_DELETED:
						if (delta->data.p.id == myid) {
							printf("scorsend: My id:%d was deleted.\n", myid);
							myid = dp_ID_NONE;
						}
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			} else if (err != dp_RES_EMPTY) {
				printf("scorsend: dpReceive returns err:%d\n", err);
				exit(1);
			}

			if (do_shutdown) {
				if (dp_RES_BUSY != dpShutdown(dp, 3000, 1000, 0))
					break;  /* break out of main loop and dpDestroy */
				continue;  /* don't process packets if we're leaving */
			}

			if (send_score && myid != dp_ID_NONE) {
				err = dpReportScoreStart(dp, 0);
				assert(!err);
				printf("scorsend: sending score %d for myid:%d\n", score, myid);
				err = dpReportScoreBuf(dp, myid, 0, scorebuf, 3);
				assert(!err);
				err = dpReportScoreEnd(dp);
				assert(!err);
			}
			if (myid != dp_ID_NONE) {
				printf("scorsend: shutting down connection...\n");
				do_shutdown = 1;
			}
		}
	}
	printf("scorsend: Scorsend succeeded.  Calling dpDestroy.\n");
	dpDestroy(dp, 0);
	return 0;
}
