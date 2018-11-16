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

/* #define VERBOSE */
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
 the output file is always yyyymmdd.dat where yyyymmdd are the year, month
 and day in GMT assocated with the record.

 This module's get routine reads from the current location of the current
 file.  If the end of file is reached, it checks for the next file.

 A wmq file has the following format in structure notation:
 struct {
	time_t tmin;	// 4 bytes, foffset 0 - start time of this file
 	time_t tmax;	// 4 bytes, foffset 4 - end time of this file
	char magic[4];	// 4 bytes, foffset 8 - wmqMagic
	// first record begins at foffset 12 = wmq_tell() offset 0
	struct {		// 20+datalen bytes total per record
		time_t t;				// 4 bytes, rcdoffset 0 - time of record
		char serverTag[6];		// 6 bytes, rcdoffset 4 - binary IP:port
		char recordTag[2];		// 2 bytes, rcdoffset 10 - type of data
		unsigned short datalen;	// 2 bytes, rcdoffset 12 - data length
		char data[datalen];		// datalen bytes, rcdoffset 14
		unsigned short datalen;	// 2 bytes, rcdoffset 14+datalen -
								// allows backstepping through records
		char magic[4];			// 4 bytes, rcdoffset 16+datalen - wmqMagic
	} wmq_record[*];
	time_t termination;	// last 4 bytes = 0; present only if terminated
 } wmq_file;
--------------------------------------------------------------------------*/

#include <fcntl.h>
#include <string.h>

#ifdef UNIX
#include <sys/errno.h>
#include <malloc.h>
#endif

#include "anet.h"
#include "contract.h"
#include "wmq.h"
#include "wmqfile.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define wmq_DIRECTORY_DELIMITER '/'
#define wmq_HEADER_LEN	12
static const char wmqMagic[4] = "WMQ1";		/* WMQ version 1.0 */
/* static const char wmqMagic[4] = "WMQ*"; */	/* Old WMQ */

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
static const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 256; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}
#endif

/*--------------------------------------------------------------------------
 Terminate a wmq file with a 0 timestamp, if it is not already terminated.
 An unterminated wmq file ends in the 4 bytes of wmqMagic, while a
 terminated file ends in that plus 4 zero bytes.
 Returns dp_RES_OK if the file is now terminated,
		 dp_RES_BADSIZE if the file is not a valid wmq file.
--------------------------------------------------------------------------*/
static dp_result_t wmq_terminate(const char *filename)
{
	FILE *fp = fopen(filename, "r+b");
	unsigned char buf[wmq_HEADER_LEN];
	time_t timestamp;
	long offset;

	DPRINT(("wmq_terminate: checking file %s\n", filename));
	if (fp == NULL) {
		DPRINT(("wmq_terminate: could not open file %s\n", filename));
		return dp_RES_BUG;
	}
	/* minimum unterminated file is tmin(4), tmax(4), magic(4) */
	if (fread(buf, wmq_HEADER_LEN, 1, fp) != 1) {
		fclose(fp);
		DPRINT(("wmq_terminate: file to short to be wmq\n"));
		return dp_RES_BADSIZE;
	}
	if (memcmp(buf + 8, wmqMagic, 4)) {
		fclose(fp);
		DPRINT(("wmq_terminate: not a wmq file\n"));
		return dp_RES_BADSIZE;
	}
	if (fseek(fp, 0, SEEK_END) != 0) {
		DPRINT(("wmq_terminate: fseek(end) error\n"));
		return dp_RES_BUG;
	}
	offset = ftell(fp);
	if (offset == -1L) {
		DPRINT(("wmq_terminate: ftell error\n"));
		return dp_RES_BUG;
	}
	if (offset < wmq_HEADER_LEN) {
		fclose(fp);
		DPRINT(("wmq_terminate: not a wmq file\n"));
		return dp_RES_BADSIZE;
	}
	if (fseek(fp, -4, SEEK_END) != 0) {
		DPRINT(("wmq_terminate: fseek(end-4) error\n"));
		return dp_RES_BUG;
	}
	if (fread(buf, 4, 1, fp) != 1) {
		DPRINT(("wmq_terminate: fread(end-4) error\n"));
		return dp_RES_BUG;
	}
	if (!memcmp(buf, wmqMagic, 4)) {
		if (fseek(fp, 0, SEEK_END) != 0) {
			DPRINT(("wmq_terminate: fseek(end) error\n"));
			return dp_RES_BUG;
		}
		if (fwrite("\0\0\0\0", 4, 1, fp) != 1) {
			DPRINT(("wmq_terminate: write error on termination attempt.\n"));
			return dp_RES_BUG;
		}
		fclose(fp);
		DPRINT(("wmq_terminate: terminated file at pos 0x%08x\n", offset));
		return dp_RES_OK;
	}
	timestamp = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
	if (timestamp == 0) {
		DPRINT(("wmq_terminate: file already terminated.\n"));
		return dp_RES_OK;
	}
	DPRINT(("wmq_terminate: not a wmq file\n"));
	return dp_RES_BADSIZE;
}

