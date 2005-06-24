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

/*
 * Bench: a multitasking scripting language designed to allow simulation
 * of hundreds of concurrently executing clients.
 */
#ifdef UNIX
#include <sys/time.h>
#endif
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* #include "anet.h" */
#include "dp2.h"
#include "eclock.h"
#include "logpkt.h"	/* for logPkt_getFilename */

#include "prog.h"

#define ASSERT

#define TRUE  1
#define FALSE 0

#define bench_NUM_GLOBALS   1000

#define bench_NUM_TIMERS	6

#ifdef UNIX
/* Milliseconds to only process processes who are waiting on data */
#define bench_PROCESS_TIMEOUT	50
#endif

#define bench_RECV_TIMEOUT		10

typedef struct bench_time_s
{
    int             on;
	unsigned long	count;
	unsigned long	sum;
	unsigned long 	sumsq;
	unsigned long	min;
	unsigned long	max;
} bench_time_t;

#define bench_BITFLAG_ERROR			1
#define bench_BITFLAG_WAIT_OPEN		2
#define bench_BITFLAG_WAIT_PLAYER	4
#define bench_BITFLAG_WAIT_LOGIN	8

typedef enum
{
	bench_flag_OK,
	bench_flag_ERROR,
	bench_flag_WAIT
} bench_flag_t;

typedef struct bench_stat_s
{
   unsigned long rx_pkts;
   unsigned long rx_bytes;
   unsigned long tx_pkts;
   unsigned long tx_bytes;
} bench_stat_t;

#define bench_VERBOSITY_NONE 0
#define bench_VERBOSITY_ANETLOG 1
#define bench_VERBOSITY_NORMAL 2
#define bench_VERBOSITY_LOTS 3

typedef struct bench_s
{
	unsigned long	number;
	prog_process_t	process;
	unsigned long	timer[bench_NUM_TIMERS];
	unsigned short	species;
	dp_t			*dp;
	dp_session_t	session;
	dpid_t			id;
	int				idrank;	/* Access only thru bench_get_rank() */
	unsigned long	wait;
	unsigned long	flag;
	unsigned long	bitflag;	/* should eventually replace flag */
   	unsigned long	verbosity;
#ifdef UNIX
	long			filedesc;
#endif
	struct bench_s	*next;
	FILE *logfp;

	dp_uid_t myUID;		/* should get cleared when you disconnect from a server... */

	int				amHost;	/* TRUE if we create session, or become host */
} bench_t;


static bench_time_t		bench_timers[bench_NUM_TIMERS];
static bench_t			*bench_process_list = NULL;
static int		        bench_process_number = 0;
static int				bench_process_id = 0;
static int				bench_exitcode = -1;
static unsigned long	bench_starttime;
static bench_stat_t		bench_stats;
static unsigned long	bench_maxexec;
static int				bench_globals[bench_NUM_GLOBALS];

/* 
 * prototypes 
 */
static void bench_time_Reset(int timer);
static void bench_time_Sample(int timer, unsigned long sample);
static void bench_time_Report(int timer);
static int bench_time_GetNumber(int timer);
static double bench_time_GetAvg(int timer);
static double bench_time_GetStdDev(int timer);
static int bench_time_GetMax(int timer);
static int bench_time_GetMin(int timer);

static prog_res_t bench_expand_var(bench_t *bench, const char *varname, char *sresult);
static prog_res_t bench_expand_intvar(bench_t *bench, const char *varname, int *piresult, char *errbuf);
static int bench_expand_intvar_quick(bench_t *bench, const char *varname);

