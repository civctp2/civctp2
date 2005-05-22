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
 *	Dialog Module, created by Resorcerer
 $Log: HostDialog.c $
 Revision 1.1 1997/08/05 dswarzman
 Fixed system includes.
 */

//#include <Desk.h>
#include <Devices.h>
#include <DiskInit.h>
#include <Dialogs.h>
#include <Events.h>
#include <Files.h>
#include <Fonts.h>
#include <Lists.h>
#include <Menus.h>
#include <Resources.h>
#include <Memory.h>
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <OSUtils.h>
//#include <OSEvents.h>
#include <Packages.h>
#include <Scrap.h>
#include <Script.h>
#include <SegLoad.h>
#include <Sound.h>
#include <StdArg.h>
#include <string.h>
#include <stdio.h>
#include <TextUtils.h>
#include <ToolUtils.h>
//#include <Values.h>
#include "ddprint.h"
#include	"HostDialog.h"

typedef struct {
	ListHandle hndl;
	Rect bounds,scroll,content,dataBounds;
	Point cSize,cell;
	short currentRow;
	short nCells;
} UserList;

#define thisDialogID 1000

/* Symbolic Dialog Item Numbers */

static enum {
	BUT1_Connect = 1,
	BUT2_Remove,
	BUT3_Add,
	LIST4,
	BUT5_Listen,
	BUT6_Back,
	BUT7_Edit,
	STXT8_Select,
	STXT9_Quick,
	STXT10_Help,
	STXT11,
	STXT12,
	STXT13_NAME,
	STXT14_IP,
	LASTITEM
	} DialogItemNumber;

#define OK_ITEM 		BUT1_Connect
#define CANCEL_ITEM 	BUT6_Back

/* Useful constants */

#ifndef ENTERkey
#define ENTERkey	0x3
#endif
#ifndef DELETEkey
#define DELETEkey	0x8
#endif
#ifndef NIL
#define NIL ((void *)0)
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef FRONT_WINDOW
#define FRONT_WINDOW  ((WindowPtr) (-1L))
#endif

/* Prototypes */

DialogPtr OpenThisDialog(void);
void      CloseThisDialog(DialogPtr dlog);
void      DoDialogUpdate(DialogPtr dlog);
void      DoDialogActivate(DialogPtr dlog, int activ);
void      DoDialogContent(DialogPtr dlog, EventRecord *evt);
int       DoDialogItem(DialogPtr dlog, short itemHit);

pascal  Boolean MyFilterHD(DialogPtr dlog, EventRecord *evt, short *itemHit);
Boolean CheckUserItems(Point where, short *itemHit);
int     AnyBadValues(DialogPtr dlog);

void    CenterWindow(WindowPtr w, int top);
//long    strlen(char *);
//char   *strcpy(char *dst, char *src);
char   *PascalToC(unsigned char *pstr);
unsigned char   *CToPascal(char *cstr);
void    PutDlgString(DialogPtr dlog, int item, unsigned char *str, int sel);
void    PutDlgWord(DialogPtr dlog, int item, int val, int sel);
void    PutDlgLong(DialogPtr dlog, int item, long val, int sel);
void    PutDlgChkRadio(DialogPtr dlog, int item, int val);
int     GetDlgString(DialogPtr dlog, int item, unsigned char *str);
int     GetDlgWord(DialogPtr dlog, int item, short *val);
int     GetDlgLong(DialogPtr dlog, int item, long *val);
int     GetDlgChkRadio(DialogPtr dlog, int item);
int     TextSelected(DialogPtr dlog);
OSType  CanPaste(int n, ...);
void    FrameDefault(DialogPtr dlog, int item, int frame);
void    GetDlgPanel(DialogPtr dlog, int item, Rect *panel);

int     BuildList(DialogPtr dlog, int item, int csize, UserList *l);
void	GetCellData(UserList *l, short i, short *len);
int     GetLengthList(UserList *l);

//	prefs functions

OSErr OpenPrefsFile(void);
void ClosePrefsFile(void);
void AddHostToPrefs(StringPtr hostName, StringPtr description);
void CheckList(void);

static Point where;
static int modifiers;

/* Lists and/or popups */

static UserList list4;
short	gPrefsResFile = -1;

#pragma export on
Str255	gSavedSelection;
#pragma export off

/*
 *	Display this modal dialog.  Return TRUE if OK, FALSE if CANCEL or error.
 *	If the dialog displays values from outside this module, you should either
 *	import them from globals, or change the argument list of this routine to
 *	bring them in and pass them to OpenThisDialog(), DoDialogItem(), etc.
 */

