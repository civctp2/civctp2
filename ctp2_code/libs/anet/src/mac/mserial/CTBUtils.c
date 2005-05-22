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
#include <stdio.h>	// MAD

#include <CommResources.h>
#include <Connections.h>
#include <LowMem.h>	// MAD
#include <Traps.h>	// MAD
#include <PLStringFuncs.h>	// MAD

#include	"CTBUtils.h"
#include	"CreateRes.h"
#include	"TransportUtils.h"
#include	"Util.h"
#include	"PrefsStuff.h"

Boolean	gCTBLoaded = false;

#define CTSWarningdlog 16807

//	this method sets up and opens a CTB connection

OSErr OpenCTBConnection(ConnHandle* connection) {
	Str255			toolName;
	Point				where;
	Ptr				configStream;
	Ptr				tempString;
	EventRecord		event;
	Rect				dialogLoc;
	OSErr				error = noErr;
	char*				here;
	char*				end;
	long				baud;
	Boolean			done;
	short				result = noErr;

	if (*connection != nil) {
	
		//	put our A5 value into the connection record so that the search callbacks
		//	can get it and restore it
		
		CMSetUserData(*connection,(long) LMGetCurrentA5());
		
		// CMChoose Dialog has to hang off this point (global coordinates)
		
		SetRect(&dialogLoc, 0, 0, 495, 285);
		CenterOnCurrentScreen(&dialogLoc);
		
		where.h = dialogLoc.left;
		where.v = dialogLoc.top;
		
		// now do CMChoose et al:
		done = false;
		do {
			result = CMChoose(connection, where, NULL);
			
			// MAD fprintf(mfp,"result is %d. major =  %d  minor = %d\n",result,chooseOKMajor,chooseOKMinor);
			
			if ((result == chooseOKMajor) || (result == chooseOKMinor)) {
				configStream = CMGetConfig(*connection);
				if (configStream == NULL) {
					done = true;
					error = -1;
				} else {
					CMGetToolName((***connection).procID, toolName);
					
					tempString = NewPtrClear(GetPtrSize(configStream) + 5);
					strcpy(tempString, "Baud ");
					here = strstr(configStream, tempString);
					if (here != nil) {
						here += strlen(tempString);
						baud = strtol(here, &end, 10);
						if (baud < 38400) {		// MAD: PC seems to default to this,
							baud = 38400;		// no matter what the user tries to set!
							strcpy(tempString, configStream);
							sprintf(tempString + (here - configStream), "%ld", baud);
							strcat(tempString, end);
							
							error = CMSetConfig(*connection, tempString);					// Try to use the modified configuration
							if (error == noErr) {													// It worked; save it
								DisposePtr(configStream);
								configStream = NewPtrClear(GetPtrSize(tempString));
								memcpy(configStream, tempString, GetPtrSize(tempString));
								DisposePtr(tempString);
							}
						}
					}
					
					strcpy(tempString, "CTS");
					here = strstr(configStream, tempString);
					if (here != nil) {
						result = CautionAlert(CTSWarningdlog, nil);
						if (result == 2) {									// The Continue option
							error = CreateConfigRes(configStream);
							error = CreateToolNameRes(toolName);
							done = true;
						}
					} else {
						error = CreateConfigRes(configStream);
						// MAD fprintf(mfp,"CreateConfigRes = %d\n",error);
						error = CreateToolNameRes(toolName);
						// MAD fprintf(mfp,"CreateToolNames = %d\n",error);
						done = true;
					}

					DisposePtr(configStream);
				}
			} else {
				//ErrorTerminate(-2, "\pее CMChoose failed. ее");

				// MAD fprintf(mfp,"CMChoose failed\n");

				if (*connection != nil) {
					CMDispose(*connection);
					*connection = nil;
				}
				error = -2;
				done = true;
			}
		} while (!done);

		//	open the connection, send some data, and then close the connection
		if (error == noErr) {
			error = CMOpen(*connection, false, nil, 0);
			// MAD fprintf(mfp,"CMOpen = %d\n",error);
		}
	}
			
	return error;	
}

