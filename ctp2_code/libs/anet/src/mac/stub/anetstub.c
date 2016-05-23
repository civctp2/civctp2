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
 Stub process for NetShell.
 $Log: anetstub.c $
 Revision 1.2  1997/08/19 23:40:27  anitalee
 defined DPRINT
 Revision 1.1  1997/08/16 18:06:02  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef __POWERPC__

#include <stdio.h>
#include <EPPC.h>
#include <Files.h>
#include <Processes.h>
#include <Events.h>
#include <Strings.h>
#include <Sioux.h>
#include "::dp:macapp.h"

#define SHELL_NAME "\p::::Mac:dll:gtest"
//#define SHELL_NAME "\pMacintosh HD:dan:anetsrc Folder:src:mac:stub:tmp:ToolboxPPC"
#define SLEEP_DEF 6000

#define DPRINT(s) printf s

int nApps = 0;
//OSErr exitcode = noErr;
Boolean relaunch = true;

/*--------------------------------------------------------------------------
 Decrements the launched application count for app died event; used by apple event handler.
 Commented out section gets the exit code of died event; not implemented correcly now.
--------------------------------------------------------------------------*/
pascal OSErr myHandleAppDied(AppleEvent *event, AppleEvent *reply, long refcon)
{
//	DescType dType;
//	Size size;
//	OSErr err, status;

//	err = AEGetParamPtr(event, keyErrorNumber, typeLongInteger, &dType, &status, sizeof(OSErr), &size);
//	if (err == noErr) {
//		exitcode = status;
//		DPRINT(("got app died exit code %d\n", status));
//	} else {
//		DPRINT(("getting died app exit status error %d\n", err));
//		exitcode = err;
//	}
	nApps--;
//	return err;
	return noErr;
}

/*--------------------------------------------------------------------------
 Launches application as specified by input launchParams.
--------------------------------------------------------------------------*/
OSErr launchapp(LaunchParamBlockRec *launchParams)
{
	OSErr err;
	err = LaunchApplication(launchParams);
	if (err != noErr) {
		DPRINT(("launching error %d\n", err));
	}
	return err;
}

/*--------------------------------------------------------------------------
 High level event handler for input event.
 For launch requests ('LAUN'), launches the application as specified in the event message.
 For app die request netshell relaunch ('OBIT'), sets flag for netshell relaunch.
 For app died events, decrements app count.
 Any other events are passed on to the Sioux handler.
--------------------------------------------------------------------------*/
OSErr myHandleHLEvent(EventRecord *event)
{
	OSErr err;
	if (event->message == STUB_EVENTID && pttolong(event->where) == STUB_LAUNCLASS) {
		LaunchParamBlockRec launchParams;
		TargetID sender;
		unsigned long msgRefcon, msglen;
//		unsigned long sendRefcon, myOpts;
		msglen = sizeof(LaunchParamBlockRec);
		err = AcceptHighLevelEvent(&sender, &msgRefcon, &launchParams, &msglen);
		if (err != noErr) {
			DPRINT(("accept high level event error %d\n", err));;
		} else if (err == noErr) {
			err = launchapp(&launchParams);
			if (err == noErr) nApps++;
		}
		/* notify shell of launch status*/
//		myOpts = receiverIDisSessionID + nAttnMsg;
//		err = PostHighLevelEvent(event, sender.sessionID, msgRefcon, &err, sizeof(err), myOpts);
//		if (err != noErr) {
//			DPRINT(("post shell about launch error %d\n", err));
//		}
		DPRINT(("got launch request\n"));
	} else if (event->message == STUB_EVENTID && pttolong(event->where) == STUB_OBITCLASS) {
		TargetID sender;
		unsigned long msgRefcon;
		unsigned long msglen = 0;
		err = AcceptHighLevelEvent(&sender, &msgRefcon, nil, &msglen);
		if (err != noErr) {
			DPRINT(("accept high level event error %d\n", err));;
		} else {
			relaunch = true;
		}
		DPRINT(("got obit notice with relaunch = %d\n", relaunch));
	} else if (event->message == kCoreEventClass && pttolong(event->where) == kAEApplicationDied) {
		err = AEProcessAppleEvent(event);
		DPRINT(("got app died event\n"));
	} else {
		SIOUXHandleOneEvent(event);
		err = noErr;
	}
	return err;
}

/*--------------------------------------------------------------------------
 Launches NetShell and any processes given it by NetShell.
 When all of these launched processes dies, relaunches NetShell if none of the
  processes exited with non-zero exit status.
 NetShell path is currently given in SHELL_NAME; could change to alias in pref
  folder later on.
--------------------------------------------------------------------------*/
void main(void)
{
	OSErr err;
	FSSpec ShellFSSpec;
	LaunchParamBlockRec launchShellParams;
	AEEventHandlerUPP myHandleAppDiedUPP;

	DPRINT(("this is your stub speaking; please fasten your seat belts and enjoy the ride\n"));
	myHandleAppDiedUPP = NewAEEventHandlerProc(myHandleAppDied);
	err = AEInstallEventHandler(kCoreEventClass, kAEApplicationDied, myHandleAppDiedUPP, 0L, false);
	if (err != noErr) {
		DPRINT(("can't install event handler error %d\n", err));
		return;
	}
	err = FSMakeFSSpec(0, 0, SHELL_NAME, &ShellFSSpec);
	if (err != noErr) {
		DPRINT(("can't make FSSpec for shell error %d\n", err));
		return;
	}
	launchShellParams.launchBlockID = extendedBlock;
	launchShellParams.launchEPBLength = extendedBlockLen;
	launchShellParams.launchFileFlags = 0;
	launchShellParams.launchControlFlags = launchContinue + launchNoFileFlags;
	launchShellParams.launchAppSpec = &ShellFSSpec;
	launchShellParams.launchAppParameters = nil;
//	while (exitcode == noErr) {
	while (relaunch == true) {
		relaunch = false;
		err = launchapp(&launchShellParams);
		if (err != noErr) return;
		nApps++;
		while (nApps > 0) {
			EventRecord event;
			if (WaitNextEvent(everyEvent, &event, SLEEP_DEF, nil)) {
				if (event.what == kHighLevelEvent) {
					err = myHandleHLEvent(&event);
				} else {
					SIOUXHandleOneEvent(&event);
				}
			}
		}
	}
}

#endif
