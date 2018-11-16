#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3_UTILITYDIALOGBOX_H__
#define __C3_UTILITYDIALOGBOX_H__

#include "c3_listitem.h"
#include "aui_action.h"
#include "keyboardhandler.h"

class Unit;
class c3_PopupWindow;
class c3_Button;
class c3_Static;
class ctp2_ListBox;
class C3TextField;
class Thermometer;
class aui_Static;
class aui_HyperTextBox;
class ctp2_Button;

enum PLAYER_ACTION {
	PLAYER_ACTION_KICK,
	PLAYER_ACTION_OPEN,
	PLAYER_ACTION_CLOSE,

	PLAYER_ACTION_MAX
};

enum EXPEL_ACTION {
	EXPEL_ACTION_ATTACK,
	EXPEL_ACTION_EXPEL,
	EXPEL_ACTION_CANCEL,

	EXPEL_ACTION_MAX
};

typedef void (c3_UtilityCityListCallback)(Unit city, sint32 val2);
typedef void (c3_PiracyCallback)(sint32 player, sint32 val2);
typedef void (c3_ExpelCallback)(sint32 val);
typedef void (c3_UtilityTextFieldCallback)(MBCHAR const *text, sint32 accepted, void *data);
typedef void (c3_UtilityTextMessageCallback)(sint32 val);
typedef void (c3_AbortMessageCallback)(sint32 val);
typedef void (c3_UtilityPlayerListCallback)(sint32 player, sint32 val2, PLAYER_ACTION action);






class c3_UtilityCityListPopup
{
public:
	c3_UtilityCityListPopup( c3_UtilityCityListCallback *callback, MBCHAR *ldlBlock = NULL );
	~c3_UtilityCityListPopup( void );

	c3_PopupWindow	*m_window;

	c3_Static		*m_title_label;
	ctp2_ListBox	*m_list;
	c3_Button		*m_ok;
	c3_Button		*m_cancel;

	c3_UtilityCityListCallback *m_callback;

public:
	sint32  Initialize(MBCHAR * ldlBlock);
	void    Cleanup(void);
	sint32  UpdateData(void);

	void RemoveWindow( void );
	void DisplayWindow( void );

};






class c3_PiracyPopup
{
public:
	c3_PiracyPopup( c3_PiracyCallback *callback, MBCHAR *ldlBlock = NULL );
	~c3_PiracyPopup( void );

	c3_PopupWindow	*m_window;

	c3_Static		*m_title_label;
	ctp2_ListBox	*m_list;
	c3_Button		*m_pirate;
	c3_Button		*m_cancel;

	c3_PiracyCallback *m_callback;

public:
	sint32  Initialize(MBCHAR * ldlBlock);
	void    Cleanup(void);
	sint32  UpdateData(void);

	void RemoveWindow( void );
	void DisplayWindow( void );

};






class c3_ExpelPopup
{
public:
	c3_ExpelPopup( c3_ExpelCallback *callback, MBCHAR *ldlBlock = NULL );
	~c3_ExpelPopup( void );

	c3_PopupWindow	*m_window;

	c3_Static		*m_title_label;
	c3_Button		*m_attack;
	c3_Button		*m_expel;
	c3_Button		*m_cancel;

	c3_ExpelCallback *m_callback;

public:
	sint32  Initialize(MBCHAR * ldlBlock);
	void    Cleanup(void);

	void RemoveWindow( void );
	void DisplayWindow( void );

};






class c3_UtilityTextFieldPopup
{
public:
	c3_UtilityTextFieldPopup( c3_UtilityTextFieldCallback *callback,
	    const MBCHAR *titleText, const MBCHAR *messageText,
	    const MBCHAR *defaultText, const MBCHAR *ldlBlock = NULL, void *data = NULL,
	    bool wantEmpties = false);
	~c3_UtilityTextFieldPopup( void );

	c3_PopupWindow	*m_window;

	c3_Static		*m_title_label;
	c3_Static		*m_message_label;

	C3TextField		*m_text;

	MBCHAR			*m_default_text;
	MBCHAR			*m_title_text;
	MBCHAR			*m_message_text;

	c3_Button		*m_ok;
	c3_Button		*m_cancel;

	c3_UtilityTextFieldCallback *m_callback;

	void			*m_data;
	bool            m_wantEmpties;

public:
	sint32  Initialize(const MBCHAR * ldlBlock);
	void    Cleanup(void);
	sint32  UpdateData(void);

	void *  GetData(void) const { return m_data; }

	void RemoveWindow( void );
	void DisplayWindow( void );
};







#define k_UTILITY_TEXTMESSAGE_OK		0
#define k_UTILITY_TEXTMESSAGE_OKCANCEL	1
#define k_UTILITY_TEXTMESSAGE_YESNO		2

class c3_UtilityTextMessagePopup
{
public:
	c3_UtilityTextMessagePopup
    (
        MBCHAR const *                  text,
        sint32                          type,
        c3_UtilityTextMessageCallback * callback = NULL,
        MBCHAR const *                  ldlBlock = NULL
    );
	~c3_UtilityTextMessagePopup( void );

