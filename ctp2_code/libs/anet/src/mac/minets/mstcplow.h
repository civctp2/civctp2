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

#ifndef mstcplow_H
#define mstcplow_H

#include "mstcp.h"

void TCPLow_Create(TCPINSTANCE* pTcp, u_short* port, TCPPEER* peer, int* status);
void TCPLow_Destroy(TCPINSTANCE* pTcp);
int TCPLow_PutPacket(TCPINSTANCE* tcp, TCPPEER* pPeer,void* bufptr,ULONG len);
int TCPLow_GetPacket(TCPINSTANCE* tcp,void* bufptr,ULONG* pLen,TCPPEER* addr);

#endif
