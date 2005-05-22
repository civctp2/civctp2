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
 Packet-oriented serial i/o driver.
 Used for both null modem and modem sessions.

 $Log: ser.h $
 Revision 1.2  1997/09/07 20:12:26  dkegel
 Compiles standalone now.  Also defined ser_RES_BUG.
 Revision 1.1  1997/03/10 05:09:04  dkegel
 Initial revision
--------------------------------------------------------------------------*/
#ifndef ser_h
#define ser_h 

#include <string.h>
#include <stddef.h>
#include "serio.h"

#pragma pack(1)

/*--------------------------------------------------------------------------
 You wouldn't think a serial driver needs a concept of address, would you?
 Unfortunately, the dp layer which uses this serial driver expects to be
 able to put an address in a packet, send the packet to another machine,
 and have the address mean the same thing on the other machine.
 This means each computer needs its own 'address'.
 Furthermore, the dp layer expects the address to not change from session
 to session.  
--------------------------------------------------------------------------*/
typedef long ser_adr_t;
#define ser_ADR_NONE 0			// Used internally to indicate no connection.

// A peer handle.  A short and non-portable notation for the address of
// an open connection.
typedef int ser_hdl_t;			// Handles used in sending packets.
#define ser_HDL_NONE		-1
#define ser_HDL_BROADCAST	0	// For this driver, same as ser_HDL_YOU.
#define ser_HDL_ME			1	// The 2nd entry is for my own address.
#define ser_HDL_YOU			2	// The 3rd entry is the other end of the wire.

#define ser_USERMAXPACK		255

#define ser_READSIZE		4096

/*-----------------------------------------------------------------------
 And now: the packetizing part!

 When ser_put is called it sends a ser_hdr_t down the wire, then the
 bode of the packet (i.e. the bytes passed to ser_put).
 Ser_get looks for a ser_hdr_t; when it finds a good one, it then reads
 the number of bytes indicated by the ser_hdr_t.  If the CRC matches,
 the bytes are then returned to the caller.

 Packets which include the ser_HDR_FRAME0 char are very unlikely to
 screw the works up, since only if all four bytes match is a header
 considered found.  Therefore no escaping of data inside the packet is done.
-----------------------------------------------------------------------*/
#define ser_HDR_FRAME0 'F'		// First byte of every packet header
#define ser_HDR_FRAME1 'R'		// Second byte of every packet header
typedef struct {
	unsigned char frame0;		// ser_HDR_FRAME0
	unsigned char frame1;		// ser_HDR_FRAME1
	unsigned char bodylen;		// max packet payload length: 255.  Jeepers.
	unsigned char hchksum;		// sum of previous three bytes of header
	unsigned char bodycrc;		// crc of payload
} ser_hdr_t;			// the six-byte header in front of each packet

//--------------------------------------------------------------------------
// A driver.  A thing which lets us send packets to and from other computers.
// The central type of this module.
//--------------------------------------------------------------------------
typedef struct {
	// Stuff about the connection to the other computer
	serio_t serio;

	// The addressing mechanism for the upper layer's convienience.
	// This is a two entry table of addresses; myAdr is the first entry,
	// yourAdr is the second entry.  The table is conceptually indexed by
	// ser_hdl_t's.  It is managed by ser_adr2hdl and ser_hdl2adr.
	ser_hdl_t nextHdl;			// next handle to hand out in ser_adr2hdl().
	ser_adr_t myAdr;			// table[ser_HDL_ME], else ser_ADR_NONE
	ser_adr_t yourAdr;			// table[ser_HDL_ME], else ser_ADR_NONE

	// Stuff about incoming serial data
	unsigned char rbuf[ser_READSIZE];	// bytes read from serial port
	int sigpos;					// used in checking for modem offline signal

#if 0
	DWORD length;
	DWORD end;
	DWORD start;
	BYTE state;
	DWORD rbufsize;
	unsigned char crc;
	BOOL empty;
#else
	// Stuff about incoming serial data
	int len;					// number of bytes in rbuf
	int head;					// index into rbuf; next byte to get out
	
	// The packet being recieved.
	struct {
		ser_hdr_t hdr;			// header of received packet
		unsigned char body[ser_USERMAXPACK];	// bytes of recieved packet
	} pkt;
	
	// 'got' is the number of bytes of packet received and recognized so far.
	// 0 <= got < sizeof(pkt)
	int got;					

#endif
} ser_t;

