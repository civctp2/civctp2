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
 Program to merge, filter, and/or sort crash log file.
--------------------------------------------------------------------------*/
 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "aehlog.h"
#include "dpprivy.h"
#include "dpcrc32.h"
#include "anet.h"

#define TMP_FILE "blag.tmp"
#define WRITE_LEN 2000

#define HASHSIZE 1024
#define AVG_RECORD_SIZE 64
#define NSORTKEY 5
enum sortindex { KEY_NINST, KEY_SESSTYPE, KEY_VERSION, KEY_FSTKNAME, KEY_FSTKADR };
static int sortlist[NSORTKEY];

typedef struct {
	unsigned long retaddr;
	unsigned int ninst;
	unsigned int buflen;
	unsigned char *buf; /* contains same info as aeh_buf_t's buf field */
} aehrec_entry_t;

typedef struct aehrec_s {
	aehrec_entry_t *aehrec;
	struct aehrec_s *next;
} aehrec_t;
static aehrec_t *aeharray[HASHSIZE];

static unsigned int naehsort = 0;
static unsigned int naehsortmax = 0;
typedef struct {
	unsigned long sortkey[NSORTKEY];
	aehrec_entry_t *aehrec; /* pointer to binary aeh record */
} aehsort_t;
static aehsort_t *aehsort; /* array of aehsort_t */

static int bQuiet = 0;

static void printUsage(void)
{
	if (!bQuiet)
		printf("\
Usage: procserv [options] file1 file2 file3...\n\
Reads given files; counts and merges adjacent identical records, writes\n\
result to atvilog.bin in current directory.\n\
If options are given, can delete records, remove portions of records, and/or\n\
sort.  If multiple sort options given, the first is the primary sort key, etc.\n\
Options:\n\
-sA         sort on module offset\n\
-sM         sort on module name\n\
-sN         sort on number of cases\n\
-sT         sort on session type\n\
-sV         sort on game version\n\
-t nnn      delete records with session type != nnn\n\
-iA         delete records which are just assertion failures\n\
-iF         delete records that look like internal msvcrt carshes (i.e. with flag values of 0 or EXCEPTION_NONCONTINUABLE)\n\
-dE         remove exception address from all records\n\
-dM         remove machine dependent information from all records\n\
-dS         remove session type from all records\n\
-dY         remove system description from all records\n\
-o outfile  Write to outfile instead of atvilog.bin\n\
-e          Read atvilog.bin from GetTempPath() directory\n\
-r          remove input files if error occurred while reading\n\
");
	exit(1);
}

static int writeSort(aehlog_t *aehlog)
{
	int err;
	unsigned int i;
	for (i = 0; i < naehsort; i++) {
		aeh_buf_t aehbuf;
		aehbuf.buflen = aehsort[i].aehrec->buflen;
		memcpy(aehbuf.buf, aehsort[i].aehrec->buf, aehbuf.buflen);
		err = aehlog_appendExceptionRecord(aehlog, &aehbuf, aehsort[i].aehrec->ninst);
		if (err != aeh_RES_OK) {
			printf("write error:%d\n", err);
			return 1;
		}
	}
	return 0;
}

static int comparecrc(const void *p1, const void *p2)
{
	if ((*((aeh_modfile_t **)p1))->crc < (*((aeh_modfile_t **)p2))->crc)
		return -1;
	else if ((*((aeh_modfile_t **)p1))->crc > (*((aeh_modfile_t **)p2))->crc)
		return 1;
	return 0;
}

static int comparekey(const void *p1, const void *p2)
{
	int i, cmp;
	for (i = 0; i < NSORTKEY && sortlist[i] >= 0; i++) {
		if (sortlist[i] == KEY_FSTKNAME) {
			if ((cmp = strcmp((char*)(((aehsort_t*)p1)->sortkey[sortlist[i]]),
						(char*)(((aehsort_t*)p2)->sortkey[sortlist[i]]))))
				return cmp;
		} else {
			if (((aehsort_t *)p1)->sortkey[sortlist[i]] < ((aehsort_t *)p2)->sortkey[sortlist[i]])
				return -1;
			else if (((aehsort_t *)p1)->sortkey[sortlist[i]] > ((aehsort_t *)p2)->sortkey[sortlist[i]])
				return 1;
		}
	}
	return 0;
}

