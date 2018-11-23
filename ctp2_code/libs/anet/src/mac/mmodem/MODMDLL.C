// Wrapper to adapt the simple packetizing serial driver to Zobel's comm API.
//
// $Log: MODMDLL.C $ 
// Revision 1.1  1997/05/30 18:28:59  dkegel 
// Initial revision 
// Revision 1.19  1996/07/01 20:52:46  dkegel 
// Check sessionID during ramp check; catches open cables. 
// Revision 1.18  1996/07/01 19:15:56  dkegel 
// Use correct bit of req->flags to decide whether to dial. 
// Revision 1.17  1996/06/30 22:22:50  dkegel 
// Loopback test now triggered by -Ktest 
// Revision 1.16  1996/06/30 21:17:09  dkegel 
// Send a ramp after connection, get ramp from peer, and report errors to log. 
// (Can't be fatal, since partner might not have ramp code.) 
// Revision 1.15  1996/06/29 17:56:06  dkegel 
// 1. No longer use HMReset etc. until we've verified that driver is 
// sending characters properly, as HMReset locks up if IRQ is wrong. 
// 2. Detect 'driver not sending characters' condition during initial 
// reset attempt, return comm_STATUS_FULL to indicate IRQ problem. 
// 3. Don't try to hang up if we never even took the darn thing offhook; 
// this saves a LOT of time if modem is not responding at all. 
// Revision 1.14  1996/06/26 18:04:08  dkegel 
// Added sessionId to log. 
// Revision 1.13  1996/06/25 21:16:46  dkegel 
// Added modem log file (modem.log) always generated when dialing/answering. 
// Revision 1.12  1996/06/25 20:05:16  dkegel 
// 1. Return comm_STATUS_BAD_VERSION if commInitReq wrong size. 
// 2. Support commInitReq.dialing_method. 
// Revision 1.11  1996/06/25 18:51:02  dkegel 
// Added support for non-standard irq's back in. 
// Now checks hwirq and baseadr for validity before using. 
// Revision 1.9  1996/06/24 18:23:08  dkegel 
// Added hwirq and baseadr parameters to ser_config to allow non-standard 
// irq's. 
// Revision 1.8  1996/06/05 19:54:55  dkegel 
// Supra makes us wait longer for responses. 
// This will make aborting painfully slow, but oh well. 
// Revision 1.7  1996/05/22 00:45:32  dkegel 
// Remove "Driver" from driver description. 
// Revision 1.6  1996/05/15 02:09:44  dkegel 
// 1. Return standard comm_STATUS codes in status field of response from 
// commInit. 
// 2. Increased dialing timeout to 60 seconds, decreased ok timeout to 1 second. 
// 3. Fixed call completion code checking to handle modems with non-standard 
// "no dial tone" message. 
// Revision 1.5  1996/05/14 23:23:58  dkegel 
// 1. Eat any and all keystrokes in the buffer at commTerm() time. 
// (Yes, this is a kludge) 
// 2. Only wait one second for modem to respond, not three. 
// Revision 1.4  1996/05/10 21:33:50  dkegel 
// 1. Retry modem reset up to four times. 
// 2. Recognize BUSY, ERROR, etc.. modem result codes. 
// 3. Clear buffers before many HM commands in hopes of working around 
// possible problems in HM commands if garbage is in buffer. 
// Revision 1.3  1996/05/09 20:28:15  dkegel 
// 1. Longer timeout, especially on answer. 
// 2. Allow lookup of our own address, i.e. commInit can now return PLAYER_ME 
// 3. Easier to read modem init code 
// 4. Moved hangup code to subroutine, but don't call it yet before 
// dialling... seems to cause problems. 
// Revision 1.2  1996/05/09 01:57:49  dkegel 
// 1. Abort dial if user hits a key. 
// 2. Call commTerm to hang up if something weird happens during dialing. 
// 3. Change driver name and flags to be right for modem. 
// Revision 1.1  1996/05/07 00:29:34  dkegel 
// Initial revision 
// Revision 1.4  1996/04/30 21:00:12  dkegel 
// Added "comm_DRIVER_IS_FAST" to capabilities on theory that 
// speed will always be 19200 and there will only be two users. 
// Revision 1.3  1996/03/06 23:46:38  dkegel 
// Shell can't handle drivers that don't need phone numbers and don't allow 
// gamelists, so turn on the allows_gamelist bit. 
// Revision 1.2  1996/02/29 01:36:48  dkegel 
// 1. Implemented commSayHi etc. 
// 2. Implemented new dpEnumTransports capabilities signature. 
// 3. Implemented new parameters to commInit. 
// Revision 1.1  1996/02/25 04:50:57  dkegel 
// Initial revision 
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <Resources.h>
#include <Unix.h>
//#include <conio.h>
#include <time.h>
#include <fcntl.h>

