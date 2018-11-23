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
 Program to launch Activenet games from the commandline.
 Creates or locates and joins an Activenet session, then
 creates a player, and launches the given command.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <time.h>
#include "cancelbox.h"
#include "dp2.h"
#include "launchparams.h"

#define MSG_CONNECTING "Connecting %d"
#define MSG_LAUNCHING  "Launching"
#define MSG_ERR_LAUNCH "Error %d launching game at line %d"
#define MSG_WIND_TITLE "Game Launcher"
#define MSG_ERROR_TITLE "Error"

int autoclick;  /* auto click on the message or dialog box */

/* Error injection scheme.  Used during testing only */
int err_line_number; /* set with -e=err_line_number */
#define FORCE_ERR (err_line_number == __LINE__)

/*--------------------------------------------------------------------------
 Generic startup code.
--------------------------------------------------------------------------*/

int my_MessageBox(
        LPCSTR lpText,
        LPCSTR lpCaption,
        UINT uType
        )
{
        static int (APIENTRY *pfnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT) = NULL;
        static HWND (APIENTRY *pfnGetActiveWindow)(void) = NULL;
        static HWND (APIENTRY *pfnGetLastActivePopup)(HWND) = NULL;

        HWND hWndParent = NULL;

        if (NULL == pfnMessageBoxA)
        {
            HANDLE hlib = LoadLibrary("user32.dll");

            if (NULL == hlib || NULL == (pfnMessageBoxA =
                        (int (APIENTRY *)(HWND, LPCSTR, LPCSTR, UINT))
                        GetProcAddress(hlib, "MessageBoxA")))
                return 0;

            pfnGetActiveWindow = (HWND (APIENTRY *)(void))
                        GetProcAddress(hlib, "GetActiveWindow");

            pfnGetLastActivePopup = (HWND (APIENTRY *)(HWND))
                        GetProcAddress(hlib, "GetLastActivePopup");
        }

        if (pfnGetActiveWindow)
            hWndParent = (*pfnGetActiveWindow)();

        if (hWndParent != NULL && pfnGetLastActivePopup)
            hWndParent = (*pfnGetLastActivePopup)(hWndParent);

        return (*pfnMessageBoxA)(hWndParent, lpText, lpCaption, uType);
}

#ifdef DEBUG
#define LPRINT(s) write_log s
FILE *flog = NULL;
static void write_log(char *fmt, ...) {
	if (!flog) {
		char logfile[MAX_PATH];
		sprintf(logfile, "%x", time(NULL));
		strcat(logfile, ".log");
		flog = fopen(logfile, "w");
	}
	if (flog) {
		va_list args;
		va_start(args, fmt);
		vfprintf(flog, fmt, args);
		va_end(args);
		fflush(flog);
	}
}
#else
#define LPRINT(s)
#endif

