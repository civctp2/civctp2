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

#include <TextUtils.h>
#include <OpenTransport.h>
#include <OpenTptAppleTalk.h>
#include <Memory.h>
#include <Resources.h>
#include <LowMem.h>
#include <Errors.h>
#include <CodeFragments.h>
#include <stdio.h>

#include "ddprint.h"

#include "otq.h"
#include "otddp3.h"
#include "otutils.h"
#include "TransportUtils.h"

#define kFlagshipNameResourceID	(-16413)

//	why isn't this in the OT headers?

typedef struct AppleTalkInfo AppleTalkInfo;

UInt8					gRecvBuf[sizeof(DDPAddress) + ddpMaxRawData + 1];

//	AppleTalk does not have a broadcast mechanism. MW2 uses a broadcast mechanism to send
//	the enumerate sessions packet out. We first register our own NBP name for other MW2 nodes
//	to find. Then, we do a OTLookupName to get all of the addresses for all nodes with the correct
//	NBP type ("NetMech Transponder"). Whenever we are told to broadcast, we send to all addresses
//	in our list of addresses.

#define	kMaxAddresses	128			//	maximum addresses we broadcast to
#define kLookupBufSize	(10*1024)	//	size of lookup buffer

Boolean					gLookupInProgress = false;
short					gLookupCount = 0;				//	this can change while we're looking
DDPAddress				gLookupResults[kMaxAddresses];

TLookupRequest			gLookupRequest;
TLookupReply			gLookupReply;
NBPEntity				gNBPEntity;

Ptr						gBuffPtr = nil;

//	these variables are used to keep track of the zone list

#define	kNoZoneList			0
#define	kZoneListInProgress	1
#define	kZoneListDone		2
#define	kZoneListProcessed	3

#define	kMaxZoneBuffSize	(32*1024)

short					gZoneListState = kNoZoneList;	//	controls starting and processing of zone list

short					gNumZones = 0;				//	number of zones in our list
short					gCurrentZone = 0;			//	current zone we are doing a name lookup in

unsigned long			gZoneBufferSize = 0;		//	amount of space used in zone buffer
Ptr						gZoneBuffer = nil;			//	used when we do a zone list
Str32*					gZoneNames = nil;			//	used to store results of zone list

//	these variables keep track of the endpoint and mapper states

Boolean					gAppleTalkNotifierInstalled = false;

Boolean					gEndPointValidNBP = false;
Boolean					gNotifierNBP = false;

Boolean					gBindDDP = false;
Boolean					gNotifierDDP = false;
Boolean					gEndPointValidDDP = false;

short					gPrefsResFile = -1;

//	other misc. globals

otq_t*					gInQueue = nil;

long					gExtraOTDataDDP = 0;
long					gExtraOTDataNBP = 0;

DDPAddress				gDDPAddress;
OTNameID				gMyNameID = 0;

extern EndpointRef		gDDPEndpoint;
extern MapperRef		gNBPMapper;
extern ATSvcRef		gAppleTalkRef;

Boolean OpenTransportAppleTalkExists(void) {
	Boolean		result = false;

	//	this method checks for the existance of Open Transport

	if ( OTOpenAppleTalkServices != (UInt16) kUnresolvedCFragSymbolAddress ) {

		//	we can find one of the calls, so, the engine must exist

		result = true;
	}

	return result;
}

OSErr OpenPrefsFile(void) {
	extern FSSpec*	gFragSpec;
	FSSpec			prefsSpec;
	OSErr			result;
	Boolean			newFile = false;

	result = FSMakeFSSpec(gFragSpec->vRefNum, gFragSpec->parID, "\pAppleTalk Prefs", &prefsSpec);
	if (result == fnfErr) {

		//	time to create the prefs file

		FSpCreateResFile(&prefsSpec, 'mwNT', 'pref', smSystemScript);
		result = FSMakeFSSpec(gFragSpec->vRefNum, gFragSpec->parID, "\pAppleTalk Prefs", &prefsSpec);
		newFile = true;
	}

	if (result == noErr) {
		gPrefsResFile = FSpOpenResFile(&prefsSpec, fsRdWrPerm);
	}

	return result;
}