#include <Gestalt.h>	// MAD
#include <Traps.h>

#include <Types.h>
#include <Script.h>
#include <Connections.h>
//#include <io.h>

#define min(a,b)	(((a) < (b)) ? (a) : (b)) 

#if 0
	#include "LoadGlobalFrag.h"
#endif

//#define DEBUG TRUE
#define DEBUG_MODULE	!TRUE

#include "ddprint.h"
//#define DPRINT(s)	printf s

#include "modm.h"
#include "commapi.h"
#include "CTBStuff.h"
//#include	"TransportUtils.h"

//#include "commlib.h"
//#include "fast.h"
//#include "gnlframp.h"

ser_t *ser = NULL;
int already_hungup = FALSE;

THz				myZone;

extern Boolean	gCTBLoaded;
extern Boolean gConnected;

#ifdef PACKETLOGGING
FILE*		packetinlog;
FILE*		packetoutlog;
#endif

//ConnHandle		connection = nil;
extern Ptr		configString;

#define ERROR	-1		// return value of open()

//#define DRIVER_VERSION		VERSION(1,0)	// Version of this comm driver

#define PLAYER_YOU 0			// What we translate ser_HDL_YOU to.

/*
 *	Do nothing.
 *
 *	Return FALSE on error.
 */
 
#pragma export on

comm_driverInfo_t gCommDriverInfo = {
		comm_DRIVER_SIGNATURE,
		sizeof(comm_driverInfo_t),
		"Modem",
		0x0100,
		(comm_DRIVER_IS_VISIBLE
		| comm_DRIVER_ALLOWS_GAMELIST),
		(comm_INIT_NEEDS_SESSIONID
		| comm_INIT_NEEDS_PORTNUM
		| comm_INIT_NEEDS_PHONENUM
		| comm_INIT_NEEDS_BAUD
		| comm_INIT_NEEDS_UI)
};

#pragma export off

static int logfd = ERROR;

static void fdclose(void)
{
	if (logfd != ERROR) {
		close(logfd);
	}
}

void fdprint(char *msg);

void fdprint(char *msg)
{
	if (logfd != ERROR) {
		write(logfd, msg, strlen(msg));
	}
}

#pragma export on

int commNoOp( commNoOpReq_t *req, commNoOpResp_t *resp)
{
    DPRINT(("commNoOp(): \n"));
    resp->status = comm_STATUS_OK;

	(void) req; (void) resp;

    return TRUE;
}

/*
 *	Tear down the communications driver.  
 *
 *	Return FALSE on error.
 */

/*
 *	Tear down the communications driver.  
 *
 *	Return FALSE on error.
 */
int commTerm(commTermReq_t *req, commTermResp_t *resp)
{
	OSErr				error;
	CMStatFlags		flags;
	CMBufferSizes	sizes;

#ifdef PACKETLOGGING
	error = fclose(packetinlog);
	error = fclose(packetoutlog);
#endif

	//Debugger();
	if (ser && (!req || (req->flags == 0))) {
		DPRINT(("commTerm: flags zero, hanging up\n"));
		// MAD SetZone(myZone);
		ser_destroy(ser);
	}
	
	#if 0
		unloadTransport();
	#endif

	return true;
}


/*-------------------------------------------------------------------------
 Initialize the communications driver.

 Return FALSE on error.

 Required parameters:
 req->sessionId
 req->baud
 req->portnum

 Optional Parameters (set to 0 if unused):
 req->baseadr
 req->hwirq
-------------------------------------------------------------------------*/

