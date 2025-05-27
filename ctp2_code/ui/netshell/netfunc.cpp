//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Net functionality (find something better if you don't like this description ;))
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
// _DEBUG
// Generate debug information.
// __AUI_USE_SDL__
// Use SDL API calls
// WIN32
// Use MS Windows32 API calls
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added SDL support.
// - Replaced typename T in specialized template member function by the
//   the type for that the function is specialized, by Martin G�hmann.
// - Display the main thread function name in the debugger.
// - Make the Linux version loading and producing Windows compatible
//   savegames. (16-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "netfunc.h"
#include "c3files.h"
#include <algorithm>

#if defined(_DEBUG)
#include "debug.h"           // Os::SetThreadName
#endif

#ifdef __AUI_USE_SDL__
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#endif

#if defined(WIN32)
#include <ras.h>
#endif

#include <algorithm>

namespace
{
    char const  COUNT_MAX   = 255u;
}

/// Operating system dependent wrapper functions
namespace Os
{
    /// Stop the current thread
    /// \param a_Code Code to return to the environment (OS/debugger)
    inline int ExitThread(int a_Code)
    {
#if defined(WIN32)
        ::ExitThread(static_cast<DWORD>(a_Code));
        UNREACHABLE_RETURN(a_Code);
#else
        return a_Code;
#endif
    }
} // namespace Os

int adialup_autodial_enabled(void)
{
#if defined(WIN32)
	HKEY hKey;
	unsigned long werr = RegOpenKeyEx(HKEY_CURRENT_USER,
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			0, KEY_EXECUTE, &hKey);

	if (werr != ERROR_SUCCESS) {
		DPRINT(("autodial_enabled: RegOpenKeyEx(%s) fails, rCode:%d\n",
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			werr));
		return FALSE;
	}

	int enableAutodial = FALSE;
	unsigned long len = sizeof(enableAutodial);
	werr = RegQueryValueEx(hKey,
			"EnableAutodial",
			NULL, NULL, (unsigned char *)&enableAutodial, &len);
	CloseHandle(hKey);
	if (werr != ERROR_SUCCESS) {
		DPRINT(("autodial_enabled: Can't find %s in subkey %s, error %d\n",
			"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
			"EnableAutodial",
			werr));
		return FALSE;
	}

	if (!enableAutodial) {
		DPRINT(("autodial_enabled: autodial is off.\n"));
		return FALSE;
	}

	DPRINT(("autodial_enabled: autodial is on.\n"));
	return TRUE;
#else
	return FALSE;
#endif
}

#define adialup_MAXCONNS 10

#if defined(WIN32)
typedef DWORD (APIENTRY *pfnRasEnumConnections_t)(LPRASCONN, LPDWORD, LPDWORD);
typedef DWORD (APIENTRY *pfnRasGetConnectStatus_t)(HRASCONN, LPRASCONNSTATUS);
#endif

int adialup_is_active(void)
{
#if defined(WIN32)
	// Windows modem dialup connection, probably not needed today.
	HANDLE hlib = LoadLibrary("rasapi32.dll");
	if (NULL == hlib) {
		DPRINT(("adialup_is_active: can't load library rasapi32.dll\n"));
		return FALSE;
	}

	pfnRasEnumConnections_t pfnRasEnumConnections =
	    (pfnRasEnumConnections_t)GetProcAddress((HINSTANCE)hlib, "RasEnumConnectionsA");
	pfnRasGetConnectStatus_t pfnRasGetConnectStatus =
	    (pfnRasGetConnectStatus_t) GetProcAddress((HINSTANCE)hlib, "RasGetConnectStatusA");

	if (!pfnRasEnumConnections || !pfnRasGetConnectStatus) {
		DPRINT(("adialup_is_active: can't get fns from library rasapi32.dll\n"));
		FreeLibrary((HINSTANCE)hlib);
		return FALSE;
	}

	RASCONN rasconnArray[adialup_MAXCONNS];
	rasconnArray[0].dwSize = sizeof(rasconnArray[0]);
	unsigned long rasconnLen = sizeof(rasconnArray);
	DWORD cConnections = adialup_MAXCONNS;
	DWORD werr = pfnRasEnumConnections( rasconnArray, &rasconnLen,  &cConnections);
	if (werr) {
		DPRINT(("adialup_is_active: RasEnumConnections fails, err %d\n", werr));
		FreeLibrary((HINSTANCE)hlib);
		return FALSE;
	}
	DPRINT(("adialup_is_active: RasEnumConnections reports %d connections\n", cConnections));
	if (cConnections < 1) {
		FreeLibrary((HINSTANCE)hlib);
		return FALSE;
	}

	RASCONNSTATUS rasconnstatus;
	for (DWORD i = 0; i < cConnections; ++i)
	{
		rasconnstatus.dwSize = sizeof(rasconnstatus);
		werr = pfnRasGetConnectStatus( rasconnArray[i].hrasconn, &rasconnstatus);
		if (rasconnstatus.rasconnstate == RASCS_Connected) {
			DPRINT(("adialup_is_active: cnxn %d is active!\n", i));
			FreeLibrary((HINSTANCE)hlib);
			return TRUE;
		}
	}

	DPRINT(("adialup_is_active: none of the connections are active\n"));
	FreeLibrary((HINSTANCE)hlib);
#endif
	return FALSE;
}

int adialup_willdial(void)
{
	return adialup_autodial_enabled() && !adialup_is_active();
}

void NETFunc::StringMix(int c, char *mix, char *msg, ...) {
	va_list al;
	va_start(al, msg);
	strcpy(mix, msg);

	char * arg = va_arg(al, char *);
	char str[] = "% ";

	for(int i = 1; i <= c; i++) {
		str[1] = static_cast<char>('0' + i);

		char * next = strstr(mix, str);
		if(next) {
			char *tmp = strdup(next + strlen(str));
			strcpy(next + strlen(arg), tmp);
			strncpy(next, arg, strlen(arg));
			free(tmp);
		}

		arg = va_arg(al, char *);
	}

	va_end(al);
}

char *NETFunc::StringDup(char *s)
{
	return (s) ? strcpy(new char[strlen(s) + 1], s) : NULL;
}

NETFunc::Timer::Timer(void)
:
    m_start   (0),
    m_finish  (0),
    m_done    (false)
{
}

void NETFunc::Timer::Start(int d)
{
	m_start = GetTickCount();
	m_finish = m_start + d;
	m_done = false;
}

bool NETFunc::Timer::Finished(void)
{
	return m_done || (GetTickCount() >= m_finish);
}

NETFunc::MessageHandler::MessageHandler(void) {
	if(s_hCount < nf_MAX_HANDLERS)
		s_hList[s_hCount++] = this;
}

NETFunc::MessageHandler::~MessageHandler(void) {
	Unregister();
}

void NETFunc::MessageHandler::Unregister(void) {
	bool f = false;
	for(int i = 0; i < s_hCount; i++) {
		if(f)
			s_hList[i-1] = s_hList[i];
		else
			f = (s_hList[i] == this);
	}
	if(f)
		s_hCount--;
}

bool NETFunc::MessageHandler::HandleAll(Message *m) {
	bool f = false;
	for(int i = 0; i < s_hCount; i++)
		f = s_hList[i]->Handle(m) || f;
	return f;
}

NETFunc::MessageHandler *NETFunc::MessageHandler::s_hList[] = {0};
int NETFunc::MessageHandler::s_hCount = 0;

NETFunc::Message::Message(CODE c, void *p, size_t s) {
	m_newbody = true;
	m_body = new char[s + sizeof(CODE)];
	*(CODE *)m_body = c;
	memcpy(m_body + sizeof(CODE), p, s);
	m_size = s + sizeof(CODE);
	m_sender = dp_ID_NAMESERVER;
}

NETFunc::Message::Message(CODE c) {
	m_newbody = true;
	m_body = new char[sizeof(CODE)];
	*(CODE *)m_body = c;
	m_size = sizeof(CODE);
	m_sender = dp_ID_NAMESERVER;
}

NETFunc::Message::Message(void *p, size_t s, dpid_t id, bool b) {
	m_newbody = b;
	if(m_newbody) {
		m_body = new char[s];
		memcpy(m_body, p, s);
	} else
		m_body = (char *)p;
	m_size = s;
	m_sender = id;
}

NETFunc::Message::Message(void) {
	m_newbody = false;
	m_size = 0;
}

NETFunc::Message::~Message(void) {
	if(m_newbody)
		delete [] m_body;
}

dp_packetType_t *NETFunc::Message::Get(void) {
	return (dp_packetType_t *)m_body;
}

void *NETFunc::Message::GetBody(void) {
	return m_body + sizeof(CODE);
}

dpid_t NETFunc::Message::GetSender(void) {
	return m_sender;
}

size_t NETFunc::Message::GetSize(void) {
	return m_size;
}

size_t NETFunc::Message::GetBodySize(void) {
	return m_size - sizeof(CODE);
}

NETFunc::Message::CODE NETFunc::Message::GetCode(void) {
	return *(CODE *)m_body;
}


NETFunc::Messages::Messages(void) {}

NETFunc::Messages::~Messages(void) {
}

void NETFunc::Messages::Push(Message *m) {
	push_back(m);
}

NETFunc::Message *NETFunc::Messages::Pop(void) {
	Message *m = *begin();
	pop_front();
	return m;
}

NETFunc::Messages NETFunc::messages = Messages();


NETFunc::Keys::Keys(void) {
	memset(&m_curkey, 0, sizeof(KeyStruct));
	m_curkey.m_len = 1;
}

void NETFunc::Keys::NextKey(void) {
	if(m_curkey.m_buf[m_curkey.m_len-1] == COUNT_MAX)
		m_curkey.m_len++;
	m_curkey.m_buf[m_curkey.m_len-1]++;
}


NETFunc::Key::Key(void) {
	memset(&m_key, 0, sizeof(KeyStruct));
}

NETFunc::Key::Key(Key *k) {
	m_key = k->m_key;
}

NETFunc::Key::Key(KeyStruct *k) {
	m_key = *k;
}
void NETFunc::Key::WriteToFile(FILE *saveFile) const
{
	sint32 first = 0; // For bytes to the first real data
	c3files_fwrite(&first, sizeof(first), 1, saveFile);

	c3files_fwrite(&m_key.m_len, sizeof(m_key.m_len), 1, saveFile);
	c3files_fwrite(&m_key.m_buf, sizeof(m_key.m_buf), 1, saveFile);
}

void NETFunc::Key::ReadFromFile(FILE *saveFile)
{
	sint32 first = 0; // For bytes to the first real data
	c3files_fread(&first, sizeof(first), 1, saveFile);

	c3files_fread(&m_key.m_len, sizeof(m_key.m_len), 1, saveFile);
	c3files_fread(&m_key.m_buf, sizeof(m_key.m_buf), 1, saveFile);
}

bool NETFunc::Key::Equals(Key *k) {
	return memcmp(&m_key, &k->m_key, sizeof(m_key.m_len) + m_key.m_len) == 0;
}

bool NETFunc::Key::Equals(KeyStruct *k) {
	return memcmp(&m_key, k, sizeof(m_key.m_len) + m_key.m_len) == 0;
}

NETFunc::KeyStruct *NETFunc::Key::GetKey(void) {
	return &m_key;
}


NETFunc::STATUS NETFunc::EnumServers(bool b) {
	KeyStruct key;
	key.m_buf[0] = dp_KEY_SERVERPINGS;
	key.m_len = 1;
	if(!b)
		PushMessage(new Message(Message::RESET, &key, sizeof(KeyStruct)));

	key.m_buf[1] = (char) dpGETSHORT_FIRSTBYTE(s_GameType);
	key.m_buf[2] = (char) dpGETSHORT_SECONDBYTE(s_GameType);
	key.m_len = 3;
	return dpRequestObjectDeltas(s_dp, b, &key.m_buf[0], key.m_len) == dp_RES_OK ? OK : ERR;
}


NETFunc::STATUS NETFunc::EnumSessions(bool b) {
	KeyStruct key;
	key.m_buf[0] = dp_KEY_SESSIONS;
	key.m_len = 1;
	if(!b)
		PushMessage(new Message(Message::RESET, &key, sizeof(KeyStruct)));

	NETFunc::STATUS s = dpRequestObjectDeltas(s_dp, b, &key.m_buf[0], key.m_len) == dp_RES_OK ? OK : ERR;

	key.m_buf[0] = dp_KEY_PLAYER_LATENCIES;
	return dpRequestObjectDeltas(s_dp, 1, &key.m_buf[0], key.m_len) == dp_RES_OK ? s : ERR;
}


NETFunc::STATUS NETFunc::EnumPlayers(bool b, KeyStruct *k) {
	KeyStruct key;
	key.m_buf[0] = dp_KEY_PLAYERS;
	memcpy(&key.m_buf[0] + 1, &k->m_buf[0], k->m_len);
	key.m_len = 1 + k->m_len;
	Receive();
	if(!b) {
		for(Messages::iterator i = messages.begin(); i != messages.end() && !messages.empty(); i++) {

			if((*i)->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
				dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)(*i)->GetBody();
				KeyStruct key = *(KeyStruct *)(&p->keylen);

				if(key.m_buf[0] == dp_KEY_PLAYERS && memcmp(&key.m_buf[key.m_len - 2], &k->m_buf[k->m_len - 2], 2) == 0) {

					messages.Del(*i);
					i = messages.begin();
				}
			}
		}
	}
	return dpRequestObjectDeltas(s_dp, b, &key.m_buf[0], key.m_len) == dp_RES_OK ? OK : ERR;
}

