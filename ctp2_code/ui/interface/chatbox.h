#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CHATBOX_H__
#define __CHATBOX_H__

#include "primitives.h"
#include "colorset.h"
#include "c3window.h"

#define k_CHATBOX_LINE_LENGTH			60




















class aui_Surface;
class aui_BitmapFont;
class aui_Window;
class C3TextField;
class c3_HyperTextBox;
class ChatBox;

class ChatWindow : public C3Window
{
public:
	ChatWindow
	(
		AUI_ERRCODE *   retval,
		uint32          id,
		const MBCHAR *  ldlBlock,
		sint32          bpp,
		AUI_WINDOW_TYPE type    = AUI_WINDOW_TYPE_STANDARD,
		ChatBox *       parent  = NULL
	);
	virtual ~ChatWindow();

	virtual AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	BOOL CheckForEasterEggs(const MBCHAR *s);

	c3_HyperTextBox	*   GetTextBox(void) const { return m_textBox; }
	C3TextField	*       GetTextField(void) const { return m_textField; }
	ChatBox	*           GetChatBox(void) const { return m_chatBox; }

	void				ColorizeString(MBCHAR *destString, const MBCHAR *srcString, COLORREF colorRef);

	static void ChatCallback(aui_Control *control, uint32 action, uint32 data, Cookie cookie) ;

private:
	c3_HyperTextBox		*m_textBox;
	C3TextField			*m_textField;
	ChatBox				*m_chatBox;
};

class ChatBox {
public:
	static void Initialize(void);
	static void Cleanup(void);

	ChatBox();
	~ChatBox();

	bool IsActive(void) { return m_active; }
	void SetActive(bool active);

	void AddText(const MBCHAR *text);
	void AddLine(sint32 playerNum, const MBCHAR *text);

private:

	ChatWindow			*m_chatWindow;
	bool				m_active;
};

#endif
