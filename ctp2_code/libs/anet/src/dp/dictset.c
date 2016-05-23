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

A set of key-value elements, supporting fast lookup of the elements
from either the key or the value.  Neither duplicate keys nor duplicate
values are allowed; ordering of elements is unimportant and is not
guaranteed to be preserved in any sense.

Implementation Notes
--------------------
Because of the requirement for fast lookup, we allocate a lookup scheme
for each direction (a key-indexed array for the key-to-value direction,
a value-indexed hash table for the value-to-key direction).

Another design constraint was that the Add method should not fail for
lack of memory.  For that reason, we pre-allocate an element for each
possible key (which, because of the 1:1 key-to-value mapping, is the
most possible elements we can handle).  Thus, we never need to allocate
during the add method.

Copyright (c) 1997 by Activision Inc.  All rights reserved.

 $Log: dictset.c $
 Revision 1.5  1997/09/03 22:31:36  lbennett
 Fixed.
 Revision 1.4  1997/09/03 20:53:20  lbennett
 Moved dcst_ASSERTALL to special debug switch.
 Revision 1.3  1997/08/29 01:15:24  lbennett
 Removed MSVC-specific pragmas from non-WIN32 builds.
 Revision 1.2  1997/08/27 23:29:23  lbennett
 Added dcstReplace().
 Revision 1.1  1997/08/26 01:43:32  lbennett
 Initial revision
-----------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <limits.h>
#if defined(__MWERKS__) || defined(__GNUC__)
    #include <string.h>
#else
    #include <malloc.h>
#endif
#include "dpmem.h"

/* Re-enable MSVC warnings (#@$&%#^& MS) */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#endif

#include "dictset.h"

/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows *linked* code has six of these ) */
#if defined(WIN32)
#pragma warning( disable : 4514 )
#endif

/**
* Types
*/

/* Hold one element (key-value pair) in the hashtable */
typedef struct dcst_element_s {
	dcst_key_t key;
	dcst_value_t value;
	struct dcst_element_s* next;
} dcst_element_t;

/* Struct for the dictionary-set itself (type declared in dictset.h) */
struct dcst_s {
	dcst_element_t* keys;		/* array, indexed by key, of elements */
	dcst_element_t** htab;		/* hashtable (array of ptrs to elements) */
	dcst_value_t* valueStorage;	/* array to hold values of elements */
	size_t valueSize;			/* number of bytes in a value */
	int maxKeys;				/* number of elements in keys */
	int maxHash;				/* number of pointers in htab */
};

/**
* Private Methods
*/

/*****************************************************************************
Return the next available key
*****************************************************************************/
dcst_key_t			/* An unused key */
dcstNextKey(
	dcst_t pDcst)	/* Set to examine */
{
	dcst_key_t i;
	dcst_ASSERTVALID(pDcst);			/* set is invalid */
	for(i= 0; i < pDcst->maxKeys; i++)
		if(pDcst->keys[i].key == dcst_INVALID_KEY)
			return i;
	return dcst_INVALID_KEY;
}

/*****************************************************************************
Return the hash index that corresponds to the value
*****************************************************************************/
int						/* The hash */
dcstHashvalue(
	dcst_t pDcst,		/* Set to examine */
	dcst_value_t value)	/* Value to compute hash of */
{
	unsigned char* val = (unsigned char*)value;
	int rVal = 0;
	unsigned int i;
	int step = 0;
	int limStep = 0;

	/* Get a type that's just big enough to hold the hash size.
	   Treat value as an array of the appropriate type, and XOR the
	   array elements together to form the base hash number */
	if (pDcst->maxHash < UCHAR_MAX)
		limStep = sizeof(unsigned char);
	else if (pDcst->maxHash < USHRT_MAX)
		limStep = sizeof(unsigned char);
	else
		limStep = sizeof(unsigned int);

	/* XOR each byte of value against a different byte of the hash */
	for(i = 0; i < pDcst->valueSize; i++) {
		rVal ^= (unsigned int)(((unsigned int)val[i]) << (step * sizeof(unsigned char)));
		step = (step + 1) % limStep;
	}

	/* Fold value to correct size and return */
	return rVal % pDcst->maxHash;
}

/**
* Public Methods
*/

