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

/*************************************************************************************

                          Comm API Shims for Modem Protocol

                              (c) 1997 By Activision

**************************************************************************************/

// 31-OCT-96  File modified to support Win32  [M. Livesay]

// Wrapper to adapt the simple packetizing serial driver to Zobel's comm API.
//
// $Log: modmdll.c $ 
// Revision 1.11  1997/10/01 21:52:27  anitalee 
// return ser error values 
// Revision 1.10  1997/09/27 01:43:00  dkegel 
// Should allow freeze/thaw of modem sessions, as long as thaw-er is child of freeze-r. 
// Revision 1.9  1997/04/02 00:50:24  dkegel 
// 1. Changed default init string to one that gets it to do status 
// codes on us robotics (the kind that have dipswitches that turn status 
// codes off by default!) 
// 2. Don't check status until we've sent init string - 'cause on 
// some modems you can't! 
// 3. Fixed bug in checking return value of MyHMSendString() 
// 4. Always produce log file 
// Revision 1.8  1997/03/10 06:05:52  dkegel 
// 1. Jerry's unified serial/modem driver code. 
// 2. Dan hardcoded serial driver to ignore baud rate, always use 38400. 
// Revision 1.7  1997/03/09 02:50:55  dkegel
// Obey special 'stop' variable;
// obey baud rate;
// fix hangup
// revision 1.6  1997/03/08 01:45:38  dkegel
// Jerry's new modem driver.
// Revision 1.5  1997/03/01 21:04:30  dkegel 
// Got rid of compiler warning by if 0'ing a dprint 
// Revision 1.4  1997/01/31 02:44:19  dkegel 
// Updated dependencies, converted to ddprint.h. 
// Revision 1.3  1996/12/13 23:15:06  jgraebner 
// Fixed invalid pointer reference. 
// Revision 1.2  1996/12/13 00:16:25  dkegel 
// nodeID now a byte array instead of a structure.  Not used, just zero. 
// Revision 1.1  1996/12/12 22:09:32  dkegel 
// Initial revision 
// Revision 1.21  1996/09/10 05:46:23  dkegel 
// Don't rely on Greenleaf to know what IRQ to use- set it every time. 
// Fixes a bug where the IRQ setting last chosen overrides 
// new setting if new setting is 'DEFAULT'. 
// Revision 1.20  1996/09/06 23:08:18  dkegel 
// Replaced commGroupSubtract with commSetParam. 
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


/*************************************************************************************

                                    Literals

**************************************************************************************/

#include <windows.h>
#include <crtdbg.h>
#include "commapi.h"
#include <stdio.h>    // for sprintf and sscanf
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <fcntl.h>
#include <io.h>

#include "ser.h"
#include "regmo.h"
#include "commapi.h"
#include "anetstub.h"

//#define _DEBUG		/* force dprints even if release build */
#define DEBUG_MODULE  !FALSE
#include "ddprint.h"

#define __ERROR -1    // return value of open()

#define DRIVER_VERSION    VERSION(1,0)  // Version of this comm driver

#define PLAYER_YOU 0      // What we translate ser_HDL_YOU to.

/*************************************************************************************

                                    Globals

**************************************************************************************/

static ser_t *  ser            = NULL;
static int      already_hungup = FALSE;

/*************************************************************************************

                                  commNoOp()

**************************************************************************************/

/*
 *  Do nothing.
 *
 *  Return FALSE on error.
 */

DLLEXPORT int cdecl commNoOp(commNoOpReq_t *req, commNoOpResp_t *resp)

{
  resp->status = comm_STATUS_OK;
  (void) req;
  (void) resp;
  return(TRUE);
}

