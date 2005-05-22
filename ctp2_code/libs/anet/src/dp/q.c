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
 $Log: q.c $
 Revision 1.4  1997/05/29 19:39:00  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.3  1997/03/07 19:39:00  dkegel
 use dp_MALLOC/FREE/REALLOC instead of malloc/free/realloc
 so the Windows DLL can use GlobalAlloc or whatever to avoid
 losing its mind when HeapDestroy is called.
 Revision 1.2  1996/12/18 19:01:44  administrator
 Switched to c style comments for benefit of gcc.
 Revision 1.1  1996/01/25 01:56:35  dkegel
 Initial revision

 A simple FIFO queue.
-----------------------------------------------------------------------*/

#include <stdlib.h>
#include "q.h"
#include "dpmem.h"
/* need for dprint's */
#include "dp.h"

#define q_SIGNATURE 0x4288

/*-----------------------------------------------------------------------
 Create a queue.  Returns NULL on failure.
-----------------------------------------------------------------------*/
q_t *q_create(void) 
{
	q_t	*pq;

	pq = (q_t *)dp_MALLOC(sizeof(q_t));
	if (!pq) return NULL;
	memset(pq, 0, sizeof(q_t));
	return pq;
}

/*-----------------------------------------------------------------------
 Destroy a queue.
 Frees all the messages that had been in the queue.
-----------------------------------------------------------------------*/
void q_destroy(q_t *pq)
{
	void *pv;
	size_t s;

	while (NULL != (pv = q_get(pq, &s)))
		dp_FREE(pv);
	dp_FREE(pq);
}

/*-----------------------------------------------------------------------
 Put data in a queue.
 Allocates the given number of bytes with malloc, copies the src buffer
 into the malloc'ed buffer, and stores a pointer to it in the queue.
 Returns NULL on failure.
 Returns the address of the data in the queue on success.
-----------------------------------------------------------------------*/
void *q_put(q_t *pq, void *src, size_t len)
{
	void *pv;
	int newHead;

	newHead = (pq->head + 1) % q_MAXELS;
	if (newHead == pq->tail) return NULL;
	pv = dp_MALLOC(len);
	if (!pv) return NULL;
	pq->elements[pq->head].p = pv;
	pq->elements[pq->head].len = len;
	memcpy(pv, src, len);
	pq->head = newHead;
	return pv;
}

/*-----------------------------------------------------------------------
 Get data out of a queue.
 Returns the address and the size of the next message in the queue.
 Note: Caller must free the message using dp_FREE() when done with it.
 Returns NULL if queue is empty.
-----------------------------------------------------------------------*/
void *q_get(q_t *pq, size_t *pLen)
{
	void *pv;

	if (pq->head == pq->tail) return NULL;
	pv = pq->elements[pq->tail].p;
	*pLen = pq->elements[pq->tail].len;
	pq->tail++;
	if (pq->tail >= q_MAXELS) pq->tail = 0;
	return pv;
}

/*-----------------------------------------------------------------------
 Returns number of free slots in queue.
-----------------------------------------------------------------------*/
int q_nfree(q_t *pq)
{
	int nfree = pq->tail - pq->head - 1;
	if (nfree < 0)
		nfree += q_MAXELS; 
	return nfree;
}

/*-----------------------------------------------------------------------
 Write a q_element_t to disk.
-----------------------------------------------------------------------*/
static int freeze_element(q_element_t *qe, FILE *fp)
{
	if (fwrite(&(qe->len), sizeof(size_t), 1, fp) != 1)
		return 1;
	if (fwrite(qe->p, sizeof(char), qe->len, fp) != qe->len)
		return 2;
	return 0;
}

/*-----------------------------------------------------------------------
 Read a q_element_t from disk.
-----------------------------------------------------------------------*/
static int thaw_element(q_element_t *qe, FILE *fp)
{
	if (fread(&(qe->len), sizeof(size_t), 1, fp) != 1)
		return 1;
	qe->p = dp_MALLOC(qe->len);
	if (qe->p == NULL)
		return 3;
	if (fread(qe->p, sizeof(char), qe->len, fp) != qe->len)
		return 2;
	return 0;
}