int commInit(commInitReq_t *req, commInitResp_t *resp)
{
	commInitResp_t	respDummy;
	long				ticks;
	char				response[256];
	char				*init = "AT&D2&K1";	// Hangup, cancel autoanswer if DTR drops; rts/cts flow control
	int				hwirq;
	int				baseadr;
	int				dialing_method;
	char				message[256];
	OSErr				error;
	Ptr				newConfig;
	Str255			tempStr;
	char*				where;
	CMStatFlags		flags;
	CMBufferSizes	sizes;
	Ptr				tempConfig;
	Boolean			keys;
	EventRecord		theEvent;
	Str255			name;
	unsigned long	value = 0;
	Boolean			result;
	Boolean			choose = false;
	Str255			helperName;

	// for cancel box
	WindowPtr 		wpt;
	
	//Debugger();

	myZone = GetZone();

	already_hungup = FALSE;
	if (resp == NULL) {
		resp = &respDummy;
	}

	DPRINT(("commInit:\n"));

	if (!req) {
		DPRINT(("commInit: NULL req\n"));
		resp->status = comm_STATUS_BUG;

		return FALSE;
	}

	if (req->reqLen != sizeof(commInitReq_t)) {
		DPRINT(("commInit: req wrong size\n"));
		resp->status = comm_STATUS_BAD_VERSION;

		return FALSE;
	}

	#if 0
	error = LoadTransport("\pModemHelper");
	if (error != noErr) {
		DPRINT(("commInit: no modemhelper\n"));
		resp->status = comm_STATUS_BUG;

		return FALSE;
	}
	#endif

#ifdef PACKETLOGGING
	packetinlog = fopen("packetinlog", "ab+");
	packetoutlog = fopen("packetoutlog", "ab+");
#endif

	//	if we are resuming, get our serial structure pointer back

	if (req->flags & comm_INIT_FLAGS_RESUME) {
		DPRINT(("Getting sysheap 'ser' address from req->baseadr"));
		ser = (ser_t *) req->baseadr;
		value = (long) ser;
		error = noErr;
		if (ser == 0) {
			return FALSE;
			resp->status = comm_STATUS_BUG;
		}
		// MAD error = GetData(&value);
		// MAD if (value == nil) {
		// MAD	resp->status = comm_STATUS_BUG;

	} else {
	
		//	if we're not resuming the connection we want to avoid the resource since
		//	it could have bad data in it, so, we set error to nothing. The value has already
		//	been initialized to nil above. If the ser is created, it's stored into the resource
		//	a little further down
		
		error = noErr;
	}
	
	if (error != noErr) {
		DPRINT(("commInit: no modemhelper value\n"));
		resp->status = comm_STATUS_BUG;

		return FALSE;
	}

	if (value == nil) {
		ser = ser_create();
		if (!ser) {
			DPRINT(("commInit: ser_create\n"));
			resp->status = comm_STATUS_BUG;

			free(ser);
			return FALSE;
		}
		
		req->baseadr = (long) ser;
		DPRINT(("Storing sysheap 'ser' address in req->baseadr"));
		// MAD error = SetData((unsigned long)ser);
		
		if (error != noErr) {
			DPRINT(("commInit: unable to set modemhelper value\n"));
			resp->status = comm_STATUS_BUG;

			error = SetData((unsigned long)nil);					// I know; it failed before.  But it might have set it anyway
			free(ser);
			return FALSE;
		}
	} else {
		result = InitializeCommToolBox();

		SetZone(SystemZone());

		ser = (ser_t*)value;

		error = CMStatus(ser->sConnection, sizes, &flags);
		if (error != noErr) {
			DPRINT(("commInit: unable to CMStatus\n"));
			resp->status = comm_STATUS_BUG;

			SetZone(myZone);
			ser_destroy(ser);
			return FALSE;
		}

		SetZone(myZone);

		resp->status = comm_STATUS_OK;

		return TRUE;
	}

	if (ser_adr2hdl(ser, &req->sessionId, TRUE) != ser_HDL_ME) {
		DPRINT(("commInit: adr2hdl\n"));
		resp->status = comm_STATUS_BUG;

		return FALSE;
	}

	// Accept dialing method if within range
	if ((req->dialing_method == PULSE) || (req->dialing_method == DEFAULT_DIALING_METHOD)) {
		dialing_method = req->dialing_method;
	} else {
		dialing_method = TOUCH_TONE;
	}

	if (req->flags & comm_INIT_FLAGS_RESUME) {
		DPRINT(("commInit: flags non-zero, hope we're already connected\n"));
		resp->status = comm_STATUS_OK;

		return TRUE;		// don't dial
	}

#if 0 // MAD
	if (req->flags & comm_INIT_CONFIG_ONLY) {
		error = config_ctb(&(ser->sConnection));
		if (error != noErr) {
			DPRINT(("commInit: unable to config_ctb\n"));
			resp->status = comm_STATUS_BUG;

			ser_destroy(ser);
			return FALSE;
		}

		resp->status = comm_STATUS_BUG;
		return TRUE;
	}
#endif
	
	/* Check to see if the CommToolBox has been started up */
	if (!gCTBLoaded) {
		if (!InitializeCommToolBox()) {									// If this fails
			resp->status = comm_STATUS_BUG;

			ser_destroy(ser);
			return false;														// We can't do anything
		}
	}

	// If there is no prefs, force a choose (must put config in ConfigString)
	error = GetToolname(name);
	if (error != noErr) {
		choose = true;
	}

	error = InitializeConnection(true, &(ser->sConnection));	// Now, configure a connection
	if ((error != noErr) || (ser->sConnection == nil)) {
		resp->status = comm_STATUS_BUG;

		ser_destroy(ser);
		return false;														// We can't do anything
	}

	// If there is no prefs, force a choose (must put config in ConfigString)
	if (choose) {
		extern FORCESCREENUPDATE	gScreenFunc;
		
		error = ConfigureConnection(&ser->sConnection);
		
		//	if we had to do a choose, the screen is in a bad state for the application. Restore
		//	it here
		
		if (gScreenFunc != nil) {
			gScreenFunc();
		}
	}

	// Kludge: if user choses pulse dialing, create a debug log.
	logfd = open("modem.log", O_RDWR | O_CREAT | O_TRUNC | O_BINARY);
	if (logfd == ERROR) {
		DPRINT(("commInit: can't create modem.log, errno %d\n", error));
	}
	sprintf(message, "log started. (sessionId %x)\r\n", req->sessionId);
	fdprint(message);

	SetZone(SystemZone());

	newConfig = NewPtrClear(GetPtrSize(configString) + 128);				// Play it safe; allow lots of room for dialing modifiers
	// If we are the originator, initiate the conversation
	if (req->phonenum && req->phonenum[0] && req->phonenum[0] != '-') {
		DPRINT(("commInit: setting dialing mode, dialing %s\n", req->phonenum));
		fdprint("dialing '");
		fdprint(req->phonenum);
		fdprint("'\r\n");
		ClearRXBuffer( ser->port );
		
		// Insert the phone number into the string
		strcpy(tempStr, "PhoneNumber \"");										// The substring we're looking for
		where = strstr(configString, tempStr);									// So find it
		if (where != nil) {
			where += strlen(tempStr);												// We really want to work from the end of it
			strncpy(newConfig, configString, where - configString);		// Copy everything up to the end of it into the new string
			newConfig[where - configString + 1] = '\0';						// Make sure it ends there
			strcat(newConfig, req->phonenum);									// Concatenate on the phone number
			strcat(newConfig, where);												// Pick the config up from the second quote mark
		} else {
			strcat(tempStr, req->phonenum);										// Concatenate on the phone number
			strcat(tempStr, "\" ");													// The substring we want to add on to the config
			strcat(newConfig, " ");
			strcat(newConfig, tempStr);
		}
	
		ticks = 60 * CLOCKS_PER_SEC;												// Wait 35 seconds for connection
	} else {	// If we are the answerer, answer the phone
		/*
		strcpy(tempStr, "TypeOfCall ");
		where = strstr(configString, tempStr);
		if (where != nil) {															// TypeOfCall entry already exists; modify it
			tempConfig = NewPtrClear(GetPtrSize(newConfig));
			where += strlen(tempStr);
			//strcpy(tempConfig, configString);
			strncpy(tempConfig, configString, where - configString);
			strcat(tempConfig, "Answer");
			where = strchr(where, ' ');
			if (where != nil) {
				strcat(tempConfig, where);
			} else {
				strcat(tempConfig, " ");
			}
			DisposePtr(newConfig);

			newConfig = NewPtrClear(GetPtrSize(tempConfig));
			memcpy(newConfig, tempConfig, GetPtrSize(tempConfig));		
			DisposePtr(tempConfig);
		} else {																			// TypeOfCall entry should be appended
			strcpy(newConfig, configString);										// Duplicate the configuration
			strcat(newConfig, " TypeOfCall Answer ");							// Append that we want to be in auto-answer mode
		}
		*/
		newConfig = NewPtrClear(GetPtrSize(configString));
		memcpy(newConfig, configString, GetPtrSize(configString));		

		DPRINT(("commInit: setting autoanswer to 1\n"));
		fdprint("setting autoanswer register to 1\r\n");
		ClearRXBuffer( ser->port );

		// Reconfigure the connection
		ticks = 60*CLOCKS_PER_SEC;													// Wait 60 seconds for connection
	}
	
	//DebugStr((StringPtr) newConfig);							//	display part of the config string in the debugger

	error = CMSetConfig(ser->sConnection, newConfig);
	if (error < noErr) {												// Should parse the whole thing; but only complain about an error
		SetZone(myZone);
		ser_destroy(ser);

		resp->status = comm_STATUS_BUG;
		return false;
	}

	// Open the connection
	
	if (req->phonenum && req->phonenum[0] && req->phonenum[0] != '-') {	// We're the dialer
		gConnected = false;

		error = CMOpen(ser->sConnection, true, nil, 0);
		if (error != noErr) {
			DPRINT(("commInit: unable to CMOpen\n"));
			resp->status = comm_STATUS_BUG;

			SetZone(myZone);
			ser_destroy(ser);
			return FALSE;
		}

		// MAD display cancel dialog box
		SetZone(myZone);
		{
			Rect mr = {0,0,50,250};
			
			// center on main screen
			Rect sr = (**GetMainDevice()).gdRect;
			int x = (sr.left + sr.right - (mr.right - mr.left)) / 2;
			int y = (sr.top + sr.bottom - (mr.bottom - mr.top)) / 2;
			OffsetRect(&mr,-mr.left + x, -mr.top + y);
			
			wpt = NewWindow(NULL,&mr,"\p",TRUE,1,(WindowPtr) -1L, FALSE,0);
			SetPort(wpt);
			MoveTo(20,wpt->portRect.bottom - 20);
			DrawString("\pPress any key to cancel...");
		}
		
		do {
			SetZone(SystemZone());
			CMIdle(ser->sConnection);													// Do something so as not to lock up the processor!
			error = CMStatus(ser->sConnection, sizes, &flags);					// Are we connected?

			SetZone(myZone);
			if (WaitNextEvent(keyUpMask | keyDownMask | mDownMask | mUpMask, &theEvent, 12, nil)) {
		 		switch(theEvent.what) {
		 			case mouseDown:
		 			case mouseUp:
		 			case keyDown:
		 			case keyUp:
						SetZone(SystemZone());
			 			error = CMAbort(ser->sConnection);
			 			error = 1024;
			 			break;
		 		}
			}
		 	if (flags & cmStatusOpen) {
		 		gConnected = true;
		 	}
		} while ((error == noErr) && (!gConnected));
	
		// MAD
		SetZone(myZone);
		DisposeWindow(wpt);

		if (error != noErr) {
			//error = CMClose(ser->sConnection, false, nil, 0, true);	// Done in ser_destroy

			SetZone(myZone);
			ser_destroy(ser);
			resp->status = comm_STATUS_BUG;
			return false;
		}
	} else {																					// Wait for a call
		gConnected = false;

		error = CMListen(ser->sConnection, true, nil, 0);
		if (error != noErr) {
			DPRINT(("commInit: unable to CMListen\n"));
			resp->status = comm_STATUS_BUG;

			SetZone(myZone);
			ser_destroy(ser);
			return FALSE;
		}

		// MAD display cancel dialog box
		SetZone(myZone);
		{
			Rect mr = {0,0,50,250};
			
			// center on main screen
			Rect sr = (**GetMainDevice()).gdRect;
			int x = (sr.left + sr.right - (mr.right - mr.left)) / 2;
			int y = (sr.top + sr.bottom - (mr.bottom - mr.top)) / 2;
			OffsetRect(&mr,-mr.left + x, -mr.top + y);
			
			wpt = NewWindow(NULL,&mr,"\p",TRUE,1,(WindowPtr) -1L, FALSE,0);
			SetPort(wpt);
			MoveTo(20,wpt->portRect.bottom - 20);
			DrawString("\pPress any key to cancel...");
		}
				
		do {
			SetZone(SystemZone());
			CMIdle(ser->sConnection);													// Do something so as not to lock up the processor!
			error = CMStatus(ser->sConnection, sizes, &flags);
			SetZone(myZone);
			if (WaitNextEvent(keyUpMask | keyDownMask | mDownMask | mUpMask, &theEvent, 12, nil)) {
		 		switch(theEvent.what) {
		 			case mouseDown:
		 			case mouseUp:
		 			case keyDown:
		 			case keyUp:
						SetZone(SystemZone());
			 			error = CMAbort(ser->sConnection);
			 			error = 1024;
			 			break;
		 		}
			}
		 	if (flags & cmStatusIncomingCallPresent) {							// Do we have one yet?
		 		gConnected = true;
		 	}
		} while ((error == noErr) && (!gConnected));

		// MAD
		SetZone(myZone);
		DisposeWindow(wpt);

		if (error != noErr) {															// Probably failed without connecting

			SetZone(myZone);
			ser_destroy(ser);

			resp->status = comm_STATUS_BUG;
			return false;
		} else {
			error = CMStatus(ser->sConnection, sizes, &flags);					// Otherwise, look for a connection
			if (!(flags & cmStatusIncomingCallPresent)) {						// Didn't get one
				error = CMAccept(ser->sConnection, false);						// Don't accept it
				error = CMClose(ser->sConnection, false, nil, 0, true);		// Close out the connectionhandle

				SetZone(myZone);
				ser_destroy(ser);

				resp->status = comm_STATUS_BUG;
				return FALSE;
			} else {
				error = CMAccept(ser->sConnection, true);							// We did get a call!
			}
		}
	}

	if ((req->flags & comm_INIT_FLAGS_TEST) && !(req->flags & comm_INIT_FLAGS_RESUME)) {
		// if -Ktest is given, and this is the initial connection,
		// do a one-second loopback test to see if data is getting through.
		exchange_ramp(ser->port, req->sessionId);									// Not implemented on the Mac.  Ever used on the PC?
	}

	DPRINT(("commInit: done\n"));
	resp->status = comm_STATUS_OK;
	fdprint("Connection successful.  End of log.\r\n");
	fdclose();

	SetZone(myZone);
	return TRUE;
}



