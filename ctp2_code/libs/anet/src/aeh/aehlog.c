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

#ifndef NO_NETWORK
#define NO_NETWORK
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aehlog.h"
#include "../dp/dpprivy.h"

/***********************************************************************
 ******** Methods for getting info to/from exception log file **********
 ***********************************************************************/

/* resets aehlog values */
static void aehlog_clear(aehlog_t *aehlog)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	aehlog->fp = NULL;
	aehlog->status = aehlog_NONE;
}

/*--------------------------------------------------------------------------
 Set the exception file path to logpath and zero the file pointer and status.
 If logpath is "", name is set to default aeh_EXCEPTION_FILE in system temp dir.
 Should be called before any other aehlog functions.
 Input: logpath
 Output: aehlog (preallocated)
 Return: aeh_RES_BAD if couldn't set file path
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aehlog_Create(const char *logpath, aehlog_t *aehlog)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!logpath || !aehlog) return aeh_RES_BAD;
	if (*logpath) {
		strcpy(aehlog->path, logpath);
	} else {
		if (!getTempDir(aehlog->path, sizeof(aehlog->path)-strlen(aehlog_EXCEPTION_FILE)-1)) return aeh_RES_BAD;
		strcat(aehlog->path, aehlog_EXCEPTION_FILE);
		if (strlen(aehlog->path) > aeh_MAX_PATH)
			aehDPRINT(("aehlog_Create: aehlog->path %d too large\n", strlen(aehlog->path)));
	}
	aehlog_clear(aehlog);
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Flush and close file specified in aehlog (a file is opened when an aehlog read
  or write method is called).
 Input: aehlog (must have been previously initialized using aehlog_Create())
--------------------------------------------------------------------------*/
void aehlog_close(aehlog_t *aehlog)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aehlog->fp)
		fclose(aehlog->fp);
	aehlog_clear(aehlog);
}

/*--------------------------------------------------------------------------
 Delete file specified in aehlog.  Will close the file first if it is open.
 Input: aehlog (must have been previously initialized using aehlog_Create())
--------------------------------------------------------------------------*/
void aehlog_delete(aehlog_t *aehlog)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	aehlog_close(aehlog);
	remove(aehlog->path);
}

/*--------------------------------------------------------------------------
 Returns the size of file in aehlog.
 Input: aehlog
 Return: -1 on error.
         0 if file cannot be opened
         size of file in aehlog otherwise
--------------------------------------------------------------------------*/
long aehlog_getfsize(aehlog_t *aehlog)
{
	FILE *fp;
	long size;

	if (!(fp = fopen(aehlog->path, "r")))
		return 0;
	if (-1 == fseek(fp, 0L, SEEK_END))
		return -1;
	size = ftell(fp);
	fclose(fp);
	return size;
}

/*--------------------------------------------------------------------------
 Returns the current offset in the aehlog or -1 on error. 
--------------------------------------------------------------------------*/
long aehlog_tell(aehlog_t *aehlog)
{
	if (!aehlog || !aehlog->fp)
		return -1;
	return ftell(aehlog->fp);
}