NETFunc::Server::Server(void) {
	memset(&m_server, 0, sizeof(dp_serverInfo_t));
}

NETFunc::Server::Server(dp_object_t *o, KeyStruct *k, long f):Key(k) {
	m_server = o->serv;
}

NETFunc::Server::~Server(void) {
}

char *NETFunc::Server::GetName(void) {
	return m_server.hostname;
}

int NETFunc::Server::GetPlayers(void) {
	return m_server.cur_sessTypeUsers;
}

int NETFunc::Server::GetPing(void) {
	return m_server.rtt_ms_avg;
}

dp_serverInfo_t *NETFunc::Server::GetServer(void) {
	return &m_server;
}

template<>
bool NETFunc::ListHandler<NETFunc::Server>::Handle(Message *m) {
	if(m->GetCode() == Message::RESET && Equals((KeyStruct *)m->GetBody())) {
		Destroy();
		Clr();
	} else if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(Equals((KeyStruct *)&p->keylen)) {
			Server t(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
			switch(p->status) {

			case dp_RES_CREATED:

			case dp_RES_CHANGED:
				if(p->data.serv.loss_percent != 100)
					if(Find(&t) != end())
						Change(Chg(&t));
					else
						Insert(Add(new Server(t)));

				return true;

			case dp_RES_DELETED:
				Delete(&t);
				Del(&t);
				return true;
			default:
				break;
			}
		}
	}
	return false;
}

template<>
void NETFunc::ListHandler<NETFunc::Server>::SetKey(void) {
	m_key.m_buf[0] = dp_KEY_SERVERPINGS;


	m_key.m_len = 1;
}


NETFunc::Contact::Contact(char *n, char *p) {
	m_name = NETFunc::StringDup(n);
	m_number = NETFunc::StringDup(p);
}

NETFunc::Contact::Contact(void) {
}

NETFunc::Contact::~Contact(void) {
	delete m_name;
	delete m_number;
}

char *NETFunc::Contact::GetName(void) {
	return m_name;
}

char *NETFunc::Contact::GetNumber(void) {
	return m_number;
}

void NETFunc::Contact::SetName(char *n) {
	delete m_name;
	m_name = NETFunc::StringDup(n);
}

void NETFunc::Contact::SetNumber(char *p) {
	delete m_number;
	m_number = NETFunc::StringDup(p);
}


NETFunc::ContactList::ContactList(void) {

}

NETFunc::ContactList::~ContactList(void) {
}

NETFunc::Port::Port(commPortName_t *p, int b, char *i) {
	m_port = *p;
	m_baud	= b;
	strncpy(m_init, i, nf_PORTINITLEN);
}

NETFunc::Port::Port(void) {
}

NETFunc::Port::~Port(void) {
}

commPortName_t *NETFunc::Port::GetPort(void) {
	return &m_port;
}

int NETFunc::Port::GetNumber(void) {
	return m_port.portnum;
}

char *NETFunc::Port::GetName(void) {
	return m_port.name;
}

int NETFunc::Port::GetBaud(void) {
	return m_baud;
}

char *NETFunc::Port::GetInit(void) {
	return m_init;
}


NETFunc::PortList::PortList(Transport *t)
{
    commPortName_t portName[10];
    int portCount;
    dp_result_t err = dpEnumPorts(&t->m_transport , portName, 10, &portCount);
    if (err == dp_RES_OK)
    {
        for (int i = 0; i < portCount; i++)
        {
            push_back(new Port(&portName[i], 0, ""));
        }
    }
}

NETFunc::PortList::PortList(void) {
}

NETFunc::PortList::~PortList(void) {
}

NETFUNC_CONNECT_RESULT NETFunc::ConnectThread(NETFUNC_CONNECT_PARAMETER t)
{
#if defined(_DEBUG)
    Os::SetThreadName("NETFunc::ConnectThread");
#endif
    s_result = dpCreate(&s_dp, ((TransportSetup *)t)->GetTransport(), ((TransportSetup *)t)->GetParams(), 0);
    return Os::ExitThread((dp_RES_OK == s_result) ? 0 : 1);
}

NETFUNC_CONNECT_RESULT NETFunc::ReConnectThread(NETFUNC_CONNECT_PARAMETER r)
{
#if defined(_DEBUG)
    Os::SetThreadName("NETFunc::ReconnectThread");
#endif
    *((bool *)r) = false;
    dpSetActiveThread(s_dp);
    while (!(*((bool *)r)))
    {
		Receive();
    }

    return Os::ExitThread(0);
}

NETFunc::Transport::Transport(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k)
:
	Key(k),
	m_transport(*t),
	m_description(*d)
{

	memset(&m_parameters, 0, sizeof(commInitReq_t));
	m_parameters.reqLen = sizeof(commInitReq_t);
	m_parameters.sessionId = GetTickCount();
	m_parameters.dialing_method = comm_INIT_DIALING_METHOD_TONE;
	status = NOSETUP;
}

NETFunc::Transport::Transport(void) {
}

NETFunc::Transport::~Transport(void) {
}

dp_transport_t *NETFunc::Transport::GetTransport(void) {
	return &m_transport;
}

commInitReq_t *NETFunc::Transport::GetParams(void) {
	return &m_parameters;
}




NETFunc::STATUS NETFunc::Transport::GetStatus(void) {
	return status;
}

NETFunc::Transport::TYPE NETFunc::Transport::GetType(void) {
	return Transport::UNKNOWN;
}

char *NETFunc::Transport::GetName(void) {
	return m_description.name;
}

char *NETFunc::Transport::GetFileName(void) {
	return m_transport.fname;
}

NETFunc::STATUS NETFunc::Transport::SetPort(long p) {
	return BADSETUP;
}

NETFunc::STATUS NETFunc::Transport::SetPort(Port *p) {
	return BADSETUP;
}

NETFunc::STATUS NETFunc::Transport::SetContact(Contact *c) {
	return BADSETUP;
}

NETFunc::TransportSetup::TransportSetup(Transport *t) {

	m_transport = *t->GetTransport();
	m_parameters = *t->GetParams();
	status = t->GetStatus();
	m_parameters.modeministr = StringDup(t->GetParams()->modeministr);
	m_parameters.phonenum = StringDup(t->GetParams()->phonenum);
	type = t->GetType();
}

NETFunc::TransportSetup::~TransportSetup(void) {

	if ( m_parameters.modeministr )
		delete [] m_parameters.modeministr;
	if ( m_parameters.phonenum )
		delete [] m_parameters.phonenum;
}

NETFunc::STATUS NETFunc::TransportSetup::GetStatus(void) {
	return status;
}

NETFunc::Transport::TYPE NETFunc::TransportSetup::GetType(void) {
	return type;
}

dp_transport_t *NETFunc::TransportSetup::GetTransport(void) {
	return &m_transport;
}

commInitReq_t *NETFunc::TransportSetup::GetParams(void) {
	return &m_parameters;
}

NETFunc::TransportSetup *NETFunc::s_transport = 0;


NETFunc::Transport::TYPE NETFunc::GetTransportType(const comm_driverInfo_t *c) {
	if((c->needs & (comm_INIT_NEEDS_PORTNUM | comm_INIT_NEEDS_PHONENUM)) == comm_INIT_NEEDS_PORTNUM)
		return Transport::NULLMODEM;
	else if((c->needs & (comm_INIT_NEEDS_PORTNUM | comm_INIT_NEEDS_PHONENUM)) == (comm_INIT_NEEDS_PORTNUM | comm_INIT_NEEDS_PHONENUM))
		return Transport::MODEM;
	else if(c->capabilities & comm_DRIVER_NO_BROADCAST)
		return Transport::INTERNET;
	else
		return Transport::IPX;
}


NETFunc::TransportList::TransportList(void) {
	dp_transport_t transport;
	strcpy (transport.fname, s_DllPath);
	memset(&m_key, 0, sizeof(KeyStruct));
	m_key.m_len = 1;
	s_result = dpEnumTransports(&transport, CallBack, this);
}

NETFunc::TransportList::~TransportList(void) {
}

NETFUNC_CALLBACK_RESULT(void)
NETFunc::TransportList::CallBack(const dp_transport_t *t, const comm_driverInfo_t *d, void *context) {
	if (comm_DRIVER_IS_VISIBLE & d->capabilities) {
		KeyStruct *k = (KeyStruct *)&((TransportList *)context)->m_key;
		if(k->m_buf[k->m_len-1] == COUNT_MAX)
			k->m_len++;
		else
			k->m_buf[k->m_len-1]++;

		switch(GetTransportType(d)) {
		case Transport::INTERNET:
			((TransportList *)context)->Add(new Internet(d, t, k));
		break;
		case Transport::IPX:
			((TransportList *)context)->Add(new IPX(d, t, k));
		break;
		case Transport::MODEM:
			((TransportList *)context)->Add(new Modem(d, t, k));
		break;
		case Transport::NULLMODEM:
			((TransportList *)context)->Add(new NullModem(d, t, k));
		break;
		default:
		break;
		}
	}
}


NETFunc::Internet::Internet(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k):Transport(d, t, k) {
}

NETFunc::Internet::~Internet(void) {
}

NETFunc::STATUS NETFunc::Internet::SetPort(long p) {
	long o = m_parameters.portnum;
	m_parameters.portnum = p;
	return o != p ? RESET : OK;
}

NETFunc::Transport::TYPE NETFunc::Internet::GetType(void) {
	return Transport::INTERNET;
}


NETFunc::IPX::IPX(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k):Transport(d, t, k) {
	status = READY;
}

NETFunc::IPX::~IPX(void) {
}

NETFunc::Transport::TYPE NETFunc::IPX::GetType(void) {
	return Transport::IPX;
}


NETFunc::Modem::Modem(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k):Transport(d, t, k) {

	m_parameters.hwirq = 12345;
	m_parameters.swint = (long) &NETFunc::s_cancelDial;

}

NETFunc::Modem::~Modem(void) {
}

NETFunc::STATUS NETFunc::Modem::SetContact(Contact *c) {
	if(c)

		m_parameters.phonenum = c->GetNumber();
	else

		m_parameters.phonenum = "";
	status = status == NOSETUP ? NOPORT :READY;
	return OK;
}

NETFunc::STATUS NETFunc::Modem::SetPort(Port *p) {

	m_parameters.baud = p->GetBaud();
	m_parameters.portnum = p->GetNumber();
	m_parameters.modeministr = p->GetInit();
	status = status == NOSETUP ? NOCONTACT : READY;
	return OK;
}

NETFunc::Transport::TYPE NETFunc::Modem::GetType(void) {
	return Transport::MODEM;
}


NETFunc::NullModem::NullModem(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k):Transport(d, t, k) {
}

NETFunc::NullModem::~NullModem(void) {
}

NETFunc::STATUS NETFunc::NullModem::SetPort(Port *p) {

	m_parameters.baud = p->GetBaud();
	m_parameters.portnum = p->GetNumber();
	m_parameters.modeministr = "";
	return OK;
}

NETFunc::Transport::TYPE NETFunc::NullModem::GetType(void) {
	return Transport::NULLMODEM;
}











NETFunc::AIPlayer::AIPlayer(void) {
}

NETFunc::AIPlayer::~AIPlayer(void) {
}

const char *NETFunc::AIPlayer::GetName(void) {
	return m_name;
}

void NETFunc::AIPlayer::SetName(const char *n) {
	strncpy(m_name, n, dp_PNAMELEN - 1);
	m_name[dp_PNAMELEN - 1] = 0;
}

unsigned char NETFunc::AIPlayer::GetGroup(void) {
	return m_group;
}

void NETFunc::AIPlayer::SetGroup(unsigned char g) {
	m_group = g;
}

void NETFunc::AIPlayer::SetKey(KeyStruct *k) {
	m_key = *k;
}

void NETFunc::AIPlayer::Pack(void) {
	Clear();
	Push(m_key.m_buf);
	Push(m_name);
	Push(m_group);
}

void NETFunc::AIPlayer::Unpack(void) {
	m_first = m_body;
	Pop(m_key.m_buf);
	m_key.m_len = static_cast<short>(strlen(m_key.m_buf));
	Pop(m_name);
	Pop(m_group);
}

NETFunc::STATUS NETFunc::AIPlayer::Load(FILE *f) {
	if(fread(&(Packet::m_size), sizeof(SizeT), 1, f) == 1)
		if(fread(m_body, Packet::m_size, 1, f) == 1) {
			Unpack();
			return OK;
		}
	return ERR;
}

NETFunc::STATUS NETFunc::AIPlayer::Save(FILE *f) {
	Pack();
	if(fwrite(&(Packet::m_size), sizeof(SizeT), 1, f) == 1)
		if(fwrite(m_body, Packet::m_size, 1, f) == 1)
			return OK;
	return ERR;
}


NETFunc::STATUS NETFunc::AIPlayers::Send(dp_t *p, dpid_t id, dpid_t from) {
	for(iterator i=begin(); i!=end(); i++)
	{
		Message message = Message(Message::ADDAIPLAYER, (*i)->GetBody(), (*i)->GetSize());
		if(NETFunc::Send(p, &message, id, from) != OK)
			return ERR;
	}
	return OK;
}


