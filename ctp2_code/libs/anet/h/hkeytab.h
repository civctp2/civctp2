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
 $Log: hkeytab.h $
 Revision 1.1  1997/06/09 23:37:04  dkegel
 Initial revision

 A simple dynamically sized associative table.
 Key values are variable length binary strings.
-----------------------------------------------------------------------*/

#ifndef hkeytab_h
#define hkeytab_h

#include <stddef.h>
#include "dynatab.h"

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.  Let's pick 1, to make it easier to calculate
 * partial struct sizes.
 * Note: Pragma pack must come after all other #include's!
 */
#include "dppack1.h"

#define hkeytab_MAXLEN 12    /* Keys must be 12 bytes in length or less. */

/*  The key is stored immediately before the value. */
typedef struct {
	char keylen PACK;
	char key[hkeytab_MAXLEN] PACK;
	char value[1] PACK;	/*  Dummy size. */
} hkeytab_item_t;
#define sizeof_hkeytab_item_t(n) (sizeof(char) + hkeytab_MAXLEN + (n))

typedef 
	dynatab_t
hkeytab_t;

/*-----------------------------------------------------------------------
 Create a table whose elements all have the given size.
 Element size is the size of the value; it doesn't include the size of
 the key.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
hkeytab_t *hkeytab_create(size_t element_size);

/*-----------------------------------------------------------------------
 Destroy a table.
-----------------------------------------------------------------------*/
void hkeytab_destroy(hkeytab_t *ptab);

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, grow.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *hkeytab_subscript_grow(hkeytab_t *tab, void *key, size_t keylen);

/*-----------------------------------------------------------------------
 Delete the given key from the table, shifts higher keys down
 (in the hkeytab_getkey sense).
 Returns 0 on success, 1 on failure.
-----------------------------------------------------------------------*/
int hkeytab_subscript_delete(hkeytab_t *tab, void *key, size_t keylen);

/*-----------------------------------------------------------------------
 Return the address of the value part of the given element of the table.
 If the table doesn't contain that key, return NULL.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
void *hkeytab_subscript(hkeytab_t *tab, const void *key, size_t keylen);

/*-----------------------------------------------------------------------
 Return ptr to the nth key/value pair.  N must be between 0 and tab->n_used.
 If you want to scan thru a table and delete items, do it
 from the top downwards.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
#define hkeytab_getkey(tab, n) ((hkeytab_item_t *)dynatab_subscript(tab, n))

/*-------------------------------------------------------------------------
 Write the dynatab to the given file in a format suitable for restoration
 with hkeytab_thaw.
-------------------------------------------------------------------------*/
#define hkeytab_freeze(tab, fp) dynatab_freeze(tab, fp)

/*-------------------------------------------------------------------------
 Restore the dynatab from disk to recreate the original hkeytab_t.
-------------------------------------------------------------------------*/
#define hkeytab_thaw(tab, fp) dynatab_thaw(tab, fp)

#include "dpunpack.h"

#endif
