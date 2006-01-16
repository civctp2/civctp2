//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer chat box
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_directui.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_ldl.h"
#include "aui_button.h"

#include "c3textfield.h"
#include "c3_header.h"
#include "c3_ranger.h"
#include "pattern.h"

#include "netshell.h"
#include "ns_string.h"

#include "ns_chatbox.h"


ns_ChatBox::ns_ChatBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	TextBox( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	m_textstyleSystem = new aui_TextBase("styles.system", (MBCHAR *)NULL);
	m_textstyleChat = new aui_TextBase("styles.chat", (MBCHAR *)NULL);
	m_textstyleWhisper = new aui_TextBase("styles.whisper", (MBCHAR *)NULL);

	
	
	m_textstyleSystem->TextReloadFont();
	m_textstyleChat->TextReloadFont();
	m_textstyleWhisper->TextReloadFont();

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateComponents();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	player = NULL;
	bWhisper = false;
	bGroup = false;
}


void ns_ChatBox::Receive(NETFunc::Player *p, TYPE t, char *m) {
	COLORREF color = 0x00ffffff;
	BOOL bold = FALSE;
	BOOL italic = FALSE;
	MBCHAR buf[256];
	Assert(strlen(m) <= 240);

	if(t == NETFunc::Chat::SYSTEM) {
		color = m_textstyleSystem->GetTextColor();
		bold = m_textstyleSystem->GetTextFont()->GetBold();
		italic = m_textstyleSystem->GetTextFont()->GetItalic();
		sprintf(
			buf,
			"<System:> %s\n",
			m );
	} else {
		static ns_String privString( "strings.privatemessage" );
		static ns_String teamString( "strings.teammessage" );
		sprintf(
			buf,
			"<%s%s:> %s\n",
			p ? p->GetName() : "???",
			(t==PRIVATE) ? privString.GetString() :
				(t==GROUP) ? teamString.GetString() : "",
			m );
		if(t == NETFunc::Chat::PRIVATE) {
			color = m_textstyleWhisper->GetTextColor();
			bold = m_textstyleWhisper->GetTextFont()->GetBold();
			italic = m_textstyleWhisper->GetTextFont()->GetItalic();
		} else {
			color = m_textstyleChat->GetTextColor();
			bold = m_textstyleChat->GetTextFont()->GetBold();
			italic = m_textstyleChat->GetTextFont()->GetItalic();
		}
	}

	AppendText( buf, color, bold, italic );
}

AUI_ERRCODE ns_ChatBox::InitCommonLdl( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE ns_ChatBox::CreateComponents( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_inputField = new C3TextField(
		&errcode,
		aui_UniqueId(),
		2,
		m_height + 2,
		m_width - 4,
		20, 
		"pattern.tga" ); 
	Assert( AUI_NEWOK(m_inputField,errcode) );
	if ( !AUI_NEWOK(m_inputField,errcode) ) return errcode;

	
	AddChild( m_inputField );

	aui_Action *action = new InputFieldAction;
	Assert( action != NULL );
	if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_inputField->SetAction( action );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ns_ChatBox::InitCommon()
{
	m_inputField = NULL;

	return AUI_ERRCODE_OK;
}



ns_ChatBox::~ns_ChatBox()
{
	aui_Action *action;

	if ( m_inputField )
	{
		if ((action = m_inputField->GetAction()))
			delete action;
		delete m_inputField;
		m_inputField = NULL;
	}
	if(m_textstyleSystem) {
		delete m_textstyleSystem;
		m_textstyleSystem = NULL;
	}
	if(m_textstyleChat) {
		delete m_textstyleChat;
		m_textstyleChat = NULL;
	}
	if(m_textstyleWhisper) {
		delete m_textstyleWhisper;
		m_textstyleWhisper = NULL;
	}
}


NETFunc::Player *ns_ChatBox::GetPlayer()
{
	return player;
}


void ns_ChatBox::SetPlayer(NETFunc::Player *p)
{
	player = p;
}


bool ns_ChatBox::IsWhisper()
{
	return bWhisper;
}


void ns_ChatBox::SetWhisper(bool w)
{
	bWhisper = w;
}


bool ns_ChatBox::IsGroup()
{
	return bGroup;
}


void ns_ChatBox::SetGroup(bool g)
{
	bGroup = g;
}


void ns_ChatBox::InputFieldAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE ) return;

	
	ns_ChatBox *chatbox = (ns_ChatBox *)control->GetParent();

	
	static MBCHAR text[ 241 ];
	chatbox->GetInputField()->GetFieldText( text, 240 );


	if(strlen(text) <= 0)
		return;

	Assert(strlen(text) <= 240);

	if(chatbox->IsWhisper()) {
		if(chatbox->GetPlayer())
			chatbox->Send(chatbox->GetPlayer(), text);
	} else if(!chatbox->IsGroup() || !g_netfunc->GetPlayer()->GetGroup())
		chatbox->Send(NULL, text);
	else
		chatbox->SendGroup(text);

	chatbox->GetInputField()->SetFieldText( "" );
}


AUI_ERRCODE ns_ChatBox::RepositionItems( void )
{
	return aui_TextBox::RepositionItems();
}


AUI_ERRCODE ns_ChatBox::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	return aui_TextBox::DrawThis( surface, x, y );
}