/*****************************************************************************
Create a dictionary set.
The set will contain values of size valueSize, and will allow the client to
store up to maxKeys entries in the set.  The set will use maxHashEntries
as the number of hashtable bins to set up; use this number for tuning.
Returns the new dictionary set, or NULL if unable to create one.
*****************************************************************************/
dcst_t 					/* The new dictionary set */
dcstCreate(
	size_t valueSize,	/* Number of bytes in a value in the dictset */
	int maxKeys,		/* Largest number of keys the set can support */
	int maxHashEntries)	/* Number of hash bins the set uses */
{
	dcst_t newSet;
	int i;
	char* pVal;

	/* Check preconditions */
	assert(valueSize > 0);		/* set must contain elements */
	assert(maxKeys > 1);		/* set must have at least 2 keys */
	assert(maxHashEntries > 1);	/* set must have at least 2 hashtable bins */

	/* Allocate space for the new stuff */
	newSet = (dcst_t) dp_MALLOC(sizeof(struct dcst_s));
	if(newSet == NULL)
		return NULL;
	newSet->keys = (dcst_element_t *) dp_MALLOC(maxKeys * sizeof(dcst_element_t));
	if(newSet->keys == NULL) {
		dp_FREE(newSet);
		return NULL;
	}
	newSet->htab = (dcst_element_t **) dp_MALLOC(maxHashEntries * sizeof(dcst_element_t*));
	if(newSet->htab == NULL) {
		dp_FREE(newSet->keys);
		dp_FREE(newSet);
		return NULL;
	}
	newSet->valueStorage = (void **) dp_MALLOC(maxKeys * valueSize);
	if(newSet->valueStorage == NULL) {
		dp_FREE(newSet->htab);
		dp_FREE(newSet->keys);
		dp_FREE(newSet);
		return NULL;
	}

	/* Initialize the values */
	newSet->valueSize = valueSize;
	newSet->maxKeys = maxKeys;
	newSet->maxHash = maxHashEntries;
	memset(newSet->valueStorage, 0, maxKeys*valueSize);
	pVal = (char*)newSet->valueStorage;
	for(i = 0; i < newSet->maxKeys; i++) {
		newSet->keys[i].key = dcst_INVALID_KEY;
		newSet->keys[i].value = pVal;
		newSet->keys[i].next = NULL;
		pVal += valueSize;
	}
	for(i = 0; i < newSet->maxHash; i++)
		newSet->htab[i] = NULL;

	/* Check postconditions */
	dcst_ASSERTVALID(newSet);			/* newly created set is invalid */

	return newSet;
}

/*****************************************************************************
Destroy a dictionary set
*****************************************************************************/
void
dcstDestroy(
	dcst_t pDcst)	/* set to destroy */
{
	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);					/* invalid set */

	/* Deallocate space */
	dp_FREE(pDcst->valueStorage);
	dp_FREE(pDcst->htab);
	dp_FREE(pDcst->keys);
	dp_FREE(pDcst);

	/* Check postconditions */
	/* none */
}

/*****************************************************************************
Add a value to the dictionary set.
If the value already exists in the set, do not add it a second time.
Return the key associated with the value, or dcst_INVALID_KEY if the value
could not be added (which only happens when the set is full).
*****************************************************************************/
dcst_key_t				/* key of new element */
dcstAdd(
	dcst_t pDcst,		/* set to operate on */
	dcst_value_t value)	/* value to add */
{
	dcst_key_t key = dcst_INVALID_KEY;

	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);					/* invalid set */
	assert(value != NULL);				/* adding nothing */
	/* assert(allocsize(value) >= pDcst->valueSize); */

	/* Check for existing value in set */
	key = dcstFindKey(pDcst, value);

	/* Insert value */
	if(key == dcst_INVALID_KEY) {
		key = dcstNextKey(pDcst);
		if(key != dcst_INVALID_KEY) {	/* if there's space left */
			int hash = dcstHashvalue(pDcst, value);
			pDcst->keys[key].key = key;
			memcpy(pDcst->keys[key].value, value, pDcst->valueSize);
			pDcst->keys[key].next = pDcst->htab[hash];
			pDcst->htab[hash] = &(pDcst->keys[key]);
		}
	}

	/* Check postconditions */
	dcst_ASSERTVALID(pDcst);					/* set rendered invalid */

	return key;
}

/*****************************************************************************
Add a value to the dictionary set using a specific key.
Test to see if the key exists using find key.
If the key does exist then a replace will suffice.
If the key does not exist then add it.
*****************************************************************************/
dcst_key_t				    /* key of new element */
dcstAddEx(
	dcst_t pDcst,		        /* set to operate on */
	dcst_key_t newkey,      /* key to add */
	dcst_value_t newvalue)	/* value to add */
{
	dcst_value_t value = NULL;

	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);			/* invalid set */
	assert(newvalue != NULL);	  	/* adding nothing */
	assert(newkey != dcst_INVALID_KEY);
	/* assert(allocsize(value) >= pDcst->valueSize); */

	/* Check to see if this key has data assigned to it */
	value = dcstFindValue(pDcst, newkey);

	/* Insert value */
	if (value == NULL)
	{
		if (newkey != dcst_INVALID_KEY)
		{
			int hash = dcstHashvalue(pDcst, newvalue);

			pDcst->keys[newkey].key = newkey;
			memcpy(pDcst->keys[newkey].value, newvalue, pDcst->valueSize);
			pDcst->keys[newkey].next = pDcst->htab[hash];
			pDcst->htab[hash] = &(pDcst->keys[newkey]);
		}
	}
	else
	{
		/* Replace value */
		dcstReplace(pDcst, newkey, newvalue);
	}

	/* Check postconditions */
	dcst_ASSERTVALID(pDcst);					/* set rendered invalid */

	return newkey;
}