/*--------------------------------------------------------------------------
 Create a web message queue.  Directory is where data files live.
 openForWrite is either 0 for reading or 1 for writing.
--------------------------------------------------------------------------*/
wmq_t *wmq_create(const char *dir, int openForWrite)
{
	wmq_t *wmq;
	int dirlen = strlen(dir);

	if (openForWrite != 0 && openForWrite != 1) {
		DPRINT(("wmq_create: %d is not a valid value for openForWrite.\n", openForWrite));
		return NULL;
	}
	if (dirlen > wmq_DIR_MAXLEN - 1) {
		DPRINT(("wmq_create: directory name %s is too long.\n", dir));
		return NULL;
	}

	wmq = (wmq_t *)malloc(sizeof(wmq_t));
	if (!wmq)
		return NULL;
	memset(wmq, 0, sizeof(wmq_t));

	wmq->openForWrite = openForWrite;
	if (dirlen == 0)
		strcpy(wmq->dir, ".");
	else {
		strcpy(wmq->dir, dir);
		if (dir[dirlen-1] == wmq_DIRECTORY_DELIMITER)
			wmq->dir[dirlen-1] = 0;  /* don't copy trailing '/' */
	}
	DPRINT(("wmq_create: write:%d dir:%s\n", wmq->openForWrite, wmq->dir));
	return wmq;
}

/*--------------------------------------------------------------------------
 Destroy a web message queue.
 Finishes writing output file, and closes the file.  Does not discard
 any data.
--------------------------------------------------------------------------*/
void wmq_destroy(wmq_t *wmq)
{
	dp_result_t err;

	if (!wmq)
		return;
	err = wmq_flush(wmq);
	if (err != dp_RES_OK)
		DPRINT(("wmq_destroy: wmq_flush returns err:%d\n", err));
	fclose(wmq->fp);

	memset(wmq, 0, sizeof(wmq_t));
	free(wmq);
}