/*
 *	Retrieve info about the communications driver.
 *
 *	Return TRUE if info was retrieved.
 */

int commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp)
{
	// We plan to make dpEnumTransports scan through the DLL file looking
	// for this signature.  It will use the info to present a driver name
	// to the user, and discover driver properties such as "expects to be
	// called by external shell (e.g. dwango)".
	static commDriverInfoResp_t	sCommDriverInfoResp =  {
		0,
		&gCommDriverInfo
	};


	if (resp == NULL) {
		return FALSE;
	}

	*resp = sCommDriverInfoResp;

	resp->status = ser_RES_OK;

	(void) req;

	return TRUE;
}



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

int commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp)
{
	static ser_adr_t	kludgeAdr;
	ser_hdl_t			h;

	if (resp == NULL) {
		return FALSE;
	}
	if (req == NULL) {
		resp->status = ser_RES_BAD;

		return FALSE;
	}

	
	if (req->player == PLAYER_ME) {
		h = ser_HDL_ME;
	} else if (req->player == PLAYER_YOU) {
		h = ser_HDL_YOU;
	} else {
		resp->status = ser_RES_FULL;

		return FALSE;
	}
	
    // index into nodes table and fill in response
	if (ser_hdl2adr(ser, h, &kludgeAdr) != ser_RES_OK) {
	//if (CTB_hdl2adr(ser, h, &kludgeAdr) != ser_RES_OK) {
		resp->status = ser_RES_EMPTY;

		return FALSE;
	}

    resp->status    = ser_RES_OK;
    resp->name      = NULL;
    resp->address   = &kludgeAdr;
    resp->addrLen   = sizeof(ser_adr_t);
    resp->nodeID[0] = (unsigned long) 0;
    resp->nodeID[1] = (unsigned long) 0;

    return TRUE;
}


