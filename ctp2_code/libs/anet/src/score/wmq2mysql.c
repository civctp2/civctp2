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
#include <mysql/mysql.h>
#include <time.h>
#include <assert.h>
#include "anet.h"
#include "scorerep.h"
#include "assoctab.h"
#include "wmq.h"

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

int update_mysql(dp_species_t sessType, dp_uid_t uid, int score, int won)
{
	MYSQL *mysql;
	MYSQL_RES *res;
	MYSQL_FIELD *field;
	MYSQL_ROW row;
	char tablename[16];
	char query[2048];
	char *errormsg;
	long score_sum;
	long score_max;
	int err;
	int i;

	mysql = mysql_connect(mysql, "localhost", "dank", "");
	if (!mysql) {
		errormsg = mysql_error(mysql);
		DPRINT(("wmq2mysql: connect error: %s\n", errormsg));
		return 1;
	}

	err = mysql_select_db(mysql, "alink");
	if (err) {
		errormsg = mysql_error(mysql);
		printf("select_db(alink) error: %d %s\n", err, errormsg);
		return 1;
	}

	sprintf(tablename, "score_%d", sessType);
	sprintf(query, "select score_sum, score_max from %s where uid=%d", tablename, uid);
	err = mysql_query(mysql, query);
	if (err) {
		errormsg = mysql_error(mysql);
		DPRINT(("wmq2mysql: query: %s\n    error: %d %s\n", query, err, errormsg));
		return 1;
	}

	res = mysql_store_result(mysql);
	if (res == NULL) {
		errormsg = mysql_error(mysql);
		if (errormsg) {
			DPRINT(("wmq2mysql: store_result error: %s\n", errormsg));
			return 1;
		}
		DPRINT(("wmq2mysql: No results\n"));
		return 0;
	}

	row = mysql_fetch_row(res);
	assert(row != NULL);
	score_sum = atol(row[0]);
	score_max = atol(row[1]);
	assert(NULL == mysql_fetch_row(res));

	DPRINT(("wmq2mysql: old score_sum:%d score_max:%d\n", score_sum, score_max)); 
	score_sum += score;
	if (score > score_max)
		score_max = score;

	sprintf(query, "update %s set score_sum=%d, score_max=%d where uid=%d", tablename, score_sum, score_max, uid);
	err = mysql_query(mysql, query);
	if (err) {
		errormsg = mysql_error(mysql);
		DPRINT(("wmq2mysql: query: %s\n    error: %d %s\n", query, err, errormsg));
		return 1;
	}
	assert(1 == mysql_affected_rows(mysql));

	mysql_close(mysql);
	return 0;
}

int main()
{
	dp_result_t err;
	time_t now, t_last, t_start = time(NULL);
	wmq_t *wmq;
	wmq_record_t record;
	char buf[1024];
	char *pbuf;
	unsigned short len;
	time_t data;
	long offset_old = 0;
	time_t t_old = t_start;

	record.buf = buf;
	record.buflen = 1024;

	wmq = wmq_create("wmq", 0);
	assert(wmq);

	now = time(0);
	err = wmq_seek(wmq, now, 0);  /* seek to beginning of today */
	assert(!err);

	while (1) {
		char *pbuf = buf;
		char sessid[8];  /* inet only: inetadr(4) port(2) karma(2) */
		dp_species_t sessType;
		dp_uid_t uid;
		dpid_t id;
		scorerep_t *rep;
		scorerep_buf_t repbuf;
		scorerep_player_t *player;
		int score;
		int won;

		err = wmq_get(wmq, &record);
		assert(err == dp_RES_OK || err == dp_RES_EMPTY);

		if (err != dp_RES_OK)
			continue;
		DPRINT(("wmq2mysql: read %d bytes, time:%08x: %s\n", record.datalen, wmq->t, hexstring(record.buf, record.datalen)));
		if (record.datalen < 12) {
			DPRINT(("wmq2mysql: len %d is not big enough for a header!\n", record.datalen));
			exit(1);
		}
		memcpy(sessid, pbuf, 8);
		pbuf += 8;
		sessType = dpMAKESHORT(pbuf[0], pbuf[1]);
		pbuf += 2;
		repbuf.len = dpMAKESHORT(pbuf[0], pbuf[1]);
		pbuf += 2;
		assert(repbuf.len > 0 && repbuf.len < scorerep_MAX_BUFLEN);
		memcpy(repbuf.buf, pbuf, repbuf.len);

		rep = scorerep_create();
		if (rep == NULL) {
			DPRINT(("wmq2mysql: can't create scorerep\n"));
			exit(1);
		}
		err = scorerep_fromBuf(rep, &repbuf);
		if (err != dp_RES_OK) {
			DPRINT(("wmq2mysql: scorerep_fromBuf returns err:%d\n", err));
			exit(1);
		}
		if (!(rep->flags & scorerep_FLAGS_SELFEXIT)) {
			DPRINT(("wmq2mysql: ignoring non-SELFEXIT score\n"));
			continue;
		}
		uid = rep->uid;
		id = rep->id;
		player = (scorerep_player_t *)assoctab_subscript(rep->players, id);
		if (!player) {
			DPRINT(("wmq2mysql: no entry for self (id:%d) in SELFEXIT!\n", id));
			exit(1);
		}
		assert(uid == player->uid);
		DPRINT(("wmq2mysql: read sessType:%d uid:%d (id:%d)\n", sessType, uid, id));
		if (player->bloblen < 3) {
			DPRINT(("wmq2mysql: report has bloblen:%d, no room for standard score + win byte\n", player->bloblen));
			continue;
		}
		score = dpMAKESHORT(player->blob[0], player->blob[1]);
		won = (int)player->blob[2];
		DPRINT(("wmq2mysql: read score:%d won:%d\n", score, won));

	}
	wmq_destroy(wmq);
	return 0;
}
