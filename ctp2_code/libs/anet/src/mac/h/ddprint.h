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
 DDPrint.h
 
 Debug printing for ActiveNet Mac drivers.

 Disabled for the moment... need to bring back part of dprint.h.

 $Log: ddprint.h $
 Revision 1.1  1997/05/30 17:51:34  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef _DDPrint_h_
#define _DDPrint_h_

#define DPRINT(p)

// Checkpoint
#define dp_CKPT		DPRINT(("@" __FILE__ " %u:\n", __LINE__))

#endif // _DDPrint_h_