void ClosePrefsFile(void) {
	if (gPrefsResFile != -1) {
		CloseResFile(gPrefsResFile);
		gPrefsResFile = -1;
	}
}

OSStatus DoAppleTalkBind(EndpointRef ep, Boolean resume) {
	OSStatus			err = kOTNoError;
	TBind				bindReq;
	TBind				bindResp;
	DDPAddress			respAddr;
	DDPAddress			reqAddr;

	//	if we are resuming a connection, we need to retrieve the address we
	//	we last using and bind to it again. If we're not resuming, we just
	//	let OT assign us an address

	//	in either case, we'll need the prefs file at this point

	err = OpenPrefsFile();
	if (err != noErr) {
		return err;
	}

	if (resume) {
		Handle		aHand;

		//	get the address we want from the prefs file

		aHand = Get1Resource('ddpA', 1000);
		if (aHand != nil) {
			HLock(aHand);
			memcpy(&reqAddr, *aHand, sizeof(DDPAddress));
			HUnlock(aHand);
			ReleaseResource(aHand);
			aHand = nil;
		} else {
			bindReq.addr.maxlen = 0;
			bindReq.addr.len = 0;			//	no specified address returns dynamic address
			bindReq.addr.buf = nil;
			bindReq.qlen = 0;
		}

		//	set up the paramater block to point to the address we want

		bindReq.addr.maxlen = sizeof(DDPAddress);
		bindReq.addr.len = sizeof(DDPAddress);
		bindReq.addr.buf = (unsigned char*) &reqAddr;
		bindReq.qlen = 0;
	} else {
		bindReq.addr.maxlen = 0;
		bindReq.addr.len = 0;			//	no specified address returns dynamic address
		bindReq.addr.buf = nil;
		bindReq.qlen = 0;
	}

	bindResp.addr.maxlen = sizeof(DDPAddress);
	bindResp.addr.len = sizeof(DDPAddress);
	bindResp.addr.buf = (unsigned char*) &respAddr;
	bindResp.qlen = 0;

	//	bind the endpoint

	err = OTBind(ep, &bindReq, &bindResp);

	if (err == noErr) {
		memcpy(&gDDPAddress, &respAddr, sizeof(DDPAddress));

		if (!resume) {
			Handle	aHand;

			//	remove the current resource

			aHand = Get1Resource('ddpA', 1000);
			if (aHand != nil) {
				RemoveResource(aHand);
				DisposeHandle(aHand);
				aHand = nil;
			}

			//	if we're not resuming, we need to save this address out
			//	to the prefs file

			err = PtrToHand(&gDDPAddress, &aHand, sizeof(DDPAddress));
			if ( (err == noErr) && (aHand != nil) ){

				//	add the handles to the resource file

				AddResource(aHand, 'ddpA', 1000, "\pDDPAddress");
				WriteResource(aHand);

			} else {

				//	something went wrong. Make sure we clean up our handles

				if (aHand != nil) {
					DisposeHandle(aHand);
					aHand = nil;
				}

			}
		}
	} else {
		#ifdef MACDEBUG
			ColorDebugStr("\pBind Failed");
		#endif
	}

	//	we're done with the prefs file

	ClosePrefsFile();

	return err;
}

OSStatus CreateAndConfigNBP(MapperRef *ep) {
	OSStatus		err;

	//	open a mapper for name binding protocol and install a notifier

	*ep = OTOpenMapper(OTCreateConfiguration(kNBPName), 0, &err);
	if (err == noErr) {
		gEndPointValidNBP = true;

		err = OTInstallNotifier(*ep, HandleEndpointEventsNBP, &gExtraOTDataNBP);
		if (err == noErr) {
			gNotifierNBP = true;

			//	register our name

		//	OTRegisterMyName();

			//	the mapper can operate in async mode from this point until we
			//	unregister our name and dispose of it

			err = OTSetAsynchronous(*ep);
			if (err != noErr) {
				#ifdef MACDEBUG
					ColorDebugStr("\pSet Async Failed");
				#endif
			}
		}
	}

	return err;
}

