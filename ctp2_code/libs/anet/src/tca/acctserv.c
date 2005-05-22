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
 An interface to the password database for use with a CGI script or similar
 form of access.
--------------------------------------------------------------------------*/

#include <time.h>
#include <string.h>
#include "acctserv.h"
#include "mywcs.h"

#ifdef acctserv_DAEMON
#if defined(_DEBUG) || defined(DEBUG) || defined(DPRNT)
#define DPRINT(a) printf a
#else
#define DPRINT(a)
#endif
#endif

/*--------------------------------------------------------------------------
 Should mirror tserv_check_email!
 Returns dp_RES_OK if the email address passed to it looks like an email
 address (i.e. something@something.something), dp_RES_BAD if not.
--------------------------------------------------------------------------*/
static dp_result_t acctserv_check_email(const char *email)
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
 Send via sendmail to the address <to> with subject <subj> and body <body>.
--------------------------------------------------------------------------*/
static void sendMailToSubjBody(const char *to, const char *subj,
	const char *body)
{
	FILE *fp = popen("/usr/sbin/sendmail -t", "w");
	fprintf(fp, "To: %s\n", to);
	fprintf(fp, "Subject: %s\n", subj);
	fprintf(fp, "\n%s\n", body);
	fclose(fp);
}

/*--------------------------------------------------------------------------
 Send an email containing the validation code to this email address.
--------------------------------------------------------------------------*/
static void send_validation_email(const char *email,
	const dp_netchar_t *validationcode)
{
	char body[256];
	wchar_t wsecretcode[tcapw_LEN_PW];
	char asecretcode[tcapw_LEN_PW];
	
	mywcs_netchar2wchar(wsecretcode, validationcode, tcapw_LEN_PW);
	mywcs_tombs(asecretcode, tcapw_LEN_PW, wsecretcode);
	
	sprintf(body, "Welcome to ActiveLink!\nYour activation code is %s\n",
		asecretcode);
	sendMailToSubjBody(email, "ActiveLink Account Activation", body);
	DPRINT(("acctserv: emailed code:%s to %s\n", validationcode, email));
}

