












#pragma once
#ifndef __STATICTEXTITEM_H__
#define __STATICTEXTITEM_H__


#include "aui_Static.h"
#include "ItemInfo.h"


class StaticTextItem : public aui_Static, public ItemInfo
{
public:
	
	StaticTextItem(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text = NULL,
		uint32 maxLength = 0,
		uint32 size = 0,
		sint32 textId = 0,
		sint32 category = 0,
		sint32 itemType = 0);	

	virtual ~StaticTextItem() {}
};


#endif 
