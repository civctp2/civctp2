#include <Folders.h>
#include <Files.h>
#include <CodeFragments.h>
#include <Resources.h>
extern "C" __initialize(CFragInitBlockPtr ibp);
extern "C" __terminate(void);
#include	"MW2TransportMain.h"
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
void __myterminate(CFragInitBlockPtr ibp) {
	short	i;

	if (gResFileID != -1) {
		CloseResFile(gResFileID);
		gResFileID = -1;
	}

	//__terminate();
}
void *operator new(unsigned long size) {
	return NewPtrSys(size);
}
void operator delete(void *ptr) {
	DisposePtr((Ptr) ptr);
}
