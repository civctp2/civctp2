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

/* User account change handling stuff. */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "anet.h"
#include "antp.h"
#include "antpget.h"
#include "tcapw.h"
#include "pwq.h"
#include "servpw.h"

#define STATE_DISCONNECTED	0
#define STATE_WAIT			1
#define STATE_SENDGET		2
#define STATE_READHEAD		3
#define STATE_READDATA		4

#define BUFLEN		1024
#define MAX_RETRIES	5		/* connect retries before a lost connection
							 * starts returning errors (0=inf).
							 */
#define RETRYTIME_CONNECT	60		/* seconds between connection retries */
#define RETRYTIME_GET	30	/* seconds between retries for new data.
							 * Note that this should be less than the
							 * connection timeout time of antpserv or
							 * connections will not be persistent.
							 */

antpget_t *antpget;
antpget_url_t url;
FILE *localfp;
char localdir[256];
long offset;
int sock;
int state;
int retries;
clock_t next_retry;
int bGotDB;
long DBtotal;
int DBprogress;
pwq_t *pwq;

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
static const char *hexstring(const unsigned char *binstr, int len)
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

/*--------------------------------------------------------------------------
 Call this once before the server has initialized dp.
 Downloads the password database from the account server, which could
 potentially take a while to finish.
 Returns the socket used by servpw or -1 on error.
--------------------------------------------------------------------------*/
int servpw_init(const char *wmqDirectory, const char *PWServerAddr,
	int PWServerPort, const char *PWFile)
{
	dp_result_t err;
	int prevDBprogress = 0;
	char temppath[256];
	clock_t now = eclock();

	if (!wmqDirectory || !PWServerAddr || !PWFile) {
		printf("servpw_init: NULL parameter\n");
		return -1;
	}

	strncpy(url.host, PWServerAddr, antpget_URL_MAXLEN);
	url.host[antpget_URL_MAXLEN-1] = '\0';
	url.port = PWServerPort;
	strncpy(url.path, PWFile, antpget_URL_MAXLEN);
	url.path[antpget_URL_MAXLEN-1] = '\0';
	strncpy(localdir, wmqDirectory, 256);
	localdir[255] = '\0';

	antpget = antpget_create();
	if (!antpget) {
		printf("servpw_init: out of memory!\n");
		return -1;
	}
	sock = antpget_setHost(antpget, url.host, url.port);
	if (sock < 0) {
		printf("servpw_init: can't bind socket.\n");
		return -1;
	}
	retries = 0;
	next_retry = now;
	state = STATE_DISCONNECTED;
	bGotDB = 0;
	DBprogress = 0;
	sprintf(temppath, "%s/%s", localdir, url.path);
	if ((localfp = fopen(temppath, "wb")) == NULL) {
		printf("servpw_init: can't write to %s\n", temppath);
		return -1;
	}

	/* get the database file */
	printf("Downloading %s from %s:%d -\n", url.path, url.host, url.port);
	while (!bGotDB) {
		err = servpw_poll(now);
		if (err == dp_RES_OK) {
			for (; prevDBprogress < DBprogress; prevDBprogress++)
				printf(".");
			continue;
		} else if (err == dp_RES_AGAIN) {
			sleep(5);
			continue;
		} else if (err == dp_RES_EMPTY) {
			if (!bGotDB) {
				printf("\nservpw_init: account server: no file %s\n", url.path);
				return -1;
			}
		} else if (err == dp_RES_HOST_NOT_RESPONDING) {
			printf("\nservpw_init: Lost connection to account server!\n");
			return -1;
		} else {
			printf("\nservpw_init: antpget_poll err:%d\n", err);
			return -1;
		}
	}
	printf("done\n");
	return sock;
}

