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

 The pipe carries an endless stream of bytes.  You can measure time
 by the number of bytes that flow past its input or output.
 Incoming packets are placed first in a queue.
 When a packet arrives at the head of the queue, a timer is set,
 corresponding to
   how long it takes the packet to flow past the input of the pipe
 plus
   how long it takes a single byte to travel the length of the pipe.
 When this timer expires, the packet is removed from the
 head of the queue, and placed in another queue for pickup.

 One problem with implementing this is the granularity of time.
 Our code only gets a chance to run infrequently, but is modelling
 a background process that is independant of our speed.
 If we submit 10 packets, we want the right number of them delivered
 by the next time we poll.

 An algorithm that might do this is:
  When queuing a packet, mark it with an exit-from-pipe time as follows:
   if the packet is at the head of the queue,
   or the next packet's 'input free' time has passed,
     set 'input free' time to the entry duration plus now.
   else
     set 'input free' time to the entry duration plus the prior packet's
     'input free' time.
   set exit time to the transit durations plus the 'input free' time.

 If we only poll 10 times/second, and we want to introduce 100ms of latency,
 we will often induce twice as much latency as we should.
 The problem is worse if two copies of our code are run in series
 (e.g. before sending a packet, and after receiving it).

 We want to model dialup modem roundtrip latency, which is about 300ms,
 or about 75ms per modem.
 Perhaps the only answer is to run this simulation in a separate thread,
 or on a very fast machine, to reduce the granularity.
 We should print a warning if we get polled too infrequently.

 $Log: delay.c $
 Revision 1.6  1997/05/29 19:38:48  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.5  1997/05/23 22:16:06  dkegel
 Use dp_MALLOC so it doesn't go away when Netmech frees local heap.
 Revision 1.4  1997/05/23 20:19:24  dkegel
 Include right headers.
 Revision 1.3  1997/05/23 01:34:42  dkegel
 Validate parameters.
 Revision 1.2  1997/05/22 20:30:28  dkegel
 Return new value in accessors.
 Revision 1.1  1997/05/15 19:26:00  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "dpmem.h"	/* for dp_MALLOC, dp_FREE */
#include "delay.h"
#include "eclock.h"

/*-----------------------------------------------------------------------
 Set bytes per second the pipe can carry.

 Returns the new value upon success.
 You can query the old value by trying to set to -1.
-----------------------------------------------------------------------*/
int delay_setBytesPerSec(delay_t *q, int bytesPerSec)
{
	if (bytesPerSec > 0)
		q->bytes_per_sec = bytesPerSec;
	return q->bytes_per_sec;
}

/*-----------------------------------------------------------------------
 Set invariant delay of the pipe - how long it takes a packet to
 travel thru it once it has fully entered it.
 Units are milliseconds.

 Returns the new value upon success.
 You can query the old value by trying to set to -1.
-----------------------------------------------------------------------*/
int delay_setDelayMillisec(delay_t *q, int invariantDelay)
{
	if (invariantDelay >= 0)
		q->invariant_delay = ((invariantDelay * ECLOCKS_PER_SEC) / 1000);
	return (q->invariant_delay * 1000) / ECLOCKS_PER_SEC;
}

/*-----------------------------------------------------------------------
 Create a delay queue.  Returns NULL on failure.
 Default speed is 1700 bytes/sec;
 default backbone delay is 100 milliseconds.
-----------------------------------------------------------------------*/
delay_t *delay_create(void)
{
	delay_t *q = (delay_t *)dp_MALLOC(sizeof(delay_t));
	if (q) {
		memset(q, 0, sizeof(delay_t));
		delay_setBytesPerSec(q, 2000);		/* roughly 20 kbps */
		delay_setDelayMillisec(q, 50);				/* 50 milliseconds */
	}
	return q;
}

/*-----------------------------------------------------------------------
 Destroy a delay queue.
-----------------------------------------------------------------------*/
void delay_destroy(delay_t *pq)
{
	dp_FREE(pq);
}

/*-----------------------------------------------------------------------
 How many slots are free in the queue.
-----------------------------------------------------------------------*/
int delay_avail(delay_t *q)
{
	int avail;

	/* Measure distance between head and tail */
	avail = q->head - q->tail;
	if (avail <= 0)
		avail += delay_QSIZE;

	/* We need to keep one empty slot always */
	return avail - 1;
}

