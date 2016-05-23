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
#include "anet.h"
#include "tcapwtab.h"

#define tcapwtab_RW_BUFFER_SIZE 1024

#define tcapwtab_MAGIC 0x9475
#define tcapwtab_LEN_HEADER 20
typedef struct {
	long timestamp;
	long offset;
	short magic;
	long n_used;
	size_t unit;
	dp_uid_t lastuid;
} tcapwtab_header_t;

/*-------------------------------------------------------------------------
 Write the tcapwtab header to file.  Read with tcapwtab_thaw_header.
 Returns 0 on success.
-------------------------------------------------------------------------*/
static int tcapwtab_freeze_header(tcapwtab_header_t *head, FILE *fp)
{
	char buf[tcapwtab_LEN_HEADER];
	char *pbuf = buf;

	assert(head->unit < 65536);
	*pbuf++ = dpGETLONG_FIRSTBYTE(head->timestamp);
	*pbuf++ = dpGETLONG_SECONDBYTE(head->timestamp);
	*pbuf++ = dpGETLONG_THIRDBYTE(head->timestamp);
	*pbuf++ = dpGETLONG_FOURTHBYTE(head->timestamp);
	*pbuf++ = dpGETLONG_FIRSTBYTE(head->offset);
	*pbuf++ = dpGETLONG_SECONDBYTE(head->offset);
	*pbuf++ = dpGETLONG_THIRDBYTE(head->offset);
	*pbuf++ = dpGETLONG_FOURTHBYTE(head->offset);
	*pbuf++ = dpGETSHORT_FIRSTBYTE(head->magic);
	*pbuf++ = dpGETSHORT_SECONDBYTE(head->magic);
	*pbuf++ = dpGETLONG_FIRSTBYTE(head->n_used);
	*pbuf++ = dpGETLONG_SECONDBYTE(head->n_used);
	*pbuf++ = dpGETLONG_THIRDBYTE(head->n_used);
	*pbuf++ = dpGETLONG_FOURTHBYTE(head->n_used);
	*pbuf++ = dpGETSHORT_FIRSTBYTE((unsigned short)head->unit);
	*pbuf++ = dpGETSHORT_SECONDBYTE((unsigned short)head->unit);
	*pbuf++ = dpGETLONG_FIRSTBYTE(head->lastuid);
	*pbuf++ = dpGETLONG_SECONDBYTE(head->lastuid);
	*pbuf++ = dpGETLONG_THIRDBYTE(head->lastuid);
	*pbuf++ = dpGETLONG_FOURTHBYTE(head->lastuid);
	assert((pbuf - buf) == tcapwtab_LEN_HEADER);
	if (fwrite(buf, (pbuf-buf), 1, fp) != 1) {
		DPRINT(("tcapwtab_freeze_encrypted: write error\n"));
		return -1;
	}
	return 0;
}

/*-------------------------------------------------------------------------
 Read the tcapwtab header from a file.
 Returns 0 on success.
-------------------------------------------------------------------------*/
static int tcapwtab_thaw_header(tcapwtab_header_t *head, FILE *fp)
{
	char buf[tcapwtab_LEN_HEADER];
	char *pbuf = buf;

	if (fread(buf, tcapwtab_LEN_HEADER, 1, fp) != 1) {
		DPRINT(("tcapwtab_thaw_encrypted: error reading header\n"));
		return -1;
	}
	head->timestamp = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
	pbuf += 4;
	head->offset = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
	pbuf += 4;
	head->magic = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += 2;
	head->n_used = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
	pbuf += 4;
	head->unit = dpMAKESHORT(pbuf[0], pbuf[1]);
	pbuf += 2;
	head->lastuid = dpMAKELONG(pbuf[0], pbuf[1], pbuf[2], pbuf[3]);
	pbuf += 4;
	assert((pbuf - buf) == tcapwtab_LEN_HEADER);
	return 0;
}

