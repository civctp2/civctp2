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
 Simple emulation of strdup function for compilers which lack them.

 $Log: strdup.c $
 Revision 1.2  1997/07/25 21:43:50  dkegel
 Now agrees with prototype
 Revision 1.1  1997/06/21 02:29:06  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifndef _WIN32

#include <stdlib.h>
#include <string.h>
#include "strdup.h"

char *strdup(const char *s)
{
	char *t;

	t = (char *)malloc(strlen(s) + 1);
	strcpy(t, s);
	return t;
}

#endif