#define PRINT(s) my_print s
static void my_print(char *fmt, ...) {
	char buf[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	if (!autoclick)
		my_MessageBox(buf, MSG_ERROR_TITLE, 0);
#ifdef DEBUG
	write_log("%s", buf);
#endif
	va_end(args);
}

#define MAXARGS 10
#define DQUOTECHAR '"'
#define SPACECHAR (char)0x20
#define TABCHAR   (char)0x08

/*-------------------------------------------------------------------------
 Parse commandline into words; double quotes protect words with spaces.
--------------------------------------------------------------------------*/
static int my_setargv(char *lpszCmdLine, char **argv, int maxargv)
{
	char *p;
	char *argcopy;
	int argi;

	/* Copy arguments so we can write over them with nulls. */
	argcopy = malloc(strlen(lpszCmdLine)+1);
	if (!argcopy || FORCE_ERR) {
		/*PRINT(("Out of memory."));*/
		PRINT((MSG_ERR_LAUNCH , dp_RES_NOMEM, __LINE__));
		return -1;
	}
	strcpy(argcopy, lpszCmdLine);
	p = argcopy;

	for (argi=0; (argi<maxargv) && *p; ) {
		/* Skip preceding whitespace. */
		while (*p && ((*p == SPACECHAR) || (*p == TABCHAR)))
			p++;
		if (!p) break;

		/* Handle quoted argument. */
		if ( *p == DQUOTECHAR ) {
			/* skip leading quote, remember start of argument. */
			p++;
			argv[argi++] = p;

			/* Eat until trailing quote */
			while ( *p && (*p != DQUOTECHAR))
				p++;
			if (!p) break;

			/* Terminate arg, skip trailing quote */
			*p++ = 0;

		} else {
			argv[argi++] = p;
			while (*p && (*p != SPACECHAR) && (*p != TABCHAR))
				p++;
			if (!*p) break;

			/* Terminate arg */
			*p++ = 0;
#if 0
			/*
 Double-dash by itself terminates parsing; it is inserted as a word,
 but rest of commandline is inserted as a single word.
			 */
			/* Check for special double-dash case */
			if (!strcmp(argv[argi-1], "--")) {
				/* Insert rest of commandline as next arg. */
				argv[argi++] = p;
				break;
			}
#endif
		}
	}

	return argi;
}

int main(int argc, char **argv);

static LONG __cdecl Debug_ExceptionFilter(LPEXCEPTION_POINTERS ep)
{
	dpReportCrash(ep);
	return (EXCEPTION_CONTINUE_SEARCH);
}

int WINAPI WinMain(HINSTANCE hinstExe, HINSTANCE hinstExePrev, LPSTR lpszCmdLine, int nCmdShow)
{
	char *argv[MAXARGS];

	LPRINT(("Hello.  Cmdline '%s'\n", GetCommandLine()));

	__try {
		return main(my_setargv(GetCommandLine(), argv, MAXARGS), argv);
	} __except(Debug_ExceptionFilter(GetExceptionInformation())) {
		;
	}
}

/*--------------------------------------------------------------------------
 Specific application code goes below here.
--------------------------------------------------------------------------*/

static void usage()
{
	static char *szUsage = "anetdrop: read anet.inf and launch an Activenet game.\n\
 Options:\n\
-n=path            	# which transport to use, e.g. -n=anet/winets2.dll\n\
-h                 	# host a session\n\
-m=maxplayers      	# Max number of players in session\n\
-j                 	# join a session\n\
-q=16              	# join any internet session (16=lobby, 0=game)\n\
-l=sessionID       	# sessionID\n\
-s=mygame          	# session name\n\
-p=myname          	# player name\n\
-a=206.43.1.0      	# address (of host; used when joining if no broadcast)\n\
-g=server_name  	# Name of the server to connect to \n\
-u=username        	# username for login\n\
-z=passwd          	# password for login\n\
-v               	# Allow player variables\n\
-w                 	# Don't exit until command does; return its exit code.\n\
-x                 	# Let launched program init comm system\n\
-y                 	# Use freeze.anetSSSS, SSSS=sessionType, append to cmdline\n\
-b=9600            	# Baud rate\n\
-c=0               	# COM port to use (0=COM1)\n\
-f=5555555        	# Phone number to dial when using modem, omit to answer\n\
-i=ATZ             	# Modem initialization string\n\
-d=1               	# Dial method (1=Tone, 0=Pulse)\n\
-o=206.41.1.1;206.41.1.2 # open connections to the list of addresses\n\
-o=@ADDRESSFILE          # open connections to the address in the file ADDRESSFILE\n\
-k                 	# Disables the dialog boxes\n\
\n\
 Example:\n\
   anetdrop -n=anet/winets2.dll -h \"-s=spam game\" \"-p=spam man\"\n\
   anetdrop -n=anet/winets2.dll -j -a=200.123.2.1 \"-p=jello man\"\n\
";
	PRINT((szUsage));
	exit(1);
}

int dp_PASCAL myCommThaw_cb(int status, void *context)
{
	static int status_old = -1;

	DPRINT(("myCommThaw_cb: status %d\n", status));

	if (status != status_old) {
		char temp[30];

		status_old = status;
		sprintf(temp, MSG_CONNECTING, status);
		cancelbox_set(MSG_WIND_TITLE, temp);
	}
	return !cancelbox_poll();
}

int main(int argc, char **argv)
{
	int argi;
	dp_t *dp = NULL;
	dp_result_t err;
	dp_appParam_t app;
	dp_launchParams_t params;
	int let_game_init_comm = FALSE;
	int use_new_filename = FALSE;
	char fname[MAX_PATH];
	char parambuf[16000];
	int parambuflen = 0;

	char	GameName[MAX_PATH]="";
	char	GamePath[MAX_PATH]="";
	char	GameArgs[MAX_PATH]="";
	char	GameCwd[MAX_PATH]="";
	char	GameShellOpts[MAX_PATH]="";

	FILE *fp;
	dpLaunchApp_result_t processinfo;

	app.name = GameName;
	app.path = GamePath;
	app.args = GameArgs;
	app.cwd = GameCwd;
	app.shellOpts = GameShellOpts;

	memset(&params, 0, sizeof(params));
	err_line_number = 0;
	autoclick = 0;

	for (argi=1; argi<argc; argi++) {
		int c = argv[argi][0];
		int c1 = argv[argi][1];
		int c2 = argv[argi][2];
		char *arg;

		LPRINT(("argv[%d] = %s\n", argi, argv[argi]));
		if (c != '-') continue;

		/* Parse argument, if present */
		/* Allow either = or : for argument separator */
		arg = NULL;
		if ((c2 == '=') || (c2 == ':'))
			arg = argv[argi]+3;

		switch (c1) {
		case 'x': let_game_init_comm=TRUE; break;
		case 'y': use_new_filename=TRUE; break;
		case 'e': err_line_number = atoi(arg); break;
		case 'k': autoclick = 1; break;

		default:
			err = dp_launchParams_parse(&params, argv[argi]+1);
			if (dp_RES_OK != err) {
				PRINT(("bad argument '%s'\n", argv[argi]));
				usage();
			}
			parambuflen += sprintf(parambuf+parambuflen, "%s\n", argv[argi] + 1);
		}
	}

	/* Got arguments. */
	LPRINT(("Before check: params.Driver=%s, params.Sessname=%s, params.Playname=%s, params.Adr=%s, params.Host=%d, params.Join=%d, params.Wait=%d, params.Maxplayers=%d, params.GameServer=%s params.Username=%s params.Password=%s\n", params.Driver, params.Sessname, params.Playname, params.Adr, params.Host, params.Join, params.Wait, params.Maxplayers, params.GameServer, params.Username, params.Password));

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
	if (params.Host
	&&  ((params.Maxplayers < 2) || (params.Maxplayers > dp_MAXREALPLAYERS))) {
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
		if (params.Host)
			strcpy(params.Playname, "host");
		else
			strcpy(params.Playname, "player");
	}

    params.commInitReq.sessionId = rand() ^ (rand() << 16) ^ time(0) ^ clock();
	params.commInitReq.reqLen = sizeof(commInitReq_t);

	/* Load the app's anet.inf. */
	err = dpReadAnetInf(".", &app);
	if ((err != dp_RES_OK) || FORCE_ERR) {
		/*PRINT(("Can't read anet.inf.\n"));*/
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}

	if (use_new_filename) {
		sprintf(fname, "freeze.anet%d", app.sessionType);
		strcat(GameArgs, " ");
		strcat(GameArgs, fname);
	} else {
		strcpy(fname, "freeze.dat");
	}
	fp = fopen(fname, "wb");
	if (!fp || FORCE_ERR) {
		/*PRINT(("Can't create freeze.dat file \n"));*/
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}

	parambuf[parambuflen++] = '!';
	parambuf[parambuflen++] = '\n';

	if ((fwrite(dp_LAUNCHPARAMS_T_SIG, sizeof(dp_LAUNCHPARAMS_T_SIG), 1, fp) != 1) ||
		(fwrite("\n", 1, 1, fp) != 1) ||
		(fwrite(parambuf, parambuflen, 1, fp) != 1)) {
		/*PRINT(("fwrite() failed to write to freeze.dat\n"));*/
		PRINT((MSG_ERR_LAUNCH , dp_RES_BAD, __LINE__));
		exit(1);
	}
	fclose(fp);

	dp_setLogFname("anetdrop.log");
	if (!let_game_init_comm) {
		fp = fopen("freeze.dat", "rb");
		if (!fp || FORCE_ERR) {
			/*PRINT(("Can't read freeze.dat file \n"));*/
			PRINT((MSG_ERR_LAUNCH, dp_RES_BAD, __LINE__));
			exit(1);
		}
		err = dpCommThaw(&dp, fp, myCommThaw_cb, NULL);
		if ((err != dp_RES_OK) || FORCE_ERR) {
			PRINT((MSG_ERR_LAUNCH, err, __LINE__));
			exit(1);
		}
		fclose(fp);

		assert(dp);
	}

	/* Launch the given app. */
	if (dp) {
		char separator;
		char freezefile[_MAX_PATH];
		DPRINT(("anetdrop: request to start; cwd %s\n", app.cwd));
		strcpy(freezefile, app.cwd);
#ifdef UNIX
		separator = '/';
#elif defined(__POWERPC__)
		separator = ':';
#else
		separator = '\\';
#endif
		if (freezefile[strlen(freezefile) - 1] != separator) {
			char buf[2];
			sprintf(buf, "%c", separator);
			strcat(freezefile, buf);
			strcat(freezefile, "freeze.dat");
		} else {
			strcat(freezefile, "freeze.dat");
		}
		DPRINT(("anetdrop: New freeze file %s\n", freezefile));
		cancelbox_set(MSG_WIND_TITLE, MSG_LAUNCHING);
		do {
			dpid_t idFrom;
			dpid_t idTo;
			char pkt[dpio_MAXLEN_UNRELIABLE];
			size_t size;
			size = sizeof(pkt);
			err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
			assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
			Sleep(10);

			err = dpReadyToFreeze(dp);
			if (err == dp_RES_OK) {
				err = dpFreeze(dp, freezefile);
				if ((err != dp_RES_OK) || FORCE_ERR) {
					/*PRINT(("Error %d freezing", err));*/
					PRINT((MSG_ERR_LAUNCH, err, __LINE__));
					exit(1);
				}
			} else if (err != dp_RES_BUSY) {
				PRINT((MSG_ERR_LAUNCH, err, __LINE__));
				exit(1);
			}
			if (cancelbox_poll())
				exit(1);
		} while (err == dp_RES_BUSY);

		/* If IPX or Internet, must destroy before launching!
		 * If Modem, must destroy after lauching!
		 * Serial could be either way...
		 */
#if 0
		err = dpGetTransportInfo(&driver, &info);
		if ((err != dp_RES_OK) || FORCE_ERR) {
			/*PRINT(("Error %d getting transport info", err));*/
			PRINT((MSG_ERR_LAUNCH, err, __LINE__));
			exit(1);
		}
#endif
		if (!(dp->dpio->driverinfo->needs & comm_INIT_NEEDS_PHONENUM)) {
			dpDestroy(dp, 0);
			dp = NULL;
		}
		cancelbox_clear();
	}
	memset(&processinfo, 0, sizeof(processinfo));
	err = dpLaunchApp2(&app, &processinfo);
	if (((err != dp_RES_OK) && (err != dp_RES_EMPTY)) || FORCE_ERR) {
		/*PRINT(("Unable to start application, error: %d", err));*/
		PRINT((MSG_ERR_LAUNCH, err, __LINE__));
		exit(1);
	} else if (dp_RES_EMPTY == err) {
		DPRINT(("anetdrop: No stub\n"));
	}
	if (dp /*&& (dp->dpio->driverinfo->needs & comm_INIT_NEEDS_PHONENUM)*/) {
		Sleep(5000);	/* hope app does dpCreate in 5 seconds or less */
		dpDestroy(dp, 1);	/* don't hang up */
		dp = NULL;
	}
	DPRINT(("anetdrop: App launched.\n"));
	if (params.Wait) {
		DWORD res;

		DPRINT(("anetdrop: waiting for app to finish; hProcess %x\n", processinfo.hProcess));
		res = WaitForSingleObject(processinfo.hProcess, INFINITE);
		DPRINT(("anetdrop: app finished; WaitForSingleObject returns %d\n", res));
		if (res == WAIT_FAILED) {
			res = GetLastError();
			DPRINT(("anetdrop: WaitForSingleObject fails; GetLastError returns %d\n", res));
		}
	}

	return 0;
}