/*
 *	Find out whether the transmit queue is full.
 *
 *	Return TRUE if no more packets can be queued for transmission at this
 *	time.
 */

int commTxFull(commTxFullReq_t *req, commTxFullResp_t *resp)
{
    commTxFullReq_t	reqDummy;
    commTxFullResp_t	respDummy;

    if (req == NULL) {
    	req = memset(&reqDummy, 0, sizeof(*req));
    }

    if (resp == NULL) {
    	resp = &respDummy;
    }

    // Not implemented.
    resp->status = ser_RES_FULL;

    return FALSE;
}


/*
 *	Send a packet.  Upon return, the buffer can be discarded, although the
 *	packet may not be sent until later.
 *
 *	Return FALSE on error.  Note that a TRUE return value does not guarantee
 *	that the packet has been (or ever will be) sent.
 */


int commTxPkt(commTxPktReq_t *req, commTxPktResp_t *resp)
{
	commTxPktResp_t   respDummy;
#ifdef PACKETLOGGING
	short					temp;
	size_t				actual;
#endif

	if (resp == NULL) {
		resp = &respDummy;
	}
	if (req == NULL) {
		resp->status = ser_RES_BAD;

		return FALSE;
	}

	// Abort if no good place to send packet.
	if ((req->dest != PLAYER_YOU || ser->yourAdr == ser_ADR_NONE) && (req->dest != PLAYER_BROADCAST)) {
		resp->status = ser_RES_FULL;

		return FALSE;
	}

#ifdef PACKETLOGGING
	memcpy(&temp, (char*)(req->buffer) + 5, 2);
	if (temp == 'MI') {
		actual = fwrite(req->buffer, 1, req->length, packetoutlog);
	}
#endif

   // if (ser_put(ser, req->buffer, req->length)) {
    if (CTB_put(ser, req->buffer, req->length)) {
		DPRINT(("commTxPkt: couldn't send packet\n"));
		resp->status = ser_RES_EMPTY;
	} else {
		resp->status = ser_RES_OK;
	}

    return (resp->status == ser_RES_OK);
}


