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

#ifndef _CommCTB_
#define _CommCTB_

#include	<Connections.h>

#define	kSERIALDRIVERNAME	"Serial\0"

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
#define ser_HDL_NONE 		-1
#define ser_HDL_BROADCAST	0	// For this driver, same as ser_HDL_YOU.
#define ser_HDL_ME			1	// The 2nd entry is for my own address.
#define ser_HDL_YOU			2	// The 3rd entry is the other end of the wire.

#define ser_USERMAXPACK		255

// The best number of bytes to read at a time is about half
// the number of bytes you expect to recieve each display cycle.
// We expect to get about 200 bytes per other player per frame.
#define ser_READSIZE		128	// how many bytes to read at a time

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
	// MAD unsigned char src;			// low byte of sender's address.
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
	ConnHandle sConnection;
	long baud;					// MAD forced to 38400 for PC compatibility
	int portnum;				// hardware comm port; 0 or 1 (or possibly more)
	int hwirq;
	int baseadr;

	// Stuff about incoming serial data
	unsigned char rbuf[ser_READSIZE];	// bytes read from serial port
	int len;					// number of bytes in rbuf
	int head;					// index into rbuf; next byte to get out
	// Stuff about the packet being recieved.
	struct {
		ser_hdr_t hdr;			// header of received packet
		unsigned char body[ser_USERMAXPACK];	// bytes of recieved packet
	} pkt;
	int got;					// index into pkt; good bytes in pkt so far
								// Also controls how incoming byte is interpreted.

	// The addressing mechanism for the upper layer's convienience.
	// This is a two entry table of addresses; myAdr is the first entry,
	// yourAdr is the second entry.  The table is conceptually indexed by
	// ser_hdl_t's.  It is managed by ser_adr2hdl and ser_hdl2adr.
	ser_hdl_t nextHdl;			// next handle to hand out in ser_adr2hdl().
	ser_adr_t myAdr;			// table[ser_HDL_ME], else ser_ADR_NONE
	ser_adr_t yourAdr;			// MAD, copied from PC ser.h
// MAD	assoctab_t *adrtab;			// indexed by handle, holds ser_adr_t.
} ser_t;

// A result.  Lets you know if a call failed, and why.

// MAD -- shouldn't these match with dp_RES_xxxxx  values in anet.h ?

typedef int ser_result_t;		// Error/success status type.
#define ser_RES_OK		0
#define ser_RES_EMPTY	2
#define ser_RES_FULL	3
#define ser_RES_BAD		5

#if 0
void CTB_Load(void);
#endif

ser_result_t CTB_hdl2adr(ser_t *ser, ser_hdl_t hdl, ser_adr_t *adr);
ser_t *CTB_create(void);
ser_result_t CTB_get(ser_t *ser, void *buf, size_t *len);
unsigned char CTB_crc(unsigned char *ptr, int len);
ser_hdl_t CTB_adr2hdl(ser_t *ser, ser_adr_t *adr, int insert);
ser_result_t CTB_hdlDestroy(ser_t *ser, ser_hdl_t hdl);
ser_result_t CTB_put(ser_t *ser, ser_hdl_t dest, void *buf, size_t len);




#endif
