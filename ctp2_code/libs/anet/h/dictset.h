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
Dictionary-Set

A set of key-value elements, supporting fast lookup of the element
from either the key or the value.  Neither duplicate keys nor duplicate
values are allowed; ordering of elements is unimportant and is not
guaranteed to be preserved in any sense.

Usage Notes
-----------
Call dcstCreate() to create a dictionary set.  Once created, use
dcstAdd() to put elements into the set; use dcstDelete*() to remove
elements from the set when they are no longer needed.  To find out
whether an element is in the set, or to get the other half of an
element, use the dcstFind*() methods.  For more complex tasks
involving the elements in a set (e.g., find an element whose
value contains a specific pattern, or add one to the value of
each element), use dcstForEach().  When the dictionary set is no
longer needed, call dcstDestroy().

Each element in the set is a key-value pair.  The key is assigned
by the set when the element is added to the set, and has the type
dcst_key_t (an integer type).  The value is chosen by the user of
the set; the set stores a pointer to it but never changes its value.

The set contains only one value for any given key, and only one key
for any given value.  Values are compared using a byte-by-byte
comparison of the first N bytes of the value, where N is chosen
by the user of the set at create time (the valueSize parameter to
dcstCreate()).

The maximum number of elements in the set is chosen at create time.
This number may not be increased subsequently.  Be sure to choose
a large enough number for the expected use, and be sure to fail
gracefully if dcstAdd() fails.

Debugging
---------
When compiled in debug mode, this module will check all parameters and
an assertion will fail if they are out of range.  Additionally, most
calls will check the set for internal consistency.  These checks will
slow execution noticably.  Programs using this module may also check
a set for internal consistency, using the dcst_ASSERTVALID() macro.

It is strongly recommended that programmers use the debug version at
any time a problem is suspected in this module.  If the speed penalty
is not prohibitive, it is recommended that the debug version be used
throughout development.

Compilation Control
-------------------
Selftest: Define DCST_SELFTEST to include all selftest code; remove the
  definition to omit selftest code.  The selftest is designed to be run
  as a standalone console application.
Debug: Do not define NDEBUG, or define _DEBUG, to include all debugging
  code (debug version); define NDEBUG and do not define _DEBUG to omit
  debug code (release version).

Copyright (c) 1997 by Activision Inc.  All rights reserved.
Copyright protection claimed includes all forms and matters of
copyrightable material and information now allowed by statutory or
judicial law or hereinafter granted, including without limitation,
material generated from the software programs which are displayed
on the screen such as icons, screen display looks, etc.

 $Log: dictset.h $
 Revision 1.4  1997/09/03 21:32:24  lbennett
 Really put dcst_AssertValid under DCST_ASSERTALL define.
 Revision 1.3  1997/09/03 20:53:40  lbennett
 Moved dcst_ASSERTALL to special debug switch.
 Revision 1.2  1997/08/27 23:30:04  lbennett
 Added dcstReplace().
 Revision 1.1  1997/08/26 01:41:42  lbennett
 Initial revision
-----------------------------------------------------------------------*/

/* Protect against double-inclusion */
#if !defined(_DICTSET_H_)
#define _DICTSET_H_

#include <stdio.h>

/**
* Constants
*/

/* Value of dcst_key_t to be interpreted as an invalid key */
#define dcst_INVALID_KEY	-1

/* Allowed values for dcst_status_t */
#define dcst_STATUS_OK			0	/* All is well */
#define dcst_STATUS_DEFERRED	1	/* Call to a method not yet implemented */

/**
* Types
*/

/* Type for the dictionary-set itself */
typedef struct dcst_s* dcst_t;

/* Type for status return values */
typedef int dcst_status_t;

/* Type for the keys into the dictionary */
typedef int dcst_key_t;

/* Type for the values in the dictionary */
typedef void* dcst_value_t;

/* Type for the for-each method callback (iterator callback)
   Return 0 to continue to next element, nonzero to end the
   for-each call immediately */
