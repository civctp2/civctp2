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

#include <Connections.h>
#include <CommResources.h>
#include <Traps.h>
#include <Folders.h>
#include <Script.h>

#include <string.h>

#include "LoadTransport.h"
#include "modm.h"

unsigned long*		hSer = nil;
unsigned char*		transportGlobalsName = "\pgSer";
ConnHandle			gConnection;

/*----------------------------------------------------------------------
 Called internally to load a code fragment.
----------------------------------------------------------------------*/
OSErr preloadTransport(StringPtr transportName, CFragConnectionID* connID)
{
	OSErr anErr;
	FSSpec spec;
	Ptr mainAddr;
	Str255 errName;
	Str255 fragName;
	CFragSymbolClass symClass;

	//	we need to setup our function pointers for the selected
	//	transport. We do this by looking for a file with a code
	//	fragment and then we load all of the function pointers
	//	from it for later use throughout the application

	//	create a pascal string which is the file name which has the
	//	fragment in it

	anErr = FSMakeFSSpec(gvRef, gparID, transportName, &spec);
	if (anErr == noErr) {

		anErr = GetDiskFragment(
			&spec,
			0,
			kCFragGoesToEOF,
			fragName,
			kLoadCFrag,
			connID,
			&mainAddr,
			errName
		);

	}
	return anErr;
}

/*----------------------------------------------------------------------
 Called internally by dpDestroy to unload a DLL.
----------------------------------------------------------------------*/
void unloadTransport(CFragConnectionID connID) {
	//	close the code fragment and ignored errors
	CloseConnection(&connID);
}

void SetBasePath(void)
{
	OSErr			error;
	FSSpec		where;
	//Handle		theString;
	Str255		name;
	short			vRef;
	long			dirID;
	long			nDirID;
	error = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, &vRef, &dirID);
	GetIndString(name, kTransportPath, 1);
	if (name[0] != '0') {
		error = FSMakeFSSpec(vRef, dirID, name, &where);	// First, check to see if the directory exists
		if (error != noErr) {
			// Create it
			error = FSpDirCreate(&where, smSystemScript, &nDirID);
		}
		p2cstr(name);
		strncat((char *)name, "Serial", 63);	// It won't matter if this exists or not; we'll get the parent directory ID this way
		c2pstr((char *)name);
		error = FSMakeFSSpec(vRef, dirID, name, &where);
		gvRef = where.vRefNum;
		gparID = where.parID;
	}
}

OSErr SetData(CFragConnectionID connID, unsigned long value)
{
	OSErr error;
	CFragSymbolClass symClass;
	long myCount;
	short myIndex;
	Str255 myName;
	Ptr myAddr;

	error = FindSymbol(connID, transportGlobalsName, (char**)&hSer, &symClass);	// Find the global
	if (error == noErr) {																// Found it; make it nil
		*hSer = value;
	}
	return error;
}

OSErr SetupData(CFragConnectionID connID)
{
	OSErr error;
	CFragSymbolClass symClass;
	long myCount;
	short myIndex;
	Str255 myName;
	Ptr myAddr;

	error = FindSymbol(connID, transportGlobalsName, (char**)&hSer, &symClass);	// Find the global
	if (error == noErr) {																			// Found it; make it nil
		*hSer = nil;
	}
	return error;
}

OSErr GetData(CFragConnectionID connID, unsigned long* value)
{
	OSErr error;
	CFragSymbolClass symClass;
	long myCount;
	short myIndex;
	Str255 myName;
	Ptr myAddr;

    /* Find the global */
	error = FindSymbol(connID, transportGlobalsName, (char**)&hSer, &symClass);
	if (error == noErr) {																			// Found it; make it nil
		*value = *hSer;
	}
	return error;
}

void CleanupTransport(CFragConnectionID connID)
{
	OSErr				error;
	ConnHandle		connection;
	unsigned long	value;
	ser_t*			ser;
	error = GetData(connID, &value);
	//Debugger();
	if (value != nil) {
		ser = (ser_t*)value;
		if (ser->sConnection != nil) {
			error = CMClose(ser->sConnection, false, nil, 0, false);
			CMDispose(ser->sConnection);
		}

		DisposePtr((Ptr)ser);
		error = SetData(connID, nil);
	}
}

OSErr TotalHack(CFragConnectionID connID)
{
	Handle				theStrings;
	CMBufferSizes		bufSizes;
	short					procID;
	Str255				toolName;
	OSErr					error;
	FSSpec				where;
	short					ref;
	char*					end;
	Ptr					tempString;
	char*					here;
	short					baud;
	Boolean				prefsChanged = false;
	THz					myZone;
	ser_t*				storage;
	gConnection = nil;

	// get a Connection Tool name
	strcpy((char *)toolName, "Apple Modem Tool");						// so try to use the Apple modem tool
	c2pstr((char *)toolName);
	end = (char *)(toolName + toolName[0] + 1);
	memset(end, '\0', 255 - toolName[0]);
	// init the CMBufferSizes variable so that Tool will init with defaults

	bufSizes[cmDataIn] = 0;
	bufSizes[cmDataOut] = 0;
	bufSizes[cmCntlIn] = 0;
	bufSizes[cmCntlOut] = 0;
	bufSizes[cmAttnIn] = 0;
	bufSizes[cmAttnOut] = 0;
	bufSizes[cmRsrvIn] = 0;
	bufSizes[cmRsrvOut] = 0;

	error = noErr;
	myZone = GetZone();
	SetZone(SystemZone());
	// get a process ID for the tool
	procID = CMGetProcID(toolName);
	if (procID == -1) {
		error = -2;
	}

	// now get a conn record set up
	if (error == noErr) {
		gConnection = CMNew(procID, cmData|cmNoMenus|cmQuiet, bufSizes, 0, 0);
		if (gConnection != nil) {
			storage = (ser_t*)NewPtrClear(sizeof(ser_t));
			storage->sConnection = gConnection;
			error = SetData(connID, (unsigned long)storage);
		}
	}

	return error;
}

//	this method initializes the comm toolbox (it first checks to see if the comm toolbox is around).
//	if there are any errors, we return FALSE.
Boolean InitializeCommToolBox(void) {
	CMErr			err;

	//CMTest();
	//CMTest2();
	//	this can only be initialized once

	if (!gCTBLoaded) {

		if (NGetTrapAddress(_CommToolboxDispatch, OSTrap) == NGetTrapAddress(_Unimplemented, OSTrap)) {
			return false;
		}

		err = InitCTBUtilities();
		if (err != noErr) {
			return false;
		}

		err = InitCRM();
		if (err != noErr) {
			return false;
		}

		err = InitCM();
		if (err == cmNoTools) {
			return false;
		}

		gCTBLoaded = true;
	}

	return true;
}
