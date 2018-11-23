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
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been modified by the Apolyton CtP2 Source Code Project.
 * Contact the authors at ctp2source@apolyton.net.
 *
 * Modifications from the Activision Anet 0.10 code:
 *
 * - 2005/03/11: Modified uid assignment to prevent a compiler complaint.
 *
 *----------------------------------------------------------------------------
 */

/*--------------------------------------------------------------------------
 Server side code to authenticate clients and keep clients up to date
 on which of their friends are logged in.
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 The client should call tserv_account_login/create/activate to request a
 send_response of the given type, and call tserv_poll periodically to
 process the requests and any necessary resends.

 To create a new account, the client should call tserv_account_create
 to connect and submit the new user info followed by tserv_request_email to
 request that a code for activating the account be emailed.

 To activate an account, the client should call tserv_account_login
 to connect followed by tserv_account_activate to submit the emailed code.

 To log in to an activated account, the client should simply call
 tserv_account_login.
--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <crtdbg.h>
#define ASSERTMEM() assert(_CrtCheckMemory())
#else
#define ASSERTMEM()
#endif

#include "dp2.h"
#include "tserv.h"
#include "eclock.h"
#include "contract.h"
#include "mywcs.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "dppack1.h"

/* All the packets this module sends can be represented by this type.
 * Their length varies, though, and sizeof(this type) isn't very useful.
 */
typedef struct {
	dp_packetType_t type;
	union {
		tserv_packet_t tserv;
		char buf[dpio_MAXLEN_UNRELIABLE];
	} u;
} PACK tserv_wrappedPacket_t;

#include "dpunpack.h"

/*--------------------------------------------------------------------------
 Used in both client and server.
 Create a tserv_t using an existing dpio and tca.
 Returns NULL on failure.
--------------------------------------------------------------------------*/
tserv_t *tserv_create(dpio_t *dpio, tca_t *tca)
{
	tserv_t *tserv;

	if (dpio == NULL || tca == NULL) {
		DPRINT(("tserv_create: NULL dpio or tca\n"));
		return NULL;
	}
	ASSERTMEM();
 	if ((tserv = (tserv_t *)malloc(sizeof(tserv_t))) == NULL)
		return NULL;
	memset(tserv, 0, sizeof(tserv_t));
	tserv->magic = tserv_MAGIC;
	tserv->dpio = dpio;
	tserv->tca = tca;
	tserv->tnextPoll = eclock() + ECLOCKS_PER_SEC;
	ASSERTMEM();

	if ((tserv->clients = assoctab_create(sizeof(tserv_clientinfo_t))) == NULL)
		return NULL;

	tserv->hServer = PLAYER_NONE;
	tserv->please_send_response = FALSE;
	tserv->please_send_newuser = FALSE;
	tserv->please_send_secretcode = FALSE;
	tserv->userinfo.uid = tcapw_UID_NONE;
	tserv->challenge_set = FALSE;
	ASSERTMEM();
	tserv_assertValid(tserv);
	return tserv;
}

/*--------------------------------------------------------------------------
 Used in both client and server.
 Destroy a tserv_t.
--------------------------------------------------------------------------*/
void tserv_destroy(tserv_t *tserv)
{
	if (tserv == NULL)
		return;
	tserv_assertValid(tserv);
	assoctab_destroy(tserv->clients);
	memset(tserv, 0, sizeof(tserv_t));
	free(tserv);
	ASSERTMEM();
}

#if 0 && (defined(DPRNT) || defined(DEBUG) || defined(_DEBUG))
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
static const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 256; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}
#endif

/*--------------------------------------------------------------------------
 Freeze a tserv_t to file.
 Returns dp_RES_OK on success,
 		 dp_RES_FULL on write failure.
--------------------------------------------------------------------------*/
dp_result_t tserv_Freeze(tserv_t *tserv, FILE *fp)
{
	dp_result_t err;
	int i;

	precondition(tserv);
	precondition(fp);
	tserv_assertValid(tserv);

	/* tserv server/peer info */
	DPRINT(("tserv_Freeze: writing %d clients and my info\n", tserv->clients->n_used));
	if (fwrite(&(tserv->clients->n_used), sizeof(int), 1, fp) != 1)
		return dp_RES_FULL;
	for (i = 0; i < tserv->clients->n_used; i++) {
		assoctab_item_t* item = assoctab_getkey(tserv->clients, i);
		tserv_clientinfo_t *pci = (tserv_clientinfo_t *)item->value;
		playerHdl_t hClient = item->key;
		assert(pci);
		err = dpio_freezeHdl(tserv->dpio, hClient, fp);
		if (err != dp_RES_OK) {
			DPRINT(("tserv_Freeze: error freezing h:%x, err:%d\n", hClient, err));
			return err;
		}
		/* DPRINT(("tserv_Freeze: h:%x loggedin:%d activated:%d need_send:%c\n",
		 * 			hClient, pci->logged_in, pci->activated, pci->need_send));
		 * DPRINT(("tserv_Freeze:     challenge (uses:%d) %s\n", pci->challenge_uses, hexstring(pci->challenge.challenge, tca_LEN_CHALLENGE)));
		 */
		if ((fwrite(&(pci->logged_in), sizeof(int), 1, fp) != 1) ||
			(fwrite(&(pci->activated), sizeof(int), 1, fp) != 1) ||
			(fwrite(&(pci->challenge), sizeof(tca_challenge_t), 1, fp) != 1) ||
			(fwrite(&(pci->challenge_uses), sizeof(int), 1, fp) != 1) ||
			(fwrite(&(pci->need_send), sizeof(char), 1, fp) != 1) ||
			(fwrite(&(pci->reason), sizeof(dp_result_t), 1, fp) != 1))
			return dp_RES_FULL;
	}

	/* tserv client info */
	/* DPRINT(("tserv_Freeze: hServer:%x\n", tserv->hServer)); */
	err = dpio_freezeHdl(tserv->dpio, tserv->hServer, fp);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_Freeze: error freezing hServer:%x, err:%d\n", tserv->hServer, err));
		return err;
	}
	/* DPRINT(("tserv_Freeze: uid:%d email:%s flags:%d\n", tserv->userinfo.uid, tserv->userinfo.email, tserv->userinfo.flags));
	 * DPRINT(("tserv_Freeze: challenge_set:%d logged_in:%d activated:%d waiting_for_rx:%d\n", tserv->challenge_set, tserv->logged_in, tserv->activated, tserv->waiting_for_rx));
	 * DPRINT(("tserv_Freeze: challenge (set:%d) %s\n", tserv->challenge_set, hexstring(tserv->challenge.challenge, tca_LEN_CHALLENGE)));
	 * DPRINT(("tserv_Freeze: please_send_response:%d newuser:%d secretcode:%d pwchage:%d\n", tserv->please_send_response, tserv->please_send_newuser, tserv->please_send_secretcode, tserv->please_send_pwchange));
	 */
	if ((fwrite(&(tserv->userinfo.uid), sizeof(dp_uid_t), 1, fp) != 1) ||
		(fwrite(&(tserv->userinfo.uname), sizeof(tcapw_uname_t), 1, fp) != 1) ||
		(fwrite(&(tserv->userinfo.hpw), sizeof(tcapw_hpw_t), 1, fp) != 1) ||
		(fwrite(tserv->userinfo.email, tcapw_MAXLEN_EMAIL, 1, fp) != 1) ||
		(fwrite(&(tserv->userinfo.flags), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->userinfo.secretcode), sizeof(short), 1, fp) != 1) ||
		(fwrite(&(tserv->userinfo.created), sizeof(time_t), 1, fp) != 1) ||
		(fwrite(&(tserv->userinfo.lastlogin), sizeof(time_t), 1, fp) != 1) ||
		(fwrite(&(tserv->secretcode), sizeof(tcapw_pw_t), 1, fp) != 1) ||
		(fwrite(&(tserv->challenge_set), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->challenge), sizeof(tca_challenge_t), 1, fp) != 1) ||
		(fwrite(&(tserv->logged_in), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->activated), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->please_send_response), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->please_send_newuser), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->please_send_secretcode), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->please_send_pwchange), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(tserv->waiting_for_rx), sizeof(int), 1, fp) != 1))
		return dp_RES_FULL;

	err = tca_Freeze(tserv->tca, fp);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_Freeze: tca_Freeze returns err:%d\n", err));
		return err;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Thaw a tserv_t from file.
 tserv is a pointer to a tserv_t created with tserv_create and a valid tca.
 Returns dp_RES_OK on success,
 		 dp_RES_EMPTY on read failure.
