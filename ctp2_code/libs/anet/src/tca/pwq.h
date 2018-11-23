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

#ifndef pwq_h
#define pwq_h

#include <time.h>
#include "aneterr.h"
#include "tcapw.h"
#include "wmq.h"

typedef struct {
	dp_uid_t uid;
} pwq_accountDelete_message_t;

#define pwq_SERVERTAG_MASTER "Master"

#define pwq_MESSAGETYPE_CREATE 'c'
#define pwq_MESSAGETYPE_CHANGE 'x'
#define pwq_MESSAGETYPE_DELETE 'd'

/* A structure passed by or returned to the caller, containing a parsed
 * account packet.
 */
typedef struct {
	char type;
	union {
		tcapw_entry_t acctCreate;
		tcapw_entry_t acctChange;
		pwq_accountDelete_message_t acctDelete;
	} u;
} pwq_message_t;

/* Instance data for pwq */
typedef struct {
	wmq_t *wmq;
	tcapw_t *tcapw;
	char masterServerTag[6];
} pwq_t;

/*--------------------------------------------------------------------------
 Create an instance of pwq.
 If write is 0, it will be a read queue, and pwq_put will fail.
 If write is non-0, it will be a write queue, and pwq_get will fail.
 Returns NULL on failure.
--------------------------------------------------------------------------*/
pwq_t *pwq_create(tcapw_t *tcapw, const char *wmqDirectory, int write, const char *masterServerTag);

/*--------------------------------------------------------------------------
 Destroy an instance of the pwq.  Saves the password database pwq->tcapw
 to disk, but does not otherwise affect it.
--------------------------------------------------------------------------*/
void pwq_destroy(pwq_t *pwq);

/*--------------------------------------------------------------------------
 Get an account message from the web message queue if one is waiting.
 Messages are returned in a pwq_message_t struct pointed to by msg.

 Returns dp_RES_OK if there is an account message to process,
 		 dp_RES_AGAIN if there was a non-account wmq entry,
         dp_RES_EMPTY if there is nothing there.
--------------------------------------------------------------------------*/
dp_result_t pwq_get(pwq_t *pwq, pwq_message_t *msg);

/*--------------------------------------------------------------------------
 Put an account message into the web message queue with the given timestamp.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t pwq_put(pwq_t *pwq, time_t now, pwq_message_t *msg);

/*--------------------------------------------------------------------------
 Get the wmq position into timestamp, offset.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t pwq_tell(pwq_t *pwq, long *poffset, time_t *ptime);

#endif
