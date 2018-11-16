#ifndef __NS_CHATBOX_H__
#define __NS_CHATBOX_H__

#include "textbox.h"
#include "aui_action.h"
#include "netfunc.h"

class aui_TextField;
class aui_Button;


class ns_ChatBox : public TextBox, public NETFunc::Chat
{
	NETFunc::Player *player;
	bool bWhisper;
	bool bGroup;
public:

	ns_ChatBox(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_ChatBox();

	NETFunc::Player *GetPlayer();
	void SetPlayer(NETFunc::Player *p);
	bool IsWhisper(void);
	void SetWhisper(bool w);
	bool IsGroup(void);
	void SetGroup(bool g);

	void Receive(NETFunc::Player *p, TYPE t, const char *m);

	AUI_ERRCODE RepositionItems( void );
	AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	aui_TextBase *m_textstyleSystem;
	aui_TextBase *m_textstyleChat;
	aui_TextBase *m_textstyleWhisper;

	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE	CreateComponents( void );

public:
	aui_TextField	*GetInputField( void ) const { return m_inputField; }

protected:
	aui_TextField	*m_inputField;

	AUI_ACTION_BASIC(InputFieldAction);
};

#endif
