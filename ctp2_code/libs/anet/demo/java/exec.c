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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <direct.h>

int main(int argc, char **argv)		//argv = {gameDrive:gameDir, gameCommand}
{
	int i;
	char currentDrive, newDrive;
	int driveVal;
	char *path;
	char **cmdargv = argv+2;

	if(argc < 3) {
		fprintf(stderr, "Usage: %s drive:dir cmd [args]\n", argv[0]);
		fprintf(stderr, "Args not supported yet.\n");
		exit(1);
	}
	newDrive = argv[1][0];
	path = argv[1]+2;
	driveVal = toupper(newDrive) - 'A' + 1;

	currentDrive = _getdrive();					//	get current drive;
	if(driveVal != currentDrive) {
		if(_chdrive(driveVal)) {				// change to game drive
			fprintf(stderr, "Error: exec: chdrive(%d) failed.\n", driveVal);
			fprintf(stderr, "path %s, drive %c, driveVal %d\n", path, newDrive, driveVal);
			exit(2);
		}
	}
	if(_chdir(path)) {						// change to game directory
		fprintf(stderr, "Error: exec: chdir %s failed.\n", path);
		fprintf(stderr, "path %s, drive %c, driveVal %d\n", path, newDrive, driveVal);
		exit(2);
	}
	_execl(cmdargv[0], cmdargv[0], NULL);		// start game.  No args yet.
	fprintf(stderr, "Error: exec(%s...) failed.\n", cmdargv[0]);
	fprintf(stderr, "path %s, drive %c, driveVal %d\n", path, newDrive, driveVal);
	exit(2);
}