int DoHostListDialog()
	{
		short itemHit,okay=FALSE,keepGoing=TRUE;
		DialogPtr dlog=NIL; GrafPtr oldPort;
		ModalFilterUPP MyFilterUPP;

		GetPort(&oldPort);

		/* On PowerPC, need a RoutineDescriptor from heap; on 68K, no allocation */
		
		MyFilterUPP = NewModalFilterProc(MyFilterHD);
		if (MyFilterUPP == NIL) goto cleanUp;

		/* Build dialog window and install its item values */
		
		OpenPrefsFile();
		
		dlog = OpenThisDialog();
		if (dlog == NIL) goto cleanUp;

		/* Entertain filtered user events until dialog is dismissed */
		
		while (keepGoing) {
			ModalDialog(MyFilterUPP,&itemHit);
			keepGoing = DoDialogItem(dlog,itemHit);
			}
		
		/*
		 *	Do final processing of item values, such as exporting them to caller.
		 *	DoDialogItem() has already called AnyBadValues().
		 */
		
		if (itemHit == OK_ITEM) {
			Point	theCell;
			short	dataLen;
			OSErr	anErr;
			Handle	aHand;
			
			//	get the current selection and store it in a global which can be accessed
			//	from the application
			
			theCell.h = 0;
			theCell.v = list4.currentRow;
			
			dataLen = 255;
			LGetCell(&gSavedSelection[1], &dataLen, theCell, list4.hndl);
			gSavedSelection[0] = dataLen;

			//	we save the current selection for the next time we use this transport
			
			//	remove the current resource
			
			aHand = Get1Resource('mw2H', 1000);
			if (aHand != nil) {
				RemoveResource(aHand);
				DisposeHandle(aHand);
				aHand = nil;
			}
			
			//	create a handle and add this resource to the resource file
			
			anErr = PtrToHand(&gSavedSelection[0], &aHand, gSavedSelection[0] + 1);		//	name and length byte
			if ( (anErr == noErr) && (aHand != nil) ) {
				AddResource(aHand, 'mw2H', 1000, "\pDefault Host");
				WriteResource(aHand);
			}
			
			//	the application likes C-strings
			
			p2cstr(gSavedSelection);
			
			okay = true;
		} else if (itemHit == BUT5_Listen) {
		
			//	the user wants to list for a connection. We need to clear out the global selection
			//	string
			
			gSavedSelection[0] = 0;

			okay = true;
		}

		/* That's all, folks! */

cleanUp:		
		ClosePrefsFile();

		if (dlog) CloseThisDialog(dlog);
		if (MyFilterUPP) DisposeRoutineDescriptor(MyFilterUPP);
		SetPort(oldPort);
		
		return(okay);
	}

/*
 *	We have to have a filter function, at the very least so that we can outline
 *	any default button, entertain keyboard editing commands, cmd-period canceling, etc.
 *	Note that you do not need to have a special user item covering the default button
 *	in your dialog item list.
 */

static pascal Boolean MyFilterHD(DialogPtr dlog, EventRecord *evt, short *itemHit)
	{
		Boolean ans=FALSE,doHilite=FALSE; WindowPtr w;
		short type,ch; Handle hndl; Rect box;
		static long then; static Point clickPt;
		static short lastItem = -1;
		
		w = (WindowPtr)(evt->message);
		switch(evt->what) {
			case nullEvent:
			
				//	set the help string based on which item is under the menubar
				
				{
					short	dItem;
					Str255	theString;
					Point	thePoint;
					
					thePoint = evt->where;
					GlobalToLocal(&thePoint);
					dItem = FindDialogItem(dlog, thePoint);
					if ( (dItem != -1) && (dItem != lastItem) ) {
						lastItem = dItem;
						GetIndString(theString, 1000, dItem + 1);
						PutDlgString(dlog, STXT10_Help, theString, FALSE);
					}
				
				}
				
				break;
			case updateEvt:
				if (w == dlog) {
					/* Update our dialog contents */
					DoDialogUpdate(dlog);
					ans = TRUE; *itemHit = 0;
					}
				 else {
					/*
					 *	Call your main event loop DoUpdate(w) routine here if you
					 *	don't want unsightly holes in background windows caused
					 *	by nested alerts, balloon help, or screen savers (see
					 *	Tech Note #304).
					 */
						 if (w != nil) {
							 BeginUpdate(w);
						 	EndUpdate(w);
						}
					}
				break;
			case activateEvt:
				if (w == dlog) {
					DoDialogActivate(dlog,(evt->modifiers & activeFlag)!=0);
					*itemHit = 0;
					}
				 else {
					/*
					 *	Call your main event loop DoActivate(w) routine here if
					 *	you want to deactivate the former frontmost window, in order
					 *	to unhighlight any selection, scroll bars, etc.
					 */
					}
				break;
			case mouseDown:
			case mouseUp:
				where = evt->where;		/* Make info available to DoDialog() */
				GlobalToLocal(&where);
				modifiers = evt->modifiers;
				ans = CheckUserItems(where,itemHit);
				break;
			case keyDown:
				if ((ch=(unsigned char)evt->message)=='\r' || ch==ENTERkey) {

					{
						short type; Handle hndl; Rect box;
						Point appCell;
						Boolean anyCell;

						SetPt(&appCell, 0, 0);
						anyCell = LGetSelect(true, &appCell, list4.hndl);
						if (anyCell) {
							*itemHit = OK_ITEM;
							doHilite = ans = TRUE;
						}
					}

					}
				 else if (evt->modifiers & cmdKey) {
					ch = (unsigned char)evt->message;
					switch(ch) {
						case 'x':
						case 'X':
							if (TextSelected(dlog))
								{ SystemEdit(3); ZeroScrap(); DialogCut(dlog); TEToScrap(); }
							 else {
								/* Cut from anything else cuttable, like a list */
								}
							break;
						case 'c':
						case 'C':
							if (TextSelected(dlog))
								{ SystemEdit(3); ZeroScrap(); DialogCopy(dlog); TEToScrap(); }
							 else {
								/* Copy from anything else copyable, like a list */
								}
							break;
						case 'v':
						case 'V':
							if (CanPaste(1,'TEXT'))
								{ TEFromScrap(); DialogPaste(dlog); }
							 else {
							 	/* Deal with any other pasteable scraps here */
								}
							break;
						case 'a':
						case 'A':
							if (((DialogPeek)dlog)->editField >= 0) {
								/* Dialog has text edit item: select all */
								SelectDialogItemText(dlog,((DialogPeek)dlog)->editField+1,0,32767);
								}
							 else {
								}
							*itemHit = 0;
							break;
						case '.':
							*itemHit = CANCEL_ITEM;
							doHilite = TRUE;
							break;
						}
					ans = TRUE;		/* Other cmd-chars ignored */
					}
				break;
			}
		if (doHilite) {
			GetDialogItem(dlog,*itemHit,&type,&hndl,&box);
			/* Reality check */
			if (type == (btnCtrl+ctrlItem)) {
				long soon = TickCount() + 7;		/* Or whatever feels right */
				HiliteControl((ControlHandle)hndl,1);
				while (TickCount() < soon) ;		/* Leave hilited for a bit */
				}
			}
		return(ans);
	}