NETFunc::AIPlayers::AIPlayers(AIPlayers *l):NETFunc::List<NETFunc::AIPlayer>(l) {
}

bool NETFunc::AIPlayers::Handle(Message *m) {
	AIPlayer t;
	if(m->GetCode() == Message::ENTERGAME) {
		Clr();
		return true;
	} else if(m->GetCode() == Message::ADDAIPLAYER) {
		t.Set(m->GetBodySize(), m->GetBody());

		Add(new AIPlayer(t));
		return true;
	} else if(m->GetCode() == Message::DELAIPLAYER) {
		t.Set(m->GetBodySize(), m->GetBody());

		Del(&t);
		return true;
	} else if(m->GetCode() == Message::CHGAIPLAYER) {
		t.Set(m->GetBodySize(), m->GetBody());

		Chg(&t);
		return true;
	}
	return false;
}

bool NETFunc::AIPlayers::Handle(dp_t *p, Message *m, dpid_t from) {
	if(m->GetCode() == Message::PLAYERENTER) {
		Send(p, m->GetSender(), from);
		return true;
	}
	return false;
}

template<>
bool NETFunc::ListHandler<NETFunc::AIPlayer>::Handle(Message *m) {
	return false;
}

template<>
void NETFunc::ListHandler<NETFunc::AIPlayer>::SetKey(void) {
}
















void NETFunc::Player::SetGroupMaster(bool b) {
	if(m_player.bloblen == 0) {
		m_player.bloblen = 1;
		m_player.blob[0] = 0;
	}
	if(b)
		m_player.blob[0] |= nf_GROUPMASTER;
	else
		m_player.blob[0] &= ~nf_GROUPMASTER;
}

void NETFunc::Player::SetKey(void) {
	m_key.m_buf[0] = (char) dpGETSHORT_FIRSTBYTE(m_player.id);
	m_key.m_buf[1] = (char) dpGETSHORT_SECONDBYTE(m_player.id);
	m_key.m_len = 2;
}

NETFunc::Player::Player(void) {
	memset(&m_player, 0, sizeof(dp_playerId_t));
	m_player.bloblen = 1;
	SetKey();
	m_flags = 0;
	m_muted = false;
}

NETFunc::Player::Player(dp_object_t *o, KeyStruct *k, long f, short l):Key(k) {
	m_player = o->p;
	SetKey();
	m_flags = f;
	m_latency = l;
	m_muted = false;
}

void NETFunc::Player::Set(dp_playerId_t *p) {
	m_player = *p;
}

NETFunc::Player::~Player(void) {
}

dpid_t NETFunc::Player::GetId(void) {
	return m_player.id;
}

char *NETFunc::Player::GetName(void) {
	return m_player.name;
}

unsigned char *NETFunc::Player::GetBlob(void) {
	return &m_player.blob[1];
}

unsigned char NETFunc::Player::GetBlobLen(void) {
	if(m_player.bloblen == 0)
		return 0;
	return m_player.bloblen - 1;
}

unsigned char NETFunc::Player::GetGroup(void) {
	if(m_player.bloblen == 0)
		return 0;
	return (m_player.blob[0] & nf_GROUPNUMBER);
}

short NETFunc::Player::GetLatency(void) {
	return m_latency;
}

bool NETFunc::Player::IsMe(void) {
	return (m_flags & dp_OBJECTDELTA_FLAG_LOCAL) != 0;
}

bool NETFunc::Player::IsMuted(void) {
	return m_muted;
}

void NETFunc::Player::SetMuted(bool m) {
	m_muted = m;
}

bool NETFunc::Player::IsInCurrentSession(void) {
	return (m_flags & dp_OBJECTDELTA_FLAG_INOPENSESS) != 0;
}

bool NETFunc::Player::IsHost(void) {
	return (m_flags & dp_OBJECTDELTA_FLAG_ISHOST) != 0;
}

bool NETFunc::Player::IsGroupMaster(void) {
	return m_player.bloblen > 0 && GetGroup() && m_player.blob[0] & nf_GROUPMASTER;
}

bool NETFunc::Player::IsReadyToLaunch(void) {
	return m_player.bloblen > 0 && m_player.blob[0] & nf_READYLAUNCH;
}

NETFunc::Player NETFunc::s_player = Player();
dp_uid_t NETFunc::s_userId = dp_UID_NONE;

template<>
bool NETFunc::ListHandler<NETFunc::Player>::Handle(Message *m) {
	if(m->GetCode() == Message::RESET) {
		Destroy();
		Clr();
	} else if(m->GetCode() == Message::UPDATE && Equals((KeyStruct *)m->GetBody())) {
		KeyStruct *k = ((KeyStruct *)m->GetBody()) + 1;
		Key key = Key(k);
		for(iterator i=begin(); i!=end(); i++)
			if((*i)->Equals(k)) {
				(*i)->SetMuted(IsMuted(&key));
				Change(Chg(*i));
			}
	} else if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(Equals((KeyStruct *)&p->keylen)) {
			Player t(&p->data, (KeyStruct *)&p->subkeylen, p->flags, p->latency);
			t.SetMuted(IsMuted(&t));
			switch(p->status) {

			case dp_RES_CREATED:

			case dp_RES_CHANGED:
				if(Find(&t) != end())
					Change(Chg(&t));
				else
					Insert(Add(new Player(t)));
				return true;

			case dp_RES_DELETED:
				Delete(&t);
				Del(&t);
				return true;
			default:
				break;
			}
		}
	}
	return false;
}

template<>
void NETFunc::ListHandler<NETFunc::Player>::SetKey(void) {
	m_key.m_buf[0] = dp_KEY_PLAYERS;
	m_key.m_len = 1;
}




NETFunc::PlayerStat::PlayerStat(void) {
	m_key.m_len = sizeof(dpid_t);
	m_name[0] = 0;
	m_group = 0;
	m_isingame = true;
	m_hasleft = false;
}

NETFunc::PlayerStat::~PlayerStat(void) {
}

char *NETFunc::PlayerStat::GetName(void) {
	return m_name;
}

void NETFunc::PlayerStat::SetName(char *n) {
	strncpy(m_name, n, dp_PNAMELEN - 1);
	m_name[dp_PNAMELEN - 1] = 0;
}

unsigned char NETFunc::PlayerStat::GetGroup(void) {
	return m_group;
}

void NETFunc::PlayerStat::SetGroup(unsigned char g) {
	m_group = g;
}

void NETFunc::PlayerStat::SetId(dpid_t i) {
	*(dpid_t *)&m_key.m_buf = i;
}

dpid_t NETFunc::PlayerStat::GetId(void) {
	return 	*(dpid_t *)&m_key.m_buf;
}

void NETFunc::PlayerStat::SetInGame(bool b) {
	m_isingame = b;
	*((bool *)GetBody() + 1) = b;
}

bool NETFunc::PlayerStat::IsInGame(void) {
	return m_isingame;
}

void NETFunc::PlayerStat::SetLeft(bool b) {
	m_hasleft = b;
	*(bool *)GetBody() = b;
}

bool NETFunc::PlayerStat::HasLeft(void) {
	return m_hasleft;
}

void NETFunc::PlayerStat::Pack(void) {
	Clear();
	Push(m_hasleft);
	Push(m_isingame);
	Push(*(dpid_t *)&m_key.m_buf);
	Push(m_name);
	Push(m_group);
}

void NETFunc::PlayerStat::Unpack(void) {
	m_first = m_body;
	Pop(m_hasleft);
	Pop(m_isingame);
	Pop(*(dpid_t *)&m_key.m_buf);
	Pop(m_name);
	Pop(m_group);
}

NETFunc::STATUS NETFunc::PlayerStat::Update(dp_t *p, bool r) {
	m_hasleft = false;
	m_isingame = true;
	Pack();
	Message message = Message(Message::ADDPLAYERSTAT, GetBody(), GetSize());
	if(NETFunc::Send(p, &message, dp_ID_BROADCAST, dp_ID_BROADCAST, r) != OK)
		return ERR;
	return OK;
}


NETFunc::PlayerStats::PlayerStats(PlayerStats *l):NETFunc::List<NETFunc::PlayerStat>(l) {
}

NETFunc::STATUS NETFunc::PlayerStats::Send(dp_t *p, dpid_t id, dpid_t from) {
	for(iterator i=begin(); i!=end(); i++)
	{
		Message message = Message(Message::ADDPLAYERSTAT, (*i)->GetBody(), (*i)->GetSize());
		if(NETFunc::Send(p, &message, id) != OK)
			return ERR;
	}
	return OK;
}

void NETFunc::PlayerStats::Left(dpid_t id) {
	PlayerStat t;
	t.SetId(id);
	iterator i = Find(&t);
	if(i != end()) {
		t = **i;
		t.SetLeft(true);
		Chg(&t);
		if(s_status != RESET)
			PushMessage(new Message(Message::CHGPLAYERSTAT, t.GetBody(), t.GetSize()));
	}
}

bool NETFunc::PlayerStats::Handle(Message *m) {
	PlayerStat t;
	if((m->GetCode() == Message::ENTERGAME && IsHost()) || (m->GetCode() == Message::GAMESESSION && !GotGameSetup())) {
		Clr();
		return true;
	} else if(m->GetCode() == Message::ADDPLAYERSTAT) {
		t.Set(m->GetBodySize(), m->GetBody());
		t.SetInGame(false);

		if(Find(&t) == end())

			Add(new PlayerStat(t));
		else {

			Chg(&t);
			*(Message::CODE *)(m->Get()) = Message::CHGPLAYERSTAT;
		}
		return true;
	} else if(m->GetCode() == Message::DELPLAYERSTAT) {
		t.Set(m->GetBodySize(), m->GetBody());

		Del(&t);
		return true;
	} else if(m->GetCode() == dp_USER_DELPLAYER_PACKET_ID) {
		Left(((dp_playerId_t *)m->GetBody())->id);
		return true;
	} else if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if((p->key[0] == dp_KEY_PLAYERS) && (p->flags & dp_OBJECTDELTA_FLAG_INOPENSESS) && (p->status == dp_RES_DELETED)) {
			Left(p->data.p.id);
			return true;
		}
	}
	return false;
}

bool NETFunc::PlayerStats::Handle(dp_t *p, Message *m, dpid_t from) {
	if(m->GetCode() == Message::PLAYERENTER) {
		Send(p, m->GetSender(), from);
		return true;
	}

	return false;
}




NETFunc::PlayerSetup::PlayerSetup(Player *p):Player(*p) {
	m_description[0] = 0;
}

NETFunc::PlayerSetup::PlayerSetup() {
	m_description[0] = 0;
}

NETFunc::PlayerSetup::~PlayerSetup() {
}

void NETFunc::PlayerSetup::SetName(char *n) {
	strncpy(m_player.name, n, dp_PNAMELEN);
	m_player.name[dp_PNAMELEN - 1] = 0;
}

void NETFunc::PlayerSetup::SetBlob(char *b) {
	SetBlob((void *)b, strlen(b) + 1);
}

void NETFunc::PlayerSetup::SetBlob(void *b, size_t s) {
    s = std::min<size_t>(s, dp_MAX_PLAYERBLOB_LEN - 1);
	memcpy(&m_player.blob[1], b, s);
	m_player.bloblen = static_cast<unsigned char>(s + 1);
}

void NETFunc::PlayerSetup::SetBlobLen(unsigned char l) {
	if(l < dp_MAX_PLAYERBLOB_LEN)
		m_player.bloblen = l + 1;
}

char *NETFunc::PlayerSetup::GetDescription(void) {
	return m_description;
}

void NETFunc::PlayerSetup::SetDescription(char *d) {
	strncpy(m_description, d, nf_PLAYERDESCLEN);
}

void NETFunc::PlayerSetup::SetGroup(char group) {
	if(m_player.bloblen == 0) {
		m_player.bloblen = 1;
		m_player.blob[0] = 0;
	}
	m_player.blob[0] &= ~nf_GROUPNUMBER;
	m_player.blob[0] |= (group & nf_GROUPNUMBER);
}

void NETFunc::PlayerSetup::SetReadyToLaunch(bool b) {
	if(m_player.bloblen == 0) {
		m_player.bloblen = 1;
		m_player.blob[0] = 0;
	}
	if(b)
		m_player.blob[0] |= nf_READYLAUNCH;
	else
		m_player.blob[0] &= ~nf_READYLAUNCH;
}

void NETFunc::PlayerSetup::Pack() {
	Clear();
	Push(m_description);
}

void NETFunc::PlayerSetup::Unpack() {
	m_first = m_body;
	Pop(m_description);
}

NETFunc::STATUS NETFunc::PlayerSetup::Load(FILE *f) {
	if(fread(&m_player, sizeof(dp_playerId_t), 1, f) == 1)
		if(fread(&(Packet::m_size), sizeof(SizeT), 1, f) == 1)
			if(fread(m_body, Packet::m_size, 1, f) == 1) {
				Unpack();
				return OK;
			}
	return ERR;
}

NETFunc::STATUS NETFunc::PlayerSetup::Save(FILE *f) {
	if(fwrite(&m_player, sizeof(dp_playerId_t), 1, f) == 1) {
		Pack();
		if(fwrite(&(Packet::m_size), sizeof(SizeT), 1, f) == 1)
			if(fwrite(m_body, Packet::m_size, 1, f) == 1)
				return OK;
	}
	return ERR;
}

