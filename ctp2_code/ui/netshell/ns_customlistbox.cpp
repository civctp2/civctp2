//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Listbox for network game setup
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
// - Replaced typename T in specialized template member function by the 
//   the type for that the function is specialized, by Martin Gühmann.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "netshell.h"
#include "ns_customlistbox.h"
#include "ns_civlistbox.h"
#include "allinonewindow.h"

#include "c3_button.h"
#include "aui_stringtable.h"
#include "aui_bitmapfont.h"
#include "ns_gplayer.h"
#include "ns_aiplayer.h"

#include "ns_tribes.h"
#include "ns_spinner.h"
#include "ns_string.h"


sint32 ns_TransportListBox::m_version = 102;


bool NETFunc::ListHandler<nf_PlayerSetup>::Handle(NETFunc::Message *m) {return false;}
void NETFunc::ListHandler<nf_PlayerSetup>::SetKey(void) {}

bool NETFunc::ListHandler<nf_GameSetup>::Handle(NETFunc::Message *m) {return false;}
void NETFunc::ListHandler<nf_GameSetup>::SetKey(void) {}

bool NETFunc::ListHandler<nf_AIPlayer>::Handle(NETFunc::Message *m) {
	nf_AIPlayer t;
	if(m->GetCode() == NETFunc::Message::ENTERGAME) {
		Destroy();
		Clr();
		return true;
	} else if(m->GetCode() == NETFunc::Message::ADDAIPLAYER) {
		t.Set(m->GetBodySize(), m->GetBody());
		
		Insert(Add(new nf_AIPlayer(t)));
		return true;
	} else if(m->GetCode() == NETFunc::Message::DELAIPLAYER) {
		t.Set(m->GetBodySize(), m->GetBody());
		
		Delete(&t);
		Del(&t);
		return true;
	} else if(m->GetCode() == NETFunc::Message::CHGAIPLAYER) {
		t.Set(m->GetBodySize(), m->GetBody());
		
		Change(Chg(&t));
		return true;
	}
	return false;
}
void NETFunc::ListHandler<nf_AIPlayer>::SetKey(void) {}

bool NETFunc::ListHandler<NETFunc::Transport>::Handle(NETFunc::Message *m) {return false;}
void NETFunc::ListHandler<NETFunc::Transport>::SetKey(void) {}

ns_PlayerSetupListBox::ns_PlayerSetupListBox (
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie,
	char *filename)

	:ns_FileDataListBox<nf_PlayerSetup, ns_PlayerSetup>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie,
	filename),

	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL )
{
	SetForceSelect( TRUE );
}

ns_GameSetupListBox::ns_GameSetupListBox (
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie,
	char *filename)

	:ns_FileDataListBox<nf_GameSetup, ns_GameSetup>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie,
	filename),

	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
}

ns_AIPlayerSetupListBox::ns_AIPlayerSetupListBox (
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie,
	char *filename)

	:ns_FileDataListBox<nf_AIPlayer, ns_AIPlayerSetup>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie,
	filename),

	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
}


EmailTransport::EmailTransport()
{
	ns_String s( "strings.email" );
	strcpy( description.name, s.GetString() );
}
HotseatTransport::HotseatTransport()
{
	ns_String s( "strings.hotseat" );
	strcpy( description.name, s.GetString() );
}

ns_TransportListBox::ns_TransportListBox (
	AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock, ControlActionCallback *ActionFunc, void *cookie )
	:ns_ListBox<NETFunc::Transport, ns_Transport> (
	retval, id, ldlBlock, ActionFunc, cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
	
	NETFunc::TransportList *l = &(g_netfunc->transportList);
	NETFunc::TransportList::iterator i;
	for(i = l->begin(); i != l->end(); i++) {
		
		switch((*i)->GetType()) {
		case NETFunc::Transport::INTERNET:
			Insert(Add(new NETFunc::Internet(*(NETFunc::Internet *)*i)));
		break;
		case NETFunc::Transport::IPX:
			Insert(Add(new NETFunc::IPX(*(NETFunc::IPX *)*i)));
		break;
		case NETFunc::Transport::MODEM:
			Insert(Add(new NETFunc::Modem(*(NETFunc::Modem *)*i)));
		break;
		case NETFunc::Transport::NULLMODEM:
			Insert(Add(new NETFunc::NullModem(*(NETFunc::NullModem *)*i)));
		break;	
		default:
		break;
		}
	}

	
	



	sint32 lasttime = 0;
	FILE *f = fopen( "nstrnsprt.cfg", "rb" );
	if ( f )
	{
		
		sint32 test = 0;
		fread( &test, sizeof( sint32 ), 1, f );

		
		
		if ( test == m_version )
		{
			if ( fread( &lasttime, sizeof( sint32 ), 1, f ) != 1 )
				lasttime = 0;
		}
		fclose( f );
	}
	if ( 0 <= lasttime && lasttime < NumItems() )
		SelectItem((sint32)lasttime);
}

