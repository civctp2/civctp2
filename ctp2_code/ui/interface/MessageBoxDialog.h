#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_MESSAGE_BOX_DIALOG_HEADER
#define ___BMH_MESSAGE_BOX_DIALOG_HEADER

#include "aui_control.h"

class ctp2_Button;
class ctp2_Static;
class ctp2_Window;
class ctp2_TextField;

class MessageBoxDialog {
public:

	typedef void (*MessageCallback)(bool response, Cookie userData);
	typedef void (*MessageTextCallback)(bool response, const char *text, Cookie userData);

	static void Information(const MBCHAR *message,
		const MBCHAR *id,
		MessageCallback callback = NULL,
		Cookie userData = NULL,
		const MBCHAR *okText = "str_ldl_MB_OK",
							bool allowDontShow = true);

	static void Query(const MBCHAR *message,
		const MBCHAR *id,
		MessageCallback callback = NULL,
		Cookie userData = NULL,
		const MBCHAR *okText = "str_ldl_MB_OK",
		const MBCHAR *cancelText = "str_ldl_MB_CANCEL");

	static void TextQuery(const MBCHAR *message,
		MessageTextCallback callback = NULL,
		Cookie userData = NULL,
		const MBCHAR *okText = "str_ldl_MB_OK",
		const MBCHAR *cancelText = "str_ldl_MB_CANCEL",
		const MBCHAR *defaultText = "");

	~MessageBoxDialog();

private:

	MessageBoxDialog(const MBCHAR *message,
		const MBCHAR *id,
		void *callback,
		Cookie userData,
		const MBCHAR *okText, const MBCHAR *cancelText);

	static void LeftButtonActionCallback(aui_Control * control, uint32 action, uint32 data, Cookie cookie) {
		ButtonActionCallback(true, control, action, data, cookie);
	}

	static void RightButtonActionCallback(aui_Control * control, uint32 action, uint32 data, Cookie cookie) {
		ButtonActionCallback(false, control, action, data, cookie);
	}

	static void ButtonActionCallback(bool response, aui_Control * control, uint32 action, uint32 data, Cookie cookie);
	static void TextFieldActionCallback(aui_Control * control, uint32 action, uint32 data, Cookie cookie);

	static void DontShowButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);

	ctp2_Window		*m_window;

	ctp2_Static		*m_messageDisplay;

	ctp2_Button		*m_leftButton;
	ctp2_Button		*m_rightButton;
	ctp2_Button		*m_dontShowButton;
	ctp2_TextField  *m_textField;

	void *m_callback;

	bool m_closing;

	Cookie			m_userData;
	MBCHAR			*m_identifier;

	bool m_isTextQuery;
};

#endif
