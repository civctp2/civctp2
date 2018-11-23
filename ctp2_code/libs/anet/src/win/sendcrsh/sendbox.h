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

/*--------------------------------------------------------------------------
 Small module for creating a send/cancel dialog box.
--------------------------------------------------------------------------*/

#ifndef sendbox_H
#define sendbox_H

#define sendbox_CLICKED_NONE		0
#define sendbox_CLICKED_OK			1
#define sendbox_CLICKED_CANCEL		2

/*--------------------------------------------------------------------------
 Create and show the send dialog box.
 The CRC field will display signature.
 The comments box will accept commentLen characters.
 If timeout is non-zero, the dialog box will cancel automatically in
 timeout seconds if the user does not interact with it.
 Returns 0 on success, non-zero on failure.
--------------------------------------------------------------------------*/
int sendbox_create(int iCmdShow, char *signature, int commentLen, int timeout);

/*--------------------------------------------------------------------------
 Process any window events for the send dialog box.
 If OK is returned, up to commentLen characters of text in the comments box
 are returned in comment.
 Returns sendbox_CLICKED_NONE if no interesting events have occured,
         sendbox_CLICKED_CANCEL if the user clicked cancel,
		 sendbox_CLICKED_OK if the user clicked OK.
--------------------------------------------------------------------------*/
int sendbox_poll(char *comment);

/*--------------------------------------------------------------------------
 Destroy the dialog box.
--------------------------------------------------------------------------*/
void sendbox_destroy();

#endif
