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
// - Make the Linux version loading and producing Windows compatible
//   savegames. (16-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __NETFUNC_H__
#define __NETFUNC_H__

#include "anet.h"
#include <locale>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <list>

#ifdef __AUI_USE_SDL__
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#endif

#if defined(WIN32)
#include <windows.h>
#define NETFUNC_CALLBACK_RESULT(a_Type) a_Type __stdcall
#else
#define NETFUNC_CALLBACK_RESULT(a_Type) a_Type
#endif

#if !defined(__AUI_USE_SDL__)
#define NETFUNC_CONNECT_RESULT          DWORD WINAPI
#define NETFUNC_CONNECT_PARAMETER       LPVOID
#else
#define NETFUNC_CONNECT_RESULT          int
#define NETFUNC_CONNECT_PARAMETER       void *
#endif

class NETFunc
{
public:

	static dp_species_t s_GameType;

	static char*        s_LobbyName;

	static char*        s_DllPath;

	static char         s_servername[64];
	static char         s_playername[dp_PNAMELEN];
	static char         s_sessionname[dp_SNAMELEN];


enum STATUS
{
	OK = 0,
	ERR,
	BADPASSWORD,
	NOSETUP,
	NOSERVER,
	NOCONTACT,
	NOPORT,
	BUSSY,
	START,
	CONNECT,
	PRECONNECT,
	LOGIN,
	READY,
	BADSETUP,
	OPENLOBBY,
	JOINLOBBY,
	OPENSESSION,
	JOINSESSION,
	QUIT,
	CLOSE,
	WAITCLOSE,
	WAITDELAY,
	CREATEPLAYER,
	OPEN,
	RESET
};

static void StringMix(int c, char *mix, char *msg, ...);
static char *StringDup(char *s);

class Timer
{
	DWORD   m_start;
	DWORD   m_finish;
	bool    m_done;
public:

	Timer(void);

	void Start(int d);

	bool Finished(void);
};
Timer m_timer;
Timer m_timeout;

struct KeyStruct
{
	short m_len;
	char  m_buf[dp_KEY_MAXLEN + 2];
};

class Keys {
friend class NETFunc;
protected:
	KeyStruct m_curkey;
public:

	Keys(void);
	void NextKey(void);
};

class Key
{
friend class NETFunc;
protected:
	KeyStruct m_key;
public:

	Key(void);

	Key(Key *k);

	Key(KeyStruct *k);

	void WriteToFile(FILE *saveFile) const;
	void ReadFromFile(FILE *saveFile);

	bool Equals(Key *k);

	bool Equals(KeyStruct *k);

	virtual void SetKey(void) {};

	KeyStruct *GetKey(void);
};

template<class T>
class List:public std::list<T *>
{
friend class NETFunc;
public:
	typename List<T>::iterator Find(T *t)
	{
		for (typename List<T>::iterator i = this->begin(); i != this->end(); ++i)
		{
			if (t->Equals(*i))
			{
				return i;
			}
		}

		return this->end();
	}

	void Clr(void)
	{
		for (typename List<T>::iterator i = this->begin(); i != this->end(); ++i)
		{
			delete *i;
		}

		this->clear();
	}

	T *Add(T *t)
	{
		typename List<T>::iterator i = Find(t);

		if (i == this->end())
		{
			this->push_back(t);
			return t;
		}
		else
		{
			memcpy(*i, t, sizeof(T));
			delete t;
			return *i;
		}
	}

	void Del(T *t)
	{
		typename List<T>::iterator i = Find(t);

		if (i != this->end())
		{
			delete *i;
			this->erase(i);
		}
	}

	T *Chg(T *t)
	{
		typename List<T>::iterator i = Find(t);

		if (i == this->end())
		{
			T *n = new T(*t);
			this->push_back(n);
			return n;
		}
		else
		{
			memcpy(*i, t, sizeof(T));
			return *i;
		}
	}

	virtual ~List(void)
	{
		this->Clr();
	}

	List(List<T> *l)
	{
		if (l)
		{
			for (typename List<T>::iterator i = l->begin(); i != l->end(); ++i)
			{
				this->Add(new T(**i));
			}
		}
	}

