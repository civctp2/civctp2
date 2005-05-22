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
  A simple dynamically sized table, which writes to disk in an encrypted
  format.
-----------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "crypttab.h"

#define DPRINT(a)
#define crypttab_RW_BUFFER_SIZE 1024

typedef struct {
	int magic;
	int n_used;
	size_t unit;
} dynatab_encrypted_freeze_t;
#define dynatab_encrypted_MAGIC 0x9474

/*-------------------------------------------------------------------------
  Write the dynatab to the given file in a format encrypted with key,
  suitable for restoration with dynatab_thaw_encrypted.
-------------------------------------------------------------------------*/
void dynatab_freeze_encrypted(dynatab_t *tab, FILE *fp, const unsigned char key[8])
{
	dynatab_encrypted_freeze_t d;
	size_t encrypted_unit;
	int nwritten;
	int n;
	size_t chunk;
	char buf[crypttab_RW_BUFFER_SIZE];
	char *pbuf;
	char unitbuf[crypttab_RW_BUFFER_SIZE];
	
	d.magic = dynatab_encrypted_MAGIC;
	d.n_used = tab->n_used;
	d.unit = tab->unit;
	/* round up to nearest multiple of 8 */
	encrypted_unit = d.unit + 7 - (d.unit + 7) % 8;

	assert(encrypted_unit <= crypttab_RW_BUFFER_SIZE);
	assert(d.n_used <= 5000);
	DPRINT(("dynatab_freeze_encrypted: Saving %d elements of size %d (%d encrypted).\n", d.n_used, tab->unit, encrypted_unit));

	nwritten = fwrite(&d, sizeof(d), 1, fp);
	if (nwritten != 1) {
		DPRINT(("dynatab_freeze_encrypted: Error writing info.\n"));
		return;
	}
	
	pbuf = buf;
	memset(unitbuf, 0, encrypted_unit);
	deskey(key, EN0);
	for (n = 0; n < tab->n_used; n++) {
		if (pbuf - buf + encrypted_unit > crypttab_RW_BUFFER_SIZE) {
			/* if the write buffer is full, write it to disk, then start
			 * adding at the beginning of the buffer again
			 */
			nwritten = fwrite(buf, (pbuf - buf), 1, fp);
			if (nwritten != 1) {
				DPRINT(("dynatab_freeze_encrypted: Error writing unit %d (wrote %d x %d).\n", n, (pbuf - buf), nwritten));
				return;  /* need an error code? */
			}
			pbuf = buf;
		}
		/* encrypt the entry and add it to the write buffer in 8 byte chunks */
		memcpy(unitbuf, (char *)tab->buf + n * tab->unit, tab->unit);
		for (chunk = 0; chunk < encrypted_unit; chunk += 8, pbuf += 8)
			des(unitbuf + chunk, pbuf);
	}
	
	/* Write the last partial buffer to disk */
	nwritten = fwrite(buf, (pbuf - buf), 1, fp);
	if (nwritten != 1) {
		DPRINT(("dynatab_freeze_encrypted: Error writing unit %d.\n", n));
		return;  /* need an error code? */
	}
}

