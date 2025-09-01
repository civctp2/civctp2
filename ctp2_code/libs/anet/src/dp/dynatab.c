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
 $Log: dynatab.c $
 Revision 1.12  1997/09/27 01:58:59  lbennett
 Added DPRINT to freeze and thaw.
 Revision 1.11  1997/09/24 21:41:49  lbennett
 Added dynatab_find and dynatab_remove, modified unit test to check them.
 Revision 1.10  1997/06/28 01:00:34  dkegel
 Paranoid - don't call fwrite with zero len
 Revision 1.9  1997/05/29 19:38:58  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.8  1997/03/07 19:38:59  dkegel
 use dp_MALLOC/FREE/REALLOC instead of malloc/free/realloc
 so the Windows DLL can use GlobalAlloc or whatever to avoid
 losing its mind when HeapDestroy is called.
 Revision 1.7  1996/12/18 19:01:43  administrator
 Switched to c style comments for benefit of gcc.
 Revision 1.6  1996/07/23 00:30:38  dkegel
 Use standard C comments for portability.
 Revision 1.5  1996/04/18 00:44:39  dkegel
 Fixed bug where size 0 tables could not be thawed.
 Fixed typo in .h file.
 Revision 1.4  1996/04/17 21:10:59  dkegel
 Ooops.
 Revision 1.3  1996/04/17 21:07:49  dkegel
 Added freeze and thaw methods.
 Revision 1.2  1996/02/12 19:52:31  dkegel
 1. Fixed memory bug in create.
 2. Beafed up test case so it would have found the bug.
 Revision 1.1  1996/01/30 01:14:37  dkegel
 Initial revision

 A simple dynamically sized table.
-----------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than sint32
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dynatab.h"
#include "dp2.h"
#include "dpmem.h"

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/*-----------------------------------------------------------------------
 Create a table whose elements all have the given size.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
dynatab_t *dynatab_create(size_t element_size)
{
	dynatab_t *pdt;

	assert(element_size > 0);

	pdt = (dynatab_t *)dp_MALLOC(sizeof(dynatab_t));
	if (!pdt) return NULL;
	pdt->buf = NULL;
	pdt->unit = element_size;
	pdt->n_used = 0;
	pdt->n_alloced = 0;

	DPRINT(("dynatab_create: %p of size %zu\n", pdt, element_size));

	return pdt;
}

/*-----------------------------------------------------------------------
 Destroy a table.
-----------------------------------------------------------------------*/
void dynatab_destroy(dynatab_t *ptab)
{
	assert(ptab != NULL);
	if (ptab->buf) dp_FREE(ptab->buf);
	ptab->buf = NULL;	/*  Paranoid */
	dp_FREE(ptab);
}

/*-----------------------------------------------------------------------
 Remove all elements of the table, but don't touch its allocation.
-----------------------------------------------------------------------*/
void dynatab_removeAll(dynatab_t *tab)
{
	assert(tab != NULL);

	/* Update element count */
	tab->n_used = 0;
}

/*-----------------------------------------------------------------------
 Remove the given element of the table.
 WARNING: Silently does nothing if subscript is out of range.
-----------------------------------------------------------------------*/
void dynatab_remove(dynatab_t *tab, sint32 subscript)
{
	assert(tab != NULL);

	/* Check the subscript for validity */
	if ((subscript < 0) || (subscript >= tab->n_used))
		return;

	/* If needed, move elements down to fill space */
	if(subscript < (tab->n_used - 1)) {
		void* dest = ((char *)tab->buf) + (subscript * tab->unit);
		void* src = ((char *)tab->buf) + ((subscript+1) * tab->unit);
		size_t count = ((tab->n_used - 1) - subscript) * tab->unit;
		memmove(dest, src, count);
	}

	/* Update element count */
	tab->n_used --;
}

/*-----------------------------------------------------------------------
 Return the address of the given element of the table.
 If the table isn't that big, grow.
 Start all elements with 0.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *dynatab_subscript_grow(dynatab_t *tab, sint32 subscript)
{
	assert(tab != NULL);
	if (subscript < 0)
		return NULL;
	if (subscript >= tab->n_alloced) {
		void *newbuf;
		sint32 new_n_alloced = tab->n_alloced + 16;
		if (new_n_alloced < subscript + 1)
			new_n_alloced = subscript + 16;

		if (tab->buf)
			newbuf = dp_REALLOC(tab->buf, new_n_alloced * tab->unit);
		else
			newbuf = dp_MALLOC(new_n_alloced * tab->unit);
		if (!newbuf) return NULL;
		tab->buf = newbuf;
		/*  tab[n_alloced...new_n_alloced] = 0; */
		/*  memset(dynatab_subscript(tab, tab->n_alloced), 0, new area)); */
		memset((char *)tab->buf + (tab->n_alloced * tab->unit), 0,
			(new_n_alloced - tab->n_alloced) * tab->unit);
		tab->n_alloced = new_n_alloced;
	}
	if (tab->n_alloced < subscript + 1) {
		/*  BUG: Should have enough allocated by now. */
		return NULL;
	}
	if (tab->n_used < subscript + 1)
		tab->n_used = subscript + 1;

	DPRINT(("dynatab_subscript_grow: %p of size %zu has grown to %d\n", tab, tab->unit, tab->n_used));

	return ((char *)tab->buf) + (subscript * tab->unit);
}

