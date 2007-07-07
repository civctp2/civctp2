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

/* creates catalog file of mapfiles */
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif
#ifdef UNIX 
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "aeh.h"
#include "dpcrc32.h"

#define PORTABLE_PATH_DELIMIT '/'
#define modcrc_MAX_LINE_LENGTH 4096

/*#undef aehDPRINT
#define aehDPRINT(x) printf x*/

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef _WIN32
#define getcwd(x, y) _getcwd(x, y)
#endif

static char rootpath[aeh_MAX_PATH];

/*------------------------------------------------------------------------------
 This list of filename crc's allows us to avoid recalculating crc's for any
 files that were already in modcrc.txt.
------------------------------------------------------------------------------*/
typedef struct modcrc_mapfilecat_s {
	unsigned long *fnamecrc;   /* a pointer to an array of the crc's of all the
								  filenames read from the current modcrc.txt */
	unsigned n;                /* the current malloc'd size of the array */
} modcrc_mapfilecat_t;


void printUsage(void)
{
	printf("\
Usage: modcrc [options]\n\
Updates mapfile catalog.\n\
Catalog is kept in a file modcrc.txt for use by analcrsh.\n\
It contains the CRC and relative path to each module we have a mapfile for.\n\
By default, recursively searches subdirectories of directory containing\n\
modcrc.txt to find modules (*.dll, *.exe) and their mapfiles (*.map).\n\
Options:\n\
   -h     help\n\
   -m DIR modcrc.txt lives in directory DIR (default is cwd)\n\
   -e     modcrc.txt lives in directory containing modcrc executable\n\
   -f     Read module paths from stdin instead of recursively searching dir\n\
");

	exit(1);
}


/* Convert path delimiter to portable */
static void convertPathDelimiters(char *s)
{
	while (*s) {
		if (*s == aeh_PATH_DELIMIT)
			*s = PORTABLE_PATH_DELIMIT;
		s++;
	}
}

/*------------------------------------------------------------------------------
 appends an entry into the crc file (fp) for the file at path/fname
 does not update file that already have an entry (i.e. are listed in files_seen)
------------------------------------------------------------------------------*/
void test_addmod(char *fpath, modcrc_mapfilecat_t *files_seen, FILE *fp)
{
	char mappath[aeh_MAX_PATH];
	char *pbuf;
	FILE *f;
	unsigned long this_fnamecrc;
	unsigned int i;

	/* Convert path delimiter to portable / before calculating CRC */
	convertPathDelimiters(fpath);

	/* construct full path to map file into mappath */
	strcpy(mappath, fpath);
	pbuf = strrchr(mappath, '.');
	if (!pbuf)
		return;
	strcpy(pbuf, ".map");

	/* Make sure the map file exists */
	if ((f = fopen(mappath, "r")) == NULL) {
		aehDPRINT(("test_addmod: error: %s not found\n", mappath));
		return;
	} 
	fclose(f);

	/* Strip off directory prefix, and calculate CRC of filename (not file) */
	pbuf = strstr(mappath, rootpath);
	if (pbuf) {
		pbuf += strlen(rootpath);
	} else if ((mappath[0] == '.') && (mappath[1] == PORTABLE_PATH_DELIMIT)) {
		pbuf = mappath + 2;
	} else
		pbuf = mappath;

	this_fnamecrc = dp_crc32((unsigned char*)pbuf, strlen(pbuf));
	
	/* check to see if fnamecrc (the crc of the filename) is already in 
	 * modcrc.txt.  If so, skip this file.
	 */
	for (i=0; i<files_seen->n; i++) {
		if (files_seen->fnamecrc[i] == this_fnamecrc) {
			aehDPRINT(("have seen %s, skipping\n", pbuf));
			return;
		}
	}

	/* Otherwise, calculate CRC of file contents */
	f = fopen(fpath, "rb");		
	if (f) {
		unsigned long crc = dp_fil_crc32(f);
		fclose(f);
		fprintf(fp, "%08lx %s\n", crc, pbuf);
		printf("adding %s\n", pbuf);
		aehDPRINT(("adding %s\n", pbuf));
	}
}