/*--------------------------------------------------------------------------
 Call this once when the server is starting up after the password database
 has been opened.
--------------------------------------------------------------------------*/
void servpw_initQ(tca_t *tca, const char *wmqDirectory)
{
	if (!tca || !wmqDirectory) {
		printf("servpw_initQ: NULL parameter\n");
		exit(1);
	}

	pwq = pwq_create(tca->tdb, wmqDirectory, 0, pwq_SERVERTAG_MASTER);
	if (pwq == NULL) {
		printf("servpw_initQ: pwq_create(%s) failed!\n", wmqDirectory);
		exit(1);
	}
}

/*--------------------------------------------------------------------------
 Set sock in <wfds> and <rfds> for use by select, based on the internal
 state of servpw.
 Returns -1 on error, or the number of socks set (1 or 0).
--------------------------------------------------------------------------*/
int servpw_setfds(fd_set *rfds, fd_set *wfds)
{
	switch (state) {
	case STATE_DISCONNECTED:
	case STATE_WAIT:
		return 0;
	case STATE_SENDGET:
		FD_SET(sock, wfds);
		return 1;
	case STATE_READHEAD:
	case STATE_READDATA:
		FD_SET(sock, rfds);
		return 1;
	default:
		return -1;
	}
}

/*--------------------------------------------------------------------------
 Call once a minute or so (regardless of select), to handle reconnects and
 processing of downloaded password queue files.
--------------------------------------------------------------------------*/
void servpw_pollQ(tca_t *tca, clock_t now)
{
	dp_result_t err;
	pwq_message_t msg;
	time_t timestamp;
	long qoffset;

	do {
		err = servpw_poll(now);
	} while (err == dp_RES_OK);
	if (err == dp_RES_HOST_NOT_RESPONDING) {
		DPRINT(("servpw_pollQ: Lost connection to account server!\n"));
	} else if ((err != dp_RES_AGAIN) && (err != dp_RES_EMPTY)) {
		DPRINT(("servpw_pollQ: antpget_poll err:%d\n", err));
		exit(1);
	}

	while (1) {
		err = pwq_get(pwq, &msg);
		if (err == dp_RES_EMPTY)
			break;
		if (err == dp_RES_AGAIN)
			continue;
		if (err != dp_RES_OK) {
			DPRINT(("servpw_pollQ: bug: pwq_get returns err:%d\n", err));
			return;
		}
		switch (msg.type) {
		case pwq_MESSAGETYPE_CREATE:
			err = tcapw_stuff_entry(tca->tdb, &msg.u.acctCreate);
			if (err != dp_RES_OK) {
				DPRINT(("servpw_pollQ: stuff_entry(%d) returns err:%d\n", msg.u.acctCreate.uid, err));
				return;
			}
			DPRINT(("servpw_pollQ: created uid:%d uname:%s\n", msg.u.acctCreate.uid, tcapw_u2ascii(msg.u.acctCreate.uname.uname, tcapw_LEN_USERNAME)));
			break;
		case pwq_MESSAGETYPE_CHANGE:
			err = tcapw_entry_change(tca->tdb, msg.u.acctChange.uid, &msg.u.acctChange.hpw, msg.u.acctChange.flags, msg.u.acctChange.email);
			if (err != dp_RES_OK) {
				DPRINT(("servpw_pollQ: entry_change(%d) returns err:%d\n", msg.u.acctChange.uid, err));
				return;
			}
			DPRINT(("servpw_pollQ: changed uid:%d uname:%s\n", msg.u.acctChange.uid, tcapw_u2ascii(msg.u.acctChange.uname.uname, tcapw_LEN_USERNAME)));
			break;
		case pwq_MESSAGETYPE_DELETE:
			err = tcapw_entry_delete(tca->tdb, msg.u.acctDelete.uid);
			if (err != dp_RES_OK) {
				DPRINT(("servpw_pollQ: entry_delete(%d) returns err:%d\n", msg.u.acctDelete.uid, err));
				return;
			}
			DPRINT(("servpw_pollQ: deleted uid:%d\n", msg.u.acctDelete.uid));
			break;
		default:
			DPRINT(("servpw_pollQ: invalid account msg type:%c\n", msg.type));
			return;
			break;
		}

		/* database was modified, so update its wmq pointer */
		err = pwq_tell(pwq, &timestamp, &qoffset);
		if (err != dp_RES_OK) {
			DPRINT(("servpw_pollQ: bug: pwq_tell returns err:%d\n", err));
			return;
		}
		err = tcapw_setWmqPosition(tca->tdb, timestamp, qoffset);
		if (err != dp_RES_OK) {
			DPRINT(("servpw_pollQ: bug: setWmqPosition returns err:%d\n", err));
			return;
		}
	}
}