/* sorts modules in an aeh record by module crc */
static void sortmod(aeh_t *aeh, aeh_buf_t *aehbuf)
{
	unsigned int i, j;
	aeh_modfile_t *modfile;
	aeh_modfile_t **pmodfile;

	if (!aeh || !aehbuf || aeh->nmod == 0) return;
	modfile = (aeh_modfile_t *)malloc(aeh->nmod * sizeof(aeh_modfile_t));
	pmodfile = (aeh_modfile_t **)malloc(aeh->nmod * sizeof(aeh_modfile_t *));
	memset(modfile, 0, aeh->nmod * sizeof(aeh_modfile_t));
	memset(pmodfile, 0, aeh->nmod * sizeof(aeh_modfile_t *));
	for (i = 0; i < aeh->nmod; i++)
		pmodfile[i] = &aeh->mod[i];
	qsort(pmodfile, aeh->nmod, sizeof(aeh_modfile_t *), comparecrc);
	for (i = 0; i < aeh->nmod; i++)
		modfile[i] = *(pmodfile[i]);
	for (i = 0; i < aeh->nmod; i++)
		memcpy(&aeh->mod[i], &modfile[i], sizeof(aeh_modfile_t));
	for (i = 0; i < aeh->nstk; i++) {
		if (aeh->stk[i].mod_index) {
			for (j = 0; j < aeh->nmod; j++) {
				if (aeh->stk[i].mod_index == (pmodfile[j] - aeh->mod) + 1) {
					aeh->stk[i].mod_index = j + 1;
					break;
				}
			}
		}
	}
	aeh_writeOutputStream(aeh, aehbuf);
	free(modfile);
	free(pmodfile);
}

/* sorts records by keys specified in command-line argument */
static void sortkey()
{
	unsigned int i;
	/* add ninst to sortkey field if sorting on ninst */
	for (i = 0; i < NSORTKEY; i++)
		if (sortlist[i] == KEY_NINST) break;
	if (i < NSORTKEY) {
		for (i = 0; i < naehsort; i++)
			aehsort[i].sortkey[KEY_NINST] = (unsigned long)(~(aehsort[i].aehrec->ninst));
	}
	qsort(aehsort, naehsort, sizeof(*aehsort), comparekey);
}

