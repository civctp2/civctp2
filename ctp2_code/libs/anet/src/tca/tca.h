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
  Trivial challenge authentication system.
--------------------------------------------------------------------------*/

#ifndef tca_h
#define tca_h

#include "tcapw.h"

/* changing these would require changes to tca.c and possibly tcapw.c, since
 * the 8 byte in/output of DES and the 16 byte output of MD5 currently
 * determine their sizes.
 */
#define tca_LEN_CHALLENGE 8
#define tca_LEN_RESPONSE 16
#define tca_LEN_PWCHANGE (tca_LEN_RESPONSE + tcapw_LEN_HASHPW)
#define tca_LEN_NEWUSER (tcapw_LEN_HASHPW + 8 /*LEN_MAGIC*/)

/* All structures sent over the network should be packed.  This is done
 * by putting #include "dppack1.h" before, and #include "dpunpack.h" after,
 * and using the keyword PACK after each structure member.
 */
#include "dppack1.h"

/* A challenge presented to a user, created by tca_challenge_generate().
 */
typedef struct {
	char challenge[tca_LEN_CHALLENGE];
} PACK tca_challenge_t;

/* A response from a user, created by tca_response_generate().
 */
typedef struct {
	char response[tca_LEN_RESPONSE];
	char unamelen;
	short uname[tcapw_LEN_USERNAME /*unamelen*/];    /* VARIABLE LENGTH USERNAME */ 
} PACK tca_response_t;

/* A password or email change request from a user, created by
 * tca_pwchange_generate().
 */
typedef struct {
	char pwchange[tca_LEN_PWCHANGE];
	unsigned long int flags;    /* | of tcapw_entry_FLAGS_* set by user */
	unsigned char emaillen;
	char email[tcapw_MAXLEN_EMAIL];
} PACK tca_pwchange_t;

/* A create new user request from a user, created by tca_newuser_generate().
 */
typedef struct {
	char newuser[tca_LEN_NEWUSER];
	unsigned long int flags;    /* | of tcapw_entry_FLAGS_* set by user */
	unsigned char unamelen;
	unsigned char emaillen;
	char storage[tcapw_LEN_USERNAME*sizeof(short) + tcapw_MAXLEN_EMAIL];
	/* VARIABLE LENGTH USERNAME AND EMAIL EMBEDDED IN storage:
	 * short uname[unamelen];
	 * char email[emaillen];
	 */
} PACK tca_newuser_t;

#include "dpunpack.h"

/* The main type of this module. */
typedef struct {
	tcapw_t *tdb;
	/* a random challenge generator's data would go here */
} tca_t;


/*--------------------------------------------------------------------------
  Create an instance of the trivial challenge authenticator module.
  Called by both client and server.
  Returns NULL on failure.
--------------------------------------------------------------------------*/
tca_t *tca_create(void);

/*--------------------------------------------------------------------------
  Load a password database from disk.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if fname was not found (not an error if you intend
		                to create a new database,
          dp_RES_BAD on bad args (NULL tca or tca->tdb).
--------------------------------------------------------------------------*/
dp_result_t tca_openpw(tca_t *tca, const char *fname);

/*--------------------------------------------------------------------------
 Discard an instance of the trivial challenge authenticator module.
 Closes the password database if it is open.
 Called by both client and server.
 Returns dp_RES_OK on success,
         dp_RES_BAD if tca is bad (tca->tdb is NULL),
         dp_RES_ALREADY if tca is NULL already,
		 dp_RES_FILE_ERROR if the database could not be written to disk.
--------------------------------------------------------------------------*/
dp_result_t tca_destroy(tca_t *tca);

/*--------------------------------------------------------------------------
 Freeze an instance of the trivial challenge authenticator module to disk.
 Freezes password database to disk as well.
 Returns dp_RES_OK on success,
 		 dp_RES_FULL on write failure.
--------------------------------------------------------------------------*/
dp_result_t tca_Freeze(tca_t *tca, FILE *fp);

/*--------------------------------------------------------------------------
 Thaw an instance of the trivial challenge authenticator module from disk.
 Loads the password database from disk as well.
 Returns dp_RES_OK on success,
 		 dp_RES_EMPTY on read failure.
--------------------------------------------------------------------------*/
dp_result_t tca_Thaw(tca_t *tca, FILE *fp);

