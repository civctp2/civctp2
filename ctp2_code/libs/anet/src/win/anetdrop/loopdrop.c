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
 Program to loop, calling anetdrop repeatedly, until it exits with an error.
 Reads commandline from a text file.
 The text file has two lines; the second line is used for all runs but the
 first.  The commandline used by the first run is set by the lineNoStart
 argument.
-------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <windows.h>

void usage(char * cmd)
{
	printf("Usage: %s lineNoStart\n", cmd);
	printf("lineNoStart may be 1 or 2\n");
	printf("reads command lines from input.txt\n");
	exit(1);
}

int main(int argc, char ** argv)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	DWORD res;
	DWORD res1;
	char cmdLines[2][1024];
	int lineNo;
	FILE *stream;
	int i;

	if (argc != 2) {
		usage(argv[0]);
	}
	lineNo = atoi(argv[1]);
	if (lineNo != 1 && lineNo != 2)
		usage(argv[0]);

	if( (stream = fopen( "input.txt", "rt" )) == NULL ) {
		printf("input.txt is not in the current directory");
		usage(argv[0]);
	}
	for (i=0; i < 2; i++) {
		int len;
		if( fgets( cmdLines[i], sizeof(cmdLines[i]), stream ) == NULL) {
			printf( "fgets error\n" );
			exit(1);
		}
		len = strlen(cmdLines[i]);
		if (cmdLines[i][len-1] == '\n')
			cmdLines[i][len-1] = 0;
	}
	fclose( stream );

	while (1) {
		DWORD exitcode = 0;

		memset(&pi, 0, sizeof(pi));
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);

		printf("running '%s'\n", cmdLines[lineNo-1]);
		if (!CreateProcess(NULL, cmdLines[lineNo-1], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
			res = GetLastError();
			if (!GetExitCodeProcess(pi.hProcess, &exitcode)) {
				printf("CreateProcess; GetExitCodeProcess failed.\n");
				return 1;
			}
			printf("Create process failed; GetLastError %d, exitcode %d\n", res,exitcode);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return 1;
		}
		printf("hProcess %x, hThread %x, dwProcessId %x, dwThreadId %x\n", pi.hProcess, pi.hThread, pi.dwProcessId, pi.dwThreadId);

		res = WaitForSingleObject(pi.hProcess, INFINITE);
#if 0
		printf("app finished; WaitForSingleObject returns %d\n", res);
		switch(res) {
		case WAIT_FAILED:
			printf("WAIT_FAILED %x\n", WAIT_FAILED);
			break;
		case WAIT_ABANDONED:
			printf("WAIT_ABANDONED %x\n", WAIT_ABANDONED);
			break;
		case WAIT_OBJECT_0:
			printf("WAIT_OBJECT_0 %x\n", WAIT_OBJECT_0);
			break;
		case WAIT_TIMEOUT:
			printf("WAIT_TIMEOUT %x\n", WAIT_TIMEOUT);
			break;
		}
#endif

		if (res == WAIT_FAILED) {
			res = GetLastError();
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			printf("WaitForSingleObject fails; GetLastError returns %d\n", res);
			return 1;
		}
		if (!GetExitCodeProcess(pi.hProcess, &exitcode)) {
			res = GetLastError();
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			printf("GetExitCodeProcess failed. GetLastError returns %d\n", res);
			return 1;
		}
		if (exitcode != 0) {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			printf("cmd '%s' failed!! exitcode %d\n", cmdLines[lineNo-1], exitcode);
			return 1;
		}
		lineNo=2; /* switch to join */
	}
	printf("the end of the program\n");
	return 0;
}
