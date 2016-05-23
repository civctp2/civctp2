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

/* prevent multiple inclusions of this header file */
#ifndef DIRENTRY_H
#define DIRENTRY_H

#define WIN32_LEAN_AND_MEAN
#define NOIME
#define NOGDI
#define NOSERVICE
#undef INC_OLE1
#undef INC_OLE2
#include <windows.h>

typedef struct dirent {
	WIN32_FIND_DATA file;	/* File data returned by Win32 Find*File */
	HANDLE handle;			/* WIN32 Find*File handle. */
	char *d_name;			/* This is the only POSIX guaranteed field */
							/* It points to cFileName in WIN32_FIND_DATA,
							 * provided the FIND_ functions below are used.
							 */
};

/* (un)defines of POSIX options - perhaps we could add these at some point */
#undef _DIRENT_HAVE_D_NAMELEN
#undef _DIRENT_HAVE_D_RECLEN
#undef _DIRENT_HAVE_D_OFF
#undef _DIRENT_HAVE_D_TYPE

int FIND_FIRST(const char *spec, struct dirent *f);
int FIND_NEXT(struct dirent *f);
int FIND_END(struct dirent *f);

#endif /* DIRENTRY_H */
