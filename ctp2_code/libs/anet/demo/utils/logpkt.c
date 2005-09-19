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

/*------------------------------------------------------------------------------
 Simple packet logging module
------------------------------------------------------------------------------*/

#include "logpkt.h"

#ifdef LOGPKT_ENABLED

#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "eclock.h"

static int nCalls = 0;

/*------------------------------------------------------------------------------
 Return the filename that the next call to logPkt_open() will use.
 filename should point to buffer of length at least 14.
------------------------------------------------------------------------------*/
void logPkt_getFilename(char *filename)
{
	if (!filename)
		return;
	sprintf(filename, "output%d.%d", 0, nCalls);
}

/*------------------------------------------------------------------------------
 Open a log file and return it's file pointer - must be used before logPkt().
 Uses filenames of the form "output#.#" beginning with "output0.0".
 If there are output files from a previous process, up to 9 backup sets are 
 kept by incrementing the first number from 0 to 1, 1 to 2, etc.
 If called multiple times within one process, the second number is incremented
 once per call (from 0 to 4999).
------------------------------------------------------------------------------*/
FILE *logPkt_open()
{
	FILE *fp = NULL;
	char filename[40];

	if (nCalls == 0) {
		/* Delete all output9.* and move all old output0.* to output1.*, 
		 * output1.* to output2.*, etc. 
		 */
		int n = 0, i;
		int retval;

		do {
			sprintf(filename, "output%d.%d", 9, n);
			retval = unlink(filename);
			n++;
		} while (retval == 0 && n < 5000);
		
		for (i = 8; i >= 0; i--) {
			char newfilename[40];
			
			n = 0;
			do {
				sprintf(filename, "output%d.%d", i, n);
				sprintf(newfilename, "output%d.%d", i+1, n);
				unlink(newfilename);   /* just to be safe */
				retval = rename(filename, newfilename);
				n++;
			} while (retval == 0 && n < 5000);
		}
	}

	if (nCalls >= 5000)
		return NULL;
	
	logPkt_getFilename(filename);
	if ((fp = fopen(filename, "wt")) != NULL) {
		nCalls++;
		return fp;
	}
	return NULL;
}


/*------------------------------------------------------------------------------
 Close the log file - use after logging is finished to be nice
------------------------------------------------------------------------------*/
void logPkt_close(FILE *outFile)
{
	if (outFile == NULL) {
		return;
	}
	fclose(outFile);
}


/*------------------------------------------------------------------------------
 Log a packet to the debugging packet log file
 converts a binary packet (of length length) in buffer to hexadecimal codes for
 each byte, extracts the 2 starting pkttype bytes, and records the information
 in the text log file.  tag is a 2 character id, usually "rx" or "tx".
------------------------------------------------------------------------------*/
void logPkt(FILE *outFile, const void *buffer, size_t length, unsigned long peer, const char *tag)
{
	size_t i;
	char *p;
	const char *xbuf = (const char *)buffer;
	char tbuf[2000];	/* Must be dpio_MAXLEN_GATHER * 3 plus noise */

	p = tbuf;
	if (outFile == NULL) {
		return;
	}

	/* First byte or two is speshul */
	*p++ = (isprint(xbuf[0]) ? xbuf[0] : '?');
	*p++ = (isprint(xbuf[1]) ? xbuf[1] : '?');
	*p++ = ' ';
	
	for (i=0; i<length; i++) {
		unsigned int c = ((unsigned char *)xbuf)[i];
		unsigned int nib;
		unsigned int hc;
		
		nib = (c >> 4) & 0xf;
		hc = nib + '0';
		if (nib > 9) hc += 'a' - (10 + '0');
		*p++ = hc;
		
		nib = (c) & 0xf;
		hc = nib + '0';
		if (nib > 9) hc += 'a' - (10 + '0');
		*p++ = hc;

		*p++ = ' ';
	}
	*p++ = 0;
	
	fprintf(outFile, "%2s %10ld %2ld %3d : %s\n", tag, eclock(), peer, length, tbuf);
}

#endif
