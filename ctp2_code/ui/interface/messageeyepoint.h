






#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef MESSAGE_EYEPOINT_H__
#define MESSAGE_EYEPOINT_H__

class MessageEyePointDropdown;
class MessageEyePointListbox;
class MessageEyePointListItem;
class MessageEyePointStandard;

#include "auitypes.h"       // AUI_ERRCODE
#include "c3_listitem.h"
#include "ctp2_inttypes.h"  // sintN

class aui_Button;
class c3_DropDown;
class c3_Static;
class MessageData;
class MessageDropdownEyePointAction;
class MessageListboxEyePointAction;
class MessageModal;
class MessageWindow;
class MessageStandardEyePointAction;
// MBCHAR

class MessageEyePointListItem : public c3_ListItem
{
public:
	MessageEyePointListItem(AUI_ERRCODE *retval, MBCHAR const *name, sint32 index, MBCHAR const *ldlBlock);

	virtual void Update(void);

	MBCHAR const * GetName( void ) const { return m_name; }
	sint32 GetIndex( void ) const { return m_index; }

	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

protected:
	MessageEyePointListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(MBCHAR const * name, sint32 index, MBCHAR const * ldlBlock);
	
private:
	MBCHAR		m_name[_MAX_PATH];
	sint32		m_index;

};



class MessageEyePointStandard 
{
public:
	MessageEyePointStandard( AUI_ERRCODE *retval, 
							 MBCHAR const *ldlBlock, 
							 MessageWindow *window );
	MessageEyePointStandard( AUI_ERRCODE *retval, 
							 MBCHAR const *ldlBlock, 
							 MessageModal *window );

	virtual ~MessageEyePointStandard();

	virtual AUI_ERRCODE InitCommon( MBCHAR const *ldlBlock, MessageWindow *window );
	virtual AUI_ERRCODE InitCommon( MBCHAR const *ldlBlock, MessageModal *window );

private:
	aui_Button						*m_button;
	MessageStandardEyePointAction	*m_action;
};



class MessageEyePointDropdown
{
public:
	MessageEyePointDropdown( AUI_ERRCODE *retval, 
							 MBCHAR const *ldlBlock, 
							 MessageWindow *window );
	MessageEyePointDropdown( AUI_ERRCODE *retval, 
							 MBCHAR const *ldlBlock, 
							 MessageModal *window );

	virtual ~MessageEyePointDropdown();

	virtual AUI_ERRCODE InitCommon(MBCHAR const * ldlBlock, MessageWindow *window );
	virtual AUI_ERRCODE InitCommon(MBCHAR const * ldlBlock, MessageModal *window );

private:
    AUI_ERRCODE InitCommonCommon
    (
        MBCHAR const *  ldlBlock,
        MessageData *   a_Message  
    );

	aui_Button *                    m_button;
	c3_DropDown *                   m_dropdown;

	MessageDropdownEyePointAction * m_action;
	MessageDropdownAction *         m_dropaction;
};



class MessageEyePointListbox
{
public:
	MessageEyePointListbox( AUI_ERRCODE *retval, 
							MBCHAR const *ldlBlock, 
							MessageWindow *window );
	MessageEyePointListbox( AUI_ERRCODE *retval, 
							MBCHAR const *ldlBlock, 
							MessageModal *window );

	virtual ~MessageEyePointListbox();

	virtual AUI_ERRCODE InitCommon( MBCHAR const *ldlBlock, MessageWindow *window );
	virtual AUI_ERRCODE InitCommon( MBCHAR const *ldlBlock, MessageModal *window );

private:
	aui_Button						*m_buttonLeft;
	aui_Button						*m_buttonRight;
	MessageListboxEyePointAction	*m_action1;
	MessageListboxEyePointAction	*m_action2;
};

#endif 
