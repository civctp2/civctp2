#include "simser.h"
#include <stdio.h>
#include <stdlib.h>

PORT *PortOpenGreenleafFast(int port, long baud, char parity, int wordsize, int stop )
{
	PORT *p = (PORT *)malloc(sizeof(PORT));
	p->head = 0;
	p->tail = 0;
	p->count = 0;
	p->status = ASSUCCESS;
	return p;
}

void PortClose(PORT *port)
{
	free(port);
}

// For test only: if count is 1<<15, buffer is assumed to be null-terminated
int WriteBuffer( PORT *port, char *buffer, unsigned int count )
{
	if (count == 1<<15) count = strlen(buffer);  // for test only; nul-term

	if (port->tail + count >= sizeof(port->buf)) {
		printf("Overwrote simbuf\n");
		exit(1);
	}
	memcpy(port->buf+port->tail, buffer, count);
	port->tail+=count;
	port->count = count;
	port->status = ASSUCCESS;
	return (ASSUCCESS);
}

int ReadBuffer( PORT *port, char *buffer, unsigned int size)
{
	port->count = 0;
	while (1) {
		if (port->head >= port->tail) return port->status = ASBUFREMPTY;
		if (port->count == size) return port->status = ASSUCCESS;
		buffer[port->count++] = port->buf[port->head++];
	}
}

int ReadChar( PORT * port )
{
	char c;
	int status;
	status = ReadBuffer( port, &c, 1);
	if ((status == ASSUCCESS) || (status == ASOVERFLOW)) return c;
	return status;
}

#ifdef SIMSER_EXE

ch()
{
    switch( heapcheck() ) {
	case _HEAPOK:
		//printf( "OK - heap is good\n" );
		break;
	case _HEAPEMPTY:
		//printf( "OK - heap is empty\n" );
		break;
	default:
		printf( "ERROR - ?? in heap\n" );
		exit(1);
		break;
	}
}

// stupid sanity check to help flush out bugs
test1 ()
{
	char tbuf[200];
	char *p;
	char src;
	int len;
	int result;
	PORT *port;

	ch();
	port = PortOpenGreenleafFast(0, 0, 0, 0, 0);
	if (!port) { printf("fail open\n"); exit(1); }
	WriteBuffer(port, "zTEST1!TEST2!TEST3", 1<<15);
	
	result = ReadChar( port);
	if (result != 'z') { printf("fail readchar\n"); exit(1); }

	result = ReadBuffer( port, tbuf, 5);
	tbuf[port->count] = '\0';
	printf("%s result: %d\n", tbuf, result);
	if ((result != ASSUCCESS) || (strcmp(tbuf,"TEST1"))) {
		printf("fail\n");
		exit(1);
	}
	result = ReadChar( port);
	if (result != '!') { printf("fail readchar\n"); exit(1); }
	
	result = ReadBuffer( port, tbuf, 5);
	tbuf[port->count] = '\0';
	printf("%s result: %d\n", tbuf, result);
	if ((result != ASSUCCESS) || (strcmp(tbuf,"TEST2"))) {
		printf("fail\n");
		exit(1);
	}
	result = ReadChar( port);
	if (result != '!') { printf("fail readchar\n"); exit(1); }
	
	result = ReadBuffer( port, tbuf, 100);
	tbuf[port->count] = '\0';
	printf("%s result: %d\n", tbuf, result);
	if ((result != ASBUFREMPTY) || (strcmp(tbuf,"TEST3"))) {
		printf("fail\n");
		exit(1);
	}
	ch();
	PortClose(port);
	ch();
}

main()
{
	int i;
	for (i=0; i<10000; i++) {		// 10k * 10k = 100M, should fill up ram if busted
		test1();
	}
	printf("Pass\n");
	exit(0);
}

#endif
