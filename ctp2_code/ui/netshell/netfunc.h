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

#ifdef USE_SDL
#include <SDL.h>
#include <SDL_thread.h>
#endif

class NETFunc {
public:

	static dp_species_t GameType;

	static const char *LobbyName;

	static const char *DllPath;

	static	char	servername[64];
	static	char	playername[dp_PNAMELEN];
	static	char	sessionname[dp_SNAMELEN];


enum STATUS {
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





static void StringMix(int c, char *mix, const char *msg, ...);


static char *StringDup(const char *s);


class Timer {
	DWORD	start;
	DWORD	finish;
	bool	done;
public:

	Timer(void);

	void Start(int d);

	bool Finished(void);
};
Timer timer;
Timer timeout;


struct KeyStruct {
	short len;
	char buf[dp_KEY_MAXLEN + 2];
};


class Keys {
friend class NETFunc;
protected:
	KeyStruct curkey;
public:

	Keys(void);
	void NextKey(void);
};


class Key {
friend class NETFunc;
protected:
	KeyStruct key;
public:

	Key(void);

	Key(const Key *k);

	Key(KeyStruct *k);

	bool Equals(const Key *k) const;

	bool Equals(const KeyStruct *k) const;

	virtual void SetKey(void) {};

	KeyStruct *GetKey(void);
};

template<class T>
class List:public std::list<T *> {
friend class NETFunc;
public:
#if defined(_MSC_VER)
	iterator Find(T *t) {
		iterator i = end();
#else
	typename NETFunc::List<T>::iterator Find(T *t) {
		typename NETFunc::List<T>::iterator i = this->end();
#endif
		if(this->size())
		for(i = this->begin(); i != this->end(); i++)
			if(t->Equals(*i))
				break;
		return i;
	}

	void Clr(void) {
#if defined(_MSC_VER)
		for(iterator i = begin(); i != end(); i++)
#else
		for(typename NETFunc::List<T>::iterator i = this->begin(); i != this->end(); i++)
#endif
			delete *i;
		this->clear();
	}

	T *Add(T *t) {
#if defined(_MSC_VER)
		iterator i = Find(t);
#else
		typename NETFunc::List<T>::iterator i = Find(t);
#endif
		if(i == this->end()) {
			this->push_back(t);
			return t;
		} else {
			memcpy(*i, t, sizeof(T));
			delete t;
			return *i;
		}
	}

	void Del(T *t) {
#if defined(_MSC_VER)
		iterator i = Find(t);
#else
		typename NETFunc::List<T>::iterator i = Find(t);
#endif

		if(i != this->end()) {
			delete *i;
			this->erase(i);
		}
	}

	T *Chg(T *t) {
#if defined(_MSC_VER)
		iterator i = Find(t);
#else
		typename NETFunc::List<T>::iterator i = Find(t);
#endif

		if(i != this->end()) {
			memcpy(*i, t, sizeof(T));
			return *i;
		} else {
			T *n = new T(*t);
			this->push_back(n);
			return n;
		}
	}

	virtual ~List(void) {
#if defined(_MSC_VER)
		for(iterator i = begin(); i != end(); i++)
#else
		for(typename NETFunc::List<T>::iterator i = this->begin(); i != this->end(); i++)
#endif
			delete *i;
	}

	List(List<T> *l) {
		if(l)
#if defined(_MSC_VER)
			for(iterator i = l->begin(); i != l->end(); i++)
#else
			for(typename NETFunc::List<T>::iterator i = l->begin(); i != l->end(); i++)
#endif
				Add(new T(**i));
	}

	List(void) {
	}
};

class Messages;




class Message:public Key {
friend class NETFunc;
	char *body;
	dpid_t sender;
	size_t size;

	bool newbody;
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

	Message(CODE c, const void *p, size_t s);

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

class Messages:public List<Message> {
public:

	Messages(void);

	~Messages(void);

	void Push(Message *m);

	Message *Pop(void);
};
static Messages messages;







class Packet {
#define nf_MAX_PACKETSIZE dpio_MAXLEN_RELIABLE - sizeof(Message::CODE)
public:
	typedef unsigned char SizeT;
protected:
	SizeT size;

	char	body[nf_MAX_PACKETSIZE];

	char	*first;
public:

	const char *GetBody(void) {
		return body;
	}

	int GetSize(void) {
		return size;
	}

