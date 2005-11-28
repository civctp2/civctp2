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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "dp2.h"	/* for DPRINT, etc. */
#include "tcapw.h"
#include "assoctab.h"
#include "crypttab.h"
#include "mywcs.h"     /* extra wcs functions */
#include "3rdparty/md5/global.h"  /* for md5 */ 
#include "3rdparty/md5/md5.h"

#ifdef _DEBUG
const char *tcapw_u2ascii(const dp_netchar_t *ncs, int maxlen)
{
	static char buf[256];
	wchar_t wcs[256];
	memset(buf, 0, maxlen+1);
	buf[0] = '?';
	mywcs_netchar2wchar(wcs, ncs, maxlen); 
	mywcs_tombs(buf, maxlen, wcs);
	return buf;
}

const char *tcapw_hexprint(const unsigned char *binstr, int len)
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
  Create a password database structure.
  Called only by server.
  Returns pointer to structure on success,
          NULL on failure.
--------------------------------------------------------------------------*/
tcapw_t *tcapw_create(void)
{
	tcapw_t *tcapw;

	if ((tcapw = (tcapw_t *) malloc(sizeof(tcapw_t))) == NULL)
		return (tcapw_t *)NULL;
	memset(tcapw, 0, sizeof(tcapw_t));
	tcapw->tab = assoctab_create(sizeof(tcapw_entry_t));

	return tcapw;
}

/*--------------------------------------------------------------------------
  Destroy a password database structure.
  Called only by server.
--------------------------------------------------------------------------*/
void tcapw_destroy(tcapw_t *tcapw)
{
	if (tcapw == NULL)
		return;
	assoctab_destroy(tcapw->tab);
	memset(tcapw, 0, sizeof(tcapw_t));
	free(tcapw);
}


const unsigned char freezekey[8] = "z8&2nD!G";

