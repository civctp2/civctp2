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

#include "dp2.h"
#include "dptab.h"
#include "contract.h"
#include "eclock.h"

/*----------------------------------------------------------------------
 When you want to quit and terminate the game, you should poll this
 routine in a loop with dpReceive.  This will close the connection to
 the server and delete all sessions and players associated with it.
 Flags should normally be zero.

 dpShutdown will wait an additional wait_after ms after closing the
 connection to handle packet retries.
 If flags is 1, the connection to the game server will not be closed.

 Steps which are dependant on network events will time out in
 timeout ms, if timeout is non-zero.  Since there are three such
 states, the maximum total time for shut down is about
 (timeout * 3 + wait_after) ms.

 Returns dp_RES_OK when finished,
         dp_RES_BUSY while still shutting down.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpShutdown(dp_t *dp, clock_t timeout, clock_t wait_after, int flags)
{
	dp_result_t err;

	precondition(dp);

	switch(dp->quitState) {
	case 0:
		DPRINT(("dpShutdown: beginning shutdown process.\n"));
		dp->quitState++;
		break;

	case 1:
		if (dp->myplayers) {
			int i;

			DPRINT(("dpShutdown: deleting my players.\n"));
			for (i=0; i<dp_PLAYERS_PER_HOST; i++)
				err = dpDestroyPlayer(dp, (dpid_t)(i + dp->firstId));
			dp->quitDeadline = dp->now + (ECLOCKS_PER_SEC * timeout)/1000;
		}
	    dp->quitState++;
		break;

	case 2:
		if (timeout && ((long)(dp->now - dp->quitDeadline) > 0)) {
			DPRINT(("dpShutdown: delete players timed out.\n"));
		} else if (dp->players) {
			int i;
			void *buf;
			size_t len;
			char subkey[dp_KEY_MAXLEN];
			int subkeylen;
			dpid_t id;

			for (i = 0; i < dpNumPlayers(dp); i++) {
				err = dptab_get_byindex(dp->players, i, &buf, &len, subkey, &subkeylen);
				if (err != dp_RES_OK) {
					DPRINT(("dpShutdown: dptab_get_byindex(%d) returns err:%d\n", i, err));
				}
				assert(subkeylen == 2);
				id = dpMAKESHORT(subkey[0], subkey[1]);
				/* Use same code as dpEnumPlayers to detect local player */
				if (dp->firstId == (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1)))
					return dp_RES_BUSY;  /* we still have a player here */
			}
			DPRINT(("dpShutdown: done deleting players.\n"));
		}
		dp->quitState++;
		break;

	case 3:
		/* Start closing session */
		DPRINT(("dpShutdown: closing session.\n"));
		err = dpClose(dp);
		if (err != dp_RES_OK) {
			DPRINT(("dpShutdown: dpClose() returns err:%d\n", err));
		}

		dp->quitDeadline = dp->now + (ECLOCKS_PER_SEC * timeout)/1000;
		dp->quitState++;
		break;

	case 4:
		/* Finish closing session */
		if (dpReadyToFreeze(dp) != dp_RES_BUSY) {
			DPRINT(("dpShutdown: session closed.\n"));
			dp->quitState++;
		} else if (timeout && ((long)(dp->now - dp->quitDeadline) > 0)) {
			DPRINT(("dpShutdown: session close timed out.\n"));
			dp->quitState++;
		}

		break;

	case 5:
		/* Start disconnecting from server */
		if ((dp->hGameServer == PLAYER_NONE) || (flags & 1)) {
			dp->quitState += 2;	/* no server discon; skip to next step pair */
			break;
		}
		DPRINT(("dpShutdown: disconnecting from server.\n"));
		err = dpSetGameServerEx(dp, NULL, 0);
		if (err != dp_RES_OK) {
			DPRINT(("dpShutdown: dpSetGameServerEx(dp, NULL, 0) returns err:%d\n", err));
		}
		dp->quitDeadline = dp->now + (ECLOCKS_PER_SEC * timeout)/1000;
		dp->quitState++;
		break;

	case 6:
		/* Finish disconnecting */
		if (dpReadyToFreeze(dp) != dp_RES_BUSY) {
			DPRINT(("dpShutdown: disconnected.\n"));
		} else if (timeout && ((long)(dp->now - dp->quitDeadline) > 0)) {
			DPRINT(("dpShutdown: disconnect timed out.\n"));
		} else
			break;
		dp->quitState++;
		break;

	case 7:
		/* Start final wait */
		DPRINT(("dpShutdown: starting final wait of %d ms.\n", wait_after));
		dp->quitDeadline = dp->now + (ECLOCKS_PER_SEC * wait_after)/1000;
		dp->quitState++;
		break;

	case 8:
		/* Finish final wait */
		if ((long)(dp->now - dp->quitDeadline) > 0)
			dp->quitState++;
		break;

	case 9:
		/* Final exit point */
		DPRINT(("dpShutdown: done shutting down.\n"));
		dp->quitState = 0;
		return dp_RES_OK;

	default:
		DPRINT(("dpShutdown: bug: unknown quitState %d!\n", dp->quitState));
		return dp_RES_BUG;
		break;
	}
	return dp_RES_BUSY;
}