ns_TransportListBox::~ns_TransportListBox()
{
	sint32 lasttime = GetSelectedItemIndex();
	if ( 0 <= lasttime && lasttime < NumItems() )
	{
		FILE *f = fopen( "nstrnsprt.cfg", "wb" );
		if ( f )
		{
			
			
			fwrite( &m_version, sizeof( sint32 ), 1, f );

			fwrite( &lasttime, sizeof( sint32 ), 1, f );

			fclose( f );
		}
	}
}

ns_SessionListBox::ns_SessionListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Session, ns_Session>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
};

ns_LobbyListBox::ns_LobbyListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Lobby, ns_Lobby>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
};

ns_GameListBox::ns_GameListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Game, ns_Game>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
};

ns_PlayerListBox::ns_PlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Player, ns_Player>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL )
{
	m_pingFastStyle = new aui_TextBase( "styles.ping.fast", (MBCHAR *)NULL );
	m_pingMedStyle = new aui_TextBase( "styles.ping.med", (MBCHAR *)NULL );
	m_pingSlowStyle = new aui_TextBase( "styles.ping.slow", (MBCHAR *)NULL );
}

ns_PlayerListBox::~ns_PlayerListBox()
{
	if ( m_pingFastStyle )
	{
		delete m_pingFastStyle;
		m_pingFastStyle = NULL;
	}

	if ( m_pingMedStyle )
	{
		delete m_pingMedStyle;
		m_pingMedStyle = NULL;
	}

	if ( m_pingSlowStyle )
	{
		delete m_pingSlowStyle;
		m_pingSlowStyle = NULL;
	}
}

void ns_PlayerListBox::Insert( NETFunc::Player *player )
{
	ns_ListBox<NETFunc::Player, ns_Player>::Insert( player );
	ColorCodePingTime( player );
}

void ns_PlayerListBox::Change( NETFunc::Player *player )
{
	ns_ListBox<NETFunc::Player, ns_Player>::Change( player );
	ColorCodePingTime( player );
}

void ns_PlayerListBox::ColorCodePingTime( NETFunc::Player *player )
{
	
	aui_Control *rowParent = (aui_Control *)FindItem( player );
	if ( !rowParent ) return;

	
	if ( rowParent->ChildList()->L() < 2 ) return;
	aui_Control *control = (aui_Control *)rowParent->ChildList()->
		GetAtIndex( 1 ); 

	ns_Player *nsPlayer = FindItem( player )->GetNetShellObject();

	if ( nsPlayer->m_ping > m_pingSlowStyle->GetTextUnderline() )
	{
		control->SetTextColor( m_pingSlowStyle->GetTextColor() );
		control->SetTextShadow( m_pingSlowStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingSlowStyle->GetTextShadowColor() );
	}
	else if ( nsPlayer->m_ping > m_pingMedStyle->GetTextUnderline() )
	{
		control->SetTextColor( m_pingMedStyle->GetTextColor() );
		control->SetTextShadow( m_pingMedStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingMedStyle->GetTextShadowColor() );
	}
	else
	{
		control->SetTextColor( m_pingFastStyle->GetTextColor() );
		control->SetTextShadow( m_pingFastStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingFastStyle->GetTextShadowColor() );
	}
}


ns_RPlayerListBox::ns_RPlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Player, ns_RPlayer>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ) {
};

ns_ServerListBox::ns_ServerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Server, ns_Server>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL )
{
	m_pingFastStyle = new aui_TextBase( "styles.ping.fast", (MBCHAR *)NULL );
	m_pingMedStyle = new aui_TextBase( "styles.ping.med", (MBCHAR *)NULL );
	m_pingSlowStyle = new aui_TextBase( "styles.ping.slow", (MBCHAR *)NULL );
}

ns_ServerListBox::~ns_ServerListBox()
{
	if ( m_pingFastStyle )
	{
		delete m_pingFastStyle;
		m_pingFastStyle = NULL;
	}

	if ( m_pingMedStyle )
	{
		delete m_pingMedStyle;
		m_pingMedStyle = NULL;
	}

	if ( m_pingSlowStyle )
	{
		delete m_pingSlowStyle;
		m_pingSlowStyle = NULL;
	}
}

