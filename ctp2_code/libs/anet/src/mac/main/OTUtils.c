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


#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <CodeFragments.h>

#include "OTUtils.h"

Boolean OpenTransportExists(void) {
	Boolean		result = false;
	
	//	this method checks for the existance of Open Transport
	
	if ( InitOpenTransport != (UInt16) kUnresolvedCFragSymbolAddress ) {
	
		//	we can find one of the calls, so, the engine must exist
		
		result = true;
	}
	
	return result;
}

OSStatus CloseTheEndPoint(EndpointRef theRef) {
		OTSetSynchronous(theRef);
		OTRemoveNotifier(theRef);
		OTUnbind(theRef);
		OTCloseProvider(theRef);
}
