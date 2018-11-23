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

// stupid sanity check to help flush out bugs
test1 ()
{
	char tbuf[200];
	size_t len;
	ser_result_t err;

	ser_t *ser;
	ser = ser_create();
	if (!ser) {
		printf("fail create\n");
		exit(1);
	}
	err = ser_config(ser, 19200, 0);
	if (err != ser_RES_OK) {
		printf("fail config\n");
		exit(1);
	}

	ser_put(ser,  "Test\x70"" 7", 7 );
	ser_put(ser,  "Test 4", 6 );
	ser_put(ser,  "Test 5", 6 );
	ser_put(ser,  "Test 6", 6 );

	printf("Hit enter when you've started this program on both machines...\n");
	gets(tbuf);

	len = sizeof(tbuf);
	err = ser_get( ser, tbuf, &len );
	if (err != ser_RES_OK) {
		printf("fail get; expected err 0, got err %d\n", err);
		exit(1);
	}
	if (len != 7) {
		printf("Test 7 fail - len %d, expected %d\n", len, 7);
		exit(1);
	}
	if (!strncmp(tbuf,"Test\160 7", 7))
		printf("Test 7 pass\n");
	else {
		printf("Test 7 fail\n");
		exit(1);
	}

	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (!strncmp(tbuf,"Test 4", 6))
		printf("Test 4 pass\n");
	else
		printf("Test 4 fail\n");

	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (!strncmp(tbuf,"Test 5", 6))
		printf("Test 5 pass\n");
	else
		printf("Test 5 fail\n");

	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (err == ser_RES_OK)
		printf("Test 6 pass\n");
	else
		printf("Test 6 fail\n");

#ifdef SIMSER
	// Test how the driver handles dropping the critical 0x41 at the
	// end of a packet
	printf("next one should fail\n");
	ser_put(ser,  "Test a", 6 );	// should fail
	ser->port->tail--;
	printf("next one should fail\n");
	ser_put(ser,  "Test b", 6 );	// should fail
	ser_put(ser,  "Test c", 6 );
	printf("next one should fail\n");
	ser_put(ser,  "Test d", 6 );	// should fail
	ser->port->buf[ser->port->tail-5]++;
	ser_put(ser,  "Test e", 6 );
	printf("next one should fail\n");
	ser_put(ser,  "Test f", 6 );	// should fail, kinda

	// What should GetPacket do if the packet is corrupt?
	// I think it should return NULL and since I am the one
	// writing the program what I say goes.
	// in fact, because we took out the packet by deleting
	// the last byte, it corrupts the following packet.
	// A single NULL should be returned for both. I think.
	printf("The next two should fail since we corrupted one byte of its payload\n");
	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (err != ser_RES_OK)
		printf("Test a pass\n");
	else
		printf("Test a fail\n");

	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (!strncmp(tbuf,"Test c", 6))
		printf("Test c pass\n");
	else
		printf("Test c fail\n");

	printf("This one should fail since we corrupted one byte of its payload\n");
	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (err != ser_RES_OK)
		printf("Test d pass\n");
	else
		printf("Test d fail\n");

	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (!strncmp(tbuf,"Test e", 6))
		printf("Test e pass\n");
	else
		printf("Test e fail\n");

	len = sizeof(tbuf);
	err = ser_get(ser, tbuf, &len );
	if (err == ser_RES_OK)
		printf("Test f pass\n");
	else
		printf("Test f fail\n");
#endif

	ser_destroy(ser);

	//printf("pass\n");
}

main()
{
	int i;
	for (i=0; i<1; i++) {
		printf("Run %d\n", i);
		test1();
	}
	printf("Pass\n");
	exit(0);
}
