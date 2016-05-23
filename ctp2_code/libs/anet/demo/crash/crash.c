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
#include "anet.h"

static long __cdecl Debug_ExceptionFilter(LPEXCEPTION_POINTERS ep)
{
	dpReportCrash(ep);

	if (ep->ExceptionRecord->ExceptionCode == aeh_ASSERTION_CODE)
		return EXCEPTION_CONTINUE_EXECUTION;

	return (EXCEPTION_CONTINUE_SEARCH);
}

main(int argc, char **argv)
{
	dp_t *dp;
	dp_result_t err;
	dp_transport_t transport;

	if (argc != 2) {
		printf("usage: crash wipx2.dll\nCreates a dp_t.  Use dp.ini to cause crash if desired.");
		exit(1);
	}
	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, argv[1]);
	printf("Creating dp with dll=%s\n", transport.fname);
	__try {
		err = dpCreate(&dp, &transport, NULL, NULL);
	} __except(Debug_ExceptionFilter(GetExceptionInformation())) {
		printf("Exception occurred.\n");
		exit(1);
	}
	printf("dpCreate returns %d\n", err);
	exit(0);
}