// A result.  Lets you know if a call failed, and why.
typedef int ser_result_t;		// Error/success status type.
#include "commerr.h"
#define ser_RES_OK		comm_STATUS_OK
#define ser_RES_FULL	comm_STATUS_FULL
#define ser_RES_EMPTY	comm_STATUS_EMPTY
#define ser_RES_BAD		comm_STATUS_BAD
#define ser_RES_BUG		comm_STATUS_BUG
#define ser_RES_NO_RESPONSE  comm_STATUS_NO_RESPONSE

/*-----------------------------------------------------------------------
 Create a serial driver.
 Returns NULL on failure.
-----------------------------------------------------------------------*/
ser_t *ser_create(void);

/*-----------------------------------------------------------------------
 Destroy a serial driver.
-----------------------------------------------------------------------*/
void ser_destroy(ser_t *ser);

/*-----------------------------------------------------------------------
 Given an ser Adress, open a handle that can be used to send an
 Adress to that Adress.
 Multiple calls with the same arg will return the same result.

 Note: the first call with insert on will assign the Adress of ser_ADR_ME.
 The second call with insert on will assign the Adress of ser_ADR_YOU.
 Further calls with insert on are illegal.

 adrMask should be set to -1 unless you only want to compare part of the
 address!

 This whole thing is a stupid convenience for the upper layer, which
 wants to be able to associate Adresses of some sort with computers.

 Returns ser_HDL_NONE on failure.
-----------------------------------------------------------------------*/
ser_hdl_t ser_adr2hdl(ser_t *ser, ser_adr_t *adr, int insert);

/*-----------------------------------------------------------------------
 Given a baud rate and port number, configure the
 serial system.
 Multiple calls with the same args are allowed, and are harmless.
 portname = "com1", "com2", etc.
 baud = 19200, 38400, 57600?
-----------------------------------------------------------------------*/
ser_result_t ser_config(ser_t *ser, long baud, const char * portname);

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding ser address.
 Returns ser_RES_EMPTY on failure.
-----------------------------------------------------------------------*/
ser_result_t ser_hdl2adr(ser_t *ser, ser_hdl_t hdl, ser_adr_t *adr);

/*-----------------------------------------------------------------------
 Send a packet via the serial port to the given destination.
 Copies len bytes from buf into internal packet queue.
-----------------------------------------------------------------------*/
ser_result_t ser_put(ser_t *ser, void *buf, size_t len);

/*-----------------------------------------------------------------------
 Receive a packet via the serial port.
 Caller must put size of buffer in len before calling.
 Packets larger than buffer will be silently ignored.
 On failure, returns
 	ser_RES_BAD if *len looked bogus, or buf was NULL,
				 or packet was too big to fit in buffer, or 0 size.
	ser_RES_EMPTY if no good packet was availible.
 On success,
 	returns ser_RES_OK,
	sets *len to the packet length,

 Strategy:
   Read ser_READSIZE bytes at a time from the serial port into ser->rbuf.
   ser->len is number of bytes in ser->rbuf.
   ser->head is the index of the next byte to get out of ser->rbuf.
   Accumulate next good packet in ser->pkt.
   ser->got is the number of good bytes gotten so far.
   If anything goes wrong, just clear ser->got, and life keeps on humming.
-----------------------------------------------------------------------*/
ser_result_t ser_get(ser_t *ser, void *buf, size_t *len);

#pragma pack()
#endif