--------------------------------------------------------------------------*/
dp_result_t tserv_Thaw(tserv_t *tserv, FILE *fp)
{
	dp_result_t err;
	int i, nClients;
	clock_t now;

	precondition(tserv);
	precondition(fp);

	/* tserv server/peer info */
	if (fread(&nClients, sizeof(int), 1, fp) != 1)
		return dp_RES_EMPTY;
	DPRINT(("tserv_Thaw: reading %d clients and my info\n", nClients));
	for (i = 0; i < nClients; i++) {
		playerHdl_t hClient;
		tserv_clientinfo_t *pci;

		err = dpio_thawHdl(tserv->dpio, &hClient, fp);
		if (err != dp_RES_OK) {
			DPRINT(("tserv_Thaw: error thawing hClient[%d], err:%d\n", i, err));
			return err;
		}
		pci = (tserv_clientinfo_t *)assoctab_subscript_grow(tserv->clients, hClient);
		assert(pci);
		if ((fread(&(pci->logged_in), sizeof(int), 1, fp) != 1) ||
			(fread(&(pci->activated), sizeof(int), 1, fp) != 1) ||
			(fread(&(pci->challenge), sizeof(tca_challenge_t), 1, fp) != 1) ||
			(fread(&(pci->challenge_uses), sizeof(int), 1, fp) != 1) ||
			(fread(&(pci->need_send), sizeof(char), 1, fp) != 1) ||
			(fread(&(pci->reason), sizeof(dp_result_t), 1, fp) != 1))
			return dp_RES_EMPTY;
		/* DPRINT(("tserv_Thaw: h:%x logged_in:%d activated:%d need_send:%c\n",
			hClient, pci->logged_in, pci->activated, pci->need_send));
		 * DPRINT(("tserv_Thaw:     challenge (uses:%d) %s\n", pci->challenge_uses, hexstring(pci->challenge.challenge, tca_LEN_CHALLENGE)));
		 */
	}
	assert(tserv->clients->n_used == nClients);  /* sanity check */

	/* tserv client info */
	err = dpio_thawHdl(tserv->dpio, &(tserv->hServer), fp);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_Thaw: error thawing hServer, err:%d\n", err));
		return err;
	}
	/* DPRINT(("tserv_Thaw: hServer:%x\n", tserv->hServer)); */
	if ((fread(&(tserv->userinfo.uid), sizeof(dp_uid_t), 1, fp) != 1) ||
		(fread(&(tserv->userinfo.uname), sizeof(tcapw_uname_t), 1, fp) != 1) ||
		(fread(&(tserv->userinfo.hpw), sizeof(tcapw_hpw_t), 1, fp) != 1) ||
		(fread(tserv->userinfo.email, tcapw_MAXLEN_EMAIL, 1, fp) != 1) ||
		(fread(&(tserv->userinfo.flags), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->userinfo.secretcode), sizeof(short), 1, fp) != 1) ||
		(fread(&(tserv->userinfo.created), sizeof(time_t), 1, fp) != 1) ||
		(fread(&(tserv->userinfo.lastlogin), sizeof(time_t), 1, fp) != 1) ||
		(fread(&(tserv->secretcode), sizeof(tcapw_pw_t), 1, fp) != 1) ||
		(fread(&(tserv->challenge_set), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->challenge), sizeof(tca_challenge_t), 1, fp) != 1) ||
		(fread(&(tserv->logged_in), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->activated), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->please_send_response), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->please_send_newuser), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->please_send_secretcode), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->please_send_pwchange), sizeof(int), 1, fp) != 1) ||
		(fread(&(tserv->waiting_for_rx), sizeof(int), 1, fp) != 1))
		return dp_RES_EMPTY;
	/* DPRINT(("tserv_Thaw: uid:%d email:%s flags:%d\n", tserv->userinfo.uid, tserv->userinfo.email, tserv->userinfo.flags));
	 * DPRINT(("tserv_Thaw: challenge_set:%d logged_in:%d activated:%d waiting_for_rx:%d\n", tserv->challenge_set, tserv->logged_in, tserv->activated, tserv->waiting_for_rx));
	 * DPRINT(("tserv_Thaw: challenge (set:%d) %s\n", tserv->challenge_set, hexstring(tserv->challenge.challenge, tca_LEN_CHALLENGE)));
	 * DPRINT(("tserv_Thaw: please_send_response:%d newuser:%d secretcode:%d pwchage:%d\n", tserv->please_send_response, tserv->please_send_newuser, tserv->please_send_secretcode, tserv->please_send_pwchange));
	 */

	err = tca_Thaw(tserv->tca, fp);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_Thaw: tca_Thaw returns err:%d\n", err));
		return err;
	}
	/* other tserv related setup */
	now = eclock();
	tserv->tnextPoll = now;
	tserv->rx_deadline = now + 30 * ECLOCKS_PER_SEC;

	tserv_assertValid(tserv);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Return the player handle of the player with the given uid, or PLAYER_NONE
 if uid is not logged in.
 Note: Clients do not currently have anyone logged in, except PLAYER_ME.
--------------------------------------------------------------------------*/
playerHdl_t tserv_uid2hdl(tserv_t *tserv, tcapw_uid_t uid)
{
	int i;
	assoctab_item_t *pi;
	tserv_clientinfo_t *pci;

	ASSERTMEM();
	if (tserv == NULL || uid == tcapw_UID_NONE)
		return PLAYER_NONE;
	if (uid == tserv->userinfo.uid)
		return PLAYER_ME;
	if (tserv->clients == NULL)
		return PLAYER_NONE;
	for (i = 0; i < tserv->clients->n_used; i++) {
		pi = (assoctab_item_t *)assoctab_getkey(tserv->clients, i);
		pci = (tserv_clientinfo_t *)pi->value;
		if (pci->uid == uid) {
			ASSERTMEM();
			return (playerHdl_t)pi->key;
		}
	}
	ASSERTMEM();
	return PLAYER_NONE;
}

/*--------------------------------------------------------------------------
 Return the uid of the player with the given handle, or tcapw_UID_NONE
 if the handle is not logged in.
 Note: Clients do not currently have anyone logged in, except themselves.
--------------------------------------------------------------------------*/
tcapw_uid_t tserv_hdl2uid(tserv_t *tserv, playerHdl_t h)
{
	tserv_clientinfo_t *pci;

	ASSERTMEM();
	if (tserv == NULL || h == PLAYER_NONE)
		return tcapw_UID_NONE;
	if (h == PLAYER_ME)
		return tserv->userinfo.uid;
	if (tserv->clients == NULL)
		return tcapw_UID_NONE;
	pci = (tserv_clientinfo_t *)assoctab_subscript(tserv->clients, h);
	ASSERTMEM();
	if (pci)
		return pci->uid;
	return tcapw_UID_NONE;
}