/*
 * Mouse down event:
 * Check if it's in some user item, and convert to itemHit if appropriate.
 */

static Boolean CheckUserItems(Point where, short *itemHit)
	{
		return(FALSE);
	}

/*
 * Redraw the contents of this dialog due to update event.
 * If you have not installed UserItem draw routines, you should redraw
 * them explicitly here; otherwise, UpdateDialog() will call your routines.
 */

static void DoDialogUpdate(DialogPtr dlog) {
	GrafPtr oldPort;
    short fontID = 0;
	short saveFont;
	short saveSize;
	Style saveStyle;

	GetPort(&oldPort); SetPort(dlog);

	BeginUpdate(dlog);

	UpdateDialog(dlog,dlog->visRgn);
	FrameDefault(dlog, BUT1_Connect, TRUE);
	FrameRect(&list4.bounds);
	
	saveFont = dlog->txFont;
	saveSize = dlog->txSize;
	saveStyle = dlog->txFace;

    GetFNum("geneva", &fontID);
	TextFont(fontID);
	TextSize(9);
	TextFace(0);

	LUpdate(((GrafPtr)dlog)->visRgn, list4.hndl);

	TextFont(saveFont);
	TextSize(saveSize);
	TextFace(saveStyle);

	EndUpdate(dlog);

	SetPort(oldPort);
}

/*
 * Activate event: Activate or deactivate this dialog and any items in it
 */

static void DoDialogActivate(DialogPtr dlog, int activ) {
    short fontID;
	short saveFont;
	short saveSize;
	Style saveStyle;

	SetPort(dlog);

	saveFont = dlog->txFont;
	saveSize = dlog->txSize;
	saveStyle = dlog->txFace;

    GetFNum("geneva", &fontID);
	TextFont(fontID);
	TextSize(9);
	TextFace(0);

	LActivate(activ,list4.hndl);
	
	TextFont(saveFont);
	TextSize(saveSize);
	TextFace(saveStyle);
}

/*
 * Build this dialog's window on desktop, and install initial item values.
 * Return the dlog opened, or NIL if error (no resource, no memory).
 */

static DialogPtr OpenThisDialog()
	{
		short type; Handle hndl; Rect box; GrafPtr oldPort;
		DialogPtr dlog; unsigned char *p,str[256];
		Str255		theString;
		OSStatus	status;
		extern InetInterfaceInfo	gInetInfo;
		extern InetSvcRef			gInetService;

		GetPort(&oldPort);
		dlog = GetNewDialog(thisDialogID,NIL,FRONT_WINDOW);
		if (dlog == NIL) { SysBeep(1); return(NIL); }	/* Poor man's error message */

		CenterWindow(dlog,0);
		SetPort(dlog);

		//	build the list before we setup anything else

		if (!BuildList(dlog,LIST4,16,&list4)) goto broken;
		
		//	setup the IP address
		
		OTInetHostToString(gInetInfo.fAddress, (char*) theString);
		c2pstr((char*) theString);
		PutDlgString(dlog, STXT14_IP, theString, FALSE);
		
		//	setup the machine name
		
		OTSetSynchronous(gInetService);
		status = OTInetAddressToName(gInetService, gInetInfo.fAddress, (char*) theString);
		OTSetAsynchronous(gInetService);
		
		if (status == noErr) {
			c2pstr((char*) theString);
			PutDlgString(dlog, STXT13_NAME, theString, FALSE);
		} else {
			PutDlgString(dlog, STXT13_NAME, "\pCannot Resolve Machine Name", FALSE);
		}
		
		//	show the window

		ShowWindow(dlog);
		return(dlog);

		/* Error return */
broken:
		CloseThisDialog(dlog);
		SetPort(oldPort);
		return(NIL);
	}

/*
 * Clean up any allocated stuff, and return dialog to primordial mists
 */

static void CloseThisDialog(DialogPtr dlog)
	{
		if (dlog) {
			if (list4.hndl) { LDispose(list4.hndl); list4.hndl = NIL; }
			DisposeDialog(dlog);	/* Call CloseDialog if you provide storage to GetNewDialog */
		}
	}
	