/*--------------------------------------------------------------------------
 Set the default server tag used by wmq_put to the 6 byte value in
 serverTag, generally the binary IP:port address.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmq_setServerTag(wmq_t *wmq, const char *serverTag, int serverTagLen)
{
	precondition(wmq);
	precondition(serverTag);

	assert(serverTagLen == 6);
	if (serverTagLen != 6)
		return dp_RES_UNIMPLEMENTED;
	memcpy(wmq->serverTag, serverTag, 6);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Get the position of the next message to be read from a web message queue.
 poffset must be the address of an uninitialized long;
 ptime must be the address of an uninitialized time_t.

 Note that the offset returned is NOT ftell(wmq->fp), since offset 0 points
 to the first record, which is after a header block.

 *ptime is set to the time associated with the most recent record read;
 *poffset is set to the offset in the file holding that record.
--------------------------------------------------------------------------*/
dp_result_t wmq_tell(wmq_t *wmq, long *poffset, time_t *ptime)
{
	precondition(wmq);
	precondition(poffset && ptime);
	precondition(wmq->fp);

	*poffset = ftell(wmq->fp);
	if (*poffset < wmq_HEADER_LEN) {
		DPRINT(("wmq_tell: bug: real file offset:%d < headerlen:%d\n",
			*poffset, wmq_HEADER_LEN));
		*poffset = 0;
		*ptime = 0;
		return dp_RES_BUG;
	}
	*ptime = wmq->t;
	*poffset -= wmq_HEADER_LEN;
	return dp_RES_OK;
}

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
dp_result_t wmq_seek(wmq_t *wmq, time_t when, long offset)
{
	dp_result_t err;
	struct tm *gmt = gmtime(&when);
	char filepath[wmq_DIR_MAXLEN + 24];
	char *file;
	long real_offset;
	char buf[wmq_HEADER_LEN];

	precondition(wmq);

	/* create the filename corresponding to when */
	strcpy(filepath, wmq->dir);
	file = filepath + strlen(wmq->dir);
	*file++ = wmq_DIRECTORY_DELIMITER;
	if (wmqfile_forTime(when, file) != dp_RES_OK) {
		DPRINT(("wmqfile_forTime returns err:%d\n", err));
		return dp_RES_BUG;
	}
	DPRINT(("wmq_seek: seeking file %s offset %d\n", filepath, offset));

	if (wmq->openForWrite == 1) {
		int terminated = FALSE;

		if (wmq->fp != NULL) {
			memset(buf, 0, 4);	/* a zero timestamp */
			if (when < wmq->t) {
				DPRINT(("wmq_seek: can't seek backwards to when:%d < t:%d while writing!\n", when, wmq->t));
				return dp_RES_BAD;
			}
			if (when < wmq->tmax) {
				DPRINT(("wmq_seek: when %d is still in current file: tmin:%d - tmax:%d\n", when, wmq->tmin, wmq->tmax));
				return dp_RES_OK;
			}
			DPRINT(("wmq_seek: terminating old file, writing to new.\n"));
			if (fwrite(buf, 4, 1, wmq->fp) != 1) {
				DPRINT(("wmq_seek: bug: can't terminate old file!\n"));
				return dp_RES_BUG;
			}
			fclose(wmq->fp);
			wmq->fp = NULL;
			terminated = TRUE;
		}
		if ((wmq->fp = fopen(filepath, "rb")) != NULL) {
			/* appending to old file, read tmin and tmax from header */
			DPRINT(("wmq_seek: %s exists, reading header\n", filepath));
			if (fread(buf, wmq_HEADER_LEN, 1, wmq->fp) != 1) {
				DPRINT(("wmq_seek: %s: EOF in header!\n", filepath));
				fclose(wmq->fp);
				wmq->fp = NULL;
				return dp_RES_BADSIZE;
			}
			if (memcmp(buf + 8, wmqMagic, 4)) {
				DPRINT(("wmq_seek: %s: bad header!\n", filepath));
				fclose(wmq->fp);
				wmq->fp = NULL;
				return dp_RES_BADSIZE;
			}
			wmq->tmin = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
			wmq->tmax = dpMAKELONG(buf[4], buf[5], buf[6], buf[7]);
			wmq->t = wmq->tmin;  /* should be t of last record, but ok */
			/* FIXME: might want to fail if file is already terminated */
			fclose(wmq->fp);
			wmq->fp = NULL;
			if ((wmq->fp = fopen(filepath, "ab")) == NULL) {
				DPRINT(("wmq_seek: can't open %s for append.\n", filepath));
				return dp_RES_CANT_WRITE;
			}
		} else {
			/* Starting new file, write tmin and tmax to new header */
			if ((wmq->fp = fopen(filepath, "wb")) == NULL) {
				DPRINT(("wmq_seek: can't open %s for writing.\n", filepath));
				return dp_RES_CANT_WRITE;
			}
			/* set tmin to the last rollover before when */
			wmq->tmin = when - when % wmqfile_ROLLOVER_TIME;
			/* set tmax to 1 sec before next rollover following when */
			wmq->tmax = wmq->tmin + wmqfile_ROLLOVER_TIME - 1;
			DPRINT(("wmq_seek: starting new file: tmin:%d - tmax:%d\n",
				wmq->tmin, wmq->tmax));
			buf[0] = dpGETLONG_FIRSTBYTE(wmq->tmin);
			buf[1] = dpGETLONG_SECONDBYTE(wmq->tmin);
			buf[2] = dpGETLONG_THIRDBYTE(wmq->tmin);
			buf[3] = dpGETLONG_FOURTHBYTE(wmq->tmin);
			buf[4] = dpGETLONG_FIRSTBYTE(wmq->tmax);
			buf[5] = dpGETLONG_SECONDBYTE(wmq->tmax);
			buf[6] = dpGETLONG_THIRDBYTE(wmq->tmax);
			buf[7] = dpGETLONG_FOURTHBYTE(wmq->tmax);
			memcpy(buf + 8, wmqMagic, 4);
			if (fwrite(buf, wmq_HEADER_LEN, 1, wmq->fp) != 1) {
				DPRINT(("wmq_seek: write error:%d starting new file\n",
					ferror(wmq->fp)));
				return dp_RES_BUG;
			}
			wmq->t = wmq->tmin;
		}
		if (!terminated) {
			char prevfilepath[wmq_DIR_MAXLEN + 24];
			char *prevfile;

			strcpy(prevfilepath, wmq->dir);
			prevfile = prevfilepath + strlen(wmq->dir);
			*prevfile++ = wmq_DIRECTORY_DELIMITER;
			err = wmqfile_prev(wmq->dir, file, prevfile);
			if (err == dp_RES_OK) {
				DPRINT(("wmq_seek: checking previous file %s for termination.\n", prevfilepath));
				err = wmq_terminate(prevfilepath);
				if (err != dp_RES_OK) {
					DPRINT(("wmq_seek: bug: couldn't terminate %s!\n", prevfilepath));
					return dp_RES_BUG;
				}
			} else if (err != dp_RES_EMPTY) {
				DPRINT(("wmq_seek: wmqfile_prev returns err:%d\n", err));
				return err;
			}
		}
	} else if (wmq->openForWrite == 0) {
		real_offset = offset + wmq_HEADER_LEN;
		if ((wmq->fp != NULL) && ((when < wmq->tmin) || (when > wmq->tmax))) {
			DPRINT(("wmq_seek: closing old file, reading from new.\n"));
			fclose(wmq->fp);
			wmq->fp = NULL;
		}
		if (wmq->fp == NULL) {
			/* Starting a new file, get tmin and tmax from header */
			if ((wmq->fp = fopen(filepath, "rb")) == NULL) {
				DPRINT(("wmq_seek: can't open %s for reading.\n", filepath));
				err = wmqfile_next(wmq->dir, file, file);
				if (err == dp_RES_OK) {
					DPRINT(("wmq_seek: next file is %s\n", filepath));
					if ((wmq->fp = fopen(filepath, "rb")) == NULL) {
						DPRINT(("wmq_seek: bug: wmqfile_next returns non-existant file %s\n", filepath));
						return dp_RES_BUG;
					}
				} else if (err == dp_RES_EMPTY) {
					return dp_RES_EMPTY;
				} else {
					DPRINT(("wmq_seek: wmqfile_next returns err:%d\n", err));
					return err;
				}
			}
			if (fread(buf, wmq_HEADER_LEN, 1, wmq->fp) != 1) {
				DPRINT(("wmq_seek: %s has incomplete header.\n", filepath));
				return dp_RES_EMPTY;
			}
			if (memcmp(buf + 8, wmqMagic, 4)) {
				DPRINT(("wmq_seek: %s is not a V1.0 wmq file.\n", filepath));
				fclose(wmq->fp);
				wmq->fp = NULL;
				return dp_RES_BADSIZE;
			}
			wmq->tmin = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
			wmq->tmax = dpMAKELONG(buf[4], buf[5], buf[6], buf[7]);
			DPRINT(("wmq_seek: for file:%s, tmin:%d tmax:%d\n", file, wmq->tmin, wmq->tmax));
			wmq->t = wmq->tmin;
		}
		if (fseek(wmq->fp, real_offset, SEEK_SET) != 0) {
			DPRINT(("wmq_seek: fseek says 0x%x (0x%x) is an invalid offset\n", offset, real_offset));
			return dp_RES_BAD;
		}
	} else
		return dp_RES_BUG;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Seek to the point following the last record before <when> in a wmq.
 If writing, returns dp_RES_UNIMPLEMENTED.

 Returns dp_RES_OK on success - wmq is updated to new location,
 		 dp_RES_EMPTY if no later records exist yet - wmq is untouched,
		 dp_RES_BADSIZE if the file is corrupted - wmq is closed.
--------------------------------------------------------------------------*/
dp_result_t wmq_seek_byTime(wmq_t *wmq, time_t when)
{
	dp_result_t err;
	struct tm *gmt;
	char filepath[wmq_DIR_MAXLEN + 24];
	char *file;
	char buf[wmq_HEADER_LEN];

	precondition(wmq);

	if (wmq->openForWrite == 1)
		return dp_RES_UNIMPLEMENTED;
	if (wmq->openForWrite != 0)
		return dp_RES_BUG;

	/* create the filename corresponding to when */
	gmt = gmtime(&when);
	strcpy(filepath, wmq->dir);
	file = filepath + strlen(wmq->dir);
	*file++ = wmq_DIRECTORY_DELIMITER;
	sprintf(file, "%04d%02d%02d.wmq", 1900+gmt->tm_year, gmt->tm_mon+1, gmt->tm_mday);
	DPRINT(("wmq_seek_byTime: seeking t:%d in file %s\n", when, filepath));

	if (wmq->fp != NULL) {
		DPRINT(("wmq_seek_byTime: closing old file, reading from new.\n"));
		fclose(wmq->fp);
		wmq->fp = NULL;
	}
	if ((wmq->fp = fopen(filepath, "rb")) == NULL) {
		DPRINT(("wmq_seek_byTime: can't open %s, finding next.\n", filepath));
		err = wmqfile_next(wmq->dir, file, file);
		if (err == dp_RES_OK) {
			if ((wmq->fp = fopen(filepath, "rb")) == NULL) {
				DPRINT(("wmq_seek_byTime: bug: wmqfile_next returns non-existant file %s?\n", filepath));
				return dp_RES_BUG;
			}
			/* point to start of next file if no current file */
		} else if (err == dp_RES_EMPTY) {
			/* no later files to read */
			return dp_RES_EMPTY;
		} else {
			DPRINT(("wmq_seek_byTime: wmqfile_next returns err:%d\n", err));
			return err;
		}
	}

	/* we're now pointed to the beginning of some file */
	if ((fread(buf, wmq_HEADER_LEN, 1, wmq->fp) != 1)
	||  memcmp(buf + 8, wmqMagic, 4)) {
		DPRINT(("wmq_seek_byTime: not a wmq file\n"));
		fclose(wmq->fp);
		wmq->fp = NULL;
		return dp_RES_BADSIZE;
	}
	wmq->tmin = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
	wmq->tmax = dpMAKELONG(buf[4], buf[5], buf[6], buf[7]);
	wmq->t = wmq->tmin;
	DPRINT(("wmq_seek_byTime: for when:%d, file:%s tmin:%d tmax:%d\n", when, file, wmq->tmin, wmq->tmax));

	/* Scan until we hit a record after <when> or the end of the queue.
	 * This can be expensive if <when> is near or after tmax;  perhaps
	 * the case of <when> > tmax should be dealt with separately?
	 */
	while (1) {
		time_t timestamp;
		long record_start;
		unsigned short datalen;

		record_start = ftell(wmq->fp);
		if (fread(buf, 4, 1, wmq->fp) != 1) {
			if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
				DPRINT(("wmq_seek_byTime: bug: EOF in timestamp, but seek back failed\n"));
				return dp_RES_BUG;
			}
			return dp_RES_EMPTY;  /* No records follow <when>, yet */
		}
		timestamp = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
		if ((timestamp >= when) || (timestamp == 0)) {
			if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
				DPRINT(("wmq_seek_byTime: bug: EOF in timestamp, but seek back failed\n"));
				return dp_RES_BUG;
			}
			if (timestamp == 0)
				return dp_RES_EMPTY;  /* No records follow <when>, yet */
			return dp_RES_OK;
		}

		if ((fseek(wmq->fp, record_start + 12, SEEK_SET) != 0)
		||  (fread(buf, 2, 1, wmq->fp) != 1)) {
			if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
				DPRINT(("wmq_seek_byTime: bug: EOF in record, but seek back failed\n"));
				return dp_RES_BUG;
			}
			return dp_RES_EMPTY;  /* No records follow <when>, yet */
		}
		datalen = dpMAKESHORT(buf[0], buf[1]);
		if ((fseek(wmq->fp, record_start + 14 + datalen, SEEK_SET) != 0)
		||  (fread(buf, 6, 1, wmq->fp) != 1)) {
			if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
				DPRINT(("wmq_seek_byTime: bug: EOF in record end, but seek back failed\n"));
				return dp_RES_BUG;
			}
			return dp_RES_EMPTY;  /* No records follow <when>, yet */
		}
		if ((datalen != dpMAKESHORT(buf[0], buf[1]))
		||  memcmp(buf + 2, wmqMagic, 4)) {
			DPRINT(("wmq_seek_byTime: corrupt wmq file\n"));
			fclose(wmq->fp);
			wmq->fp = NULL;
			return dp_RES_BADSIZE;
		}
		wmq->t = timestamp;
	}
	return dp_RES_BUG;  /* How'd we get here?! */
}

