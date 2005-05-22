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
#include "assoctab.h"
#include "pwq.h"

/*--------------------------------------------------------------------------
 Create an instance of pwq. 
 If write is 0, it will be a read queue, and pwq_put will fail.
 If write is non-0, it will be a write queue, and pwq_get will fail. 
 Returns NULL on failure.
--------------------------------------------------------------------------*/
pwq_t *pwq_create(tcapw_t *tcapw, const char *wmqDirectory, int write, const char *masterServerTag)
{
	dp_result_t err;
	pwq_t *pwq;
	wmq_t *wmq;
	time_t timestamp;
	long offset;

	if (tcapw == NULL || wmqDirectory == NULL || masterServerTag == NULL || tcapw->tab == NULL)
		return NULL;

	wmq = wmq_create(wmqDirectory, write);
	if (wmq == NULL) {
		DPRINT(("pwq_create: wmq_create(%s, %d) failed\n", wmqDirectory, write));
		return NULL;
	}
	if (write) {
		wmq_setServerTag(wmq, masterServerTag, 6);
		timestamp = time(NULL);
		err = wmq_seek(wmq, timestamp, 0);
		if (err != dp_RES_OK) {
			DPRINT(("pwq_create: wmq_seek(now:%8x, 0) returns err:%d\n", timestamp, err));
			return NULL;
		}
	} else {
		err = tcapw_getWmqPosition(tcapw, &timestamp, &offset);
		if (err != dp_RES_OK) {
			DPRINT(("pwq_create: getWmqPosition returns err:%d\n", err));
			return NULL;
		}
		err = wmq_seek(wmq, timestamp, offset);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			DPRINT(("pwq_create: wmq_seek(%8x, %8x) returns err:%d\n", timestamp, offset, err));
			return NULL;
		}
	}
	pwq = (pwq_t *) malloc(sizeof(pwq_t));
	if (pwq == NULL) {
		DPRINT(("pwq_create: malloc failed\n"));
		return NULL;
	}
	pwq->tcapw = tcapw;
	pwq->wmq = wmq;
	memcpy(pwq->masterServerTag, masterServerTag, 6);

	return pwq;
}

/*--------------------------------------------------------------------------
 Destroy an instance of the pwq.  Saves the password database pwq->tcapw
 to disk, but does not otherwise affect it.
--------------------------------------------------------------------------*/
void pwq_destroy(pwq_t *pwq)
{
	if (pwq->wmq->openForWrite)
		wmq_flush(pwq->wmq);
	wmq_destroy(pwq->wmq);
	/* tcapw_close just saves the db to disk. It doesn't change tcapw. */
	tcapw_close(pwq->tcapw);
	memset(pwq, 0, sizeof(pwq_t));
	free(pwq);
}