	List(void) {
	}
};

class Messages;

class Message:public Key
{
friend class NETFunc;
	char*  m_body;
	dpid_t m_sender;
	size_t m_size;

	bool m_newbody;
#define nf_PACKET_INITIALBYTE 'n'
#define ns_PACKET_INITIALBYTE 's'
public:

	enum Code {
		ENTERGAME			= dppt_MAKE(nf_PACKET_INITIALBYTE, 0),
		ENTERLOBBY			= dppt_MAKE(nf_PACKET_INITIALBYTE, 1),
		UPDATE				= dppt_MAKE(nf_PACKET_INITIALBYTE, 2),
		RESET				= dppt_MAKE(nf_PACKET_INITIALBYTE, 3),
		CHAT				= dppt_MAKE('c', 'h'),
		GAMESESSION			= dppt_MAKE(nf_PACKET_INITIALBYTE, 5),
		GAMEPACKET			= dppt_MAKE(nf_PACKET_INITIALBYTE, 6),
		GAMEHOST			= dppt_MAKE(nf_PACKET_INITIALBYTE, 7),
		GROUPMASTER			= dppt_MAKE(nf_PACKET_INITIALBYTE, 8),
		GAMELAUNCH			= dppt_MAKE(nf_PACKET_INITIALBYTE, 9),
		PLAYERPACKET		= dppt_MAKE(nf_PACKET_INITIALBYTE, 10),
		SETPLAYERRECORD		= dppt_MAKE(nf_PACKET_INITIALBYTE, 11),
		SETPLAYERPACKET		= dppt_MAKE(nf_PACKET_INITIALBYTE, 12),
		PLAYERENTER			= dppt_MAKE(nf_PACKET_INITIALBYTE, 13),
		PLAYERKICK			= dppt_MAKE(nf_PACKET_INITIALBYTE, 14),

		LOBBYDELTAPACKET	= dppt_MAKE(nf_PACKET_INITIALBYTE, 15),
		GAMEDELTAPACKET		= dppt_MAKE(nf_PACKET_INITIALBYTE, 16),

		ADDAIPLAYER			= dppt_MAKE(nf_PACKET_INITIALBYTE, 17),
		DELAIPLAYER			= dppt_MAKE(nf_PACKET_INITIALBYTE, 18),
		CHGAIPLAYER			= dppt_MAKE(nf_PACKET_INITIALBYTE, 19),

		ADDPLAYERSTAT		= dppt_MAKE(nf_PACKET_INITIALBYTE, 20),
		DELPLAYERSTAT		= dppt_MAKE(nf_PACKET_INITIALBYTE, 21),
		CHGPLAYERSTAT		= dppt_MAKE(nf_PACKET_INITIALBYTE, 22),

		LOGINOK				= dppt_MAKE(nf_PACKET_INITIALBYTE, 23),
		LOGINERR			= dppt_MAKE(nf_PACKET_INITIALBYTE, 24),

		NETWORKERR			= dppt_MAKE(nf_PACKET_INITIALBYTE, 25),
		SESSIONERR			= dppt_MAKE(nf_PACKET_INITIALBYTE, 26),

		KICKED				= dppt_MAKE(nf_PACKET_INITIALBYTE, 27),

		PINGBACK			= dppt_MAKE(nf_PACKET_INITIALBYTE, 28),
		UNLAUNCH			= dppt_MAKE(nf_PACKET_INITIALBYTE, 29)
	};

	typedef sint16 CODE;

	Message(CODE c, void *p, size_t s);

	Message(CODE c);

	Message(void);

	Message(void *p, size_t s, dpid_t id, bool b = true);

	virtual ~Message(void);

	CODE GetCode(void);

	void *GetBody(void);

	dp_packetType_t *Get(void);

	size_t GetSize(void);

	size_t GetBodySize(void);

	dpid_t GetSender(void);
};
friend class Message;






class Messages:public List<Message>
{
public:

	Messages(void);

	~Messages(void);

	void Push(Message *m);

