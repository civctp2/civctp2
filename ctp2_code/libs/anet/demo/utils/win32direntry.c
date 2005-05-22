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

#include "win32direntry.h"

int FIND_FIRST(const char *spec, struct dirent *f)
{
	f->handle = FindFirstFile(spec, &(f->file));
	if (INVALID_HANDLE_VALUE == f->handle)
		return -1;
	f->d_name = f->file.cFileName;
	return 0;
}

int FIND_NEXT(struct dirent *f)
{
	if (FALSE == FindNextFile(f->handle, &(f->file)))
		return -1;
	f->d_name = f->file.cFileName;
	return 0;
}

int FIND_END(struct dirent *f)
{
	return (int)FindClose(f->handle);
}