/*--------------------------------------------------------------------------
 Get an account message from the web message queue if one is waiting.
 Messages are returned in a pwq_message_t struct pointed to by msg. 
 
 Returns dp_RES_OK if there is an account message to process,
 		 dp_RES_AGAIN if there was a non-account wmq entry,
         dp_RES_EMPTY if there is nothing there.
--------------------------------------------------------------------------*/
dp_result_t pwq_get(pwq_t *pwq, pwq_message_t *msg)
{
	dp_result_t err;
	wmq_record_t record;
	char getbuf[1024];
	char *pbuf = getbuf;
	int emaillen;

	if (pwq == NULL || msg == NULL || pwq->wmq->openForWrite)
		return dp_RES_BAD;

	record.buf = getbuf;
	record.buflen = 1024;

	err = wmq_get(pwq->wmq, &record);
	if (err == dp_RES_EMPTY)
		return dp_RES_EMPTY;
	if (err != dp_RES_OK) {
		DPRINT(("pwq_get: bug: wmq_get returns err:%d\n", err));
		return dp_RES_BUG;
	}

	if (memcmp(record.recordTag, wmq_RECORDTAG_ACCOUNT, 2))
		return dp_RES_AGAIN;
	if (memcmp(record.serverTag, pwq->masterServerTag, 6)) {
		DPRINT(("pwq_get: account message not from master server\n"));
		return dp_RES_AGAIN;
	}

	memset(msg, 0, sizeof(pwq_message_t));
	msg->type = *pbuf;
	pbuf++;
	switch (msg->type) {
	case pwq_MESSAGETYPE_CREATE:
	case pwq_MESSAGETYPE_CHANGE:
		if (record.datalen < 8 + sizeof(tcapw_uname_t) + sizeof(tcapw_hpw_t)) {
			DPRINT(("pwq_get: invalid record.  datalen:%d < %d\n", record.datalen, 8 + sizeof(tcapw_uname_t) + sizeof(tcapw_hpw_t)));
			return dp_RES_BADSIZE;
		}
		msg->u.acctChange.uid = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
		pbuf += 4;
		msg->u.acctChange.flags = dpMAKESHORT(pbuf[0], pbuf[1]);
		pbuf += 2;
		msg->u.acctChange.secretcode = dpMAKESHORT(pbuf[0], pbuf[1]);
		pbuf += 2;
		msg->u.acctChange.created = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
		pbuf += 4;
		msg->u.acctChange.lastlogin = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
		pbuf += 4;
		memcpy(&msg->u.acctChange.uname, pbuf, sizeof(tcapw_uname_t));
		pbuf += sizeof(tcapw_uname_t);
		memcpy(&msg->u.acctChange.hpw, pbuf, sizeof(tcapw_hpw_t));
		pbuf += sizeof(tcapw_hpw_t);
		emaillen = (unsigned char)*pbuf;
		pbuf++;
		if (emaillen >= tcapw_MAXLEN_EMAIL) {
			DPRINT(("pwq_get: invalid record.  emaillen:%d > %d\n", emaillen, tcapw_MAXLEN_EMAIL - 1));
			return dp_RES_BADSIZE;
		}
		if (record.datalen != 8 + sizeof(tcapw_uname_t) + sizeof(tcapw_hpw_t) + emaillen) {
			DPRINT(("pwq_get: invalid record.  datalen:%d != %d\n", record.datalen, 8 + sizeof(tcapw_uname_t) + sizeof(tcapw_hpw_t) + emaillen));
			return dp_RES_BADSIZE;
		}
		strncpy(msg->u.acctChange.email, pbuf, emaillen);
		msg->u.acctChange.email[emaillen] = '\0';
		break;
	case pwq_MESSAGETYPE_DELETE:
		if (record.datalen != 5) {
			DPRINT(("pwq_get: invalid delete record: datalen:%d != 5\n", record.datalen));
			return dp_RES_BADSIZE;
		}
		msg->u.acctDelete.uid = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
		break;
	default:
		DPRINT(("pwq_get: invalid account record type:%c\n", msg->type)); 
		return dp_RES_BADSIZE;
		break;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Put an account message into the web message queue with the given timestamp.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t pwq_put(pwq_t *pwq, time_t now, pwq_message_t *msg)
{
	dp_result_t err;
	char putbuf[1024];
	char *pbuf = putbuf;
	int datalen;
	int emaillen;

	if (pwq == NULL || msg == NULL || !pwq->wmq->openForWrite)
		return dp_RES_BAD;

	*pbuf++ = msg->type;
	switch (msg->type) {
	case pwq_MESSAGETYPE_CREATE:
	case pwq_MESSAGETYPE_CHANGE:
		*pbuf++ = dpGETLONG_FIRSTBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETLONG_SECONDBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETLONG_THIRDBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETLONG_FOURTHBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETSHORT_FIRSTBYTE(msg->u.acctChange.flags);
		*pbuf++ = dpGETSHORT_SECONDBYTE(msg->u.acctChange.flags);
		*pbuf++ = dpGETSHORT_FIRSTBYTE(msg->u.acctChange.secretcode);
		*pbuf++ = dpGETSHORT_SECONDBYTE(msg->u.acctChange.secretcode);
		*pbuf++ = dpGETLONG_FIRSTBYTE(msg->u.acctChange.created);
		*pbuf++ = dpGETLONG_SECONDBYTE(msg->u.acctChange.created);
		*pbuf++ = dpGETLONG_THIRDBYTE(msg->u.acctChange.created);
		*pbuf++ = dpGETLONG_FOURTHBYTE(msg->u.acctChange.created);
		*pbuf++ = dpGETLONG_FIRSTBYTE(msg->u.acctChange.lastlogin);
		*pbuf++ = dpGETLONG_SECONDBYTE(msg->u.acctChange.lastlogin);
		*pbuf++ = dpGETLONG_THIRDBYTE(msg->u.acctChange.lastlogin);
		*pbuf++ = dpGETLONG_FOURTHBYTE(msg->u.acctChange.lastlogin);
		memcpy(pbuf, &msg->u.acctChange.uname, sizeof(tcapw_uname_t));
		pbuf += sizeof(tcapw_uname_t);
		memcpy(pbuf, &msg->u.acctChange.hpw, sizeof(tcapw_hpw_t));
		pbuf += sizeof(tcapw_hpw_t);
		emaillen = strlen(msg->u.acctChange.email);
		assert(emaillen < tcapw_MAXLEN_EMAIL && emaillen < 256);
		*pbuf++ = (char)emaillen;
		memcpy(pbuf, msg->u.acctChange.email, emaillen);
		pbuf += emaillen;
		break;
	case pwq_MESSAGETYPE_DELETE:
		*pbuf++ = dpGETLONG_FIRSTBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETLONG_SECONDBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETLONG_THIRDBYTE(msg->u.acctChange.uid);
		*pbuf++ = dpGETLONG_FOURTHBYTE(msg->u.acctChange.uid);
		break;
	default:
		DPRINT(("pwq_put: invalid account record type:%c\n", msg->type)); 
		return dp_RES_BAD;
		break;
	}

	err = wmq_put(pwq->wmq, now, wmq_RECORDTAG_ACCOUNT, putbuf, (unsigned short)(pbuf-putbuf));
	if (err != dp_RES_OK) {
		DPRINT(("pwq_put: wmq_put returns err:%d", err));
		return dp_RES_BUG;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Get the wmq position into timestamp, offset.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t pwq_tell(pwq_t *pwq, time_t *ptime, long *poffset)
{
	if (pwq == NULL)
		return dp_RES_BAD;
	return wmq_tell(pwq->wmq, poffset, ptime);
}
