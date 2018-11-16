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

/*----------------------------------------------------------------------
 DLL loading, unloading, enumerating, and diddling functions for dp on mac.
 Copyright (C) 1996, Activision.

 $Log: dpdll.c $
 Revision 1.5  1997/07/28 02:40:33  dkegel
 Works!
 To specify "Look in the DLL subdirectory", use path :dll:
 Pretty much any legal combination of directory ID and path can be used
 to specify where to look for DLL's in both dpLoadDLL and dpEnumTransports.
 Revision 1.4  1997/07/27 23:01:31  dkegel
 First cut at supporting vRefNum and dirID in dp_transport_t's.
 dp_transport_t now used uniformly in API when referring to transport DLL's.
 dpLoadDLL and dpUnloadDLL now have access to the dpio_t so they can
 store things in commInitReq to be saved across freeze/thaw cycles.
 Revision 1.3  1997/06/02 01:26:32  dkegel
 1. Deleted unused code.
 2. Support static linking if prefix file sets DYNALINK to zero.
 Revision 1.2  1997/05/31 22:51:32  dkegel
 Compiles under CW10, too.
 Revision 1.1  1997/05/30 18:21:11  dkegel
 Initial revision
 Imported from Howard Shear's Netmech sources.
----------------------------------------------------------------------*/

#include <CodeFragments.h>
#include <Files.h>
#include <Folders.h>

#include <stddef.h>
#include <string.h>

#ifndef DYNALINK
#define DYNALINK 1		// set to zero to test with static linking
#endif

#include "dp.h"			/* should only need dpio, I suppose */

/* Transport DLL's must have this type and creator */
#define kTransportType		'shlb'
#define kTransportCreator	'mwNT'

#if DYNALINK
//	these variables contain function pointers for the current transport
COMMINIT commInit = nil;
COMMTERM commTerm = nil;
COMMDRIVERINFO commDriverInfo = nil;
COMMPLAYERINFO commPlayerInfo = nil;
COMMTXFULL commTxFull = nil;
COMMTXPKT commTxPkt = nil;
COMMPEEKPKT commPeekPkt = nil;
COMMRXPKT commRxPkt = nil;
COMMSCANADDR commScanAddr = nil;
COMMPRINTADDR commPrintAddr = nil;
COMMGROUPALLOC commGroupAlloc = nil;
COMMGROUPFREE commGroupFree = nil;
COMMGROUPADD commGroupAdd = nil;
//COMMGROUPSUBTRACT commGroupSubtract = nil;
COMMSETPARAM commSetParam = nil;
COMMSAYHI commSayHi = nil;
COMMSAYBYE commSayBye = nil;
COMMLOADTEST commLoadTest = nil;
COMMNEEDSUI commNeedsUI = nil;

#endif

/* Note: drivers are not unloaded except when hanging up.
 * The pointer to the driver is usually kept in dp->commInitParams,
 * but if running without dp, we have to keep it here.
 */
CFragConnectionID gConnID = 0;

/*----------------------------------------------------------------------
 Convert a dp_transport_t into an FSSpec.
 The name field of the dp_transport_t is in C format;
 the name field of the FSSpec is in Pascal format;
----------------------------------------------------------------------*/
static OSErr makeFSSpecFromTransport(const dp_transport_t *src, FSSpec *dest)
{
	char buf[256];
	strcpy(buf, src->fname);
	c2pstr(buf);
	return FSMakeFSSpec(src->vRefNum, src->dirID, buf, dest);
}

#if 0
/*----------------------------------------------------------------------
 Convert an FSSpec into a dp_transport_t.
 The name field of the FSSpec is in Pascal format;
 the name field of the dp_transport_t is in C format.
----------------------------------------------------------------------*/
static void makeTransportFromFSSpec(const FSSpec *src, dp_transport_t *dest)
{
	unsigned int len;
	memset(dest, 0, sizeof(dp_transport_t));
	dest->dirID = src->parID;
	dest->vRefNum = src->vRefNum;
	/* Copy the pascal string and convert to C */
	len = 255 & (src->name[0]);
	memcpy(dest->fname, src->name+1, len);
	dest->fname[len] = 0;
}
#endif

