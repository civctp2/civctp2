
#include "c3.h"
#include "netconsole.h"
#include "c3cmdline.h"
#include <winsock.h>

static int s_winsockInitialized = 0;

#define k_MAX_SOCK_READ 2048

NetConsole *g_netConsole = NULL;

void netconsole_Initialize()
{
	if(!g_netConsole)
		g_netConsole = new NetConsole(9999);
}

void netconsole_Cleanup()
{
	if(g_netConsole) {
		delete g_netConsole;
		g_netConsole = NULL;
	}
}

static void initWinsock()
{
	if(s_winsockInitialized) {
		s_winsockInitialized++;
		return;
	}

	WORD wVersionRequested = MAKEWORD(1, 1);        
	WSADATA wsaData;                                                        
	int nErrorStatus;                                                       

	nErrorStatus = WSAStartup(wVersionRequested, &wsaData);

	if(nErrorStatus != 0) {
		
		

		return;
	}

	
	
	if( LOBYTE(wsaData.wVersion) != LOBYTE(wVersionRequested) ||  HIBYTE(wsaData.wVersion) != HIBYTE(wVersionRequested) ) {
		
		
		
		
		WSACleanup();   
		return;
	}

	s_winsockInitialized = 1;
}

static void cleanupWinsock()
{

	if(!s_winsockInitialized)
		return;

	s_winsockInitialized--;
	if(!s_winsockInitialized)
		WSACleanup();
}

NetConsole::NetConsole(uint16 port)
{
	initWinsock();
	Assert(s_winsockInitialized);

	m_listenSock = -1;
	sint32 i;
	for(i = 0; i < k_MAX_CONNECTIONS; i++) {
		m_connections[i] = -1;
	}

	if(s_winsockInitialized) {
		m_listenSock = socket(AF_INET, SOCK_STREAM, 0);
		Assert(m_listenSock >= 0);

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);
		if(bind(m_listenSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			c3errors_ErrorDialog("NetConsole", "Can't open listen port");
			closesocket(m_listenSock);
			m_listenSock = -1;
		} else {
			if(listen(m_listenSock, 1) < 0) {
				
				c3errors_ErrorDialog("NetConsole", "Can't listen on port");
				closesocket(m_listenSock);
				m_listenSock = -1;
			}
		}
	}
}

NetConsole::~NetConsole()
{
	if(m_listenSock >= 0) {
		closesocket(m_listenSock);
		m_listenSock = -1;
	}

	sint32 i;
	for(i = 0; i < k_MAX_CONNECTIONS; i++) {
		if(m_connections[i] >= 0) {
			closesocket(m_connections[i]);
			m_connections[i] = -1;
		}
	}

	cleanupWinsock();
}

void NetConsole::Idle()
{
#ifdef _DEBUG
	fd_set readFds;
	sint32 i;
	struct timeval tv;
	sint32 maxSock = 0;

	FD_ZERO(&readFds);
	if(m_listenSock >= 0) {
		FD_SET(m_listenSock, &readFds);
		if(m_listenSock > maxSock)
			maxSock = m_listenSock;
	}

	for(i = 0; i < k_MAX_CONNECTIONS; i++) {
		if(m_connections[i] >= 0) {
			FD_SET(m_connections[i], &readFds);
			if(m_connections[i] > maxSock)
				maxSock = m_connections[i];
		}
	}

	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	sint32 st = select(maxSock + 1, &readFds, 0, 0, &tv);
	if(st >= 0) {
		if(m_listenSock >= 0 && FD_ISSET(m_listenSock, &readFds)) {
			struct sockaddr_in naddr;
			sint32 len = sizeof(naddr);
			sock_t newsock = accept(m_listenSock, (struct sockaddr *)&naddr, (int *)&len);
			sint32 whichSock;
			for(whichSock = 0; whichSock < k_MAX_CONNECTIONS; whichSock++) {
				if(m_connections[whichSock] < 0) {
					m_connections[whichSock] = newsock;
					break;
				}
			}
			if(whichSock >= k_MAX_CONNECTIONS) {
				closesocket(newsock);
			} else {
				Print("Slot %d connected\n", whichSock);
			}
		}

		for(i = 0; i < k_MAX_CONNECTIONS; i++) {
			if(m_connections[i] >= 0 && FD_ISSET(m_connections[i], &readFds)) {
				
				uint8 buf[k_MAX_SOCK_READ];
				sint32 r = recv(m_connections[i], (char *)buf, k_MAX_SOCK_READ - 1, 0);
				if(r < 0) {
					closesocket(m_connections[i]);
					m_connections[i] = -1;
				} else {
					buf[r] = 0;
					uint8 *c;
					for(c = buf; c < buf + r; c++) {
						g_commandLine.AddKey(*c);
					}
				}
			}
		}
	}
#endif
}

void NetConsole::Print(const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	Print(fmt, vl);
	va_end(vl);
}

void NetConsole::Print(const char *fmt, va_list vl)
{
	
	
	static char buf[33000];
	sint32 len;
	vsprintf((char *)buf, fmt, vl);
	len = strlen(buf);

	
	char *c = buf;
	while(*c && c < buf+len && len < k_MAX_SOCK_READ) {
		if(*c == '\n') {
			memmove(c+1, c, len - (c - buf));
			len++;
			*c = '\r';
			c++;
		}
		c++;
	}

	sint32 i;
	for(i = 0; i < k_MAX_CONNECTIONS; i++) {
		if(m_connections[i] < 0)
			continue;

		sint32 w = send(m_connections[i], buf, len, 0);
		if(w < len) {
			closesocket(m_connections[i]);
			m_connections[i] = -1;
		}
	}
}
