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

 $Log: EnterNewAddress.c $
 Revision 1.2 1997/08/05 dswarzman
 Fixed system includes.
 */

//#include <Desk.h>
#include <Types.h>
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
#include <string.h>
#include <StdArg.h>
#include <stdio.h>
#include <TextUtils.h>
#include <ToolUtils.h>
//#include <Values.h>

#include "HostDialog.h"
#include "ddprint.h"

#define thisDialogID 2000

/* Symbolic Dialog Item Numbers */

static enum {
	BUT1_OK = 1,
	BUT2_Cancel,
	STXT3_Host,
	STXT4_Notes,
	EDIT5,				//	name
	EDIT6,				//	description
	STXT7_Enter,
	LASTITEM
	} DialogItemNumber;

#define OK_ITEM 	BUT1_OK
#define CANCEL_ITEM 	BUT2_Cancel

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

DialogPtr OpenThisDialog(StringPtr nameString, StringPtr descString);
void      CloseThisDialog(DialogPtr dlog);
void      DoDialogUpdate(DialogPtr dlog);
void      DoDialogActivate(DialogPtr dlog, sint32 activ);
void      DoDialogContent(DialogPtr dlog, EventRecord *evt);
sint32       DoDialogItem(DialogPtr dlog, sint16 itemHit);

pascal  Boolean MyFilter(DialogPtr dlog, EventRecord *evt, sint16 *itemHit);
Boolean CheckUserItems(Point where, sint16 *itemHit);
sint32     AnyBadValues(DialogPtr dlog);

void    CenterWindow(WindowPtr w, sint32 top);
//sint32    strlen(char *);
//char   *strcpy(char *dst, char *src);
char   *PascalToC(uint8 *pstr);
uint8   *CToPascal(char *cstr);
void    PutDlgString(DialogPtr dlog, sint32 item, uint8 *str, sint32 sel);
void    PutDlgWord(DialogPtr dlog, sint32 item, sint32 val, sint32 sel);
void    PutDlgLong(DialogPtr dlog, sint32 item, sint32 val, sint32 sel);
void    PutDlgChkRadio(DialogPtr dlog, sint32 item, sint32 val);
sint32     GetDlgString(DialogPtr dlog, sint32 item, uint8 *str);
sint32     GetDlgWord(DialogPtr dlog, sint32 item, sint16 *val);
sint32     GetDlgLong(DialogPtr dlog, sint32 item, sint32 *val);
sint32     GetDlgChkRadio(DialogPtr dlog, sint32 item);
sint32     TextSelected(DialogPtr dlog);
OSType  CanPaste(sint32 n, ...);
void    FrameDefault(DialogPtr dlog, sint32 item, sint32 frame);
void    GetDlgPanel(DialogPtr dlog, sint32 item, Rect *panel);

static Point where;
static sint32 modifiers;

/*
 *	Display this modal dialog.  Return TRUE if OK, FALSE if CANCEL or error.
 *	If the dialog displays values from outside this module, you should either
 *	import them from globals, or change the argument list of this routine to
 *	bring them in and pass them to OpenThisDialog(), DoDialogItem(), etc.
 */