/*****************************************************************************
Replace a value in the dictionary set.
Deletes any value associated with the key; adds the value to the set
and returns the new key associated with it.
Return the key associated with the value, or dcst_INVALID_KEY if the value
could not be added.
*****************************************************************************/
dcst_key_t				/* key of new element */
dcstReplace(
	dcst_t pDcst,		/* set to operate on */
	dcst_key_t key,		/* key of element to replace */
	dcst_value_t value)	/* value of new element */
{
	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);			/* invalid set */
	assert(value != NULL);				/* adding nothing */
	/* assert(allocsize(value) >= pDcst->valueSize); */

	/* Delete old, insert new */
	if(dcst_INVALID_KEY != key)
		dcstDeleteKey(pDcst, key);
	return dcstAdd(pDcst, value);
}

/*****************************************************************************
Replace a value in the dictionary set.
Deletes any value associated with the key; adds the value to the set
and returns the new key associated with it.
The new key will be identical to the one passed in
Return the key associated with the value, or dcst_INVALID_KEY if the value
could not be added.
*****************************************************************************/
dcst_key_t				/* key of new element */
dcstReplaceEx(
	dcst_t pDcst,		/* set to operate on */
	dcst_key_t key,		/* key of element to replace */
	dcst_value_t value)	/* value of new element */
{
	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);			/* invalid set */
	assert(value != NULL);				/* adding nothing */
	/* assert(allocsize(value) >= pDcst->valueSize); */

	/* Delete old, insert new */
	if(dcst_INVALID_KEY != key)
	{
		dcstDeleteKey(pDcst, key);
	}
	return dcstAddEx(pDcst, key, value);
}


/*****************************************************************************
Delete a value from the dictionary set (the key is also deleted).
It is an error to call this method with a NULL pDcst or a NULL value, or
with a value that's not a member of the set.
Returns the status of the delete (always dcst_STATUS_OK).
*****************************************************************************/
dcst_status_t			/* completion status */
dcstDeleteValue(
	dcst_t pDcst,		/* Set to delete from */
	dcst_value_t value)	/* value of element to delete */
{
	dcst_key_t key;

	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);					/* invalid set */
	assert(value != NULL);				/* deleting nothing */

	/* Find the key */
	key = dcstFindKey(pDcst, value);
	assert(dcst_INVALID_KEY != key);	/* deleting a non-member */

	/* Delete by key; rely on postcondition checking in dcstDeleteKey() */
	return dcstDeleteKey(pDcst, key);
}

/*****************************************************************************
Delete a key from the dictionary set (the value is also deleted).
It is an error to call this method with a NULL pDcst, invalid key, or a key
that is not in use in the set.
Returns the status of the delete (always dcst_STATUS_OK).
*****************************************************************************/
dcst_status_t			/* completion status */
dcstDeleteKey(
	dcst_t pDcst,		/* set to delete from */
	dcst_key_t key)		/* key of element to delete */
{
	int hash;

	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);									/* invalid set */
	assert(dcst_INVALID_KEY != key);					/* key not valid */
	assert((key >= 0) && (key < pDcst->maxKeys));		/* key out of range */
	assert(dcst_INVALID_KEY != pDcst->keys[key].key);	/* key not in use */

	/* Remove the element from the hashtable */
	hash = dcstHashvalue(pDcst, pDcst->keys[key].value);
	if(pDcst->htab[hash] == &(pDcst->keys[key]))
		pDcst->htab[hash] = pDcst->htab[hash]->next;
	else {
		dcst_element_t* elem;
		for(elem = pDcst->htab[hash]; elem != NULL; elem = elem->next)
			if(elem->next == &(pDcst->keys[key]))
				elem->next = pDcst->keys[key].next;
	}

	/* Remove the element from the key table */
	pDcst->keys[key].key = dcst_INVALID_KEY;
	memset(pDcst->keys[key].value, 0, pDcst->valueSize);
	pDcst->keys[key].next = NULL;

	/* Check postconditions */
	dcst_ASSERTVALID(pDcst);									/* set rendered invalid */
	assert(dcst_INVALID_KEY == pDcst->keys[key].key);	/* key still in use */

	return dcst_STATUS_OK;
}