/*----------------------------------------------------------------------
 Read description out of the DLL file.

 Mac-specific:
 If driver has a "commLoadTest" method, call it to see if the driver
 wants to say "Don't load me".
----------------------------------------------------------------------*/
dp_result_t dpGetTransportInfo(
	dp_transport_t *path,
	comm_driverInfo_t *info)
{
	dp_result_t theResult;
	Str255 pTransportName;
	OSErr anErr;
	CFragConnectionID connID;
	Ptr mainAddr;
	Str255 errName;
	CFragSymbolClass symClass;
	comm_driverInfo_t* infoFromCodeFragment;
	FSSpec spec;

	if (!path || !info)
		return dp_RES_BUG;

	anErr = makeFSSpecFromTransport(path, &spec);
	if (anErr != noErr)
		return dp_RES_EMPTY;

	/* Load the disk fragment */
	anErr = GetDiskFragment(
		&spec,
		0,
		kCFragGoesToEOF,
		nil,
		kLoadCFrag,
		&connID,
		&mainAddr,
		errName
	);
	if (anErr != noErr)
		return dp_RES_BAD;

	/* Once past this point, we must call CloseConnection when exiting. */

	// get a pointer to the comm driver info structure in the code fragment.
	anErr = FindSymbol(connID, "\pgCommDriverInfo", (Ptr*) &infoFromCodeFragment, &symClass);
	theResult = dp_RES_BAD;
	if (anErr == noErr) {
		// Copy the desired info.
		memcpy(info, infoFromCodeFragment, sizeof(comm_driverInfo_t));

		// Give the driver a chance to say "Don't load me"
#if DYNALINK
		anErr = FindSymbol(connID, "\pcommLoadTest", (Ptr*) &commLoadTest, &symClass);
#endif
		theResult = dp_RES_OK;
		if (anErr == noErr) {
			Boolean	list;
			list = commLoadTest();
			if (!list) {
				theResult = dp_RES_BAD;
			}
		}
	}

	//	close the code fragment; ignore any errors closing.
	anErr = CloseConnection(&connID);

	return theResult;
}

/*----------------------------------------------------------------------
 Calls the given function once for each transport available.
 All calls are made before dpEnumTransports returns.
 Path argument is to directory to search for DLL's.

 Reads description out of the DLL files.

 PC-Specific:
 Path may be NULL or empty, in which case the current directory is searched.
 Path may end in a / or \, or not.

 Mac-specific:
 'path' either has vRefNum and dirID zero and a folder path in fname,
 or has fname empty and vRefNum and dirID pointing to the driver folder.
----------------------------------------------------------------------*/
dp_result_t dpEnumTransports(dp_transport_t *path, dpEnumTransportCallback_t cb, void *context)
{
	comm_driverInfo_t	info;
	dp_result_t			theResult;
	OSErr				anErr;
	short				index;
	Str255				ioName;
	CInfoPBRec			myCPB;
	FSSpec				spec;
	dp_transport_t		transport;
	int					dirID;

	if (!path || !cb)
		return dp_RES_BUG;

	/* Get the FSSpec for the directory */
	anErr = makeFSSpecFromTransport(path, &spec);
	if (anErr != noErr)
		return dp_RES_EMPTY;

	/* Turn the FSSpec of the directory into a directory ID */
	myCPB.dirInfo.ioNamePtr = spec.name;
	myCPB.dirInfo.ioVRefNum = spec.vRefNum;
	myCPB.dirInfo.ioFDirIndex = 0;
	myCPB.dirInfo.ioDrDirID = spec.parID;
	anErr = PBGetCatInfo(&myCPB, false);
	if (anErr != noErr)
		return dp_RES_EMPTY;
	dirID = myCPB.dirInfo.ioDrDirID;

	/* Loop over all items in that directory */
	myCPB.hFileInfo.ioNamePtr = (unsigned char *)ioName;
	for (index=1; ; index++) {
		myCPB.hFileInfo.ioFDirIndex = index;
		myCPB.hFileInfo.ioVRefNum = spec.vRefNum;
		myCPB.hFileInfo.ioDirID = dirID;
		ioName[0] = '\0';
		anErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);

		/* Stop if no more files in folder */
		if (anErr != noErr)
			break;

		/* skip to next file if this file is a folder */
		if (myCPB.hFileInfo.ioFlAttrib & 0x10)
			continue;

		// Skip to next file if type or creator wrong
		if (((myCPB.hFileInfo.ioFlFndrInfo).fdType != kTransportType)
		||  ((myCPB.hFileInfo.ioFlFndrInfo).fdCreator != kTransportCreator))
			continue;

		/* Make a dp_transport_t corresponding to this file */
		transport.vRefNum = spec.vRefNum;
		transport.dirID = dirID;
		p2cstr(ioName);
		strcpy(transport.fname, ioName);

		// Retrieve this transport's info
		theResult = dpGetTransportInfo(&transport, &info);
		if (theResult != dp_RES_OK)
			continue;

		// Pass this transport's info to caller
		cb(&transport, &info, context);
	}

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Called internally by dpCreate to load a DLL.

 Mac-specific:
 1) The first parameter must really be a dp_t.
 2) The second parameter must be a dp_transport_t; on the Mac,
 dp_transport_t's have a FSSpec at offset 0, and the string FSSpec
 at offset 100.

 Transports are loaded into the system heap by the shell.
 When the shell does a dpFreeze, the handle is saved in the freeze file.
 dpCreate, when it reads the freeze file, finds the transport by
 reading the handle from the freeze file!
 Thus the first parameter must really be a dp_t!
 The handle to the transport is stored in dp->commInitReq.hwirq.
