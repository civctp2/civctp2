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
 Uniform error codes for all modules obeying the interface defined
 in commapi.h.

 $Log: commerr.h $
 Revision 1.3  1997/08/22 23:13:36  lbennett
 Added error constants for memory and unimplemented.
 Revision 1.2  1997/05/12 19:23:33  anitalee
 added typedef for comm_status_t
 Revision 1.1  1997/02/06 23:16:34  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef commerr_h
#define commerr_h

typedef long comm_status_t;

/* Chosen to coincide with the error codes in dppkt.h. */
/* Once a number is assigned, never reassign it. */

/* Generic */
#define comm_STATUS_OK 0
#define comm_STATUS_EMPTY 2
#define comm_STATUS_FULL  3
#define comm_STATUS_BAD 5			/* caller gave us bad parameters */
#define comm_STATUS_ALREADY 7
#define comm_STATUS_BUG 8			/* internal error */
#define comm_STATUS_HOST_UNREACHABLE 16
#define comm_STATUS_UNIMPLEMENTED 101	/* that feature not supported by this driver */

/* Specific to dpCreate / commInit */
#define comm_STATUS_BUSY 9			/* dp_RES_MODEM_BUSY, not dp_RES_BUSY */
#define comm_STATUS_NO_ANSWER 10
#define comm_STATUS_NO_DIALTONE 11
#define comm_STATUS_NO_RESPONSE 12
#define comm_STATUS_NETWORK_NOT_PRESENT 13
#define comm_STATUS_BAD_VERSION 14
#define comm_STATUS_BAD_INITSTRING 17

/*
 * If your driver has its own error categories that don't fit the above,
 * you can use values comm_STATUS_MEMORY and above.
 */

#define comm_STATUS_MEMORY			100
/* Used by dpCommThaw and commInit */
#define comm_STATUS_OK_AM_MASTER	102	/* please host the session */
/* Used by commPlayerInfo */
#define comm_STATUS_DUAL_ADDRESS	103 /* used to inform dpio about dual addresses */

#endif
