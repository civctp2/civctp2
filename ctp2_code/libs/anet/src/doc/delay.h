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
 Delay module.
 Simulates a modem which
 can accept a certain number of bytes per second (bytes_per_sec),
 takes a certain number of seconds to carry a single byte from one end
 to the other (sec_en_route),
 and can store a large number of them at the input in preparation
 for shooting them into the pipe one at a time, and a large number
 of them at the end for pickup.

 See .c file for more comments.

 $Log: delay.h $
 Revision 1.1  1997/08/12 17:56:30  dkegel
 Initial revision
 Revision 1.1  1997/05/15 19:26:02  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef delay_h
#define delay_h

#include <time.h>
#include <stddef.h>

#define delay_MAXPKTLEN 300

typedef struct {
	char buf[delay_MAXPKTLEN];/* the packet */
	size_t	len;              /* its length */
	clock_t entry_done;       /* when packet has fully entered the pipe */
	clock_t exit_done;        /* when packet has fully exited the pipe */
} delay_pkt_t;

#define delay_QSIZE 100       /* number of packets that can be in pipe and buffers */
typedef struct {
	delay_pkt_t q[delay_QSIZE];
	int head;
	int tail;
	int bytes_per_sec;
	clock_t invariant_delay;
} delay_t;

/* Number of hidden overhead bytes when sending a packet */
#define delay_HEADER_BYTES 30

/*-----------------------------------------------------------------------
 Create a delay queue.  Returns NULL on failure.
-----------------------------------------------------------------------*/
delay_t *delay_create(void);

/*-----------------------------------------------------------------------
 Destroy a delay queue.
-----------------------------------------------------------------------*/
void delay_destroy(delay_t *pq);

/*-----------------------------------------------------------------------
 Put data in a delay queue.
 If queue is full, discard the data.
 Caller should provide current value of clock().
 Returns 0 on success, nonzero on discard.
-----------------------------------------------------------------------*/
int delay_put(delay_t *q, void *buf, size_t len, clock_t now);

/*-----------------------------------------------------------------------
 Get data out of a queue.
 Caller should provide current value of clock().
 Always returns number of bytes in next available packet.
 Copies data to *buf if buflen was large enough.
-----------------------------------------------------------------------*/
size_t delay_get(delay_t *q, void *buf, size_t buflen, clock_t now);

#endif
