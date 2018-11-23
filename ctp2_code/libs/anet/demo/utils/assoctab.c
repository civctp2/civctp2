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
 $Log: assoctab.c $
 Revision 1.7  1997/05/29 19:38:21  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.6  1997/03/02 22:11:04  dkegel
 Fixed horrible bug found by Marshall.  Thanks, man.
 Revision 1.5  1997/02/26 02:39:42  dkegel
 assoctab_subscript_grow now avoids crash if can't grow
 Revision 1.4  1996/12/18 19:01:09  administrator
 Switched to c style comments for benefit of gcc.
 Revision 1.3  1996/07/23 00:30:59  dkegel
 Use standard C comments for portability.
 Revision 1.2  1996/04/30 20:59:04  dkegel
 Got rid of compiler warning.
 Revision 1.1  1996/02/16 20:13:57  dkegel
 Initial revision

 A simple dynamically sized associative table.
 For simplicity, key values will be integers.
-----------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "assoctab.h"

/*-----------------------------------------------------------------------
 Create a table whose elements all have the given size.
 Element size is the size of the value; it doesn't include the size of
 the key.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
assoctab_t *assoctab_create(size_t element_size)
{
	/*assoctab_item_t *pi; */

	return (assoctab_t *)dynatab_create(element_size + sizeof(int /*pi->key*/));
}

/*-----------------------------------------------------------------------
 Destroy a table.
-----------------------------------------------------------------------*/
void assoctab_destroy(assoctab_t *ptab)
{
	dynatab_destroy((dynatab_t *)ptab);
}

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, grow; new element guaranteed
 to be zeroed.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *assoctab_subscript_grow(assoctab_t *tab, int key)
{
	char *p;
	int i;
	assoctab_item_t *pi;

	/* Use brute force approach, and just do a linear search. */
	for (i=0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (assoctab_item_t *) p;
		if (pi->key == key) return pi->value;
	}
	pi = (assoctab_item_t *) dynatab_subscript_grow((dynatab_t *)tab, tab->n_used);
	if (!pi)
		return NULL;
	//assert(pi[0..tab->unit-1] == 0);
	pi->key = key;
	return pi->value;
}

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, return NULL.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *assoctab_subscript(assoctab_t *tab, int key)
{
	char *p;
	int i;
	assoctab_item_t *pi;

	/* Use brute force approach, and just do a linear search. */
	for (i=0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (assoctab_item_t *) p;
		if (pi->key == key) return pi->value;
	}
	return NULL;
}

/*-----------------------------------------------------------------------
 Delete the given key from the table, shifts higher keys down
 (in the assoctab_getkey sense).
 Returns 0 on success, 1 on failure.
-----------------------------------------------------------------------*/
int assoctab_subscript_delete(assoctab_t *tab, int key)
{
	char *p;
	int i;
	assoctab_item_t *pi;

	/* Use brute force approach, and just do a linear search. */
	for (i=0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (assoctab_item_t *) p;
		if (pi->key == key) {
			memmove(p, p + tab->unit, (tab->n_used - i - 1) * tab->unit);
			tab->n_used--;
			/* Must zero vacated spot.  Dynatab will zero growth areas. */
			memset((char *)tab->buf + tab->n_used * tab->unit, 0, tab->unit);
			return 0;
		}
	}
	return 1;
}

#if 0

#include <stdio.h>

void test1(assoctab_t *pt, int callnum)
{
	int i;
	int errs;
	int *p;
	/* Try to delete something that's not there */
	if (assoctab_subscript_delete(pt, 0) == 0) {
		printf("delete0: test failed\n");
		exit(1);
	}
	/* Fill it up */
	for (i=32; i>=0; i--) {
		p = assoctab_subscript_grow(pt, i);
		if (p == NULL) {
			printf("grow1: test failed on i=%d\n", i);
			exit(1);
		}
		if (*p != 0) {
			printf("initial value test: test failed, i %d, *p %d\n", i, *p);
			errs++;
		}
		*p = i;
		printf(".");
	}
	/* Make sure it got filled up right */
	for (i=0; i<33; i++) {
		p = assoctab_subscript(pt, i);
		if (p == NULL) {
			printf("subscript2: test failed\n");
			exit(1);
		}
		if (*p != i) {
			printf("value test: test failed\n");
			exit(1);
		}
	}
	/* Make sure it's empty past the end */
	if (assoctab_subscript(pt, i) != NULL) {
		printf("subscript3: test failed\n");
		exit(1);
	}
	/* Delete halfway thru the table */
	if (assoctab_subscript_delete(pt, 16)) {
		printf("delete1: test failed\n");
		exit(1);
	}
	/* Verify that rest of table still there */
	errs = 0;
	for (i=0; i<33; i++) {
		p = assoctab_subscript(pt, i);
		if (i == 16) {
			if (p != NULL) {
				printf("subscript5: test failed, i %d: expected null, got %p, %d\n", i, p, *p);
				errs ++;
			}
		} else {
			if (p == NULL) {
				printf("subscript5: test failed, i %d, got p NULL\n", i);
				errs ++;
			}
			if (*p != i) {
				printf("value test 2: test failed, i %d.  Expected %d, got %d\n", i, i, *p);
				errs ++;
			}
		}
	}
	/* Delete elements, get ready for next run */
	for (i=0; i<33; i++) {
		int b;
		b = assoctab_subscript_delete(pt, i);
		if (b ^ (i == 16)) {
			printf("delete2: test failed, i %d.  Expected %d, got %d\n", i==16, b);
			errs ++;
		}
	}
	if (pt->n_used != 0) {
		printf("delete3: test failed.  Expected n_used = zero, got %d\n", pt->n_used);
		errs ++;
	}
	if (errs) {
		printf("test: failed on call %d\n", callnum);
		exit(1);
	}
}

#define NTABS 100
main()
{
	int i;
	assoctab_t *pt[NTABS];

	for (i=0;i<NTABS;i++) {
		pt[i] = assoctab_create(sizeof(int));
		if (!pt[i]) {
			printf("create: test failed\n");
			exit(1);
		}
		if (assoctab_subscript(pt[i], 1) != NULL) {
			printf("subcript1: test failed\n");
			exit(1);
		}
	}
	for (i=0;i<NTABS;i++) {
		int h;
		h = _heapchk();
		if (h != _HEAPOK) {
			printf("heapchk: test failed\n");
			exit(1);
		}
		test1(pt[i], 1);
		test1(pt[i], 2);
		test1(pt[i], 3);
	}
	for (i=0;i<NTABS;i++)
		assoctab_destroy(pt[i]);
	printf("test okay\n");
	return 0;
}
#endif