/*--------------------------------------------------------------------------
 Finishes writing any data to output file.
--------------------------------------------------------------------------*/
dp_result_t wmq_flush(wmq_t *wmq)
{
	precondition(wmq);
	DPRINT(("wmq_flush: fflush(wmq->fp)\n"));
	if ((wmq->openForWrite == 1) && wmq->fp)
		fflush(wmq->fp);
	return dp_RES_OK;
}

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
dp_result_t wmq_put(wmq_t *wmq, time_t now, const char *recordTag, const char *data, unsigned short datalen)
{
	return wmq_putServer(wmq, now, NULL, recordTag, data, datalen);
}

dp_result_t wmq_putServer(wmq_t *wmq, time_t now, const char *serverTag, const char *recordTag, const char *data, unsigned short datalen)
{
	dp_result_t err;
	char buf[4];

	precondition(wmq);
	precondition(wmq->openForWrite == 1);
	precondition(wmq->fp);

	/* A wmq record has the following format:
	 * start bytes contents
	 *   0     4   <now>		time() in GMT, network byte order
	 *   4     6   <serverTag>	binary IP:port Address of server
	 *  10     2   <recordTag>	identifies content type
	 *  12     2   <datalen>	(= L), network byte order
	 *  14     L   <data>		network byte order prefered
	 * L +14   2   <datalen>	allows backward reading
	 * L +16   4   <wmqMagic>	validates record
	 *       L + 20 bytes total
	 */

	DPRINT(("wmq_put: writing timestamp:%d serverTag:%s ", now, hexstring(((serverTag) ? serverTag : wmq->serverTag), 6)));
	DPRINT(("recordTag:%s datalen:%u\n", hexstring(recordTag, 2), datalen));
#ifdef VERBOSE
	DPRINT(("wmq_put: buf:%s\n", hexstring(data, datalen)));
#endif
	if (now < wmq->t)
		DPRINT(("wmq_put: warning: writing out of order timestamps\n"));

	if (now > wmq->tmax) {
		/* open a new file for writing.
		 * wmq_seek will terminate this file.
		 */
		DPRINT(("wmq_put: now:%d > wmq->tmax:%d, opening new file\n", now, wmq->tmax));
		err = wmq_seek(wmq, now, 0);
		if (err != dp_RES_OK) {
			DPRINT(("wmq_put: wmq_seek to new file failed with err:%d\n", err));
			return err;
		}
	}

	buf[0] = dpGETLONG_FIRSTBYTE(now);
	buf[1] = dpGETLONG_SECONDBYTE(now);
	buf[2] = dpGETLONG_THIRDBYTE(now);
	buf[3] = dpGETLONG_FOURTHBYTE(now);
	if (fwrite(buf, 4, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write timestamp to file!\n"));
		return dp_RES_BUG;
	}
	if (fwrite(((serverTag)? serverTag : wmq->serverTag), 6, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write serverTag to file!\n"));
		return dp_RES_BUG;
	}
	if (fwrite(recordTag, 2, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write recordTag to file!\n"));
		return dp_RES_BUG;
	}
	buf[0] = dpGETSHORT_FIRSTBYTE(datalen);
	buf[1] = dpGETSHORT_SECONDBYTE(datalen);
	if (fwrite(buf, 2, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write datalen to file!\n"));
		return dp_RES_BUG;
	}
	if (fwrite(data, datalen, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write data to file!\n"));
		return dp_RES_BUG;
	}
	if (fwrite(buf, 2, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write datalen (2) to file!\n"));
		return dp_RES_BUG;
	}
	if (fwrite(wmqMagic, 4, 1, wmq->fp) != 1) {
		DPRINT(("wmq_put: bug: can't write magic to file!\n"));
		return dp_RES_BUG;
	}
	wmq->t = now;
	return dp_RES_OK;
}

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
dp_result_t wmq_get(wmq_t *wmq, wmq_record_t *record)
{
	dp_result_t err;
	char buf[4];
	long record_start;
	unsigned short enddatalen;

	precondition(wmq);
	precondition(wmq->openForWrite == 0);
	precondition(record && record->buf && record->buflen);

	if (!wmq->fp) {
		err = wmq_seek(wmq, 0, 0);
		if (err == dp_RES_EMPTY) {
			return dp_RES_EMPTY;  /* no file to read yet */
		} else if (err != dp_RES_OK) {
			DPRINT(("wmq_get: wmq_seek returned err:%d\n", err));
			return err;
		}
		return wmq_get(wmq, record);
	}

	record_start = ftell(wmq->fp);
	if (record_start == -1L) {
		DPRINT(("wmq_get: bug: ftell(record_start) error\n"));
		return dp_RES_BUG;
	} else if (record_start < wmq_HEADER_LEN) {
		DPRINT(("wmq_get: no next file yet, trying again (t:%d)\n", wmq->tmax+1));
		err = wmq_seek(wmq, wmq->tmax+1, 0);
		if (err == dp_RES_EMPTY) {
			return dp_RES_EMPTY;  /* no file to read yet */
		} else if (err != dp_RES_OK) {
			DPRINT(("wmq_get: wmq_seek returned err:%d\n", err));
			return err;
		}
		return wmq_get(wmq, record);
	}
	if (fread(buf, 4, 1, wmq->fp) != 1) {
		if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
			DPRINT(("wmq_get: bug: EOF in timestamp, but seek back failed\n"));
			return dp_RES_BUG;
		}
		return dp_RES_EMPTY;
	}

	record->timestamp = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
	if (record->timestamp == 0) {
		DPRINT(("wmq_get: read termination, seeking next file (t:%d)\n", wmq->tmax+1));
		err = wmq_seek(wmq, wmq->tmax+1, 0);
		if (err == dp_RES_EMPTY) {
			return dp_RES_EMPTY;  /* no file to read yet */
		} else if (err != dp_RES_OK) {
			DPRINT(("wmq_get: wmq_seek returned err:%d\n", err));
			return err;
		}
		return wmq_get(wmq, record);
	}

	if ((fread(record->serverTag, 6, 1, wmq->fp) != 1)
	||  (fread(record->recordTag, 2, 1, wmq->fp) != 1)
	||  (fread(buf, 2, 1, wmq->fp) != 1)) {
		if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
			DPRINT(("wmq_get: bug: EOF in record, but seek back failed\n"));
			return dp_RES_BUG;
		}
		return dp_RES_EMPTY;
	}
	record->datalen = dpMAKESHORT(buf[0], buf[1]);

	if (record->datalen > record->buflen) {
		if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
			DPRINT(("wmq_get: bug: record->buf too small for data (%d bytes), but seek back failed\n", record->datalen));
			return dp_RES_BUG;
		}
		return dp_RES_FULL;
	}
	if (fread(record->buf, record->datalen, 1, wmq->fp) != 1) {
		if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
			DPRINT(("wmq_get: bug: EOF in data (%d bytes), but seek back failed\n", record->datalen));
			return dp_RES_BUG;
		}
		return dp_RES_EMPTY;
	}
	if (fread(buf, 2, 1, wmq->fp) != 1) {
		if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
			DPRINT(("wmq_get: bug: EOF in record end, but seek back failed\n"));
			return dp_RES_BUG;
		}
		return dp_RES_EMPTY;
	}
	enddatalen = dpMAKESHORT(buf[0], buf[1]);
	if (fread(buf, 4, 1, wmq->fp) != 1) {
		if (fseek(wmq->fp, record_start, SEEK_SET) != 0) {
			DPRINT(("wmq_get: bug: EOF in record end, but seek back failed\n"));
			return dp_RES_BUG;
		}
		return dp_RES_EMPTY;
	}
	if ((enddatalen != record->datalen) || memcmp(buf, wmqMagic, 4)) {
		DPRINT(("wmq_get: bad record: datalens %d != %d ", record->datalen, enddatalen, hexstring(buf, 4)));
		DPRINT(("|| magic %s != %s\n", hexstring(wmqMagic, 4)));
		return dp_RES_BADSIZE;
	}

	wmq->t = record->timestamp;
	DPRINT(("wmq_get: read timestamp:%d serverTag:%s ", wmq->t, hexstring(record->serverTag, 6)));
	DPRINT(("recordTag:%s datalen:%u\n", hexstring(record->recordTag, 2), record->datalen));
#ifdef VERBOSE
	DPRINT(("wmq_get: buf:%s\n", hexstring(record->buf, record->datalen)));
#endif
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Save the current location in the wmq into the file descriptor fd.
 Restore with wmq_restorePosition.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmq_savePosition(wmq_t *wmq, int fd)
{
#ifdef UNIX
	char buf[8];
	char *pbuf = buf;
	time_t t;
	long offset;

	precondition(wmq);
	precondition(fd >= 0);

	if (wmq_tell(wmq, &offset, &t) != dp_RES_OK) {
		DPRINT(("wmq_savePosition: wmq_tell error\n"));
		return dp_RES_BUG;
	}
	if (lseek(fd, 0L, 0) != 0) {
		DPRINT(("wmq_savePosition: lseek(start) error:%d\n", errno));
		return dp_RES_BUG;
	}
	*pbuf++ = dpGETLONG_FIRSTBYTE(t);
	*pbuf++ = dpGETLONG_SECONDBYTE(t);
	*pbuf++ = dpGETLONG_THIRDBYTE(t);
	*pbuf++ = dpGETLONG_FOURTHBYTE(t);
	*pbuf++ = dpGETLONG_FIRSTBYTE(offset);
	*pbuf++ = dpGETLONG_SECONDBYTE(offset);
	*pbuf++ = dpGETLONG_THIRDBYTE(offset);
	*pbuf++ = dpGETLONG_FOURTHBYTE(offset);
	if (write(fd, buf, 8) != 8) {
		DPRINT(("wmq_savePosition: write error:%d\n", errno));
		return dp_RES_BUG;
	}
#endif
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Restore the position in a wmq from a file descriptor fd, created
 by a previous wmq_savePosition.
 Returns dp_RES_OK on success,
 		 dp_RES_EMPTY if the file is empty.
--------------------------------------------------------------------------*/
dp_result_t wmq_restorePosition(wmq_t *wmq, int fd)
{
#ifdef UNIX
	char buf[8];
	long filelen;
	time_t t;
	long offset;

	precondition(wmq);
	precondition(fd >= 0);

	if ((filelen = lseek(fd, 0L, 2)) == -1L) {
		DPRINT(("wmq_restorePosition: lseek(END) error:%d\n", errno));
		return dp_RES_BUG;
	}
	if (filelen == 0) {
		DPRINT(("wmq_restorePosition: empty file\n"));
		return dp_RES_EMPTY;
	}
	if (lseek(fd, 0L, 0) != 0) {
		DPRINT(("wmq_restorePosition: lseek error:%d\n", errno));
		return dp_RES_BUG;
	}
	if (read(fd, buf, 8) != 8) {
		DPRINT(("wmq_restorePosition: read error:%d\n", errno));
		return dp_RES_BUG;
	}
	t = dpMAKELONG(buf[0], buf[1], buf[2], buf[3]);
	offset = dpMAKELONG(buf[4], buf[5], buf[6], buf[7]);

	return wmq_seek(wmq, t, offset);
#else
	return dp_RES_UNIMPLEMENTED;
#endif
}