/*-------------------------------------------------------------------------
 Write the dynatab to the given file in a format encrypted with key,
 suitable for restoration with tcapwtab_thaw_encrypted.
 Saves the wmq position (timestamp, offset) and lastuid in the header.
-------------------------------------------------------------------------*/
void tcapwtab_freeze_encrypted(dynatab_t *tab, FILE *fp, const unsigned char key[8], time_t timestamp, long offset, dp_uid_t lastuid)
{
	tcapwtab_header_t d;
	size_t encrypted_unit;
	int nwritten;
	int n;
	size_t chunk;
	char buf[tcapwtab_RW_BUFFER_SIZE];
	char *pbuf;
	char unitbuf[tcapwtab_RW_BUFFER_SIZE];

	d.magic = (short)tcapwtab_MAGIC;
	d.n_used = tab->n_used;
	d.unit = tab->unit;
	d.timestamp = timestamp;
	d.offset = offset;
	d.lastuid = lastuid;
	/* round up to nearest multiple of 8 */
	encrypted_unit = d.unit + 7 - (d.unit + 7) % 8;

	assert(encrypted_unit <= tcapwtab_RW_BUFFER_SIZE);
	DPRINT(("tcapwtab_freeze_encrypted: Saving %d elements of size %d (%d encrypted) with timestamp:%8x.\n", d.n_used, d.unit, encrypted_unit, d.timestamp));

	if (tcapwtab_freeze_header(&d, fp) != 0) {
		DPRINT(("tcapwtab_freeze_encrypted: Error writing header.\n"));
		return;
	}

	pbuf = buf;
	memset(unitbuf, 0, encrypted_unit);
	deskey(key, EN0);
	for (n = 0; n < tab->n_used; n++) {
		if (pbuf - buf + encrypted_unit > tcapwtab_RW_BUFFER_SIZE) {
			/* if the write buffer is full, write it to disk, then start
			 * adding at the beginning of the buffer again
			 */
			nwritten = fwrite(buf, (pbuf - buf), 1, fp);
			if (nwritten != 1) {
				DPRINT(("tcapwtab_freeze_encrypted: Error writing unit %d (wrote %d x %d).\n", n, (pbuf - buf), nwritten));
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
		DPRINT(("tcapwtab_freeze_encrypted: Error writing unit %d.\n", n));
		return;  /* need an error code? */
	}
}

/*-------------------------------------------------------------------------
 Read the dynatab (encrypted with key) from the given file.
 Replaces the previous contents of the dynatab.
 Returns a pointer to the first element on success,
         NULL on failure.
 On success, returns the wmq position and lastuid set at freeze in
 *timestamp, *offset, and *lastuid.
 Assumes that the items contain no pointers or other data that needs to
 be modified to reflect new conditions upon thawing.
-------------------------------------------------------------------------*/
void *tcapwtab_thaw_encrypted(dynatab_t *tab, FILE *fp, const unsigned char key[8], time_t *timestamp, long *offset, dp_uid_t *lastuid)
{
	int nread;
	void *p;
	tcapwtab_header_t d;
	size_t encrypted_unit;

	if ((tab == NULL) || (fp == NULL) || (timestamp == NULL) || (offset == NULL)) {
		DPRINT(("tcapwtab_thaw_encrypted: Null pointer in params\n"));
		return NULL;
	}
	if (tcapwtab_thaw_header(&d, fp) != 0) {
		DPRINT(("tcapwtab_thaw_encrypted: Could not read header.\n"));
		return NULL;
	}

	/* round up to nearest multiple of 8 */
	encrypted_unit = d.unit + 7 - (d.unit + 7) % 8;
	assert(encrypted_unit <= tcapwtab_RW_BUFFER_SIZE);

	DPRINT(("tcapwtab_thaw_encrypted: Reading %d elements of size %d (%d encrypted).\n", d.n_used, d.unit, encrypted_unit));
	tab->unit = d.unit;
	if (d.n_used != 0) {
		int n;
		int n_end;
		size_t chunk;
		char buf[tcapwtab_RW_BUFFER_SIZE];
		char *pbuf;
		char unitbuf[tcapwtab_RW_BUFFER_SIZE];

		p = dynatab_subscript_grow(tab, d.n_used-1);
		if (!p) {
			DPRINT(("tcapwtab_thaw_encrypted: Could not allocate %d elements\n", d.n_used));
			return NULL;
		}
		n = 0;
		memset(unitbuf, 0, encrypted_unit);
		deskey(key, DE1);
		while (n < d.n_used) {
			n_end = n + tcapwtab_RW_BUFFER_SIZE / encrypted_unit;
			if (n_end > d.n_used)
				n_end = d.n_used;
			nread = fread(buf, encrypted_unit, n_end - n, fp);
			if (nread != n_end - n) {
				/*  Read failed */
				DPRINT(("tcapwtab_thaw_encrypted: Could not read element %d\n", n));
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
	*timestamp = d.timestamp;
	*offset = d.offset;
	*lastuid = d.lastuid;
	DPRINT(("tcapwtab_thaw_encrypted: successful.\n"));
	return (void *) tab->buf;
}

#if defined(tcapwtab_UNITTEST)

#include <stdio.h>
#include <time.h>
#define NENTRIES 300

void test1(dynatab_t *pt)
{
	int i;
	FILE *fp;
	dynatab_t *th;
	time_t timestamp;
	time_t timestamp2;

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
	timestamp = time(NULL);
	tcapwtab_freeze_encrypted(pt, fp, "Password", timestamp);
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
	if (!tcapwtab_thaw_encrypted(th, fp, "Password", &timestamp2)) {
		perror("thaw_encrypted");
		exit(1);
	}
	fclose(fp);
	if ((th->n_used != pt->n_used) || (th->unit != pt->unit)) {
		printf("bad n_used or unit: test failed\n");
		exit(1);
	}
	if (timestamp2 != timestamp) {
		printf("timestamp did not restore properly\n");
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

#endif /* tcapwtab_UNITTEST */