	Message *Pop(void);
};
static Messages messages;

class Packet
{
#define nf_MAX_PACKETSIZE dpio_MAXLEN_RELIABLE - sizeof(Message::CODE)
public:
	typedef unsigned char SizeT;
protected:
	SizeT  m_size;

	char   m_body[nf_MAX_PACKETSIZE];

	char*  m_first;
public:

	void WriteToFile(FILE *saveFile) const;
	void ReadFromFile(FILE *saveFile);

	char *GetBody(void) {
		return m_body;
	}

	size_t GetSize(void) {
		return m_size;
	}

	Packet() {

		m_size = 0;

		m_first = m_body;
	}

	void Set(size_t s, void * b)
	{
		Assert(s < nf_MAX_PACKETSIZE);
		memcpy(m_body, b, s);
		m_size = static_cast<NETFunc::Packet::SizeT>(s);
		m_first = m_body;
		Unpack();
	}

	Packet(SizeT s, void *b) {
		Set(s, b);
	}

	virtual ~Packet() {
	}

	virtual void Pack(void) = 0;

	virtual void Unpack(void) = 0;

	void Clear() {
		m_size = 0;
		m_first = m_body;
	}

	void Grow(size_t s) {
		Assert((m_size + s) <= nf_MAX_PACKETSIZE);
		m_size = static_cast<SizeT>(m_size + s);
	}

	template<typename type>
	void Push(type t) {
		Grow(sizeof(type));
		*(type *)(m_body + m_size - sizeof(type)) = t;
	}

	template<typename type>
	void Pop(type& t) {

		t = *(type *)(m_first);
		m_first += sizeof(type);

	}

	void Push(char *c) {
		size_t const    s = strlen(c) + 1;
		Grow(s);
		strcpy(m_body + m_size - s, c);
	}

	void Pop(char *c) {
		size_t const    s = strlen(m_first) + 1;

		strcpy(c, m_first);
		m_first += s;
	}
};






class MessageHandler
{
#define nf_MAX_HANDLERS 128

	static MessageHandler *s_hList[nf_MAX_HANDLERS];

	static int s_hCount;

	virtual bool Handle(Message *m) = 0;
protected:

	MessageHandler(void);

	virtual ~MessageHandler(void);
public:

	void Unregister(void);

	static bool HandleAll(Message *m);
};







template<class T>
class ListHandler:public MessageHandler, public List<T>, public Key
{
	STATUS m_status;
public:

	ListHandler(void) {
		SetKey();
	}

	virtual ~ListHandler(void) {
	}

	void SetKey();
	bool Equals(KeyStruct *k) {

		if(m_key.m_buf[0] == dp_KEY_PLAYERS)
			return (k->m_buf[0] == dp_KEY_PLAYERS && memcmp(&m_key.m_buf[m_key.m_len - 2], &k->m_buf[k->m_len - 2], 2) == 0);
		else
			return Key::Equals(k);
	}

	void SetKey(KeyStruct *k) {
		if(!this->empty()) {
			Destroy();
			this->Clr();
		}
		if(m_key.m_buf[0] == dp_KEY_PLAYERS) {
			if(k) {
				memcpy(&m_key.m_buf[1], &k->m_buf[0], k->m_len);
				m_key.m_len = k->m_len + 1;
			} else
				memset(&m_key.m_buf[1], 0, m_key.m_len - 1);
		}
	}

	virtual void Insert(T *t) = 0;

	virtual void Delete(T *t) = 0;

	virtual void Change(T *t) = 0;

	virtual void Destroy() = 0;

	bool Handle(Message *m);
};


static STATUS EnumServers(bool b);


static STATUS EnumSessions(bool b);


static STATUS EnumPlayers(bool b, KeyStruct *k);




class Server:public Key
{
	dp_serverInfo_t m_server;
public:

	void SetKey(void) {};

	Server(void);

	Server(dp_object_t *o, KeyStruct *k, long f);

	virtual ~Server(void);

	char *GetName(void);

	int GetPlayers(void);

	int GetPing(void);

	dp_serverInfo_t *GetServer(void);
};





class Contact:public Key
{
	char	*m_name;
	char	*m_number;
public:

