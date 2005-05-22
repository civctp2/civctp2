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

#include <string.h>

#include	<OpenTransport.h>
#include	<OpenTptInternet.h>
#include  <Memory.h>

#include	"otq.h"
#include	"otudp.h"

#include "mstcplow.h"

void TCPLow_Create(TCPINSTANCE* pTcp, u_short* port, TCPPEER* peer, int* status)
{
	TBind boundAddr;
	InetAddress boundInetAddr;
	InetInterfaceInfo inetInfo;
	OSStatus err;

	pTcp->isValid = false;
		
	//	Initialize OpenTransport

	err = InitOpenTransport();
	if (err != noErr) {
		*status = comm_STATUS_BAD;
		return;
	}
	
	//	initialize Internet Services

	pTcp->inetService = OTOpenInternetServices(kDefaultInternetServicesPath, 0, &err);
	if (err != noErr) {
		*status = comm_STATUS_BAD;
		return;
	}
	
	//	open an endpoint for sending and recieving data

	err = CreateAndConfigUDP(&pTcp->udpEndpoint);
	if (err != noErr) {
		ShutDownUDP(pTcp->udpEndpoint);
		*status = comm_STATUS_BAD;
		return;
	}
	pTcp->isValid = true;
			
	//	Get information about Internet
	
	err = OTInetGetInterfaceInfo(&inetInfo, kDefaultInetInterface);
	if (err != noErr) {
		//DebugStr("\pCannot Get Information About Default Interface");
		*status = comm_STATUS_BAD;
		ShutDownUDP(pTcp->udpEndpoint);
		return;
	}

	boundAddr.addr.maxlen = sizeof(boundInetAddr);
	boundAddr.addr.buf = (unsigned char*) &boundInetAddr;;
	err = OTGetProtAddress(pTcp->udpEndpoint,&boundAddr,NULL);
	if (err != noErr) {
		*status = comm_STATUS_BAD;
		ShutDownUDP(pTcp->udpEndpoint);
		return;
	}
	
	peer->addr = inetInfo.fAddress;
	peer->port = *port = boundInetAddr.fPort;
	pTcp->myHandle = dcstReplace(pTcp->handles,pTcp->myHandle,peer);
}

void TCPLow_Destroy(TCPINSTANCE* pTcp)
{
	if (pTcp->isValid)
		ShutDownUDP(pTcp->udpEndpoint);
	pTcp->isValid = false;
}

int TCPLow_PutPacket(TCPINSTANCE* tcp, TCPPEER* pPeer,void* bufptr,ULONG len)
{
	TUnitData	udata;
	OSStatus	err;
	OTResult	theResult;
	InetAddress	destAddr;
	unsigned char *pt;
	
	OTInitInetAddress(&destAddr,pPeer->port,pPeer->addr);
	
	udata.addr.maxlen = sizeof(InetAddress);
	udata.addr.len = sizeof(InetAddress);
	udata.addr.buf = (unsigned char *) &destAddr;
	
	udata.opt.maxlen = 0;
	udata.opt.len = 0;
	udata.opt.buf = nil;
	
	udata.udata.maxlen = len;
	udata.udata.len = len;
	udata.udata.buf = (unsigned char *)bufptr;
	
	do {
		err = OTSndUData(tcp->udpEndpoint, &udata);
		if (err == kOTLookErr) {
			theResult = OTLook(tcp->udpEndpoint);
			if (theResult == T_UDERR) {
				HandleErrorUDERR();
				err = 666;
			}
		}
	} while (err == 666);

	return err;
}

int TCPLow_GetPacket(TCPINSTANCE* tcp,void* bufptr,ULONG* pLen,TCPPEER* addr)
{
	//	this method reads a packet and returns the
	//	size of the data read and sets up the senders
	//	address for the status routine
				   
	unsigned char		recvBuf[sizeof(InetAddress) + udpMaxRawData + 1];
	InetAddress*		srcAddr;
	unsigned long		theSize = udpMaxRawData;
	Boolean				gotPacket;
	extern otq_t*		gInQueue;

	gotPacket = otq_get(gInQueue, recvBuf, &theSize);
	if (gotPacket) {
		theSize -= sizeof(InetAddress);
		memcpy(bufptr, recvBuf + sizeof(InetAddress), theSize);
		srcAddr = (InetAddress*) recvBuf;
		addr->addr = srcAddr->fHost;
		addr->port = srcAddr->fPort;
		*pLen = theSize;
	} else
		return TCP_RES_EMPTY;
		
	return TCP_RES_OK;
}
