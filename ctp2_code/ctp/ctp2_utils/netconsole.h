//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
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