/*
 *	Get information about a pending incoming packet.
 *
 *	Return TRUE if a packet was retrieved.
 */

int commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp)
{
    commPeekPktReq_t		reqDummy;
    commPeekPktResp_t	respDummy;

    if (req == NULL) {
    	req = memset(&reqDummy, 0, sizeof(*req));
    }
    if (resp == NULL) {
    	resp = &respDummy;
    }

	resp->status = ser_RES_FULL;

	DPRINT(("commPeekPkt: not implemented.\n"));

	return FALSE;
}


/*
 *	Retrieve a pending incoming packet.
 *
 *	Return TRUE if a packet was retrieved.
 */

int commRxPkt(commRxPktReq_t *req, commRxPktResp_t *resp)
{
	commRxPktReq_t    reqDummy;
	commRxPktResp_t   respDummy;
	ser_result_t		res;
#ifdef PACKETLOGGING
	short					temp;
	size_t				actual;
#endif

	if (req == NULL) {
		req = memset(&reqDummy, 0, sizeof(*req));
	}

	if (resp == NULL) {
		resp = &respDummy;
	}

	resp->length = req->size;
	//res = ser_get(ser, req->buffer, &resp->length);
	res = CTB_get(ser, req->buffer, &resp->length);

	resp->status = res;				// Fortunately, ser_RES_OK == 0
	resp->src    = PLAYER_YOU;

	if (resp->status == ser_RES_OK) {
		//DPRINT(("commRxPkt: GOOD PACKET\n"));
#ifdef PACKETLOGGING
		memcpy(&temp, ((char*)(req->buffer)) + 5, 2);
		if (temp == 'MI') {
			actual = fwrite(req->buffer, 1, resp->length, packetinlog);
		}
#endif
	} else if (resp->status != ser_RES_EMPTY) {
		DPRINT(("commRxPkt: BAD PACKET: %d\n", resp->status));
	}

	return (resp->status == ser_RES_OK);
}