/*-----------------------------------------------------------------------
 Inserts a record into the table before the nth record.
 Grows the table.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *dynatab_subscript_insert(dynatab_t *tab, sint32 n)
{
	void *p;

	assert(tab != NULL);
	if (n < 0)
		return NULL;
	if (n > tab->n_used)
		return NULL;

	if (tab->n_used >= tab->n_alloced) {
		void *newbuf;
		sint32 new_n_alloced = tab->n_alloced + 16;
		if (new_n_alloced < tab->n_used + 1)
			new_n_alloced = tab->n_used + 16;

		if (tab->buf)
			newbuf = dp_REALLOC(tab->buf, new_n_alloced * tab->unit);
		else
			newbuf = dp_MALLOC(new_n_alloced * tab->unit);
		if (!newbuf) return NULL;
		tab->buf = newbuf;
		/*  tab[n_alloced...new_n_alloced] = 0; */
		/*  memset(dynatab_subscript(tab, tab->n_alloced), 0, new area)); */
		memset((char *)tab->buf + (tab->n_alloced * tab->unit), 0,
			(new_n_alloced - tab->n_alloced) * tab->unit);
		tab->n_alloced = new_n_alloced;
	}
	if (tab->n_alloced < tab->n_used + 1) {
		/*  BUG: Should have enough allocated by now. */
		return NULL;
	}

	/* Make pointer to new record */
	p = ((char *)tab->buf) + (n * tab->unit);

	/* If needed, move elements up to make space */
	if (n < tab->n_used) {
		void* dest = ((char *)p) + tab->unit;
		size_t count = (tab->n_used - n) * tab->unit;
		memmove(dest, p, count);
	}

	tab->n_used = tab->n_used + 1;

	/* Zero out new record */
	memset(p, 0, tab->unit);

	return ((char *)p);
}

/*-----------------------------------------------------------------------
 Return the address of the given element of the table.
 If the table isn't that big, return NULL.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
#ifdef dyatab_NONINLINE
void *dynatab_subscript(dynatab_t *tab, sint32 subscript)
{
	assert(tab != NULL);
	if (subscript < 0 || subscript >= tab->n_used)
		return NULL;
	return ((char *)tab->buf) + (subscript * tab->unit);
}
#endif

/*-----------------------------------------------------------------------
 Return the index of the element of the table that holds addr.
 If the table doesn't hold addr, return dynatab_NONE.
-----------------------------------------------------------------------*/
sint32					/* Index of addr, or dynatab_NONE */
dynatab_find(
	dynatab_t *tab,	/* table to search */
	void* addr)		/* value to search for */
{
	sint32 i;

	assert(tab != NULL);
	assert(addr != NULL);

	for(i = 0; i < tab->n_used; i++)
		if (0 == memcmp(addr, ((char*)tab->buf)+(i*tab->unit), tab->unit))
			return i;

	return dynatab_NONE;
}

/*****************/
/* Freeze & Thaw */
/*****************/

typedef struct {
	sint32 magic;
	sint32 n_used;
	uint32 unit;
} dynatab_freeze_t;
#define dynatab_MAGIC 0x9473