	Contact(char *n, char *p);

	Contact(void);

	virtual ~Contact(void);

	char *GetName(void);

	char *GetNumber(void);

	void SetName(char *n);

	void SetNumber(char *p);
};




class ContactList:public List<Contact>
{
public:

	ContactList(void);

	~ContactList(void);
};
ContactList	m_contactList;




class Port:public Key
{
#define nf_PORTINITLEN 128
	commPortName_t m_port;
	int		m_baud;
	char m_init[nf_PORTINITLEN];
public:

	Port(commPortName_t *p, int b, char *i);

	Port(void);

	virtual ~Port(void);

	commPortName_t *GetPort(void);

	int GetNumber(void);

	char *GetName(void);

	int GetBaud(void);

	char *GetInit(void);
};

class Transport;


class PortList:public List<Port>
{
public:

	PortList(Transport *t);

	PortList(void);

	~PortList(void);
};

PortList m_portList;
friend class PortList;


static NETFUNC_CONNECT_RESULT ConnectThread(NETFUNC_CONNECT_PARAMETER);
static NETFUNC_CONNECT_RESULT ReConnectThread(NETFUNC_CONNECT_PARAMETER);




class Transport:public Key
{
friend class PortList;
protected:
	dp_transport_t		m_transport;
	comm_driverInfo_t	m_description;
	commInitReq_t		m_parameters;

	STATUS status;
public:

	enum TYPE
	{
		UNKNOWN,
		INTERNET,
		IPX,
		MODEM,
		NULLMODEM
	};

	Transport(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	Transport(void);

	virtual ~Transport(void);

	dp_transport_t *GetTransport(void);

	commInitReq_t *GetParams(void);

	virtual STATUS GetStatus(void);

	virtual TYPE GetType(void);

	char *GetName(void);

	char *GetFileName(void);

	virtual STATUS SetPort(long p);

	virtual STATUS SetPort(Port *p);

	virtual STATUS SetContact(Contact *c);
};





class TransportSetup
{
	dp_transport_t		m_transport;
	commInitReq_t		m_parameters;

	STATUS status;
	Transport::TYPE type;
public:

	TransportSetup(Transport *t);

	~TransportSetup(void);

	Transport::TYPE GetType(void);

	STATUS GetStatus(void);

	dp_transport_t		*GetTransport(void);

	commInitReq_t		*GetParams(void);
};
static TransportSetup *s_transport;
friend class TransportSetup;


static Transport::TYPE GetTransportType(const comm_driverInfo_t *c);


class Internet:public Transport
{
public:

	Internet(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~Internet(void);

	STATUS SetPort(long p);

	TYPE GetType(void);
};
friend class Internet;


class IPX:public Transport
{
public:

	IPX(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~IPX(void);

	TYPE GetType(void);
};
friend class IPX;


class Modem:public Transport
{
public:

	Modem(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~Modem(void);

	STATUS SetContact(Contact *c);

	STATUS SetPort(Port *p);

	TYPE GetType(void);
};
friend class Modem;


class NullModem:public Transport
{
public:

	NullModem(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~NullModem(void);

	STATUS SetPort(Port *p);

	TYPE GetType(void);
};
friend class NullModem;


class TransportList:public List<Transport>
{
	KeyStruct m_key;

	static NETFUNC_CALLBACK_RESULT(void)
	CallBack(const dp_transport_t *t, const comm_driverInfo_t *d, void *context);
public:

	TransportList(void);

	~TransportList(void);
};
TransportList m_transportList;
friend class TransportList;










class AIPlayer:public Key, public Packet
{
friend class NETFunc;
protected:
	char	m_name[dp_PNAMELEN];
	unsigned char m_group;
public:

	AIPlayer(void);

	~AIPlayer(void);

	const char *GetName(void);

	void SetName(const char *name);

	unsigned char GetGroup(void);

	void SetGroup(unsigned char group);

	void SetKey(KeyStruct *k);

	void Pack(void);

	void Unpack(void);

	STATUS Save(FILE *f);