void ns_ServerListBox::Insert( NETFunc::Server *server )
{
	ns_ListBox<NETFunc::Server, ns_Server>::Insert( server );
	ColorCodePingTime( server );
}

void ns_ServerListBox::Change( NETFunc::Server *server )
{
	ns_ListBox<NETFunc::Server, ns_Server>::Change( server );
	ColorCodePingTime( server );
}

void ns_ServerListBox::ColorCodePingTime( NETFunc::Server *server )
{
	
	aui_Control *rowParent = (aui_Control *)FindItem( server );
	if ( !rowParent ) return;

	
	if ( rowParent->ChildList()->L() < 2 ) return;
	aui_Control *control = (aui_Control *)rowParent->ChildList()->
		GetAtIndex( 1 ); 

	if ( server->GetPing() > m_pingSlowStyle->GetTextUnderline() )
	{
		control->SetTextColor( m_pingSlowStyle->GetTextColor() );
		control->SetTextShadow( m_pingSlowStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingSlowStyle->GetTextShadowColor() );
	}
	else if ( server->GetPing() > m_pingMedStyle->GetTextUnderline() )
	{
		control->SetTextColor( m_pingMedStyle->GetTextColor() );
		control->SetTextShadow( m_pingMedStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingMedStyle->GetTextShadowColor() );
	}
	else
	{
		control->SetTextColor( m_pingFastStyle->GetTextColor() );
		control->SetTextShadow( m_pingFastStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingFastStyle->GetTextShadowColor() );
	}
}


ns_GPlayerListBox::ns_GPlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ns_HPlayerListBox *hplayerlistbox,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<NETFunc::Player, ns_GPlayer>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	m_hplayerlistbox( hplayerlistbox )
{
	m_pingFastStyle = new aui_TextBase( "styles.ping.fast", (MBCHAR *)NULL );
	m_pingMedStyle = new aui_TextBase( "styles.ping.med", (MBCHAR *)NULL );
	m_pingSlowStyle = new aui_TextBase( "styles.ping.slow", (MBCHAR *)NULL );
}

ns_GPlayerListBox::~ns_GPlayerListBox()
{
	if ( m_pingFastStyle )
	{
		delete m_pingFastStyle;
		m_pingFastStyle = NULL;
	}

	if ( m_pingMedStyle )
	{
		delete m_pingMedStyle;
		m_pingMedStyle = NULL;
	}

	if ( m_pingSlowStyle )
	{
		delete m_pingSlowStyle;
		m_pingSlowStyle = NULL;
	}
}

void ns_GPlayerListBox::ColorCodePingTime( NETFunc::Player *player )
{
	
	ns_HPlayerItem *rowParent = FindHPlayerItem( player );
	if ( !rowParent ) return;

	aui_Control *control = rowParent->GetPingItem();

	ns_GPlayer *nsPlayer = FindItem( player )->GetNetShellObject();

	if ( nsPlayer->m_ping > m_pingSlowStyle->GetTextUnderline() )
	{
		control->SetTextColor( m_pingSlowStyle->GetTextColor() );
		control->SetTextShadow( m_pingSlowStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingSlowStyle->GetTextShadowColor() );
	}
	else if ( nsPlayer->m_ping > m_pingMedStyle->GetTextUnderline() )
	{
		control->SetTextColor( m_pingMedStyle->GetTextColor() );
		control->SetTextShadow( m_pingMedStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingMedStyle->GetTextShadowColor() );
	}
	else
	{
		control->SetTextColor( m_pingFastStyle->GetTextColor() );
		control->SetTextShadow( m_pingFastStyle->GetTextShadow() );
		control->SetTextShadowColor( m_pingFastStyle->GetTextShadowColor() );
	}
}