/*-------------------------------------------------------------------------
 Return true if the given com port is available.
-------------------------------------------------------------------------*/
static int commPortIsAvailable(const char *name)
{
	SECURITY_ATTRIBUTES SecurityAttributes;
	HANDLE h;

	memset(&SecurityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
	SecurityAttributes.nLength = sizeof( SECURITY_ATTRIBUTES ); 

	h = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
		0,                    // exclusive access
		&SecurityAttributes,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (INVALID_HANDLE_VALUE != h) {
		CloseHandle(h);
		return TRUE;
	}
	return FALSE;
}

/*-------------------------------------------------------------------------
 List the available comm ports.
 Implemented only for serial and modem drivers.
-------------------------------------------------------------------------*/
DLLEXPORT int cdecl
commEnumPorts(
	commEnumPortsReq_t *	req,		/* Request (or NULL) */
	commEnumPortsResp_t *	resp)		/* Response (or NULL) */
{
	int i;
	int nports;
	commPortName_t *pPort;
	int keys[10];
	int nkeys;
	regmo_t regmo;
	int err;

	if (!resp) {
		DPRINT(("commEnumPorts: bug: resp is null!\n"));
		return FALSE;
	}
	if (!req) {
		DPRINT(("commEnumPorts: bug: req is null!\n"));
		resp->numports = 0;
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	if (req->reqLen != sizeof(commEnumPortsReq_t)) {
		DPRINT(("commEnumPorts: bug: req is wrong size!\n"));
		resp->numports = 0;
		resp->status = comm_STATUS_BAD_VERSION;
		return FALSE;
	}

	nports = 0;
	pPort = req->buf;

	/* list available com ports */
	for (i=0; (i<10) && (nports < req->maxports); i++) {
		char			szPort[ 15 ];
		sprintf( szPort, "%s%d", "COM", i + 1);

		if (commPortIsAvailable(szPort)) {
			pPort->portnum = i;
			strncpy(pPort->name, szPort, sizeof(pPort->name));
			pPort->name[sizeof(pPort->name)-1] = 0;
			nports++;
			pPort++;
		}
	}

	/* list installed modems.  portnum is modemkey + 0x8000 */
	nkeys = regmo_enum(keys, 10);
	for (i=0; (i<nkeys) && (nports < req->maxports); i++) {
		err = regmo_get(&regmo, keys[i]);
		if (!err && commPortIsAvailable(regmo.AttachedTo)) {
			pPort->portnum = keys[i] + 0x8000;
			strncpy(pPort->name, regmo.DriverDesc, sizeof(pPort->name));
			pPort->name[sizeof(pPort->name)-1] = 0;
			nports++;
			pPort++;
		}
	}
	resp->numports = nports;
	resp->status = comm_STATUS_OK;
	DPRINT(("commEnumPorts: %d port(s) found.\n", nports));
	return TRUE;
}
		
/*-------------------------------------------------------------------------
 Notifies stub of new serial handle or ask it to destroy its copy (by
 passing INVALID_HANDLE_VALUE) so new one can be created.
-------------------------------------------------------------------------*/
static int notifystub(HANDLE hser)
{
	char strhandle[MAX_HSTR];
	char *pstrhandle;
	int nwrite;
	HANDLE hstubser, hstub, hpipe;
	unsigned long tag = DPSTUB_TAG_SERHDL;
	/* get stub handle values */
	if (!GetEnvironmentVariable(DPSTUB,strhandle,MAX_HSTR) ||
		(sscanf(strhandle, "%x", &hstub) != 1) ||
		!GetEnvironmentVariable(DPSTUBWPIPE,strhandle,MAX_HSTR) ||
		(sscanf(strhandle, "%x", &hpipe) != 1)) {
		DPRINT(("wmodem: err: couldn't get stub env vars\n"));
		return comm_STATUS_BAD;
	}
	DPRINT(("wmodem: found stub\n"));
	/* duplicate serial handle for stub if this is a new serial handle */
	if (hser != INVALID_HANDLE_VALUE) {
		if (!DuplicateHandle(GetCurrentProcess(), hser, hstub, &hstubser, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
			DPRINT(("wmodem: duplicate serial handle error %x.\n", GetLastError()));
			return comm_STATUS_BAD;
		}
	} else
		hstubser = hser;
	if (!WriteFile(hpipe,(char *)&tag,sizeof(tag),&nwrite,NULL) ||
		!WriteFile(hpipe,(char *)&hstubser,sizeof(hstubser),&nwrite,NULL)) {
		DPRINT(("wmodem: write to stub error %x\n",GetLastError()));
		return comm_STATUS_BAD;
	}
	/* set/unset environment variable to show that stub has/is deleting copy
	 * of serial handle; stub will do the same thing so that processes it
	 * launches have this set the same too */
	if (hser != INVALID_HANDLE_VALUE) {
		sprintf(strhandle, "%x", hstubser);
		pstrhandle = strhandle;
	} else
		pstrhandle = NULL;
	if (!SetEnvironmentVariable(DPSTUBMODEM, pstrhandle)) {
		DPRINT(("stub: set environment variable error %x.\n", GetLastError()));
		return comm_STATUS_BAD;
	}
	return comm_STATUS_OK;
}

/*************************************************************************************

                                  myHMInputLine()

**************************************************************************************/

/*-------------------------------------------------------------------------
 Read a line from the modem.
 Return when the line is ready, when the time runs out, or when the user
 hits a key.
 Return value is time remaining, or 0 if user hit a key.
 Time is in clock ticks, unlike the Greenleaf function of the similar name.
-------------------------------------------------------------------------*/

static int myHMInputLine(ser_t *ser, int timeout, char *buf, size_t buflen, long *stop)
{
	DWORD	dwReadLength;
	int	tStartTime = clock();
	char *p = buf;

	assert(buf && ser && buflen > 0);

	while (((long)(clock() - tStartTime) < timeout) && (buflen > 0)) {
		if (stop && *stop) {
			*p++ = 0;
			return 0;
		}

		if (serio_read(&ser->serio, p, 1, &dwReadLength) == serio_RES_OK) {
			if (1 == dwReadLength) {
				/*DPRINT(("myHMInputLine: got char '%c'\n", *p));*/
				if (*p == '\n' || *p == '\r')
					break;
				else if (*p != 0) {
					buflen--; 
					p++;
				}
			}
		}
	}
	*p++ = 0;

	timeout -= clock() - tStartTime;
	DPRINT(("myHMInputLine: Returns buffer '%s', time remaining %d\n", buf, timeout));
	return timeout;
}


static int logfd = __ERROR;

/*************************************************************************************

                                  fdclose()

**************************************************************************************/

static void fdclose(void)
{
  if (logfd != __ERROR)  close(logfd);
}


/*************************************************************************************

                                  fdprint()

**************************************************************************************/

void fdprint(char *msg)
{
  if (logfd != __ERROR)  write(logfd, msg, strlen(msg));
}

//#define MY_LOG		/* Enable this to force DPRINT's to go to modem.log */
#ifdef MY_LOG
/*--------------------------------------------------------------------------
 Call dp_dprintf_set(my_ddprintf) to force DPRINT's to go to our own log file.
--------------------------------------------------------------------------*/

int cdecl my_ddprintf(const char *	__format,  ...)
{
    int     len;
    char    buf[1024];
  	va_list	argptr;
  
    va_start(argptr, __format);
  
  	len = vsprintf(buf, __format, argptr);
  
  	va_end(argptr);
  
    fdprint(buf);
  
  	return(len);
}
#endif

/*************************************************************************************

                                  MyHMSendString()

**************************************************************************************/

// Send a command and get OK.
// Returns zero on success.

static int MyHMSendString(ser_t *ser, char *s, long *stop)
{
  char response[256];
  int ok = FALSE;
  serio_res_t err;
  int timeout = 10 * CLOCKS_PER_SEC;

  assert(ser && s);

  fdprint("MyHMSendString: sending '");
  fdprint(s);
  fdprint("'\r\n");

  serio_purge_read(&ser->serio);
  serio_purge_write(&ser->serio);

  strcpy(response, s);
  strcat(response, "\r");
  err = serio_write(&ser->serio, response, strlen(response));
  if (err != serio_RES_OK) {
	  DPRINT(("MyHMSendString: serio_write failed, err:%d\n", err));
	  return TRUE;
  }

  do {
	  if (stop && *stop)
		  return TRUE;

	  DPRINT(("myHMSendString: timeout = %d\n", timeout));
	  timeout = myHMInputLine(ser, timeout, response, sizeof(response), stop);
	  if (timeout <= 0) {
		  DPRINT(("myHMSendString: Timedout\n"));
		  break;
	  } else {
		  fdprint("got '");
		  fdprint(response);
		  fdprint("'\r\n");
		  if (!strncmp(response, "OK", sizeof("OK") - 1)) ok = TRUE;
		  if (!strncmp(response, "0", sizeof("0") - 1)) ok = TRUE;
		  DPRINT(("myHMSendString: response is '%s'\n", response));
	  }
  } 
  while (!ok);

  return !ok;
}

/*************************************************************************************

                                  MyHMDial()

**************************************************************************************/

// Send a command and get OK.
// Returns zero on success.

static int MyHMDial(ser_t *ser, char *s, int dialing_method)
{
  char buf[256];
  serio_res_t err;

  assert(ser && s);

  if (dialing_method)
	  strcpy(buf, "ATDT");
  else
	  strcpy(buf, "ATDP");

  strcat(buf, s);
  strcat(buf, "\r");

  fdprint("sending '");
  fdprint(buf);
  fdprint("'\r\n");

  serio_purge_read(&ser->serio);
  serio_purge_write(&ser->serio);

  err = serio_write(&ser->serio, buf, strlen(buf));
  if (err != serio_RES_OK) {
	  DPRINT(("MyHMDial: serio_write failed, err:%d\n", err));
	  return TRUE;
  }
  return FALSE;
}


/*************************************************************************************

                                  MyHMReset()

**************************************************************************************/

// Reset the modem.
// Tries ntries times.
// Returns zero on success.
static int MyHMReset(ser_t *ser, int ntries, long *stop)

{
	int i;
	
	for (i = 0; i < ntries; i++)
	{
		if (stop && *stop)
			return 1;

		if (!MyHMSendString(ser, "ATZ", stop))
			return 0;
	}
	return 1;
}


/*************************************************************************************

                                  HangUp()

**************************************************************************************/

void serio_Sleep(serio_t *serio, int ms) 
{
	while (ms > 0) {
		serio_poll(serio);
		Sleep(50);
		ms -= 50;
	}
}

/*-------------------------------------------------------------------------
 Get the modem back in command mode, and hang up the line.
-------------------------------------------------------------------------*/

static void HangUp(void)

{
	int err;
	int i;

	DPRINT(("HangUp: Starting at %ld\n", time(0)));

	if (!already_hungup) 
	{
		for (i = 0; i<2; i++) 
		{
			DPRINT(("HangUp: Trying to hang up %ld\n", time(0)));

			serio_purge_write(&ser->serio);

			serio_Sleep(&ser->serio, 1000);
			if (serio_write(&ser->serio, "+++", 3) != serio_RES_OK)
				return;
			serio_purge_read(&ser->serio);
			serio_Sleep(&ser->serio, 3000);
			DPRINT(("HangUp: Resetting %ld\n", time(0)));
			err = MyHMReset(ser, 1, 0);
			DPRINT(("HangUp: DONE Resetting %ld\n", time(0)));
			if (err == 0) break;
		}
		DPRINT(("HangUp: Sending hangup string %ld\n", time(0)));
		
		serio_Sleep(&ser->serio, 1000);
		err = MyHMSendString(ser, "ATH0", 0);
		DPRINT(("HangUp: DONE Sendint hangup string %ld\n", time(0)));
	}
	
	EscapeCommFunction(serio_get_handle(&ser->serio), CLRDTR); // hope this hangs up modem and puts us in command mode
	DPRINT(("HangUp: Done at %ld\n", time(0)));
}


/*************************************************************************************

                                  commInit()

**************************************************************************************/

/*-------------------------------------------------------------------------
 Initialize the communications driver.

 Return FALSE on error.

 Required parameters:
 req->sessionId
 req->baud
 req->portnum
 req->phonenum;
 req->modeministr;
 req->flags;	if (req->flags & comm_INIT_FLAGS_RESUME), connection already up
 req->dialing_method;	either tone or pulse

 Optional Parameters (set to 0 if unused):
 req->hwirq = set to 12345 if swirq is to be obeyed
 req->swirq = pointer to volatile abort flag; if *(char *)req->swirq, 
  dialing aborted
 
 req->baseadr;      If nonzero, use this as the comm port handle!

 On exit,
 sets req->baseadr to the comm port handle!  Intended to be written to
 a file & passed in by a child process to its commInit so it can resume
 communications.
-------------------------------------------------------------------------*/

DLLEXPORT int cdecl commInit(commInitReq_t *req, commInitResp_t *resp)

{
	long *stop;
	commInitResp_t  respDummy;
	int				timeout;
	char            response[256];
	char *          init = "ATQ0V1&K0";  // Hangup, cancel autoanswer if DTR drops; rts/cts flow control (from USR sportster manual)
	char            message[256];
	char strhandle[MAX_HSTR];
	regmo_t regmo;
	int use_regmo;
	int i;
	int mode;
	int bNewConnection = 0;
	
	already_hungup = FALSE;
	if (resp == NULL) resp = &respDummy;
	
	if (!req) 
	{
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	
	if (req->reqLen != sizeof(commInitReq_t)) 
	{
		resp->status = comm_STATUS_BAD_VERSION;
		return FALSE;
	}
	
	ser = ser_create();
	if (!ser) 
	{
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	
	if (ser_adr2hdl(ser, &req->sessionId, TRUE) != ser_HDL_ME) 
	{
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}
	use_regmo = FALSE;
	memset(&regmo, 0, sizeof(regmo));
	if (req->baseadr) {
		/* use handle from freeze file */
		serio_open_handle(&ser->serio, (HANDLE)req->baseadr);
		DPRINT(("wmodem:commInit: reusing old handle %x\n", req->baseadr));
	} else if (GetEnvironmentVariable(DPSTUBMODEM, strhandle, MAX_HSTR)) {
		/* use handle value set by stub */
		DPRINT(("wmodem: found serial handle set by stub\n"));
		sscanf(strhandle, "%x", &(req->baseadr));
		req->flags |= comm_INIT_FLAGS_RESUME;
		serio_open_handle(&ser->serio, (HANDLE)req->baseadr);
		DPRINT(("wmodem:commInit: reusing old handle %x\n", req->baseadr));
	} else {
		char szPortName[64];
		int err;

		if (req->portnum >= 0x8000) {
			err = regmo_get(&regmo, req->portnum - 0x8000);
			if (err) {
				DPRINT(("wmodem:commInit: Can't find modem for portnum %x\n", req->portnum));
				resp->status = comm_STATUS_BAD;
				return FALSE;
			}
			use_regmo = TRUE;
			strcpy(szPortName, regmo.AttachedTo);	
		} else {
			sprintf(szPortName, "COM%d", req->portnum + 1);

#if 0	/* For gateway 2000 "Extreme Games" HG1.1 bundle - drk */
			{
			/* If user happens to pick a com port which has an
			 * attached winmodem, use its init string
			 */
			int keys[10];
			int nkeys;
			int i;
			nkeys = regmo_enum(keys, 10);
			for (i=0; i<nkeys; i++) {
				err = regmo_get(&regmo, keys[i]);
				if (!err) {
					char szPortNamelc[64];
					strcpy(szPortNamelc, szPortName);
					strlwr(szPortNamelc);

					if (strstr(regmo.AttachedTo, szPortName)
					||  strstr(regmo.AttachedTo, szPortNamelc)) {
						/* Use the init string from registry! */
						use_regmo = TRUE;
						DPRINT(("wmodem: User selected com name %s for winmodem, using regmo init string!\n", szPortName));
						break;
					}
				}
			}
			}
#endif
		}
		if ((resp->status = ser_config(ser, req->baud, szPortName)) != ser_RES_OK)
			return FALSE;
		req->baseadr = (long) serio_get_handle(&ser->serio);
		if (GetEnvironmentVariable(DPSTUB,strhandle,MAX_HSTR))
			bNewConnection = 1;
	}
	
	mode = O_RDWR | O_CREAT | O_BINARY;
	if (req->flags & comm_INIT_FLAGS_RESUME) 
		mode |= O_APPEND;
	else
		mode |= O_TRUNC;
    logfd = open("modem.log", mode, 0644);
    sprintf(message, "log started. (mode %o, sessionId %x)\r\n", mode, req->sessionId);
    fdprint(message);
	
#ifdef MY_LOG
	dp_dprintf_set(my_ddprintf);
#endif

	if (req->flags & comm_INIT_FLAGS_RESUME) 
	{
		fdprint("Connection resumed.  End of log.\r\n");
#ifndef MY_LOG
		fdclose();
#endif
		resp->status = comm_STATUS_OK;
		return TRUE;    // don't dial
	}

	if (req->modeministr && *req->modeministr)  init = req->modeministr;
	if (!use_regmo) {
		strcpy(regmo.Init[0], init);
	}

	// Talk to the modem in command mode
	if (!EscapeCommFunction(serio_get_handle(&ser->serio), SETDTR))
		return FALSE;
	
	/* Set up a variable to look at if the user wants to stop dialing or answering */
	if (12345 == req->hwirq)
		stop = (long *) req->swint;
	else
		stop = 0;
	
    /* Don't check result of reset, since some modems don't return result
	 * codes by default (e.g. USRobotics that have dipswitches)
	 */
	MyHMReset(ser, 2, stop);
	if (stop && *stop) {
		resp->status = comm_STATUS_NO_RESPONSE;
	    ser_destroy(ser);
	    ser = NULL;
		fdclose();
		return FALSE;
	}

    /* Send an init string that hopefully gets the modem to do status codes */
	DPRINT(("commInit: about to send init strings... regmo.Init[0] = %s\n", regmo.Init[0]));
	for (i=0; (i<regmo_N_INIT) && regmo.Init[i][0]; i++) {
		fdprint("sending '");
		fdprint(regmo.Init[i]);
		fdprint("'\r\n");
		serio_purge_read(&ser->serio);
		serio_purge_write(&ser->serio);
		if (MyHMSendString(ser, regmo.Init[i], stop)) 
		{
			fdprint("modem returns an error!\n");
			resp->status = comm_STATUS_BAD_INITSTRING;
			ser_destroy(ser);
			ser = NULL;
			fdclose();
			return FALSE;
		}
		if (stop && *stop) {
			resp->status = comm_STATUS_NO_RESPONSE;
			ser_destroy(ser);
			ser = NULL;
			fdclose();
			return FALSE;
		}
	}

	serio_purge_read(&ser->serio);
	serio_purge_write(&ser->serio);
	// If we are the originator, initiate the conversation
	if (req->phonenum && req->phonenum[0] && req->phonenum[0] != '-') 
	{
		fdprint("dialing '");
		fdprint(req->phonenum);
		fdprint("'\r\n");
		if (MyHMDial(ser, req->phonenum, req->dialing_method))
		{
			fdprint("modem returns an error!\n");
			resp->status = comm_STATUS_NO_RESPONSE;
			commTerm(NULL, NULL);
			fdclose();
			return FALSE;
		}
	} 
	else 
	{  // If we are the answerer, answer the phone
		fdprint("setting autoanswer register to 1\r\n");
		if (MyHMSendString(ser, "ATS0=1", stop)) 
		{
			fdprint("modem returns an error!\n");
			resp->status = comm_STATUS_NO_RESPONSE;
			commTerm(NULL, NULL);
			fdclose();
			return FALSE;
		}
	}
	timeout = 60 * CLOCKS_PER_SEC;
	if (stop && *stop) {
		resp->status = comm_STATUS_NO_RESPONSE;
		commTerm(NULL,NULL);
		fdclose();
		return FALSE;
	}
	
	do {  // Wait for the connection to be established
		int err;
		
		timeout = myHMInputLine(ser,timeout,response,sizeof(response), stop);
		if (timeout < 0) {
			fdprint("timeout!\r\n");
			resp->status = comm_STATUS_NO_RESPONSE;
			commTerm(NULL, NULL);
			fdclose();
			return FALSE;
		}
		if (stop && *stop) {
			resp->status = comm_STATUS_NO_RESPONSE;
			commTerm(NULL,NULL);
			fdclose();
			return FALSE;
		}
		
		fdprint("got '");
		fdprint(response);
		fdprint("'\r\n");
		err = 0;    // Keep looping if not obvious error
		if      (!strncmp(response, "BUSY",         sizeof("BUSY") - 1)        )
			err = comm_STATUS_BUSY;
		else if (!strncmp(response, "ERROR",        sizeof("ERROR") - 1)       )
			err = comm_STATUS_NO_RESPONSE;
		else if (!strncmp(response, "NO DIALTONE",  sizeof("NO DIALTONE") - 1) )
			err = comm_STATUS_NO_DIALTONE;
		else if (!strncmp(response, "NO DIAL TONE", sizeof("NO DIAL TONE") - 1))
			err = comm_STATUS_NO_DIALTONE;
		else if (!strncmp(response, "NO ANSWER",    sizeof("NO ANSWER") - 1)   )
			err = comm_STATUS_NO_ANSWER;
		
		if (err) {
			fdprint("modem reports call not completed.\n");
			resp->status = err;
			commTerm(NULL, NULL);
			fdclose();
			return FALSE;
		}
	} 
	while (strncmp(response, "CONNECT", sizeof("CONNECT") - 1) != 0);
	
	resp->status = comm_STATUS_OK;
	DPRINT(("wmodem:commInit: using new handle %x\n", req->baseadr));
	if (bNewConnection &&
		((resp->status = notifystub((HANDLE)req->baseadr)) != comm_STATUS_OK)) {
		DPRINT(("wmodem:commInit: cannot notify stub\n"));
		commTerm(NULL, NULL);
		return FALSE;
	}
	fdprint("Connection successful.  End of log.\r\n");
#ifndef MY_LOG
	fdclose();
#endif
	
	return TRUE;
}


/*************************************************************************************

                                  commTerm()

**************************************************************************************/

/*
 *  Tear down the communications driver.  
 *
 *  Return FALSE on error.
 */

DLLEXPORT int cdecl commTerm(commTermReq_t *req, commTermResp_t *resp)

{
  if (ser && (!req || (req->flags == 0))) 
  {
	char strhandle[MAX_HSTR];
    DPRINT(("commTerm: hanging up\n"));
    HangUp();
	/* if stub has serial handle, ask it to close it */
	if (GetEnvironmentVariable(DPSTUBMODEM, strhandle, MAX_HSTR))
		notifystub(INVALID_HANDLE_VALUE);
  }

  if (ser) 
  {
    DPRINT(("commTerm: destroying ser\n"));
    ser_destroy(ser);
    ser = NULL;
  }

  (void) req;
  (void) resp;
  DPRINT(("commTerm: finished\n"));

#ifdef MY_LOG
	fdclose();
#endif
  return(TRUE);
}



/*************************************************************************************

                               commDriverInfo()

**************************************************************************************/

/*
 *  Retrieve info about the communications driver.
 *
 *  Return TRUE if info was retrieved.
 */

DLLEXPORT int cdecl commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp)

{
  // We plan to make dpEnumTransports scan through the DLL file looking
  // for this signature.  It will use the info to present a driver name
  // to the user, and discover driver properties such as "expects to be
  // called by external shell (e.g. dwango)".
  static comm_driverInfo_t commDriverInfo =
  {
    comm_DRIVER_SIGNATURE,
    sizeof(comm_driverInfo_t),
    "Modem",
    0x0100,
    (comm_DRIVER_IS_VISIBLE | comm_DRIVER_ALLOWS_GAMELIST),
    (comm_INIT_NEEDS_SESSIONID | comm_INIT_NEEDS_PORTNUM| comm_INIT_NEEDS_PHONENUM | comm_INIT_NEEDS_BAUD)
  };

  static commDriverInfoResp_t commDriverInfoResp =
  {
    0,
    &commDriverInfo
  };

  if (resp == NULL) return FALSE;
  *resp = commDriverInfoResp;

  resp->status = 0;

  (void) req;

  return (TRUE);
}



/*************************************************************************************

                               commPlayerInfo()

**************************************************************************************/

/*
 *  Retrieve info about a player, including ourselves.
 *
 *  Return TRUE if the player is known and is not a group.
 *
 *  Given a player handle returned by commSayHi, you can get back
 *  which network address it is.  The network address is portable-
 *  that is, it must be unique among all computers currently connected,
 *  and it must retain its meaning when put in a packet and sent to
 *  another station.
 */

DLLEXPORT int cdecl commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp)

{
  static ser_adr_t kludgeAdr;
  ser_hdl_t h;


  if (resp == NULL)  return(FALSE);

  if (req == NULL) 
  {
    resp->status = ser_RES_BAD;
    return(FALSE);
  }

  if (req->player == PLAYER_ME)
    h = ser_HDL_ME;
  else if (req->player == PLAYER_YOU)
    h = ser_HDL_YOU;
  else 
  {
    resp->status = 1;
    return FALSE;
  }

    // index into nodes table and fill in response
  if ((resp->status = ser_hdl2adr(ser, h, &kludgeAdr)) != ser_RES_OK) 
  {
    return FALSE;
  }

  resp->status    = 0;
  resp->name      = NULL;
  resp->address   = &kludgeAdr;
  resp->addrLen   = sizeof(ser_adr_t);
  memset(&resp->nodeID, 0, sizeof(resp->nodeID));

  return(TRUE);
}


/*************************************************************************************

                               commTxFull()

**************************************************************************************/

/*
 *  Find out whether the transmit queue is full.
 *
 *  Return TRUE if no more packets can be queued for transmission at this
 *  time.
 */

DLLEXPORT int cdecl commTxFull(commTxFullReq_t *req, commTxFullResp_t *resp)

{
  commTxFullReq_t reqDummy;
  commTxFullResp_t  respDummy;


  if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL)  resp = &respDummy;

  // Not implemented.
  resp->status = 1;

  return(FALSE);
}


/*************************************************************************************

                               commTxPkt()

**************************************************************************************/

/*
 *  Send a packet.  Upon return, the buffer can be discarded, although the
 *  packet may not be sent until later.
 *
 *  Return FALSE on error.  Note that a TRUE return value does not guarantee
 *  that the packet has been (or ever will be) sent.
 */


DLLEXPORT int cdecl commTxPkt( commTxPktReq_t *req, commTxPktResp_t *resp)

{
  commTxPktResp_t   respDummy;


  if (resp == NULL) resp = &respDummy;
  if (req == NULL) 
  {
    resp->status = 3;
    return FALSE;
  }

  //DPRINT(("commTxPkt: dest h:%x, length %d\n", req->dest, req->length));
  // Abort if no good place to send packet.
  if ((req->dest != PLAYER_YOU || ser->yourAdr == ser_ADR_NONE) && (req->dest != PLAYER_BROADCAST)) 
  {
    resp->status = 1;
    return FALSE;
  }

  resp->status = ser_put(ser, req->buffer, req->length);

  return(resp->status == 0);
}


/*************************************************************************************

                               commPeekPkt()

**************************************************************************************/

/*
 *  Get information about a pending incoming packet.
 *
 *  Return TRUE if a packet was retrieved.
 */

DLLEXPORT int cdecl commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp)

{
  commPeekPktReq_t  reqDummy;
  commPeekPktResp_t respDummy;

  if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  resp->status = 1;

  return(FALSE);
}


/*************************************************************************************

                               commRxPkt()

**************************************************************************************/

/*
 *  Retrieve a pending incoming packet.
 *
 *  Return TRUE if a packet was retrieved.
 */

DLLEXPORT int cdecl commRxPkt(commRxPktReq_t *req, commRxPktResp_t *resp)

{
  commRxPktReq_t   reqDummy;
  commRxPktResp_t  respDummy;
  ser_result_t     res;

  if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  resp->length = req->size;
  res = ser_get(ser, req->buffer, &resp->length);
#if 0
  if (res == ser_RES_OK) {
	  DPRINT(("commRxPkt: got packet, length %d\n", resp->length));
  }
#endif

  resp->status = res;     // Fortunately, ser_RES_OK == 0
  resp->src    = PLAYER_YOU;

  return (resp->status == ser_RES_OK);
}


/*************************************************************************************

                               commScanAddr()

**************************************************************************************/

/*
 *  Attempt to parse a NUL-terminated address string into a free-format
 *  buffer.
 *
 *  Return TRUE if the string was parsed successfully.
 */

DLLEXPORT int cdecl commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp)

