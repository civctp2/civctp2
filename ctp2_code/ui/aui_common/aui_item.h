










#ifndef __AUI_ITEM_H__
#define __AUI_ITEM_H__


#include "aui_control.h"



class aui_Item : public aui_Control
{
public:
	
	aui_Item(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR const *ldlBlock )
		:
		aui_Control( retval, id, (MBCHAR *) ldlBlock ),
		aui_ImageBase( ldlBlock ),
		aui_TextBase( ldlBlock, (const MBCHAR *)NULL ) {}
	aui_Item(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height )
		:
		aui_Control( retval, id, x, y, width, height ) {}
	virtual ~aui_Item() {}

protected:
	aui_Item() : aui_Control() {}
};


#endif 
