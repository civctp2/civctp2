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

/*----------------------------------------------------------------------------
 Functions to do I/O on dp_launchParams_t.
----------------------------------------------------------------------------*/

#ifndef LAUNCHPARAM_H
#define LAUNCHPARAM_H

#include "dp2.h"

/*----------------------------------------------------------------------------
 Read one field of a dp_launchParams_t from a string.
----------------------------------------------------------------------------*/
dp_result_t dp_launchParams_parse(dp_launchParams_t *app, char *line);

#endif 
