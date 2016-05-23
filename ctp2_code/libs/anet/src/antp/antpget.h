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
 An implimentation of non-blocking ANTP/1.1 partial gets.
--------------------------------------------------------------------------*/

#include "aneterr.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define antpget_URL_MAXLEN		256		/* max length of URL */
#define antpget_HEADER_MAXLEN	4096	/* max length of ANTP header */
#define antpget_HEADER_MAXLINES	16	/* max number of lines in header */

typedef struct {
	char host[antpget_URL_MAXLEN];	/* host part of URL to antpget */
	int  port;					/* port part of URL to antpget */
	char path[antpget_URL_MAXLEN];	/* path part of URL to antpget */
} antpget_url_t;

typedef struct {
	int status;						/* antp status ret'd in response header */
	char path[antpget_URL_MAXLEN];	/* path returned in response header */
	long offset;					/* offset returned in response header */
	long length;					/* length returned in response header */
	char buf[antpget_HEADER_MAXLEN];	/* buffer containing header */
	int nlines;							/* number of lines parsed */
	char *line[antpget_HEADER_MAXLINES];	/* pointers to lines in buf,
											 * each terminated by a \0.
											 */
} antpget_header_t;

typedef struct {
	int sockfd;					/* socket file descriptor */

	antpget_url_t url;				/* current url */
	struct protoent protoent;		/* current protocol info */
	struct sockaddr_in sockaddr;	/* current socket info */
	long offset;					/* file offset of the current request */

	int respState;	/* State of ANTP response parsing state machine */
	int bNewLine;	/* TRUE if previous character was '\n' */

	antpget_header_t header;	/* the parsed antp header */
	char *pbuf;		/* current location in header.buf;  points to beginning
					 * of data when readHeader is finished.
					 */
	int datalen;	/* length of data remaining at the end of header.buf */

	int readLength;	/* total bytes of data read so far in this response */
} antpget_t;

/*--------------------------------------------------------------------------
 Create an instance of the antpget module.
--------------------------------------------------------------------------*/
antpget_t *antpget_create();

/*--------------------------------------------------------------------------
 Destroy an instance of the antpget module.
--------------------------------------------------------------------------*/
void antpget_destroy(antpget_t *antpget);

/*--------------------------------------------------------------------------
 Parse a url string <urlstr> into its component parts in <url>.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t antpget_parseURL(const char *urlstr, antpget_url_t *url);

/*--------------------------------------------------------------------------
 Set the host for future connections in this instance of antpget.
 Warning: DNS lookups may take a significant amount of time.
 Returns the socket file descriptor on success, or -1 on failure.
--------------------------------------------------------------------------*/
int antpget_setHost(antpget_t *antpget, const char *host, int port);

/*--------------------------------------------------------------------------
 Connect to the host and port specified with antpget_setHost.
 Returns dp_RES_OK on success,
 		 dp_RES_HOST_NOT_RESPONDING if no connection could be established.
--------------------------------------------------------------------------*/
dp_result_t antpget_connect(antpget_t *antpget);

/*--------------------------------------------------------------------------
 Request a partial get at byte <offset> of the file at <path>.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t antpget_requestGet(antpget_t *antpget, const char *path, long offset);

/*--------------------------------------------------------------------------
 Request the name of the first file on the server.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t antpget_requestFirst(antpget_t *antpget);

/*--------------------------------------------------------------------------
 Read from the current response, parsing as an ANTP/1.0 header.
 To parse the header, call this repeatedly while it returns dp_RES_OK.
 dp_RES_AGAIN indicates the caller should wait for more data to accumulate
 before calling again.  dp_RES_EMPTY indicates parsing is done and the
 caller can move on to calling antpget_readData.
 On dp_RES_EMPTY, a pointer to the parsed header is returned in phdr.

 In normal usage, returns:
	dp_RES_OK if any more of the header was processed,
	dp_RES_AGAIN if not, but the header has yet to be terminated,
	dp_RES_EMPTY if there is no more header to be parsed
		(phdr now points to the parsed header),
	dp_RES_HOST_NOT_RESPONDING if the antp connection was lost.
 Abnormal returns include the standard dp_RES_BAD and dp_RES_BUG and:
	dp_RES_NOTYET if no request has been sent,
	dp_RES_ACCESS if the antp server returned an antp error,
	dp_RES_BADSIZE if the antp server returned an invalid header.
--------------------------------------------------------------------------*/
dp_result_t antpget_readHeader(antpget_t *antpget, antpget_header_t **phdr);

/*--------------------------------------------------------------------------
 Read any data available in current response.  No data will be available
 until after the header has been successfully parsed by antpget_readHeader().
 Returns the number of bytes read in nread.
 To read all data, call this repeatedly to get whatever is currently
 available while it returns dp_RES_OK.  dp_RES_AGAIN indicates the caller
 should wait for more data to accumulate before calling again.
 dp_RES_EMPTY indicates this response is done.

 In normal usage, returns:
	dp_RES_OK if there was data,
	dp_RES_AGAIN if there was no data, but more data is expected,
	dp_RES_EMPTY if the current response's data is finished
		(the last of the data, if any, is returned as well),
	dp_RES_HOST_NOT_RESPONDING if the antp connection was lost.
 Abnormal returns include the standard dp_RES_BAD and dp_RES_BUG and:
	dp_RES_NOTYET if the header has not been parsed,
	dp_RES_BADSIZE if the antp server returned an invalid header.
--------------------------------------------------------------------------*/
dp_result_t antpget_readData(antpget_t *antpget, char *buf, int buflen, int *nread);
