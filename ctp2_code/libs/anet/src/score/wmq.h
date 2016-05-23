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

/*--------------------------------------------------------------------------
 Web message queue module Version 1.0
 Lets you write messages into the queue, retrieve them in time order,
 and seek to a particular time in the queue.

 The queue is stored as a set of disk files, roughly one file per day.
 This allows the queue to be read by remote computers which can access
 the directory in which the files are stored.

 This module's put routine appends the message together with its
 timestamp to the current output file, and advances to the next output
 file if it is time.
 There is an unambiguous mapping between the time/date of a record and
 the file to which the record is written by the put routine:
 the output file is always yyyymmdd.wmq where yyyymmdd are the year, month
 and day in GMT assocated with the record.

 This module's get routine reads from the current location of the current
 file.  If the end of file is reached, it checks for the next file.
--------------------------------------------------------------------------*/

#ifndef WMQ_H
#define WMQ_H

#include <stdio.h>
#include <time.h>
#include "aneterr.h"

#define wmq_RECORDTAG_SCORE			"SC"
#define wmq_RECORDTAG_SESSIONCLOSED	"SX"
#define wmq_RECORDTAG_TEST			"TT"

#define wmq_DIR_MAXLEN	256

/* an instance of the wmq module */
typedef struct {
	char dir[wmq_DIR_MAXLEN];	/* directory holding data files */
	int openForWrite;	/* Must specify read or write when creating */
	time_t t;			/* Most recent time read or written */
	time_t tmin;		/* range of times covered by current file */
	time_t tmax;
	FILE *fp;			/* file pointer at next location to read/write */
	char serverTag[6];	/* unique server identifier - e.g. IP address */
} wmq_t;

/* a wmq record in memory */
typedef struct {
	time_t timestamp;		/* time when this record was written */
	char serverTag[6];		/* unique server identifier - e.g. IP address */
	char recordTag[2];		/* tag identifying type of data */
	unsigned short datalen;	/* length of the data in this record */
	unsigned short buflen;	/* currently allocated length of buf */
	char *buf;				/* pointer to data buffer */
} wmq_record_t;

/*--------------------------------------------------------------------------
 Create a web message queue.  Directory is where data files live.
 openForWrite is either 0 for reading or 1 for writing.
--------------------------------------------------------------------------*/
wmq_t *wmq_create(const char *dir, int openForWrite);

/*--------------------------------------------------------------------------
 Destroy a web message queue.
 Finishes writing output file, and closes the file.  Does not discard
 any data.
--------------------------------------------------------------------------*/
void wmq_destroy(wmq_t *wmq);

/*--------------------------------------------------------------------------
 Set the default server tag used by wmq_put to the 4 byte value in
 serverTag, generally the binary IP:port address.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmq_setServerTag(wmq_t *wmq, const char *serverTag, int serverTagLen);

/*--------------------------------------------------------------------------
 Get the position of the next message to be read from a web message queue.
 poffset must be the address of an uninitialized long;
 ptime must be the address of an uninitialized time_t.

 Note that the offset returned is NOT ftell(wmq->fp), since offset 0 points
 to the first record, which is after a header block.

 *ptime is set to the time associated with the most recent record read;
 *poffset is set to the offset in the file holding that record.
--------------------------------------------------------------------------*/
dp_result_t wmq_tell(wmq_t *wmq, long *poffset, time_t *ptime);

/*--------------------------------------------------------------------------
 Seek to the given position in a web message queue.
 Finds the file corresponding to <when>, then seeks to an <offset> of the
 sort wmq_tell() returns.
 If there is no file corresponding to <when>, wmq_seek will seek to offset
 0 in the next file following <when>.

 Returns dp_RES_OK on success - wmq is updated to new location,
 		 dp_RES_EMPTY if no appropriate file exists yet - wmq is untouched,
		 dp_RES_BADSIZE if the file is corrupted - wmq is closed.
--------------------------------------------------------------------------*/
dp_result_t wmq_seek(wmq_t *wmq, time_t when, long offset);

/*--------------------------------------------------------------------------
 Seek to the point following the last record before <when> in a wmq.
 If writing, returns dp_RES_UNIMPLEMENTED.

 Returns dp_RES_OK on success - wmq is updated to new location,
 		 dp_RES_EMPTY if no later records exist yet - wmq is untouched,
		 dp_RES_BADSIZE if the file is corrupted - wmq is closed.
--------------------------------------------------------------------------*/
dp_result_t wmq_seek_byTime(wmq_t *wmq, time_t when);

/*--------------------------------------------------------------------------
 Finishes writing any data to output file.
--------------------------------------------------------------------------*/
dp_result_t wmq_flush(wmq_t *wmq);

/*--------------------------------------------------------------------------
 Write a record to a web message queue.
 Can only be called if wmq_create was called with openForWrite=1 and
 wmq_seek has been called with now to open the correct file.

 now should be the time associated with the record in GMT.
 recordTag should be a 2 byte tag identifying the type of this data.

 For wmq_putServer, serverTag should be a unique 4 byte tag for this server,
 i.e. IP address.  For wmq_put or if serverTag is NULL, the value set by
 wmq_setServerTag is used.

 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmq_put(wmq_t *wmq, time_t now, const char *recordTag, const char *data, unsigned short datalen);

dp_result_t wmq_putServer(wmq_t *wmq, time_t now, const char *serverTag, const char *recordTag, const char *data, unsigned short datalen);

/*--------------------------------------------------------------------------
 Read the next record from a web message queue into a record structure.
 Can only be called if wmq_create was called with openForWrite=0.

 record->buflen is the allocated length of record->buf.  If this is not
 large enough for the record being read, record->datalen is set to the
 needed size, dp_RES_FULL is returned, and the next call will try again.

 Returns dp_RES_OK on success,
		 dp_RES_EMPTY if no record is available to get,
		 dp_RES_FULL and sets record->datalen if buffer too small,
		 dp_RES_BADSIZE if the wmq file appears to be invalid.
--------------------------------------------------------------------------*/
dp_result_t wmq_get(wmq_t *wmq, wmq_record_t *record);

/*--------------------------------------------------------------------------
 Save the current location in the wmq into the file descriptor fd.
 Restore with wmq_restorePosition.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmq_savePosition(wmq_t *wmq, int fd);

/*--------------------------------------------------------------------------
 Restore the position in a wmq from a file descriptor fd, created
 by a previous wmq_savePosition.
 Returns dp_RES_OK on success,
 		 dp_RES_EMPTY if the file is empty.
--------------------------------------------------------------------------*/
dp_result_t wmq_restorePosition(wmq_t *wmq, int fd);

#endif
