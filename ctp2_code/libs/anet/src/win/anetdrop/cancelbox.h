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

#ifndef cancelbox_h
#define cancelbox_h

/*--------------------------------------------------------------------------
 create a modeless dialog box with a single cancel button.
 msg is the text to display in the dialog box
 The caption is used only if it is passed into CreateWindows.
--------------------------------------------------------------------------*/
void cancelbox_set(const char *caption, const char *msg);

/*--------------------------------------------------------------------------
 Hide a cancelbox
--------------------------------------------------------------------------*/
void cancelbox_clear();

/*--------------------------------------------------------------------------
 call this often while the cancelbox is on the screen.
 It returns TRUE if the user has clicked on Cancel.
--------------------------------------------------------------------------*/
int cancelbox_poll(void);

#endif
