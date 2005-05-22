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

#ifndef LOGPKT_H
#define LOGPKT_H

#if defined(_DEBUG) || defined(_PROFILE) || defined(DPRNT)
#define LOGPKT_ENABLED
#endif

#include <stdio.h>

/*------------------------------------------------------------------------------
 Return the filename that the next call to logPkt_open() will use.
 filename should point to buffer of length at least 14.
------------------------------------------------------------------------------*/
#ifdef LOGPKT_ENABLED
void logPkt_getFilename(char *filename);
#else
#define logPkt_getFilename(filename)
#endif

/*------------------------------------------------------------------------------
 Open a log file and return it's file pointer - must be used before logPkt().
 Uses filenames of the form "output#.#" beginning with "output0.0".
 If there are output files from a previous process, up to 9 backup sets are 
 kept by incrementing the first number from 0 to 1, 1 to 2, etc.
 If called multiple times within one process, the second number is incremented
 once per call (from 0 to 4999).
------------------------------------------------------------------------------*/
#ifdef LOGPKT_ENABLED
FILE *logPkt_open();
#else
#define logPkt_open() ((FILE *)NULL)
#endif

/*------------------------------------------------------------------------------
 Close the log file - use after logging is finished to be nice
------------------------------------------------------------------------------*/
#ifdef LOGPKT_ENABLED
void logPkt_close(FILE *outFile);
#else
#define logPkt_close(fp)
#endif

/*------------------------------------------------------------------------------
 Log a packet to the debugging packet log file
 converts a binary packet (of length length) in buffer to hexadecimal codes for
 each byte, extracts the 2 starting pkttype bytes, and records the information
 in the text log file.  tag is a 2 character id, usually "rx" or "tx".
------------------------------------------------------------------------------*/
#ifdef LOGPKT_ENABLED
void logPkt(FILE *outFile, const void *buffer, size_t length, unsigned long peer, const char *tag);
#else
#define logPkt(fp,buf,len,peer,tag)
#endif

#endif
