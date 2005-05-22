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
 cgi-bin program to update game server text files and optionally reset server.
 Requires user to enter correct admin password.
 Redirects user to http://$server/anet2/etc/servfil.html
 then writes completion status there slowly.  
 (That way, the browser's reload can grab updated status without accidentally
 reloading the cgi-bin script!)
--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*********** CONFIGURATION ***********/

/* Where etc/start2 is located */
const char *servdir = "/home/alink";

/* Where url http://$server/anet2/ maps to */
const char *htmldir = "/home/alink/public_html";

/*********** END CONFIGURATION ***********/

static char *server;

/* parse argument string */
static char **parseArgs(char *in, int *narg)
{
	int i;
	char **parsed;
	char buf[1024];
	char *ptr, *ptr0 = in;
	*narg = 1;
	while ((ptr = strchr(ptr0, '&'))) {
		(*narg)++;
		ptr0 = ptr + 1;
	}
	parsed = (char **)malloc((*narg) * sizeof(char *));
	ptr = buf;
	i = 0;
	while (*in) {
		switch(*in) {
			int val, val1;
			case '%':
				in++;
				if ((sscanf(in,"%2x",&val)==1) &&
					(val==0xD) && (*(in+2)=='%') &&
					(sscanf(in+3, "%2x", &val1)==1)&&
					(val1 == 0xA)) {
					*ptr = '\n';
					in+=5;
					break;
				}
				*ptr = val;
				in += 2;
				ptr++;
				break;
			case '+':
				*ptr = ' ';
				in++;
				ptr++;
				break;
			case '&':
				*ptr = '\0';
				parsed[i] = strdup(buf);
				ptr = buf;
				i++;
				in++;
				break;
			default:
				*ptr = *in;
				ptr++;
				in++;
				break;
		}
	}
	*ptr = '\0';
	parsed[i] = strdup(buf);
	return parsed;
}

/* return update failure */
static void writeFail(char *text)
{
	printf("\n");
	printf("<HTML><HEAD>\n");
	printf("<TITLE>");
	printf("%s update", server);
	printf("</TITLE></HEAD>\n");
	printf("<BODY><h2>");
	printf("%s update unsuccessful", server);
	printf("</h2>\n<pre>\n");
	printf("%s", text);
	printf("</pre></BODY></HTML>\n");
}

static char mask[] = "255.255.252.0";
static char baseip[] = "207.82.188.0";
static struct {
	char letter;
	int buffer;
} passwrd[] = {{'h', 0}, {'a', 0}, {'c', 0}, {'k', 0}, {'@', 0}, {'s', 0}, {'h', 0}, {'a', 0}, {'q', 0}, {'\0', 0}};

/* check password and restart option; then fork and exec servfil script to
 * perform desired action and return completion status */