OSStatus CreateAndConfigDDP(EndpointRef *ep, Boolean resume) {
	OSStatus			err;
	TBind				bindInfo;

	//	create a queue to recieve packets into

	gInQueue = otq_create();
	if (gInQueue != nil) {

		*ep = OTOpenEndpoint(OTCreateConfiguration(kDDPName), 0, nil, &err);
		if (err == noErr) {
			gEndPointValidDDP = true;

			//	install a notifier and bind the endpoint

			err = OTInstallNotifier(*ep, HandleEndpointEventsDDP, &gExtraOTDataDDP);
			if (err == noErr) {
				gNotifierDDP = true;

    			err = DoAppleTalkBind(*ep, resume);
				if (err == noErr) {
					gBindDDP = true;

					//	once we're all setup, we want to start operating in async mode (if
					//	it would stop returning an error!)

					err = OTSetAsynchronous(*ep);
					if (err == noErr) {
						err = OTDontAckSends(*ep);
						if (err != noErr) {
							#ifdef MACDEBUG
								ColorDebugStr("\pNo Ack Sends");
							#endif
						}
					} else {
						#ifdef MACDEBUG
							ColorDebugStr("\pSet Async 2 Failed");
						#endif
					}
				}
			} else {
				#ifdef MACDEBUG
					ColorDebugStr("\pInstall Notifier Failed");
				#endif
			}

		} else {
			#ifdef MACDEBUG
				ColorDebugStr("\pOpen Endpoint Failed");
			#endif
		}

	} else {
		err = MemError();
		#ifdef MACDEBUG
			ColorDebugStr("\potq_create failed");
		#endif
	}

	return (err);
}

OSStatus ShutDownAppleTalk(void) {
	OSStatus	result = noErr;
	OSStatus	err;

	//	get rid of the mapper

	if (gEndPointValidNBP) {
		err = OTSetSynchronous(gNBPMapper);
		if (err != noErr) {
			#ifdef MACDEBUG
				ColorDebugStr("\pSet Sync Name Provider Failed");
			#endif
		}

		if (gMyNameID != 0) {
			err = OTDeleteNameByID(gNBPMapper, gMyNameID);
			if (err != noErr) {
				#ifdef MACDEBUG
					ColorDebugStr("\pName Delete Failed");
				#endif
			}
		}

		if (gNotifierNBP) {
			OTRemoveNotifier(gNBPMapper);
			gNotifierNBP = false;
		}

		err = OTCloseProvider(gNBPMapper);
		if (err != noErr) {
			#ifdef MACDEBUG
				ColorDebugStr("\pError Closing Name Provider");
			#endif
		}
		gEndPointValidNBP = false;
	}

	//	get rid of our endpoint

	if (gEndPointValidDDP) {

		//	wait for all of these calls to finish

		err = OTSetSynchronous(gDDPEndpoint);
		if (err != noErr) {
			#ifdef MACDEBUG
				ColorDebugStr("\pSet Sync DDP Provider Failed");
			#endif
		}

		if (gBindDDP) {
			err = OTUnbind(gDDPEndpoint);
			if (err != noErr) {
				#ifdef MACDEBUG
					ColorDebugStr("\pUnbind Failed");
				#endif
			}
			gBindDDP = false;
		}

		if (gNotifierDDP) {
			OTRemoveNotifier(gDDPEndpoint);
			gNotifierDDP = false;
		}

		err = OTCloseProvider(gDDPEndpoint);
		if (err != noErr) {
			#ifdef MACDEBUG
				ColorDebugStr("\pError Closing DDP Provider");
			#endif
		}
		gEndPointValidDDP = false;
	}

	if (gAppleTalkNotifierInstalled) {
		OTRemoveNotifier(gAppleTalkRef);
	}

	CloseOpenTransport();

	//	dispose of any memory we've allocated

	if (gBuffPtr != nil) {
		DisposePtr(gBuffPtr);
		gBuffPtr = nil;
	}

	otq_destroy(gInQueue);
	gInQueue = nil;

	return result;
}

