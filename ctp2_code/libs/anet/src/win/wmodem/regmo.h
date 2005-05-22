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
 Class to extract information about installed modems from the windows 
 registry.
--------------------------------------------------------------------------*/

#ifndef regmo_h
#define regmo_h

#define regmo_STRLEN 64
#define regmo_N_INIT 8

/*------------------------------------------------------------------------- 
 Information known about a particular modem. 
 Member name are the same as the registry key names.
 See the Windows 95 Resource Kit, chapter 25, page 812.
-------------------------------------------------------------------------*/
typedef struct {
	char DriverDesc[regmo_STRLEN];
	char AttachedTo[regmo_STRLEN];
	char Init[regmo_N_INIT][regmo_STRLEN];
} regmo_t;

/*--------------------------------------------------------------------------
 Enumerate installed modems.
 The given array is filled with each installed modem's key.
 The return value is the number of keys placed in the array.
--------------------------------------------------------------------------*/
int regmo_enum(int *keys, int maxkeys);

/*--------------------------------------------------------------------------
 Read the key'th modem definition from the Windows registry.
 Key is a value gotten from regmo_enum; usually, 0 <= key <= 10.
 Returns 0 on success, non-zero on failure.
--------------------------------------------------------------------------*/
int regmo_get(regmo_t * regmo, int key);

#endif
