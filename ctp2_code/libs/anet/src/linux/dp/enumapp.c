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
 Enumerate applications installed on this machine.
 Copied from DOS version.

 $Log: enumapp.c $
 Revision 1.1  1997/07/22 18:17:08  anitalee
 Initial revision
--------------------------------------------------------------------------*/
 
#include "dp.h"


#if defined(dp_ANET2)

/*--------------------------------------------------------------------------
Save the apps list to disk.
(Called at quit time to provide quick info on next startup).
--------------------------------------------------------------------------*/
dp_result_t			/* status */
dp_freezeAppsList(
	dp_t *dp)		/* Owner of apps table */
{
	/* Not implemented */
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
Subscribe to the server's table. 
----------------------------------------------------------------------*/
dp_result_t			/* status */
dp_subscribeAppsList(
	dp_t *dp)		/* Owner of apps table */
{
	/* Not implemented */
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Determine the current application's version, and whether it needs to be 
 updated.
 Returns info about the current app in *app.  Strings are allocated with
 strdup.

 Returns dp_RES_OK if update needed,
 dp_RES_EMPTY if no update needed,
 other values on failure.
----------------------------------------------------------------------*/
DP_API dp_result_t			/* status */
dpGetAppVersion(
	dp_t* dp,
	dp_appParam_t *app)
{
	/* Not implemented */
	return dp_RES_BUG;
}

/*-------------------------------------------------------------------------
 Call to download the patch corresponding to the given product,
 but only if dpGetAppVersion says you need to.
 app is the structure filled in by dpGetAppVersion.
 Returns dp_RES_OK if patch is downloaded and running;
 dp_RES_EMPTY if no patch needed,
 dp_RES_BAD if can't find updater dll (anet/dkupddll.dll).
 If it returns dp_RES_OK, you should exit your application immediately!
-------------------------------------------------------------------------*/
DP_API dp_result_t dpDownloadUpdate(dp_t *dp, const dp_appParam_t *app)
{
	/* Not implemented */
	return dp_RES_BUG;
}

#endif

/*----------------------------------------------------------------------
 Enumerate applications installed on this machine.
 Callback is called once for each application found.
 Callback ends when dpEnumApp() exits.
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnumApp(dp_t* dp, dpEnumAppCallback_t cb, void *context)
{
	/* Not implemented */
	return dp_RES_BUG;
}
