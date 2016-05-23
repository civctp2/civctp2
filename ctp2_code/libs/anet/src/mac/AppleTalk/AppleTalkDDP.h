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

/*
 * Copyright (C) 1994-1996, Michael S. Durkin & Tsoft.  All Rights Reserved.
 *    Please see legalese in README.1ST or contact us for details:
 *        Contact:  Mike Durkin        mdurkin@tsoft.net (email preferred)
 *                  c/o Tsoft
 *                  P.O. Box 14897
 *                  Berkeley, CA 94712    USA
 */

/*
 * This file is part of the C language TCP ABI interface library.
 * Please report problems to mdurkin@tsoft.net, or include a fix
 *    if available.
 * Some code may be leached, hopefully noted in comments.  Terms
 * throughout leached from the tcp201.txt specification.
 */

#ifndef _atalk_ddp_h
#define _atalk_ddp_h

#include	<OpenTransport.h>
#include	<OpenTptAppleTalk.h>

// ---------------- UDP ---------------

/* flags for atalk_ddp_open() call */
#define UDP_OPEN_NORMAL    0
#define UDP_OPEN_ASYNCH    128

#define UDP_CLOSE_NORMAL   0
#define UDP_CLOSE_ASYNCH   128

#define UDP_SEND_NORMAL    0
#define UDP_SEND_ASYNCH    128

#define UDP_RECV_NORMAL    0
#define UDP_RECV_ASYNCH    128

#define NULL_HANDLE  		(0xFFFF)
#define atalk_HDL_NONE	NULL_HANDLE
#define atalk_HDL_ME		0xfffe

#define TTLTOS_NORMAL        0

typedef enum {
   TCPABI_NO_ERROR         = 0,
   TCPABI_ERR_BADCALL      = 1,
   TCPABI_ERR_CRITICAL     = 2,
   TCPABI_ERR_NOHANDLES    = 3,
   TCPABI_ERR_BADHANDLE    = 4,
   TCPABI_ERR_TIMEOUT      = 5,
   TCPABI_ERR_BADSESSION   = 6,
   TCPABI_MAXERROR
} atalk_error;

typedef struct {
   DDPAddress			ip_src;        // local AppleTalk address
   unsigned short		port_loc;      // local port  -- NOTE: diff from tcp201
   DDPAddress			ip_dst;        // remote AppleTalk address
   unsigned short		port_rem;      // remote port -- NOTE: diff from tcp201
   unsigned char		ip_prot;
   unsigned char		active;
} atalk_session_info_t;

unsigned atalk_ddp_open(DDPAddress* ip_dst, unsigned char flags, unsigned *local_port );
int atalk_ddp_close( unsigned handle, unsigned char flags );
int atalk_ddp_recv( unsigned handle, void *buf, unsigned len,
                   unsigned timeout, unsigned char flags,
                   unsigned *ttltos, unsigned *id );
int atalk_ddp_send( unsigned handle, void *buf, unsigned len,
                       unsigned ttltos, unsigned id, unsigned char flags );
int atalk_ddp_status( unsigned handle, unsigned char flags, unsigned *size_next,
                           atalk_session_info_t **info );
int atalk_ddp_broadcast(void *buf, unsigned len);

#endif