void ns_GPlayerListBox::Insert( NETFunc::Player *player )
{
	ns_ListBox<NETFunc::Player, ns_GPlayer>::Insert( player );

	AUI_ERRCODE errcode;
	ns_HPlayerItem *item = new ns_HPlayerItem(
		&errcode,
		player,
		FALSE,
		"listitems.hplayeritem" );
	Assert( AUI_NEWOK(item,errcode) );
	if ( !AUI_NEWOK(item,errcode) ) return;

	m_hplayerlistbox->AddItem( item );

	UpdateHPlayerItem( player );

	if ( !FindItem(player)->GetNetShellObject()->IsMine() )
	{
		item->GetTribeButton()->Enable( FALSE );

		item->GetCivpointsButton()->Enable( FALSE );
		item->GetPwpointsButton()->Enable( FALSE );
	}
	else
	{
		
		if ( !g_gamesetup.GetHandicapping() )
		{
			item->GetCivpointsButton()->Enable( FALSE );
			item->GetPwpointsButton()->Enable( FALSE );
		}



	}

	
	switch ( g_allinoneWindow->GetMode() )
	{
	case g_allinoneWindow->CONTINUE_CREATE:
	case g_allinoneWindow->CONTINUE_JOIN:
		if(!g_allinoneWindow->GetScenarioInfo() ||
			!g_allinoneWindow->GetScenarioInfo()->isScenario) {
			item->GetTribeButton()->Enable( FALSE );
		}
		break;
	}

	ColorCodePingTime( player );
}


void ns_GPlayerListBox::EnableTribeButton( NETFunc::Player *player )
{
	ns_HPlayerItem *item = (ns_HPlayerItem *)FindItem(player);
	
	if(item) {
		item->GetTribeButton()->Enable(TRUE);
	}
}


void ns_GPlayerListBox::Delete( NETFunc::Player *player )
{
	ns_ListBox<NETFunc::Player, ns_GPlayer>::Delete( player );

	ns_HPlayerItem *item = FindHPlayerItem( player );
	Assert( item != NULL );
	if ( !item ) return;

	m_hplayerlistbox->RemoveItem( item->Id() );

	
	delete item;

	
	if ( g_netfunc->IsHost() )
	{
		BOOL success = g_allinoneWindow->AssignTribe(
			0,
			*(uint16 *)player->GetKey()->buf,
			FALSE,
			FALSE,
			TRUE );
		Assert( success );
	}

	
	m_hplayerlistbox->SelectItem( m_hplayerlistbox->GetSelectedItemIndex() );
}



void ns_GPlayerListBox::Change( NETFunc::Player *player )
{
	ns_ListBox<NETFunc::Player, ns_GPlayer>::Change( player );

	ns_HPlayerItem *item = FindHPlayerItem( player );
	Assert( item != NULL );
	if ( !item ) return;

	
	bool selfMotivated = true;
	if ( !g_gamesetup.GetHandicapping() ||
		 !FindItem(player)->GetNetShellObject()->IsMine() )
	{
		selfMotivated = false;
	}

	UpdateHPlayerItem( player, selfMotivated );

	
	SortByColumn(
		m_hplayerlistbox->GetSortColumn(),
		m_hplayerlistbox->GetSortAscending() );

	ColorCodePingTime( player );
}



void ns_GPlayerListBox::Destroy( void )
{
	
	for(iterator i=begin(); i!=end(); i++)
		Delete(*i);
	

}



AUI_ERRCODE ns_GPlayerListBox::SortByColumn( sint32 column, BOOL ascending )
{





	return m_hplayerlistbox->SortByColumn( column, ascending );
}



ns_HPlayerItem *ns_GPlayerListBox::FindHPlayerItem( NETFunc::Player *player )
{
	ns_HPlayerItem *item;

	ListPos position =
		m_hplayerlistbox->GetPane()->ChildList()->GetHeadPosition();
	for ( sint32 i = m_hplayerlistbox->GetPane()->ChildList()->L(); i; i-- )
	{
		item = (ns_HPlayerItem *)
			m_hplayerlistbox->GetPane()->ChildList()->GetNext( position );

		if ( !item->IsAI() && player->Equals( item->GetPlayer() ) )
			return item;
	}

	return NULL;
}



void ns_GPlayerListBox::UpdateHPlayerItem(
	NETFunc::Player *player,
	bool selfMotivated )
{
	ns_HPlayerItem *item = FindHPlayerItem( player );
	Assert( item != NULL );
	if ( !item ) return;

	nf_PlayerSetup playersetup( player );

	item->GetHostItem()->
		SetImage( player->IsHost() ? g_netshell->GetTrueBmp() : 0 );

	item->GetLaunchedItem()->
		SetImage( player->IsReadyToLaunch() ? g_netshell->GetTrueBmp() : 0 );

	
	MBCHAR name[ dp_PNAMELEN + 1 ];
	strncpy( name, player->GetName(), dp_PNAMELEN );

	
	if ( !item->GetNameItem()->GetTextFont() )
		item->GetNameItem()->TextReloadFont();

	item->GetNameItem()->GetTextFont()->TruncateString(
		name,
		item->GetNameItem()->Width() );

	item->GetNameItem()->
		SetText( name );

	MBCHAR ping[ 10 ];
	sprintf( ping, "%d", player->GetLatency() );
	item->GetPingItem()->
		SetText( ping );

	item->SetTribe( playersetup.GetTribe() );



	
	
	if ( !selfMotivated )
	{
		item->SetCivpoints( playersetup.GetCivPoints() );
		item->SetPwpoints( playersetup.GetPwPoints() );
	}

	m_hplayerlistbox->ShouldDraw();
}





