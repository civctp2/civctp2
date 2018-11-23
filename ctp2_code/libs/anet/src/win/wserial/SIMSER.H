// simulated greenleaf comm system
#define	ASSUCCESS	0
#define ASBUFREMPTY -1
#define ASOVERFLOW	-2

typedef struct {
	int count;
	char buf[10240];
	int head;
	int tail;
	int status;
} PORT;

int WriteBuffer( PORT *port, char *buffer, unsigned int count);
int ReadBuffer( PORT *port, char *buffer, unsigned int size);
PORT *PortOpenGreenleafFast(int port, long baud, char parity, int wordsize, int stop );
void PortClose(PORT *port);

