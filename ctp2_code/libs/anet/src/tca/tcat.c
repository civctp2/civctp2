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
  Unit test for tca and tcapw.
--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>   /* needed to explicitly include memset */
#include "tca.h"
#include "tcapw.h"

#define DPRINT(a) printf a

#if defined(SPARC) || defined(__POWERPC__)
#define SWAB(v)		(v)
#else
#define SWAB(v)		((unsigned short) ((v) << 8) | (unsigned char) ((v) >> 8))
#endif

/* Need to simulate this function for tca_freeze/thaw to compile */
dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt) 
{
	printf("dpReportAssertionFailure(%d, %s, %s)\n",
			lineno, file, linetxt);
	exit(1);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Convert a ASCII string into a big-endian UCS2 string.
  The input string is null terminated;
  The output string is trailing-zero-filled or truncated to the given length.
  Convenience method; useful for developers who aren't familliar with Unicode.
  Returns dp_RES_OK on success.
          dp_RES_BAD if bad args, i.e. input contains an extended (> 127)
		              ascii char or *in or *out is NULL;
--------------------------------------------------------------------------*/
dp_result_t tcapw_ascii2ucs2(tcapw_t *tcapw, const char *in, short *out, int maxchars)
{
	int i;

	if (in == NULL || out == NULL)
		return dp_RES_BAD;
	
	memset(out, 0, maxchars * sizeof(short));
	for(i=0; in[i] && i < maxchars; i++) {
		if (in[i] & 0x80)
			return dp_RES_BAD;
		out[i] = SWAB(in[i]);
	}
	
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
  Convert a big-endian UCS2 string into an ASCII string.
  The input string is trailing-zero-filled or truncated to the given length;
  the output string is null terminated, and should be length maxchars+1 to
  hold a final \0 if it is needed.
  Convenience method; useful for developers who aren't familiar with Unicode.
  Returns length of output string on success,
          -1 on error.
--------------------------------------------------------------------------*/
int tcapw_ucs22ascii(tcapw_t *tcapw, const short *in, char *out, int maxchars)
{
	int i;

	if (in == NULL || out == NULL)
		return -1;
	
	for (i=0; i < maxchars; i++) {
		out[i] = SWAB(in[i]) & 0x7f;
		if (out[i] == '\0')
			return i;
	}
	out[i] = '\0';
	
	return i;
}


void test_tcapw()
{
	tcapw_t *tcapw = NULL;

	tcapw_entry_t entry1, entry2, entry3;
	tcapw_entry_t entry1b, entry2b;
	tcapw_pw_t    pw1,    pw2;
	tcapw_hpw_t   hpw1,   hpw2;
	tcapw_uname_t uname1, uname2, uname3;
	tcapw_uid_t uid1, uid2;
	char email[] = "user@host";
	int flags = 0;
	
	char namebuf[256];
	dp_result_t res;

	memset(&entry1, 0, sizeof(entry1));
	memset(&entry2, 0, sizeof(entry1));
	memset(&entry3, 0, sizeof(entry1));
	memset(&entry1b, 0, sizeof(entry1));
	memset(&entry2b, 0, sizeof(entry1));

	/* Create a password database, add a user to it,
	 * make sure you can look him up various ways,
	 * change his password,
	 * save it to disk, restore it from disk.
	 */

	/* Create the database */
	tcapw = tcapw_create();
	assert(tcapw);
	remove("pw.dat");
	res = tcapw_open(tcapw, "pw.dat");
	assert(res == dp_RES_EMPTY);

	/* Add a user to it */
	res = tcapw_ascii2ucs2(tcapw, "user1", uname1.uname, tcapw_LEN_USERNAME);
	assert(!res);
	memset(&uname3, 0, sizeof(uname3));
	uname3.uname[0] = SWAB('u');
	uname3.uname[1] = SWAB('s');
	uname3.uname[2] = SWAB('e');
	uname3.uname[3] = SWAB('r');
	uname3.uname[4] = SWAB('1');
	assert(!memcmp(&uname1, &uname3, sizeof(uname1)));
	res = tcapw_ucs22ascii(tcapw, uname1.uname, namebuf, tcapw_LEN_USERNAME);
	assert(!strcmp(namebuf, "user1"));

  	res = tcapw_ascii2ucs2(tcapw, "pass1", pw1.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw1, &hpw1);
	assert(!res);
	res = tcapw_entry_create(tcapw, &uname1, &hpw1, flags, email, &uid1);
	printf("res = %d\n", res);
	assert(!res);

	/* check that you can find him by name, and that his entry is ok */
	res = tcapw_entry_find_byname(tcapw, &uname1, &entry1);
	assert(!res);
	res = tcapw_ucs22ascii(tcapw, entry1.uname.uname, namebuf, tcapw_LEN_USERNAME);
	assert(res);
   	assert(!memcmp(&uname1, &entry1.uname, sizeof(tcapw_uname_t)));
	assert(!memcmp(&hpw1, &entry1.hpw, sizeof(hpw1)));

	/* check that you can't find him by a bogus name */
	res = tcapw_ascii2ucs2(tcapw, "user2", uname2.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_entry_find_byname(tcapw, &uname2, &entry1);
	assert(res == dp_RES_EMPTY);
	
	/* add a second user */
	res = tcapw_ascii2ucs2(tcapw, "user2", uname2.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass2", pw2.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw2, &hpw2);
	assert(!res);	
	res = tcapw_entry_create(tcapw, &uname2, &hpw2, flags, email, &uid2);
	assert(!res);

	/* Make sure you can find both users by name, and that the entries match */
	res = tcapw_entry_find_byname(tcapw, &uname1, &entry1);
	assert(!res);
	assert(!memcmp(&uname1, &entry1.uname, sizeof(uname1)));
	res = tcapw_password_hash(tcapw, &pw1, &hpw1);
	assert(!res);
	assert(!memcmp(&hpw1, &entry1.hpw, sizeof(hpw1)));
	assert(entry1.uid == uid1);
	
	res = tcapw_entry_find_byname(tcapw, &uname2, &entry2);
	assert(!res);
	assert(!memcmp(&uname2, &entry2.uname, sizeof(uname2)));
	assert(!memcmp(&hpw2, &entry2.hpw, sizeof(hpw2)));
	assert(entry2.uid == uid2);
	
	/* Make sure their id's aren't UID_NONE or the same */
	assert(entry1.uid != tcapw_UID_NONE);
	assert(entry2.uid != tcapw_UID_NONE);
	assert(entry1.uid != entry2.uid);

	/* Make sure you can find both users by uid, and that the entries match */
	res = tcapw_entry_find_byid(tcapw, entry1.uid, &entry1b);
	assert(!res);
	assert(!memcmp(&uname1, &entry1b.uname, sizeof(uname1)));
	assert(!memcmp(&hpw1, &entry1b.hpw, sizeof(hpw1)));
	assert(entry1b.uid == entry1.uid);
	
	res = tcapw_entry_find_byid(tcapw, entry2.uid, &entry2b);
	assert(!res);
	assert(!memcmp(&uname2, &entry2b.uname, sizeof(uname2)));
	assert(!memcmp(&hpw2, &entry2b.hpw, sizeof(hpw2)));
	assert(entry2b.uid == entry2.uid);
	
	/* Make sure you can't find a bogus user */
	res = tcapw_ascii2ucs2(tcapw, "bogus", uname3.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_entry_find_byname(tcapw, &uname3, &entry3);
	assert(res);

	/* Delete one user, make sure you can't find him, but can find the other */
	res = tcapw_entry_delete(tcapw, entry1.uid);
	assert(!res);
	res = tcapw_entry_find_byname(tcapw, &uname1, &entry1);
	assert(res);
	res = tcapw_entry_find_byid(tcapw, entry1b.uid, &entry1);
	assert(res);
	
	res = tcapw_entry_find_byname(tcapw, &uname2, &entry2);
	assert(!res);
	assert(!memcmp(&uname2, &entry2.uname, sizeof(uname2)));
	assert(!memcmp(&hpw2, &entry2.hpw, sizeof(hpw2)));
	res = tcapw_entry_find_byid(tcapw, entry2b.uid, &entry2);
	assert(!res);
	assert(!memcmp(&uname2, &entry2.uname, sizeof(uname2)));
	assert(!memcmp(&hpw2, &entry2.hpw, sizeof(hpw2)));
	
	/* Add a third user */
	res = tcapw_ascii2ucs2(tcapw, "user3", uname1.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass3", pw1.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw1, &hpw1);
	assert(!res);	
	res = tcapw_entry_create(tcapw, &uname1, &hpw1, flags, email, &uid1);
	assert(!res);
	res = tcapw_entry_find_byname(tcapw, &uname1, &entry3);
	assert(!res);

	/* Change the 2nd user's password and check that it changed */
	res = tcapw_ascii2ucs2(tcapw, "pass2b", pw2.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw2, &hpw2);
	assert(!res);
	res = tcapw_entry_change(tcapw, entry2.uid, &hpw2, flags, email);
	assert(!res);

	res = tcapw_entry_find_byname(tcapw, &uname2, &entry2);
	assert(!res);
	assert(!memcmp(&uname2, &entry2.uname, sizeof(uname2)));
	assert(!memcmp(&hpw2, &entry2.hpw, sizeof(hpw2)));

	/* Check that user3 is still there */
	res = tcapw_entry_find_byname(tcapw, &uname1, &entry3);
	assert(!res);

	{
		int i;
		assoctab_item_t *a;
		tcapw_entry_t *p;
		char namebuf[tcapw_LEN_USERNAME];
		
		DPRINT(("read %d users:\n", tcapw->tab->n_used)); 
		for (i = 0; i < tcapw->tab->n_used; i++) {
			a = assoctab_getkey(tcapw->tab, i);
			p = (tcapw_entry_t *)a->value;
			tcapw_ucs22ascii(tcapw, p->uname.uname, namebuf, tcapw_LEN_USERNAME);
			DPRINT(("  %d %s\n", p->uid, namebuf));
		}
	}
	
	DPRINT(("Saving DB\n"));
	res = tcapw_close(tcapw);
	assert(!res);
	tcapw_destroy(tcapw);

	DPRINT(("Zeroing data\n"));	
	/* Make sure the data made it to disk */
	memset(&entry1, 0, sizeof(entry1));
	memset(&entry2, 0, sizeof(entry1));
	memset(&entry3, 0, sizeof(entry1));
	memset(&entry1b, 0, sizeof(entry1));
	memset(&entry2b, 0, sizeof(entry1));

	tcapw = tcapw_create();
	assert(tcapw);
	DPRINT(("Opening DB\n"));
	res = tcapw_open(tcapw, "pw.dat");
	assert(!res);

	{
		int i;
		assoctab_item_t *a;
		tcapw_entry_t *p;
		char namebuf[tcapw_LEN_USERNAME];
		
		DPRINT(("read %d users:\n", tcapw->tab->n_used));
		for (i = 0; i < tcapw->tab->n_used; i++) {
			a = assoctab_getkey(tcapw->tab, i);
			p = (tcapw_entry_t *)a->value;
			tcapw_ucs22ascii(tcapw, p->uname.uname, namebuf, tcapw_LEN_USERNAME);
			DPRINT(("  %d %s\n", p->uid, namebuf));
		}
	}
		
	DPRINT(("Testing Data\n"));
	/* Make sure you can find both users by name, and that the entries match */
	res = tcapw_entry_find_byname(tcapw, &uname1, &entry1);
	assert(!res);
	assert(!memcmp(&uname1, &entry1.uname, sizeof(uname1)));
	res = tcapw_password_hash(tcapw, &pw1, &hpw1);
	assert(!res);
	assert(!memcmp(&hpw1, &entry1.hpw, sizeof(hpw1)));

	res = tcapw_entry_find_byname(tcapw, &uname2, &entry2);
	assert(!res);
	assert(!memcmp(&uname2, &entry2.uname, sizeof(uname2)));
	res = tcapw_password_hash(tcapw, &pw2, &hpw2);
	assert(!res);
	assert(!memcmp(&hpw2, &entry2.hpw, sizeof(hpw2)));

	/* Make sure their id's aren't the same */
	assert(entry1.uid != 0);
	assert(entry1.uid != entry2.uid);

	/* Make sure you can find both users by uid, and that the entries match */
	res = tcapw_entry_find_byid(tcapw, entry1.uid, &entry1b);
	assert(!res);
	assert(!memcmp(&uname1, &entry1b.uname, sizeof(uname1)));
	assert(!memcmp(&hpw1, &entry1b.hpw, sizeof(hpw1)));

	res = tcapw_entry_find_byid(tcapw, entry2.uid, &entry2b);
	assert(!res);
	assert(!memcmp(&uname2, &entry2b.uname, sizeof(uname2)));
	assert(!memcmp(&hpw2, &entry2b.hpw, sizeof(hpw2)));

	/* Make sure you can't find a bogus user */
	res = tcapw_ascii2ucs2(tcapw, "user1", uname3.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_entry_find_byname(tcapw, &uname3, &entry3);
	assert(res);

	{
		int i;
		assoctab_item_t *a;
		tcapw_entry_t *p;
		char namebuf[tcapw_LEN_USERNAME];
		
		DPRINT(("read %d users:\n", tcapw->tab->n_used)); 
		for (i = 0; i < tcapw->tab->n_used; i++) {
			a = assoctab_getkey(tcapw->tab, i);
			p = (tcapw_entry_t *)a->value;
			tcapw_ucs22ascii(tcapw, p->uname.uname, namebuf, tcapw_LEN_USERNAME);
			DPRINT(("  %d %s\n", p->uid, namebuf));
		}
	}
	
	res = tcapw_close(tcapw);
	assert(!res);
	tcapw_destroy(tcapw);
}

void test_tca()
{
	tca_t *tca, *tcac;
	tcapw_t *tcapw;
	dp_result_t res;
	tcapw_uid_t uid, uid2;
	tcapw_uname_t uname, unameb;
	tcapw_pw_t pw, pw2;
	tcapw_hpw_t hpw, hpw2;
	tca_challenge_t challenge, challenge2;
	tca_response_t response;
	tca_pwchange_t pwchange;
	tca_newuser_t newuser;
	int pktlen;
	char email[] = "user@host";
	int flags = 0;
	
	/* Assume a database named pw.dat has been created
	 * with user2/pass2b and user3/pass3.
	 */

	DPRINT(("Creating\n"));
	tca = tca_create();
	assert(tca);
	tcac = tca_create();
	assert(tcac);

	DPRINT(("Opening\n"));
	res = tca_openpw(tca, "pw.dat");
	assert(!res);
	tcapw = tca->tdb;
	assert(tcapw);

	{
		int i;
		assoctab_item_t *a;
		tcapw_entry_t *p;
		char namebuf[tcapw_LEN_USERNAME];
		
		DPRINT(("test_tca: read %d users:\n", tcapw->tab->n_used));
		for (i = 0; i < tcapw->tab->n_used; i++) {
			a = assoctab_getkey(tcapw->tab, i);
			p = (tcapw_entry_t *)a->value;
			tcapw_ucs22ascii(tcapw, p->uname.uname, namebuf, tcapw_LEN_USERNAME);
			DPRINT(("  %d %s\n", p->uid, namebuf));
		}
	}
	
	DPRINT(("Testing 1\n"));
	/* Try to log in user2.  Check the uid->uname while we're at it. */
	res = tcapw_ascii2ucs2(tcapw, "user2", uname.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass2b", pw.pw, tcapw_LEN_PW);
	assert(!res);
	uid2 = tca_uname2uid(tca, &uname);
	assert(uid2 != tcapw_UID_NONE);
	res = tca_uid2uname(tca, uid2, &unameb);
	assert(!res);
	assert(!memcmp(&uname, &unameb, sizeof(uname)));

	DPRINT(("Testing 1 - generating challenge\n"));	
	tca_challenge_generate(tca, &challenge);
	{
		int i;
		printf("challenge:");
		for (i=0; i<tca_LEN_CHALLENGE; i++)
			printf(" %2.2x", (unsigned char)challenge.challenge[i]);
		printf("\n");
	}
	res = tcapw_password_hash(tcac->tdb, &pw, &hpw);
	assert(!res);
	DPRINT(("Testing 1 - generating response\n"));
	res = tca_response_generate(tcac, &challenge, &uname, &hpw, &response, &pktlen);
	assert(!res);
	DPRINT(("Testing 1 - validating response\n"));
	res = tca_response_validate(tca, &challenge, &response, &uid);
	assert(!res);

	DPRINT(("Testing 2 - logging in user3\n"));
	/* Try to log in user3. */
	res = tcapw_ascii2ucs2(tcapw, "user3", uname.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass3", pw.pw, tcapw_LEN_PW);
	assert(!res);
	uid = tca_uname2uid(tca, &uname);
	assert(uid != tcapw_UID_NONE);
	res = tca_uid2uname(tca, uid, &unameb);
	assert(!res);
	assert(!memcmp(&uname, &unameb, sizeof(uname)));

	tca_challenge_generate(tca, &challenge);
	res = tcapw_password_hash(tcac->tdb, &pw, &hpw);
	assert(!res);
	res = tca_response_generate(tcac, &challenge, &uname, &hpw, &response, &pktlen);
	assert(!res);
	res = tca_response_validate(tca, &challenge, &response, &uid);
	assert(!res);

	DPRINT(("Testing 3 - logging in user2 w/ wrong password\n"));
	/* Try to log in user 2 with the wrong password. */
	res = tcapw_ascii2ucs2(tcapw, "user2", uname.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass2", pw.pw, tcapw_LEN_PW);
	assert(!res);
	uid = tca_uname2uid(tca, &uname);

	tca_challenge_generate(tca, &challenge);
	res = tcapw_password_hash(tcac->tdb, &pw, &hpw);
	assert(!res);
	tca_response_generate(tcac, &challenge, &uname, &hpw, &response, &pktlen);
	assert(!res);
	res = tca_response_validate(tca, &challenge, &response, &uid);
	assert(res);

	DPRINT(("Testing 4 - checking challenge generation\n"));
	/* Check that the challenges are not the same. */
	tca_challenge_generate(tca, &challenge2);
	assert(memcmp(&challenge, &challenge2, sizeof(challenge)));

	/* Try to reply to the wrong challenge */
	tca_challenge_generate(tca, &challenge);
	res = tca_response_generate(tcac, &challenge2, &uname, &hpw, &response, &pktlen);
	assert(!res);
	res = tca_response_validate(tca, &challenge, &response, &uid);
	assert(res);

	/* Try to change password, correctly */
	res = tcapw_ascii2ucs2(tcapw, "pass2b", pw.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass2c", pw2.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw, &hpw);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw2, &hpw2);
	assert(!res);
	tca_challenge_generate(tca, &challenge);
	res = tca_pwchange_generate(tcac, &challenge, &hpw, &hpw2, flags, email, &pwchange, &pktlen);
	assert(!res);
	res = tca_pwchange_validate(tca, uid2, &challenge, &pwchange);
	printf("pwchange_validate res = %d\n", res);
	assert(!res);
	
	/* Try to change password, with the wrong password */
	res = tcapw_ascii2ucs2(tcapw, "pass2d", pw.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass2e", pw2.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw, &hpw);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw2, &hpw2);
	assert(!res);
	tca_challenge_generate(tca, &challenge);
	res = tca_pwchange_generate(tcac, &challenge, &hpw, &hpw2, flags, email, &pwchange, &pktlen);
	assert(!res);
	res = tca_pwchange_validate(tca, uid, &challenge, &pwchange);
	assert(res);

	/* Try to change password, with the wrong challenge */
	res = tcapw_ascii2ucs2(tcapw, "pass2c", pw.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass2e", pw2.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw, &hpw);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw2, &hpw2);
	assert(!res);
	tca_challenge_generate(tca, &challenge);
	tca_challenge_generate(tca, &challenge2);
	tca_pwchange_generate(tcac, &challenge, &hpw, &hpw2, flags, email, &pwchange, &pktlen);
	res = tca_pwchange_validate(tca, uid, &challenge2, &pwchange);
	assert(res);

	/* create a new user, using the newuser system: user4/pass4 */
	res = tcapw_ascii2ucs2(tcapw, "user4", uname.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass4", pw.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcapw, &pw, &hpw);
	assert(!res);
	tca_challenge_generate(tca, &challenge);
	res = tca_newuser_generate(tcac, &challenge, &uname, &hpw, flags, email, &newuser, &pktlen);
	assert(!res);
	res = tca_newuser_validate(tca, &challenge, &newuser, &uid);
	assert(!res);

	/* check that user4 exists and can log in */
	uid2 = tca_uname2uid(tca, &uname);
	assert(uid2 != tcapw_UID_NONE);
	tca_challenge_generate(tca, &challenge);
	res = tca_response_generate(tcac, &challenge, &uname, &hpw, &response, &pktlen);
	assert(!res);
	res = tca_response_validate(tca, &challenge, &response, &uid);
	assert(!res);
	assert(uid == uid2);

	/* attempt to create a new user with an incorrect challenge */
	res = tcapw_ascii2ucs2(tcapw, "user5", unameb.uname, tcapw_LEN_USERNAME);
	assert(!res);
	res = tcapw_ascii2ucs2(tcapw, "pass5", pw2.pw, tcapw_LEN_PW);
	assert(!res);
	res = tcapw_password_hash(tcac->tdb, &pw2, &hpw2);
	assert(!res);
	tca_challenge_generate(tca, &challenge2);
	res = tca_newuser_generate(tcac, &challenge, &unameb, &hpw2, flags, email, &newuser, &pktlen);
	assert(!res);
	res = tca_newuser_validate(tca, &challenge2, &newuser, &uid);
	assert(res);

	DPRINT(("Added user4, failed (correctly) to add user5\n"));
	{
		int i;
		assoctab_item_t *a;
		tcapw_entry_t *p;
		char namebuf[tcapw_LEN_USERNAME];
		
		DPRINT(("test_tca: read %d users:\n", tcapw->tab->n_used));
		for (i = 0; i < tcapw->tab->n_used; i++) {
			a = assoctab_getkey(tcapw->tab, i);
			p = (tcapw_entry_t *)a->value;
			tcapw_ucs22ascii(tcapw, p->uname.uname, namebuf, tcapw_LEN_USERNAME);
			DPRINT(("  %d %s\n", p->uid, namebuf));
		}
	}
	/* delete user4, just to clean up */
	res = tcapw_entry_delete(tca->tdb, uid2);
	printf("entry_delete %d, res = %d\n", uid2, res);
	assert(!res);
	
	tca_destroy(tca);
	tca_destroy(tcac);
}


#include "ptimer.h"
#define NUSERS 1000
#define NTRIALS 1000

void test_tca_speed()
{
	int i;
	dp_result_t err;
	tca_t *tca, *tcac;
	char name[tcapw_LEN_USERNAME];
	char pass[tcapw_LEN_PW];
	tcapw_uname_t uname, uname2;
	tcapw_pw_t pw;
	tcapw_hpw_t hpw;
	tcapw_uid_t uid;
	tca_challenge_t challenge;
	tca_response_t response;
	int pktlen;
	
	tca = tca_create();
	tcac = tca_create();
	err = tca_openpw(tca, "pw.dat");

	/* add NUSERS users into the database */
	DPRINT(("Adding %d users\n", NUSERS));
	for (i = 1; i <= NUSERS; i++) {
		sprintf(name, "%duser\0", i);
		sprintf(pass, "%dpass\0", i);
		err = tcapw_ascii2ucs2(tca->tdb, name, uname.uname, tcapw_LEN_USERNAME);
		assert(!err);
		err = tcapw_ascii2ucs2(tca->tdb, pass, pw.pw, tcapw_LEN_PW);
		assert(!err);
		err = tcapw_password_hash(tca->tdb, &pw, &hpw);
		assert(!err);
		err = tcapw_entry_create(tca->tdb, &uname, &hpw, 0, "", &uid);
		assert(!err);
	}

	ptimer_disableInterrupt();
	ptimer_zeroAll();
	
	for (i = 1; i <= NTRIALS; i++) {
		DPRINT(("T%4.4d:", i));
		err = tcapw_ascii2ucs2(tca->tdb, "1user", uname.uname, tcapw_LEN_USERNAME);
		assert(!err);
		/* look up someone added first by uname */
		DPRINT((" 1un"));
		ptimer_Enter(2, "find first uname");
		uid = tca_uname2uid(tca, &uname);
		ptimer_Exit(2, 1);
		assert(uid != tcapw_UID_NONE);
		/* look up someone added first by uid */
		DPRINT((" 1id"));
		ptimer_Enter(3, "find first uid");
		err = tca_uid2uname(tca, uid, &uname2);
		ptimer_Exit(3, 1);
		assert(!err);

		sprintf(name, "%duser\0", NUSERS/2);
		sprintf(pass, "%dpass\0", NUSERS/2);
		err = tcapw_ascii2ucs2(tca->tdb, name, uname.uname, tcapw_LEN_USERNAME);
		assert(!err);
		err = tcapw_ascii2ucs2(tca->tdb, pass, pw.pw, tcapw_LEN_PW);
		assert(!err);
		/* look up someone in the middle by uname */
		DPRINT((" 2un"));
		ptimer_Enter(4, "find middle uname");
		uid = tca_uname2uid(tca, &uname);
		ptimer_Exit(4, 1);
		assert(uid != tcapw_UID_NONE);
		/* look up someone in the middle by uid */
		DPRINT((" 2id"));
		ptimer_Enter(5, "find middle uid");
		err = tca_uid2uname(tca, uid, &uname2);
		ptimer_Exit(5, 1);
		assert(!err);
		
		/* generate a challenge for middle uid */
		DPRINT((" ch"));
		ptimer_Enter(10, "generate challenge");
		err = tca_challenge_generate(tca, &challenge);
		ptimer_Exit(10, 1);
		assert(!err);
		/* generate a response */
		DPRINT((" rsp"));
		err = tcapw_password_hash(tcac->tdb, &pw, &hpw);
		assert(!err);
		ptimer_Enter(11, "generate response");
		err = tca_response_generate(tcac, &challenge, &uname, &hpw, &response, &pktlen);
		ptimer_Exit(11, 1);
		assert(!err);
		/* validate the response */
		DPRINT((" val"));
		ptimer_Enter(12, "validate response");
		err = tca_response_validate(tca, &challenge, &response, &uid);
		ptimer_Exit(12, 1);
		assert(!err);
		
		err = tcapw_ascii2ucs2(tca->tdb, "bogus", uname.uname, tcapw_LEN_USERNAME);
		assert(!err);
		/* look up someone by a bogus uname */
		DPRINT((" 0un"));
		ptimer_Enter(6, "find bogus uname");
		uid = tca_uname2uid(tca, &uname);
		ptimer_Exit(6, 1);
		assert(uid == tcapw_UID_NONE);
		/* look up someone by a bogus uid */
		DPRINT((" 0id"));
		ptimer_Enter(7, "find bogus uid");
		err = tca_uid2uname(tca, 1, &uname2);
		ptimer_Exit(7, 1);
		assert(err == dp_RES_EMPTY);

		sprintf(name, "%duser\0", NUSERS+1);
		sprintf(pass, "%dpass\0", NUSERS+1);
		err = tcapw_ascii2ucs2(tca->tdb, name, uname.uname, tcapw_LEN_USERNAME);
		assert(!err);
		err = tcapw_ascii2ucs2(tca->tdb, pass, pw.pw, tcapw_LEN_PW);
		assert(!err);
		err = tcapw_password_hash(tca->tdb, &pw, &hpw);
		assert(!err);
		/* create someone new */
		DPRINT((" cre"));
		ptimer_Enter(13, "create new user");
		err = tcapw_entry_create(tca->tdb, &uname, &hpw, 0, "", &uid);
		assert(uid != tcapw_UID_NONE);
		ptimer_Exit(13, 1);
		assert(!err);
		/* look up someone added last by uname */
		DPRINT((" 3un"));
		ptimer_Enter(8, "find last uname");
		uid = tca_uname2uid(tca, &uname);
		ptimer_Exit(8, 1);
		assert(uid != tcapw_UID_NONE);
		/* look up someone added last by uid */
		DPRINT((" 3id"));
		ptimer_Enter(9, "find last uid");
		err = tca_uid2uname(tca, uid, &uname2);
		ptimer_Exit(9, 1);
		assert(!err);
		/* delete them */
		DPRINT((" del\n"));
		ptimer_Enter(14, "delete last user");
		err = tcapw_entry_delete(tca->tdb, uid);
		ptimer_Exit(14, 1);
		assert(!err);
	}
	DPRINT(("saving tca to disk\n"));
	ptimer_Enter(15, "tca_destroy");
	err = tca_destroy(tca);
	ptimer_Exit(15, 1);
	assert(!err);

	DPRINT(("reading tca from disk\n"));
	ptimer_Enter(16, "tca_openpw");		
	tca = tca_create();
	err = tca_openpw(tca, "pw.dat");
	ptimer_Exit(16, 1);
	assert(!err);

	DPRINT(("logging speed test results to tca_speed.log\n"));
	ptimer_enableInterrupt();
	ptimer_dumpToFile("tca_speed.log");

	/* delete all the entries so it can run again */
	DPRINT(("deleting test users\n"));
	for (i = NUSERS; i >= 1; i--) {
		DPRINT(("sprintf"));
		sprintf(name, "%duser\0", i);
		DPRINT((" ascii2ucs2"));
		err = tcapw_ascii2ucs2(tca->tdb, name, uname.uname, tcapw_LEN_USERNAME);
		assert(!err);
		DPRINT((" uname2uid"));
		uid = tca_uname2uid(tca, &uname);
		DPRINT((" entry_delete\n"));
		err = tcapw_entry_delete(tca->tdb, uid);
		assert(!err);
	}
	err = tca_destroy(tca);
	assert(!err);
}


int main()
{
	test_tcapw();
	test_tca();
	test_tca_speed();
	printf("No test failed, tca_speed.log contains %d user speed test results.\n", NUSERS);
	return 0;
}



