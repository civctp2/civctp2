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

/* Stolen directly from CTBUtils.c, but needed changes to cope with Modem	*/
/* So, instead of messing it up there, I copied it in here.  WHL  1/19/97	*/
#include <CommResources.h>
#include <Connections.h>
#include <Files.h>
#include <PLStringFuncs.h>	// MAD
#include <Resources.h>
#include <Folders.h>
#include <LowMem.h>
#include <ToolUtils.h>
#include <Traps.h>
#include <CommResources.h>
#include <Connections.h>

#include "Modm.h"
#include	"CTBStuff.h"
#include	"CTBUtils.h"
#include	"CreateRes.h"
#include	"TransportUtils.h"
#include "commapi.h"  // drk
//#include	"Util.h"
#include "LoadGlobalFrag.h"
Boolean			gCTBLoaded = false;
Boolean			gConnected = false;
Ptr				configString = nil;
ConnHandle		gConnection;
extern ser_t*	ser;
#define kTransportPath	12000
#define kModemTool "Apple Modem Tool"
#pragma segment Main
void TearDownCTB(void) {
	if (gCTBLoaded) {
		gCTBLoaded = false;
	}
}

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
	strncat(name, "Modem prefs", 63);
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

OSErr PutToolname(char* toolName)
{
}

OSErr CreateToolNameRes(StringPtr toolName) {
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

			//ReleaseResource(theStrings);		// Release the resource from memory.
			//error = ResError();
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
			// Apple Modem Tool 1.5.1 insists on adding a configuration
			// piece that it then refuses to process.  Remove it by nulling it out.
			where = strstr(configPtr, "ModemLogFile");
			if (where != nil) {
				do {
					*where = '\0';
					++where;
				} while (where < configPtr + len);
			}
			//if (len > 255) {
			//	configPtr[255] = '\0';
			//}
			//c2pstr(configPtr);
			error = PtrToHand(configPtr, &theStrings, len);

			AddResource(theStrings, 'STR ', kConfigRes, "\pconfig");
			error = ResError();

			ChangedResource(theStrings);
			error = ResError();

			WriteResource(theStrings);
			error = ResError();

			//ReleaseResource(theStrings);		// Release the resource from memory.
			//error = ResError();
			UpdateResFile(ref);					// No screwing around; write that resource!
			error = ResError();
			FlushVol(nil, prefs.vRefNum);

			CloseResFile(ref);
			//p2cstr(configPtr);
		} else {
			error = ResError();
		}
	}
	return error;
}

//	this method sets up and opens a CTB connection
OSErr ConfigureConnection(ConnHandle* connection)
{
	Str255			toolName;
	Point				where;
	short				result = -1;
	//Ptr				configStream;
	EventRecord		event;
	Rect				dialogLoc;
	FSSpec			prefs;
	short				ref;
	OSErr				error;
	THz				myZone;
	myZone = GetZone();
	SetZone(SystemZone());

	if (*connection != nil) {

		//	put our A5 value into the connection record so that the search callbacks
		//	can get it and restore it

		CMSetUserData(*connection, (long) LMGetCurrentA5());

		// CMChoose Dialog has to hang off this point (global coordinates)

		SetRect(&dialogLoc, 0, 0, 495, 285);
		CenterOnCurrentScreen(&dialogLoc);

		where.h = dialogLoc.left;
		where.v = dialogLoc.top;

		//	setup the screen(s) for the dialog

		// now do CMChoose et al:

		result = CMChoose(connection, where, NULL);

		if ((result == chooseOKMajor) || (result == chooseOKMinor)) {
			configString = CMGetConfig(*connection);
			if (configString == NULL) {
				error = -2;
			} else {
				CMGetToolName((***connection).procID, toolName);
				error = CreateToolNameRes(toolName);
				error = CreateConfigRes(configString);
			}
		} else {
			error = -1;
		}
	}

	SetZone(myZone);
	return error;
}