/*-------------------------------------------------------------------------
 Write the dynatab to the given file in a format suitable for restoration
 with dynatab_thaw.
 Assumes that the items contain no pointers or other data that needs to
 be modified to reflect new conditions upon thawing.
-------------------------------------------------------------------------*/
void dynatab_freeze(dynatab_t *tab, FILE *fp)
{
	dynatab_freeze_t d;
	size_t i;

	size_t datalen = tab->unit - sizeof(dptab_varinfo_t);

	d.magic  = dynatab_MAGIC;
	d.n_used = tab->n_used;
	d.unit   = (uint32)(datalen + sizeof(playerHdl_t) + sizeof(sint32) + sizeof(uint32));
//	These are the types of the following, see for loop below
//	This is necessary to have servers.dat compatible between Windows and Linux
//	d.unit   = (uint32)tab->unit - datalen + sizeof(varInfo->src) + sizeof(varInfo->hops) + sizeof(arrived);


assert(d.n_used <= 5000);
	DPRINT(("dynatab_freeze: Saving %d elements of size %d.\n", d.n_used, d.unit));
	i = fwrite(&d, sizeof(d), 1, fp);
	if (i != 1) {
		DPRINT(("dynatab_freeze: Error writing info.\n"));
		;
	}
	if (tab->n_used > 0) {
		size_t datalen = tab->unit - sizeof(dptab_varinfo_t);
		for(uint8* p = tab->buf; p < (uint8*)tab->buf + tab->unit * d.n_used; p += tab->unit) {
			if (fwrite(p, sizeof(char), datalen, fp) != datalen) {
				DPRINT(("dynatab_freeze: Error writing element %p.\n", p));
				;
			}

			dptab_varinfo_t* varInfo = (dptab_varinfo_t*)((char*)p + datalen);

			if (fwrite(&varInfo->src,  sizeof(varInfo->src),  1, fp) != 1) {
				DPRINT(("dynatab_freeze: Error writing element %p.\n", p));
				;
			}
			if (fwrite(&varInfo->hops, sizeof(varInfo->hops), 1, fp) != 1) {
				DPRINT(("dynatab_freeze: Error writing element %p.\n", p));
				;
			}

			uint32 arrived = (uint32)varInfo->arrived;
			if (fwrite(&arrived, sizeof(arrived), 1, fp) != 1) {
				DPRINT(("dynatab_freeze: Error writing element %p.\n", p));
				;
			}
		}
	}
}

/*-------------------------------------------------------------------------
 Read the dynatab from the given file.
 Replaces the previous contents of the dynatab.
 On success, returns a pointer to the first element.
 On failure, returns NULL.
 Assumes that the items contain no pointers or other data that needs to
 be modified to reflect new conditions upon thawing.
-------------------------------------------------------------------------*/
void *dynatab_thaw(dynatab_t *tab, FILE *fp)
{
	size_t i;
	void *p;
	dynatab_freeze_t d;

	i = fread(&d, sizeof(d), 1, fp);
	if (i != 1 || d.magic != dynatab_MAGIC) {
		/*  Read failed, or bad format */
		DPRINT(("dynatab_thaw: Could not read table info.\n"));
		return NULL;
	}
	if (d.magic != dynatab_MAGIC) {
		/*  Read failed, or bad format */
		DPRINT(("dynatab_thaw: signature incorrect: %d != %d\n", d.magic, dynatab_MAGIC));
		return NULL;
	}
	DPRINT(("dynatab_thaw: Reading %d elements of size %d.\n", d.n_used, d.unit));
	// This is needed to assure compatibilty of servers.dat files between Linux
	// and Windows. On 32 and 64 bit Windows, clock_t is 4 bytes, while on 
	// 64 bit Linux it is 8 bytes.
	tab->unit = (size_t)d.unit - sizeof(uint32) + sizeof(clock_t);
	if (d.n_used != 0) {
		p = dynatab_subscript_grow(tab, d.n_used-1);
		if (!p) {
			DPRINT(("dynatab_thaw: Could not allocate element %p\n", p));
			return NULL;
		}

		size_t datalen = tab->unit - sizeof(dptab_varinfo_t);
		for(uint8* p = tab->buf; p < (uint8*)tab->buf + tab->unit * d.n_used; p += tab->unit) {
			if(fread(p, sizeof(char), datalen, fp) != datalen) {
				DPRINT(("dynatab_thaw: error reading buf %p\n", p));
				return NULL;
			}

			/* Seperate these so that we do not have a problem with packing*/
			dptab_varinfo_t* varInfo = (dptab_varinfo_t*)((char*)p + datalen);

			if(fread(&varInfo->src, sizeof(varInfo->src), 1, fp) != 1) {
				DPRINT(("dynatab_thaw: error reading varInfo->src %p\n", p));
				return NULL;
			}
			if(fread(&varInfo->hops, sizeof(varInfo->hops), 1, fp) != 1) {
				DPRINT(("dynatab_thaw: error reading varInfo->hops %p\n", p));
				return NULL;
			}

			uint32 arrived = 0; /* This needs to be 32 bit for compatibilty, clock_t is 32 bit on 64 bit Windows and 64 bit on 64 bit Linux */
			if(fread(&arrived, sizeof(arrived), 1, fp) != 1) {
				DPRINT(("dynatab_thaw: error reading varInfo->arrived %p\n", p));
				return NULL;
			}
			varInfo->arrived = (clock_t)arrived;
		}
	} else {
		/*  Kludge: force non-NULL buffer */
		/*  Should just change return convention */
		p = dynatab_subscript_grow(tab, 1);
	}
	tab->n_used = d.n_used;
	DPRINT(("dynatab_thaw: %p of size %zu successful.\n", tab, tab->unit));
	return (void *) tab->buf;
}

