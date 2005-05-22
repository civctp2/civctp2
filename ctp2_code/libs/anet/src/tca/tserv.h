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
 Object that manages the account creation, login, and update process.

 All methods that take strings have two versions: a version ending in
 W that takes Unicode strings (wchar_t *'s), and a version ending in A
 that takes strings in the local 8 bit or multibyte character set (char *'s).

 All functions that return dp_result_t's in any way reserve the following two
 codes to always have the same meaning:
 dp_RES_BUG  - an internal failure has occurred.
 dp_RES_BAD  - the function was passed bad parameters.
 If either of these two codes are returned, the caller should probably abort
 the program, and the programmers responsible should fix the bug.
--------------------------------------------------------------------------*/

#ifndef tserv_h
#define tserv_h

#include "aneterr.h"
#include "commapi.h"
#include "dpio.h"
#include "tca.h"
#include "eclock.h"

#define tserv_MAGIC 0xdeadbeef
#ifdef _DEBUG
#define tserv_assertValid(t) assert((t)->magic == tserv_MAGIC);
#else
#define tserv_assertValid(t)
#endif

/* All info regarding client authentication status lives in a tserv_t. */
typedef struct {
	dpio_t *dpio;
	tca_t *tca;

	clock_t tnextPoll;
	
	long magic;
	
	/* server info */
	assoctab_t *clients;        /* list of clients' info indexed by handle */
	
	/* client info */
	playerHdl_t hServer;
   	tcapw_entry_t userinfo;
	tcapw_pw_t secretcode;      /* secretcode used by tserv_account_activate */
	tcapw_hpw_t newhpw;         /* newpassword used by tserv_change_password */
	int challenge_set;          /* TRUE if we have received a challenge */
	tca_challenge_t challenge;
	int logged_in;              /* TRUE if we are logged in */
	int activated;              /* TRUE if we are activated */
    /* all please_send's are TRUE if a packet of the given type has been
	 * scheduled using tserv_account_* or tserv_change_password.
	 */
	int please_send_response;
	int please_send_newuser;
	int please_send_secretcode;
	int please_send_pwchange;
	int waiting_for_rx;         /* TRUE if we are expecting a packet */
	clock_t rx_deadline;        /* If the server does not send an expected
								 * packet before this 30s deadline, time out.
								 */
} tserv_t;

/* An entry in the clients assoctab on the server */
typedef struct {
	tcapw_uid_t uid;		/* tcapw_UID_NONE until successful login */
	int logged_in;          /* TRUE if password has been validated */
	int activated;          /* TRUE if email has been validated */
	tca_challenge_t challenge;
	int challenge_uses;         /* number of uses of this challenge */
	/* Following field used to control retransmission in case packet
	 * couldn't be queued on first try
	 */
	char need_send;		    /* 0, tserv_KIND_CHALLENGE, _VALIDATE, or _PWVALIDATE */
	dp_result_t reason;     /* dp_RES_OK or reason for (pw)validate failure */
} tserv_clientinfo_t;

/* Number of reuses of one challenge before we report to the caller. */
#define tserv_MAX_CHALLENGE_USES 10

/* All packets generated or used by this module have this header. */
#define dp_TSERV_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'(')

/* Values for tserv_packet_t.kind */
#define tserv_KIND_CHALLENGE   'c'
#define tserv_KIND_RESPONSE    'r'
#define tserv_KIND_NEWUSER     'n'
#define tserv_KIND_SECRETCODE  's'
#define tserv_KIND_PWCHANGE    'p'
#define tserv_KIND_VALIDATE    'v'
#define tserv_KIND_PWVALIDATE  'w'
#define tserv_KIND_EMAILREQ    'q'
#define tserv_KIND_CREDENTIALS 'k'

/* The packets that go over the wire.  Packed to avoid wasted space
 * and for portability. 
 */
#include "dppack1.h"

/* Packet sent from server to client after server gets response. */
typedef struct {
	tcapw_uid_t uid PACK;		/* tcapw_UID_NONE if login failed */
	dp_result_t reason PACK;    /* dp_RES_OK or reason for failure */
} tserv_packet_validate_t;

/* Packet sent from server to client after server gets pwchange. */
typedef struct {
	dp_result_t reason PACK;    /* dp_RES_OK or reason for failure */
} tserv_packet_pwvalidate_t;

/*
 * This packet is from a server to a client requesting
 * that a connection be opened to that specified address pair
 * This packet should only be accepted by clients if it comes from a 
 * server and should be discarded otherwise.
 *
 * The reason for this packets existence is allow people behind firewalls
 * to open connections to each other, and to tell the client the uid of the 
 * guy he's opening a connection to.
 *
 * The two addresses adr, adr2 are the two address of the player who
 * wishes to join the session.  They may be identical, in which case the
 * player has only one known addresses, or they may be different in which
 * case the player is multihomed or is behind a renumbering firewall.
 *
 */

typedef struct {
	unsigned char	adr[dp_MAX_ADR_LEN] PACK;
	unsigned char	adr2[dp_MAX_ADR_LEN] PACK;
	tcapw_uid_t uid PACK;		/* uid of guy at that address */
} tserv_packet_credentials_t;

typedef struct {
	unsigned char kind PACK;
	union {
		tca_challenge_t challenge;
		tca_response_t  response;
		tca_pwchange_t  pwchange;
		tca_newuser_t   newuser;
		tserv_packet_validate_t   validate;
		tserv_packet_pwvalidate_t pwvalidate;
		tserv_packet_credentials_t credentials;
	} u PACK;
} tserv_packet_t;
#include "dpunpack.h"

/* Structure used to notify caller of server or client status changes. */
typedef struct {
	playerHdl_t h;		/* handle this event is related to */
	tcapw_uid_t uid;	/* uid of logged in client */
	dp_result_t reason;	/* 0 on success, dp_RES_* on failure */
} tserv_event_t;

/***** Shared stuff *****/

/*--------------------------------------------------------------------------
 Create a tserv_t using an existing dpio and tca.
 Returns NULL on failure.
--------------------------------------------------------------------------*/
tserv_t *tserv_create(dpio_t *dpio, tca_t *tca);

/*--------------------------------------------------------------------------
 Destroy a tserv_t.
--------------------------------------------------------------------------*/
void tserv_destroy(tserv_t *tserv);

/*--------------------------------------------------------------------------
 Freeze a tserv_t to file.
 Returns dp_RES_OK on success,
 		 dp_RES_FULL on write failure.
--------------------------------------------------------------------------*/
dp_result_t tserv_Freeze(tserv_t *tserv, FILE *fp);

/*--------------------------------------------------------------------------
 Thaw a tserv_t from file.
 tserv is a pointer to a tserv_t created with tserv_create and a valid tca.
 Returns dp_RES_OK on success,
 		 dp_RES_EMPTY on read failure.
--------------------------------------------------------------------------*/
dp_result_t tserv_Thaw(tserv_t *tserv, FILE *fp);

/*--------------------------------------------------------------------------
 Return the player handle of the player with the given uid, or PLAYER_NONE
 if uid is not logged in.
--------------------------------------------------------------------------*/
playerHdl_t tserv_uid2hdl(tserv_t *tserv, tcapw_uid_t uid);

/*--------------------------------------------------------------------------
 Return the uid of the player with the given handle, or tcapw_UID_NONE
 if the handle is not logged in.
--------------------------------------------------------------------------*/
tcapw_uid_t tserv_hdl2uid(tserv_t *tserv, playerHdl_t h);

/*--------------------------------------------------------------------------
 Server only.
 Gets a user's info from the database into the space pointed to by *entry.
 Does not get users password.
 Returns dp_RES_OK on success,
         dp_RES_EMPTY if uid is not in the database or is tcapw_UID_NONE,
         dp_RES_BAD on bad arguments.
--------------------------------------------------------------------------*/
dp_result_t tserv_uid2info(tserv_t *tserv, tcapw_uid_t uid, tcapw_entry_t *entry);

/*--------------------------------------------------------------------------
 Perform periodic housekeeping.
 Call once every second or so.
 Returns dp_RES_OK normally,
         dp_RES_HOST_NOT_RESPONDING for the client if the server hasn't
		         replied to the last client tserv packet within 30s.
 		 dp_RES_NOTYET if tserv has not been initialized,
		 dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tserv_poll(tserv_t *tserv);

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
dp_result_t tserv_handle_packet(tserv_t *tserv, playerHdl_t src, int pkt_flags, tserv_packet_t *pkt, tserv_event_t *result);

/***** Server stuff *****/

/*--------------------------------------------------------------------------
 Call when a new client handle is created.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t tserv_client_add(tserv_t *tserv, playerHdl_t h);

/*--------------------------------------------------------------------------
 Call when a client handle is destroyed.
 Returns dp_RES_OK on success.
--------------------------------------------------------------------------*/
dp_result_t tserv_client_delete(tserv_t *tserv, playerHdl_t h);

/*--------------------------------------------------------------------------
 Server only.
 Call to tell two clients to shake hands and be friends (and prove to
 each other what their uid's are).
 Returns dp_RES_OK on success.
 		 dp_RES_NOTYET if tserv has not been initialized.
--------------------------------------------------------------------------*/
dp_result_t tserv_send_credentials(tserv_t *tserv, playerHdl_t h1, playerHdl_t h2);

/***** Client Stuff *****/

/*--------------------------------------------------------------------------
 Call to initiate a connection to a server.
 Handle to server must be opened immediately before this call
 (i.e. without any intervening calls to dpReceive).
 Returns dp_RES_OK if request is queued successfully.
--------------------------------------------------------------------------*/
dp_result_t tserv_server_add(tserv_t *tserv, playerHdl_t hServer);

/*--------------------------------------------------------------------------
 Client only.
 Call to log in to an existing account.

 'username' is the user name specified by the user in tserv_account_create().

 'password' is the password set by the user in tserv_account_create() or
 tserv_change_password().

 Returns dp_RES_OK if the request was successfully queued,
         dp_RES_BUSY if a request is already in progress,
		 dp_RES_BAD on bad args.

 Success or failure is indicated by the result event from a later call
 to tserv_handle_packet().
 If the account has not yet been activated, the 
--------------------------------------------------------------------------*/
dp_result_t tserv_account_loginW(tserv_t *tserv, const wchar_t *username, const wchar_t *password);

dp_result_t tserv_account_loginA(tserv_t *tserv, const char *username, const char *password);

#ifdef UNICODE
#define tserv_account_login tserv_account_loginW
#else
#define tserv_account_login tserv_account_loginA
#endif

/*--------------------------------------------------------------------------
 Client only.
 Call to create a new account.

 flags is either zero (if user wants junk mail) or 
 tcapw_entry_FLAGS_EMAIL_PRIVATE (if user prefers privacy).

 'username' is an arbitrary name the user would like to use; it must not be
 the same as an existing name, and should not contain unprintable characters
 or leading or trailing spaces.

 'password' is an arbitrary string the user can remember; he will need it to
 log into the account later.  If this is easy to guess, the account is
 vulnerable to hacking.  

 'email' is the user's Internet email address; this is required so we can
 make it harder for cheaters to create new accounts frequently and avoid
 a bad reputation.

 Returns dp_RES_OK if the request was successfully queued,
         dp_RES_BUSY if a request is already in progress,
		 dp_RES_BAD on bad args.

 Success or failure is indicated by the result event from a later call
 to tserv_handle_packet().  
 If it succeeds, the user will receive an email containing a secret code
 needed to activate the account.  This should be entered by calling
 tserv_account_activate() after calling tserv_account_login() and receiving 
 an event from tserv_handle_packet() containing reason dp_RES_NOTYET.
--------------------------------------------------------------------------*/
dp_result_t tserv_account_createW(tserv_t *tserv, const wchar_t *username, const wchar_t *password, int flags, const wchar_t *email);

dp_result_t tserv_account_createA(tserv_t *tserv, const char *username, const char *password, int flags, const char *email);

#ifdef UNICODE
#define tserv_account_create tserv_account_createW
#else
#define tserv_account_create tserv_account_createA
#endif

/*--------------------------------------------------------------------------
 Client only.
 Call to activate an account.

 'secretcode' is a string the user must type in from an email sent him by
 tserv_account_create() or tserv_request_email().

 Returns dp_RES_OK if the request was successfully queued,
         dp_RES_BUSY if a request is already in progress,
		 dp_RES_BAD on bad args.

 Success or failure is indicated by the result event from a later call
 to tserv_handle_packet().
 If it succeeds, the account is activated, and this function will no longer
 need to be called after login.
--------------------------------------------------------------------------*/
dp_result_t tserv_account_activateW(tserv_t *tserv, const wchar_t *secretcode);

dp_result_t tserv_account_activateA(tserv_t *tserv, const char *secretcode);

#ifdef UNICODE
#define tserv_account_activate tserv_account_activateW
#else
#define tserv_account_activate tserv_account_activateA
#endif

/*--------------------------------------------------------------------------
 Client only.
 Send a userinfo change request using the challenge we logged in with.
 Client must be logged in and authenticated to call this function.

 username, password, and email are in native byte-order unicode or ASCII.
 flags is a union of user set tcapw_entry_FLAGS_*.
  
 Returns dp_RES_OK if the request was successfully queued,
         dp_RES_BAD if the client was not logged in or authenticated,
         dp_RES_* if the request can't be queued.

 Success or failure is indicated by the result event from a later call
 to tserv_handle_packet().
--------------------------------------------------------------------------*/
dp_result_t tserv_change_passwordW(tserv_t *tserv, const wchar_t *oldpassword, const wchar_t *newpassword, int flags, const wchar_t *email);

dp_result_t tserv_change_passwordA(tserv_t *tserv, const char *oldpassword, const char *newpassword, int flags, const char *email);

#ifdef UNICODE
#define tserv_change_password tserv_change_passwordW
#else
#define tserv_change_password tserv_change_passwordA
#endif

/*--------------------------------------------------------------------------
 Client only.
 Used only if user loses the email containing the secret code sent at account
 creation time.
 Can only be called after logging in, but before account has been activated.
 Call this to request that the server send the secret code needed for 
 account activation to the user's email address.
 Multiple calls will result in the same message being sent.
--------------------------------------------------------------------------*/
dp_result_t tserv_request_email(tserv_t *tserv);

#endif