/* adds records to sort array to be sorted later, merging identical records */
static int arrayadd(aehlog_t *aehlog, unsigned short thisSessType, int bRetFlag, int bNoSessDesc, int bNoSysDesc, int bNoMachInfo, int bNoExcpAddress, int bNoAssert)
{
	int err;
	unsigned int ninst = 0;
	aeh_buf_t aehbuf;
	/* int n_rec = 1; */

	memset(&aehbuf, 0, sizeof(aeh_buf_t));
	aehDPRINT(("arrayadd: processing %s\n", aehlog->path));

	while ((err = aehlog_readExceptionRecord(aehlog, &aehbuf, &ninst)) == aeh_RES_OK) {
		aeh_t aeh;
		aehrec_t *paehrec;
		unsigned long crc;
		unsigned long retaddr = 0;
		unsigned short sessType = DP_ILLEGAL_SPECIES;
		unsigned long version = 0;
		char *firststkname;
		unsigned long firststkadr = 0;

		firststkname = strdup("ZZZZZZ");
		memset(&aeh, 0, sizeof(aeh_t));
		err = aeh_readInputStream(&aeh, &aehbuf);
		if (((err != aeh_RES_OK) && (err != aeh_RES_FULL)) ||
			((thisSessType != DP_ILLEGAL_SPECIES) && (aeh.app.sessionType != thisSessType)) ||
			(bNoAssert && (aeh.retcode == aeh_ASSERTION_CODE)) ||
			(bRetFlag && aeh.retflag && (aeh.retflag != EXCEPTION_NONCONTINUABLE))) {
			aeh_Destroy(&aeh);
			continue;
		}
		/* sort modules by crc in case the similar records happen to have
		 * the modules listed in a different order */
		sortmod(&aeh, &aehbuf);

		/* get session type, version, and stack info for sorting later */
		sessType = aeh.app.sessionType;
		version = ((unsigned long)(~aeh.app.major_version&0xffff))<<16;
		version |= ~aeh.app.minor_version&0xffff;
		if (aeh.nstk) {
			firststkadr = aeh.stk[0].offset_addr;
			if (aeh.stk[0].mod_index == 0) {
				if (aeh.stk[0].offset_addr > aeh_WIN32_ADDR_LIMIT)
					firststkname = strdup("ZZZZ(kernel)");
				else
					firststkname = strdup("ZZZZ(unknown)");
			} else if ((aeh.stk[0].mod_index <= aeh.nmod) &&
				aeh.mod[aeh.stk[0].mod_index - 1].name &&
				*aeh.mod[aeh.stk[0].mod_index - 1].name) {
				firststkname = strdup(aeh.mod[aeh.stk[0].mod_index - 1].name);
			}
		}
		retaddr = aeh.retaddr;
		/* printf("reading record %d: crash in %s:0x%x\n", n_rec, firststkname, retaddr); */
		if (bNoSysDesc)
			aeh_stripSysDesc(&aeh);
		if (bNoMachInfo && aeh.ninfo) {
			unsigned int id[1] = {aeh_WIN32info_ID};
			aeh_stripInfoId(&aeh, 1, id);
		}
		if (bNoExcpAddress)
			aeh_stripExceptionAddress(&aeh);
		aeh_writeOutputStream(&aeh, &aehbuf);
		aeh_Destroy(&aeh);

		/* check in hash table if already have this record; hashed by crc */
		crc = dp_crc32(aehbuf.buf, aehbuf.buflen) % HASHSIZE;
		/* get first value from hash entry then go thru list */
		paehrec = aeharray[crc];
		while (paehrec != NULL) {
			if (!memcmp(paehrec->aehrec->buf, aehbuf.buf, aehbuf.buflen)) {
				paehrec->aehrec->ninst += ninst;
				break;
			}
			paehrec = paehrec->next;
		}
		if (paehrec == NULL) {   /* no identical record found */
			/* allocate new entry in hash table and sort array */
			aehrec_t *aehrecbuf = (aehrec_t *)malloc(sizeof(aehrec_t));
			aehrecbuf->aehrec=(aehrec_entry_t *)malloc(sizeof(aehrec_entry_t));
			aehrecbuf->aehrec->buf=(unsigned char *)malloc(aehbuf.buflen);
			if (!aehrecbuf || !aehrecbuf->aehrec || !aehrecbuf->aehrec->buf) {
				printf("error: cannot allocate memory for aehrec, naehsort %d\n", naehsort);
				exit(1);
			}
			aehrecbuf->aehrec->retaddr = retaddr;
			aehrecbuf->aehrec->ninst = ninst;
			aehrecbuf->aehrec->buflen = aehbuf.buflen;
			memcpy(aehrecbuf->aehrec->buf, aehbuf.buf, aehbuf.buflen);
			aehrecbuf->next = aeharray[crc];
			aeharray[crc] = aehrecbuf;
			aehsort[naehsort].sortkey[KEY_VERSION] = (unsigned long)version;
			aehsort[naehsort].sortkey[KEY_SESSTYPE] = (unsigned long)sessType;
			aehsort[naehsort].sortkey[KEY_FSTKNAME] = (unsigned long)strdup(firststkname);
			aehsort[naehsort].sortkey[KEY_FSTKADR] = (unsigned long)firststkadr;
			aehsort[naehsort].aehrec = aehrecbuf->aehrec;
			naehsort++;

			/* reallocate aehsort if necessary*/
			if (naehsort >= naehsortmax) {
				aehsort_t *aehsortbuf;
				naehsortmax = (3 * naehsortmax) / 2;
				aehsortbuf = realloc((void*)aehsort, naehsortmax*sizeof(aehsort_t));
				if (!aehsortbuf) {
					printf("error: cannot allocate more memory to sort array\n");
					printf("tried to allocate %d elements", naehsortmax);
					aehlog_close(aehlog);
					exit(1);
				}
				aehsort = aehsortbuf;
				aehDPRINT(("increased sort array to %d\n", naehsortmax));
			}
		}
		if (firststkname)
			free(firststkname);
		memset(&aehbuf, 0, sizeof(aeh_buf_t));
		/* n_rec++; */
	}
	if (err != aeh_RES_EMPTY) {
		aehDPRINT(("arrayadd: error reading exception record"));
		return err;
	}
	return aeh_RES_OK;
}

