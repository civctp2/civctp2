// Simple packetizing serial driver.
//
// $Log: MODM.C $
// Revision 1.1  1997/05/30 18:28:59  dkegel
// Initial revision
// Revision 1.8  1996/06/29 17:55:38  dkegel
// Return ser_RES_FULL status properly.
// Revision 1.7  1996/06/25 18:49:39  dkegel
// Added support for non-standard irq's back in.
// Now checks hwirq and baseadr for validity before using.
// Revision 1.5  1996/06/24 18:23:34  dkegel
// Added hwirq and baseadr parameters to ser_config to allow non-standard
// irq's.
// Revision 1.4  1996/05/10 21:31:32  dkegel
// 1. Clear buffers before PortClose, and surround it with DPRINT's, 
// as part of our quest to find why PortClose crashes sometimes.
// Revision 1.3  1996/05/09 20:27:46  dkegel
// DPRINTs in adr2hdl.
// Revision 1.2  1996/05/09 01:59:55  dkegel
// 1. Lots of DPRINTs.
// 2. Added two pad bytes to make it run at all on 8250.
// (otherwise, it dropped far too many packets.)
// Revision 1.1  1996/05/07 00:29:29  dkegel
// Initial revision
// Revision 1.2  1996/02/29 01:34:19  dkegel
// 1. Fixed uninitialized member bug.
// 2. Fixed ser_hdl2adr stupid bug.
// 3. Added kludge workaround for possible weirdness in gcl510a's WriteBuffer.
// Revision 1.1  1996/02/25 04:50:55  dkegel
// Initial revision

#include <stdlib.h>
#include <stdio.h>

#include "modm.h"
#include "CTBStuff.h"
#include "ddprint.h"

#if 0
	#include "LoadGlobalFrag.h"
#endif

//#include <stdio.h>
//#define DPRINT(p)		(printf p )

/*-----------------------------------------------------------------------
 Create a serial driver.

 Sadly, the upper level needs to have Adresses for each computer.
 You need to call ser_adr2hdl once to set local Adress before
 commPlayerInfo will be able to report the local Adress properly.
 You need to call ser_adr2hdl again to set the remote Adress before
 commPlayerInfo will be able to report properly.

 You need to call ser_config to config the port before you can call
 ser_get.
-----------------------------------------------------------------------*/
ser_t *ser_create(void)
{
	ser_t *ser;
	THz	myZone;

	DPRINT(("ser_create:\n"));

	myZone = GetZone();

	SetZone(SystemZone());

	//ser = malloc(sizeof(ser_t));
	ser = (ser_t*)NewPtrClear(sizeof(ser_t));


	if (!ser) {
		// ABORT! Out of Memory
		SetZone(myZone);
		return NULL;
	}
	ser->port = NULL;
	ser->baud = 0;
	ser->portnum = -1;

	ser->yourAdr = ser_ADR_NONE;	// no connection yet
	ser->myAdr = ser_ADR_NONE;

	ser->nextHdl = ser_HDL_ME;		// first adr2hdl sets my adr
	ser->got = 0;
	ser->head = 0;
	ser->len = 0;

	/* Mac specific fields */
	ser->sConnection = nil;

	DPRINT(("ser_create: done\n"));

	SetZone(myZone);
	return ser;
}

/*-----------------------------------------------------------------------
 Destroy a serial driver.
-----------------------------------------------------------------------*/
void ser_destroy(ser_t *ser)
{
	OSErr	error;
	THz	myZone;

	if (ser != nil) {
		myZone = GetZone();
	
		SetZone(SystemZone());
		if (ser->sConnection != nil) {
			error = CMClose(ser->sConnection, false, nil, 0, true);
			CMDispose(ser->sConnection);
			ser->sConnection = nil;
		}

		free(ser);
		ser = nil;
		
		SetZone(myZone);

		error = SetData(nil);
	}
}


