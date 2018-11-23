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

/* DRK:
 * This file must be linked in to each transport DLL
 * to allow the main program to tell us what rectangle to use
 * for any user interface we might need to do..
 */
#include	<Palettes.h>
#include	"MacSystemCode.h"
#include	"commapi.h"

#include "TransportUtils.h"	// MAD

Rect					gScreenRect = {-1,-1,-1,-1};		// MAD
FORCESCREENUPDATE		gScreenFunc = nil;

#pragma export on
void SetScreenRect(Rect* theRect) {
	gScreenRect = *theRect;
}
void SetScreenFunc(FORCESCREENUPDATE theFunc) {
	gScreenFunc = theFunc;
}
#pragma export off

void CenterOnCurrentScreen(Rect* theRect) {
	Point		topLeft;
	//	center the specified rectangle on the current screen

	if (gScreenRect.left == -1 && gScreenRect.right == -1 && gScreenRect.top == -1 && gScreenRect.bottom == -1)
		gScreenRect = (**GetMainDevice()).gdRect;

	topLeft.h = gScreenRect.left + ((gScreenRect.right - gScreenRect.left) - (theRect->right - theRect->left)) / 2;
	topLeft.v = gScreenRect.top + ((gScreenRect.bottom - gScreenRect.top) - (theRect->bottom - theRect->top)) / 2;

	OffsetRect(theRect, topLeft.h, topLeft.v);

}
void ColorDebugStr(StringPtr theString) {
	RestoreDeviceClut(GetMainDevice());
	DebugStr(theString);
}