/*****************************************************************************
Find the key associated with the given value in the dictionary set.
It is an error to call this method with a NULL pDcst or a NULL value.
Return the key, or dcst_INVALID_KEY if the value is not in the set.
*****************************************************************************/
dcst_key_t				/* key associated with value */
dcstFindKey(
	dcst_t pDcst,		/* set to examine */
	dcst_value_t value)	/* value to search for */
{
	int hash;

	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);					/* invalid set */
	assert(value != NULL);				/* looking for nothing */

	/* Search the hash table */
	hash = dcstHashvalue(pDcst, value);
	if(NULL == pDcst->htab[hash])
		return dcst_INVALID_KEY;
	else if(0 == memcmp(pDcst->htab[hash]->value, value, pDcst->valueSize))
		return pDcst->htab[hash]->key;
	else {
		dcst_element_t* elem;
		for(elem = pDcst->htab[hash]; elem != NULL; elem = elem->next)
			if(0 == memcmp(elem->value, value, pDcst->valueSize))
				return elem->key;
	}

	/* Not found */
	return dcst_INVALID_KEY;
}

/*****************************************************************************
Find the value associated with the given key in the dictionary set
It is an error to call this method with a NULL pDcst or invalid key.
Returns the value, or NULL if the key is not in use.
*****************************************************************************/
dcst_value_t		/* value associated with key */
dcstFindValue(
	dcst_t pDcst,	/* set to examine */
	dcst_key_t key)	/* key to search for */
{
	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);								/* invalid set */
	assert(dcst_INVALID_KEY != key);				/* key not valid */
	assert((key >= 0) && (key < pDcst->maxKeys));	/* key out of range */

	/* Return the value, or NULL if the key is not in use */
	if(dcst_INVALID_KEY == pDcst->keys[key].key)
		return NULL;
	return pDcst->keys[key].value;
}

/*****************************************************************************
 Call a method on each member of the set
 cb will be called once for each member, with parameters set as follows:
		pDcst - The table being handled
		key - The key of the current element
		value - The value of the current element
		context - The context passed in to dcstForEach()
   The cb method should not depend on receiving the elements in any
   particular order, as none is guaranteed.  The cb must return 0 to continue
   to next element, nonzero to end the for-each call immediately.
*****************************************************************************/
void
dcstForEach(
	dcst_t pDcst,		/* set to operate on */
	dcst_forEach_cb cb,	/* method to call on each element */
	void* context)		/* data for callback method */
{
	int i;

	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);		/* invalid set */
	assert(cb != NULL);		/* invalid callback method */

	/* Iterate and call */
	for(i = 0; i < pDcst->maxKeys; i++) {
		if(dcst_INVALID_KEY != pDcst->keys[i].key)
			if((*cb)(pDcst, pDcst->keys[i].key, pDcst->keys[i].value, context))
				break;
	}

	/* Check postconditions */
	dcst_ASSERTVALID(pDcst);					/* set rendered invalid */
}

/*****************************************************************************
 Save a dictionary set to a stream
*****************************************************************************/
dcst_status_t		/* completion status */
dcstFreeze(
	dcst_t pDcst,	/* set to store */
	FILE *fp)		/* stream to store set to */
{
	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);			/* invalid set */
	assert(fp != NULL);			/* invalid stream */

	(void) pDcst;
	(void) fp;

	/* Check postconditions */
	/*dcst_ASSERTVALID(pDcst);			/* set rendered invalid */

	return dcst_STATUS_DEFERRED;
}

/*****************************************************************************
 Restore a dictionary set from a stream
*****************************************************************************/
dcst_status_t		/* completion status */
dcstThaw(
	dcst_t pDcst,	/* set to restore into */
	FILE *fp)		/* stream to restore from */
{
	/* Check preconditions */
	assert(NULL != pDcst);				/* must supply a set */
	assert(NULL != fp);					/* must supply a stream */

	(void) pDcst;
	(void) fp;

	/* Check postconditions */
	/*dcst_ASSERTVALID(pDcst);					/* set restored to invalid state */

	return dcst_STATUS_DEFERRED;
}

/**
* Debugging support
*/
#if defined(DCST_ASSERTALL)

#include <stdlib.h>