pascal void HandleProvider(void* contextPtr, OTEventCode code, OTResult result, void* cookie) {
	TNetbuf*	theNetBuf;

	//	this method handles events for this endpoint

	switch (code) {

		case T_GETZONELISTCOMPLETE:

			if (gZoneListState == kZoneListInProgress) {
				theNetBuf = (TNetbuf*) cookie;

				gZoneBufferSize = theNetBuf->len;	//	amount of space used in buffer
				gZoneListState = kZoneListDone;		//	set flag so the data is processed later
			}
			break;

	}

}

#pragma segment Main
OSStatus OTRegisterMyName(void) {
	TRegisterRequest 	regreq;
	TRegisterReply		regreply;
	char ** /*StringHandle*/		userNameHndl = nil;
	OSErr				err;
	OSStatus			status;
	UInt8				nameBuf[100];
	unsigned long		theTime;

	//	if we have already registered our name, don't do it again

	if (gMyNameID != 0) {
		return noErr;
	}

	// for registering a name, we'll first try to get the flagship name
	// otherwise, we'll use the machinename

//	userNameHndl = GetString(kFlagshipNameResourceID);
//	if ( (userNameHndl == nil) || ((*userNameHndl)[0] == 0) )  {

		userNameHndl = (char ** /*StringHandle*/) NewHandle(40);		//	need room for whole string
		if (userNameHndl) {
			HLock(userNameHndl);

			theTime = LMGetTicks();
			sprintf(*userNameHndl, "MW2 %08x", theTime);

			c2pstr(*userNameHndl);
			HUnlock(userNameHndl);
		} else {
			#ifdef MACDEBUG
				ColorDebugStr("\pNo Memory For Name Handle");
			#endif
			return MemError();
		}

//	}

	HLock(userNameHndl);

	// create the NBP name string and set the len field for the string

	regreq.name.len = OTMySetNBPEntity((char*)nameBuf, (Ptr) *userNameHndl, (Ptr) kSocketType, (Ptr) "\p*");
	regreq.name.maxlen = sizeof(nameBuf);
	regreq.name.buf = nameBuf;

	// bind this name to our address and socket number

	regreq.addr.len = regreq.addr.maxlen = sizeof(DDPAddress);
	regreq.addr.buf = (unsigned char*) &gDDPAddress;

	// set up regreply

	regreply.addr.maxlen = 0;
	regreply.addr.buf = nil;

	//	make sure we wait around to be registered

	status = OTSetSynchronous(gNBPMapper);
	if (status != noErr) {
		#ifdef MACDEBUG
			ColorDebugStr("\pSet Sync (Name) Failed");
		#endif
	}

	err = OTRegisterName(gNBPMapper, &regreq, &regreply);
	if (err == noErr) {
		gMyNameID = regreply.nameid;
	} else {
		#ifdef MACDEBUG
			ColorDebugStr("\pRegister Name Failed");
		#endif
	}

	status = OTSetAsynchronous(gNBPMapper);
	if (status != noErr) {
		#ifdef MACDEBUG
			ColorDebugStr("\pSet Async (Name) Failed");
		#endif
	}

	HUnlock(userNameHndl);
	DisposeHandle(userNameHndl);

	return err;
}

#pragma segment Main
UInt16 OTMySetNBPEntity(char *buffer, Ptr nbpObject, Ptr nbpType, Ptr nbpZone) {
	char*	bufPtr;
	UInt16	len;

	bufPtr = buffer;

	BlockMove((Ptr)&nbpObject[1], bufPtr, nbpObject[0]);
	bufPtr += nbpObject[0];		// point buffer to end of current string
	len = nbpObject[0];			// collect number of chars moved to buffer

		// add the ":" character between the object and type strings
	*bufPtr = ':';
	bufPtr++;
	len++;

	BlockMove((Ptr)&nbpType[1], bufPtr, nbpType[0]);
	bufPtr += nbpType[0];		// point buffer to end of current string
	len += nbpType[0];			// collect number of chars moved to buffer

		// add the "@" character between the type and zone strings
	*bufPtr = '@';
	bufPtr++;
	len++;

	BlockMove((Ptr)&nbpZone[1], bufPtr, nbpZone[0]);
	len += nbpZone[0];			// collect number of chars moved to buffer
	return len;
}