/*--------------------------------------------------------------------------
 Check a username/password.  Return the entry if password is correct.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if password is incorrect,
		 dp_RES_EMPTY if username doesn't exist.
--------------------------------------------------------------------------*/
static dp_result_t check_password(tcapw_t *tcapw, const char *username,
	const char *password, tcapw_entry_t *pentry)
{
	dp_result_t err;
	wchar_t wuname[tcapw_LEN_USERNAME];
	tcapw_uname_t uname;
	wchar_t wpw[tcapw_LEN_PW];
	tcapw_pw_t pw;
	tcapw_hpw_t hpw;
	int len;

	if (tcapw == NULL || username == NULL || password == NULL)
		return dp_RES_BAD;

	if (mywcs_frommbs(wuname, tcapw_LEN_USERNAME, username) == -1) {
		DPRINT(("check_password: invalid char in username:%s\n", username));
		return dp_RES_BADSIZE;
	}
	len = mywcs_wchar2netchar(uname.uname, wuname, tcapw_LEN_USERNAME);
	if (len < 1) {
		DPRINT(("check_password: username:%s is too short\n",
			tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME)));
		return dp_RES_BADSIZE;
	}
	err = tcapw_entry_find_byname(tcapw, &uname, pentry);
	if (err == dp_RES_EMPTY) {
		DPRINT(("check_password: username:%s does not exist\n",
			tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME)));
		return dp_RES_EMPTY;
	} else if (err != dp_RES_OK) {
		DPRINT(("check_password: find_byname(%s) returns err:%d\n",
			tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME), err));
		return dp_RES_BUG;
	}

	if (mywcs_frommbs(wpw, tcapw_LEN_PW, password) == -1) {
		DPRINT(("check_password: invalid char in password:%s\n", password));
		return dp_RES_BADSIZE;
	}
	len = mywcs_wchar2netchar(pw.pw, wpw, tcapw_LEN_PW);
	if (len < 8) {
		DPRINT(("check_password: password:%s is too short\n",
			tcapw_u2ascii(pw.pw, tcapw_LEN_PW)));
		memset(pentry, 0, sizeof(tcapw_entry_t));
		return dp_RES_ACCESS;
	}
	err = tcapw_password_hash(tcapw, &pw, &hpw);
	if (err != dp_RES_OK) {
		DPRINT(("check_password: hash(%s) returns err:%d\n",
			tcapw_u2ascii(pw.pw, tcapw_LEN_PW), err));
		memset(pentry, 0, sizeof(tcapw_entry_t));
		return dp_RES_BUG;
	}
	if (memcmp(&hpw, &pentry->hpw, sizeof(tcapw_hpw_t))) {
		DPRINT(("check_password: incorrect password\n"));
		memset(pentry, 0, sizeof(tcapw_entry_t));
		return dp_RES_ACCESS;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Instantiate acctserv using the message queue pwq and the associated
 password database it contains.
--------------------------------------------------------------------------*/
acctserv_t *acctserv_create(pwq_t *pwq)
{
	dp_result_t err;
	acctserv_t *acctserv;
	time_t timestamp;
	long offset;

	if (pwq == NULL || pwq->tcapw == NULL || pwq->wmq == NULL
	||  !pwq->wmq->openForWrite)
		return NULL;
	acctserv = (acctserv_t *)malloc(sizeof(acctserv_t));
	acctserv->pwq = pwq;
	acctserv->tcapw = pwq->tcapw;

	return acctserv;
}

/*--------------------------------------------------------------------------
 Clean up an instance of acctserv.  Does not destroy pwq.
--------------------------------------------------------------------------*/
void acctserv_destroy(acctserv_t *acctserv)
{
	if (acctserv == NULL)
		return;
	memset(acctserv, 0, sizeof(acctserv_t));
	free(acctserv);
}

/*--------------------------------------------------------------------------
 Given an ascii username and password and an email address, add a new
 account to the password database and, on success, email the user a
 validation code and add the new entry to the wmq.
 Returns dp_RES_OK on success,
		 dp_RES_ALREADY if the username is in use,
		 dp_RES_BADSIZE if the username or password is unacceptable.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_create(acctserv_t *acctserv,
	const char *username, const char *password, const char *email)
{
	dp_result_t err;
	wchar_t wuname[tcapw_LEN_USERNAME];
	tcapw_uname_t uname;
	wchar_t wpw[tcapw_LEN_PW];
	tcapw_pw_t pw;
	tcapw_hpw_t hpw;
	char myemail[tcapw_MAXLEN_EMAIL];
	dp_netchar_t secretcode[tcapw_LEN_PW];
	dp_uid_t uid;
	pwq_message_t msg;
	int len;

	if (acctserv == NULL || username == NULL || password == NULL
	||  email == NULL)
		return dp_RES_BAD;

	if (mywcs_frommbs(wuname, tcapw_LEN_USERNAME, username) == -1) {
		DPRINT(("check_password: invalid char in username:%s\n", username));
		return dp_RES_BADSIZE;
	}
	len = mywcs_wchar2netchar(uname.uname, wuname, tcapw_LEN_USERNAME);
	if (len < 1) {
		DPRINT(("acctserv_account_create: username:%s is too short\n", tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME)));
		return dp_RES_BADSIZE;
	}
	if (mywcs_frommbs(wpw, tcapw_LEN_PW, password) == -1) {
		DPRINT(("check_password: invalid char in password:%s\n", password));
		return dp_RES_BADSIZE;
	}
	len = mywcs_wchar2netchar(pw.pw, wpw, tcapw_LEN_PW);
	if (len < 8) {
		DPRINT(("acctserv_account_create: password:%s is too short\n", tcapw_u2ascii(pw.pw, tcapw_LEN_PW)));
		return dp_RES_BADSIZE;
	}
	err = tcapw_password_hash(acctserv->tcapw, &pw, &hpw);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_create: hash(%s) returns err:%d\n", tcapw_u2ascii(pw.pw, tcapw_LEN_PW), err));
		return dp_RES_BUG;
	}
	strncpy(myemail, email, tcapw_MAXLEN_EMAIL);
	myemail[tcapw_MAXLEN_EMAIL-1] = '\0';
	if (!acctserv_check_email(myemail)) {
		DPRINT(("acctserv_account_create: email:%s is invalid\n", myemail));
		return dp_RES_BADSIZE;
	}

	err = tcapw_entry_create(acctserv->tcapw, &uname, &hpw, 0,
		email, &uid);
	if (err == dp_RES_ALREADY) {
		DPRINT(("acctserv_account_create: username:%s is in use\n",
			tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME)));
		return dp_RES_ALREADY;
	} else if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_create: create(%s) returns err:%d\n",
			tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME), err));
		return dp_RES_BUG;
	}

	err = tcapw_entry_find_byid(acctserv->tcapw, uid, &msg.u.acctCreate);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_create: find_byid(%d) returns err:%d\n",
			uid, err));
		return dp_RES_BUG;
	}
	err = tcapw_get_secretcode(&msg.u.acctCreate, secretcode);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_create: get_secretcode(%d) returns err:%d\n",
			uid, err));
		return dp_RES_BUG;
	}
	send_validation_email(msg.u.acctCreate.email, secretcode);
	msg.type = pwq_MESSAGETYPE_CREATE;
	err = pwq_put(acctserv->pwq, time(NULL), &msg);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_create: pwq_put(%d) err:%d\n",
			msg.u.acctCreate.uid, err));
		return dp_RES_BUG;
	}
	DPRINT(("acctserv_account_create: created uid:%d for uname:%s\n",
		uid, tcapw_u2ascii(uname.uname, tcapw_LEN_USERNAME)));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Given a previously created username and the correct validation code,
 activates an account.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password or code is incorrect,
		 dp_RES_ALREADY if the account was already activated,
		 dp_RES_EMPTY if the account hasn't been created yet.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_activate(acctserv_t *acctserv,
	const char *username, const char *password, const char *validationcode)
{
	dp_result_t err;
	char avalidationcode[tcapw_LEN_PW];
	dp_netchar_t secretcode[tcapw_LEN_PW];
	wchar_t wsecretcode[tcapw_LEN_PW];
	char asecretcode[tcapw_LEN_PW];
	pwq_message_t msg;

	if (acctserv == NULL || username == NULL || password == NULL
	||  validationcode == NULL)
		return dp_RES_BAD;

	err = check_password(acctserv->tcapw, username, password, &msg.u.acctChange);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_activate: check_password err:%d\n", err));
		return err;
	}
	if (msg.u.acctChange.flags & tcapw_entry_FLAGS_EMAIL_VALID) {
		DPRINT(("acctserv_account_activate: uid:%d already activated\n",
			msg.u.acctChange.uid));
		return dp_RES_ALREADY;
	}

	strncpy(avalidationcode, validationcode, tcapw_LEN_PW);
	avalidationcode[tcapw_LEN_PW-1] = '\0';
	if (strlen(avalidationcode) < 1) {
		DPRINT(("acctserv_account_activate: code:%s is too short\n",
			avalidationcode));
		return dp_RES_BADSIZE;
	}
	err = tcapw_get_secretcode(&msg.u.acctChange, secretcode);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_activate: getsecretcode(%d) returns err:%d\n",
			msg.u.acctChange.uid, err));
		return dp_RES_BUG;
	}
	mywcs_netchar2wchar(wsecretcode, secretcode, tcapw_LEN_PW);
	mywcs_tombs(asecretcode, tcapw_LEN_PW, wsecretcode);
	if (strncmp(asecretcode, avalidationcode, tcapw_LEN_PW)) {
		DPRINT(("acctserv_account_activate: incorrect code:%s != %s\n",
			avalidationcode, asecretcode));
		return dp_RES_ACCESS;
	}

	err = tcapw_entry_change(acctserv->tcapw, msg.u.acctChange.uid, NULL,
		(msg.u.acctChange.flags | tcapw_entry_FLAGS_EMAIL_VALID), NULL);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_activate: entry_change(%d) err:%d\n",
			msg.u.acctChange.uid, err));
		return dp_RES_BUG;
	}
	msg.type = pwq_MESSAGETYPE_CHANGE;
	err = pwq_put(acctserv->pwq, time(NULL), &msg);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_activate: pwq_put(%d) err:%d\n",
			msg.u.acctChange.uid, err));
		return dp_RES_BUG;
	}
	DPRINT(("acctserv_account_activate: activated uid:%d email:%s\n",
		msg.u.acctChange.uid, msg.u.acctChange.email));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Check the password for username.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password is incorrect,
		 dp_RES_NOTYET if the account has not been activated yet.
--------------------------------------------------------------------------*/
dp_result_t acctserv_check_password(acctserv_t *acctserv,
	const char *username, const char *password)
{
	dp_result_t err;
	tcapw_entry_t entry;

	if (acctserv == NULL || username == NULL || password == NULL)
		return dp_RES_BAD;

	err = check_password(acctserv->tcapw, username, password, &entry);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_check_password: check_password err:%d\n", err));
		return err;
	}

	if (!(entry.flags & tcapw_entry_FLAGS_EMAIL_VALID)) {
		DPRINT(("acctserv_check_password: uid:%d is inactive\n", entry.uid));
		return dp_RES_NOTYET;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Change a user's password or email.  If newpassword or newemail is NULL,
 leave that field alone.  If email is changed, send a validation code to
 the new email and deactivate the account for future reactivation.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password is incorrect.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_change(acctserv_t *acctserv,
	const char *username, const char *password,
	const char *newpassword, const char *newemail)
{
	dp_result_t err;
	wchar_t wnewpw[tcapw_LEN_PW];
	tcapw_pw_t newpw;
	tcapw_hpw_t newhpw, *pnewhpw;
	char mynewemail[tcapw_MAXLEN_EMAIL], *pnewemail;
	int newflags;
	dp_netchar_t secretcode[tcapw_LEN_PW];
	pwq_message_t msg;

	if (acctserv == NULL || username == NULL || password == NULL
	||  (newpassword == NULL && newemail == NULL))
		return dp_RES_BAD;

	err = check_password(acctserv->tcapw, username, password, &msg.u.acctChange);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_change: check_password err:%d\n", err));
		return err;
	}

	if (newpassword == NULL) {
		pnewhpw = NULL;
	} else {
		int len;

		if (mywcs_frommbs(wnewpw, tcapw_LEN_PW, newpassword) == -1) {
			DPRINT(("acctserv_account_change: invalid char in password:%s\n", newpassword));
			return dp_RES_BADSIZE;
		}
		len = mywcs_wchar2netchar(newpw.pw, wnewpw, tcapw_LEN_PW);
		if (len < 8) {
			DPRINT(("acctserv_account_change: password:%s is too short\n", tcapw_u2ascii(newpw.pw, tcapw_LEN_PW)));
			return dp_RES_BADSIZE;
		}
		err = tcapw_password_hash(acctserv->tcapw, &newpw, &newhpw);
		if (err != dp_RES_OK) {
			DPRINT(("acctserv_account_change: hash(%s) returns err:%d\n", tcapw_u2ascii(newpw.pw, tcapw_LEN_PW), err));
			return dp_RES_BUG;
		}
		pnewhpw = &newhpw;
	}
	if (newemail == NULL) {
		pnewemail = NULL;
		newflags = msg.u.acctChange.flags;
	} else {
		strncpy(mynewemail, newemail, tcapw_MAXLEN_EMAIL);
		mynewemail[tcapw_MAXLEN_EMAIL-1] = '\0';
		if (!acctserv_check_email(mynewemail)) {
			DPRINT(("acctserv_account_change: email:%s is invalid\n",
				mynewemail));
			return dp_RES_BADSIZE;
		}
		pnewemail = mynewemail;
		newflags = msg.u.acctChange.flags & ~tcapw_entry_FLAGS_EMAIL_VALID;
	}

	err = tcapw_entry_change(acctserv->tcapw, msg.u.acctChange.uid,
		pnewhpw, newflags, pnewemail);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_change: entry_change(%d) err:%d\n",
			msg.u.acctChange.uid, err));
		return dp_RES_BUG;
	}
	msg.type = pwq_MESSAGETYPE_CHANGE;
	err = pwq_put(acctserv->pwq, time(NULL), &msg);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_change: pwq_put(%d) err:%d\n",
			msg.u.acctChange.uid, err));
		return dp_RES_BUG;
	}
	if (!(newflags & tcapw_entry_FLAGS_EMAIL_VALID)) {
		err = tcapw_get_secretcode(&msg.u.acctChange, secretcode);
		if (err != dp_RES_OK) {
			DPRINT(("acctserv_account_change: get_secretcode(%d) err:%d\n",
				msg.u.acctChange.uid, err));
			return dp_RES_BUG;
		}
	}
	send_validation_email(msg.u.acctChange.email, secretcode);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Delete a user's account.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password is incorrect.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_delete(acctserv_t *acctserv,
	const char *username, const char *password)
{
	dp_result_t err;
	tcapw_entry_t entry;
	pwq_message_t msg;

	if (acctserv == NULL || username == NULL || password == NULL)
		return dp_RES_BAD;

	err = check_password(acctserv->tcapw, username, password, &entry);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_delete: check_password err:%d\n", err));
		return err;
	}

	err = tcapw_entry_delete(acctserv->tcapw, entry.uid);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_delete: delete(%d) returns err:%d\n", 
			entry.uid, err));
		return dp_RES_BUG;
	}
	msg.type = pwq_MESSAGETYPE_DELETE;
	msg.u.acctDelete.uid = entry.uid;
	err = pwq_put(acctserv->pwq, time(NULL), &msg);
	if (err != dp_RES_OK) {
		DPRINT(("acctserv_account_delete: pwq_put(%d) err:%d\n",
			msg.u.acctChange.uid, err));
		return dp_RES_BUG;
	}
	return dp_RES_OK;
}


