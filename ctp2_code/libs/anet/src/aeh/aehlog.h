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

/*----------------------------------------------------------------------
 Private functions manipulating exception log file;
 independent of dp stuff.
 Everything written to file and to game server are in portable byte order.
----------------------------------------------------------------------*/

#ifndef aehlog_h
#define aehlog_h

#include "aeh.h"

#define aehlog_EXCEPTION_FILE "atvilog.bin"	/* name of default exception log file */
#define aehlog_MAGIC 0x2316  /* tag before each record in exception file */
#define aehlog_MAXSEND 4096  /* max size to send from exception log file */
#define aehlog_MAXLEN 5000L  /* max exception log file length before file is deleted for client */
#define aehlog_SERVER_MAXLEN 1000000L /* max exception log file length before file is deleted for server */

/* values for aehlog_t status */
#define aehlog_NONE 0
#define aehlog_READ 1
#define aehlog_WRITE 2

/* exception log file info */
typedef struct {
	char path[aeh_MAX_PATH];  /* path of exception log file */
	FILE *fp;     /* file pointer to exception log file (can be NULL) */
	int status;   /* if file is open for read, write, or not open */
} aehlog_t;

/*--------------------------------------------------------------------------
 Set the exception file path to logpath and zero the file pointer and status.
 If logpath is "", name is set to default aeh_EXCEPTION_FILE in system temp dir.
 Should be called before any other aehlog functions.
 Input: logpath
 Output: aehlog (preallocated)
 Return: aeh_RES_BAD if couldn't set file path
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aehlog_Create(const char *logpath, aehlog_t *aehlog);

/*--------------------------------------------------------------------------
 Flush and close file specified in aehlog (a file is opened when an aehlog read
  or write method is called).
 Input: aehlog (must have been previously initialized using aehlog_Create())
--------------------------------------------------------------------------*/
void aehlog_close(aehlog_t *aehlog);

/*--------------------------------------------------------------------------
 Delete file specified in aehlog.  Will close the file first if it is open.
 Input: aehlog (must have been previously initialized using aehlog_Create())
--------------------------------------------------------------------------*/
void aehlog_delete(aehlog_t *aehlog);

/*--------------------------------------------------------------------------
 Returns the size of file in aehlog.
 Input: aehlog
 Return: -1 on error.
         0 if file cannot be opened
         size of file in aehlog otherwise
--------------------------------------------------------------------------*/
long aehlog_getfsize(aehlog_t *aehlog);

/*--------------------------------------------------------------------------
 Returns the current offset in the aehlog or -1 on error. 
--------------------------------------------------------------------------*/
long aehlog_tell(aehlog_t *aehlog);

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
int aehlog_seek(aehlog_t *aehlog, long offset);

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
int aehlog_readExceptionRecord(aehlog_t *aehlog, aeh_buf_t *aehbuf, unsigned int *ninst);

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
int aehlog_writeExceptionRecord(aehlog_t *aehlog, const aeh_buf_t *aehbuf, const unsigned int ninst, const unsigned long fmaxsize);

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
int aehlog_appendExceptionRecord(aehlog_t *aehlog, const aeh_buf_t *aehbuf, const unsigned int ninst);

/*--------------------------------------------------------------------------
 Read an aehbuf from a buf in memory in the format of an atvilog.bin file, 
 i.e. of the sort created by aehlog_writetobuf().
 Returns number of bytes read or -1 on failure.
--------------------------------------------------------------------------*/
int aehlog_readfrombuf(aeh_buf_t *aehbuf, unsigned int *ninst, const char *buf, int buflen);

/*--------------------------------------------------------------------------
 Write an aehlog to buf in memory in the format of an atvilog.bin file,
 suitable for use with aehlog_appendMultExceptionRecords.
 buf must be at least aehbuf->buflen + 12 bytes in length.
 Returns number of bytes written or -1 on failure.
--------------------------------------------------------------------------*/
int aehlog_writetobuf(const aeh_buf_t *aehbuf, const unsigned int ninst, char *buf, int buflen);

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
int aehlog_appendMultExceptionRecords(aehlog_t *aehlog, const unsigned char *buf, const unsigned int buflen, const unsigned long fmaxsize);

#endif
