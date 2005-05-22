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


#include <stdarg.h>
#include <stdio.h>

#define	DEBUG
#define	DEBUG_MODULE	1

#include	"DPRINT.h"

int dprintf( const char *	__format, ...) {
	static unsigned long debugCount = 0;
	va_list	argptr;
	int		len = 0;
	
	FILE* theFile = fopen("dprint.out", "a");

	if (theFile != NULL) {
		va_start(argptr, __format);

		fprintf(theFile, "\n");
		len = fprintf(theFile, __format, argptr);

		fclose(theFile);
	}

	debugCount++;
	
	return len;
}

void logmessage(char* theMessage) {
	
	FILE* theFile = fopen("dprint.out", "a");
	if (theFile != NULL) {

		fprintf(theFile, "\n");
		fprintf(theFile, theMessage);

		fclose(theFile);
	}

}

