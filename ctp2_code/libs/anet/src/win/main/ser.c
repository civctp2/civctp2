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

 $Log: ser.c $
 Revision 1.5  1997/10/03 00:44:05  anitalee
 zeroed Security Attributes in ser_config and added some DPRINT's
 Revision 1.4  1997/09/27 01:42:35  dkegel
 Should allow freeze/thaw of modem sessions, as long as thaw-er is child of freeze-r.
 Revision 1.3  1997/09/07 20:12:14  dkegel
 Added DPRINT's.
 Revision 1.2  1997/04/02 00:51:54  dkegel
 Force baud rates to those supported by standard comm ports!
 Otherwise users who enter "14400" are in for a nasty suprise.
 Revision 1.1  1997/03/10 05:09:02  dkegel
 Initial revision
--------------------------------------------------------------------------*/

/*************************************************************************************

                            Win32 Driver for Serial Protocol

                              (c) 1997 By Activision

**************************************************************************************/

/*************************************************************************************

                                    Literals

**************************************************************************************/

#include <windows.h>
#include <assert.h>
#include <crtdbg.h>
#include <stdlib.h>
#include <stdio.h>

#include "ser.h"
#include "dpmem.h"

#if 1
#define DEBUG_MODULE 1
#include "ddprint.h"
#else
#define DPRINT(s) printf s
#endif

/*************************************************************************************

                                    Globals

**************************************************************************************/

// modem signal when is no longer connected (translates to \r\nNO CARRIER\r\n)
const unsigned char SIGNAL_HANGUP[] =
							{0x0d, 0x0a, 0x4e, 0x4f, 0x20, 0x43, 0x41, 0x52,
                             0x52, 0x49, 0x45, 0x52, 0x0d, 0x0a};
#if 0
// States for reading
#define READY_TO_READ 0
#define WAITING_ON_READ 1

// States for buffer processing during reading
#define LOOKING_FOR_HDR 0
#define LOOKING_FOR_BODY 1

#define RBUF_CRC(x) (((x)->start + (x)->length <= (x)->rbufsize) \
	? crc(&(x)->rbuf[(x)->start], (x)->length, 0, 0) \
	: crc(&(x)->rbuf[(x)->start], (x)->rbufsize - (x)->start, \
		(x)->rbuf, (x)->length - ((x)->rbufsize - (x)->start)))

#define RBUF_DATA(x)	((x)->end <= (x)->start && !(x)->empty ? (x)->rbufsize - (x)->start + (x)->end \
						: (x)->end - (x)->start)
#define RBUF_EMPTY(x)	((x)->end <= (x)->start && !(x)->empty ? (x)->start - (x)->end \
						: (x)->rbufsize - (x)->end)
#define RBUF_INCREMENT_END(x, y)	{ (x)->end = ((x)->end  + y) % (x)->rbufsize ; \
									  (x)->empty = (x)->empty && y <= 0; }
#define RBUF_INCREMENT_START(x, y)	{ (x)->start = ((x)->start  + y) % (x)->rbufsize; \
									  (x)->empty = ((x)->start == (x)->end); }
#define RBUF_OFFSET(x, y) (((x)->start + (y)) % (x)->rbufsize)
#endif

/* convert a serio_res_t to a  ser_result_t */
/* KLUDGE: relies on both being the same as comm_error_t! */
#define CONVERT_ERR(e) (ser_result_t)e

#if 0
/*-----------------------------------------------------------------------
 Debugging stuff.
-----------------------------------------------------------------------*/
void printbytes(void *buf, char *msg, long len)
{
	int i;
	if (!buf) return;
	if (msg) DPRINT(("%s ", msg));
	for (i = 0; i < len; i++)
		DPRINT(("%02x ", ((char *)buf)[i] & 0xff));
	DPRINT(("\n"));
}
#endif

/*************************************************************************************

                                  ser_create()

**************************************************************************************/

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

  ser = dp_MALLOC(sizeof(ser_t));
  if (!ser)
    return NULL;

  ser->myAdr   = ser_ADR_NONE;
  ser->yourAdr = ser_ADR_NONE;  // no connection yet

  ser->nextHdl = ser_HDL_ME;    // first adr2hdl sets my adr

#if 0
  ser->start = 0;
  ser->end = 0;
  ser->length = 0;
  ser->rbufsize = ser_READSIZE;
  ser->sigpos = 0;
  ser->state = LOOKING_FOR_HDR;
  ser->empty = TRUE;
