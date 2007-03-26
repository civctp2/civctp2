






#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef MESSAGE_RESPONSE_H__
#define MESSAGE_RESPONSE_H__

class MessageResponseListItem;
class MessageResponseStandard;
class MessageResponseDropdown;

#include "auitypes.h"       // AUI_ERRCODE
#include "c3_listitem.h"    // c3_ListItem
#include "ctp2_inttypes.h"  // sint32
#include "tech_wllist.h"

class c3_DropDown;
class ctp2_Button;
class MessageWindow;
class MessageResponseAction;
class MessageResponseSubmitAction;
// MBCHAR


class MessageResponseListItem : public c3_ListItem
{
public:
	MessageResponseListItem(AUI_ERRCODE *retval, MBCHAR const * name, sint32 index, MBCHAR *ldlBlock);
	virtual void Update(void);

	MBCHAR const *  GetName(void) const { return m_name; }
	sint32	        GetIndex(void) const { return m_index; }

	virtual sint32  Compare(c3_ListItem * item2, uint32 column);

protected:
	MessageResponseListItem() 
    : 
        c3_ListItem (),
        m_index     (0)
    {
        m_name[0] = 0;
    }

	AUI_ERRCODE InitCommonLdl(MBCHAR const * name, sint32 index, MBCHAR *ldlBlock);
	
private:
	MBCHAR		m_name[_MAX_PATH];
	sint32		m_index;
};



class MessageResponseStandard 
{
public:
	MessageResponseStandard (
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock,
		MessageWindow *window );

	virtual ~MessageResponseStandard ();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageWindow *window );

	static void DontShowButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

private:
	tech_WLList<ctp2_Button *>				*m_messageResponseButton;
	tech_WLList<MessageResponseAction *>	*m_messageResponseAction;

	ctp2_Button		*m_dontShowButton;	
	char *m_identifier;
};



class MessageResponseDropdown
{
public:
	MessageResponseDropdown(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock,
		MessageWindow *window );

	virtual ~MessageResponseDropdown();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageWindow *window );

private:
	ctp2_Button						*m_submitButton;
	c3_DropDown						*m_dropdown;

	MessageResponseSubmitAction		*m_action;
};

#endif 
