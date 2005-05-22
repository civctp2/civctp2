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

	#define	ddpMaxRawData		512
	#define kSocketType			"\pNetMech Transponder"
	
	Boolean OpenTransportAppleTalkExists(void);

	OSStatus CreateAndConfigDDP(EndpointRef *ep, Boolean resume);
	OSStatus CreateAndConfigNBP(MapperRef *ep);
	
	OSStatus StartupAppleTalk(void);
	OSStatus ShutDownAppleTalk(void);
	
	OSStatus DoAppleTalkBind(EndpointRef ep, Boolean resume);
	
	OSStatus HandleErrorUDERR(void);
	
	OSStatus OTRegisterMyName(void);
	OSStatus DoOTNameLookup(void);
	UInt16 OTMySetNBPEntity(char *buffer, Ptr nbpObject, Ptr nbpType, Ptr nbpZone);
	
	void CheckZoneList(void);
	short CountZoneNamesInBuffer(Ptr packedBuffer, unsigned long bufferSize);
	short UnpackZoneNames(Ptr packedBuffer, unsigned long bufferSize);
	
	pascal void  HandleEndpointEventsDDP(void* contextPtr, OTEventCode code, OTResult result, void* it);
	pascal void HandleEndpointEventsNBP(void* contextPtr, OTEventCode code, OTResult result, void* it);
	pascal void HandleProvider(void* contextPtr, OTEventCode code, OTResult result, void* it);
	
#ifdef __cplusplus
}
#endif

#endif