#if defined(dynatab_UNITTEST)

#include <stdio.h>

void test1(dynatab_t *pt)
{
	sint32 i;
	FILE *fp;
	dynatab_t *th;

	for (i=32; i>=0; i--) {
	/* for (i=0; i<33; i++) { */
		sint32 *p = dynatab_subscript_grow(pt, i);
		if (p == NULL) {
			printf("grow1: test failed on i=%d\n", i);
			exit(1);
		}
		if (*p != 0) {
			printf("init value test: test failed\n");
			exit(1);
		}
		*p = i;
		printf(".");
	}
	for (i=0; i<33; i++) {
		sint32 *p = dynatab_subscript(pt, i);
		if (p == NULL) {
			printf("subscript2: test failed\n");
			exit(1);
		}
		if (*p != i) {
			printf("value test: test failed\n");
			exit(1);
		}
		if (dynatab_find(pt, p) != i) {
			printf("find test: test failed\n");
			exit(1);
		}
	}
	if (dynatab_subscript(pt, i) != NULL) {
		printf("subscript3: test failed\n");
		exit(1);
	}

	/* Insert an out-of-order value in middle */
	{
		sint32 *p = dynatab_subscript_insert(pt, 10);
		if (p == NULL) {
			printf("insert1: test failed\n");
			exit(1);
		}
		if (*p) {
			printf("insert1: initial value test failed\n");
			exit(1);
		}
		*p = 1000;

		for (i=0; i<34; i++) {
			sint32 *p = dynatab_subscript(pt, i);
			sint32 val;

			if (p == NULL) {
				printf("insert subscript2: test failed\n");
				exit(1);
			}
			val = i;
			if (i == 10) val = 1000;
			if (i > 10) val--;

			if (*p != val) {
				printf("value test after insert: test failed; i %d, *p %d, val %d\n", i, *p, val);
				exit(1);
			}
			if (dynatab_find(pt, p) != i) {
				printf("find test: test failed\n");
				exit(1);
			}
		}
		dynatab_remove(pt, 10);
	}

	fp = fopen("spam.dat", "wb");
	if (!fp) {
		perror("spam.dat");
		exit(1);
	}
	dynatab_freeze(pt, fp);
	if (fclose(fp)) {
		perror("closing spam.dat");
		exit(1);
	}
	th = dynatab_create(sizeof(sint32));
	if (!th) exit(1);

	fp = fopen("spam.dat", "rb");
	if (!fp) {
		perror("spam.dat");
		exit(1);
	}
	if (!dynatab_thaw(th, fp)) {
		perror("thaw");
		exit(1);
	}
	fclose(fp);
	if ((th->n_used != pt->n_used) || (th->unit != pt->unit)) {
		printf("bad n_used or unit: test failed\n");
		exit(1);
	}
	for (i=0; i<33; i++) {
		sint32 *p = dynatab_subscript(th, i);
		if (p == NULL) {
			printf("subscript2: test failed\n");
			exit(1);
		}
		if (*p != i) {
			printf("value test: test failed\n");
			exit(1);
		}
		if (dynatab_find(th, p) != i) {
			printf("find test: test failed %d\n", i);
			exit(1);
		}
	}
	for (i = 0; i < 20; i++) {	/* Don't remove them all */
		sint32 *p = dynatab_subscript(th, 0);
		sint32 val = *p;
		if (p == NULL) {
			printf("subscript3: test failed\n");
			exit(1);
		}
		dynatab_remove(th, 0);
		if (val == *((sint32*)dynatab_subscript(th, 0))) {
			printf("remove test: test failed\n");
			exit(1);
		}
		if (dynatab_find(th, &val) != dynatab_NONE) {
			printf("find-after-remove test: test failed\n");
			exit(1);
		}
	}
	dynatab_destroy(th);
}

#if defined(dynatab_UNITTEST)

#include <malloc.h>		/* for _heapchk() */

#define NTABS 100
main()
{
	sint32 i;
	dynatab_t *pt[NTABS];

	for (i=0;i<NTABS;i++) {
		pt[i] = dynatab_create(sizeof(sint32));
		if (!pt[i]) {
			printf("create: test failed\n");
			exit(1);
		}
		if (dynatab_subscript(pt[i], 1) != NULL) {
			printf("subcript1: test failed\n");
			exit(1);
		}
	}
	for (i=0;i<NTABS;i++) {
		sint32 h;
		h = _heapchk();
		if (h != _HEAPOK) {
			printf("heapchk: test failed\n");
			exit(1);
		}
		test1(pt[i]);
	}
	for (i=0;i<NTABS;i++)
		dynatab_destroy(pt[i]);
	printf("test okay\n");
	return 0;
}

#endif /* dynatab_UNITTEST */
#endif /* UNITTEST */
