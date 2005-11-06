#ifndef __NS_CUSTOMLISTBOX_H__
#define __NS_CUSTOMLISTBOX_H__

#include "ns_listbox.h"
#include "ns_session.h"
#include "ns_lobby.h"
#include "ns_game.h"
#include "ns_player.h"
#include "ns_rplayer.h"
#include "ns_gplayer.h"
#include "ns_server.h"
#include "ns_transport.h"
#include "ns_playersetup.h"
#include "ns_gamesetup.h"
#include "ns_aiplayer.h"
#include "ns_aiplayersetup.h"

class ns_HPlayerListBox;

typedef ns_Item<NETFunc::Session, ns_Session> ns_SessionItem;

typedef ns_Item<NETFunc::Lobby, ns_Lobby> ns_LobbyItem;

typedef ns_Item<NETFunc::Game, ns_Game> ns_GameItem;

typedef ns_Item<NETFunc::Player, ns_Player> ns_PlayerItem;

typedef ns_Item<NETFunc::Player, ns_RPlayer> ns_RPlayerItem;

typedef ns_Item<NETFunc::Player, ns_GPlayer> ns_GPlayerItem;

typedef ns_Item<NETFunc::Server, ns_Server> ns_ServerItem;

typedef ns_Item<NETFunc::Transport, ns_Transport> ns_TransportItem;

typedef ns_Item<nf_PlayerSetup, ns_PlayerSetup> ns_PlayerSetupItem;

typedef ns_Item<nf_GameSetup, ns_GameSetup> ns_GameSetupItem;

typedef ns_Item<nf_AIPlayer, ns_AIPlayer> ns_AIPlayerItem;

typedef ns_Item<nf_AIPlayer, ns_AIPlayerSetup> ns_AIPlayerSetupItem;


class FakeTransport:public NETFunc::Transport {
public:
	enum SUBTYPE {
		UNKNOWNSUBTYPE,
		EMAIL,
		HOTSEAT
	};
	TYPE GetType(void) { return UNKNOWN; }
	virtual SUBTYPE GetSubType(void) = 0;
};

class EmailTransport:public FakeTransport {
public:
	EmailTransport();
	SUBTYPE GetSubType(void) { return EMAIL; }
};

class HotseatTransport:public FakeTransport {
public:
	HotseatTransport();
	SUBTYPE GetSubType(void) { return HOTSEAT; }
};

class ns_TransportListBox:public ns_ListBox<NETFunc::Transport, ns_Transport> {
public:

	static sint32 m_version;

	ns_TransportListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_TransportListBox();
};

template <class NFT, class NST>
class ns_FileDataListBox:public NETFunc::Keys,public ns_ListBox<NFT, NST> {
private:
	char m_filename[65];
public:
	ns_FileDataListBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
#if defined(_MSC_VER)
        ControlActionCallback * ActionFunc  = NULL,
#else
		typename ns_FileDataListBox<NFT,NST>::ControlActionCallback *ActionFunc = NULL,
#endif

		void *cookie = NULL,
		char *filename = "")

		:
		aui_ImageBase( ldlBlock),
		aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
		Keys(),
		ns_ListBox<NFT, NST>(
		retval,
		id,
		ldlBlock,
		ActionFunc,
		cookie )
	{

		strncpy(m_filename, filename, 64);
		FILE *file = fopen(filename, "rb");
		if(file) {
			
			sint32 test = 0;
			fread( &test, sizeof( sint32 ), 1, file );

			
			
			if ( test == NFT::m_version )
			{
				NFT *t;
				do {
					t = new NFT();
					t->SetKey(&(this->curkey));
					if(t->Load(file) == NETFunc::OK) {

						InsertItem(t);
					}
					else {
						delete t;
						t = 0;
					}
				} while(t);
			}
			fclose(file);
			if(size())
				SelectItem((sint32)0);
		}
	}

	void InsertItem(NFT *t) {
		NextKey();
		t->SetKey(&(this->curkey));
		Insert(Add(t));
	}

	void ChangeItem(NFT *t) {
		Change(Chg(t));
	}

	void DeleteItem(NFT *t) {
		Delete(t);
		Del(t);
	}

	AUI_ERRCODE Save(void) {
		
		memset(&(this->curkey), 0, sizeof(this->curkey));
		this->curkey.len = 1;
		FILE *file = fopen(m_filename, "wb");
		if(file) {
			
			
			fwrite( &NFT::m_version, sizeof( sint32 ), 1, file );

			
			
			sint32 first = -1;
			sint32 j = 0;
			typename ns_FileDataListBox<NFT,NST>::iterator i = begin();
			for(; i!=end(); i++, j++) {
				if ( FindItem( *i ) == GetSelectedItem() )
				{
					(*i)->SetKey(&(this->curkey));
					(*i)->Save(file);
					NextKey();

					first = j;
					break;
				}
			}
			for(i=begin(), j=0; i!=end(); i++, j++) {
				if ( j != first )
				{
					(*i)->SetKey(&(this->curkey));
					(*i)->Save(file);
					NextKey();
				}
			}
			fclose(file);
		}
		return AUI_ERRCODE_OK;
	}
};

