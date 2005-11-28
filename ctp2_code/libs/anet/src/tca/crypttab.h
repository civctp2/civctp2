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

/*-----------------------------------------------------------------------
 A simple dynamically sized table, which writes to disk in an encrypted
 format.
-----------------------------------------------------------------------*/

#ifndef crypttab_h
#define crypttab_h

#include <stddef.h>
#include "dynatab.h"
#include "3rdparty/d3des/d3des.h"

/*-------------------------------------------------------------------------
 Write the dynatab to the given file in a format encrypted with key,
 suitable for restoration with dynatab_thaw_encrypted.
-------------------------------------------------------------------------*/
void dynatab_freeze_encrypted(dynatab_t *tab, FILE *fp, const unsigned char key[8]);

/*-------------------------------------------------------------------------
 Read the dynatab (encrypted with key) from the given file.
 Returns a pointer to the first element on success or NULL on failure.
-------------------------------------------------------------------------*/
void *dynatab_thaw_encrypted(dynatab_t *tab, FILE *fp, const unsigned char key[8]);

#endif
