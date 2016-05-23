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

#include <Folders.h>
#include <Files.h>
#include <CodeFragments.h>
#include <Resources.h>

#include	"macdll.h"

/* __myinitialize needs to be specified in the Codewarrior IDE as the
 * startup entry point for this shared library.
 * Only needed to give the shared library access to its resource fork.
 */
short	gResFileID = -1;
FSSpec*	gFragSpec;
OSErr __myinitialize(CFragInitBlockPtr ibp) {
	OSErr	err = noErr;

	// Initialization routine for when the library is "opened" by an app
	err = __initialize(ibp);

	if (err != noErr) {
		return err;
	}

	//	open the fragment's resource file (we don't care if there is an error
	//	because all transports don't have resource forks)

	gFragSpec = ibp->fragLocator.u.onDisk.fileSpec;
	gResFileID = FSpOpenResFile(gFragSpec, fsRdWrPerm);	//	exclusive read & write

	return err;
}

/* __myterminate needs to be specified in the Codewarrior IDE as the
 * cleanup entry point for this shared library.
 */

void __myterminate(CFragInitBlockPtr ibp) {
	short	i;

	if (gResFileID != -1) {
		CloseResFile(gResFileID);
		gResFileID = -1;
	}

	__terminate();
}