/*
 *	Attempt to parse a NUL-terminated address string into a free-format
 *	buffer.
 *
 *	Return TRUE if the string was parsed successfully.
 */

int commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp)
{
   commScanAddrReq_t		reqDummy;
   commScanAddrResp_t	respDummy;
	ser_adr_t				adr;

	if (req == NULL) {
		req = memset(&reqDummy, 0, sizeof(*req));
	}
	if (resp == NULL) {
		resp = &respDummy;
	}

	// convert into our internal address
	if (sscanf(req->printable, "SERIAL:%x", &adr) != 1) {
		resp->status = ser_RES_FULL;

		return FALSE;
	}

	resp->length = sizeof(adr);

	if (req->address != NULL) {
		memcpy(req->address, &adr, min(req->size, resp->length));
	}

	resp->status = ser_RES_OK;

	return TRUE;
}


/*
 *	Attempt to format a free-format address buffer into a NUL-terminated
 *	string.
 *
 *	Return TRUE if the buffer was formatted successfully.
 */

int commPrintAddr( commPrintAddrReq_t *req, commPrintAddrResp_t *resp)
{
	commPrintAddrReq_t	reqDummy;
	commPrintAddrResp_t	respDummy;
	char						printable[50];

	if (req == NULL) {
		req = memset(&reqDummy, 0, sizeof(*req));
	}
	if (resp == NULL) {
		resp = &respDummy;
	}

	if ((req->length != sizeof(ser_adr_t)) || (req->address == NULL)) {
		DPRINT(("length != sizeof(ser_adr_t) or req-address == NULL\n"));
		resp->status = ser_RES_FULL;

		return FALSE;
	}

	sprintf(printable, "SERIAL:%x", *(ser_adr_t *) req->address);

	if (req->printable != NULL) {
		if (req->size < strlen(printable) + 1) {
			resp->status = ser_RES_EMPTY;

			return FALSE;
		}
		strcpy(req->printable, printable);
	}

	resp->status = ser_RES_OK;

	return TRUE;
}