/*-----------------------------------------------------------------------
 Put data in a delay queue.
 If queue is full, discard the data.
 Caller should provide current value of clock().
 Returns 0 on success, nonzero on discard.
-----------------------------------------------------------------------*/
int delay_put(delay_t *q, void *buf, size_t len, clock_t now)
{
	delay_pkt_t *pkt;
	clock_t entry_start;

	/* Verify space */
	if (delay_avail(q) < 1) return 1;
	if (len > delay_MAXPKTLEN) return 2;

	/* Find space */
	pkt = &q->q[q->tail];
	if (q->tail >= delay_QSIZE)
		q->tail = 0;

	/* Copy packet */
	memcpy(pkt->buf, buf, len);
	pkt->len = len;

	/* Compute when it finishes entering and exiting the pipe:
	 * if the packet is at the head of the queue,
	 * or the next packet's 'input free' time has passed,
	 *   set 'input free' time to the entry duration plus now.
	 * else
	 *   set 'input free' time to the entry duration plus the prior packet's
	 *   'input free' time.
	 */
	entry_start = now;
	if (q->head != q->tail) {
		delay_pkt_t *pktAheadOfUs;
		pktAheadOfUs = (q->tail == 0) ? q->q : &q->q[q->tail-1];
		/* If there's a packet still entering the pipe, wait for it. */
		if ((long)(pktAheadOfUs->entry_done - now) > 0)
			entry_start = pktAheadOfUs->entry_done;
	}
	if (!q->bytes_per_sec)
		pkt->entry_done = entry_start;	/* wrong, but avoids divide by zero err */
	else
		pkt->entry_done = entry_start + ((len + delay_HEADER_BYTES) * ECLOCKS_PER_SEC) / q->bytes_per_sec;
	/*printf("delay_put: entry_done %d + ((%d + %d) * %d) / %d\n",
			entry_start, len, delay_HEADER_BYTES, ECLOCKS_PER_SEC, q->bytes_per_sec);*/
    /* set exit time to the transit durations plus the 'input free' time. */
	/*printf("delay_put: exit_done %d + %d\n", q->invariant_delay, pkt->entry_done);*/
	pkt->exit_done = q->invariant_delay + pkt->entry_done;

	/* Finish allocating space */
	q->tail++;
	if (q->tail >= delay_QSIZE)
		q->tail = 0;

	return 0;
}

/*-----------------------------------------------------------------------
 Get data out of a queue.
 Caller should provide current value of clock().
 Always returns number of bytes in next available packet.
 Copies data to *buf if buflen was large enough.
-----------------------------------------------------------------------*/
size_t delay_get(delay_t *q, void *buf, size_t buflen, clock_t now)
{
	delay_pkt_t *pkt;

	if (q->head == q->tail)
		return 0;			/* empty */

	pkt = &q->q[q->head];
	/*printf("delay_get: exit_done %d, now %d\n", pkt->exit_done, now);*/
	if ((long)(pkt->exit_done - now) >= 0)
		return 0;			/* not out of the pipe yet */

	/* Packet out of the pipe- remove it from queue */
	q->head++;
	if (q->head >= delay_QSIZE)
		q->head = 0;

	/* Pass it to user code */
	if (buflen < pkt->len)
		return 0;			/* doesn't fit - drop on floor */
	memcpy(buf, pkt->buf, buflen);
	return pkt->len;
}

#ifdef TEST_MAIN
#include <stdio.h>
#include <assert.h>

test(delay_t *q)
{
	int i;
	size_t len;
	char buf[256];
	clock_t now;
	clock_t start;
	long bytes;

	/* Make sure you can't overempty it. */
	assert(!delay_get(q, buf, sizeof(buf), clock()));

	/* Make sure you can't put illegal sized things into it. */
	assert(delay_put(q, 0, 1000, clock()));

	/* fill queue with legal elements. */
	start = clock();
	now = start;
	bytes = 0;
	for (i=0; i<delay_QSIZE-1; i++) {
		int res;

		/* Put in ten at a time */
		if ((i % 10) == 9) now = clock();

		len = 100;
		memset(buf, i, len);
		res = delay_put(q, buf, len, now);
		if (res)
			printf("test failed; i=%d, res=%d\n", i, res);
		assert(!res);
		bytes += len;
	}

	/* Make sure you can't overfill it. */
	assert(delay_put(q, buf, len, clock()));

	/* Watch the elements come out */
	for (i=0; i<delay_QSIZE-1; i++) {
		do {
			len = delay_get(q, buf, sizeof(buf), clock());
		} while (!len);
		printf("Got element %d at time %d\n", i, clock() - start);
	}

	/* Make sure you can't overempty it. */
	assert(!delay_get(q, buf, sizeof(buf), clock()));
	now = clock();
	printf("%d bytes, %d clocks, %d bytes/sec\n", bytes, now - start,
		(bytes * ECLOCKS_PER_SEC)/ (now - start));
}

main()
{
	delay_t *q;
	int i;
	q = delay_create();
	assert(q);

	for (i=0; i<10; i++) {
		delay_setBytesPerSec(q, 1700 * (i+1));
		test(q);
	}

	delay_destroy(q);
	printf("No tests failed.\n");
}
#endif
