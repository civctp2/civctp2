
#ifndef __NET_CONSOLE_H__
#define __NET_CONSOLE_H__

typedef sint32 sock_t;


#define k_MAX_CONNECTIONS 4

void netconsole_Initialize();
void netconsole_Cleanup();

class NetConsole
{
private:
	sock_t m_listenSock;
	sock_t m_connections[k_MAX_CONNECTIONS];

public:
	NetConsole(uint16 port);
	~NetConsole();

	void Idle();
	void Print(const char *fmt, ...);
	void Print(const char *fmt, va_list list);
};

extern NetConsole *g_netConsole;
	
#endif