//	this method initializes our connection and returns the connection record handle
OSErr InitializeConnection(Boolean readPrefs, ConnHandle* connection)
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
	*connection = nil;

	// get a Connection Tool name
	if (readPrefs) {
		// Open the prefs, and get the ref to user with these calls
		error = GetToolname(toolName);
		if (error != noErr) {												// We couldn't get a tool name from prefs
			readPrefs = false;												// We're using a substitute tool, so don't use it's config
			prefsChanged = true;
			strcpy(toolName, kModemTool);									// so try to use the Apple modem tool
			error = VerifyToolExists(toolName);
			if (error != noErr) {
				error = CRMGetIndToolName(classCM, 1, toolName);	// But we have to use something, so go for the first one in the list
				if (error != noErr) {
					return error;
				}
			}
		}
		if (configString == nil) {
			configString = NewPtrClear(256);
		}
		error = GetConfig(configString);
		if ((error != noErr) && (configString[0] == '\0')) {
			if (configString != nil) {
				DisposePtr(configString);
				configString = nil;
			}
			prefsChanged = true;
		}
	} else {																		// Don't even try the prefs; use the first available tool
		error = CRMGetIndToolName(classCM, 1, toolName);
		if (error != noErr) {
			return error;
		}
	}
	end = toolName + toolName[0] + 1;
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
	} else {
		// now get a conn record set up
		*connection = CMNew(procID, cmData|cmNoMenus|cmQuiet, bufSizes, 0, 0);
		if (*connection == nil) {
			error = -3;
		}
		if (error == noErr) {
			if (configString != nil)	{
				error = CMSetConfig(*connection, configString);
				if (error != noErr) {
					prefsChanged = false;
				}
			} else {
				/*
				configString = CMGetConfig(*connection);
				tempString = NewPtrClear(GetPtrSize(configString) + 5);
				strcpy(tempString, "Baud ");
				here = strstr(configString, tempString);
				if (here != nil) {
					here += strlen(tempString);
					baud = strtol(here, &end, 10);
					if (baud < 19200) {
						baud = 19200;
						strcpy(tempString, configString);
						sprintf(tempString + (here - configString), "%ld", baud);
						strcat(tempString, end);

						error = CMSetConfig(*connection, tempString);					// Try to use the modified configuration
						if (error == noErr) {													// It worked; save it
							DisposePtr(configString);
							configString = NewPtrClear(GetPtrSize(tempString));
							memcpy(configString, tempString, GetPtrSize(tempString));
							DisposePtr(tempString);
						}
					}
				}
				prefsChanged = true;
				*/
			}
		}
	}
	SetZone(myZone);

	/*
	if (prefsChanged) {
		// The preferences were not valid.  Replace them.
		error = CreateToolNameRes(toolName);
		error = CreateConfigRes(configString);
	}
	*/
	return error;
}

//	this method initializes the comm toolbox (it first checks to see if the comm toolbox is around).
//	if there are any errors, we return FALSE.
Boolean InitializeCommToolBox(void)
{
	CMErr			err;

	//CMTest();
	//CMTest2();
	//	this can only be initialized once

	//if (!gCTBLoaded) {

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
	//}

	return true;
}

int config_ctb(ConnHandle* connection)
{
 	OSErr		error = noErr;
	Boolean	loaded;
	THz		myZone;
	//	allocate the memory for the communications record (this
	//	also initializes all of the fields in the structure)

	loaded = InitializeCommToolBox();
	if (loaded) {

		//	initialize the connection, using the resources in the prefs file, if available
		error = InitializeConnection(true, connection);
		if (error == noErr) {
			//	That worked; now let the user change anything they want to
			error = ConfigureConnection(connection);				// Let the user alter the configuration, and update the resources

			if (*connection != nil) {
				myZone = GetZone();
				SetZone(SystemZone());
				CMDispose(*connection);									//	it was initialized, so, it must be disposed of
				*connection = nil;
				SetZone(myZone);
				error = SetData(nil);
			}
		}
	} else {
		error = -6;															// Tell the caller that something failed
	}

	return error;
}

OSErr GetData(unsigned long* value) {
	OSErr			error = noErr;
	FSSpec		where;
	short			ref;
	Handle		theString;

	//	get the pointer to the serial structure in the system heap from the prefs file
	error = FindPrefs(&where);
	ref = FSpOpenResFile(&where, fsRdWrPerm);
	if (ref != -1) {
		theString = Get1Resource(kSerPtrResType, kSerPtrRes);			// Should look in most recent file first
		if (theString != nil) {
			HLock(theString);
			memcpy(value, *theString, sizeof(unsigned long));
			HUnlock(theString);
			ReleaseResource(theString);
		} else {

			//	if the resource does not exist, we just return nil. This tells the called
			//	to create the serial structure for the first time

			*value = 0;
		}
		CloseResFile(ref);
	} else {
		error = ResError();
	}

	return error;
}

OSErr SetData(unsigned long value) {
	OSErr			err = noErr;
	FSSpec		where;
	short			ref;
	Handle		aHand;

	//	write the address of the serial structure into a resource for use in the next
	//	application
	err = FindPrefs(&where);
	ref = FSpOpenResFile(&where, fsRdWrPerm);
	if (ref != -1) {
		Handle	aHand;

		//	remove the current resource (if there is one)

		aHand = Get1Resource(kSerPtrResType, kSerPtrRes);
		if (aHand != nil) {
			RemoveResource(aHand);
			DisposeHandle(aHand);
			aHand = nil;
		}

		//	if we're not resuming, we need to save this address out
		//	to the prefs file

		err = PtrToHand(&value, &aHand, sizeof(unsigned long));
		if ( (err == noErr) && (aHand != nil) ){

			//	add the handles to the resource file

			AddResource(aHand, kSerPtrResType, kSerPtrRes, "\pSerial Structure Address");
			WriteResource(aHand);

		} else {

			//	something went wrong. Make sure we clean up our handles

			if (aHand != nil) {
				DisposeHandle(aHand);
				aHand = nil;
			}

		}

		CloseResFile(ref);
	} else {
		err = ResError();
	}

	return err;
}
