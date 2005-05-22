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


#include <Traps.h>	// MAD
#include <Gestalt.h>

#include "assoctab.h"
#include <stdlib.h>
#include <string.h>
#include "ddprint.h"		// MAD
// #include "dpdll.h"
#include "commapi.h"
#include "CommCTB.h"
#include "CTBUtils.h"
#include "TransportUtils.h"		// MAD

ser_t*				ser = NULL;

#define PLAYER_YOU 0	// MAD


#define min(a,b)	( ((a) < (b)) ? (a) : (b) )

#pragma export on

comm_driverInfo_t gCommDriverInfo = {
	comm_DRIVER_SIGNATURE,
	sizeof(comm_driverInfo_t),
	"Serial",
	0x0100,
	(comm_DRIVER_IS_VISIBLE
	| comm_DRIVER_IS_FAST			// Assuming they pick 19200 or higher
	| comm_DRIVER_ALLOWS_GAMELIST),
	comm_INIT_NEEDS_UI
};

int commNoOp( commNoOpReq_t *req, commNoOpResp_t *resp) {

	/*
	 *	Do nothing.
	 *
	 *	Return FALSE on error.
	 */

	resp->status = ser_RES_OK;
	return(TRUE);
}

int commInit(commInitReq_t *req, commInitResp_t *resp) {
 	OSErr				error = noErr;
	ConnHandle			connection = nil;
    commInitResp_t   	respDummy;
    Boolean				loaded;

	/*
	 *	Initialize the communications driver.  
	 *
	 *	Return FALSE on error.
	 */

    if (resp == NULL) resp = &respDummy;

	if (!req) {
		resp->status = comm_STATUS_BUG;
		return false;
	}

	#if 0
	if (req->reqLen != sizeof(commInitReq_t)) {
		resp->status = comm_STATUS_BAD_VERSION;
		return false;
	}
	#endif

	//	allocate the memory for the communications record (this
	//	also initializes all of the fields in the structure)
	
	ser = CTB_create();
	if (ser == nil) {
		resp->status = comm_STATUS_BUG;
		return false;
	}

	// openMFP();	// MAD	
	
	loaded = InitializeCommToolBox();
	if (loaded) {
		// MAD fprintf(mfp,"InitCommToolbox -- loaded\n");	// MAD
		if (req->flags & comm_INIT_FLAGS_RESUME) {
			//	initialize the connection
			error = InitializeConnection(true, &connection);		// use the resources; that's what they're there for
			// MAD fprintf(mfp,"Init Connection = %d\n",error);	// MAD
			if (error != noErr) {
				if (connection != nil) {
					CMDispose(connection);							//	it was initialized, so, it must be disposed of
				}

				return false;
			}
		
			error = ReopenCTBConnection(&connection);
			// MAD fprintf(mfp,"ReOpen Connection = %d\n",error);	// MAD
			if (error != noErr) {
				if (connection != nil) {
					CMDispose(connection);							//	it was initialized, so, it must be disposed of
				}

				return false;
			}
		} else {													// Doing a new connection
			// MAD fprintf(mfp,"InitCommToolbox -- must load\n");	// MAD

			//	choose & configure the connection
			error = InitializeConnection(true, &connection);		//	do not use resources
			// MAD fprintf(mfp,"Init Connection = %d\n",error);	// MAD
			if (error != noErr) {
				if (connection != nil) {
					CMDispose(connection);							//	it was initialized, so, it must be disposed of
				}

				return false;
			}
		
			error = OpenCTBConnection(&connection);
			// MAD fprintf(mfp,"Open Connection = %d\n",error);	// MAD
			if (error != noErr) {
				if (connection != nil) {
					CMDispose(connection);				//	it was initialized, so, it must be disposed of
				}

				return false;
			}
		}

		//	there is an open connection, save the ConnHandle for later use
		
		ser->sConnection = connection;
		
		if (CTB_adr2hdl(ser, &req->sessionId, TRUE) != ser_HDL_ME) {
			// MAD fprintf(mfp,"adr2hdl = %d\n",error);	// MAD
			resp->status = comm_STATUS_BUG;
			if (connection != nil) {
				CMDispose(connection);				//	it was initialized, so, it must be disposed of
			}

			return false;
		}
	    resp->status = comm_STATUS_OK;
	}
	return true;
}