/*--------------------------------------------------------------------------
 Server only.
 Gets a user's info from the database into the space pointed to by *entry.
 Username is returned in network byte order.
 Does not return user's password.
 Returns dp_RES_OK on success,
         dp_RES_EMPTY if uid is not in the database or is tcapw_UID_NONE,
         dp_RES_BAD on bad arguments.
--------------------------------------------------------------------------*/
dp_result_t tserv_uid2info(tserv_t *tserv, tcapw_uid_t uid, tcapw_entry_t *entry)
{
	dp_result_t err;
	tcapw_entry_t e;

	precondition(tserv);
	precondition(tserv->tca);
	precondition(entry);

	ASSERTMEM();
	if (uid == tcapw_UID_NONE)
		return dp_RES_EMPTY;
	err = tcapw_entry_find_byid(tserv->tca->tdb, uid, &e);
	ASSERTMEM();
	if (err != dp_RES_OK)
		return err;

	mywcs_ncpy0(entry->uname.uname, e.uname.uname, tcapw_LEN_USERNAME);
	strncpy(entry->email, e.email, tcapw_MAXLEN_EMAIL);
	entry->uid = e.uid;
	entry->flags = e.flags;
	entry->created = e.created;
	entry->lastlogin = e.lastlogin;
	entry->secretcode = e.secretcode;
	ASSERTMEM();

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Returns dp_RES_OK if the email address passed to it looks like an email
 address (i.e. something@something.something), dp_RES_BAD if not.
--------------------------------------------------------------------------*/
static dp_result_t tserv_check_email(const char *email)
{
	int i, i_at = -1, i_dot = -1;

	/* some basic checks: */
	for (i = 0; email[i] && i < tcapw_MAXLEN_EMAIL; i++) {
		/* strictly 7-bit ASCII */
		if ((unsigned)email[i] > 0x7f) return dp_RES_BAD;
		/* no more than one '@' */
		if (email[i] == '@' && (i_at != -1 || i == 0)) return dp_RES_BAD;
		if (email[i] == '@') i_at = i;
		if (email[i] == '.') i_dot = i;
	}
	/* at least one '@' and one '.' after the '@' */
	if (i_at == -1 || i_dot < i_at) return dp_RES_BAD;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Server only.
 Call to send (or resend) a challenge to a client.
--------------------------------------------------------------------------*/
static dp_result_t tserv_send_challenge(tserv_t *tserv, playerHdl_t h, tserv_clientinfo_t *pci)
{
	tserv_wrappedPacket_t pkt;
	dp_result_t err;

	precondition(tserv);
	precondition(tserv->dpio);
	precondition(h != PLAYER_NONE);
	precondition(pci);

	/* Generate new challenge regardless; it doesn't matter if we use
	 * the old one.
	 */
	tca_challenge_generate(tserv->tca, &pci->challenge);
	pci->need_send = 0;

	/* Send the client a challenge */
	pkt.type = dp_TSERV_PACKET_ID;
	pkt.u.tserv.kind = tserv_KIND_CHALLENGE;
	memcpy(&pkt.u.tserv.u.challenge, &pci->challenge, sizeof(pci->challenge));

	err = dpio_put_reliable(tserv->dpio, &h, 1, &pkt,
		  sizeof(pkt.type)+sizeof(pkt.u.tserv.kind)+sizeof(pkt.u.tserv.u.challenge), NULL);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_send_challenge: Can't send, queuing.\n"));
		pci->need_send = tserv_KIND_CHALLENGE;
	} else {
		DPRINT(("tserv_send_challenge: Sent challenge to h:%x.\n", h));
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Client, internal use only.
 Sends a response of the requested type to the challenge, if we have it yet.
 Returns dp_RES_OK on successful send,
         dp_RES_AGAIN if there was no challenge or no pending request,
		 dp_RES_BAD on bad args or if incompatible requests are pending,
		 dp_RES_BUSY if the packet could not be queued, etc.
--------------------------------------------------------------------------*/
static dp_result_t tserv_send_response(tserv_t *tserv)
{
	dp_result_t err;
	tserv_wrappedPacket_t pkt;
	int len;

	precondition(tserv);
	precondition(tserv->dpio);
	precondition(!(tserv->please_send_response && tserv->please_send_newuser));

	ASSERTMEM();
	if (tserv->hServer == PLAYER_NONE) {
		DPRINT(("tserv_send_response: not connected yet, waiting.\n"));
		return dp_RES_AGAIN;
	}
	if (!tserv->challenge_set) {
		DPRINT(("tserv_send_response: no challenge received yet, waiting.\n"));
		return dp_RES_AGAIN;
	}
	if (tserv->please_send_response) {
		if (tserv->logged_in) {
			DPRINT(("tserv_send_response: already logged in, can't send response\n"));
			tserv->please_send_response = FALSE;
			return dp_RES_ALREADY;
		}
		DPRINT(("tserv_send_response: sending response\n"));
		err = tca_response_generate(tserv->tca, &tserv->challenge, &tserv->userinfo.uname, &tserv->userinfo.hpw, &pkt.u.tserv.u.response, &len);
		pkt.u.tserv.kind = tserv_KIND_RESPONSE;
	} else if (tserv->please_send_newuser) {
		if (tserv->logged_in) {
			DPRINT(("tserv_send_response: already logged in, can't send newuser\n"));
			tserv->please_send_newuser = FALSE;
			return dp_RES_ALREADY;
		}
		DPRINT(("tserv_send_response: sending newuser\n"));
		err = tca_newuser_generate(tserv->tca, &tserv->challenge, &tserv->userinfo.uname, &tserv->userinfo.hpw, tserv->userinfo.flags, tserv->userinfo.email, &pkt.u.tserv.u.newuser, &len);
		pkt.u.tserv.kind = tserv_KIND_NEWUSER;
	} else if (tserv->please_send_secretcode) {
		tcapw_hpw_t hpw;
		tcapw_uname_t uname;

		if (!tserv->logged_in) {
			DPRINT(("tserv_send_response: not logged in yet, secretcode waiting.\n"));
			return dp_RES_AGAIN;
		}
		if (tserv->activated) {
			DPRINT(("tserv_send_response: already logged in, can't send newuser\n"));
			tserv->please_send_newuser = FALSE;
			return dp_RES_ALREADY;
		}
		DPRINT(("tserv_send_response: sending secretcode %s\n", tcapw_u2ascii(tserv->secretcode.pw, tcapw_LEN_PW)));
		tcapw_password_hash(tserv->tca->tdb, &tserv->secretcode, &hpw);
		memset(&uname, 0, sizeof(uname));  /* No need to send username */
		err = tca_response_generate(tserv->tca, &tserv->challenge, &uname, &hpw, &pkt.u.tserv.u.response, &len);
		pkt.u.tserv.kind = tserv_KIND_SECRETCODE;
	} else {
		/* DPRINT(("tserv_send_response: no response pending\n")); */
		return dp_RES_AGAIN;
	}
	ASSERTMEM();
	pkt.type = dp_TSERV_PACKET_ID;
	DPRINT(("tserv_send_response: sending response kind %c\n", pkt.u.tserv.kind));

	/* And send it to the server. */
	err = dpio_put_reliable(tserv->dpio, &tserv->hServer, 1, &pkt,
			sizeof(pkt.type)+sizeof(pkt.u.tserv.kind)+len, NULL);
	ASSERTMEM();
	if (err == dp_RES_OK) {
		tserv->waiting_for_rx = TRUE;
		tserv->rx_deadline = eclock() + 30*ECLOCKS_PER_SEC;
		switch (pkt.u.tserv.kind) {
		case tserv_KIND_RESPONSE:
			tserv->please_send_response = FALSE;
			DPRINT(("tserv_send_response: Sent response to server h:%x, name %s.\n", tserv->hServer, tcapw_u2ascii(pkt.u.tserv.u.response.uname, pkt.u.tserv.u.response.unamelen)));
			break;
		case tserv_KIND_NEWUSER:
			tserv->please_send_newuser = FALSE;
			DPRINT(("tserv_send_response: Sent response to server h:%x, name %s.\n", tserv->hServer, tcapw_u2ascii((short *)pkt.u.tserv.u.newuser.storage, pkt.u.tserv.u.newuser.unamelen)));
			break;
		case tserv_KIND_SECRETCODE:
			tserv->please_send_secretcode = FALSE;
			DPRINT(("tserv_send_response: Sent response to server h:%x, name %s.\n", tserv->hServer, tcapw_u2ascii(pkt.u.tserv.u.response.uname, pkt.u.tserv.u.response.unamelen)));
			break;
		}
	} else {
		DPRINT(("tserv_send_response: Could not send %c packet to server, leaving in queue\n", pkt.u.tserv.kind));
	}
    return err;
}

/*--------------------------------------------------------------------------
 Client, internal use only.
 Sends a pwchange using the challenge from login.
 Returns dp_RES_OK on successful send,
         dp_RES_AGAIN if there was no pending request,
		 dp_RES_BAD on bad args,
		 dp_RES_BUSY if the packet could not be queued, etc.
--------------------------------------------------------------------------*/
static dp_result_t tserv_send_pwchange(tserv_t *tserv)
{
	dp_result_t err;
	tserv_wrappedPacket_t pkt;
	int len;

	precondition(tserv);
	if (!tserv->please_send_pwchange)
		return dp_RES_AGAIN;
	precondition(tserv->dpio);
	precondition(tserv->tca);
	if (!tserv->logged_in) {
		DPRINT(("tserv_send_pwchange: I am not logged in!\n"));
		return dp_RES_BUG;
	}

	if ((err = tca_pwchange_generate(tserv->tca, &tserv->challenge, &tserv->userinfo.hpw, &tserv->newhpw, tserv->userinfo.flags, tserv->userinfo.email, &pkt.u.tserv.u.pwchange, &len)) != dp_RES_OK) {
		DPRINT(("tserv_sent_pwchange: pwchange_generate err:%d\n", err));
		tserv->please_send_pwchange = FALSE;
		return err;
	}

	pkt.type = dp_TSERV_PACKET_ID;
	pkt.u.tserv.kind = tserv_KIND_PWCHANGE;

	/* send it to the server. */
	err = dpio_put_reliable(tserv->dpio, &tserv->hServer, 1, &pkt,
			sizeof(pkt.type)+sizeof(pkt.u.tserv.kind)+len, NULL);
	if (err == dp_RES_OK) {
		tserv->waiting_for_rx = TRUE;
		tserv->rx_deadline = eclock() + 30*ECLOCKS_PER_SEC;
		DPRINT(("tserv_change_password: Sent request to server h:%x\n", tserv->hServer));
		tserv->please_send_pwchange = FALSE;
	} else {
		DPRINT(("tserv_change_password: Could not send, queuing.\n"));
	}
	return err;
}

/*--------------------------------------------------------------------------
 Server, internal use only.
 Call to send (or resend) a validation reply to a client.
--------------------------------------------------------------------------*/
static dp_result_t tserv_send_validate(tserv_t *tserv, playerHdl_t h, tserv_clientinfo_t *pci)
{
	tserv_wrappedPacket_t pkt;
	dp_result_t err;

	precondition(tserv);
	precondition(h != PLAYER_NONE);
	precondition(pci);

	pci->need_send = 0;

	pkt.type = dp_TSERV_PACKET_ID;

	pkt.u.tserv.kind = tserv_KIND_VALIDATE;
	pkt.u.tserv.u.validate.uid = pci->uid;
	pkt.u.tserv.u.validate.reason = pci->reason;

	err = dpio_put_reliable(tserv->dpio, &h, 1, &pkt,
			sizeof(pkt.type)+sizeof(pkt.u.tserv.kind)+sizeof(pkt.u.tserv.u.validate), NULL);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_send_validate: Can't send, queuing.\n"));
		pci->need_send = tserv_KIND_VALIDATE;
	} else {
		DPRINT(("tserv_send_validate: Sent validate (uid:%d, reason:%d) to h:%x.\n", pci->uid, pci->reason, h));
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Server, internal use only.
 Call to send (or resend) a userinfo change validation reply to a client.
--------------------------------------------------------------------------*/
static dp_result_t tserv_send_pwvalidate(tserv_t *tserv, playerHdl_t h, tserv_clientinfo_t *pci)
{
	tserv_wrappedPacket_t pkt;
	dp_result_t err;

	precondition(tserv);
	precondition(h != PLAYER_NONE);
	precondition(pci);

	pci->need_send = 0;

	pkt.type = dp_TSERV_PACKET_ID;

	pkt.u.tserv.kind = tserv_KIND_PWVALIDATE;
	pkt.u.tserv.u.pwvalidate.reason = pci->reason;

	err = dpio_put_reliable(tserv->dpio, &h, 1, &pkt,
			sizeof(pkt.type)+sizeof(pkt.u.tserv.kind)+sizeof(pkt.u.tserv.u.pwvalidate), NULL);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_send_pwvalidate: Can't send, queuing.\n"));
		pci->need_send = tserv_KIND_PWVALIDATE;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Server, internal use within tserv_handle_packet only.
 Returns the client table entry for a given handle.  Notifies the client
 and server if the there is no client table entry for the handle.
 Assumes arguments are ok, since handle_packet does those checks.
--------------------------------------------------------------------------*/
static tserv_clientinfo_t *tserv_hdl2pci(tserv_t *tserv, playerHdl_t h, tserv_event_t *result)
{
	tserv_clientinfo_t *pci;

	pci = (tserv_clientinfo_t *)assoctab_subscript(tserv->clients, h);
	if (!pci) {
		tserv_clientinfo_t temppci;

		temppci.uid = tcapw_UID_NONE;
		temppci.reason = dp_RES_BUG;
		tserv_send_validate(tserv, h, &temppci);

		result->h = h;
		result->uid = tcapw_UID_NONE;
		result->reason = dp_RES_EMPTY;  /* caller should close handle */
	}
	return pci;
}

/*--------------------------------------------------------------------------
 Server only.
 Call when a new client handle is created.
--------------------------------------------------------------------------*/
dp_result_t tserv_client_add(tserv_t *tserv, playerHdl_t h)
{
	tserv_clientinfo_t *pci;

	precondition(tserv);
	precondition(tserv->clients);
	precondition(h != PLAYER_NONE);

	/* Allocate new record for client, fill it, send it. */
	pci = (tserv_clientinfo_t *)assoctab_subscript_grow(tserv->clients, h);
	pci->uid = tcapw_UID_NONE;
	pci->logged_in = FALSE;
	pci->activated = FALSE;
	pci->need_send = 0;
	pci->challenge_uses = 0;
	pci->reason = dp_RES_OK;

	return tserv_send_challenge(tserv, h, pci);
}

/*--------------------------------------------------------------------------
 Server only.
 Call when a client handle is destroyed.
--------------------------------------------------------------------------*/
dp_result_t tserv_client_delete(tserv_t *tserv, playerHdl_t h)
{
	precondition(tserv);
	precondition(h != PLAYER_NONE);

	assoctab_subscript_delete(tserv->clients, h);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Used in both client and server.
 Perform periodic housekeeping.
 Call once every second or so.
 Returns dp_RES_OK normally,
         dp_RES_HOST_NOT_RESPONDING for the client if the server hasn't
		         replied to the last client tserv packet within 30s.
 		 dp_RES_NOTYET if tserv has not been initialized,
		 dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tserv_poll(tserv_t *tserv)
{
	clock_t now = eclock();

	if (!tserv)
		return dp_RES_NOTYET;
	if (tserv->clients == NULL)
		return dp_RES_BAD;
	tserv_assertValid(tserv);
	if ((long)(now - tserv->tnextPoll) > 0) {
		/* Schedule next run. */
		tserv->tnextPoll = now + ECLOCKS_PER_SEC;

		ASSERTMEM();
		if (tserv->hServer != PLAYER_NONE) {
			/* Client - send/resend challenge responses to server if needed */
			(void) tserv_send_response(tserv);
			(void) tserv_send_pwchange(tserv);

			/* Time out the server if we haven't received a validate for
			 * a response we sent in 30 seconds
			 */
			if (tserv->waiting_for_rx && (long)(now - tserv->rx_deadline) > 0)
				return dp_RES_HOST_NOT_RESPONDING;
		} else {
			int i;

			/* Server - resend challenges and replies to clients if needed */
			for (i = tserv->clients->n_used - 1; i >= 0; i--) {
				assoctab_item_t *pe;
				tserv_clientinfo_t *pci;
				playerHdl_t h;

				pe = assoctab_getkey(tserv->clients, i);
				if (!pe) {
					assert(FALSE);
					return dp_RES_BUG;
				}
				pci = (tserv_clientinfo_t *) &pe->value;
				h = (playerHdl_t) pe->key;
				if (pci->need_send == tserv_KIND_CHALLENGE)
					tserv_send_challenge(tserv, h, pci);
				else if (pci->need_send == tserv_KIND_VALIDATE)
					tserv_send_validate(tserv, h, pci);
				else if (pci->need_send == tserv_KIND_PWVALIDATE)
					tserv_send_pwvalidate(tserv, h, pci);
			}
		}
		ASSERTMEM();
	}
	tserv_assertValid(tserv);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Server only.  Internal use only.
 Send credentials for hFrom to hTo.
 This routine may get more complicated if we make the two hosts prove their
 uid to each other by sending a mutual secret to them.
 Returns dp_RES_OK on success.
 Returns dp_RES_NOTYET if hFrom has not logged in.
--------------------------------------------------------------------------*/
static dp_result_t tserv_send_credentials1(tserv_t *tserv, playerHdl_t hFrom, playerHdl_t hTo)
{
#include "dppack1.h"
	struct {
		dp_packetType_t	tag;
		tserv_packet_t  body;
	} PACK pkt;
#include "dpunpack.h"

	int len;
	dp_uid_t uid;
	dp_result_t err;

	ASSERTMEM();
	/* For now, don't need to send 'em to myself */
	if (hTo == PLAYER_ME)
		return dp_RES_OK;

	pkt.tag = dp_TSERV_PACKET_ID;
	pkt.body.kind = tserv_KIND_CREDENTIALS;

	/* Get both known addresses of the handle */
	len = dp_MAX_ADR_LEN;
	err = dpio_hdl2adr2(tserv->dpio, hFrom, pkt.body.u.credentials.adr, pkt.body.u.credentials.adr2, &len);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("tserv_send_credentials: bug: can't get adr for h:%x\n", hFrom));
		return err;
	}
	/* Look up the handle's uid */
	uid = tserv_hdl2uid(tserv, hFrom);
	ASSERTMEM();
	if (uid == dp_UID_NONE) {
		DPRINT(("tserv_send_credentials: bug: can't get uid for h:%x\n", hFrom));
		return dp_RES_NOTYET;
	}
	/* Compose a packet and send it to the destination handle */
	pkt.body.u.credentials.uid = SwapBytes4(uid);
	err = dpio_put_reliable(tserv->dpio, &hTo, 1, &pkt, sizeof (pkt), NULL);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("tserv_send_credentials: bug: could not send credentials to host h:%x\n", hTo));
		/* Note: this is LIKELY TO HAPPEN because only 32 packets fit in
		 * the outgoing queue for each host at the moment.
		 * Fix will probably be to reimplement dpio_put_reliable to
		 * remove this limit.  Another fix would be to implement a way
		 * to queue this request at this level for a later dpio_put_reliable.
		 */
		return err;
	}

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Server only.
 Call to tell two clients to shake hands and be friends (and prove to
 each other what their uid's are).
 Returns dp_RES_OK on success.
 		 dp_RES_NOTYET if tserv has not been initialized.
--------------------------------------------------------------------------*/
dp_result_t tserv_send_credentials(tserv_t *tserv, playerHdl_t h1, playerHdl_t h2)
{
	dp_result_t err;

	if (!tserv)
		return dp_RES_NOTYET;

	DPRINT(("tserv_send_credentials: h1:%x h2:%x\n", h1, h2));
	ASSERTMEM();
	err = tserv_send_credentials1(tserv, h1, h2);
	if (err != dp_RES_OK) return err;
	return tserv_send_credentials1(tserv, h2, h1);
}

/*--------------------------------------------------------------------------
 Client only.  Internal use only.
 Handle a credentials packet from the server.
 Open a handle to the specified address, record its uid,
 and fill in the result record with an event to hand the caller of
 tserv_handle_packet().

 Returns dp_RES_OK and an event in *result upon success.
 Else *event is not changed.

 These are requests from a game server to open a connection to
 a designated address.  We must insure that this packet has come from a game
 server and then we open a handle to the player in the normal way.
 This routine should not be called if the packet was not sent reliably.
--------------------------------------------------------------------------*/

static dp_result_t tserv_handle_credentials1(tserv_t *tserv, playerHdl_t src, tserv_packet_credentials_t *cred, tserv_event_t *result)
{
	playerHdl_t h;
	dp_uid_t uid;
	tserv_clientinfo_t *pci;

	uid = SwapBytes4(cred->uid);
	DPRINT(("tserv_handle_credentials1: Got credentials packet for uid:%d from h:%x\n", uid, src));
	ASSERTMEM();

	if ((src != tserv->hServer) || (tserv->hServer == PLAYER_NONE)) {
		DPRINT(("tserv_handle_credentials1: credentials received from someone who is NOT our server!"));
		return dp_RES_ACCESS;
	}

	/* If addresses are the same, only pass one in to openHdl */
	if (memcmp(cred->adr, cred->adr2, tserv->dpio->myAdrLen))
		h = dpio_openHdl2(tserv->dpio, cred->adr, cred->adr2);
	else
		h = dpio_openHdl2(tserv->dpio, cred->adr, NULL);
	ASSERTMEM();

	DPRINT(("tserv_handle_credentials1: Opening Connection to uid:%d ", uid));
	/*printAdr(tserv->dpio->myAdrLen, cred->adr);
	DPRINT((", "));
	printAdr(tserv->dpio->myAdrLen, cred->adr2);*/
	DPRINT((" -> h:%x\n", h));

	if (h == PLAYER_NONE) {
		DPRINT(("tserv_handle_credentials1: can't open handle!"));
		return dp_RES_BUG;
	}

	/* The Moment of Trust */
	/* Record this handle's uid. */
	pci = (tserv_clientinfo_t *)assoctab_subscript_grow(tserv->clients, h);
	ASSERTMEM();

	if (!pci) {
		DPRINT(("tserv_handle_credentials1: Can't grow client table\n"));
		return dp_RES_NOMEM;
	}
	pci->activated = TRUE;
	pci->need_send = 0;
	pci->challenge_uses = 0;
	pci->reason = dp_RES_OK;
	pci->uid = uid;
	pci->logged_in = TRUE;

	/* Inform the caller of the new trust. */
	result->h = h;
	result->uid = uid;
	result->reason = dp_RES_OK;
	ASSERTMEM();

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Used in both client and server.
 Call when a message with header dp_TSERV_PACKET_ID is received.
 pkt_flags must be the result flags from dpio_get.

 Returns dp_RES_EMPTY normally.
 Returns dp_RES_OK and places info in *result if something interesting
     happened as a result of the packet (or as a result of time passing).
 Returns dp_RES_NOTYET if tserv has not been initialized.

 Possible values of return->reason for the server on validation are:
     dp_RES_OK      player h/uid is now logged in and activated.
	 dp_RES_ACCESS  player h/uid's challenge is overused, please close handle
	 dp_RES_EMPTY   packet rec'd from handle h not in client table,
	                please close handle with dpio_closeHdl(result->h);
     dp_RES_CREATED	player created and logged in with given h/uid,
	                please send a secret code email.
	 dp_RES_CHANGED player h/uid changed his password, email, or flags.
	 dp_RES_HELP    please send another secret code email.
 Possible values of return->reason for the client on validation are:
     dp_RES_OK      login was successful and account is activated.
	 dp_RES_NOTYET  login successful, but account not yet activated.
	 dp_RES_ACCESS  login unsuccessful, incorrect challenge response
	 dp_RES_ALREADY login unsuccessful, username is in use or logged in
	 dp_RES_CHANGED password, email, and flags changed successfully
	 dp_RES_BUG     server error, try again
--------------------------------------------------------------------------*/
dp_result_t tserv_handle_packet(tserv_t *tserv, playerHdl_t src, int pkt_flags, tserv_packet_t *pkt, tserv_event_t *result)
{
	dp_result_t err;
	tcapw_uid_t uid;
	tserv_clientinfo_t *pci;
	playerHdl_t phdl;

	if (!tserv)
		return dp_RES_NOTYET;
	precondition(pkt);
	precondition(result);
	precondition(src != PLAYER_NONE);

	ASSERTMEM();
	DPRINT(("tserv_handle_packet: processing packet kind:%c from h:%x\n", pkt->kind, src));
	if (!(pkt_flags & dpio_GET_RELIABLE)) {
		DPRINT(("tserv_handle_packet: received unexpected unreliable packet\n"));
		return dp_RES_EMPTY;
	}
	switch (pkt->kind) {
	case tserv_KIND_CHALLENGE:
		if (tserv->hServer == PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c from h:%x, but I am not a client\n", pkt->kind, src));
			return dp_RES_BUG;
		}
		/* got the challenge we were waiting for, don't time out the server */
		tserv->waiting_for_rx = FALSE;

		/* Save the challenge */
		memcpy(&tserv->challenge, &pkt->u.challenge, sizeof(tserv->challenge));
		tserv->challenge_set = TRUE;

		/* And respond to it, if we can. */
		(void) tserv_send_response(tserv);
		break;

	case tserv_KIND_RESPONSE:
		if (tserv->hServer != PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c, but I am not a server\n", pkt->kind));
			return dp_RES_BUG;
		} else if ((pci = tserv_hdl2pci(tserv, src, result)) == NULL) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is unknown\n", pkt->kind, src));
			return dp_RES_OK;
		} else if (pci->need_send != 0) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x had need_send=%c\n", pkt->kind, src, pci->need_send));
		} else if (pci->logged_in) {
			assert(pci->uid != tcapw_UID_NONE);
			DPRINT(("tserv_handle_packet: got %c, but h:%x is logged in\n", pkt->kind, src));
		} else {
			uid = tcapw_UID_NONE;
			err = tca_response_validate(tserv->tca, &pci->challenge, &pkt->u.response, &uid);
			pci->challenge_uses++;
			if ((phdl = tserv_uid2hdl(tserv, uid)) != PLAYER_NONE) {
				assert(uid != tcapw_UID_NONE);
				DPRINT(("tserv_handle_packet: login failed, uid:%d is logged in already at h:%x\n", uid, phdl));
				/* This uid is already logged in, tell the client. */
				pci->uid = tcapw_UID_NONE;
				pci->reason = dp_RES_ALREADY;
				tserv_send_validate(tserv, src, pci);
			} else if (err == dp_RES_OK) {
				assert(uid != tcapw_UID_NONE);
				/* User's password validated, user logged in */
				pci->uid = uid;        /* put our uid in the clients table */
				pci->logged_in = TRUE;

				/* pull email_valid flag out of the db, if we need to */
				if (!pci->activated) {
					tcapw_entry_t entry;

					DPRINT(("tserv_handle_packet: looking up activated status of uid:%d\n", uid));
					err = tserv_uid2info(tserv, uid, &entry);
					assert(!err);
					DPRINT(("tserv_handle_packet: read flags:%d for uid:%d\n", entry.flags, uid));
					pci->activated = entry.flags & tcapw_entry_FLAGS_EMAIL_VALID;
				}
				if (pci->activated) {
					DPRINT(("tserv_handle_packet: uid:%d logged in.\n", uid));

					/* Tell the client that the login succeeded */
					pci->reason = dp_RES_OK;
					tserv_send_validate(tserv, src, pci);

					/* Tell the server that the login succeeded */
					result->h = src;
					result->uid = uid;
					result->reason = dp_RES_OK;	/* should be dp_RES_LOGIN? */
					ASSERTMEM();

					return dp_RES_OK;
				} else {
					DPRINT(("tserv_handle_packet: uid:%d logged in, not activated.\n", uid));
					/* Client's account hasn't been activated, tell them */
					pci->reason = dp_RES_NOTYET;
					tserv_send_validate(tserv, src, pci);
				}
			} else {
				assert(uid == tcapw_UID_NONE);
				DPRINT(("tserv_handle_packet: access denied, premap err:%d\n", err));
				/* Tell the client that the login failed */
				pci->uid = tcapw_UID_NONE;
				if (err == dp_RES_ACCESS || dp_RES_EMPTY)
					pci->reason = dp_RES_ACCESS;
				else  /* failed for some buggy reason */
					pci->reason = dp_RES_BUG;
				tserv_send_validate(tserv, src, pci);

				/* drop client, tell server if challenge has been overused */
				if (pci->challenge_uses > tserv_MAX_CHALLENGE_USES) {
					tserv_client_delete(tserv, src);
					result->h = src;
					result->uid = uid;
					result->reason = dp_RES_ACCESS;
					ASSERTMEM();

					return dp_RES_OK;
				}
			}
		}
		break;

	case tserv_KIND_NEWUSER:
		if (tserv->hServer != PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c, but I am not a server\n", pkt->kind));
			return dp_RES_BUG;
		} else if ((pci = tserv_hdl2pci(tserv, src, result)) == NULL) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is unknown\n", pkt->kind, src));
			return dp_RES_OK;
		} else if (pci->need_send != 0) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x had need_send=%c\n", pkt->kind, src, pci->need_send));
		} else if (pci->logged_in) {
			assert(pci->uid != tcapw_UID_NONE);
			DPRINT(("tserv_handle_packet: got %c, but h:%x is logged in\n", pkt->kind, src));
		} else {
			uid = tcapw_UID_NONE;
			err = tca_newuser_validate(tserv->tca, &pci->challenge, &pkt->u.newuser, &uid);
			pci->uid = uid;
			pci->activated = FALSE;
			pci->challenge_uses++;
			if (err == dp_RES_OK) {
				/* User created and logged in! */
				assert(uid != tcapw_UID_NONE);
				pci->logged_in = TRUE;
				DPRINT(("tserv_handle_packet: uid:%d created and logged in.\n", uid));
				/* Tell the client that the login succeeded, but they'll
				 * need to activate it.
				 */
				pci->reason = dp_RES_NOTYET;
				tserv_send_validate(tserv, src, pci);

				/* Tell the server that the login succeeded. */
				/* The server should send a secret code email to this user */
				result->h = src;
				result->uid = uid;
				result->reason = dp_RES_CREATED;
				ASSERTMEM();

				return dp_RES_OK;
			} else {
				assert(uid == tcapw_UID_NONE);
				DPRINT(("tserv_handle_packet: newuser denied, premap err:%d\n", err));
				/* Tell the client that the login failed */
				if (err == dp_RES_ACCESS)
					pci->reason = dp_RES_ACCESS;
				else if (err == dp_RES_ALREADY)
					pci->reason = dp_RES_ALREADY;
				else  /* failed for some buggy reason */
					pci->reason = dp_RES_BUG;
				tserv_send_validate(tserv, src, pci);

				/* drop client, tell server if challenge has been overused */
				if (pci->challenge_uses > tserv_MAX_CHALLENGE_USES) {
					tserv_client_delete(tserv, src);
					result->h = src;
					result->uid = uid;
					result->reason = dp_RES_ACCESS;
					ASSERTMEM();

					return dp_RES_OK;
				}
			}
		}
		break;

	case tserv_KIND_PWCHANGE:
		if (tserv->hServer != PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c, but I am not a server\n", pkt->kind));
			return dp_RES_BUG;
		} else if ((pci = tserv_hdl2pci(tserv, src, result)) == NULL) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is unknown\n", pkt->kind, src));
			return dp_RES_OK;
		} else if (pci->need_send != 0) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x had need_send=%c\n", pkt->kind, src, pci->need_send));
		} else if (!pci->logged_in) {
			assert(pci->uid == tcapw_UID_NONE);
			DPRINT(("tserv_handle_packet: got %c, but h:%x is not logged in\n", pkt->kind, src));
		} else {
			uid = pci->uid;
			err = tca_pwchange_validate(tserv->tca, uid, &pci->challenge, &pkt->u.pwchange);
			pci->challenge_uses++;
			if (err == dp_RES_OK) {
				/* User's info changed */
				DPRINT(("tserv_handle_packet: uid:%d info changed.\n", uid));
				/* Tell the client that the pwchange succeeded.
				 * If their email has still not been validated, tell them.
				 */
				pci->reason = dp_RES_OK;
				if (!pci->activated)
					pci->reason = dp_RES_NOTYET;
				tserv_send_pwvalidate(tserv, src, pci);

				/* notify the server of the change */
				result->h = src;
				result->uid = uid;
				result->reason = dp_RES_CHANGED;
				ASSERTMEM();

				return dp_RES_OK;
			} else {
				/* Tell the client that the pwchange failed */
				DPRINT(("tserv_handle_packet: pwchange denied, premap err:%d\n", err));
				if (err == dp_RES_ACCESS || err == dp_RES_EMPTY)
					pci->reason = dp_RES_ACCESS;
				else  /* failed for some buggy reason */
					pci->reason = dp_RES_BUG;
				tserv_send_pwvalidate(tserv, src, pci);

				/* drop client, tell server if challenge has been overused */
				if (pci->challenge_uses > tserv_MAX_CHALLENGE_USES) {
					tserv_client_delete(tserv, src);
					result->h = src;
					result->uid = uid;
					result->reason = dp_RES_ACCESS;
					ASSERTMEM();

					return dp_RES_OK;
				}
			}
		}
		break;

	case tserv_KIND_VALIDATE:
		if (tserv->hServer == PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c from h:%x, but I am not a client\n", pkt->kind, src));
			return dp_RES_BUG;
		} else if (tserv->activated) {
			DPRINT(("tserv_handle_packet: got %c from h:%x, but I am activated\n", pkt->kind, src));
		} else {
			/* We've been answered, don't time out the server*/
			tserv->waiting_for_rx = FALSE;

			/* We've been answered, notify the caller of the results. */
			result->h = PLAYER_ME;
			result->uid = pkt->u.validate.uid;
			result->reason = pkt->u.validate.reason;
			DPRINT(("tserv_handle_packet: validate recd, premap err:%d\n", pkt->u.validate.reason));

			/* wipe the password we set for the response. */
			memset(&tserv->userinfo.hpw, 0, sizeof(tserv->userinfo.hpw));
			if (pkt->u.validate.reason == dp_RES_OK) {
				assert(pkt->u.validate.uid != tcapw_UID_NONE);
				/* now logged in and activated, record our id */
				tserv->userinfo.uid = pkt->u.validate.uid;
				tserv->logged_in = TRUE;
				tserv->activated = TRUE;
				tserv->userinfo.secretcode = 0;
			} else if (pkt->u.validate.reason == dp_RES_NOTYET) {
				/* Normally expected error results are:
				 * dp_RES_NOTYET = Our username and password are ok,
				 *    but we need to validate our email with a secret code.
				 *    The caller should prompt the user to enter the code or
				 *    request to have one sent with a tserv_KIND_EMAILREQ.
				 * dp_RES_ALREADY = Username already in use
				 * dp_RES_ACCESS = Wrong password
				 */
				tserv->logged_in = TRUE;
				tserv->activated = FALSE;
				/* keep out secret code in this case, wipe it otherwise */
			} else if (pkt->u.validate.reason == dp_RES_ALREADY ||
					   pkt->u.validate.reason == dp_RES_ACCESS) {
				tserv->logged_in = FALSE;
				tserv->activated = FALSE;
				tserv->userinfo.secretcode = 0;
			} else {
				/* We failed to log in for some buggy reason like
				 * dp_RES_NOMEM, dp_RES_BUG, or dp_RES_BAD, so notify
				 * the caller that we should give up on this server.
				 */
				result->reason = dp_RES_BUG;
				tserv->logged_in = FALSE;
				tserv->activated = FALSE;
				tserv->userinfo.secretcode = 0;
			}
			ASSERTMEM();

			return dp_RES_OK;
		}
		break;

	case tserv_KIND_PWVALIDATE:
		if (tserv->hServer == PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c from h:%x, but I am not a client\n", pkt->kind, src));
			return dp_RES_BUG;
		} else if (!tserv->logged_in) {
			DPRINT(("tserv_handle_packet: got %c from h:%x, but I am not logged in\n", pkt->kind, src));
		} else {
			/* We've been answered, don't time out the server*/
			tserv->waiting_for_rx = FALSE;

			/* Notify the caller of the success or failure */
			result->h = PLAYER_ME;
			result->uid = uid;
			DPRINT(("tserv_handle_packet: pwvalidate recd, premap reason:%d\n", pkt->u.pwvalidate.reason));
			/* wipe the passwords we set for the pwchange. */
			memset(&tserv->userinfo.hpw, 0, sizeof(tserv->userinfo.hpw));
			memset(&tserv->newhpw, 0, sizeof(tserv->newhpw));
			if (pkt->u.pwvalidate.reason == dp_RES_OK) {
				/* password changed successfully */
				result->reason = dp_RES_CHANGED;
			} else if (pkt->u.pwvalidate.reason == dp_RES_NOTYET) {
				/* Normally expected error results are:
				 * dp_RES_NOTYET = Our username and password are ok,
				 *    but we need to validate our email with a secret code.
				 *    The caller should prompt the user to enter the code or
				 *    request to have one sent with a tserv_KIND_EMAILREQ.
				 * dp_RES_ACCESS = Wrong password
				 */
				tserv->activated = FALSE;
				result->reason = dp_RES_NOTYET;
				/* keep out secret code in this case, wipe it otherwise */
			} else if (pkt->u.pwvalidate.reason == dp_RES_ACCESS) {
				result->reason = dp_RES_ACCESS;
				tserv->userinfo.secretcode = 0;
			} else {
				/* failed for some buggy reason */
				result->reason = dp_RES_BUG;
				tserv->userinfo.secretcode = 0;
			}
			ASSERTMEM();

			return dp_RES_OK;
		}
		break;

	case tserv_KIND_EMAILREQ:
		/* First, look them up.  They should be logged in but not activated. */
		if (tserv->hServer != PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c, but I am not a server\n", pkt->kind));
			return dp_RES_BUG;
		} else if ((pci = tserv_hdl2pci(tserv, src, result)) == NULL) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is unknown\n", pkt->kind, src));
			return dp_RES_OK;
		} else if (pci->need_send != 0) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x had need_send=%c\n", pkt->kind, src, pci->need_send));
		} else if (!pci->logged_in) {
			assert(pci->uid == tcapw_UID_NONE);
			DPRINT(("tserv_handle_packet: got %c, but h:%x is not logged in\n", pkt->kind, src));
		} else if (pci->activated) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is activated\n", pkt->kind, src));
		} else {
			/* Tell the caller to send them an email containing the secretcode
			 * that was generated when they created their account.
			 */
			result->h = src;
			result->uid = pci->uid;
			result->reason = dp_RES_HELP;
			ASSERTMEM();

			return dp_RES_OK;
		}
		break;

	case tserv_KIND_SECRETCODE:
		/* First, look them up.  They should be logged in but not activated. */
		if (tserv->hServer != PLAYER_NONE) {
			DPRINT(("tserv_handle_packet: got %c, but I am not a server\n", pkt->kind));
			return dp_RES_BUG;
		} else if ((pci = tserv_hdl2pci(tserv, src, result)) == NULL) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is unknown\n", pkt->kind, src));
			return dp_RES_OK;
		} else if (pci->need_send != 0) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x had need_send=%c\n", pkt->kind, src, pci->need_send));
		} else if (!pci->logged_in) {
			assert(pci->uid == tcapw_UID_NONE);
			DPRINT(("tserv_handle_packet: got %c, but h:%x is not logged in\n", pkt->kind, src));
		} else if (pci->activated) {
			DPRINT(("tserv_handle_packet: got %c, but h:%x is activated\n", pkt->kind, src));
		} else {
			/* Check the secret code response against the one in their
			 * permanent record, notify them of the results, and notify
			 * the caller if they finish logging in.
			 */
			uid = pci->uid;
			err = tca_secretcode_validate(tserv->tca, uid, &pci->challenge, &pkt->u.response);
			pci->challenge_uses++;
			if (err == dp_RES_OK) {
				assert(uid != tcapw_UID_NONE);
				/* Secret code validated! */
				DPRINT(("tserv_handle_packet: uid:%d email validated and logged in.\n", uid));
				/* activate the account and copy the activation into the db */
				pci->activated = TRUE;
				{  /* this should be rewritten when we have a better
					* replacement for tcapw_entry_change().
					*/
					tcapw_entry_t entry;
					/* can't use tserv_uid2info, because we need hpw */
					err = tcapw_entry_find_byid(tserv->tca->tdb, uid, &entry);
					assert(!err);
					entry.flags |= tcapw_entry_FLAGS_EMAIL_VALID;
					err = tcapw_entry_change(tserv->tca->tdb, uid, &entry.hpw, entry.flags, entry.email);
					assert(!err);
					DPRINT(("tserv_handle_packet: set flags:%d for uid:%d\n", entry.flags, uid));
				}
				/* Tell the client that the login succeeded */
				pci->reason = dp_RES_OK;
				tserv_send_validate(tserv, src, pci);

				/* Tell the server that the login succeeded */
				result->h = src;
				result->uid = uid;
				result->reason = dp_RES_OK;
				ASSERTMEM();

				return dp_RES_OK;
			} else {
				DPRINT(("tserv_handle_packet: access denied, error %d.\n", err));
				/* Tell the client that the activation failed */
				if (err = dp_RES_ACCESS)
					pci->reason = dp_RES_NOTYET;
				else  /* failed for a buggy reason */
					pci->reason = dp_RES_BUG;
				tserv_send_validate(tserv, src, pci);

				/* drop client, tell server if challenge has been overused */
				if (pci->challenge_uses > tserv_MAX_CHALLENGE_USES) {
					tserv_client_delete(tserv, src);
					result->h = src;
					result->uid = uid;
					result->reason = dp_RES_ACCESS;
					ASSERTMEM();

					return dp_RES_OK;
				}
			}
		}
		break;
	case tserv_KIND_CREDENTIALS:
		err = tserv_handle_credentials1(tserv, src, &(pkt->u.credentials), result);
		ASSERTMEM();

		return err;
    }
	ASSERTMEM();
	/* Nothing in result, return empty */
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Client only.
 Call to initiate a connection to a server.
 Handle to server must be opened immediately before this call
 (i.e. without any intervening calls to dpReceive).
