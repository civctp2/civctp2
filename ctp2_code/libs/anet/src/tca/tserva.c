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

#include "tserv.h"
#include "mywcs.h"

#define MYLEN 128

/*--------------------------------------------------------------------------
 Calls to schedule responses and inform this layer of the needed userinfo.

 Arguments are same as for tserv_account_login/createW, but
 username and password are in the current locale's 8-bit or multibyte-string
 character set instead of Unicode.
--------------------------------------------------------------------------*/
dp_result_t tserv_account_loginA(tserv_t *tserv, const char *username, const char *password)
{
	wchar_t wuname[MYLEN];
	wchar_t wpw[MYLEN];

	if (mywcs_frommbs(wuname, MYLEN, username) == -1)
		return dp_RES_BAD;
	if (mywcs_frommbs(wpw, MYLEN, password) == -1)
		return dp_RES_BAD;

	return tserv_account_loginW(tserv, wuname, wpw);
}

dp_result_t tserv_account_createA(tserv_t *tserv, const char *username, const char *password, int flags, const char *email)
{
	wchar_t wuname[MYLEN];
	wchar_t wpw[MYLEN];
	wchar_t wemail[MYLEN];

	if (mywcs_frommbs(wuname, MYLEN, username) == -1)
		return dp_RES_BAD;
	if (mywcs_frommbs(wpw, MYLEN, password) == -1)
		return dp_RES_BAD;
	if (mywcs_frommbs(wemail, MYLEN, email) == -1)
		return dp_RES_BAD;
	return tserv_account_createW(tserv, wuname, wpw, flags, wemail);
}

dp_result_t tserv_account_activateA(tserv_t *tserv, const char *secretcode)
{
	wchar_t wsecretcode[MYLEN];

	if (mywcs_frommbs(wsecretcode, MYLEN, secretcode) == -1)
		return dp_RES_BAD;

	return tserv_account_activateW(tserv, wsecretcode);
}

/*--------------------------------------------------------------------------
  Send a userinfo change request using the challenge we logged in with.

  Arguments are same as for tserv_change_passwordW, but
  passwords are in the current locale's 8-bit or multibyte-string
  character set instead of Unicode.
--------------------------------------------------------------------------*/
dp_result_t tserv_change_passwordA(tserv_t *tserv, const char *oldpassword, const char *newpassword, int flags, const char *email)
{
	wchar_t woldpw[MYLEN];
	wchar_t wnewpw[MYLEN];
	wchar_t wemail[MYLEN];

	if (mywcs_frommbs(woldpw, MYLEN, oldpassword) == -1)
		return dp_RES_BAD;
	if (mywcs_frommbs(wnewpw, MYLEN, newpassword) == -1)
		return dp_RES_BAD;
	if (mywcs_frommbs(wemail, MYLEN, email) == -1)
		return dp_RES_BAD;
	return tserv_change_passwordW(tserv, woldpw, wnewpw, flags, wemail);
}