void AddNewEntry(DialogPtr dlog, char* theName, char* theDesc, Boolean checkOriginalName, char* theOriginalName) {
	short		result;
	
	result = DoEnterNewAddressDialog(theName, theDesc);
	if (result) {

		short saveFont;
		short saveSize;
		Style saveStyle;
        short fontID;

		saveFont = dlog->txFont;
		saveSize = dlog->txSize;
		saveStyle = dlog->txFace;
	
        GetFNum("geneva", &fontID);
	    TextFont(fontID);
		TextSize(9);
		TextFace(0);
		
		{
			Boolean		aBool;
			Point		theCell = {0, 0};
			Handle		aHand;
		
			//	if this name is in the list, remove it
			
			aBool = LSearch(&theName[1], theName[0], nil, &theCell, list4.hndl);
			if (aBool) {
				if (theCell.h == 0) {
					LDelRow(1, theCell.v, list4.hndl);
					list4.nCells--;
					
					if (list4.currentRow > theCell.v) {
						list4.currentRow = theCell.v;
					}
					
				}
			}
			
			//	if there is a resource with this name, remove it
			
			aHand = Get1NamedResource('STR ', theName);
			if (aHand != nil) {
				RemoveResource(aHand);
				DisposeHandle(aHand);
				aHand = nil;
			}
		
		}

		if (checkOriginalName) {
			Boolean		aBool;
			Point		theCell = {0, 0};
			Handle		aHand;
		
			//	if this name is in the list, remove it
			
			aBool = LSearch(&theOriginalName[1], theOriginalName[0], nil, &theCell, list4.hndl);
			if (aBool) {
				if (theCell.h == 0) {
					LDelRow(1, theCell.v, list4.hndl);
					list4.nCells--;
					
					if (list4.currentRow > theCell.v) {
						list4.currentRow = theCell.v;
					}
					
				}
			}
			
			//	if there is a resource with this name, remove it
			
			aHand = Get1NamedResource('STR ', theOriginalName);
			if (aHand != nil) {
				RemoveResource(aHand);
				DisposeHandle(aHand);
				aHand = nil;
			}
		
		}

		LAddRow(1, list4.nCells, list4.hndl);
		list4.nCells++;
		
		//	add the data to the new row
		
		{
			Point	theCell;
			
			theCell.h = 0;
			theCell.v = list4.nCells - 1;
			LSetCell(&theName[1], theName[0], theCell, list4.hndl);
		
			theCell.h = 1;
			LSetCell(&theDesc[1], theDesc[0], theCell, list4.hndl);
		
		}
		
		TextFont(saveFont);
		TextSize(saveSize);
		TextFace(saveStyle);

		AddHostToPrefs(theName, theDesc);
		
		//	select the new row
		
		//list4.currentRow = list4.nCells - 1;
		{
			Point	theCell;
			
			theCell.h = 0;
			theCell.v = list4.nCells - 1;
			LSetSelect(TRUE, theCell, list4.hndl);
			CheckList();								//	unselect current row and select last row
		}
		
		{
			Handle hndl;
			Rect box;
			short type;
			
			GetDialogItem(dlog, LIST4, &type, &hndl, &box);
			InvalRect(&box);
		}

	}
}

void EditEntry(DialogPtr dlog) {
	Point		theCell = {0, 0};
	Boolean		anyCell;
	short		dataLen;
	Str255		theName = "\pNew Warrior Location";
	Str255		theDesc = "\pAnother Mech To Destroy!";
	Str255		theOriginalName = "\p";

	//	get the current selection and edit it
	
	anyCell = LGetSelect(TRUE, &theCell, list4.hndl);
	if (anyCell) {
	
		dataLen = 255;
		LGetCell(&theName[1], &dataLen, theCell, list4.hndl);
		theName[0] = dataLen;

		theCell.h = 1;
		dataLen = 255;
		LGetCell(&theDesc[1], &dataLen, theCell, list4.hndl);
		theDesc[0] = dataLen;

		//	copy the name for later deletion
		
		memcpy(&theOriginalName[0], &theName[0], theName[0] + 1);
		
		//	add the new entry (this will remove the one with the original name)

		AddNewEntry(dlog, theName, theDesc, true, theOriginalName);
	}

}

