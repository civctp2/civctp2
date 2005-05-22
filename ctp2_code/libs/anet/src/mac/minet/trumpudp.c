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
 * trumpudp.c
 $Log: trumpudp.c $
 Revision 1.2  1997/08/22 18:21:39  anitalee
 changes made by Danny Swarzman.
 Revision 1.1 1997/08/05 dswarzman
 Fixed system includes.
*/

#include	<OpenTransport.h>
#include	<OpenTptInternet.h>
#include  <Memory.h>
#include	"ddprint.h"

#include	"dp.h"
#include	"otq.h"
#include	"otudp.h"
#include	"trumpudp.h"

extern EndpointRef		gUDPEndpoint;

typedef struct {
	InetAddress dest_addr;			//	OT destination address
	unsigned long ipDest;			//	destination address
	unsigned long portDest;			//	destination port
	unsigned long	portSource;		//	source port;
	unsigned char	flags;
	unsigned char inUse;
} DestStruct;

DestStruct*				gDests = nil;
tcpabi_session_info_t	gSessionInfo;

#define	kMaxAddressesIP	32								//	maximum addresses we broadcast to

typedef struct {
	unsigned short		packetType;
	unsigned short		addressCount;
	InetHost				addressList[kMaxAddressesIP];
} AddressListStruct;

//typedef struct {
//	InetHost			address;
//	short				lastCountSent;
//} SendCountStruct;

AddressListStruct		gAddressList;

//short					gSendCountCount;
//SendCountStruct			gSendCount[kMaxAddressesIP];

#define	MAX_PLAYERS	10
#define	MAX_DESTS	(MAX_PLAYERS*2)

unsigned tcpabi_udp_open( unsigned long ip_dst, unsigned dst_port,
                     unsigned src_port,
                     unsigned char flags, unsigned *local_port ) {
	unsigned long		i;
					 
	//	save all of the paramaters in our array and return the index
	//	that we just used
	
	if (gDests == nil) {
		gDests = (DestStruct*) NewPtrClear(sizeof(DestStruct) * MAX_DESTS);		//	space for dest records
		if (gDests == nil) {
			return trump_HDL_NONE;
		}
	}
	
	for (i = 0; i < MAX_DESTS; i++) {
		if (!gDests[i].inUse) {
			break;
		}
	}
	
	OTInitInetAddress(&gDests[i].dest_addr, dst_port, ip_dst);		
	
	gDests[i].inUse = true;
	gDests[i].ipDest = ip_dst;
	gDests[i].portDest = dst_port;
	gDests[i].portSource = src_port;
	gDests[i].flags = flags;
	
	*local_port = 0;			//	never used
	
	return i;
}

int tcpabi_udp_close( unsigned handle, unsigned char flags ) {
	gDests[handle].inUse = false;
	//gDests[handle].dest_addr = 0;
	gDests[handle].ipDest = 0;
	gDests[handle].portDest = 0;
	gDests[handle].portSource = 0;
	gDests[handle].flags = flags;
}