	STATUS Load(FILE *f);
};
friend class AIPlayer;


class AIPlayers:public List<AIPlayer>, public Keys
{
protected:

	STATUS Send(dp_t *p, dpid_t id, dpid_t from = dp_ID_BROADCAST);
public:

	AIPlayers(AIPlayers *l = NULL);

	bool Handle(Message *m);

	bool Handle(dp_t *p, Message *m, dpid_t from = dp_ID_BROADCAST);
};
AIPlayers *m_aiPlayers;
friend class AIPlayers;

class Players;




class Player:public Key
{
friend class NETFunc;
friend class Players;
#define nf_GROUPMASTER	0x40
#define nf_READYLAUNCH	0x20
#define nf_GROUPNUMBER	0x1f
protected:
	dp_playerId_t	m_player;
	long			m_flags;
	short			m_latency;
	bool			m_muted;

	void SetGroupMaster(bool b);
public:

	void SetKey(void);

	Player(void);

	Player(dp_object_t *o, KeyStruct *k, long f, short l = 0);

	void Set(dp_playerId_t *p);

	virtual ~Player(void);

	dpid_t GetId(void);

	char *GetName(void);

	unsigned char *GetBlob(void);

	unsigned char GetBlobLen(void);

	unsigned char GetGroup(void);

	short GetLatency(void);

	bool IsMe(void);

	bool IsMuted(void);

	void SetMuted(bool m);

	bool IsInCurrentSession(void);

	bool IsHost(void);

	bool IsGroupMaster(void);

	bool IsReadyToLaunch(void);
};
friend class Player;
static Player s_player;
static dp_uid_t s_userId;


class Players:public List<Player>
{
public:

	Players(Players *l = NULL);

	Player *FindGroupMate(char g);

	Player *FindGroupMaster(char g);

	char FindSmallestGroup(void);

	bool ReadyToLaunch(void);

	bool Handle(Message *m);
};
Players players;
friend class Players;


class PlayerStat:public Key, public Packet
{
friend class NETFunc;
protected:
	char	m_name[dp_PNAMELEN];
	unsigned char m_group;
	bool m_isingame;
	bool m_hasleft;
public:

	void Pack(void);

	void Unpack(void);

	PlayerStat(void);

	~PlayerStat(void);

	void SetName(char *n);

	char *GetName(void);

	void SetGroup(unsigned char group);

	unsigned char GetGroup(void);

	void SetId(dpid_t id);

	dpid_t GetId(void);

	void SetInGame(bool b);

	bool IsInGame(void);

	void SetLeft(bool b);

	bool HasLeft(void);

	STATUS Update(dp_t *p, bool r = false);
};
friend class PlayerStat;


class PlayerStats:public List<PlayerStat>
{
protected:

	STATUS Send(dp_t *p, dpid_t id, dpid_t from = dp_ID_BROADCAST);

	void Left(dpid_t id);
public:

	PlayerStats(PlayerStats *l = NULL);

	bool Handle(Message *m);

	bool Handle(dp_t *p, Message *m, dpid_t from = dp_ID_BROADCAST);
};
PlayerStats *m_playerStats;
friend class PlayerStats;

class PlayerSetup:public Player, public Packet
{
friend class NETFunc;
#define nf_PLAYERDESCLEN 128
private:
	char m_description[nf_PLAYERDESCLEN];
protected:

	void Pack(void);

	void Unpack(void);
public:

	PlayerSetup(void);

	PlayerSetup(Player *p);

	~PlayerSetup();

	void SetName(char *n);

	void SetBlob(char *b);

	void SetBlob(void *b, size_t s);

	void SetBlobLen(unsigned char l);

	char *GetDescription(void);

	void SetDescription(char *d);

	void SetGroup(char group);

	STATUS Save(FILE *f);

	STATUS Load(FILE *f);

	STATUS Update(void);


	void SetReadyToLaunch(bool b);
};
friend class PlayerSetup;

class Game;
class Lobby;
class Session:public Key
{
friend class NETFunc;
friend class ListHandler<Game>;
friend class ListHandler<Lobby>;
friend class ListHandler<Session>;
protected:
	dp_session_t m_session;
	long m_flags;
public:

