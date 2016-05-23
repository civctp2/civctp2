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
#include <fcntl.h>
#include "anet.h"
#include "wmq.h"
#include "wmqfile.h"
#include "time.h"

#undef DPRINT
#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
#define DPRINT(a) printf a
/* fake functions so we don't have to include dp2.lib */
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
#else
#define DPRINT(a)
#endif
/* fake functions so we don't have to include dp2.lib */
dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt)
{
	printf("dpReportAssertionFailure: %s, %d: %s\n", file, lineno, linetxt);
	return dp_RES_OK;
}

#define MAX_WMQS		16	/* Maximum number of wmqs we can monitor */
#define WMQMERGE_DAT	"wmqmerge.dat"	/* filename of dat files */
#define INIT_BUFSIZE	256	/* Initial size of record buffers */
#define RETRYTIME_GET	30	/* secs between retries for more data */
#define MAX_CLOCKSKEW	180	/* secs of clock skew allowed between servers
							 * in incoming data (including internet lag)
							 * before a queue is considered truly empty.
							 */

main(int argc, char **argv)
{
	dp_result_t err;
	wmq_t *wmq_in[MAX_WMQS];
	wmq_record_t wmq_record[MAX_WMQS];
	int outdatfd;
	int indatfd[MAX_WMQS];
	time_t t_now;					/* current local time() */
	long t_offset;					/* t_now + t_offset = timestamp_now */
	time_t t_next_read[MAX_WMQS];	/* local time to attempt next read */
	wmq_t *wmq_out;
	int n_wmqs;
	int i_oldest;
	char filepath[wmq_DIR_MAXLEN + 24];
	int wmq_out_restored = 0;

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	/* process command line parameters */
	if (argc < 4) {
		printf("\
Usage: %s <outdir> <indir1> <indir2> ...\n\
	Merge the wmq's found in <indir1>, etc. into one time ordered wmq in\n\
	<outdir>.  Up to %d wmq's may be merged.\n\
	Keeps track of current status in <outdir/indir*>/%s.\n\
	If present, these files are used on restart.\n",
			argv[0], MAX_WMQS, WMQMERGE_DAT);
		exit(0);
	}

	/* create all the wmq's */
	t_now = time(NULL);
	wmq_out = wmq_create(argv[1], 1);  /* open for writing */
	if (!wmq_out) {
		printf("Can't open wmq dir %s\n", argv[1]);
		exit(1);
	}
	sprintf(filepath, "%s/%s", wmq_out->dir, WMQMERGE_DAT);
	if ((outdatfd = open(filepath, O_RDONLY, 0)) < 0) {
		printf("%s: %s not found, seeking later\n", wmq_out->dir, filepath);
	} else {
		time_t t;
		long offset;

		printf("%s: reading startup info from %s\n", wmq_out->dir, filepath);
		err = wmq_restorePosition(wmq_out, outdatfd);
		if (err == dp_RES_OK) {
			wmq_tell(wmq_out, &offset, &t);
			printf("%s: wmq_restorePosition(t:0x%x, o:0x%x)\n",
				wmq_out->dir, t, offset);
			wmq_out_restored = 1;
		} else if (err == dp_RES_EMPTY) {
			printf("%s: empty datfile, seeking later\n", wmq_out->dir);
		} else {
			printf("%s: %s invalid!\n", wmq_out->dir, filepath);
			exit(1);
		}
		close(outdatfd);
	}
	if ((outdatfd = open(filepath, O_WRONLY | O_CREAT, 0644)) < 0) {
		printf("%s: can't write to %s!\n", wmq_out->dir, filepath);
		exit(1);
	}

	n_wmqs = 0;
	while ((n_wmqs + 2 < argc) && (n_wmqs < MAX_WMQS)) {
		wmq_in[n_wmqs] = wmq_create(argv[n_wmqs+2], 0);  /* open for reading */
		if (!wmq_in[n_wmqs]) {
			printf("Can't open wmq dir %s\n", argv[n_wmqs + 2]);
			exit(1);
		}
		sprintf(filepath, "%s/%s", (wmq_in[n_wmqs])->dir, WMQMERGE_DAT);
		if ((indatfd[n_wmqs] = open(filepath, O_RDONLY, 0)) < 0) {
			printf("%s: %s not found, seeking to start\n",
				wmq_in[n_wmqs]->dir, filepath);
			err = wmq_seek(wmq_in[n_wmqs], 0, 0);
			if ((err != dp_RES_EMPTY) && (err != dp_RES_OK)) {
				printf("%s: seek(t:0, o:0x0) returned err:%d\n",
					wmq_in[n_wmqs]->dir, err);
				exit(1);
			}
		} else {
			time_t t;
			long offset;

			printf("%s: reading startup info from %s\n",
				wmq_in[n_wmqs]->dir, filepath);
			err = wmq_restorePosition(wmq_in[n_wmqs], indatfd[n_wmqs]);
			if (err == dp_RES_EMPTY) {
				printf("%s: empty datfile, seeking(t:0x0, o:0x0)\n",
						wmq_in[n_wmqs]->dir);
				err = wmq_seek(wmq_in[n_wmqs], 0, 0);
				if (err == dp_RES_OK) {
					wmq_tell(wmq_in[n_wmqs], &offset, &t);
					printf("%s: wmq_restorePosition(t:0x%x, o:0x%x)\n",
						wmq_in[n_wmqs]->dir, t, offset);
				} else if (err == dp_RES_EMPTY) {
					printf("%s: no files yet.\n", wmq_in[n_wmqs]->dir);
				} else {
					printf("%s: wmq_seek err:%d\n", wmq_in[n_wmqs]->dir, err);
					exit(1);
				}
			} else if (err != dp_RES_OK) {
				printf("%s: %s invalid!\n", wmq_in[n_wmqs]->dir, filepath);
				exit(1);
			}
			close(indatfd[n_wmqs]);
		}
		if ((indatfd[n_wmqs] = open(filepath, O_WRONLY | O_CREAT, 0644)) < 0) {
			printf("%s: can't write to %s!\n", wmq_in[n_wmqs]->dir, filepath);
			exit(1);
		}

		/* Malloc the starting wmq_record buffers */
		wmq_record[n_wmqs].buf = (char *)malloc(INIT_BUFSIZE);
		if (wmq_record[n_wmqs].buf == NULL) {
			printf("%s: can't malloc %d bytes for reading\n",
				wmq_in[n_wmqs]->dir, INIT_BUFSIZE);
			exit(1);
		}
		wmq_record[n_wmqs].buflen = INIT_BUFSIZE;

		wmq_record[n_wmqs].timestamp = 0;
		t_next_read[n_wmqs] = t_now;
		n_wmqs++;
	}

	printf("Merging %d files\n", n_wmqs);
	/* Need timestamp_now = (t_now + t_offset) to be 0, initially */
	t_offset = (long)(-t_now);
	i_oldest = -1;
	while (1) {
		int i;
		int n_empty = 0;
		time_t timestamp_now;

		t_now = time(NULL);
		timestamp_now = (time_t)(t_now + t_offset);
		for (i = 0; i < n_wmqs; i++) {
			if (wmq_record[i].timestamp == 0) {
				if ((long)(t_now - t_next_read[i]) < 0) {
					n_empty++;
					continue;
				}
				err = wmq_get(wmq_in[i], &wmq_record[i]);
				if (err == dp_RES_EMPTY) {
					/* wmq_in[i] is empty, wait for new data */
					t_next_read[i] = t_now + RETRYTIME_GET;
					printf("%s: t:%d, empty\n", wmq_in[i]->dir, timestamp_now);
					n_empty++;
					continue;
				} else if (err == dp_RES_FULL) {
					char *temp;

					/* we need a bigger buf in wmq_record[i] */
					temp = (char *)realloc(wmq_record[i].datalen, wmq_record[i].buf);
					if (!temp) {
						printf("%s: can't realloc %d bytes to read\n",
							wmq_in[i]->dir, wmq_record[i].datalen);
						exit(1);
					}
					printf("%s: record buf expanded to %d bytes\n",
						wmq_in[i]->dir, wmq_record[i].datalen);
					wmq_record[i].buf = temp;
					wmq_record[i].buflen = wmq_record[i].datalen;
					/* backstep i and retry this get */
					i--;
					continue;
				} else if (err == dp_RES_BADSIZE) {
					/* wmq_in[i] is corrupted */
					printf("%s: file corrupted!\n", wmq_in[i]->dir);
					exit(1);
				} else if (err != dp_RES_OK) {
					/* some sort of serious internal bug */
					printf("%s: wmq_get error:%d!\n", wmq_in[i]->dir, err);
					exit(1);
				}

				/* Got new data. */
				/* Check if it is the most recent, i.e. best estimate of now */
				if ((long)(wmq_record[i].timestamp - timestamp_now) > 0) {
					t_offset = (long)(wmq_record[i].timestamp - t_now);
					timestamp_now = wmq_record[i].timestamp;
					printf("t_now:%d + t_offset:%d = t:%d\n",
						t_now, t_offset, timestamp_now);
				}
				/* Check if it is the oldest, i.e. next to write.
				 * Ties are broken by serverTag (smallest first).
				 */
				if ((i_oldest == -1)
				||  (wmq_record[i].timestamp < wmq_record[i_oldest].timestamp)
				||  ((wmq_record[i].timestamp == wmq_record[i_oldest].timestamp)
				&&   (memcmp(wmq_record[i].serverTag,
					         wmq_record[i_oldest].serverTag, 4) < 0)))
					i_oldest = i;
				printf("%s: t:%d, read %d byte record, t:%d\n",
					wmq_in[i]->dir, timestamp_now, wmq_record[i].datalen,
					wmq_record[i].timestamp);
			}
		}

		if (i_oldest == -1) {
			printf("t:%d, n_empty:%d/%d\n", timestamp_now, n_empty, n_wmqs);
			sleep(RETRYTIME_GET);
			continue;
		} else if ((n_empty > 0)
		       &&  ((long)(timestamp_now - MAX_CLOCKSKEW -
		                   wmq_record[i_oldest].timestamp) < 0)) {
			printf("t:%d, n_empty:%d/%d oldest:t[%s]=%d\n", timestamp_now,
				n_empty, n_wmqs, wmq_in[i_oldest]->dir,
				wmq_record[i_oldest].timestamp);
			printf("Waiting until t:%d to write oldest\n",
				wmq_record[i_oldest].timestamp + MAX_CLOCKSKEW);
			sleep(RETRYTIME_GET);
			continue;
		} else {
			printf("t:%d, n_empty:%d/%d oldest:t[%s]=%d\n", timestamp_now,
				n_empty, n_wmqs, wmq_in[i_oldest]->dir,
				wmq_record[i_oldest].timestamp);
		}

		if (!wmq_out_restored) {
			time_t t;
			long offset;

			/* seek to end of file that would contain this timestamp */
			err = wmq_seek(wmq_out, wmq_record[i_oldest].timestamp, 0);
			if (err != dp_RES_OK) {
				printf("%s: error:%d seeking(t:0x%x, o:END)\n",
					wmq_out->dir, err, wmq_record[i_oldest].timestamp);
				exit(1);
			}
			wmq_out_restored = 1;
			/* out dat file points to end of queue. */
			err = wmq_savePosition(wmq_out, outdatfd);
			if (err != dp_RES_OK) {
				printf("%s: wmq_savePosition returns err:%d\n",
					wmq_out->dir, err);
				exit(1);
			}
			wmq_tell(wmq_out, &offset, &t);
			printf("%s: wrote(t:0x%x, o:0x%x)\n", wmq_out->dir, t, offset);
		}
		err = wmq_putServer(wmq_out, wmq_record[i_oldest].timestamp,
			wmq_record[i_oldest].serverTag, wmq_record[i_oldest].recordTag,
			wmq_record[i_oldest].buf, wmq_record[i_oldest].datalen);
		if (err != dp_RES_OK) {
			/* some sort of serious internal bug */
			printf("%s: wmq_put error:%d!\n", wmq_out->dir, err);
			exit(1);
		} else {
			time_t tin, tout;
			long offsetin, offsetout;

			/* out dat file points to end of queue. */
			err = wmq_savePosition(wmq_out, outdatfd);
			if (err != dp_RES_OK) {
				printf("%s: wmq_savePosition returns err:%d\n",
					wmq_out->dir, err);
				exit(1);
			}
			/* in dat files must point to record after last written. */
			err = wmq_savePosition(wmq_in[i_oldest], indatfd[i_oldest]);
			if (err != dp_RES_OK) {
				printf("%s: wmq_savePosition returns err:%d, not sync'd!\n",
					wmq_in[i_oldest]->dir, err);
				exit(1);
			}
			wmq_tell(wmq_in[i_oldest], &offsetin, &tin);
			printf("%s: read(t:0x%x, o:0x%x)\n",
				wmq_in[i_oldest]->dir, tin, offsetin);
			wmq_tell(wmq_out, &offsetout, &tout);
			printf("%s: wrote(t:0x%x, o:0x%x)\n",
				wmq_out->dir, tout, offsetout);
		}
		printf("%s: t:%d, wrote %d byte record, t:%d from %s\n",
			wmq_out->dir, timestamp_now, wmq_record[i_oldest].datalen,
			wmq_record[i_oldest].timestamp, wmq_in[i_oldest]->dir);

		/* Find oldest of remaining data.
		 * Ties are broken by serverTag (smallest first).
		 */
		wmq_record[i_oldest].timestamp = 0;
		i_oldest = -1;
		for (i = 0; i < n_wmqs; i++) {
			if (wmq_record[i].timestamp == 0)
				continue;
			if ((i_oldest == -1)
			||  (wmq_record[i].timestamp < wmq_record[i_oldest].timestamp)
			||  ((wmq_record[i].timestamp == wmq_record[i_oldest].timestamp)
			&&   (memcmp(wmq_record[i].serverTag,
						 wmq_record[i_oldest].serverTag, 4) < 0)))
				i_oldest = i;
		}
	}
}