#ifdef acctserv_DAEMON

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/errno.h>

#define acctserv_SOCKETNAME "acctserv.so"
#define BUFLEN		256
#define MAX_ARGS	5
#define TIME_SAVE	600	/* secs between saves to disk */

#define STATE_READREQUEST 1
#define STATE_SENDRESPONSE 2

#if defined(_DEBUG) || defined(DEBUG) || defined(DPRNT)
int cdecl
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
{
#include <stdarg.h>
	va_list argptr = NULL;
	int len = 0;

	if (__format) {
		va_start(argptr, __format);
		len = vprintf(__format, argptr);
		va_end(argptr);
	}

	return len;
}

dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt) 
{
	printf("dpReportAssertionFailure(%d, %s, %s)\n",
			lineno, file, linetxt);
	exit(1);
	return dp_RES_OK;
}
#endif

static void acctserv_response(int sockfd, int errcode, char *msg)
{
	char resp[BUFLEN];
	int nbytes;

	sprintf(resp, "%d %s\n", errcode, msg); 
	nbytes = write(sockfd, resp, strlen(resp));
	if (nbytes < strlen(resp))
		DPRINT(("acctserv: write error:%d\n", errno));
	return;
}

static void acctserv_handleRequest(acctserv_t *acctserv, int sockfd)
{
	dp_result_t err;
	char buf[BUFLEN];
	int nbytes;
	char *pbuf;
	char *argv[MAX_ARGS];
	int argc = 0;
	int state;
	int bNewArg = 1;
	time_t now;

	/* wait for a new request */
	now = time(NULL);
	pbuf = buf;
	state = STATE_READREQUEST;
	while (1) {
		char *c;

		now = time(NULL);
		switch (state) {
		case STATE_READREQUEST:
			/* blocking read; wait for data */
			nbytes = read(sockfd, pbuf, BUFLEN-(pbuf-buf));
			if (nbytes < 0) {
				DPRINT(("acctserv: read error:%d\n", errno));
				return;  /* read error, give up */
			} else if (nbytes == 0)
				continue;  /* no data, wait */

			for (c = pbuf; c < pbuf + nbytes; c++) {
				if (c > buf + BUFLEN) {
					DPRINT(("acctserv: header too long for buf\n"));
					acctserv_response(sockfd, 4, "Buffer Overflow"); 
					return;
				}
				if (*c == ' ' || *c == '\t') {
					*c = '\0';
					bNewArg = 1;
				} else if (bNewArg) {
					argv[argc++] = c;
					bNewArg = 0;
				}
				if (!isprint(*c) && (*c != '\t') && (*c != '\n')
				&&  (*c != '\r')) {
					DPRINT(("acctserv: bad char:%d in request\n", *c));
					acctserv_response(sockfd, 3, "Bad Character in Request"); 
					return;
				}
				if (*c == '\r')
					*c = '\0';  /* \r ends a line but doesn't start a new one */
				if (*c != '\n')
					continue;

				/* newline triggers processing of a line */
				*c = '\0';
				state = STATE_SENDRESPONSE;
				break;
			}
			pbuf = c;
			break;

		case STATE_SENDRESPONSE:
			switch (buf[0]) {
			case 'N':	/* N(ew-account) username password email */
				if (argc != 4)
					acctserv_response(sockfd, 3, "Invalid Number of Arguments");
				err = acctserv_account_create(acctserv, argv[1], argv[2],
					argv[3]);
				if (err == dp_RES_OK)
					acctserv_response(sockfd, 1, "Account Created, Please Activate");
				else if (err == dp_RES_ALREADY)
					acctserv_response(sockfd, 2, "Username In Use");
				else if (err == dp_RES_BADSIZE)
					acctserv_response(sockfd, 3, "Invalid Argument");
				else
					acctserv_response(sockfd, 4, "Server Error");
				break;
			case 'A':	/* A(ctivate-account) username password code */
				if (argc != 4)
					acctserv_response(sockfd, 3, "Invalid Number of Arguments");
				err = acctserv_account_activate(acctserv, argv[1], argv[2],
					argv[3]);
				if (err == dp_RES_OK || err == dp_RES_ALREADY)
					acctserv_response(sockfd, 0, "Account Activated");
				else if (err == dp_RES_ACCESS)
					acctserv_response(sockfd, 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					acctserv_response(sockfd, 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					acctserv_response(sockfd, 3, "Invalid Argument");
				else
					acctserv_response(sockfd, 4, "Server Error");
				break;
			case 'D':	/* D(elete-account) username password */
				if (argc != 3)
					acctserv_response(sockfd, 3, "Invalid Number of Arguments");
				err = acctserv_account_delete(acctserv, argv[1], argv[2]);
				if (err == dp_RES_OK)
					acctserv_response(sockfd, 0, "Account Deleted");
				else if (err == dp_RES_ACCESS)
					acctserv_response(sockfd, 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					acctserv_response(sockfd, 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					acctserv_response(sockfd, 3, "Invalid Argument");
				else
					acctserv_response(sockfd, 4, "Server Error");
				break;
			case 'P':	/* P(assword-change) username password newpassword */
				if (argc != 4)
					acctserv_response(sockfd, 3, "Invalid Number of Arguments");
				err = acctserv_account_change(acctserv, argv[1], argv[2],
					argv[3], NULL);
				if (err == dp_RES_OK)
					acctserv_response(sockfd, 0, "Password Changed");
				else if (err == dp_RES_ACCESS)
					acctserv_response(sockfd, 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					acctserv_response(sockfd, 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					acctserv_response(sockfd, 3, "Invalid Argument");
				else
					acctserv_response(sockfd, 4, "Server Error");
				break;
			case 'E':	/* E(mail-change) username password newemail */
				if (argc != 4)
					acctserv_response(sockfd, 3, "Invalid Number of Arguments");
				err = acctserv_account_change(acctserv, argv[1], argv[2],
					NULL, argv[3]);
				if (err == dp_RES_OK)
					acctserv_response(sockfd, 1, "Email Changed, Please Activate");
				else if (err == dp_RES_ACCESS)
					acctserv_response(sockfd, 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					acctserv_response(sockfd, 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					acctserv_response(sockfd, 3, "Invalid Argument");
				else
					acctserv_response(sockfd, 4, "Server Error");
				break;
			default:
				if (isprint(buf[0])) {
					DPRINT(("acctserv: bad command:%c in request\n", buf[0]));
				} else {
					DPRINT(("acctserv: bad command:0x%2x in request\n",buf[0]));
				}
				acctserv_response(sockfd, 3, "Bad Command");
				return;
			}
			return;
			break;

		default:
			DPRINT(("acctserv: bad state:%d\n", state));
			acctserv_response(sockfd, 4, "Server Error");
			return;
		}
	}
	return;
}

static dp_result_t acctserv_save(acctserv_t *acctserv)
{
	dp_result_t err;

	wmq_flush(acctserv->pwq->wmq);
	/* tcapw_close just saves the db to disk. It doesn't change tcapw. */
	err = tcapw_close(acctserv->tcapw);
	if (err != dp_RES_OK) {
		printf("acctserv: tcapw_close err:%d\n", err);
		return err;
	}
}

static acctserv_t *acctserv_restore(const char *dir)
{
	dp_result_t err;
	tcapw_t *tcapw;
	wmq_t *wmq;
	pwq_t *pwq;
	char PWFile[256];
	int len;
	time_t timestamp;
	long offset;

	len = strlen(dir);
	if (len > 240)
		return NULL;
	strcpy(PWFile, dir);
	if (dir[len-1] != '/') {
		PWFile[len++] = '/';
	}
	strcpy(PWFile + len, "prefs.dat");

	tcapw = tcapw_create();
	if (!tcapw)
		return NULL;
	err = tcapw_open(tcapw, PWFile);
	if (err == dp_RES_EMPTY) {
		printf("acctserv: pwfile:%s not found\n", PWFile);
		return NULL;
	} else if (err != dp_RES_OK) {
		printf("acctserv: tcapw_open err:%d\n", err);
		return NULL;
	}

	pwq = pwq_create(tcapw, dir, 0, pwq_SERVERTAG_MASTER);
	if (!pwq)
		return NULL;
	err = pwq_tell(pwq, &timestamp, &offset);
	if (err == dp_RES_OK) {
		while (1) {
			pwq_message_t msg;

			err = pwq_get(pwq, &msg);
			if (err == dp_RES_EMPTY)
				break;
			else if (err == dp_RES_AGAIN)
				continue;
			else if (err != dp_RES_OK)
				return NULL;

			switch (msg.type) {
			case pwq_MESSAGETYPE_CREATE:
				err = tcapw_stuff_entry(tcapw, &msg.u.acctCreate);
				if (err != dp_RES_OK) {
					printf("acctserv: entry_stuff(%d) returns err:%d\n", msg.u.acctCreate.uid, err);
					return NULL;
				}
				break;
			case pwq_MESSAGETYPE_CHANGE:
				err = tcapw_entry_change(tcapw, msg.u.acctChange.uid, &msg.u.acctChange.hpw, msg.u.acctChange.flags, msg.u.acctChange.email);
				if (err != dp_RES_OK) {
					printf("servpw_poll: entry_change(%d) returns err:%d\n", msg.u.acctChange.uid, err);
					return NULL;
				}
				break;
			case pwq_MESSAGETYPE_DELETE:
				err = tcapw_entry_delete(tcapw, msg.u.acctDelete.uid);
				if (err != dp_RES_OK) {
					printf("acctserv: entry_delete(%d) returns err:%d\n", msg.u.acctDelete.uid, err);
					return NULL;
				}
				break;
			default:
				printf("acctserv: unknown message type:%c\n", msg.type);
				return NULL;
			}

			/* database was modified, so update its wmq pointer */
			err = pwq_tell(pwq, &timestamp, &offset);
			if (err != dp_RES_OK) {
				printf("acctserv: bug: pwq_tell returns err:%d\n", err);
				return NULL;
			}
			err = tcapw_setWmqPosition(tcapw, timestamp, offset);
			if (err != dp_RES_OK) {
				printf("servpw_poll: bug: setWmqPosition returns err:%d\n", err);
				return NULL;
			}
		}
	} else if (err != dp_RES_EMPTY) {
		printf("acctserv: bug: pwq_tell returns err:%d\n", err);
		return NULL;
	}

	pwq_destroy(pwq);  /* This incidentally saves tcapw to disk. */

	pwq = pwq_create(tcapw, dir, 1, pwq_SERVERTAG_MASTER);
	if (!pwq)
		return NULL;
	return acctserv_create(pwq);
}

void main(int argc, char *argv[])
{
	dp_result_t err;
	acctserv_t *acctserv;
	int sockfd; 
	struct sockaddr_un my_addr;
	int pid;
	time_t now, next_save;

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	if (argc < 2) {
		printf("\
Usage: %s <Dir>\n\
	Starts account server which listens for and handles account requests.\n\
	<Dir> is used as the working directory, and should contain a password\n\
	database file prefs.dat and any wmq files associated with it.\n", argv[0]);
		exit(0);
	}
	printf("acctserv: starting %s %s\n", argv[0], argv[1]);

	close(0);		/* close stdin */

	acctserv = acctserv_restore(argv[1]);
	if (!acctserv) {
		printf("acctserv: error restoring from dir:%s\n", argv[1]);
		exit(1);
	}

	/* PARENT handles accept */
	printf("acctserv: creating socket\n");
	if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
		printf("acctserv: socket error:%d\n", errno);
		exit(1);
	}

	printf("acctserv: binding my local socket\n");
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sun_family = AF_LOCAL; 
	strcpy(my_addr.sun_path, acctserv_SOCKETNAME);
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		printf("acctserv: bind error:%d\n", errno);
		exit(1);
	}

	printf("acctserv: starting listen\n");
	if (listen(sockfd, 5) < 0) {
		printf("acctserv: listen error:%d\n", errno);
		exit(1);
	}

	next_save = time(NULL) + TIME_SAVE;
	while (1) {
		int newsock;
		struct sockaddr_un client_addr;
		int client_addrlen;

		now = time(NULL);
		if ((long)(now - next_save) > 0) {
			err = acctserv_save(acctserv);
			if (err != dp_RES_OK) {
				printf("acctserv: save err:%d\n", err);
				exit(1);
			}
			printf("acctserv: database saved\n");
			next_save = now + TIME_SAVE;
		}
		/* ACCEPT A CONNECTION AND THEN CREATE A CHILD TO DO THE WORK */ 
		/* LOOP BACK AND WAIT FOR ANOTHER CONNECTION */ 
		printf("acctserv: starting accept\n");
		newsock = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
		if (newsock < 0) {
			printf("acctserv: accept error:%d\n", errno);
			exit(1);
		}
		printf("acctserv: connection requested from %s on socket:%d\n",
			client_addr.sun_path, newsock);
		switch (pid = fork()) {
		case -1:
			printf("acctserv: fork error:%d\n", errno);
			exit(1);

		case 0:
			/* CHILD PROCESS */ 
			close(sockfd);	/* child only uses newsock */
#if 0
			/* Multiple simultaneous requests don't work due to wmq/db
			 * conflicts, now.
			 */
			/* set our process group ID to 0, so init will handle cleanup */
			if (setpgid(0, 0) != 0) {
				printf("acctserv: setpgid error:%d\n", errno);
				exit(1);
			}
			close(1);		/* close stdout */
			close(2);		/* close stderr */
			switch (pid = fork()) {
			case -1:
				exit(1);
			case 0:		/* ORPHANED GRANDCHILD */
				break;
			default:	/* CHILD */
				exit(0);  /* orphan the grandchild */
			}
			/* ORPHANED GRANDCHILD handles connection */
#else
			/* only one at a time, CHILD handles connection */
#endif
			acctserv_handleRequest(acctserv, newsock); 
			close(newsock); 
			exit(0);
			break;

		default:
			/* PARENT PROCESS */
			close(newsock);  /* only the child needs this socket now */
			if (wait(NULL) != pid) {
				printf("acctserv: wait for child %d failed\n", pid);
				exit(1);
			}
			break;
		}
	}

	err = acctserv_save(acctserv);
	if (err != dp_RES_OK) {
		printf("acctserv: save err:%d\n", err);
		exit(1);
	}
	printf("acctserv: database saved\n");
	pwq_destroy(acctserv->pwq);
	tcapw_destroy(acctserv->tcapw);
	acctserv_destroy(acctserv);
	exit(0);
}

#endif  /* acctserv_DAEMON */
