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
 Private functions having to do with listing game servers.
 $Log: enumserv.h $
 Revision 1.2  1997/09/28 03:56:09  dkegel
 Provide way to save server list to disk
 Revision 1.1  1997/05/29 19:39:27  dkegel
 Initial revision
----------------------------------------------------------------------*/

#ifndef _enumserv_h_
#define _enumserv_h_

/*----------------------------------------------------------------------
 Call this to get the latest server list and initialize the server ping
 reply array.
----------------------------------------------------------------------*/
void dp_initEnumServers(dp_t *dp);

/*----------------------------------------------------------------------
 Call this from dpPoll or dpEnumServers when it's time to terminate
 the current dpEnumServers enumeration.
----------------------------------------------------------------------*/
void dp_endEnumServers(
	dp_t *dp);

#ifdef dp_ANET2
/*--------------------------------------------------------------------------
 Call this at quit time to save the servers list to disk.
--------------------------------------------------------------------------*/
void dp_saveServerList(dp_t *dp);
#endif

#endif