AUI_ERRCODE ns_ListBox<NETFunc::Player, ns_Player>::StoreAppropriateData(
	ns_Item<NETFunc::Player, ns_Player> *item,
	sint32 i )
{
	
	static MBCHAR scratch[ k_NS_ITEM_MAXTEXT + 1 ];
#if defined(ACTIVISION_ORIGINAL)
	AUI_ERRCODE r;
#endif
	ns_Player *netShellObject = item->GetNetShellObject();
	if ( !netShellObject )
		netShellObject = ((ns_Item<NETFunc::Player, ns_Player> *)item->
						  GetParent())->
			GetNetShellObject();

#if defined(ACTIVISION_ORIGINAL)
	switch ( netShellObject->type( i ) )
	{
	case ns_Accessor<NETFunc::Player>::STRING:

		{
			
			MBCHAR name[ dp_PNAMELEN + 1 ];
 
			strncpy( name, (MBCHAR *)netShellObject->data( i ), dp_PNAMELEN );
			
			if ( !item->GetTextFont() )
				item->TextReloadFont();

			item->GetTextFont()->TruncateString(
				name,
				item->Width() );

			r = item->SetText( name );
		}
		if(netShellObject->IsMine())
			item->SetTextBold(true);
		else
			item->SetTextBold(false);
		return r;
	case ns_Accessor<NETFunc::Player>::INT:
		r = item->SetText(
			itoa( (sint32)netShellObject->data( i ), scratch, 10 ) );
		if(netShellObject->IsMine())
			item->SetTextBold(true);
		else
			item->SetTextBold(false);
		return r;
	case ns_Accessor<NETFunc::Player>::ICON:
		return item->SetIcon( (MBCHAR *)netShellObject->data( i ) );

	default:
		
		Assert( FALSE );
		return AUI_ERRCODE_INVALIDPARAM;
	}
#else	// ACTIVISION_ORIGINAL
	void *	dataPtr	= netShellObject->data(i);

	if (dataPtr)
	{
		switch (netShellObject->type(i))
		{
		case ns_Accessor<NETFunc::Player>::STRING:
			{
				MBCHAR name[dp_PNAMELEN + 1];
				strncpy(name, * reinterpret_cast<MBCHAR const * *>(dataPtr), dp_PNAMELEN);
			
				if (!item->GetTextFont())
				{
					item->TextReloadFont();
				}

				item->GetTextFont()->TruncateString(name, item->Width());
				item->SetTextBold(netShellObject->IsMine());
				return item->SetText(name);
			}

		case ns_Accessor<NETFunc::Player>::INT:
			item->SetTextBold(netShellObject->IsMine());
			return item->SetText
				(itoa(* reinterpret_cast<sint32 const *>(dataPtr), scratch, 10));

		case ns_Accessor<NETFunc::Player>::ICON:
			return item->SetIcon(* reinterpret_cast<MBCHAR * *>(dataPtr));

		default:
			break;
		} // switch
	}

	Assert(FALSE);
	return AUI_ERRCODE_INVALIDPARAM;
#endif	// ACTIVISION_ORIGINAL
}



ns_AIPlayerListBox::ns_AIPlayerListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ns_HPlayerListBox *hplayerlistbox,
		ControlActionCallback *ActionFunc,
		void *cookie )
	:ns_ListBox<nf_AIPlayer, ns_AIPlayer>(
	retval,
	id,
	ldlBlock,
	ActionFunc,
	cookie ),
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	m_hplayerlistbox( hplayerlistbox )
{
}