#else
	ser->got = 0;
	ser->head = 0;
	ser->len = 0;
  	ser->sigpos = 0;
#endif

  return ser;
}

/*************************************************************************************

                                  ser_config()

**************************************************************************************/

/*-----------------------------------------------------------------------
 Given a baud rate and port number, configure the
 serial system.
 Multiple calls with the same args are allowed, and are harmless.
 portname = "com1", "com2", etc.
 baud = 19200, 38400, 57600?
-----------------------------------------------------------------------*/
ser_result_t ser_config(ser_t *ser, long baud, const char * szPort)
{
	serio_res_t	    err;

	err = serio_open(&ser->serio, baud, szPort);
	if (err == serio_RES_OK)
		return ser_RES_OK;
	DPRINT(("ser_config: error in serio_open %d\n", err));
	return CONVERT_ERR(err);
}

/*************************************************************************************

                                  ser_destroy()

**************************************************************************************/

/*-----------------------------------------------------------------------
 Destroy a serial driver.
-----------------------------------------------------------------------*/
void ser_destroy(ser_t *ser)
{
	if (ser) {
		serio_close(&ser->serio);
		dp_FREE(ser);
	}
}

/*************************************************************************************

                                  ser_adr2hdl()

**************************************************************************************/

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

ser_hdl_t ser_adr2hdl(ser_t *ser, ser_adr_t *adr, int insert)

{
  if (*adr == ser->myAdr  ) return ser_HDL_ME;
  if (*adr == ser->yourAdr) return ser_HDL_YOU;

  if (!insert)
  {
    return ser_HDL_NONE;
  }

  if (ser->nextHdl == ser_HDL_ME)
  {
    // Caller is setting this station's Adress.
    ser->myAdr = *adr;
    return ser->nextHdl++;
  }

  if (ser->nextHdl == ser_HDL_YOU)
  {
    // Caller is setting other station's Adress.
    ser->yourAdr = *adr;
    return ser->nextHdl++;
  }

  return ser_HDL_NONE;
}

/*************************************************************************************

                                  ser_hdl2adr()

**************************************************************************************/

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding ser Adress.
 Returns ser_RES_EMPTY on failure.
-----------------------------------------------------------------------*/
ser_result_t ser_hdl2adr(ser_t *ser, ser_hdl_t hdl, ser_adr_t *adr)

{
	if (hdl == ser_HDL_ME)
		*adr = ser->myAdr;
	else if (hdl == ser_HDL_YOU)
		*adr = ser->yourAdr;
	else
		*adr = ser_ADR_NONE;

	return (*adr != ser_ADR_NONE) ? ser_RES_OK : ser_RES_EMPTY;
}

/*************************************************************************************

                              ser_hdlDestroy()

**************************************************************************************/

/*-----------------------------------------------------------------------
 Destroy a handle.
 Returns ser_RES_BAD on failure.
-----------------------------------------------------------------------*/
ser_result_t ser_hdlDestroy(ser_t *ser, ser_hdl_t hdl)

{
    return ser_RES_OK;
}

/*************************************************************************************

                                     crc()

**************************************************************************************/

/*-----------------------------------------------------------------------
 Calculate a CRC checksum of a buffer.
 Duh, I think this works.  Kee gave it to me.  Looks plausible, anyway.
 Is this really better than just adding the bytes together?
-----------------------------------------------------------------------*/
static unsigned char crc(unsigned char *ptr1, int len1)
{
	short crc = 0;
	int i, j;

	for (i = 0; i < len1; i++)
	{
		crc = crc ^ (int) ptr1[i] << 8;

		for (j = 0; j < 8; ++j)
		{
			if (crc & 0x8000) crc = crc << 1 ^ 0x1021;  else  crc = crc << 1;
		}
	}

	return crc & 0xff;
}

/*************************************************************************************

                                   ser_put()

**************************************************************************************/

/*-----------------------------------------------------------------------
 Send a packet via the serial port to the given destination.
 Copies len bytes from buf into internal packet queue.
-----------------------------------------------------------------------*/

ser_result_t ser_put(ser_t *ser, void *buf, size_t len)

