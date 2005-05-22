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

/* analyzes exception log files */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef UNIX
#include <unistd.h>
#include <fcntl.h>
#define _stat			stat
#else
#ifdef _WIN32
#include <direct.h>
#define mkdir(dir,mode) _mkdir(dir)
#endif
#endif
#include "aehlog.h"

#define BUFFER_SIZE 4096
int bQuiet = 0;

void printUsage(void)
{
	if (!bQuiet)
		printf("\
Usage: analcrsh [options] [-o outfile1] infile1 [-o outfile2] infile2...\n\
Options:\n\
-q               quiet\n\
-r rel_path      output written to directory at rel_path from input (default is cwd)\n\
-d               equivalent to -r .\n\
-u               only analyze if the output file is out of date\n\
-s session_type  only output exception records of given session type\n\
-m mapcatfile    Mapfile catalog (default is modcrc.txt in same dir as pgm)\n\
-p               Read input filenames from stdin, one per line\n\
Reads crash data files with .bin filename suffix (possibly downloaded from a\n\
game server), and outputs human-readable version (with .txt filename suffix).\n\
");
/* -e read input from 'standard place' $TMPDIR/atvilog.bin */
	exit(1);
}

/* get rid of foo/../ */
static void canonicalize_path(char *path)
{
	char dotdot[] = { aeh_PATH_DELIMIT, '.', '.', aeh_PATH_DELIMIT, '\0' };
	char *p, *s;

	/* Remove all internal 'dir/../' */
	while ((p = strstr(path, dotdot))) {
		*p = '\0';
		s = strrchr(path, aeh_PATH_DELIMIT);
		if (s)
			s++;
		else
			s = path;
		strcpy(s, p + strlen(dotdot));
	}
	/* Remove initial './' */
	if ((path[0] == '.') && (path[1] == aeh_PATH_DELIMIT)) {
		memmove(path, path + 2, strlen(path) - 1);
	}
}

/* make this directory and all missing parents */
static int mkdirs_to(char *path, int mode)
{
	char *p;
	int err;

	/* easy case: all parents exist */
	if ((0 == mkdir(path, mode)) || (errno == EEXIST))
		return 0;
	/* else try to make parent directory */
	p = strrchr(path, aeh_PATH_DELIMIT);
	if (!p)
		return -1;	/* can't make outermost parent */
	*p = '\0';
	err = mkdirs_to(path, mode);
	*p = aeh_PATH_DELIMIT;
	if (err)
		return err;
	/* once the parent exists, try making the directory again */
	return mkdir(path, mode);
}

/* Returns the last modification time of a file or 0 on error. */
static time_t get_mtime(char *file)
{
	struct _stat st;
	if (-1 == _stat(file, &st))
		return 0;
	return st.st_mtime;
}