{
  commScanAddrReq_t  reqDummy;
  commScanAddrResp_t respDummy;
  ser_adr_t          adr;


  if (req == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  // convert into our internal address
  if (sscanf(req->printable, "SERIAL:%x", &adr) != 1) 
  {
    resp->status = 1;
    return(FALSE);
  }

  resp->length = sizeof(adr);

  if (req->address != NULL) memcpy(req->address, &adr, min(req->size, resp->length));

  resp->status = 0;

  return (TRUE);
}


/*************************************************************************************

                               commScanAddr()

**************************************************************************************/

/*
 *  Attempt to format a free-format address buffer into a NUL-terminated
 *  string.
 *
 *  Return TRUE if the buffer was formatted successfully.
 */

DLLEXPORT int cdecl commPrintAddr(commPrintAddrReq_t *req, commPrintAddrResp_t *resp)

{
  commPrintAddrReq_t  reqDummy;
  commPrintAddrResp_t respDummy;
  char                printable[50];


  if (req ==  NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  if ((req->length != sizeof(ser_adr_t)) || (req->address == NULL)) 
  {
    resp->status = 1;

    return(FALSE);
  }

  sprintf(printable, "SERIAL:%x", *(ser_adr_t *) req->address);

  if (req->printable != NULL) 
  {
    if (req->size < strlen(printable) + 1) 
    {
      resp->status = 2;
      return(FALSE);
    }

    strcpy(req->printable, printable);
  }

  resp->status = 0;

  return(TRUE);
}


/*************************************************************************************

                               commGroupAlloc()

**************************************************************************************/

/*
 *  Generate a pseudo-player handle referring to a group of players.  Handy
 *  for multicasting.  A group can have zero players.
 *
 *  Return TRUE if the pseudo-player handle was generated.
 */

DLLEXPORT int cdecl commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp)

{
  req  = req;
  resp = resp;

  return(FALSE);
}


/*************************************************************************************

                               commGroupFree()

**************************************************************************************/

/*
 *  Invalidate a pseudo-player handle referring to a group of players.
 *
 *  Return TRUE if the pseudo-player handle was invalidated.
 */

DLLEXPORT int cdecl commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp)