/*-------------------------------------------------------------------------
  Read the dynatab (encrypted with key) from the given file.
  Replaces the previous contents of the dynatab.
  Returns a pointer to the first element on success,
          NULL on failure.
  Assumes that the items contain no pointers or other data that needs to
  be modified to reflect new conditions upon thawing.
-------------------------------------------------------------------------*/
void *dynatab_thaw_encrypted(dynatab_t *tab, FILE *fp, const unsigned char key[8])
{
	int nread;
	void *p;
	dynatab_encrypted_freeze_t d;
	size_t encrypted_unit;

	nread = fread(&d, sizeof(d), 1, fp);
	if (nread != 1 || d.magic != dynatab_encrypted_MAGIC) {
		/*  Read failed, or bad format */
		DPRINT(("dynatab_thaw_encrypted: Could not read table info.\n"));
		return NULL;
	}
	if (d.magic != dynatab_encrypted_MAGIC) {
		/*  Read failed, or bad format */
		DPRINT(("dynatab_thaw_encrypted: signature incorrect: %d != %d\n", d.magic, dynatab_encrypted_MAGIC));
		return NULL;
	}

	/* round up to nearest multiple of 8 */
	encrypted_unit = d.unit + 7 - (d.unit + 7) % 8;
	assert(encrypted_unit <= crypttab_RW_BUFFER_SIZE);
	
	DPRINT(("dynatab_thaw_encrypted: Reading %d elements of size %d (%d encrypted).\n", d.n_used, d.unit, encrypted_unit));
	tab->unit = d.unit;
	if (d.n_used != 0) {
		int n;
		int n_end;
		size_t chunk;
		char buf[crypttab_RW_BUFFER_SIZE];
		char *pbuf;
		char unitbuf[crypttab_RW_BUFFER_SIZE];
		
		p = dynatab_subscript_grow(tab, d.n_used-1);
		if (!p) {
			DPRINT(("dynatab_thaw_encrypted: Could not allocate %d elements\n", d.n_used));
			return NULL;
		}
		n = 0;
		memset(unitbuf, 0, encrypted_unit);
		deskey(key, DE1);
		while (n < d.n_used) { 
			n_end = n + crypttab_RW_BUFFER_SIZE / encrypted_unit;
			if (n_end > d.n_used)
				n_end = d.n_used;
			nread = fread(buf, encrypted_unit, n_end - n, fp);
			if (nread != n_end - n) {
				/*  Read failed */
				DPRINT(("dynatab_thaw_encrypted: Could not read element %d\n", n));
				return NULL;
			}
			for (pbuf = buf; n < n_end; n++, pbuf += encrypted_unit) {
				for (chunk = 0; chunk <= encrypted_unit; chunk += 8)
					des(pbuf + chunk, unitbuf + chunk);
				memcpy((char *)tab->buf + n * tab->unit, unitbuf, tab->unit);
			}
		}
	} else {
		/*  Kludge: force non-NULL buffer */
		/*  Should just change return convention */
		p = dynatab_subscript_grow(tab, 1);
	}
	tab->n_used = d.n_used;
	DPRINT(("dynatab_thaw_encrypted: successful.\n"));
	return (void *) tab->buf;	
}


#if defined(crypttab_UNITTEST)

#include <stdio.h>
#define NENTRIES 300

void test1(dynatab_t *pt)
{
	int i;
	FILE *fp;
	dynatab_t *th;

	for (i = NENTRIES-1; i >= 0; i--) {
	/* for (i = 0; i < NENTRIES; i++) { */
		int *p = dynatab_subscript_grow(pt, i);
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
	for (i = 0; i < NENTRIES; i++) {
		int *p = dynatab_subscript(pt, i);
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
	fp = fopen("spam.dat", "wb");
	if (!fp) {
		perror("spam.dat");
		exit(1);
	}
	dynatab_freeze_encrypted(pt, fp, "Password");
	if (fclose(fp)) {
		perror("closing spam.dat");
		exit(1);
	}
	th = dynatab_create(sizeof(int));
	if (!th) exit(1);

	fp = fopen("spam.dat", "rb");
	if (!fp) {
		perror("spam.dat");
		exit(1);
	}
	if (!dynatab_thaw_encrypted(th, fp, "Password")) {
		perror("thaw_encrypted");
		exit(1);
	}
	fclose(fp);
	if ((th->n_used != pt->n_used) || (th->unit != pt->unit)) {
		printf("bad n_used or unit: test failed\n");
		exit(1);
	}
	for (i = 0; i < NENTRIES; i++) {
		int *p = dynatab_subscript(th, i);
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
	for (i = 0; i < NENTRIES/2; i++) {	/* Don't remove them all */
		int *p = dynatab_subscript(th, 0);
		int val = *p;
		if (p == NULL) {
			printf("subscript3: test failed\n");
			exit(1);
		}
		dynatab_remove(th, 0);
		if (val == *((int*)dynatab_subscript(th, 0))) {
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

#include <malloc.h>		/* for _heapchk() */

#define NTABS 100
main()
{
	int i;
	dynatab_t *pt[NTABS];

	for (i=0;i<NTABS;i++) {
		pt[i] = dynatab_create(sizeof(int));
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
		int h;
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

#endif /* crypttab_UNITTEST */