{
	ser_hdr_t hdr;
	serio_res_t err;

	if (!ser) {
		DPRINT(("ser_put: null ser\n"));
		return ser_RES_BUG;
	}
	if (!buf) {
		DPRINT(("ser_put: null buf\n"));
		return ser_RES_BAD;
	}

	if (len > 255)
		return ser_RES_FULL;

	hdr.frame0 = ser_HDR_FRAME0;
	hdr.frame1 = ser_HDR_FRAME1;
	hdr.bodylen = len;
	hdr.hchksum = (ser_HDR_FRAME0 + ser_HDR_FRAME1 + len) & 0xff;
	hdr.bodycrc = crc(buf, len);
	err = serio_write(&ser->serio, &hdr, sizeof(hdr));
	if (err != serio_RES_OK) {
		DPRINT(("ser_put: serio_write header failed %d\n", err));
		return CONVERT_ERR(err);
	}
	/* printbytes((void *)(&hdr), "ser_put:hdr:", sizeof(hdr)); */
	err = serio_write(&ser->serio, buf, len);
	if (err != serio_RES_OK) {
		DPRINT(("ser_put: serio_write body failed %d\n", err));
		return CONVERT_ERR(err);
	}
	/* printbytes(buf, "ser_put:buf:", len); */
	DPRINT(("ser_put: bodycrc %4x, len %d, first 2 bytes %02x %02x, last 5 bytes %02x %02x %02x %02x %02x\n",
		hdr.bodycrc,
		hdr.bodylen,
		((unsigned char *)buf)[0],
		((unsigned char *)buf)[1],
		((unsigned char *)buf)[len-5],
		((unsigned char *)buf)[len-4],
		((unsigned char *)buf)[len-3],
		((unsigned char *)buf)[len-2],
		((unsigned char *)buf)[len-1]));
	return ser_RES_OK;
}

/*************************************************************************************

                                   ser_get()

**************************************************************************************/

#if 0
/*-----------------------------------------------------------------------
 Receive a packet via the serial port.
 Caller must put size of buffer in len before calling.
 Packets larger than buffer will be held until a buffer that is big enough is passed in
 On failure, returns
  ser_RES_EMPTY if no good packet was availible.
  ser_RES_FULL if len is not big enough.  Also sets *len to the required packet length.
 On success,
  returns ser_RES_OK,
  sets *len to the packet length,

 Strategy:
   Read whatever is in the queue into ser->rbuf or until ser->rbuf is full.
   ser->start points to the start of interesting data in ser->rbuf.
   ser->end points to the insert point for new data.
   ser->rbufsize is the size of ser->rbuf.
   ser->read_state is either READY_TO_READ or WAITING_ON_READ
   ser->state is either LOOKING_FOR_HDR_FRAME0, LOOKING_FOR_HDR_FRAME1, LOOKING_FOR_HDR_REST,
   or LOOKING_FOR_BODY
   ser->length is number of bytes in the message.
-----------------------------------------------------------------------*/