OSStatus StartupAppleTalk(void) {
	OSStatus	err;

	gAppleTalkRef = OTOpenAppleTalkServices(kDefaultAppleTalkServicesPath, 0, &err);
	if (err == noErr) {

		//	install our handler (used for the zone list)

		err = OTInstallNotifier(gAppleTalkRef, HandleProvider, nil);
		if (err == noErr) {
			gAppleTalkNotifierInstalled = true;
		} else {
			#ifdef MACDEBUG
				ColorDebugStr("\pError Installing AppleTalk Notifier");
			#endif
		}

		//	make sure we're operating in async mode once the services have been started

		err = OTSetAsynchronous(gAppleTalkRef);
		if (err != noErr) {
			#ifdef MACDEBUG
				ColorDebugStr("\pSet AppleTalk Async Failed");
			#endif
		}

	}

	return err;
}

#pragma segment Main
pascal void HandleEndpointEventsDDP(void* contextPtr, OTEventCode code, OTResult result, void* it) {
	OSStatus 					error;
	static unsigned long		overflowErrors = 0;
	Boolean						doneReading = false;
	OTResult					epState;

	//	this method handles events for this endpoint

	switch (code) {
		case T_DATA:
			do {
				OTFlags				flags = 0;
				TUnitData			theData;

				theData.addr.buf = (UInt8*) gRecvBuf;				//	address destination
				theData.addr.maxlen = sizeof(DDPAddress);			//	address length
				theData.opt.len = 0;
				theData.opt.buf = nil;
				theData.udata.buf = gRecvBuf + sizeof(DDPAddress);	//	data destination
				theData.udata.maxlen = ddpMaxRawData;				//	buffer size
				theData.opt.maxlen = 0;

				error = OTRcvUData(gDDPEndpoint, &theData, &flags);

				//	if there is too much data for our buffer, there is something
				//	wrong with our implementation

				if (flags & T_MORE) {
					#ifdef MACDEBUG
						ColorDebugStr("\pData Overflow");
					#endif
				}

				//	process errors and keep good packets

				switch (error) {
					case kOTNoError:
						otq_put(gInQueue, gRecvBuf, sizeof(DDPAddress) + theData.udata.len);
						break;

					case kOTLookErr:
						error = OTLook(gDDPEndpoint);
						if (error == T_ORDREL) {
							doneReading = true;
						} else {
							epState = OTGetEndpointState(gDDPEndpoint);
							if (error == kOTOutStateErr && epState == T_INREL) {
								doneReading = true;
							}
						}
						break;

					case kOTNoDataErr:
						doneReading = true;
						break;
				}

			} while (!doneReading);

			break;

		case T_UDERR:
			HandleErrorUDERR();
			break;

		case T_OPENCOMPLETE:
			gDDPEndpoint = * (EndpointRef*) it;
			break;

		case T_LISTEN:
		case T_CONNECT:
		case T_EXDATA:
		case T_DISCONNECT:
		case T_ERROR:
		case T_ORDREL:
		case T_GODATA:
		case T_GOEXDATA:
		case T_REQUEST:
		case T_REPLY:
		case T_PASSCON:
		case T_RESET:
		case T_BINDCOMPLETE:
		case T_UNBINDCOMPLETE:
		case T_ACCEPTCOMPLETE:
		case T_REPLYCOMPLETE:
		case T_DISCONNECTCOMPLETE:
		case T_OPTMGMTCOMPLETE:
		case T_GETPROTADDRCOMPLETE:
		case T_RESOLVEADDRCOMPLETE:
		case T_GETINFOCOMPLETE:
		case T_SYNCCOMPLETE:
		case T_MEMORYRELEASED:
		case T_REGNAMECOMPLETE:
		case T_DELNAMECOMPLETE:
		case T_LKUPNAMECOMPLETE:
		case T_LKUPNAMERESULT:
		case kOTProviderWillClose:
		case kOTProviderIsClosed:
		case kOTConfigurationChanged:
			break;

		default:
			break;

	}

}

