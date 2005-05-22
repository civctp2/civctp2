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

#ifndef _TRUMPDLL_
#define _TRUMPDLL_

#ifdef __cplusplus
extern "C" {
#endif

#include "commapi.h"

typedef long ip_adr_t;
typedef int trump_hdl_t;

void printAdr(ip_adr_t adr);
trump_hdl_t trump_adr2hdl(ip_adr_t remoteadr, short localport, short remoteport, int insert);
comm_status_t trump_hdl2adr(trump_hdl_t hdl, ip_adr_t *adr);

#ifdef __cplusplus
}
#endif
#endif
