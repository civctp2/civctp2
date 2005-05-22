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

/*
	CreateRes.c
	© 1994 Green Dragon Creations, Inc.
	
	Description:
		This code saves STR resources into the currently open resource file
	
*/

#include <Aliases.h>
#include <AppleEvents.h>
// MAD #include <Desk.h>
#include <DiskInit.h>
#include <Dialogs.h>
#include <Errors.h>
#include <Events.h>
#include <Files.h>
#include <Folders.h>
#include <Fonts.h>
#include <Lists.h>
#include <Menus.h>
#include <Memory.h>
#include <OSUtils.h>
// MAD #include <OSEvents.h>
#include <Packages.h>
#include <Processes.h>
#include <Resources.h>
#include <Scrap.h>
#include <Script.h>
#include <SegLoad.h>
#include <Sound.h>
#include <StandardFile.h>
#include <StdArg.h>
#include <StdIo.h>
#include <String.h>
#include <ToolUtils.h>
#include <Types.h>

#include	"CreateRes.h"

//	This code creates STR resources in the currently open file for saving preferences information

OSErr CreateRes(Ptr configPtr,StringPtr toolName) {
	short			volRefNum;
	long			dirID;
	FSSpec			theFileSpec;
	OSErr			theErr;
	Handle			theStrings;
	Ptr				inPtr;
	long			theStringsSize;
	short			fRefNum;

	c2pstr(configPtr);		
	//	build the 'STR ' resource
	
	theStrings = GetResource('STR ',5000);
	if (theStrings != nil) {
		RemoveResource(theStrings);
	}			
	theErr = PtrToHand(configPtr,&theStrings,configPtr[0]+1);
	AddResource(theStrings, 'STR ', 5000, "\pconfig");
	ChangedResource(theStrings);
	WriteResource(theStrings);
	
	p2cstr((StringPtr)configPtr);		
	
	c2pstr((char*)toolName);		
	//	build the 'STR ' resource
	
	theStrings = GetResource('STR ',6000);
	if (theStrings != nil) {
		RemoveResource(theStrings);
	}			
	theErr = PtrToHand(toolName,&theStrings,toolName[0]+1);
	AddResource(theStrings, 'STR ', 6000, "\ptoolName");
	ChangedResource(theStrings);
	WriteResource(theStrings);
	
	p2cstr((StringPtr)toolName);		
	return theErr;
}

