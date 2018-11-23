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
 * Functions to implement a simple string lookup system.
 * The programmer should put STRLOOKUP_INIT before any call to STRLOOKUP
 * is made, and to free up the stuff STRLOOKUP_TERMINATE should be called.
 *
 * If a file lang.txt exists in the current working directory, it will be
 * accessed to build an associated lookup table sorted by the english
 * string.  STRLOOKUP will lookup the corresponding translated string with
 * the english one and return that lookedup string.  If the english string
 * is not found in the table, then it is returned.
 *
 * STRFORMAT is used to handle constructed strings; it uses a special
 * extension to C printf format specs to associate each format spec in the
 * format string with a particular argument.  STRFORMAT format specs look
 * like %num:fmt where num is the argument #, and fmt is the standard C
 * format spec.  STRFORMAT calls STRLOOKUP on the format string before
 * parsing.
 *
 * There once was a "fast" lookup here; it was dangerously slow and was
 * removed.
 */

#if !defined(_STRLOOKUP_H_)
#define _STRLOOKUP_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _StrLookup
{
	long stringcount;				/* Number of strings in the slow array */
	struct StrLookup_table {
		char *index;				/* English */
		char *string;				/* Translation */
	} *lookup_table;
} StrLookupStruct, *StrLookup;

typedef struct StrLookup_table *Table_ptr;

StrLookup	StrLookupCreate(char *filename);
void		StrLookupDestroy(StrLookup obj);
char *		StrLookupFind(StrLookup obj, char *string);
int			StrLookupFormat(char *buffer, char *fmt, ...);

extern StrLookup StrLookup_Global_Object;

#define STRLOOKUP(str) StrLookupFind(StrLookup_Global_Object, str)
#define STRLOOKUP_INIT(file) StrLookup_Global_Object = StrLookupCreate(file)
#define STRLOOKUP_TERMINATE StrLookupDestroy(StrLookup_Global_Object)
#define STRFORMAT StrLookupFormat

#ifdef __cplusplus
}
#endif

#endif