NETFunc::STATUS NETFunc::PlayerSetup::Update(void) {
	Pack();
	if(SetPlayerSetupPlayer(this) != OK)
		return ERR;
	if(SetPlayerSetupPacket(this) != OK)
		return ERR;
	return OK;
}



































void NETFunc::Session::SetKey() {
	int i;
	dpGetSessionId(s_dp, &m_session, &m_key.m_buf[0], &i);

	m_key.m_len = static_cast<short>(i);
}

NETFunc::Session::Session(void) {
	memset(&m_session, 0, sizeof(dp_session_t));
	m_session.dwSize = sizeof(dp_session_t);
	m_session.maxPlayers = dp_MAXREALPLAYERS;
	m_session.flags = dp_SESSION_FLAGS_CREATESESSION | dp_SESSION_FLAGS_ENABLE_PLAYERVARS;
	m_session.guidApplication = s_GameType;
}

NETFunc::Session::Session(dp_object_t *o, KeyStruct *k, long f):Key(k) {
	m_session = o->sess;
	m_flags = f;
}

void NETFunc::Session::WriteToFile(FILE *saveFile) const
{
	NETFunc::Key::WriteToFile(saveFile);
	sint32 tmp = 0;

	c3files_fwrite(&m_session.dwSize, sizeof(m_session.dwSize), 1, saveFile);
	c3files_fwrite(&m_session.adrMaster, sizeof(m_session.adrMaster), 1, saveFile); // 10
	c3files_fwrite(&m_session.masterPseudoplayer, sizeof(m_session.masterPseudoplayer), 1, saveFile);
	c3files_fwrite(&m_session.sessionType, sizeof(m_session.sessionType), 1, saveFile);
	c3files_fwrite(&m_session.dwSession, sizeof(m_session.dwSession), 1, saveFile);
	c3files_fwrite(&m_session.dwMaxPlayers, sizeof(m_session.dwMaxPlayers), 1, saveFile);
	c3files_fwrite(&m_session.dwCurrentPlayers, sizeof(m_session.dwCurrentPlayers), 1, saveFile);
	c3files_fwrite(&m_session.dwFlags, sizeof(m_session.dwFlags), 1, saveFile);
	c3files_fwrite(&m_session.sessionName, sizeof(m_session.sessionName), 1, saveFile);
	c3files_fwrite(&m_session.szUserField, sizeof(m_session.szUserField), 1, saveFile);
	c3files_fwrite(&m_session.szPassword, sizeof(m_session.szPassword), 1, saveFile);

	tmp = static_cast<sint32>(m_session.dwReserved1); // Long 4 byte or 8 byte
	c3files_fwrite(&tmp, sizeof(tmp), 1, saveFile);

	tmp = static_cast<sint32>(m_session.dwUser1); // Long 4 byte or 8 byte
	c3files_fwrite(&tmp, sizeof(tmp), 1, saveFile);

	c3files_fwrite(&m_session.reserved2, sizeof(m_session.reserved2), 1, saveFile);

	sint8 space = 0; // Spacing in the original implementation
	c3files_fwrite(&space, sizeof(space), 1, saveFile);

	tmp = static_cast<sint32>(m_flags);
	c3files_fwrite(&tmp, sizeof(tmp), 1, saveFile); // Long 4 byte or 8 byte
}

void NETFunc::Session::ReadFromFile(FILE *saveFile)
{
	NETFunc::Key::ReadFromFile(saveFile);
	sint32 tmp = 0;

	c3files_fread(&m_session.dwSize, sizeof(m_session.dwSize), 1, saveFile);
	c3files_fread(&m_session.adrMaster, sizeof(m_session.adrMaster), 1, saveFile);
	c3files_fread(&m_session.masterPseudoplayer, sizeof(m_session.masterPseudoplayer), 1, saveFile);
	c3files_fread(&m_session.sessionType, sizeof(m_session.sessionType), 1, saveFile);
	c3files_fread(&m_session.dwSession, sizeof(m_session.dwSession), 1, saveFile);
	c3files_fread(&m_session.dwMaxPlayers, sizeof(m_session.dwMaxPlayers), 1, saveFile);
	c3files_fread(&m_session.dwCurrentPlayers, sizeof(m_session.dwCurrentPlayers), 1, saveFile);
	c3files_fread(&m_session.dwFlags, sizeof(m_session.dwFlags), 1, saveFile);
	c3files_fread(&m_session.sessionName, sizeof(m_session.sessionName), 1, saveFile);
	c3files_fread(&m_session.szUserField, sizeof(m_session.szUserField), 1, saveFile);
	c3files_fread(&m_session.szPassword, sizeof(m_session.szPassword), 1, saveFile);

	c3files_fread(&tmp, sizeof(tmp), 1, saveFile);
	m_session.dwReserved1 = tmp; // Long 4 byte or 8 byte, for that the anet code must be fixed
	c3files_fread(&tmp, sizeof(tmp), 1, saveFile);
	m_session.dwUser1 = tmp; // Long 4 byte or 8 byte
	c3files_fread(&m_session.reserved2, sizeof(m_session.reserved2), 1, saveFile);

	sint8 space = 0; // Spacing in the original implementation
	c3files_fread(&space, sizeof(space), 1, saveFile);

	c3files_fread(&tmp, sizeof(tmp), 1, saveFile);
	m_flags = tmp; // Long 4 byte or 8 byte
}

char *NETFunc::Session::GetName(void) {
	return m_session.sessionName;
}

char *NETFunc::Session::GetPassword(void) {
	return m_session.szPassword;
}

short NETFunc::Session::GetPlayers(void) {
	return m_session.currentPlayers;
}

short NETFunc::Session::GetMaxPlayers(void) {
	return m_session.maxPlayers;
}

short NETFunc::Session::GetFree() {
	return GetMaxPlayers() - GetPlayers();
}

char *NETFunc::Session::GetUserField() {
	return m_session.szUserField;
}

dp_karma_t NETFunc::Session::GetKarma() {
	return m_session.karma;
}

bool NETFunc::Session::IsLobby() {
	return (m_session.flags & dp_SESSION_FLAGS_ISLOBBY) != 0;
}

bool NETFunc::Session::IsServer() {
	return (m_session.flags & dp_SESSION_FLAGS_ISSERVER) != 0;
}

bool NETFunc::Session::IsClosed() {
	return (m_session.flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS) == 0;
}

bool NETFunc::Session::IsMine(void) {
	return (m_flags & dp_OBJECTDELTA_FLAG_LOCAL) != 0;
}

bool NETFunc::Session::IsCurrentSession(void) {
	return (m_flags & dp_OBJECTDELTA_FLAG_INOPENSESS) != 0;
}

NETFunc::Session NETFunc::m_session = Session();

template<>
bool NETFunc::ListHandler<NETFunc::Session>::Handle(Message *m) {
	if(m->GetCode() == Message::RESET && Equals((KeyStruct *)m->GetBody())) {
		Destroy();
		Clr();
	} else if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(Equals((KeyStruct *)&p->keylen)) {
			Session t(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
			if(t.m_session.sessionType != s_GameType)
				return false;
			switch(p->status) {

			case dp_RES_CREATED:

			case dp_RES_CHANGED:
				if(Find(&t) != end())
					Change(Chg(&t));
				else
					Insert(Add(new Session(t)));
				return true;

			case dp_RES_DELETED:
				Delete(&t);
				Del(&t);
				return true;
			default:
				break;
			}
		}
	}
	return false;
}

template<>
void NETFunc::ListHandler<NETFunc::Session>::SetKey(void) {
	m_key.m_buf[0] = dp_KEY_SESSIONS;
	m_key.m_len = 1;
}


NETFunc::Game::Game(void): Session() {
	m_hostile = false;
}

NETFunc::Game::Game(dp_object_t *o, KeyStruct *k, long f): Session(o, k, f) {
	m_hostile = false;
}

NETFunc::Game::Game(Session *s): Session(*s) {
	m_hostile = false;
}

void NETFunc::Game::WriteToFile(FILE *saveFile) const
{
	NETFunc::Session::WriteToFile(saveFile);
	c3files_fwrite(&m_hostile, sizeof(m_hostile), 1, saveFile);
}

void NETFunc::Game::ReadFromFile(FILE *saveFile)
{
	NETFunc::Session::ReadFromFile(saveFile);
	c3files_fread(&m_hostile, sizeof(m_hostile), 1, saveFile);
}

void NETFunc::Packet::WriteToFile(FILE *saveFile) const
{
	sint32 space = 0; // Add the space from the original implementation
	c3files_fwrite(&space, sizeof(space), 1, saveFile);

	c3files_fwrite(&m_size, sizeof(m_size), 1, saveFile);
	c3files_fwrite(&m_body, sizeof(m_body), 1, saveFile);

	sint8 littleSpace = 0;
	c3files_fwrite(&littleSpace, sizeof(littleSpace), 1, saveFile);
	c3files_fwrite(&littleSpace, sizeof(littleSpace), 1, saveFile);
	c3files_fwrite(&littleSpace, sizeof(littleSpace), 1, saveFile);
	// Has another size on a 64 bit system, pointers cannot be saved this way anyway
//	c3files_fwrite(&first, sizeof(first), 1, saveFile); // Problem
	sint32 replace = 0;
	c3files_fwrite(&replace, sizeof(replace), 1, saveFile); // Problem
}

void NETFunc::Packet::ReadFromFile(FILE *saveFile)
{
	sint32 space = 0; // Add the space from the original implementation
	c3files_fread(&space, sizeof(space), 1, saveFile);

	c3files_fread(&m_size, sizeof(m_size), 1, saveFile);
	c3files_fread(&m_body, sizeof(m_body), 1, saveFile);

	sint8 littleSpace = 0;
	c3files_fread(&littleSpace, sizeof(littleSpace), 1, saveFile);
	c3files_fread(&littleSpace, sizeof(littleSpace), 1, saveFile);
	c3files_fread(&littleSpace, sizeof(littleSpace), 1, saveFile);
	// Has another size on a 64 bit system, pointers cannot be saved this way anyway
//	c3files_fread(&first, sizeof(first), 1, saveFile); // Problem
	sint32 replace = 0;
	c3files_fread(&replace, sizeof(replace), 1, saveFile); // Problem

	m_first = m_body + m_size;
}


char NETFunc::Game::GetGroups(void)
{
	return static_cast<char>(m_session.dwUser1 & nf_GROUPNUMBER);
}

void NETFunc::Game::Set(dp_session_t *s)
{
	m_session = *s;
}

bool NETFunc::Game::IsLaunched(void) {
	return (m_session.dwUser1 & nf_LAUNCHED) != 0;
}

bool NETFunc::Game::IsSyncLaunch(void) {
	return (m_session.dwUser1 & nf_SYNCLAUNCH) != 0;
}

bool NETFunc::Game::IsHostile(void) {
	return m_hostile;
}

void NETFunc::Game::SetHostile(bool h) {
	m_hostile = h;
}


NETFunc::Lobby::Lobby(void): Session(), m_bad(false) {
	strncpy(m_session.sessionName, s_LobbyName, dp_SNAMELEN);
	m_session.flags |= dp_SESSION_FLAGS_ISLOBBY;
}

NETFunc::Lobby::Lobby(dp_object_t *o, KeyStruct *k, long f): Session(o, k, f), m_bad(false) {
}

void NETFunc::Lobby::SetBad(bool b) {
	m_bad = b;
}

bool NETFunc::Lobby::IsBad(void) {
	return m_bad;
}

NETFunc::Lobby NETFunc::s_lobby = Lobby();

template<>
bool NETFunc::ListHandler<NETFunc::Lobby>::Handle(Message *m) {
	if(m->GetCode() == Message::RESET && Equals((KeyStruct *)m->GetBody())) {
		Destroy();
		Clr();
	} else if(m->GetCode() == Message::LOBBYDELTAPACKET) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(Equals((KeyStruct *)&p->keylen)) {
			Lobby t(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
			if(t.m_session.sessionType != s_GameType)
				return false;
			switch(p->status) {

			case dp_RES_CREATED:

			case dp_RES_CHANGED:
				if(Find(&t) != end())
					Change(Chg(&t));
				else
					Insert(Add(new Lobby(t)));
				return true;

			case dp_RES_DELETED:
				Delete(&t);
				Del(&t);
				return true;
			default:
				break;
			}
		}
	}
	return false;
}

template<>
void NETFunc::ListHandler<NETFunc::Lobby>::SetKey(void) {
	m_key.m_buf[0] = dp_KEY_SESSIONS;
	m_key.m_len = 1;
}

template<>
bool NETFunc::ListHandler<NETFunc::Game>::Handle(Message *m) {
	if(m->GetCode() == Message::RESET && Equals((KeyStruct *)m->GetBody())) {
		Destroy();
		Clr();
	} else if(m->GetCode() == Message::UPDATE && Equals((KeyStruct *)m->GetBody())) {
		KeyStruct *k = ((KeyStruct *)m->GetBody()) + 1;
		Key key = Key(k);
		for(iterator i=begin(); i!=end(); i++)
			if((*i)->Equals(k)) {
				(*i)->SetHostile(IsHostile(&key));
				Change(Chg(*i));
			}
	} else if(m->GetCode() == Message::GAMEDELTAPACKET) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(Equals((KeyStruct *)&p->keylen)) {
			Game t(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
			if(t.m_session.sessionType != s_GameType)
				return false;
			t.SetHostile(IsHostile(&t));
			switch(p->status) {

			case dp_RES_CREATED:

			case dp_RES_CHANGED:
				if(Find(&t) != end())
					Change(Chg(&t));
				else
					Insert(Add(new Game(t)));
				return true;

			case dp_RES_DELETED:
				Delete(&t);
				Del(&t);
				return true;
			default:
				break;
			}
		}
	}
	return false;
}