typedef int (*dcst_forEach_cb) (
	dcst_t pDcst,		/* set being operated on */
	dcst_key_t key,		/* key of current element */
	dcst_value_t value,	/* value of current element */
	void *context);		/* context passed to dcstForEach() */

/**
* Methods
*/

/* Create a dictionary set, whose values are the given size, and which
can accomodate a maximum of maxKeys elements.  Use maxHashEntries to
tune the size vs. speed of the lookup; a value of maxKeys/2 should
produce close to optimum response time. */
dcst_t dcstCreate(size_t valueSize, int maxKeys, int maxHashEntries);

/* Destroy a dictionary set */
void dcstDestroy(dcst_t pDcst);

/* Add a value to the dictionary set.  Returns the key under which the
value is stored; if the value is already stored in the set, then the
key of the existing value is returned (and the value is not added
to the set a second time). */
dcst_key_t dcstAdd(dcst_t pDcst, dcst_value_t value);

/* Adds a value to the dictionary using a specifiy key.  Useful when
there are multiple dictionaries with common keys */
dcst_key_t dcstAddEx(dcst_t pDcst, dcst_key_t key, dcst_value_t value);

/* Replace a value in the dictionary set.  Deletes any element stored
under the key, and adds the new value to the set.  Returns the key
of the new element. */
dcst_key_t dcstReplace(dcst_t pDcst, dcst_key_t key, dcst_value_t value);

/* Replaces the key ensuring that the new data has the same key value */
dcst_key_t dcstReplaceEx(dcst_t pDcst, dcst_key_t key, dcst_value_t value);

/* Delete a value from the dictionary set (the key is also deleted).
It is an error to call this with a value that is not in the set. */
dcst_status_t dcstDeleteValue(dcst_t pDcst, dcst_value_t value);

/* Delete a key from the dictionary set (the value is also deleted).
It is an error to call this with a key that is not in the set. */
dcst_status_t dcstDeleteKey(dcst_t pDcst, dcst_key_t key);

/* Find the key associated with the given value in the dictionary set.
Returns the key, or dcst_INVALID_KEY if the value is not in the set. */
dcst_key_t dcstFindKey(dcst_t pDcst, dcst_value_t value);

/* Find the value associated with the given key in the dictionary set.
Return the value, or NULL if the key is not in the set. */
dcst_value_t dcstFindValue(dcst_t pDcst, dcst_key_t key);

/* Call a method on each member of the set */
/* cb will be called once for each member, with parameters set as follows:
	pDcst - The table being handled
	key - The key of the current element
	value - The value of the current element
	context - The context passed in to dcstForEach()
The cb method should not depend on receiving the elements in any
particular order, as none is guaranteed.  Adding or deleting elements
of the set from within the callback may produce unexpected results.
The cb must return 0 to continue to next element, nonzero to end the
for-each call immediately */
void dcstForEach(dcst_t pDcst, dcst_forEach_cb cb, void* context);

/* Save a dictionary set to a stream.  Not yet implemented. */
dcst_status_t dcstFreeze(dcst_t pDcst, FILE *fp);

/* Restore a dictionary set from a stream.  Not yet implemented. */
dcst_status_t dcstThaw(dcst_t pDcst, FILE *fp);

/**
* Debugging support
*/
#if defined(DCST_ASSERTALL)

/* Assert that the dictset has a consistent internal structure */
#define dcst_ASSERTVALID(d)	dcstAssertValid(d,__FILE__, __LINE__)
void dcstAssertValid(dcst_t pDcst, char* file, int line);

#else

/* Do nothing in non-debug mode */
#define dcst_ASSERTVALID(d)

#endif /* debugging support */

/**
* Self-test support
*/
#if DCST_SELFTEST

/* Test callback for dcstForEach() */
int forEachTestCb (
	dcst_t pDcst,		/* Dictset being operated on */
	dcst_key_t key,		/* key of current element */
	dcst_value_t value,	/* value of current element */
	void *context);		/* context passed to dcstForEach() */

/* Test executive */
int main(int argc, char** argv);

#endif /* self-test support */

#endif /* Protect against double-inclusion */