/*-----------------------------------------------------------------------
 Write queue to disk.
-----------------------------------------------------------------------*/
void q_freeze(q_t *pq, FILE *fp)
{
	int i, err;
	int sig = q_SIGNATURE;

	if (fwrite(&sig, sizeof(sig), 1, fp) != 1) {
		DPRINT(("q_freeze: error writing sig\n"));
		return;
	}
	if ((fwrite(&(pq->head), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(pq->tail), sizeof(int), 1, fp) != 1)) {
		DPRINT(("q_freeze: error writing head/tail\n"));
		return;
	}
	if (pq->head == pq->tail) return;
	if (pq->head > pq->tail) {
		for (i = pq->tail; i < pq->head; i++) {
			if ((err = freeze_element(&(pq->elements[i]), fp)) == 1) {
				DPRINT(("q_freeze: error writing length of element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return;
			} else if (err == 2) {
				DPRINT(("q_freeze: error writing element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return;
			}
		}
	} else if (pq->head < pq->tail) {
		for (i = pq->tail; i < q_MAXELS - 1; i++) {
			if ((err = freeze_element(&(pq->elements[i]), fp)) == 1) {
				DPRINT(("q_freeze: error writing length of element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return;
			} else if (err == 2) {
				DPRINT(("q_freeze: error writing element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return;
			}
		}
		for (i = 0; i < pq->head; i++) {
			if ((err = freeze_element(&(pq->elements[i]), fp)) == 1) {
				DPRINT(("q_freeze: error writing length of element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return;
			} else if (err == 2) {
				DPRINT(("q_freeze: error writing element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return;
			}
		}
	}
	return;
}

/*-----------------------------------------------------------------------
 Read queue from disk.
-----------------------------------------------------------------------*/
int q_thaw(q_t *pq, FILE *fp)
{
	int i, err;
	int sig;

	if (fread(&sig, sizeof(sig), 1, fp) != 1) {
		DPRINT(("q_thaw: error reading signature\n"));
		return 1;
	}
	if (sig != q_SIGNATURE) {
		DPRINT(("q_thaw: signature doesn't match\n"));
		return 1;
	}
	if ((fread(&(pq->head), sizeof(int), 1, fp) != 1) ||
		(fread(&(pq->tail), sizeof(int), 1, fp) != 1)) {
		DPRINT(("q_thaw: error reading tail/head\n"));
		return 1;
	}
	if (pq->head == pq->tail) return 0;
	if (pq->head > pq->tail) {
		for (i = pq->tail; i < pq->head; i++) {
			if ((err = thaw_element(&(pq->elements[i]), fp)) == 1) {
				DPRINT(("q_thaw: error reading length of element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return 1;
			} else if (err == 2) {
				DPRINT(("q_thaw: error reading element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return 1;
			} else if (err == 3) {
				DPRINT(("q_thaw: error allocating space for element %d\n"));
				return 1;
			}
		}
	} else if (pq->head < pq->tail) {
		for (i = pq->tail; i < q_MAXELS - 1; i++) {
			if ((err = thaw_element(&(pq->elements[i]), fp)) == 1) {
				DPRINT(("q_thaw: error reading length of element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return 1;
			} else if (err == 2) {
				DPRINT(("q_thaw: error reading element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return 1;
			} else if (err == 3) {
				DPRINT(("q_thaw: error allocating space for element %d\n"));
				return 1;
			}
		}
		for (i = 0; i < pq->head; i++) {
			if ((err = thaw_element(&(pq->elements[i]), fp)) == 1) {
				DPRINT(("q_thaw: error reading length of element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return 1;
			} else if (err == 2) {
				DPRINT(("q_thaw: error reading element %d for tail %d, head %d\n", i, pq->tail, pq->head));
				return 1;
			} else if (err == 3) {
				DPRINT(("q_thaw: error allocating space for element %d\n"));
				return 1;
			}
		}
	}
	return 0;
}

#if 0

main()
{
	int i, j;
	q_t	*pq;
	void *pv;
	size_t len;
	FILE *fpq;

	for (j = 0; j < 2; j++) {
		pq = q_create();
		printf("in loop %d\n", j);
		if (j == 0) {
			for (i=0; i<q_MAXELS - 1; i++) {
				if (q_put(pq, &i, sizeof(i)) == NULL) {
					printf("Test Failed\n");
					exit(1);
				}
			}
			if (q_put(pq, &i, sizeof(i)) != NULL) {
				printf("Test Failed--Overfilling queue did not return error!\n");
				exit(1);
			}
			/* Test q_freeze() */
			fpq = fopen("q.frz", "wb");
			q_freeze(pq, fpq);
			fclose(fpq);
		} else {
			/* Test q_thaw() */
			fpq = fopen("q.frz", "rb");
			q_thaw(pq, fpq);
			fclose(fpq);
		}
		/* Test q_get() */
		for (i=0; i<q_MAXELS - 1; i++) {
			if ((pv = q_get(pq, &len)) == NULL) {
				printf("Test Failed, could not get data out\n");
				exit(1);
			}
			if (len != sizeof(int)) {
				printf("Test Failed, bad length\n");
				exit(1);
			}
			if (*(int *)pv != i) {
				printf("Test Failed, bad data\n");
				exit(1);
			}
			dp_FREE(pv);
		}
		if (q_get(pq, &len) != NULL) {
			printf("Test Failed, data still exists\n");
			exit(1);
		}
		q_destroy(pq);
	}
	printf("Test Passed.\n");
	exit(0);
}
#endif

