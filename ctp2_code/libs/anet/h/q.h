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

/*-----------------------------------------------------------------------
 $Log: q.h $
 Revision 1.7  1997/09/11 17:24:24  lbennett
 Local queue wasn't big enough to handle 32-player lobbies.
 Revision 1.6  1997/07/26 18:05:15  dkegel
 Don't use #pragma pack; include dp*pack.h instead
 Revision 1.5  1997/05/31 22:42:24  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.4  1997/03/07 19:34:40  dkegel
 Mention dp_FREE instead of free().
 Revision 1.3  1997/01/31 07:47:44  dkegel
 Settled the structure packing question by using 
 #pragma pack(x) / #pragma pack() pairs at top and bottom of
 all .h files that declare structures.  Was careful to put 
 #pragma pack(x) after last #include (to avoid wierd nesting effects
 due to lack of #pragma pack(pop).
 Revision 1.2  1996/12/18 18:20:30  administrator
 Change to c style comments for benefit of gcc.
 Revision 1.1  1996/01/25 01:56:46  dkegel
 Initial revision

 A simple FIFO queue.
-----------------------------------------------------------------------*/

#ifndef q_h
#define q_h 

#include <string.h>
#include <stddef.h>
#include <stdio.h>

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.
 * Note: this must come after all other #include's!
 */
#include "dppackn.h"

typedef struct {
	void	*p;
	size_t	len;
} q_element_t;

/* Must be greater than the max number of players that can fit
 * in a single packet, since real packets are packed, but local
 * player creation messages are unpacked.
 */
#define q_MAXELS 128

typedef struct {
	/* Circular queue buffer. */
	q_element_t	elements[q_MAXELS];
	int			head;	/* Index into elements of place to put next message. */
	int			tail;	/* Index into elements of place to get next message. */
} q_t;

/*-----------------------------------------------------------------------
 Create a queue.  Returns NULL on failure.
-----------------------------------------------------------------------*/
q_t *q_create(void);

/*-----------------------------------------------------------------------
 Destroy a queue.
 Frees all the messages that had been in the queue.
-----------------------------------------------------------------------*/
void q_destroy(q_t *pq);

/*-----------------------------------------------------------------------
 Put data in a queue.
 Allocates the given number of bytes with malloc, copies the src buffer
 into the malloc'ed buffer, and stores a pointer to it in the queue.
 Returns NULL on failure.
 Returns the address of the data in the queue on success.
-----------------------------------------------------------------------*/
void *q_put(q_t *q, void *src, size_t len);

/*-----------------------------------------------------------------------
 Get data out of a queue.
 Returns the address and the size of the next message in the queue.
 Note: Caller must free the message using dp_FREE() when done with it;
 see dpmem.h.
 Returns NULL if queue is empty.
-----------------------------------------------------------------------*/
void *q_get(q_t *q, size_t *pLen);

/*-----------------------------------------------------------------------
 Returns number of free slots in queue.
-----------------------------------------------------------------------*/
int q_nfree(q_t *pq);

/*-----------------------------------------------------------------------
 Write queue to disk.
-----------------------------------------------------------------------*/
void q_freeze(q_t *pq, FILE *fp);

/*-----------------------------------------------------------------------
 Read queue from disk.
-----------------------------------------------------------------------*/
int q_thaw(q_t *pq, FILE *fp);

#include "dpunpack.h"
#endif
