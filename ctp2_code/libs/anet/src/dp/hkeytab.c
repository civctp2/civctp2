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
 $Log: hkeytab.c $
 Revision 1.2  1997/06/17 23:21:15  dkegel
 Disable test code.
 Revision 1.1  1997/06/09 23:36:56  dkegel
 Initial revision

 A simple dynamically sized associative table.
 Key values are variable length binary strings.
-----------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#if __MWERKS__
    #include <string.h>
#endif
#include "hkeytab.h"

/*-----------------------------------------------------------------------
 Create a table whose elements all have the given size.
 Element size is the size of the value; it doesn't include the size of
 the key.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
hkeytab_t *hkeytab_create(size_t element_size)
{
	return (hkeytab_t *)dynatab_create(sizeof_hkeytab_item_t(element_size));
}

/*-----------------------------------------------------------------------
 Destroy a table.
-----------------------------------------------------------------------*/
void hkeytab_destroy(hkeytab_t *ptab)
{
	dynatab_destroy((dynatab_t *)ptab);
}

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, grow; new element guaranteed
 to be zeroed.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *hkeytab_subscript_grow(hkeytab_t *tab, void *key, size_t keylen)
{
	char *p;
	int i;
	hkeytab_item_t *pi;

	/* Use brute force approach, and just do a linear search. */
	for (i=0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (hkeytab_item_t *) p;
		if ((((size_t)pi->keylen) == keylen)
		&& !memcmp(pi->key, (char *)key, keylen)) {
				return pi->value;
		}
	}
	pi = (hkeytab_item_t *) dynatab_subscript_grow((dynatab_t *)tab, tab->n_used);
	if (!pi)
		return NULL;
	//assert(pi[0..tab->unit-1] == 0);
	memcpy(pi->key, (char *)key, keylen);
	pi->keylen = (unsigned char)keylen;
	return pi->value;
}

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, return NULL.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *hkeytab_subscript(hkeytab_t *tab, const void *key, size_t keylen)
{
	char *p;
	int i;
	hkeytab_item_t *pi;

	/* Use brute force approach, and just do a linear search. */
	for (i=0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (hkeytab_item_t *) p;
		if ((((size_t)pi->keylen) == keylen)
		&& !memcmp(pi->key, (char *)key, keylen)) {
				return pi->value;
		}
	}
	return NULL;
}

/*-----------------------------------------------------------------------
 Delete the given key from the table, shifts higher keys down
 (in the hkeytab_getkey sense).
 Returns 0 on success, 1 on failure.
-----------------------------------------------------------------------*/
int hkeytab_subscript_delete(hkeytab_t *tab, void *key, size_t keylen)
{
	char *p;
	int i;
	hkeytab_item_t *pi;

	/* Use brute force approach, and just do a linear search. */
	for (i=0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (hkeytab_item_t *) p;
		if ((((size_t)pi->keylen) == keylen)
		&& !memcmp(pi->key, (char *)key, keylen)) {
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

void test1(hkeytab_t *pt, int callnum)
{
	int i;
	int errs;
	int *p;
	int dummy;
	/* Try to delete something that's not there */
	dummy = 0;
	if (hkeytab_subscript_delete(pt, &dummy, sizeof(dummy)) == 0) {
		printf("delete0: test failed\n");
		exit(1);
	}
	/* Fill it up */
	for (i=32; i>=0; i--) {
		p = hkeytab_subscript_grow(pt, &i, sizeof(i));
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
		p = hkeytab_subscript(pt, &i, sizeof(i));
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
	if (hkeytab_subscript(pt, &i, sizeof(i)) != NULL) {
		printf("subscript3: test failed\n");
		exit(1);
	}
	/* Delete halfway thru the table */
	dummy = 16;
	if (hkeytab_subscript_delete(pt, &dummy, sizeof(dummy))) {
		printf("delete1: test failed\n");
		exit(1);
	}
	/* Verify that rest of table still there */
	errs = 0;
	for (i=0; i<33; i++) {
		p = hkeytab_subscript(pt, &i, sizeof(i));
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
		b = hkeytab_subscript_delete(pt, &i, sizeof(i));
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
	hkeytab_t *pt[NTABS];
	int dummy;

	for (i=0;i<NTABS;i++) {
		pt[i] = hkeytab_create(sizeof(int));
		if (!pt[i]) {
			printf("create: test failed\n");
			exit(1);
		}
		dummy = 1;
		if (hkeytab_subscript(pt[i], &dummy, sizeof(dummy)) != NULL) {
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
		hkeytab_destroy(pt[i]);
	printf("test okay\n");
	return 0;
}
#endif
