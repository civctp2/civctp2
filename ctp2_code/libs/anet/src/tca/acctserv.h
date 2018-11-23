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

#ifndef ACCTSERV_H
#define ACCTSERV_H

#include "aneterr.h"
#include "tcapw.h"
#include "pwq.h"

/* Instance data of acctserv */
typedef struct {
	pwq_t *pwq;
	tcapw_t *tcapw;
} acctserv_t;

/*--------------------------------------------------------------------------
 Instantiate acctserv using the message queue pwq and the associated
 password database it contains.
--------------------------------------------------------------------------*/
acctserv_t *acctserv_create(pwq_t *pwq);

/*--------------------------------------------------------------------------
 Clean up an instance of acctserv.  Does not destroy pwq.
--------------------------------------------------------------------------*/
void acctserv_destroy(acctserv_t *acctserv);

/*--------------------------------------------------------------------------
 Given an ascii username and password and an email address, add a new
 account to the password database and, on success, email the user a
 validation code and add the new entry to the wmq.
 Returns dp_RES_OK on success,
		 dp_RES_ALREADY if the username is in use.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_create(acctserv_t *acctserv,
	const char *username, const char *password, const char *email);

/*--------------------------------------------------------------------------
 Given a previously created username and the correct validation code,
 activates an account.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the validation code is incorrect,
		 dp_RES_ALREADY if the account was already activated.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_activate(acctserv_t *acctserv,
	const char *username, const char *password, const char *validationcode);

/*--------------------------------------------------------------------------
 Check the password for username.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password is incorrect,
		 dp_RES_NOTYET if the password is ok but the account has not been
		 	activated yet.
--------------------------------------------------------------------------*/
dp_result_t acctserv_check_password(acctserv_t *acctserv,
	const char *username, const char *password);

/*--------------------------------------------------------------------------
 Change a user's password or email.  If newpassword or newemail is NULL,
 leave that field alone.  If email is changed, send a validation code to
 the new email and deactivate the account for future reactivation.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password is incorrect.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_change(acctserv_t *acctserv,
	const char *username, const char *password,
	const char *newpassword, const char *newemail);

/*--------------------------------------------------------------------------
 Delete a user's account.
 Returns dp_RES_OK on success,
		 dp_RES_ACCESS if the password is incorrect.
--------------------------------------------------------------------------*/
dp_result_t acctserv_account_delete(acctserv_t *acctserv,
	const char *username, const char *password);

#endif