/* initialize sort array */
static int initarray(size_t fsize)
{
	naehsortmax = fsize / AVG_RECORD_SIZE;
	aehsort = (aehsort_t *)malloc(naehsortmax * sizeof(aehsort_t));
	if (!aehsort) {
		printf("error allocating %d sort array elements\n", naehsortmax);
		return 1;
	}
	memset(aehsort, 0, naehsortmax * sizeof(aehsort_t));
	return 0;
}

/* free memory allocated by sort array */
static void destroyarray()
{
	unsigned int i;
	for (i = 0; i < HASHSIZE; i++) {
		aehrec_t *curr = aeharray[i];
		while (curr) {
			aehrec_t *next = curr->next;
			if (curr->aehrec->buf)
				free(curr->aehrec->buf);
			if (curr->aehrec)
				free(curr->aehrec);
			free(curr);
			curr = next;
		}
	}
	for (i = 0; i < naehsort; i++) {
		if (aehsort[i].sortkey[KEY_FSTKNAME])
			free((char *)aehsort[i].sortkey[KEY_FSTKNAME]);
	}
	free(aehsort);
}

int main(int argc, char **argv)
{
	int err, iarg, iargst, ikey;
	int bDefaultLog = 0;
	int bRetFlag = 0;
	int bNoAssert = 0;
	int bRemoveOnErr = 0;
	int bNoSessDesc = 0;
	int bNoSysDesc = 0;
	int bNoMachInfo = 0;
	int bNoExcpAddress = 0;
	unsigned short sessType = DP_ILLEGAL_SPECIES;
	size_t fsize = 0;
	aehlog_t aehlog;
	char tmpfile[aeh_MAX_PATH];   /* buffer file path */
	char outfile[aeh_MAX_PATH];   /* output file path */
	char *ptr;

	/* process command-line options */
	strcpy(outfile, "atvilog.bin");
	for (ikey = 0; ikey < NSORTKEY; ikey++)
		sortlist[ikey] = -1;
	iarg = 1;
	if (argc > 1) {
		while (iarg < argc) {
			if (*argv[iarg] == '-') {
				argv[iarg]++;
				switch (tolower(*argv[iarg])) {
					case 'd':
						switch (tolower(*++argv[iarg])) {
							case 'e':
								bNoExcpAddress = 1;
								break;
							case 'm':
								bNoMachInfo = 1;
								break;
							case 's':
								bNoSessDesc = 1;
								break;
							case 'y':
								bNoSysDesc = 1;
								break;
							default:
								printf("Error: bad flag -d%s\n", argv[iarg]);
								printUsage();
								break;
						}
						iarg++;
						break;
					case 'e':
						bDefaultLog = 1;
						iarg++;
						break;
					case 'i':
						switch (tolower(*++argv[iarg])) {
							case 'a':
								bNoAssert = 1;
								break;
							case 'f':
								bRetFlag = 1;
								break;
							default:
								printf("Error: bad flag -d%s\n", argv[iarg]);
								printUsage();
								break;
						}
						iarg++;
						break;
					case 'q':
						bQuiet = 1;
						iarg++;
						break;
					case 't':
						if (argc < iarg + 2) printUsage();
						sessType = atoi(argv[iarg+1]);
						iarg += 2;
						break;
					case 'o':
						if (argc < iarg + 2) printUsage();
						strcpy(outfile, argv[iarg+1]);
						iarg += 2;
						break;
					case 'r':
						bRemoveOnErr = 1;
						iarg++;
						break;
					case 's': 
					{
						int key;
						switch (tolower(*++argv[iarg])) {
							case 'a':
								key = KEY_FSTKADR;
								break;
							case 'm':
								key = KEY_FSTKNAME;
								break;
							case 'n':
								key = KEY_NINST;
								break;
							case 't':
								key = KEY_SESSTYPE;
								break;
							case 'v':
								key = KEY_VERSION;
								break;
							default:
								printf("Error: bad flag -s%s\n", argv[iarg]);
								printUsage();
								break;
						}
						for (ikey = 0; ikey < NSORTKEY; ikey++) {
							if (sortlist[ikey] == -1) {
								sortlist[ikey] = key;
								break;
							}
						}
						iarg++;
						break;
					}
					case 'h':
						printUsage();
						break;
					default:
						printf("Error: bad flag -%s\n", argv[iarg]);
						printUsage();
						break;
				}
			} else {
				break;
			}
		}
	}
	strcpy(tmpfile, outfile);
	if ((ptr = strrchr(tmpfile, aeh_PATH_DELIMIT)))
		ptr++;
	else
		ptr = &tmpfile[0];
	strcpy(ptr, TMP_FILE);

	/* sort file records, using tmpfile as write buffer file */
	iargst = iarg;
	if (iarg < argc) {
		for (iarg = iargst; iarg < argc; iarg++) {
			long size;
			err = aehlog_Create(argv[iarg], &aehlog);
			if (err != aeh_RES_OK) {
				printf("error doing aehlog_Create on input file %s\n", argv[iarg]);
				continue;
			}
			if ((size = aehlog_getfsize(&aehlog)) <= 0) {
				if (size < 0)
					printf("error getting file size for %s\n", aehlog.path);
				else
					printf("%s does not exist\n", argv[iarg]);
				aehlog_close(&aehlog);
				continue;
			}
			aehlog_close(&aehlog);
			fsize += size;
		}
		if (initarray(fsize))
			exit(1);
		for (iarg = iargst; iarg < argc; iarg++) {
			err = aehlog_Create(argv[iarg], &aehlog);
			if (err != aeh_RES_OK) {
				printf("error doing aehlog_Create on input file %s\n", argv[iarg]);
				continue;
			}
			err = arrayadd(&aehlog, sessType, bRetFlag, bNoSessDesc, bNoSysDesc, bNoMachInfo, bNoExcpAddress, bNoAssert);
			if (err) {
				printf("file read error occurred for %s\n", aehlog.path);
				if (bRemoveOnErr)
					aehlog_delete(&aehlog);
			}
			aehlog_close(&aehlog);
		}
	} else if (bDefaultLog) {
		long size;
		/* get default logfile name */
		err = aehlog_Create("", &aehlog);
		if (err != aeh_RES_OK) {
			printf("error getting default logfile name\n");
			exit(1);
		}
		if ((size = aehlog_getfsize(&aehlog)) <= 0) {
			if (size < 0)
				printf("error getting file size for %s\n", aehlog.path);
			else
				printf("%s does not exist\n", aehlog.path);
			aehlog_close(&aehlog);
			exit(1);
		}
		fsize = (size_t)size;
		if (initarray(fsize))
			exit(1);
		err = arrayadd(&aehlog, sessType, bRetFlag, bNoSessDesc, bNoSysDesc, bNoMachInfo, bNoExcpAddress, bNoAssert);
		if (err) {
			printf("file read error ocurred for %s\n", aehlog.path);
			if (bRemoveOnErr)
				aehlog_delete(&aehlog);
		}
		aehlog_close(&aehlog);
	} else {
		printf("Need to specify path to log file or -e option\n");
		printUsage();
	}
	sortkey();
	err = aehlog_Create(tmpfile, &aehlog);
	if (err != aeh_RES_OK) {
		printf("error doing aehlog_Create on tmp output file\n");
		exit(1);
	}
	/* remove any existing file since writeSort appends */
	aehlog_delete(&aehlog);
	writeSort(&aehlog);
	aehlog_close(&aehlog);
	destroyarray();

	/* see if tmpfile exists and nonzero size; if yes, rename to outfile */
	{	FILE *fp;
		if ((fp = fopen(tmpfile, "rb"))) {
			fseek(fp, 0L, SEEK_END);
			if (ftell(fp) > 0)
				err = 0;
			else
				err = 1;
			fclose(fp);
			if (!err) {
				remove(outfile);
				rename(tmpfile, outfile);
			}
		}
	}
	remove(tmpfile);
	return 0;
}