	void    Cleanup(void);
	sint32  UpdateData(MBCHAR const * text);

	void RemoveWindow( void );
	void DisplayWindow( MBCHAR const *text );

	c3_PopupWindow	*m_window;
	c3_UtilityTextMessageCallback *m_callback;
	sint32			m_type;

private:
	sint32 Initialize (const MBCHAR *ldlBlock );

	c3_Static		*m_title_label;
	c3_Static		*m_text;

	c3_Button		*m_ok;
	c3_Button		*m_cancel;
};

AUI_ACTION_BASIC(c3_UtilityTextMessageCleanupAction);

class c3_UtilityTextMessageCreateAction : public aui_Action
{
public:
	c3_UtilityTextMessageCreateAction
    (
        MBCHAR const *                  text        = NULL,
        sint32                          type        = 0,
        c3_UtilityTextMessageCallback * callback    = NULL,
        MBCHAR const *                  ldlBlock    = NULL
    );

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	MBCHAR const *                  m_text;
	sint32                          m_type;
	c3_UtilityTextMessageCallback * m_callback;
	MBCHAR const *                  m_ldlBlock;
};

void c3_TextMessage( MBCHAR const *text, sint32 type = 0, c3_UtilityTextMessageCallback *callback = NULL, MBCHAR const * ldlBlock = NULL );
void c3_KillTextMessage( void );







#define k_UTILITY_ABORT		0
#define k_UTILITY_PROGRESS_ABORT	1

class c3_UtilityAbortPopup : public KeyboardHandler
{
public:
	c3_UtilityAbortPopup( MBCHAR const *text, sint32 type, c3_UtilityTextMessageCallback *callback = NULL, MBCHAR const *ldlBlock = NULL );
	virtual ~c3_UtilityAbortPopup( void );

	c3_PopupWindow	*m_window;

	c3_Static		*m_text;
	Thermometer		*m_meter;

	ctp2_Button		*m_abort;

	sint32			m_type;

	c3_AbortMessageCallback *m_callback;

public:
	sint32 Initialize (const MBCHAR *ldlBlock);
	void Cleanup ( void );
	sint32 UpdateData (MBCHAR const *text);
	sint32 UpdateMeter( sint32 percentFilled );

	void RemoveWindow( void );
	void DisplayWindow( MBCHAR const *text = NULL, sint32 percentFilled  = 0);

	void kh_Close();
};

AUI_ACTION_BASIC(c3_UtilityAbortCleanupAction);

void c3_AbortMessage(const MBCHAR *text = NULL, sint32 type = 0, c3_UtilityTextMessageCallback *callback = NULL, const MBCHAR *ldlBlock = NULL);
void c3_AbortUpdateData(const MBCHAR *text, sint32 percentFilled);
void c3_RemoveAbortMessage( void );

class c3_UtilityPlayerListPopup : public KeyboardHandler
{
public:
	c3_UtilityPlayerListPopup(c3_UtilityPlayerListCallback *callback = NULL, const MBCHAR *ldlBlock = NULL);
	virtual ~c3_UtilityPlayerListPopup( void );

	c3_PopupWindow	*m_window;

	ctp2_ListBox	*m_list;
	ctp2_Button		*m_abort;

	ctp2_Button		*m_kick;
	ctp2_Button		*m_open;
	ctp2_Button		*m_close;

	c3_UtilityPlayerListCallback *m_callback;

public:
	sint32 Initialize (const MBCHAR *ldlBlock);
	void Cleanup ( void );
	sint32 UpdateData ( void );

	sint32 EnableButtons( void );
	sint32 DisableButtons( void );

	void RemoveWindow( void );
	void DisplayWindow( void );

	void SetText(const MBCHAR *s, sint32 index);

	void kh_Close();
};





class DoubleListItem : public c3_ListItem
{
public:

	DoubleListItem(AUI_ERRCODE *retval, const MBCHAR *name, sint32 value,
	    const MBCHAR *text, const MBCHAR *ldlBlock);


	virtual void Update(void);

	const MBCHAR	*GetName( void ) { return m_name; }
	sint32	GetValue( void ) { return m_value; }

	sint32	SetSecondColumn(const MBCHAR *s);

protected:
	DoubleListItem() : c3_ListItem() {}


	AUI_ERRCODE InitCommonLdl(const MBCHAR *name, sint32 value,
	    const MBCHAR *text, const MBCHAR *ldlBlock);

public:

	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	MBCHAR			m_name[256];
	sint32			m_value;
	MBCHAR			m_text[_MAX_PATH];
};

void c3Expel_Initialize( void );
void c3Expel_Cleanup( void );

void NameTheCityDialogBoxCallback(const MBCHAR *text, sint32 val2, void *data);
void c3_utilitydialogbox_NameCity(Unit city);
void c3_utilitydialogbox_NameCityCleanup(void);


void c3_utilitydialogbox_TextFieldDialog(const MBCHAR *titleText,
    const MBCHAR *defaultText,
    const MBCHAR *messageText,
    c3_UtilityTextFieldCallback *callback,
    const MBCHAR *ldlBlock);
void c3_utilitydialogbox_CleanupTextFieldDialog(void);

#endif