/*--------------------------------------------------------------------------
  Load a password database from disk.
  Called only by server.
  Returns dp_RES_OK on success.
          dp_RES_EMPTY if file doesn't exist; this is not a failure, if
                        you were intending to create a database.
		  dp_RES_NOMEM on failure to allocate memory
          dp_RES_BAD on bad arguments, i.e. NULL *tcapw
--------------------------------------------------------------------------*/
dp_result_t tcapw_open(tcapw_t *tcapw, const char *fname)
{
	void *p;
	FILE *fp;

	if (tcapw == NULL)
		return dp_RES_BAD;
	if (fname[0] == '\0')
		return dp_RES_BAD;

	strcpy(tcapw->filename, fname);
	DPRINT(("tcapw_open: Opening database %s\n", fname));
	if ((fp = fopen(fname, "rb")) == NULL) {
		DPRINT(("tcapw_open: file not found, creating new.\n"));
		return dp_RES_EMPTY;
	}

	/* read the encrypted database from disk. */
	p = dynatab_thaw_encrypted(tcapw->tab, fp, freezekey);

	fclose(fp);
	if (p == NULL) {
		DPRINT(("tcapw_open: read error!\n"));
		return dp_RES_NOMEM;
	}

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Save a password database to disk.
 Called only by server.
 Returns dp_RES_OK on success,
		 dp_RES_EMPTY if no database was opened,
         dp_RES_CANT_WRITE on write failure,
		 dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_close(tcapw_t *tcapw)
{
	FILE *fp;

	if (tcapw == NULL)
		return dp_RES_BAD;
	if (tcapw->filename[0] == '\0')
		return dp_RES_EMPTY;

	if ((fp = fopen(tcapw->filename, "wb")) == NULL)
		return dp_RES_CANT_WRITE;

	/* need error code? */
	dynatab_freeze_encrypted(tcapw->tab, fp, freezekey);
	fclose(fp);

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Compute the hash of a password.
  Called by server and client.
  Returns dp_RES_OK on success.
          dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_password_hash(tcapw_t *tcapw, const tcapw_pw_t *pw, tcapw_hpw_t *hpw)
{
	MD5_CTX context;
	int len_pw;
	
	if (tcapw == NULL || pw == NULL || hpw == NULL)
		return dp_RES_BAD;

	/* zero pad the password out to the maximum length to be consistant */
	len_pw = mywcs_lenn(pw->pw, tcapw_LEN_PW);
	memset((void *)(pw->pw + len_pw), 0, tcapw_LEN_PW - len_pw);
		
	MD5Init(&context);
	MD5Update(&context, (char *)pw->pw, tcapw_LEN_PW*sizeof(short));
	MD5Final(hpw->hpw, &context);
	return dp_RES_OK;
}

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
dp_result_t tcapw_entry_create(tcapw_t *tcapw, const tcapw_uname_t *username, const tcapw_hpw_t *hpw, int flags, const char *email, tcapw_uid_t *uid)
{
	dp_result_t err;
	tcapw_entry_t *p;
	tcapw_entry_t entry;
	
	if (tcapw == NULL || username == NULL || hpw == NULL || uid == NULL || email == NULL)
		return dp_RES_BAD;

	*uid = tcapw_UID_NONE;
	/* Is the username in use? */
	if ((err = tcapw_entry_find_byname(tcapw, username, &entry)) == dp_RES_OK)
		return dp_RES_ALREADY;
	else if (err != dp_RES_EMPTY)  /* failed to find username for a bad reason */
		return err;
	
	/* Need to figure out how to pick a uid safely */
	if (tcapw->tab->n_used == 0) {
		p = &entry;
		*uid = 66000;
	} else {
		p = (tcapw_entry_t *)assoctab_getkey(tcapw->tab, tcapw->tab->n_used-1);
 		*uid = p->uid + 1;
	}
	err = tcapw_entry_find_byid(tcapw, *uid, p);
	if (err != dp_RES_EMPTY) {
		*uid = tcapw_UID_NONE;
		return dp_RES_BUG;
	}
	if ((p = (tcapw_entry_t *)assoctab_subscript_grow(tcapw->tab, *uid)) == NULL) {
		*uid = tcapw_UID_NONE;
		return dp_RES_NOMEM;
	}

	p->uid = *uid;
	mywcs_ncpy0(p->uname.uname, username->uname, tcapw_LEN_USERNAME);
	memcpy(&(p->hpw), hpw, sizeof(tcapw_hpw_t));
	strncpy(p->email, email, tcapw_MAXLEN_EMAIL);
	p->flags = flags;
	p->created = time(NULL);
	p->lastlogin = p->created;
	p->secretcode = (unsigned short)((unsigned short)eclock()*(long)3001);
	
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Delete a user's password entry given his uid.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if uid was not found,
		  dp_RES_BAD on bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_delete(tcapw_t *tcapw, tcapw_uid_t uid)
{
	dp_result_t err;
	
	if (tcapw == NULL || uid == tcapw_UID_NONE)
		return dp_RES_BAD;
	if ((err = assoctab_subscript_delete(tcapw->tab, uid)) != dp_RES_OK)
		return dp_RES_EMPTY;  /* uid was not in assoctab */
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Change a user's password, email, or flags from tcapw_entry_FLAGS_*.
  Takes the hash of the new password as input.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_BAD on bad args,
		  dp_RES_BUG on internal error.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_change(tcapw_t *tcapw, tcapw_uid_t uid, const tcapw_hpw_t *new_hpw, int flags, const char *email)
{
	tcapw_entry_t *p;
	
	if (tcapw == NULL || uid == tcapw_UID_NONE || new_hpw == NULL || email == NULL)
		return dp_RES_BAD;
	
	p = (tcapw_entry_t *) assoctab_subscript(tcapw->tab, uid);
	memcpy(&(p->hpw), new_hpw, sizeof(tcapw_hpw_t));
	p->flags = flags;
	strncpy(p->email, email, tcapw_MAXLEN_EMAIL);
	
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Retrieve a user's password entry given his uid.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if the uid does not exist,
		  dp_RES_BAD if bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_find_byid(tcapw_t *tcapw, tcapw_uid_t uid, tcapw_entry_t *buf)
{
	tcapw_entry_t *p;
	
	if (tcapw == NULL || uid == tcapw_UID_NONE || buf == NULL)
		return dp_RES_BAD;
	/* brute force linear search, hidden inside assoctab_subscript */
	if ((p = (tcapw_entry_t *)assoctab_subscript(tcapw->tab, uid)) == NULL)
		return dp_RES_EMPTY;
	memcpy(buf, p, sizeof(tcapw_entry_t));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Retrieve a user's password entry given his username.
  Called only by server.
  Returns dp_RES_OK on success,
          dp_RES_EMPTY if the username does not exist,
		  dp_RES_BAD if bad args.
--------------------------------------------------------------------------*/
dp_result_t tcapw_entry_find_byname(tcapw_t *tcapw, const tcapw_uname_t *username, tcapw_entry_t *buf)
{
	int i;
	tcapw_entry_t *p;
	assoctab_item_t *a;

	if (tcapw == NULL || username == NULL || buf == NULL)
		return dp_RES_BAD;

	/* brute force linear search */
	for (i = 0; i < tcapw->tab->n_used; i++) {
		a = assoctab_getkey(tcapw->tab, i);
		p = (tcapw_entry_t *)a->value;
		if (!mywcs_ncmp(p->uname.uname, username->uname, tcapw_LEN_USERNAME)) {
			memcpy(buf, p, sizeof(tcapw_entry_t));
			return dp_RES_OK;
		}
	}
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Return the network byte order Unicode representation of a player's
 secret code in 'secretcode' from the database entry pointed to by 'pe'.
 'secretcode' is a dp_netchar_t buffer of length tcapw_LEN_PW of the sort
 contained in a tcapw_pw_t, for instance.
 Returns dp_RES_OK on success,
         dp_RES_BAD on bad arguments.
--------------------------------------------------------------------------*/
dp_result_t tcapw_get_secretcode(const tcapw_entry_t *pe, dp_netchar_t *secretcode)
{
	short buf[6];  /* unsigned short int -> 5 digits + '\0' maximum */
	short *c;
	unsigned short x;

	if (pe == NULL || secretcode == NULL) return dp_RES_BAD;
	buf[5] = 0;
	c = buf + 5;
	for (x = pe->secretcode; x > 0; x /= 10)
		*(--c) = '0' + x%10;
	mywcs_swabncpy0(secretcode, c, tcapw_LEN_PW);
	return dp_RES_OK;
}
