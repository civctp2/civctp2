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
 Launches application specified by appParam.
 $Log: launchap.c $
 Revision 1.1  1997/07/26 00:44:42  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include "dp.h"

/*--------------------------------------------------------------------------
 Launches an application from another application and notifies stub
  of the newly launched application, if stub exists.
--------------------------------------------------------------------------*/

DP_API dp_result_t dpLaunchApp(dp_appParam_t *appParam)
{
	/* Not yet supported */
	return dp_RES_BUG;
}
