



#pragma once

#ifndef __C3_HEADER_H__
#define __C3_HEADER_H__

#include "c3_listbox.h"
#include "aui_header.h"


class c3_Header : public aui_Header
{
public:
	
	c3_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	c3_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~c3_Header();

protected:
	c3_Header() : aui_Header() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateSwitches( MBCHAR *ldlBlock = NULL );
};


#endif 