int tcpabi_udp_recv( unsigned handle, void *buf, unsigned len,
                   unsigned timeout, unsigned char flags,
                   unsigned *ttltos, unsigned *id ) {
				   
	//	this method reads a packet and returns the
	//	size of the data read and sets up the senders
	//	address for the status routine
				   
	unsigned char		recvBuf[sizeof(InetAddress) + udpMaxRawData + 1];
	AddressListStruct*	addressList;
	InetAddress*		srcAddress;
	unsigned long		theSize = udpMaxRawData;
	Boolean				gotPacket;
	extern otq_t*		gInQueue;

	gotPacket = otq_get(gInQueue, recvBuf, &theSize);
	if (gotPacket) {
	
		//	check to see if this is an address list packet. If it is, the transport handles
		//	it and returns nothing to the outside world
		
		addressList = (AddressListStruct*) (recvBuf + sizeof(InetAddress));
		if ( addressList->packetType == gAddressList.packetType) {
		
			AddAddressListToList(&(addressList->addressList[0]), addressList->addressCount);
		
			//	we didn't get a packet that the application needs to worry about
			
			theSize = -1;
			
		} else {
		
			//	there is a new packet, deal with it
		
			theSize -= sizeof(InetAddress);
			memcpy(buf, recvBuf + sizeof(InetAddress), theSize);
			
			//	save the senders address
	
			srcAddress = (InetAddress*) recvBuf;
			gSessionInfo.ip_dst = srcAddress->fHost;
			
			//	if this is a session packet, we need to send our address list back in
			//	response. This will cause the address lists to propagate in the opposite direction
			//	from the enumerate sessions packets
			
			if (addressList->packetType == dp_SESSION_PACKET_ID) {
			//	short		lastSendCount;
			
				//	we first check to see how many addresses we have sent to this address
				//	already. If we don't have any new ones, there is no point in wasting bandwidth
				
			//	lastSendCount = GetSendCount(srcAddress->fHost);
			
			//	if (gAddressList.addressCount > lastSendCount) {
			//		ChangeSendCount(srcAddress->fHost, gAddressList.addressCount);
					tcpabi_udp_send_to_address(srcAddress, &gAddressList, sizeof(AddressListStruct));
			//	}
			}
		
		}
		
	} else {
	
		//	there was no packet to read
		
		theSize = -1;
	}

	return theSize;
}


int tcpabi_udp_send( unsigned handle, void *buf, unsigned len, unsigned ttltos, unsigned id, unsigned char flags ) {
	TUnitData	udata;
	OSStatus	err;
	OTResult	theResult;

	udata.addr.maxlen = sizeof(gDests[handle].dest_addr);
	udata.addr.len = sizeof(gDests[handle].dest_addr);
	udata.addr.buf = (unsigned char *) &gDests[handle].dest_addr;
	
	udata.opt.maxlen = 0;
	udata.opt.len = 0;
	udata.opt.buf = nil;
	
	udata.udata.maxlen = len;
	udata.udata.len = len;
	udata.udata.buf = (unsigned char *)buf;
	
	do {
		err = OTSndUData(gUDPEndpoint, &udata);
		if (err == kOTLookErr) {
			theResult = OTLook(gUDPEndpoint);
			if (theResult == T_UDERR) {
				HandleErrorUDERR();
				err = 666;
			}
		}
	} while (err == 666);

	return err;
}

int tcpabi_udp_send_to_address(InetAddress* theAddress, void *buf, unsigned len) {
	TUnitData	udata;
	OSStatus	err;
	OTResult	theResult;
	
	//	send our list of addresses to the destination address

	udata.addr.maxlen = sizeof(InetAddress);
	udata.addr.len = sizeof(InetAddress);
	udata.addr.buf = (unsigned char *) theAddress;
	
	udata.opt.maxlen = 0;
	udata.opt.len = 0;
	udata.opt.buf = nil;
	
	udata.udata.maxlen = len;
	udata.udata.len = len;
	udata.udata.buf = (unsigned char *)buf;
	
	do {
		err = OTSndUData(gUDPEndpoint, &udata);
		if (err == kOTLookErr) {
			theResult = OTLook(gUDPEndpoint);
			if (theResult == T_UDERR) {
				HandleErrorUDERR();
				err = 666;
			}
		}
	} while (err == 666);
	
	//	add the destination address to our list of addresses
	
	AddAddressToList(theAddress->fHost);

	return err;
}


int tcpabi_udp_status( unsigned handle, unsigned char flags, unsigned *size_next,
                           tcpabi_session_info_t **info ) {
	*info = &gSessionInfo;
}

