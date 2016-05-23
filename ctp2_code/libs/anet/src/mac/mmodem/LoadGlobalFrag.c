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

#include "LoadGlobalFrag.h"
unsigned long*			hSer;
char*						transportGlobalsName = "\pgSer";
short						gvRef;
long						gparID;
CFragConnectionID		connID;
void SetBasePath(void);
/*----------------------------------------------------------------------
 Called internally to load a code fragment.
----------------------------------------------------------------------*/
OSErr LoadTransport(char *transportName)
{
	Str255				pTransportName;
	OSErr					anErr;
	FSSpec				spec;
	Ptr					mainAddr;
	Str255 				errName;
	SymClass				symClass;
	SetBasePath();
	//	we need to setup our function pointers for the selected
	//	transport. We do this by looking for a file with a code
	//	fragment and then we load all of the function pointers
	//	from it for later use throughout the application

	//	create a pascal string which is the file name which has the
	//	fragment in it

	strcpy((char*) pTransportName, transportName);
	c2pstr((char*) pTransportName);

	anErr = FSMakeFSSpec(gvRef, gparID, pTransportName, &spec);	if (anErr == noErr) {

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

	}
	return anErr;
}
/*----------------------------------------------------------------------
 Called internally by dpDestroy to unload a DLL.
----------------------------------------------------------------------*/
void unloadTransport(void)
{
	//	close the code fragment and ignored errors
	CloseConnection(&connID);
}
OSErr GetData(unsigned long* value)
{
	OSErr		error;
	SymClass	symClass;
	long		myCount;
	short		myIndex;
	Str255	myName;
	Ptr		myAddr;
	error = FindSymbol(connID, transportGlobalsName, (char**)&hSer, &symClass);	// Find the global
	if (error == noErr) {																// Found it; make it nil
		*value = *hSer;
	}
	return error;
}
OSErr SetData(unsigned long value)
{
	OSErr		error;
	SymClass	symClass;
	long		myCount;
	short		myIndex;
	Str255	myName;
	Ptr		myAddr;
	error = FindSymbol(connID, transportGlobalsName, (char**)&hSer, &symClass);	// Find the global
	if (error == noErr) {																// Found it; make it nil
		*hSer = value;
	}
	return error;
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
		strncat(name, "Serial", 63);	// It won't matter if this exists or not; we'll get the parent directory ID this way
		c2pstr(name);
		error = FSMakeFSSpec(vRef, dirID, name, &where);
		gvRef = where.vRefNum;
		gparID = where.parID;
	}
}
