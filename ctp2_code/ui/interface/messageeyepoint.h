#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __MESSAGE_EYEPOINT_H__
#define __MESSAGE_EYEPOINT_H__

#include "c3_listitem.h"

class c3_Static;

class MessageStandardEyePointAction;
class MessageDropdownEyePointAction;
class MessageListboxEyePointAction;

class MessageEyePointListItem : public c3_ListItem
{
public:

	MessageEyePointListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 index, MBCHAR *ldlBlock);


	virtual void Update(void);

	MBCHAR	*GetName( void ) { return m_name; }
	sint32	GetIndex( void ) { return m_index; }

protected:
	MessageEyePointListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(MBCHAR *name, sint32 index, MBCHAR *ldlBlock);

public:
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);
private:
	MBCHAR		m_name[_MAX_PATH];
	sint32		m_index;

};


class MessageEyePointStandard
{
public:
	MessageEyePointStandard( AUI_ERRCODE *retval,
							 MBCHAR *ldlBlock,
							 MessageWindow *window );
	MessageEyePointStandard( AUI_ERRCODE *retval,
							 MBCHAR *ldlBlock,
							 MessageModal *window );

	virtual ~MessageEyePointStandard();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageWindow *window );
	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageModal *window );

private:
	aui_Button						*m_button;
	MessageStandardEyePointAction	*m_action;
};


class MessageEyePointDropdown
{
public:
	MessageEyePointDropdown( AUI_ERRCODE *retval,
							 MBCHAR *ldlBlock,
							 MessageWindow *window );
	MessageEyePointDropdown( AUI_ERRCODE *retval,
							 MBCHAR *ldlBlock,
							 MessageModal *window );

	virtual ~MessageEyePointDropdown();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageWindow *window );
	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageModal *window );

private:
	aui_Button					*m_button;
	c3_DropDown					*m_dropdown;

	MessageDropdownEyePointAction	*m_action;
	MessageDropdownAction		*m_dropaction;
};


class MessageEyePointListbox
{
public:
	MessageEyePointListbox( AUI_ERRCODE *retval,
							MBCHAR *ldlBlock,
							MessageWindow *window );
	MessageEyePointListbox( AUI_ERRCODE *retval,
							MBCHAR *ldlBlock,
							MessageModal *window );

	virtual ~MessageEyePointListbox();

	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageWindow *window );
	virtual AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, MessageModal *window );

private:
	aui_Button						*m_buttonLeft;
	aui_Button						*m_buttonRight;
	MessageListboxEyePointAction	*m_action1;
	MessageListboxEyePointAction	*m_action2;
};

#else

class MessageEyePointStandard;
class MessageEyePointDropdown;
class MessageEyePointListbox;

#endif