/*-----------------------------------------------------------------------
 Given an ser Adress, open a handle that can be used to send an
 Adress to that Adress.
 Multiple calls with the same arg will return the same result.

 Note: the first call with insert on will assign the Adress of ser_ADR_ME.
 The second call with insert on will assign the Adress of ser_ADR_YOU.
 Further calls with insert on are illegal.

 This whole thing is a stupid convenience for the upper layer, which
 wants to be able to associate Adresses of some sort with computers.

 Returns ser_HDL_NONE on failure.
-----------------------------------------------------------------------*/
ser_hdl_t ser_adr2hdl(ser_t *ser, ser_adr_t *adr, int insert)
{
	if (*adr == ser->myAdr) {
		return ser_HDL_ME;
	}

	if (*adr == ser->yourAdr) {
		return ser_HDL_YOU;
	}

	if (!insert) {
		DPRINT(("ser_adr2hdl(adr %x, insert %d): myAdr %x, yourAdr %x, no match\n", *adr, insert, ser->myAdr, ser->yourAdr));
		return ser_HDL_NONE;
	}

	if (ser->nextHdl == ser_HDL_ME) {
		DPRINT(("ser_adr2hdl: setting myAdr to %x\n", *adr));
		// Caller is setting this station's Adress.
		ser->myAdr = *adr;

		return ser->nextHdl++;
	}
	if (ser->nextHdl == ser_HDL_YOU) {
		DPRINT(("ser_adr2hdl: setting yourAdr to %x\n", *adr));
		// Caller is setting other station's Adress.
		ser->yourAdr = *adr;

		return ser->nextHdl++;
	}
	DPRINT(("ser_adr2hdl(adr %x, insert %d): myAdr %x, yourAdr %x, no match, out of handles\n", *adr, insert, ser->myAdr, ser->yourAdr));

	return ser_HDL_NONE;
}

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding ser Adress.
 Returns ser_RES_EMPTY on failure.
-----------------------------------------------------------------------*/
ser_result_t ser_hdl2adr(ser_t *ser, ser_hdl_t hdl, ser_adr_t *adr)
{
	if (hdl == ser_HDL_ME) {
		*adr = ser->myAdr;
	} else if (hdl == ser_HDL_YOU) {
		*adr = ser->yourAdr;
	} else {
		*adr = ser_ADR_NONE;
	}

	return (*adr != ser_ADR_NONE) ? ser_RES_OK : ser_RES_EMPTY;
}

/*-----------------------------------------------------------------------
 Calculate a CRC checksum of a buffer.
 Duh, I think this works.  Kee gave it to me.  Looks plausible, anyway.
 Is this really better than just adding the bytes together?
-----------------------------------------------------------------------*/
static unsigned char crc(unsigned char *ptr, int len)
{
	short	crc = 0;
	int	i;
	int	j;

	for (i = 0; i < len; i++) {
		crc = crc ^ (int) ptr[i] << 8;

		for (j = 0; j < 8; ++j) {
			if (crc & 0x8000) {
				crc = crc << 1 ^ 0x1021;
			} else {
				crc = crc << 1;
			}
		}
	}
	return crc & 0xff;
}

unsigned char CTB_crc(unsigned char *ptr, int len) {
    short crc = 0;
    int i, j;

    for (i = 0; i < len; i++) {
		crc = crc ^ (int) ptr[i] << 8;

        for (j = 0; j < 8; ++j) {
            if (crc & 0x8000) crc = crc << 1 ^ 0x1021;
            else              crc = crc << 1;
        }
    }
	return crc & 0xff;
}

ser_result_t CTB_put(ser_t *ser, void *buf, size_t len)
{
	ser_hdr_t	hdr;
	ser_hdl_t	dest = ser_HDL_YOU;
	THz			myZone;
	
	//if ((dest != ser_HDL_BROADCAST) && !assoctab_subscript(ser->adrtab, dest)) {
	//	DPRINT(("ser_put: handle %d not in table\n", dest));
	//	return ser_RES_EMPTY;
	//}
	//DPRINT(("ser_put: len: %ld\n", len));
	if (ser->sConnection == NULL) {
		return ser_RES_BAD;
	}

	if (len > 255) {
		return ser_RES_FULL;		// sorry, charlie, we've got our limits
	}

	myZone = GetZone();
	SetZone(SystemZone());

	hdr.frame0 = ser_HDR_FRAME0;
	hdr.frame1 = ser_HDR_FRAME1;
	//hdr.src = ser->myAdr & 255;
	hdr.bodylen = len;
	hdr.hchksum = (ser_HDR_FRAME0 + ser_HDR_FRAME1 + len) & 0xff;
	hdr.bodycrc = CTB_crc(buf, len);
	{
		CMErr		theErr;
		long		nBytes;
		char kludge[1000];
		memcpy(kludge, &hdr, 5);
		memcpy(kludge + 5, buf, len);
		
		nBytes = len + 5;
		theErr = CMWrite(
			ser->sConnection,			//	connection to write to
			kludge,						//	pointer to data
			&nBytes,					//	number of bytes to write
			cmData,						//	write to data part of connection
			false,						//	synchronus
			nil,						//	no callback
			0,							//	no timeout
			0							//	no flags
		);

		if (theErr != noErr) {
			SetZone(myZone);
			return ser_RES_FULL;
		}
	}

	SetZone(myZone);
	return ser_RES_OK;
}


