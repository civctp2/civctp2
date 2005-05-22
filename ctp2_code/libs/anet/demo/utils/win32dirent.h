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

 Created 17-Nov-98 by Jason Scanlin (jscanlin@activision.com).
 
 Copyright 1998 by Activision.
 This source code is free and may be used or redistributed under the 
 terms GNU General Public Licence.
------------------------------------------------------------------------*/

#ifndef DIRENT_H
#define DIRENT_H

#include "win32direntry.h"

/* POSIX-style directory structure - opaque to outside world */
#ifndef DIR_INTERNAL
typedef struct {
	char dummy[1];
} DIR;
#endif

extern int dirent_errno;

/*------------------------------------------------------------------------
 Open a directory for reading at the first entry.

 Returns a pointer to a DIR structure, or NULL on error.

 dirent_errno is set to 0 on success or to one of the following on error:
	ENOMEM - Not enough memory
	ENAMETOOLONG - filename is too long
------------------------------------------------------------------------*/
DIR *opendir(const char *);

/*------------------------------------------------------------------------
 Close a DIR* previously opened with a call to opendir().

 Returns 0 on success or -1 on failure.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed. 
------------------------------------------------------------------------*/
int closedir(DIR *);

/*------------------------------------------------------------------------
 Rewind a directory DIR* to point at the first entry.

 Returns 0 on success or -1 on failure.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed. 
------------------------------------------------------------------------*/
int rewinddir(DIR *);

/*------------------------------------------------------------------------
 Point DIR* at a given offset.  Use telldir() to get valid offsets for
 seekdir.

 Returns 0 on success or -1 on error.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed.
	EOF - Attempt to seek past end of directory.
------------------------------------------------------------------------*/
int seekdir(DIR *, int);

/*------------------------------------------------------------------------
 Return the current offset of a DIR* or -1 on error;

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed.
------------------------------------------------------------------------*/
int telldir(DIR *);

/*------------------------------------------------------------------------
 Read an entry from DIR* at the current location, advancing DIR* to
 point at the next entry.

 Returns a struct dirent pointer to the entry on success or NULL on error.
 Subsequent calls to readdir on DIR* will overwrite this data.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad dirp, probably not opened or already closed.
	EOF - Attempt to seek past end of directory.
------------------------------------------------------------------------*/
struct dirent *readdir(DIR *);

/*------------------------------------------------------------------------
 Scan the directory dir, calling <selector> on each directory entry.
 Entries for which <selector> returns nonzero are individually malloc'd,
 sorted using qsort with <cmp>, and collected in a malloc'd array in
 *<namelist>.
 A null <selector> selects all files.  A null <cmp> does no sorting.
 
 Returns the number of entries selected, or -1 on error.

 dirent_errno is set to 0 on success or to one of the following on error:
	EINVAL - Bad args, probably null.
	EOF - Attempt to seek past end of directory.
------------------------------------------------------------------------*/
int scandir(const char *__dir,
	struct dirent ***__namelist,
	int (*__selector)(struct dirent *),
	int (__cdecl *__cmp)(const struct dirent **, const struct dirent **));

/*------------------------------------------------------------------------
 Comparison function for alphabetically sorting struct dirent *'s.
------------------------------------------------------------------------*/
int __cdecl alphasort(const struct dirent **, const struct dirent **);

#endif /* DIRENT_H */