/*****************************************************************************
 Assert that the dictset has a consistent internal structure
*****************************************************************************/
void
dcstAssertValid(
	dcst_t pDcst,	/* set to check */
	char* file,		/* filename for error reporting */
	int line)		/* line number for error reporting */
{
	int i, j;

	/* Protect against a NULL file */
	if(NULL == file)
		file = "an unknown file";

	/* Check for NULL dictset */
	if(NULL == pDcst) {
		fprintf(stderr, "Assertion failed in %s, line %d: NULL dcst_t*\n", file, line);
		exit(-1);
	}

	for(i = 0; i < pDcst->maxKeys; i++) {
		if(dcst_INVALID_KEY == pDcst->keys[i].key) {
			/* Verify that unused keys have no values */
			/* compare against all-zero?  Naah, that's a legal value too. */
			/* Verify that unused keys don't keep hashtable links */
			if(NULL != pDcst->keys[i].next) {
				fprintf(stderr, "Assertion failed in %s, line %d: next not null when key is unused.\n", file, line);
				exit(-1);
			}
		} else {
			/* Verify that elements in use store the correct key */
			if(i != pDcst->keys[i].key) {
				fprintf(stderr, "Assertion failed in %s, line %d: key mismatch.\n", file, line);
				exit(-1);
			}
			/* Verify that in-use keys have values */
			/* compare against all-zero?  Naah, that's a legal value too. */
			/* Verify that elements in same hashtable slot have same hash */
			if(NULL != pDcst->keys[i].next) {
				if(dcstHashvalue(pDcst, pDcst->keys[i].value) !=
							dcstHashvalue(pDcst, pDcst->keys[i].next->value)) {
					fprintf(stderr, "Assertion failed in %s, line %d: hashing error.\n", file, line);
					exit(-1);
				}
			}
		}
		/* Check that no two values share the same key */
		for(j = i+1; j < pDcst->maxKeys; j++) {
			if((dcst_INVALID_KEY != pDcst->keys[i].key) && (dcst_INVALID_KEY != pDcst->keys[j].key) &&
					(0 == memcmp(pDcst->keys[i].value, pDcst->keys[j].value, pDcst->valueSize))) {
				fprintf(stderr, "Assertion failed in %s, line %d: two keys with same value.\n", file, line);
				exit(-1);
			}
		}
	}

	/* Check that all elements are stored in the right hash bin */
	for(i = 0; i < pDcst->maxHash; i++) {
		if(NULL != pDcst->htab[i]) {
			if(i != dcstHashvalue(pDcst, pDcst->htab[i]->value)) {
				fprintf(stderr, "Assertion failed in %s, line %d: hashtable error.\n", file, line);
				exit(-1);
			}
		}
	}
}

#endif /* debugging support */

/**
* Self-test support
*/
#if DCST_SELFTEST

/*****************************************************************************
*****************************************************************************/
int						/* continue? nonzero=yes, 0=no */
forEachTestCb (
	dcst_t pDcst,		/* Dictset being operated on */
	dcst_key_t key,		/* key of current element */
	dcst_value_t value,	/* value of current element */
	void *context)		/* context passed to dcstForEach() */
{
	/* Check preconditions */
	dcst_ASSERTVALID(pDcst);

	/* Verify that we're getting these things */
	if(*((int*)context) < 0) {
		*((int*)context) -= 1;
		return 1;	/* Abort */
	} else
		*((int*)context) += (pDcst->valueSize+1);

	/* Compare the keys to the values, for the fun of it */
	if(key == *((dcst_key_t *) value))
		printf("What a coincidince!\n");
	else
		printf("No match, all is well...\n");

	/* Check postconditions */
	dcst_ASSERTVALID(pDcst);					/* set rendered invalid */

	return 0;	/* Continue */
}