----------------------------------------------------------------------*/
dp_result_t dpLoadDLL(dpio_t *dpio, dp_transport_t *transport)
{
	dp_result_t			result = dp_RES_BAD;

	OSErr anErr;
	Ptr mainAddr;
	Str255 errName;
	CFragSymbolClass symClass;
	static SETSCREENRECT setScreenRect;
	extern Rect gScreenRect;
	FSSpec spec;

	typedef struct {
		unsigned char *name;
		Ptr *ptr;
	} sym_t;
	static sym_t syms[] = {
		{"\pcommInit", (Ptr*) &commInit},
		{"\pcommTerm", (Ptr*) &commTerm},
		{"\pcommDriverInfo", (Ptr*) &commDriverInfo},
		{"\pcommPlayerInfo", (Ptr*) &commPlayerInfo},
		{"\pcommTxFull", (Ptr*) &commTxFull},
		{"\pcommTxPkt", (Ptr*) &commTxPkt},
		{"\pcommPeekPkt", (Ptr*) &commPeekPkt},
		{"\pcommRxPkt", (Ptr*) &commRxPkt},
		{"\pcommScanAddr", (Ptr*) &commScanAddr},
		{"\pcommPrintAddr", (Ptr*) &commPrintAddr},
		{"\pcommGroupAlloc", (Ptr*) &commGroupAlloc},
		{"\pcommGroupFree", (Ptr*) &commGroupFree},
		{"\pcommGroupAdd", (Ptr*) &commGroupAdd},
		//{"\pcommGroupSubtract", (Ptr*) &commGroupSubtract},
		{"\pcommSetParam", (Ptr*) &commSetParam},
		{"\pcommSayHi", (Ptr*) &commSayHi},
		{"\pcommSayBye", (Ptr*) &commSayBye},
		{"\pSetScreenRect", (Ptr*) &setScreenRect},
		{NULL, NULL}
	};
	sym_t *sym;

	if (!dpio || !transport)
		return dp_RES_BAD;
	anErr = makeFSSpecFromTransport(transport, &spec);
	if (anErr != noErr)
		return dp_RES_EMPTY;

#if !DYNALINK

	(void) dpio;
	return dp_RES_OK;

#else
	//	we need to setup our function pointers for the selected
	//	transport. We do this by looking for a file with a code
	//	fragment and then we load all of the function pointers
	//	from it for later use throughout the application

	anErr = GetDiskFragment(
		&spec,
		0,
		kCFragGoesToEOF,
		nil,
		kLoadCFrag,
		&gConnID,
		&mainAddr,
		errName
	);
	if (anErr != noErr) {
		DPRINT(("GetDiskFragment failed!\n"));
		return dp_RES_EMPTY;
	}

	//	the code fragment has been loaded
	anErr = noErr;
	for (sym=syms; sym->name; sym++) {
		OSErr err;
		err = FindSymbol(gConnID, sym->name, sym->ptr, &symClass);
		if (err != noErr) {
			p2cstr(sym->name);
			DPRINT(("dpLoadDLL: Can't find symbol %s; FindSymbol returns OSErr %d (%x)\n", sym->name, err, err));
			c2pstr((char *)sym->name);
		}
		anErr |= err;
	}

	if (anErr == noErr) {

		//	we don't need this function, so, we don't check for errors

		FindSymbol(gConnID, "\pcommNeedsUI", (Ptr*) &commNeedsUI, &symClass);

		//	pass the screen rectangle into the code fragment for user interface
		//	stuff
		//setScreenRect(&gScreenRect);  // -- JL  FIX WHEN DOING MODEM DRIVER
		#if defined(NETSHELL)
			//	see if we need to give this the screen control function
			{
				SETSCREENFUNC	setScreenFunc;

				FindSymbol(gConnID, setScreenFuncName, (Ptr*) &setScreenFunc, &symClass);
				if (setScreenFunc != nil) {
					setScreenFunc(ForceScreenUpdate);
				}
			}
		#endif

		//	make sure we remember the connection ID so that we can
		//	close the code fragment later, even if the dpUnloadDLL
		//	is called another app which by an app we launch instead of us.
		if (dpio)
			dpio->commInitReq.hwirq = (long) gConnID;

		//	everything initialized fine, so, we return in a happy state
		result = dp_RES_OK;
	}

	return result;

#endif
}

/*----------------------------------------------------------------------
 Called internally by dpDestroy to unload a DLL.
----------------------------------------------------------------------*/
void dpUnloadDLL(dpio_t *dpio) {

#if DYNALINK
	//	close the code fragment and ignore errors
	if (dpio && !gConnID)
		gConnID = (CFragConnectionID) dpio->commInitReq.hwirq;
	CloseConnection(&gConnID);
#endif
	(void) dpio;
}
