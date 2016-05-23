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
#include <ctype.h>
#include <windows.h>

#include "dp2.h"
#undef DPRINT
#include "aeh.h"
#include "aehlog.h"
#include "portable.h"
#include "sbdclnt.h"
#include "sendbox.h"
#include "eclock.h"

#include "logprint.h"

#define sendcrsh_COMMENT_MAXLEN	70

int winCmdShow;

/*--------------------------------------------------------------------------
 Generic startup code.
--------------------------------------------------------------------------*/

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
	if (!argcopy) {
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
		}
	}

	return argi;
}

int main(int argc, char **argv);

#define MAXARGS 4

int WINAPI WinMain(HINSTANCE hinstExe, HINSTANCE hinstExePrev, LPSTR lpszCmdLine, int iCmdShow)
{
	char *argv[MAXARGS];

	winCmdShow = iCmdShow;
	return main(my_setargv(GetCommandLine(), argv, MAXARGS), argv);
}

/*--------------------------------------------------------------------------
 Specific application code goes below here.
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 Find the last exception record in a buf of the sort written by
 aeh_writetobuffer, convert it and put it into aeh.
 Returns ninst in the space provided.
 Returns the offset of the record read or -1 on error.
--------------------------------------------------------------------------*/
static int getLastRecord(char *buf, int len, aeh_t *aeh, int *ninst)
{
	aeh_buf_t aehbuf;
	int pos = 0;
	int recordlen = 0;

	/* find the last record in buf */
	do {
		recordlen = aehlog_readfrombuf(&aehbuf, ninst, buf+pos, len-pos);
		if (recordlen == -1) {
			DPRINT(("getLastRecord: can't find last record\n"));
			return -1;
		}
		pos += recordlen;
	} while (pos < len);
	pos -= recordlen;	/* pos = start of last record in buf */

	/* convert it to an aeh_t */
	if (aeh_RES_OK != aeh_readInputStream(aeh, &aehbuf)) {
		DPRINT(("getLastRecord: can't convert aehbuf to aeh\n"));
		return -1;
	}
	return pos;
}

/*--------------------------------------------------------------------------
 Returns 1 if the anet library set up our environment or 0 if not.
--------------------------------------------------------------------------*/
static int checkEnvironment()
{
	char envval[256];
	DWORD envlen;

	envlen = GetEnvironmentVariable("SENDCRSH", envval, 256);
	if (envlen == 0) {
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);
		DPRINT(("getenv error:%s\n", lpMsgBuf));
		LocalFree( lpMsgBuf );
		return 0;
	} else if (strcmp(envval, "Y")) {
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 Returns 1 if this is the only instance of sendcrsh running or 0 if not.
--------------------------------------------------------------------------*/
static int checkUnique()
{
	static char szMutexName[] = "MySendcrshMutex9283";
	HANDLE hMutex;
	/* Get a mutex handle and fail the current application instance if the
	 * mutex existed before the call to CreateMutex.
	 * The mutex will be deleted automatically when the application exits.
	 */
	hMutex = CreateMutex(NULL, FALSE, szMutexName);
	if ((hMutex != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS)) {
		CloseHandle(hMutex);
		return 0;
	}
	return 1;
}

#define xdigit2int(h) ((isdigit(h)) ? ((h) - '0') : (10 + (tolower(h) - 'a')))

/*--------------------------------------------------------------------------
 Convert len*2 hex digit characters in hexbuf into len bytes in buf.
 Returns buf on success,
 		 NULL on error (invalid hex digit, short hexbuf)
--------------------------------------------------------------------------*/
static char *hex2buf(const char *hexbuf, char *buf, int len)
{
	int i;

	if (!hexbuf || !buf || !len)
		return 0;

	for (i = 0; i < len; i++) {
		if (!isxdigit(hexbuf[2*i]) || !isxdigit(hexbuf[2*i+1]))
			return 0;
		buf[i] = xdigit2int(hexbuf[2*i])*16 + xdigit2int(hexbuf[2*i+1]);
	}
	return buf;
}

#ifdef logprint_ENABLED
void dumpbuf(const char *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		DPRINT(("%02x", (unsigned char)buf[i]));
		if (!((i+1)%32)) {
			DPRINT(("\n"));
		}
	}
	DPRINT(("\n"));
}
#else
#define dumpbuf(buf,len)
#endif