/*
 *	Generate a pseudo-player handle referring to a group of players.  Handy
 *	for multicasting.  A group can have zero players.
 *
 *	Return TRUE if the pseudo-player handle was generated.
 */

int commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp)
{
	req = req;
	resp = resp;

	return FALSE;
}


/*
 *	Invalidate a pseudo-player handle referring to a group of players.
 *
 *	Return TRUE if the pseudo-player handle was invalidated.
 */

int commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp)
{
	req = req;
	resp = resp;

	return FALSE;
}


/*
 *	Add one or more players to a group.
 *
 *	Return TRUE if the players were all added.
 */

int commGroupAdd(commGroupAddReq_t *req, commGroupAddResp_t *resp)
{
   req = req;
   resp = resp;

   return FALSE;
}

#if 0 // MAD

/*
 *	Subtract one or more players from a group.  Do not delete the group,
 *	even if it is left empty.
 *
 *	Return TRUE unless there was a problem subtracting one or more players.
 */

int commGroupSubtract(commGroupSubtractReq_t *req, commGroupSubtractResp_t *resp)
{
   req = req;
   resp = resp;

   return FALSE;
}

#else

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

/*
 *	Establish a data link to a player and shake hands with him.  This does
 *	not actually establish a connection in the IP sense of the word.
 *
 *	Return TRUE if the link was established and we shook hands.
 *
 *  req->address is in one of the following formats:
 *
 *    for dwango setup:
 *      "DWANGO:<opponent player id>"
 *
 *    for null modem setup:
 *      "SERIAL:<opponent player id>"          
 *
 *    for modem setup:
 *      "SERIAL:<opponent player id>;<modem init and dial string>"
 *
 */


int commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp)
{
	commSayHiReq_t		reqDummy;
	commSayHiResp_t	respDummy;
	ser_hdl_t			h;

    if (req == NULL) {
    	req = memset(&reqDummy, 0, sizeof(*req));
    }
    if (resp == NULL) {
    	resp = &respDummy;
    }

    if (req->length == 0) {
		DPRINT(("commSayHi: req->length == 0"));
		resp->status = ser_RES_FULL;

		return FALSE;
	}

    h = ser_adr2hdl(ser, req->address, TRUE);
    //h = CTB_adr2hdl(ser, req->address, TRUE, -1);
	// Upper layer won't send packets to itself, but it can't tell
	// that until it sees PLAYER_ME.
   if (h == ser_HDL_ME) {
		resp->player = PLAYER_ME;
		resp->status = ser_RES_OK;

		return TRUE;
	}
   if (h == ser_HDL_YOU) {
		resp->player = PLAYER_YOU;
		resp->status = ser_RES_OK;

		return TRUE;
	}

	resp->status = ser_RES_EMPTY;
	return FALSE;
}



/*
 *	Tear down a data link to a player.  The link or the player may already be
 *	down, so don't shake hands.
 *
 *	Return TRUE if the link was successfully broken.
 */

int commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp)
{
    commSayByeReq_t	reqDummy;
    commSayByeResp_t	respDummy;

    if (req == NULL) {
    	req = memset(&reqDummy, 0, sizeof(*req));
    }
    if (resp == NULL) {
    	resp = &respDummy;
    }

    resp->status = ser_RES_OK;

    return TRUE;
}

Boolean commLoadTest(void);

Boolean commLoadTest(void)
{
	Boolean	result = false;
	long		value;
	OSErr		error;
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

#if 0 // MAD
Boolean commNeedsUI(commInitReq_t* req)
{
	Boolean	result = false;
	Str255	name;

	//	if this is a resume, we don't need the UI

	if (req->flags & comm_INIT_FLAGS_RESUME) {				// Just restarting a pre-existing connection; no UI needed
		result = false;
	} else 	if (req->flags & comm_INIT_CONFIG_ONLY) {		// Configuring the modem connection; needs a UI
		result = true;
	} else {															// Normal call to CommInit
		result = InitializeCommToolBox();
		if (result) {
			if (GetToolname(name) != noErr) {						// If there are no prefs in the file, we need a UI
				result = true;
			} else {														// Otherwise, we don't need it
				result = false;
			}
		}
	}
	
	return result;
}
#endif

#pragma export off
