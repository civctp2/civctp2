/*-----------------------------------------------------------------------
 $Log: MODM.H $
 Revision 1.1  1997/05/30 18:28:59  dkegel
 Initial revision
 Revision 1.4  1996/06/25 18:51:00  dkegel
 Added support for non-standard irq's back in.
 Now checks hwirq and baseadr for validity before using.
 Revision 1.2  1996/06/24 18:23:37  dkegel
 Added hwirq and baseadr parameters to ser_config to allow non-standard
 irq's.
 Revision 1.1  1996/05/07 00:29:32  dkegel
 Initial revision
 Revision 1.2  1996/02/29 01:35:52  dkegel
 Improved documentation.
 Revision 1.1  1996/02/25 04:51:00  dkegel
 Initial revision

 A simple packetizing serial driver.

 Has an 'addressing' mechanism that just serves as a convienience for
 the layer above this, which really was written with networks in mind.
-----------------------------------------------------------------------*/

#ifndef ser_h
#define ser_h 

#include <string.h>
#include <stddef.h>

// Use Greenleaf serial i/o library
#ifdef TEST
#include "simser.h"
#else
//#include "commlib.h"
//#include "modem.h"
#include "GCLHack.h"
#include <Connections.h>
//#include "assoctab.h"

#endif

//#pragma pack(1)

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
#define ser_HDL_NONE -1
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

#pragma options align=packed
typedef struct {
	unsigned char frame0;		// ser_HDR_FRAME0
	unsigned char frame1;		// ser_HDR_FRAME1
	unsigned char bodylen;		// max packet payload length: 255.  Jeepers.
	unsigned char hchksum;		// sum of previous three bytes of header
	unsigned char bodycrc;		// crc of payload
} ser_hdr_t;			// the five-byte header in front of each packet
#pragma options align=reset

//--------------------------------------------------------------------------
// A driver.  A thing which lets us send packets to and from other computers.
// The central type of this module.
//--------------------------------------------------------------------------
#pragma options align=packed
typedef struct {
	// Stuff about the connection to the other computer
	ConnHandle		sConnection;
	PORT				*port;							// returned from Greenleaf
	long				baud;								// usually 19200
	int				portnum;							// hardware comm port; 0 or 1 (or possibly more)
	int				hwirq;
	int				baseadr;

	// Stuff about incoming serial data
	unsigned char	rbuf[ser_READSIZE];			// bytes read from serial port
	int				len;								// number of bytes in rbuf
	int				head;								// index into rbuf; next byte to get out
	// Stuff about the packet being recieved.
	struct {
		ser_hdr_t		hdr;							// header of received packet
		unsigned char	body[ser_USERMAXPACK];	// bytes of recieved packet
	} pkt;
	int				got;								// index into pkt; good bytes in pkt so far
															// Also controls how incoming byte is interpreted.

	// The addressing mechanism for the upper layer's convienience.
	// This is a two entry table of addresses; myAdr is the first entry,
	// yourAdr is the second entry.  The table is conceptually indexed by
	// ser_hdl_t's.  It is managed by ser_adr2hdl and ser_hdl2adr.
	ser_hdl_t		nextHdl;						// next handle to hand out in ser_adr2hdl().
	ser_adr_t		myAdr;						// table[ser_HDL_ME], else ser_ADR_NONE
	ser_adr_t		yourAdr;						// table[ser_HDL_YOU], else ser_ADR_NONE
} ser_t;
#pragma options align=reset

// A result.  Lets you know if a call failed, and why.
typedef int ser_result_t;		// Error/success status type.
#define ser_RES_OK		0
#define ser_RES_FULL	1
#define ser_RES_EMPTY	2
#define ser_RES_BAD		3

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
 Given an ser address, open a handle that can be used to send an
 address to that address.
 Multiple calls with the same arg will return the same result.

 Note: the first call with insert on will assign the address of ser_ADR_ME.
 The second call with insert on will assign the address of ser_ADR_YOU.
 Further calls with insert on are illegal.

 This whole thing is a stupid convenience for the upper layer, which
 wants to be able to associate addresses of some sort with computers.

 Returns ser_HDL_NONE on failure.
-----------------------------------------------------------------------*/
ser_hdl_t ser_adr2hdl(ser_t *ser, ser_adr_t *adr, int insert);

/*-----------------------------------------------------------------------
 Given a baud rate and port number, configure the
 serial system.
 Multiple calls with the same args are allowed, and are harmless.
 port = 0 for com1:, 1 for com2:, etc.
 baud = 19200, 38400, 57600?
 If you want to override the standard baseadr or irq for the port, set
 baseadr or hwirq to the desired value; else set them to zero.
-----------------------------------------------------------------------*/
ser_result_t ser_config(ser_t *ser, long baud, int portnum, int baseadr, int hwirq);

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding ser address.
 Returns ser_RES_EMPTY on failure.
-----------------------------------------------------------------------*/
ser_result_t ser_hdl2adr(ser_t *ser, ser_hdl_t hdl, ser_adr_t *adr);

/*-----------------------------------------------------------------------
 Send a packet via the serial port to the given destination.
 Copies len bytes from buf into internal packet queue.

 Can only send to ser_HDL_YOU, so no destination argument is needed.
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

 Can only recieve from ser_HDL_YOU, so no source adr need be returned.
-----------------------------------------------------------------------*/
ser_result_t ser_get(ser_t *ser, void *buf, size_t *len);

ser_result_t CTB_get(ser_t *ser, void *buf, size_t *len);
ser_result_t CTB_put(ser_t *ser, void *buf, size_t len);

//#pragma pack()
#endif
