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

#ifndef WMQFILE_H
#define WMQFILE_H

#include <time.h>

/* Time length in seconds of a wmq file before moving on to the next file.
 * Must be at least 87.  File names will be more comprehensible if they
 * divide into 86400.
 * Don't try to mix data from queues with different values set for this!
 */
#ifndef wmqfile_ROLLOVER_TIME
#define wmqfile_ROLLOVER_TIME 86400
#endif

/*--------------------------------------------------------------------------
 Returns 1 if <filename> looks like a wmq filename or 0 if not.
--------------------------------------------------------------------------*/
int wmqfile_iswmqfile(const char *filename);

/*--------------------------------------------------------------------------
 Puts the filename corresponding to <when> in <filename>, which will
 have a maximum of 16 characters (plus one for a terminating '\0').
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_forTime(time_t when, char *filename);

/*--------------------------------------------------------------------------
 Find the first wmq file sequentially after <file> in <dir>, and return its
 name in <nextfile>.  Note that <file> itself need not exist.
 Returns dp_RES_OK and <nextfile> is set on success,
 		 dp_RES_EMPTY and <nextfile> is unchanged if no file follows <file>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_next(const char *dir, const char *file, char *nextfile);

/*--------------------------------------------------------------------------
 Find the last wmq file sequentially before <file> in <dir>, and return
 its name in <prevfile>.  Note that <file> itself need not exist.
 Returns dp_RES_OK and <prevfile> is set on success,
 		 dp_RES_EMPTY and <prevfile> is unchanged if no file preceeds <file>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_prev(const char *dir, const char *file, char *prevfile);

/*--------------------------------------------------------------------------
 Find the first wmq file sequentially in <dir>, and return its name in
 <firstfile>.
 Returns dp_RES_OK and <firstfile> is set on success,
 		 dp_RES_EMPTY and <firstfile> is unchanged if no wmq files are in <dir>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_first(const char *dir, char *firstfile);

/*--------------------------------------------------------------------------
 Find the last wmq file sequentially in <dir>, and return its name in
 <lastfile>.
 Returns dp_RES_OK and <lastfile> is set on success,
 		 dp_RES_EMPTY and <lastfile> is unchanged if no wmq files are in <dir>,
		 dp_RES_NOMEM if scandir fails due to lack of memory.
--------------------------------------------------------------------------*/
dp_result_t wmqfile_last(const char *dir, char *lastfile);

#endif