int main(int argc, char **argv)
{
	FILE *fOut;
	int i, bDone;
	unsigned int err, ninst, bDefaultLog = 0, bUseRelDir = 0;
	int bReadFilesFromStdin = 0, bUpdate = 0;
	aeh_t aeh;
	aeh_mapcat_t aehmapcat;
	aeh_buf_t aehbuf;
	aehlog_t aehlog;
	unsigned int len;
	char dat[BUFFER_SIZE];
	int sessType = 0;
	char logpath[aeh_MAX_PATH];
	char catpath[aeh_MAX_PATH];
	char outpath[aeh_MAX_PATH];
	char reloutdir[aeh_MAX_PATH];

	catpath[0] = '\0';
	if (argc < 2) {
		printf("Need to specify path to log file or -p or -e options\n");
		printUsage();
	}
	i = 1;
	bDone = 0;
	while (!bDone && (i < argc)) {
		if (*argv[i] == '-') {
			switch (tolower(*(argv[i]+1))) {
				case 'u':
					bUpdate = 1;
					i++;
					break;
				case 'p':
					bReadFilesFromStdin = 1;
					i++;
					break;
				case 'r':
					bUseRelDir = 1;
					if (argc < i + 2) printUsage();
					strcpy(reloutdir, argv[i+1]);
					i += 2;
					break;
				case 'd':
					bUseRelDir = 1;
					sprintf(reloutdir, ".");
					i++;
					break;
				case 'e':
					bDefaultLog = 1;
					i++;
					break;
				case 'q':
					bQuiet = 1;
					i++;
					break;
				case 'm':
					if (argc < i + 2) printUsage();
					strcpy(catpath, argv[i+1]);
					i += 2;
					break;
				case 'o':
					bDone = 1;
					break;
				case 's':
					if (argc < i + 2) printUsage();
					sessType = atoi(argv[i+1]);
					i += 2;
					break;
				case 'h':
					printUsage();
					break;
				default:
					printf("Error: bad flag\n");
					printUsage();
					break;
			}
		} else {
			bDone = 1;
			break;
		}
	}
	if ((i >= argc) && !bDefaultLog && !bReadFilesFromStdin) {
		printf("Error: nothing to do!\n");
		printUsage();
	}

	/* determine mapfile catalog and initialize mapfile structure */
	if (!(catpath[0])) {
		char *ptr;
#ifdef _WIN32
		GetModuleFileName(NULL, catpath, aeh_MAX_PATH);
#else
		strcpy(catpath, argv[0]);
#endif
		if (catpath[strlen(catpath) - 1] == aeh_PATH_DELIMIT)
			catpath[strlen(catpath) - 1] = '\0';
		if ((ptr = strrchr(catpath, aeh_PATH_DELIMIT)))
			*(ptr + 1) = '\0';
		else
			catpath[0] = '\0';
		strcat(catpath, "modcrc.txt");
	}
	err = aeh_mapcat_Create(&aehmapcat, catpath);
	if (err != aeh_RES_OK) {
		if (err == aeh_RES_BUG) {
			printf("Error: can't open catalog file %s\n", catpath);
			printUsage();
		} else
			printf("Error: err %d during aeh_mapcat_Create\n", err);
		exit(1);
	}

	/* loop over output file names and exception logs, getting records */
	while ((i < argc) || bDefaultLog || bReadFilesFromStdin) {
		logpath[0] = '\0';
		outpath[0] = '\0';

		/* Default: read files from commandline */
		if (i < argc) {
			/* Check to see if there's a -o outfile before the filename */
			if (argv[i][0] == '-') {
				if ((i+3 > argc) || strcmp(argv[i], "-o")) {
					aeh_mapcat_Destroy(&aehmapcat);
					printUsage();
				}
				i++;
				strcpy(outpath, argv[i]);
				i++;
			}
			strcpy(logpath, argv[i]);
			i++;
			err = aehlog_Create(logpath, &aehlog);
		} else if (bReadFilesFromStdin) {
			/* If none on commandline, check stdin */
			if (!fgets(logpath, aeh_MAX_PATH, stdin)) {
				bReadFilesFromStdin = 0;
				continue;
			} else {
				FILE *fp_test;
				int ilast;
				for (ilast = strlen(logpath) - 1; isspace(logpath[ilast]); ilast--)
					logpath[ilast] = '\0';
				fp_test = fopen(logpath, "r");
				if (fp_test) {
					fclose(fp_test);
				} else {
					printf("log:%s does not exist!\n", logpath);
					exit(1);
				}
				err = aehlog_Create(logpath, &aehlog);
			}
		} else if (bDefaultLog) {
			/* If none in stdin, check for 'standard place' */
			err = aehlog_Create("", &aehlog);
			bDefaultLog = 0;
		} else {
			printf("No log files to process!\n");
			aeh_mapcat_Destroy(&aehmapcat);
			printUsage();
		}
		if (err != aeh_RES_OK) {
			printf("aehlog_Create error %d\n", err);
			continue;
		}
		/* if there's no -o option for this filename, calculate full path to output file */
		if (!(outpath[0])) {
			char *ptr;
			if (bUseRelDir) {
				char outdir[aeh_MAX_PATH];

				/* for aehlog.path of form dir1/dir2/name.bin,
				 * make outpath = dir1/dir2/reloutdir/name.txt
				 */
				strcpy(outdir, aehlog.path);
				if ((ptr = strrchr(outdir, aeh_PATH_DELIMIT)))
					ptr++;
				else
					ptr = outdir;
				strcpy(ptr, reloutdir);
				canonicalize_path(outdir);
				if (mkdirs_to(outdir, 0755)) {
					printf("can't mkdirs_to %s\n", outdir);
					continue;
				}
				sprintf(outpath, "%s%c%s", outdir, aeh_PATH_DELIMIT,
					aehlog.path + (ptr - outdir));
			} else {
				/* No options were given, so put it in the current directory. */
				if ((ptr = strrchr(aehlog.path, aeh_PATH_DELIMIT))) {
					ptr++;
					strcpy(outpath, ptr);
				} else
					strcpy(outpath, aehlog.path);
			}
			if ((ptr = strrchr(outpath, '.')))
				strcpy(ptr, ".txt");
			else
				strcat(outpath, ".txt");
		}
		if (bUpdate) {
			time_t bintime = get_mtime(aehlog.path);
			time_t txttime = get_mtime(outpath);
			/* Only write text file if the bin file is newer */
			if (bintime && txttime && (bintime <= txttime)) {
				if (!bQuiet)
					printf("%s is up to date\n", outpath);
				continue;
			}
		}
		fOut = fopen(outpath, "w");
		if (!fOut) {
			printf("Error: can't open output file %s\n", outpath);
			aehlog_close(&aehlog);
			continue;
		}
		if (!bQuiet)
			printf("Writing output to %s\n", outpath);
		while ((err = aehlog_readExceptionRecord(&aehlog, &aehbuf, &ninst)) == aeh_RES_OK) {
			err = aeh_readInputStream(&aeh, &aehbuf);
			if (err != aeh_RES_OK && err != aeh_RES_FULL) {
				aehDPRINT(("error %d getting aeh\n", err));
				aeh_Destroy(&aeh);
				continue;
			} else if (err == aeh_RES_FULL)
				aehDPRINT(("incomplete aeh record\n"));
			if (sessType && (aeh.app.sessionType != sessType)) {
				aeh_Destroy(&aeh);
				continue;
			}
			err = aeh_getAllInfo(&aeh, &aehmapcat);
			if (err != aeh_RES_OK)
				aehDPRINT(("error %d getting all info\n", err));
			fprintf(fOut, "******* Crash stat *******\n");
			fprintf(fOut, "Number of cases: %d\n", ninst);
			len = BUFFER_SIZE;
			aeh_toString(&aeh, dat, &len);
			fprintf(fOut, "%s\n", dat);
			fflush(fOut);
			aeh_Destroy(&aeh);
		}
		if (err != aeh_RES_EMPTY)
			printf("readExceptionRecord error %d\n", err);
		else if (!aehlog.fp)
			printf("Error: can't find exception log %s\n", aehlog.path);
		else if (!bQuiet)
			printf("completed successfully\n");
		aehlog_close(&aehlog);
		fclose(fOut);
	}
	aeh_mapcat_Destroy(&aehmapcat);
	/*return (err != aeh_RES_EMPTY);*/
	return 0;
}