/*--------------------------------------------------------------------------
  Look up a user's uid from his name.
  Called by both client and server.
  Returns uid on success,
          tcapw_UID_NONE on failure.
--------------------------------------------------------------------------*/
tcapw_uid_t tca_uname2uid(tca_t *tca, const tcapw_uname_t *uname);

/*--------------------------------------------------------------------------
 Look up a user's uname from his uid.
 Called by both client and server.
 Returns dp_RES_OK on success,
         dp_RES_EMPTY if uid does not exist,
		 dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_uid2uname(tca_t *tca, tcapw_uid_t uid, tcapw_uname_t *uname);

/*--------------------------------------------------------------------------
  Generate a new challenge.
  Called by server.
  Challenge stored in the given buffer.
  No two challenges are ever the same.
  Returns dp_RES_OK on success,
          dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_challenge_generate(tca_t *tca, tca_challenge_t *challenge);

/*--------------------------------------------------------------------------
  Check a user's response to a challenge generated by tca_challenge_generate.
  Returns the user's uid into the tcapw_uid_t pointed to by uid.
  Called by server.
  Returns dp_RES_OK on success,
          dp_RES_ACCESS if the response was invalid,
		  dp_RES_EMPTY if the uid does not exist,
		  dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_response_validate(tca_t *tca, const tca_challenge_t *challenge, const tca_response_t *response, tcapw_uid_t *uid);

/*--------------------------------------------------------------------------
  Generate a response for a challenge.
  Called by client.
  On entry, response points to a buffer of size sizeof(tca_response_t),
            responselen points to an uninitialized integer.
  On success, response points to a filled response struct
              responselen points to the total used size of the struct.
  Returns dp_RES_OK on success,
          dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_response_generate(tca_t *tca, const tca_challenge_t *challenge, const tcapw_uname_t *username, const tcapw_hpw_t *hpw, tca_response_t *response, int *responselen);

/*--------------------------------------------------------------------------
  Generate a password or email change request.
  Called by client.
  Just like a normal response, but also contains the desired new password,
  email, and flags from tcapw_entry_FLAGS_*.
  Returns dp_RES_OK on success,
          dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_pwchange_generate(tca_t *tca, const tca_challenge_t *challenge, const tcapw_hpw_t *oldhpw, const tcapw_hpw_t *newhpw, int flags, const char *email, tca_pwchange_t *pwchange, int *pwchangelen);

/*--------------------------------------------------------------------------
  Change the user's password if the challenge response contained in pwchange
  is valid.
  Called by server.
  Returns dp_RES_OK on success,
          dp_RES_ACCESS if the response was invalid,
		  dp_RES_BUG if the password change failed,
		  dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_pwchange_validate(tca_t *tca, tcapw_uid_t uid, const tca_challenge_t *challenge, const tca_pwchange_t *pwchange);

/*--------------------------------------------------------------------------
  Generate a newuser request.
  Called by client.
  The newuser packet contains the desired new username, email, hashed
  password, and flags from tcapw_entry_FLAGS_*.
  On entry, newuser points to a buffer of size sizeof(tca_newuser_t),
            newuserlen points to an uninitialized integer.
  On success, newuser points to a filled newuser struct
              newuserlen points to the total used size of the struct.
  Returns dp_RES_OK on success,
          dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_newuser_generate(tca_t *tca, const tca_challenge_t *challenge, const tcapw_uname_t *newusername, const tcapw_hpw_t *newhpw, int flags, const char *email, tca_newuser_t *newuser, int *newuserlen);

/*--------------------------------------------------------------------------
  Create a new user with the username and password contained in newuser
  if the challenge response contained in newuser is valid.
  Returns the user's uid into the tcapw_uid_t pointed to by uid.
  Called by server.
  Returns dp_RES_OK on success,
          dp_RES_ACCESS if the response was invalid,
		  dp_RES_ALREADY if the username is in use,
		  dp_RES_BUG if the user creation failed for another reason,
		  dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_newuser_validate(tca_t *tca, const tca_challenge_t *challenge, tca_newuser_t *newuser, tcapw_uid_t *uid);

/*--------------------------------------------------------------------------
  Check a user's email validating secret code response to a challenge
  generated by tca_challenge_generate.
  Called by server.
  Returns dp_RES_OK on success,
          dp_RES_ACCESS if the response was invalid,
		  dp_RES_EMPTY if the uid does not exist,
		  dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tca_secretcode_validate(tca_t *tca, tcapw_uid_t uid, const tca_challenge_t *challenge, const tca_response_t *response);

#endif
