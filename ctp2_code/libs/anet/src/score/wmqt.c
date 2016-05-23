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
#include <assert.h>
#include "anet.h"
#include "wmq.h"

char WMQDir[wmq_DIR_MAXLEN] = "./";
char ServerTag[] = "wmqt";

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

int test_wmq(int write)
{
	dp_result_t err;
	time_t now, t_last, t_start;
	wmq_t *wmq;
	char buf[10];
	char *pbuf;
	unsigned short len;

	t_start = time(NULL);
	if (write) {
		wmq = wmq_create(WMQDir, 1);
		assert(wmq);

		now = time(0);
		err = wmq_seek(wmq, now, 0);
		if (err) {
			printf("wmq_seek returns err:%d\n", err);
			assert(!err);
		}

		t_last = now;
		while (1) {
			now = time(0);
			if (now > t_last) {
				pbuf = buf;
				*pbuf++ = dpGETLONG_FIRSTBYTE(now);
				*pbuf++ = dpGETLONG_SECONDBYTE(now);
				*pbuf++ = dpGETLONG_THIRDBYTE(now);
				*pbuf++ = dpGETLONG_FOURTHBYTE(now);
				printf("writing: t:%08x", now);
				if (!(now%10)) {
					*pbuf++ = dpGETLONG_FIRSTBYTE(t_start);
					*pbuf++ = dpGETLONG_SECONDBYTE(t_start);
					*pbuf++ = dpGETLONG_THIRDBYTE(t_start);
					*pbuf++ = dpGETLONG_FOURTHBYTE(t_start);
					printf(" (ts:%08x)", t_start);
				}
				printf("\n");
				len = pbuf - buf;
				err = wmq_putServer(wmq, now, ServerTag,
						wmq_RECORDTAG_TEST, buf, len);
				assert(!err);
				if (now > t_start + 120)
					break;
				t_last = now;
				wmq_flush(wmq);
			}
		}
	} else {
		time_t data;
		long offset_old = 0;
		time_t t_old = t_start;
		wmq_record_t record;

		record.buflen = 1024;
		record.buf = (char *)malloc(record.buflen);
		if (record.buf == NULL) {
			printf("wmqt: can't malloc record.buf!\n");
			assert(record.buf);
			exit(1);
		}

		wmq = wmq_create(WMQDir, 0);
		assert(wmq);

		now = time(0);
		err = wmq_seek(wmq, now, 0);  /* seek to beginning of today */
		assert(!err);

		while (1) {
			err = wmq_get(wmq, &record);
			assert(err == dp_RES_OK || err == dp_RES_EMPTY);

			if (err != dp_RES_OK)
				continue;
			printf("reading: ");
			assert(!memcmp(record.serverTag, ServerTag, 4));
			assert(!memcmp(record.recordTag, wmq_RECORDTAG_TEST, 2));
			printf("datalen:%d\n", record.datalen);
			if (wmq->t % 10) {
				assert(record.datalen == 4);
			} else {
				assert(record.datalen == 8);
				data = dpMAKELONG(record.buf[4], record.buf[5],
					record.buf[6], record.buf[7]);
				printf("(ts:%08x) ", data);
				assert(data <= wmq->t);
				/* assert(data >= wmq->t - 120); */
			}
			data = dpMAKELONG(record.buf[0], record.buf[1],
				record.buf[2], record.buf[3]);
			printf("td:%08x =? wmq->t:%08x\n", data, wmq->t);
			assert(data == wmq->t);

			if (!(wmq->t % 15)) {
				long offset_cur;
				long t_cur;

				err = wmq_tell(wmq, &offset_cur, &t_cur);
				assert(!err);

				err = wmq_seek(wmq, t_old, offset_old);
				assert(!err);

				err = wmq_get(wmq, &record);
				assert(err == dp_RES_OK);

				printf("seeking back and reading: ");
				assert(!memcmp(record.serverTag, ServerTag, 4));
				assert(!memcmp(record.recordTag, wmq_RECORDTAG_TEST, 2));
				printf("datalen:%d\n", record.datalen);
				if (wmq->t % 10) {
					assert(record.datalen == 4);
				} else {
					assert(record.datalen == 8);
					data = dpMAKELONG(record.buf[4], record.buf[5],
						record.buf[6], record.buf[7]);
					printf("(ts:%08x) ", data);
					assert(data <= wmq->t);
					/* assert(data >= wmq->t - 120); */
				}
				data = dpMAKELONG(record.buf[0], record.buf[1],
					record.buf[2], record.buf[3]);
				printf("td:%08x =? wmq->t:%08x\n", data, wmq->t);
				assert(data == wmq->t);

				err = wmq_seek(wmq, t_cur, offset_cur);
				assert(!err);
				t_old = t_cur;
				offset_old = offset_cur;
			}
		}
	}
	wmq_destroy(wmq);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	int write;

	if (argc < 2) {
		printf("\
Usage: %s (1|0) [dir] [tag]\n\
	1=write 0=read\n\
	wmqdir [dir] defaults to %s\n\
	serverTag [tag] defaults to 0.0.0.0\n", argv[0], WMQDir);
		return 1;
	}

	write = atoi(argv[1]);
	if (argc > 2) {
		strncpy(WMQDir, argv[2], wmq_DIR_MAXLEN);
		WMQDir[wmq_DIR_MAXLEN-1] = '\0';
	}
	if (argc > 3) {
		int temp[4];
		if ((sscanf(argv[3], "%d.%d.%d.%d",
			&(temp[0]), &(temp[1]), &(temp[2]), &(temp[3])) != 4)
		||  ((temp[0] & 0xff) != temp[0]) || ((temp[1] & 0xff) != temp[1])
		||  ((temp[2] & 0xff) != temp[2]) || ((temp[3] & 0xff) != temp[3])) {
			printf("invalid tag %s; should look like an ipaddress\n", argv[3]);
			return 1;
		}
		ServerTag[0] = (char)(temp[0] & 0xff);
		ServerTag[1] = (char)(temp[1] & 0xff);
		ServerTag[2] = (char)(temp[2] & 0xff);
		ServerTag[3] = (char)(temp[3] & 0xff);
	}
	ret = test_wmq(write);
	if (!ret)
		printf("wmq test passed\n");
	else
		printf("wmq test failed\n");
	return ret;
}