{
  req  = req;
  resp = resp;

  return (FALSE);
}


/*************************************************************************************

                               commGroupAdd()

**************************************************************************************/

/*
 *  Add one or more players to a group.
 *
 *  Return TRUE if the players were all added.
 */

DLLEXPORT int cdecl commGroupAdd(commGroupAddReq_t *req, commGroupAddResp_t *resp)
{
    req = req;
    resp = resp;

    return(FALSE);
}


/*************************************************************************************

                               commGroupAdd()

**************************************************************************************/

/*
 *  Set driver parameters.
 */

DLLEXPORT int cdecl commSetParam(commSetParamReq_t * req, commSetParamResp_t *  resp)

{
	commSetParamReq_t reqDummy;
	commSetParamResp_t  respDummy;

	if (req  == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL) resp = &respDummy;

	switch (req->param_num) {
	case comm_PARAM_DPRINTF:
#ifndef MY_LOG
		//DPRINT(("commSetParam: calling dp_dprintf_set\n"));
		dp_dprintf_set((dp_dprintf_t) req->param_value);
		//DPRINT(("commSetParam: return from dp_dprintf_set\n"));
#endif
		break;
	default:
		resp->status = comm_STATUS_UNIMPLEMENTED;
		return FALSE;
	}

	DPRINT(("commSetParam: set param 0x%x to 0x%x\n", req->param_num, req->param_value));

	resp->status = 0;
	return TRUE;
}