/*--------------------------------------------------------------------------
 Handle downloads from the account server.
 When select returns a socket set by servpw_setfds(), call this function
 repeatedly while it returns dp_RES_OK.
 Returns dp_RES_OK if more processing can be done now,
		 dp_RES_EMPTY if the system is waiting for more data,
		 dp_RES_AGAIN if the system is waiting for another reason,
		 dp_RES_HOST_NOT_RESPONDING if the remote host is not responding.
--------------------------------------------------------------------------*/
dp_result_t servpw_poll(clock_t now)
{
	dp_result_t err;
	antpget_header_t *header;
	int total = 0;
	int nread;

	switch (state) {
	case STATE_DISCONNECTED:
		/* Reconnect if we're ready */
		if ((long)(now - next_retry) < 0)
			return dp_RES_AGAIN;
		retries++;
		err = antpget_connect(antpget);
		if (err == dp_RES_HOST_NOT_RESPONDING) {
			next_retry = now + RETRYTIME_CONNECT * ECLOCKS_PER_SEC;
			DPRINT(("servpw_poll: t:%d -connect %d failed, retry at t:%d\n",
					now, retries, next_retry));
			if (MAX_RETRIES && (retries >= MAX_RETRIES))
				return dp_RES_HOST_NOT_RESPONDING;
			return dp_RES_AGAIN;
		} else if (err != dp_RES_OK) {
			DPRINT(("servpw_poll: connect error:%d\n", err));
			return dp_RES_BUG;
		}
		retries = 0;
		DPRINT(("servpw_poll: Reconnected to account server, sending GET\n"));
		state = STATE_SENDGET;
		return dp_RES_OK;

	case STATE_WAIT:
		if ((long)(now - next_retry) < 0)
			return dp_RES_EMPTY;
		state = STATE_SENDGET;
		/* Fall through to SENDGET */

	case STATE_SENDGET:
		err = antpget_requestGet(antpget, url.path, offset);
		if (err == dp_RES_AGAIN) {
			return dp_RES_AGAIN;
		} else if (err == dp_RES_OK) {
			DPRINT(("servpw_poll: %s - sent GET %d EOF\n", url.path, offset));
			state = STATE_READHEAD;
			return dp_RES_OK;
		} else if (err == dp_RES_HOST_NOT_RESPONDING) {
			state = STATE_DISCONNECTED;
			next_retry = now;
			return dp_RES_AGAIN;
		} else {
			DPRINT(("servpw_poll: %s - requestGet error:%d\n", url.path, err));
			close(sock);
			return dp_RES_BUG;
		}
		break;

	case STATE_READHEAD:
		err = antpget_readHeader(antpget, &header);
		if ((err == dp_RES_OK) || (err == dp_RES_AGAIN)) {
			return err;
		} else if (err == dp_RES_EMPTY) {
			int j;
			char temppath[256];

			for (j = 0; j < header->nlines; j++)
				DPRINT(("servpw_poll: %s> %s\n", url.path, header->line[j]));
			switch (header->status) {
			case ANTP_STATUS_NEWFILE:
				DPRINT(("servpw_poll: %s> new file %s\n",
					url.path, header->path));
				bGotDB = 1;
				strcpy(url.path, header->path);
				offset = header->offset;
				if (localfp)
					fclose(localfp);
				sprintf(temppath, "%s/%s", localdir, url.path);
				if ((localfp = fopen(temppath, "wb")) == NULL) {
					DPRINT(("servpw_poll: %s - Can't open!\n", url.path));
					close(sock);
					return dp_RES_CANT_WRITE;
				}
				if (header->length <= 0) {
					/* probably a response to a FIRST */
					state = STATE_SENDGET;
					return dp_RES_OK;
				}
				/* Fall through to OK */
			case ANTP_STATUS_OK:
				if (!bGotDB)
					DBtotal = header->length;
				state = STATE_READDATA;
				break;
			case ANTP_STATUS_EMPTY:
				next_retry = now + RETRYTIME_GET * ECLOCKS_PER_SEC;
				DPRINT(("servpw_poll: sending next GET at t:%d\n", next_retry));
				state = STATE_WAIT;
				return dp_RES_EMPTY;
			default:
				DPRINT(("servpw_poll: unknown status %03d\n", header->status));
				close(sock);
				return dp_RES_BUG;
			}
		} else if (err == dp_RES_HOST_NOT_RESPONDING) {
			state = STATE_DISCONNECTED;
			return dp_RES_AGAIN;
		} else {
			if (header->line[0][0])
				DPRINT(("servpw_poll: %s> %s\n", url.path, header->line[0]));
			DPRINT(("servpw_poll: %s - readHeader error:%d\n", url.path, err));
			close(sock);
			return dp_RES_BUG;
		}
		if ((err != dp_RES_EMPTY) || (state != STATE_READDATA))
			return dp_RES_AGAIN;  /* This test should never be true */
		/* else fall through to READDATA while sock is still set
		 * to be read, in order to read the rest of the current buffer
		 * as data.
		 */

	case STATE_READDATA:
		do {
			char buf[BUFLEN];

			/* keep reading until we get a partial buffer */
			err = antpget_readData(antpget, buf, BUFLEN, &nread);
			if (((err == dp_RES_OK) || (err == dp_RES_EMPTY))
			&&  (nread > 0)) {
				int j;

				offset += nread;
				total += nread;
				if (fwrite(buf, nread, 1, localfp) != 1) {
					DPRINT(("servpw_poll: %s - fwrite error:%d\n",
						url.path, ferror(localfp)));
					close(sock);
					return dp_RES_BUG;
				}
			}
		} while ((nread == BUFLEN) && (err == dp_RES_OK));
		if ((err == dp_RES_OK) || (err == dp_RES_AGAIN)) {
			DPRINT(("servpw_poll: %s -read %d bytes total\n", url.path, total));
			if (total > 0) {
				fflush(localfp);
				if (!bGotDB)
					DBprogress = (offset * 50)/DBtotal;
			}
			return dp_RES_AGAIN;
		} else if (err == dp_RES_EMPTY) {
			DPRINT(("servpw_poll: %s -read %d bytes total\n", url.path, total));
			if (total > 0) {
				fflush(localfp);
				if (!bGotDB)
					DBprogress = (offset * 50)/DBtotal;
			}
			next_retry = now;
			state = STATE_SENDGET;
			return dp_RES_OK;
		} else if (err == dp_RES_HOST_NOT_RESPONDING) {
			state = STATE_DISCONNECTED;
			return dp_RES_AGAIN;
		} else {
			DPRINT(("servpw_poll: %s - readData error:%d\n", url.path, err));
			close(sock);
			return dp_RES_BUG;
		}
		break;

	default:
		DPRINT(("servpw_poll: %s - bad state:%d\n", url.path, state));
		close(sock);
		return dp_RES_BUG;
	}
	DPRINT(("servpw_poll: Shouldn't get here!\n"));
	return dp_RES_BUG;
}