int commTerm(commTermReq_t *req, commTermResp_t *resp) {

/*
 *	Tear down the communications driver.  
 *
 *	Return FALSE on error.
 */
 
 	//	shut down and dispose of the CTB connection

	if (ser->sConnection != nil) {
		CMClose(ser->sConnection, false, nil, 0, false);
		CMDispose(ser->sConnection);
		ser->sConnection = nil;
	}
	
	//	dispose of the connection record
	
	free(ser);
	ser = nil;

	return true;
}



int commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp) {
	static commDriverInfoResp_t	sCommDriverInfoResp =  {
		0,
		&gCommDriverInfo
	};

	/*
	 *	Retrieve info about the communications driver.
	 *
	 *	Return TRUE if info was retrieved.
	 */

	//	when the driver is loaded (by calling CTB_Load), the global
	//	driver info structure is initialized

    if (resp == NULL) {
		return FALSE;
	} else {
		*resp = sCommDriverInfoResp;
    	resp->status = 0;
	}

    return (TRUE);
}



int commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp) {
	static ser_adr_t kludgeAdr;
	ser_hdl_t h;

/*
 *	Retrieve info about a player, including ourselves.
 *
 *	Return TRUE if the player is known and is not a group.
 *
 *  Given a player handle returned by commSayHi, you can get back
 *  which network address it is.  The network address is portable-
 *  that is, it must be unique among all computers currently connected,
 *  and it must retain its meaning when put in a packet and sent to
 *  another station.
 */

    if (resp == NULL) {
		return(FALSE);
	}
    if (req == NULL) {
		resp->status = ser_RES_BAD;
		return(FALSE);
    }

	if (req->player == PLAYER_ME) {
		h = ser_HDL_ME;
	} else if (req->player == PLAYER_YOU)
		h = ser_HDL_YOU;
	else
	{
		resp->status=1;
		return FALSE;
	}

    // index into nodes table and fill in response
	if (CTB_hdl2adr(ser, h, &kludgeAdr) != ser_RES_OK) {
		resp->status = 2;
		return FALSE;
	}

    resp->status    = 0;
    resp->name      = NULL;
    resp->address   = &kludgeAdr;
    resp->addrLen   = sizeof(ser_adr_t);
 	memset(&resp->nodeID, 0, sizeof(resp->nodeID));	// MAD

    return(TRUE);
}


int commTxFull(commTxFullReq_t *req, commTxFullResp_t *resp) {

/*
 *	Find out whether the transmit queue is full.
 *
 *	Return TRUE if no more packets can be queued for transmission at this
 *	time.
 */

    commTxFullReq_t	reqDummy;
    commTxFullResp_t	respDummy;

    if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
    if (resp == NULL) resp = &respDummy;

    // Not implemented.
    resp->status = 1;
    return(FALSE);
}


int commTxPkt( commTxPktReq_t *req, commTxPktResp_t *resp) {

/*
 *	Send a packet.  Upon return, the buffer can be discarded, although the
 *	packet may not be sent until later.
 *
 *	Return FALSE on error.  Note that a TRUE return value does not guarantee
 *	that the packet has been (or ever will be) sent.
 */

    commTxPktResp_t   respDummy;
	ser_hdl_t h;

    if (resp == NULL) resp = &respDummy;
    if (req == NULL) {
		resp->status = 3;
		return FALSE;
	}

	h = req->dest;
	if (h == PLAYER_BROADCAST)
		h = ser_HDL_BROADCAST;
	// Abort if no good place to send packet.
	if (req->dest == PLAYER_ME) {
		DPRINT(("commTxPkt: can't send to myself\n"));
		resp->status = 1;
		return FALSE;
	}

    if (CTB_put(ser, h, req->buffer, req->length)) {
		DPRINT(("commTxPkt: couldn't send packet\n"));
		resp->status = 2;
	} else
		resp->status = 0;

    return(resp->status == 0);
}


int commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp) {
    commPeekPktReq_t	reqDummy;
    commPeekPktResp_t	respDummy;

	/*
	 *	Get information about a pending incoming packet.
	 *
	 *	Return TRUE if a packet was retrieved.
	 */

    if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
    if (resp == NULL) resp = &respDummy;

	resp->status = 1;

	return(FALSE);
}


int commRxPkt(commRxPktReq_t *req, commRxPktResp_t *resp) {

	/*
	 *	Retrieve a pending incoming packet.
	 *
	 *	Return TRUE if a packet was retrieved.
	 */

    commRxPktReq_t    reqDummy;
    commRxPktResp_t   respDummy;
	ser_result_t res;

    if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
    if (resp == NULL) resp = &respDummy;

	resp->length = req->size;
	res = CTB_get(ser, req->buffer, &resp->length);

    resp->status = res;			// Fortunately, ser_RES_OK == 0
    resp->src    = PLAYER_YOU;

	if (resp->status == ser_RES_OK)
		DPRINT(("commRxPkt: GOOD PACKET\n"));
    return (resp->status == ser_RES_OK);
}


int commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp) {

/*
 *	Attempt to parse a NUL-terminated address string into a free-format
 *	buffer.
 *
 *	Return TRUE if the string was parsed successfully.
 */

    commScanAddrReq_t	reqDummy;
    commScanAddrResp_t	respDummy;
	ser_adr_t adr;

    if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
    if (resp == NULL) resp = &respDummy;

    // convert into our internal address
    if (sscanf(req->printable, "SERIAL:%x", &adr) != 1) {
        resp->status = 1;
		return(FALSE);
    }

    resp->length = sizeof(adr);

    if (req->address != NULL) {
        memcpy(req->address, &adr, min(req->size, resp->length));
    }

    resp->status = 0;
    return (TRUE);
}


int commPrintAddr( commPrintAddrReq_t *req, commPrintAddrResp_t *resp) {
    commPrintAddrReq_t	reqDummy;
    commPrintAddrResp_t	respDummy;
    char printable[50];

	/*
	 *	Attempt to format a free-format address buffer into a NUL-terminated
	 *	string.
	 *
	 *	Return TRUE if the buffer was formatted successfully.
	 */

    if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
    if (resp == NULL) resp = &respDummy;

    if ((req->length != sizeof(ser_adr_t)) || (req->address == NULL)) {
		DPRINT(("length != sizeof(ser_adr_t) or req-address == NULL\n"));
		resp->status = 1;
		return(FALSE);
    }

    sprintf(printable, "SERIAL:%x", *(ser_adr_t *) req->address);

    if (req->printable != NULL) {
		if (req->size < strlen(printable) + 1) {
			resp->status = 2;
			return(FALSE);
		}
        strcpy(req->printable, printable);
    }

    resp->status = 0;
    return(TRUE);
}


int commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp) {

/*
 *	Generate a pseudo-player handle referring to a group of players.  Handy
 *	for multicasting.  A group can have zero players.
 *
 *	Return TRUE if the pseudo-player handle was generated.
 */

    req = req;
    resp = resp;
    return(FALSE);
}


int commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp) {

/*
 *	Invalidate a pseudo-player handle referring to a group of players.
 *
 *	Return TRUE if the pseudo-player handle was invalidated.
 */

    req = req;
    resp = resp;
	return (FALSE);
}



int commGroupAdd(commGroupAddReq_t *req, commGroupAddResp_t *resp) {

/*
 *	Add one or more players to a group.
 *
 *	Return TRUE if the players were all added.
 */
 
    req = req;
    resp = resp;

    return(FALSE);
}

#if 0 // MAD	 -- this stuff seems to be commented out in commapi.h

int commGroupSubtract(commGroupSubtractReq_t *req, commGroupSubtractResp_t *resp) {

/*
 *	Subtract one or more players from a group.  Do not delete the group,
 *	even if it is left empty.
 *
 *	Return TRUE unless there was a problem subtracting one or more players.
 */

    req = req;
    resp = resp;

    return(FALSE);

}

