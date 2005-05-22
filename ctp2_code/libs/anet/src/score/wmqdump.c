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

int main()
{
	dp_result_t err;
	time_t now, t_last, t_start = time(NULL);
	wmq_t *wmq;
	wmq_record_t record;
	char buf[1024];
	unsigned short len;
	time_t data;
	long offset_old = 0;
	time_t t_old = t_start;

	record.buf = buf;
	record.buflen = 1024;

	wmq = wmq_create("./", 0);
	assert(wmq);

	now = time(0);
	err = wmq_seek(wmq, now, 0);  /* seek to beginning of today */
	assert(!err);

	while (1) {
		err = wmq_get(wmq, &record);
		assert(err == dp_RES_OK || err == dp_RES_EMPTY);

		if (err != dp_RES_OK)
			continue;
		printf("reading %d bytes, time:%08x: %s\n", record.datalen, wmq->t, hexstring(record.buf, record.datalen));
	}
	wmq_destroy(wmq);
	return 0;
}