static int DoDialogItem(DialogPtr dlog, short itemHit)
	{
		short type,okay=FALSE,keepGoing=TRUE,val;
		Handle hndl; Rect box; Point pt;
		unsigned char *p,str[256];
		short		aShort;
		Str255		theName = "\pNew Warrior Location";
		Str255		theDesc = "\pAnother Mech To Destroy!";

		/*
		 * Deal with user clicking on an item in this dialog, either modal or non-modal.
		 * The local point is in where; modifiers in modifiers.
		 * Returns whether or not the dialog should be closed (keepGoing).
		 */
		 
		if (itemHit<1 || itemHit>=LASTITEM)
			return(keepGoing);				/* Only legal items, please */

		GetDialogItem(dlog,itemHit,&type,&hndl,&box);
		switch(type) {
			case ctrlItem+btnCtrl:
				switch(itemHit) {
					case BUT1_Connect:
						keepGoing = FALSE; okay = TRUE;
						break;
					case BUT2_Remove:
						{
							Boolean	anyCell;
							Point	theCell = {0, 0};
							short	dataLen;
							Handle	aHand;
							
							//	if there is a current selection, remove it
							
							anyCell = LGetSelect(TRUE, &theCell, list4.hndl);
							if (anyCell) {
							
								//	get the data and remove the resource from our file
								
								theCell.h = 0;
								dataLen = 255;
								LGetCell(&theName[1], &dataLen, theCell, list4.hndl);
								theName[0] = dataLen;
								
								ParamText(theName, "\p", "\p", "\p");	//	add name to be deleted
								aShort = CautionAlert(1500, nil);		//	returns 1 if OK to rmeove
								
								if (aShort == 1) {
								
									//	the user really want's to do this
									
									aHand = Get1NamedResource('STR ', theName);
									if (aHand != nil) {
										RemoveResource(aHand);
										DisposeHandle(aHand);
									}
									
									//	remove the resource from the list
									
									{
										short saveFont;
										short saveSize;
										Style saveStyle;
                                        short fontID;
	
										saveFont = dlog->txFont;
										saveSize = dlog->txSize;
										saveStyle = dlog->txFace;
									
                                        GetFNum("geneva", &fontID);
	                                    TextFont(fontID);
										TextSize(9);
										TextFace(0);
										
										LDelRow(1, theCell.v, list4.hndl);
	
										TextFont(saveFont);
										TextSize(saveSize);
										TextFace(saveStyle);
									}
									
									if (theCell.v == list4.nCells - 1) {
										list4.currentRow -= 1;
									}
									
									list4.nCells--;
								}
							}
							
							//	make sure something is still selected
							
							CheckList();
							
						}
						break;
					case BUT7_Edit:
						EditEntry(dlog);
						break;
					case BUT3_Add:
						AddNewEntry(dlog, theName, theDesc, false, nil);
						break;
					case BUT6_Back:
						keepGoing = FALSE;
						break;
					case BUT5_Listen:
						keepGoing = FALSE; okay = TRUE;
						break;
					}
				break;
			case ctrlItem+chkCtrl:
				break;
			case ctrlItem+radCtrl:
				break;
			case ctrlItem+resCtrl:
				break;
			case statText:
				switch(itemHit) {
					case STXT8_Select:		/* NOT Enabled */
						break;
					case STXT9_Quick:		/* NOT Enabled */
						break;
					case STXT10_Help:		/* NOT Enabled */
						break;
					}
				break;
			case editText:
				break;
			case iconItem:
				break;
			case picItem:
				break;
			case userItem:
				switch(itemHit) {
					case LIST4:
						if (LClick(where, modifiers, list4.hndl)) {
							
							//	double click is the same as clicking the edit button
							
							EditEntry(dlog);
							
						} else {
							CheckList();
						}
						break;
					}
				break;
			}
			
		//	change the hilite state of the default button
	
		{
			short type; Handle hndl; Rect box;
			Point appCell;
			Boolean anyCell;

			SetPt(&appCell, 0, 0);
			anyCell = LGetSelect(true, &appCell, list4.hndl);
			
			GetDialogItem(dlog, BUT1_Connect, &type, &hndl, &box);
			if (anyCell) {
				HiliteControl((ControlHandle) hndl, 0);
			} else {
				HiliteControl((ControlHandle) hndl, 255);
			}
			
			GetDialogItem(dlog, BUT2_Remove, &type, &hndl, &box);
			if (anyCell) {
				HiliteControl((ControlHandle) hndl, 0);
			} else {
				HiliteControl((ControlHandle) hndl, 255);
			}
			
			GetDialogItem(dlog, BUT7_Edit, &type, &hndl, &box);
			if (anyCell) {
				HiliteControl((ControlHandle) hndl, 0);
			} else {
				HiliteControl((ControlHandle) hndl, 255);
			}
			
		}
	
		if (okay) keepGoing = AnyBadValues(dlog);
		return(keepGoing);
	}

/*
 * Pull values out of dialog items and deliver TRUE if any of them are
 * illegal or inconsistent; otherwise deliver FALSE.  If any values are bad,
 * you should inform your user about the problem here before delivering TRUE.
 * If any items are missing values, this is the place to assign any defaults.
 */

static int AnyBadValues(DialogPtr dlog)
	{
		unsigned char str[256]; short val,len; Cell cell;

		return(FALSE);
	}

/*¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥*/

/*
 *  The following are various utility routines for general dialog management.
 *  Typically, you'll want to keep them in a library that is available to all
 *  your dialog modules; however, they are included here (and declared static)
 *  as a private library so that you can quickly compile this file for testing.
 */

#define _PrivateLibraries_
#ifdef  _PrivateLibraries_

/*
 *	Center a given window, w, horizontally in the main screen, top pixels from
 *	the top, or centered vertically if top is 0.  The window should be invisible.
 */

