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

#ifndef _OTUDP_
#define _OTUDP_

#ifdef __cplusplus
extern "C" {
#endif

	#define	udpMaxRawData	512
	#define	SOCKET_MW2		(0x52A1+commapi_NET_PORTOFFSET)		// (('M'<<4 + 'W')<<4 + '1')

	Boolean OpenTransportInetExists(void);
	OSStatus DoInetBind(EndpointRef ep);
	OSStatus CreateAndConfigUDP(EndpointRef *ep);
	OSStatus ShutDownUDP(void);
	OSStatus HandleErrorUDERR(void);

	pascal void  HandleEndpointEventsUDP(void* contextPtr, OTEventCode code, OTResult result, void* it);

#ifdef __cplusplus
}
#endif

#endif