	Packet() {

		size = 0;

		first = body;
	}

	void Set(SizeT s, void *b) {
		memcpy(body, b, s);
		size = s;
		first = body;
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
		size = 0;
		first = body;
	}

	void Grow(SizeT s) {
		size += s;

		assert(size <= nf_MAX_PACKETSIZE);
	}

	template<typename type>
	void Push(type t) {
		Grow(sizeof(type));
		*(type *)(body + size - sizeof(type)) = t;
	}

	template<typename type>
	void Pop(type& t) {

		t = *(type *)(first);
		first += sizeof(type);

	}

	void Push(const char *c) {
		int s = strlen(c) + 1;
		Grow(s);
		strcpy(body + size - s, c);
	}

	void Pop(char *c) {
		int s = strlen(first) + 1;

		strcpy(c, first);
		first += s;

	}
};






class MessageHandler {
#define nf_MAX_HANDLERS 128

	static MessageHandler *hList[nf_MAX_HANDLERS];

	static int hCount;

	virtual bool Handle(Message *m) = 0;
protected:

	MessageHandler(void);

	virtual ~MessageHandler(void);
public:

	void Unregister(void);

	static bool HandleAll(Message *m);
};







template<class T>
class ListHandler:public MessageHandler, public List<T>, public Key {
	STATUS status;
public:

	ListHandler(void) {
		SetKey();
	}

	virtual ~ListHandler(void) {
	}

	void SetKey();
	bool Equals(KeyStruct *k) {

		if(key.buf[0] == dp_KEY_PLAYERS)
			return (k->buf[0] == dp_KEY_PLAYERS && memcmp(&key.buf[key.len - 2], &k->buf[k->len - 2], 2) == 0);
		else
			return Key::Equals(k);
	}

