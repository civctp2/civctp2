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
 Launches application specified by appParam.
 $Log: launchap.c $
 Revision 1.1  1997/08/15 19:05:02  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef __POWERPC__

#include <stdio.h>
#include <Strings.h>
#include <Files.h>
#include <Processes.h>
#include <Events.h>
#include <EPPC.h>
//#include <Sioux.h>
#include "dp.h"
#include "macapp.h"

#define REFCON 1092
//#define SLEEP_DEF 10
//#define MAX_WAIT 1000

/*--------------------------------------------------------------------------
 Waits for stub acknowledgement.  In order to work, need to find some way to
  get event from game's event loop.  Not implemented correctly now.
--------------------------------------------------------------------------*/
//static dp_result_t stubacknowledge(unsigned long msgRefcon) {
//	const int iwait = MAX_WAIT / SLEEP_DEF;
//	int i;
//	int done = 0;
//	unsigned long sendRefcon, msglen;
//	OSErr err;
//	EventRecord event;
//	while (!done) {		/* wait for return receipt */
//		WaitNextEvent(everyEvent, &event, SLEEP_DEF, nil);
//		if (event.message != HighLevelEventMsgClass || pttolong(event.where) != rtrnReceiptMsgID) {
//			SIOUXHandleOneEvent(&event);
//		} else {
//			msglen = 0;
//			err = AcceptHighLevelEvent(&sender, &sendRefcon, nil, &msglen);
//			if (err == bufferIsSmall) {
//				DPRINT(("dpLaunchApp: return receipt has nonzero message buffer\n"));
//				return dp_RES_BUG;
//			} else if (err == noErr) {
//				if (sendRefcon == msgRefcon) {
//					if (event.modifiers != msgWasFullyAccepted) {
//						DPRINT(("dpLaunchApp: launch request rejected by stub error %d\n", event.modifiers));
//						return dp_RES_BAD;
//					}
//					done = 1;
//				} else {
//					DPRINT(("dpLaunchApp: got return receipt from unknown source\n"));
//				}
//			}
//		}
//	}
	/* stub accepted launch request; now wait for it to finish or timeout to expire*/
//	for (i = 0, done = 0; !done && i < iwait; i++) {
//		WaitNextEvent(everyEvent, &event, SLEEP_DEF, nil);
//		if (event.message != STUB_EVENTID || pttolong(event.where) != STUB_LAUNCLASS) {
//			SIOUXHandleOneEvent(&event);
//		} else {
//			OSErr stub_err;
//			msglen = sizeof(OSErr);
//			err = AcceptHighLevelEvent(&sender, &sendRefcon, &stub_err, &msglen);
//			if (err == bufferIsSmall) {
//				DPRINT(("dpLaunchApp: high level event buffer too small\n"));
//				return dp_RES_BUG;
//			} else if (err == noErr) {
//				if (sendRefcon == msgRefcon) {
//					if (stub_err != noErr) {
//						DPRINT(("dpLaunchApp: stub launch error %d\n", stub_err));
//						return dp_RES_BAD;
//					}
//				}
//				if (sendRefcon == msgRefcon) done = 1;
//			}
//		}
//	}
//	if (i == iwait) {
//		DPRINT(("dpLaunchApp: wait for stub timed out\n"));
//		return dp_RES_BAD;
//	}
//}

/*--------------------------------------------------------------------------
 Launches an application from another application and notifies stub
  of the newly launched application, if stub exists.
--------------------------------------------------------------------------*/
DP_API dp_result_t dpLaunchApp(dp_appParam_t *appParam)
{
	static ProcessSerialNumber stubPSN;
	OSErr err;
	short myVRefNum;
	long myDirID;
	char buf[FILENAME_MAX];		// c2pstr alters its arg so use a buffer
	dp_result_t dperr;
	FSSpec appFSSpec;
	LaunchParamBlockRec launchParams;
	static ProcessSerialNumber launchPSN;

	/* set up launchParams for appParam->path */
	strcpy(buf, appParam->path);
	err = FSMakeFSSpec(0, 0, c2pstr(buf), &appFSSpec);
	if (err != noErr) {
		DPRINT(("dpLaunchApp: can't make FSSpec from path error %d\n", err));
		return dp_RES_BAD;
	}
	launchParams.launchBlockID = extendedBlock;
	launchParams.launchEPBLength = extendedBlockLen;
	launchParams.launchFileFlags = 0;
	launchParams.launchControlFlags = launchContinue + launchNoFileFlags;
	launchParams.launchAppSpec = &appFSSpec;
	launchParams.launchAppParameters = nil;
	dperr = getstub(&stubPSN);
	if (dperr == dp_RES_OK) {		/* have stub so ask stub to launch new process */
		TargetID sender;
		unsigned long msgRefcon = REFCON;
		unsigned long myOpts;
		EventRecord event;
		event.what = kHighLevelEvent;
		event.message = STUB_EVENTID;
		event.where = longtopt(STUB_LAUNCLASS);
//		myOpts = receiverIDisPSN + nReturnReceipt + nAttnMsg;
		myOpts = receiverIDisPSN + nAttnMsg;
		err = PostHighLevelEvent(&event, &stubPSN, msgRefcon, &launchParams, sizeof(launchParams), myOpts);
		if (err != noErr) {
			DPRINT(("post to stub to launch error %d\n", err));
			return dp_RES_BAD;
		}
//		err = stubacknowledge(msgRefcon)
//		if (err != noErr) {
//			return err;
//		}
	} else {					/* have no stub or got error while finding stub so launch here */
		err = LaunchApplication(&launchParams);
		if (err != noErr) {
			DPRINT(("dpLaunchApp: launching error %d\n", err));
			return dp_RES_BAD;
		} else {
			DPRINT(("dpLaunchApp: launched app\n"));
		}
	}
	return dp_RES_OK;
}

#endif
