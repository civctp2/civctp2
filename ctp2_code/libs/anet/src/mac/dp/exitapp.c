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
 Exit function called by an application so stub will know if NetShell is to be relaunched.
 $Log: exitapp.c $
 Revision 1.2  1997/08/19 23:32:57  anitalee
 added a needed include file
 Revision 1.1  1997/08/15 19:05:25  anitalee
 Initial revision
---------------------------------------------------------------------------*/

#ifdef __POWERPC__

#include <Files.h>
#include <Processes.h>
#include <Events.h>
#include <EPPC.h>
#include <Strings.h>
#include <SegLoad.h>
#include "dp.h"
#include "macapp.h"

#define REFCON 1097

/*--------------------------------------------------------------------------
 Searches for stub and if it exists, notifies it of pending exit and if it
  wants NetShell relaunched (yes if status is zero) then exits application.
--------------------------------------------------------------------------*/
DP_API void dpExitFromApp(int status)
{
	OSErr err;
	ProcessSerialNumber stubPSN;

	if (!status) {
		err = getstub(&stubPSN);
		if (err == dp_RES_OK ) {
			int done = 0;
			TargetID sender;
			unsigned long msgRefcon = REFCON;
			unsigned long sendRefcon, myOpts, msglen;
			EventRecord event;
			event.what = kHighLevelEvent;
			event.message = STUB_EVENTID;
			event.where = longtopt(STUB_OBITCLASS);
			myOpts = receiverIDisPSN  + nAttnMsg;
			err = PostHighLevelEvent(&event, &stubPSN, msgRefcon, nil, 0, myOpts);
			if (err != noErr) {
				DPRINT(("post to stub about die error %d\n", err));
			}
		}
	}
	ExitToShell();
}

#endif
