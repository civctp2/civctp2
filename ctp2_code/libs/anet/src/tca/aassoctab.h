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
 $Log: assoctab.h $
 Revision 1.7  1997/09/27 05:33:59  lbennett
 Added assertValid.
 Revision 1.6  1997/05/31 22:42:16  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.5  1997/01/31 07:46:11  dkegel
 Settled the structure packing question by using
 #pragma pack(x) / #pragma pack() pairs at top and bottom of
 all .h files that declare structures.  Was careful to put
 #pragma pack(x) after last #include (to avoid wierd nesting effects
 due to lack of #pragma pack(pop).
 Revision 1.4  1996/12/18 18:19:09  administrator
 Change to c style comments for benefit of gcc.
 Revision 1.3  1996/07/23 00:31:00  dkegel
 Use standard C comments for portability.
 Revision 1.2  1996/04/18 00:45:14  dkegel
 Added freeze and thaw methods.
 Revision 1.1  1996/02/16 20:13:48  dkegel
 Initial revision

 A simple dynamically sized associative table.
 For simplicity, key values will be integers.
-----------------------------------------------------------------------*/

#ifndef aassoctab_h
#define aassoctab_h

#include <stddef.h>
#include "dynatab.h"
#include "crypttab.h"

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.  Let's pick 4.
 * Note: Pragma pack must come after all other #include's!
 */
#include "dppackn.h"

/*  The key is stored immediately before the value. */
typedef struct {
	int key;
	char value[1];	/*  Dummy size. */
} assoctab_item_t;

typedef
	dynatab_t
assoctab_t;

/*-----------------------------------------------------------------------
 Create a table whose elements all have the given size.
 Element size is the size of the value; it doesn't include the size of
 the key.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
assoctab_t *assoctab_create(size_t element_size);

/*-----------------------------------------------------------------------
 Destroy a table.
-----------------------------------------------------------------------*/
void assoctab_destroy(assoctab_t *ptab);

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, grow.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *assoctab_subscript_grow(assoctab_t *tab, int key);

/*-----------------------------------------------------------------------
 Delete the given key from the table, shifts higher keys down
 (in the assoctab_getkey sense).
 Returns 0 on success, 1 on failure.
-----------------------------------------------------------------------*/
int assoctab_subscript_delete(assoctab_t *tab, int key);

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, return NULL.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *assoctab_subscript(assoctab_t *tab, int key);

/*-----------------------------------------------------------------------
 Return ptr to the nth key/value pair.  N must be between 0 and tab->n_used.
 If you want to scan thru a table and delete items, do it
 from the top downwards.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
#define assoctab_getkey(tab, n) ((assoctab_item_t *)dynatab_subscript(tab, n))

/*-------------------------------------------------------------------------
  Write the dynatab to file in an encrypted format suitable for reading
  with assoctab_thaw.
-------------------------------------------------------------------------*/
#define assoctab_freeze(tab, fp, key) dynatab_freeze_encrypted(tab, fp, key)

/*-------------------------------------------------------------------------
  Read the dynatab from the encrypted file.
-------------------------------------------------------------------------*/
#define assoctab_thaw(tab, fp, key) dynatab_thaw_encrypted(tab, fp, key)

/*-------------------------------------------------------------------------
  Debug-only support
-------------------------------------------------------------------------*/
#if !defined(NDEBUG)

#define assoctab_assertValid(a) {\
	assert((a) != NULL);\
	assert((a)->unit < 20000);\
	assert((a)->n_used < 5000);\
	assert((a)->n_alloced < 5000);\
}

#else
#define assoctab_assertValid(d)
#endif

#include "dpunpack.h"

#endif