int main(int argc, char **argv)
{
	sbdclnt_t *sbdclnt;
	int port;
	int clicked;
	clock_t timeout;
	aeh_t aeh;
	int ninst;
	char comments[sendcrsh_COMMENT_MAXLEN];
	int len, lensent;
	char buf[sbd_MAXLEN];
	int pos;
	unsigned long crc;
	char signature[10];
	int i = 0;

	logprint_setFile("sendcrsh.log");

	if (!checkEnvironment()) {
		DPRINT(("anet2.dll did not set our environment!\n"));
		return 1;
	}
	if (!checkUnique()) {
		DPRINT(("Another instance of sendcrsh was running!\n"));
		return 1;
	}

	if (argc <= 3) { /* Usage: sendcrsh <host> <port> <hexdata> */
		DPRINT(("sendcrsh invoked with fewer than 3 arguments\n"));
		return 1;
	}
	DPRINT(("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]));

	port = atoi(argv[2]);
	if ((port < 1024) || (port > 65536)) {
		DPRINT(("sendcrsh invoked with invalid port %d\n", port));
		return 1;
	}

	if (NULL == hex2buf(argv[3], buf, 2)) {
		DPRINT(("hex2buf(datalen) failed\n"));
		return 1;
	}
	len = dpMAKESHORT(buf[0], buf[1]);
	if (NULL == hex2buf(argv[3] + 4, buf, len)) {
		DPRINT(("hex2buf(data) failed\n"));
		return 1;
	}
	if (argv[3][4 + 2*len] != '\0') {
		DPRINT(("data does not end at datalen:%d\n", len));
		return 1;
	}
	if (len > sbd_MAXLEN) {
		DPRINT(("datalen:%d is greater than max:%d\n", len, sbd_MAXLEN));
		return 1;
	}
	pos = getLastRecord(buf, len, &aeh, &ninst);
	if (pos == -1) {
		DPRINT(("getLastRecord failed\n"));
		return 1;
	}
	if (aeh.nstk == 0) {
		DPRINT(("exception has 0 length stack trace\n"));
	}
	crc = aeh_getSignature(&aeh);
	if (crc == 0) {
		DPRINT(("aeh_getSignature returns 0 crc, error?\n"));
	}
	aeh_signature_toString(crc, signature);

	DPRINT(("%s to %s:%d len:%d data:%s\n", argv[0], argv[1], port, len,
		argv[3] + 4));
	sendbox_create(winCmdShow, signature, sendcrsh_COMMENT_MAXLEN, 120);
	while (1) {
		clock_t now = eclock();

		clicked = sendbox_poll(comments);
		if (clicked != sendbox_CLICKED_NONE) {
			DPRINT(("t:%d, clicked:%d, breaking\n", now, clicked));
			break;
		}
		Sleep(100);
		if (!((++i)%10)) {
			DPRINT(("%dth call to sendbox_poll at t:%d\n", i, now));
		}
	}
	sendbox_destroy();

	if (clicked != sendbox_CLICKED_OK) {
		DPRINT(("User cancelled send, clicked:%d\n", clicked));
		return 0;
	}

	timeout = eclock() + 30 * ECLOCKS_PER_SEC;
	/* add the comments to the buffer */
	if (comments[0]) {
		aeh_buf_t aehbuf;
		int commentlen = strlen(comments);
		int nwritten;

		if ((len + sizeof(unsigned int /* aeh_info_t.id */) + sizeof(commentlen) + commentlen) > sbd_MAXLEN) {
			DPRINT(("no room in buf for comment\n"));
			return 1;
		}
		/* stuff the comment into the info list */
		if (aeh_RES_OK != aeh_addComment(&aeh, comments)) {
			DPRINT(("aeh_addComment failed\n"));
			return 1;
		}
		/* convert it back to a buf */
		if (aeh_RES_OK != aeh_writeOutputStream(&aeh, &aehbuf)) {
			DPRINT(("can't convert aeh back to aehbuf\n"));
			return 1;
		}
		DPRINT(("writing new record at pos:%d buf:\n", pos));
		dumpbuf(aehbuf.buf, aehbuf.buflen);
		nwritten = aehlog_writetobuf(&aehbuf, ninst, buf+pos, sbd_MAXLEN-pos);
		if (-1 == nwritten) {
			DPRINT(("can't convert aehbuf back to buf\n"));
			return 1;
		}
		len = pos + nwritten;
	}
	aeh_Destroy(&aeh);

	/* send the buffer */
	sbdclnt = sbdclnt_create(buf, len, argv[1], (unsigned short)port);
	if (sbdclnt == NULL) {
		DPRINT(("sbdclnt_create failed\n"));
		return 1;
	}
	while ((long)(eclock() - timeout) < 0) {
		lensent = sbdclnt_poll(sbdclnt);
		if (lensent != 0)
			break;
		Sleep(100);
	}
	sbdclnt_destroy(sbdclnt);
	if (lensent != len) {
		DPRINT(("send only %d of %d bytes!\n", lensent, len));
		return 1;
	}
	DPRINT(("send completed successfully\n"));
	return 0;
}