static void CenterWindow(WindowPtr w, int top)
	{
		Rect scr; Point p;
		int rsize,size,margin,xoff,yoff;

		scr = qd.screenBits.bounds;
		SetPort(w);
		p.h = w->portRect.left; p.v = w->portRect.top;
		LocalToGlobal(&p);

		size = scr.right - scr.left;
		rsize = w->portRect.right - w->portRect.left;
		margin = size - rsize;
		if (margin > 0) {
			margin >>= 1;
			p.h = scr.left + margin;
			}
		size = scr.bottom - scr.top;
		rsize = w->portRect.bottom - w->portRect.top;
		margin = size - rsize;
		if (margin > 0) {
			margin >>= 1;
			p.v = scr.top + margin;
			}
		MoveWindow(w,p.h,top?top:p.v,FALSE);
	}

/* Convert in place a Pascal string to C string, and deliver its address */

static char *PascalToC(unsigned char *str)
	{
		register unsigned char *p,*q,*end;

		end = str + *str;
		q = (p=str) + 1;
		while (p < end) *p++ = *q++;
		*p = '\0';
		return((char *)str);
	}

/*
 *	Convert in place a C string to Pascal string, and deliver its address.
 *	The C string should not be greater than 255 chars in length, or the
 *	resulting Pascal string will be truncated to 255 chars.
 */

static unsigned char *CToPascal(char *str)
	{
		register char *p,*q;
		register long len;

		len = strlen(str);
		if (len > 255) len = 255;
		p = str + len;
		q = p-1;
		while (p != str) *p-- = *q--;
		*str = len;
		return((unsigned char *)str);
	}

/* Dialog Item Stuffers */

/*
 *	Install a given Pascal string, str, into the given static or edit text item
 *	in the dialog, dlog.  If the item is an edit text item, leave the installed
 *	text selected or not according to the value of sel (TRUE or FALSE).
 */

static void PutDlgString(DialogPtr dlog, int item, unsigned char *str, int sel)
	{
		short type; Handle hndl; Rect box;

		GetDialogItem(dlog,item,&type,&hndl,&box);
		SetDialogItemText(hndl,str);
		if (type == editText)
			SelectDialogItemText(dlog,item,sel?0:32767,32767);
		InvalRect(&box);
	}

/*
 *	Install a given decimal long value into the static or edit text item of the
 *	given dialog, dlog.  If the item is an edit text item, leave the installed
 *	text for the number selected or not according to sel (TRUE or FALSE).
 */

static void PutDlgLong(DialogPtr dlog, int item, long val, int sel)
	{
		unsigned char str[32];

		NumToString(val,str);
		PutDlgString(dlog,item,str,sel);
	}

/*
 *	Same as above, only for an int (word) decimal number.
 */

static void PutDlgWord(DialogPtr dlog, int item, int val, int sel)
	{
		PutDlgLong(dlog,item,(long)val,sel);
	}

/*
 *	Set the given check box or radio button item of the given dialog, dlog, to
 *	on or off, according to val.
 */

static void PutDlgChkRadio(DialogPtr dlog, int item, int val)
	{
		short type; Handle hndl; Rect box;

		GetDialogItem(dlog,item,&type,&hndl,&box);
		SetControlValue((ControlHandle)hndl,val!=0);
	}

/*
 *	Deliver the value of the checkbox or radio button item of the given dialog.
 */

static int GetDlgChkRadio(DialogPtr dlog, int item)
	{
		short type; Handle hndl; Rect box;
		
		GetDialogItem(dlog,item,&type,&hndl,&box);
		return(GetControlValue((ControlHandle)hndl) != 0);
	}

/* Dialog Item Unstuffers */

/*
 *	Retrieve the value of an edit text item in a given dialog, placing the
 *	resulting Pascal string in the buffer, str, which is assumed large enough
 *	to hold the text (256 bytes max).  If item is the number of a static text
 *	item, the empty string is delivered.  Delivers TRUE or FALSE according to
 *	whether or not the text so delivered was empty.  
 */

static int GetDlgString(DialogPtr dlog, int item, unsigned char *str)
	{
		short type; Handle hndl; Rect box;

		GetDialogItem(dlog,item,&type,&hndl,&box);
		if (type == editText) GetDialogItemText(hndl,str);
		 else                 *str = 0;
		return(*str != 0);
	}

/*
 *	Retrieve the value of an edit text item in a given dialog, converting the
 *	Pascal string to a long and setting *val to it.  Delivers TRUE or FALSE
 *	according to whether or not the text so delivered was empty.  If FALSE,
 *	*val is set to 0; if TRUE, *val is set to whatever StringToNum() says the
 *	value is, even if the text contains non-numerical characters.
 */

static int GetDlgLong(DialogPtr dlog, int item, long *val)
	{
		int ans; unsigned char str[256];

		*val = 0;
		ans = GetDlgString(dlog,item,str);
		if (ans)
			StringToNum(str,val);
		return(ans);
		}

/* Same as above, only delivers the value of a word */

static int GetDlgWord(DialogPtr dlog, int item, short *val)
	{
		int ans; long num;

		*val = 0;
		ans = GetDlgLong(dlog,item,&num);
		if (ans)
			*val = num;
		return(ans);
	}

/*
 *	Deliver the number of the current editText item in given dialog if any text
 *	is selected in it, or 0 if none selected.
 */

int TextSelected(DialogPtr dlog)
	{
		register TEHandle textH; int item = 0;
		
		textH = ((DialogPeek)dlog)->textH;
		if (*textH)
			if ( (*textH)->selStart != (*textH)->selEnd )
				item = ((DialogPeek)dlog)->editField+1;
		return(item);
	}

