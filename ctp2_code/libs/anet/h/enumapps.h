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
 Private functions having to do with listing applications.
 $Log: enumapps.h $
----------------------------------------------------------------------*/

#ifndef _enumapps_h_
#define _enumapps_h_

#ifdef dp_ANET2

/*----------------------------------------------------------------------
Subscribe to the server's table. 
(Called when the server changes)
----------------------------------------------------------------------*/
dp_result_t dp_subscribeAppsList(dp_t *dp);

                    
/*--------------------------------------------------------------------------
Save the apps list to disk.
(Called at quit time to provide quick info on next startup).
--------------------------------------------------------------------------*/
dp_result_t dp_freezeAppsList(dp_t *dp);

#endif

#endif
