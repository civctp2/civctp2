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
 ActiveNet Transport DLL loading functions.
 $Log: dpdll.h $
 Revision 1.5  1997/07/27 22:56:00  dkegel
 Now methods of dpio rather than dp.  That way they can store
 things in the dpio_t, which is good for two reasons:
 1. it lets us get away from statics if we like
 2. it lets us save things across freeze-thaw cycles (like OS comm handles)
 Revision 1.4  1997/02/27 02:32:36  dkegel
 Moved public definitions from dpdll.h into anet.h
 Revision 1.3  1997/02/27 01:51:23  dkegel
 THE BIG REARRANGEMENT: split public API into anet.h; dp.h and dppkt.h
 remain for those who need the full API including the real dp_t;
 care must be taken to avoid circular includes and to avoid activating
 too much of anet.h when using the full API.
 Revision 1.2  1997/02/12 23:25:48  dkegel
 Oops.
 Revision 1.1  1997/02/10 07:03:42  dkegel
 Initial revision
----------------------------------------------------------------------*/

#ifndef dpdll_h
#define dpdll_h

#ifdef __cplusplus
extern "C" {
#endif

/* Included after dpio_t, dp_transport_t, and dp_result_t already known */

/*----------------------------------------------------------------------
 Called internally by dpCreate to load a DLL.
----------------------------------------------------------------------*/
dp_result_t dpLoadDLL(dpio_t *dpio, const dp_transport_t *transportName);

/*----------------------------------------------------------------------
 Called internally by dpDestroy to unload a DLL.
----------------------------------------------------------------------*/
void dpUnloadDLL(dpio_t *dpio);

#ifdef __cplusplus
}
#endif


#endif