/*--------------------------------------------------------------------------
 Seek to a given offset in the aehlog and check that it is the beginning of
 a valid record.  Can only be used with read!
 On OK or EMPTY, aehlog is left open so the next call to
 aehlog_readExceptionRecord will get the record at offset.
 On BUG or BAD error, aehlog is restored to its previous state.

 Input: aehlog (must have been previously initialized using aehlog_Create())
 		offset (of the sort returned by aehlog_tell)
 Returns: aeh_RES_OK on success,
          aeh_RES_EMPTY on empty file (EOF),
          aeh_RES_BUG on bad offset (in file, but not a valid record),
		  aeh_RES_BAD if bad arguments,
--------------------------------------------------------------------------*/
int aehlog_seek(aehlog_t *aehlog, long offset)
{
	long old_offset = 0;
	unsigned char buf[3 * sizeof(unsigned int)];
	unsigned char *ptr = buf;
	unsigned int aehlog_tag, ninst, buflen;

	if (!aehlog) return aeh_RES_BAD;
	if (aehlog->status == aehlog_WRITE) 
		aehlog_close(aehlog);
	if (!aehlog->fp) {
		if (!(aehlog->fp = fopen(aehlog->path, "rb")))
			return aeh_RES_EMPTY;
	} else
		old_offset = ftell(aehlog->fp);
	aehlog->status = aehlog_READ;
	if (-1 == fseek(aehlog->fp, offset, SEEK_SET)) {
		if (!old_offset)
			aehlog_close(aehlog);
		else
			fseek(aehlog->fp, old_offset, SEEK_SET);
		return aeh_RES_BAD;		/* offset not in file */
	}

	if (fread(buf, sizeof(unsigned int), 3, aehlog->fp) != 3) {
		if (feof(aehlog->fp))
			return aeh_RES_EMPTY;
		aehDPRINT(("aehlog_readfromfile: read error\n"));
		if (!old_offset)
			aehlog_close(aehlog);
		else
			fseek(aehlog->fp, old_offset, SEEK_SET);
		return aeh_RES_BUG;
	}
	void *vptr = (void *) ptr;
	readSwap((const void **) &vptr, &aehlog_tag, sizeof(int));
	readSwap((const void **) &vptr, &ninst, sizeof(int));
	readSwap((const void **) &vptr, &buflen, sizeof(int));
	if ((aehlog_tag != aehlog_MAGIC) || (buflen > aeh_BUF_MAXLEN)) {
		aehDPRINT(("aehlog_seek: bad offset:%d : tag %x, len %d\n",
			offset, aehlog_tag, buflen));
		if (!old_offset)
			aehlog_close(aehlog);
		else
			fseek(aehlog->fp, old_offset, SEEK_SET);
		return aeh_RES_BUG;
	}
	fseek(aehlog->fp, offset, SEEK_SET);
	return aeh_RES_OK;
}

/* read next aehbuf from aehlog->fp */
static int aehlog_readfromfile(aehlog_t *aehlog, aeh_buf_t *aehbuf, unsigned int *ninst)
{
	unsigned char buf[3 * sizeof(unsigned int)];
	unsigned char *ptr = buf;
	int aehlog_tag;
	if (fread(buf, sizeof(unsigned int), 3, aehlog->fp) != 3) {
		if (feof(aehlog->fp)) return aeh_RES_EMPTY;
		aehDPRINT(("aehlog_readfromfile: read error\n"));
		return aeh_RES_BUG;
	}
	void *vptr = (void *) ptr;
	readSwap((const void**)&vptr, &aehlog_tag, sizeof(aehlog_tag));
	readSwap((const void**)&vptr, ninst, sizeof(*ninst));
	readSwap((const void**)&vptr, &(aehbuf->buflen), sizeof(aehbuf->buflen));
	if ((aehlog_tag != aehlog_MAGIC) ||
		(aehbuf->buflen > aeh_BUF_MAXLEN) ||
		(fread(aehbuf->buf, sizeof(unsigned char), aehbuf->buflen, aehlog->fp) != aehbuf->buflen)) {
		if (feof(aehlog->fp)) return aeh_RES_EMPTY;
		aehDPRINT(("aehlog_readfromfile: err: tag %x, len %d\n", aehlog_tag, aehbuf->buflen));
		return aeh_RES_BUG;
	}
	return aeh_RES_OK;
}