template<>
void NETFunc::ListHandler<NETFunc::Game>::SetKey(void) {
	m_key.m_buf[0] = dp_KEY_SESSIONS;
	m_key.m_len = 1;
}


NETFunc::PlayerList::PlayerList(void) {
	s_count++;
	if(!s_players)
		s_players = new Players();
}

NETFunc::PlayerList::~PlayerList() {
	s_count--;
	if(!s_count) {
		delete s_players;
		s_players = 0;
	}
}

void NETFunc::PlayerList::SetKey(KeyStruct *k) {
	if(s_players)
		s_players->SetKey(k);
}

NETFunc::Player *NETFunc::PlayerList::FindPlayer(dpid_t id) {
	Players::iterator i;
	for(i = s_players->begin(); i!=s_players->end(); i++)
		if((*i)->GetId() == id)
			return *i;
	return 0;
};

NETFunc::PlayerList::Players *NETFunc::PlayerList::s_players = 0;
int NETFunc::PlayerList::s_count = 0;

NETFunc::GameSetup::GameSetup() {
	m_description[0] = 0;
}

NETFunc::GameSetup::GameSetup(Game *g):Game(*g), Packet() {
	m_description[0] = 0;
}

NETFunc::GameSetup::~GameSetup() {
}

void NETFunc::GameSetup::WriteToFile(FILE *saveFile) const
{
	NETFunc::Game::WriteToFile(saveFile);
	sint8 space = 0; // Add the space from the original implementation
	c3files_fwrite(&space, sizeof(space), 1, saveFile);
	c3files_fwrite(&space, sizeof(space), 1, saveFile);
	c3files_fwrite(&space, sizeof(space), 1, saveFile);
	NETFunc::Packet::WriteToFile(saveFile);
	c3files_fwrite(&m_description, sizeof(m_description), 1, saveFile);
}

void NETFunc::GameSetup::ReadFromFile(FILE *saveFile)
{
	NETFunc::Game::ReadFromFile(saveFile);
	sint8 space = 0; // Add the space from the original implementation
	c3files_fread(&space, sizeof(space), 1, saveFile);
	c3files_fread(&space, sizeof(space), 1, saveFile);
	c3files_fread(&space, sizeof(space), 1, saveFile);
	NETFunc::Packet::ReadFromFile(saveFile);
	c3files_fread(&m_description, sizeof(m_description), 1, saveFile);
}

char *NETFunc::GameSetup::GetDescription(void) {
	return m_description;
}

void NETFunc::GameSetup::SetDescription(char *d) {
	strncpy(m_description, d, nf_GAMEDESCLEN);
}

void NETFunc::GameSetup::SetGroups(char groups) {
	m_session.dwUser1 &= ~nf_GROUPNUMBER;
	m_session.dwUser1 |= (groups & nf_GROUPNUMBER);
}

void NETFunc::GameSetup::SetName(char *n) {
	strncpy(m_session.sessionName, n, dp_SNAMELEN);
}

void NETFunc::GameSetup::SetPassword(char *p) {
	strncpy(m_session.szPassword, p, dp_PASSWORDLEN);
}

void NETFunc::GameSetup::SetSize(short s) {
	m_session.maxPlayers = s;
}

void NETFunc::GameSetup::SetUserField(char *u) {
	SetUserField((void *)u, strlen(u) + 1);
}

void NETFunc::GameSetup::SetUserField(void *u, size_t s) {
	memcpy(m_session.szUserField, u, std::min<size_t>(s, dp_USERFIELDLEN));
}

void NETFunc::GameSetup::SetLaunched(bool b) {
	if(b)
		m_session.dwUser1 |= nf_LAUNCHED;
	else
		m_session.dwUser1 &= ~nf_LAUNCHED;
}

void NETFunc::GameSetup::SetSyncLaunch(bool s) {
	if(s)
		m_session.dwUser1 |= nf_SYNCLAUNCH;
	else
		m_session.dwUser1 &= ~nf_SYNCLAUNCH;
}

void NETFunc::GameSetup::SetClosed(bool c) {
	if(c)
		m_session.flags &= ~dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
	else
		m_session.flags |= dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
}

void NETFunc::GameSetup::Pack() {
	Clear();
	Push(m_description);
}

void NETFunc::GameSetup::Unpack() {
	m_first = m_body;
	Pop(m_description);
}

NETFunc::STATUS NETFunc::GameSetup::Send(dp_t *p, dpid_t id, dpid_t from) {
	Message message = Message(Message::GAMESESSION, &m_session, sizeof(dp_session_t));
	if(NETFunc::Send(p, &message, id, from) != OK)
		return ERR;
	message = Message(Message::GAMEPACKET, GetBody(), GetSize());
	if(NETFunc::Send(p, &message, id, from) != OK)
		return ERR;
	return OK;
}

bool NETFunc::GameSetup::Handle(Message *m) {
	if(m->GetCode() == Message::GAMESESSION) {
		Game::Set((dp_session_t *)m->GetBody());
		s_gotgame = true;
		return true;
	} else if(m->GetCode() == Message::GAMEPACKET) {
		Packet::Set(m->GetBodySize(), m->GetBody());
		s_gotgamepacket = true;
		return true;
	}
	return false;
}

bool NETFunc::GameSetup::Handle(dp_t *p, Message *m, dpid_t from) {
	if(m->GetCode() == Message::PLAYERENTER) {
		Send(p, m->GetSender(), from);
		return true;
	}
	return false;
}

NETFunc::STATUS NETFunc::GameSetup::Load(FILE *f) {
	if(fread(&m_session, sizeof(dp_session_t), 1, f) == 1)
		if(fread(&(Packet::m_size), sizeof(SizeT), 1, f) == 1)
			if(fread(m_body, Packet::m_size, 1, f) == 1) {
				Unpack();
				return OK;
			}
	return ERR;
}

NETFunc::STATUS NETFunc::GameSetup::Save(FILE *f) {
	if(fwrite(&m_session, sizeof(dp_session_t), 1, f) == 1) {
		Pack();
		if(fwrite(&(Packet::m_size), sizeof(SizeT), 1, f) == 1)
			if(fwrite(m_body, Packet::m_size, 1, f) == 1)
				return OK;
	}
	return ERR;
}

NETFunc::STATUS NETFunc::GameSetup::Update(bool b) {
	Pack();
	if(b)
		NETFunc::UnLaunchAll();
	if(SetGameSetupSession(this) != OK)
		return ERR;
	if(SetGameSetupPacket(this) != OK)
		return ERR;
	return OK;
}




NETFunc::STATUS	NETFunc::Chat::Send(Player *p, char *m) {
	size_t size = strlen(m) + 1;
	char buffer[dpio_MAXLEN_UNRELIABLE];
	*(TYPE *)buffer = PUBLIC;
	memcpy(buffer + sizeof(TYPE), m , size);
	size += sizeof(TYPE);
	dpid_t id = dp_ID_BROADCAST;
	if(p) {
		id = p->GetId();
		*(TYPE *)buffer = PRIVATE;
	}
	if(GetStatus() != OK)
		return BUSSY;
	else {
		Receive(&s_player, *(TYPE *)buffer, m);
		Message message = Message(Message::CHAT, buffer, size);
		return NETFunc::Send(m_netf->GetDP(), &message, id);
	}
}

NETFunc::STATUS	NETFunc::Chat::SendGroup(char *m) {
	size_t size = strlen(m) + 1;
	char buffer[dpio_MAXLEN_UNRELIABLE];
	*(TYPE *)buffer = GROUP;
	memcpy(buffer + sizeof(TYPE), m , size);
	size += sizeof(TYPE);
	if(GetStatus() != OK)
		return BUSSY;
	else {
		Receive(&s_player, *(TYPE *)buffer, m);
		Message message = Message(Message::CHAT, buffer, size);
		return NETFunc::Send(m_netf->GetDP(), &message, dp_ID_BROADCAST);
	}
}

bool NETFunc::Chat::Handle(Message *m) {
	if(m->GetCode() == Message::CHAT) {
		Player *p = FindPlayer(m->GetSender());
		if(!IsMuted(p) && ((*(TYPE *)m->GetBody() == GROUP && p->GetGroup() == s_player.GetGroup()) || *(TYPE *)m->GetBody() != GROUP))
			Receive(p, *(TYPE *)m->GetBody(), (char *)m->GetBody() + sizeof(TYPE));
		return true;
	}
	return false;
}

NETFunc::Lobby *NETFunc::Lobbies::FindBest(void) {
	iterator i;
	Lobby *l, *lobby = 0;

	for(i = begin(); i != end(); i++) {
		l = *i;
		if
		(!l->IsBad() && l->GetFree() &&

			(!lobby ||

				(l->GetFree() < lobby->GetFree() ||

				l->GetKarma() < lobby->GetKarma()
		)	)	)
		lobby = l;
	}
	return lobby;
}

void NETFunc::Lobbies::Reset(void) {
	iterator i;

	for(i = begin(); i != end(); i++)
		(*i)->SetBad(false);
}

NETFunc::Players::Players(Players *l):NETFunc::List<NETFunc::Player>(l) {
}

bool NETFunc::Players::ReadyToLaunch(void) {
	iterator i;




	for(i = begin(); i!=end(); i++)
		if(!((*i)->IsReadyToLaunch()))
			return false;
	return true;
};

NETFunc::Player *NETFunc::Players::FindGroupMate(char g) {
	iterator i;
	for(i = begin(); i!=end(); i++)
		if((*i)->GetGroup() == g)
			return (*i);
	return 0;
};

NETFunc::Player *NETFunc::Players::FindGroupMaster(char g) {
	iterator i;
	for(i = begin(); i!=end(); i++)
		if((*i)->GetGroup() == g && (*i)->IsGroupMaster())
			return (*i);
	return 0;
};

char NETFunc::Players::FindSmallestGroup(void) {
	if(!gameSetup.GetGroups())
		return 0;
	iterator i;
	char gc[nf_GROUPNUMBER];
	for(int c = 0; c<nf_GROUPNUMBER; c++)
		gc[c] = 0;
	for(i = begin(); i!=end(); i++)
		gc[(*i)->GetGroup()]++;
	char sg = 0;
	int sn = 255;
	for(char g = 1; g <= gameSetup.GetGroups(); g++) {
		if(gc[g] < sn) {
			sn = gc[g];
			sg = g;
		}
	}
	return sg;
};

bool NETFunc::Players::Handle(Message *m) {
	if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(p->key[0] == dp_KEY_PLAYERS) {
			Player t = Player(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
			if(p->flags & dp_OBJECTDELTA_FLAG_INOPENSESS) {
				switch(p->status) {

				case dp_RES_CREATED:
					Add(new Player(t));




























					return true;
					break;

				case dp_RES_DELETED: {
					for(iterator i = begin(); i!=end(); i++)
						if((*i)->Equals(&t) && (*i)->IsHost()) {
							for(iterator j = begin(); j!=end(); j++)
								((PlayerSetup *)(*j))->SetReadyToLaunch(false);
							PushMessage(new Message(Message::UNLAUNCH));
							break;
						}
					Del(&t);












					return true;
					break;
					}

				case dp_RES_CHANGED:
					Chg(&t);




















					return true;
					break;
				default:
					break;
				}
			}
		}
	} else if(m->GetCode() == Message::RESET) {
		Clr();
		return true;
	} else if(m->GetCode() == Message::UNLAUNCH) {
		for(iterator i = begin(); i!=end(); i++)
			((PlayerSetup *)(*i))->SetReadyToLaunch(false);
		return true;
	}
	return false;
}

bool NETFunc::Hostiles::Check(Key *s) {
	iterator i;
	if(s)
	for(i = begin(); i != end(); i++)
		if(s->Equals(*i))
			return true;
	return false;
}

NETFunc::Hostiles NETFunc::s_hostiles = Hostiles();

bool NETFunc::Mutes::Check(Key *p) {
	iterator i;
	if(p)
	for(i = begin(); i != end(); i++)
		if(p->Equals(*i))
			return true;
	return false;
}

NETFunc::Mutes NETFunc::s_mutes = Mutes();


NETFunc::NETFunc(void) {
	s_transport = 0;
	m_playerStats = new PlayerStats();
	m_aiPlayers = new AIPlayers();
	s_connected = false;
	s_reconnected = true;
	s_playerset = false;
	s_game = false;
	s_theHost = false;
	s_launch = false;
	s_launched = false;
	s_lobby = Lobby();
	memset(&m_appParam, 0, sizeof(dp_appParam_t));

	FILE *f = fopen("netf.def", "rb");
	if(f) {
		fread(&s_servername, sizeof(s_servername) + sizeof(s_playername) + sizeof(s_sessionname), 1, f);
		fclose(f);
	}

	s_status = START;
}