pascal void HandleEndpointEventsNBP(void* contextPtr, OTEventCode code, OTResult result, void* cookie) {
	TLookupReply*		theResponse = (TLookupReply*) cookie;
	DDPAddress*			theAddress;

	//	handle events for our mapper

	switch (code) {
		case T_LKUPNAMECOMPLETE:

			//	the lookup is over. There could be an entry when we get this
			//	event as well, so, we fall through to the T_LKUPNAMERESULT code

			gLookupInProgress = false;

		case T_LKUPNAMERESULT:

			//	if there is a result, save the DDP address into our array of
			//	addresses and increment the counter which the broadcast code
			//	uses to see how many addresses are in this list

			if (theResponse->rspcount != 0) {

				//	there is a response in the buffer, get the ddp address and ignore the
				//	name

				if (gLookupCount < (unsigned short) kMaxAddresses) {
					theAddress = (DDPAddress*) (theResponse->names.buf + 2 * sizeof(short));	//	2 words, ddp address, nbp address

					//	if this is our address, we want to ignore it

					if ( (theAddress->fNetwork != gDDPAddress.fNetwork) || (theAddress->fNodeID != gDDPAddress.fNodeID) ) {

						gLookupResults[gLookupCount] = *theAddress;
						gLookupCount++;

					}
				}

				//	allow the mapper to overwrite the buffer with the next name

				theResponse->rspcount = 0;
				theResponse->names.len = 0;
			}

			break;

	}

}

#pragma segment Main
OSStatus HandleErrorUDERR(void) {
	TUDErr		uderr;

	char		addrBuf[sizeof(DDPAddress)];
	char		optBuff[100];

	uderr.addr.maxlen = sizeof(DDPAddress);
	uderr.addr.buf = (unsigned char *)addrBuf;

	uderr.opt.maxlen = 100;
	uderr.opt.buf = (unsigned char *)optBuff;

	return OTRcvUDErr(gDDPEndpoint, &uderr);
}

#pragma segment Main
OSStatus DoOTNameLookup(void) {
	OSErr	err;
	Str32	nameString;
	Str32	zoneNameString;

	// first check to see whether a Lookup is in progress

	if (gLookupInProgress) {
		return unimpErr;       // drk: kludge
	}

	//	allocate the buffer we will use for our name lookups (if one has
	//	not been allocated already)

	if (gBuffPtr == nil) {
		gBuffPtr = NewPtr(kLookupBufSize);
		if (gBuffPtr == nil) {
			#ifdef MACDEBUG
				ColorDebugStr("\pNo Memory For Name Buffer");
			#endif
			return MemError();			//	we can't do much without this buffer
		}
	}

	//	we don't want to have more than one lookup running at a time

	gLookupInProgress = true;

	//	setup the request paramaters (this needs to be a global structure since
	//	we're doing the lookup in async. mode)

	nameString[0] = 1;
	nameString[1] = kNBPWildCard;									//	match any name with our type in the current zone

	//	we default to the local zone (the zoneNameStringPtr points to this string by default)

	zoneNameString[0] = 1;
	zoneNameString[1] = '*';										//	match the local zone

	//	if there are any zones in our zone list, get the next one and use it instead of the local zone

	if ( (gNumZones > 0) && (gZoneNames != nil) ) {

		//	use the current zone name

		memcpy(zoneNameString, gZoneNames[gCurrentZone], gZoneNames[gCurrentZone][0] + 1);

		//	increment the current zone index

		gCurrentZone++;
		if (gCurrentZone >= gNumZones) {
			gCurrentZone = 0;

			//	we always clear the list of addresses when we start with the
			//	first zone again

			gLookupCount = 0;
			memset(gLookupResults, 0, kMaxAddresses * sizeof(DDPAddress));

		}
	} else {

		//	we always clear the list of addresses before processing the
		//	local zone

		gLookupCount = 0;
		memset(gLookupResults, 0, kMaxAddresses * sizeof(DDPAddress));

	}

	gLookupRequest.maxcnt = kLookupBufSize / kAppleTalkAddressLength;	// set the maximum number of names to match
	gLookupRequest.timeout = 8000;										// 8 seconds in milliseconds
	gLookupRequest.name.len = (size_t) OTMySetNBPEntity((char*) &gNBPEntity, (Ptr) nameString, (Ptr) kSocketType, (Ptr) zoneNameString);
	gLookupRequest.name.buf = (UInt8*) &gNBPEntity;

	gLookupRequest.addr.len = 0;
	gLookupRequest.addr.maxlen = 0;
	gLookupRequest.addr.buf = nil;

	// zero out the buffer so that extraneous characters don't get left
	// in the buffer to confuse the name parsing routines which are looking
	// for null terminated strings.

	memset(gBuffPtr, 0, kLookupBufSize);

	//	setup the reply paramaters (this needs to be a global structure since
	//	we're doing the lookup in async. mode)

	gLookupReply.names.maxlen = kLookupBufSize;
	gLookupReply.names.buf = (UInt8*) gBuffPtr;

	//	start the lookup

	err = OTLookupName(gNBPMapper, &gLookupRequest, &gLookupReply);
	if (err != noErr) {
		#ifdef MACDEBUG
			ColorDebugStr("\pName Lookup Error");
		#endif
		gLookupInProgress = false;
	}
	return unimpErr;       // drk: kludge, don't know what to return
}

