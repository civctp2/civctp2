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
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include "win32dirent.h"
#else
#include <dirent.h>
#endif

#include "anet.h"
#include "wmqfile.h"

/*--------------------------------------------------------------------------
 Selector function to pick out *.wmq from a directory for use with scandir()
--------------------------------------------------------------------------*/
static int is_wmqfile(const struct dirent *dirent)
{
	return wmqfile_iswmqfile(dirent->d_name);
}

/*--------------------------------------------------------------------------
 Returns 1 if <filename> looks like a wmq filename or 0 if not.
--------------------------------------------------------------------------*/
int wmqfile_iswmqfile(const char *filename)
{
	int i;

	/* wmq filenames are in format ########.wmq or ########-##?#?.wmq
	 * where the first 8 characters are the gmt date YYYYMMDD, optionally
	 * followed by a - and 1 to 3 digits identifying which file on that
	 * date this is, followed by ".wmq"
	 */
	for (i = 0; i < 8; i++)
		if (!isdigit(filename[i]))
			return 0;
	if (filename[i] == '-') {
		i++;
		if (!isdigit(filename[i]))
			return 0;
		for (i++; i < 12; i++)
			if (!isdigit(filename[i]))
				break;
	}
	if (strcmp(filename + i, ".wmq"))
		return 0;
	return 1;
}

/*--------------------------------------------------------------------------
 Puts the filename corresponding to <when> in <filename>, which will
 have a maximum of 16 characters (plus one for a terminating '\0').
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_forTime(time_t when, char *filename)
{
	struct tm *gmt = gmtime(&when);
	long files_per_day;
	int n_digits;

	files_per_day = (long)(86400 / wmqfile_ROLLOVER_TIME)
		+ ((86400 % wmqfile_ROLLOVER_TIME) ? 1 : 0);
	if ((files_per_day > 1000) || (files_per_day < 1))
		return dp_RES_BUG;
	n_digits = ((files_per_day <= 1) ? 0 :
	            ((files_per_day <= 10) ? 1 :
	             ((files_per_day <= 100) ? 2 : 3)));
	if (n_digits > 0) {
		int filenum;
		filenum = ((long)(gmt->tm_hour * 3600 + gmt->tm_min*60 + gmt->tm_sec) /
				         wmqfile_ROLLOVER_TIME);
		sprintf(filename, "%04d%02d%02d-%0*d.wmq", 1900+gmt->tm_year,
			gmt->tm_mon+1, gmt->tm_mday, n_digits, filenum);
	} else {
		sprintf(filename, "%04d%02d%02d.wmq", 1900+gmt->tm_year,
			gmt->tm_mon+1, gmt->tm_mday);
	}
	DPRINT(("wmqfile_forTime: for t:%d, file is %s\n", when, filename));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Find the first wmq file sequentially after <file> in <dir>, and return its
 name in <nextfile>.  Note that <file> itself need not exist.
 Returns dp_RES_OK and <nextfile> is set on success,
 		 dp_RES_EMPTY and <nextfile> is unchanged if no file follows <file>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_next(const char *dir, const char *file, char *nextfile)
{
	struct dirent **dirlist;
	int n_match = 0;

	n_match = scandir(dir, &dirlist, is_wmqfile, alphasort);
	DPRINT(("wmqfile_next: scandir matched %d files in dir:%s\n", n_match, dir));
	if (n_match > 0) {
		int i;

		/* Start at last, look for first before or at <file>, return next */
		for (i = n_match - 1; i >= 0; i--) {
			if (strcmp(dirlist[i]->d_name, file) <= 0) {
				i++;
				if (i < n_match) {
					strcpy(nextfile, dirlist[i]->d_name);
					DPRINT(("wmqfile_next: next file is #%d:%s\n", i, nextfile));
					return dp_RES_OK;
				} else {
					DPRINT(("wmqfile_next: no next file\n"));
					return dp_RES_EMPTY;
				}
			}
		}
		/* All files were greater than <file>, so return the first */
		strcpy(nextfile, dirlist[0]->d_name);
		DPRINT(("wmqfile_next: next file is #0:%s\n", nextfile));
		return dp_RES_OK;
	} else if (n_match < 0) {
		DPRINT(("wmqfile_next: scandir out of memory!\n"));
		return dp_RES_NOMEM;
	}
	DPRINT(("wmqfile_next: no files\n"));
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Find the last wmq file sequentially before <file> in <dir>, and return
 its name in <prevfile>.  Note that <file> itself need not exist.
 Returns dp_RES_OK and <prevfile> is set on success,
 		 dp_RES_EMPTY and <prevfile> is unchanged if no file preceeds <file>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_prev(const char *dir, const char *file, char *prevfile)
{
	struct dirent **dirlist;
	int n_match = 0;

	n_match = scandir(dir, &dirlist, is_wmqfile, alphasort);
	DPRINT(("wmqfile_prev: scandir matched %d files in dir:%s\n", n_match, dir));
	if (n_match > 0) {
		int i;

		/* Start at last, look for first before <file>, return it */
		for (i = n_match-1; i >= 0; i--) {
			if (strcmp(dirlist[i]->d_name, file) < 0) {
				strcpy(prevfile, dirlist[i]->d_name);
				DPRINT(("wmqfile_prev: previous file is #%di:%s\n", i, prevfile));
				return dp_RES_OK;
			}
		}
		DPRINT(("wmqfile_prev: no previous file\n"));
		return dp_RES_EMPTY;
	} else if (n_match < 0) {
		DPRINT(("wmqfile_prev: scandir out of memory!\n"));
		return dp_RES_NOMEM;
	}
	DPRINT(("wmqfile_prev: no files\n"));
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Find the first wmq file sequentially in <dir>, and return its name in
 <firstfile>.
 Returns dp_RES_OK and <firstfile> is set on success,
 		 dp_RES_EMPTY and <firstfile> is unchanged if no wmq files are in <dir>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_first(const char *dir, char *firstfile)
{
	struct dirent **dirlist;
	int n_match = 0;

	n_match = scandir(dir, &dirlist, is_wmqfile, alphasort);
	DPRINT(("wmqfile_first: scandir matched %d files in dir:%s\n", n_match, dir));
	if (n_match > 0) {
		/* alpha sorting will put files in time order */
		strcpy(firstfile, dirlist[0]->d_name);
		DPRINT(("wmqfile_first: first file is %s\n", firstfile));
		return dp_RES_OK;
	} else if (n_match < 0) {
		DPRINT(("wmqfile_first: scandir out of memory!\n"));
		return dp_RES_NOMEM;
	}
	DPRINT(("wmqfile_first: no files\n"));
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Find the last wmq file sequentially in <dir>, and return its name in
 <lastfile>.
 Returns dp_RES_OK and <lastfile> is set on success,
 		 dp_RES_EMPTY and <lastfile> is unchanged if no wmq files are in <dir>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_last(const char *dir, char *lastfile)
{
	struct dirent **dirlist;
	int n_match = 0;

	n_match = scandir(dir, &dirlist, is_wmqfile, alphasort);
	DPRINT(("wmqfile_last: scandir matched %d files in dir:%s\n", n_match, dir));
	if (n_match > 0) {
		/* alpha sorting will put files in time order */
		strcpy(lastfile, dirlist[n_match-1]->d_name);
		DPRINT(("wmqfile_last: last file is %s\n", lastfile));
		return dp_RES_OK;
	} else if (n_match < 0) {
		DPRINT(("wmqfile_last: scandir out of memory!\n"));
		return dp_RES_NOMEM;
	}
	DPRINT(("wmqfile_last: no files\n"));
	return dp_RES_EMPTY;
}