NETFunc::STATUS NETFunc::Connect(char *file) {

	s_result = dpCreate(&s_dp, NULL, NULL, file);
	if(s_result != dp_RES_OK)
		return ERR;

	s_connected = true;
	dp_transport_t t;
	comm_driverInfo_t i;

	dpGetCurrentTransportInfo(s_dp, &t, &i);

	KeyStruct k;
	k.m_len = 0;

	Transport trans = Transport(&i, &t, &k);
	s_transport = new TransportSetup(&trans);

	s_connected = true;

	s_size = sizeof(dp_session_t);

	if(dpGetSessionDesc(s_dp, &m_session.m_session, &s_size) != dp_RES_OK)
		return ERR;

	m_session.SetKey();

	dp_caps_t info;
	s_result = dpGetCaps(s_dp, &info, 0);
	if(s_result != dp_RES_OK)
		return ERR;

	s_theHost = info.dwFlags & dp_CAPS_FLAGS_ISHOST;

	EnumPlayers(true, m_session.GetKey());

	s_status = READY;
	s_game = true;
	s_launch = false;
	s_launched = false;
	s_canlaunch = false;


	return OK;
}

NETFunc::~NETFunc(void) {

	FILE *f = fopen("netf.def", "wb");
	if(f) {
		fwrite(&s_servername, sizeof(s_servername) + sizeof(s_playername) + sizeof(s_sessionname), 1, f);
		fclose(f);
	}

	Quit();

	while(GetStatus() != NETFunc::START) {
		Message *m = GetMessage();
		if(m)
			delete m;
	}

	messages.clear();
	if(s_transport)
		delete s_transport;
	if(m_playerStats)
		delete m_playerStats;
	if(m_aiPlayers)
		delete m_aiPlayers;
}

NETFunc::STATUS NETFunc::GetStatus(void) {
	return s_status;
}

bool NETFunc::Connected(void) {
	return s_connected;
}

dp_t *NETFunc::GetDP(void) {
	return s_dp;
}

NETFunc::Session *NETFunc::GetSession(void) {
	return &m_session;
}

NETFunc::Player *NETFunc::GetPlayer(void) {
	return &s_player;
}

NETFunc::TransportSetup *NETFunc::GetTransport(void) {
	return s_transport;
}

NETFunc::STATUS NETFunc::SetTransport(Transport *t) {
	if(s_status == START) {
		if(s_transport)
			delete s_transport;
		if(t->GetType() == Transport::INTERNET && adialup_willdial())
			return BUSSY;
		s_transport = new TransportSetup(t);
		if(s_transport->GetType() == Transport::INTERNET) {
			s_userId = dp_UID_NONE;
			m_nextStatus = PRECONNECT;
		} else {
			s_servername[0] = 0;
			m_nextStatus = READY;
		}
		s_cancelDial = 0;
#ifdef __AUI_USE_SDL__
		m_threadHandle = SDL_CreateThread(ConnectThread, "Connect-thread", (void *)s_transport);
#else
		m_threadHandle = CreateThread(0, 0, ConnectThread, (void *)s_transport, 0, &m_threadId);
#endif
		if(m_threadHandle) {
			s_status = CONNECT;
			return OK;
		}
	}
	return ERR;
}


NETFunc::STATUS NETFunc::SetPlayerSetup(PlayerSetup *p) {
	p->SetReadyToLaunch(false);
	p->SetGroup(0);
	p->SetGroupMaster(false);
	s_playerSetup = *p;
	s_playerset = true;
	s_setplayer = true;
	s_setplayerpacket = true;
	return OK;
}

NETFunc::STATUS NETFunc::SetPlayerSetupPacket(PlayerSetup *p) {
	if(!s_playerset)
		return ERR;
	s_playerSetup = *p;
	s_setplayerpacket = true;
	if(s_status != OK)
		return OK;

	if(dpSetPlayerData(s_dp, s_player.GetId(), s_PlayerSetupPacketKey, p->GetBody(), p->GetSize(), 0) != dp_RES_OK)
		return ERR;
	s_setplayerpacket = false;
	return OK;
}

NETFunc::STATUS NETFunc::SetPlayerSetupPlayer(PlayerSetup *p) {
	if(!s_playerset)
		return ERR;
	s_playerSetup = *p;
	s_setplayer = true;
	if(s_status != OK)
		return OK;
	if(dpSetPlayerName(s_dp, s_player.GetId(), p->GetName()) != dp_RES_OK)
		return ERR;
	p->m_player.id = s_player.m_player.id;
	s_player.m_player = p->m_player;
	if(dpSetPlayerBlob(s_dp, s_player.GetId(), p->m_player.blob, p->m_player.bloblen) != dp_RES_OK)
		return ERR;
	s_setplayer = false;
	return OK;
}

NETFunc::STATUS NETFunc::GetPlayerSetupPacket(Player *p) {

	if(p->IsInCurrentSession()) {
		s_size = dpio_MAXLEN_UNRELIABLE;


		if(dpGetPlayerData(s_dp, p->GetId(), s_PlayerSetupPacketKey, s_buffer, &s_size, 0) == dp_RES_OK) {
			PushMessage(new Message(Message::PLAYERPACKET, s_buffer, s_size));
			return OK;
		}
	}
	return ERR;
}

NETFunc::STATUS NETFunc::SetReadyToLaunch(bool b) {
	if(!s_playerset)
		return ERR;
	s_playerSetup.SetReadyToLaunch(b);
	SetPlayerSetupPlayer(&s_playerSetup);
	return OK;
}

NETFunc::STATUS NETFunc::SetGroupMaster(Player *p) {
	if(!s_theHost)
		return ERR;
	Player *m = players.FindGroupMaster(p->GetGroup());
	PlayerSetup s;
	if(m) {
		if(m->Equals(p))
			return OK;
		m->SetGroupMaster(false);
		s.Player::Set(&m->m_player);
		SetRemotePlayerRecord(&s);
	}
	p->SetGroupMaster(true);
	s.Player::Set(&p->m_player);
	SetRemotePlayerRecord(&s);
	return OK;
}

NETFunc::STATUS NETFunc::SetRemotePlayerRecord(PlayerSetup *p) {
	if(s_status != OK)
		return ERR;

	if(!s_theHost && (!s_playerset || !s_playerSetup.GetGroup() || s_playerSetup.GetGroup() != p->GetGroup() || s_player.IsGroupMaster()))
		return ERR;
	if(p->GetId() == s_player.GetId()) {
		PushMessage(new Message(Message::SETPLAYERRECORD, &p->m_player, sizeof(dp_playerId_t)));
	} else {
		Message message = Message(Message::SETPLAYERRECORD, &p->m_player, sizeof(dp_playerId_t));
		if(Send(s_dp, &message, p->GetId()) != OK)
			return ERR;
	}
	return OK;
}

NETFunc::STATUS NETFunc::SetRemotePlayerPacket(PlayerSetup *p) {
	if(s_status != OK)
		return ERR;

	if(!s_theHost && (!s_playerset || !s_playerSetup.GetGroup() || s_playerSetup.GetGroup() != p->GetGroup() || s_player.IsGroupMaster()))
		return ERR;
	if(p->GetId() == s_player.GetId())
		SetPlayerSetupPacket(p);
	else {
		Message message = Message(Message::SETPLAYERPACKET, p->GetBody(), p->GetSize());
		if(Send(s_dp, &message, p->GetId()) != OK)
			return ERR;
	}
	return OK;
}


NETFunc::STATUS NETFunc::SetGameSetup(GameSetup *g) {
	if(!s_theHost)
		return ERR;
	g->SetLaunched(false);
	gameSetup = *g;
	s_game = true;
	s_launch = false;
	s_launched = false;
	s_canlaunch = !gameSetup.IsSyncLaunch();
	s_setgame = true;
	s_setgamepacket = true;

	return OK;
}

NETFunc::STATUS NETFunc::SetGameSetupPacket(GameSetup *g) {
	gameSetup = *g;
	s_setgamepacket = true;
	if(s_status != OK)
		return OK;
	Message message = Message(Message::GAMEPACKET, g->GetBody(), g->GetSize());
	if(Send(s_dp, &message, dp_ID_BROADCAST) != OK)
		return ERR;
	s_setgamepacket = false;
	return OK;
}

NETFunc::STATUS NETFunc::SetGameSetupSession(GameSetup *g) {
	gameSetup = *g;
	s_setgame = true;
	if(!gameSetup.IsSyncLaunch())
		s_canlaunch = true;
	if(s_status != OK)
		return OK;
	if(dpSetSessionDesc(s_dp, &g->m_session, 0) != dp_RES_OK)
		return ERR;
	Message *m = new Message(Message::GAMESESSION, &g->m_session, sizeof(dp_session_t));

	PushMessage(m);
	if(Send(s_dp, m, dp_ID_BROADCAST) != OK)
		return ERR;
	s_setgame = false;
	return OK;
}

NETFunc::STATUS NETFunc::InsertAIPlayer(AIPlayer *p) {
	if(!m_aiPlayers)
		return ERR;
	AIPlayer aip(*p);
	m_aiPlayers->NextKey();
	aip.m_key = m_aiPlayers->m_curkey;
	Message *m = new Message(Message::ADDAIPLAYER, aip.GetBody(), aip.GetSize());
	PushMessage(m);
	if(Send(s_dp, m, dp_ID_BROADCAST) != OK)
		return ERR;
	return OK;
}

NETFunc::STATUS NETFunc::DeleteAIPlayer(AIPlayer *p) {
	if(!m_aiPlayers)
		return ERR;
	Message *m = new Message(Message::DELAIPLAYER, p->GetBody(), p->GetSize());
	PushMessage(m);
	if(Send(s_dp, m, dp_ID_BROADCAST) != OK)
		return ERR;
	return OK;
}

NETFunc::STATUS NETFunc::ChangeAIPlayer(AIPlayer *p) {
	if(!m_aiPlayers)
		return ERR;
	Message *m = new Message(Message::CHGAIPLAYER, p->GetBody(), p->GetSize());
	PushMessage(m);
	if(Send(s_dp, m, dp_ID_BROADCAST) != OK)
		return ERR;
	return OK;
}

NETFunc::STATUS NETFunc::Launch() {
	if(!s_launch)
		SetReadyToLaunch(true);


	s_launch = true;
	if(s_status == OK && s_canlaunch && !s_launched) {
		if(s_theHost) {
			gameSetup.SetLaunched(true);
			if(gameSetup.IsSyncLaunch())
				gameSetup.SetClosed(true);
			SetGameSetupSession(&gameSetup);
		}
		s_launched = true;
		PushMessage(new Message(Message::GAMELAUNCH));
		return OK;
	}
	return ERR;
}

NETFunc::STATUS NETFunc::UnLaunchAll() {
	Message *m = new Message(Message::UNLAUNCH);
	PushMessage(m);
	if(NETFunc::Send(s_dp, m, dp_ID_BROADCAST) != OK)
		return ERR;
	return OK;
}

NETFunc::STATUS NETFunc::Login(char *username, char *password) {
	if(s_status == LOGIN) {
		if(s_userId != dp_UID_NONE || !strlen(username)) {
			PushMessage(new Message(Message::LOGINOK));
		} else if(dpAccountLogin(s_dp, username, password) != dp_RES_OK)
			return ERR;
		return OK;
	}
	return ERR;
}

NETFunc::STATUS NETFunc::SetServer(Server *s) {

	if(s_status == PRECONNECT || s_status == LOGIN) {
		if(dpSetGameServerEx(s_dp, s->GetName(), s_GameType) != dp_RES_OK)
			return ERR;
		s_status = LOGIN;

		strncpy(s_servername, s->GetName(), 64);
		return OK;
	}
	return ERR;
}

bool NETFunc::NeedUpdate(void) {

	if(s_status == LOGIN) {
		s_result = dpGetAppVersion(s_dp, &m_appParam);
		if(s_result == dp_RES_OK)
			return true;
	}
	return false;
}

NETFunc::STATUS NETFunc::DoUpdate(void) {

	if(s_status == LOGIN) {

		if(m_appParam.name) {
			Quit();

			while(GetStatus() != NETFunc::START) {
				Message *m = GetMessage();
				if(m)
					delete m;
			}

			s_result = dpDownloadUpdate(s_dp, &m_appParam);
			if(s_result == dp_RES_OK)
				return BUSSY;
			else if(s_result == dp_RES_EMPTY)
				return OK;
		}
	}
	return ERR;
}

void NETFunc::SetHost(bool h) {
	s_theHost = h;
}

bool NETFunc::GotGameSetup() {
	return s_gotgame && s_gotgamepacket;
}

bool NETFunc::IsHost() {
	return s_theHost;








}

bool NETFunc::IsHostile(Key *s) {
	return s_hostiles.Check(s);
}

bool NETFunc::IsMuted(Key *p) {
	return s_mutes.Check(p);
}

void NETFunc::Mute(Key *p, bool m) {
	KeyStruct keys[2];
	keys[0].m_buf[0] = dp_KEY_PLAYERS;
	memcpy(&keys[0].m_buf[1], &m_session.m_key.m_buf[0], m_session.m_key.m_len);
	keys[0].m_len = m_session.m_key.m_len + 1;
	memcpy(&keys[1], &p->m_key, sizeof(KeyStruct));
	if(m) {
		if(s_mutes.Find(p) == s_mutes.end())
			s_mutes.Add(new Key(*p));
	} else
		s_mutes.Del(p);

	PushMessage(new Message(Message::UPDATE, &keys[0], (sizeof(KeyStruct)<<1)));
}

