#include "c3.h"
#include "aui_action.h"

#include "MessageBoxDialog.h"

#include "aui_ldl.h"
#include "c3ui.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"
#include "ctp2_textfield.h"
#include "StrDB.h"

#include "CriticalMessagesPrefs.h"

#include "soundmanager.h"
#include "gamesounds.h"

extern C3UI *g_c3ui;

MessageBoxDialog *s_messageBoxDialog;

void MessageBoxDialog::Information(const MBCHAR *message,
								   const MBCHAR *id,
								   MessageCallback callback,
								   void *userData,
								   const MBCHAR *okText,
								   bool allowDontShow)
{
	if(!g_theCriticalMessagesPrefs)
	{
		g_theCriticalMessagesPrefs=new CriticalMessagesPrefs;
	}

	if(g_theCriticalMessagesPrefs->IsEnabled(id))
	{
		s_messageBoxDialog = new MessageBoxDialog(message, id, (void*)callback, userData, okText, NULL);
		s_messageBoxDialog->m_leftButton->Hide();
		if(!allowDontShow)
			s_messageBoxDialog->m_dontShowButton->Hide();
	}
	else
	{
		if(callback)
		{
			callback(true, userData);
		}
	}
}

void MessageBoxDialog::Query(const MBCHAR *message,
							 const MBCHAR *id,
							 MessageCallback callback,
							 void *userData,
							 const MBCHAR *okText,
							 const MBCHAR *cancelText)
{
	if(!g_theCriticalMessagesPrefs)
	{
		g_theCriticalMessagesPrefs=new CriticalMessagesPrefs;
	}
	if(g_theCriticalMessagesPrefs->IsEnabled(id))
	{
		s_messageBoxDialog = new MessageBoxDialog(message, id, (void*)callback, userData, okText, cancelText);
	}
	else
	{
		if(callback)
		{
			callback(true, userData);
		}
	}
}

void MessageBoxDialog::TextQuery(const MBCHAR *message,
								 MessageTextCallback callback,
								 void *userData,
								 const MBCHAR *okText,
								 const MBCHAR *cancelText,
								 const MBCHAR *defaultText)
{
	s_messageBoxDialog = new MessageBoxDialog(message, NULL, (void*)callback, userData, okText, cancelText);
	s_messageBoxDialog->m_isTextQuery = true;
	s_messageBoxDialog->m_textField->SetFieldText(defaultText);
	s_messageBoxDialog->m_textField->Show();
	s_messageBoxDialog->m_dontShowButton->Hide();
}


MessageBoxDialog::MessageBoxDialog(const MBCHAR *message,
								   const MBCHAR *id,
								   void *callback,
								   void *userData,
								   const MBCHAR *okText,
								   const MBCHAR *cancelText) :
m_window(static_cast<ctp2_Window*>(
		 aui_Ldl::BuildHierarchyFromRoot("MessageBoxDialog"))),
m_messageDisplay(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"MessageBoxDialog.DialogBackground.MessageDisplay"))),
m_leftButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"MessageBoxDialog.DialogBackground.LeftButton"))),
m_rightButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"MessageBoxDialog.DialogBackground.RightButton"))),
m_dontShowButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"MessageBoxDialog.DialogBackground.DontShowButton"))),
m_textField(static_cast<ctp2_TextField*>(aui_Ldl::GetObject(
	"MessageBoxDialog.DialogBackground.TextField"))),