/* write aehbuf to aehlog->fp */
static int aehlog_writetofile(aehlog_t *aehlog, const aeh_buf_t *aehbuf, const unsigned int ninst)
{
	unsigned char buf[3 * sizeof(unsigned int)];
	unsigned char *ptr = buf;
	int aehlog_tag = aehlog_MAGIC;
	void *vptr = (void *) ptr;
	writeSwap((void**)&vptr, &aehlog_tag, sizeof(aehlog_tag));
	writeSwap((void**)&vptr, &ninst, sizeof(ninst));
	writeSwap((void**)&vptr, &(aehbuf->buflen), sizeof(aehbuf->buflen));
	if ((fwrite(buf, sizeof(unsigned int), 3, aehlog->fp) != 3) ||
		(fwrite(aehbuf->buf, sizeof(unsigned char), aehbuf->buflen, aehlog->fp) != aehbuf->buflen)) {
		aehDPRINT(("aehlog_writetofile: write err: tag %x, len %d\n", aehlog_tag, aehbuf->buflen));
		return aeh_RES_BUG;
	}
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Read an aehbuf from a buf in memory in the format of an atvilog.bin file, 
 i.e. of the sort created by aehlog_writetobuf().
 Returns number of bytes read or -1 on failure.
--------------------------------------------------------------------------*/
int aehlog_readfrombuf(aeh_buf_t *aehbuf, unsigned int *ninst, const char *buf, int buflen)
{
	const char *ptr = buf;
	int aehlog_tag;

	if (!aehbuf || !buf || (buflen < 3 * sizeof(int)))
		return -1;
	void *vptr = (void *) &ptr;
	readSwap((const void**)&vptr, &aehlog_tag, sizeof(aehlog_tag));
	readSwap((const void**)&vptr, ninst, sizeof(*ninst));
	readSwap((const void**)&vptr, &(aehbuf->buflen), sizeof(aehbuf->buflen));
	if (aehlog_tag != aehlog_MAGIC) {
		aehDPRINT(("aehlog_readfrombuf: err: tag %x != %x\n", aehlog_tag, aehlog_MAGIC));
		return -1;
	} else if (aehbuf->buflen > aeh_BUF_MAXLEN) {
		aehDPRINT(("aehlog_readfrombuf: err: len %d > max %d\n", aehbuf->buflen, aeh_BUF_MAXLEN));
		return -1;
	} else if ((int)aehbuf->buflen > (buflen - (ptr - buf))) {
		aehDPRINT(("aehlog_readfrombuf: err: len %d > buflen %d\n", aehbuf->buflen + (ptr - buf), buflen));
		return -1;
	}
	memcpy(aehbuf->buf, ptr, aehbuf->buflen);
	return (aehbuf->buflen + (ptr - buf));
}

/*--------------------------------------------------------------------------
 Write an aehbuf to buf in memory in the format of an atvilog.bin file, 
 suitable for use with aehlog_appendMultExceptionRecords.
 buf must be at least aehbuf->buflen + 12 bytes in length.
 Returns number of bytes written or -1 on failure.
--------------------------------------------------------------------------*/
int aehlog_writetobuf(const aeh_buf_t *aehbuf, const unsigned int ninst, char *buf, int buflen)
{
	char *ptr = buf;
	int aehlog_tag = aehlog_MAGIC;

	if (!aehbuf || !buf || (buflen < (int)aehbuf->buflen + 3*sizeof(int)))
		return -1;
	if (aehbuf->buflen > aeh_BUF_MAXLEN) {
		aehDPRINT(("aehlog_writetobuf: err: aehbuf->buflen %d > max %d\n", aehbuf->buflen, aeh_BUF_MAXLEN));
		return -1;
	}
	void *vptr = (void *) ptr;
	writeSwap((void**)&vptr, &aehlog_tag, sizeof(aehlog_tag));
	writeSwap((void**)&vptr, &ninst, sizeof(ninst));
	writeSwap((void**)&vptr, &(aehbuf->buflen), sizeof(aehbuf->buflen));
	memcpy(ptr, aehbuf->buf, aehbuf->buflen);
	return (aehbuf->buflen + (ptr - buf));
}

/* see if exception is already in the logfile; if so, set file position to
 * position of that exception and return the exception count,
 * otherwise set file position to end of file and return zero. */
static int checkDupException(const unsigned char *buf, unsigned int size, unsigned long fsize, aehlog_t *aehlog)
{
	int err;
	unsigned int ninst;
	aeh_buf_t tmpaehbuf;
	fpos_t fptr;
	long fpos;
	aeh_SetCurrent(__LINE__, __FILE__);
	fseek(aehlog->fp, 0L, SEEK_SET);
	while ((fpos = ftell(aehlog->fp)) >= 0 && fpos < fsize) {
		fgetpos(aehlog->fp, &fptr);
		if ((err = aehlog_readfromfile(aehlog, &tmpaehbuf, &ninst)) != aeh_RES_OK) {
			if (err == aeh_RES_EMPTY) break;
			aehDPRINT(("aehlog_checkDupException: err reading from file\n"));
			return -1;
		}
		if (size == tmpaehbuf.buflen &&
			!memcmp(tmpaehbuf.buf, buf, tmpaehbuf.buflen)) {
			fsetpos(aehlog->fp, &fptr);
			return ninst;
		}
	}
	fseek(aehlog->fp, 0L, SEEK_END);
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}
	
/* get the exception file and set the file position for writing */
static int initExceptionFileStream(aehlog_t *aehlog, const aeh_buf_t *aehbuf, unsigned int *ninst, unsigned long fmaxsize)
{
	int retVal;
	unsigned long fsize;
	aeh_SetCurrent(__LINE__, __FILE__);
	*ninst = 0;
	if (!aehlog->fp) {
		if (!(aehlog->fp = fopen(aehlog->path, "r+b"))) {
			if (!(aehlog->fp = fopen(aehlog->path, "w+b")))
				return aeh_RES_EMPTY;
			else
				return aeh_RES_OK;
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (fseek(aehlog->fp, 0L, SEEK_END)) {
		aehDPRINT(("initExceptionFileStream: can't find file end\n"));
		aehlog_close(aehlog);
		if (!(aehlog->fp = fopen(aehlog->path, "w+b")))
			return aeh_RES_EMPTY;
		else
			return aeh_RES_OK;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (((fsize = ftell(aehlog->fp)) > fmaxsize) || ((retVal = checkDupException(aehbuf->buf, aehbuf->buflen, fsize, aehlog)) < 0)) {
		aehDPRINT(("initExceptionFileStream: err:fsize,retVal:%lu, %d\n", fsize, retVal));
		aehlog_close(aehlog);
		if (!(aehlog->fp = fopen(aehlog->path, "w+b")))
			return aeh_RES_EMPTY;
	} else
		*ninst = retVal;
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Write info on a single crash to file. Checks in file for identical crash
  record and increments its count if found; otherwise appends the new record.
 File is opened if not already open; it is not automatically closed,
  call aehlog_close() when done reading.
 Input: aehlog (must have been previously initialized using aehlog_Create())
        aehbuf
		ninst (number of instances of this record)
        fmaxsize (if file larger than this, file contents deleted before info is written)
 Returns: aeh_RES_EMPTY if file could not be opened
          aeh_RES_BUG if file write error occurred(file is deleted in this case)
		  aeh_RES_BAD if bad arguments
          aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aehlog_writeExceptionRecord(aehlog_t *aehlog, const aeh_buf_t *aehbuf, const unsigned int ninst, const unsigned long fmaxsize)
{
	int err;
	unsigned int file_ninst;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aehlog || !aehbuf || aehbuf->buflen > aeh_BUF_MAXLEN) return aeh_RES_BAD;
	if (aehlog->status == aehlog_READ) {
		aehDPRINT(("aehlog_writeExceptionRecord: was reading; deleting file\n"));
		aehlog_delete(aehlog);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	err = initExceptionFileStream(aehlog, aehbuf, &file_ninst, fmaxsize);
	if (err != aeh_RES_OK) {
		aehDPRINT(("aehlog_writeExceptionRecord: initException err %d\n", err));
		return err;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	file_ninst += ninst;
	aehlog->status = aehlog_WRITE;
	if ((err = aehlog_writetofile(aehlog, aehbuf, file_ninst)) != aeh_RES_OK) {
		aehDPRINT(("aehlog_writeExceptionRecord: err writing to file\n"));
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return err;
}

/*--------------------------------------------------------------------------
 Append info (consisting of one or more records packed together) to file.
  Used by game server to quickly add info to file after receiving it from dptab.
 File is opened if not already open; it is not automatically closed,
  call aehlog_close() when done reading.
 Input: aehlog (must have been previously initialized using aehlog_Create())
        buf (one or more crash records)
        buflen (length of buf)
        fmaxsize (if file larger than this, file contents deleted before info is written)
 Returns: aeh_RES_EMPTY if file could not be opened
          aeh_RES_BUG if file write error occurred(file is deleted in this case)
		  aeh_RES_BAD if bad arguments
          aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aehlog_appendMultExceptionRecords(aehlog_t *aehlog, const unsigned char *buf, const unsigned int buflen, const unsigned long fmaxsize)
{
	if (!aehlog || !buf || buflen > fmaxsize) return aeh_RES_BAD;
	if (aehlog->status == aehlog_READ) {
		aehDPRINT(("aehlog_appendMultExceptionRecords: was reading; deleting file\n"));
		aehlog_delete(aehlog);
	}
	if (!aehlog->fp)
		if (!(aehlog->fp = fopen(aehlog->path, "ab"))) return aeh_RES_EMPTY;
	aehlog->status = aehlog_WRITE;
	if (fwrite(buf, sizeof(unsigned char), buflen, aehlog->fp) != buflen) {
		return aeh_RES_BUG;
	}
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Simple append info on a single crash to file (no checking in file for
  identical crash record or for max file size); closes file when done appending.
 Currently used when exception has occurred due to exception handler itself
  and a simple, bug-free (hopefully) way to write to file is needed.
 Input: aehlog (must have been previously initialized using aehlog_Create())
        aehbuf
		ninst (number of instances of this record)
 Returns: aeh_RES_EMPTY if file could not be opened
          aeh_RES_BUG if file write error occurred(file is deleted in this case)
		  aeh_RES_BAD if bad arguments
          aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aehlog_appendExceptionRecord(aehlog_t *aehlog, const aeh_buf_t *aehbuf, const unsigned int ninst)
{
	int err;
	if (!aehlog || !aehbuf || aehbuf->buflen > aeh_BUF_MAXLEN) return aeh_RES_BAD;
	if (aehlog->fp)
		aehlog_close(aehlog);
	if (!(aehlog->fp = fopen(aehlog->path, "ab"))) return aeh_RES_EMPTY;
	aehlog->status = aehlog_WRITE;
	if ((err = aehlog_writetofile(aehlog, aehbuf, ninst)) != aeh_RES_OK) {
		aehDPRINT(("aehlog_appendExceptionRecord: err writing to file\n"));
	}
	return err;
}

/*--------------------------------------------------------------------------
 Read info on a single crash from file; loop to read entire file.
 File is opened if not already open; it is not automatically closed;
  call aehlog_close() when done reading.
 Input: aehlog (must have been previously initialized using aehlog_Create())
 Output: aehbuf (preallocated)
         ninst (number of instances of this record)
 Returns: aeh_RES_EMPTY if file could not be opened or eof is detected
          aeh_RES_BUG if file read error occurred(file is deleted in this case)
		  aeh_RES_BAD if bad arguments
          aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aehlog_readExceptionRecord(aehlog_t *aehlog, aeh_buf_t *aehbuf, unsigned int *ninst)
{
	int err;
	if (!aehlog || !aehbuf || !ninst) return aeh_RES_BAD;
	if (aehlog->status == aehlog_WRITE) 
		aehlog_close(aehlog);
	if (!aehlog->fp)
		if (!(aehlog->fp = fopen(aehlog->path, "rb"))) return aeh_RES_EMPTY;
	aehlog->status = aehlog_READ;
	if ((err = aehlog_readfromfile(aehlog, aehbuf, ninst)) != aeh_RES_OK &&
		(err != aeh_RES_EMPTY)) {
		aehDPRINT(("aehlog_readExceptionRecord: err reading from file\n"));
	}
	return err;
}