NETFunc::STATUS NETFunc::Connect() {

	if(!s_playerset)
		return ERR;

	if(s_status == READY) {

		if(s_transport && s_transport->GetType() == Transport::INTERNET) {
			m_timer.Start(1000);
			s_status = WAITDELAY;
			m_nextStatus = OPENLOBBY;

		} else {
			EnumSessions(false);
			EnumSessions(true);
			m_timer.Start(2000);
			s_status = WAITDELAY;
			m_nextStatus = OPENLOBBY;
		}
		return OK;
	}
	return ERR;
}

NETFunc::STATUS NETFunc::Connect(dp_t *d, PlayerStats *stats, bool h) {

	s_dp = d;

	s_connected = true;

	s_theHost = h;

	strncpy(s_player.m_player.name, s_playername, dp_PNAMELEN);
	strncpy(m_session.m_session.sessionName, s_sessionname, dp_SNAMELEN);

	if(stats) {
		m_playerStats = new PlayerStats();

		PlayerStats::iterator i;
		for(i=stats->begin(); i!=stats->end(); i++)
			PushMessage(new Message(Message::ADDPLAYERSTAT, (*i)->GetBody(), (*i)->GetSize()));
	}

	s_status = READY;
	s_game = true;
	s_launch = false;
	s_launched = false;
	s_canlaunch = false;
	s_reconnected = true;

	dp_transport_t t;
	comm_driverInfo_t i;

	dpGetCurrentTransportInfo(s_dp, &t, &i);

	KeyStruct k;
	k.m_len = 0;

	switch(GetTransportType(&i)) {
	case Transport::INTERNET:
		{
			Internet internet = Internet(&i, &t, &k);
			s_transport = new TransportSetup(&internet);
		}
		if(dpSetGameServerEx(s_dp, s_servername, s_GameType) != dp_RES_OK)
			return ERR;
	break;
	case Transport::IPX:
		{
			IPX ipx = IPX(&i, &t, &k);
			s_transport = new TransportSetup(&ipx);
		}
	break;
	case Transport::MODEM:
		{
			Modem modem = Modem(&i, &t, &k);
			s_transport = new TransportSetup(&modem);
		}
	break;
	case Transport::NULLMODEM:
		{
			NullModem nullModem = NullModem(&i, &t, &k);
			s_transport = new TransportSetup(&nullModem);
		}
	break;
	default:
		{
			Transport trans = Transport(&i, &t, &k);
			s_transport = new TransportSetup(&trans);
		}
		break;
	}

	s_size = sizeof(dp_session_t);

	if(dpGetSessionDesc(s_dp, &m_session.m_session, &s_size) != dp_RES_OK)
		return ERR;

	m_session.SetKey();

	EnumPlayers(false, m_session.GetKey());
	EnumPlayers(true, m_session.GetKey());

	dp_caps_t info;
	s_result = dpGetCaps(s_dp, &info, 0);
	if(s_result != dp_RES_OK)
		return ERR;

#ifdef __AUI_USE_SDL__
	m_threadHandle = SDL_CreateThread(ReConnectThread, "Reconnect-thread", (void *) &s_reconnected);
#else
	m_threadHandle = CreateThread(0, 0, ReConnectThread, (void *)&s_reconnected, 0, &m_threadId);
#endif

	return (m_threadHandle) ? OK : ERR;
}

void NETFunc::ReConnect(void) {
	if(!s_reconnected) {
		s_reconnected = true;
#ifdef __AUI_USE_SDL__
		SDL_WaitThread(m_threadHandle, NULL);
#else
		DWORD dw;

		do
			GetExitCodeThread(m_threadHandle, &dw);
		while(dw == STILL_ACTIVE);

		CloseHandle(m_threadHandle);
#endif
		m_threadHandle = 0;
		dpSetActiveThread(s_dp);
	}
}

NETFunc::STATUS NETFunc::Reset() {

	if(!s_playerset)
		return ERR;
	s_status = READY;
	messages.clear();


	return OK;
}

NETFunc::STATUS NETFunc::Join(Game *g, const char *password) {

	if(g->IsCurrentSession())
		return ERR;

	if(s_status == OK) {

		if(strlen(g->GetPassword()) != strlen(password) || strncmp(g->GetPassword(), password, dp_SNAMELEN)) {

			return BADPASSWORD;

		} else if(g->GetFree() && !g->IsClosed() && !g->IsHostile()) {
			Close();
			*(Game *)&gameSetup = *g;
			s_theHost = false;
			s_game = true;
			s_launch = false;
			s_launched = false;
			s_canlaunch = false;
			s_gotgame = false;
			s_gotgamepacket = false;
			m_session= *(Session *)g;
			m_nextStatus = OPENSESSION;
			return OK;
		} else
			return ERR;
	}
	return BUSSY;
}

NETFunc::STATUS NETFunc::Join(Lobby *l) {

	if(l->IsCurrentSession())
		return ERR;

	if(s_status == OK) {

		if(l->GetFree()) {
			Close();
			s_game = false;
			m_session = *(Session *)l;
			m_nextStatus = OPENSESSION;
			return OK;
		} else
			return ERR;
	}
	return BUSSY;
}

NETFunc::STATUS NETFunc::Create(GameSetup *g) {
	if(!s_playerset)
		return ERR;

	if(s_status == OK) {
		Close();
		g->SetLaunched(false);
		gameSetup = *g;
		s_theHost = true;
		s_game = true;
		s_launch = false;
		s_launched = false;
		s_canlaunch = !gameSetup.IsSyncLaunch();
		s_gotgame = true;
		s_gotgamepacket = true;
		s_setgame = false;
		s_setgamepacket = false;

		m_session = *(Session *)g;
		m_session.m_session.flags = dp_SESSION_FLAGS_CREATESESSION | dp_SESSION_FLAGS_ENABLE_PLAYERVARS;
		m_nextStatus = OPENSESSION;
		return OK;
	}
	return BUSSY;
}

NETFunc::STATUS NETFunc::Leave(void) {
	s_game = false;
	s_theHost = false;
	s_launch = false;
	s_launched = false;
	s_canlaunch = false;
	SetPlayerSetup(&s_playerSetup);
	Close();
	m_nextStatus = OPENLOBBY;
	return OK;
}

NETFunc::STATUS NETFunc::Disconnect(void) {
	if(s_status == WAITCLOSE)
		return ERR;
	if(s_status != START)
	if((s_status == READY && s_transport && s_transport->GetType() == Transport::INTERNET)) {
		s_userId = dp_UID_NONE;
		EnumServers(true);

		m_timer.Start(1000);
		s_status = WAITDELAY;
		m_nextStatus = PRECONNECT;
	} else if(s_status == PRECONNECT || s_status == LOGIN) {
		EnumServers(false);
		dpSetGameServerEx(s_dp, 0, s_GameType);
		s_status = QUIT;
	} else if(s_status == READY) {
		s_status = QUIT;
	} else if(s_status == WAITDELAY && m_nextStatus == PRECONNECT) {
		return BUSSY;
	} else {
		EnumSessions(false);
		Close();
		m_nextStatus = READY;
	}
	return OK;
}

NETFunc::STATUS NETFunc::Quit(void) {
	if(s_status == START)
		return OK;
	if(s_status == PRECONNECT || s_status == READY || s_status == LOGIN)
		if(s_transport && s_transport->GetType() == Transport::INTERNET) {
			dpSetGameServerEx(s_dp, 0, s_GameType);
			m_timer.Start(1500);
			s_status = WAITDELAY;
			m_nextStatus = QUIT;
		} else
			s_status = QUIT;
	else {
		Close();
		m_nextStatus = QUIT;
	}
	return OK;
}

NETFunc::STATUS NETFunc::Send(dp_t *p, Message *m, dpid_t id, dpid_t from, bool r) {
	char buf[dpio_MAXLEN_UNRELIABLE];

	if(m->GetSize() + 6 > dpio_MAXLEN_UNRELIABLE)
		return ERR;
	memcpy(buf, m->Get(), m->GetSize());
	if(from == dp_ID_BROADCAST)
		from = s_player.GetId();
	return dpSend(p, from, id, r ? dp_SEND_RELIABLE : 0, buf, m->GetSize()) == dp_RES_OK ? OK : ERR;
}

