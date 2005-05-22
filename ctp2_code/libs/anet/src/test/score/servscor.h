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
 Game server score handling stuff.
--------------------------------------------------------------------------*/

#ifndef servscor_h
#define servscor_h

#include "dp2.h"

/*--------------------------------------------------------------------------
 Call this once when the server is starting up.
--------------------------------------------------------------------------*/
void servscor_init(dp_t *dp);

/*--------------------------------------------------------------------------
 Call once a minute or so to update all cumulative scores.
--------------------------------------------------------------------------*/
void servscor_poll(dp_t *dp);

/*--------------------------------------------------------------------------
 Call when a new client connects to the server. 
--------------------------------------------------------------------------*/
void servscor_addClient(dp_t *dp, playerHdl_t h);

#endif
