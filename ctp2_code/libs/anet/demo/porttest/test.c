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

/*-------------------------------------------------------------------------
 Example of how to list available ports for a particular activenet transport.
-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "anet.h"

#define MAX_PORTS 10
void main(int argc, char **argv)
{
	dp_transport_t transport;
	commPortName_t portName[MAX_PORTS]; 
	int nportName;
	int i;
	dp_result_t err;

	if (argc != 2) {
		printf("Usage: %s driver.dll\n\
Example: %s wmodem.dll\n\
Lists available values for portnum for given driver, along with\n\
the human readable name for each port.\n", argv[0], argv[0]);
		 exit(1);
	}
	
	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, argv[1]);
	err = dpEnumPorts(&transport, portName, MAX_PORTS, &nportName);
	if (err != dp_RES_OK) {
		printf("Can't enum ports!\n");
		exit(1);
	}

	for (i=0; i<nportName; i++) 
		printf("portnum %d, name %s\n", portName[i].portnum, portName[i].name);
	exit(0);
}
