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
 $Log: dynatab.h $
 Revision 1.10  1997/09/24 22:22:30  lbennett
 Added dynatab_size().
 Revision 1.9  1997/09/24 21:42:31  lbennett
 Added dynatab_find and dynatab_remove.
 Revision 1.8  1997/07/26 18:05:28  dkegel
 Don't use #pragma pack; include dp*pack.h instead
 Revision 1.7  1997/05/31 22:42:23  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.6  1997/01/31 07:47:42  dkegel
 Settled the structure packing question by using 
 #pragma pack(x) / #pragma pack() pairs at top and bottom of
 all .h files that declare structures.  Was careful to put 
 #pragma pack(x) after last #include (to avoid wierd nesting effects
 due to lack of #pragma pack(pop).
 Revision 1.5  1996/12/18 18:20:29  administrator
 Change to c style comments for benefit of gcc.
 Revision 1.4  1996/07/23 00:30:57  dkegel
 Use standard C comments for portability.
 Revision 1.3  1996/04/18 00:45:00  dkegel
 Fixed bug where size 0 tables could not be thawed.
 Fixed typo in .h file.
 Revision 1.2  1996/04/17 21:07:40  dkegel
 Added freeze and thaw methods.
 Revision 1.1  1996/01/30 01:14:33  dkegel
 Initial revision

 A simple dynamically sized table.
-----------------------------------------------------------------------*/

#ifndef dynatab_h
#define dynatab_h

#include <stdio.h>
#include <stddef.h>

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.
 * Note: this must come after all other #include's!
 */
#include "dppackn.h"

/* Not-an-index value */
#define dynatab_NONE	-1

typedef struct {
	void *buf;
	size_t unit;	/*  Size of one element in the buffer. */
	int  n_used;	/*  How many elements in the table have been used. */
	int  n_alloced;	/*  How many have been allocated. */
} dynatab_t;

/*-----------------------------------------------------------------------
 Create a table whose elements all have the given size.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
dynatab_t *dynatab_create(size_t element_size);

/*-----------------------------------------------------------------------
 Destroy a table.
-----------------------------------------------------------------------*/
void dynatab_destroy(dynatab_t *ptab);

/*-----------------------------------------------------------------------
 Remove the given element of the table.
 WARNING: Silently does nothing if subscript is out of range.
-----------------------------------------------------------------------*/
void dynatab_remove(dynatab_t *tab, int subscript);

/*-----------------------------------------------------------------------
 Remove all elements of the table, but don't touch its allocation.
-----------------------------------------------------------------------*/
void dynatab_removeAll(dynatab_t *tab);

/*-----------------------------------------------------------------------
 Return the address of the given element of the table.
 If the table isn't that big, grow.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *dynatab_subscript_grow(dynatab_t *tab, int subscript);

/*-----------------------------------------------------------------------
 Inserts a record into the table before the nth record.
 Grows the table.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *dynatab_subscript_insert(dynatab_t *tab, int n);

/*-----------------------------------------------------------------------
 Return the address of the given element of the table.
 If the table isn't that big, return NULL.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
#ifdef dyatab_NONINLINE
void *dynatab_subscript(dynatab_t *tab, int subscript);
#else
#define dynatab_subscript(tab, subscript) \
	((void *)(((subscript) < 0 || (subscript) >= (tab)->n_used) ? NULL : \
	(((char *)((tab)->buf) + ((subscript) * (tab)->unit)))))
#endif

/*-----------------------------------------------------------------------
 Return the index of the element of the table that holds addr.
 If the table doesn't hold addr, return dynatab_NONE.
-----------------------------------------------------------------------*/
int dynatab_find(dynatab_t *tab, void* addr);

/*-----------------------------------------------------------------------
 Return the number of elements in the table.
-----------------------------------------------------------------------*/
#define dynatab_size(tab)	(tab->n_used)

/*-----------------------------------------------------------------------
 Return the number of elements in the table.
-----------------------------------------------------------------------*/
#define dynatab_elemSize(tab)	(tab->unit)

/*-------------------------------------------------------------------------
 Write the dynatab to the given file in a format suitable for restoration
 with dynatab_thaw.
-------------------------------------------------------------------------*/
void dynatab_freeze(dynatab_t *tab, FILE *fp);

/*-------------------------------------------------------------------------
 Read the dynatab from the given file.
 Replaces the previous contents of the dynatab.
 On success, returns a pointer to the first element.
 On failure, returns NULL.
-------------------------------------------------------------------------*/
void *dynatab_thaw(dynatab_t *tab, FILE *fp);

#include "dpunpack.h"
#endif