#pragma segment Main
void CheckZoneList(void) {
	static TNetbuf		zoneListReply;
	short				numZoneNames;
	OSStatus			err;

	//	this method handles the various zone list states

	switch (gZoneListState) {
		case kNoZoneList:

			//	set the state before we start the zone list. The lookup can finish
			//	quickly enough that the state will change before we leave this case
			//	of the switch statement, so, the state cannot be changed in the case
			//	after the zone list is started

			gZoneListState = kZoneListInProgress;

			//	start the async. zone list

			gZoneBuffer = NewPtr(kMaxZoneBuffSize);		//	allocate a big buffer for the results
			if (gZoneBuffer != nil) {

				zoneListReply.buf = (UInt8*) gZoneBuffer;
				zoneListReply.maxlen = kMaxZoneBuffSize;

				err = OTATalkGetZoneList(gAppleTalkRef, &zoneListReply);
			}

			break;

		case kZoneListDone:

			//	the zone list has finished, process the data in the buffer and dispose of the buffer

			numZoneNames = CountZoneNamesInBuffer(gZoneBuffer, gZoneBufferSize);
			if (numZoneNames > 0) {
				gZoneNames = (Str32*) NewPtr(numZoneNames * sizeof(Str32));
				if (gZoneNames != nil) {

					//	unpack the data into our array

					gNumZones = UnpackZoneNames(gZoneBuffer, gZoneBufferSize);

				}
			}

			//	we're done with the zone data buffer

			if (gZoneBuffer) {
				DisposePtr(gZoneBuffer);
				gZoneBuffer = nil;
			}

			gZoneListState = kZoneListProcessed;

			break;

		case kZoneListInProgress:
		case kZoneListProcessed:

			//	we are either waiting for the zone list to complete, or, we have already finsihed
			//	processing the data from the zone list. In either case, there is nothing left to
			//	do, so, we just happily return

			break;

	}

}

#pragma segment Main
short CountZoneNamesInBuffer(Ptr packedBuffer, unsigned long bufferSize) {
	short			nStrings = 0;
	unsigned long	index = 0;

	//	the buffer has packed pascal strings in it. This routine
	//	returns the number of pascal strings in the buffer

	while (index < bufferSize) {

		//	there is a name left in the buffer

		nStrings++;

		//	find the start of the next string

		index += packedBuffer[index] + 1;			//	add string length and 1 byte to get to start of next string
	}

	return nStrings;
}

#pragma segment Main
short UnpackZoneNames(Ptr packedBuffer, unsigned long bufferSize) {
	unsigned long	index = 0;
	short			nStrings = 0;

	//	the buffer has packed pascal strings in it. This routine
	//	returns the number of pascal strings in the buffer

	while (index < bufferSize) {

		//	there is a name left in the buffer, copy it to our array of zone names

		memcpy(&gZoneNames[nStrings], &packedBuffer[index], packedBuffer[index] + 1);

		//	find the start of the next string

		index += packedBuffer[index] + 1;			//	add string length and 1 byte to get to start of next string
		nStrings++;

	}

	return nStrings;
}