	void SetKey(void);

	Session(void);

	Session(dp_object_t *o, KeyStruct *k, long f);

	void WriteToFile(FILE *saveFile) const;
	void ReadFromFile(FILE *saveFile);

	char *GetName(void);

	char *GetPassword(void);

	short GetPlayers(void);

	short GetMaxPlayers(void);

	short GetFree();

	char *GetUserField();

	dp_karma_t GetKarma();

	bool IsLobby();

	bool IsServer();

	bool IsClosed();

	bool IsMine(void);

	bool IsCurrentSession(void);
};
static Session m_session;
friend class Session;


class Game:public Session
{
friend class NETFunc;
#define nf_LAUNCHED 0x40
#define nf_SYNCLAUNCH 0x20
protected:
	bool m_hostile;
public:

	Game(void);

	Game(dp_object_t *o, KeyStruct *k, long f);

	Game(Session *s);

	void WriteToFile(FILE *saveFile) const;
	void ReadFromFile(FILE *saveFile);

	char GetGroups();

	void Set(dp_session_t *s);

	bool IsLaunched(void);

	bool IsSyncLaunch(void);

	bool IsHostile(void);

	void SetHostile(bool h);
};
friend class Game;


class Lobby:public Session {
friend class NETFunc;
bool m_bad;
public:

	Lobby(void);

	Lobby(dp_object_t *o, KeyStruct *k, long f);

	void SetBad(bool b = true);

	bool IsBad(void);
};
static Lobby s_lobby;
friend class Lobby;

class PlayerList
{
	public:
	class Players:public ListHandler<Player> {
		void Insert(Player *) {};
		void Delete(Player *) {};
		void Change(Player *) {};
		void Destroy(void) {};
	};
	static Players *s_players;
	static int s_count;

	PlayerList(void);

	~PlayerList();

	void SetKey(KeyStruct *k);

	Player *FindPlayer(dpid_t id);
};

class GameSetup : public Game, public Packet
{
friend class NETFunc;
#define nf_GAMEDESCLEN 128
	char	m_description[nf_GAMEDESCLEN];

public:
	void SetLaunched(bool b);
protected:

	STATUS Send(dp_t *p, dpid_t id, dpid_t from = dp_ID_BROADCAST);

	void Pack(void);

	void Unpack(void);
public:

	GameSetup(void);

	GameSetup(Game *g);

	~GameSetup();

	void WriteToFile(FILE *saveFile) const;
	void ReadFromFile(FILE *saveFile);

	char *GetDescription(void);

	void SetDescription(char *d);

	void SetName(char *n);

	void SetPassword(char *p);

	void SetSize(short s);

	void SetUserField(char *u);

	void SetUserField(void *u, size_t s);

	void SetSyncLaunch(bool s);

	void SetClosed(bool c);

	void SetHost(NETFunc *n);

	void SetGroups(char groups);

	bool Handle(Message *m);

	bool Handle(dp_t *p, Message *m, dpid_t from = dp_ID_BROADCAST);

	STATUS Save(FILE *f);

	STATUS Load(FILE *f);

	STATUS Update(bool b = false);
};
friend class GameSetup;




class Chat:public MessageHandler, public PlayerList {
	NETFunc *m_netf;
public:
	enum Type {
		PRIVATE = 'r',
		PUBLIC = 'p',
		GROUP = 'g',
		SYSTEM = 's'
	};

	typedef char TYPE;


	STATUS	Send(Player *p, char *m);

	STATUS	SendGroup(char *m);

	virtual void Receive(Player *p, TYPE t, char *m) = 0;

	bool Handle(Message *m);
};

	NETFunc(void);

	~NETFunc(void);

	STATUS Quit(void);

	static STATUS GetStatus(void);




	STATUS SetPlayerSetup(PlayerSetup *p);

	static STATUS SetPlayerSetupPlayer(PlayerSetup *p);

	static STATUS SetPlayerSetupPacket(PlayerSetup *p);

	STATUS GetPlayerSetupPacket(Player *p);

