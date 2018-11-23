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
#include "../tca/tserv.h"
#include "contract.h"
#include "dpprivy.h"

/*------------------------------------------------------------------------
 After using dpSetGameServerEx(),
 call this function to log in to an existing account on the server.
 dpReceive() will later return a packet of type dp_ACCOUNT_PACKET_ID to
 indicate success or failure, or to tell you to call dpAccountActivate().

 'username' is the user name specified by the user in dpAccountCreate().
 'password' is the password set by the user in dpAccountCreate() (or the
            last dpChangePassword() call).
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpAccountLoginW(
	dp_t *dp,
	const wchar_t *username,
	const wchar_t *password)
{
	precondition(dp);
	return tserv_account_loginW(dp->tserv, username, password);
}

DP_API dp_result_t DP_APIX dpAccountLoginA(
	dp_t *dp,
	const char *username,
	const char *password)
{
	precondition(dp);
	return tserv_account_loginA(dp->tserv, username, password);
}

#if 0
/*------------------------------------------------------------------------
 After using dpSetGameServerEx(),
 call this function to log in to create a new account on the server.
 dpReceive() will later return a packet of type dp_ACCOUNT_PACKET_ID
 to indicate failure or, if successful, to tell you to call
 dpAccountActivate().  On success, the server will email a secret code
 for use in activating the account to the email address given here.

 'username' is the user name desired.
 'password' is the password desired.
 'flags' is a union of tcapw_entry_FLAGS_* bit flags (see tcapw.h).
 'email' is the valid internet address of the new user.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpAccountCreateW(
	dp_t *dp,
	const wchar_t *username,
	const wchar_t *password,
	int flags,
	const wchar_t *email)
{
	precondition(dp);
	return tserv_account_createW(dp->tserv, username, password, flags, email);
}

DP_API dp_result_t DP_APIX dpAccountCreateA(
	dp_t *dp,
	const char *username,
	const char *password,
	int flags,
	const char *email)
{
	precondition(dp);
	return tserv_account_createA(dp->tserv, username, password, flags, email);
}
#endif

/*------------------------------------------------------------------------
 Call this function after receiving a packet telling you to activate
 your account in response to dpAccountLogin() or dpAccountCreate()
 to activate your account.

 'secretcode' is the secret code emailed to you by the server when the
              account was created.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpAccountActivateW(
	dp_t *dp,
	const wchar_t *secretcode)
{
	precondition(dp);
	return tserv_account_activateW(dp->tserv, secretcode);
}

DP_API dp_result_t DP_APIX dpAccountActivateA(
	dp_t *dp,
	const char *secretcode)
{
	precondition(dp);
	return tserv_account_activateA(dp->tserv, secretcode);
}

#if 0
/*------------------------------------------------------------------------
 Once logged in using dpAccountLogin() or dpAccountCreate(), call this
 function to change your password, flags, or email address.  You need not
 be activated.

 'password' is the password set with dpAccountCreate() (or the last
            dpChangePassword() call).
 'newpassword' is the new password desired.
 'flags' is the new flags bit field desired.
 'email' is the new email address desired.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpChangePasswordW(
	dp_t *dp,
	const wchar_t *password,
	const wchar_t *newpassword,
	int flags,
	const wchar_t *email)
{
	precondition(dp);
	return tserv_change_passwordW(dp->tserv, password, newpassword, flags, email);
}

DP_API dp_result_t DP_APIX dpChangePasswordA(
	dp_t *dp,
	const char *password,
	const char *newpassword,
	int flags,
	const char *email)
{
	precondition(dp);
	return tserv_change_passwordA(dp->tserv, password, newpassword, flags, email);
}
#endif

/*------------------------------------------------------------------------
 Call this function after receiving a packet telling you to activate
 your account in response to dpAccountLogin() or dpAccountCreate()
 to request that your secret code be emailed again.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpRequestEmail(dp_t *dp)
{
	precondition(dp);
	return tserv_request_email(dp->tserv);
}

/*------------------------------------------------------------------------
 Convert a player id (dpid_t) to a user id (dp_uid_t).
 Returns dp_UID_NONE on any error.
------------------------------------------------------------------------*/
DP_API dp_uid_t DP_APIX dpGetPlayerUid(dp_t *dp, dpid_t id)
{
	dp_result_t err;
	dp_playerId_t player;
	char *playerbuf;
	size_t len_used;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);
	if (!dp->players) return dp_UID_NONE;
	if (!dp->groups) return dp_UID_NONE;

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	if (id >= dp->firstGId && id < dp->firstGId + dp_MAX_GROUPS) {
		DPRINT(("dpGetPlayerUid: player groups don't have UIDs\n"));
		return dp_UID_NONE;
	}

	err = dptab_get_bykey(dp->players, subkey, subkeylen, (void **)&playerbuf, &len);
	if (err != dp_RES_OK) {
		DPRINT(("dpGetPlayerUid: dptab_get_bykey(players, id:%d) returns err:%d\n", id, err));
		dp_assertValid(dp);
		return dp_UID_NONE;
	}
	len_used = dp_unpack_playerId(id, playerbuf, &player);
	if (len != len_used) {
		DPRINT(("dpGetPlayerUid: can't unpack player %d.\n", id));
		dp_assertValid(dp);
		return dp_UID_NONE;
	}

	if ((unsigned long)player.karma == dp_UID_NONE)
		return dp_UID_NONE;  /* Better not let UID get to 131072! FIXME */

	/* Kludge: only the lower 16 bits of uid available at moment */
	return (dp_uid_t)(0x10000 + (unsigned long)(player.karma));
}
