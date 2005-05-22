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
 Game server password database handling stuff.
--------------------------------------------------------------------------*/

#ifndef servpw_h
#define servpw_h

#include "tca.h"
#include "eclock.h"

/*--------------------------------------------------------------------------
 Call this once before the server has initialized dp.
 Downloads the password database from the account server, which could
 potentially take a while to finish.
 Returns the socket used by servpw or -1 on error. 
--------------------------------------------------------------------------*/
int servpw_init(const char *wmqDirectory, const char *PWServerAddr,
	int PWServerPort, const char *PWFile);

/*--------------------------------------------------------------------------
 Call this once when the server is starting up after the password database
 has been opened.
--------------------------------------------------------------------------*/
void servpw_initQ(tca_t *tca, const char *wmqDirectory);

/*--------------------------------------------------------------------------
 Set sock in <wfds> and <rfds> for use by select, based on the internal
 state of servpw.
 Returns -1 on error, or the number of socks set (1 or 0).
--------------------------------------------------------------------------*/
int servpw_setfds(fd_set *rfds, fd_set *wfds);

/*--------------------------------------------------------------------------
 Handle downloads from the account server.
 When select returns a socket set by servpw_setfds(), call this function
 repeatedly while it returns dp_RES_OK.
 Returns dp_RES_OK if more processing can be done now,
		 dp_RES_EMPTY if the system is waiting for more data,
		 dp_RES_AGAIN if the system is waiting for another reason,
		 dp_RES_HOST_NOT_RESPONDING if the remote host times out.
--------------------------------------------------------------------------*/
dp_result_t servpw_poll(clock_t now);

/*--------------------------------------------------------------------------
 Call once a minute or so (regardless of select), to handle reconnects and
 processing of downloaded password queue files.
--------------------------------------------------------------------------*/
void servpw_pollQ(tca_t *tca, clock_t now);

#endif
