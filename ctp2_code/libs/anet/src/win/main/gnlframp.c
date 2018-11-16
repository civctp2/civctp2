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

/*---------------------------------------------------------------------------
 Code to check whether a Greenleaf connection to a peer is working properly.

 $Log: gnlframp.c $
 Revision 1.2  1997/01/31 02:49:21  dkegel
 Switched to ddprint.h (for drivers)
 Revision 1.1  1996/12/13 23:25:42  jgraebner
 Initial revision
 Revision 1.5  1996/07/01 22:43:09  dkegel
 Improved error message if ramp was unsuccessful.
 Revision 1.4  1996/07/01 20:51:05  dkegel
 1. Read all 256 bytes of ramp, even if there's a newline before it.
 2. Send and check sessionID to make sure it's not the same as ours.
 (Which happens when cable not attached.)
 Revision 1.3  1996/07/01 19:15:22  dkegel
 Ignore last newline leftover from modem commands if present.
 Revision 1.2  1996/06/30 21:37:47  dkegel
 Be paranoid about overrunning buffers.
 Revision 1.1  1996/06/30 21:16:34  dkegel
 Initial revision
---------------------------------------------------------------------------*/

#ifdef _WINDOWS
  #include <windows.h>
  #define sleep(x)  Sleep((x) * 1000)
#endif

#include <string.h>
#include <stdio.h>

#define DEBUG_MODULE	!FALSE
#include "ddprint.h"
#include "gnlframp.h"

extern void fdprint(char *msg);

/*---------------------------------------------------------------------------
 Send a ramp and our ID to the other system, get their ramp and ID.
 print any differences in the ramp to both DPRINT and fdprint.
 If session ID's are the same, print warning to both DPRINT and fdprint.
---------------------------------------------------------------------------*/
void exchange_ramp(PORT *port, long sessionID)
{
	// See if data is getting through to the other side.
	unsigned char ibuf[256], obuf[256];
	char message[256];
	long theirID;
	int i;

	// Generate ramp and send it.
	for (i=0; i<sizeof(obuf); i++)
		obuf[i] = i;
	WriteBuffer(port, obuf, sizeof(obuf));
	// Send sessionID.
	WriteBuffer(port, (char *)&sessionID, sizeof(sessionID));
	// Let other side do the same, and let data percolate.
	sleep(1);
	if (SpaceUsedInTXBuffer(port) > 0) {
		sprintf(message, "Warning: %d bytes left in output buffer after 1 second\r\n",SpaceUsedInTXBuffer(port));
		fdprint(message);
		DPRINT((message));
	}

	// Skip newline if present
	// Read the other side's ramp.
	ReadBuffer(port, ibuf, 1);
	if (ibuf[0] == '\n') {
		fdprint("skipping newline\r\n");
		DPRINT(("skipping newline\n"));
		ReadBuffer(port, ibuf, sizeof(obuf));
	} else {
		ReadBuffer(port, ibuf+1, sizeof(obuf)-1);
		// kludge
		port->count++;
	}

	sprintf(message, "Sent %d byte ramp, read %d bytes.\r\n", sizeof(obuf), port->count);
	fdprint(message);
	DPRINT((message));
	if (port->count < 10) {
		fdprint("\r\nError: Ramp not received.  Check to make sure the cable is connected,\r\n"
		"that you have selected the right comm port on both computers,\r\n"
		"and that both players ran netmech with -Ktest.\r\n\r\n");
		return;
	}

	// Find first byte that differs, print rest out.
	for (i=0; (i<port->count) && (ibuf[i] == obuf[i]) && (i < sizeof(obuf)); i++)
		;
	if (i < port->count) {
		sprintf(message, "Error in received ramp starting at byte %d:\r\n", i);
		fdprint(message);
		DPRINT((message));
		while ((i < port->count) && (i < sizeof(ibuf))) {
			if (!(i % 16)) { fdprint("\r\n"); DPRINT(("\n")); }
			sprintf(message, "%2.2x ", ibuf[i]);
			fdprint(message);
			DPRINT((message));
			i++;
		}
		fdprint("\r\n");
		DPRINT(("\n"));
	}
	if (i < sizeof(obuf)) {
		fdprint("Input ramp ends early.\r\n");
		DPRINT(("Input ramp ends early.\n"));
	}
	// Get their ID
	ReadBuffer(port, (char *)&theirID, sizeof(theirID));
	sprintf(message, "Trying to read their sessionID; read %d bytes, theirID=%x.\r\n", port->count, theirID);
	fdprint(message);
	DPRINT((message));
	if (theirID == sessionID) {
		sprintf(message,
		"Error: Their sessionID same as ours!  Looks like there's no serial\r\n"
		"cable or modem attached!!  Please check your cables.\r\n");
		fdprint(message);
		DPRINT((message));
	}
}