#else

// MAD below

int
commSetParam(
	commSetParamReq_t *	req,	// Request
	commSetParamResp_t *	resp)	// Response
{
	commSetParamResp_t	respDummy;
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}

#endif


int commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp) {

/*
 *	Establish a data link to a player and shake hands with him.  This does
 *	not actually establish a connection in the IP sense of the word.
 *
 *	Return TRUE if the link was established and we shook hands.
 *
 *  req->address is a ser_adr_t, which is just a random long int.
 *
 */

    commSayHiReq_t	reqDummy;
    commSayHiResp_t	respDummy;
	ser_hdl_t		hdl;

    if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
    if (resp == NULL) resp = &respDummy;

    if (req->length == 0) {
		DPRINT(("commSayHi: req->length == 0"));
		resp->status = 1;
		return FALSE;
	}

    hdl = CTB_adr2hdl(ser, req->address, TRUE);
	DPRINT(("commSayHi: adr %x, hdl %d\n", *(ser_adr_t *)req->address, hdl));
	if (hdl == ser_HDL_ME) {
		resp->player = PLAYER_ME; 
		resp->status = 0;
		return TRUE;
	}
	
	if (hdl == ser_HDL_YOU) {
		resp->player = PLAYER_YOU;	// MAD 
		resp->status = 0;
		return TRUE;
	}

    resp->status = 2;
    return TRUE;
}


int commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp) {

/*
 *	Tear down a data link to a player.  The link or the player may already be
 *	down, so don't shake hands.
 *
 *	Return TRUE if the link was successfully broken.
 */

    commSayByeResp_t	respDummy;
	ser_result_t err;
	ser_hdl_t h;

    if (resp == NULL) resp = &respDummy;
    if (req == NULL) {
		resp->status = 1;
		return FALSE;
	}
	if (req->player == PLAYER_ME)
		h = ser_HDL_ME;
	else
		h = req->player;

	err = CTB_hdlDestroy(ser, h);
	DPRINT(("commSayBye: player %d, hdl %d, err %d\n", req->player, h, err));
	if (err != ser_RES_OK) {
		DPRINT(("commSayBye: ser_hdlDestroy returns %d\n", err));
		resp->status = 2;
		return FALSE;
	}

    resp->status = 0;
    return (TRUE);
}

Boolean commLoadTest(void);		// this prototype should be in commapi.h

Boolean commLoadTest(void)
{
	Boolean	result = false;
	long	value;
	OSErr	error;
	FSSpec	where;
	
	if (NGetTrapAddress(_CommToolboxDispatch, OSTrap) != NGetTrapAddress(_Unimplemented, OSTrap)) {
		if (NGetTrapAddress(_Gestalt, OSTrap) != NGetTrapAddress(_Unimplemented, OSTrap)) {

			// Check for Gestalt first
			error = Gestalt(gestaltVersion, &value);
			if (error == noErr) {
				// Check for CTB
				error = Gestalt(gestaltCTBVersion, &value);
				
				if (error == noErr) {						// There is a comm toolbox; we can procede
					result = true;
				}
			}
		}
	}

	return result;
}

#if 0

Boolean commNeedsUI(commInitReq_t* req) {

	//	if this is a resume, we don't need the UI

	if (req->flags & comm_INIT_FLAGS_RESUME) {
		return false;
	} else {
		return true;
	}
	
}

#endif

#pragma export off

#if 0
void CTB_Unload(void) {

	//	shut down the communications toolbox
	
	TearDownCTB();
	
}
#endif

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