static prog_cmd_res_t benchCmd_DPRINT(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_Verbosity(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpCreate(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpDestroy(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpSetGameServer(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpOpen(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpClose(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpCreatePlayer(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpDestroyPlayer(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpSend(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_dpGetStats(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_Timer(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_Wait(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_Spawn(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_Print(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_LoseHost(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_Login(prog_process_t *process, const char *params, void *context);
static prog_cmd_res_t benchCmd_ReportScore(prog_process_t *process, const char *params, void *context);

static int dp_PASCAL bench_cb_session_join(dp_session_t *, long *, long, void *);
static void dp_PASCAL bench_cb_player_create(dpid_t, char_t *, long, void *);

static bench_t *bench_process_Create(prog_t *program, prog_line_t *start);
static prog_res_t bench_process_Start(bench_t *bench);
static prog_res_t bench_process_Stop(bench_t *bench);
static prog_res_t bench_process_Step();

#define bench_RANK
#ifdef bench_RANK

int bench_rank_dirty;

/*--------------------------------------------------------------------------
 Call this to set the current process's dpid.
--------------------------------------------------------------------------*/
static void bench_set_id(bench_t *bench, dpid_t id)
{
	bench->id = id;
	bench_rank_dirty = 1;
}

#define bench_MAX_PROCS 1000
dpid_t bench_compare_hostid;
bench_t *bench_compare_array[bench_MAX_PROCS];

int bench_compare_rank(const void *a, const void *b)
{
	const bench_t * const *ba, * const *bb;
	int ia, ib;

	ba = a;
	bb = b;
	ia = (int) (*ba)->id; if (ia == bench_compare_hostid) ia = 20;
	ib = (int) (*bb)->id; if (ib == bench_compare_hostid) ib = 20;
	return (ia - ib);
}

/*--------------------------------------------------------------------------
 Call this to get the current process's id rank.
 Assumes that all processes are in the same session; else idrank is garbage.
 Each process's idrank is set to -1 if he's not in a game, to 0 if he's host, 
 and to 1, 2, 3... if he's the normal client with the lowest, next lowest,
 etc. id.
--------------------------------------------------------------------------*/
static int bench_get_rank(bench_t *bench)
{
	int i;
	int n;
	bench_t *pb;
	dp_result_t err;

	if (!bench_rank_dirty)
		return bench->idrank;

	err = dpGetCurrentHostId(bench->dp, &bench_compare_hostid);
	if (err != dp_RES_OK) 
		return -1;

	bench_rank_dirty = 0;

	/* Update everyone's rank */

	for (i=0, pb=bench_process_list; pb; pb = pb->next) {
		pb->idrank = -1;
		if (pb->id != 0 && pb->id != dp_ID_NONE)
			bench_compare_array[i++] = pb;
	}
	n = i;
	/*printf("Before sort:\n");
	for (i=0; i<n; i++) {
		printf("rank %d id %d proc %d\n", i, 
				bench_compare_array[i]->id,
				bench_compare_array[i]->number);
	}*/
	qsort(bench_compare_array, n, sizeof(bench_t *), bench_compare_rank);
	/*printf("After sort:\n");*/
	for (i=0; i<n; i++) {
		bench_compare_array[i]->idrank = i;
		/*printf("rank %d id %d proc %d\n", i, 
				bench_compare_array[i]->id,
				bench_compare_array[i]->number);*/
	}

	return bench->idrank;
}

#endif

/*
 * bench_uptime
 */
unsigned long bench_uptime(void)
{
	return (eclock() - bench_starttime);
}

/*
 * bench_print
 */
void bench_print(bench_t *bench, const char *fmt, ...)
{
   	static char buff[1024];
   	va_list args;

   	if (bench->verbosity >= bench_VERBOSITY_NORMAL)
	{
   		va_start(args, fmt);
		vsprintf(buff, fmt, args);
   		va_end(args);
   
		printf("%03d : %07d : %s", bench->number, bench_uptime(), buff);
	}
}

/*
 * benchCmd_DPRINT
 *
 * Prints a message to the dp log
 *
 */
prog_cmd_res_t benchCmd_DPRINT(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Log\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Prints a message to the dp log; only useful in debug build\n");
		return (prog_cmd_RES_OK);
	}
   
   	DPRINT(("%s\n", params));
   
   	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_Verbosity
 *
 * Changes the current verbosity level
 *
 */
prog_cmd_res_t benchCmd_Verbosity(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Verbosity level\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Changes the verbosity level (currently 0-3)\n");
		return (prog_cmd_RES_OK);
	}
   
   	bench->verbosity = bench_expand_intvar_quick(bench, params);
   
   	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_dpCreate
 *
 * Create dp
 *
 */
prog_cmd_res_t benchCmd_dpCreate(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char outfile[40] = "";
	dp_result_t	res;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpCreate driver\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Creates the dp object which initializes the selected network driver\n");
		return (prog_cmd_RES_OK);
	}

	logPkt_getFilename(outfile);
	bench_print(bench, "dpCreate %s logfile:%s\n", params, outfile);

   	res = dpCreate(&bench->dp, (dp_transport_t *) params, NULL, NULL);

	if (res != dp_RES_OK)
	{
		printf("Could not create dp [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	
#ifdef UNIX	
	
	/*
	 * Change the rx mode to no waiting
	 */
	res = dpReportScore2(bench->dp, 0, dp_PARAMID_RXWAIT, 0);
	
	if (res != dp_RES_OK)
	{
		printf("Could not set rxWait mode to 0 [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}
	
	/*
	 * Obtain the file descriptor of the socket
	 */
	bench->filedesc = 0;
	dpGetParameterLong(bench->dp, dp_PARAMID_FILEDESC, &bench->filedesc);
	bench_print(bench, "dpGetParameterLong returned file descriptor %d\n", bench->filedesc);
		
#endif
	
	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_dpDestroy
 *
 */
prog_cmd_res_t benchCmd_dpDestroy(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	dp_result_t	res;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpDestroy\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Destroys the dp object which shuts down the network driver\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "dpDestroy\n");

	res = dpDestroy(bench->dp, 0);
	bench->dp = NULL;
	bench_set_id(bench, 0);

	if (res != dp_RES_OK)
	{
		printf("Could not destroy dp [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_dpSetGameServer
 *
 */
prog_cmd_res_t benchCmd_dpSetGameServer(prog_process_t *process, const char *params, void *context)
{
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];

	bench_t		*bench = (bench_t *) context;
	dp_result_t	res;

	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);
	
	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpSetGameServer server species.\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Changes the selected game server to the name specified.\n" \
				"Only certain drivers make use of this, eg. winets2.dll.\n");
		return (prog_cmd_RES_OK);
	}

	if (argc == 0)
	{
		bench_print(bench, "dpSetGameServer NULL 0\n");
		res = dpSetGameServerEx(bench->dp, NULL, 0);
	}	
	else
	{
		char server[80];
		char *pserver;
		clock_t t_start, t_finish;

		if (argc != 2)
		{
			printf("dpSetGameServer: error: expected 2 args, got %d\n", argc);
			return (prog_cmd_RES_ERROR);
		}

		if (argv[0][0] == '$') {
			prog_res_t res = bench_expand_var(bench, argv[0], server);
			if (res != prog_RES_OK) {
				printf("dpSetGameServer: Bad stringvar for server: %s\n", argv[0]);
				return (prog_cmd_RES_ERROR);
			}
			pserver = server;
		} else
			pserver = argv[0];

		bench->species = bench_expand_intvar_quick(bench, argv[1]);

		bench_print(bench, "dpSetGameServer %s %d\n", pserver, bench->species);
		t_start = eclock();
		res = dpSetGameServerEx(bench->dp, pserver, bench->species);
		t_finish = eclock();
		if ((long)(t_finish - t_start) > ECLOCKS_PER_SEC/3) {
			printf("dpSetGameServer: DNS for %s took %d ms, add it to etc/hosts!\n", argv[0], (1000 * (long)(t_finish - t_start))/ECLOCKS_PER_SEC);
			exit(1);
		}
	}
	
	if (res != dp_RES_OK)
	{
		printf("Could not set game server [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	return (prog_cmd_RES_CONTINUE);
}

/*-------------------------------------------------------------------------
 Callback triggered by listing sessions when trying to join a specific one.
 Saves session in bench->session if its name matches
 bench->session.sessionName.  (Yeah, it's a kludge.)
-------------------------------------------------------------------------*/
static int dp_PASCAL bench_cb_session_find(dp_session_t *sDesc, long *pTimeout,long flags,void *context)
{
	bench_t *bench = (bench_t *)context;
	assert(bench);

	if (!sDesc) {
		return FALSE;
	}

	/*DPRINT(("(k %d; %d/%d plrs; spec %d; fl %x; U1 %x; pw '%s') name %s reserved2 %d.%d.%d.%d\n",
		sDesc->karma & 0xffff, 
		sDesc->currentPlayers,
		sDesc->maxPlayers,
		sDesc->sessionType, sDesc->flags,
		sDesc->dwUser1, sDesc->szPassword,
		sDesc->sessionName,
		sDesc->reserved2[0] & 0xff,
		sDesc->reserved2[1] & 0xff,
		sDesc->reserved2[2] & 0xff,
		sDesc->reserved2[3] & 0xff
		));
	*/

	if (!strncmp(bench->session.sessionName, sDesc->sessionName, strlen(bench->session.sessionName))) {
		DPRINT(("bench_cb_session_find: found session %s\n", sDesc->sessionName));
		bench->session = *sDesc;
		return FALSE;
	}

	return TRUE;
}


/*
 * benchCmd_dpOpen
 *
 */
prog_cmd_res_t benchCmd_dpOpen(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	dp_result_t	res;
	char sessname[256];

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpOpen [CREATE] ANY|name\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Joins a named session.\n" \
				"If CREATE is specified then a session is created, otherwise the session is joined.\n" \
				"When joining a session, ANY can be used which joins any sessions.\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "dpOpen %s\n", params);

	/* Test to see if there is a creation message */
	if (!strncmp(params, "CREATE", 6))
	{
		/* Create a new session */
		char *ptr;

		ptr = strchr(params, ' ');
		if (!ptr)
		{
			printf("no name specified\n");
			return (prog_RES_ERROR);
		}
		ptr++;

		res = bench_expand_var(bench, ptr, sessname);
		if (res == prog_RES_OK)
			ptr = sessname;
			
		bench_print(bench, "Creating Session %s\n", ptr);

		memset(&bench->session, 0x00, sizeof (bench->session));
		strncpy(bench->session.sessionName, ptr, sizeof (bench->session.sessionName));
		bench->session.maxPlayers = dp_MAXREALPLAYERS;
		bench->session.flags = dp_SESSION_FLAGS_CREATESESSION;
		bench->session.sessionType = bench->species;

		res = dpOpen(bench->dp, &bench->session, bench_cb_session_join, bench);
		bench->amHost = TRUE;
	}
	else if (!strncmp(params, "ANY", 3))
	{
		/* Join Any Session */
		bench_print(bench, "Joining ANY session\n");
		res = dpOpen(bench->dp, NULL, bench_cb_session_join, bench);
	}
	else
	{
		res = bench_expand_var(bench, params, sessname);
		if (res != prog_RES_OK)
			strcpy(sessname, params);

		/* Join Specific Session */
		bench_print(bench, "Joining particular session %s\n", sessname);
		memset(&bench->session, 0x00, sizeof (bench->session));
		strncpy(bench->session.sessionName, sessname, sizeof (bench->session.sessionName));
		res = dpEnumSessions(bench->dp, &bench->session, NULL, 0, bench_cb_session_find, bench);
		if ((res != dp_RES_OK) || !bench->session.karma) {
			/* Note: if an enumsess is still active here, it still
			 * has a pointer to the callback function and its
			 * context pointer, which is why the context variable
			 * must be declared static.  I use timeout zero to avoid? this.
			 * FIXME
			 */
			res = dp_RES_BAD;
		} else {
			bench_print(bench, "Found particular session %s\n", bench->session.sessionName);
			res = dpOpen(bench->dp, &bench->session, bench_cb_session_join, bench);
		}
	}

	if (res != dp_RES_OK)
	{
		assert(res != dp_RES_BAD);
		
		printf("Could not open session [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	bench->bitflag |= bench_BITFLAG_WAIT_OPEN;
	bench->bitflag &= ~bench_BITFLAG_ERROR;
	bench->flag = bench_flag_WAIT;

	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_dpClose
 *
 */
prog_cmd_res_t benchCmd_dpClose(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	dp_result_t	res;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpClose\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Leaves the current session.\n" \
				"This should be done before attempting to join another session.\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "dpClose\n");

	bench->amHost = FALSE;
	res = dpClose(bench->dp);
	bench_set_id(bench, 0);

	if (res != dp_RES_OK)
	{
		printf("Could not close session [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_dpCreatePlayer
 *
 */
prog_cmd_res_t benchCmd_dpCreatePlayer(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		name[25];
	char		bname[32];
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	dp_result_t res;
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpCreatePlayer playername\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Creates a player in the current session with the name playername.\n" \
				"Appends the process number to the player name\n");
		return (prog_cmd_RES_OK);
	}

	if (argc != 1)
	{
		printf("Expecting 1 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the name */
	if (argv[0][0] == '$') {
		res = bench_expand_var(bench, argv[0], name);
		if (res != prog_RES_OK) {
			printf("ReportScore: Bad stringvar for name: %s\n", argv[0]);
			return (prog_cmd_RES_ERROR);
		}
	} else
		strncpy(name, argv[0], 24);
	name[24] = '\0';

	sprintf(bname, "%s[%d]", name, bench->number);
	bench_print(bench, "dpCreatePlayer %s\n", bname);
	
	res = dpCreatePlayer(bench->dp, bench_cb_player_create, bench, bname);

	if (res != dp_RES_OK)
	{
		printf("Could not create player [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	bench->bitflag |= bench_BITFLAG_WAIT_PLAYER;
	bench->bitflag &= ~bench_BITFLAG_ERROR;
	bench->flag = bench_flag_WAIT;

	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_dpDestroyPlayer
 *
 */
prog_cmd_res_t benchCmd_dpDestroyPlayer(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	dp_result_t res;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpDestroyPlayer\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Destroys the current player.\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "dpDestroyPlayer %s\n", params);

	res = dpDestroyPlayer(bench->dp, bench->id);
	bench_set_id(bench, 0);

	if (res != dp_RES_OK)
	{
		printf("Could not destroy player [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_dpSend
 *
 */
prog_cmd_res_t benchCmd_dpSend(prog_process_t *process, const char *params, void *context)
{
	struct
	{
		dp_packetType_t type;
		char			buffer[dpio_MAXLEN_UNRELIABLE];
	} packet;
	bench_t			*bench = (bench_t *) context;
	dp_result_t		res;
	unsigned long	length;
	char			*ptr;
	dpid_t			id;

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("dpSend to message\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Sends a message to the specified player.\n" \
				"If the player is BROADCAST then the message gets sent to all.\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "dpSend %s\n", params);

	if (!strncmp(params, "BROADCAST", 9))
	{
		id = dp_ID_BROADCAST;
	}
	else
	{
		id = 0;
	}

	ptr = strchr(params, ' ');
	if (!ptr)
	{
		printf("No message specified\n");
		return (prog_cmd_RES_ERROR);
	}
	ptr++;

	packet.type = dppt_MAKE('C', 'H');
	strncpy(packet.buffer, ptr, dpio_MAXLEN_UNRELIABLE - sizeof (dp_packetType_t));
	length = sizeof (dp_packetType_t) + strlen(packet.buffer) + 1;

	res = dpSend(bench->dp, bench->id, id, dp_SEND_RELIABLE, &packet, length);

	if (res != dp_RES_OK)
	{
		printf("Couldn't send message [%d]\n", res);
		return (prog_cmd_RES_ERROR);
	}

	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_dpGetStats
 */
static prog_cmd_res_t benchCmd_dpGetStats(prog_process_t *process, const char *params, void *context)
{
   bench_t     *bench = (bench_t *) context;
   dp_result_t res;
   dp_stat_t   stat;
   
   res = dpGetStats(bench->dp, dp_STAT_DPIO_RX_UNREL_PKTS, &stat, sizeof (dp_stat_t));
   bench_stats.rx_pkts += stat.in;
   bench_print(bench, "Unreliable Packets RX:  %5d [%7d]\n", stat.in, bench_stats.rx_pkts);
   res = dpGetStats(bench->dp, dp_STAT_DPIO_RX_UNREL_BYTES, &stat, sizeof (dp_stat_t)); 
   bench_stats.rx_bytes += stat.in;
   bench_print(bench, "Unreliable Bytes RX:    %5d [%7d]\n", stat.in, bench_stats.rx_bytes);
   res = dpGetStats(bench->dp, dp_STAT_DPIO_TX_UNREL_PKTS, &stat, sizeof (dp_stat_t)); 
   bench_stats.tx_pkts += stat.out;
   bench_print(bench, "Unreliable Packets TX : %5d [%7d]\n", stat.out, bench_stats.tx_pkts);
   res = dpGetStats(bench->dp, dp_STAT_DPIO_TX_UNREL_BYTES, &stat, sizeof (dp_stat_t)); 
   bench_stats.tx_bytes += stat.out;
   bench_print(bench, "Unreliable Bytes TX:    %5d [%7d]\n", stat.out, bench_stats.tx_bytes);
   
   return (prog_cmd_RES_CONTINUE);
}

/* Convert a key to ASCII for debug printing */
static char *key2buf(const char *key, int keylen, char *buf)
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
static char key2a_buf[256];
static char key2a_buf2[256];
static char key2a_buf3[256];

/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)
/* You can use this one for the third key in a printf */
#define key2a3(key, keylen) key2buf(key, keylen, key2a_buf3)

/*--------------------------------------------------------------------------
 convert a dp_playerId_t to ASCII
--------------------------------------------------------------------------*/
static void dp_playerId_t_toString(const dp_playerId_t *src, char *buf)
{
	int i;
	
	/* print out dp_playerId_t field */
	sprintf(buf, "playerId: id=%d, karma=%d, name=%s, blob=", src->id, src->karma, src->name);
	for (i=0; i< src->bloblen; i++)
		sprintf(buf + strlen(buf), " %02x", src->blob[i]);
}

/*--------------------------------------------------------------------------
 convert a dp_session_t to ASCII
--------------------------------------------------------------------------*/
static void dp_session_t_toString(const dp_session_t *src, char *buf)
{
	int i;

	sprintf(buf, "session: sessType %d, name %s, k%d u%x f%x, cur %d, max %d, reserved2 %s, u ",
			src->sessionType, src->sessionName,
			src->karma, src->dwUser1, src->flags,
			src->currentPlayers,
			src->maxPlayers,
			key2a2(src->reserved2, sizeof(src->reserved2))
			);
	for (i=0; i < sizeof(src->szUserField); i++) 
		sprintf(buf + strlen(buf), " %02x", src->szUserField[i]);
	sprintf(buf +strlen(buf), ", adrMaster ");
	for (i=0; i < sizeof(src->adrMaster); i++) 
		sprintf(buf + strlen(buf), " %02x", src->adrMaster[i]);
}

/*--------------------------------------------------------------------------
 convert a dp_serverInfo_t to ASCII
--------------------------------------------------------------------------*/
static void dp_serverInfo_t_toString(const dp_serverInfo_t *src, char *buf)
{
	sprintf(buf, "serverInfo: host %s, rtt %d, loss %d%% sessType %d, cur_users %d, max_users %d, sessTypeUsers %d", 
			src->hostname, src->rtt_ms_avg, src->loss_percent, 
			src->sessType, src->cur_users, src->max_users, 
			src->cur_sessTypeUsers);
}

/*--------------------------------------------------------------------------
  converts fields in dp_objectDelta_packet_t into ASCII characters and put 
  them into a buffer.
--------------------------------------------------------------------------*/
static void dp_objectDelta2string(dp_objectDelta_packet_t *delta, char *buf) 
{
	if (delta->key[0] == dp_KEY_PLAYERS)
		sprintf(buf, "objectDelta: status:%d, latency:%dms, flags:%lx, key:%s, subkey:%s\n", delta->status, delta->latency, delta->flags, key2a(delta->key, delta->keylen), key2a2(delta->subkey, delta->subkeylen));
	else
		sprintf(buf, "objectDelta: status:%d, flags:%lx, key:%s, subkey:%s\n", delta->status, delta->flags, key2a(delta->key, delta->keylen), key2a2(delta->subkey, delta->subkeylen));
	
	switch (delta->key[0]) {
	case dp_KEY_PLAYERS:
		dp_playerId_t_toString(&delta->data.p, buf+strlen(buf));
		break;

	case dp_KEY_SESSIONS:
		dp_session_t_toString(&delta->data.sess, buf+strlen(buf));
		break;

	case dp_KEY_SERVERPINGS:
		dp_serverInfo_t_toString(&delta->data.serv, buf+strlen(buf));
		break;

	default:
		break;
	}
}


/*
 * benchCmd_dpReceive
 * 
 */
prog_cmd_res_t benchCmd_dpReceive(prog_process_t *process, const char *params, void *context)
{
	bench_t			*bench = (bench_t *) context;
	char			buf[dpio_MAXLEN_UNRELIABLE];
	dp_result_t		res;
	dpid_t			idFrom;
	dpid_t			idTo;
	size_t      	size;
	
	ASSERT(bench);

	/* If there is no dp then do nothing */
	if (!bench->dp)
	{
		bench_print(bench, "Call to bench poll when dp is invalid\n");
		return (prog_cmd_RES_ERROR);
	}
	
	/* Spin the network */
	while (1)
    {
   		size = sizeof (buf);
	   	res = dpReceive(bench->dp, &idFrom, &idTo, 0, &buf, &size);
			
		/* Was there any data ? */
		if (res == dp_RES_EMPTY)
		{
			dpFlush(bench->dp);
			return (prog_cmd_RES_CONTINUE);
		}

   		/* Has the host stopped responding ? */
    	if (res == dp_RES_HOST_NOT_RESPONDING)
    	{
			bench_print(bench, "Host is no longer responding\n");
			return (prog_cmd_RES_ERROR);
		}

		/* Was there an error */
		if (res == dp_RES_BAD)
		{
			bench_print(bench, "Error calling dpReceive\n");
			return (prog_cmd_RES_ERROR);
		}
		
		/* Was there something other than OK */
		if (res != dp_RES_OK)
		{
			bench_print(bench, "Unknown response from dpReceive [%d]\n", res); 
			return (prog_cmd_RES_ERROR);
		}
		
		/* There was data */
    	{
#include "dppack1.h"
		struct {
			dp_packetType_t type PACK;
			union {
				dp_user_addPlayer_packet_t addPlayer;
				dp_user_delPlayer_packet_t delPlayer;
				dp_user_addGroup_packet_t ag;
				dp_user_delGroup_packet_t dg;
				dp_addPlayerToGroup_packet_t gap;
				dp_delPlayerFromGroup_packet_t gdp;
				dp_user_playerData_packet_t pv;
				dp_objectDelta_packet_t delta;
				dp_account_packet_t acctpkt;
				dp_sessionResult_packet_t sessRes;
				unsigned char buf[512];
			} u PACK;
		} *pkt = (void *)buf;
#include "dpunpack.h"

			switch (pkt->type)
			{
				case dppt_MAKE('C', 'H'):
					bench_print(bench, "Received CHAT : From %04d : %s\n", idFrom, pkt->u.buf);
					break;
			
				case dp_USER_ADDPLAYER_PACKET_ID:
				{
					if (bench->verbosity >= bench_VERBOSITY_LOTS)
					{
						bench_print(bench, "Player '%s' [%d] added to session\n", pkt->u.addPlayer.name, pkt->u.addPlayer.id);
				 	}
				   	break;
				}
			
				case dp_USER_DELPLAYER_PACKET_ID:
				{
					if (bench->verbosity >= bench_VERBOSITY_LOTS)
				   	{
				   		bench_print(bench, "Player '%s' [%d] removed from session\n", pkt->u.delPlayer.name, pkt->u.delPlayer.id);
					}
				   	break;
				}
	
				case dp_USER_ADDGROUP_PACKET_ID:
					bench_print(bench, "Group %s (%d) added.\n", pkt->u.ag.name, pkt->u.ag.id);
					break;
		
				case dp_USER_DELGROUP_PACKET_ID:
					bench_print(bench, "Group %s (%d) deleted.\n", pkt->u.dg.name, pkt->u.dg.id);
					break;
		
				case dp_USER_ADDPLAYERTOGROUP_PACKET_ID:
					bench_print(bench, "Player %d added to group %d.\n", pkt->u.gap.dpIdPlayer, pkt->u.gap.dpIdGroup);
					break;
		
				case dp_USER_DELPLAYERFROMGROUP_PACKET_ID:
					bench_print(bench, "Player %d deleted from group %d.\n", pkt->u.gdp.dpIdPlayer, pkt->u.gdp.dpIdGroup);
					break;
		
				case dp_USER_PLAYERDATA_PACKET_ID:
					bench_print(bench, "New value for player %d's variable %d: len %d, value %-20.20s\n",
						pkt->u.pv.id, pkt->u.pv.key, pkt->u.pv.len, pkt->u.pv.data);
					break;
		
				case dp_USER_HOST_PACKET_ID:
					bench_print(bench, "This machine is now host.\n");
					bench->amHost = TRUE;
					break;

				case dp_SESSIONLOST_PACKET_ID:
					bench_print(bench, "Session lost!\n");
					bench->amHost = FALSE;
					bench_set_id(bench, 0);
					break;

				case dp_OBJECTDELTA_PACKET_ID:
					{
						char bigBuf[2048];
						dp_objectDelta2string(&pkt->u.delta, bigBuf);
						bench_print(bench, bigBuf);
					}
					break;

				case dp_ACCOUNT_PACKET_ID:
					switch (pkt->u.acctpkt.reason) {
					case dp_RES_OK:       /* logged in, account is activated */
						bench->myUID = pkt->u.acctpkt.uid;
						assert(bench->myUID != dp_UID_NONE);
						bench_print(bench, "Logged as uid %d. (account activated)\n", bench->myUID);
						bench->flag = bench_flag_OK;  /* clear the wait flag */
						bench->bitflag &= ~(bench_BITFLAG_WAIT_LOGIN
										  | bench_BITFLAG_ERROR);
						break;
					case dp_RES_NOTYET:   /* logged in, account is not activated */
						bench->myUID = pkt->u.acctpkt.uid;
						assert(bench->myUID != dp_UID_NONE);
						bench_print(bench, "Logged as uid %d. You must activate this account now.\n", bench->myUID);
						bench->flag = bench_flag_ERROR;  /* can't deal with this */
						bench->bitflag &= ~bench_BITFLAG_WAIT_LOGIN;
						bench->bitflag |= bench_BITFLAG_ERROR;
						break;
					case dp_RES_CHANGED:  /* acctchange succeeded */
						bench->myUID = pkt->u.acctpkt.uid;
						assert(bench->myUID != dp_UID_NONE);
						bench_print(bench, "Account change successful for uid %d.\n", bench->myUID);
						bench->flag = bench_flag_ERROR;  /* Huh? */
						bench->bitflag &= ~bench_BITFLAG_WAIT_LOGIN;
						bench->bitflag |= bench_BITFLAG_ERROR;
						break;
					case dp_RES_ACCESS:   /* failed, name/password incorrect */
						/* leave uid alone, since this may be a response to a
						 * password change attempt (failure != logout)
						 */
						bench_print(bench, "Access Denied.\n");
						bench->flag = bench_flag_ERROR;  /* Didn't get in */
						bench->bitflag &= ~bench_BITFLAG_WAIT_LOGIN;
						bench->bitflag |= bench_BITFLAG_ERROR;
						break;
					case dp_RES_ALREADY:  /* failed, account is in use */
						/* definitely not logged in */
						bench->myUID = dp_UID_NONE;
						bench_print(bench, "Access Denied, account is in use.\n");
						bench->flag = bench_flag_ERROR;  /* Didn't get in */
						bench->bitflag &= ~bench_BITFLAG_WAIT_LOGIN;
						bench->bitflag |= bench_BITFLAG_ERROR;
						break;
					default:              /* failed, bug */
						/* leave uid alone, since this may be a response to a
						 * password change attempt (failure != logout)
						 */
						bench_print(bench, "Account request failed, reason %d\n", pkt->u.acctpkt.reason);
						bench->flag = bench_flag_ERROR;  /* Bad error */
						bench->bitflag &= ~bench_BITFLAG_WAIT_LOGIN;
						bench->bitflag |= bench_BITFLAG_ERROR;
						break;
					}
					break;
				
				case dp_SESSIONRESULT_PACKET_ID:
					if (pkt->u.sessRes.reason != dp_RES_OK) {
						bench_print(bench, "Session join failed, reason:%d\n", pkt->u.sessRes.reason);
					} else {
						bench_print(bench, "Joined session %s, type:%d\n", pkt->u.sessRes.sess.sessionName, pkt->u.sessRes.sess.sessionType);
					}
					break;
			
				default:
					bench_print(bench, "Received %c%c : From %04d : Len %d : Data %02x %02x %02x %02x   %02x %02x %02x %02x\n",
						*buf, *(buf + 1), idFrom, size, 
						*(buf + 2) & 0xFF, *(buf + 3) & 0xFF, *(buf + 4) & 0xFF, *(buf + 5) & 0xFF,
						*(buf + 6) & 0xFF, *(buf + 7) & 0xFF, *(buf + 8) & 0xFF, *(buf + 9) & 0xFF);
					break;
			}
		}
	}
	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_Timer
 *
 * command "TIMER"
 * parameters "TIMER NUMBER" "OPTIONS"
 * options START END
 * 
 * example TIMER 0 START
 *
 */
prog_cmd_res_t benchCmd_Timer(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int 		timer;
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Timer timer command\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("There are %d timers available.\n" \
				"The following commands can be used for each timer.\n" \
				"START : starts a timer\n" \
				"END   : stops a timer and adds the sample to the timer statistics\n" \
				"COPY  : NUMBER|AVG|STDEV : copies the value into I0 (rounded)\n" \
				"REPORT: prints a report concerning the statistics\n" \
				"ZERO  : clears a timer" \
				"ON    : turns ON a timer" \
				"OFF   : turns OFF a timer", bench_NUM_TIMERS);
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "Timer %s\n", params);

	if (argc < 2)
	{
		printf("Expecting 2 or more arguments, found %d\n", argc);
	}

	/* Get the number of the timer */
	timer = bench_expand_intvar_quick(bench, argv[0]);
	if (timer >= bench_NUM_TIMERS || timer < 0)
	{
		printf("Invalid Timer!\n");
		return (FALSE);
	}

	/* Process the command type */
	switch (*argv[1])
	{
		case 's':
		case 'S':
			/* Start a particular timer */
			bench->timer[timer] = eclock();
			break;

		case 'e':
		case 'E':
			/* Finish a particular timer */
			bench_time_Sample(timer, (eclock() - bench->timer[timer]) * 1000 / ECLOCKS_PER_SEC);
			break;
		
		case 'c':
		case 'C':
			/* Copy a particular statistic into I0 */
			switch (*argv[2])
			{
				case 'n':
				case 'N':
					prog_process_SetInteger(&bench->process, 0, bench_time_GetNumber(timer));
					break;
				
				case 'a':
				case 'A':
					prog_process_SetInteger(&bench->process, 0, (int) bench_time_GetAvg(timer));
					break;
				
				case 's':
				case 'S':
					prog_process_SetInteger(&bench->process, 0, (int) bench_time_GetStdDev(timer));
					break;
				
				default:
					printf ("Unknown Argument\n");
					return (prog_cmd_RES_ERROR);
					break;
			}
			break;
				
		case 'r':
		case 'R':
			bench_time_Report(timer);
			break;
	 
	 	case 'o':
	 	case 'O':
	   		
	   		switch (*(argv[1] + 1))
		    {
			 	case 'n':
			   	case 'N':
			   		bench_print(bench, "Turning Timer %d ON\n", timer);
			   		bench_timers[timer].on = 1;
			   		break;
			   
			 	case 'f':
			 	case 'F':
			   		bench_print(bench, "Turning Timer %d OFF\n", timer);
	   				bench_timers[timer].on = 0;
			   		break;
			    
			 	default:
			   		printf("Unknown Argument\n");
			   		break;
			}
	   		break;

		case 'z':
		case 'Z':
			bench_time_Reset(timer);
			break;
	   
		default:
			printf("Unknown Argument\n");
			return (prog_cmd_RES_ERROR);
			break;
	}
	return (prog_cmd_RES_CONTINUE);
}


/*
 * benchCmd_Wait
 *
 * Whilst we are waiting, spin the network...
 *
 * Note: the main code calls the current command only when there is
 * network traffic, or when 100ms (specified on commandline) has elapsed.
 * This limits the accuracy and sets a certain granularity for these waits!
 */
prog_cmd_res_t benchCmd_Wait(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	prog_cmd_res_t	res;
	long		waitflags;		/* what we're waiting for */
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);
	
	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Wait [time [time [Shutdown] | Freeze]] | Solitary]\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Waits for the wait flag to clear\n" \
				"[time] can be used to denote the number of ms to wait\n" \
				"[time time] waits a random number between the two times\n" \
				"[time Freeze] indicates to wait until the network is ready to freeze or until the timer expires\n" \
				"[time1 time2 Shutdown] closes the connection nicely, timing out each step (of 3) in time1 ms and waiting time2 ms after\n" \
				"[Solitary] indicates to wait until this is the only process running\n");
		return (prog_cmd_RES_OK);
	}

	if (bench->verbosity >= bench_VERBOSITY_LOTS)
	{
		bench_print(bench, "Wait %s\n", params);
	}

	/* If a callback declared an error, barf.  We're not picky about
	 * this, since any error aborts our programs (or triggers the error
	 * handler).
	 */
	if (bench->flag == bench_flag_ERROR) {
		return (prog_cmd_RES_ERROR);
	}

	/* Handle waits that do NOT want network traffic. */
	if ((argc >= 1) && (*argv[0] == 'S')) {
		/* wait until we are the solitary process, do NOT spin the network ! */	
		if (bench_process_number > 1) {
			return (prog_cmd_RES_WAIT);
		}	
		return (prog_cmd_RES_CONTINUE);
	}

	/* Pump network, whether or not the wait condition is over. */
	if (bench->dp) {
		res = benchCmd_dpReceive(process, params, context);
		if (res != prog_cmd_RES_CONTINUE) {
			bench_print(bench, "Wait aborted : %d\n", res);
			return (res);
		}
	}

	/* Handle generic callback waits. */
	if (argc == 0) {
		if (bench->flag == bench_flag_WAIT)
			return (prog_cmd_RES_WAIT);
		bench->wait = 0;
		return (prog_cmd_RES_CONTINUE);
	}

	/* Handle specific callback waits. */
	waitflags = 0;
	if (!strcmp(argv[0], "dpOpen")) {
		waitflags = bench_BITFLAG_WAIT_OPEN;
	} else if (!strcmp(argv[0], "dpCreatePlayer")) {
		waitflags = bench_BITFLAG_WAIT_PLAYER;
	} else if (!strcmp(argv[0], "Login")) {
		waitflags = bench_BITFLAG_WAIT_LOGIN;
	}
	if (waitflags) {
		if (bench->bitflag & waitflags)
			return (prog_cmd_RES_WAIT);
		if (bench->verbosity >= bench_VERBOSITY_LOTS)
			bench_print(bench, "Wait over.\n");
		bench->wait = 0;
		return (prog_cmd_RES_CONTINUE);
	}

	if ((argc >= 3) && (*argv[2] == 'S')) {
		clock_t timeout, waitafter;

		timeout = atol(argv[0]);
		waitafter = atol(argv[1]);

		/* Rely on shutdown's internal timeouts */
		if (dpShutdown(bench->dp, timeout, waitafter, 0) != dp_RES_BUSY) {
			bench_print(bench, "Finished shutting down\n");
			bench->amHost = FALSE;
			bench->flag = bench_flag_OK;
			bench->wait = 0;
			return prog_cmd_RES_CONTINUE;
		}
		return prog_cmd_RES_WAIT;
	}

	/* Following commands all use bench->wait to hold the time at
	 * which the command will time out.
	 * bench->wait is conveniently 0 normally, which lets us sense whether
	 * it's been set yet or not.
	 *
	 * Commands handled are:
	 * Wait nnnn
	 * Wait nnnn nnnn
	 * Wait nnnn FREEZE
	 */
	if (bench->wait == 0) {
		long msec_to_wait;

		msec_to_wait = atol(argv[0]);
		if ((argc >= 2) && isdigit(argv[1][0])) {
			long max_msec_to_wait = atol(argv[1]);
			msec_to_wait += rand() % (max_msec_to_wait - msec_to_wait);
		}
		if (bench->verbosity >= bench_VERBOSITY_LOTS) {
			bench_print(bench, "Waiting for %d ms\n", msec_to_wait);
		}
		bench->wait = eclock() + (msec_to_wait * ECLOCKS_PER_SEC) / 1000;
	}

	/* Freeze is just like a normal wait, but can exit out early */
	if ((argc == 2) && (*argv[1] == 'F'))
	{
		/* Test to see if we can freeze */
		if (dpReadyToFreeze(bench->dp) != dp_RES_BUSY)
		{
			bench_print(bench, "Ready to freeze\n");
			bench->flag = bench_flag_OK;
			bench->wait = 0;
			return (prog_cmd_RES_CONTINUE);
		}
	}

	/* See if wait is up */
	if ((long)(eclock() - bench->wait) > 0) {
		if (bench->verbosity >= bench_VERBOSITY_LOTS) {
			bench_print(bench, "Wait over.\n");

		}
		bench->flag = bench_flag_OK;
		bench->wait = 0;
		return (prog_cmd_RES_CONTINUE);
	}
	bench->flag = bench_flag_WAIT;
	return (prog_cmd_RES_WAIT);
}


/*
 * benchCmd_Spawn
 *
 * Spawn a new copy of this process starting at the label specified
 * The number indicates the number of copies to spawn
 *
 */
prog_cmd_res_t benchCmd_Spawn(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int 		num;
	prog_line_t *line;
	prog_cmd_res_t res;
	char		errbuf[256];
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);
	
	if (argc < 2)
	{
		printf("Expected at least 2 parameters, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}
	
	res = bench_expand_intvar(bench, argv[1], &num, errbuf);
	if (res != prog_RES_OK) {
		printf("Spawn: 2nd argument %s: %s\n", argv[1], errbuf);
		return (prog_cmd_RES_ERROR);
	}
	line = prog_Resolv(argv[0], process->program);
	
	if (num < 1 || num > 256)
	{
		printf("Number to Spawn must be in the range 1-256, supplied was '%s'=%d\n", argv[1], num);
		return (prog_cmd_RES_ERROR);
	}
	
	if (!line)
	{
		printf("Spawn: Could not resolve label name %s\n", argv[0]);
		return (prog_cmd_RES_ERROR);
	}
	
	while (num--)
	{
		int argi;
		bench_t *b = bench_process_Create(process->program, line);
		if (!b)
		{
			printf("Could not create bench\n");
			return (prog_cmd_RES_ERROR);
		}		
		
		bench_print(bench, "Spawning new process %d at %s\n", b->number, argv[0]);
		/* Pass in remaining parameters in the processes string array,
		 * evaluating any variables now.
		 */
		for (argi = 2; (argi < argc) && (argi < PROG_MAX_STRING + 2); argi++) {
			char *parg;
			char string[PROG_MAX_STRINGLEN];
			if (argv[argi][0] == '$') {
				int res = bench_expand_var(bench, argv[argi], string);
				if (res != prog_RES_OK)
					break;
				parg = string;
			} else
				parg = argv[argi];
			prog_process_SetString(&b->process, argi-2, parg);
			bench_print(bench, "   Param $S%d = %s\n", argi-2, parg);
		}
		bench_process_Start(b);
  	}
		
	return (prog_cmd_RES_CONTINUE);
}

/*--------------------------------------------------------------------------
 Return the value of the given integer expression, or 0 on error.
--------------------------------------------------------------------------*/
static int bench_expand_intvar_quick(bench_t *bench, const char *varname)
{
	prog_res_t res;
	int val;
	char errbuf[256];
	
	res = bench_expand_intvar(bench, varname, &val, errbuf);
	if (res != prog_RES_OK) {
		bench_print(bench, "bench_expand_intvar_quick: %s\n", errbuf);
		return 0;
	}
	return val;
}

/*--------------------------------------------------------------------------
 Expand integer variable references at runtime to their numerical values.
 Does not give access to string variables.
 On entry, varname is a string starting with $, a name, and then possibly a number.
 e.g.
    %d   is interpreted as a literal integer
    $dp 0 if no dp created, nonzero if dp already created
    $amHost 0 if not host, 1 if host
	$hostId dpid of current host of current session
	$myId   dpid of most recently locally created player
	$myUID  uid of most recently login
	$rank   rank of this player among all processes; 0=host, 1=lowest nonhost, etc.
	$numPlayers  number of players in current session
	$uptime Ticks since bench startup
	$procid id of current process
	$D%d (e.g. $D0 or $D15) expands to dpId of player ranked %d.
	$G%d  value of global integer variable %d
	$I%d  value of integer variable %d
	$R%d  Random integer from 0 to %d-1
	$S%d  atoi of string variable %d
	$TC%d current value of timer %d
	$TA%d average of past values of timer %d
	$TD%d standard deviation of past values of timer %d
	$Tm%d minimum of past values of timer %d
	$TM%d maximum of past values of timer %d

 On exit, value of that variable is placed in *piresult, and prog_RES_OK
 is returned.
 On error, prog_RES_SYNTAX is returned if variable name can't be parsed,
 and error message is copied to errbuf.
--------------------------------------------------------------------------*/
static prog_res_t bench_expand_intvar(bench_t *bench, const char *varname, int *piresult, char *errbuf)
{
	dpid_t hid;
	dp_result_t err;
	char *string;

	*errbuf = 0;

	/* Handle literal integers */
    if (isdigit(varname[0]) || (varname[0] == '-')) {
		*piresult = atoi(varname);
		return prog_RES_OK;
	}

	*piresult = 0;

    if (varname[0] != '$') {
		sprintf(errbuf, "Variable name %s did not start with $", varname);
		return prog_RES_SYNTAX;
	}

	switch (varname[1])
	{
		case 'a':
			*piresult = bench->amHost;
			break;

		case 'd':
			*piresult = (size_t) (bench->dp);
			break;

		case 'h':
			err = dpGetCurrentHostId(bench->dp, &hid);
			if (err != dp_RES_OK) hid = 0;
			*piresult = hid;
			break;

		case 'm':
			/* distinguish between $myUID and $myId */
			if (!strncmp(varname + 2, "yU", 2))
				*piresult = bench->myUID;
			else if (!strncmp(varname + 2, "yI", 2))
				*piresult = bench->id;
			break;

		case 'n':
			*piresult = dpNumPlayers(bench->dp);
			break;

		case 'p':
			*piresult = bench->number;
			break;

		case 'r':
			*piresult = bench_get_rank(bench);
			break;

		case 'u':
			*piresult = bench_uptime();
			break;

		case 'D':
			bench_get_rank(bench);
			*piresult = bench_compare_array[bench_expand_intvar_quick(bench, varname + 1)]->id;
			break;

		case 'G':
			*piresult = bench_globals[bench_expand_intvar_quick(bench, varname + 2)];
			break;

		case 'I':
			*piresult = prog_process_GetInteger(&bench->process, bench_expand_intvar_quick(bench, varname + 2));
			break;

		case 'R':
			*piresult = rand() % bench_expand_intvar_quick(bench, varname + 2);
			break;

		case 'S':
			/* It would be dangerous to expand instead of using atoi here, for
			 * instance, if $S0 contained the string "$S0".
			 */
			string = prog_process_GetString(&bench->process, bench_expand_intvar_quick(bench, varname + 2));
			if (!isdigit(string[0])
			&&  !((string[0] == '-') && isdigit(string[1])))
				return prog_RES_SYNTAX;
			*piresult = atoi(string);
			break;

		case 'T':
			switch (varname[2])
			{
				case 'C':
					*piresult = bench_time_GetNumber(bench_expand_intvar_quick(bench, varname + 3));
					break;

				case 'A':
					*piresult = (int) bench_time_GetAvg(bench_expand_intvar_quick(bench, varname + 3));
					break;

				case 'D':
					*piresult = (int) bench_time_GetStdDev(bench_expand_intvar_quick(bench, varname + 3));
					break;

				case 'm':
					*piresult = bench_time_GetMin(bench_expand_intvar_quick(bench, varname + 3));
					break;

				case 'M':
					*piresult = bench_time_GetMax(bench_expand_intvar_quick(bench, varname + 3));
					break;

				default:
					sprintf(errbuf, "bench_expand_intvar: Following $T: Expected [CADmM] but got %c\n", varname[2]);
					return prog_RES_SYNTAX;
					break;
			}
			break;

		default:
			sprintf(errbuf, "bench_expand_intvar: Following $: %c is invalid\n", varname[1]);
			return prog_RES_SYNTAX;
			break;
	}
	return prog_RES_OK;
}	

/*--------------------------------------------------------------------------
 Expand any variable references at runtime to their string values.
 Can expand any integer reference as a string.
 On entry, varname is a string starting with $, followed by a name
 (S%d or any of the values mentioned in bench_expand_intvar).
 On exit, value of that variable is copied into sresult, and prog_RES_OK
 is returned.
 On error, prog_RES_SYNTAX is returned if variable name can't be parsed,
 and error message is copied to sresult.
--------------------------------------------------------------------------*/
static prog_res_t bench_expand_var(bench_t *bench, const char *varname, char *sresult)
{
	int iresult;
	prog_cmd_res_t res;

	*sresult = 0;

    if (varname[0] != '$')
		return prog_RES_SYNTAX;

	res = prog_RES_OK;

	switch (varname[1])
	{
		case 'S':
			strcpy(sresult, prog_process_GetString(&bench->process, bench_expand_intvar_quick(bench, varname + 2)));
			break;

		default:
			res = bench_expand_intvar(bench, varname, &iresult, sresult);
			if (res == prog_RES_OK)
				sprintf(sresult, "%d", iresult);
			break;
	}
	return res;
}	

/*
 * benchCmd_Print
 *
 * Used so that the program can print out diagnostic information
 * The $ character is reserved for Timer and Variable expansion
 *
 */
prog_cmd_res_t benchCmd_Print(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int 		num;
	char		obuf[2048];
	char		*obufp;
	prog_res_t res;
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);
	
	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Print string, expanding $[IST] keywords\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		puts("Prints given string, expanding keywords\n" \
				"$amHost 0 if not host, 1 if host\n" \
				"$hostId dpid of current host of current session\n" \
				"$myId   dpid of most recently locally created player\n" \
				"$numPlayers  number of players in current session\n" \
				"$uptime Ticks since bench startup\n" \
				"$procid id of current process\n" \
				"$uptime Ticks since bench startup\n" \
				"$D%d (e.g. $D0 or $D15) expands to id of player rank %d\n" \
				"$G%d (e.g. $G0 or $G15) expands to value of global integer variable %d\n" \
				"$I%d (e.g. $I0 or $I15) expands to value of integer variable %d\n" \
				"$R%d  Random integer from 0 to %d-1\n" \
				"$S%d (e.g. $S0 or $S15) expands to value of string variable %d\n" \
				"$TC%d (e.g. $TC0 or $TC1) expands to current value of timer %d\n" \
				"$TA%d (e.g. $TA0 or $TA1) expands to average of past values of timer %d\n" \
				"$TD%d (e.g. $TD0 or $TD1) expands to standard deviation of past values of timer %d\n" \
				"$Tm%d (e.g. $Tm0 or $Tm1) expands to minimum of past values of timer %d\n" \
				"$TM%d (e.g. $TM0 or $TM1) expands to maximum of past values of timer %d\n" \
		);
		return (prog_cmd_RES_OK);
	}

	/* Expand arguments into obuf.  Res ends up OK on success. */
	obufp = obuf;
	res = prog_RES_OK;
	for (num = 0; num < argc; num++) {
		if (argv[num][0] == '$') {
			res = bench_expand_var(bench, argv[num], obufp);
			if (res != prog_RES_OK)
				break;
		} else {
			strcpy(obufp, argv[num]);
		}
		obufp += strlen(obufp);
		if (num < argc) 
			*obufp++ = ' ';
		assert(obufp - obuf < sizeof(obuf));
	}
	*obufp++ = '\n';
	*obufp++ = 0;

	fputs(obuf, stdout);
   	DPRINT(("%s\n", obuf));
	if (res != prog_RES_OK)
		return prog_cmd_RES_ERROR;
	return prog_cmd_RES_CONTINUE;
}

/*
 * benchCmd_Eval
 *
 * command "Eval"
 * parameters "INFO_NAME VARIABLE_NUMBER" 
 * 
 * example Eval $amHost 0
 *
 */
prog_cmd_res_t benchCmd_Eval(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	char 		*infoName;
	int         varNum;
	prog_res_t res;
	int			iresult;
	char		errbuf[256];

	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Eval $infoname varnumber\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Copies current value of named variable into the given integer variable.\n" \
				"Same variables parsed as by Print command.\n" \
		);
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "Eval %s\n", params);

	if (argc != 2)
	{
		printf("Expecting 2 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	infoName = argv[0];
	varNum = bench_expand_intvar_quick(bench, argv[1]);

	res = bench_expand_intvar(bench, infoName, &iresult, errbuf);

	if (res == prog_RES_OK) {
		prog_process_SetInteger(&bench->process, varNum, iresult);
	} else {
		printf("Eval: Unknown Argument: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}
	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_SetGlobal
 *
 * command "SetGlobal"
 * parameters "GLOBALNUMBER" "VALUE"
 * example SetGlobal 0 10
 * example SetGlobal 0 $I3
 *
 */
prog_cmd_res_t benchCmd_SetGlobal(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int 		gnum;
	prog_cmd_res_t res;
	char		errbuf[256];
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("SETGLOBAL globalnum value\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("There are %d global variables available.\n" \
				"The first arg is the global to set;\n", 
				"The second arg is the value to set it to; see Print for a list of supported values.\n", 
				"Example: SETGLOBAL 0 1 : sets global #0 to 1\n", 
				"Example: SETGLOBAL 0 $hostId : sets global #0 to value of $hostid\n", 
				bench_NUM_GLOBALS);
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "SetGlobal %s\n", params);

	if (argc != 2)
	{
		printf("Expecting 2 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the number of the global */
	res = bench_expand_intvar(bench, argv[0], &gnum, errbuf);
	if (res != prog_RES_OK) {
		printf("SetGlobal: Unknown Argument in global number: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}
	if (gnum >= bench_NUM_GLOBALS || gnum < 0)
	{
		printf("Invalid Global %d!\n", gnum);
		return (prog_cmd_RES_ERROR);
	}

	/* Set the indicated global to the indicated value */
	res = bench_expand_intvar(bench, argv[1], &bench_globals[gnum], errbuf);
	if (res != prog_RES_OK) {
		printf("SetGlobal: Unknown Argument in global value: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}
	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_ReadLineIntoGlobals
 *
 * command "ReadLineIntoGlobals"
 * parameters "GLOBALNUMBER"
 * example ReadLineIntoGlobals 0 
 *
 */
prog_cmd_res_t benchCmd_ReadLineIntoGlobals(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int 		gnum;
	prog_cmd_res_t res;
	char		errbuf[256];
	char		linebuf[256];
	char		*tok;
	int         i;
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("ReadLineIntoGlobals globalnum\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Reads a line from stdin\n" \
				"The first arg is the first global to set.\n", 
				"Example: ReadLineIntoGlobals 0 : sets global #0 and up to the values on the next line of stdin\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "ReadLineIntoGlobals %s\n", params);

	if (argc != 1)
	{
		printf("Expecting 1 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the number of the global */
	res = bench_expand_intvar(bench, argv[0], &gnum, errbuf);
	if (res != prog_RES_OK) {
		printf("ReadLineIntoGlobals: Unknown Argument in global number: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}
	if (gnum >= bench_NUM_GLOBALS || gnum < 0)
	{
		printf("Invalid Global %d!\n", gnum);
		return (prog_cmd_RES_ERROR);
	}

	/* Read line from stdin, parse as integers */
	if (!fgets(linebuf, sizeof(linebuf), stdin)) {
		printf("ReadLineIntoGlobals: end of file\n");
		return (prog_cmd_RES_ERROR);
	}
	for (i=0, tok=strtok(linebuf, " \t"); tok; tok=strtok(NULL, " \t"), i++) {
		bench_globals[gnum + i] = atoi(tok);
	}
	bench_print(bench, 
			"ReadLineIntoGlobals: read %d values into globals [%d..%d]\n",
			i, gnum, gnum+i-1);

	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_LoseHost
 *
 * Provoke an election by losing our connection to the host.
 *
 */
prog_cmd_res_t benchCmd_LoseHost(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	dp_result_t err;

	err = dpio_closeHdl(bench->dp->dpio, bench->dp->hMaster);
	return prog_cmd_RES_CONTINUE;
}

/*
 * benchCmd_LosePeerWithRank
 *
 * Provoke an error condition by closing our dpio connection to a player
 * whose dpid has the given rank
 *
 * command "LosePeerWithRank"
 * parameters "INTEGER OR INTVAR"
 * example LosePeerWithRank 0 
 * example LosePeerWithRank $I1
 */
prog_cmd_res_t benchCmd_LosePeerWithRank(prog_process_t *process, const char *params, void *context)
{
	bench_t		*bench = (bench_t *) context;
	dp_result_t err;
	char		buff[PROG_MAX_PARAM_LENGTH];
	char		errbuf[256];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int			rank;
	playerHdl_t h;
	dpid_t		id;
	prog_cmd_res_t res;

	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("LosePeerWithRank Integer\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Closes dpio handle to peer whose dpid has the given rank\n");
		return (prog_cmd_RES_OK);
	}

	if (argc != 1)
	{
		printf("Expecting 1 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the rank */
	res = bench_expand_intvar(bench, argv[0], &rank, errbuf);
	if (res != prog_RES_OK) {
		printf("LosePeerWithRank: Unknown Argument: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}

	id = bench_compare_array[rank]->id;
	h = dpid2commHdl(bench->dp, id);
	err = dpio_closeHdl(bench->dp->dpio, h);
	bench_print(bench, "LosePeerWithRank %s: rank:%d, id:%d, h:%x, closeHdl returns %d\n", params, rank, id, h, err);

	return prog_cmd_RES_CONTINUE;
}

/*
 * benchCmd_Login
 *
 * command "Login"
 * parameters "USERNAME" "PASSWORD"
 * example Login user1 pass1
 * example Login $S1 $S2
 *
 */
prog_cmd_res_t benchCmd_Login(prog_process_t *process, const char *params, void *context)
{
	dp_result_t err;
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	char		username[PROG_MAX_STRINGLEN];
	char		password[PROG_MAX_STRINGLEN];
	prog_cmd_res_t res;
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("Login username password\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Begin logging in a player with username and password.\n",
				"Example: LOGIN $S1 $S2 : Login player $S1 $S2\n");
		return (prog_cmd_RES_OK);
	}

	if (argc != 2)
	{
		printf("Expecting 2 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the username and password */
	if (argv[0][0] == '$') {
		res = bench_expand_var(bench, argv[0], username);
		if (res != prog_RES_OK) {
			printf("ReportScore: Bad stringvar for username: %s\n", argv[0]);
			return (prog_cmd_RES_ERROR);
		}
	} else
		strncpy(username, argv[0], 16);
	username[16] = '\0';
	if (argv[1][0] == '$') {
		res = bench_expand_var(bench, argv[1], password);
		if (res != prog_RES_OK) {
			printf("ReportScore: Bad stringvar for password: %s\n", argv[1]);
			return (prog_cmd_RES_ERROR);
		}
	} else 
		strncpy(password, argv[1], 16);
	password[16] = '\0';

	bench_print(bench, "Login %s %s\n", username, password);

	err = dpAccountLoginA(bench->dp, username, password);
	if (err != dp_RES_OK) {
		printf("Login: Error requesting login: %d\n", err);
		return (prog_cmd_RES_ERROR);
	}

	/* Wait for the server's response */
	bench->bitflag |= bench_BITFLAG_WAIT_LOGIN;
	bench->bitflag &= ~bench_BITFLAG_ERROR;
	bench->flag = bench_flag_WAIT;
	
	return (prog_cmd_RES_CONTINUE);
}

/*
 * benchCmd_ReportScore
 *
 * command "ReportScore"
 * parameters "ID" "SCORE"
 * example ReportScore 40 10
 * example ReportScore $D1 $G1
 *
 */
prog_cmd_res_t benchCmd_ReportScore(prog_process_t *process, const char *params, void *context)
{
	dp_result_t err;
	bench_t		*bench = (bench_t *) context;
	char		buff[PROG_MAX_PARAM_LENGTH];
	int 		argc;
	char	 	*argv[PROG_MAX_PARAM];
	int			id;
	int			score;
	char		scorebuf[3];
	prog_cmd_res_t res;
	char		errbuf[256];
	
	memcpy(buff, params, PROG_MAX_PARAM_LENGTH);
	prog_Char2Args(buff, &argc, argv);

	if (PROG_CMD_TEST_PARAM(params))
	{
		printf("ReportScore id score\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO(params))
	{
		printf("Report a score for player id to the dp network layer.\n",
				"A player must be logged in to report a score successfully.\n",
				"Example: REPORTSCORE $D1 $G1 : Report that player $D1\n",
				"has the score $G1 now\n");
		return (prog_cmd_RES_OK);
	}

	bench_print(bench, "ReportScore %s\n", params);

	if (argc != 2)
	{
		printf("Expecting 2 arguments, found %d\n", argc);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the id */
	res = bench_expand_intvar(bench, argv[0], &id, errbuf);
	if (res != prog_RES_OK) {
		printf("ReportScore: Unknown Argument in id: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}

	/* Get the score */
	res = bench_expand_intvar(bench, argv[1], &score, errbuf);
	if (res != prog_RES_OK) {
		printf("ReportScore: Unknown Argument in score: %s\n", errbuf);
		return (prog_cmd_RES_ERROR);
	}

	/* Report the score */
	scorebuf[0] = dpGETSHORT_FIRSTBYTE((short)score);
	scorebuf[1] = dpGETSHORT_SECONDBYTE((short)score);
	scorebuf[2] = 0;
	err = dpReportScoreBuf(bench->dp, id, 0, scorebuf, 3);
	if (err != dp_RES_OK) {
		printf("ReportScore: Invalid id: %d.\n", id);
		return (prog_cmd_RES_ERROR);
	}
	
	return (prog_cmd_RES_CONTINUE);
}

/*
 * main
 *
 */
int main(int argc, char **argv)
{
	bench_t			*bench;
	prog_syntax_t	syntax;
	prog_t			program;
	int				i;
	int				bail;
	unsigned long	endtime;
	bench_t	*pptr;
   
	srand(eclock());
	bench_starttime = eclock();
	
   	bench_stats.rx_pkts = 0;
   	bench_stats.rx_bytes = 0;
   	bench_stats.rx_pkts = 0;
   	bench_stats.rx_bytes = 0;
   
	if (argc < 3)
	{
		printf("useage - bench \"program\" maxexec [parameters]\n");
		printf(" program is the .pr file to be executed\n");
		printf(" maxexec is the maximum amount of time between executions (ms)\n");
		return (1);
	}
	
	bench_maxexec = (atoi(argv[2]) * ECLOCKS_PER_SEC) / 1000;

	prog_syntax_Create(&syntax);
	
	prog_syntax_AddCmd(&syntax, "DPRINT", benchCmd_DPRINT);
	prog_syntax_AddCmd(&syntax, "Verbosity", benchCmd_Verbosity);
   	prog_syntax_AddCmd(&syntax, "dpCreate", benchCmd_dpCreate);
	prog_syntax_AddCmd(&syntax, "dpDestroy", benchCmd_dpDestroy);
	prog_syntax_AddCmd(&syntax, "dpSetGameServer", benchCmd_dpSetGameServer);
	prog_syntax_AddCmd(&syntax, "dpOpen", benchCmd_dpOpen);
	prog_syntax_AddCmd(&syntax, "dpClose", benchCmd_dpClose);
	prog_syntax_AddCmd(&syntax, "dpCreatePlayer", benchCmd_dpCreatePlayer);
	prog_syntax_AddCmd(&syntax, "dpDestroyPlayer", benchCmd_dpDestroyPlayer);
	prog_syntax_AddCmd(&syntax, "dpSend", benchCmd_dpSend);
	prog_syntax_AddCmd(&syntax, "dpReceive", benchCmd_dpReceive);
	prog_syntax_AddCmd(&syntax, "dpGetStats", benchCmd_dpGetStats);
	prog_syntax_AddCmd(&syntax, "Wait", benchCmd_Wait);
	prog_syntax_AddCmd(&syntax, "Timer", benchCmd_Timer);
	prog_syntax_AddCmd(&syntax, "SetGlobal", benchCmd_SetGlobal);
	prog_syntax_AddCmd(&syntax, "ReadLineIntoGlobals", benchCmd_ReadLineIntoGlobals);
	prog_syntax_AddCmd(&syntax, "Spawn", benchCmd_Spawn);
	prog_syntax_AddCmd(&syntax, "Print", benchCmd_Print);
	prog_syntax_AddCmd(&syntax, "LoseHost", benchCmd_LoseHost);
	prog_syntax_AddCmd(&syntax, "LosePeerWithRank", benchCmd_LosePeerWithRank);
	prog_syntax_AddCmd(&syntax, "Eval", benchCmd_Eval);
	prog_syntax_AddCmd(&syntax, "Login", benchCmd_Login);
	prog_syntax_AddCmd(&syntax, "ReportScore", benchCmd_ReportScore);
	
	switch (prog_Load(&program, &syntax, argv[1], argc - 2, argv + 2))
	{
		case prog_RES_OK:
			break;

		case prog_RES_SYNTAX:
			printf("SYNTAX error in %s\n", argv[1]);
			return (1);
			break;

		case prog_RES_FILE:
			printf("FILE error in %s\n", argv[1]);
			return (1);
			break;

		case prog_RES_MEM:
			printf("MEM error in %s\n", argv[1]);
			return (1);
			break;

		case prog_RES_ERROR:
			printf("ERROR in %s\n", argv[1]);
			return (1);
			break;
	}
	
	DPRINT(("Bench: Start of main loop (initializing logging)\n"));
	dp_flushLog();

	memset((char *)bench_globals, 0, sizeof(bench_globals));

	for (i = 0; i < bench_NUM_TIMERS; i++)
	{
		bench_time_Reset(i);
	}
	
	bench = bench_process_Create(&program, NULL);
	
	if (!bench)
	{
		printf("Could not create bench\n");
		return (1);
	}
	
	bench_process_Start(bench);
	
	bail = 0;

	while (!bail)
	{
		unsigned long timer;
	   	unsigned long timer2;

		timer = eclock();

		switch (bench_process_Step())
		{
			case prog_RES_OK:
				break;
			case prog_RES_EXIT:
			case prog_RES_ERROR:
			case prog_RES_END:
				bail = 1;
				break;
		}

	   	timer2 = eclock();
	   
//		bench_time_Sample(4, (timer2 - timer) * 1000 / ECLOCKS_PER_SEC);		
	/*	
		if ((timer2 - timer) > (prog_process_GetInteger(&bench->process, 9) * ECLOCKS_PER_SEC / 1000))
		{
			printf("FATAL: Step timer exceeds allowed value\n");
			printf("FATAL: Time %d, Limit %d\n", (timer2 - timer) * 1000 / ECLOCKS_PER_SEC, prog_process_GetInteger(&bench->process, 9));
			bail = 1;
			break;
		}
		
		if ((timer2 - timer) > (prog_process_GetInteger(&bench->process, 8) * ECLOCKS_PER_SEC / 1000))
		{
			printf("WARN: Step timer exceeds allowed value\n");
			printf("WARN: Time %d, Limit %d\n", (timer2 - timer) * 1000 / ECLOCKS_PER_SEC, prog_process_GetInteger(&bench->process, 8));
		} */
	}

	/* Flush log files. */
	for (pptr=bench_process_list; pptr; pptr=pptr->next) {
		if (pptr->verbosity > bench_VERBOSITY_NONE) {
			dp_setLogFP(pptr->logfp);
			dp_flushLog();
		}
	}

   	endtime = eclock();
  /* 
   	printf("Global Statistics:\n");
   	printf("Average Connect Time:             : %f\n", bench_time_GetAvg(2));
	printf("Success Rate:                     : %d [%d]\n", bench_time_GetNumber(5), bench_time_GetNumber(1));
   	printf("Timer Resolution:                 : %d\n", ECLOCKS_PER_SEC);
   	printf("Start Time                        : %d\n", bench_starttime);
   	printf("End Time                          : %d\n", endtime);
   	printf("Elapsed Time                      : %d\n", endtime - bench_starttime);
   	printf("Packets Received                  : %d\n", bench_stats.rx_pkts);
   	printf("Bytes Received                    : %d\n", bench_stats.rx_bytes);
	printf("Total Received (pkts*32+bytes)    : %d\n", bench_stats.rx_pkts * 32 + bench_stats.rx_bytes);
   	printf("Bits Received per second          : %d\n", (bench_stats.rx_pkts * 32 + bench_stats.rx_bytes) * ECLOCKS_PER_SEC * 10 / (endtime - bench_starttime)); 
   	printf("Packets Transmitted               : %d\n", bench_stats.tx_pkts);
   	printf("Bytes Transmitted                 : %d\n", bench_stats.tx_bytes);
	printf("Total Transmitted (pkts*32+bytes) : %d\n", bench_stats.tx_pkts * 32 + bench_stats.tx_bytes);
   	printf("Bits Transmitted per second       : %d\n", (bench_stats.tx_pkts * 32 + bench_stats.tx_bytes) * ECLOCKS_PER_SEC * 10 / (endtime - bench_starttime)); 
  */ 
  // 	printf(" - Unloading program\n");
	prog_UnLoad(&program);
 //	printf(" - Destroying syntax\n");
	prog_syntax_Destroy(&syntax);

	return (bench_exitcode);
}



/*
 * bench_time_Reset
 *
 * Resets a benchmark timer
 *
 */
void bench_time_Reset(int timer)
{
    bench_timers[timer].on = 1;
	bench_timers[timer].count = 0;
	bench_timers[timer].sum = 0;
	bench_timers[timer].sumsq = 0;
	bench_timers[timer].min = 0xFFFFFFFF;
	bench_timers[timer].max = 0;
};


/*
 * bench_time_Sample
 *
 * Adds a sample to the timer
 *
 */
void bench_time_Sample(int timer, unsigned long sample)
{
    if (bench_timers[timer].on)
	{
		bench_timers[timer].count++;
		bench_timers[timer].sum += sample;
		bench_timers[timer].sumsq += sample * sample;
		bench_timers[timer].min = (bench_timers[timer].min < sample) ? bench_timers[timer].min : sample;
		bench_timers[timer].max = (bench_timers[timer].max > sample) ? bench_timers[timer].max : sample;
	}
}


/*
 * bench_time_Report
 *
 * Writes a report about a timer
 *
 */
void bench_time_Report(int timer)
{
	printf("Timer %d\n", timer);
	printf("Samples: %d Sum: %d: SumSq: %d Min: %d Max: %d\n", bench_timers[timer].count, bench_timers[timer].sum, bench_timers[timer].sumsq, bench_timers[timer].min, bench_timers[timer].max);
	if (bench_timers[timer].count > 0)
	{
		double avg = ((double) bench_timers[timer].sum) / (double) bench_timers[timer].count;
		double avgsq = ((double) bench_timers[timer].sumsq) / (double) bench_timers[timer].count;
		printf("Avg:        %f\n", avg);
		printf("StdDev:     %f\n", sqrt(avgsq - avg * avg));
	}
	else
	{
		printf("No samples, cannot calculate average or stdev\n");
	}
	printf("\n");
}

/*
 * bench_time_GetNumber
 * 
 */
int bench_time_GetNumber(int timer)
{
	return (bench_timers[timer].count);
}
	
/*
 * bench_time_GetAvg
 *
 */
double bench_time_GetAvg(int timer)
{
	return (((double) bench_timers[timer].sum) / (double) bench_timers[timer].count);
}
	
/*
 * bench_time_GetStdDev
 * 
 */
double bench_time_GetStdDev(int timer)
{
	double avg = ((double) bench_timers[timer].sum) / (double) bench_timers[timer].count;
	double avgsq = ((double) bench_timers[timer].sumsq) / (double) bench_timers[timer].count;
	return (sqrt(avgsq - avg * avg));
}


/*
 * bench_time_GetMax
 * 
 */
static int bench_time_GetMax(int timer)
{
	return (bench_timers[timer].max);
}


/*
 * bench_time_GetMin
 * 
 */
static int bench_time_GetMin(int timer)
{
	return (bench_timers[timer].min);
}


/*
 * bench_cb_session_join
 * 
 */
int dp_PASCAL bench_cb_session_join(dp_session_t *session, long *timeout, long flags, void *context)
{
	bench_t *bench = (bench_t *) context;

	if (session)
	{
		/* We got in, save the session and clear the wait flag */
		bench_print(bench, "Success joining session %s\n", session->sessionName);
		bench->session = *session;
		bench->bitflag &= ~(bench_BITFLAG_WAIT_OPEN | bench_BITFLAG_ERROR);
		bench->flag = bench_flag_OK;
	}
	else
	{
		/* We failed to get in :( */
		bench_print(bench, "Failure joining session\n");
		bench->bitflag &= ~bench_BITFLAG_WAIT_OPEN;
		bench->bitflag |= bench_BITFLAG_ERROR;
		bench->flag = bench_flag_ERROR;
		bench->amHost = FALSE;
	}

	return (TRUE);
}

/*
 * bench_cb_player_create
 *
 */
void dp_PASCAL bench_cb_player_create(dpid_t id, char_t *name, long flags, void *context)
{
	bench_t *bench = (bench_t *) context;

	if (id != dp_ID_NONE)
	{
		/* We created a player, save the id */
		bench_print(bench, "Success creating player %s [%d]\n", name, id);
		bench_set_id(bench, id);
		bench->bitflag &= ~(bench_BITFLAG_WAIT_PLAYER | bench_BITFLAG_ERROR);
		bench->flag = bench_flag_OK;
	}
	else
	{
		/* We failed to create the player */
		bench_print(bench, "Failure creating player\n");
		bench->bitflag &= ~bench_BITFLAG_WAIT_PLAYER;
		bench->bitflag |= bench_BITFLAG_ERROR;
		bench->flag = bench_flag_ERROR;
	}
}


/*
 * bench_process_Create
 * 
 * Creates a bench_t which 
 * 
 * in  -> program: program to use
 *        start: starting location in the program
 * out <- pointer to initialized bench structure
 */
static bench_t *bench_process_Create(prog_t *program, prog_line_t *start)
{
	bench_t *bench;
	char logfname[128];
	
	bench = (bench_t *) malloc(sizeof (bench_t));

	if (!bench)
	{
		return (NULL);
	}
	memset(bench, 0, sizeof(bench_t));
	
	bench->number = bench_process_id++;
	bench->dp = NULL;
   	bench->verbosity = 0;
	sprintf(logfname, "bench%d.log", bench->number);
	dp_setLogFP(NULL);
	dp_setLogFname(logfname);
	DPRINT(("bench_process_Create: logging started.\n"));
	bench->logfp = dp_getLogFP();
	DPRINT(("bench_process_Create: logfp=%p.\n", bench->logfp));
	prog_process_Init(&bench->process, program, start);

    return (bench);
}



/*
 * bench_process_Start
 *
 * Starts a process, effectively adds it to the list of running processes
 *
 * in  -> process: process to be started
 * out <- prog_RES_OK: process started normally
 *        prog_RES_ERROR: process could not be started
 *
 */
prog_res_t bench_process_Start(bench_t *bench)
{
	ASSERT(bench);

	/*
	 * Add the process to the list
	 */
	bench->next = bench_process_list;
	bench_process_list = bench;
	bench_process_number++;

   	if (bench->verbosity >= bench_VERBOSITY_NORMAL)
    {
		printf("Adding process to process list\n");
	}
	return (prog_RES_OK);
}


/*
 * bench_process_Stop
 *
 * Stops a process, removes it from the list of running processes
 *
 * in  -> bench: process to be stoped
 * out <- prog_RES_OK: process stopped normally
 *        prog_RES_ERROR: process could not be stopped
 */
prog_res_t bench_process_Stop(bench_t *bench)
{
	bench_t	*pptr;
	bench_t	*prev;

	ASSERT(bench);

   	if (bench->verbosity >= bench_VERBOSITY_NORMAL)
    {
		bench_print(bench, "Removing process from process list\n");
	}
	pptr = bench_process_list;
	prev = NULL;

	while (pptr)
	{
		if (bench == pptr)
		{
			if (!prev)
			{
				bench_process_list = pptr->next;
			}
			else
			{
				prev->next = pptr->next;
			}
			free (bench);
			bench_process_number--;
   			return (prog_RES_OK);
		}

		prev = pptr;
		pptr = pptr->next;
	}
	return (prog_RES_ERROR);
}


/*
 * bench_ProcessStep
 *
 * Steps through all of the processes which are currently running
 *
 * out <- prog_RES_OK: program executed successfully
 *        prog_RES_ERROR: error occured during execution
 */
prog_res_t bench_process_Step()
{
	bench_t	*pptr;
	bench_t	*next;

#ifdef UNIX
	static unsigned long lasttime = 0;
	long delta_t;

	if (!lasttime)
		lasttime = eclock();
	delta_t = (long)(eclock()-lasttime);
	if (delta_t < bench_maxexec)
	{
		fd_set 			fds;
		struct timeval	timeout;
		int				n;
		int				max = 0;
	
		pptr = bench_process_list;
	
		/* 
		 * For UNIX we "select" the file descriptors of all the sockets
		 * to see which ones require processing.  Only those processes
		 * which are waiting are considered.
		 */
	
		FD_ZERO(&fds);
	
		while (pptr)
		{
			if (pptr->dp && pptr->flag & bench_flag_WAIT)
			{
				if (pptr->filedesc >= max)
				{
					max = pptr->filedesc + 1;
				}
				
//				printf("Waiting on filedesc %d\n", pptr->filedesc);
				
				FD_SET(pptr->filedesc, &fds);
			}
			pptr = pptr->next;
		}
	
		timeout.tv_usec = 2000;
		timeout.tv_sec = 0;
		
		n = select(max, &fds, NULL, NULL, &timeout);
		
		if (n)
		{
			/*
			 * Something happened, proceed through the file descriptors 
			 * to see which ones have pending IO and then step them
			 */
			pptr = bench_process_list;
			
			while (pptr)
			{
				next = pptr->next;
				
				if (pptr->dp && FD_ISSET(pptr->filedesc, &fds))
				{
				   	if (pptr->verbosity >= bench_VERBOSITY_LOTS)
					{
						bench_print(pptr, "Data pending (filedesc %d)\n", pptr->filedesc);
					}

					if (pptr->verbosity > bench_VERBOSITY_NONE)
					{
						dp_setLogFP(pptr->logfp);
					}
					else
					{
						dp_setLogFP(dp_LOG_FILE_NONE);
					}

					switch (prog_process_Step(&pptr->process, pptr))
					{
						default:
						case prog_RES_OK:
							break;
		
						case prog_RES_END:
							bench_process_Stop(pptr);
							break;
		
						case prog_RES_ERROR:
							return (prog_RES_ERROR);
							break;
				
						case prog_RES_EXIT:
							return (prog_RES_EXIT);
							break;
					}
				}
				pptr = next;
			}
		}
		return(prog_RES_OK);
	}

	lasttime = eclock();
	
#endif

	pptr = bench_process_list;

	while (pptr)
	{
		next = pptr->next;
	
		if (pptr->verbosity > bench_VERBOSITY_NONE)
		{
			dp_setLogFP(pptr->logfp);
		}
		else
		{
			dp_setLogFP(dp_LOG_FILE_NONE);
		}
		
		switch (prog_process_Step(&pptr->process, pptr))
		{
			default:
			case prog_RES_OK:
				break;

			case prog_RES_END:
				bench_process_Stop(pptr);
				break;

			case prog_RES_ERROR:
				bench_process_Stop(pptr);
				return (prog_RES_ERROR);
				break;
			
			case prog_RES_EXIT:
				bench_exitcode = prog_process_GetExitCode(&pptr->process);
				bench_process_Stop(pptr);
				return (prog_RES_EXIT);
				break;
		}
		pptr = next;
	}

	return (prog_RES_OK);
}