/*************************************************************************************

                               commSayHi()

**************************************************************************************/

/*
 *  Establish a data link to a player and shake hands with him.  This does
 *  not actually establish a connection in the IP sense of the word.
 *
 *  Return TRUE if the link was established and we shook hands.
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

DLLEXPORT int cdecl commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp)

{
  commSayHiReq_t  reqDummy;
  commSayHiResp_t respDummy;
  ser_hdl_t h;


  if (req  == NULL)  req = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  if (req->length == 0) 
  {
    resp->status = 1;

    return FALSE;
  }

  h = ser_adr2hdl(ser, req->address, TRUE);
  // Upper layer won't send packets to itself, but it can't tell
  // that until it sees PLAYER_ME.
  if (h == ser_HDL_ME) 
  {
    resp->player = PLAYER_ME;
    resp->status = 0;
    return TRUE;
  }

  if (h == ser_HDL_YOU) 
  {
    resp->player = PLAYER_YOU;
    resp->status = 0;
    return TRUE;
  }

  resp->status = 2;

  return FALSE;
}



/*************************************************************************************

                               commSayBye()

**************************************************************************************/

/*
 *  Tear down a data link to a player.  The link or the player may already be
 *  down, so don't shake hands.
 *
 *  Return TRUE if the link was successfully broken.
 */

DLLEXPORT int cdecl commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp)

{
  commSayByeReq_t  reqDummy;
  commSayByeResp_t respDummy;


  if (req  == NULL) req  = memset(&reqDummy, 0, sizeof(*req));
  if (resp == NULL) resp = &respDummy;

  resp->status = 0;

  return (TRUE);
}

/*************************************************************************************

                               DllMain()

**************************************************************************************/

BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)

{
  switch (fdwReason)
  {
    case DLL_PROCESS_ATTACH:
      // do any startup stuff here
    return(TRUE);
 
    case DLL_PROCESS_DETACH:
      // do any shutdown stuff here
    return(TRUE);

    case DLL_THREAD_ATTACH:
    return(TRUE);
 
    case DLL_THREAD_DETACH:
    return(TRUE);
  }
 
  return(TRUE);
}