	STATUS SetReadyToLaunch(bool b);

	STATUS SetGroupMaster(Player *p);

	static STATUS SetRemotePlayerRecord(PlayerSetup *p);

	static STATUS SetRemotePlayerPacket(PlayerSetup *p);




	STATUS SetGameSetup(GameSetup *g);

	static STATUS SetGameSetupPacket(GameSetup *g);

	static STATUS SetGameSetupSession(GameSetup *g);

	STATUS InsertAIPlayer(AIPlayer *p);

	STATUS DeleteAIPlayer(AIPlayer *p);

	STATUS ChangeAIPlayer(AIPlayer *p);











	STATUS Launch(void);

	static STATUS UnLaunchAll(void);

	STATUS Login(char *username, char *password);

	STATUS SetTransport(Transport *t);

	bool NeedUpdate(void);

	STATUS DoUpdate(void);

	STATUS SetServer(Server *s);

	static bool IsHost();

	static bool GotGameSetup();

	void SetHost(bool h);

	static bool IsHostile(Key *s);

	static bool IsMuted(Key *p);

	void Mute(Key *p, bool m);

	bool Connected(void);

	void CancelDial(void);

	STATUS Connect(void);

	STATUS Connect(dp_t *d, PlayerStats *stats = NULL, bool host = false);

	void ReConnect(void);

	STATUS Reset(void);

	STATUS Connect(char *file);

	STATUS Join(Game *g, const char *password = "");

	STATUS Join(Lobby *l);

	STATUS Create(GameSetup *g);

	STATUS Leave();

	STATUS Disconnect();

	static STATUS Kick(Player *p);

	static void Receive(void);

	Message *GetMessage(void);

	dp_t *GetDP(void);

	Session *GetSession(void);

	Player *GetPlayer(void);

	TransportSetup *GetTransport(void);

	void PushChatMessage(char *m);

	static void PushMessage(Message *m);

	bool HandleMessage(Message *p);

	static STATUS Send(dp_t *p, Message *m, dpid_t id, dpid_t from = dp_ID_BROADCAST, bool r = true);







private:
#ifdef __AUI_USE_SDL__
	SDL_Thread *m_threadHandle;
	Uint32 m_threadId;
#else
	HANDLE m_threadHandle;
	DWORD m_threadId;
#endif
	static long s_cancelDial;

	dp_appParam_t m_appParam;

	static dp_t *s_dp;

	static dp_result_t	s_result;

	static char s_buffer[dpio_MAXLEN_UNRELIABLE];

	static size_t s_size;

	static dpid_t s_source;

	static dpid_t s_destination;

	static int s_PlayerSetupPacketKey;

	static STATUS s_status;

	STATUS m_nextStatus;

	static bool s_connected;

	static bool s_reconnected;

	int m_retries;
public:

	static PlayerSetup s_playerSetup;
private:

	static bool s_playerset;

	static bool s_setplayerpacket;

	static bool s_setplayer;
public:

	static GameSetup gameSetup;
private:

	static bool s_game;

	static bool s_theHost;

	static bool s_launch;

	static bool s_launched;

	static bool s_canlaunch;

	static bool s_setgamepacket;

	static bool s_setgame;

	static bool s_gotgamepacket;

	static bool s_gotgame;

bool Handle(Message *m);

void Execute(void);

STATUS Close(void);

static NETFUNC_CALLBACK_RESULT(int)
SessionCallBack(dp_session_t *s, long *pTimeout, long flags, void *context);

static NETFUNC_CALLBACK_RESULT(void)
PlayerCallBack(dpid_t id, dp_char_t *n, long flags, void *context);




class Lobbies:public List<Lobby> {
public:

	Lobby *FindBest(void);

	void Reset(void);
};
Lobbies s_lobbies;
friend class Lobbies;




class Hostiles:public List<Key> {
public:

	bool Check(Key *s);
};
static Hostiles s_hostiles;
friend class Hostiles;




class Mutes:public List<Key> {
public:

	bool Check(Key *p);
};
static Mutes s_mutes;
friend class Mutes;
};

#endif