ser_result_t ser_get(ser_t *ser, void *buf, size_t *len)
{
	DWORD	dwErrorFlags;
	DWORD	dwReadLength;
	BOOL	bContinue = TRUE;
	DWORD	start_tmp;

	while (bContinue) {
		if (LOOKING_FOR_HDR == ser->state) {
			if (RBUF_DATA(ser) >= sizeof(ser_hdr_t)) {
				/* printbytes((void *)(&(ser->rbuf[ser->start])), "ser_get:hdr:", sizeof(ser_hdr_t)); */
				if (ser_HDR_FRAME0 != ser->rbuf[ser->start]) {
					RBUF_INCREMENT_START(ser, 1); continue;
				}
				RBUF_INCREMENT_START(ser, 1);
				if (ser_HDR_FRAME1 != ser->rbuf[ser->start]) {
					RBUF_INCREMENT_START(ser, 1); continue;
				}
				RBUF_INCREMENT_START(ser, 1);
				ser->length = (ser->rbuf[RBUF_OFFSET(ser, 0)] & 0xff);
				if (((ser_HDR_FRAME0 + ser_HDR_FRAME1 + ser->length) & 0xff) != ser->rbuf[RBUF_OFFSET(ser, 1)]) {
					/* DPRINT(("ser_get:got ser->rbuf[RBUF_OFFSET(ser,1)] incorrect\n")); */
					continue;
				}
				ser->crc = ser->rbuf[RBUF_OFFSET(ser, 2)];
				ser->state = LOOKING_FOR_BODY;
			}
			else {
				/* DPRINT(("ser_get: RBUF_DATA len:%d is smaller than %d\n", RBUF_DATA(ser), sizeof(ser_hdr_t))); */
				bContinue = FALSE;
			}
		}
		if (LOOKING_FOR_BODY == ser->state) {
			if (RBUF_DATA(ser) >= ser->length + 3) {
				start_tmp = ser->start; RBUF_INCREMENT_START(ser, 3);
				if (RBUF_CRC(ser) == ser->crc) {
					if (ser->length > *len) {
						/* DPRINT(("ser_get: ser->length=%d > *len=%d\n", ser->length, *len)); */
						*len = (ser->length & 0xff); return ser_RES_FULL;
					} else {
						*len = (ser->length & 0xff);
						if (ser->start + ser->length <= ser->rbufsize) {
							memcpy(buf, ser->rbuf + ser->start, ser->length);
						} else {
							memcpy(buf, ser->rbuf + ser->start, ser->rbufsize - ser->start);
							memcpy((char *) buf + ser->rbufsize - ser->start, ser->rbuf, ser->length - (ser->rbufsize - ser->start));
						}
						/* printbytes(buf, "ser_get:buf:", ser->length); */
						RBUF_INCREMENT_START(ser, ser->length);
						ser->state = LOOKING_FOR_HDR;
						return ser_RES_OK;
					}
				} else {
					ser->start = start_tmp;
					ser->state = LOOKING_FOR_HDR; continue;
				}
			} else {
				/* DPRINT(("ser_get: RBUF_DATA len:%d is smaller than ser->len+3=%d\n", RBUF_DATA(ser), ser->length + 3)); */
				bContinue = FALSE;
			}
		}
		dwReadLength = RBUF_EMPTY(ser);
		if (dwReadLength > 0) {
			serio_res_t err;
			err = serio_read(&ser->serio, &ser->rbuf[ser->end], dwReadLength, &dwReadLength);
			if ((err != serio_RES_OK) && (err != serio_RES_EMPTY)) {
				DPRINT(("ser_get: serio_read failed %d\n", err));
			} else {
				/* check if got a modem not connected signal */
				{	DWORD i;
					int pos;
					unsigned char *p;
					pos = ser->sigpos;
					for (i = 1, p = &(ser->rbuf[ser->end]); i <= dwReadLength; i++, p++) {
						if (*p == SIGNAL_HANGUP[pos])  {
							pos++;
							if (pos >= sizeof(SIGNAL_HANGUP))
								return ser_RES_NO_RESPONSE;
						} else if (pos) {
							pos = 0;
							if (*p == SIGNAL_HANGUP[pos])
								pos++;
						}
					}
					ser->sigpos = pos;
				}
#if 0
				DPRINT(("ser_get: read %d bytes\n", dwReadLength));
				printbytes((void *)(&(ser->rbuf[ser->end])), "ser_get:readbuf:", dwReadLength);
#endif
				RBUF_INCREMENT_END(ser, dwReadLength); bContinue = TRUE;
			}
		}
	}
	return ser_RES_EMPTY;
}
#else

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

 CRC errors cause ser_RES_EMPTY rather than ser_RES_BAD, since
 ser_RES_BAD is reserved for caller brain damage errors.

 Strategy:
   Read ser_READSIZE bytes at a time from the serial port into ser->rbuf.
   ser->len is number of bytes in ser->rbuf.
   ser->head is the index of the next byte to get out of ser->rbuf.
   Accumulate next good packet in ser->pkt.
   ser->got is the number of good bytes gotten so far.
   If anything goes wrong, just clear ser->got, and life keeps on humming.
