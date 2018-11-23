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
 Code to set up and manage a TLI (OpenTransport) UDP socket.
 by Howard Shere of Green Dragon
 (C) 1996 Activision

 $Log: otudp.c $
 Revision 1.2  1997/08/22 18:18:36  anitalee
 changes made by Danny Swarzman.
 Revision 1.1  1997/05/31 22:54:24  dkegel
 Initial revision
--------------------------------------------------------------------------*/
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <CodeFragments.h>
#include "commapi.h"
#include "ddprint.h"
#include "otq.h"
#include "otudp.h"
#include "otutils.h"

UInt8					gRecvBuf[sizeof(InetAddress) + udpMaxRawData + 1];
Boolean					gEndPointValidUDP = false;
TEndpointInfo			gEndPointInfo;
otq_t*					gInQueue = nil;
long					gExtraOTDataUDP = 0;
EndpointRef				gUDPEndpoint;

Boolean OpenTransportInetExists(void) {
	Boolean		result = false;

	//	this method checks for the existance of Open Transport

	if ( OTOpenInternetServices != (UInt16) kUnresolvedCFragSymbolAddress ) {

		//	we can find one of the calls, so, the engine must exist

		result = true;
	}

	return result;
}
OSStatus DoInetBind(EndpointRef ep) {
	OSStatus			err;
	TBind				bindReq;
	InetAddress			inAddr;
	InetInterfaceInfo	theInfo;

	//	build an internet address with the port number for MW2
	//	and our IP address

	OTInetGetInterfaceInfo(&theInfo, kDefaultInetInterface);
	OTInitInetAddress(&inAddr, SOCKET_MW2, theInfo.fAddress);

	bindReq.addr.maxlen = sizeof(InetAddress);
	bindReq.addr.len = sizeof(InetAddress);
	bindReq.addr.buf = (unsigned char*) &inAddr;
	bindReq.qlen = 0;

	err = OTBind(ep, &bindReq, nil);
	return err;
}
OSStatus CreateAndConfigUDP(EndpointRef *ep) {
	OSStatus			err;
	TBind				bindInfo;

	//	create a queue to recieve packets into
	gInQueue = otq_create();
	if (gInQueue != nil) {
		*ep = OTOpenEndpoint(OTCreateConfiguration(kUDPName), 0, nil, &err);
		if (err == noErr) {

			err = OTGetEndpointInfo(*ep, &gEndPointInfo);
			if (err == noErr) {

				//	install a notifier and bind the endpoint

				err = OTInstallNotifier(*ep, HandleEndpointEventsUDP, &gExtraOTDataUDP);
				if (err == noErr) {
	    			err = DoInetBind(*ep);
					if (err == noErr) {

						//	once we're all setup, we want to start operating in async mode

						err = OTSetAsynchronous(*ep);
						if (err == noErr) {
							//err = OTSetBlocking(*ep);
							err = OTDontAckSends(*ep);
							gEndPointValidUDP = true;
						}
					}
				}
			}

		}

	} else {
		err = MemError();
	}

	gUDPEndpoint = *ep;
	return (err);
}
OSStatus ShutDownUDP(EndpointRef ep) {
	OSStatus	result = noErr;
	CloseTheEndPoint(ep);			//	set the endpoint back to sync mode to tear it down
	CloseOpenTransport();

	otq_destroy(gInQueue);
	gInQueue = nil;
	return result;
}

#pragma segment Main
pascal void HandleEndpointEventsUDP(void* contextPtr, OTEventCode code, OTResult result, void* it) {
	OSStatus 					error;
	static unsigned long		unhandled = 0;
	static unsigned long		unknown = 0;
	static unsigned long		overflowErrors = 0;
	Boolean						doneReading = false;
	OTResult					epState;
	//	this method handles events for this endpoint
	switch (code) {
		case T_DATA:

			do {
				OTFlags				flags = 0;
				TUnitData			theData;

				theData.addr.buf = (UInt8*) gRecvBuf;				//	address destination
				theData.addr.maxlen = sizeof(InetAddress);			//	address length
				theData.opt.len = 0;
				theData.opt.buf = nil;
				theData.udata.buf = gRecvBuf + sizeof(InetAddress);	//	data destination
				theData.udata.maxlen = udpMaxRawData;				//	buffer size
				theData.opt.maxlen = 0;

				error = OTRcvUData(gUDPEndpoint, &theData, &flags);

				//	if there is too much data for our buffer, there is something
				//	wrong with our implementation

				if (flags & T_MORE) {
					overflowErrors++;
				}

				//	process errors and keep good packets

				switch (error) {
					case kOTNoError:
						otq_put(gInQueue, gRecvBuf, sizeof(InetAddress) + theData.udata.len);
						break;

					case kOTLookErr:
						error = OTLook(gUDPEndpoint);
						if (error == T_ORDREL) {
							doneReading = true;
						} else {
							epState = OTGetEndpointState(gUDPEndpoint);
							if ( (error == kOTOutStateErr) && (epState == T_INREL) ) {
								doneReading = true;
							}
							if (epState == T_IDLE) {
								doneReading = true;
							}
						}
						break;

					case kOTNoDataErr:
						doneReading = true;
						break;
				}

			} while (!doneReading);

			break;

		case T_UDERR:
			HandleErrorUDERR();
			break;
		case T_LISTEN:
		case T_CONNECT:
		case T_EXDATA:
		case T_DISCONNECT:
		case T_ERROR:
		case T_ORDREL:
		case T_GODATA:
		case T_GOEXDATA:
		case T_REQUEST:
		case T_REPLY:
		case T_PASSCON:
		case T_RESET:
		case T_BINDCOMPLETE:
		case T_UNBINDCOMPLETE:
		case T_ACCEPTCOMPLETE:
		case T_REPLYCOMPLETE:
		case T_DISCONNECTCOMPLETE:
		case T_OPTMGMTCOMPLETE:
		case T_OPENCOMPLETE:
		case T_GETPROTADDRCOMPLETE:
		case T_RESOLVEADDRCOMPLETE:
		case T_GETINFOCOMPLETE:
		case T_SYNCCOMPLETE:
		case T_MEMORYRELEASED:
		case T_REGNAMECOMPLETE:
		case T_DELNAMECOMPLETE:
		case T_LKUPNAMECOMPLETE:
		case T_LKUPNAMERESULT:
		case kOTProviderWillClose:
		case kOTProviderIsClosed:
		case kOTConfigurationChanged:
			unhandled++;
			break;

		default:
			unknown++;
			break;
	}
}
#pragma segment Main
OSStatus HandleErrorUDERR(void) {
	TUDErr		uderr;
	char		addrBuf[sizeof(InetAddress)];
	char		optBuff[100];
	uderr.addr.maxlen = sizeof(InetAddress);
	uderr.addr.buf = (unsigned char *)addrBuf;

	uderr.opt.maxlen = 100;
	uderr.opt.buf = (unsigned char *)optBuff;
	return OTRcvUDErr(gUDPEndpoint, &uderr);
}
