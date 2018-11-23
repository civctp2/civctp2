#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __MESSAGE_RESPONSE_H__
#define __MESSAGE_RESPONSE_H__

#include "c3_listitem.h"

class ctp2_Button;

class MessageResponseAction;
class MessageResponseSubmitAction;


class MessageResponseListItem : public c3_ListItem
{
public:
	MessageResponseListItem(AUI_ERRCODE *retval, const MBCHAR *name, sint32 index, const MBCHAR *ldlBlock);
	virtual void Update(void);

	const MBCHAR	*GetName( void ) { return m_name; }
	sint32	GetIndex( void ) { return m_index; }

protected:
	MessageResponseListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(const MBCHAR *name, sint32 index, const MBCHAR *ldlBlock);

public:
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);
private:
	MBCHAR		m_name[_MAX_PATH];
	sint32		m_index;

};


class MessageResponseStandard
{
public:
	MessageResponseStandard (
		AUI_ERRCODE *retval,
		const MBCHAR *ldlBlock,
		MessageWindow *window );

	virtual ~MessageResponseStandard ();

	virtual AUI_ERRCODE InitCommon(const MBCHAR *ldlBlock, MessageWindow *window );

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
		const MBCHAR *ldlBlock,
		MessageWindow *window );

	virtual ~MessageResponseDropdown();

	virtual AUI_ERRCODE InitCommon(const MBCHAR *ldlBlock, MessageWindow *window );

private:
	ctp2_Button						*m_submitButton;
	c3_DropDown						*m_dropdown;

	MessageResponseSubmitAction		*m_action;
};


#else

class MessageResponseStandard;
class MessageResponseDropdown;

#endif
