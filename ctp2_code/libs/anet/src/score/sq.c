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

#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <assert.h>
#include "anet.h"
#include "scorerep.h"
#include "assoctab.h"
#include "wmq.h"
#include "sq.h"

#define SQ_DAT "jscore.dat"

wmq_t *wmq;
int outdatfd;

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
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

/* fake a few functions so we don't have to include dp2.lib */
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)			/* printf-style arguments on stack (if any) */
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

dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt)
{
	printf("dpReportAssertionFailure: %s, %d: %s\n", file, lineno, linetxt);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Call before using sq_get to initialize the web message queue.
--------------------------------------------------------------------------*/
void sq_init(const char *wmqDirectory)
{
#ifdef UNIX
	dp_result_t err;
	time_t now = time(0);
	char filepath[wmq_DIR_MAXLEN + 24];
	int indatfd;
#endif

	wmq = wmq_create(wmqDirectory, 0);
	assert(wmq);

#ifdef UNIX
	sprintf(filepath, "%s/%s", wmq->dir, SQ_DAT);
	if ((indatfd = open(filepath, O_RDONLY, 0)) < 0) {
		printf("sq %s: %s not found, seeking start\n", wmq->dir, filepath);
		err = wmq_seek(wmq, 0, 0);  /* seek to beginning of queue */
		if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)) {
			DPRINT(("sq_init: can't seek start, err:%d\n", err));
			assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
			exit(1);
		}
	} else {
		time_t t;
		long offset;

		printf("sq %s: reading startup info from %s\n", wmq->dir, filepath);
		err = wmq_restorePosition(wmq, indatfd);
		if (err == dp_RES_EMPTY) {
			printf("sq %s: %s empty, seeking start\n", wmq->dir, filepath);
			err = wmq_seek(wmq, 0, 0);
			if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)) {
				printf("sq %s: wmq_seek err:%d\n", wmq->dir, err);
				assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
				exit(1);
			}
		} else if (err != dp_RES_OK) {
			printf("sq %s: %s invalid!\n", wmq->dir, filepath);
			exit(1);
		} else {
			wmq_tell(wmq, &offset, &t);
			printf("%s: wmq_restorePosition(t:0x%x, o:0x%x)\n",
				wmq->dir, t, offset);
		}
		close(indatfd);
	}
	if ((outdatfd = open(filepath, O_WRONLY | O_CREAT, 0644)) < 0) {
		printf("sq %s: can't write to %s!\n", wmq->dir, filepath);
		exit(1);
	}
#endif
}

/*--------------------------------------------------------------------------
 Call to clean up when finished reading the web message queue
--------------------------------------------------------------------------*/
void sq_destroy()
{
	wmq_destroy(wmq);
}

/*--------------------------------------------------------------------------
 Get a score report message from the web message queue if one is waiting.
 Puts the submitting player's session id, and session type into the
 spaces pointed to by sessid, and sessType.
 The sessid returned will have length no greater than dp_KEY_MAXLEN;
 The length used is returned in sessidlen.

 Messages are returned in an sq_message_t struct pointed to by msg.

 Returns dp_RES_OK if there is a scorerep_buf to process,
 		 dp_RES_AGAIN if there was a non-score wmq entry,
         dp_RES_EMPTY if there is nothing there.
--------------------------------------------------------------------------*/
dp_result_t sq_get(unsigned char *sessid, int *sessidlen, dp_species_t *sessType, sq_message_t *msg)
{
	dp_result_t err;
	time_t now = time(0);
	wmq_record_t record;
	char getbuf[1024];
	char *pbuf = getbuf;
	scorerep_buf_t repbuf;

	record.buf = getbuf;
	record.buflen = 1024;

	err = wmq_get(wmq, &record);
	if (err == dp_RES_EMPTY)
		return dp_RES_EMPTY;
	if (err != dp_RES_OK) {
		DPRINT(("sq_get: bug: wmq_get returns err:%d\n", err));
		return dp_RES_BUG;
	}
	if (record.datalen < 10) {
		DPRINT(("sq_get: bug: len %d is not big enough for a record!\n",
			record.datalen));
		return dp_RES_BUG;
	}
	memset(msg, 0, sizeof(sq_message_t));
	*sessidlen = 8;
	memcpy(sessid, pbuf, 8);
	pbuf += 8;
	*sessType = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += 2;
	memcpy(msg->recordTag, record.recordTag, 2);
	if (!memcmp(record.recordTag, wmq_RECORDTAG_SCORE, 2)) {
		/* Score Report */
		DPRINT(("sq_get: read ScoreReport time:%08x server:%s len:%d\n",
			record.timestamp, hexstring(record.serverTag, 6), record.datalen));
		DPRINT(("sq_get: sessionId:%s sessType:%d player left\n",
			hexstring(sessid, 8), *sessType));
		repbuf.len = dpMAKESHORT(pbuf[0], pbuf[1]);
		pbuf += 2;
		assert(repbuf.len > 0 && repbuf.len < scorerep_MAX_BUFLEN);
		memcpy(repbuf.buf, pbuf, repbuf.len);

		err = scorerep_fromBuf(&(msg->u.scorerep), &repbuf);
		assert(msg->u.scorerep.players);
		assert(msg->u.scorerep.players->buf);
		assert(msg->u.scorerep.players->n_used < 1000);
		assert(msg->u.scorerep.players->n_alloced < 1000);
		assert(msg->u.scorerep.players->unit < 4000);
		if (err != dp_RES_OK) {
			DPRINT(("sq_get: scorerep_fromBuf returns err:%d\n", err));
			return dp_RES_BUG;
		}
		err = wmq_savePosition(wmq, outdatfd);
		if (err != dp_RES_OK) {
			DPRINT(("sq_get: wmq_savePosition returns err:%d\n", err));
			return dp_RES_BUG;
		}
		return dp_RES_OK;
	} else if (!memcmp(record.recordTag, wmq_RECORDTAG_SESSIONCLOSED, 2)) {
		/* Session Closed */
		DPRINT(("sq_get: read SessionClose time:%08x server:%s len:%d\n",
			record.timestamp, hexstring(record.serverTag, 6), record.datalen));
		DPRINT(("sq_get: sessionId:%s sessType:%d closed\n",
			hexstring(sessid, 8), *sessType));

		err = wmq_savePosition(wmq, outdatfd);
		if (err != dp_RES_OK) {
			DPRINT(("sq_get: wmq_savePosition returns err:%d\n", err));
			return dp_RES_BUG;
		}
		return dp_RES_OK;
	} else {
		/* Some other message */
		err = wmq_savePosition(wmq, outdatfd);
		if (err != dp_RES_OK) {
			DPRINT(("sq_get: wmq_savePosition returns err:%d\n", err));
			return dp_RES_BUG;
		}
		return dp_RES_AGAIN;
	}
}

#if 0
int main()
{
	dp_result_t err;
	dp_uid_t uid;
	dp_species_t sessType;
	char sessid[dp_KEY_MAXLEN];
	int sessidlen;
	scorerep_t *rep;

	DPRINT(("entering main\n"));
	sq_init();

	while (1) {
		err = sq_get(sessid, &sessidlen, &sessType, &rep);
		if (err != dp_RES_OK && err != dp_RES_EMPTY)
			exit(1);
		if (err != dp_RES_OK)
			continue;
		printf("got a score report from uid:%d sessType:%d\n", rep->uid, sessType);
	}

	sq_destroy();
	return 0;
}
#endif