bool NETFunc::Handle(Message *m) {
	if(m_aiPlayers)
		m_aiPlayers->Handle(m);
	if(m_playerStats)
		m_playerStats->Handle(m);
	gameSetup.Handle(m);
	players.Handle(m);

	if(s_theHost && s_game) {
		gameSetup.Handle(s_dp, m);
		if(m_aiPlayers)
			m_aiPlayers->Handle(s_dp, m);
		if(m_playerStats)
			m_playerStats->Handle(s_dp, m);

		if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
			dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
			if(p->key[0] == dp_KEY_PLAYERS) {
				Player t = Player(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
				if(p->flags & dp_OBJECTDELTA_FLAG_INOPENSESS) {
					if(p->status == dp_RES_DELETED || p->status == dp_RES_CREATED) {
						gameSetup.SetGroups(static_cast<char>(players.size() + (m_aiPlayers ? m_aiPlayers->size() : 0)));
						SetGameSetupSession(&gameSetup);
					}
				}
			}
		} else if(m->GetCode() == Message::ADDAIPLAYER || m->GetCode() == Message::CHGAIPLAYER || m->GetCode() == Message::DELAIPLAYER) {
			gameSetup.SetGroups(static_cast<char>(players.size() + (m_aiPlayers ? m_aiPlayers->size() : 0)));
			SetGameSetupSession(&gameSetup);
		}

	}

	if(m->GetCode() == dp_ACCOUNT_PACKET_ID) {
		dp_account_packet_t *p = (dp_account_packet_t *)m->GetBody();
		s_userId = p->uid;
		if(p->reason == dp_RES_OK && s_status == LOGIN) {
			PushMessage(new Message(Message::LOGINOK));
		} else {
			PushMessage(new Message(Message::LOGINERR));
		}
		return true;
	} else if(m->GetCode() == Message::LOGINOK) {
		EnumServers(false);
		s_status = READY;
		return true;
	} else if(m->GetCode() == Message::PLAYERENTER) {
		dp_playerId_t *p = (dp_playerId_t *)m->GetBody();
		Message message = Message(Message::PINGBACK);
		Send(s_dp, &message, p->id);
		return true;
	}
	else if(m->GetCode() == Message::RESET) {
		if(((KeyStruct *)m->GetBody())->m_buf[0] == dp_KEY_SESSIONS)
			s_lobbies.Clr();
		return true;
	} else if(m->GetCode() == dp_USER_HOST_PACKET_ID) {
		s_theHost = true;
		if(!gameSetup.IsSyncLaunch())
			s_canlaunch = true;
		PushMessage(new Message(Message::GAMEHOST));
		return true;
	} else if(m->GetCode() == Message::UNLAUNCH) {
		s_launch = false;
		s_launched = false;
		s_playerSetup.SetReadyToLaunch(false);
		SetPlayerSetupPlayer(&s_playerSetup);
		return true;
	} else if(m->GetCode() == dp_SESSIONLOST_PACKET_ID) {
		s_status = CLOSE;
		m_nextStatus = OPENLOBBY;

		PushMessage(new Message(Message::SESSIONERR));
		return true;
	} else if(m->GetCode() == Message::GAMESESSION || m->GetCode() == Message::GAMEPACKET) {
		if(gameSetup.IsLaunched() && GotGameSetup())
			s_canlaunch = true;
		return true;
	} else if(m->GetCode() == Message::SETPLAYERRECORD) {
		s_playerSetup.Player::Set((dp_playerId_t *)m->GetBody());
		s_player.Set((dp_playerId_t *)m->GetBody());
		SetPlayerSetupPlayer(&s_playerSetup);
		return true;
	} else if(m->GetCode() == Message::SETPLAYERPACKET) {
		s_playerSetup.Packet::Set(m->GetBodySize(), m->GetBody());
		SetPlayerSetupPacket(&s_playerSetup);
		return true;
	} else if(m->GetCode() == Message::KICKED) {






		PushMessage(new Message(Message::PLAYERKICK));

		Leave();
	} else if(m->GetCode() == dp_OBJECTDELTA_PACKET_ID) {
		dp_objectDelta_packet_t *p = (dp_objectDelta_packet_t *)m->GetBody();
		if(p->key[0] == dp_KEY_SESSIONS) {
			Session s = Session(&p->data, (KeyStruct *)&p->subkeylen, p->flags);

			if(s.m_session.sessionType == s_GameType)
			if(s.IsLobby()) {
				*(Message::CODE *)(m->m_body) = Message::LOBBYDELTAPACKET;
				Lobby l(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
				switch(p->status) {

				case dp_RES_CREATED:
					s_lobbies.Add(new Lobby(l));
					break;

				case dp_RES_DELETED:
					s_lobbies.Del(&l);
					break;

				case dp_RES_CHANGED:
					s_lobbies.Chg(&l);
					break;
				default:
					break;
				}
			} else
				*(Message::CODE *)(m->m_body) = Message::GAMEDELTAPACKET;

			if( ( s.Equals(&m_session) || s.IsCurrentSession() )
				&& (p->status == dp_RES_CREATED || p->status == dp_RES_CHANGED)) {
				if((m_session.m_session.flags & dp_OBJECTDELTA_FLAG_INOPENSESS) == 0) {
					p->flags |= dp_OBJECTDELTA_FLAG_INOPENSESS;
					s_mutes.Clr();
					players.Clr();
				}
				s.m_flags |= dp_OBJECTDELTA_FLAG_INOPENSESS;
				m_session = s;
			}
			return true;
		} else if(p->key[0] == dp_KEY_PLAYERS) {
			Player t = Player(&p->data, (KeyStruct *)&p->subkeylen, p->flags);
			if(p->flags & dp_OBJECTDELTA_FLAG_LOCAL) {
				if(p->status == dp_RES_DELETED) {











				} else if(p->status == dp_RES_CREATED || p->status == dp_RES_CHANGED) {

					s_player = t;
					if(s_status == READY) {
						s_status = OK;

						PushMessage(new Message(Message::ENTERGAME, m_session.GetKey(), sizeof(KeyStruct)));
					}
					if(p->status == dp_RES_CREATED) {
						Message message = Message(Message::PLAYERENTER, &s_player.m_player, sizeof(dp_playerId_t));
						Send(s_dp, &message, dp_ID_BROADCAST);
					}
				}
			}

			if(p->flags & dp_OBJECTDELTA_FLAG_INOPENSESS) {
				if(p->status == dp_RES_DELETED)
					s_mutes.Del(&t);

				if(s_theHost && gameSetup.IsSyncLaunch()) {

					s_canlaunch = players.ReadyToLaunch() && GotGameSetup() &&
						!(t.IsHost() && p->status == dp_RES_DELETED);
				}
				return true;
			}
		}
	}
	return false;
}

void NETFunc::Execute(void) {
	switch(s_status) {
	case CONNECT:
#ifdef __AUI_USE_SDL__
		int dw;
		// This is ugly, but there is no other way in SDL
		// to determine if a thread is running
		// SDL3 has a better function for that
		SDL_WaitThread(m_threadHandle, &dw);

			m_threadHandle = NULL;
			if (dw > 0) {
#else
		DWORD dw;

		GetExitCodeThread(m_threadHandle, &dw);
		if(dw != STILL_ACTIVE) {
			CloseHandle(m_threadHandle);

			m_threadHandle = NULL;
			if (dw) {
#endif
				s_status = START;

				PushMessage(new Message(Message::NETWORKERR));
			} else {

				dpSetActiveThread(s_dp);
				s_connected = true;
				s_status = m_nextStatus;
				if(s_status == PRECONNECT)
					EnumServers(true);
			}
#ifndef __AUI_USE_SDL__
		}
#endif
		break;
	case OK:
		if(!m_session.IsServer() && m_session.IsLobby() && m_session.GetPlayers() == 1) {

			Lobby *l = s_lobbies.FindBest();
			if(l) {

				if(!m_session.Equals(l)) {

					s_status = CLOSE;
					m_nextStatus = OPENSESSION;
					m_session.m_session = l->m_session;
				}
			}
		}
		if(s_game) {
			if(s_theHost) {

				if(s_setgame)
					SetGameSetupSession(&gameSetup);

				if(s_setgamepacket)
					SetGameSetupPacket(&gameSetup);
			}

			if(!s_launched && s_launch && s_canlaunch) {
				Launch();
			}
		}
		if(s_playerset) {
			if(s_setplayer)
				SetPlayerSetupPlayer(&s_playerSetup);
			if(s_setplayerpacket)
				SetPlayerSetupPacket(&s_playerSetup);
		}
		break;
	case OPENLOBBY:
		if(m_timeout.Finished() || m_retries == 0) {
			PushMessage(new Message(Message::NETWORKERR));
			s_status = READY;
			break;
		} else
			m_retries--;
		s_status = JOINLOBBY;

		if(s_transport && s_transport->GetType() == Transport::INTERNET) {
			dpSetGameServerEx(s_dp, s_servername, s_GameType);
			if(dpOpen(s_dp, 0, SessionCallBack, this) != dp_RES_OK)
				s_status = OPENLOBBY;
		} else {

			Lobby *l = s_lobbies.FindBest();
			if(l) {

				if(dpOpen(s_dp, &l->m_session, SessionCallBack, this) != dp_RES_OK) {
					l->SetBad();
					s_lobbies.Chg(l);
					s_status = OPENLOBBY;
				} else
					m_session.m_session = l->m_session;
			} else

				if(dpOpen(s_dp, &s_lobby.m_session, SessionCallBack, this) != dp_RES_OK)
					s_status = OPENLOBBY;
		}
		break;
	case OPENSESSION:
		s_status = JOINSESSION;
		if(dpOpen(s_dp, &m_session.m_session, SessionCallBack, this) != dp_RES_OK) {
			s_status = OPENLOBBY;

			PushMessage(new Message(Message::SESSIONERR));
		}
		break;
	case CREATEPLAYER:

		s_setplayerpacket = true;
		s_setplayer = true;
		if(dpCreatePlayer(s_dp, PlayerCallBack, this, s_playerSetup.GetName()) != dp_RES_OK) {
			s_status = CLOSE;
			m_nextStatus = OPENLOBBY;

			PushMessage(new Message(Message::SESSIONERR));
		} else if(s_status == CREATEPLAYER)
			s_status = BUSSY;
		break;
	case CLOSE:
		dpClose(s_dp);
		m_timer.Start(8000);
		s_status = WAITCLOSE;
	case WAITCLOSE:
		if(dpReadyToFreeze(s_dp) == dp_RES_OK || m_timer.Finished()) {
			if(m_nextStatus != READY && m_nextStatus != OPENLOBBY && m_nextStatus != OPENSESSION && s_transport && s_transport->GetType() == Transport::INTERNET)
				dpSetGameServerEx(s_dp, 0, s_GameType);
			m_timer.Start(2000);
			s_status = WAITDELAY;
		}
		break;
	case WAITDELAY:
		if(m_timer.Finished()) {
			s_status = m_nextStatus;
			if(s_status == OPENLOBBY) {
				s_lobbies.Reset();
				m_timeout.Start(60000);
				m_retries = 6;
			}
		}
		if(s_status != START)
			break;
	case QUIT:
		dpDestroy(s_dp, 1);
		s_connected = false;
		s_status = START;
		break;
	}
}

NETFUNC_CALLBACK_RESULT(int)
NETFunc::SessionCallBack(dp_session_t *s, long *pTimeout, long flags, void *context) {

	if(s) {

		((NETFunc *)context)->m_session.m_session = *s;
		((NETFunc *)context)->m_session.SetKey();
		((NETFunc *)context)->m_session.m_flags = 0;

		strncpy(s_sessionname, s->sessionName, dp_SNAMELEN);
		EnumSessions(false);
		if(((NETFunc *)context)->m_session.m_session.flags & dp_SESSION_FLAGS_ISLOBBY)
			EnumSessions(true);
		dpSetPingIntervals( ((NETFunc *)context)->GetDP(), 5000, 5000 );
		((NETFunc *)context)->s_status = CREATEPLAYER;
	} else {
		if((((NETFunc *)context)->m_session.m_session.flags & dp_SESSION_FLAGS_ISLOBBY)
			&& ((NETFunc *)context)->s_transport
			&& ((NETFunc *)context)->s_transport->GetType() != Transport::INTERNET) {

			Lobby l;
			m_session.m_session = l.m_session;
			l.SetBad();
			((NETFunc *)context)->s_lobbies.Chg(&l);
		}
		((NETFunc *)context)->s_status = OPENLOBBY;

		PushMessage(new Message(Message::SESSIONERR));
	}
	return 0;
}

NETFUNC_CALLBACK_RESULT(void)
NETFunc::PlayerCallBack(dpid_t id, dp_char_t *n, long flags, void *context) {

	if(n) {

		strcpy(((NETFunc *)context)->s_player.m_player.name, n);
		((NETFunc *)context)->s_player.m_player.id = id;
		((NETFunc *)context)->s_player.SetKey();
		((NETFunc *)context)->s_status = OK;

		strncpy(s_playername, n, dp_PNAMELEN);

		if(((NETFunc *)context)->m_session.IsLobby())
			PushMessage(new Message(Message::ENTERLOBBY, ((NETFunc *)context)->m_session.GetKey(), sizeof(KeyStruct)));
		else
			PushMessage(new Message(Message::ENTERGAME, ((NETFunc *)context)->m_session.GetKey(), sizeof(KeyStruct)));
	} else {
		((NETFunc *)context)->s_status = CLOSE;
		((NETFunc *)context)->m_nextStatus = OPENLOBBY;

		PushMessage(new Message(Message::SESSIONERR));
	}
}

void NETFunc::CancelDial(void) {
	s_cancelDial = 1;
}

NETFunc::STATUS NETFunc::Kick(Player *p) {
	if(s_status == OK && p->IsInCurrentSession() && !p->IsMe())
		if(s_player.IsHost()){
			Message message = Message(Message::KICKED);
			return Send(s_dp, &message, p->GetId(), s_player.GetId());
		}

		else if(s_player.IsGroupMaster() && s_player.GetGroup() == p->GetGroup())
		{
			Message message = Message(Message::KICKED);
			return Send(s_dp, &message, p->GetId(), s_player.GetId());
		}
	return ERR;
}

NETFunc::STATUS NETFunc::Close(void) {




		m_session = Session();
		s_setplayer = true;
		s_setplayerpacket = true;


		s_mutes.Clr();
		players.Clr();
		s_status = CLOSE;
		return OK;

}































bool NETFunc::HandleMessage(Message *m) {
	return MessageHandler::HandleAll(m);
}

void NETFunc::Receive(void) {

	s_size = 0;
	s_source = dp_ID_NONE;
	s_destination = dp_ID_NONE;
	Message *m;
	if(s_connected) do {

		s_size = dpio_MAXLEN_UNRELIABLE;
		s_result = dpReceive(s_dp, &s_source, &s_destination, 0, s_buffer, &s_size);
		switch (s_result) {
		case dp_RES_EMPTY:
		break;
		case dp_RES_OK:
			m = 0;
			m = new Message(s_buffer, s_size, s_source);
			if(m && ((char)m->GetCode() == dp_PACKET_INITIALBYTE || (char)m->GetCode() == nf_PACKET_INITIALBYTE || (char)m->GetCode() == ns_PACKET_INITIALBYTE) || m->GetCode() == Message::CHAT)
				PushMessage(m);
		break;

		case dp_RES_BUG:
		case dp_RES_BAD:
			;
			break;
		case dp_RES_MODEM_NORESPONSE:
		case dp_RES_NETWORK_NOT_RESPONDING:
			;

		case dp_RES_HOST_NOT_RESPONDING:
			;
			PushMessage(new Message(Message::NETWORKERR));
			dpDestroy(s_dp, 1);
			s_connected = false;
			s_status = START;
			break;
			default:
			break;
		}
	} while(s_result == dp_RES_OK);
}

NETFunc::Message *NETFunc::GetMessage(void) {

	Execute();

	Receive();

	Message *m = 0;

	if(!messages.empty()) {
		m = messages.Pop();
		Handle(m);
	}
	return m;
}

void NETFunc::PushChatMessage(char *m) {
	size_t size = strlen(m) + 1;
	char buffer[dpio_MAXLEN_UNRELIABLE];
	*(Chat::TYPE *)buffer = Chat::SYSTEM;
	memcpy(buffer + sizeof(Chat::TYPE), m , size);
	size += sizeof(Chat::TYPE);
	PushMessage(new Message(Message::CHAT, buffer, size));
}

void NETFunc::PushMessage(Message *m) {
	messages.Push(m);
}

dp_t *NETFunc::s_dp = 0;

dp_result_t	NETFunc::s_result = 0;

char NETFunc::s_buffer[dpio_MAXLEN_UNRELIABLE] = "";

size_t NETFunc::s_size = 0;

dpid_t NETFunc::s_source = 0;

NETFunc::STATUS NETFunc::s_status = RESET;

dpid_t NETFunc::s_destination = 0;

dp_species_t NETFunc::s_GameType = 0;

char *NETFunc::s_LobbyName = "Lobby";

char *NETFunc::s_DllPath = "anet";

int NETFunc::s_PlayerSetupPacketKey = 0;

long NETFunc::s_cancelDial = 0;

bool NETFunc::s_connected = false;

bool NETFunc::s_reconnected = true;

NETFunc::PlayerSetup NETFunc::s_playerSetup = NETFunc::PlayerSetup();

bool NETFunc::s_playerset = false;

bool NETFunc::s_setplayerpacket = false;

bool NETFunc::s_setplayer = false;

NETFunc::GameSetup NETFunc::gameSetup = NETFunc::GameSetup();

bool NETFunc::s_game = false;

bool NETFunc::s_theHost = false;

bool NETFunc::s_launch = false;

bool NETFunc::s_launched = false;

bool NETFunc::s_canlaunch = false;

bool NETFunc::s_setgamepacket = false;

bool NETFunc::s_setgame = false;

bool NETFunc::s_gotgamepacket = false;

bool NETFunc::s_gotgame = false;

char NETFunc::s_servername[64] = "";
char NETFunc::s_playername[dp_PNAMELEN] = "";
char NETFunc::s_sessionname[dp_SNAMELEN] = "";