void ns_AIPlayerListBox::Insert( nf_AIPlayer *player )
{
	ns_ListBox<nf_AIPlayer, ns_AIPlayer>::Insert( player );

	AUI_ERRCODE errcode;
	ns_HPlayerItem *item = new ns_HPlayerItem(
		&errcode,
		player,
		TRUE,
		"listitems.hplayeritem" );
	Assert( AUI_NEWOK(item,errcode) );
	if ( !AUI_NEWOK(item,errcode) ) return;

	m_hplayerlistbox->AddItem( item );

	UpdateHPlayerItem( player );

	if ( !g_netfunc->IsHost() )
	{
		item->GetTribeButton()->Enable( FALSE );

		item->GetCivpointsButton()->Enable( FALSE );
		item->GetPwpointsButton()->Enable( FALSE );
	}
	else
	{
		
		if ( !g_gamesetup.GetHandicapping() )
		{
			item->GetCivpointsButton()->Enable( FALSE );
			item->GetPwpointsButton()->Enable( FALSE );
		}



	}

	
	switch ( g_allinoneWindow->GetMode() )
	{
	case g_allinoneWindow->CONTINUE_CREATE:
	case g_allinoneWindow->CONTINUE_JOIN:
		item->GetTribeButton()->Enable( FALSE );
		break;
	}
}



void ns_AIPlayerListBox::Delete( nf_AIPlayer *player )
{
	ns_ListBox<nf_AIPlayer, ns_AIPlayer>::Delete( player );

	ns_HPlayerItem *item = FindHPlayerItem( player );
	Assert( item != NULL );
	if ( !item ) return;

	m_hplayerlistbox->RemoveItem( item->Id() );

	
	delete item;

	
	if ( g_netfunc->IsHost() )
	{
		BOOL success = g_allinoneWindow->AssignTribe(
			0,
			*(uint16 *)player->GetKey()->buf,
			TRUE,
			FALSE,
			TRUE );
		Assert( success );
	}

	
	m_hplayerlistbox->SelectItem( m_hplayerlistbox->GetSelectedItemIndex() );
}



void ns_AIPlayerListBox::Change( nf_AIPlayer *player )
{
	ns_ListBox<nf_AIPlayer, ns_AIPlayer>::Change( player );

	ns_HPlayerItem *item = FindHPlayerItem( player );
	Assert( item != NULL );
	if ( !item ) return;

	
	bool selfMotivated = true;
	if ( !g_gamesetup.GetHandicapping() || !g_netfunc->IsHost() )
	{
		selfMotivated = false;
	}

	UpdateHPlayerItem( player, selfMotivated );

	
	SortByColumn(
		m_hplayerlistbox->GetSortColumn(),
		m_hplayerlistbox->GetSortAscending() );
}



void ns_AIPlayerListBox::Destroy( void )
{
	
	for(iterator i=begin(); i!=end(); i++)
		Delete(*i);


}



AUI_ERRCODE ns_AIPlayerListBox::SortByColumn( sint32 column, BOOL ascending )
{





	return m_hplayerlistbox->SortByColumn( column, ascending );
}



ns_HPlayerItem *ns_AIPlayerListBox::FindHPlayerItem( nf_AIPlayer *player )
{
	ns_HPlayerItem *item;

	ListPos position =
		m_hplayerlistbox->GetPane()->ChildList()->GetHeadPosition();
	for ( sint32 i = m_hplayerlistbox->GetPane()->ChildList()->L(); i; i-- )
	{
		item = (ns_HPlayerItem *)
			m_hplayerlistbox->GetPane()->ChildList()->GetNext( position );

		if ( item->IsAI() && player->Equals( item->GetAIPlayer() ) )
			return item;
	}

	return NULL;
}



void ns_AIPlayerListBox::UpdateHPlayerItem(
	nf_AIPlayer *player,
	bool selfMotivated )
{
	ns_HPlayerItem *item = FindHPlayerItem( player );
	Assert( item != NULL );
	if ( !item ) return;

	
	item->GetHostItem()->
		SetImage( 0 );

	
	item->GetLaunchedItem()->
		SetImage( g_netshell->GetTrueBmp() );

	
	MBCHAR name[ dp_PNAMELEN + 1 ];
	strncpy( name, player->GetName(), dp_PNAMELEN );

	
	if ( !item->GetNameItem()->GetTextFont() )
		item->GetNameItem()->TextReloadFont();

	item->GetNameItem()->GetTextFont()->TruncateString(
		name,
		item->GetNameItem()->Width() );

	item->GetNameItem()->
		SetText( name );

	
	static ns_String aiString( "strings.ai" );
	item->GetPingItem()->
		SetText( aiString.GetString() );

	item->SetTribe( player->GetTribe() );



	
	
	if ( !selfMotivated )
	{
		item->SetCivpoints( player->GetCivPoints() );
		item->SetPwpoints( player->GetPwPoints() );
	}

	m_hplayerlistbox->ShouldDraw();
}