/*****************************************************************************
*****************************************************************************/
int
main(
	int argc,
	char** argv)
{
	#define VALUESIZE	7
	#define NUMKEYS		350
	#define HASHSIZE	143

	dcst_t pDcst = NULL;
	char value1[VALUESIZE] = {'1','2','3','4','5','6','\0'};
	char value2[VALUESIZE] = {'2','3','4','5','6','7','\0'};
	char value3[VALUESIZE] = {'\1','4','5','6','\6','8','\0'};
	char value4[VALUESIZE] = {'\6','4','5','6','\1','8','\0'};
	char value5[VALUESIZE] = {'\3','4','5','6','\4','8','\0'};
	char value6[VALUESIZE] = {'\4','4','5','6','\3','8','\0'};
	char dup1[VALUESIZE] = {'1','2','3','4','5','6','\0'};
	char dummy[] = {"Separate dup1 and dup2 for hash test."};
	char dup2[VALUESIZE] = {'2','3','4','5','6','7','\0'};
	dcst_key_t key1 = dcst_INVALID_KEY;
	dcst_key_t key2 = dcst_INVALID_KEY;
	dcst_key_t key3 = dcst_INVALID_KEY;
	dcst_key_t key4 = dcst_INVALID_KEY;
	dcst_key_t key5 = dcst_INVALID_KEY;
	dcst_key_t key6 = dcst_INVALID_KEY;
	dcst_key_t tmpKey = dcst_INVALID_KEY;
	dcst_value_t tmpVal = NULL;
	int context;
	int i;

	/* Check command-line parameters */
	if(argc > 1) {
		fprintf(stderr, "Usage: %1\n", argv[0]);
		exit(1);
	}

	/* Test dcstCreate() */
	/* Memory failure test not yet usable (waiting for dp_MEMFAIL)
	for(i = 1; i < 4; i++) {
		dp_MEMFAIL(i);
		pDcst = dcstCreate(6);
		if(pDcst != NULL) {
			fprintf(stderr, "dcstCreate(): memory failure incorrect.\n");
			exit(-1);
		}
	}
	dp_MEMFAIL(0); */
	pDcst = dcstCreate(VALUESIZE, NUMKEYS, HASHSIZE);
	if(pDcst == NULL) {
		fprintf(stderr, "dcstCreate(): failed.\n");
		exit(-1);
	}

	/* Test dcstFind*() on an empty set */
	if(dcst_INVALID_KEY != dcstFindKey(pDcst, value1)) {
		fprintf(stderr, "dcstFindKey(): found when nothing present.\n");
		exit(-1);
	}
	if(NULL != dcstFindValue(pDcst, 2)) {
		fprintf(stderr, "dcstFindValue(): found when nothing present.\n");
		exit(-1);
	}

	/* Test dcstForEach() on an empty set */
	context = 0;
	dcstForEach(pDcst, forEachTestCb, &context);
	if(0 != context) {
		fprintf(stderr, "dcstForEach(): Cb called too many times.\n");
		exit(-1);
	}

	/* Test dcstFreeze() on an empty set */
	/* not implemented */

	/* Test dcstThaw() on an empty set */
	/* not implemented */

	/* Test dcstAdd() on an empty set */
	key1 = dcstAdd(pDcst, value1);
	if(dcst_INVALID_KEY == key1) {
		fprintf(stderr, "dcstAdd(): unable to add first value.\n");
		exit(-1);
	}
	tmpKey = dcstAdd(pDcst, dup1);
	if(dcst_INVALID_KEY == tmpKey) {
		fprintf(stderr, "dcstAdd(): unable to add duplicate first value.\n");
		exit(-1);
	}
	if(key1 != tmpKey) {
		fprintf(stderr, "dcstAdd(): duplicate values have different keys.\n");
		exit(-1);
	}

	/* Test dcstFind*() on a set with one element */
	if(key1 != dcstFindKey(pDcst, value1)) {
		fprintf(stderr, "dcstFindKey(): didn't find first item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key1);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value1))) {
		fprintf(stderr, "dcstFindValue(): didn't find first item.\n");
		exit(-1);
	}

	/* Test dcstAdd() on a set with one element */
	key2 = dcstAdd(pDcst, value2);
	if(dcst_INVALID_KEY == key2) {
		fprintf(stderr, "dcstAdd(): unable to add second value.\n");
		exit(-1);
	}
	tmpKey = dcstAdd(pDcst, dup2);
	if(dcst_INVALID_KEY == tmpKey) {
		fprintf(stderr, "dcstAdd(): unable to add duplicate second value.\n");
		exit(-1);
	}
	if(key2 != tmpKey) {
		fprintf(stderr, "dcstAdd(): duplicate 2nd values have different keys.\n");
		exit(-1);
	}

	/* Test dcstHashvalue() */
	if(dcstHashvalue(pDcst, value1) != dcstHashvalue(pDcst, dup1)) {
		fprintf(stderr, "dcstHashvalue(): failed to collide 1-d1.\n");
		exit(-1);
	}
	if(dcstHashvalue(pDcst, value2) != dcstHashvalue(pDcst, dup2)) {
		fprintf(stderr, "dcstHashvalue(): failed to collide 2-d2.\n");
		exit(-1);
	}
	if(dcstHashvalue(pDcst, value1) == dcstHashvalue(pDcst, value2)) {
		fprintf(stderr, "dcstHashvalue(): unexpected collision 1-2.\n");
		exit(-1);
	}
	if(dcstHashvalue(pDcst, value2) == dcstHashvalue(pDcst, value3)) {
		fprintf(stderr, "dcstHashvalue(): unexpected collision 2-3.\n");
		exit(-1);
	}
	if(dcstHashvalue(pDcst, value3) != dcstHashvalue(pDcst, value4)) {
		fprintf(stderr, "dcstHashvalue(): failed to collide 3-4.\n");
		exit(-1);
	}
	if(dcstHashvalue(pDcst, value4) != dcstHashvalue(pDcst, value5)) {
		fprintf(stderr, "dcstHashvalue(): failed to collide 4-5.\n");
		exit(-1);
	}
	if(dcstHashvalue(pDcst, value5) != dcstHashvalue(pDcst, value6)) {
		fprintf(stderr, "dcstHashvalue(): failed to collide 5-6.\n");
		exit(-1);
	}

	/* Test dcstAdd() to create a hashing collision */
	key3 = dcstAdd(pDcst, value3);
	if(dcst_INVALID_KEY == key3) {
		fprintf(stderr, "dcstAdd(): unable to add third value.\n");
		exit(-1);
	}
	key4 = dcstAdd(pDcst, value4);
	if(dcst_INVALID_KEY == key4) {
		fprintf(stderr, "dcstAdd(): unable to add fourth value.\n");
		exit(-1);
	}
	key5 = dcstAdd(pDcst, value5);
	if(dcst_INVALID_KEY == key5) {
		fprintf(stderr, "dcstAdd(): unable to add fifth value.\n");
		exit(-1);
	}
	key6 = dcstAdd(pDcst, value6);
	if(dcst_INVALID_KEY == key6) {
		fprintf(stderr, "dcstAdd(): unable to add sixth value.\n");
		exit(-1);
	}

	/* Test dcstFind*() on a set with several elements */
	if(key1 != dcstFindKey(pDcst, value1)) {
		fprintf(stderr, "dcstFindKey(): didn't find first item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key1);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value1))) {
		fprintf(stderr, "dcstFindValue(): didn't find first item.\n");
		exit(-1);
	}
	if(key2 != dcstFindKey(pDcst, value2)) {
		fprintf(stderr, "dcstFindKey(): didn't find second item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key2);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value2))) {
		fprintf(stderr, "dcstFindValue(): didn't find second item.\n");
		exit(-1);
	}
	if(key3 != dcstFindKey(pDcst, value3)) {
		fprintf(stderr, "dcstFindKey(): didn't find third item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key3);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value3))) {
		fprintf(stderr, "dcstFindValue(): didn't find third item.\n");
		exit(-1);
	}
	if(key4 != dcstFindKey(pDcst, value4)) {
		fprintf(stderr, "dcstFindKey(): didn't find fourth item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key4);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value4))) {
		fprintf(stderr, "dcstFindValue(): didn't find fourth item.\n");
		exit(-1);
	}
	if(key5 != dcstFindKey(pDcst, value5)) {
		fprintf(stderr, "dcstFindKey(): didn't find fifth item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key5);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value5))) {
		fprintf(stderr, "dcstFindValue(): didn't find fifth item.\n");
		exit(-1);
	}
	if(key6 != dcstFindKey(pDcst, value6)) {
		fprintf(stderr, "dcstFindKey(): didn't find sixth item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key6);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value6))) {
		fprintf(stderr, "dcstFindValue(): didn't find sixth item.\n");
		exit(-1);
	}

	/* Test dcstDeleteValue() down to empty */
	if(dcst_STATUS_OK != dcstDeleteValue(pDcst, value1)) {
		fprintf(stderr, "dcstDeleteValue(): didn't delete first item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteValue(pDcst, value2)) {
		fprintf(stderr, "dcstDeleteValue(): didn't delete second item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteValue(pDcst, value3)) {
		fprintf(stderr, "dcstDeleteValue(): didn't delete third item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteValue(pDcst, value4)) {
		fprintf(stderr, "dcstDeleteValue(): didn't delete fourth item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteValue(pDcst, value5)) {
		fprintf(stderr, "dcstDeleteValue(): didn't delete fifth item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteValue(pDcst, value6)) {
		fprintf(stderr, "dcstDeleteValue(): didn't delete sixth item.\n");
		exit(-1);
	}

	/* Test dcstFind*() on an empty set (verify emptiness) */
	if(dcst_INVALID_KEY != dcstFindKey(pDcst, value2)) {
		fprintf(stderr, "dcstFindKey()/2: found when nothing present.\n");
		exit(-1);
	}
	if(NULL != dcstFindValue(pDcst, key2)) {
		fprintf(stderr, "dcstFindValue()/2: found when nothing present.\n");
		exit(-1);
	}

	/* Use dcstAdd() to create another hashing collision */
	key6 = dcstAdd(pDcst, value6);
	if(dcst_INVALID_KEY == key6) {
		fprintf(stderr, "dcstAdd(): unable to add sixth value.\n");
		exit(-1);
	}
	key5 = dcstAdd(pDcst, value5);
	if(dcst_INVALID_KEY == key5) {
		fprintf(stderr, "dcstAdd(): unable to add fifth value.\n");
		exit(-1);
	}
	key4 = dcstAdd(pDcst, value1);
	if(dcst_INVALID_KEY == key4) {
		fprintf(stderr, "dcstAdd(): unable to add fourth value.\n");
		exit(-1);
	}

	/* Test dcstReplace() */
	key4 = dcstReplace(pDcst, key4, value4);
	if(dcst_INVALID_KEY == key4) {
		fprintf(stderr, "dcstReplace(): unable to replace fourth value.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key4);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value4))) {
		fprintf(stderr, "dcstFindValue(): didn't replace fourth item.\n");
		exit(-1);
	}
	key3 = dcstReplace(pDcst, dcst_INVALID_KEY, value3);
	if(dcst_INVALID_KEY == key3) {
		fprintf(stderr, "dcstReplace(): unable to add third value.\n");
		exit(-1);
	}

	/* Test dcstForEach() on a set with several elements (verify re-fill) */
	context = 0;
	dcstForEach(pDcst, forEachTestCb, &context);	/* Continue case */
	if(4*(VALUESIZE+1) != context) {
		fprintf(stderr, "dcstForEach(): Cb called incorrect number of times.\n");
		exit(-1);
	}
	context = -1;
	dcstForEach(pDcst, forEachTestCb, &context);	/* Abort case */
	if(-2 != context) {
		fprintf(stderr, "dcstForEach(): Cb called improperly.\n");
		exit(-1);
	}

	/* Test dcstFind*() on a set with several elements (verify re-fill) */
	if(dcst_INVALID_KEY != dcstFindKey(pDcst, value1)) {
		fprintf(stderr, "dcstFindKey(): incorrectly found first item.\n");
		exit(-1);
	}
	if(dcst_INVALID_KEY != dcstFindKey(pDcst, value2)) {
		fprintf(stderr, "dcstFindKey(): incorrectly found second item.\n");
		exit(-1);
	}
	if(key3 != dcstFindKey(pDcst, value3)) {
		fprintf(stderr, "dcstFindKey(): didn't find third item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key3);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value3))) {
		fprintf(stderr, "dcstFindValue(): didn't find third item.\n");
		exit(-1);
	}
	if(key4 != dcstFindKey(pDcst, value4)) {
		fprintf(stderr, "dcstFindKey(): didn't find fourth item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key4);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value4))) {
		fprintf(stderr, "dcstFindValue(): didn't find fourth item.\n");
		exit(-1);
	}
	if(key5 != dcstFindKey(pDcst, value5)) {
		fprintf(stderr, "dcstFindKey(): didn't find fifth item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key5);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value5))) {
		fprintf(stderr, "dcstFindValue(): didn't find fifth item.\n");
		exit(-1);
	}
	if(key6 != dcstFindKey(pDcst, value6)) {
		fprintf(stderr, "dcstFindKey(): didn't find sixth item.\n");
		exit(-1);
	}
	tmpVal = dcstFindValue(pDcst, key6);
	if((NULL == tmpVal) || (0 != strcmp(tmpVal, value6))) {
		fprintf(stderr, "dcstFindValue(): didn't find sixth item.\n");
		exit(-1);
	}

	/* Test dcstFreeze() */
	/* not implemented */

	/* Test dcstThaw() */
	/* not implemented */

	/* Test dcstDeleteKey() down to empty */
	if(dcst_STATUS_OK != dcstDeleteKey(pDcst, key3)) {
		fprintf(stderr, "dcstDeleteKey(): didn't delete third item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteKey(pDcst, key4)) {
		fprintf(stderr, "dcstDeleteKey(): didn't delete fourth item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteKey(pDcst, key5)) {
		fprintf(stderr, "dcstDeleteKey(): didn't delete fifth item.\n");
		exit(-1);
	}
	if(dcst_STATUS_OK != dcstDeleteKey(pDcst, key6)) {
		fprintf(stderr, "dcstDeleteKey(): didn't delete sixth item.\n");
		exit(-1);
	}

	/* Test dcstFind*() on an empty set (verify emptiness) */
	if(dcst_INVALID_KEY != dcstFindKey(pDcst, value3)) {
		fprintf(stderr, "dcstFindKey()/3: found when nothing present.\n");
		exit(-1);
	}
	if(NULL != dcstFindValue(pDcst, key3)) {
		fprintf(stderr, "dcstFindValue()/3: found when nothing present.\n");
		exit(-1);
	}

	/* Exhaust the key space and attempt to add another */
	for(i = 0; i < pDcst->maxKeys; i++) {
		tmpVal = (dcst_value_t) dp_MALLOC(VALUESIZE);
		sprintf((char*)tmpVal, "%x", i);
		if(dcst_INVALID_KEY == dcstAdd(pDcst, tmpVal)) {
			fprintf(stderr, "dcstAdd()/e: can't add item %d.\n",i);
			exit(-1);
		}
	}
	if(dcst_INVALID_KEY != dcstAdd(pDcst, value6)) {
		fprintf(stderr, "dcstAdd(): able to add too many times.\n");
		exit(-1);
	}

	/* Test dcstDestroy() */
	dcstDestroy(pDcst);

	fprintf(stderr, "dictset test passed.\n");
	return 0;
}

#endif /* self-test support */