/*
 *  If any of the variable argument scrap types are available for pasting from
 *  the scrap, deliver the first one.  Otherwise, deliver 0.  For example,
 *	
 *      if (whichType = CanPaste(3,'TEXT','PICT','STUF')) ...
 *
 *  There can be any number of types in the list, as long as the preceding count, n,
 *  is correct.
 */

static OSType CanPaste(int n, ...)
	{
		register OSType nextType,ans = 0L;
		long err,offset;
		va_list nextArg;
		
		va_start(nextArg,n);
		nextType = va_arg(nextArg, OSType);
		
		while (n-- > 0) {
			err = GetScrap(NIL, nextType, &offset);
			if (err >= -1) {
				ans = nextType;
				break;
				}
			nextType = va_arg(nextArg, OSType);
			}
		
		va_end(nextArg);
		return(ans);
	}

/*
 *	Frame or unframe a default dialog item (presumed a button) in given dialog.
 *	Note that you don't need to use an extra user item to do this unless you
 *	are doing some sort of non-standard default highlighting (not recommended).
 */

void FrameDefault(DialogPtr dlog, int item, int frame)
	{
		short type; Handle hndl; Rect box;
		GrafPtr oldPort; PenState oldPen;
		
		Pattern			bPat, wPat;
		short			i;
		
		if (frame) {
			short type; Handle hndl; Rect box;
			Point appCell;

			SetPt(&appCell, 0, 0);
			frame = LGetSelect(true, &appCell, list4.hndl);
		}

		for (i = 0; i < 8; i++) {
			bPat.pat[i] = 0xff;
			wPat.pat[i] = 0x00;
		}
		
		GetPort(&oldPort); SetPort(dlog);
		GetPenState(&oldPen);
		
		GetDialogItem(dlog,item,&type,&hndl,&box);
		InsetRect(&box,-4,-4);
		
		PenMode(srcCopy);
		PenSize(3,3);
		
		if (frame) {
			PenPat(&bPat);
		} else {
			PenPat(&wPat);
		}
		FrameRoundRect(&box,16,16);
		
		SetPenState(&oldPen);
		SetPort(oldPort);
	}

/*
 *	Get rectangle, *panel, for a given item (usually a user or picture item)
 *	and then hide the item so that it doesn't interfere with mouse clicking.
 *	This lets you stop worrying about the item order any user or pict items that
 *	obscure other items in the item list, which can affect how the DialogMgr
 *	returns itemHits.
 */

static void GetDlgPanel(DialogPtr dlog, int item, Rect *panel)
	{
		short type; Handle hndl;
		
		GetDialogItem(dlog,item,&type,&hndl,panel);
		HideDialogItem(dlog,item);
	}


/*
 *	GetLengthList() should compute a given list's length and prepare
 *	its data to be inserted into screen cells.
 */

static int GetLengthList(UserList *l) {

	//	each 'STR ' resource contains a host name. We need to return the count

	l->nCells = Count1Resources('STR ');

	return l->nCells;
}

/*
 *	GetCellData(l,i,len) should deliver start of pure data of i'th cell's
 *	contents in the given list, and sets the data's length in *len.  It can
 *	be assumed to be called sequentially from i=0 to l->nCells-1.  The prototype code
 *	here should be replaced with your content-specific instructions.
 */

static void GetCellData(UserList *l, short i, short *len) {
	Handle		theStringHandle;
	StringPtr	theString;
	Point		theCell;
	Str255		name;
	short		theID;
	OSType		theType;

	//	get the next string and return a pointer to it. In order to do this
	//	we copy the data into a static string variable. The index passed in
	//	is 0 based, resources are 1 based.

	theStringHandle = Get1IndResource('STR ', i + 1);
	if (theStringHandle != nil) {
		HLock(theStringHandle);
		theString = (StringPtr) *theStringHandle;
		
		//	store the description in the second column
		
		theCell = l->cell;
		theCell.h = 1;
		LSetCell(&theString[1], theString[0], theCell, l->hndl);
		
		//	store the name in the first column
		
		theCell.h = 0;
		GetResInfo(theStringHandle, &theID, &theType, name);
		LSetCell(&name[1], name[0], theCell, l->hndl);

		HUnlock(theStringHandle);
	}

}

/*
 *	Build a new list in given user item box of dialog, dlog,
 *	with cell height, csize.  If success, delivers TRUE; if couldn't
 *	allocate ListMgr list (no more memory or whatever), delivers FALSE.
 */