	void SetKey(KeyStruct *k) {
		if(!this->empty()) {
			Destroy();
			this->Clr();
		}
		if(key.buf[0] == dp_KEY_PLAYERS) {
			if(k) {
				memcpy(&key.buf[1], &k->buf[0], k->len);
				key.len = k->len + 1;
			} else
				memset(&key.buf[1], 0, key.len - 1);
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




class Server:public Key {
	dp_serverInfo_t server;
public:

	void SetKey(void) {};

	Server(void);

	Server(dp_object_t *o, KeyStruct *k, long f);

	virtual ~Server(void);

	const char *GetName(void);

	int GetPlayers(void);

	int GetPing(void);

	dp_serverInfo_t *GetServer(void);
};


class Contact:public Key {
	char	*name;
	char	*number;
public:

	Contact(const char *n, const char *p);

	Contact(void);

	virtual ~Contact(void);

	const char *GetName(void);

	const char *GetNumber(void);

	void SetName(const char *n);

	void SetNumber(const char *p);
};




class ContactList:public List<Contact> {
public:

	ContactList(void);

	~ContactList(void);
};
ContactList	contactList;




class Port:public Key {
#define nf_PORTINITLEN 128
	commPortName_t port;
	int		baud;
	char init[nf_PORTINITLEN];
public:

	Port(commPortName_t *p, int b, const char *i);

	Port(void);

	virtual ~Port(void);

	commPortName_t *GetPort(void);

	int GetNumber(void);

	const char *GetName(void);

	int GetBaud(void);

	const char *GetInit(void);
};

class Transport;


class PortList:public List<Port> {
public:

	PortList(Transport *t);

	PortList(void);

	~PortList(void);
};

PortList portList;
friend class PortList;




#ifdef USE_SDL
static int ConnectThread(void *t);
static int ReConnectThread(void *r);
#else
static DWORD WINAPI ConnectThread(LPVOID t);
static DWORD WINAPI ReConnectThread(LPVOID r);
#endif












class Transport:public Key {
friend class PortList;
protected:
	dp_transport_t		transport;
	comm_driverInfo_t	description;
	commInitReq_t		parameters;

	STATUS status;
public:

	enum TYPE {
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

	const char *GetName(void);

	const char *GetFileName(void);

	virtual STATUS SetPort(long p);

	virtual STATUS SetPort(Port *p);

	virtual STATUS SetContact(Contact *c);
};





class TransportSetup {
	dp_transport_t		transport;
	commInitReq_t		parameters;

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
static TransportSetup *transport;
friend class TransportSetup;


static Transport::TYPE GetTransportType(const comm_driverInfo_t *c);


class Internet:public Transport {
public:

	Internet(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~Internet(void);

	STATUS SetPort(long p);

	TYPE GetType(void);
};
friend class Internet;


class IPX:public Transport {
public:

	IPX(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~IPX(void);

	TYPE GetType(void);
};
friend class IPX;


class Modem:public Transport {
public:

	Modem(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~Modem(void);

	STATUS SetContact(Contact *c);

	STATUS SetPort(Port *p);

	TYPE GetType(void);
};
friend class Modem;


class NullModem:public Transport {
public:

	NullModem(const comm_driverInfo_t *d, const dp_transport_t *t, KeyStruct *k);

	~NullModem(void);

	STATUS SetPort(Port *p);

	TYPE GetType(void);
};
friend class NullModem;


class TransportList:public List<Transport> {
	KeyStruct key;

#ifdef WIN32
static void __stdcall
#else
static void
#endif
	CallBack(const dp_transport_t *t, const comm_driverInfo_t *d, void *context);
public:

	TransportList(void);

	~TransportList(void);
};
TransportList transportList;
friend class TransportList;










class AIPlayer:public Key, public Packet {
friend class NETFunc;
protected:
	char	name[dp_PNAMELEN];
	unsigned char group;
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


class AIPlayers:public List<AIPlayer>, public Keys {
protected:

	STATUS Send(dp_t *p, dpid_t id, dpid_t from = dp_ID_BROADCAST);
public:

	AIPlayers(AIPlayers *l = NULL);

	bool Handle(Message *m);

	bool Handle(dp_t *p, Message *m, dpid_t from = dp_ID_BROADCAST);
};
AIPlayers *aiPlayers;
friend class AIPlayers;

class Players;




class Player:public Key {
friend class NETFunc;
friend class Players;
#define nf_GROUPMASTER	0x40
#define nf_READYLAUNCH	0x20
#define nf_GROUPNUMBER	0x1f
protected:
	dp_playerId_t	player;
	long			flags;
	short			latency;
	bool			muted;

	void SetGroupMaster(bool b);
public:

	void SetKey(void);

	Player(void);

	Player(dp_object_t *o, KeyStruct *k, long f, short l = 0);

	void Set(dp_playerId_t *p);

	virtual ~Player(void);

	dpid_t GetId(void);

	const char *GetName(void);

	unsigned const char *GetBlob(void);

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
static Player player;
static dp_uid_t userId;


class Players:public List<Player> {
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


class PlayerStat:public Key, public Packet {
friend class NETFunc;
protected:
	char	name[dp_PNAMELEN];
	unsigned char group;
	bool isingame;
	bool hasleft;
public:

	void Pack(void);

	void Unpack(void);

	PlayerStat(void);

	~PlayerStat(void);

	void SetName(const char *n);

	const char *GetName(void);

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


class PlayerStats:public List<PlayerStat> {
protected:

	STATUS Send(dp_t *p, dpid_t id, dpid_t from = dp_ID_BROADCAST);

	void Left(dpid_t id);
public:

	PlayerStats(PlayerStats *l = NULL);

	bool Handle(Message *m);

	bool Handle(dp_t *p, Message *m, dpid_t from = dp_ID_BROADCAST);
};
PlayerStats *playerStats;
friend class PlayerStats;












class PlayerSetup:public Player, public Packet {
friend class NETFunc;
#define nf_PLAYERDESCLEN 128
private:
	char description[nf_PLAYERDESCLEN];
protected:

	void Pack(void);

	void Unpack(void);
public:

	PlayerSetup(void);

	PlayerSetup(Player *p);

	~PlayerSetup();

	void SetName(const char *n);

	void SetBlob(const char *b);

	void SetBlob(void *b, unsigned char s);

	void SetBlobLen(unsigned char l);

	const char *GetDescription(void);

	void SetDescription(const char *d);

	void SetGroup(char group);

	STATUS Save(FILE *f);

	STATUS Load(FILE *f);

	STATUS Update(void);


	void SetReadyToLaunch(bool b);
};
friend class PlayerSetup;












class Game;
class Lobby;
class Session:public Key {
friend class NETFunc;
friend class ListHandler<Game>;
friend class ListHandler<Lobby>;
friend class ListHandler<Session>;
protected:
	dp_session_t session;
	long flags;
public:

	void SetKey(void);

	Session(void);

	Session(dp_object_t *o, KeyStruct *k, long f);

	const char *GetName(void);

	const char *GetPassword(void);

	short GetPlayers(void);

	short GetMaxPlayers(void);

	short GetFree();

	const char *GetUserField();

	dp_karma_t GetKarma();

	bool IsLobby();

	bool IsServer();

	bool IsClosed();

	bool IsMine(void);

	bool IsCurrentSession(void);
};
static Session session;
friend class Session;


class Game:public Session {
friend class NETFunc;
#define nf_LAUNCHED 0x40
#define nf_SYNCLAUNCH 0x20
protected:
	bool hostile;
public:

	Game(void);

	Game(dp_object_t *o, KeyStruct *k, long f);

	Game(Session *s);

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
bool bad;
public:

	Lobby(void);

	Lobby(dp_object_t *o, KeyStruct *k, long f);

	void SetBad(bool b = true);

	bool IsBad(void);
};
static Lobby lobby;
friend class Lobby;




class PlayerList {
	public:
	class Players:public ListHandler<Player> {
		void Insert(Player *) {};
		void Delete(Player *) {};
		void Change(Player *) {};
		void Destroy(void) {};
	};
	static Players *players;
	static int count;

	PlayerList(void);

	~PlayerList();

	void SetKey(KeyStruct *k);

	Player *FindPlayer(dpid_t id);
};








class GameSetup:public Game, public Packet {
friend class NETFunc;
#define nf_GAMEDESCLEN 128
	char	description[nf_GAMEDESCLEN];

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

	const char *GetDescription(void);

	void SetDescription(const char *d);

	void SetName(const char *n);

	void SetPassword(const char *p);

	void SetSize(short s);

	void SetUserField(const char *u);

	void SetUserField(const void *u, short s);

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
	NETFunc *netf;
public:
	enum Type {
		PRIVATE = 'r',
		PUBLIC = 'p',
		GROUP = 'g',
		SYSTEM = 's'
	};

	typedef char TYPE;


	STATUS	Send(Player *p, const char *m);

	STATUS	SendGroup(const char *m);

	virtual void Receive(Player *p, TYPE t, const char *m) = 0;

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

	STATUS Login(const char *username, const char *password);

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

	STATUS Connect(const char *file);

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

	void PushChatMessage(const char *m);

	static void PushMessage(Message *m);

	bool HandleMessage(Message *p);

	static STATUS Send(dp_t *p, Message *m, dpid_t id, dpid_t from = dp_ID_BROADCAST, bool r = true);


private:
#ifdef USE_SDL
	SDL_Thread *threadHandle;
	Uint32 threadId;
#else
	HANDLE threadHandle;
	DWORD threadId;
#endif
	static long cancelDial;

	dp_appParam_t appParam;

	static dp_t *dp;

	static dp_result_t	result;

	static char buffer[dpio_MAXLEN_UNRELIABLE];

	static size_t size;

	static dpid_t source;

	static dpid_t destination;

	static int PlayerSetupPacketKey;

	static STATUS status;

	STATUS nextStatus;

	static bool connected;

	static bool reconnected;

	int retries;
public:

	static PlayerSetup playerSetup;
private:

	static bool playerset;

	static bool setplayerpacket;

	static bool setplayer;
public:

	static GameSetup gameSetup;
private:

	static bool	game;

	static bool	host;

	static bool	launch;

	static bool	launched;

	static bool	canlaunch;

	static bool setgamepacket;

	static bool setgame;

	static bool gotgamepacket;

	static bool gotgame;

bool Handle(Message *m);

void Execute(void);

STATUS Close(void);

#ifdef WIN32
static int __stdcall
#else
static int
#endif
SessionCallBack(dp_session_t *s, long *pTimeout, long flags, void *context);

#ifdef WIN32
static void __stdcall
#else
static void
#endif
PlayerCallBack(dpid_t id, dp_char_t *n, long flags, void *context);




class Lobbies:public List<Lobby> {
public:

	Lobby *FindBest(void);

	void Reset(void);
};
Lobbies lobbies;
friend class Lobbies;




class Hostiles:public List<Key> {
public:

	bool Check(Key *s);
};
static Hostiles hostiles;
friend class Hostiles;




class Mutes:public List<Key> {
public:

	bool Check(Key *p);
};
static Mutes mutes;
friend class Mutes;
};

#endif