int tcpabi_udp_broadcast(void *buf, unsigned len) {
	OSStatus			err = noErr;
	TUnitData			udata;
	OTResult			theResult;
	short				i;
	InetAddress			currentAddress;
	
	//	this method sends the specified packet to our list of
	//	IP addreses. We return noErr if they were all sent
	//	without any trouble (we should ignore errors which
	//	indicate that the other end is not there since we
	//	really don't care if all of these packets are delivered)
	
	//	we don't broadcast to the first address because it's our own
	
	for (i = 1; i < gAddressList.addressCount; i++) {
	
		OTInitInetAddress(&currentAddress, SOCKET_MW2, gAddressList.addressList[i]);

		udata.addr.maxlen = sizeof(InetAddress);
		udata.addr.len = sizeof(InetAddress);
		udata.addr.buf = (unsigned char *) &currentAddress;
		
		udata.opt.maxlen = 0;
		udata.opt.len = 0;
		udata.opt.buf = nil;
		
		udata.udata.maxlen = len;
		udata.udata.len = len;
		udata.udata.buf = (unsigned char *)buf;
		
		do {
			err = OTSndUData(gUDPEndpoint, &udata);
			if (err == kOTLookErr) {
				theResult = OTLook(gUDPEndpoint);
				if (theResult == T_UDERR) {
					HandleErrorUDERR();
					err = 666;
				}
			}
		} while (err == 666);
		
	}
	
	return err;
}

void AddAddressToList(InetHost newAddress) {
	short						i;
	Boolean						found = false;
	extern InetInterfaceInfo	gInetInfo;
	
	//	we don't want to add our own address to the address list
	
//	if (newAddress != gInetInfo.fAddress) {
	
		//	check to see if there is room in the list before adding a new address
		
		if (gAddressList.addressCount < kMaxAddressesIP - 1) {
	
			//	add this address to the list if it's not in the list already
			
			for (i = 0; i < gAddressList.addressCount; i++) {
				if (gAddressList.addressList[i] == newAddress) {
					found = true;
					break;
				}
			}
			
			if (!found) {
				gAddressList.addressList[gAddressList.addressCount] = newAddress;
				gAddressList.addressCount++;
			}
		
		}
	
//	}
	
}

void AddAddressListToList(InetHost* newAddressList, short count) {
	short		i;
	
	//	if the list is full, bail out
	
	if (gAddressList.addressCount < kMaxAddressesIP - 1) {
	
		for (i = 0; i < count; i++) {
			AddAddressToList(*newAddressList);	//	add this IP to our list
			newAddressList++;					//	next IP address
		}
		
	}
	
}

void InitAddressList(void) {

	//	clear the address list
	
	memset(&gAddressList, sizeof(AddressListStruct), 0);
//	memset(&gSendCount, sizeof(SendCountStruct), 0);
	
	//	set up the packet type
	
	gAddressList.packetType = 'la';
}

/*
void ChangeSendCount(InetHost theAddress, short newCount) {
	short		i;
	short		index = -1;
	
	//	this routine sets the count for the specified address. If the address
	//	is not in the list, it is added at the end
	
	//	search for this address in the list

	for (i = 0; i < gSendCountCount; i++) {
		if (gSendCount[i].address == theAddress) {
			index = i;
			break;
		}
	}
	
	//	add the address to the list if it isn't in the list and there is room
	
	if (index == -1) {
		if (gSendCountCount < kMaxAddressesIP - 1) {
		
			//	add this address to the list
			
			index = gSendCountCount;
			gSendCountCount++;

			gSendCount[index].address = theAddress;
		
		}	
	}
	
	//	set the count of the current address
	
	if (index != -1) {
		gSendCount[index].lastCountSent = newCount;
	}
	
}

short GetSendCount(InetHost theAddress) {
	short	i;
	short	result = -1;
	short	index = -1;
	
	//	search for this address in the list

	for (i = 0; i < gSendCountCount; i++) {
		if (gSendCount[i].address == theAddress) {
			index = i;
			break;
		}
	}
	
	if (index != -1) {
		result = gSendCount[index].lastCountSent;
	}

	return result;
}
*/