ser_hdl_t CTB_adr2hdl(ser_t *ser, ser_adr_t *adr, int insert) {

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
	
#if 0 // MAD
	int i;
	ser_adr_t *padr;
	for (i=0; i<ser->adrtab->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(ser->adrtab, i);
		if (!pe)
			break;
		padr = (ser_adr_t *) &pe->value;
		if ((*padr & adrMask) == (*adr & adrMask)) {
			DPRINT(("ser_adr2hdl(%x): found hdl %d\n", *adr, pe->key));
			return (ser_hdl_t) pe->key;
		}
	}
	if (insert) {
		padr = assoctab_subscript_grow(ser->adrtab, ser->nextHdl);
		if (ser->nextHdl == ser_HDL_ME) {
			// Caller is setting this station's Adress.
			ser->myAdr = *adr;
		}
	}
	if (!insert || !padr)
		return ser_HDL_NONE;
	*padr = *adr;
	return ser->nextHdl++;
#else
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

#endif
}


ser_result_t CTB_hdlDestroy(ser_t *ser, ser_hdl_t hdl) {
	// MAD assoctab_subscript_delete(ser->adrtab, hdl);
	return ser_RES_OK;
}


ser_result_t CTB_put(ser_t *ser, ser_hdl_t dest, void *buf, size_t len) {
	ser_hdr_t		hdr;
	CMStatFlags		flags;
	CMBufferSizes	sizes;

	
#if 0 // MAD
	if ((dest != ser_HDL_BROADCAST) && !assoctab_subscript(ser->adrtab, dest)) {
		DPRINT(("ser_put: handle %d not in table\n", dest));
		return ser_RES_EMPTY;
	}
#endif

	//DPRINT(("ser_put: len: %ld\n", len));
	if (ser->sConnection == NULL)
		return ser_RES_BAD;
	if (len > 255)
		return ser_RES_FULL;		// sorry, charlie, we've got our limits

	hdr.frame0 = ser_HDR_FRAME0;
	hdr.frame1 = ser_HDR_FRAME1;
	// MAD hdr.src = ser->myAdr & 255;
	hdr.bodylen = len;
	hdr.hchksum = (ser_HDR_FRAME0 + ser_HDR_FRAME1 + len) & 255;
	hdr.bodycrc = CTB_crc(buf, len);
	{
		CMErr		theErr;
		long		nBytes;
		char kludge[1000];
		memcpy(kludge, &hdr, 5);	// MAD sizeof(hdr));
		memcpy(kludge+ 5 /* MAD sizeof(hdr) */, buf, len);
		
		nBytes = len + 5; // MAD sizeof(hdr);
		theErr = CMWrite(
			ser->sConnection,			//	connection to write to
			kludge,						//	pointer to data
			&nBytes,					//	number of bytes to write
			cmData,						//	write to data part of connection
			false,						//	synchronus
			nil,						//	no callback
			180,						//	try a 3 second timeout
			0							//	no flags
		);

		if (theErr != noErr) {
			return ser_RES_FULL;
		}
	}

	return ser_RES_OK;
}


ser_result_t CTB_get(ser_t *ser, void *buf, size_t *len) {
	
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
	FILE *mfp;	// MAD

	//	make sure there is an open connection

	if (ser->sConnection == NULL) {
		// fprintf(mfp,"rx pkt: no connection\n");
		return ser_RES_BAD;
	}
	
	mfp = fopen("md.log","a");
	
	while (true) {
		int c;
	
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

			fprintf(mfp,"CTB_get %d bytes\n",nBytes);

			//	if there was an error, it's time to bail
			
			if (theErr != noErr && theErr != cmTimeOut) {	// MAD
				fprintf(mfp,"rx pkt: CMRead  = %d\n",theErr);
				return ser_RES_BAD;
			}
			
			//	if we didn't read anything, there is nothing to do
	
			if (nBytes <= 0) {
				fprintf(mfp,"rx pkt: nothing to read: res_empty\n");
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
		} else if (ser->got == 4) {	// MAD 4 since removed src byte
			// Is header Checksum valid?
			int cs = ((ser_HDR_FRAME0 + ser_HDR_FRAME1 + ser->pkt.hdr.bodylen) & 0xff);
			fprintf(mfp, "checksum: have %d, want %d + %d + %d = %d\n",
					c, ser_HDR_FRAME0 , ser_HDR_FRAME1, + ser->pkt.hdr.bodylen,  cs);
			if (c != cs) {
				ser->got = 0;			// no, just start over.
			}
		} else if (ser->got >= 5 + ser->pkt.hdr.bodylen) {	// MAD
			ser->got = 0;
			// Done with packet.  Does crc match?
			if (CTB_crc(ser->pkt.body, ser->pkt.hdr.bodylen) != ser->pkt.hdr.bodycrc) {
				// fprintf(mfp,"rx pkt: RES_BAD\n");
				return ser_RES_BAD;
			}
			// Is user buffer big enough to fit this packet?
			if (ser->pkt.hdr.bodylen > *len) {
			// fprintf(mfp,"rx pkt: RES_FULL\n");
				return ser_RES_FULL;	// no, packet too big to fit, fail
			} else {
				ser_adr_t adr;
				// Yes.  Copy packet to user, prepare for next packet.
				memcpy(buf, ser->pkt.body, ser->pkt.hdr.bodylen);
				*len = ser->pkt.hdr.bodylen;
				// MAD adr = ser->pkt.hdr.src;	// packet only has low byte of address
				// MAD *src = CTB_adr2hdl(ser, &adr, FALSE, 255);	// so mask all but low byte out
				
				// fprintf(mfp,"rx pkt: RES_OK %d\n",*len);

				return ser_RES_OK;
			}
		}

	}
	fclose(mfp);	// MAD
}