-----------------------------------------------------------------------*/
ser_result_t ser_get(ser_t *ser, void *buf, size_t *len)
{
    for (;;) {
		int c;
		assert((0 <= ser->got) && (ser->got < sizeof(ser->pkt)));

		//DPRINT(("ser_get: top: got = %d\n", ser->got));
		// Fill small input buffer if empty.
		if (ser->head >= ser->len) {
			size_t n_received;
			serio_res_t err;
			//DPRINT(("ser_get: calling serio_read\n"));
			err = serio_read(&ser->serio, ser->rbuf, ser_READSIZE, &n_received);
			if ((err != serio_RES_OK) && (err != serio_RES_EMPTY)) {
				DPRINT(("ser_get: serio_read failed %d\n", err));
			} else if ((n_received == 0) || (err == serio_RES_EMPTY)) {
				DPRINT(("ser_get: returning EMPTY\n"));
				return ser_RES_EMPTY;
			}
			ser->head = 0;
			ser->len = n_received;
			/* check if got a modem not connected signal */
			{	unsigned char *p = &ser->rbuf[0];
				int pos = ser->sigpos;
				for ( ; n_received--; p++) {
					if (*p == SIGNAL_HANGUP[pos])  {
						DPRINT(("ser_get: matched SH[%d]=%c; pos now %d\n", pos, *p, pos+1));
						pos++;
						if (pos >= sizeof(SIGNAL_HANGUP)) {
							DPRINT(("ser_get: returning NO_RESPONSE\n"));
							return ser_RES_NO_RESPONSE;
						}
					} else if (pos) {
						DPRINT(("ser_get: cleared pos\n"));
						pos = 0;
						if (*p == SIGNAL_HANGUP[pos]) {
							DPRINT(("ser_get: matched SH[%d]=%c; pos now %d\n", pos, *p, pos+1));
							pos++;
						}
					}
				}
				ser->sigpos = pos;
			}
		}
		// Continue accumulating bytes of packet.
		c = ser->rbuf[ser->head++];
		((unsigned char *)&ser->pkt)[ser->got++] = c;

		DPRINT(("ser_get: got char 0x%x, got now %d\n", c, ser->got));
		// Do header processing.
		if        (ser->got == 1) {
			// Does 1st byte of header match?
			if (c != ser_HDR_FRAME0)
				ser->got = 0;			// no, just start over.
		} else if (ser->got == 2) {
			// Does 2nd byte of header match?
			if (c != ser_HDR_FRAME1)
				ser->got = 0;			// no, just start over.
		} else if (ser->got == 4) {
			// Is header Checksum valid?
			if (c != ((ser_HDR_FRAME0 + ser_HDR_FRAME1 + ser->pkt.hdr.bodylen) & 0xff))
				ser->got = 0;			// no, just start over.
		} else if ((size_t)(ser->got) >= sizeof(ser_hdr_t) + ser->pkt.hdr.bodylen) {
			int c = crc(ser->pkt.body, ser->pkt.hdr.bodylen);
			ser->got = 0;
			// Done with packet.  Does crc match?
			DPRINT(("ser_get: bodycrc %4x, len %d, first 2 bytes %02x %02x, last 5 bytes %02x %02x %02x %02x %02x\n",
				ser->pkt.hdr.bodycrc,
				ser->pkt.hdr.bodylen,
				ser->pkt.body[0],
				ser->pkt.body[1],
				ser->pkt.body[ser->pkt.hdr.bodylen-5],
				ser->pkt.body[ser->pkt.hdr.bodylen-4],
				ser->pkt.body[ser->pkt.hdr.bodylen-3],
				ser->pkt.body[ser->pkt.hdr.bodylen-2],
				ser->pkt.body[ser->pkt.hdr.bodylen-1]));
			if (c != ser->pkt.hdr.bodycrc) {
				DPRINT(("ser_get: returning EMPTY; bad crc. last 4 bytes %02x %02x %02x %02x\n",
					ser->pkt.body[ser->pkt.hdr.bodylen-4],
					ser->pkt.body[ser->pkt.hdr.bodylen-3],
					ser->pkt.body[ser->pkt.hdr.bodylen-2],
					ser->pkt.body[ser->pkt.hdr.bodylen-1]));
				return ser_RES_EMPTY;
			}
			// Is user buffer big enough to fit this packet?
			if (ser->pkt.hdr.bodylen > *len) {
				DPRINT(("ser_get: returning FULL\n"));
				return ser_RES_FULL;	// no, packet too big to fit, fail
			} else {
				// Yes.  Copy packet to user, prepare for next packet.
				memcpy(buf, ser->pkt.body, ser->pkt.hdr.bodylen);
				*len = ser->pkt.hdr.bodylen;
				DPRINT(("ser_get: returning OK\n"));
				return ser_RES_OK;
			}
		}
	}
	DPRINT(("ser_get: bug: unreachable code reached\n"));
	return ser_RES_EMPTY;
}
#endif
