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

#include <string.h>	// MAD
#include <stdlib.h>	// MAD

#include <CommResources.h>
#include <Connections.h>
#include <Resources.h>	// MAD
#include <ToolUtils.h>	// MAD
#include <Folders.h>	// MAD
#include <Files.h>		// MAD
#include <Strings.h>		// MAD

#include "PrefsStuff.h"

OSErr	FindPrefs(FSSpec* where)
{
	OSErr		error = noErr;
	Str255	theString;
	Str255	name;
	FSSpec	spec;
	short		foundVRefNum;
	long		foundDirID;

	// Look for the prefs in the folder
	GetIndString(theString, kTransportPath, 1);
	p2cstr(theString);
	strncpy(name, theString, 63);
	strncat(name, "Serial prefs", 63);
	c2pstr(name);

	error = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);

	if (error == noErr) {
		error = FSMakeFSSpec(foundVRefNum, foundDirID, name, where);

		if (error == fnfErr) {		// May need to create the file
			FSpCreateResFile(where, 'mwNT', 'pref', smSystemScript);
			error = ResError();
		}
	}

	return error;
}

OSErr VerifyToolExists(char* toolName);

OSErr VerifyToolExists(char* toolName)
{
	Str255	tempName;
	short		index;
	short		result;
	OSErr		error = noErr;

	result = CMGetProcID(toolName);
	if (result == -1) {
		error = -7;
	}

	return error;
}

OSErr CreateConfigRes(Ptr configPtr)
{
	short			ref;
	FSSpec		prefs;
	OSErr			error;
	Handle		theStrings;
	long			len;
	char*			where;

	//	build the 'STR ' resource

	error = FindPrefs(&prefs);
	if (error == noErr) {
		ref = FSpOpenResFile(&prefs, fsRdWrPerm);
		if (ref != -1) {
			theStrings = Get1Resource('STR ', kConfigRes);
			if (theStrings != nil) {
				RemoveResource(theStrings);
				error = ResError();
			}
			len = GetPtrSize(configPtr);

			error = PtrToHand(configPtr, &theStrings, len);

			AddResource(theStrings, 'STR ', kConfigRes, "\pconfig");
			error = ResError();

			ChangedResource(theStrings);
			error = ResError();

			WriteResource(theStrings);
			error = ResError();

			UpdateResFile(ref);					// No screwing around; write that resource!
			error = ResError();

			FlushVol(nil, prefs.vRefNum);

			CloseResFile(ref);
		} else {
			error = ResError();
		}
	}

	return error;
}

OSErr CreateToolNameRes(StringPtr toolName)
{
	short			ref;
	FSSpec		prefs;
	OSErr			error;
	Handle		theStrings;

	//	build the 'STR ' resource

	error = FindPrefs(&prefs);
	if (error == noErr) {
		ref = FSpOpenResFile(&prefs, fsRdWrPerm);
		if (ref != -1) {
			theStrings = Get1Resource('STR ',kToolNameRes);
			if (theStrings != nil) {
				RemoveResource(theStrings);
				error = ResError();
			}

			error = PtrToHand(toolName, &theStrings, toolName[0] + 1);

			AddResource(theStrings, 'STR ', kToolNameRes, "\ptoolName");
			error = ResError();

			ChangedResource(theStrings);
			error = ResError();

			WriteResource(theStrings);
			error = ResError();

			UpdateResFile(ref);						// No screwing around; write that resource!
			error = ResError();
			FlushVol(nil, prefs.vRefNum);

			CloseResFile(ref);
		} else {
			error = ResError();
		}
	}

	return error;
}

OSErr GetConfig(Ptr config)
{
	OSErr			error = noErr;
	FSSpec		where;
	short			ref;
	Handle		theString;
	Ptr			temp = nil;
	long			len;

	config[0] = '\0';											// Start without a name

	error = FindPrefs(&where);
	ref = FSpOpenResFile(&where, fsRdWrPerm);
	if (ref != -1) {
		theString = Get1Resource('STR ',kConfigRes);	// Should look in most recent file first
		if (theString != nil) {
			len = SizeResource(theString);
			SetPtrSize(config, len);						// Set the size
			error = MemError();								// And make sure it worked
			if (error == noErr) {
				HLock(theString);
				temp = (Ptr)*theString;
				memcpy(config, temp, len);
				HUnlock(theString);
				ReleaseResource(theString);
			}
		} else {
			error = -4;
		}

		CloseResFile(ref);
	} else {
		error = ResError();
	}

	return error;
}

OSErr GetToolname(char* toolName)
{
	OSErr			error = noErr;
	FSSpec		where;
	short			ref;
	Handle		theString;

	toolName[0] = '\0';													// Start without a name

	error = FindPrefs(&where);
	ref = FSpOpenResFile(&where, fsRdWrPerm);
	if (ref != -1) {
		theString = Get1Resource('STR ',kToolNameRes);			// Should look in most recent file first
		if (theString != nil) {
			HLock(theString);
			memcpy(toolName, *theString, (*theString)[0] + 1);
			HUnlock(theString);
			ReleaseResource(theString);
		} else {
			error = -5;
		}

		CloseResFile(ref);

		if (error == noErr) {											// CTB is touchy; make sure this tool exists
			error = VerifyToolExists(toolName);
		}
	} else {
		error = ResError();
	}

	return error;
}