m_callback(callback),
m_userData(userData)
{

	m_window->SetType(AUI_WINDOW_TYPE_POPUP);

	g_c3ui->AddWindow(m_window);

	Assert(m_messageDisplay);
	Assert(m_leftButton);
	Assert(m_rightButton);
	Assert(m_dontShowButton);

	m_isTextQuery = false;


	const MBCHAR *str = g_theStringDB->GetNameStr(message);
	if(str) {
		m_messageDisplay->SetText(str);
	} else {
		m_messageDisplay->SetText(message);
	}

	if(cancelText) {

		Assert(okText);
		m_leftButton->SetText(g_theStringDB->GetNameStr(okText));

		m_rightButton->SetText(g_theStringDB->GetNameStr(cancelText));
	} else {

		m_leftButton->Hide();

		Assert(okText);
		m_rightButton->SetText(g_theStringDB->GetNameStr(okText));
	}

	// Right button is always visible and acts like Cancel in a two-button query
	// dialog or like OK in a one-button informational dialog.
	m_rightButton->SetActionKeyToESC();

	m_leftButton->SetActionFuncAndCookie(
		LeftButtonActionCallback, this);
	m_rightButton->SetActionFuncAndCookie(
		RightButtonActionCallback, this);
	m_textField->SetActionFuncAndCookie(
		TextFieldActionCallback, this);
	m_dontShowButton->SetActionFuncAndCookie(
		DontShowButtonActionCallback, this);

	m_window->SetStronglyModal(true);

	m_window->Show();
	g_soundManager->AddGameSound(GAMESOUNDS_POPUP);

	m_textField->Hide();

	if(id)
	{
		m_identifier=new MBCHAR[strlen(id)+1];
		strcpy(m_identifier,id);
	}
	else
	{
		m_identifier=NULL;
		m_dontShowButton->Hide();
	}
	m_closing = false;
}

MessageBoxDialog::~MessageBoxDialog()
{

	m_window->Hide();
	g_c3ui->RemoveWindow(m_window->Id());
	aui_Ldl::DeleteHierarchyFromRoot("MessageBoxDialog");
	if(m_identifier)
	{
		delete m_identifier;
		m_identifier=NULL;
	}
}

class DismissMessageBoxAction : public aui_Action
{
public:
	DismissMessageBoxAction(MessageBoxDialog * dialog)
    :   aui_Action  (),
        m_dialog    (dialog)
    { ; };
	virtual ~DismissMessageBoxAction(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	)
    {
        delete  m_dialog;
    };

private:
	MessageBoxDialog *  m_dialog;
};

void MessageBoxDialog::ButtonActionCallback(bool response, aui_Control * control, uint32 action, uint32 data,
		void * cookie)
{
	if (action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE)) {
		return;
	}

	MessageBoxDialog * dialog = static_cast<MessageBoxDialog*>(cookie);
	if (dialog->m_closing) {
		return;
	}
	dialog->m_closing = true;

	g_c3ui->AddAction(new DismissMessageBoxAction(dialog));

	static char text[256];
	if (dialog->m_callback)
	{
		if (dialog->m_isTextQuery)
		{
			dialog->m_textField->GetFieldText(text, 255);
			MessageTextCallback callback = (MessageTextCallback)dialog->m_callback;
			callback(response, text, dialog->m_userData);
		}
		else
		{
			MessageCallback callback = (MessageCallback)dialog->m_callback;
			callback(response, dialog->m_userData);
		}
	}

	if (!dialog->m_isTextQuery && dialog->m_dontShowButton->GetToggleState() && dialog->m_identifier) {
		g_theCriticalMessagesPrefs->SetEnabled(dialog->m_identifier, !dialog->m_dontShowButton->GetToggleState());
	}
}

void MessageBoxDialog::TextFieldActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != AUI_TEXTFIELD_ACTION_EXECUTE)
		return;

	MessageBoxDialog *dialog =
		static_cast<MessageBoxDialog*>(cookie);

	if(dialog->m_closing) {

		return;
	}

	dialog->m_closing = true;

	char text[256];

	g_c3ui->AddAction(new DismissMessageBoxAction(dialog));

	if(dialog->m_callback) {
		if(dialog->m_isTextQuery) {
			dialog->m_textField->GetFieldText(text, 255);
			MessageTextCallback cb = (MessageTextCallback)dialog->m_callback;
			cb(true, text, dialog->m_userData);
		} else {
			MessageCallback cb = (MessageCallback)dialog->m_callback;
			cb(true, dialog->m_userData);
		}
	}

}

void MessageBoxDialog::DontShowButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	MessageBoxDialog *dialog =
		static_cast<MessageBoxDialog*>(cookie);

	dialog->m_dontShowButton->SetToggleState(!dialog->m_dontShowButton->GetToggleState());
}
