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

#include <stdlib.h>
#include <stdio.h>
#include "ser.h"

void quit(int i)
{
	Sleep(100);
	exit(i);
}

/* Get a packet. If one isn't ready right away, wait up to 1 second for 
 * a packet to arrive.  If none arrives, or if it isn't len_want bytes 
 * long, abort.
 */
void ser_get_blocking(ser_t *ser, void *buf, size_t len, size_t len_want)
{
	size_t len_got;
	int i;
	ser_result_t err;

	for (i = 1; i <= 10; i++) {
		
		len_got = len;
		err = ser_get( ser, buf, &len_got);
		if (err == ser_RES_OK) 
			break;
		if (err != ser_RES_EMPTY) {
			printf("ser_get_blocking: expected err 0, got err %d\n", err);
			quit(1);
		} 
		Sleep(100);
	}
	if (err == ser_RES_EMPTY) {
		printf("ser_get_blocking: ser_get did not get any packet\n");
		quit(1);
	} 
	if (len_got != len_want) {
		printf("ser_get_blocking: expected %d byte got %d\n", len, len_got);
		quit(1);
	}
}

// stupid sanity check to help flush out bugs
void test1 (int portnum)
{
	char tbuf[ser_USERMAXPACK];
	char rbuf[ser_USERMAXPACK];
	ser_result_t err;
	int testnum;

	ser_t *ser;
	ser = ser_create();
	if (!ser) {
		printf("fail create\n");
		quit(1);
	}
	err = ser_config(ser, 19200, portnum);
	if (err != ser_RES_OK) {
		printf("fail config\n");
		quit(1);
	}

	printf("Hit enter when you've started this program on both machines...\n");
	gets(tbuf);

	for (testnum = 0; testnum < 100; testnum++) {
		int i;
		ser_put(ser,  "Test\x70"" 7", 7 );
		ser_put(ser,  "Test 4", 6 );
		ser_put(ser,  "Test 5", 6 );
		ser_put(ser,  "Test 6", 6 );
		for (i=0; i < ser_USERMAXPACK; i++)
			tbuf[i] = i;
		ser_put(ser,  tbuf, ser_USERMAXPACK);	

		ser_get_blocking(ser, tbuf, sizeof(tbuf), 7 );
		if (!strncmp(tbuf,"Test\160 7", 7))
			printf("Test 7 pass\n");
		else {
			printf("Test 7 fail\n");
			quit(1);
		}

		ser_get_blocking(ser, tbuf, sizeof(tbuf), 6 );
		if (!strncmp(tbuf,"Test 4", 6))
			printf("Test 4 pass\n");
		else
			printf("Test 4 fail\n");

		ser_get_blocking(ser, tbuf, sizeof(tbuf), 6 );
		if (!strncmp(tbuf,"Test 5", 6))
			printf("Test 5 pass\n");
		else
			printf("Test 5 fail\n");

		ser_get_blocking(ser, tbuf, sizeof(tbuf), 6 );
		if (!strncmp(tbuf,"Test 6", 6))
			printf("Test 6 pass\n");
		else
			printf("Test 6 fail\n");

		ser_get_blocking(ser, tbuf, sizeof(tbuf), ser_USERMAXPACK );
		for (i=0; i < ser_USERMAXPACK; i++)
			rbuf[i] = i;
		if (!memcmp(tbuf,rbuf, ser_USERMAXPACK))
			printf("big test pass\n");
		else
			printf("big test fail\n");
	}
	
	Sleep(1000);
	ser_destroy(ser);

	//printf("pass\n");
}

void main(int argc, char **argv)
{
	int i;
	int portnum = 0;
	
	for (i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			printf("Options must start with -\n");
			quit(1);
		}
		if (argv[i][1] == 0) {
			printf("Expected letter after -\n");
			quit(1);
		}
		if (argv[i][2] != '=' && argv[i][2] != ':') {
			printf("Expected = after -%c\n", argv[i][1]);
			quit(1);
		}

		switch (toupper(argv[i][1])) {

		case 'Y':   /*  Set comm port */
			sscanf(argv[i]+3, "%d", &portnum);
			break;

		default:
			printf("Usage: %s [-Y:COMPORTNUM] e.g. -Y:0 for COM1\n"
				"Unknown arg %s\n", argv[0], argv[i]);
			quit(1);
		} /*  end switch */
	}
	for (i=0; i<1; i++) {
		printf("Run %d\n", i);
		test1(portnum);
	}
	printf("Pass\n");
	quit(0);
}