int main(int argc, char **argv)
{
	int i, inlen, status, narg, ret, bCorrectPwd = 0;
	char *bRestart = "0";
	FILE *fp;
	pid_t pid;
	uid_t uid;
	gid_t gid;
	char *buf;
	char *input = NULL;
	char **parsed;
	char htmlfile[256], servfile[256];
	printf("Content-type: text/html\n");
	fflush(stdout);
	if (!(server = getenv("SERVER_NAME")) &&
		!(server = getenv("HOSTNAME")))
		server = "";
	sprintf(htmlfile, "%s/%s", htmldir, "etc/servfil.html");
	sprintf(servfile, "%s/%s", servdir, "etc/servfil");
	if ((fp = fopen("/tmp/servfil.running", "r"))) {
		char buf[256];
		sprintf(buf, "servfil already running\n<a href=\"http://%s/anet2/etc/servfil.html\">servfil status</a>\n", server);
		writeFail(buf);
		fclose(fp);
		exit(1);
	} else if ((fp = fopen("/tmp/servfil.running", "w")))
		fclose(fp);
	if ((fp = fopen(htmlfile, "r"))) {
		fclose(fp);
		remove(htmlfile);
	}
	if (!(buf = getenv("REMOTE_ADDR"))) {
		writeFail("cannot authenticate your ip address\n");
		remove("/tmp/servfil.running");
		exit(1);
	} else {
		int i;
		char *pmask = mask;
		char *pbaseip = baseip;
		char *ptrbuf, *ptrmask, *ptrbaseip;
		for (i = 0; i < 3; i++) {
			if ((ptrbuf = strchr(buf, '.')) &&
				(ptrmask = strchr(pmask, '.')) &&
				(ptrbaseip = strchr(pbaseip, '.'))) {
				*ptrbuf = '\0'; *ptrmask = '\0'; *ptrbaseip = '\0';
				ptrbuf++; ptrmask++; ptrbaseip++;
				if ((atoi(buf) & atoi(pmask)) != atoi(pbaseip))
					break;
				buf = ptrbuf; pmask = ptrmask; pbaseip = ptrbaseip;
			} else
				break;
		}
		if (i < 3 || !buf || !pmask || !pbaseip ||
			((atoi(buf) & atoi(pmask)) != atoi(pbaseip))) {
			writeFail("cannot run from your ip address\n");
			remove("/tmp/servfil.running");
			exit(1);
		}
	}
	if ((buf = getenv("CONTENT_LENGTH")) &&
		(inlen = atoi(buf))) {
		input = (char *)malloc(inlen);
		fread(input, 1, inlen, stdin);
	} else if ((input = getenv("QUERY_STRING"))) {
		buf = strchr(input, '?');
		input = buf + 1;
	} else if (argc > 1)
		input = argv[1];
	if (!input) {
		writeFail("need password\n");
		remove("/tmp/servfil.running");
		exit(1);
	}
	parsed = parseArgs(input, &narg);
	if (inlen)
		free(input);
	for (i = 0; i < narg; i++) {
		char *ptr;
		if ((ptr = strchr(parsed[i], '='))) {
			char *var, *val;
			*ptr = '\0';
			var = parsed[i];
			val = ptr+1;
			if (!strcmp(var, "password")) {
				int j, jmax = sizeof(passwrd)/sizeof(*passwrd);
				for (j = 0; j < jmax; j++) {
					if (*val++ != passwrd[j].letter)
						break;
				}
				if (j >= jmax)
					bCorrectPwd = 1;
			} else if (!strcmp(var, "restart")) {
				if (!strcmp(val, "true"))
					bRestart = "1";	
			} else if (!strcmp(var, "reload")) {
				if (!strcmp(val, "true")) {
					/* restart has precedence over reload */
					if (strcmp(bRestart, "1"))
						bRestart = "2";	
				}
			}
		}
	}
	if (!bCorrectPwd) {
		writeFail("wrong password\n");
		remove("/tmp/servfil.running");
		exit(1);
	}
	uid = geteuid();
	setreuid(uid, uid);
	gid = getegid();
	setregid(gid, gid);
	pid = fork();
	if (pid < 0) {
		writeFail("cannot fork\n");
		remove("/tmp/servfil.running");
		exit(1);
	} else if (pid == 0) {
		freopen(htmlfile, "w", stdout);
		printf("<HTML><HEAD>\n");
		printf("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"10\">\n");
		printf("<TITLE>");
		printf("%s update", server);
		printf("</TITLE></HEAD>\n");
		printf("<BODY><h2>");
		printf("<a href=\"http://%s/anet2\"> %s</a> update", server, server);
		printf("</h2>\n<pre>\n");
		printf("<a href=\"http://%s/anet2/etc\">check files</a>\n\n", server);
		printf("Update status:\n");
		fflush(stdout);
		ret = execl("/usr/bin/env", "/usr/bin/env", "-i", servfile, bRestart, NULL);
		if (ret < 0) {
			printf("child process cannot exec servfil\n");
			remove("/tmp/servfil.running");
			exit(1);
		}
	} else if (pid > 0) {
		printf("Status: 302\n");
		printf("Location: http://%s/anet2/etc/servfil.html\n", server);
		printf("Content-length: 0\n\n");
		fflush(stdout);
	}
}
