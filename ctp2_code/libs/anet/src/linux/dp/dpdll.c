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

/*----------------------------------------------------------------------
 DLL loading, unloading, enumerating, and diddling functions for dp.
 Copyright (C) 1996, Activision.

 Whittled down to nothing for use with static library in an environment
 where only the UDP transport is available, and exports the symbol
 uudp_commDriverInfo.

 $Log: dpdll.c $
 Revision 1.3  1997/07/28 19:42:17  lbennett
 Conform to anet.h/dpdll.h changes (dp_transport_t, etc.)
 Revision 1.2  1997/03/06 03:25:31  geychaner
 Changed LoadDLL and UnloadDLL to void * arguments (same as .h file)
 Revision 1.1  1996/12/16 03:09:51  dkegel
 Initial revision
----------------------------------------------------------------------*/

#include <stddef.h>

#define DEBUG_MODULE TRUE   /* enable DPRINT in this file */
#include "dprint.h"
#include "commapi.h"

#include "dp.h"

/* Horrible kludge */
extern comm_driverInfo_t uudp_commDriverInfo;

/*----------------------------------------------------------------------
 Read description out of the DLL file.
----------------------------------------------------------------------*/
dp_result_t dpGetTransportInfo(
	dp_transport_t *path,
	comm_driverInfo_t *info)
{
	(void) path;

	*info = uudp_commDriverInfo;

	DPRINT(("dpGetTransportInfo(%s): needs %x, capabilities %x\n",
		path, info->needs, info->capabilities));

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Calls the given function once for each transport available.
 The given function should copy rather than save pointers to the transports 
 it is passed.
----------------------------------------------------------------------*/
dp_result_t dpEnumTransports(
	dp_transport_t *path,
	dpEnumTransportCallback_t cb,
	void *context)
{
	comm_driverInfo_t info;
	static dp_transport_t dummy;	/* static just in case they misbehave */

	/* Only one transport available.
	 * Get its info, and pass to the callback.
	 */
	memset(&dummy, 0, sizeof(dummy));
	strcpy(dummy.fname, "udp");
	dpGetTransportInfo(&dummy, &info);
	cb(&dummy, &info, context);

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Called internally by dpCreate to load a DLL.
----------------------------------------------------------------------*/
dp_result_t dpLoadDLL(
	dpio_t *dpio,
	const dp_transport_t *transportName)
{
	(void) dpio;
	(void) transportName;
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Called internally by dpDestroy to unload a DLL.
----------------------------------------------------------------------*/
void dpUnloadDLL(dpio_t *dpio)
{
	(void) dpio;
}

/*-------------------------------------------------------------------------
 Call this during or after dpEnumTransports, but before dpCreate,
 and only on transports which have the comm_INIT_NEEDS_PORTNUM bit set
 in comm_driverInfo_t.needs.
 Present the returned list of port names to the user, then use the
 matching portnum as the value for commInitReq_t.portnum in dpCreate.

 On entry, 
 transport is the name (from dpEnumTransports) of the transport to query.
 ports is an array to be filled with portnames.
 maxports is the size of the ports array.

 On exit, 
 ports is filled with descriptions of the available ports.
 *pnPorts is set to the number of portnames placed in the ports array.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumPorts(const dp_transport_t *transport, commPortName_t *ports, int maxports, int *pnPorts)
{
	/* unimplemented */
	return dp_RES_BAD;
}
