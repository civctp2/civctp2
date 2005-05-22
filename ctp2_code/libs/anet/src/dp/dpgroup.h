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

/*-------------------------------------------------------------------------
 Multiplayer game layer: group management.
 Definitions for non-user-visible functions.

 Copyright (C) 1997, Activision.

 $Log: dpgroup.h $
 Revision 1.6  1997/03/10 03:30:02  dkegel
 Let dp delete players from all groups when players go away.
 Revision 1.5  1997/02/16 04:06:54  dkegel
 dpGroup_handleAddGroupPacket now checks length itself.
 Revision 1.4  1997/02/13 00:29:18  dkegel
 Groups now saved by freeze/thaw.
 Revision 1.3  1997/02/12 03:03:15  dkegel
 Finished converting to use playerHdl_t instead of dpid_t for sending
 group info.
 Added more null-pointer checks.
 Revision 1.2  1997/02/10 07:10:44  dkegel
 1. Added handleAddGroupPacket, handleDelGroupPacket.
 2. Fixed bug in sendAddGroupPacket.
 Revision 1.1  1997/02/07 00:08:28  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef dpgroup_h
#define dpgroup_h

#include <stdio.h>
#include "dp.h"

/*----------------------------------------------------------------------
 Create the dp->groups object.  Return dp_RES_OK upon success.
 Called by dpOpen and its ilk.
----------------------------------------------------------------------*/
dp_result_t dpGroup_create(dp_t *dp);

/*----------------------------------------------------------------------
 Destroy the dp->groups object.  Don't destroy it if it's NULL.
 Call this when opening a session.
 Called by dpClose and its ilk.
----------------------------------------------------------------------*/
void dpGroup_destroy(dp_t *dp);

/*-----------------------------------------------------------------------
 Save the state of the groups table to disk.
-----------------------------------------------------------------------*/
dp_result_t dpGroup_freeze(dp_t *dp, FILE *fp);

/*-----------------------------------------------------------------------
 Restore the state of the groups table from disk.
-----------------------------------------------------------------------*/
dp_result_t dpGroup_thaw(dp_t *dp, FILE *fp);

/*--------------------------------------------------------------------------
 Notify a new player about every existing player group.

 Called only by dpReceive on host.

 Should only be called if
 we're the host,
 we're creating a new player,
 the new player is remote,
 this is the first time we've received a request to create him,
 AND it's a pseudoplayer.
--------------------------------------------------------------------------*/
dp_result_t dpGroup_InformNewMachine(
	dp_t *dp,
	playerHdl_t dest);

/*----------------------------------------------------------------------
 Handle an add group packet.  Creates the group locally.
----------------------------------------------------------------------*/
dp_result_t dpGroup_HandleAddGroupPacket(
	dp_t	*dp,
	dp_group_t *body,	/* real packet variable length */
	size_t len);

/*----------------------------------------------------------------------
 Handle an del group packet.  Deletes the group locally.
----------------------------------------------------------------------*/
dp_result_t dpGroup_HandleDelGroupPacket(
	dp_t	*dp,
	dp_user_delGroup_packet_t *body);

/*----------------------------------------------------------------------
 Add player idPlayer to the group idGroup, but don't broadcast notification.
 Called only by dpReceive and dpAddPlayerToGroup.
----------------------------------------------------------------------*/
dp_result_t dpGroup_AddPlayerLocal(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer);

/*----------------------------------------------------------------------
 Delete player idPlayer from idGroup, but don't broadcast notification.
 Called only by dpReceive and dpDeletePlayerFromGroup.
----------------------------------------------------------------------*/
dp_result_t dpGroup_DeletePlayerLocal(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer);

/*----------------------------------------------------------------------
 Delete player idPlayer from all groups, but don't broadcast notification.
----------------------------------------------------------------------*/
dp_result_t dpGroup_DeletePlayerFromAllGroups(
	dp_t	*dp,
	dpid_t	idPlayer);

#endif
