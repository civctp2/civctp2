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

#include	<OpenTransport.h>
#include	<OpenTptAppleTalk.h>
#include	<Memory.h>

#include	"ddprint.h"

#include	"otq.h"
#include	"otddp3.h"
#include	"AppleTalkDDP.h"

extern EndpointRef		gDDPEndpoint;

typedef struct {
	DDPAddress dest_addr;			//	OT destination address
	uint8	flags;
	uint8 inUse;
} DestStruct;

DestStruct*				gDests = nil;
atalk_session_info_t	gSessionInfo;
#define MAX_PLAYERS 8
#define	MAX_DESTS	(MAX_PLAYERS*2)

uint32 atalk_ddp_open(DDPAddress* ip_dst, uint8 flags, uint32 *local_port ) {
	uint32		i;

	//	save all of the paramaters in our array and return the index
	//	that we just used

	if (gDests == nil) {
		gDests = (DestStruct*) NewPtrClear(sizeof(DestStruct) * MAX_DESTS);		//	space for dest records
		if (gDests == nil) {
			return atalk_HDL_NONE;
		}
	}

	for (i = 0; i < MAX_DESTS; i++) {
		if (!gDests[i].inUse) {
			break;
		}
	}

	OTInitDDPAddress(&gDests[i].dest_addr, ip_dst->fNetwork, ip_dst->fNodeID, ip_dst->fSocket, 0);

	gDests[i].inUse = true;
	gDests[i].flags = flags;

	*local_port = 0;			//	never used

	return i;
}

sint32 atalk_ddp_close( uint32 handle, uint8 flags ) {
	gDests[handle].inUse = false;
	gDests[handle].flags = flags;
}

sint32 atalk_ddp_recv( uint32 handle, void *buf, uint32 len,
                   uint32 timeout, uint8 flags,
                   uint32 *ttltos, uint32 *id ) {

	//	this method reads a packet and returns the
	//	size of the data read and sets up the senders
	//	address for the status routine

	uint8		recvBuf[sizeof(DDPAddress) + ddpMaxRawData + 1];
	DDPAddress*			srcAddress;
	uint32		theSize = ddpMaxRawData;
	Boolean				gotPacket;
	extern otq_t*		gInQueue;

	gotPacket = otq_get(gInQueue, recvBuf, &theSize);
	if (gotPacket) {

		//	there is a new packet, deal with it

		theSize -= sizeof(DDPAddress);
		memcpy(buf, recvBuf + sizeof(DDPAddress), theSize);

		//	save the senders address

		srcAddress = (DDPAddress*) recvBuf;
		memcpy(&gSessionInfo.ip_dst, srcAddress, sizeof(DDPAddress));

	} else {

		//	there was no packet to read

		theSize = -1;
	}

	return theSize;
}

sint32 atalk_ddp_send( uint32 handle, void *buf, uint32 len,
                       uint32 ttltos, uint32 id, uint8 flags ) {
	TUnitData	udata;
	OSStatus	err;
	OTResult	theResult;

	udata.addr.len = sizeof(DDPAddress);
	udata.addr.buf = (uint8 *) &gDests[handle].dest_addr;

	udata.opt.len = 0;
	udata.opt.buf = nil;

	udata.udata.len = len;
	udata.udata.buf = (uint8 *)buf;

	do {
		err = OTSndUData(gDDPEndpoint, &udata);
		if (err == kOTLookErr) {
			theResult = OTLook(gDDPEndpoint);
			if (theResult == T_UDERR) {
				HandleErrorUDERR();
				err = 666;
			}
		}
	} while (err == 666);

	return err;
}

sint32 atalk_ddp_status( uint32 handle, uint8 flags, uint32 *size_next,
                           atalk_session_info_t **info ) {
	*info = &gSessionInfo;
}


sint32 atalk_ddp_broadcast(void *buf, uint32 len) {
	OSStatus			err = noErr;
	TUnitData			udata;
	OTResult			theResult;
	sint16				i;
	extern sint16		gLookupCount;
	extern DDPAddress	gLookupResults[];

	//	this method sends the specified packet to our list of
	//	DDP addreses. We return noErr if they were all sent
	//	without any trouble (we should ignore errors which
	//	indicate that the other end is not there since we
	//	really don't care if all of these packets are delivered)

	for (i = 0; i < gLookupCount; i++) {

		udata.addr.len = sizeof(DDPAddress);
		udata.addr.buf = (uint8 *) &gLookupResults[i];

		udata.opt.len = 0;
		udata.opt.buf = nil;

		udata.udata.len = len;
		udata.udata.buf = (uint8 *)buf;

		do {
			err = OTSndUData(gDDPEndpoint, &udata);
			if (err == kOTLookErr) {
				theResult = OTLook(gDDPEndpoint);
				if (theResult == T_UDERR) {
					HandleErrorUDERR();
					err = 666;
				}
			}
		} while (err == 666);

	}

	return err;
}
