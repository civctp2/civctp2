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
 Command line database file editor for tserv.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include "dp2.h"
#include "tcapw.h"
#include "mywcs.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* globals for use by main, ProcessCommandLine */
char dbFilename[80];
char action;
char name[tcapw_LEN_USERNAME];
char password[tcapw_LEN_PW];
char email[tcapw_MAXLEN_EMAIL];
int flags;

/*----------------------------------------------------------------------------
  Processes the command line arguments.  Some arguments are handled
  directly by this routine.  Others simply result in an update to
  the global switches data structure.
----------------------------------------------------------------------------*/
void ProcessCommandLine(int argc, char **argv)
{
	char *chptr;
	int   i;
	
	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
			/* deal with args that start with - or / */
			if (argv[i][1] == 0) {
				printf("Expected letter after -\n");
				exit(1);
			}
			switch(toupper(argv[i][1])) {
			case 'A':  /* Add a user */
				action = 'A';
				/* the next 4 parameters should be name pw email flags */
				if (argc < i + 4) {
					printf("%c requires 4 parameters - name pw email flags\n");
					exit(1);
				}
				strncpy(name, argv[++i], tcapw_LEN_USERNAME);
				strncpy(password, argv[++i], tcapw_LEN_PW);
				strncpy(email, argv[++i], tcapw_MAXLEN_EMAIL);
				flags = atoi(argv[++i]);
				break;
			case 'C':  /* Change a user's entry */
				action = 'C';
				printf("%c not yet supported!", action);
				exit(1);
				break;
			case 'D':  /* Delete a user */
				action = 'D';
				printf("%c not yet supported!", action);
				exit(1);
				break;
			case 'F':  /* Set dbfile */
				if ((chptr = strchr(argv[i], '=')) == NULL) {
					printf("-F flag requires -F=dbfile");
					exit(1);
				}
				chptr++;
				strncpy(dbFilename, chptr, 80);
				break;
			case 'L':  /* List */
				action = 'L';
				printf("%c not yet supported!", action);
				exit(1);
				break;

			default:
				printf("\
Usage: %s [-F=dbfile]\n\
        (-L(ist) | -A(dd) name password email flags | -D(elete) uid |\n\
         -C(hange) uid [N=name] [P=password] [E=email] [F=flags])\n\
", argv[0]);
				printf("\
A simple command line tcapw database editor.\n");
				exit(1);
			} /*  end switch */
		}
	}
}


int main(int argc, char *argv[])
{
	tcapw_t *tcapw = NULL;

	tcapw_entry_t entry1;
	tcapw_pw_t    pw1;
	tcapw_hpw_t   hpw1;
	tcapw_uname_t uname1;
	tcapw_uid_t uid1;
	wchar_t wcbuf[256];
	
	dp_result_t res;

	if (argc < 2) {
		printf("\
Usage: %s [-F=dbfile]\n\
        (-L(ist) | -A(dd) name password email flags | -D(elete) uid |\n\
         -C(hange) uid [N=name] [P=password] [E=email] [F=flags])\n\
", argv[0]);
		printf("\
    A simple command line tcapw database editor.\n");
		exit(0);
	}

	/* defaults */
	strcpy(dbFilename, "prefs.dat");  
	action = 0;
	/* these are all disallowed 'null' values.  If the -C option is
	 * used, any fields which retain the values below are left alone.
	 */
	name[0] = 0;
	password[0] = 0;
	email[0] = 0;
	flags = -1;
	ProcessCommandLine(argc, argv);

	memset(&entry1, 0, sizeof(entry1));
	/* Create a password database, add a user to it,
	 * make sure you can look him up various ways,
	 * change his password,
	 * save it to disk, restore it from disk.
	 */

	/* Create the database */
	tcapw = tcapw_create();
	assert(tcapw);
	res = tcapw_open(tcapw, dbFilename);
	if (res == dp_RES_EMPTY)
		printf("Created dbfile %s.\n", dbFilename);
	else if (res == dp_RES_OK)
		printf("Opened dbfile %s.\n", dbFilename);
	else {
		printf("Error opening dbfile %s: %d.\n", dbFilename, res);
		exit(1);
	}

	switch (action) {
	case 'A':
		if (name[0] == 0 || password[0] == 0 || email[0] == 0 || flags == -1) {
			printf("Invalid user parameters\n");
			exit(1);
		}
		mywcs_frommbs(wcbuf, tcapw_LEN_USERNAME, name);
		mywcs_wchar2netchar(uname1.uname, wcbuf, tcapw_LEN_USERNAME);
		mywcs_frommbs(wcbuf, tcapw_LEN_PW, password);
		mywcs_wchar2netchar(pw1.pw, wcbuf, tcapw_LEN_PW);
		res = tcapw_password_hash(tcapw, &pw1, &hpw1);
		assert(!res);
		res = tcapw_entry_create(tcapw, &uname1, &hpw1, flags, email, &uid1);
		if (res == dp_RES_ALREADY) {
			printf("Username %s is already in use\n", name);
		} else if (res == dp_RES_OK) {
			printf("User %s added with uid %d\n", name, uid1);
		} else {
			printf("Could not add user %s, err %d\n", name, res);
		}
		break;

	default:
		printf("Unsupported action %c\n", action);
		break;
	}
	
	res = tcapw_close(tcapw);
	if (res != dp_RES_OK)
		printf("Error writing dbfile %s: %d\n", dbFilename, res);
	tcapw_destroy(tcapw);
	
	return(0);
}