--------------------------------------------------------------------------*/
dp_result_t tserv_server_add(tserv_t *tserv, playerHdl_t hServer)
{
	precondition(tserv);
	precondition(hServer != PLAYER_NONE);
	ASSERTMEM();

	if (tserv->hServer != PLAYER_NONE)
		return dp_RES_ALREADY;

	/* we are now listening for challenge packets from hServer.  */
	tserv->hServer = hServer;
	tserv->challenge_set = FALSE;
	tserv->logged_in = FALSE;
	tserv->activated = FALSE;

	tserv->waiting_for_rx = TRUE;
	tserv->rx_deadline = eclock() + 30*ECLOCKS_PER_SEC;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Client only.
 Calls to schedule responses and inform this layer of the needed userinfo.

 username, password, and email are in native byte-order unicode.
 See tserva.c for ASCII versions of these functions.
 email must look like an email address at this call, and will be validated
 at the account activation stage of login.
 flags is a union of user set tcapw_entry_FLAGS_*.

 Returns dp_RES_OK if the request was successfully queued,
         dp_RES_BUSY if a request is already in progress,
		 dp_RES_BAD on bad args.

 Success or failure is indicated by the return value from a later call
 to tserv_handle_packet().
--------------------------------------------------------------------------*/
dp_result_t tserv_account_loginW(tserv_t *tserv, const wchar_t *username, const wchar_t *password)
{
	dp_result_t err;
	tcapw_pw_t pw;

	precondition(tserv);
	precondition(tserv->tca);
	precondition(tserv->tca->tdb);
	ASSERTMEM();

	if (tserv->logged_in) {
		DPRINT(("tserv_account_login: I am already logged in!\n"));
		return dp_RES_ALREADY;
	}
	if (tserv->please_send_response || tserv->please_send_newuser) {
		DPRINT(("tserv_account_login: already sending a login\n"));
		return dp_RES_BUSY;
	}

	/* convert WCS parameters to network order and hash the password */
	mywcs_wchar2netchar(tserv->userinfo.uname.uname, username, tcapw_LEN_USERNAME);
  	mywcs_wchar2netchar(pw.pw, password, tcapw_LEN_PW);
	err = tcapw_password_hash(tserv->tca->tdb, &pw, &tserv->userinfo.hpw);
	assert(!err);

	/* schedule a response packet */
	tserv->please_send_response = TRUE;
	DPRINT(("tserv_account_login: login %s response pending\n", tcapw_u2ascii(tserv->userinfo.uname.uname, tcapw_LEN_USERNAME)));

	/* tserv_poll would do this for us, but let's speed up the process */
	tserv_send_response(tserv);
	ASSERTMEM();

	return dp_RES_OK;
}

dp_result_t tserv_account_createW(tserv_t *tserv, const wchar_t *username, const wchar_t *password, int flags, const wchar_t *email)
{
	dp_result_t err;
	tcapw_pw_t pw;
	char emailA[tcapw_MAXLEN_EMAIL+1];

	precondition(tserv);
	precondition(tserv->tca);
	precondition(tserv->tca->tdb);
	ASSERTMEM();

	if (tserv->logged_in) {
		DPRINT(("tserv_account_create: I am already logged in!\n"));
		return dp_RES_ALREADY;
	}
	if (tserv->please_send_response || tserv->please_send_newuser) {
		DPRINT(("tserv_account_create: already sending a login\n"));
		return dp_RES_BUSY;
	}

	/* Convert parameters to the internally used (tca level) types */
	mywcs_wchar2netchar(tserv->userinfo.uname.uname, username, tcapw_LEN_USERNAME);
  	mywcs_wchar2netchar(pw.pw, password, tcapw_LEN_PW);
	err = tcapw_password_hash(tserv->tca->tdb, &pw, &tserv->userinfo.hpw);
	assert(!err);
	mywcs_tombs(emailA, tcapw_MAXLEN_EMAIL, email);
	if ((err = tserv_check_email(emailA)) != dp_RES_OK) {
		DPRINT(("tserv_account_create: bad email %s\n", emailA));
		return dp_RES_BAD;
	}
	strncpy(tserv->userinfo.email, emailA, tcapw_MAXLEN_EMAIL);
	tserv->userinfo.flags = flags;

	/* schedule a newuser packet */
	tserv->please_send_newuser = TRUE;
	DPRINT(("tserv_account_create: newuser %s response pending\n", tcapw_u2ascii(tserv->userinfo.uname.uname, tcapw_LEN_USERNAME)));

	/* tserv_poll would do this for us, but let's speed up the process */
	tserv_send_response(tserv);
	ASSERTMEM();

	return dp_RES_OK;
}

dp_result_t tserv_account_activateW(tserv_t *tserv, const wchar_t *secretcode)
{
	precondition(tserv);
	precondition(secretcode);

	if (tserv->activated) {
		DPRINT(("tserv_account_activate: I am already activated!\n"));
		return dp_RES_ALREADY;
	}
	if (tserv->please_send_secretcode) {
		DPRINT(("tserv_account_activate: already sending an activate\n"));
		return dp_RES_BUSY;
	}

	mywcs_wchar2netchar(tserv->secretcode.pw, secretcode, tcapw_LEN_PW);

	/* schedule a secretcode packet */
	tserv->please_send_secretcode = TRUE;
	DPRINT(("tserv_account_activate: secretcode %d response pending\n", tserv->userinfo.secretcode));

	/* tserv_poll would do this for us, but let's speed up the process */
	tserv_send_response(tserv);

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Client only.
  Send a userinfo change request using the challenge we logged in with.
  Client must be logged in and authenticated to call this function.

  username, password, and email are in native byte-order unicode.
  See tserva.c for ASCII versions of these functions.
  flags is a union of user set tcapw_entry_FLAGS_*.

  Returns dp_RES_OK if the request was successfully queued,
          dp_RES_BAD if the client was not logged in or authenticated,
          dp_RES_* if the request can't be queued.

  Success or failure is indicated by the return value from a later call
  to tserv_handle_packet().
--------------------------------------------------------------------------*/
dp_result_t tserv_change_passwordW(tserv_t *tserv, const wchar_t *oldpassword, const wchar_t *newpassword, int flags, const wchar_t *email)
{
	dp_result_t err;
	tcapw_pw_t oldpw;
	tcapw_pw_t newpw;

	precondition(tserv);
	precondition(tserv->tca);
	precondition(tserv->tca->tdb);
	if (!tserv->logged_in) {
		DPRINT(("tserv_change_password: I am not logged in!\n"));
		return dp_RES_AGAIN;
	}
	if (!tserv->challenge_set) {
		DPRINT(("tserv_change_password: No challenge to reply to!\n"));
		return dp_RES_AGAIN;
	}
	if (tserv->please_send_pwchange) {
		DPRINT(("tserv_change_password: pwchange already queued\n"));
		return dp_RES_BUSY;
	}
	assert(!tserv->please_send_response && !tserv->please_send_newuser);

	/* convert to network byte order, hash the password, and store in tserv */
	mywcs_tombs(tserv->userinfo.email, tcapw_MAXLEN_EMAIL, email);
	if ((err = tserv_check_email(tserv->userinfo.email)) != dp_RES_OK) {
		DPRINT(("tserv_change_password: bad email %s\n", tserv->userinfo.email));
		memset(tserv->userinfo.email, 0, tcapw_MAXLEN_EMAIL);
		return dp_RES_BAD;
	}
	mywcs_wchar2netchar(oldpw.pw, oldpassword, tcapw_LEN_PW);
	mywcs_wchar2netchar(newpw.pw, newpassword, tcapw_LEN_PW);
	err = tcapw_password_hash(tserv->tca->tdb, &oldpw, &tserv->userinfo.hpw);
	assert(!err);
	err = tcapw_password_hash(tserv->tca->tdb, &newpw, &tserv->newhpw);
	assert(!err);
	tserv->userinfo.flags = flags;

	/* schedule a pwchange packet */
	tserv->please_send_pwchange = TRUE;
	DPRINT(("tserv_change_password: pwchange response pending\n"));

	/* tserv_poll would do this for us, but let's speed up the process */
	tserv_send_pwchange(tserv);

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Client only.
 Call to request that the server email a secret code to activate account.
--------------------------------------------------------------------------*/
dp_result_t tserv_request_email(tserv_t *tserv)
{
	tserv_wrappedPacket_t pkt;
	dp_result_t err;

	precondition(tserv);
	precondition(tserv->hServer != PLAYER_NONE);
	precondition(tserv->dpio);

	pkt.type = dp_TSERV_PACKET_ID;
	pkt.u.tserv.kind = tserv_KIND_EMAILREQ;

	err = dpio_put_reliable(tserv->dpio, &tserv->hServer, 1, &pkt,
			sizeof(pkt.type)+sizeof(pkt.u.tserv.kind), NULL);
	if (err != dp_RES_OK) {
		DPRINT(("tserv_request_email: Can't send, try again later.\n"));
		return err;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Client only.
 Call to stop trusting the current server.
--------------------------------------------------------------------------*/
dp_result_t tserv_server_delete(tserv_t *tserv)
{
	precondition(tserv);
	ASSERTMEM();

	tserv->hServer = PLAYER_NONE;
	tserv->challenge_set = FALSE;
	tserv->logged_in = FALSE;
	tserv->activated = FALSE;
	return dp_RES_OK;
}
