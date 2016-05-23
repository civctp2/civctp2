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
  Password database for trivial challenge authentication system.
--------------------------------------------------------------------------*/

#ifndef tcapw_h
#define tcapw_h

#include <stdio.h>
#include <time.h>
#include "aneterr.h"
#include "assoctab.h"

/* tcapw_LEN_PW can't be changed without modifications to tca.c, since
 * the D2DES routines depend on using passwords as 16 byte keys.
 * tcapw_LEN_HASHPW is set by the 16 byte output of MD5.
 */
#define tcapw_LEN_USERNAME 16
#define tcapw_LEN_PW 16
#define tcapw_LEN_HASHPW 16

#define tcapw_MAXLEN_EMAIL 64

/* A numerical code for a user */
typedef int tcapw_uid_t;
#define tcapw_UID_NONE 0

/* All structures sent over the network should be packed.  This is done
 * by putting #include "dppack1.h" before, and #include "dpunpack.h" after,
 * and using the keyword PACK after each structure member.
 */
#include "dppack1.h"

/* A user's username, stored as network byte order UCS2 Unicode,
 * trailing-zero-padded out or truncated to tcapw_LEN_USERNAME chars as needed.
 */
typedef struct {
	dp_netchar_t uname[tcapw_LEN_USERNAME] PACK;
} tcapw_uname_t;

/* A user's password, stored as network byte order UCS2 Unicode,
 * trailing-zero-padded out or truncated to tcapw_LEN_PW chars, as needed.
 */
typedef struct {
	dp_netchar_t pw[tcapw_LEN_PW];
} PACK tcapw_pw_t;

/* A user's hashed password, computed as the MD5 hash of the tcapw_pw_t
 * representation of a user's password.
 */
typedef struct {
	char hpw[tcapw_LEN_HASHPW];
} PACK tcapw_hpw_t;

/* Hashed passwords are stored on disk as an encrypted array of this
 * structure.
 */
#define tcapw_entry_FLAGS_EMAIL_VALID   1  /* valid reply has been received */
#define tcapw_entry_FLAGS_EMAIL_PRIVATE 2  /* marketing email not ok */
/* tcapw_entry_CLIENT_SET_FLAGS should be set to the union of all
 * tcapw_entry_FLAGS which the client is allowed to set.
 */
#define tcapw_entry_CLIENT_SET_FLAGS (tcapw_entry_FLAGS_EMAIL_PRIVATE)
typedef struct {
	tcapw_uid_t uid;
	tcapw_uname_t uname;
	tcapw_hpw_t hpw;
	char email[tcapw_MAXLEN_EMAIL];		/* User's email address */
	int flags PACK;				/* Bitwise OR of tcapw_entry_FLAGS_* */
	unsigned short int secretcode;
                             	/* A secret code emailed to the user
								 * to validate their email address.
								 */
	time_t created;		/* time() when account created */
	time_t lastlogin;		/* time() when account last logged in */
} PACK tcapw_entry_t;

#include "dpunpack.h"

/* A password database.
 */
typedef struct {
	/* OK, so I'm a cheezemeister.  We'll do a real database later. */
	assoctab_t *tab;
	char filename[256];  /* empty string "" indicates no database is open */
} tcapw_t;

/*--------------------------------------------------------------------------
  Create a password database structure.
  Called only by server.
  Returns pointer to structure on success,
          NULL on failure.
--------------------------------------------------------------------------*/
tcapw_t *tcapw_create(void);

/*--------------------------------------------------------------------------
  Destroy a password database structure.
  Called only by server.
--------------------------------------------------------------------------*/
void tcapw_destroy(tcapw_t *tcapw);

/*--------------------------------------------------------------------------
  Load a password database from disk.
  Called only by server.
  Returns dp_RES_OK on success.
          dp_RES_EMPTY if file doesn't exist; this is not a failure, if
                        you were intending to create a database.
		  dp_RES_NOMEM on failure to allocate memory
          dp_RES_BAD on bad arguments, i.e. NULL *tcapw
--------------------------------------------------------------------------*/
dp_result_t tcapw_open(tcapw_t *tcapw, const char *fname);

/*--------------------------------------------------------------------------
 Save a password database to disk.
 Called only by server.
 Returns dp_RES_OK on success,
		 dp_RES_EMPTY if no database was opened,
         dp_RES_CANT_WRITE on write failure,
		 dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_close(tcapw_t *tcapw);

/*--------------------------------------------------------------------------
  Compute the hash of a password.
  Called by server and client.
  Returns dp_RES_OK on success.
          dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_password_hash(tcapw_t *tcapw, const tcapw_pw_t *pw, tcapw_hpw_t *hpw);

/*--------------------------------------------------------------------------
  Create a new user's password entry given his name, the hash of his initial
  password, his email, and any user flags from tcapw_entry_FLAGS_*.
  Generates a secretcode for tserv level account validation.
  Returns the user's new uid into the tcapw_uid_t pointed to by uid.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_ALREADY if username is in use,
		  dp_RES_NOMEM on failure to allocate memory,
		  dp_RES_BAD on bad args,
		  dp_RES_BUG if finding a uid fails.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_create(tcapw_t *tcapw, const tcapw_uname_t *username, const tcapw_hpw_t *hpw, int flags, const char *email, tcapw_uid_t *uid);

/*--------------------------------------------------------------------------
  Delete a user's password entry given his uid.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if uid was not found,
		  dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_delete(tcapw_t *tcapw, tcapw_uid_t uid);

/*--------------------------------------------------------------------------
  Change a user's password, email, or flags from tcapw_entry_FLAGS_*.
  Takes the hash of the new password as input.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_BAD on bad args,
		  dp_RES_BUG on internal error.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_change(tcapw_t *tcapw, tcapw_uid_t uid, const tcapw_hpw_t *new_hpw, int flags, const char *email);

/*--------------------------------------------------------------------------
  Retrieve a user's password entry given his uid.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if the uid does not exist,
		  dp_RES_BAD if bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_find_byid(tcapw_t *tcapw, tcapw_uid_t uid, tcapw_entry_t *buf);

/*--------------------------------------------------------------------------
  Retrieve a user's password entry given his username.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if the username does not exist,
		  dp_RES_BAD if bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_find_byname(tcapw_t *tcapw, const tcapw_uname_t *username, tcapw_entry_t *buf);

/*--------------------------------------------------------------------------
 Return the network byte order Unicode representation of a player's
 secret code in 'secretcode' from the database entry pointed to by 'pe'.
 'secretcode' is a dp_netchar_t buffer of length tcapw_LEN_PW of the sort
 contained in a tcapw_pw_t, for instance.
 Returns dp_RES_OK on success,
         dp_RES_BAD on bad arguments.
--------------------------------------------------------------------------*/
dp_result_t tcapw_get_secretcode(const tcapw_entry_t *pe, dp_netchar_t *secretcode);

#ifdef _DEBUG
const char *tcapw_u2ascii(const short *ucs, int maxlen);
const char *tcapw_hexprint(const unsigned char *binstr, int len);
#else
#define tcapw_u2ascii(a, b) "NDEBUG"
#define tcapw_hexprint(a, b) "NDEBUG"
#endif

#endif
