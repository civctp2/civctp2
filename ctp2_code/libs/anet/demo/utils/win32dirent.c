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

/*------------------------------------------------------------------------
 POSIX-style directory processing for Win32.
 This file is intended for use only with the win32dirent.h interface.

 Created 17-Nov-98 by Jason Scanlin (jscanlin@activision.com).
 
 Copyright 1998 by Activision.
 This source code is free and may be used or redistributed under the 
 terms GNU General Public Licence.
------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
#include <errno.h>

#ifndef MAX_PATH
#define MAX_PATH	260
#endif

#include "win32direntry.h"

#define DIR_INTERNAL
/* POSIX-style directory structure */
typedef struct {
	unsigned		d_loc;
	struct dirent	d_buf;
	char			d_name[MAX_PATH];
} DIR;

#include "win32dirent.h"

int dirent_errno;

/*------------------------------------------------------------------------
 Open a directory <dir> for reading at the first entry.

 Returns a pointer to a DIR structure, or NULL on error.

 dirent_errno is set to 0 on success or to one of the following on error:
	ENOMEM - Not enough memory
	ENAMETOOLONG - filename is too long
------------------------------------------------------------------------*/
DIR *opendir(const char *dir)
{
	DIR *dirp;
	int len;
	int i;
	char *p;

	if (NULL == (dirp = (DIR *)malloc(sizeof(DIR)))) {
		dirent_errno = ENOMEM;
		return NULL;
	}
	strncpy(dirp->d_name, dir, MAX_PATH);
	dirp->d_name[259] = '\0';

	if ((':' == dirp->d_name[1]) && (1 < strlen(dirp->d_name)))
		p = dirp->d_name + 2;
	else
		p = dirp->d_name;
	for (i = strlen(p) - 1; (0 < i) && ('\\' == p[i]); i--)
		p[i] = '\0';

	if (!strlen(p))
		strcat(dirp->d_name, ".");

	len = strlen(dirp->d_name);
	if ('\\' != dirp->d_name[len - 1]) {
		if (len + 1 > MAX_PATH) {
			free(dirp);
			dirent_errno = ENAMETOOLONG;
			return NULL;
		}
		strcat(dirp->d_name, "\\");
		len++;
	}
	if (len + 3 > MAX_PATH) {
		free(dirp);
		dirent_errno = ENAMETOOLONG;
		return NULL;
	}
	strcat(dirp->d_name, "*.*");

	dirp->d_loc  = 0;
	dirent_errno = 0;
	return dirp;
}

/*------------------------------------------------------------------------
 Close a directory <dirp> previously opened with a call to opendir().

 Returns 0 on success or -1 on failure.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed. 
------------------------------------------------------------------------*/
int closedir(DIR *dirp)
{
	if (NULL == dirp) {
		dirent_errno = EINVAL;
		return -1;
	}
	FIND_END(&(dirp->d_buf));
	memset(dirp, 0, sizeof(DIR));
	free(dirp);
	dirent_errno = 0;
	return 0;
}

/*------------------------------------------------------------------------
 Rewind a directory <dirp> to point at the first entry.

 Returns 0 on success or -1 on failure.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed. 
------------------------------------------------------------------------*/
int rewinddir(DIR *dirp)
{
	if (NULL == dirp) {
		dirent_errno = EINVAL;
		return -1;
	}
	dirp->d_loc = 0;
	dirent_errno = 0;
	return 0;
}

/*------------------------------------------------------------------------
 Point <dirp> at entry <offset>.  Use telldir() to get valid offsets for
 seekdir.

 Returns 0 on success or -1 on error.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed.
	EOF - Attempt to seek past end of directory.
------------------------------------------------------------------------*/
int seekdir(DIR *dirp, int offset)
{
	int i;

	if (NULL == dirp) {
		dirent_errno = EINVAL;
		return -1;
	}
	rewinddir(dirp);
	for (i = 0; i < offset; i++)
		if (NULL == readdir(dirp)) {
			dirent_errno = EOF;
			return -1;
		}
	dirent_errno = 0;
	return 0;
}

/*------------------------------------------------------------------------
 Return the current offset of <dirp> or -1 on error;

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed.
------------------------------------------------------------------------*/
int telldir(DIR *dirp)
{
	if (NULL == dirp) {
		dirent_errno = EINVAL;
		return -1;
	}
	dirent_errno = 0;
	return dirp->d_loc;
}

