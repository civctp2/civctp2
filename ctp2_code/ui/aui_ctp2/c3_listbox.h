

#pragma once

#ifndef __C3_LISTBOX__
#define __C3_LISTBOX__

#include "PatternBase.h"
#include "aui_ListBox.h"

#define k_C3_LISTBOX_LDL_BEVELWIDTH		"bevelwidth"
#define k_C3_LISTBOX_LDL_BEVELTYPE		"beveltype"

class aui_Surface;
class c3_ListItem;

class c3_ListBox : public aui_ListBox, public PatternBase 
{
public:
	c3_ListBox() : aui_ListBox() {}
	c3_ListBox(AUI_ERRCODE *retval,	uint32 id, MBCHAR *ldlBlock, 
							ControlActionCallback *ActionFunc=NULL, void *cookie=NULL );
	c3_ListBox(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height, 
							MBCHAR *pattern, sint32 bevelwidth = 0, sint32 beveltype = 0,
							ControlActionCallback *ActionFunc = NULL, void *cookie = NULL);

	~c3_ListBox();

	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(sint32 bevelWidth, sint32 bevelType );
	AUI_ERRCODE CreateRangersAndHeader( MBCHAR *ldlBlock = NULL );
	
	void Clear(void);

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );
	virtual AUI_ERRCODE Draw(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0);
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0 );

	sint32 GetBevelWidth(void) { return m_bevelWidth; }
	sint32 GetBevelType(void) { return m_bevelType; }

protected:
	AUI_ERRCODE ReformatItemFromHeader(aui_Item *item);		
															
private:
	sint32		m_bevelWidth;
	sint32		m_bevelType;
};


#endif