ser_result_t CTB_get(ser_t *ser, void *buf, size_t *len)
{
	
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
	CMFlags	flags;
	THz		myZone;

	//	make sure there is an open connection

	if (ser->sConnection == NULL)
		return ser_RES_BAD;
	
	myZone = GetZone();
	SetZone(SystemZone());

	while (true) {
		int c;
		unsigned char	csum;
	
		//	if we have used up all of the bytes in our recieve buffer, read some more
		
		if (ser->head >= ser->len) {
			long	nBytes = ser_READSIZE;
			CMErr	theErr;
			
			theErr = CMRead(
				ser->sConnection,			//	CTB connection to read from
				&(ser->rbuf[0]),			//	buffer to read into
				&nBytes,					//	in = max bytes to read, out = bytes read
				cmData,						//	read from data channel
				false,						//	synchronus
				nil,						//	completion routine
				0,							//	timeout
				&flags							//	flags
			);
			
			//	if there was an error, it's time to bail
			if (theErr != noErr && theErr != cmTimeOut) {
				SetZone(myZone);
				return ser_RES_BAD;
			}
			
			//	if we didn't read anything, there is nothing to do
	
			if (nBytes <= 0) {
				SetZone(myZone);
				return ser_RES_EMPTY;
			}
			
			//	start processing bytes from the start of the buffer (save
			//	the number of bytes in the buffer so we know when we're done)
			
			ser->head = 0;
			ser->len = nBytes;
		}
		
		//	process the bytes in the buffer
		
		// Continue accumulating bytes of packet.
		
		c = ser->rbuf[ser->head++];
		if (ser->got < 5)		// if still filling header[
			((unsigned char *)&ser->pkt)[ser->got++] = c;
		else	
			ser->pkt.body[ser->got++ - 5] = c;

		// Do header processing.
		if (ser->got == 1) {
			// Does 1st byte of header match?
			if (c != ser_HDR_FRAME0) {
				ser->got = 0;			// no, just start over.
			}
		} else if (ser->got == 2) {
			// Does 2nd byte of header match?
			if (c != ser_HDR_FRAME1) {
				ser->got = 0;			// no, just start over.
			}
		} else if (ser->got == 4) {
			// Is header Checksum valid?
			csum = (ser_HDR_FRAME0 + ser_HDR_FRAME1 + ser->pkt.hdr.bodylen) & 0xff;
			if (((unsigned char) c) != csum) {
				ser->got = 0;			// no, just start over.
			}
		} else if (ser->got >= 5 + ser->pkt.hdr.bodylen) {
			ser->got = 0;
			// Done with packet.  Does crc match?
			if (CTB_crc(ser->pkt.body, ser->pkt.hdr.bodylen) != ser->pkt.hdr.bodycrc) {
				SetZone(myZone);
				return ser_RES_BAD;
			}

			// Is user buffer big enough to fit this packet?
			if (ser->pkt.hdr.bodylen > *len) {
				SetZone(myZone);
				return ser_RES_FULL;	// no, packet too big to fit, fail
			} else {
				//ser_adr_t	adr;
				short			lngth;
				char*			srcPtr;
				
				// Yes.  Copy packet to user, prepare for next packet.
				//memcpy(buf, ser->pkt.body, ser->pkt.hdr.bodylen);
				lngth = ser->pkt.hdr.bodylen;
				srcPtr = ser->pkt.body;
				memcpy(buf, srcPtr, lngth);
				*len = ser->pkt.hdr.bodylen;
				//adr = ser->pkt.hdr.src;	// packet only has low byte of address
				//*src = CTB_adr2hdl(ser, &adr, FALSE, 255);	// so mask all but low byte out

				SetZone(myZone);
				return ser_RES_OK;
			}
		}
	}
}