#ifdef _WIN32
/*------------------------------------------------------------------------------
 Return 1 if it is a valid directory (and isn't .\ or ..\), 0 otherwise
------------------------------------------------------------------------------*/
BOOLEAN validdir(LPWIN32_FIND_DATA lpffd)
{
	return (((lpffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) &&
			strcmp(lpffd->cFileName, ".") && strcmp(lpffd->cFileName, ".."));
}


/*------------------------------------------------------------------------------
 Recursively search (path) and its sub-directories for files matching (pattern)
 call test_addmod() to add crc entries for any new files into the crc file (fp) 
------------------------------------------------------------------------------*/
void test_getmod(const char *path, char *pattern, modcrc_mapfilecat_t *files_seen, FILE *fp)
{
	HANDLE hFile;
	WIN32_FIND_DATA wfd;
	LPWIN32_FIND_DATA lpffd = &wfd;
	char buf[aeh_MAX_PATH];
	char *ptmp;

	/* Fill buf with wildcard for this directory */
	strcpy(buf, path);
	aehDPRINT(("looking in path %s\n", buf));
	if (ptmp = strrchr(buf, PORTABLE_PATH_DELIMIT))
		ptmp++;
	else
		ptmp = buf;
	strcpy(ptmp, pattern);
	aehDPRINT(("looking for %s\n", buf));

	/* Process matching files in this directory */
	hFile = FindFirstFile((LPCTSTR)buf, lpffd);
	if (hFile != INVALID_HANDLE_VALUE) do {
		char fpath[aeh_MAX_PATH];
		char *pbuf;
		/* tack the filename onto the path */
		strcpy(fpath, buf);
		pbuf = strrchr(fpath, PORTABLE_PATH_DELIMIT);
		if (pbuf)
			pbuf++;
		else
			pbuf = &(fpath[0]);
		strcpy(pbuf, lpffd->cFileName);
		/* Add the file's crc to modcrc.txt */
		test_addmod(fpath, files_seen, fp);
	} while (FindNextFile(hFile, lpffd));

	/* Recurse */
	strcpy(ptmp, "*.*");
	aehDPRINT(("looking for %s\n", buf));
	
	hFile=FindFirstFile(buf, lpffd); 
	if (hFile != INVALID_HANDLE_VALUE) do { 
		if (validdir(lpffd)) {
			strcpy(ptmp, lpffd->cFileName);
			buf[strlen(buf) + 1] = '\0';
			buf[strlen(buf)] = PORTABLE_PATH_DELIMIT;
			aehDPRINT(("found subdirectory %s\n", buf));
			test_getmod(buf, pattern, files_seen, fp);
		}
	} while (FindNextFile(hFile, lpffd));
	FindClose(hFile);
}
#endif

/*------------------------------------------------------------------------------
 Read the crc file (fp) and record all already indexed files into (files_seen)
 Returns: aeh_RES_NOMEM if out of memory (a partial files_seen is possible)
          aeh_RES_OK if successful
------------------------------------------------------------------------------*/
int test_getfilesseen(modcrc_mapfilecat_t *files_seen, FILE *fp)
{
	char buf[modcrc_MAX_LINE_LENGTH];
	char path[aeh_MAX_PATH];
	unsigned long fnamecrc, crc;
	unsigned long *temp;

	while (fgets(buf, modcrc_MAX_LINE_LENGTH, fp) != NULL)
		if (sscanf(buf, "%lx %[^\n\r\f]", &crc, path) == 2) {
			fnamecrc = dp_crc32((unsigned char *)path, strlen(path));
			files_seen->n++;
			aehDPRINT(("read %s, now seen\n", path));
			if ((temp = (unsigned long *)realloc(files_seen->fnamecrc, files_seen->n * sizeof(long))) == NULL) {
				files_seen->n--;
				return aeh_RES_NOMEM;
			} else {
				files_seen->fnamecrc = temp;
				files_seen->fnamecrc[files_seen->n-1] = fnamecrc;
			}
		}
	return aeh_RES_OK;
}


int main(int argc, char **argv)
{
	FILE *fp;
	char tmppath[aeh_MAX_PATH];
	modcrc_mapfilecat_t files_seen;
	int readNamesFromStdin;
	int i;
	int len;

	files_seen.fnamecrc = (unsigned long*)NULL;
	files_seen.n = 0;
	
	/* Set defaults */
	readNamesFromStdin = FALSE;
	if (!getcwd(rootpath, sizeof(rootpath))) {
		perror("Can't get cur dir");
		exit(1);
	}

	/* Parse command line */
	for (i=1; i<argc; i++) {
		if (argv[i][0] != '-') {
			printUsage();
		} else switch(argv[i][1]) {
#ifdef _WIN32
		case 'e':
			{
			char *ptr;

			GetModuleFileName(NULL, rootpath, aeh_MAX_PATH);
			if (rootpath[strlen(rootpath) - 1] == aeh_PATH_DELIMIT)
				rootpath[strlen(rootpath) - 1] = '\0';
			if (ptr = strrchr(rootpath, aeh_PATH_DELIMIT))
				*(ptr + 1) = '\0';
			else
				rootpath[0] = '\0';
			}
			break;
#endif
		case 'm':
			if (i >= (argc-1))
				printUsage();
			strcpy(rootpath, argv[++i]);
			break;
		case 'f':
			readNamesFromStdin = TRUE;
			break;
		case 'h':
		default:
			printUsage();
			break;
		}
	}
	if (rootpath[0] == 0)
		printUsage();

	/* Always use forward slashes as directory separators,
	 * so modcrc.txt is portable
	 */
	convertPathDelimiters(rootpath);
	/* Make sure rootpath ends in a slash */
	len = strlen(rootpath);
	if (rootpath[len - 1] != PORTABLE_PATH_DELIMIT) {
		rootpath[len] = PORTABLE_PATH_DELIMIT;
		rootpath[len + 1] = 0;
	}

	/* Directory containing modcrc.txt now in rootpath.  Load it, and open
	 * it for append.
	 */
	strcpy(tmppath, rootpath);
	strcat(tmppath, "modcrc.txt");
	printf("Opening %s\n", tmppath);
	fp = fopen(tmppath, "r");
	if (fp) {
		if (test_getfilesseen(&files_seen, fp) != aeh_RES_OK) {
			printf("Error: Out of memory for reading from %s\n", tmppath);
			printUsage();
		}
		fclose(fp);
		if (!(fp = fopen(tmppath, "a"))) {
			printf("Error: cannot open %s to append output\n", tmppath);
			printUsage();
		}
	} else {
		printf("%s not found for input, building a new one\n", tmppath);
		if (!(fp = fopen(tmppath, "w"))) {
			printf("Error: cannot open %s for writing\n", tmppath);
			printUsage();
		}
	}

	if (readNamesFromStdin) {
		/* Read path to each module from stdin */
		char buf[aeh_MAX_PATH];

		while (fgets(buf, sizeof(buf), stdin)) {
			char *p;
			
			p = strrchr(buf, '\n');
			if (p)
				*p = 0;
			test_addmod(buf, &files_seen, fp);
		}
	} else {
#ifdef _WIN32
		/* Recursively search rootpath and all subdirectories for modules
		 * with maps, and add them to modcrc.txt.
		 */
		strcpy(tmppath, rootpath);
		test_getmod(tmppath, "*.dll", &files_seen, fp);
		strcpy(tmppath, rootpath);
		test_getmod(tmppath, "*.exe", &files_seen, fp);
#else
		printf("Not supported.\n");
		exit(1);
#endif
	}
	fclose(fp);
	return 0;
}
