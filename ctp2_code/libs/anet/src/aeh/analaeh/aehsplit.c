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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "aneterr.h"
#include "aeh.h"
#include "aehlog.h"
#include "logprint.h"
#include "portable.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define aehsplit_DATFILE "aehsplit.dat"

static void exitUsage(char *msg)
{
	printf("\
Usage: aehsplit <-c crash_dir> <crashlog>\n\
	Read crashes from <crashlog>, write out analyzed text files to
	crash_dir in a format suitable for use by crshmail.pl.\n\
	<-c crash_dir>   Index unique crashes by signature in crash_dir.\n\
	%s\n", msg);
	exit(1);
}

static int mkdir_if_none(char *path, int mode)
{
	struct stat st;
	if (-1 == stat(path, &st)) {
		DPRINT(("mkdir_if_none: %s does not exist, creating\n", path));
		if (-1 == mkdir(path, mode)) {
			DPRINT(("mkdir_if_none: mkdir(%s, %#o) error:%d\n", path, mode, errno));
			return -1;
		}
	} else if (!S_ISDIR(st.st_mode)) {
		DPRINT(("mkdir_if_none: %s exists but is not a directory\n", path));
		return -1;
	} else if ((st.st_mode & mode) != mode) {
		DPRINT(("mkdir_if_none: %s exists but has mode:%#o != %#o\n", path, st.st_mode, mode));
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	dp_result_t err;
	aehlog_t aehlog;
	char BaseDir[aeh_MAX_PATH] = "";
	char LogPath[aeh_MAX_PATH] = "";
	char CrashPath[aeh_MAX_PATH] = "";
	char DatPath[aeh_MAX_PATH] = "";
	char tmp[80];
	char *c;
	FILE *fp_dat;
	long filelen;
	long offset;
	int bQuit = FALSE;
	int i;

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	if (argc <= 1)
		exitUsage("");
	c = strrchr(argv[0], aeh_PATH_DELIMIT);
	if (!c) {
		strcpy(BaseDir, ".");
	} else if (c - argv[0] >= aeh_MAX_PATH) {
		DPRINT(("path to executable is too long\n"));
		exit(1);
	} else {
		strncpy(BaseDir, argv[0], c - argv[0]);
	}
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'c':
				if (++i >= argc)
					exitUsage("-c requires a parameter\n");
				strncpy(CrashPath, argv[i], aeh_MAX_PATH);
				break;
			default:
				sprintf(tmp, "unknown option -%c", argv[i][1]);
				exitUsage(tmp);
				break;
			}
		} else if (LogPath[0]) {
			exitUsage("Only one crashlog can be read.\n");
		} else {
			strncpy(LogPath, argv[i], aeh_MAX_PATH);
		}
	}
	if (!LogPath[0] || !CrashPath[0])
		exitUsage("crashlog and crash_dir must be specified");

	if (-1 == mkdir_if_none(CrashPath, 0755)) {
		sprintf(tmp, "Error: can't open or create crash_dir %s\n", CrashPath);
		exitUsage(tmp);
	}
	err = aehlog_Create(LogPath, &aehlog);
	if (err != dp_RES_OK) {
		printf("aehlog_Create %s failed with error:%d\n", LogPath, err);
		exit(1);
	}
	filelen = aehlog_getfsize(&aehlog);

	/* We'd like to use a file creation time or other unique stamp
	 * to identify the atvilog.bin unambiguously, but we don't have one,
	 * so just check that the length hasn't gotten smaller and that
	 * there's a valid record at the offset.  If not, assume the 
	 * file has changed and start from the beginning.
	 */
	sprintf(DatPath, "%s/%s", CrashPath, aehsplit_DATFILE);
	if (NULL == (fp_dat = fopen(DatPath, "r"))) {
		/* No old file */
		offset = 0;
		if (filelen < 0L) {
			DPRINT(("aehlog_getfsize returns len:%d\n", filelen));
			sprintf(tmp, "Can't get crashlog %s length\n", LogPath);
			exitUsage(tmp);
		}
		DPRINT(("Datfile %s doesn't exist, creating new.\n", DatPath));
		if (NULL == (fp_dat = fopen(DatPath, "w"))) {
			DPRINT(("Datfile %s doesn't exist, and create failed.\n", DatPath));
			sprintf(tmp, "Can't write datfile %s\n", DatPath);
			exitUsage(tmp);
		}
	} else {
		long old_filelen;
		long old_offset;

		if (2 != fscanf(fp_dat, "l:%ld o:%ld", &old_filelen, &old_offset)) {
			DPRINT(("Datfile %s is not valid\n", DatPath));
			sprintf(tmp, "Datfile %s is not valid, delete and try again", DatPath);
			exitUsage(tmp);
		}	
		fclose(fp_dat);
		if (NULL == (fp_dat = fopen(DatPath, "w"))) {
			DPRINT(("Can't write to datfile %s\n", DatPath));
			sprintf(tmp, "Can't write datfile %s\n", DatPath);
			exitUsage(tmp);
		}
		if (old_filelen > filelen) {
			DPRINT(("new filelen:%ld < old:%ld, must have rolled over?\n",
				filelen, old_filelen));
			offset = 0;
		} else {
			err = aehlog_seek(&aehlog, old_offset);
			if ((err == aeh_RES_OK) || (err == aeh_RES_EMPTY)) {
				offset = old_offset;
			} else if (err == aeh_RES_BUG) {
				DPRINT(("aehlog_seek offset:%ld is invalid, must have rolled over?\n", offset));
				offset = 0;
			} else {
				DPRINT(("aehlog_seek offset:%ld err:%d\n", offset, err));
				exit(1);
			}
		}
	}
	DPRINT(("Writing filelen:%ld offset:%ld to %s\n", filelen, offset, DatPath));
	fprintf(fp_dat, "l:%ld o:%ld", filelen, offset);
	fclose(fp_dat);

	while (!bQuit) {
		aeh_t aeh;
		aeh_buf_t aehbuf;
		aehlog_t outlog;
		unsigned int ninst;
		long stackcrc;
		char signature[10];
		char outdir[aeh_MAX_PATH];
		char outbinfile[aeh_MAX_PATH];
		time_t now;
#if 0
		int bNewCrash;
		FILE *fp;
#endif

		err = aehlog_readExceptionRecord(&aehlog, &aehbuf, &ninst);
		if (err == dp_RES_EMPTY) {
			break;
		} else if (err != dp_RES_OK) {
			DPRINT(("aehlog_readExceptionRecord returns err:%d\n", err));
			break;
		}

		err = aeh_readInputStream(&aeh, &aehbuf);
		if (err == dp_RES_FULL) {
			DPRINT(("Read partial aeh record\n"));
		} else if (err != dp_RES_OK) {
			DPRINT(("aeh_readInputStream returns err:%d\n", err));
			break;
		}
		stackcrc = aeh_getSignature(&aeh);
		aeh_signature_toString(stackcrc, signature);
		now = time(NULL);
		DPRINT(("Read crash at t:%d: sig:%s sessType:%d build:%d.%d ninst:%d\n",
			now, signature, aeh.app.sessionType, aeh.app.major_version,
			aeh.app.minor_version, ninst));

		/* look up the crash by signature, sessionType */
		sprintf(outdir, "%s/%d", CrashPath, aeh.app.sessionType);
		if (-1 == mkdir_if_none(outdir, 0755)) {
			DPRINT(("Can't open or create directory %s\n", outdir));
			break;
		}
		sprintf(outdir, "%s/%d/%d.%d", CrashPath, aeh.app.sessionType,
			aeh.app.major_version, aeh.app.minor_version);
		if (-1 == mkdir_if_none(outdir, 0755)) {
			DPRINT(("Can't open or create directory %s\n", outdir));
			break;
		}
		sprintf(outdir, "%s/%d/%d.%d/bin", CrashPath, aeh.app.sessionType,
			aeh.app.major_version, aeh.app.minor_version);
		if (-1 == mkdir_if_none(outdir, 0755)) {
			DPRINT(("Can't open or create directory %s\n", outdir));
			break;
		}
		sprintf(outbinfile, "%s/%s.bin", outdir, signature);
		err = aehlog_Create(outbinfile, &outlog);
		if (err != dp_RES_OK) {
			printf("aehlog_Create %s failed with error:%d\n", outbinfile, err);
			break;
		}

#if 0
		fp = fopen(outbinfile, "r");
		if (fp) {
			fclose(fp);
			bNewCrash = FALSE;	/* have seen this crash before */
		} else {
			bNewCrash = TRUE;	/* haven't seen this crash before */
		}
		if (bNewCrash) {
			char outdatfile[aeh_MAX_PATH];
			char buf[4];

			sprintf(outdatfile, "%s/%s.dat", outdir, signature);
			if (NULL == (fp = fopen(outbinfile, "w"))) {
				DPRINT(("can't write new crash file %s\n", outbinfile));
				break;
			}
			buf[0] = dpGETLONG_FIRSTBYTE(now);
			buf[1] = dpGETLONG_SECONDBYTE(now);
			buf[2] = dpGETLONG_THIRDBYTE(now);
			buf[3] = dpGETLONG_FOURTHBYTE(now);
			if (1 != fwrite(fp, 4, 1, buf)) {
				DPRINT(("error writing timestamp to %s\n", outdatfile));
				fclose(fp);
				unlink(outdatfile);
			}
			fclose(fp);
		}
#endif
		err = aehlog_appendExceptionRecord(&outlog, &aehbuf, ninst);
		if (err != aeh_RES_OK) {
			DPRINT(("can't append crash to %s\n", outbinfile));
			break;
		}
		/* note where we are in the atvilog file so we can restart */
		if (NULL == (fp_dat = fopen(DatPath, "w"))) {
			DPRINT(("Write to %s failed\n", DatPath));
			break;
		} else {
			long new_offset = aehlog_tell(&aehlog);
			long new_filelen = aehlog_getfsize(&aehlog);

			if ((-1L == new_offset) || (-1L == new_filelen)) {
				DPRINT(("aehlog_tell:%d or getfsize:%d error\n", new_offset, new_filelen));
				break;
			}
			if (new_filelen < filelen) {
				/* getfsize does an fopen and will end up in the new file
				 * if a rollover has occurred.
				 * We're still in the old file until we aehlog_close.
				 */
				DPRINT(("new_filelen:%ld < old_filelen:%ld - rollover\n"));
				new_filelen = filelen;
			}
			filelen = new_filelen;
			offset = new_offset;
			DPRINT(("Writing filelen:%ld offset:%ld to %s\n", filelen, offset, DatPath));
			fprintf(fp_dat, "l:%ld o:%ld", filelen, offset);
			fclose(fp_dat);
		}

		aeh_Destroy(&aeh);	/* Clear the aeh in preparation for next read */
	}

	aehlog_close(&aehlog);
	return 0;
}
