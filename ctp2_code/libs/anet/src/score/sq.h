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

#ifndef sq_h
#define sq_h
#include "anet.h"
#include "scorerep.h"

typedef struct {
	char dummy[1];	/* Nothing to put here yet... */
} sq_sessionclose_message_t;

typedef struct {
	char recordTag[2];
	union {
		scorerep_t scorerep;
		sq_sessionclose_message_t sessclose;
	} u;
} sq_message_t;

/*--------------------------------------------------------------------------
 Call before using sq_get to initialize the web message queue.
--------------------------------------------------------------------------*/
void sq_init(const char *wmqDirectory);

/*--------------------------------------------------------------------------
 Call to clean up when finished reading the web message queue
--------------------------------------------------------------------------*/
void sq_destroy();

/*--------------------------------------------------------------------------
 Get a score report message from the web message queue if one is waiting.
 Puts the submitting player's session id, and session type into the 
 spaces pointed to by sessid, and sessType.
 The sessid returned will have length no greater than dp_KEY_MAXLEN;
 The length used is returned in sessidlen.

 Messages are returned in an sq_message_t struct pointed to by msg. 
 
 Returns dp_RES_OK if there is a scorerep_buf to process,
 		 dp_RES_AGAIN if there was a non-score wmq entry,
         dp_RES_EMPTY if there is nothing there.
--------------------------------------------------------------------------*/
dp_result_t sq_get(unsigned char *sessid, int *sessidlen, dp_species_t *sessType, sq_message_t *msg);

#endif
