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
 
#ifndef _tcpabi_udp_h
#define _tcpabi_udp_h
// ---------------- UDP ---------------

/* flags for tcpabi_udp_open() call */
#define UDP_OPEN_NORMAL    0
#define UDP_OPEN_ASYNCH    128

#define UDP_CLOSE_NORMAL   0
#define UDP_CLOSE_ASYNCH   128

#define UDP_SEND_NORMAL    0
#define UDP_SEND_ASYNCH    128

#define UDP_RECV_NORMAL    0
#define UDP_RECV_ASYNCH    128

#define NULL_HANDLE  	(0xFFFF)
#define trump_HDL_NONE	NULL_HANDLE
#define trump_HDL_ME	0xfffe

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
} tcpabi_error;

typedef struct {
   unsigned long     ip_src;        // local IP address
   unsigned short    port_loc;      // local port  -- NOTE: diff from tcp201
   unsigned long     ip_dst;        // remote IP address
   unsigned short    port_rem;      // remote port -- NOTE: diff from tcp201
   unsigned char     ip_prot;
   unsigned char     active;
} tcpabi_session_info_t;

unsigned tcpabi_udp_open( unsigned long ip_dst, unsigned dst_port,
                     unsigned src_port,
                     unsigned char flags, unsigned *local_port );
int tcpabi_udp_close( unsigned handle, unsigned char flags );
int tcpabi_udp_recv( unsigned handle, void *buf, unsigned len,
                   unsigned timeout, unsigned char flags,
                   unsigned *ttltos, unsigned *id );
int tcpabi_udp_send( unsigned handle, void *buf, unsigned len,
                       unsigned ttltos, unsigned id, unsigned char flags );
int tcpabi_udp_status( unsigned handle, unsigned char flags, unsigned *size_next,
                           tcpabi_session_info_t **info );
													 
int tcpabi_udp_send_to_address(InetAddress* theAddress, void *buf, unsigned len);
int tcpabi_udp_broadcast(void *buf, unsigned len);

void AddAddressToList(InetHost newAddress);
void AddAddressListToList(InetHost* newAddressList, short count);
void InitAddressList(void);

short GetSendCount(InetHost theAddress);
void ChangeSendCount(InetHost theAddress, short newCount);

#endif
