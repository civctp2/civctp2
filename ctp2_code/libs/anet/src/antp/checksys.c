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
#include <sys/stat.h>
#include <fcntl.h>

#define DPRINT(a) printf a

#define MAX_MEM 512
#define MAX_FD 256

static void checksys()
{
	char *memptr[MAX_MEM];
	int fd[MAX_FD];
	const char file[] = "/dev/null";
	int i;

	for (i = 0; i < MAX_MEM; i++) {
		memptr[i] = (char *)malloc(1024*1024);
		if (memptr[i] == NULL)
			break;
	}
	if (i >= MAX_MEM) {
		DPRINT(("checksys: memory: > %d MB\n", MAX_MEM));
	} else {
		DPRINT(("checksys: memory: %d MB\n", i));
	}
	for (i--; i >= 0; i--) {
		free(memptr[i]);
	}

	for (i = 0; i < MAX_FD; i++) {
		fd[i] = open(file, O_RDONLY);
		if (fd[i] < 0)
			break;
	}
	if (i >= MAX_FD) {
		DPRINT(("checksys: file descriptors: > %d\n", MAX_FD));
	} else {
		DPRINT(("checksys: file descriptors: %d\n", i));
	}
	for (i--; i >= 0; i--) {
		close(fd[i]);
	}
	return;
}

void main()
{
	checksys();
}