//	this method sets up and opens a CTB connection

OSErr ReopenCTBConnection(ConnHandle* connection) {
	Str255				toolName;
	Ptr					configStream;
	EventRecord			event;
	Rect				dialogLoc;
	OSErr				error;

	if (*connection != nil) {
		// Get the configuration from the prefs, or fail
		configStream = NewPtrClear(256);
		error = GetConfig(configStream);
		if (error == noErr) {
			//	configure the connection
			error = CMSetConfig(*connection, configStream);
			error = CMOpen(*connection, false, nil, 0);
		}
	}
			
	return error;	
}

//	this method initializes the comm toolbox (it first checks to see if the comm toolbox is around).
//	if there are any errors, we return FALSE.

Boolean InitializeCommToolBox(void) {
	CMErr			err;
	
	//	this can only be initialized once
	
	if (!gCTBLoaded) {
	
		if (NGetTrapAddress(_CommToolboxDispatch, OSTrap) == NGetTrapAddress(_Unimplemented, OSTrap)) {
			return false;
		}
		
		err = InitCRM();
		if (err != noErr) {
			return false;
		}
		
		err = InitCTBUtilities();
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

//	this method initializes our connection and returns the connection record handle

OSErr InitializeConnection(Boolean readPrefs, ConnHandle* connection) {
	Handle			theStrings;
	Ptr				configString = nil;
	CMErr				err;
	CMBufferSizes	bufSizes;
	short				procID;
	StringPtr		temp = nil;	
	Str255			toolName;

	*connection = nil;
	
	// get a Connection Tool name

	if (readPrefs) {
		err = GetToolname(toolName);
		if (err != noErr) {
			readPrefs = false;
			
			//	if there is a serial tool, use that name
			
			PLstrcpy(toolName, "\pSerial Tool");
			procID = CMGetProcID(toolName);
			if (procID == -1) {
			
				//	we could not find the serial tool, use what ever is first
			
				err = CRMGetIndToolName(classCM, 1, toolName);
				if (err != noErr) {
					err = -1;
					return err;
				}
				
			}
		}
	} else {

		//	if there is a serial tool, use that name
		
		PLstrcpy(toolName, "\pSerial Tool");
		procID = CMGetProcID(toolName);
		if (procID == -1) {

			err = CRMGetIndToolName(classCM, 1, toolName);
			if (err != noErr) {
				return err;
			}
			
		}
	}

	err = noErr;							// We got this far, so we have a toolname
	// get a resource ID for it
	
	procID = CMGetProcID(toolName);
	if (procID == -1) {
		return -1;
	}
	
	// init the CMBufferSizes variable so that Tool will init with defaults
	
	bufSizes[cmDataIn] = 0;
	bufSizes[cmDataOut] = 0;
	bufSizes[cmCntlIn] = 0;
	bufSizes[cmCntlOut] = 0;
	bufSizes[cmAttnIn] = 0;
	bufSizes[cmAttnOut] = 0;
	bufSizes[cmRsrvIn] = 0;
	bufSizes[cmRsrvOut] = 0;
	
	// now get a conn record set up 
	
	*connection = CMNew(procID, cmData|cmNoMenus|cmQuiet, bufSizes, 0, 0);
	if (*connection == nil) {
		return -1;
	}

	if (readPrefs) {
		configString = NewPtrClear(256);
		err = GetConfig(configString);
		if (err == noErr) {
			//	set the initial settings for the choose dialog
			
			err = CMSetConfig(*connection, configString);
			
			if (err > 0) {												// Ignore parse errors
				err = noErr;
			}

			DisposePtr(configString);
		}
	}
	
	return err;
}

#pragma segment Main
void TearDownCTB(void) {

	if (gCTBLoaded) {
		gCTBLoaded = false;
	}
	
}