/*------------------------------------------------------------------------
 Read an entry from <dirp> at the current location, advancing <dirp> to
 point at the next entry.

 Returns a struct dirent pointer to the entry on success or NULL on error.
 Subsequent calls to readdir on <dirp> will overwrite this data.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed.
	EOF - Attempt to seek past end of directory.
------------------------------------------------------------------------*/
struct dirent *readdir(DIR *dirp)
{
	if (NULL == dirp) {
		dirent_errno = EINVAL;
		return NULL;
	}
	if (((0 == dirp->d_loc)
		 && (0 == FIND_FIRST(dirp->d_name, &(dirp->d_buf))))
	||  (0 == FIND_NEXT(&(dirp->d_buf)))) {
		dirp->d_loc++;
		dirent_errno = 0;
		return (&(dirp->d_buf));
	}
	dirent_errno = EOF;
	return NULL;
}

/*------------------------------------------------------------------------
 Scan the directory <dirp>, calling <selector> on each directory entry.
 Entries for which <selector> returns nonzero are individually malloc'd,
 sorted using qsort with <cmp>, and collected in a malloc'd array in
 *<namelist>.
 A null <selector> selects all files.  A null <cmp> does no sorting.
 
 Returns the number of entries selected, or -1 on error.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad args, probably null.
	EOF - Attempt to seek past end of directory.
------------------------------------------------------------------------*/
int scandir(const char *dir,
		struct dirent ***namelist,
		int (*selector)(struct dirent *),
		int (__cdecl *cmp)(const struct dirent **, const struct dirent **))
{
	DIR *dirp;
	int n_match = 0;
	int n = 0;
	struct dirent *direntp;

	if ((NULL == dir) || (NULL == namelist) || (NULL == cmp)) {
		dirent_errno = EINVAL;
		return -1;
	}
 	dirp = opendir(dir);
	if (NULL == dirp) {
printf("scandir: opendir(%s) err:%d\n", dir, dirent_errno);
		return -1;
	}
printf("scandir: opened %s\n", dirp->d_name);
	while (NULL != (direntp = readdir(dirp))) {
		if (NULL == direntp) {
			int temperr = dirent_errno;
printf("scandir: readdir(%s[%d]) err:%d\n", dirp->d_name, n, dirent_errno);
			closedir(dirp);
			dirent_errno = temperr;
			return -1;
		}
		if ((NULL == selector) || (*selector)(direntp)) {
			struct dirent **temp;
			if (0 == n_match)
				temp = (struct dirent **)malloc(sizeof(struct dirent *));
			else 
				temp = (struct dirent **)realloc(*namelist,
					(n_match + 1)*sizeof(struct dirent *));
			if (NULL == temp) {
				int i;

printf("scandir: realloc(list[%d]) failed\n", n_match);
				/* free anything we have so far */
				if (0 < n_match) {
					for (i = 0; i < n_match; i++)
						free((*namelist)[i]);
					free(*namelist);
				}
				closedir(dirp);
				dirent_errno = ENOMEM;
				return -1;
			}
			*namelist = temp;
			(*namelist)[n_match] =
				(struct dirent *)malloc(sizeof(struct dirent));
			if (NULL == (*namelist)[n_match]) {
				int i;

printf("scandir: malloc(list[%d]->dirent) failed\n", n_match);
				/* free anything we have so far */
				if (0 < n_match) {
					for (i = 0; i < n_match; i++)
						free((*namelist)[i]);
					free(*namelist);
				}
				closedir(dirp);
				dirent_errno = ENOMEM;
				return -1;
			}
			memcpy((*namelist)[n_match], direntp, sizeof(struct dirent));
			n_match++;
		}
		n++;
	}

	if (NULL != cmp)
		qsort(*namelist, n_match, sizeof(struct dirent *), cmp);
	return n_match;
}

/*------------------------------------------------------------------------
 Comparison function for alphabetically sorting struct dirent *'s.
------------------------------------------------------------------------*/
int __cdecl alphasort(const struct dirent **a, const struct dirent **b)
{
    return strncmp((*a)->d_name, (*b)->d_name, MAX_PATH);
}
