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
 Functions to find out about Windows 95's dialup Internet system.

 Copyright 1998 Activision.
 Dan Kegel
--------------------------------------------------------------------------*/

#ifndef adialup_h
#define adialup_h

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 Return TRUE if autodial is enabled.
--------------------------------------------------------------------------*/
int adialup_autodial_enabled(void);

/*--------------------------------------------------------------------------
 Enable or disable autodial.
 Return TRUE on success.
--------------------------------------------------------------------------*/
int adialup_autodial_set(int enable);

/*--------------------------------------------------------------------------
 Return TRUE if there is an active, ready-for-data, dialup connection,
 or if there are no dialup connections, active or not.
 Return FALSE if there are dialup connections, but none are active.
--------------------------------------------------------------------------*/
int adialup_is_active(void);

/*--------------------------------------------------------------------------
 Return TRUE if Internet activity is likely to trigger an autodial.
 This is same as
    adialup_autodial_enabled() && !adialup_is_active()
--------------------------------------------------------------------------*/
int adialup_willdial(void);

#ifdef __cplusplus
}
#endif

#endif