ser_t *CTB_create(void) {

	ser_t *ser;
	ser = malloc(sizeof(ser_t));
	if (!ser) {
		// ABORT! Out of Memory
		return NULL;
	}
	
	ser->sConnection = NULL;		//	ConnHandle will go here later
	ser->baud = 0;
	ser->portnum = -1;

	ser->myAdr = ser_ADR_NONE;
	ser->yourAdr = ser_ADR_NONE;
	
#if 0 // MAD
	ser->adrtab = assoctab_create(sizeof(ser_adr_t));
	if (!ser->adrtab) {
		free(ser);
		return NULL;
	}
#endif

	ser->nextHdl = ser_HDL_ME;		// first adr2hdl sets my adr
	ser->got = 0;
	ser->head = 0;
	ser->len = 0;

	return ser;
}

#if 0
void CTB_Load(void) {

	//	load the CTB communications function pointers and driver information block

	commInit = CTB_commInit;
	commTerm = CTB_commTerm;
	commDriverInfo = CTB_commDriverInfo;
	commPlayerInfo = CTB_commPlayerInfo;
	commTxFull = CTB_commTxFull;
	commTxPkt = CTB_commTxPkt;
	commPeekPkt = CTB_commPeekPkt;
	commRxPkt = CTB_commRxPkt;
	commScanAddr = CTB_commScanAddr;
	commPrintAddr = CTB_commPrintAddr;
	commGroupAlloc = CTB_commGroupAlloc;
	commGroupFree = CTB_commGroupFree;
	commGroupAdd = CTB_commGroupAdd;
	commGroupSubtract = CTB_commGroupSubtract;
	commSayHi = CTB_commSayHi;
	commSayBye = CTB_commSayBye;
	
	commUnload = CTB_Unload;
	
	//	load our resource into the global driver information block

	dpGetTransportInfo(kSERIALDRIVERNAME, &gCommDriverInfo_CTB);

	//	initialize the CTB
	
	InitializeCommToolBox();

}
#endif

ser_result_t CTB_hdl2adr(ser_t *ser, ser_hdl_t hdl, ser_adr_t *adr) {
#if 1	// MAD
	if (hdl == ser_HDL_ME)
		*adr = ser->myAdr;
	else if (hdl == ser_HDL_YOU)
		*adr = ser->yourAdr;
	else
		*adr = ser_ADR_NONE;
	
	return (*adr != ser_ADR_NONE) ? ser_RES_OK : ser_RES_EMPTY;
#else
	ser_adr_t *padr = assoctab_subscript(ser->adrtab, hdl);
	if (!padr) {
		*adr = ser_ADR_NONE;
		return ser_RES_EMPTY;
	}
	*adr = *padr;
	return ser_RES_OK;
#endif
}