static int BuildList(DialogPtr dlog, int item, int csize, UserList *l)
	{
		short i,len,type;
        Rect box;
        Handle hndl;
        unsigned char *data;
		short saveFont;
		short saveSize;
		Style saveStyle;
        short fontID;

		saveFont = dlog->txFont;
		saveSize = dlog->txSize;
		saveStyle = dlog->txFace;
	
        GetFNum("geneva", &fontID);
	    TextFont(fontID);
		TextSize(9);
		TextFace(0);
	
		/* Content area (plus scroll bar) of list corresponds to user item box */
		
		GetDialogItem(dlog,item,&type,&hndl,&box);
		l->bounds = box; InsetRect(&l->bounds,-1,-1);
		SetDialogItem(dlog,item,userItem,NIL,&l->bounds);

		l->scroll = box; l->scroll.left = l->scroll.right - 15;		/* Scrollbar width */
		l->content = box; l->content.right = l->scroll.left;

		SetRect(&l->dataBounds,0,0,2,0);				//	name & description
		//l->cSize.v = csize > 0 ? csize : 1;
		l->cSize.v = 0;
		l->cSize.h = (l->content.right - l->content.left) / 2;

		l->hndl = LNew(&l->content, &l->dataBounds, l->cSize, 0, dlog, FALSE, FALSE, FALSE, TRUE);
		if (l->hndl) {
			(*l->hndl)->selFlags = lOnlyOne;		/* Or whatever */
			GetLengthList(l);
			LAddRow(l->nCells,0,l->hndl);
			for (i=0; i<l->nCells; i++) {
				l->cell.h = 0; l->cell.v = i;
				GetCellData(l,i,&len);
				//data = GetCellData(l,i,&len);
				//LSetCell((Ptr)data,len,l->cell,l->hndl);
				}
			l->cell.v = 0;
			l->currentRow = 0;
			
			if (gSavedSelection[0] != 0) {
				Boolean		aBool;
				Point		theCell = {0, 0};
				
				aBool = LSearch(&gSavedSelection[1], gSavedSelection[0], nil, &theCell, l->hndl);
				if (aBool) {
					if (theCell.h == 0) {
						l->currentRow = theCell.v;
					}
				}
			}
			
			CheckList();
			EraseRect(&l->content);
			InvalRect(&l->bounds);
			LSetDrawingMode(TRUE, l->hndl);
			}

		TextFont(saveFont);
		TextSize(saveSize);
		TextFace(saveStyle);

		return(l->hndl!=NIL);
	}
	
void CheckList(void) {
	Boolean	anyCell;
	Point	theCell = {0,0};
	
	//	unslect the previous row in the list
	
	theCell.v = list4.currentRow;
	LSetSelect(FALSE, theCell, list4.hndl);
	theCell.h = 1;
	LSetSelect(FALSE, theCell, list4.hndl);
	
	theCell.h = 0;
	theCell.v = 0;
	anyCell = LGetSelect(TRUE, &theCell, list4.hndl);
	if (anyCell) {
		list4.currentRow = theCell.v;
	}

	//	select the specified row in the list
	
	theCell.h = 0;
	theCell.v = list4.currentRow;
	LSetSelect(TRUE, theCell, list4.hndl);
	theCell.h = 1;
	LSetSelect(TRUE, theCell, list4.hndl);

	LAutoScroll(list4.hndl);
}

#endif

OSErr OpenPrefsFile(void) {
	extern FSSpec*	gFragSpec;
	FSSpec			prefsSpec;
	OSErr			result;
	Boolean			newFile = false;
	Handle			theSavedString;
	
	DPRINT (( "Open prefs file" ) );
	return fnfErr;
	result = FSMakeFSSpec(gFragSpec->vRefNum, gFragSpec->parID, "\pInternet Prefs", &prefsSpec);
	if (result == fnfErr) {
	
		//	time to create the prefs file
		
		FSpCreateResFile(&prefsSpec, 'mwNT', 'pref', smSystemScript);
		result = FSMakeFSSpec(gFragSpec->vRefNum, gFragSpec->parID, "\pInternet Prefs", &prefsSpec);
		newFile = true;
	}
	
	if (result == noErr) {
		gPrefsResFile = FSpOpenResFile(&prefsSpec, fsRdWrPerm);
		
		/*
		if (newFile) {
		
			//	add the default item to the file. This item cannot be removed.
			
			AddHostToPrefs("\p255.255.255.255", "\pEveryone on the local subnet");
		
		}
		*/
		
		//	if there is a saved selection, get it now
		
		theSavedString = Get1Resource('mw2H', 1000);
		if (theSavedString == nil) {
			gSavedSelection[0] = 0;				//	no saved selection
		} else {
			HLock(theSavedString);
			memcpy(&gSavedSelection[0], *theSavedString, (*theSavedString)[0] + 1);
			HUnlock(theSavedString);
		}
		
	}
	
	return result;
}

void ClosePrefsFile(void) {
	if (gPrefsResFile != -1) {
		CloseResFile(gPrefsResFile);
		gPrefsResFile = -1;
	}
}

#define	kMaxHostResID	10000

void AddHostToPrefs(StringPtr hostName, StringPtr description) {
	unsigned short	resID;
	OSErr				err;
	Handle				descHandle = nil;

	//	add this host and description to our preferences file
	
	//	get a unique resource ID for our string. The description will be stored
	//	as the name of the resource. We use 1000 for the current session name
	
	do {
		resID = Unique1ID('STR ');
	} while (resID <= 1000);
	
	err = PtrToHand(&description[0], &descHandle, description[0] + 1);	//	string and length byte

	if ( (err == noErr) && (descHandle != nil) ){
	
		//	add the handles to the resource file
		
		AddResource(descHandle, 'STR ', resID, hostName);		//	name and store the resource
		WriteResource(descHandle);
		
	} else {
	
		//	something went wrong. Make sure we clean up our handles
	
		if (descHandle != nil) {
			DisposeHandle(descHandle);
		}
	
	}
}