class ns_PlayerSetupListBox:public ns_FileDataListBox<nf_PlayerSetup, ns_PlayerSetup> {
public:
	ns_PlayerSetupListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		char *filename = "nsplayer.cfg");
};

class ns_GameSetupListBox:public ns_FileDataListBox<nf_GameSetup, ns_GameSetup> {
public:
	ns_GameSetupListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		char *filename = "nsgame.cfg");
};

class ns_AIPlayerSetupListBox:public ns_FileDataListBox<nf_AIPlayer, ns_AIPlayerSetup> {
public:
	ns_AIPlayerSetupListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		char *filename = "nsai.cfg");
};

class ns_SessionListBox:public ns_ListBox<NETFunc::Session, ns_Session> {
public:
	ns_SessionListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
};

class ns_LobbyListBox:public ns_ListBox<NETFunc::Lobby, ns_Lobby> {
public:
	ns_LobbyListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
};

class ns_GameListBox:public ns_ListBox<NETFunc::Game, ns_Game> {
public:
	ns_GameListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
};

class ns_PlayerListBox:public ns_ListBox<NETFunc::Player, ns_Player> {
public:
	ns_PlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_PlayerListBox();

	aui_TextBase *m_pingFastStyle;
	aui_TextBase *m_pingMedStyle;
	aui_TextBase *m_pingSlowStyle;

	virtual void Insert( NETFunc::Player *object );
	virtual void Change( NETFunc::Player *object );

	void ColorCodePingTime( NETFunc::Player *object );
};

class ns_RPlayerListBox:public ns_ListBox<NETFunc::Player, ns_RPlayer> {
public:
	ns_RPlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
};

class ns_ServerListBox:public ns_ListBox<NETFunc::Server, ns_Server> {
public:
	ns_ServerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_ServerListBox();

	aui_TextBase *m_pingFastStyle;
	aui_TextBase *m_pingMedStyle;
	aui_TextBase *m_pingSlowStyle;

	virtual void Insert( NETFunc::Server *object );
	virtual void Change( NETFunc::Server *object );

	void ColorCodePingTime( NETFunc::Server *object );
};



class ns_GPlayerListBox:public ns_ListBox<NETFunc::Player, ns_GPlayer> {
public:
	ns_GPlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ns_HPlayerListBox *hplayerlistbox,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_GPlayerListBox();

	
	virtual void Insert( NETFunc::Player *object );
	virtual void Delete( NETFunc::Player *object );
	virtual void Change( NETFunc::Player *object );
	virtual void Destroy( void );
	void EnableTribeButton( NETFunc::Player *player );

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );

	ns_HPlayerItem *FindHPlayerItem( NETFunc::Player *player );
	void UpdateHPlayerItem(
		NETFunc::Player *player,
		bool selfMotivated = false );

	ns_HPlayerListBox *m_hplayerlistbox;

	aui_TextBase *m_pingFastStyle;
	aui_TextBase *m_pingMedStyle;
	aui_TextBase *m_pingSlowStyle;

	void ColorCodePingTime( NETFunc::Player *object );
};



class ns_AIPlayerListBox:public ns_ListBox<nf_AIPlayer, ns_AIPlayer> {
public:
	ns_AIPlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ns_HPlayerListBox *hplayerlistbox,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	
	virtual void Insert( nf_AIPlayer *object );
	virtual void Delete( nf_AIPlayer *object );
	virtual void Change( nf_AIPlayer *object );
	virtual void Destroy( void );

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );

	ns_HPlayerItem *FindHPlayerItem( nf_AIPlayer *player );
	void UpdateHPlayerItem(
		nf_AIPlayer *player,
		bool selfMotivated = false );

	ns_HPlayerListBox *m_hplayerlistbox;
};

#endif 