sint32 DoEnterNewAddressDialog(StringPtr name, StringPtr description)
	{
		sint16 itemHit,okay=FALSE,keepGoing=TRUE;
		DialogPtr dlog=NIL; GrafPtr oldPort;
		ModalFilterUPP MyFilterUPP;

		GetPort(&oldPort);

		/* On PowerPC, need a RoutineDescriptor from heap; on 68K, no allocation */

		MyFilterUPP = NewModalFilterProc(MyFilter);
		if (MyFilterUPP == NIL) goto cleanUp;

		/* Build dialog window and install its item values */

		dlog = OpenThisDialog(name, description);
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

		okay = (itemHit == OK_ITEM);
		if (okay) {
			GetDlgString(dlog, EDIT5, name);
			GetDlgString(dlog, EDIT6, description);
		}

		/* That's all, folks! */

cleanUp:
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

static pascal Boolean MyFilter(DialogPtr dlog, EventRecord *evt, sint16 *itemHit)
	{
		Boolean ans=FALSE,doHilite=FALSE; WindowPtr w;
		sint16 type,ch; Handle hndl; Rect box;
		static sint32 then; static Point clickPt;

		w = (WindowPtr)(evt->message);
		switch(evt->what) {
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
				if ((ch=(uint8)evt->message)=='\r' || ch==ENTERkey) {
					*itemHit = OK_ITEM /* Default Item Number here */;
					doHilite = ans = TRUE;
					}
				 else if (evt->modifiers & cmdKey) {
					ch = (uint8)evt->message;
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
				sint32 soon = TickCount() + 7;		/* Or whatever feels right */
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

static Boolean CheckUserItems(Point where, sint16 *itemHit)
	{
		return(FALSE);
	}

/*
 * Redraw the contents of this dialog due to update event.
 * If you have not installed UserItem draw routines, you should redraw
 * them explicitly here; otherwise, UpdateDialog() will call your routines.
 */

static void DoDialogUpdate(DialogPtr dlog)
	{
		GrafPtr oldPort;

		GetPort(&oldPort); SetPort(dlog);
		BeginUpdate(dlog);

		UpdateDialog(dlog,dlog->visRgn);
		FrameDefault(dlog,BUT1_OK,TRUE);

		EndUpdate(dlog);
		SetPort(oldPort);
	}

/*
 * Activate event: Activate or deactivate this dialog and any items in it
 */

static void DoDialogActivate(DialogPtr dlog, sint32 activ)
	{
		SetPort(dlog);
	}

/*
 * Build this dialog's window on desktop, and install initial item values.
 * Return the dlog opened, or NIL if error (no resource, no memory).
 */

static DialogPtr OpenThisDialog(StringPtr nameString, StringPtr descString)
	{
		sint16 type; Handle hndl; Rect box; GrafPtr oldPort;
		DialogPtr dlog; uint8 *p,str[256];
		DebugStr ( "/pOpenThisDialog" );

		DPRINT (( "OpenThisDialog - nameString : %s descString : %s",
			nameString, descString ));
		return NIL;

		GetPort(&oldPort);
		dlog = GetNewDialog(thisDialogID,NIL,FRONT_WINDOW);
		if (dlog == NIL) { SysBeep(1); return(NIL); }	/* Poor man's error message */

		CenterWindow(dlog,0);
		SetPort(dlog);

		//	insert the initial values into our fields

		PutDlgString(dlog, EDIT6, descString, FALSE);
		PutDlgString(dlog, EDIT5, nameString, TRUE);

		ShowWindow(dlog);
		return(dlog);

	}

/*
 * Clean up any allocated stuff, and return dialog to primordial mists
 */

static void CloseThisDialog(DialogPtr dlog)
	{
		if (dlog) {
			DisposeDialog(dlog);	/* Call CloseDialog if you provide storage to GetNewDialog */
		}
	}

/*
 * Deal with user clicking on an item in this dialog, either modal or non-modal.
 * The local point is in where; modifiers in modifiers.
 * Returns whether or not the dialog should be closed (keepGoing).
 */

static sint32 DoDialogItem(DialogPtr dlog, sint16 itemHit) {
		sint16 type,okay=FALSE,keepGoing=TRUE,val;
		Handle hndl; Rect box; Point pt;
		uint8 *p,str[256];

		if (itemHit<1 || itemHit>=LASTITEM)
			return(keepGoing);				/* Only legal items, please */

		GetDialogItem(dlog,itemHit,&type,&hndl,&box);
		switch(type) {
			case ctrlItem+btnCtrl:
				switch(itemHit) {
					case BUT1_OK:

						//	the default is that we're done

						keepGoing = FALSE; okay = TRUE;

						//	check to see if the name that has been entered can be resolved

						{
							extern InetSvcRef	gInetService;

							Str255				theString;
							InetHostInfo		theHost;
							sint16				aShort;
							OSStatus			err;

							GetDlgString(dlog, EDIT5, theString);		//	machine name
							ParamText(theString, "\p", "\p", "\p");		//	show the name in the dialog
							p2cstr(theString);

							OTSetSynchronous(gInetService);
							err = OTInetStringToAddress(gInetService, theString, &theHost);
							OTSetAsynchronous(gInetService);
							if (err != noErr) {

								//	the name cannot be looked up, ask the user if they want
								//	to add the entry anyway

								aShort = CautionAlert(1501, nil);		//	returns 1 if OK to add
								if (aShort != 1) {
									keepGoing = TRUE; okay = FALSE;
								}

							}

						}

						break;
					case BUT2_Cancel:
						keepGoing = FALSE;
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
					case STXT3_Host:		/* NOT Enabled */
						break;
					case STXT4_Notes:		/* NOT Enabled */
						break;
					case STXT7_Enter:		/* NOT Enabled */
						break;
					}
				break;
			case editText:
				switch(itemHit) {
					case EDIT5:
						break;
					case EDIT6:
						break;
					}
				break;
			case iconItem:
				break;
			case picItem:
				break;
			case userItem:
				break;
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

static sint32 AnyBadValues(DialogPtr dlog)
	{
		uint8 str[256]; sint16 val,len;

		if (GetDlgString(dlog,EDIT5,str)) {
			/* Got a string (can also call GetDlgWord(), etc. here) */
			}
		else {
			/* Field was empty */
			}
		if (GetDlgString(dlog,EDIT6,str)) {
			}
		else {
			}
		return(FALSE);
	}

/*€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€*/

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

static void CenterWindow(WindowPtr w, sint32 top)
	{
		Rect scr; Point p;
		sint32 rsize,size,margin,xoff,yoff;

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

static char *PascalToC(uint8 *str)
	{
		register uint8 *p,*q,*end;

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

static uint8 *CToPascal(char *str)
	{
		register char *p,*q;
		register sint32 len;

		len = strlen(str);
		if (len > 255) len = 255;
		p = str + len;
		q = p-1;
		while (p != str) *p-- = *q--;
		*str = len;
		return((uint8 *)str);
	}

/* Dialog Item Stuffers */

/*
 *	Install a given Pascal string, str, into the given static or edit text item
 *	in the dialog, dlog.  If the item is an edit text item, leave the installed
 *	text selected or not according to the value of sel (TRUE or FALSE).
 */

static void PutDlgString(DialogPtr dlog, sint32 item, uint8 *str, sint32 sel)
	{
		sint16 type; Handle hndl; Rect box;

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

static void PutDlgLong(DialogPtr dlog, sint32 item, sint32 val, sint32 sel)
	{
		uint8 str[32];

		NumToString(val,str);
		PutDlgString(dlog,item,str,sel);
	}

/*
 *	Same as above, only for an sint32 (word) decimal number.
 */

static void PutDlgWord(DialogPtr dlog, sint32 item, sint32 val, sint32 sel)
	{
		PutDlgLong(dlog,item,(sint32)val,sel);
	}

/*
 *	Set the given check box or radio button item of the given dialog, dlog, to
 *	on or off, according to val.
 */

static void PutDlgChkRadio(DialogPtr dlog, sint32 item, sint32 val)
	{
		sint16 type; Handle hndl; Rect box;

		GetDialogItem(dlog,item,&type,&hndl,&box);
		SetControlValue((ControlHandle)hndl,val!=0);
	}

/*
 *	Deliver the value of the checkbox or radio button item of the given dialog.
 */

static sint32 GetDlgChkRadio(DialogPtr dlog, sint32 item)
	{
		sint16 type; Handle hndl; Rect box;

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

static sint32 GetDlgString(DialogPtr dlog, sint32 item, uint8 *str)
	{
		sint16 type; Handle hndl; Rect box;

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

static sint32 GetDlgLong(DialogPtr dlog, sint32 item, sint32 *val)
	{
		sint32 ans; uint8 str[256];

		*val = 0;
		ans = GetDlgString(dlog,item,str);
		if (ans)
			StringToNum(str,val);
		return(ans);
		}

/* Same as above, only delivers the value of a word */

static sint32 GetDlgWord(DialogPtr dlog, sint32 item, sint16 *val)
	{
		sint32 ans; sint32 num;

		*val = 0;
		ans = GetDlgLong(dlog,item,&num);
		if (ans)
			*val = num;
		return(ans);
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

static OSType CanPaste(sint32 n, ...)
	{
		register OSType nextType,ans = 0L;
		sint32 err,offset;
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
 *	Get rectangle, *panel, for a given item (usually a user or picture item)
 *	and then hide the item so that it doesn't interfere with mouse clicking.
 *	This lets you stop worrying about the item order any user or pict items that
 *	obscure other items in the item list, which can affect how the DialogMgr
 *	returns itemHits.
 */

static void GetDlgPanel(DialogPtr dlog, sint32 item, Rect *panel)
	{
		sint16 type; Handle hndl;

		GetDialogItem(dlog,item,&type,&hndl,panel);
		HideDialogItem(dlog,item);
	}

#endif
